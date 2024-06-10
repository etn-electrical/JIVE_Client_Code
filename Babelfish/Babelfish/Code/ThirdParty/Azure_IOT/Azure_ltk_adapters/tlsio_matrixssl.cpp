//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE file in the project root for full license information.

/* This is a template file used for porting */

/* This is a template for a TLS IO adapter for a TLS library that directly talks to the sockets
   Go through each TODO item in this file and replace the called out function calls with your own TLS library calls.
   Please refer to the porting guide for more details.

   Make sure that you replace tlsio_template everywhere in this file with your own TLS library name (like tlsio_mytls)
 */

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "matrixsslApi.h"
#include "matrixssllib.h"
#include "platform_ltk_azure.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio.h"
#include "tlsio_matrixssl.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/socketio.h"
#include "iot_device_platform.h"
#include "ssl_connection.h"
#include "OS_Tasker.h"
#include "OS_Semaphore.h"
#include "SSL_Context.h"
#include "Ram.h"


//Prototype function, eg from mtxssl_if.c:
extern "C" void rtk_get_ciphers_and_options( void* ssl_ctx_ptr, const uint16_t** ciphers, int* cipherSpec_Len,
											 sslSessOpts_t* options );

#define MAX_CIPHER_COUNT 12     //Well beyond our needs

/** Set the wait time we use in the receive loop for the semaphore Pend() operation to 50ms. */
#define HANDSHAKE_WAIT_TIME ( OS_TICK_RATE_MS * 50 )
/** Set the wait loop count limit for each handshake reply to 30s x20,
 * since we will use a 50ms wait on the semaphore. */
#define MAX_HANDSHAKE_RESPONSE_WAIT 30 * 20

//#define SSL_TIMEOUT         6000
//static const uint16_t TLS_RECEIVE_BUFFER_SIZE = 256;

/** The enumerated set of Internal TLSIO states.
 * If more states are needed, simply add them to the enum.
 */
typedef enum TLSIO_STATE_TAG
{
	TLSIO_STATE_NOT_OPEN,
	TLSIO_STATE_OPENING_UNDERLYING_IO,
	TLSIO_STATE_IN_HANDSHAKE,
	TLSIO_STATE_OPEN,
	TLSIO_STATE_CLOSING,
	TLSIO_STATE_ERROR
} TLSIO_STATE;

/** The internal structure that manages the stateful TLS layer.
 * This layer is created and opened through the Azure code by the websocket layer, and this
 * structure stores the callbacks to be used to communicate back to the next upper layer.
 * It also holds the handle to the next lower ("underlying") layer and to the Matrix SSL interface.
 */
typedef struct TLS_IO_INSTANCE_TAG
{
	/** Handle to the underlying (next lower) IO layer.
	 * While it is always xio, that generic layer points to
	 * either an http_proxy layer (for the proxied connection case) or to a socket_io layer.
	 * In either case, this is how we do the IO, once this layer has done the TLS spin on the data.
	 */
	XIO_HANDLE underlying_io;
	/** Callback to the websocket handling code that invokes tlsio_matrixssl_open().
	 * To be invoked when the open() operation here is complete. */
	ON_IO_OPEN_COMPLETE on_io_open_complete;
	/** Context given to us by the websocket handler to be given to on_io_open_complete() . */
	void* on_io_open_complete_context;
	/** Callback to the websocket handling code that invokes tlsio_matrixssl_open().
	 * To be invoked when the matrix SSL connection receives input. */
	ON_BYTES_RECEIVED on_bytes_received;
	/** Context given to us by the websocket handler to be given to on_bytes_received() . */
	void* on_bytes_received_context;
	/** Callback to the websocket handling code that invokes tlsio_matrixssl_open().
	 * To be invoked when we need to tell the upper layers that we have encountered an error
	 * at this level (eg, when the attempt to open fails). */
	ON_IO_ERROR on_io_error;
	/** Context given to us by the websocket handler to be given to on_io_error() . */
	void* on_io_error_context;
	/** Our internal sense of the state of this TLS connection. */
	TLSIO_STATE tlsio_state;
	/** Name of the host that we are to connect to, given by the TLS configuration.
	 * Will be the name of the proxy if the proxy is enabled. */
	char* hostname;
	/** Port number that we are to connect to, given by the TLS configuration. */
	int port;
	/** Holds an opaque pointer to the Matrix SSL context, created by matrixSslNewClientSession.
	 * This is the type needed by the other Matrix SSL API functions.	 */
	ssl_t* matrix_ssl_ctx;
	/** When the TLS session is opened and handshaking completed, this will hold the ID of
	 * that session with that (Azure) server.
	 * The same ID can be re-used then if we resume a session with that server. */
	sslSessionId_t sessionId;
	/** Semaphore for waiting on lower layer to signal it is done sending. */
	OS_Semaphore* xio_send_done_semph;
	/** Semaphore for waiting until we can proceed to the next step of TLS handshaking. */
	OS_Semaphore* handshake_proceed_semph;
	/** Handle to the Babelfish SSL (TLS) context that holds our trusted Azure Certificate Authority cert.  */
	Ssl_Context* bf_ssl_context;

} TLS_IO_INSTANCE;

// A local copy of our TLS IO instance
static TLS_IO_INSTANCE* local_tls_io_instance = nullptr;
static bool tls_io_retry_in_progress = false;

static int tlsio_matrixssl_close( CONCRETE_IO_HANDLE tls_io,
								  ON_IO_CLOSE_COMPLETE on_io_close_complete, void* on_io_close_complete_context );

/** Callback invoked by a lower layer when it experiences an error.
 * The action we take here depends upon this layer's present state.
 * @param context Opaque Pointer to "this" TLS IO instance's data structure (local to this file).
 */
