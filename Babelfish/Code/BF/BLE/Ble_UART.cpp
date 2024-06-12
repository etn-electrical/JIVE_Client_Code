/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Ble_UART.h"
#include "Babelfish_Assert.h"
#include "BLE_Debug.h"

extern "C" {
extern uint8_t ble_state;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ble_UART::Ble_UART( uC_USART_Buff* usart_ctrl, Ble_Display* ble_display_ctrl,
					uint16_t buffer_size, uint8_t priority ) :
	Ble_Serial(),
	m_port_flags_u8( 0U ),
	m_rx_data_length_u16( 0U ),
	m_rx_buffer_size_u16( buffer_size ),
	m_rx_buff( reinterpret_cast<uint8_t*>( Ram::Allocate(
											   static_cast<size_t>( buffer_size ) ) ) ),
	m_tx_buff( reinterpret_cast<uint8_t*>( Ram::Allocate(
											   static_cast<size_t>( buffer_size ) ) ) ),
	m_tx_state_u8( 0U ),
	m_usart( usart_ctrl ),
	m_ble_display( ble_display_ctrl ),
	m_rx_que( nullptr ),
	m_front( QUEUE_FIRST_POSITION ),
	m_rear( QUEUE_FIRST_POSITION )
{
	m_tx_struct.data = nullptr;
	m_tx_struct.data_length = 0U;
	Ble_UART::Disable_Port();
	// coverity[leaked_storage]
	new CR_Tasker( &RX_Msg_Handler_Static, static_cast<CR_TASKER_PARAM>( this ), priority,
				   "Ble UART RX Msg Handler" );

	m_rx_que = new CR_Queue();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ble_UART::~Ble_UART( void )
{
	Ram::De_Allocate( m_rx_buff );
	delete m_tx_buff;
	delete m_usart;
	delete m_ble_display;
	delete m_rx_que;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_UART::RX_Msg_Handler( CR_Tasker* cr_task )
{
	CR_QUEUE_STATUS result;

	CR_Tasker_Begin( cr_task );

	for (;;)
	{
		CR_Queue_Receive( cr_task, m_rx_que, nullptr, CR_QUEUE_INDEFINITE_TIMEOUT, result );

		if ( result == CR_QUEUE_PASS )
		{
			if ( m_tx_state_u8 == TX_IDLE )
			{
				m_usart->Start_RX( m_rx_buff, m_rx_buffer_size_u16 );
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
void Ble_UART::Send_Frame( ble_frame_struct_t* tx_struct )
{
	m_tx_struct.data = tx_struct->data;
	m_tx_struct.data_length = tx_struct->data_length;
	m_tx_state_u8 = TX_DATA_READY;
	Send_Frame_Right_Now();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_UART::Send_Frame_Right_Now( void )
{
	m_tx_state_u8 = TX_BUSY;
	m_usart->Stop_RX();
	m_usart->Start_TX( m_tx_struct.data, m_tx_struct.data_length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_UART::Enable_Port( ble_parity_t parity_select, uint32_t baud_rate,
							ble_stop_bits_t stop_bit_select )
{
	uint32_t rx_done_time = RX_DONE_LONG_HOLDOFF_TIME;
	uint32_t tx_holdoff_time = TX_LONG_HOLDOFF_TIME;
	uC_USART_Buff::parity_t usart_parity = uC_USART_Buff::PARITY_NONE;
	uC_USART_Buff::char_size_t usart_char_size;
	uC_USART_Buff::stop_bit_t usart_stop_bit = uC_USART_Buff::STOP_BIT_1;
	uC_USART_Buff::cback_status_t usart_dma_req_bits;

	BF_Lib::Bit::Set( m_port_flags_u8, PORT_ENABLED );
	m_tx_state_u8 = TX_IDLE;

	switch ( parity_select )
	{
		case BLE_PARITY_EVEN:
			usart_parity = uC_USART_Buff::PARITY_EVEN;
			break;

		case BLE_PARITY_ODD:
			usart_parity = uC_USART_Buff::PARITY_ODD;
			break;

		case BLE_PARITY_NONE:
			usart_parity = uC_USART_Buff::PARITY_NONE;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	switch ( stop_bit_select )
	{
		case BLE_1_STOP_BIT:
			usart_stop_bit = uC_USART_Buff::STOP_BIT_1;
			break;

		case BLE_2_STOP_BIT:
			usart_stop_bit = uC_USART_Buff::STOP_BIT_2;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers,
	 * not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	usart_char_size = static_cast<uC_USART_Buff::char_size_t>( uC_USART_Buff::CHAR_SIZE_8BIT );
	usart_dma_req_bits = 0U;
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::RX_BYTE_TIMEOUT );
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::RX_BUFF_FULL );
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::TX_COMPLETE );
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::TXRX_HOLDOFF_COMPLETE );
	m_usart->Config_Callback( &UART_Callback_Static, this, usart_dma_req_bits,
							  rx_done_time, tx_holdoff_time );
	m_usart->Config_Port( baud_rate, usart_char_size, usart_parity, usart_stop_bit );
	m_rx_data_length_u16 = 0U;
	m_usart->Start_RX( m_rx_buff, m_rx_buffer_size_u16 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_UART::UART_Callback( uC_USART_Buff::cback_status_t status )
{
	uint_fast32_t loopvar;

	if ( Test_Bit( status, uC_USART_Buff::RX_BYTE_TIMEOUT ) )
	{
		m_usart->Stop_RX();
		m_rx_data_length_u16 = m_usart->Get_RX_Length();
		for ( loopvar = 0U; loopvar < m_rx_data_length_u16; loopvar++ )
		{
			enQueue( m_rx_buff[loopvar] );
		}
		m_usart->Start_RX( m_rx_buff, m_rx_buffer_size_u16 );
		// coverity[var_deref_model]
		m_rx_que->Send_From_ISR( nullptr, FALSE );
	}

	if ( Test_Bit( status, uC_USART_Buff::TX_COMPLETE ) )
	{
		m_tx_state_u8 = TX_IDLE;
		m_usart->Start_RX( m_rx_buff, m_rx_buffer_size_u16 );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_UART::Disable_Port( void )
{
	BF_Lib::Bit::Clr( m_port_flags_u8, PORT_ENABLED );

	m_usart->Stop_RX();
	m_usart->Stop_TX();
	m_tx_state_u8 = TX_IDLE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ble_UART::Port_Enabled( void )
{
	return ( BF_Lib::Bit::Test( m_port_flags_u8, PORT_ENABLED ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Ble_UART::Bg_Lib_Read( uint32_t data_length, uint8_t* data )
{
	uint_fast32_t loop_var = 0U;
	int32_t que_size;

	que_size = m_rear - m_front + 1U;
	int8_t result;

	if ( data_length > que_size )
	{
		result = NO_SUFFICIENT_DATA_IN_QUEUE;
	}
	else
	{
		for ( loop_var = 0U; loop_var < data_length; loop_var++ )
		{
			*data = deQueue();
			data++;
		}

		result = QUEUE_IS_NOT_EMPTY;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_UART::Bg_Lib_Write( uint32_t data_length, uint8_t* data )
{
	uint_fast32_t loopvar;

	for ( loopvar = 0U; loopvar < data_length; loopvar++ )
	{
		m_tx_buff[loopvar] = *data;
		data++;
	}

	m_tx_struct.data = m_tx_buff;
	m_tx_struct.data_length = data_length;
	m_tx_state_u8 = TX_DATA_READY;
	Send_Frame_Right_Now();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ble_UART::Is_Queue_Full( void )
{
	bool status = false;

	if ( ( m_front == 0U ) && ( m_rear == ( DEFAULT_BUFFER_SIZE - 1U ) ) )
	{
		status = true;
	}

	if ( m_front == ( m_rear + 1U ) )
	{
		status = true;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ble_UART::Is_Queue_Empty( void )
{
	bool status = false;

	if ( m_front == QUEUE_IS_EMPTY )
	{
		status = true;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_UART::enQueue( int element )
{
	if ( Is_Queue_Full() )
	{
		BLE_DEBUG_PRINT( DBG_MSG_INFO, "Ble Receive Buffer is Full \n" );
	}
	else
	{
		if ( m_front == QUEUE_IS_EMPTY )
		{
			m_front = 0U;
		}
		m_rear = ( m_rear + 1U ) % DEFAULT_BUFFER_SIZE;
		m_ble_recv_buf[m_rear] = element;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int8_t Ble_UART::deQueue( void )
{
	uint8_t element;
	int8_t result;

	if ( Is_Queue_Empty() )
	{
		result = QUEUE_IS_EMPTY;
	}
	else
	{
		element = m_ble_recv_buf[m_front];
		if ( m_front == m_rear )
		{
			m_front = QUEUE_FIRST_POSITION;
			m_rear = QUEUE_FIRST_POSITION;
		}	/* Q has only one element, so we reset the queue after deleting it. */
		else
		{
			m_front = ( m_front + 1U ) % DEFAULT_BUFFER_SIZE;
		}

		result = element;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_UART::display( void )
{
	/* Function to display status of Circular Queue */
	int32_t i;

	if ( Is_Queue_Empty() )
	{
		BLE_DEBUG_PRINT( DBG_MSG_INFO, "Empty Queue \n" );
	}
	else
	{
		BLE_DEBUG_PRINT( DBG_MSG_INFO, "Front -> %d\n m_ble_recv_buf ->", m_front );
		for ( i = m_front; i != m_rear; i = ( i + 1U ) % DEFAULT_BUFFER_SIZE )
		{
			BLE_DEBUG_PRINT( DBG_MSG_INFO, "%d %d Rear -> %d \n", m_ble_recv_buf[i], m_ble_recv_buf[i], m_rear );

		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_UART::Ble_Led_Display_Control( void )
{
	if ( m_ble_display != nullptr )
	{
		switch ( ble_state )
		{
			case BLE_IDLE_OR_DISCONNECTED:
				m_ble_display->Device_Disconnected();
				break;

			case BLE_PAIRING:
				m_ble_display->Pairing_Mode();
				break;

			case BLE_CONNECTED:
				m_ble_display->Device_Connected();
				break;

			case BLE_POLLING:
				m_ble_display->Data_Operations();
				break;

			case BLE_FAULT:
				m_ble_display->Device_Faulted();
				break;

			case BLE_ATTENTION:
				m_ble_display->Attention();
				break;
		}
	}
}
