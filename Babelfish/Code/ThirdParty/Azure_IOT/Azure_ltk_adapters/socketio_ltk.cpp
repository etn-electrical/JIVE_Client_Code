// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
// #include "azure_c_shared_utility/tcpsocketconnection_c.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/xlogging.h"

#include "opensslSocket.h"
#include "OS_Tasker.h"
#include "app.h"
#include "sockets.h"
#include "mtxssl_if.h"
#include "platform_ltk_azure.h"
#include "Ram.h"


/** Size of the receive buffer, when processing bytes received from the network socket.
 * This choice balances RAM consumption with the need to quickly offload bytes from the socket. */
#define SOCKET_RECEIVE_BUFFER_SIZE    128

/** Set the maximum delay for waiting for an acknowledgement, to 30sec. */
#define DEFAULT_CONNACK_TIMEOUT_MS             ( 30 * 1000 )// 30 seconds
#define WOULDBLOCK_DELAY_TIME_MS 100

typedef enum IO_STATE_TAG
{
	IO_STATE_CLOSED,
	IO_STATE_OPENING,
	IO_STATE_OPEN,
	IO_STATE_CLOSING,
	IO_STATE_ERROR
} IO_STATE;

typedef struct PENDING_SOCKET_IO_TAG
{
	unsigned char* bytes;
	size_t size;
	ON_SEND_COMPLETE on_send_complete;
	void* callback_context;
	SINGLYLINKEDLIST_HANDLE pending_io_list;
} PENDING_SOCKET_IO;

/** Structure for the Socket IO interface, to be used as the bottom layer for either the HTTP Proxy
 * layer or for the TLS layer without an intervening proxy layer.
 */
typedef struct SOCKET_IO_INSTANCE_TAG
{
	/** Callback to the next-up layer that invoked socketio_open() (eg, via tlsio_matrixssl_open()).
	 * To be invoked when the socket receives input. */
	ON_BYTES_RECEIVED on_bytes_received;
	/** Callback to the next-up layer that invoked socketio_open().
	 * To be invoked when we need to tell the upper layers that we have encountered an error
	 * at this level (eg, when the attempt to open fails). */
	ON_IO_ERROR on_io_error;
	/** Context given to us by the next-up layer to be given to on_bytes_received() . */
	void* on_bytes_received_context;
	/** Context given to us by the next-up layer to be given to on_io_error() . */
	void* on_io_error_context;
	/** Name of the host that we are to connect to, given by the socket_io_config.
	 * Will be the name of the proxy if the proxy is enabled. */
	char* hostname;
	/** Port number that we are to connect to, given by the socket IO configuration. */
	int port;
	/** Internal state of this IO interface. */
	IO_STATE io_state;
	/** The Azure list of messages to be sent (if any). */
	SINGLYLINKEDLIST_HANDLE pending_io_list;
	/** Socket instance handle for our socket for this interface.
	 * This comes from lwip_connect() ultimately; set during socketio_open(). */
	SOCKET c_fd;
	/** Buffer allocated for receiving data from the socket, to be given to the upper layer.
	 * Allocated here to avoid the overhead of constantly mallocing and freeing this necessary buffer.
	 */
	unsigned char receive_buffer[SOCKET_RECEIVE_BUFFER_SIZE];

} SOCKET_IO_INSTANCE;

// Will be given socket_io_instance->c_fd's value when we open the connection.
static int fd = INVALID_SOCKET;

/** Public function to get the socket handle for this IO connection.
 * @return The handle of the socket opened with socketio_open() (and lwip_connect()).
 * 			Will return INVALID_SOCKET if this connection hasn't been successfully opened yet.
 */
int socketio_get_fd()
{
	return ( fd );
}

/*this function will clone an option given by name and value*/
static void* socketio_CloneOption( const char* name, const void* value )
{
	void* result;

	if ( name != NULL )
	{
		result = NULL;

		if ( strcmp( name, OPTION_NET_INT_MAC_ADDRESS ) == 0 )
		{
			if ( value == NULL )
			{
				LogError( "Failed cloning option %s (value is NULL)", name );
			}
			else
			{
				if ( ( result = Ram::Allocate_Basic( sizeof( char ) * ( strlen( ( char* )value ) + 1 ) ) ) == NULL )
				{
					LogError( "Failed cloning option %s (malloc failed)", name );
				}
				else if ( strcpy( ( char* )result, ( char* )value ) == NULL )
				{
					LogError( "Failed cloning option %s (strcpy failed)", name );
					Ram::De_Allocate( result );
					result = NULL;
				}
			}
		}
		else
		{
			LogError( "Cannot clone option %s (not suppported)", name );
		}
	}
	else
	{
		result = NULL;
	}
	return ( result );
}