static void on_underlying_io_error( void* context )
{
	if ( context != NULL )
	{
		TLS_IO_INSTANCE* tls_io_instance = ( TLS_IO_INSTANCE* )context;

		switch ( tls_io_instance->tlsio_state )
		{
			case TLSIO_STATE_OPENING_UNDERLYING_IO:
			case TLSIO_STATE_IN_HANDSHAKE:
				LogError( "Underlying IO Error while Opening or Handshaking for TLS" );
				tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
				if ( tls_io_instance->on_io_open_complete != nullptr )
				{
					tls_io_instance->on_io_open_complete( tls_io_instance->on_io_open_complete_context,
														  IO_OPEN_ERROR );
				}
				break;

			case TLSIO_STATE_OPEN:
				LogError( "Underlying IO Error while TLS open." );
				//Should we change state yet?
				//tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
				if ( tls_io_instance->on_io_error != NULL )
				{
					tls_io_instance->on_io_error( tls_io_instance->on_io_error_context );
				}
				break;

			case TLSIO_STATE_NOT_OPEN:
			case TLSIO_STATE_ERROR:
			default:
				LogError( "Underlying IO Error while TLS not open; ignored." );
				break;
		}
	}
	else
	{
		LogError( "Supplied context is NULL on io_error" );
	}
}

/** Callback invoked when xio_send() has completed.
 * @param context Pointer to "this" TLS IO instance's data structure (local to this file).
 * @param send_result The result from xio_send(); one of IO_SEND_OK, IO_SEND_ERROR, IO_SEND_CANCELLED.
 */
static void on_xio_send_complete( void* context, IO_SEND_RESULT send_result )
{
	TLS_IO_INSTANCE* tls_io_instance = ( TLS_IO_INSTANCE* )context;

	//OS_Tasker::Delay( 1U ); - maybe wait here, to give Pend() a chance to be called?
	tls_io_instance->xio_send_done_semph->Post();
}

/** Get data that Matrix SSL has ready to send, and write it to the next underlying layer.
 * Keep writing until Matrix indicates that all has been written, or until we encounter a failure.
 * @param tls_io_instance Pointer to "this" TLS IO instance's data structure (local to this file).
 * @return The return codes from matrixSslSentData():
 * 		MATRIXSSL_SUCCESS All data has been sent.
 * 		MATRIXSSL_REQUEST_CLOSE Done sending a close alert; close the connection.
 * 		MATRIXSSL_HANDSHAKE_COMPLETE Done sending a resumed Handshake.
 * 		Else is an error, eg PS_ARG_FAIL.
 */
static int ssl_write_loop( TLS_IO_INSTANCE* tls_io_instance )
{
	unsigned char* buf = nullptr;
	int32_t len;
	int rc = MATRIXSSL_SUCCESS;

	//@ToDo: check tls state
	while ( ( len = matrixSslGetOutdata( tls_io_instance->matrix_ssl_ctx, &buf ) ) > 0 )
	{
		//Got "len" bytes to send; write them out
		rc = xio_send( tls_io_instance->underlying_io, buf, len, on_xio_send_complete, ( void* ) tls_io_instance );
        if ( rc != 0 )
        {
            LogError( "Failed to send %d bytes, rc = %d", len, rc );
            rc = PS_PLATFORM_FAIL;      // Since the send call failed.
            break;
        }
		//Wait on the semaphore for up to 5s (plenty long enough)
		if ( !tls_io_instance->xio_send_done_semph->Pend( OS_TICK_RATE_MS * 5000 ) )
		{
			LogInfo( "Timed out waiting for XIO send to be done" );
			//But proceed; maybe it was signaled before we pended.
		}
		//Tell Matrix SSL that we sent those bytes; see if it has anything more for us
		rc = matrixSslSentData( tls_io_instance->matrix_ssl_ctx, len );
		if ( rc != MATRIXSSL_REQUEST_SEND )
		{
			//If not the one result code that would have us loop back and get more data from Matrix SSL,
			//then we are done here.
			break;
		}
		rc = MATRIXSSL_SUCCESS;
		LogInfo( "Waiting for more Matrix SSL data" );
	}

	return ( rc );
}

/** Open a new SSL (ie, TLS) session, adding the session info to our instance data.
 * @param tls_io_instance Pointer to "this" TLS IO instance's data structure (local to this file).
 * @return 0 on success, else the line number where the failure occurred.
 */
