// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SSL_CONNECTION_H
#define SSL_CONNECTION_H

#include "includes.h"
#include "IoT_Config.h"
#include "matrixsslConfig.h"
#include "core/include/crypto_list.h"
#include "Ssl_Context.h"
#include "HttpsConfig.h"

// #define LTK_TLS_DEBUG 1
#ifdef LTK_TLS_DEBUG

enum ssl_close_type
{
	NO_CONN,
	ACCEPTED,
	NORMAL_CLOSE,
	TIMEOUT_CLOSE,
	BUF_ERR_CLOSE,
	REQUEST_CLOSE,
	SOCKET_CLOSE,
	NOTIFY_CLOSE,
	FATAL_CLOSE,
	SSL_ERR_CLOSE,
	MAX_CLOSE
};

struct debug_ssl_time
{
	enum ssl_close_type close;
	uint16_t hs_time;
	uint16_t process_time;
	uint16_t total_conn_time;
	uint16_t loop_count;
	uint32_t bytesSent;
	uint32_t total_length;
	uint16_t remote_port;
};

// value of DEBUG_ARRAY_SIZE should be atleast 1
#define DEBUG_ARRAY_SIZE 1

struct debug_max_data
{
	uint16_t opened_session;
	uint16_t closed_session;
	uint16_t max_handshake_id;
	uint16_t max_handshake_time;
	uint16_t max_process_id;
	uint16_t max_process_time;
	uint16_t max_connection_id;
	uint16_t max_connection_time;
	uint16_t max_loop_id;
	uint16_t max_loop_count;
	uint16_t max_wouldblk_delay_time_ms;
	uint16_t max_memory_reallocated;
	uint16_t max_active_conn;
	uint16_t max_select_time;
	uint32_t time_since_1st_conn;
	uint8_t max_cstack_percent;
	uint8_t max_heap_percent;
	uint8_t max_task_stack_percent;
};

extern struct debug_ssl_time debug_time[DEBUG_ARRAY_SIZE];
extern struct debug_max_data max_data;

#endif

enum ssl_conn_state
{
	COMMON_NO_STATUS = 0,
	CLIENT_SSL_INIT_DONE = ( 1 << 0 ),
	COMMON_HS_COMPLETE = ( 1 << 2 ),
	SERVER_PARSER_ALLOCATED = ( 1 << 3 ),
	NONBLOCKING = ( 1 << 4 ),
};

class Ssl_Connection
	// : public TCPSocketConnection
{
	public:

		//
		// sslConnection()
		//
		Ssl_Connection( Ssl_Context* ssl_context );

		//
		// sslConnection(int32_t)
		//
		Ssl_Connection( int32_t fd, Ssl_Context* ssl_context, bool set_soc_option = true );

		//
		// ~sslConnection()
		//
		virtual ~Ssl_Connection();

		int Ssl_Connect( const char* host, const int port );

		int Ssl_Accept();

		void Close_Connection();

		int Ssl_Send( char* data, int length );

		int Send_All( char* data, int length );

		int Receive( unsigned char** data );

		int Receive_All( unsigned char** data );

		int Receive( char* data, int length );

		int Receive_all( char* data, int length );

		int32_t Processed_Data( uint8_t** data, int32_t length );

		bool Load_Certificate( const unsigned char* certificate, size_t size );

		int Get_Ssl_Error();

		bool Ssl_Is_Timeout();

		bool Idle_State_Timeout_Status();

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Set_Parser_Allocated( bool value )
		{
			if ( value == true )
			{
				m_conn_state |= SERVER_PARSER_ALLOCATED;
			}
			else
			{
				m_conn_state &= ~SERVER_PARSER_ALLOCATED;
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		bool Get_Parser_Allocated()
		{
			return ( m_conn_state & SERVER_PARSER_ALLOCATED ? true : false );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void* Get_Parser()
		{
			return ( m_parser );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Set_Parser( void* parser )
		{
			m_parser = parser;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		int32_t Get_FD()
		{
			return ( m_fd );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Set_FD( int32_t fd )
		{
			m_fd = fd;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void* Get_CP()
		{
			return ( m_cp );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		bool Is_Handshake_Complete()
		{
			return ( m_conn_state & COMMON_HS_COMPLETE ? true : false );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		bool Is_Non_Blocking_Socket()
		{
			return ( m_conn_state & NONBLOCKING ? true : false );
		}

		int32_t Do_Handshake();

		void Update_Timeout();

		int32_t Get_Out_Data();

	public:
		DLListEntry List;

#ifdef LTK_TLS_DEBUG
		uint32_t while_loop_count;
		uint32_t connection_start_time;
		uint32_t connection_time;
		uint32_t handshake_time;
		uint32_t process_time;
		uint32_t unique_id;
		enum ssl_close_type close;
		static uint32_t first_conn_start_time;
		uint32_t bytesSent;
		uint32_t total_length;
		uint16_t wouldblk_delay_time_ms;
#endif
		uint32_t ip_addr;
		uint16_t remote_port;
#ifdef HTTPS_PERSISTENT_CONNECTION
		bool keep_alive;
#endif
		// HTTPS server will use following members to store current state
		// of SSL connection ( Bug LTK-3621 )
		bool send_pending;
		bool webui_resource_flag;// to differentiate between static data or Https Webui data
		uint8_t* send_pend_data;
		uint32_t send_pend_len;

	private:
#ifdef USE_REHANDSHAKING
		int32_t Do_Rehandshake();

#endif

		void Set_Socket_Options( void );

		void Ssl_Initiate_Handshake( void );

		static const uint32_t WOULD_BLOCK_TIMEOUT_MS = 220U;// ms
		static const uint8_t WOULDBLOCK_DELAY_TIME = 20U;
		static const uint32_t SSL_CONNECT_TIMEOUT_MS = ( WOULD_BLOCK_TIMEOUT_MS * 100 );
		static const uint32_t SSL_HANDSHAKE_TIMEOUT_MS = ( WOULD_BLOCK_TIMEOUT_MS * 10 );

		Ssl_Context* m_ssl_context;
		void* m_cp;
		void* m_parser;
		int8_t m_fd;
		uint8_t m_conn_state;

};

#endif	/* SSL_CONNECTION_H */
