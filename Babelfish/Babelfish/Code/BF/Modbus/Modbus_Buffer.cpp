/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Modbus_Buffer.h"

namespace BF_Mbus
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Buffer::Get_Remote_Modbus_Static_Address()
{
	return ( REMOTE_MODBUS_STATIC_ADDRESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
parity_t Modbus_Buffer::Get_Remote_Modbus_Parity_Mode()
{
	return ( REMOTE_MODBUS_PARITY_MODE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
tx_mode_t Modbus_Buffer::Get_Remote_Modbus_Tx_Mode()
{
	return ( REMOTE_MODBUS_TX_MODE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Modbus_Buffer::Get_Remote_Modbus_Baud_Rate()
{
	return ( REMOTE_MODBUS_BAUD_RATE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Buffer::Modbus_Buffer( USB* usb ) :
	Modbus_Serial(),
	usb_ctrl( reinterpret_cast<USB*>( usb ) ),
	m_port_enabled( false ),
	m_msg_timeout_tmr( 0U ),
	m_rx_done_time( 0U ),
	m_rx_cback( nullptr ),
	m_rx_cback_handle( nullptr ),
	m_rx_que( nullptr ),
	m_msg_tmout_crt( nullptr ),
	m_rx_state( 0U ),
	buffer_cnt( 0U )
{
	new CR_Tasker( &RX_Msg_Handler_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
				   CR_TASKER_PRIORITY_2 );

	usb_ctrl = usb;

	m_msg_tmout_crt = new CR_Tasker( &Msg_Timeout_Check_Static,
									 reinterpret_cast<CR_TASKER_PARAM>( this ), CR_TASKER_IDLE_PRIORITY );
	m_msg_tmout_crt->Suspend();

	m_rx_que = new CR_Queue();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Buffer::Enable_Port( MB_RX_PROC_CBACK rx_cback,
								 MB_RX_PROC_CBACK_HANDLE rx_cback_handle,
								 tx_mode_t modbus_tx_mode, parity_t parity_select, uint32_t baud_rate,
								 bool use_long_holdoff, stop_bits_t stop_bit_select )
{

	m_rx_cback = rx_cback;
	m_rx_cback_handle = rx_cback_handle;

	m_port_enabled = true;

	m_rx_state = MODBUS_SERIAL_RX_IDLE;

	if ( modbus_tx_mode == MODBUS_RTU_TX_MODE )
	{
		m_msg_tmout_crt->Resume();

		m_rx_done_time = MODBUS_RTU_RX_DONE_LONG_HOLDOFF_TIME;
	}
	else
	{
		m_msg_tmout_crt->Suspend_And_Reset();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Buffer::Msg_Timeout_Check( CR_Tasker* cr_task )
{
	CR_QUEUE_STATUS result;
	// uint32_t length;
	/// TODO:  We should not be allocating this big of a block on the stack.  It is not good.
	uint8_t tmp_buffer[USB_SERIAL_BUFFER_SIZE];

	CR_Tasker_Begin( cr_task );

	if ( !usb_ctrl->is_serial_buffer_empty() )
	{
		uint32_t length = usb_ctrl->get_serial_buffer_data( tmp_buffer );
		Add_Data_To_Buffer( tmp_buffer, length );

		m_msg_timeout_tmr = BF_Lib::Timers_uSec::Get_Time();
	}
	if ( buffer_cnt == 0U )
	{
		m_msg_timeout_tmr = BF_Lib::Timers_uSec::Get_Time();
	}
	else if ( BF_Lib::Timers_uSec::Expired( m_msg_timeout_tmr, m_rx_done_time ) )
	{
		m_rx_state = MODBUS_SERIAL_RX_MSG_READY;
		CR_Queue_Send( cr_task, m_rx_que, nullptr, OS_TASKER_INDEFINITE_TIMEOUT, result );
		CR_Tasker_Restart( cr_task );
	}
	else
	{	/* do nothing*/
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Buffer::RX_Msg_Handler( CR_Tasker* cr_task )
{
	MB_FRAME_STRUCT m_rx_struct;
	CR_QUEUE_STATUS result;

	CR_Tasker_Begin( cr_task );

	for (;;)
	{
		CR_Queue_Receive( cr_task, m_rx_que, nullptr, OS_TASKER_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			if ( m_rx_state == MODBUS_SERIAL_RX_MSG_READY )
			{
				m_rx_state = MODBUS_SERIAL_PROCESSING;
				m_rx_struct.data = buffer;
				m_rx_struct.data_length = static_cast<uint16_t>( buffer_cnt );
				( *m_rx_cback )( m_rx_cback_handle, &m_rx_struct );
				buffer_cnt = 0U;
				m_rx_state = MODBUS_SERIAL_RX_IDLE;
				// m_modbus_display->Bus_Idle();
			}
		}
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Modbus_Buffer::Add_Data_To_Buffer( uint8_t* data, uint32_t length )
{
	uint16_t i;
	uint16_t j;
	uint32_t end_pos = buffer_cnt + length;

	// NOTE TO NISHCHAL:  SERIAL_BUFFER_SIZE should be a const or a local member variable passed in during construction.
	// So SERIAL_BUFFER_SIZE at the easiest will become a uint16_t USB::SERIAL_BUFFER_SIZE = 255;
	if ( end_pos > static_cast<uint32_t>( USB_SERIAL_BUFFER_SIZE ) )
	{
		end_pos = static_cast<uint32_t>( USB_SERIAL_BUFFER_SIZE );
	}

	i = static_cast<uint16_t>( buffer_cnt );
	j = 0U;
	while ( i < end_pos )	// NOTE TO NISHCHAL:  You could also consider leveraging BF_Lib::Copy_String() from the
							// StdMV_Lib.c.
	{
		buffer[i] = data[j];
		i++;
		j++;
	}
	buffer_cnt += i;

	return ( i );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Buffer::Send_Frame( MB_FRAME_STRUCT* tx_struct )
{
	usb_ctrl->send_data( tx_struct->data, tx_struct->data_length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Buffer::Disable_Port( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Buffer::Port_Enabled( void )
{
	return ( m_port_enabled );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Buffer::Idle( void )
{
	bool temp_srl_buff_emp = usb_ctrl->is_serial_buffer_empty();
	bool temp_snd_buff_emp = usb_ctrl->is_send_buffer_empty();

	return ( ( m_rx_state == MODBUS_SERIAL_RX_IDLE ) && temp_srl_buff_emp && temp_snd_buff_emp );
}

}	/* End namespace BF_Mbus for this module*/