static int do_ssl_connect( TLS_IO_INSTANCE* tls_io_instance )
{
	int result = 0;

	//This is highly unlikely
	if ( ( tls_io_instance == NULL ) || ( tls_io_instance->bf_ssl_context == nullptr ) )
	{
		LogError( "Failed TLS socketConnect." );
		result = __LINE__;
	}
	else
	{
		LogInfo( "Opening new TLS session..." );
		//We will get the keys from our own SSL Context (they are the major contents in the context)
		void* ssl_ctx_ptr = tls_io_instance->bf_ssl_context->Get_Context();
		/* For matrixSslNewClientSession(), tried to only provide null for most of the arguments,
		 * in order to use their defaults with our Matrix SSL configuration,
		 * but that blew up in the Matrix SSL handling of the the pre-loaded keys options.
		 * So pulling the settings we used when first initializing Matrix SSL and re-using them here. */

		//We do have to get the Ciphers our product uses and the SSL options structure for this call.
		const uint16_t* cipherSuites;
		int cipherCount;
		sslSessOpts_t tls_options;
		rtk_get_ciphers_and_options( ssl_ctx_ptr, &cipherSuites, &cipherCount, &tls_options );
		// The pointer to the registered keys will be tls_options.userPtr
		const sslKeys_t* registered_keys = (const sslKeys_t*) tls_options.userPtr;
		// This will return the new matrix_ssl_ctx, which will now be valid to use with Matrix SSL functions.
		int32_t rc = matrixSslNewClientSession( &tls_io_instance->matrix_ssl_ctx, registered_keys,
												&tls_io_instance->sessionId,
												cipherSuites, cipherCount, nullptr, nullptr, nullptr, nullptr,
												&tls_options );
		if ( rc != MATRIXSSL_REQUEST_SEND )
		{
			LogError( "Failed new TLS session: error = %d.", rc );
			result = __LINE__;
		}
		else if ( tls_io_instance->matrix_ssl_ctx == nullptr )
		{
			LogError( "Failed to get a new TLS session" );
			result = __LINE__;
		}
		else
		{
			LogInfo( "TLS State  TLSIO_STATE_IN_HANDSHAKE " );
			tls_io_instance->tlsio_state = TLSIO_STATE_IN_HANDSHAKE;
			while ( tls_io_instance->tlsio_state == TLSIO_STATE_IN_HANDSHAKE )
			{
				//Matrix SSL should have prepared a handshake message for us to send.
				rc = ssl_write_loop( tls_io_instance );

				if ( rc == MATRIXSSL_HANDSHAKE_COMPLETE )
				{
					/* The client should only see this case when reconnecting and
					 * re-using session information. */
					LogInfo( "TLS State Handshake Complete TLSIO_STATE_OPEN " );
					tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
					if ( tls_io_instance->on_io_open_complete != nullptr )
					{
						tls_io_instance->on_io_open_complete( tls_io_instance->on_io_open_complete_context,
															  IO_OPEN_OK );
					}
					result = 0;
				}
				else if ( rc == MATRIXSSL_SUCCESS )
				{
					//Then we need to run the lower layer's DoWork function and wait for the server to reply; 
				  	// wait up to 30s
					//30s is a long time, so also check if the state has been changed on us meanwhile
					size_t loop_count = 0;
					while ( ( loop_count < MAX_HANDSHAKE_RESPONSE_WAIT ) &&
							( tls_io_instance->tlsio_state == TLSIO_STATE_IN_HANDSHAKE ) )
					{
						if ( tls_io_instance->underlying_io != nullptr )
						{
							xio_dowork( tls_io_instance->underlying_io );
						}
						//Wait 100ms on the semaphore that indicates we got the reply; break if it is active
						if ( tls_io_instance->handshake_proceed_semph->Pend( HANDSHAKE_WAIT_TIME ) )
						{
							LogInfo( "Breaking out of receive loop at count = %zu", loop_count );
							break;
						}
						loop_count++;
					}
					if ( ( loop_count >= MAX_HANDSHAKE_RESPONSE_WAIT ) &&
						 ( tls_io_instance->tlsio_state == TLSIO_STATE_IN_HANDSHAKE ) )
					{
						LogInfo( "Failed: Timed out waiting for next handshake step" );
						tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
						result = loop_count;
					}
					else if ( ( loop_count < MAX_HANDSHAKE_RESPONSE_WAIT ) &&
							  ( tls_io_instance->tlsio_state == TLSIO_STATE_IN_HANDSHAKE ) )
					{
						LogInfo( "Going back to send next handshake message" );
						continue;
					}
					else if ( tls_io_instance->tlsio_state == TLSIO_STATE_OPEN )
					{
						result = 0;
					}
					else
					{
						/* Either we have to go back and send the next message in the handshake process,
						 * or something went wrong, and the state indicates what it is.
						 * Provide an exit value, just in case. */
						result = __LINE__;
					}
				}
				else
				{
					//MATRIXSSL_REQUEST_CLOSE (unexpected here) or PS_ARG_FAIL or some other error
					LogInfo( "TLS State Handshake Failed TLSIO_STATE_ERROR " );
					tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
					result = rc;
			}
		}
	}
	if ( ( result != 0 ) && ( tls_io_instance->on_io_error != NULL ) )
	{
		LogError( "Failed to open TLS connection." );
		tls_io_instance->on_io_error( tls_io_instance->on_io_error_context );
	}
	}
	return ( result );
}

static void on_underlying_io_open_complete( void* context, IO_OPEN_RESULT open_result )
{
	TLS_IO_INSTANCE* tls_io_instance = ( TLS_IO_INSTANCE* )context;

	if ( open_result != IO_OPEN_OK )
	{
		LogError( "Failed when opening underlying IO; result = %d", open_result );
		tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
		if ( tls_io_instance->on_io_error != NULL )
		{
			tls_io_instance->on_io_error( tls_io_instance->on_io_error_context );
		}
	}
	else
	{
		LogInfo( "Underlying XIO layer successfully opened." );
		tls_io_instance->handshake_proceed_semph->Post();
	}
}

/** This callback will be invoked whenever the underlying layer (eg, socket_io) has received
 * bytes and is passing them up to us.
 * At this layer, we pass the data to the Matrix SSL code to assemble the message and determine
 * when the message is complete, and perhaps is to be passed up to the next layer (aka "application")
 * versus consumed by Matrix SSL itself (eg, for handshaking).
 * Note: since socket_io uses a small buffer (ie, relative to the handshaking messages), this function
 * may be called several times in a row when fresh data is received by the socket.
 * @param context Pointer to "this" TLS IO instance's data structure (local to this file).
 * @param recd_buffer The buffer that contains the received bytes
 * @param recd_size The number of bytes in that buffer.
 */
