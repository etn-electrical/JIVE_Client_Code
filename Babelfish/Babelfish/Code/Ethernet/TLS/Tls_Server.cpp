/**
 **********************************************************************************************
 * @file            tls_server.c
 *
 * @brief           TLS Web Server Implementation based on matrix-ssl cryptography library
 * @details         This file include custom TLS Web server implementation based on
 *                  Matrix-ssl cryptography library. TLS web server use LWIP as
 *                  underlying socket interface and PX Green HAMS Web Server
 *                  to support HTTP APIs.
 *
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "OS_Tasker.h"
#include "Timers_Lib.h"
#include "server.h"
#include "lwip/pbuf.h"
#include "lwip/sockets.h"

#include "app.h"
#include "matrixssl/matrixsslConfig.h"
#include "mtxssl_if.h"
#include "ssl_connection.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwipopts.h"
#include "Eth_if.h"
#include "Server_IO.h"
#include "Ssl_Context.h"
#include "HttpsConfig.h"
#include "TLS_Debug.h"
#include "Web_Debug.h"
#include "Babelfish_Assert.h"
#include "Rest_WebUI_Interface.h"
#include "OS_Binary_Semaphore.h"
#ifdef ESP32_SETUP
#include "OS_Task_Config.h"
#endif

#ifdef USE_SERVER_SIDE_SSL

#include <signal.h>			/* Defines SIGTERM, etc. */

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
enum tls_server_state_t
{
	HTTPS_SUCCESS,
	HTTPS_BUF_ERR,
	HTTPS_SSL_ERR,
	HTTPS_SOCKET_ERR,
	HTTPS_PROCESS_COMPLETE,
	HTTPS_MAX_STATE
};

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
static DLListEntry g_conns;
static Ssl_Context* m_ssl_context = nullptr;

#ifdef WEBUI_ON_EXT_FLASH
// Protect HTTP and HTTPS from simultaneous read from NV and transmit the data
extern OS_Binary_Semaphore* webui_semaphore;
#endif	// WEBUI_ON_EXT_FLASH

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
static bool Server_Process( hams_parser* parser, struct pbuf* p );

uint32_t S_Https_Send_Cb( hams_parser* parser, const uint8_t* data,
						  uint32_t total_length, unsigned copy );

static void Tls_Server( void* pParams );

static void Select_Loop( void* keys, SOCKET lfd );

static void Set_Socket_Options( SOCKET fd );

static SOCKET LSocket_Listen( short port, int32_t* err );

static int32_t Process_Tls_Data( Ssl_Connection* ssl_con, uint8_t* data_buf,
								 int32_t data_length );

static void Close_Connection( Ssl_Connection* ssl_con );

uint32_t Https_Send_Static_Resource( Ssl_Connection* con );

/*
    This function is used to send Webui Data for HTTPS page.
    From server.c file in function s_xmit_webui_data() "copy" variable is
    set to '2' for https page webui data. In s_https_send_cb() function
    con->webui_resource_flag is set to identify weather it needs to send
    the static data or webui data. In SelectLoop() Function
    con->webui_resource_flag is check to initiate the valid function call.
 */
uint32_t Https_Send_Webpage_Resource( Ssl_Connection* con );

void Attach_TLS_Server_SSL_Context( Ssl_Context* ssl_context );

/******************************************************************************/
/*
   Non-blocking socket event handler
   Wait one time in select for events on any socket
   This will accept new connections, read and write to sockets that are
   connected, and close sockets as required.
 */