/*this function destroys an option previously created*/
static void socketio_DestroyOption( const char* name, const void* value )
{
	if ( name != NULL )
	{
		if ( ( strcmp( name, OPTION_NET_INT_MAC_ADDRESS ) == 0 ) && ( value != NULL ) )
		{
			Ram::De_Allocate( ( void* )value );
		}
	}
}

static OPTIONHANDLER_HANDLE socketio_retrieveoptions( CONCRETE_IO_HANDLE handle )
{
	OPTIONHANDLER_HANDLE result;

	if ( handle == NULL )
	{
		LogError( "failed retrieving options (handle is NULL)" );
		result = NULL;
	}
	else
	{
#if 0
		// Excluded by Px_Green
		SOCKET_IO_INSTANCE* socket_io_instance = ( SOCKET_IO_INSTANCE* )handle;
#endif

		result = OptionHandler_Create( socketio_CloneOption, socketio_DestroyOption, socketio_setoption );
		if ( result == NULL )
		{
			LogError( "unable to OptionHandler_Create" );
		}
#if 0
		// Excluded by Px_Green
		else if ( ( socket_io_instance->target_mac_address != NULL ) &&
				  ( OptionHandler_AddOption( result, OPTION_NET_INT_MAC_ADDRESS,
											 socket_io_instance->target_mac_address ) != OPTIONHANDLER_OK ) )
		{
			LogError( "failed retrieving options (failed adding net_interface_mac_address)" );
			OptionHandler_Destroy( result );
			result = NULL;
		}
#endif
	}

	return ( result );
}

static const IO_INTERFACE_DESCRIPTION socket_io_interface_description =
{
	socketio_retrieveoptions,
	socketio_create,
	socketio_destroy,
	socketio_open,
	socketio_close,
	socketio_send,
	socketio_dowork,
	socketio_setoption
};


/** Logs an error message, sets the state to IO_STATE_ERROR, and informs the upper layer of the error.
 * @param socket_io_instance Pointer to the Socket IO structure we established in socketio_create().
 * @param error_text The error message to log as an error.
 */
static void indicate_error( SOCKET_IO_INSTANCE* socket_io_instance, const char* error_text )
{
	socket_io_instance->io_state = IO_STATE_ERROR;
	LogError( error_text );
	if ( socket_io_instance->on_io_error != NULL )
	{
		socket_io_instance->on_io_error( socket_io_instance->on_io_error_context );
	}
}

static int add_pending_io( SOCKET_IO_INSTANCE* socket_io_instance, const unsigned char* buffer,
						   size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context )
{
	int result;
	PENDING_SOCKET_IO* pending_socket_io = ( PENDING_SOCKET_IO* )Ram::Allocate_Basic( sizeof( PENDING_SOCKET_IO ) );

	if ( pending_socket_io == NULL )
	{
		result = __FAILURE__;
	}
	else
	{
		pending_socket_io->bytes = ( unsigned char* )Ram::Allocate_Basic( size );
		if ( pending_socket_io->bytes == NULL )
		{
			LogError( "Allocation Failure: Unable to allocate pending list." );
			Ram::De_Allocate( pending_socket_io );
			result = __FAILURE__;
		}
		else
		{
			pending_socket_io->size = size;
			pending_socket_io->on_send_complete = on_send_complete;
			pending_socket_io->callback_context = callback_context;
			pending_socket_io->pending_io_list = socket_io_instance->pending_io_list;
			( void )memcpy( pending_socket_io->bytes, buffer, size );

			if ( singlylinkedlist_add( socket_io_instance->pending_io_list,
									   pending_socket_io ) == NULL )
			{
				LogError( "Failure: Unable to add socket to pending list." );
				Ram::De_Allocate( pending_socket_io->bytes );
				Ram::De_Allocate( pending_socket_io );
				result = __FAILURE__;
			}
			else
			{
				result = 0;
			}
		}
	}

	return ( result );
}