static void on_underlying_io_bytes_received( void* context, const unsigned char* recd_buffer,
											 size_t recd_size )
{
	if ( ( context == NULL ) || ( recd_buffer == NULL ) )
	{
		LogError( "Supplied context is NULL on bytes_received" );
	}
	else
	{
		unsigned char* ssl_buf;
		unsigned char* processed_buf = nullptr;
		uint32_t processed_len;
		int32_t len, rc = PS_FAIL;
		size_t recd_remaining = recd_size;
		TLS_IO_INSTANCE* tls_io_instance = ( TLS_IO_INSTANCE* )context;

		//@ToDo: check tls state

		/* We handle 2 cases here for incoming data:
		 * 1) The recd_size fits into the len that matrixSslGetReadbuf() provides;
		 * 		then we process with matrixSslReceivedData and proceed.
		 * 		If we get MATRIXSSL_REQUEST_RECV after that (ie, not done),
		 * 		then we have to exit and wait for more bytes to be received.
		 * 2) The recd_size is greater than the initial len that matrixSslGetReadbuf() provides;
		 * 		we process it with matrixSslReceivedData(), should get MATRIXSSL_REQUEST_RECV,
		 * 		and go back to matrixSslGetReadbuf() to be able to copy more.
		 * 		Same end conditions as in 1), after copying in all the received bytes.
		 */
		size_t start_location = 0;
		while ( recd_remaining > 0 )
		{
			/* Now run an inner loop which might read TLS data more than once */
			rc = MATRIXSSL_REQUEST_RECV;
			while ( ( rc == MATRIXSSL_REQUEST_RECV ) && ( recd_remaining > 0 ) )
			{
				len = matrixSslGetReadbuf( tls_io_instance->matrix_ssl_ctx, &ssl_buf );
				if ( len <= 0 )
				{
					//This is an error case; exit
					rc = PS_MEM_FAIL;
					LogInfo( "Failed to get buffer from Matrix SSL" );
					break;
				}
				//Copy the received data into the buffer and process it
				size_t copy_len = len;
				if ( recd_remaining < len )
				{
					copy_len = recd_remaining;
				}
				recd_remaining = recd_remaining - copy_len;
				/* Note: these debug messages can only be used briefly in development, because there is a limit of
				* one or two Term IO messages per second, and these can come fast and furious from the lower layer(s).
				* So only uncomment them when necessary and then comment them out when done.
				*/
				//LogInfo( "TLS Received buffer of %zu bytes; copying %zu, and %zu remain", recd_size, copy_len,
				//recd_remaining );
				memcpy( ssl_buf, &recd_buffer[start_location], copy_len );
				//Now have Matrix SSL process what has been received so far
				//Give it processed_buf pointer in case we have a complete message to send
				//up to the next layer (aka "application data")
				rc = matrixSslReceivedData( tls_io_instance->matrix_ssl_ctx, ( int32 ) copy_len, &processed_buf,
											&processed_len );
				if ( rc != MATRIXSSL_REQUEST_RECV )
				{
					//Ideally we are done at this point
					if ( recd_remaining > 0 )
					{
						Log_Info( "TLS Received buffer of %zu bytes; copying %zu, and %zu remain", recd_size, copy_len, recd_remaining );
						Log_Info( "Done with processing input, but %zu bytes remain", recd_remaining );
						// Advance the starting point for next time through
						start_location = copy_len;
					}
				}
				else if ( recd_remaining > 0 )
				{
					//Else, go back and submit more bytes to Matrix SSL (unless we have run out)
					// Having received at least some bytes
					Log_Info( "Looping back with MATRIXSSL_REQUEST_RECV" );
					//Ahmed. Fix based on Tom finding
					start_location = copy_len;
				}
			}

			/* We have to have another inner loop here, in case matrixSslProcessedData is called, which will give another
			* rc and possibly processed_buf to be processed. */
			bool done_processing = false;
			while ( !done_processing )
			{
				done_processing = true;
				/* Now we have to handle the various return codes. */
				switch ( rc )
				{
					case PS_SUCCESS:
						LogInfo( "Done with empty message" );
						break;

					case MATRIXSSL_REQUEST_SEND:
						Log_Info( "Got %zu bytes; take the next step: ask Matrix TLS to send something.", recd_size );
						tls_io_instance->handshake_proceed_semph->Post();
						break;

					case MATRIXSSL_REQUEST_RECV:
						/* Nothing more we can do here (used all the received bytes); will have to wait until we are called
						* again. This case is frequently encountered as the socket_io layer doles out 128 byte chunks
						* of a received message. */
						//Log_Info( "TLS wants more, but we processed all received bytes" );
						break;

					case MATRIXSSL_HANDSHAKE_COMPLETE:
						//This is normally how a client connection attempt ends, if not re-establishing a previous
						//connection
						Log_Info( "Done with Handshaking; TLS connection is now Open." );
						tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
						break;

					case MATRIXSSL_RECEIVED_ALERT:
						if ( processed_buf != nullptr )
						{

							Log_Info( "Received a TLS alert, level = %u, description = %u", processed_buf[0],
									processed_buf[0] );
							//@ToDo: do something with this. Could be closing the connection.

							//Now must call matrixSslProcessedData to indicate the data was processed, so it can be released
							rc = matrixSslProcessedData( tls_io_instance->matrix_ssl_ctx, &processed_buf, &processed_len );
							//Not "done_processing" since there might be another message in the received data
							done_processing = false;
						}
						break;

					case MATRIXSSL_APP_DATA:
					case MATRIXSSL_APP_DATA_COMPRESSED:     //Don't expect this one, but just in case...
						if ( processed_buf != nullptr )
						{
							Log_Info( "TLS decoded %u bytes to be sent up to next layer", processed_len );
							if ( tls_io_instance->on_bytes_received != nullptr )
							{
								//Call the upper layer handler and give it the decoded data bytes
								tls_io_instance->on_bytes_received( tls_io_instance->on_bytes_received_context,
																	processed_buf, processed_len );
							}
							//Now must call matrixSslProcessedData to indicate the data was processed, so it can be released
							rc = matrixSslProcessedData( tls_io_instance->matrix_ssl_ctx, &processed_buf, &processed_len );
							//Not "done_processing" since there might be another message in the received data
							done_processing = false;
						}
						break;

					case PS_MEM_FAIL:       //Listing these errors here just for clarity; "default" covers them.
					case PS_ARG_FAIL:
					case PS_PROTOCOL_FAIL:
					default:
						Log_Info( "Failed TLS receive with code = %d", rc );
						//Nothing more we can do here (used all the received bytes); will have to wait until we are called
						//again.
						break;
				}
			}
		}

	}
}

