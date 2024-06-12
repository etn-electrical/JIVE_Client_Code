/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
 * casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts
 * are required for all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions
 * shall not change the signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as
 * is The note is re-enabled at the end of this file
 */
/*lint -e1960  */

#include "Includes.h"
#include "uC_USART_DMA.h"
#include "Exception.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */


/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART_DMA::uC_USART_DMA( uint8_t hw_usart_num, uint8_t hw_timer_num,
							const uC_BASE_USART_IO_PORT_STRUCT* io_port ) :
	uC_USART_HW( hw_usart_num, io_port, true ),
	m_cback_request( 0U ),
	m_cback_func( static_cast<uC_USART_Buff::cback_func_t>( 0U ) ),
	m_cback_param( static_cast<uC_USART_Buff::cback_param_t>( 0U ) ),
	m_rx_dma_counts_remaining( 0U ),
	m_tx_holdoff( false ),
	m_timer( reinterpret_cast<uC_Multi_Timers*>( nullptr ) ),
	m_rx_state( RX_IDLE ),
	m_tx_state( TX_IDLE ),
	m_int_enabled_mask( 0U ),
	m_premature_rx_timeout( false ),
	m_premature_tx_timeout( false ),
	m_buffer_head( reinterpret_cast<uint8_t*>( nullptr ) ),
	uC_USART_Buff()
{
	Push_TGINT();

	Disable_RX();
	Disable_TX();

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not
	 * enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	Set_Int_Vect( &USART_Int_Static, this, static_cast<uint8_t>( uC_INT_HW_PRIORITY_12 ) );

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not
	 * enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	m_rx_dma_ctrl->Attach_Callback( &RX_DMA_Int_Static,
									static_cast<uC_DMA::cback_param_t>( this ),
									static_cast<uint8_t>( uC_INT_HW_PRIORITY_12 ) );
	m_usart->CR3 |= USART_CR3_DMAR;

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not
	 * enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	m_tx_dma_ctrl->Attach_Callback( &TX_DMA_Int_Static,
									static_cast<uC_DMA::cback_param_t>( this ),
									static_cast<uint8_t>( uC_INT_HW_PRIORITY_12 ) );
	m_usart->CR3 |= USART_CR3_DMAT;

	// Because of the possibility to miss a timer interrupt we should set the interrupt priority
	// of the timer to be less than the rx interrupt.
	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not
	 * enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	m_timer = new uC_Multi_Timers( hw_timer_num, TOTAL_TIMERS,
								   static_cast<uint8_t>( uC_INT_HW_PRIORITY_13 ) );

	m_timer->Set_Callback( RX_TIMER, &RX_Timer_Int_Static, this );
	m_timer->Set_Callback( TX_TIMER, &TX_Timer_Int_Static, this );

	uC_USART_DMA::Stop_TX();
	uC_USART_DMA::Stop_RX();

	m_tx_holdoff = false;

	Enable_TX();
	Enable_Int();

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART_DMA::~uC_USART_DMA( void )
{
	m_cback_func = static_cast<uC_USART_Buff::cback_func_t>( nullptr );
	m_cback_param = static_cast<uC_USART_Buff::cback_param_t>( nullptr );
	delete m_timer;
	m_buffer_head = reinterpret_cast<uint8_t*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Config_Callback( uC_USART_Buff::cback_func_t cback_func,
									uC_USART_Buff::cback_param_t cback_param,
									uC_USART_Buff::cback_status_t cback_req_mask,
									uint32_t rx_byte_time, uint32_t tx_holdoff_time )
{
	uint32_t rx_freq_calc;
	uint32_t tx_freq_calc;

	Stop_TX();
	Stop_RX();

	m_cback_func = cback_func;
	m_cback_param = cback_param;
	m_cback_request = cback_req_mask;

	tx_freq_calc = Round_Div( M_TIME_TO_FREQ_CALC, tx_holdoff_time );
	rx_freq_calc = Round_Div( M_TIME_TO_FREQ_CALC, rx_byte_time );

	if ( tx_freq_calc < rx_freq_calc )
	{
		m_timer->Set_Min_Frequency( tx_freq_calc );
	}
	else
	{
		m_timer->Set_Min_Frequency( rx_freq_calc );
	}
	if ( ( tx_holdoff_time != 0U ) &&
		 ( BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) ||
		   BF_Lib::Bit::Test( m_cback_request,
							  TX_HOLDOFF_COMPLETE ) ) )
	{
		m_timer->Set_Timeout( TX_TIMER, tx_freq_calc, FALSE );
		m_tx_holdoff = false;
	}

	if ( ( rx_byte_time != 0U ) && BF_Lib::Bit::Test( m_cback_request, RX_BYTE_TIMEOUT ) )
	{
		m_timer->Set_Timeout( RX_TIMER, rx_freq_calc, FALSE );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Start_RX( uint8_t* data, uint16_t length )
{
	volatile uint8_t dump_data;

	Push_TGINT();

	Stop_RX();

	m_rx_state = RX_WAITING;
	m_premature_rx_timeout = false;

	m_buffer_head = data;

	dump_data = static_cast<uint8_t>( m_usart->RDR );

	m_rx_dma_ctrl->Set_Buff( data, &m_usart->RDR, length );
	m_rx_dma_counts_remaining = m_rx_dma_ctrl->Counts_Remaining();
	if ( BF_Lib::Bit::Test( m_cback_request, RX_BUFF_FULL ) )
	{
		m_rx_dma_ctrl->Enable_End_Buff_Int();
		m_rx_dma_ctrl->Enable_Int();
	}
	else
	{
		m_rx_dma_ctrl->Disable_End_Buff_Int();
		m_rx_dma_ctrl->Disable_Int();
	}

	m_usart->ISR &= ~USART_ISR_RXNE;
	if ( BF_Lib::Bit::Test( m_cback_request, RX_BYTE_TIMEOUT ) ||
		 BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) )
	{
		// Debatable whether we add this later or not.  Maybe as an additional option.
		// if ( BF_Lib::Bit::Test( m_cback_request, RX_BYTE_TIMEOUT ) )
		// {
		// m_timer->Start_Timeout( RX_TIMER );
		// m_rx_timer->Start_Timeout();
		// }

		m_int_enabled_mask |= USART_CR1_RXNEIE;
		m_usart->CR1 |= USART_CR1_RXNEIE;
	}
	else
	{
		m_int_enabled_mask &= ~USART_CR1_RXNEIE;
		m_usart->CR1 &= ~USART_CR1_RXNEIE;
	}

	m_rx_dma_ctrl->Enable();

	Enable_RX();

	Pop_TGINT();
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-4: Symbol 'dump_data' not accessed
	 * @n PCLint:
	 * @n Justification: The data assignment is to dump the data, data is not used.
	 * This is per design.
	 */
	/*lint -e{550}*/

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART_Buff::rx_state_t uC_USART_DMA::RX_State( void )
{
	return ( m_rx_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t uC_USART_DMA::Get_RX_Length( void )
{
	return ( m_rx_dma_ctrl->Counts_Done() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Stop_RX( void )
{
	Push_TGINT();

	m_rx_state = RX_IDLE;

	m_rx_dma_ctrl->Disable();
	m_rx_dma_ctrl->Clear_Int();

	m_timer->Stop_Timeout( RX_TIMER );
	m_int_enabled_mask &= ~USART_CR1_RXNEIE;	/* Disable RXNE interrupt */
	m_usart->CR1 &= ~USART_CR1_RXNEIE;

	Disable_RX();

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Start_TX( uint8_t* data, uint16_t length )
{
	Push_TGINT();

	Stop_TX();

	m_tx_state = TX_RUNNING;

	m_tx_holdoff = true;
	m_premature_tx_timeout = false;

	m_tx_dma_ctrl->Set_Buff( data, &m_usart->TDR, length );

	m_tx_dma_ctrl->Enable_End_Buff_Int();
	m_tx_dma_ctrl->Enable_Int();

	m_usart->ISR &= ~USART_ISR_TC;
	if ( BF_Lib::Bit::Test( m_cback_request, TX_COMPLETE ) ||
		 BF_Lib::Bit::Test( m_cback_request, TX_HOLDOFF_COMPLETE ) ||
		 BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) )
	{
		m_int_enabled_mask |= USART_CR1_TCIE;
		m_usart->CR1 |= USART_CR1_TCIE;
	}
	else
	{
		m_int_enabled_mask &= ~USART_CR1_TCIE;
		m_usart->CR1 &= ~USART_CR1_TCIE;
	}

	m_tx_dma_ctrl->Enable();

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART_Buff::tx_state_t uC_USART_DMA::TX_State( void )
{
	return ( m_tx_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART_DMA::TX_Holdoff( void )
{
	return ( m_tx_holdoff );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Stop_TX( void )
{
	Push_TGINT();

	m_tx_state = TX_IDLE;

	m_tx_holdoff = false;

	m_tx_dma_ctrl->Disable();
	m_tx_dma_ctrl->Clear_Int();

	m_timer->Stop_Timeout( TX_TIMER );

	m_int_enabled_mask &= ~USART_CR1_TCIE;
	m_usart->CR1 &= ~USART_CR1_TCIE;

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::USART_Int( void )
{
	uint32_t int_status;
	cback_status_t cback_status_bits = 0U;
	uint16_t rx_dma_counts_remaining;
	uint16_t rx_msg_length = 0U;

	int_status = m_usart->ISR & static_cast<uint32_t>( m_int_enabled_mask );
	m_usart->ISR = 0U;

	if ( ( m_usart->CR1 & USART_CR1_RXNEIE ) != 0U )
	{
		rx_dma_counts_remaining = m_rx_dma_ctrl->Counts_Remaining();
		if ( m_rx_dma_counts_remaining != rx_dma_counts_remaining )
		{
			m_rx_dma_counts_remaining = rx_dma_counts_remaining;

			m_rx_state = RX_RECEIVING;
			// Check whether the End of Frame has to be detected
			if ( BF_Lib::Bit::Test( m_cback_request, RX_BYTE_COMPLETE ) )
			{
				// Get the total bytes received from UART.
				rx_msg_length = m_rx_dma_ctrl->Counts_Done();
				// Mask the MSB, if it is a parity bit.
				Screen_Last_RX_Byte( m_buffer_head, rx_msg_length );
				BF_Lib::Bit::Set( cback_status_bits, RX_BYTE_COMPLETE );
				( *m_cback_func )( m_cback_param, cback_status_bits );
				cback_status_bits = 0U;
			}
			if ( BF_Lib::Bit::Test( m_cback_request, RX_BYTE_TIMEOUT ) )
			{
				if ( m_timer->Timeout_Running( RX_TIMER ) )
				{
					m_timer->Restart_Timeout( RX_TIMER );
				}
				else
				{
					m_timer->Start_Timeout( RX_TIMER );
				}
				// If we get here and a timeout is pending then we likely were
				// delayed in processing the timer interrupt.  So we make sure to follow
				// on and reset it later.
				if ( m_timer->Timeout_Pending( RX_TIMER ) )
				{
					m_premature_rx_timeout = true;
				}
			}

			if ( BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) )
			{
				if ( m_timer->Timeout_Running( TX_TIMER ) )
				{
					m_timer->Restart_Timeout( TX_TIMER );
				}
				else
				{
					m_timer->Start_Timeout( TX_TIMER );
					m_tx_holdoff = true;
				}
				// If we get here and a timeout is pending then we likely were
				// delayed in processing the timer interrupt.  So we make sure to follow
				// on and reset it later.
				if ( m_timer->Timeout_Pending( TX_TIMER ) )
				{
					m_premature_tx_timeout = true;
				}
			}
		}
	}

	if ( ( int_status & USART_ISR_TC ) != 0U )
	{
		Stop_TX();

		if ( BF_Lib::Bit::Test( m_cback_request, TX_COMPLETE ) )
		{
			BF_Lib::Bit::Set( cback_status_bits, TX_COMPLETE );
		}
	}

	if ( cback_status_bits != 0U )
	{
		( *m_cback_func )( m_cback_param, cback_status_bits );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::RX_Timer_Int( void )
{
	cback_status_t cback_status_bits = 0U;

	if ( m_premature_rx_timeout == false )
	{
		m_rx_state = RX_IDLE;
		Stop_RX();

		if ( BF_Lib::Bit::Test( m_cback_request, RX_BYTE_TIMEOUT ) )
		{
			Screen_RX_Data( m_buffer_head, m_rx_dma_ctrl->Counts_Done() );
			BF_Lib::Bit::Set( cback_status_bits, RX_BYTE_TIMEOUT );
			( *m_cback_func )( m_cback_param, cback_status_bits );
		}
	}
	else
	{
		// We restart the timeout because we had a receive happen before we were really ready.
		// likely because we were delayed in processing.
		m_timer->Start_Timeout( RX_TIMER );
		m_premature_rx_timeout = false;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::TX_Timer_Int( void )
{
	cback_status_t cback_status_bits = 0U;

	if ( m_premature_tx_timeout == false )
	{
		m_tx_state = TX_IDLE;
		m_tx_holdoff = false;

		m_timer->Stop_Timeout( TX_TIMER );

		if ( BF_Lib::Bit::Test( m_cback_request, TX_HOLDOFF_COMPLETE ) )
		{
			BF_Lib::Bit::Set( cback_status_bits, TX_HOLDOFF_COMPLETE );
		}
		if ( BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) )
		{
			BF_Lib::Bit::Set( cback_status_bits, TXRX_HOLDOFF_COMPLETE );
		}

		if ( cback_status_bits != 0U )
		{
			( *m_cback_func )( m_cback_param, cback_status_bits );
		}
	}
	else
	{
		// We restart the timeout because we had a receive happen before we were really ready.
		// likely because we were delayed in processing.
		m_timer->Start_Timeout( TX_TIMER );
		m_premature_tx_timeout = false;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::RX_DMA_Int( void )
{
	cback_status_t cback_status_bits = 0U;

	// If it is not empty then we received the interrupt because the dma automatically empties
	// after it is disabled.
	if ( m_rx_dma_ctrl->Empty() )
	{
		Stop_RX();

		if ( BF_Lib::Bit::Test( m_cback_request, RX_BUFF_FULL ) )
		{
			Screen_RX_Data( m_buffer_head, m_rx_dma_ctrl->Counts_Done() );
			BF_Lib::Bit::Set( cback_status_bits, RX_BUFF_FULL );
			( *m_cback_func )( m_cback_param, cback_status_bits );
		}

		m_rx_state = RX_IDLE;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::TX_DMA_Int( void )
{
	cback_status_t cback_status_bits = 0U;

	m_tx_dma_ctrl->Disable();
	m_tx_dma_ctrl->Clear_Int();

	if ( BF_Lib::Bit::Test( m_cback_request, TX_HOLDOFF_COMPLETE ) ||
		 BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) )
	{
		if ( m_timer->Timeout_Running( TX_TIMER ) )
		{
			m_timer->Restart_Timeout( TX_TIMER );
		}
		else
		{
			m_timer->Start_Timeout( TX_TIMER );
			m_tx_holdoff = true;
		}
		// If we get here and a timeout is pending then we likely were
		// delayed in processing the timer interrupt.  So we make sure to follow
		// on and reset it later.
		if ( m_timer->Timeout_Pending( TX_TIMER ) )
		{
			m_premature_tx_timeout = true;
		}
	}

	if ( BF_Lib::Bit::Test( m_cback_request, TX_BUFF_EMPTY ) )
	{
		BF_Lib::Bit::Set( cback_status_bits, TX_BUFF_EMPTY );
		( *m_cback_func )( m_cback_param, cback_status_bits );
	}

	m_tx_state = TX_IDLE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART_DMA::Config_Port( uint32_t baud_rate, uC_USART_Buff::char_size_t char_size,
								uC_USART_Buff::parity_t parity,
								uC_USART_Buff::stop_bit_t stop_bit_count )
{
	uC_USART_HW::char_size_t hw_char_size;
	uC_USART_HW::parity_t hw_parity;
	uC_USART_HW::stop_bit_t hw_stop_bit_count;
	bool success = true;

	switch ( char_size )
	{
		case uC_USART_Buff::CHAR_SIZE_5BIT:
			hw_char_size = uC_USART_HW::CHAR_SIZE_5BIT;
			break;

		case uC_USART_Buff::CHAR_SIZE_6BIT:
			hw_char_size = uC_USART_HW::CHAR_SIZE_6BIT;
			break;

		case uC_USART_Buff::CHAR_SIZE_7BIT:
			hw_char_size = uC_USART_HW::CHAR_SIZE_7BIT;
			break;

		case uC_USART_Buff::CHAR_SIZE_8BIT:
			hw_char_size = uC_USART_HW::CHAR_SIZE_8BIT;
			break;

		case uC_USART_Buff::CHAR_SIZE_9BIT:
			hw_char_size = uC_USART_HW::CHAR_SIZE_9BIT;
			break;

		default:
			hw_char_size = uC_USART_HW::CHAR_SIZE_8BIT;
			success = false;
			break;
	}

	switch ( parity )
	{
		case uC_USART_Buff::PARITY_EVEN:
			hw_parity = uC_USART_HW::PARITY_EVEN;
			break;

		case uC_USART_Buff::PARITY_ODD:
			hw_parity = uC_USART_HW::PARITY_ODD;
			break;

		case uC_USART_Buff::PARITY_SPACE:
			hw_parity = uC_USART_HW::PARITY_SPACE;
			break;

		case uC_USART_Buff::PARITY_MARK:
			hw_parity = uC_USART_HW::PARITY_MARK;
			break;

		case uC_USART_Buff::PARITY_NONE:
			hw_parity = uC_USART_HW::PARITY_NONE;
			break;

		default:
			hw_parity = uC_USART_HW::PARITY_NONE;
			success = false;
			break;
	}

	switch ( stop_bit_count )
	{
		case uC_USART_Buff::STOP_BIT_1:
			hw_stop_bit_count = uC_USART_HW::STOP_BIT_1;
			break;

		case uC_USART_Buff::STOP_BIT_1p5:
			hw_stop_bit_count = uC_USART_HW::STOP_BIT_1p5;
			break;

		case uC_USART_Buff::STOP_BIT_2:
			hw_stop_bit_count = uC_USART_HW::STOP_BIT_2;
			break;

		default:
			hw_stop_bit_count = uC_USART_HW::STOP_BIT_1;
			success = false;
			break;
	}

	if ( success == true )
	{
		Configure( baud_rate, hw_char_size, hw_parity, hw_stop_bit_count );
	}

	Enable_TX();
	Enable_Int();
	return ( success );
}

/* lint +e1924
   lint +e1960*/
