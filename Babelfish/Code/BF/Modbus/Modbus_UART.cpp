/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_UART.h"
#include "Output_Null.h"
#include "Babelfish_Assert.h"
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_UART::Modbus_UART( uC_USART_Buff* usart_ctrl, BF_Mbus::Modbus_Display* modbus_display_ctrl,
						  BF_Lib::Output* tx_enable_out, uint16_t rx_buffer_size, uint8_t priority,
						  bool ascii_support ) :
	Modbus_Serial(),
	m_port_flags( 0U ),
	m_rx_data_ptr( nullptr ),
	m_rx_data_length( 0U ),
	m_rx_state( 0U ),
	m_tx_state( 0U ),
	m_tx_enable_out( nullptr ),
	m_usart( nullptr ),
	m_modbus_display( nullptr ),
	m_rx_cback( nullptr ),
	m_rx_cback_handle( nullptr ),
	m_rx_que( nullptr ),
	m_tx_struct{ nullptr, 0U },
	m_rx_struct{ nullptr, 0U }
{
	/**
	 * The argument "rx_buffer_size" should be equal to DEFAULT_BUFFER_SIZE.
	 * Otherwise, define MODBUS_UART_BUFFER_SIZE to rx_buffer_size value.
	 * Example: For buffer size 513
	 * Add "-D MODBUS_UART_BUFFER_SIZE=513" in common_define_config.xcl
	 */
	BF_ASSERT( rx_buffer_size == DEFAULT_BUFFER_SIZE );

#ifdef MODBUS_ASCII_SUPPORT
	BF_Lib::Bit::Set( m_port_flags, ASCII_SUPPORT );
	/**
	 * The argument "ascii_support" should be true when MODBUS_ASCII_SUPPORT
	 * macro is defined. Otherwise, remove defined MODBUS_ASCII_SUPPORT macro from
	 * common_define_config.xcl
	 */
	BF_ASSERT( ascii_support == true );

#else
	/**
	 * The argument "ascii_support" should be false when MODBUS_ASCII_SUPPORT
	 * macro is not defined. Otherwise, define MODBUS_ASCII_SUPPORT in
	 * common_define_config.xcl
	 * Example: -D MODBUS_ASCII_SUPPORT
	 */
	BF_ASSERT( ascii_support == false );
#endif

	m_modbus_display = modbus_display_ctrl;

	if ( m_modbus_display != nullptr )
	{
		m_modbus_display->Bus_Idle();
	}

	if ( tx_enable_out == nullptr )
	{
		m_tx_enable_out = new BF_Lib::Output_Null();
	}
	else
	{
		m_tx_enable_out = tx_enable_out;
	}

	// Set up the USART
	m_usart = usart_ctrl;

	Modbus_UART::Disable_Port();

	new CR_Tasker( &RX_Msg_Handler_Static, static_cast<CR_TASKER_PARAM>( this ), priority,
				   "Modbus UART RX Msg Handler" );

	m_rx_que = new CR_Queue();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_UART::~Modbus_UART( void )
{
	delete m_rx_data_ptr;
	delete m_tx_enable_out;
	delete m_usart;
	delete m_modbus_display;
	m_rx_cback_handle = nullptr;		// m_rx_cback_handle is a void * so cannot be deleted using
										// delete also no memory is assigned to it so making to point to
										// nullptr to avoid dangling pointer
	// delete m_rx_cback_handle;
	delete m_rx_que;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_UART::RX_Msg_Handler( CR_Tasker* cr_task )
{
	CR_QUEUE_STATUS result = CR_QUEUE_FAIL;

	CR_Tasker_Begin( cr_task );

	for (;;)
	{
		CR_Queue_Receive( cr_task, m_rx_que, nullptr, CR_QUEUE_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			if ( m_modbus_display != nullptr )
			{
				m_modbus_display->Bus_Active();
			}

			if ( m_rx_state == RX_MSG_READY )
			{
				m_rx_state = PROCESSING;
				m_rx_struct.data = m_rx_buff;
				m_rx_struct.data_length = m_rx_data_length;
				( *m_rx_cback )( m_rx_cback_handle, &m_rx_struct );
				m_rx_state = RX_IDLE;
			}

			if ( m_tx_state == TX_IDLE )
			{
				m_rx_data_length = 0U;
				m_usart->Start_RX( m_rx_buff, RX_BUFFER_SIZE_INTERNAL );
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
void Modbus_UART::Send_Frame( MB_FRAME_STRUCT* tx_struct )
{
	m_tx_struct.data = tx_struct->data;
	m_tx_struct.data_length = tx_struct->data_length;
	bool tx_holdoff;
	bool ascii_mode;

	Push_TGINT();

	if ( m_usart->TX_Holdoff() != true )
	{
		tx_holdoff = true;
	}
	else
	{
		tx_holdoff = false;
	}

	ascii_mode = BF_Lib::Bit::Test( m_port_flags, TX_MODE_ASCII );

	if ( ( ascii_mode == true ) ||
		 ( tx_holdoff == true ) )
	{
		Pop_TGINT();
		Send_Frame_Right_Now();
	}
	else
	{
		m_tx_state = TX_DATA_READY;
		Pop_TGINT();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_UART::Send_Frame_Right_Now( void )
{
	m_tx_state = TX_BUSY;

	m_tx_enable_out->On();

	m_usart->Stop_RX();
	m_usart->Start_TX( m_tx_struct.data, m_tx_struct.data_length );

	if ( m_modbus_display != nullptr )
	{
		m_modbus_display->Bus_Active();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_UART::Enable_Port( MB_RX_PROC_CBACK rx_cback,
							   MB_RX_PROC_CBACK_HANDLE rx_cback_handle,
							   tx_mode_t modbus_tx_mode, parity_t parity_select, uint32_t baud_rate,
							   bool use_long_holdoff, stop_bits_t stop_bit_select )
{
	uint32_t rx_done_time = 0U;
	uint32_t tx_holdoff_time = 0U;

	uC_USART_Buff::parity_t usart_parity = uC_USART_Buff::PARITY_EVEN;
	uC_USART_Buff::char_size_t usart_char_size;
	uC_USART_Buff::stop_bit_t usart_stop_bit = uC_USART_Buff::STOP_BIT_1;
	uC_USART_Buff::cback_status_t usart_dma_req_bits = 0U;

	m_rx_cback = rx_cback;
	m_rx_cback_handle = rx_cback_handle;

	BF_Lib::Bit::Set( m_port_flags, PORT_ENABLED );

	m_rx_data_ptr = m_rx_buff;
	m_rx_state = RX_IDLE;

	m_tx_state = TX_IDLE;

	if ( m_modbus_display != nullptr )
	{
		m_modbus_display->Bus_Idle();
	}

	switch ( parity_select )
	{
		case MODBUS_PARITY_EVEN:
			usart_parity = uC_USART_Buff::PARITY_EVEN;
			break;

		case MODBUS_PARITY_ODD:
			usart_parity = uC_USART_Buff::PARITY_ODD;
			break;

		case MODBUS_PARITY_NONE:
			usart_parity = uC_USART_Buff::PARITY_NONE;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	switch ( stop_bit_select )
	{
		case MODBUS_1_STOP_BIT:
			usart_stop_bit = uC_USART_Buff::STOP_BIT_1;
			break;

		case MODBUS_2_STOP_BIT:
			usart_stop_bit = uC_USART_Buff::STOP_BIT_2;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	if ( modbus_tx_mode == MODBUS_RTU_TX_MODE )
	{
		BF_Lib::Bit::Clr( m_port_flags, TX_MODE_ASCII );

		if ( ( use_long_holdoff == true ) &&
			 ( baud_rate > RTU_LONG_HOLDOFF_BAUD_RATE_MIN ) )
		{
			rx_done_time = RX_DONE_LONG_HOLDOFF_TIME;
			tx_holdoff_time = TX_LONG_HOLDOFF_TIME;
		}
		else
		{
			rx_done_time = Get_Rx_Done_Time_Calc() / baud_rate;
			tx_holdoff_time = Get_Tx_Holdoff_Time_Calc() / baud_rate;
		}
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
		 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers,
		 * not enums.
		 */
		/*lint -e{930} # MISRA Deviation */
		usart_char_size = static_cast<uC_USART_Buff::char_size_t>( uC_USART_Buff::CHAR_SIZE_8BIT );
	}
	else
	{
		BF_ASSERT( BF_Lib::Bit::Test( m_port_flags, ASCII_SUPPORT ) );
		BF_Lib::Bit::Set( m_port_flags, TX_MODE_ASCII );
		rx_done_time = MODBUS_ASCII_INTER_CHARACTER_DELAY;
		BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::RX_BYTE_COMPLETE );

		usart_char_size = uC_USART_Buff::CHAR_SIZE_7BIT;
	}

	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::RX_BYTE_TIMEOUT );
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::RX_BUFF_FULL );
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::TX_COMPLETE );
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::TXRX_HOLDOFF_COMPLETE );
	m_usart->Config_Callback( &UART_Callback_Static, this, usart_dma_req_bits,
							  rx_done_time, tx_holdoff_time );

	m_usart->Config_Port( baud_rate, usart_char_size, usart_parity, usart_stop_bit );

	m_rx_data_length = 0U;
	m_usart->Start_RX( m_rx_buff, RX_BUFFER_SIZE_INTERNAL );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_UART::UART_Callback( uC_USART_Buff::cback_status_t status )
{
	bool buff_full;
	bool byte_timeout;
	bool byte_received = false;

	buff_full = BF_Lib::Bit::Test( status, uC_USART_Buff::RX_BUFF_FULL );
	byte_timeout = BF_Lib::Bit::Test( status, uC_USART_Buff::RX_BYTE_TIMEOUT );
	byte_received = BF_Lib::Bit::Test( status, uC_USART_Buff::RX_BYTE_COMPLETE );
	if ( ( buff_full == true ) || ( byte_timeout == true ) || ( byte_received == true ) )
	{
		if ( !BF_Lib::Bit::Test( m_port_flags, TX_MODE_ASCII ) )
		{	// If this is RTU Mode.
			m_rx_state = RX_MSG_READY;
			m_rx_que->Send_From_ISR( nullptr, false );
			m_rx_data_length = m_usart->Get_RX_Length();
		}
		else	// If this is ASCII Mode.
		{
			m_rx_data_length = m_usart->Get_RX_Length();
			if ( m_rx_buff[0U] != MODBUS_ASCII_BOF )
			{	// Start the frame over.
				m_rx_data_length = 0U;
				m_usart->Start_RX( &m_rx_buff[0U], RX_BUFFER_SIZE_INTERNAL );
			}
			else if ( ( m_rx_data_length > MODBUS_ASCII_FRAMING_SIZE ) &&
					  ( ( m_rx_buff[m_rx_data_length - 1U] == MODBUS_ASCII_LF ) &&
						( m_rx_buff[m_rx_data_length - 2U] == MODBUS_ASCII_CR ) ) )
			{
				m_usart->Stop_RX();
				m_rx_state = RX_MSG_READY;
				m_rx_que->Send_From_ISR( nullptr, false );
			}
			else if ( ( byte_timeout == true ) || ( buff_full == true ) )
			{
				m_rx_data_length = 0U;
				m_usart->Start_RX( &m_rx_buff[m_rx_data_length],
								   RX_BUFFER_SIZE_INTERNAL );
			}
		}
	}

	if ( BF_Lib::Bit::Test( status, static_cast<uint_fast8_t>( uC_USART_Buff::TX_COMPLETE ) ) )
	{
		m_tx_state = TX_IDLE;
		m_tx_enable_out->Off();
		m_rx_data_length = 0U;
		m_usart->Start_RX( m_rx_buff, RX_BUFFER_SIZE_INTERNAL );
	}

	if ( BF_Lib::Bit::Test( status,
							static_cast<uint_fast8_t>( uC_USART_Buff::TXRX_HOLDOFF_COMPLETE ) ) )
	{
		if ( m_tx_state == TX_DATA_READY )
		{
			Send_Frame_Right_Now();
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_UART::Disable_Port( void )
{
	BF_Lib::Bit::Clr( m_port_flags, PORT_ENABLED );
	m_tx_enable_out->Off();						/* LTK-3775 */
	if ( m_modbus_display != nullptr )
	{
		m_modbus_display->Bus_Idle();
	}

	m_usart->Stop_RX();
	m_usart->Stop_TX();

	m_rx_state = RX_IDLE;
	m_tx_state = TX_IDLE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_UART::Port_Enabled( void )
{
	return ( BF_Lib::Bit::Test( m_port_flags, PORT_ENABLED ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_UART::Idle( void )
{
	return ( ( m_rx_state == RX_IDLE ) &&
			 ( m_tx_state == TX_IDLE ) );
}