static CONCRETE_IO_HANDLE tlsio_matrixssl_create( void* io_create_parameters )
{
	TLS_IO_INSTANCE* result;

	/* check whether the argument is good */
	if ( io_create_parameters == NULL )
	{
		result = NULL;
		LogError( "NULL tls_io_config." );
	}
	else
	{
		TLSIO_CONFIG* tls_io_config = ( TLSIO_CONFIG* )io_create_parameters;
		//We will need a handle to our Ssl_Context, too.
		Ssl_Context* bf_iot_ssl_context = Get_IOT_SSL_Context();

		/* check if the hostname is good */
		if ( tls_io_config->hostname == NULL )
		{
			result = NULL;
			LogError( "NULL hostname in the TLS IO configuration." );
		}
		else if ( bf_iot_ssl_context == nullptr )
		{
			result = NULL;
			LogError( "No SSL context available for TLS IO creation." );
		}
		else
		{
			/* allocate tls_io_instance */
			result = ( TLS_IO_INSTANCE* )Ram::Allocate_Basic( sizeof( TLS_IO_INSTANCE ) );
			if ( result == NULL )
			{
				LogError( "Failed allocating TLSIO instance." );
			}
			else
			{
				memset( result, 0, sizeof( TLS_IO_INSTANCE ) );
				result->hostname = const_cast<char*>( tls_io_config->hostname );
				/* copy port and initialize all the callback data */
				result->port = tls_io_config->port;
				result->tlsio_state = TLSIO_STATE_NOT_OPEN;

				//Zero out this structure now; on first connect, Matrix SSL will give it its ID
				memset( &result->sessionId, 0, sizeof( sslSessionId_t ) );
				result->xio_send_done_semph = new OS_Semaphore();
				result->handshake_proceed_semph = new OS_Semaphore();
				result->bf_ssl_context = bf_iot_ssl_context;

#ifdef IOT_INCLUDE_HTTP
				//Direct method, no XIO
				result->underlying_io = ( XIO_HANDLE )tls_io_config->underlying_io_parameters;
				/* @ToDO: this case, if it ever exists, is not handled correctly in tlsio_create() or tlsio_open(). */
#else
				/* There are two cases here: if Azure's upper layer (ie, websocket layer)
				 * provided the information on the next lower layer (ie, "underlying" layer),
				 * then we can use that directly; this happens for the case where the connection
				 * to Azure uses an HTTP Proxy.
				 * For the other (non-proxy) case, we need to add our own interface to the socketio layer
				 * that we need to manager the socket connection.
				 */
				if ( tls_io_config->underlying_io_interface != NULL )
				{
					LogInfo( "Creating XIO instance for the proxy layer" );
					result->underlying_io = xio_create( tls_io_config->underlying_io_interface,
														tls_io_config->underlying_io_parameters );
				}
				else
				{
					SOCKETIO_CONFIG socketio_config;
					socketio_config.hostname = tls_io_config->hostname;
					socketio_config.port = tls_io_config->port;
					socketio_config.accepted_socket = NULL;

					LogInfo( "Creating XIO instance for the socketio layer" );
					const IO_INTERFACE_DESCRIPTION* io_description = socketio_get_interface_description();
					result->underlying_io = xio_create( io_description, &socketio_config );
				}
#endif
				if ( result->underlying_io == NULL )
				{
					LogError( "Failure connecting to underlying proxy or socket_io" );
					delete( result->xio_send_done_semph );
					delete( result->handshake_proceed_semph );
					Ram::De_Allocate( result );
					result = NULL;
				}
			}
		}
	}
	local_tls_io_instance = result;
	return ( result );
}

static void tlsio_matrixssl_destroy( CONCRETE_IO_HANDLE tls_io )
{
	if ( tls_io == NULL )
	{
		LogError( "NULL tls_io." );
	}
	else
	{
		TLS_IO_INSTANCE* tls_io_instance = ( TLS_IO_INSTANCE* )tls_io;

		LogInfo( "Closing TLS IO instance." );
		/* force a close when destroying */
		tlsio_matrixssl_close( tls_io, NULL, NULL );
		delete( tls_io_instance->xio_send_done_semph );
		delete( tls_io_instance->handshake_proceed_semph );
		if ( Ram::Valid_Ram_Ptr( tls_io_instance->underlying_io ) )
		{
		xio_destroy( tls_io_instance->underlying_io );
			tls_io_instance->underlying_io = nullptr;
		}
		Ram::De_Allocate( tls_io );
		local_tls_io_instance = nullptr;
	}
}

/** Opens the Matrix SSL connection as part of the Azure initialization.
 * There are two flows here; "chained" if the proxy is enabled (underlying_io is non-null), or "direct" with no proxy.
 * In the direct case, this is the level that owns the TCP socket.
 *
 * The callstack to reach this function is informative; for the direct, non-proxy case, it is:
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
 * @param tls_io Pointer to the TLS IO structure we established in tlsio_matrixssl_create().
 * @param on_io_open_complete Callback to the websocket handling code,
 * 								to be invoked when the open() operation here is complete with success.
 * 								On failure, we shall not call on_io_open_complete.
 * @param on_io_open_complete_context Context given to us by the websocket handler to be given to on_io_open_complete().
 * @param on_bytes_received Callback to the websocket handling code,
 * 								to be invoked when the matrix SSL connection receives input.
 * @param on_bytes_received_context Context given to us by the websocket handler to be given to on_bytes_received().
 * @param on_io_error Callback to the websocket handling code,
 * 								to be invoked when we need to tell the upper layers that we have encountered an error
 * 								at this level (eg, when the attempt to open fails).
 * @param on_io_error_context Context given to us by the websocket handler to be given to on_io_error().
 * @return 
 */