CONCRETE_IO_HANDLE socketio_create( void* io_create_parameters )
{
	SOCKETIO_CONFIG* socket_io_config = ( SOCKETIO_CONFIG* )io_create_parameters;
	SOCKET_IO_INSTANCE* result;

	if ( socket_io_config == NULL )
	{
		LogError( "Invalid argument: socket_io_config is NULL" );
		result = NULL;
	}
	else
	{
		result = ( SOCKET_IO_INSTANCE* )Ram::Allocate_Basic( sizeof( SOCKET_IO_INSTANCE ) );
		if ( result != NULL )
		{
			result->pending_io_list = singlylinkedlist_create();
			if ( result->pending_io_list == NULL )
			{
				LogError( "Failure: singlylinkedlist_create unable to create pending list." );
				Ram::De_Allocate( result );
				result = NULL;
			}
			else
			{
				result->hostname = ( char* )Ram::Allocate_Basic( strlen( socket_io_config->hostname ) + 1 );
				if ( result->hostname == NULL )
				{
					singlylinkedlist_destroy( result->pending_io_list );
					Ram::De_Allocate( result );
					result = NULL;
				}
				else
				{
					strcpy( result->hostname, socket_io_config->hostname );
					result->port = socket_io_config->port;
					result->on_bytes_received = NULL;
					result->on_io_error = NULL;
					result->on_bytes_received_context = NULL;
					result->on_io_error_context = NULL;
					result->io_state = IO_STATE_CLOSED;
					result->c_fd = INVALID_SOCKET;
				}
			}
		}
	}

	return ( result );
}

void socketio_destroy( CONCRETE_IO_HANDLE socket_io )
{
	if ( socket_io != NULL )
	{
		SOCKET_IO_INSTANCE* socket_io_instance = ( SOCKET_IO_INSTANCE* )socket_io;

		// tcpsocketconnection_destroy(socket_io_instance->tcp_socket_connection);

		/* clear all pending IOs */
		LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(
			socket_io_instance->pending_io_list );
		while ( first_pending_io != NULL )
		{
			PENDING_SOCKET_IO* pending_socket_io =
				( PENDING_SOCKET_IO* )singlylinkedlist_item_get_value( first_pending_io );
			if ( pending_socket_io != NULL )
			{
				Ram::De_Allocate( pending_socket_io->bytes );
				Ram::De_Allocate( pending_socket_io );
			}

			( void )singlylinkedlist_remove( socket_io_instance->pending_io_list, first_pending_io );
			first_pending_io = singlylinkedlist_get_head_item(
				socket_io_instance->pending_io_list );
		}

		singlylinkedlist_destroy( socket_io_instance->pending_io_list );
		Ram::De_Allocate( socket_io_instance->hostname );
		Ram::De_Allocate( socket_io );
	}
}

int socketio_data()
{
	return ( fd );
}

/** Opens the SSL Socket IO connection as part of the Azure initialization.
 *
 * The callstack to reach this function is informative; it is:
 *		xio_open
 *		http_proxy_io_open
 *		xio_open
 *		tlsio_matrixssl_open()
 *		xio_open
 *		uws_client_open_async
 *		wsio_open
 *		xio_open
 *		mqtt_client_connect
 *		SendMqttConnectMsg
 *		InitializeConnection
 *		IoTHubTransport_MQTT_Common_DoWork
 *		IoTHubClientCore_LL_DoWork
 *		messageThread(void *)
 *		prvTaskExitError()
 *
 * Socketio forms the lowest layer in the Azure SDK stack.
 *
 * @param socket_io Pointer to the Socket IO structure we established in socketio_create().
 * @param on_io_open_complete Callback to the upper layer (eg, TLS layer) code,
 * 								to be invoked when the open() operation here is complete.
 * @param on_io_open_complete_context Context given to us by the upper layer to be given to on_io_open_complete().
 * @param on_bytes_received Callback to the upper layer code,
 * 								to be invoked when the SSL socket receives input.
 * @param on_bytes_received_context Context given to us by the upper layer to be given to on_bytes_received().
 * @param on_io_error Callback to the upper layer code,
 * 								to be invoked when we need to tell the upper layers that we have encountered an error
 * 								at this level (eg, when the attempt to open fails).
 * @param on_io_error_context Context given to us by the upper layer to be given to on_io_error().
 */
