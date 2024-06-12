/**
 *****************************************************************************************
 *	@file BACnetUARTInterface.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "includes.h"
#include "BACnetUARTInterface.h"
#include "Output_Null.h"
#include "Babelfish_Assert.h"
/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */

// We need to multiply the bits per character times the number of characters hold off.
// If we have 10 bits per character and 2 character times.
// This is merely the bit times for an EOF RX Idle interrupt.
// Time calc is boosted by a value which boosts things into a microsecond type of framework.
// Calculation is microsecond boost * bits_per_char * msg_done_characters
const uint32_t BACnet_UART::BACNET_RX_DONE_TIME_CALC = static_cast<uint32_t>( 1000000.0 * 10.0 * 2.0 );	/*BACnet's
																										   Tframe_gap */

// Calculation is microsecond boost * bits_per_char * msg_done_characters
const uint32_t BACnet_UART::BACNET_TX_HOLDOFF_TIME_CALC = static_cast<uint32_t>( 1000000.0 * 10.0 * 4.0 );	/*BACnet's
																											   Tturnaround
																											 */

extern volatile int SilenceTimer;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BACnet_UART::BACnet_UART( uC_USART_Buff* bac_usart_ctrl,
						  BF_Lib::Output* tx_enable_out, uint16_t bacnet_rx_buffer_size,
						  CR_TASKER_PRIOR_TD bacnet_rx_task_priority ) :
	m_b_tx_mode( BACNET_MSTP_TX_MODE ),
	m_b_parity_error( FALSE ),
	m_b_rx_data_ptr( nullptr ),
	m_b_rx_data_length( 0U ),
	m_b_tx_struct{ nullptr, 0U }
{
	m_b_rx_buff_size = bacnet_rx_buffer_size + 1U;
	m_b_rx_buff = ( uint8_t* )Ram::Allocate( ( size_t )m_b_rx_buff_size );

	// m_modbus_display = modbus_display_ctrl;
	// m_modbus_display->Bus_Idle();

	if ( tx_enable_out == NULL )
	{
		m_b_tx_enable_out = new BF_Lib::Output_Null();
	}
	else
	{
		m_b_tx_enable_out = tx_enable_out;
	}

	// Set up the USART
	m_b_usart = bac_usart_ctrl;

	Disable_Port();

	new CR_Tasker( BACnet_RX_Msg_Handler_Static, ( CR_TASKER_PARAM )this, bacnet_rx_task_priority,
				   "BACnet Communication" );

	m_b_rx_que = new CR_Queue();

	m_b_bad_frame_count = 0U;

	BF_Lib::Timers* timer_for_silencer = new BF_Lib::Timers( BACnet_Silencer_Static, this,
															 ( CR_TASKER_PRIOR_TD )CR_TASKER_PRIORITY_4,
															 "BACnet Timer For Silencer" );

	timer_for_silencer->Start( 1, TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BACnet_UART::BACnet_Silencer( void )
{
	if ( m_b_usart->RX_State() == uC_USART_Buff::RX_RECEIVING )
	{
		SilenceTimer = 0;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BACnet_UART::~BACnet_UART( void )
{
	Ram::De_Allocate( m_b_rx_buff );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t recvBuf[rxmax];
uint16_t recvBufindex;
uint16_t recvBufCount;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BACnet_UART::BACnet_RX_Msg_Handler( CR_Tasker* cr_task )
{
	BACNET_FRAME_STRUCT b_rx_struct;
	CR_QUEUE_STATUS result;
	uint16_t cnt = 0U;

	CR_Tasker_Begin( cr_task );

	while ( TRUE )
	{
		CR_Queue_Receive( cr_task, m_b_rx_que, NULL, CR_QUEUE_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			// m_modbus_display->Bus_Active();
			if ( m_b_rx_state == BACnet_RX_MSG_READY )
			{
				if ( m_b_parity_error == FALSE )
				{
					m_b_rx_state = BACnet_PROCESSING;
					b_rx_struct.data = m_b_rx_buff;
					b_rx_struct.data_length = m_b_rx_data_length;
					// (*b_rx_cback)( b_rx_cback_handle, &b_rx_struct ); //here we receive complete BACnet buffer do we
					// need a callback ?
					/*First store the packests*/
					for ( cnt = 0U; cnt < b_rx_struct.data_length; cnt++ )
					{
						recvBuf[recvBufindex] = b_rx_struct.data[cnt];
						if ( ++recvBufindex == rxmax )
						{
							recvBufindex = 0U;
						}
					}
					recvBufCount += b_rx_struct.data_length;
				}
				else if ( m_b_bad_frame_count < UINT16_MAX )
				{
					m_b_bad_frame_count++;
				}
				m_b_rx_state = BACnet_RX_IDLE;
			}
			if ( m_b_tx_state == BACnet_TX_IDLE )
			{
				m_b_usart->Start_RX( m_b_rx_buff, m_b_rx_buff_size );
			}
		}
	}

	CR_Tasker_End();
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
void BACnet_UART::BACnet_Send_Frame( uint8_t* data, uint16_t len )
{
	m_b_tx_struct.data = data;
	m_b_tx_struct.data_length = len;

	Push_TGINT();
	if ( m_b_usart->TX_Holdoff() != TRUE )
	{
		Pop_TGINT();
		Send_BACNET_Frame_Right_Now();
	}
	else
	{
		m_b_tx_state = BACnet_TX_DATA_READY;
		Pop_TGINT();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BACnet_UART::Send_BACNET_Frame_Right_Now( void )
{
	// Added by Prateek
	// To stop transmission when reception in progress causing failure for WPM service.

	/*	while ( m_b_usart->RX_State() == uC_USART_Buff::RX_RECEIVING )
	    {
	    }*/
	m_b_tx_state = BACnet_TX_BUSY;

	m_b_tx_enable_out->On();

	m_b_usart->Stop_RX();
	m_b_usart->Start_TX( m_b_tx_struct.data, m_b_tx_struct.data_length );

	// m_modbus_display->Bus_Active();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BACnet_UART::Enable_Port( BACNET_TX_MODES_EN bacnet_tx_mode, uint8_t parity_select,
							   uint32_t baud_rate, uint8_t stop_bit_select )
{
	uint32_t rx_done_time = 0U;
	uint32_t tx_holdoff_time = 0U;
	uC_USART_Buff::parity_t usart_parity = uC_USART_Buff::PARITY_NONE;
	uC_USART_Buff::char_size_t usart_char_size;
	uC_USART_Buff::stop_bit_t usart_stop_bit = uC_USART_Buff::STOP_BIT_1;
	uC_USART_Buff::cback_status_t usart_dma_req_bits;

	// b_rx_cback = rx_cback;
	// b_rx_cback_handle = rx_cback_handle;

	m_b_port_enabled = TRUE;
	m_b_rx_data_ptr = m_b_rx_buff;
	m_b_rx_state = BACnet_RX_IDLE;
	m_b_tx_mode = bacnet_tx_mode;

	m_b_tx_state = BACnet_TX_IDLE;

	// m_modbus_display->Bus_Idle();

	switch ( parity_select )
	{
		case BACNET_PARITY_EVEN:
			usart_parity = uC_USART_Buff::PARITY_EVEN;
			break;

		case BACNET_PARITY_ODD:
			usart_parity = uC_USART_Buff::PARITY_ODD;
			break;

		case BACNET_PARITY_NONE:
			usart_parity = uC_USART_Buff::PARITY_NONE;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	switch ( stop_bit_select )
	{
		case BACNET_1_STOP_BIT:
			usart_stop_bit = uC_USART_Buff::STOP_BIT_1;
			break;

		case BACNET_2_STOP_BIT:
			usart_stop_bit = uC_USART_Buff::STOP_BIT_2;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	if ( bacnet_tx_mode == BACNET_MSTP_TX_MODE )
	{
		/* SRP: As BACnet doesn't recommend any fixed inter-character time-out  or interframe delay hence removed
		   if ( ( use_long_holdoff == TRUE ) &&
		   ( baud_rate > MSTP_LONG_HOLDOFF_BAUD_RATE_MIN ) )
		   {
		   rx_done_time = BACNET_RX_DONE_LONG_HOLDOFF_TIME;
		   tx_holdoff_time = BACNET_TX_LONG_HOLDOFF_TIME;
		   }
		   else */
		// {
		rx_done_time = ( uint32_t )BACNET_RX_DONE_TIME_CALC / ( uint32_t )baud_rate;
		tx_holdoff_time = ( uint32_t )BACNET_TX_HOLDOFF_TIME_CALC / ( uint32_t )baud_rate;
		// }
		usart_char_size = uC_USART_Buff::CHAR_SIZE_8BIT;
	}
	else
	{
		usart_char_size = uC_USART_Buff::CHAR_SIZE_7BIT;
	}

	m_b_usart->Config_Port( baud_rate, usart_char_size, usart_parity, usart_stop_bit );

	usart_dma_req_bits = 0U;
	Set_Bit( usart_dma_req_bits, uC_USART_Buff::RX_BYTE_TIMEOUT );
	// Set_Bit( usart_dma_req_bits, uC_USART_Buff::RX_BUFF_FULL );
	Set_Bit( usart_dma_req_bits, uC_USART_Buff::TX_COMPLETE );
	Set_Bit( usart_dma_req_bits, uC_USART_Buff::TXRX_HOLDOFF_COMPLETE );
	m_b_usart->Config_Callback( BACNET_UART_Callback_Static, this, usart_dma_req_bits,
								rx_done_time, tx_holdoff_time );

	m_b_usart->Start_RX( m_b_rx_buff, m_b_rx_buff_size );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BACnet_UART::BACnet_UART_Callback( uC_USART_Buff::cback_status_t status )
{
	if ( Test_Bit( status, uC_USART_Buff::RX_BYTE_TIMEOUT ) )
	{
		m_b_usart->Stop_RX();
		m_b_rx_state = BACnet_RX_MSG_READY;
		m_b_rx_que->Send_From_ISR( NULL, FALSE );
		m_b_rx_data_length = m_b_usart->Get_RX_Length();
	}

	if ( Test_Bit( status, uC_USART_Buff::TX_COMPLETE ) )
	{
		m_b_tx_state = BACnet_TX_IDLE;
		m_b_tx_enable_out->Off();
		m_b_usart->Start_RX( m_b_rx_buff, m_b_rx_buff_size );
	}

	if ( Test_Bit( status, uC_USART_Buff::TXRX_HOLDOFF_COMPLETE ) )
	{
		if ( m_b_tx_state == BACnet_TX_DATA_READY )
		{
			Send_BACNET_Frame_Right_Now();	// SRP: check this
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BACnet_UART::Disable_Port( void )
{
	m_b_port_enabled = FALSE;

	// m_modbus_display->Bus_Idle();

	m_b_usart->Stop_RX();
	m_b_usart->Stop_TX();

	m_b_rx_state = BACnet_RX_IDLE;
	m_b_tx_state = BACnet_TX_IDLE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t BACnet_UART::Port_Enabled( void )
{
	return ( m_b_port_enabled == TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t BACnet_UART::Idle( void )
{
	return ( ( m_b_rx_state == BACnet_RX_IDLE ) &&
			 ( m_b_tx_state == BACnet_TX_IDLE ) );
}