static int tlsio_matrixssl_open( CONCRETE_IO_HANDLE tls_io, ON_IO_OPEN_COMPLETE on_io_open_complete,
								 void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received,
								 void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context )
{
	int result;

	/* check arguments */
	if ( ( tls_io == NULL ) ||
		 ( on_io_open_complete == NULL ) ||
		 ( on_bytes_received == NULL ) ||
		 ( on_io_error == NULL ) )
	{
		result = __LINE__;
		LogError( "NULL tls_io at open." );
	}
	else
	{
		TLS_IO_INSTANCE* tls_io_instance = ( TLS_IO_INSTANCE* )tls_io;

		if ( tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN )
		{
			result = __LINE__;
			LogError( "Invalid tlsio_state %d on open. Expected state is TLSIO_STATE_NOT_OPEN.",
					  ( int ) tls_io_instance->tlsio_state );
		}
		else if ( tls_io_instance->underlying_io == NULL )
		{
			result = __LINE__;
			LogError( "Not connected to underlying proxy or socket_io" );
		}
		else
		{
			//Save these callbacks and their contexts for the upper layer:
			tls_io_instance->on_bytes_received = on_bytes_received;
			tls_io_instance->on_bytes_received_context = on_bytes_received_context;

			tls_io_instance->on_io_error = on_io_error;
			tls_io_instance->on_io_error_context = on_io_error_context;

			tls_io_instance->on_io_open_complete = on_io_open_complete;
			tls_io_instance->on_io_open_complete_context = on_io_open_complete_context;

			/* We don't process the trusted_certificate member here, since we did that when
			 * initializing via Ssl_Context::Load_Keys().
			 * if ( tls_io_instance->trusted_certificate != NULL )
			 */

			//Make sure the semaphore is reset to its unsignaled state (using a timeout of 0)
			tls_io_instance->handshake_proceed_semph->Pend( 0 );

			/* Substitute our callbacks here when opening the "underlying IO" layers
			 * This will open the proxy (if there is one) and invoke our socket_io layer ultimately.
			 */
			LogInfo( "Opening xio for underlying layer..." );
			if ( xio_open( tls_io_instance->underlying_io, on_underlying_io_open_complete,
						   tls_io_instance, on_underlying_io_bytes_received, tls_io_instance,
						   on_underlying_io_error, tls_io_instance ) != 0 )
			{
				LogError( "Underlying IO open failed" );
				tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
				result = __LINE__;
				//But not call on_io_error() here?
			}
			else
			{
				size_t retry_count = 0;
				while ( retry_count < 3 )
				{
					set_tls_io_retry_status( true );
					//Wait for the underlying XIO layer to signal that it is done; use 5s timeout
					if ( tls_io_instance->handshake_proceed_semph->Pend( OS_TICK_RATE_MS * 5 * 1000 ) )
					{
						break;
					}
					// The Proxy connection needs us to run DoWork() to receive its Connect message
					xio_dowork( tls_io_instance->underlying_io );		// But this might take 30s
					retry_count++;
				}
				/* Retries are over or tlsio open is successful */
				set_tls_io_retry_status( false );
				if ( retry_count >= 3 )
				{
					LogInfo( "Timed out waiting for XIO open to be done" );
					//But proceed; maybe it was signaled before we pended.
				}

				result = do_ssl_connect( tls_io_instance );
				if ( result != 0 )
				{
					LogError( "Failure to make TLS connection." );
					//result already set
				}
				else
				{
					LogInfo( "Success opening TLS connection." );
					on_io_open_complete( on_io_open_complete_context, IO_OPEN_OK );
					//result already = 0;
				}
			}
		}
	}

	return ( result );
}

/** Close the TLS connection, and its underlying layer(s).
 * This would be called by the upper XIO layer. Does not free any resources.
 * @param tls_io Pointer to the TLS IO structure we established in tlsio_matrixssl_create().
 * @param on_io_close_complete Callback to the websocket handling code,
 * 								to be invoked when the close() operation here is complete with success.
 * 								On failure, we shall not call on_io_close_complete.
 * @param on_io_close_complete_context Context given to us by the websocket handler to be given to
 * on_io_close_complete().
 */
static int tlsio_matrixssl_close( CONCRETE_IO_HANDLE tls_io,
								  ON_IO_CLOSE_COMPLETE on_io_close_complete, void* on_io_close_complete_context )
{
	int result = 0;

	if ( tls_io == NULL )
	{
		result = __LINE__;
		LogError( "NULL tls_io on close." );
	}
	else
	{
		TLS_IO_INSTANCE* tls_io_instance = ( TLS_IO_INSTANCE* )tls_io;

		/* If we're not open do not try to close */
		if ( tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN )
		{
			LogInfo( "TLS IO already closed." );
		}
		else
		{
			tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
			if ( tls_io_instance->underlying_io != nullptr )
			{
				// Should we call matrixSslEncodeClosureAlert() first?
				LogInfo( "Closing underlying XIO layer(s)." );
				// No on_io_close_complete() callback needed here, no need to check the return code:
				xio_close( tls_io_instance->underlying_io, nullptr, nullptr );
			}
		}

		matrixSslDeleteSession( tls_io_instance->matrix_ssl_ctx );
		tls_io_instance->matrix_ssl_ctx = nullptr;

		/* trigger the callback and return */
		if ( on_io_close_complete != NULL )
		{
			on_io_close_complete( on_io_close_complete_context );
		}

		result = 0;
	}

	return ( result );
}

