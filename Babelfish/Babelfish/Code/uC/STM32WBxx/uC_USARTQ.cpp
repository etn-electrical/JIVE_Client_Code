/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_USARTQ.h"
#include "Ram.h"
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
uC_USARTQ::uC_USARTQ( uC_USART* usart_handle, uint16_t tx_buff_size, uint16_t rx_buff_size )
{
	volatile uint32_t dump;

	m_io_ctrl = ( uC_Base::Self() )->Get_USART_Ctrl( port_requested );
	BF_ASSERT( m_io_ctrl != 0 );
	m_usart = m_io_ctrl->reg_ctrl;

	uC_Base::Reset_Periph( &m_io_ctrl->periph_def );
	uC_Base::Enable_Periph_Clock( &m_io_ctrl->periph_def );

	Disable_RX();
	Disable_TX();

	if ( ( m_io_ctrl->tx_dma_channel != uC_DMA_CHANNEL_DNE ) &&
		 ( m_io_ctrl->tx_dma_channel != uC_DMA_CHANNEL_DNE ) )
	{
		m_tx_dma_ctrl = new uC_DMA( m_io_ctrl->tx_dma_channel, uC_DMA_DIR_FROM_MEMORY );
		m_rx_dma_ctrl = new uC_DMA( m_io_ctrl->rx_dma_channel, uC_DMA_DIR_FROM_PERIPH );
	}
	else
	{
		m_tx_dma_ctrl = NULL;
		m_rx_dma_ctrl = NULL;
	}

	m_tx_buff_size = tx_buff_size;
	m_tx_buff = ( uint8_t* )Ram::Allocate( tx_buff_size + 3 );
	m_tx_buff = ( m_tx_buff + 3 ) & ( ~( uint32_t )0x3 );
	m_rx_buff_size = rx_buff_size;
	m_rx_buff = ( uint8_t* )Ram::Allocate( rx_buff_size + 3 );
	m_rx_buff = ( m_rx_buff + 3 ) & ( ~( uint32_t )0x3 );

	m_tx_dma_ctrl->Set_Buff( m_tx_buff, &m_usart->DR, m_tx_buff_size, true );
	m_rx_dma_ctrl->Set_Buff( m_rx_buff, &m_usart->DR, m_rx_buff_size, true );

	m_usart->CR3 |= ( USART_CR3_DMAT | USART_CR3_DMAR );

	m_int_status_mask = 0;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Enable_RX( void )
{
	uC_IO_Config::Enable_Periph_Input_Pin( m_io_ctrl->rx_data_pio );
	m_usart->CR1 |= ( USART_CR1_RE | USART_CR1_UE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Disable_RX( void )
{
	uint16_t temp_mask;

	uC_IO_Config::Disable_Periph_Pin_Set_To_Input( m_io_ctrl->rx_data_pio, uC_IO_PULLUP_ENABLE );

	temp_mask = USART_CR1_RE;
	if ( ( m_usart->CR1 & USART_CR1_TE ) == 0 )
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
void uC_USART::Enable_TX( void )
{
	uC_IO_Config::Enable_Periph_Output_Pin( m_io_ctrl->tx_data_pio, uC_IO_OUT_INIT_HIGH );
	m_usart->CR1 |= ( USART_CR1_TE | USART_CR1_UE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Disable_TX( bool output_state )
{
	uint16_t temp_mask;

	uC_IO_Config::Disable_Periph_Pin_Set_To_Output( m_io_ctrl->tx_data_pio, output_state );

	temp_mask = USART_CR1_RE;
	if ( ( m_usart->CR1 & USART_CR1_RE ) == 0 )
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
void uC_USART::Set_Baud_Rate( uint32_t baud_rate, bool sample_count )
{
	uint32_t usart_clock_freq;
	uint32_t temp_baud;

	usart_clock_freq = ( uC_Base::Self() )->Get_PClock_Freq( m_io_ctrl->periph_def.pclock_reg_index );
	usart_clock_freq = usart_clock_freq << 4;

	temp_baud = usart_clock_freq / ( baud_rate << MULT_BY_16 );

	m_usart->BRR = temp_baud;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Configure( uint32_t char_size, uint32_t desired_mode, uint32_t parity,
						  uint32_t stop_bit_count )
{
	// if parity is enabled we can only use 8bit character sizes because the 9th bit is for parity.
	if ( parity != uC_USART_PARITY_NONE )
	{
		if ( char_size == uC_USART_CHAR_SIZE_8BIT )
		{
			m_usart->CR1 |= ( USART_CR1_PCE | USART_CR1_M );
		}
		else
		{
			BF_ASSERT( false );
		}
	}
	else if ( char_size == uC_USART_CHAR_SIZE_9BIT )
	{
		m_usart->CR1 &= ~USART_CR1_PCE;
		m_usart->CR1 |= USART_CR1_M;
	}
	else if ( char_size == uC_USART_CHAR_SIZE_8BIT )
	{
		m_usart->CR1 &= ~( USART_CR1_PCE | USART_CR1_M );
	}
	else
	{
		BF_ASSERT( false );
	}

	m_usart->CR2 &= USART_CR2_STOP;
	switch ( stop_bit_count )
	{
		case uC_USART_1p5_STOP_BIT:
			m_usart->CR2 |= USART_CR2_STOP_1;
			break;

		case uC_USART_2_STOP_BIT:
			m_usart->CR2 |= USART_CR2_STOP;
			break;

		case uC_USART_1_STOP_BIT:
		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_Int_Vect( uC_USART_INT_CBACK* int_handler,
							 uC_USART_HANDLE_TD param, uint8_t priority )
{
	*m_io_ctrl->obj_param = param;
	*m_io_ctrl->obj_func = int_handler;

	uC_Interrupt::Set_Vector( ( INT_CALLBACK_FUNC* )m_io_ctrl->usart_int_cback,
							  m_io_ctrl->irq_num, priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_OS_Int_Vect( uC_USART_INT_CBACK* int_handler,
								uC_USART_HANDLE_TD param, uint8_t priority )
{
	*m_io_ctrl->obj_param = param;
	*m_io_ctrl->obj_func = int_handler;

	uC_OS_Interrupt::Set_OS_Vect( ( INT_CALLBACK_FUNC* )m_io_ctrl->usart_int_cback,
								  m_io_ctrl->irq_num, priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_TX_DMA( uint8_t* data, uint16_t length )
{
	m_tx_dma_ctrl->Disable();

	m_tx_dma_ctrl->Set_Buff( data, &m_usart->DR, length, false );

	m_usart->CR3 |= USART_CR3_DMAT;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USARTQ::Enque( uint8_t data )			// uint8_t data
{
	bool success = true;

	OS_Tasker::Enter_Critical();



	m_usart->US_PTCR = AT91C_PDC_TXTDIS;

	if ( ( m_usart->US_TNCR + m_usart->US_TCR ) < m_tx_que_byte_size )
	{
		*m_tx_que_head = data;
		if ( ( uint8_t* )m_usart->US_TPR > m_tx_que_head )
		{
			m_usart->US_TNPR = ( uint32_t )m_tx_que_start;
			m_usart->US_TNCR++;
		}
		else
		{
			m_usart->US_TCR++;
		}
		m_tx_que_head++;
		if ( m_tx_que_head >= m_tx_que_end )
		{
			m_tx_que_head = m_tx_que_start;
		}
	}
	else
	{
		success = false;
	}

	m_usart->US_PTCR = AT91C_PDC_TXTEN;
	OS_Tasker::Exit_Critical();

	return ( success );
}

#if 0

#define uC_USART_OVER_DIV_8         8
#define uC_USART_OVER_DIV_16        16


const uint8_t uC_USART::m_parity_masks[uC_USART_PARITY_MAX_OPTIONS] =
{
	AT91C_US_PAR_EVEN >> 9,
	AT91C_US_PAR_ODD >> 9,
	AT91C_US_PAR_SPACE >> 9,
	AT91C_US_PAR_MARK >> 9,
	AT91C_US_PAR_NONE >> 9,
	AT91C_US_PAR_MULTI_DROP >> 9
};

#define uC_USART_RX_QUE_SAFETY          4		// This should give 2 bytes.

#ifdef uC_USART_EXTERN_CLOCK_FREQ
	#define uC_USART_MASTER_CLOCK_FREQ      uC_USART_EXTERN_CLOCK_FREQ
#else
	#define uC_USART_MASTER_CLOCK_FREQ      MASTER_CLOCK_FREQUENCY
#endif

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
uC_USART::uC_USART( uint8_t port_requested )
{
	volatile uint32_t dump;

	m_io_ctrl = ( uC_Base::Self() )->Get_USART_Ctrl( port_requested );
	BF_ASSERT( m_io_ctrl != 0 );
	m_usart = m_io_ctrl->reg_ctrl;

	uC_Base::Enable_PMC( m_io_ctrl->pid );

	m_usart->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX;
	dump = m_usart->US_RHR;
	m_usart->US_CSR = AT91C_US_RSTSTA;

	m_usart->US_IDR = USINT32_BIT_MASK;

	Disable_RX();
	Disable_TX();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART::uC_USART( uint8_t port_requested, uint16_t que_byte_size )
{
	volatile uint32_t dump;

	m_io_ctrl = ( uC_Base::Self() )->Get_USART_Ctrl( port_requested );
	BF_ASSERT( m_io_ctrl != 0 );
	m_usart = m_io_ctrl->reg_ctrl;

	uC_Base::Enable_PMC( m_io_ctrl->pid );

	m_usart->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX;
	dump = m_usart->US_RHR;
	m_usart->US_CSR = AT91C_US_RSTSTA;

	m_usart->US_IDR = USINT32_BIT_MASK;

	Disable_RX();
	Disable_TX();

	m_tx_que_byte_size = que_byte_size;
	m_tx_que_count = que_byte_size;
	m_tx_que_start = ( uint8_t* )Ram::Allocate( ( size_t )que_byte_size );
	m_rx_que_start = ( uint8_t* )Ram::Allocate( ( size_t )que_byte_size );

	m_rx_que_byte_size = que_byte_size + uC_USART_RX_QUE_SAFETY;
	m_rx_que_count = que_byte_size + uC_USART_RX_QUE_SAFETY;
	m_rx_que_tail = m_rx_que_start;
	m_rx_que_end = m_rx_que_start + m_rx_que_byte_size;
	m_usart->US_RPR = ( uint32_t )m_rx_que_start;
	m_usart->US_RCR = m_rx_que_count;
	m_usart->US_RNPR = ( uint32_t )m_rx_que_start;
	m_usart->US_RNCR = 0;

	m_usart->US_TPR = ( uint32_t )m_tx_que_start;
	m_usart->US_TCR = 0;
	m_usart->US_TNPR = ( uint32_t )m_tx_que_start;
	m_usart->US_TNCR = 0;
	m_tx_que_byte_size = que_byte_size;
	m_tx_que_count = que_byte_size;
	m_tx_que_head = m_tx_que_start;
	m_tx_que_end = m_tx_que_start + que_byte_size;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_Baud_Rate( uint32_t baud_rate, bool sample_count )
{
	uint32_t temp_baud;

	Set_Mask( m_usart->US_MR, AT91C_US_CLKS, AT91C_US_CLKS_CLOCK );

	switch ( sample_count )
	{
		case uC_USART_SAMPLE_COUNT_16:
			Clr_Mask( m_usart->US_MR, AT91C_US_OVER );
			temp_baud = uC_USART_OVER_DIV_16 * baud_rate;
			break;

		case uC_USART_SAMPLE_COUNT_8:
			Set_Mask( m_usart->US_MR, AT91C_US_OVER, AT91C_US_OVER );
			temp_baud = uC_USART_OVER_DIV_8 * baud_rate;
			break;

		default:
			BF_ASSERT( false );
			break;
	}
	temp_baud = Round_Div( uC_USART_MASTER_CLOCK_FREQ, temp_baud );	// uC_USART_MASTER_CLOCK_FREQ / temp_baud;

	m_usart->US_BRGR = temp_baud;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_Int_Vect( INT_CALLBACK_FUNC* int_handler, uint8_t priority )
{
	uC_Interrupt::Set_Vector( int_handler, m_io_ctrl->pid,
							  priority, uC_INT_PERIPH_HIGH_LEVEL );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_OS_Int_Vect( OS_INT_CALLBACK_FUNC* int_handler, uint8_t priority )
{
	uC_OS_Interrupt::Set_OS_Vect( int_handler, m_io_ctrl->pid,
								  priority, uC_INT_PERIPH_HIGH_LEVEL );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_TX_DMA( uint8_t* data, uint16_t length )
{
	m_usart->US_TPR = ( uint32_t )data;
	m_usart->US_TCR = length;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_RX_DMA( uint8_t* data, uint16_t length )
{
	m_usart->US_RPR = ( uint32_t )data;
	m_usart->US_RCR = length;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_Bus_Idle_Timeout( uint16_t bit_times )
{
	m_usart->US_RTOR = bit_times;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART::Enque( uint16_t data )			// uint16_t data
{
	bool success = true;
	uint16_t* temp_ptr;

	OS_Tasker::Enter_Critical();
	m_usart->US_PTCR = AT91C_PDC_TXTDIS;

	if ( ( m_usart->US_TNCR + m_usart->US_TCR ) < m_tx_que_byte_size )
	{
		temp_ptr = ( uint16_t* )m_tx_que_head;
		*temp_ptr = data;
		if ( ( uint8_t* )m_usart->US_TPR > m_tx_que_head )
		{
			m_usart->US_TNCR++;
			m_usart->US_TNPR = ( uint32_t )m_tx_que_start;
		}
		else
		{
			m_usart->US_TCR++;
		}
		m_tx_que_head++;
		if ( m_tx_que_head >= m_tx_que_end )
		{
			m_tx_que_head = m_tx_que_start;
		}
	}
	else
	{
		success = false;
	}

	m_usart->US_PTCR = AT91C_PDC_TXTEN;
	OS_Tasker::Exit_Critical();

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART::Enque( uint8_t data )			// uint8_t data
{
	bool success = true;

	OS_Tasker::Enter_Critical();
	m_usart->US_PTCR = AT91C_PDC_TXTDIS;

	if ( ( m_usart->US_TNCR + m_usart->US_TCR ) < m_tx_que_byte_size )
	{
		*m_tx_que_head = data;
		if ( ( uint8_t* )m_usart->US_TPR > m_tx_que_head )
		{
			m_usart->US_TNPR = ( uint32_t )m_tx_que_start;
			m_usart->US_TNCR++;
		}
		else
		{
			m_usart->US_TCR++;
		}
		m_tx_que_head++;
		if ( m_tx_que_head >= m_tx_que_end )
		{
			m_tx_que_head = m_tx_que_start;
		}
	}
	else
	{
		success = false;
	}

	m_usart->US_PTCR = AT91C_PDC_TXTEN;
	OS_Tasker::Exit_Critical();

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART::Enque_Full( void )
{
	return ( ( m_usart->US_TNCR + m_usart->US_TCR ) == m_tx_que_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART::Enque_Empty( void )
{
	return ( ( m_usart->US_TNCR + m_usart->US_TCR ) == 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART::Deque( uint16_t* data )
{
	bool success = true;

	OS_Tasker::Enter_Critical();
	m_usart->US_PTCR = AT91C_PDC_RXTDIS;

	if ( ( m_usart->US_RNCR + m_usart->US_RCR ) < m_rx_que_count )
	{
		*data = *m_rx_que_tail;
		if ( m_rx_que_tail < ( uint8_t* )m_usart->US_RPR )
		{
			m_usart->US_RNCR++;
			m_usart->US_RNPR = ( uint32_t )m_rx_que_start;
		}
		else
		{
			m_usart->US_RCR++;
		}
		m_rx_que_tail += 2;
		if ( m_rx_que_tail >= m_rx_que_end )
		{
			m_rx_que_tail = m_rx_que_start;
		}
	}
	else
	{
		success = false;
	}

	m_usart->US_PTCR = AT91C_PDC_RXTEN;
	OS_Tasker::Exit_Critical();

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART::Deque( uint8_t* data )
{
	bool success = true;

	OS_Tasker::Enter_Critical();
	m_usart->US_PTCR = AT91C_PDC_RXTDIS;

	if ( ( m_usart->US_RNCR + m_usart->US_RCR ) < m_rx_que_count )
	{
		*data = *m_rx_que_tail;
		if ( m_rx_que_tail < ( uint8_t* )m_usart->US_RPR )
		{
			m_usart->US_RNPR = ( uint32_t )m_rx_que_start;
			m_usart->US_RNCR++;
		}
		else
		{
			m_usart->US_RCR++;
		}
		m_rx_que_tail++;
		if ( m_rx_que_tail >= m_rx_que_end )
		{
			m_rx_que_tail = m_rx_que_start;
		}
	}
	else
	{
		success = false;
	}

	m_usart->US_PTCR = AT91C_PDC_RXTEN;
	OS_Tasker::Exit_Critical();

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART::Deque_Full( void )
{
	return ( ( m_usart->US_RNCR + m_usart->US_RCR ) == 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART::Deque_Empty( void )
{
	return ( ( m_usart->US_RNCR + m_usart->US_RCR ) == m_rx_que_count );
}

#endif