static void Select_Loop( void* keys, SOCKET lfd )
{
	DLListEntry pend_conns;
	Ssl_Connection* con;
	DLListEntry connsTmp;
	DLListEntry* pList;

	fd_set readfd, writefd;
	struct timeval timeout;
	SOCKET fd, maxfd;

	int32_t return_val = PS_SUCCESS_T;
	int32_t active_conn = 0;

	DLListInit( &connsTmp );
	DLListInit( &pend_conns );

	maxfd = INVALID_SOCKET;
	timeout.tv_sec = SELECT_TIME / 1000;
	timeout.tv_usec = ( SELECT_TIME % 1000 ) * 1000;
	FD_ZERO( &readfd );
	FD_ZERO( &writefd );

	/* Always set readfd for listening socket */
	FD_SET( lfd, &readfd );
	if ( lfd > maxfd )
	{
		maxfd = lfd;
	}
	/*
	   Check timeouts and set readfd and writefd for connections as required.
	   We use connsTemp so that removal on error from the active iteration list
	   doesn't interfere with list traversal
	 */
	while ( !DLListIsEmpty( &g_conns ) )
	{
		pList = DLListGetHead( &g_conns )
		;
		con = DLListGetContainer( pList, Ssl_Connection, List );
		mtxssl_if_DLListInsertTail( &connsTmp, &con->List );
		/*	If timeout != 0 msec ith no new data, close */

		if ( con->Ssl_Is_Timeout()
#ifdef HTTPS_PERSISTENT_CONNECTION
			/* Close connection once tls server has served complete HTTP request and
			   socket is in idle state for more than KEEP-ALIVE time */
			 || ( ( con->Idle_State_Timeout_Status() == true ) && ( con->Get_Parser_Allocated() == false ) )
#endif
			 )
		{
#ifdef LTK_TLS_DEBUG
			con->close = TIMEOUT_CLOSE;
#endif
			Close_Connection( con );
			DLListRemove( &con->List );
			delete con;
		}
		else
		{
			/* Always select for read */
			FD_SET( con->Get_FD(), &readfd );
			/* Housekeeping for maxsock in select call */
			if ( con->Get_FD() > maxfd )
			{
				maxfd = con->Get_FD();
			}
			active_conn++;
		}
	}

	/* Use select to check for events on the sockets */
	if ( select( maxfd + 1, &readfd, &writefd, NULL, &timeout ) <= 0 )
	{
		return_val = PS_PLATFORM_FAIL_T;

		/* On error, restore global connections list */
		while ( !DLListIsEmpty( &connsTmp ) )
		{
			// No event observed. Thus move :
			// Pending connections to "pend_conns" list
			// Impending connections to "g_conns" list

			pList = DLListGetHead( &connsTmp );
			con = DLListGetContainer( pList, Ssl_Connection, List );

			if ( con->send_pending == true )
			{
				mtxssl_if_DLListInsertTail( &pend_conns, &con->List );
				return_val = PS_SUCCESS_T;
			}
			else
			{
				mtxssl_if_DLListInsertTail( &g_conns, &con->List );
			}
		}
		if ( return_val == PS_SUCCESS_T )
		{
			while ( !DLListIsEmpty( &pend_conns ) )
			{
				pList = DLListGetHead( &pend_conns );
				con = DLListGetContainer( pList, Ssl_Connection, List );
				mtxssl_if_DLListInsertTail( &connsTmp, &con->List );
			}
		}
	}

	if ( return_val == PS_SUCCESS_T )
	{
#ifdef LTK_TLS_DEBUG
		uint16_t DLList_size = 0;
		uint32_t select_start_time = 0U;
		mtxssl_if_psGetTime( &select_start_time );
#endif
		/* Check listener for new incoming socket connections */
		struct sockaddr client_addr = { };
		socklen_t addrlen = sizeof ( client_addr );

		if ( FD_ISSET( lfd, &readfd ) )
		{
			const int32_t dyn_accept_queue = ( MAXCONN - active_conn );
			for ( int32_t acceptSanity = 0; acceptSanity < dyn_accept_queue;
				  acceptSanity++ )
			{
				fd = accept( lfd, &client_addr, &addrlen );
				/* if invalid, Nothing more to accept; next listener */
				if ( fd != INVALID_SOCKET )
				{
					con = new Ssl_Connection( fd, m_ssl_context );

					con->remote_port = ( client_addr.sa_data[0] << 8U ) | ( client_addr.sa_data[1] );
					con->ip_addr = ( client_addr.sa_data[5] << 24U ) | ( client_addr.sa_data[4] << 16U ) |
						( client_addr.sa_data[3] << 8U ) | ( client_addr.sa_data[2] );

					if ( con->Ssl_Accept() < 0 )
					{
						Close_Connection( con );
						delete con;
					}
					else
					{
						// set readfd and expect data
						FD_SET( fd, &readfd );
						mtxssl_if_DLListInsertTail( &connsTmp, &con->List );
					}
					fd = INVALID_SOCKET;
				}
				else
				{
					acceptSanity = dyn_accept_queue;
				}
			}
		}
		/* Check each connection for read/write activity */

		while ( !DLListIsEmpty( &connsTmp ) )
		{
#ifdef LTK_TLS_DEBUG
			DLList_size++;
			if ( max_data.max_active_conn < DLList_size )
			{
				max_data.max_active_conn = DLList_size;
			}
#endif
			pList = DLListGetHead( &connsTmp );
			Ssl_Connection* con = DLListGetContainer( pList, Ssl_Connection, List );
			mtxssl_if_DLListInsertTail( &g_conns, &con->List );


			if ( ( FD_ISSET( con->Get_FD(), &readfd ) ) || ( con->send_pending == true ) )
			{
				con->Update_Timeout();
				if ( !con->Is_Handshake_Complete() )
				{
					if ( con->Do_Handshake() < 0 )
					{
						TLS_SERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Handshake Failed" );
						Close_Connection( con );
						DLListRemove( &con->List );
						delete con;
						continue;
					}
				}
				if ( con->Is_Handshake_Complete() )
				{
					TLS_SERVER_DEBUG_PRINT( DBG_MSG_INFO, "Handshake Complete" );
					// 1 ms delay after eash connection served.
					OS_Tasker::Delay( 1U );

					int32_t process_status = HTTPS_MAX_STATE;
					uint8_t* http_buf = nullptr;
					int32_t buf_size = 0;
					while ( ( buf_size = con->Receive_All( &http_buf ) ) > 0 )
					{
						do
						{
							if ( process_status != HTTPS_PROCESS_COMPLETE )
							{
								process_status = Process_Tls_Data( con, ( uint8_t* )http_buf, buf_size );
							}
							buf_size = con->Processed_Data( &http_buf, buf_size );
						} while ( buf_size > 0 );
					}

					if ( con->send_pending == true )
					{
						uint32_t status = 0;
						if ( con->webui_resource_flag == true )
						{
							status = Https_Send_Webpage_Resource( con );
						}
						else
						{
							status = Https_Send_Static_Resource( con );
						}

						// 0 = All data transffered
						// 1 = CHUNK has been sent out.
						// 2 = Error occured during transmission
						if ( status == 0U )
						{
							TLS_SERVER_DEBUG_PRINT( DBG_MSG_INFO, "All data is transferred" );
							con->send_pending = false;
							con->webui_resource_flag = false;
							process_status = HTTPS_PROCESS_COMPLETE;
						}
						else if ( status == 1U )
						{
							// socket got blocked. Do something else.
							TLS_SERVER_DEBUG_PRINT( DBG_MSG_INFO, "Socket got blocked" );
							con->Update_Timeout();
							// We have spent SERVING_TIME_PER_CONN time ( 1000 ms ) for tls_server
							// Lets give 2 msec time to lower priority routines.
							OS_Tasker::Delay( 2U );
						}
						else
						{
							// Got socket error.
							TLS_SERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Socket Error" );
							con->send_pending = false;
							con->webui_resource_flag = false;
							process_status = HTTPS_PROCESS_COMPLETE;
						}
					}

					if ( ( buf_size < 0 ) ||
						 ( process_status == HTTPS_PROCESS_COMPLETE ) )
					{
						if ( con->send_pending != true )
						{
#ifdef LTK_TLS_DEBUG
							con->close = NORMAL_CLOSE;
							uint32_t now = 0U;
							mtxssl_if_psGetTime( &now );
							con->process_time = now - con->connection_start_time;
#endif
							// send pending data if any
							con->Send_All( nullptr, 0 );
#ifdef HTTPS_PERSISTENT_CONNECTION
							/* Do not close connection when Keep-Alive is enabled */
							if ( ( con->keep_alive == false ) || ( buf_size < 0 ) )
#endif
							{
								Close_Connection( con );
								DLListRemove( &con->List );
								delete con;
							}
						}
					}
#ifdef LTK_TLS_DEBUG
					else
					{
						con->while_loop_count++;
					}
#endif
				}
			}
		}
#ifdef LTK_TLS_DEBUG
		uint32_t now_time = 0U;
		mtxssl_if_psGetTime( &now_time );
		if ( ( now_time - select_start_time ) > max_data.max_select_time )
		{
			max_data.max_select_time = now_time - select_start_time;
		}
#endif

	}	/* connection loop */
	// coverity[leaked_storage]
}

