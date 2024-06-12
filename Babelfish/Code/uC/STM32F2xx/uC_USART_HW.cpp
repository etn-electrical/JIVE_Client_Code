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
	m_usart( reinterpret_cast<USART_TypeDef*>( nullptr ) ),
	m_io_ctrl( reinterpret_cast<uC_BASE_USART_IO_STRUCT const*>( nullptr ) ),
	m_parity_bit_mask( 0U ),
	m_int_vect_cback( reinterpret_cast<Advanced_Vect*>( nullptr ) )
{
	m_io_ctrl = ( uC_Base::Self() )->Get_USART_Ctrl( port_requested );

	BF_ASSERT( m_io_ctrl != reinterpret_cast<uC_BASE_USART_IO_STRUCT const*>( nullptr ) );

	m_usart = m_io_ctrl->reg_ctrl;

	uC_Base::Reset_Periph( &m_io_ctrl->periph_def );
	uC_Base::Enable_Periph_Clock( &m_io_ctrl->periph_def );

	if ( io_port == NULL )
	{
		io_port = m_io_ctrl->default_port_io;
	}

	uC_IO_Config::Enable_Periph_Input_Pin( io_port->rx_port );
	uC_IO_Config::Enable_Periph_Output_Pin( io_port->tx_port );

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
	m_usart->CR1 |= ( USART_CR1_RE | USART_CR1_UE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Disable_RX( void )
{
	uint32_t temp_mask;

	temp_mask = USART_CR1_RE;
	if ( ( m_usart->CR1 & USART_CR1_TE ) == 0U )
	{
		temp_mask |= USART_CR1_UE;	// Disable the USART if the TX is not enabled.
	}
	m_usart->CR1 &= ~temp_mask;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Enable_TX( void )
{
	m_usart->CR1 |= ( USART_CR1_TE | USART_CR1_UE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Disable_TX( void )
{
	uint32_t temp_mask;

	temp_mask = USART_CR1_TE;
	if ( ( m_usart->CR1 & USART_CR1_RE ) == 0U )
	{
		temp_mask |= USART_CR1_UE;	// Disable the USART if the TX is not enabled.
	}
	m_usart->CR1 &= ~temp_mask;
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
		m_usart->CR1 |= USART_CR1_PCE;		// Enable the parity checker.
		switch ( parity )
		{
			case PARITY_EVEN:
				m_usart->CR1 &= ~USART_CR1_PS;		// This selects Even Parity.
				break;

			case PARITY_ODD:
				m_usart->CR1 |= USART_CR1_PS;		// This selects Even Parity.
				break;

			default:
				BF_ASSERT( false );
				break;
		}
		switch ( char_size )
		{
			case CHAR_SIZE_8BIT:
				m_usart->CR1 |= USART_CR1_M;
				m_parity_bit_mask = 0xFFU;
				break;

			case CHAR_SIZE_7BIT:
				m_usart->CR1 &= ~USART_CR1_M;
				m_parity_bit_mask = 0x7FU;
				break;

			default:
				BF_ASSERT( false );
				break;
		}
	}
	else if ( char_size == CHAR_SIZE_9BIT )
	{
		m_usart->CR1 &= ~USART_CR1_PCE;
		m_usart->CR1 |= USART_CR1_M;
	}
	else if ( char_size == CHAR_SIZE_8BIT )
	{
		m_usart->CR1 &= ~( USART_CR1_PCE | USART_CR1_M );
	}
	else// We are unable to support anything else.  Like 7bit with no parity.
	{
		// configure for 8-bit no-parity 1-stopbit - which is equivalent to 7-bit no-parity
		// 2-stopbits
		m_usart->CR1 &= ~( USART_CR1_PCE | USART_CR1_M );
		stop_bit_count = STOP_BIT_1;
		m_parity_bit_mask = 0x7FU;
	}

	m_usart->CR2 &= ~USART_CR2_STOP;
	switch ( stop_bit_count )
	{
		case STOP_BIT_1p5:
			m_usart->CR2 |= USART_CR2_STOP_1;	// This selects 1.5 stop bits.
			break;

		case STOP_BIT_2:
			m_usart->CR2 |= USART_CR2_STOP;
			break;

		case STOP_BIT_1:
		default:
			break;
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
	uint32_t usart_clock_freq;
	uint32_t temp_baud;

	usart_clock_freq =
		( uC_Base::Self() )->Get_PClock_Freq( m_io_ctrl->periph_def.pclock_reg_index );

	usart_clock_freq = usart_clock_freq << 4;

	temp_baud = usart_clock_freq / ( baud_rate << MULT_BY_16 );

	m_usart->BRR = temp_baud;
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
