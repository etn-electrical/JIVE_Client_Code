// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ssl_connection.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "app.h"
#include "OS_Tasker.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "server.h"
#include "Eth_if.h"
#include "Timers_Lib.h"
#include "uC_Watchdog.h"
#include "mtxssl_if.h"
#include "TLS_Debug.h"
#include "LWIPService.h"

#ifdef LTK_TLS_DEBUG
#include "uC_CStack.h"
#include "Ram.h"
#include "OS_Task_Mon.h"
#endif

#ifdef LTK_TLS_DEBUG
uint32_t Ssl_Connection::first_conn_start_time = 0;
#endif

#ifdef LTK_TLS_DEBUG
struct debug_ssl_time debug_time[DEBUG_ARRAY_SIZE];
struct debug_max_data max_data;
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ssl_Connection::Ssl_Connection( Ssl_Context* ssl_context ) :
	m_ssl_context( ssl_context ),
	remote_port( 0U ),
	ip_addr( 0U ),
	m_fd( 0U ),
	m_parser( nullptr ),
	m_conn_state( COMMON_NO_STATUS ),
	m_cp( nullptr ),
	List{ nullptr }
{
	send_pending = false;
	send_pend_data = nullptr;
	send_pend_len = 0;
	webui_resource_flag = false;

#ifdef HTTPS_PERSISTENT_CONNECTION
	keep_alive = false;
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ssl_Connection::Ssl_Connection( int32_t fd, Ssl_Context* ssl_context, bool set_soc_option ) :
	m_ssl_context( ssl_context ),
	remote_port( 0U ),
	ip_addr( 0U ),
	m_conn_state( COMMON_NO_STATUS ),
	m_fd( fd ),
	List{ nullptr }
{
	send_pending = false;
	send_pend_data = nullptr;
	send_pend_len = 0;

#ifdef HTTPS_PERSISTENT_CONNECTION
	keep_alive = false;
#endif
#ifdef LTK_TLS_DEBUG
	uint32_t now = 0u;
	mtxssl_if_psGetTime( &now );
	connection_start_time = now;
	unique_id = max_data.opened_session++;
	close = ACCEPTED;
	if ( unique_id == 0 )
	{
		first_conn_start_time = connection_start_time;
	}
#endif

	if ( set_soc_option == true )
	{
		Set_Socket_Options();
	}
	m_cp = mtxssl_if_SSL_new( m_ssl_context->Get_Context(), m_fd );
	m_parser = nullptr;

	Update_Timeout();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ssl_Connection::Update_Timeout()
{
	mtxssl_if_update_timeout( m_cp, m_ssl_context->m_conn_timeout,
							  m_ssl_context->m_conn_idle_state_timeout );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ssl_Connection::Ssl_Is_Timeout()
{
	return ( mtxssl_if_timeout_status( m_cp ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ssl_Connection::Idle_State_Timeout_Status()
{
	return ( mtxssl_if_idle_state_timeout_status( m_cp ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ssl_Connection::~Ssl_Connection()
{
	mtxssl_if_SSL_free( m_cp );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int Ssl_Connection::Ssl_Accept()
{
	return ( mtxssl_if_accept( m_cp ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int Ssl_Connection::Ssl_Send( char* data, int length )
{
	int bytesSent = 0;

	if ( length < 0 )
	{
		data = nullptr;
		length = 0U;
	}
	bytesSent = mtxssl_if_write( m_cp, data, length );

	return ( bytesSent );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Ssl_Connection::Get_Out_Data()
{
	return ( mtxssl_if_matrixSslGetOutdata( m_cp, nullptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int Ssl_Connection::Send_All( char* data, int length )
{
	uint32_t retry_count = 0;

	mtxssl_if_clear_wouldblock( m_cp );
	int bytesSent = Ssl_Send( data, length );

	while ( bytesSent < 1 )
	{
		if ( ( bytesSent < 0 ) && ( mtxssl_if_get_wouldblock( m_cp ) == 1 ) &&
			 ( retry_count++ < ( WOULD_BLOCK_TIMEOUT_MS / WOULDBLOCK_DELAY_TIME ) ) )
		{
#ifdef LTK_TLS_DEBUG
			wouldblk_delay_time_ms += WOULDBLOCK_DELAY_TIME;
#endif
			OS_Tasker::Delay( WOULDBLOCK_DELAY_TIME );
			mtxssl_if_clear_wouldblock( m_cp );
			bytesSent = Ssl_Send( nullptr, 0 );
		}
		else
		{
			int32_t last_socket_err = Get_LWIP_Errno();
			SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Ssl_Send failed, Socket error= %d",
								  last_socket_err );
			return ( -1 );
		}
	}
	return ( bytesSent );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int Ssl_Connection::Get_Ssl_Error()
{
	int ret_val = 0;		// initialize ret_val

	return ( mtxssl_if_SSL_get_error( m_cp, ret_val ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int Ssl_Connection::Receive( char* data, int length )
{
	mtxssl_if_clear_wouldblock( m_cp );
	int32_t rc = mtxssl_if_SSL_read( m_cp, data, length );

	if ( rc < 1 )
	{
		if ( rc < 0 )
		{
			if ( mtxssl_if_get_wouldblock( m_cp ) > 0 )
			{
				rc = 0;
			}
#ifdef USE_REHANDSHAKING
			else if ( mtxssl_if_rehandshake_status( m_cp ) == 1 )	/// > Non Zero return value means
			// rehandshake is enabled
			{
				int rc_temp = Do_Rehandshake();
				if ( rc_temp > 0 )
				{
					SSL_CONN_DEBUG_PRINT( DBG_MSG_INFO, "Re-handshaking Successful" );
					rc = 0;
				}
				else
				{
					SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Re-handshaking failed" );
					rc = -1;
				}

			}
#endif
			else
			{
				rc = -1;
				SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Receive failed- possibly socket error" );
			}
		}
		else
		{
			rc = -1;
			SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Receive failed- possibly remote connection closed" );
		}
	}
#ifdef USE_REHANDSHAKING
	else
	{
		if ( mtxssl_if_rehandshake_status( m_cp ) == 1 )/// > Non Zero return value means
		// rehandshake is enabled
		{
			int return_val = Do_Rehandshake();
			if ( return_val < 1 )
			{
				SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Re-handshaking failed" );
				rc = -1;
			}
			else
			{
				SSL_CONN_DEBUG_PRINT( DBG_MSG_INFO, "Re-handshaking Successful" );
			}
		}
	}
#endif
	return ( rc );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int Ssl_Connection::Receive_all( char* data, int length )
{
	return ( Receive( data, length ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int Ssl_Connection::Receive( unsigned char** data )
{
	int32_t length = 0;

	mtxssl_if_clear_wouldblock( m_cp );
	int32_t rc = mtxssl_if_SSL_get_data( m_cp, data, &length );

	if ( rc < 1 )
	{
		if ( ( rc < 0 ) && ( mtxssl_if_get_wouldblock( m_cp ) == 1 ) )
		{
			rc = 0;
		}
		else
		{
			rc = -1;
			SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Receive failed- possibly socket error" );
		}
	}
	return ( rc );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int Ssl_Connection::Receive_All( unsigned char** data )
{
	return ( Receive( data ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Ssl_Connection::Do_Handshake()
{
	int32_t return_val = 0;

	mtxssl_if_clear_wouldblock( m_cp );
	int32_t rc = mtxssl_if_do_handshake( m_cp );

	if ( rc > 0 )
	{
#ifdef LTK_TLS_DEBUG
		uint32_t now = 0u;
		mtxssl_if_psGetTime( &now );
		handshake_time = now - connection_start_time;
#endif
		m_conn_state |= COMMON_HS_COMPLETE;
#ifdef USE_REHANDSHAKING
		mtxssl_if_ssl_rehandshake_disable( m_cp );
#endif
		return_val = 1;
	}
	else if ( ( rc == 0 ) || ( mtxssl_if_get_wouldblock( m_cp ) == 0 ) )
	{
		return_val = -1;
		int32_t last_socket_err = Get_LWIP_Errno();
		SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Receive failed- possibly socket error, Socker Error= %d",
							  last_socket_err );
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ssl_Connection::Close_Connection()
{
	m_conn_state = COMMON_NO_STATUS;
#ifdef LTK_TLS_DEBUG
	uint32_t now = 0u;
	mtxssl_if_psGetTime( &now );
	connection_time = now - connection_start_time;
	if ( unique_id < DEBUG_ARRAY_SIZE )
	{
		debug_time[unique_id].hs_time = handshake_time;
		debug_time[unique_id].process_time = process_time;
		debug_time[unique_id].total_conn_time = connection_time;
		debug_time[unique_id].loop_count = while_loop_count;
		debug_time[unique_id].close = close;
		debug_time[unique_id].bytesSent = bytesSent;
		debug_time[unique_id].total_length = total_length;
		debug_time[unique_id].remote_port = remote_port;
	}
	if ( handshake_time > max_data.max_handshake_time )
	{
		max_data.max_handshake_time = handshake_time;
		max_data.max_handshake_id = unique_id;
	}
	if ( process_time > max_data.max_process_time )
	{
		max_data.max_process_time = process_time;
		max_data.max_process_id = unique_id;
	}
	if ( connection_time > max_data.max_connection_time )
	{
		max_data.max_connection_time = connection_time;
		max_data.max_connection_id = unique_id;
	}
	if ( while_loop_count > max_data.max_loop_count )
	{
		max_data.max_loop_count = while_loop_count;
		max_data.max_loop_id = unique_id;
	}
	max_data.max_cstack_percent = uC_CSTACK_Used();
	max_data.max_heap_percent = Ram::Used();
	max_data.max_task_stack_percent = OS_Task_Mon::Fragile_Task_Stack_Usage();

	max_data.time_since_1st_conn = ( now - first_conn_start_time );			/// 1000; // in second
	max_data.max_wouldblk_delay_time_ms =
		( max_data.max_wouldblk_delay_time_ms < wouldblk_delay_time_ms ) ? wouldblk_delay_time_ms :
		max_data.max_wouldblk_delay_time_ms;
	max_data.closed_session++;

#endif

	if ( m_fd != INVALID_SOCKET )
	{
		close( m_fd );	// TODO: bring this back to opensslSocket.c
	}

	/* Instead of calling mtxssl_if_SSL_free( ) only while closing the connection.
	 * Calling the same in the default destructor of Ssl_Connection to freeup the
	 * memory
	 */
	// mtxssl_if_SSL_free( m_cp ); ///* Commented to fix LTK-7375 bug */
}

#if 0
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ssl_Connection::set_ssl_options( sslSessOpts_t* options )
{
	bool result = true;

	memset( options, 0x0, sizeof( sslSessOpts_t ) );
	options->versionFlag = SSL_FLAGS_TLS_1_2;
	// options->userPtr = m_keys;
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static int receiveCallback( WOLFSSL* ssl, char* buf, int sz, void* ctx )
{
	int fd = *( int* )ctx;
	int result;

	( void )ssl;

	fd_set rfds;

	FD_ZERO( &rfds );
	FD_SET( fd, &rfds );

	if ( lwip_select( FD_SETSIZE, &rfds, nullptr, nullptr, &receiveTimeout ) < 0 )
	{
		result = -1;
	}
	else
	{
		result = lwip_recv( fd, buf, sz, 0 );
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static int sendCallback( WOLFSSL* ssl, char* buf, int sz, void* ctx )
{
	int fd = *( int* )ctx;
	int result;

	( void )ssl;

	fd_set wfds;

	FD_ZERO( &wfds );
	FD_SET( fd, &wfds );

	if ( lwip_select( FD_SETSIZE, nullptr, &wfds, nullptr, &sendTimeout ) < 0 )
	{
		return ( -1 );
	}
	else
	{
		result = lwip_send( fd, buf, sz, 0 );
	}

	return ( result );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int Ssl_Connection::Ssl_Connect( const char* host, const int port )
{
	int result = 1;
	uint32_t retry_count = 0;

	if ( !( m_conn_state & CLIENT_SSL_INIT_DONE ) )
	{
		mtxssl_if_set_hostname( m_cp, host );
		result = mtxssl_if_SSL_connect( m_cp );
		if ( result > 0 )
		{
			m_conn_state |= CLIENT_SSL_INIT_DONE;
		}
		else
		{
			SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Unable to establish connection" );
		}
	}
	else
	{
		SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Client SSL Initiation failed" );
	}
	if ( result > 0 )
	{
		result = Do_Handshake();
		while ( ( result == 0 ) &&
				( ++retry_count < ( ( SSL_CONNECT_TIMEOUT_MS ) / WOULDBLOCK_DELAY_TIME ) ) )
		{
			OS_Tasker::Delay( WOULDBLOCK_DELAY_TIME );
			result = Do_Handshake();
		}
		if ( result > 0 )
		{
			m_conn_state |= COMMON_HS_COMPLETE;
		}
		else
		{
			SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Handshake faiure, Connection state= %d", m_conn_state );
		}
	}
	else
	{
		int32_t last_socket_err = Get_LWIP_Errno();
		SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Unable to connect, Socket error= %d", last_socket_err );
	}
	return ( result );
}

#ifdef USE_REHANDSHAKING
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Ssl_Connection::Do_Rehandshake()
{
	int result;
	uint32_t retry_count = 0;

	result = Do_Handshake();

	while ( ( result == 0 ) &&
			( ++retry_count < ( ( SSL_HANDSHAKE_TIMEOUT_MS ) / WOULDBLOCK_DELAY_TIME ) ) )
	{
#ifdef LTK_TLS_DEBUG
		wouldblk_delay_time_ms += WOULDBLOCK_DELAY_TIME;
#endif
		OS_Tasker::Delay( WOULDBLOCK_DELAY_TIME );
		result = Do_Handshake();
	}
	if ( result < 0 )
	{
		int32_t last_socket_err = Get_LWIP_Errno();
		SSL_CONN_DEBUG_PRINT( DBG_MSG_ERROR, "Do_Rehandshake failed, Socket error= %d, Retry Count= %d",
							  last_socket_err, retry_count );
	}

	return ( result );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ssl_Connection::Set_Socket_Options( void )
{
	int32_t rc = 1;

	setsockopt( m_fd, SOL_SOCKET, SO_REUSEADDR, ( char* )&rc, sizeof ( rc ) );
	mtxssl_if_setSocketNodelay( m_fd );
	mtxssl_if_setSocketNonblock( m_fd );
	m_conn_state |= NONBLOCKING;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Ssl_Connection::Processed_Data( uint8_t** data, int32_t length )
{
	return ( mtxssl_if_SSL_processed_data( m_cp, data, &length ) );
}