/******************************************************************************/
/*
   Main non-blocking SSL server
   Initialize MatrixSSL and sockets layer, and loop on select
 */
void Tls_Server( void* pParams )
{
	SOCKET lfd;
	int32_t err;
	void* keys = NULL;

	DLListInit( &g_conns );
	lfd = INVALID_SOCKET;


	/* Create the listening socket that will accept incoming connections */
	while ( lfd == INVALID_SOCKET )
	{
		lfd = LSocket_Listen( HTTPS_PORT, &err );
		if ( lfd == INVALID_SOCKET )
		{
			/* 1 sec delay if socket not allocated */
			OS_Tasker::Delay( 1000U );
		}
	}
	Eth_if::Wait_Till_Ip_Configured();
	/* Wait for ssl context */
	BF_ASSERT( m_ssl_context != nullptr );

	m_ssl_context->Wait_For_SSL_Context();

	Register_Media_On_Server( HTTPS_MEDIA, nullptr, S_Https_Send_Cb );

#ifdef HTTPS_PERSISTENT_CONNECTION
	int32_t keep_alive_millisec_time = KEEP_ALIVE_TIME * 1000U;
	m_ssl_context->Initialize_Timeout( SSL_TIMEOUT, keep_alive_millisec_time );
#else
	m_ssl_context->Initialize_Timeout( SSL_TIMEOUT );
#endif
	/* Main select loop to handle sockets events */
	// coverity[no_escape]
	while ( 1 )
	{
		Select_Loop( keys, lfd );
	}
}

