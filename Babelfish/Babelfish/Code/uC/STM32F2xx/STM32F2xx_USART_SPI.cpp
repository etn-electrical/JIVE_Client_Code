/**
 *****************************************************************************************
 *	@file STM32F2xx_USART_SPI.cpp
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
#include "STM32F2xx_USART_SPI.h"
#include "uC_IO_Config.h"
#include "Advanced_Vect.h"
#include "uC_Interrupt.h"


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
STM32F2xx_USART_SPI::STM32F2xx_USART_SPI( uint8_t hw_usart_num, uint8_t total_chip_sel,
										  uC_USER_IO_STRUCT const* const* chip_sel_ctrls,
										  const uC_BASE_USART_IO_PORT_STRUCT* usart_io,
										  const uC_PERIPH_IO_STRUCT* usart_clock_io, bool master,
										  bool enable_dma ) :
	uC_Queued_SPI( total_chip_sel ),
	m_usart( reinterpret_cast<USART_TypeDef*>( nullptr ) ),
	m_usart_periph_ctrl( reinterpret_cast<uC_BASE_USART_IO_STRUCT*>( nullptr ) ),
	m_uart_vector_obj( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_slave_config( new select_config_t[total_chip_sel] ),
	m_dummy_data( 0U ),
	m_active_config( reinterpret_cast<select_config_t const*>( nullptr ) ),
	m_rx_dma_ctrl( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_tx_dma_ctrl( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_uart_vector_number( NVIC_ENUM_SIZE ),
	m_rx_data( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_tx_data( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_exchange_length( 0U )
{
	volatile uint16_t data_dump;

	m_usart_periph_ctrl = ( uC_Base::Self() )->Get_USART_Ctrl( hw_usart_num );


	if ( ( master == false ) ||
		 ( m_usart_periph_ctrl == reinterpret_cast<uC_BASE_USART_IO_STRUCT const*>( nullptr ) ) )
	{}
	else
	{
		m_usart = m_usart_periph_ctrl->reg_ctrl;
		Initialize_Peripheral_Clock( &m_usart_periph_ctrl->periph_def );

		m_usart->CR1 = 0U;
		m_usart->CR2 = 0U;
		m_usart->CR3 = 0U;

		data_dump = m_usart->DR;
		/**
		 * @remark Coding Standard Deviation:
		 * @n MISRA-C++[2008] Rule 0-1-9: Previously assigned value to variable 'data_dump' has not been
		 * used
		 * @n PCLint:
		 * @n Justification: The data assignment is to dump the data, data is not used.
		 * This is per design.
		 */
		/*lint -e{838}*/
		data_dump = m_usart->SR;

		if ( ( m_usart_periph_ctrl->rx_dma_channel != uC_DMA_STREAM_DNE ) &&
			 ( m_usart_periph_ctrl->tx_dma_channel != uC_DMA_STREAM_DNE ) &&
			 ( enable_dma == true ) )
		{
			m_tx_dma_ctrl = new uC_DMA( m_usart_periph_ctrl->tx_dma_channel, uC_DMA_DIR_FROM_MEMORY,
										sizeof( uint8_t ), uC_DMA_MEM_INC_TRUE,
										uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_FALSE );

			m_rx_dma_ctrl = new uC_DMA( m_usart_periph_ctrl->rx_dma_channel, uC_DMA_DIR_FROM_PERIPH,
										sizeof( uint8_t ), uC_DMA_MEM_INC_TRUE,
										uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_FALSE );

			m_rx_dma_ctrl->Attach_Callback( &DMA_ISR_Static,
											reinterpret_cast<uC_DMA::cback_param_t>( this ),
											uC_INT_HW_PRIORITY_6 );
			m_rx_dma_ctrl->Enable_Int();

		}
		else
		{
			m_uart_vector_number = m_usart_periph_ctrl->irq_num;

			m_uart_vector_obj = new Advanced_Vect();

			m_uart_vector_obj->Set_Vect( &ISR_Static, this );
			uC_Interrupt::Set_Vector(
				static_cast<INT_CALLBACK_FUNC*>( m_uart_vector_obj->Get_Int_Func() ),
				m_uart_vector_number, uC_INT_HW_PRIORITY_6 );

			uC_Interrupt::Enable_Int( m_uart_vector_number );

			m_tx_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
			m_rx_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
		}

		Assume_IO_Control( usart_io, usart_clock_io );

		for ( uint8_t index = 0U; index < total_chip_sel; ++index )
		{
			m_slave_config[index].select_pio = chip_sel_ctrls[index];
			uC_IO_Config::Set_Pin_To_Output( chip_sel_ctrls[index], uC_IO_OUT_INIT_HIGH );
		}

		m_active_config = reinterpret_cast<select_config_t const*>( nullptr );

		/**
		 * @remark Coding Standard Deviation:
		 * @n MISRA-C++[2008] Rule 0-1-4: Symbol 'data_dump' not accessed
		 * @n PCLint:
		 * @n Justification: The data assignment is to dump the data, data is not used.
		 * This is per design.
		 */
		/*lint -e{550}*/
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
STM32F2xx_USART_SPI::~STM32F2xx_USART_SPI( void )
{
	uC_Interrupt::Clear_Vector( m_uart_vector_number );

	delete m_tx_dma_ctrl;
	delete m_rx_dma_ctrl;
	delete[] m_slave_config;

	m_usart->CR1 = 0U;
	m_usart->CR2 = 0U;
	m_usart->CR3 = 0U;
	m_usart = reinterpret_cast<USART_TypeDef*>( nullptr );
	m_active_config = reinterpret_cast<select_config_t const*>( nullptr );
	m_rx_data = reinterpret_cast<uint8_t*>( nullptr );
	m_tx_data = reinterpret_cast<uint8_t*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_USART_SPI::Start_Exchange( bool new_exchange )
{
	volatile uint16_t data_dump;

	// ASSERT(m_exchange_queue->select_id < m_select_count);

	if ( false != new_exchange )
	{
		m_active_config = &m_slave_config[m_active_select];

		// ASSERT ( nullptr != m_active_config );

		if ( nullptr != m_active_config->select_pio )
		{
			// Assert Chip Select if one in use
			uC_IO_Config::Clr_Out( m_active_config->select_pio );
		}
	}

	data_dump = m_usart->DR;
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-9: Previously assigned value to variable 'data_dump' has not been
	 * used
	 * @n PCLint:
	 * @n Justification: The data assignment is to dump the data, data is not used.
	 * This is per design.
	 */
	/*lint -e{838}*/
	data_dump = m_usart->SR;

	m_dummy_data = 0U;

	if ( m_tx_dma_ctrl == reinterpret_cast<uC_DMA*>( nullptr ) )
	{
		if ( nullptr == m_active_segment->rx_data )
		{
			m_rx_data = &m_dummy_data;
		}
		else
		{
			m_rx_data = m_active_segment->rx_data;
		}
		if ( nullptr == m_active_segment->tx_data )
		{
			m_tx_data = &m_dummy_data;
		}
		else
		{
			m_tx_data = m_active_segment->tx_data;
		}

		m_exchange_length = m_active_segment->length;

		// We have to disable the USART before we can configure the CPOL etc.
		m_usart->CR1 = 0U;
		m_usart->CR2 = USART_CR2_CLKEN | USART_CR2_LBCL | m_active_config->usart_cr2;
		m_usart->CR3 = 0U;
		m_usart->BRR = m_active_config->usart_brr;
		m_usart->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE;
		m_usart->DR = *m_tx_data;
	}
	else
	{
		// Check if rx data buffer exists
		if ( nullptr != m_active_segment->rx_data )
		{
			m_rx_dma_ctrl->Set_Buff( m_active_segment->rx_data, &m_usart->DR,
									 m_active_segment->length );
		}
		else
		{
			m_rx_dma_ctrl->Set_Buff( uC_DMA::NULL_8b_DATA, &m_usart->DR, m_active_segment->length );
		}

		// Check if tx data buffer exisits
		if ( nullptr != m_active_segment->tx_data )
		{
			m_tx_dma_ctrl->Set_Buff( m_active_segment->tx_data, &m_usart->DR,
									 m_active_segment->length );
		}
		else
		{
			m_tx_dma_ctrl->Set_Buff( uC_DMA::NULL_8b_DATA, &m_usart->DR, m_active_segment->length );
		}

		m_rx_dma_ctrl->Enable_End_Buff_Int();

		m_usart->CR1 = 0U;
		m_usart->CR2 = USART_CR2_CLKEN | USART_CR2_LBCL | m_active_config->usart_cr2;
		m_usart->CR3 = USART_CR3_DMAR | USART_CR3_DMAT;
		m_usart->BRR = m_active_config->usart_brr;
		m_usart->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
		// m_usart->DR = *m_tx_data;

		m_rx_dma_ctrl->Enable();
		m_tx_dma_ctrl->Enable();
	}
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-4: Symbol 'data_dump' not accessed
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
void STM32F2xx_USART_SPI::DMA_ISR( void )
{
	bool failure = false;

	while ( m_tx_dma_ctrl->Busy() )
	{}

	m_usart->CR1 = 0U;

	m_rx_dma_ctrl->Disable();
	m_tx_dma_ctrl->Disable();

	if ( nullptr == m_active_segment->next_exchange_segment )
	{
		if ( m_active_config->select_pio != reinterpret_cast<uC_USER_IO_STRUCT*>( nullptr ) )
		{
			// Deassert Chip Select if one in use
			uC_IO_Config::Set_Out( m_active_config->select_pio );
		}
	}

	Exchange_Complete( failure );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_USART_SPI::ISR( void )
{
	*m_rx_data = static_cast<uint8_t>( m_usart->DR );
	m_exchange_length -= 1U;

	if ( 0U != ( m_usart->SR & ( USART_SR_PE | USART_SR_FE | USART_SR_NE | USART_SR_ORE ) ) )
	{
		m_usart->CR1 = 0U;
		m_usart->CR2 = 0U;
		m_usart->CR3 = 0U;

		if ( m_active_config->select_pio != reinterpret_cast<uC_USER_IO_STRUCT*>( nullptr ) )
		{
			// Deassert Chip Select if one in use
			uC_IO_Config::Set_Out( m_active_config->select_pio );
		}

		Exchange_Complete( true );
	}
	else if ( 0U < m_exchange_length )
	{
		if ( &m_dummy_data != m_tx_data )
		{
			m_tx_data += 1U;
		}

		m_usart->DR = *m_tx_data;

		if ( &m_dummy_data != m_rx_data )
		{
			m_rx_data += 1U;
		}
	}
	else
	{
		m_usart->CR1 = 0U;
		m_usart->CR2 = 0U;
		m_usart->CR3 = 0U;

		if ( ( nullptr == m_active_segment->next_exchange_segment ) &&
			 ( nullptr != m_active_config->select_pio ) )
		{
			// Deassert Chip Select if one in use
			uC_IO_Config::Set_Out( m_active_config->select_pio );
		}

		Exchange_Complete( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_USART_SPI::Assume_IO_Control( uC_BASE_USART_IO_PORT_STRUCT const* usart_pio,
											 uC_PERIPH_IO_STRUCT const* usart_clock_pio ) const
{
	uC_IO_Config::Enable_Periph_Input_Pin( usart_pio->rx_port );
	uC_IO_Config::Enable_Periph_Output_Pin( usart_pio->tx_port, true );
	uC_IO_Config::Enable_Periph_Output_Pin( usart_clock_pio, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_USART_SPI::Initialize_Peripheral_Clock( const PERIPH_DEF_ST* periph_def ) const
{
	uC_Base::Reset_Periph( const_cast<PERIPH_DEF_ST*>( periph_def ) );
	uC_Base::Enable_Periph_Clock( const_cast<PERIPH_DEF_ST*>( periph_def ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_USART_SPI::SPI_Chip_Configuration( spi_mode mode, uint32_t chip_clock_freq,
												  uint8_t assign_chip_id )
{
	uint32_t usart_clock_freq;
	uint32_t temp_baud;
	uint32_t periph_clock_freq;

	periph_clock_freq = ( uC_Base::Self() )->Get_PClock_Freq(
		m_usart_periph_ctrl->periph_def.pclock_reg_index );

	m_slave_config[assign_chip_id].usart_cr2 = 0U;

	if ( 0U != ( mode & 1U ) )
	{
		m_slave_config[assign_chip_id].usart_cr2 |= USART_CR2_CPHA;
	}

	if ( 0U != ( mode & 2U ) )
	{
		m_slave_config[assign_chip_id].usart_cr2 |= USART_CR2_CPOL;
	}

	usart_clock_freq = periph_clock_freq;

	usart_clock_freq = usart_clock_freq << 4U;

	temp_baud = usart_clock_freq / ( chip_clock_freq << MULT_BY_16 );

	m_slave_config[assign_chip_id].usart_brr = static_cast<uint16_t>( temp_baud );
}

/* lint +e1924
  lint +e1960*/