/** Encode and send the given data to the underlying layer(s).
 * This would be called by the upper XIO layer.
 * @param tls_io Pointer to the TLS IO structure we established in tlsio_matrixssl_create().
 * @param buffer The buffer of bytes to be sent
 * @param size The size of the data in buffer[].
 * @param on_send_complete Callback to the websocket handling code,
 * 								to be invoked when the send() operation here is complete with success.
 * 								On failure, we shall not call on_send_complete.
 * @param on_send_complete_context Context given to us by the websocket handler to be given to on_send_complete().
 */
static int tlsio_matrixssl_send( CONCRETE_IO_HANDLE tls_io, const void* buffer, size_t size,
								 ON_SEND_COMPLETE on_send_complete, void* on_send_complete_context )
{
	int result;

	if ( ( tls_io == NULL ) ||
		 ( buffer == NULL ) ||
		 ( size == 0 ) )
	{
		result = __LINE__;
		LogError( "NULL tls_io on send." );
	}
	else
	{
		TLS_IO_INSTANCE* tls_io_instance = ( TLS_IO_INSTANCE* )tls_io;

		/* If we are not open, do not try to send */
		if ( tls_io_instance->tlsio_state != TLSIO_STATE_OPEN )
		{
			result = __LINE__;
			LogError( "Invalid tlsio_state %d on send. Expected state is TLSIO_STATE_OPEN.",
					  ( int ) tls_io_instance->tlsio_state );
		}
		else if ( tls_io_instance->matrix_ssl_ctx == NULL )
		{
			result = __LINE__;
			LogError( "TLS Connection not established; cannot send." );
		}
		else
		{
			/* Call here the TLS library function to encrypt the bytes. */
			int encoded_len =
				matrixSslEncodeToOutdata( tls_io_instance->matrix_ssl_ctx, ( unsigned char* ) buffer, size );
			if ( encoded_len > 0 )
			{
				LogInfo( "TLS Encrypted %u bytes into %d bytes to send", size, encoded_len );
				int rc = ssl_write_loop( tls_io_instance );
				switch ( rc )
				{
					case MATRIXSSL_SUCCESS:
						if ( on_send_complete != NULL )
						{
							on_send_complete( on_send_complete_context, IO_SEND_OK );
						}
						result = 0;
						break;

					case MATRIXSSL_REQUEST_SEND:
						//Don't really expect this case, but here goes...
						LogInfo( "Matrix TLS expects more data to complete the message!" );
						result = 0;
						break;

					case MATRIXSSL_REQUEST_CLOSE:
						LogInfo( "TLS indicates that we must close now!" );
						//Call close() but don't provide on_close() callback
						tlsio_matrixssl_close( tls_io_instance, nullptr, nullptr );
						result = 0;
						break;

					default:        //including the unexpected MATRIXSSL_HANDSHAKE_COMPLETE
						LogError( "TLS send failed, code = %d", rc );
						result = rc;
						break;
				}
			}
			else
			{
				result = __LINE__;
				LogError( "TLS encoding failed; error code = %d", encoded_len );
			}
		}
	}

	return ( result );
}

/** The DoWork function is used for normal housekeeping activities.
 * It is called periodically by the system, giving this layer a chance
 * to do anything that needs doing.
 *
 * Since most of the activity of this layer is driven by the create(), open(),
 * send(), on_underlying_io_error(), and on_underlying_io_bytes_received() functions,
 * there really isn't anything for this function to do at this layer.
 *
 * However, it is important for it to call the next underlying layer
 * so it can run its DoWork function. Ultimately, this will reach socketio_dowork(),
 * which is what drives the sending and receiving of bytes on the wire.
 * 
 * It also sometimes happens that we need to get Matrix SSL to send something it
 * is already ready to send, eg after the websocket upgrade message has been sent.
 * So check the semaphore to see if we have been signaled for that.
 *
 * @param tls_io Pointer to the TLS IO structure we established in tlsio_matrixssl_create().
 */
static void tlsio_matrixssl_dowork( CONCRETE_IO_HANDLE tls_io )
{
	/* check arguments */
	if ( tls_io == NULL )
	{
		LogError( "NULL tls_io on DoWork." );
	}
	else
	{
		TLS_IO_INSTANCE* tls_io_instance = ( TLS_IO_INSTANCE* )tls_io;

		/* only perform work if we are not in error */
		if ( ( tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN ) ||
			 ( tls_io_instance->tlsio_state == TLSIO_STATE_ERROR ) )
		{
			LogError( "No work can be done in TLS state %d", ( int ) tls_io_instance->tlsio_state );
		}
		else if ( tls_io_instance->underlying_io != nullptr )
		{
			xio_dowork( tls_io_instance->underlying_io );
		}

		// The 50ms timeout is satisfactory here.
		if ( tls_io_instance->handshake_proceed_semph->Pend( HANDSHAKE_WAIT_TIME ) )
		{
			// We were signaled! Get Matrix SSL to send its stuff.
			int rc = ssl_write_loop( tls_io_instance );
			// Check for one return code; the rest are don't cares for us
			if ( rc == MATRIXSSL_REQUEST_CLOSE )
			{
				LogInfo( "TLS indicates that we must close now!" );
				//Call close() but don't provide on_close() callback
				tlsio_matrixssl_close( tls_io_instance, nullptr, nullptr );
			}
		}
	}
}

/***************************************************************
***** The (unused?) option handlers ***************************
***************************************************************/