/*
 *****************************************************************************************
 * Establish a listening socket for incomming connections
 *****************************************************************************************
 */
static SOCKET LSocket_Listen( short port, int32_t* err )
{
	struct sockaddr_in addr;
	SOCKET fd;

	if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
	{
		*err = SOCKET_ERRNO;
		return ( INVALID_SOCKET );
	}

	Set_Socket_Options( fd );

	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = INADDR_ANY;
	if ( bind( fd, ( struct sockaddr* ) &addr, sizeof( addr ) ) < 0 )
	{
		*err = SOCKET_ERRNO;
		return ( INVALID_SOCKET );
	}
	if ( listen( fd, MAXCONN ) < 0 )
	{
		*err = SOCKET_ERRNO;
		return ( INVALID_SOCKET );
	}
	return ( fd );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void Set_Socket_Options( SOCKET fd )
{
	int32_t rc;

	rc = 1;
	setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, ( char* )&rc, sizeof ( rc ) );
	rc = 1;
	setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, ( void* )&rc, sizeof ( rc ) );
	rc = 1;
	ioctlsocket( fd, FIONBIO, &rc );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t S_Https_Send_Cb( hams_parser* parser, const uint8_t* data,
						  uint32_t total_length, unsigned copy )
{
	uint32_t ret_status = 0;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	Ssl_Connection* con = ( ( Ssl_Connection* )rs->stack_data );
	uint8_t* src_buf = ( uint8_t* )data;
	uint32_t local_bytesSent = 0U;

	WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "Send HTTPS Server Page Data" );

	if ( copy >= WRITE_FLAG_DYNAMIC_COPY )
	{
		if ( copy == WRITE_FLAG_NV_CTRL_COPY )
		{
			con->send_pending = true;
			con->send_pend_data = const_cast<uint8_t*>( data );
			con->send_pend_len = total_length;
			con->webui_resource_flag = true;
		}
		else if ( ( data != NULL ) && ( total_length > 0U ) )
		{
#ifdef LTK_TLS_DEBUG
			con->total_length += total_length;
#endif
			int32_t bytesReturned = 0;
			uint32_t send_buf_length = 0U;
			while ( local_bytesSent < total_length && ret_status == 0 )
			{
				int32_t out_data = con->Get_Out_Data();
				if ( out_data > -1 )
				{
					if ( out_data < MAX_PLAIN_TEXT_IN_OUT_BUF )	/// > All data sent
					{
						send_buf_length =
							min( total_length - local_bytesSent, ( MAX_PLAIN_TEXT_IN_OUT_BUF - out_data ) );
						bytesReturned = con->Send_All( ( char* )src_buf, send_buf_length );
					}
					else				/// > Send remaining data
					{
						send_buf_length = 0U;
						bytesReturned = con->Send_All( nullptr, send_buf_length );
					}
					if ( bytesReturned > 0 )
					{
						local_bytesSent += send_buf_length;
						src_buf += send_buf_length;
#ifdef LTK_TLS_DEBUG
						con->bytesSent += send_buf_length;
#endif
					}
					else
					{
						ret_status = 2;
					}
				}
				else
				{
					ret_status = 2;
				}
			}
		}
		else if ( copy == WRITE_FLAG_CLOSE_COPY )
		{
			con->Set_Parser_Allocated( false );
		}
	}
	else// if ( copy == WRITE_FLAG_STATIC_COPY )
	{
		con->send_pending = true;
		con->send_pend_data = const_cast<uint8_t*>( data );
		con->send_pend_len = total_length;
		con->webui_resource_flag = false;
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static int32_t Process_Tls_Data( Ssl_Connection* con, uint8_t* data_buf,
								 int32_t data_length )
{
	int32_t return_val = HTTPS_SUCCESS;

	hams_parser* parser = nullptr;

	if ( con->Get_Parser_Allocated() == false )
	{
		parser = Http_Server_Alloc_Parser( ( void* )con, true );

		if ( parser != nullptr )
		{
			con->Set_Parser_Allocated( true );
			con->Set_Parser( parser );

			parser->general_use_reg = con->ip_addr;
			parser->remote_port = con->remote_port;
		}
		else
		{
			return_val = HTTPS_PROCESS_COMPLETE;
		}
	}
	else
	{
		if ( con->Get_Parser() != NULL )
		{
			parser = ( hams_parser* )con->Get_Parser();
		}
		else
		{
			return_val = HTTPS_PROCESS_COMPLETE;
		}
	}

	if ( return_val == HTTPS_SUCCESS )
	{
		bool hams_err;
		struct pbuf p;
		p.payload = ( void* )data_buf;
		p.len = data_length;
		hams_err = Server_Process( parser, &p );
		if ( hams_err != true )
		{
			if ( ( ( parser->flags & HAMS_FLAG_ERROR ) == 0U ) &&
				 ( ( parser->flags & HAMS_FLAG_REDIRECT ) == 0U ) &&
				 ( con->Get_Parser_Allocated() == true ) )
			{
				while ( Http_Server_Output_Sent( parser, 1U ) )
				{
					// This delay will allow routines to get executed for large payload request.
					OS_Tasker::Delay( 1U );
				}
			}
			con->Set_Parser_Allocated( false );
			return_val = HTTPS_PROCESS_COMPLETE;
#ifdef HTTPS_PERSISTENT_CONNECTION
			if ( HTTP_METHOD_GET == parser->method )
			{
				/* PUT,POST and DELETE does not use persistant connection.
				   Thus instead of keeping socket in idle state, we are closing from server side.
				   Now we are keeping socket alive only for GET request */
				con->keep_alive = true;
			}
#endif
		}
		else
		{}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Https_Init( void )
{
	bool init_success = true;

	// Create task using "sys_thread_new" to use timeout functionality of lwip/core/sys.c
	if ( sys_thread_new( ( char* )TLS_THREAD_NAME, Tls_Server, NULL,
						 TLS_THREAD_STACKSIZE, TLS_THREAD_PRIO ) == NULL )
	{
		init_success = false;
		WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "HTTPS Server Initializatiion Failed" );
	}

	return ( init_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Server_Process( hams_parser* parser, struct pbuf* p )
{
	uint16_t length = 0U;
	uint32_t processed_length = 0;
	bool_t loop_break = false;
	bool return_val = false;

	while ( ( p->len > 0 ) && ( false == loop_break ) )
	{
		uint32_t buf = ( uint32_t )p->payload + length;
		p->payload = ( void* ) ( buf );
		length = p->len > 255 ? 255 : p->len;
		processed_length += length;
		p->len -= length;
		if ( !( Http_Server_Input( parser,
								   ( reinterpret_cast<const uint8_t*>( p->payload ) ), length )
				& HAMS_EXPECT_RX ) )
		{
			/* On error, just drop further input. */
			loop_break = true;
		}
		else
		{
			if ( p->len < 1 )
			{
				return_val = true;
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void Close_Connection( Ssl_Connection* ssl_con )
{
	if ( ssl_con->Get_Parser_Allocated() == true )
	{
		hams_parser* parser = reinterpret_cast<hams_parser*>( ssl_con->Get_Parser() );
		Http_Server_Output_Sent( parser, 0 );
		ssl_con->Set_Parser_Allocated( false );
	}
	ssl_con->Close_Connection();
	WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "HTTPS Server SSL Connection closed" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Attach_TLS_Server_SSL_Context( Ssl_Context* ssl_context )
{
	if ( ssl_context != nullptr )
	{
		m_ssl_context = ssl_context;
	}
}

#endif	/* USE_SERVER_SIDE_SSL */
/******************************************************************************/

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Https_Send_Static_Resource( Ssl_Connection* con )
{
	int32_t bytesReturned = 0;
	uint32_t send_buf_length = 0U;
	uint8_t* src_buf = con->send_pend_data;
	uint32_t total_length = con->send_pend_len;
	uint32_t local_bytesSent = 0U;
	uint32_t ret_status = 0U;

	uint32_t current_time = 0;
	uint32_t loop_break_time = 0;

	mtxssl_if_psGetTime( &current_time );
	loop_break_time = current_time + SERVING_TIME_PER_CONN;

#ifdef LTK_TLS_DEBUG
	con->total_length += total_length;
#endif

	while ( local_bytesSent < total_length && ret_status == 0U )
	{
		int32_t out_data = con->Get_Out_Data();
		if ( out_data > -1 )
		{
			if ( out_data < MAX_PLAIN_TEXT_IN_OUT_BUF )	/// > All data sent
			{
				send_buf_length = min( total_length - local_bytesSent, ( MAX_PLAIN_TEXT_IN_OUT_BUF - out_data ) );
				bytesReturned = con->Send_All( ( char* )src_buf, send_buf_length );
			}
			else
			{
				send_buf_length = 0U;
				bytesReturned = con->Send_All( nullptr, send_buf_length );
			}
			if ( ( bytesReturned > 0 ) && ( send_buf_length > 0U ) )
			{
				local_bytesSent += send_buf_length;
				src_buf += send_buf_length;

				mtxssl_if_psGetTime( &current_time );

				/* Store connection state only if
				   1. SERVING_TIME_PER_CONN is elapsed.
				   2. Total data is not yet transferred */
				if ( ( current_time > loop_break_time ) && ( local_bytesSent < total_length ) )
				{
					con->send_pend_data = src_buf;
					con->send_pend_len -= local_bytesSent;
					ret_status = 1U;
				}
#ifdef LTK_TLS_DEBUG
				con->bytesSent += send_buf_length;
#endif
			}
			else if ( bytesReturned == 0 )	/// > Socket got blocked. Store the current conn state.
			{
				con->send_pend_data = src_buf;
				con->send_pend_len -= local_bytesSent;
				ret_status = 1U;
				WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "Socket Got Blocked, Store The Current Connection State" );
			}
			else if ( bytesReturned < 0 )	/// > Got socket error. Close the connection.
			{
				ret_status = 2;
				WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "Socket Error, Close the Connection" );
			}
		}
		else
		{
			ret_status = 2;
		}
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Https_Send_Webpage_Resource( Ssl_Connection* con )
{
	int32_t bytesReturned = 0;
	uint32_t send_buf_length = 0U;
	uint8_t* src_buf = con->send_pend_data;
	uint32_t total_length = con->send_pend_len;
	uint32_t local_bytesSent = 0U;
	uint32_t ret_status = 0U;

	uint32_t current_time = 0;
	uint32_t loop_break_time = 0;

	mtxssl_if_psGetTime( &current_time );
	loop_break_time = current_time + SERVING_TIME_PER_CONN;

#ifdef LTK_TLS_DEBUG
	con->total_length += total_length;
#endif

	while ( local_bytesSent < total_length && ret_status == 0U )
	{
		int32_t out_data = con->Get_Out_Data();
		if ( out_data > -1 )
		{
			if ( out_data < MAX_PLAIN_TEXT_IN_OUT_BUF )	/// > All data sent
			{
				send_buf_length = min( total_length - local_bytesSent, ( MAX_PLAIN_TEXT_IN_OUT_BUF - out_data ) );
#ifdef WEBUI_ON_EXT_FLASH
				uint16_t server_flag = Get_Server_Flags();
				uint32_t address = reinterpret_cast<uint32_t>( src_buf );
				hams_parser* parser = reinterpret_cast<hams_parser*>( con->Get_Parser() );
				/* To synchronize between HTTP and HTTPS response semaphore is used to avoid the
				   simulateous access of NV data and transmission to web browser */
				if ( webui_semaphore->Pend( OS_BINARY_SEMAPHORE_INDEFINITE_TIMEOUT ) )	// acquire semaphore
				{
					if ( server_flag == HTTP_SERVER_FLAG_WEB_PAGES )
					{
						if ( WEB_PAGE_READ_BUFF_SIZE < send_buf_length )
						{
							send_buf_length = WEB_PAGE_READ_BUFF_SIZE;
						}
						src_buf = const_cast<uint8_t*>
							( Fw_WWW_Get_File_Data_Chunk( parser, address, send_buf_length ) );
					}
					// if src buff is pointing to valid data then transmit the data chunk
					if ( src_buf == nullptr )
					{
						send_buf_length = 0U;
					}
					bytesReturned = con->Send_All( ( char* )src_buf, send_buf_length );
					webui_semaphore->Post();// release semaphore
				}
				src_buf = reinterpret_cast<uint8_t*>( address );		// store the external memory address back
#else
				bytesReturned = con->Send_All( ( char* )src_buf, send_buf_length );
#endif
			}
			else
			{
				send_buf_length = 0U;
				bytesReturned = con->Send_All( nullptr, send_buf_length );
			}
			if ( ( bytesReturned > 0 ) && ( send_buf_length > 0U ) )
			{
				local_bytesSent += send_buf_length;
				src_buf += send_buf_length;

				mtxssl_if_psGetTime( &current_time );

				/* Store connection state only if
				   1. SERVING_TIME_PER_CONN is elapsed.
				   2. Total data is not yet transferred */
				if ( ( current_time > loop_break_time ) && ( local_bytesSent < total_length ) )
				{
					con->send_pend_data = src_buf;
					con->send_pend_len -= local_bytesSent;
					ret_status = 1U;
				}
#ifdef LTK_TLS_DEBUG
				con->bytesSent += send_buf_length;
#endif
			}
			else if ( bytesReturned == 0 )	/// > Socket got blocked. Store the current conn state.
			{
				con->send_pend_data = src_buf;
				con->send_pend_len -= local_bytesSent;
				ret_status = 1U;
				WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "Socket Got Blocked, Store The Current Connection State" );

			}
			else if ( bytesReturned < 0 )	/// > Got socket error. Close the connection.
			{
				ret_status = 2;
				WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "Socket Error, Close the Connection" );
			}
		}
		else
		{
			ret_status = 2;
		}
	}
	return ( ret_status );
}
