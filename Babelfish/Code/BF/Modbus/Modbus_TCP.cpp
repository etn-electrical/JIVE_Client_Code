/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include <string.h>
#include "Stdlib_MV.h"
#include "Modbus_TCP.h"
#include "EthConfig.h"
#include "lwip/inet.h"
#include "Modbus_TCP_Debug.h"
#include "Babelfish_Assert.h"
#ifdef TRUSTED_IP_FILTER
#include "Trusted_IP_Filter.h"
#endif

#define MEMP_MB_NUM_BACKLOG               5
/* we will be using LWIP mem pool to allocate memory for mb_app_struct from lwip 2.1.0 which was added into tcp_pcb upto
   lwip 1.3.2 . All the api have been changed accodingly to work with lwip 2.1.0*/
LWIP_MEMPOOL_DECLARE( MB_APP_STRUCT,  MEMP_NUM_MB_APP_STRUCT,   sizeof( struct Modbus_TCP::mb_app_struct ),
					  "MB_APP_STRUCT" )
#define MB_ALLOC_MB_STRUCT() ( struct mb_app_struct* )LWIP_MEMPOOL_ALLOC( MB_APP_STRUCT )
#define MB_FREE_MB_STRUCT( x ) LWIP_MEMPOOL_FREE( MB_APP_STRUCT, ( x ) )
/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
Modbus_TCP::Modbus_TCP( BF_Mbus::Modbus_Display* modbus_display_ctrl,
						uint16_t modbus_port, DCI_Owner* session_timeout_owner ) :
	Modbus_Serial(),
	m_port_enabled( false ),
	m_modbus_port( modbus_port ),
	// m_tx_struct() ( nullptr, 0U ),
	m_tx_state( 0U ),
	m_modbus_display( modbus_display_ctrl ),
	m_rx_cback( nullptr ),
	m_rx_cback_handle( nullptr ),
	m_event_cback( nullptr ),
	m_event_handle( nullptr ),
	m_event_que( nullptr ),
	m_msg_ready_to_tx( false ),
	m_pcb_listen1( nullptr ),
	m_pcb_listen2( nullptr ),
	m_curr_pcb_client( nullptr ),
	m_tx_string{ 0U },
	m_session_timeout_owner( session_timeout_owner ),
	m_tx_struct{ nullptr, 0U }
{
	BF_Lib::Timers* session_check_timer;
	uint8_t counter_var;

	for ( counter_var = 0U; counter_var < MEMP_NUM_TCP_PCB; counter_var++ )
	{
		m_session_timeout_list[counter_var].pcb = nullptr;
		m_session_timeout_list[counter_var].time_sample = 0U;
		m_session_timeout_list[counter_var].pcb_mb_app = nullptr;
	}

	if ( nullptr != m_modbus_display )
	{
		m_modbus_display->Bus_Idle();
	}

	TCP_Init();

	MODBUS_TCP_DEBUG_PRINT( DBG_MSG_INFO, "Total Available Modbus TCP Connections : %d ", MEMP_NUM_TCP_PCB );

	// New timer created to handle the timeout checking.
	session_check_timer = new BF_Lib::Timers( Session_Timeout_Checker_Static,
											  reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
											  CR_TASKER_PRIORITY_0 );
	// coverity[leaked_storage]
	session_check_timer->Start( CONNECTION_TIMEOUT_CHECK_TIME, true );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void Modbus_TCP::Enable_Port( MB_RX_PROC_CBACK rx_cback, MB_RX_PROC_CBACK_HANDLE rx_cback_handle,
							  tx_mode_t modbus_tx_mode, parity_t parity_select, uint32_t baud_rate,
							  bool use_long_holdoff, stop_bits_t stop_bits )
{
	BF_Lib::Unused<tx_mode_t>::Okay( modbus_tx_mode );
	BF_Lib::Unused<parity_t>::Okay( parity_select );
	BF_Lib::Unused<uint32_t>::Okay( baud_rate );
	BF_Lib::Unused<bool>::Okay( use_long_holdoff );
	BF_Lib::Unused<stop_bits_t>::Okay( stop_bits );

	m_rx_cback = rx_cback;
	m_rx_cback_handle = rx_cback_handle;

	m_port_enabled = true;

	if ( nullptr != m_modbus_display )
	{
		m_modbus_display->Bus_Idle();
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void Modbus_TCP::Disable_Port( void )
{
	m_port_enabled = false;

	if ( nullptr != m_modbus_display )
	{
		m_modbus_display->Bus_Idle();
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool Modbus_TCP::Port_Enabled( void )
{
	return ( m_port_enabled == true );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool Modbus_TCP::Idle( void )
{
	return ( true );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void Modbus_TCP::Send_Frame( MB_FRAME_STRUCT* tx_struct )
{
	m_tx_struct.data = tx_struct->data;
	m_tx_struct.data_length = tx_struct->data_length;

	m_msg_ready_to_tx = true;
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

bool Modbus_TCP::PCB_Init( struct tcp_pcb** pcb, uint16_t port )
{
	struct tcp_pcb* pcb_listen;
	struct tcp_pcb* pcb_listen_copy;
	bool success = false;

	pcb_listen = tcp_new();

	pcb_listen_copy = pcb_listen;

	if ( pcb_listen != nullptr )
	{
		/**
		 * @remark Coding Standard Deviation:
		 * @n MISRA-C++[2008] Rule: 5-2-4
		 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and
		 * notation casts shall not be used
		 * @n Justification: The violation exists in IP_ADDR_ANY which is defined in ip_addr.h which
		 * is third party code.
		 */
		/*lint -e{1924} */
		if ( ERR_OK != tcp_bind( pcb_listen, IP_ADDR_ANY, port ) )
		{
			tcp_close( pcb_listen_copy );
		}
		else
		{
			/**
			 * @remark Coding Standard Deviation:
			 * @n MISRA-C++[2008] Rule: 5-0-4
			 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral
			 * conversions shall not change the signedness of the underlying type
			 * @n Justification: The violation exists in tcp_listen which is defined in tcp.h which
			 * is third party code.
			 */
			/*lint -e{1960}  */
			pcb_listen =
				reinterpret_cast<struct tcp_pcb*>( tcp_listen_with_backlog( pcb_listen, MEMP_MB_NUM_BACKLOG ) );

			if ( nullptr == pcb_listen )
			{
				tcp_close( pcb_listen_copy );
			}
			else
			{
				LWIP_MEMPOOL_INIT( MB_APP_STRUCT );

				// tcp_setprio( pcb_listen, MODBUS_TCP_PCB_PRIO );// commenting for ltk-3362
				tcp_arg( pcb_listen, static_cast<void*>( this ) );
#ifdef TRUSTED_IP_FILTER
				bool registered = Trusted_IP_Filter::Register_Accept_Callback(
					MODBUS_TCP_DEFAULT_PORT, pcb_listen, TCP_Accept_Static );
				if ( registered == false )
				{
					tcp_accept( pcb_listen, &TCP_Accept_Static );
				}
#else
				tcp_accept( pcb_listen, &TCP_Accept_Static );
#endif
				*pcb = pcb_listen;
				success = true;
			}
		}
	}
	return ( success );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool Modbus_TCP::TCP_Init( void )
{
	bool success;

	success = PCB_Init( &m_pcb_listen1, MODBUS_TCP_DEFAULT_PORT );

	if ( ( true == success ) &&
		 ( MODBUS_TCP_DEFAULT_PORT != m_modbus_port ) )
	{
		MODBUS_TCP_DEBUG_PRINT( DBG_MSG_DEBUG, "Default Modbus TCP port is not used. Requested Port is %d",
								m_modbus_port );
		// if the first port was opened successfully and need to open second port
		success = PCB_Init( &m_pcb_listen2, m_modbus_port );
	}

	return ( success );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
err_t Modbus_TCP::TCP_Accept( void* arg, struct tcp_pcb* pcb, err_t error )
{
	err_t return_error;
	uint8_t timeout_index = 0U;
	struct mb_app_struct* pcb_mb_app = nullptr;

	if ( ERR_OK != error )
	{
		return_error = error;
	}
	else
	{
		// allocate buffer, if failed, release client
		pcb_mb_app = MB_ALLOC_MB_STRUCT();
		if ( pcb_mb_app != nullptr )
		{
			pcb_mb_app->handle = this;
			// Search for free index in array
			while ( ( m_session_timeout_list[timeout_index].pcb != nullptr ) &&
					( timeout_index < MEMP_NUM_TCP_PCB ) )
			{
				timeout_index++;
			}

			// If we got here then the TCP stack seemed to give up more connections
			// than it is supposed to.  We use the same number of
			// connections from the TCP stack so it should have stopped there.
			BF_ASSERT( timeout_index < MEMP_NUM_TCP_PCB );
#ifdef ESP32_SETUP
			uint32_t temp_remote_ip = pcb->remote_ip.u_addr.ip4.addr;
#else
			uint32_t temp_remote_ip = pcb->remote_ip.addr;
#endif
			char_t src_ip_addr[16] = "000.000.000.000";
			strncpy( src_ip_addr, reinterpret_cast<char_t*>( inet_ntoa( temp_remote_ip ) ), sizeof( src_ip_addr ) );
			src_ip_addr[sizeof( src_ip_addr ) - 1] = '\0';
			MODBUS_TCP_DEBUG_PRINT( DBG_MSG_INFO,
									"New MB TCP connection opened. PCB - %X Remote IP - %s Total Active connections - %d", pcb, src_ip_addr,
									timeout_index + 1 );


			pcb_mb_app->timeout_num = timeout_index;
			m_session_timeout_list[timeout_index].pcb = pcb;
			m_session_timeout_list[timeout_index].pcb_mb_app = pcb_mb_app;

			// Initialize the timeout value for the first time.
			Reset_Session_Timeout( pcb_mb_app, pcb );

			// Set up the receive function TCP_Receive() to be called when data
			// arrives.
			tcp_recv( pcb, &TCP_Receive_Static );

			// Register error handler.
			tcp_err( pcb, &TCP_Error_Static );

			// Set callback argument later used in the error handler.
			tcp_arg( pcb, static_cast<void*>( pcb_mb_app ) );

			// Reset the buffers and state variables.
			// m_buffer_index = 0;
			pcb_mb_app->rx_buff_index = 0U;

			if ( m_event_cback != nullptr )
			{
				( *m_event_cback )( m_event_handle, CONNECTION_OPENED, nullptr );
			}

			return_error = ERR_OK;
		}
		else
		{
			// Returning out of memory error
			MODBUS_TCP_DEBUG_PRINT( DBG_MSG_ERROR, "MB TCP Buffer allocation failure" );
			return_error = ERR_MEM;
		}
	}
	return ( return_error );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
err_t Modbus_TCP::TCP_Receive( void* arg, struct tcp_pcb* pcb, struct pbuf* p_buffer,
							   err_t error )
{
	uint16_t rx_length;
	err_t return_error = ERR_OK;
	MB_FRAME_STRUCT rx_struct;
	struct mb_app_struct* pcb_mb_app =
		static_cast<struct mb_app_struct*>( arg );

	if ( error != ERR_OK )
	{
		return_error = error;
	}
	else if ( p_buffer == nullptr )
	{
		MODBUS_TCP_DEBUG_PRINT( DBG_MSG_INFO, "MB TCP closed: Remote end has closed connection" );
		Release_Client( pcb_mb_app, pcb );
		return_error = ERR_OK;
	}
	else
	{

		// Acknowledge that we have received the data bytes.
		tcp_recved( pcb, p_buffer->tot_len );

		// if protocol not enabled, drop the message and do nothing
		if ( true != m_port_enabled )
		{
			pbuf_free( p_buffer );
			return_error = ERR_OK;
			MODBUS_TCP_DEBUG_PRINT( DBG_MSG_DEBUG, "MB TCP disabled, data discarded" );
		}
		else if ( ( pcb_mb_app->rx_buff_index + p_buffer->tot_len ) >
				  ( MAX_MODBUS_SERIAL_PDU + MODBUS_TCP_MBAP_HEADER_LENGTH ) )
		{
			// if there's a potential buffer overflow risk, drop the packet
			// and what's already in the buffer
			pbuf_free( p_buffer );
			pcb_mb_app->rx_buff_index = 0U;
			return_error = ERR_OK;
			MODBUS_TCP_DEBUG_PRINT( DBG_MSG_DEBUG, "MB TCP potential buffer overflow, data discarded" );
		}
		else
		{
			// now we can copy the pbuf to the pcb's buffer
			struct pbuf* p_curr = p_buffer;
			struct pbuf* p_next = p_buffer;

			if ( m_modbus_display != nullptr )
			{
				m_modbus_display->Bus_Active();
			}

			do
			{
				p_curr = p_next;
				// copy from p_curr to pcb's buffer
				BF_Lib::Copy_String( &pcb_mb_app->rx_buff[pcb_mb_app->rx_buff_index],
									 static_cast<uint8_t*>( p_curr->payload ), p_curr->len );
				pcb_mb_app->rx_buff_index += p_curr->len;
				p_next = p_curr->next;
			} while ( ( p_curr->tot_len != p_curr->len ) && ( nullptr != p_next ) );

			// now we can remove the p_buffer
			pbuf_free( p_buffer );

			// If we have received the MBAP header we can analyze it and calculate
			// the number of bytes left to complete the current request. If complete
			// notify the protocol stack.

			if ( pcb_mb_app->rx_buff_index >=
				 MODBUS_TCP_MBAP_HEADER_LENGTH )
			{
				// Length is a byte count of Modbus PDU (function code + data) and the
				// unit identifier.
				rx_length =
					static_cast<uint16_t>( pcb_mb_app->rx_buff[MODBUS_TCP_MBAP_LENGTH_HIGH] )
						<< 8;
				rx_length |= pcb_mb_app->rx_buff[MODBUS_TCP_MBAP_LENGTH_LOW];

				// check sanity of rx_length
				if ( rx_length > ( ( MAX_MODBUS_SERIAL_PDU
									 + MODBUS_TCP_MBAP_HEADER_LENGTH )
								   - MODBUS_TCP_MBAP_UNIT_ID ) )
				{
					// if rx_length does not make sense (too long)
					// drop the packet and what's in the buffer
					pcb_mb_app->rx_buff_index = 0U;
					return_error = ERR_OK;
				}
				else if ( pcb_mb_app->rx_buff_index < ( rx_length +
														MODBUS_TCP_MBAP_UNIT_ID ) )
				{
					// still waiting for the PDU to complete
					return_error = ERR_OK;
				}
				else if ( pcb_mb_app->rx_buff_index < ( rx_length +
														MODBUS_TCP_MBAP_UNIT_ID ) )
				{
					// something is wrong - we received more than we need
					// drop what's in the buffer
					pcb_mb_app->rx_buff_index = 0U;
					return_error = ERR_OK;
				}
				else
				{
					// now we received a complete PDU
					// further processing
					rx_struct.data = &pcb_mb_app->rx_buff[MODBUS_TCP_MBAP_UNIT_ID];
					rx_struct.data_length = pcb_mb_app->rx_buff_index
						- MODBUS_TCP_MBAP_UNIT_ID;
					( *m_rx_cback )( m_rx_cback_handle, &rx_struct );

					// TODO: make sure that the rx_buff_index is reset after packet is sent
					if ( m_msg_ready_to_tx == true )
					{
						m_msg_ready_to_tx = false;

						m_tx_string[MODBUS_TCP_MBAP_TRANSACTION_ID_LOW] =
							pcb_mb_app->rx_buff[MODBUS_TCP_MBAP_TRANSACTION_ID_LOW];
						m_tx_string[MODBUS_TCP_MBAP_TRANSACTION_ID_HIGH] =
							pcb_mb_app->rx_buff[MODBUS_TCP_MBAP_TRANSACTION_ID_HIGH];
						m_tx_string[MODBUS_TCP_MBAP_PROTOCOL_ID_LOW] =
							pcb_mb_app->rx_buff[MODBUS_TCP_MBAP_PROTOCOL_ID_LOW];
						m_tx_string[MODBUS_TCP_MBAP_PROTOCOL_ID_HIGH] =
							pcb_mb_app->rx_buff[MODBUS_TCP_MBAP_PROTOCOL_ID_HIGH];
						m_tx_string[MODBUS_TCP_MBAP_LENGTH_LOW] =
							static_cast<uint8_t>( m_tx_struct.data_length );
						m_tx_string[MODBUS_TCP_MBAP_LENGTH_HIGH] = 0U;
						m_tx_string[MODBUS_TCP_MBAP_UNIT_ID] =
							pcb_mb_app->rx_buff[MODBUS_TCP_MBAP_UNIT_ID];
						BF_Lib::Copy_String( &m_tx_string[MODBUS_TCP_MBAP_HEADER_LENGTH],
											 &m_tx_struct.data[1],
											 ( static_cast<uint32_t>( m_tx_struct.data_length ) -
											   1U ) );
						pcb_mb_app->rx_buff_index = 0U;

						m_curr_pcb_client = pcb;

						MBTCP_Send( pcb_mb_app, m_tx_string, ( ( m_tx_struct.data_length - 1U )
															   + MODBUS_TCP_MBAP_HEADER_LENGTH ) );

						/* Reset Inactivity timeout */
						Reset_Session_Timeout( pcb_mb_app, pcb );
					}

					return_error = ERR_OK;
				}
			}

		}

	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool Modbus_TCP::MBTCP_Send( struct mb_app_struct* pcb_mb_app, uint8_t* mbus_frame, uint16_t frame_length )
{
	bool frame_sent = false;

	if ( tcp_write( m_curr_pcb_client, mbus_frame, frame_length,
					static_cast<u8_t>( NETCONN_COPY ) ) == ERR_OK )
	{
		tcp_output( m_curr_pcb_client );
		frame_sent = true;
	}
	else
	{
		MODBUS_TCP_DEBUG_PRINT( DBG_MSG_INFO, "MB TCP closed: Network Error" );
		Release_Client( pcb_mb_app, m_curr_pcb_client );
	}

	return ( frame_sent );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
/* Called in case of an unrecoverable error. In any case we drop the client
 * connection. */
void Modbus_TCP::TCP_Error( void* arg, err_t xErr )
{
	BF_Lib::Unused<err_t>::Okay( xErr );

	// A quick test.  If we get here then something has gone terribly wrong.
	// Sadly the callback does not send which pcb had the error.  So
	// we can spin through our list and just close everything but it all depends
	// on what the issue is.  For now I am going to stop here to catch it if we are
	// debugging and then otherwise just release all the client connections.  It says
	// unrecoverable.  I think this is a suitable brute force solution.
#ifdef DEBUG
	// BF_ASSERT( false );
#endif

	for ( uint_fast8_t timeout_index = 0U; timeout_index < MEMP_NUM_TCP_PCB;
		  timeout_index++ )
	{
		if ( m_session_timeout_list[timeout_index].pcb != nullptr )
		{
			MODBUS_TCP_DEBUG_PRINT( DBG_MSG_INFO, "MB TCP closed: Network Error" );
			// If it exists... Kill it.  I am out of near term ideas here.
			Release_Client( m_session_timeout_list[timeout_index].pcb_mb_app,
							m_session_timeout_list[timeout_index].pcb );
		}
	}

}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void Modbus_TCP::Release_Client( struct mb_app_struct* pcb_mb_app, struct tcp_pcb* pcb )
{
	uint8_t mb_timeout_tpcb_index = 0U;

	if ( pcb != nullptr )
	{
		if ( pcb->state != CLOSED )
		{
			tcp_arg( pcb, nullptr );
			tcp_err( pcb, nullptr );
			tcp_recv( pcb, nullptr );
			if ( tcp_close( pcb ) != ERR_OK )
			{
				tcp_abort( pcb );
			}
		}

		if ( m_event_cback != nullptr )
		{
			( *m_event_cback )( m_event_handle, CONNECTION_CLOSED, nullptr );
		}

		// Search for PCB to be closed in timeout array
		while ( ( m_session_timeout_list[mb_timeout_tpcb_index].pcb != pcb ) &&
				( mb_timeout_tpcb_index < MEMP_NUM_TCP_PCB ) )
		{
			mb_timeout_tpcb_index++;
		}
		if ( mb_timeout_tpcb_index < MEMP_NUM_TCP_PCB )
		{
			// Reset that particular array index
			m_session_timeout_list[mb_timeout_tpcb_index].pcb = nullptr;
			MB_FREE_MB_STRUCT( m_session_timeout_list[mb_timeout_tpcb_index].pcb_mb_app );

			mb_timeout_tpcb_index = 0U;
			// Calculating Total Active connection
			for ( uint8_t i = 0U; i < MEMP_NUM_TCP_PCB; i++ )
			{
				if ( m_session_timeout_list[i].pcb != nullptr )
				{
					mb_timeout_tpcb_index++;
				}
			}
			MODBUS_TCP_DEBUG_PRINT( DBG_MSG_INFO,
									"Closed connection PCB - %X Connection Time - %d Seconds. Total Active Connections - %d",
									pcb, ( pcb->tmr ) / 2, mb_timeout_tpcb_index );
		}
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void Modbus_TCP::Session_Timeout_Checker( void )
{
	BF_Lib::Timers::TIMERS_TIME_TD timeout_value = 0;

	if ( m_session_timeout_owner != nullptr )
	{
		m_session_timeout_owner->Check_Out(
			reinterpret_cast<DCI_DATA_PASS_TD*>( &timeout_value ) );
	}
	else
	{
		timeout_value = SESSION_TIMEOUT_DEFAULT;
	}

	// If the timeout is 0 then it is effectively disabled.
	if ( timeout_value > 0U )
	{
		for ( uint_fast8_t timeout_index = 0U; timeout_index < MEMP_NUM_TCP_PCB;
			  timeout_index++ )
		{
			if ( ( m_session_timeout_list[timeout_index].pcb != nullptr ) &&
				 ( BF_Lib::Timers::Expired(
					   m_session_timeout_list[timeout_index].time_sample,
					   timeout_value ) ) )
			{
				MODBUS_TCP_DEBUG_PRINT( DBG_MSG_INFO, "MB TCP closed: Connection Timeout " );
				// Session has timed out from inactivity.  Call the release.
				Release_Client( m_session_timeout_list[timeout_index].pcb_mb_app,
								m_session_timeout_list[timeout_index].pcb );
			}
		}
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void Modbus_TCP::Reset_Session_Timeout( struct mb_app_struct* pcb_mb_app, struct tcp_pcb* pcb )
{
	// If you got here something went wrong with the PCB assignments and you are
	// resetting the time before it even exists as a PCB.
	// Or something horrible happened with the PCB pointer which is even worse.
	BF_ASSERT( pcb != nullptr );
	// Capture a new time for the timeout.  We will use this later to compare
	// against.
	m_session_timeout_list[pcb_mb_app->timeout_num].time_sample =
		BF_Lib::Timers::Get_Time();
}
