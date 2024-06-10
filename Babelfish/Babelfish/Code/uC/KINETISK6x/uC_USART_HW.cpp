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

#include "uC_USART_HW.h"
#include "Babelfish_Assert.h"
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
uC_USART_HW::uC_USART_HW( uint8_t port_requested,
						  uC_BASE_USART_IO_PORT_STRUCT const* io_port, bool enable_dma ) :
	m_rx_dma_ctrl( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_tx_dma_ctrl( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_usart( reinterpret_cast<UART_Type*>( nullptr ) ),
	m_io_ctrl( reinterpret_cast<uC_BASE_USART_IO_STRUCT const*>( nullptr ) ),
	m_parity_bit_mask( 0U ),
	m_int_vect_cback( reinterpret_cast<Advanced_Vect*>( nullptr ) )
{
	m_io_ctrl = ( uC_Base::Self() )->Get_USART_Ctrl( port_requested );

	BF_ASSERT( m_io_ctrl != reinterpret_cast<uC_BASE_USART_IO_STRUCT const*>( nullptr ) )

	m_usart = m_io_ctrl->reg_ctrl;

	uC_Base::Reset_Periph( &m_io_ctrl->periph_def );
	uC_Base::Enable_Periph_Clock( &m_io_ctrl->periph_def );

	if ( io_port == NULL )
	{
		io_port = m_io_ctrl->default_port_io;
	}

	uC_IO_Config::Enable_Periph_Input_Pin( io_port->rx_port );
	uC_IO_Config::Enable_Periph_Output_Pin( io_port->tx_port );
	/*uC_IO_Config::Enable_Periph_Output_Pin( io_port->rts_port );*/

	Disable_RX();
	Disable_TX();

	if ( ( m_io_ctrl->tx_dma_channel != uC_DMA_STREAM_DNE ) &&
		 ( m_io_ctrl->rx_dma_channel != uC_DMA_STREAM_DNE ) &&
		 ( enable_dma == true ) )
	{
		m_tx_dma_ctrl = new uC_DMA( m_io_ctrl->tx_dma_channel, uC_DMA_DIR_FROM_MEMORY,
									static_cast<uint8_t>( sizeof( uint8_t ) ),
									uC_DMA_MEM_INC_TRUE,
									uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_FALSE );
		m_rx_dma_ctrl = new uC_DMA( m_io_ctrl->rx_dma_channel, uC_DMA_DIR_FROM_PERIPH,
									static_cast<uint8_t>( sizeof( uint8_t ) ),
									uC_DMA_MEM_INC_TRUE,
									uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_FALSE );
	}
	else
	{
		m_tx_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
		m_rx_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
	}

	m_int_vect_cback = reinterpret_cast<Advanced_Vect*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Enable_RX( void )
{
	m_usart->C2 |= UART_C2_RE_MASK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Disable_RX( void )
{
	m_usart->C2 &= ~UART_C2_RE_MASK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Enable_TX( void )
{
	m_usart->C2 |= UART_C2_TE_MASK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Disable_TX( void )
{
	m_usart->C2 &= ~UART_C2_TE_MASK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Configure( uint32_t baud_rate, char_size_t char_size,
							 parity_t parity, stop_bit_t stop_bit_count )
{
	m_parity_bit_mask = 0x1FFU;	/// A data read will get the data including the parity bit in the
	// response.  We need to mask it off.

	// if parity is enabled we can only use 7 or 8bit character sizes because the 9th bit is for
	// parity.
	if ( parity != PARITY_NONE )
	{
		m_usart->C1 |= UART_C1_PE_MASK;		// Enable the parity checker.
		switch ( parity )
		{
			case PARITY_EVEN:
				m_usart->C1 &= ~UART_C1_PT_MASK;
				break;

			case PARITY_ODD:
				m_usart->C1 |= UART_C1_PT_MASK;
				break;

			default:
				BF_ASSERT( false );
				break;
		}
		switch ( char_size )
		{
			case CHAR_SIZE_8BIT:
				m_usart->C1 |= UART_C1_M_MASK;
				m_parity_bit_mask = 0xFFU;
				break;

			case CHAR_SIZE_7BIT:
				m_usart->C1 &= ~UART_C1_M_MASK;
				m_parity_bit_mask = 0x7FU;
				break;

			default:
				BF_ASSERT( false );
				break;
		}
	}
	else if ( char_size == CHAR_SIZE_9BIT )
	{
		m_usart->C1 &= ~UART_C1_PE_MASK;
		m_usart->C1 |= UART_C1_M_MASK;
	}
	else if ( char_size == CHAR_SIZE_8BIT )
	{
		m_usart->C1 &= ~( UART_C1_PE_MASK | UART_C1_M_MASK );
	}
	else// We are unable to support anything else.  Like 7bit with no parity.
	{
		// configure for 8-bit no-parity 1-stopbit - which is equivalent to 7-bit no-parity
		// 2-stopbits
		m_usart->C1 &= ~( UART_C1_PE_MASK | UART_C1_M_MASK );
		stop_bit_count = STOP_BIT_1;
	}

	switch ( stop_bit_count )
	{
		case STOP_BIT_1:
			break;

		default:
			break;	// TODO: should we have an exception here?
	}

	Set_Baud_Rate( baud_rate );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Set_Baud_Rate( uint32_t baud_rate )
{
	// UART baud rate calculation for k60
	// SBR and BRFA are settable
	// BR = UART Clock / (16 * (SBR + BRFD))
	// BRFD = BRFA/32

	// SBR = UART Clock / ( BR * 16 )
	// baud_err = UART Clock % ( BR * 16 )
	// BRFA = (2*baud_err)/BR

	uint32_t usart_clock_freq = 0U;
	volatile uint16_t baud_SBR;
	volatile uint8_t baud_BRFA;
	volatile uint8_t BDH_data;
	uint32_t baud_err;
	uint8_t temp;

	// UART Clock = 120 Mhz for UART0 and UART1, 60 Mhz for UART2-UART5 for K66FN
	// UART Clock = 100 Mhz for UART0 and UART1, 50 Mhz for UART2-UART5 for K60DN
	switch ( m_io_ctrl->usart_id )
	{
		case uC_BASE_USART_PORT_0:
		case uC_BASE_USART_PORT_1:
			usart_clock_freq = AHB1_PERIPH_CLOCK_FREQ;
			break;

		case uC_BASE_USART_PORT_2:
		case uC_BASE_USART_PORT_3:
		case uC_BASE_USART_PORT_4:
		case uC_BASE_USART_PORT_5:
			usart_clock_freq = APB2_PERIPH_CLOCK_FREQ;
			break;

		case uC_BASE_MAX_USART_PORTS:
		default:
			BF_ASSERT( false );
			break;
	}

	baud_SBR = ( uint16_t )( usart_clock_freq / ( baud_rate << MULT_BY_16 ) );
	baud_err = usart_clock_freq % ( baud_rate << MULT_BY_16 );
	baud_BRFA = ( uint8_t )( ( baud_err << MULT_BY_2 ) / baud_rate );

	// clear out the SBR bits from BDH before masking in the new bits so that
	// LBKDIE and RXEDGIE are not touched
	BDH_data = m_usart->BDH & ( ~UART_BDH_SBR_MASK );
	temp = ( ( uint8_t )( ( baud_SBR & 0xFF00 ) >> 8 ) & UART_BDH_SBR_MASK );	///< "temp" variable
	// used to remove
	///< Warning[Pa082]
	BDH_data |= temp;
	m_usart->BDH = BDH_data;
	// writing to BDL latches in the new BDH
	m_usart->BDL = ( uint8_t )( baud_SBR & 0x00FF );
	temp = ( uint8_t )( baud_BRFA & UART_C4_BRFA_MASK );
	m_usart->C4 |= temp;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Set_Int_Vect( cback_func_t int_handler,
								cback_param_t param, uint8_t priority )
{
	if ( m_int_vect_cback == NULL )
	{
		m_int_vect_cback = new Advanced_Vect();
	}
	m_int_vect_cback->Set_Vect( int_handler, param );
	uC_Interrupt::Set_Vector(
		reinterpret_cast<INT_CALLBACK_FUNC*>( m_int_vect_cback->Get_Int_Func() ),
		m_io_ctrl->irq_num, priority );
}

/*lint +e1924 */