int socketio_open( CONCRETE_IO_HANDLE socket_io, ON_IO_OPEN_COMPLETE on_io_open_complete,
				   void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received,
				   void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context )
{
	int result;

	SOCKET_IO_INSTANCE* socket_io_instance = ( SOCKET_IO_INSTANCE* )socket_io;

	if ( socket_io == NULL )
	{
		result = __FAILURE__;
	}
	else
	{
		int err;
		socket_io_instance->c_fd = socketConnect( socket_io_instance->hostname,
												  socket_io_instance->port, &err );
		if ( socket_io_instance->c_fd != INVALID_SOCKET )
		{
			setSocketNonblock( socket_io_instance->c_fd );
			LogInfo( "socketio_open: Socket Connected to %s", socket_io_instance->hostname );
			fd = socket_io_instance->c_fd;
			socket_io_instance->on_bytes_received = on_bytes_received;
			socket_io_instance->on_bytes_received_context = on_bytes_received_context;

			socket_io_instance->on_io_error = on_io_error;
			socket_io_instance->on_io_error_context = on_io_error_context;

			socket_io_instance->io_state = IO_STATE_OPEN;

			if ( on_io_open_complete != NULL )
			{
				on_io_open_complete( on_io_open_complete_context, IO_OPEN_OK );
			}

			result = 0;
		}
		else
		{
			result = __FAILURE__;
		}

	}

	return ( result );
}

int socketio_close( CONCRETE_IO_HANDLE socket_io, ON_IO_CLOSE_COMPLETE on_io_close_complete,
					void* callback_context )
{
	int result = 0;

	if ( socket_io == NULL )
	{
		result = __FAILURE__;
	}
	else
	{
		SOCKET_IO_INSTANCE* socket_io_instance = ( SOCKET_IO_INSTANCE* )socket_io;

		if ( ( socket_io_instance->io_state == IO_STATE_CLOSED ) ||
			 ( socket_io_instance->io_state == IO_STATE_CLOSING ) )
		{
			result = __FAILURE__;
		}
		else
		{
			socketClose( socket_io_instance->c_fd );
			fd = socket_io_instance->c_fd = INVALID_SOCKET;
			socket_io_instance->io_state = IO_STATE_CLOSED;

			if ( on_io_close_complete != NULL )
			{
				on_io_close_complete( callback_context );
			}

			result = 0;
		}
	}

	return ( result );
}

int socketio_send( CONCRETE_IO_HANDLE socket_io, const void* buffer, size_t size,
				   ON_SEND_COMPLETE on_send_complete, void* callback_context )
{
	int result;

	if ( ( socket_io == NULL ) ||
		 ( buffer == NULL ) ||
		 ( size == 0 ) )
	{
		/* Invalid arguments */
		result = __FAILURE__;
	}
	else
	{
		SOCKET_IO_INSTANCE* socket_io_instance = ( SOCKET_IO_INSTANCE* )socket_io;
		if ( socket_io_instance->io_state != IO_STATE_OPEN )
		{
			result = __FAILURE__;
		}
		else
		{
			LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(
				socket_io_instance->pending_io_list );
			if ( first_pending_io != NULL )
			{
				if ( add_pending_io( socket_io_instance, ( unsigned char const* )buffer, size,
									 on_send_complete, callback_context ) != 0 )
				{
					result = __FAILURE__;
				}
				else
				{
					result = 0;
				}
			}
			else
			{
				int send_result = socketWrite( socket_io_instance->c_fd, ( unsigned char* )buffer,
											   size );
				if ( send_result != size )
				{
					if ( send_result <= 0 )
					{
						result = __FAILURE__;
					}
					else
					{
						/* queue the remaining data to be sent later in DoWork() */
						if ( add_pending_io( socket_io_instance,
											 ( unsigned char* )buffer + send_result, size - send_result,
											 on_send_complete, callback_context ) != 0 )
						{
							result = __FAILURE__;
						}
						else
						{
							result = 0;
						}
					}
				}
				else
				{
					LogInfo( "Sent (immediately) %zu bytes", size );
					if ( on_send_complete != NULL )
					{
						on_send_complete( callback_context, IO_SEND_OK );
					}

					result = 0;
				}
			}
		}
	}

	return ( result );
}

/** The DoWork function for the Socket IO layer; checks for messages to send or receive.
 * First, sends any (all) messages queued up in its pending_io_list.
 * Then checks if there are any bytes to be received on the socket;
 * 		keeps getting them until we think we are done (ie, get less than our buffer size of 128),
 * 		and sends them up to the next layer.
 * 		If we haven't received anything but just sent a message (and so expect some response),
 * 		wait up to 30s to see if we get a reply.
 *
 * @param socket_io Pointer to the Socket IO structure we established in socketio_create().
 */