/*this function will clone an option given by name and value*/
static void* tlsio_matrixssl_clone_option( const char* name, const void* value )
{
	void* result;

	if ( ( name == NULL ) || ( value == NULL ) )
	{
		LogError( "invalid parameter detected: const char* name=%p, const void* value=%p", name,
				  value );
		result = NULL;
	}
	else
	{
		/* TODO: This only handles TrustedCerts, if you need to handle more options specific to your TLS library, fill
		   the code in here

		   if (strcmp(name, "...my_option...") == 0)
		   {
		   // ... copy the option and assign it to result to be returned.
		   }
		   else
		 */
		if ( strcmp( name, "TrustedCerts" ) == 0 )
		{
			if ( mallocAndStrcpy_s( ( char** )&result, ( char const* )value ) != 0 )
			{
				LogError( "unable to mallocAndStrcpy_s TrustedCerts value" );
				result = NULL;
			}
			else
			{
				/*return as is*/
			}
		}
		else
		{
			LogError( "not handled option : %s", name );
			result = NULL;
		}
	}
	return ( result );
}

/*this function destroys an option previously created*/
static void tlsio_matrixssl_destroy_option( const char* name, const void* value )
{
	/*since all options for this layer are actually string copies., disposing of one is just calling free*/

	if ( ( name == NULL ) || ( value == NULL ) )
	{
		LogError( "invalid parameter detected: const char* name=%p, const void* value=%p", name,
				  value );
	}
	else
	{
		/* TODO: This only handles TrustedCerts, if you need to handle more options specific to your TLS library, fill
		   the code in here

		   if (strcmp(name, "...my_option...") == 0)
		   {
		   // ... free any resources for the option
		   }
		   else
		 */
		if ( strcmp( name, "TrustedCerts" ) == 0 )
		{
			Ram::De_Allocate( ( void* )value );
		}
		else
		{
			LogError( "not handled option : %s", name );
		}
	}
}

//iot_open() will ultimately invoke this to set the Azure "Baltimore" trusted certificate.
static int tlsio_matrixssl_setoption( CONCRETE_IO_HANDLE tls_io, const char* optionName,
									  const void* value )
{
	int result;

	/* check arguments */
	if ( ( tls_io == NULL ) || ( optionName == NULL ) )
	{
		LogError( "NULL tls_io" );
		result = __LINE__;
	}
	else
	{
		/* We don't update the trusted_certificate member here, since we did that when
		 * initializing via Ssl_Context::Load_Keys(). So just mark this as a "pass".
		 * if ( strcmp( "TrustedCerts", optionName ) == 0 )
		 */
		result = 0;
	}

	return ( result );
}

/* We could provide one option here: a copy of our Azure Trusted Certificate.
 * This is invoked when re-establishing the connection (even the first time).
 * However, since we long ago established the trusted certificate in Ssl_Context::Load_Keys(),
 * we just do nothing here. */
static OPTIONHANDLER_HANDLE tlsio_matrixssl_retrieve_options( CONCRETE_IO_HANDLE handle )
{
	OPTIONHANDLER_HANDLE result = NULL;

	/* Codes_SRS_tlsio_template_01_064: [ If parameter handle is `NULL` then `tlsio_template_retrieve_options` shall
	   fail and return NULL. ]*/
	if ( handle == NULL )
	{
		LogError( "invalid parameter detected: CONCRETE_IO_HANDLE handle=%p", handle );
	}
	else
	{
		/* Codes_SRS_tlsio_template_01_065: [ `tlsio_template_retrieve_options` shall produce an OPTIONHANDLER_HANDLE.
		   ]*/
		result = OptionHandler_Create( tlsio_matrixssl_clone_option, tlsio_matrixssl_destroy_option,
									   tlsio_matrixssl_setoption );
		if ( result == NULL )
		{
			/* Codes_SRS_tlsio_template_01_068: [ If producing the OPTIONHANDLER_HANDLE fails then
			   tlsio_template_retrieve_options shall fail and return NULL. ]*/
			LogError( "unable to OptionHandler_Create" );
			/*return as is*/
		}
		else
		{
			/* Nothing to do here; return the empty OptionHandler to avoid complaints upstream.
			 * if ( OptionHandler_AddOption( result, "TrustedCerts", tls_io_instance->trusted_certificate ) != 0 )
			 */
			LogInfo( "Retrieved TLS Options - (nothing of interest)" );
		}
	}
	return ( result );
}

/** This is the XIO interface methods structure that Azure uses for underlying IO.
 *  Here we provide the interface methods to Matrix SSL code and data. */
static const IO_INTERFACE_DESCRIPTION tlsio_matrixssl_interface_description =
{
	tlsio_matrixssl_retrieve_options,
	tlsio_matrixssl_create,
	tlsio_matrixssl_destroy,
	tlsio_matrixssl_open,
	tlsio_matrixssl_close,
	tlsio_matrixssl_send,
	tlsio_matrixssl_dowork,
	tlsio_matrixssl_setoption
};

/* This simply returns the concrete implementations for the TLS adapter */
const IO_INTERFACE_DESCRIPTION* tlsio_matrixssl_get_interface_description( void )
{
	return ( &tlsio_matrixssl_interface_description );
}

// Simple utility that lets app code check that the tlsio_matrixssl layer is "open".
bool is_tlsio_open()
{
	if ( local_tls_io_instance == nullptr )
	{
		return ( false );
	}
	// else
	return ( local_tls_io_instance->tlsio_state == TLSIO_STATE_OPEN );
}

/******************************************************************************************
* See header file for function definition.
******************************************************************************************/
bool get_tlsio_retry_status( void )
{
  	return tls_io_retry_in_progress;
}

/******************************************************************************************
* See header file for function definition.
******************************************************************************************/
void set_tls_io_retry_status( bool retry_status )
{
  	tls_io_retry_in_progress = retry_status;
}