void socketio_dowork( CONCRETE_IO_HANDLE socket_io )
{
	if ( socket_io != NULL )
	{
		SOCKET_IO_INSTANCE* socket_io_instance = ( SOCKET_IO_INSTANCE* )socket_io;
		bool need_acknowledgement = false;

		LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(
			socket_io_instance->pending_io_list );
		// @ToDo: Maybe we should just send one and move on to receive?
		while ( first_pending_io != NULL )
		{
			PENDING_SOCKET_IO* pending_socket_io =
				( PENDING_SOCKET_IO* )singlylinkedlist_item_get_value(
					first_pending_io );
			if ( pending_socket_io == NULL )
			{
				indicate_error( socket_io_instance, "Failed to get message to send from the queue" );
				break;
			}

			int send_result = socketWrite( socket_io_instance->c_fd,
										   ( unsigned char* )pending_socket_io->bytes,
										   pending_socket_io->size );
			if ( send_result != pending_socket_io->size )
			{
				if ( send_result <= 0 )
				{
					Ram::De_Allocate( pending_socket_io->bytes );
					Ram::De_Allocate( pending_socket_io );
					( void )singlylinkedlist_remove(
						socket_io_instance->pending_io_list,
						first_pending_io );

					indicate_error( socket_io_instance, "Failure: sending Socket information." );
				}
				else
				{
					/* send something, wait for the rest
					 * @ToDo If this case ever happens, it is not doing what it says here;
					 * 			Needs to go back and send more. But maybe this just never happens. */
					( void )memmove( pending_socket_io->bytes,
									 pending_socket_io->bytes + send_result,
									 pending_socket_io->size - send_result );
				}
			}
			else
			{
				LogInfo( "Sent %zu bytes", pending_socket_io->size );
				need_acknowledgement = true;
				if ( pending_socket_io->on_send_complete != NULL )
				{
					pending_socket_io->on_send_complete(
						pending_socket_io->callback_context, IO_SEND_OK );
				}

				Ram::De_Allocate( pending_socket_io->bytes );
				Ram::De_Allocate( pending_socket_io );
				if ( singlylinkedlist_remove( socket_io_instance->pending_io_list,
											  first_pending_io ) != 0 )
				{
					indicate_error( socket_io_instance, "Failure: unable to remove socket from list" );
				}
			}

			first_pending_io = singlylinkedlist_get_head_item(
				socket_io_instance->pending_io_list );
		}

		if ( socket_io_instance->io_state == IO_STATE_OPEN )
		{
			uint32_t retry_count = 0;
			int received;
			int recd_count = 0;

			do
			{
				received = socketRead( socket_io_instance->c_fd,
									   socket_io_instance->receive_buffer,
									   SOCKET_RECEIVE_BUFFER_SIZE );

				if ( received > 0 )
				{
					recd_count += received;
					if ( received < SOCKET_RECEIVE_BUFFER_SIZE )
					{
						LogInfo( "Have received %d bytes", recd_count );
					}
					if ( socket_io_instance->on_bytes_received != NULL )
					{
						/* explictly ignoring here the result of the callback */
						socket_io_instance->on_bytes_received(
							socket_io_instance->on_bytes_received_context,
							socket_io_instance->receive_buffer, received );
						// received = -1;
						// We will keep feeding more bytes to the upper layer as quickly as we can.
					}
					/* We will break out the receive loop here and return, if the number of
					 * received bytes is > 0 but less than SOCKET_RECEIVE_BUFFER_SIZE;
					 * this generally means that we are done receiving the received message.
					 */
					if ( received < SOCKET_RECEIVE_BUFFER_SIZE )
					{
						break;
					}
				}
				else if ( need_acknowledgement )
				{
					// See if the socket return code indicates that we would block:
					bool would_block = IS_WOULDBLOCK( received );
					if ( would_block )
					{
						// Stay in the Receive loop for up to 30s
						if ( ++retry_count <
							 ( ( DEFAULT_CONNACK_TIMEOUT_MS )
							   / WOULDBLOCK_DELAY_TIME_MS ) )
						{
							OS_Tasker::Delay( WOULDBLOCK_DELAY_TIME_MS );
						}
						else
						{
							/* mark state as error and indicate it to the upper layer
							 * @ToDo: Should this count as an error, as opposed to a simple timeout? */
							indicate_error( socket_io_instance, "Timed out waiting to receive data" );
							break;
						}
					}
					else
					{
						/* mark state as error and indicate it to the upper layer */
						indicate_error( socket_io_instance, "Error on socket receive." );
						break;
					}
				}
				// Else: not waiting for anything
			} while ( received > 0 );
		}
	}
}

int socketio_setoption( CONCRETE_IO_HANDLE socket_io, const char* optionName, const void* value )
{
	/* Not implementing any options */
	return ( __FAILURE__ );
}

const IO_INTERFACE_DESCRIPTION* socketio_get_interface_description( void )
{
	return ( &socket_io_interface_description );
}
