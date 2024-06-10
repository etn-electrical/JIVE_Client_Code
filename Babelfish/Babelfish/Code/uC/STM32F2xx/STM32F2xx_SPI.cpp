/**
 *****************************************************************************************
 *	@file STM32F2xx_SPI.cpp
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
#include "STM32F2xx_SPI.h"
#include "uC_IO_Config.h"
#include "Advanced_Vect.h"
#include "uC_Interrupt.h"
#include "uC_RAM.h"
#include "Babelfish_Assert.h"

#define MAX_SPI_CLOCK_PRESCALE  7U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
STM32F2xx_SPI::STM32F2xx_SPI( uint8_t channel, uint8_t no_slaves, bool_t master,
							  uC_BASE_SPI_IO_PORT_STRUCT const* io_ctrl,
							  uC_USER_IO_STRUCT const* const pio[] ) :
	uC_Queued_SPI( no_slaves ),
	m_active_config( reinterpret_cast<select_config_t const*>( nullptr ) ),
	m_spi( reinterpret_cast<SPI_TypeDef*>( nullptr ) ),
	m_rx_stream( reinterpret_cast<DMA_Stream_TypeDef*>( nullptr ) ),
	m_tx_stream( reinterpret_cast<DMA_Stream_TypeDef*>( nullptr ) ),
	m_rx_dma_ctrl( reinterpret_cast<DMA_TypeDef*>( nullptr ) ),
	m_tx_dma_ctrl( reinterpret_cast<DMA_TypeDef*>( nullptr ) ),
	m_spi_vector_obj( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_dma_vector_obj( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_spi_vector_number( NVIC_ENUM_SIZE ),
	m_dma_vector_number( NVIC_ENUM_SIZE ),
	m_rx_data( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_tx_data( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_exchange_length( 0U ),
	m_rx_dma_id( 0U ),
	m_tx_dma_id( 0U ),
	m_master( FALSE ),
	m_data_sink( 0U ),
	m_config( reinterpret_cast<uC_BASE_SPI_IO_STRUCT const*>( nullptr ) ),
	m_rx_dma( reinterpret_cast<uC_BASE_DMA_IO_STRUCT const*>( nullptr ) ),
	m_tx_dma( reinterpret_cast<uC_BASE_DMA_IO_STRUCT const*>( nullptr ) ),
	m_spi_io( reinterpret_cast<uC_BASE_SPI_IO_PORT_STRUCT const*>( nullptr ) ),
	m_total_devices( 0U ),
	m_slave_config( reinterpret_cast<select_config_t*>( nullptr ) ),
	m_dummy_data( 0U ),
	m_volatile_item_rx{ nullptr },
	m_volatile_item_tx{ nullptr }
{
	uint8_t int_priority = static_cast<uint8_t>( uC_INT_HW_PRIORITY_2 );

	m_total_devices = no_slaves;

	m_spi_io = io_ctrl;

	m_slave_config = new select_config_t[m_total_devices];
	for ( uint8_t index = 0U; index < m_total_devices; index++ )
	{
		m_slave_config[index].select_pio = const_cast<uC_USER_IO_STRUCT*>( pio[index] );
		if ( ( ( m_spi_io->ss_in_pio->reg_ctrl ) ==
			   ( m_slave_config[index].select_pio->reg_ctrl ) ) &&
			 ( ( m_spi_io->ss_in_pio->pio_num ) == ( m_slave_config[index].select_pio->pio_num ) ) )
		{
			m_slave_config[index].spi_cr2 = SPI_CR2_SSOE;
		}
	}

	m_config = ( uC_Base::Self() )->Get_SPI_Ctrl( channel );

	if ( m_config != nullptr )
	{
		m_rx_dma = ( uC_Base::Self() )->Get_DMA_Ctrl( m_config->rx_dma_channel );
		m_tx_dma = ( uC_Base::Self() )->Get_DMA_Ctrl( m_config->tx_dma_channel );

		m_spi = m_config->reg_ctrl;
		m_spi_vector_number = m_config->irq_num;
		m_master = master;

		Initialize_Peripheral_Clock( &m_config->periph_def );

		m_spi_vector_obj = new Advanced_Vect();

		m_spi_vector_obj->Set_Vect( &SPI_ISR_Static, this );
		uC_Interrupt::Set_Vector(
			static_cast<INT_CALLBACK_FUNC*>( m_spi_vector_obj->Get_Int_Func() ),
			m_spi_vector_number, int_priority );

		/* Disable SPI, and clear any pending errors */
		m_spi->CR1 = 0U;
		m_data_sink = m_spi->DR;
		m_data_sink = m_spi->SR;

		uC_Interrupt::Enable_Int( m_spi_vector_number );

		if ( ( nullptr != m_rx_dma ) && ( nullptr != m_tx_dma ) )
		{
			m_dma_vector_number = m_rx_dma->irq_num;
			m_rx_stream = m_rx_dma->dma_channel_ctrl;
			m_rx_dma_id = m_rx_dma->dma_num;
			m_rx_dma_ctrl = m_rx_dma->dma_ctrl;
			m_tx_stream = m_tx_dma->dma_channel_ctrl;
			m_tx_dma_id = m_tx_dma->dma_num;
			m_tx_dma_ctrl = m_tx_dma->dma_ctrl;

			Initialize_Peripheral_Clock( &m_rx_dma->periph_def );
			Initialize_Peripheral_Clock( &m_tx_dma->periph_def );

			m_dma_vector_obj = new Advanced_Vect();

			m_dma_vector_obj->Set_Vect( &DMA_ISR_Static, this );
			uC_Interrupt::Set_Vector(
				static_cast<INT_CALLBACK_FUNC*>( m_dma_vector_obj->Get_Int_Func() ),
				m_dma_vector_number, int_priority );
			Clear_DMA_Interrupts();
			uC_Interrupt::Enable_Int( m_dma_vector_number );
		}
		else
		{
			m_rx_stream = reinterpret_cast<DMA_Stream_TypeDef*>( nullptr );
			m_tx_stream = reinterpret_cast<DMA_Stream_TypeDef*>( nullptr );
			m_dma_vector_obj = reinterpret_cast<Advanced_Vect*>( nullptr );
			m_rx_dma_id = 0U;
		}

		Assume_IO_Control( m_spi_io, m_master );
		/* Deassert the chip slect pin. It shall be pulled low when communication with slave needed */
		for ( uint8_t index = 0U; index < m_total_devices; ++index )
		{
			if ( nullptr != m_slave_config[index].select_pio )
			{
				uC_IO_Config::Set_Pin_To_Output( m_slave_config[index].select_pio,
												 uC_IO_OUT_INIT_HIGH );
			}
		}

		m_active_config = reinterpret_cast<select_config_t const*>( nullptr );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
STM32F2xx_SPI::~STM32F2xx_SPI( void )
{
	uC_Interrupt::Clear_Vector( m_spi_vector_number );
	delete m_spi_vector_obj;
	delete m_slave_config;

	if ( nullptr != m_tx_stream )
	{
		m_tx_stream->CR = 0U;
	}

	if ( nullptr != m_rx_stream )
	{
		m_rx_stream->CR = 0U;
		uC_Interrupt::Clear_Vector( m_dma_vector_number );
		delete m_dma_vector_obj;
	}

	m_spi->CR1 = 0U;
	m_spi->CR2 = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_SPI::Start_Exchange( bool_t new_exchange )
{
	uint32_t rx_cr_value;
	uint32_t tx_cr_value;
	uint16_t spi_cr1_base;

	if ( false != new_exchange )
	{
		m_active_config = &m_slave_config[m_active_select];

		if ( nullptr != m_active_config->select_pio )
		{
			/* Assert Chip Select if one in use */
			uC_IO_Config::Clr_Out( m_active_config->select_pio );
		}
	}

	m_data_sink = m_spi->DR;
	m_data_sink = m_spi->SR;
	m_spi->CR1 = SPI_CR1_SPE;
	m_dummy_data = 0U;
	spi_cr1_base = ( false == m_master ) ?
		( m_active_config->spi_cr1 & SPI_CR1_MASK ) :
		( ( m_active_config->spi_cr1 & SPI_CR1_MASK ) |
		  SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_MSTR );

	if ( nullptr == m_dma_vector_obj )
	{
		m_rx_data = ( nullptr == m_active_segment->rx_data ) ?
			&m_dummy_data : m_active_segment->rx_data;
		m_tx_data = ( nullptr == m_active_segment->tx_data ) ?
			&m_dummy_data : m_active_segment->tx_data;
		m_exchange_length = m_active_segment->length;

		m_spi->CR2 = ( m_active_config->spi_cr2 & SPI_CR2_MASK ) | SPI_CR2_RXNEIE;
		m_spi->CR1 = spi_cr1_base | SPI_CR1_SPE;
		m_spi->DR = *m_tx_data;
	}
	else
	{
		/* Set base CR value with class controlled bits masked off */
		rx_cr_value = DMA_SxCR_PL_1 |
			DMA_SxCR_TCIE |
			DMA_SxCR_TEIE |
			DMA_SxCR_DMEIE;

		tx_cr_value = DMA_SxCR_PL_1 |
			DMA_SxCR_DIR_0;

		/* Disable DMA Channels and assign peripheral address */
		m_rx_stream->CR &= ~( DMA_SxCR_EN );
		m_rx_stream->CR = 0U;
		RAM_Diag::uC_RAM::Unmark_As_Volatile( &m_volatile_item_rx );
		m_rx_stream->PAR = reinterpret_cast<uint32_t>( &( m_spi->DR ) );

		m_tx_stream->CR &= ~( DMA_SxCR_EN );
		m_tx_stream->CR = 0U;
		RAM_Diag::uC_RAM::Unmark_As_Volatile( &m_volatile_item_tx );
		m_tx_stream->PAR = reinterpret_cast<uint32_t>( &( m_spi->DR ) );

		m_rx_stream->NDTR = m_active_segment->length;
		m_tx_stream->NDTR = m_active_segment->length;

		// Check if rx data buffer exists
		if ( nullptr != m_active_segment->rx_data )
		{
			m_rx_stream->M0AR = reinterpret_cast<uint32_t>( m_active_segment->rx_data );
			m_volatile_item_rx.start = m_active_segment->rx_data;
			m_volatile_item_rx.end = ( m_volatile_item_rx.start + m_active_segment->length ) - 1U;
			rx_cr_value |= DMA_SxCR_MINC;
		}
		else
		{
			m_rx_stream->M0AR = reinterpret_cast<uint32_t>( &m_dummy_data );
			m_volatile_item_rx.start = &m_dummy_data;
			m_volatile_item_rx.end = ( m_volatile_item_rx.start + m_active_segment->length ) - 1U;
		}

		// Check if tx data buffer exists
		if ( nullptr != m_active_segment->tx_data )
		{
			m_tx_stream->M0AR = reinterpret_cast<uint32_t>( m_active_segment->tx_data );
			m_volatile_item_tx.start = m_active_segment->tx_data;
			m_volatile_item_tx.end = ( m_volatile_item_tx.start + m_active_segment->length ) - 1U;
			tx_cr_value |= DMA_SxCR_MINC;
		}
		else
		{
			m_tx_stream->M0AR = reinterpret_cast<uint32_t>( &m_dummy_data );
			m_volatile_item_tx.start = &m_dummy_data;
			m_volatile_item_tx.end = ( m_volatile_item_tx.start + m_active_segment->length ) - 1U;
		}

		m_spi->CR2 = ( m_active_config->spi_cr2 & SPI_CR2_MASK ) |
			SPI_CR2_RXDMAEN |
			SPI_CR2_TXDMAEN;

		m_spi->CR1 = spi_cr1_base |
			SPI_CR1_SPE;

		m_rx_stream->CR = rx_cr_value;
		m_rx_stream->CR |= DMA_SxCR_EN;
		RAM_Diag::uC_RAM::Mark_As_Volatile( &m_volatile_item_rx );

		m_tx_stream->CR = tx_cr_value;
		m_tx_stream->CR |= DMA_SxCR_EN;
		RAM_Diag::uC_RAM::Mark_As_Volatile( &m_volatile_item_tx );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_SPI::DMA_ISR( void )
{
	bool_t failure;

	while ( 0U != ( m_spi->SR & SPI_SR_BSY ) )
	{}

	m_spi->CR1 = 0U;

	m_tx_stream->CR &= ~( static_cast<uint32_t>( DMA_SxCR_EN ) );
	m_tx_stream->CR = 0U;
	RAM_Diag::uC_RAM::Unmark_As_Volatile( &m_volatile_item_tx );

	m_rx_stream->CR &= ~( static_cast<uint32_t>( DMA_SxCR_EN ) );
	m_rx_stream->CR = 0U;
	RAM_Diag::uC_RAM::Unmark_As_Volatile( &m_volatile_item_rx );

	failure = Clear_DMA_Interrupts();

	if ( ( false != failure ) ||
		 ( nullptr == m_active_segment->next_exchange_segment ) )
	{
		if ( m_active_config->select_pio != reinterpret_cast<uC_USER_IO_STRUCT*>( nullptr ) )
		{
			/* Deassert Chip Select if one in use */
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
void STM32F2xx_SPI::SPI_ISR( void )
{
	*m_rx_data = static_cast<uint8_t>( m_spi->DR );
	m_exchange_length -= 1U;

	if ( 0U != ( m_spi->SR & ( SPI_SR_UDR | SPI_SR_OVR | SPI_SR_MODF ) ) )
	{
		m_spi->CR1 = 0U;
		m_spi->CR2 = 0U;

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

		m_spi->DR = *m_tx_data;

		if ( &m_dummy_data != m_rx_data )
		{
			m_rx_data += 1U;
		}
	}
	else
	{
		m_spi->CR1 = 0U;
		m_spi->CR2 = 0U;

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
void STM32F2xx_SPI::Assume_IO_Control( const uC_BASE_SPI_IO_PORT_STRUCT* spi_io,
									   bool_t master_operation ) const
{
	if ( true == master_operation )
	{
		uC_IO_Config::Enable_Periph_Input_Pin( spi_io->miso_pio );
		uC_IO_Config::Enable_Periph_Output_Pin( spi_io->mosi_pio, true );
		uC_IO_Config::Enable_Periph_Output_Pin( spi_io->clock_pio, true );
	}
	else
	{
		uC_IO_Config::Enable_Periph_Output_Pin( spi_io->miso_pio, false );
		uC_IO_Config::Enable_Periph_Input_Pin( spi_io->mosi_pio );
		uC_IO_Config::Enable_Periph_Input_Pin( spi_io->clock_pio );
		uC_IO_Config::Enable_Periph_Input_Pin( spi_io->ss_in_pio );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_SPI::Initialize_Peripheral_Clock( const PERIPH_DEF_ST* periph_def ) const
{
	uC_Base::Reset_Periph( const_cast<PERIPH_DEF_ST*>( periph_def ) );
	uC_Base::Enable_Periph_Clock( const_cast<PERIPH_DEF_ST*>( periph_def ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t STM32F2xx_SPI::Clear_DMA_Interrupts()
{
	static const uint32_t dma_int_flags[] =
	{
		DMA_LISR_TCIF0 | DMA_LISR_HTIF0 | DMA_LISR_TEIF0 | DMA_LISR_DMEIF0 | DMA_LISR_FEIF0,
		DMA_LISR_TCIF1 | DMA_LISR_HTIF1 | DMA_LISR_TEIF1 | DMA_LISR_DMEIF1 | DMA_LISR_FEIF1,
		DMA_LISR_TCIF2 | DMA_LISR_HTIF2 | DMA_LISR_TEIF2 | DMA_LISR_DMEIF2 | DMA_LISR_FEIF2,
		DMA_LISR_TCIF3 | DMA_LISR_HTIF3 | DMA_LISR_TEIF3 | DMA_LISR_DMEIF3 | DMA_LISR_FEIF3,
		DMA_HISR_TCIF4 | DMA_HISR_HTIF4 | DMA_HISR_TEIF4 | DMA_HISR_DMEIF4 | DMA_HISR_FEIF4,
		DMA_HISR_TCIF5 | DMA_HISR_HTIF5 | DMA_HISR_TEIF5 | DMA_HISR_DMEIF5 | DMA_HISR_FEIF5,
		DMA_HISR_TCIF6 | DMA_HISR_HTIF6 | DMA_HISR_TEIF6 | DMA_HISR_DMEIF6 | DMA_HISR_FEIF6,
		DMA_HISR_TCIF7 | DMA_HISR_HTIF7 | DMA_HISR_TEIF7 | DMA_HISR_DMEIF7 | DMA_HISR_FEIF7
	};

	uint32_t flag_settings;

	// ASSERT(m_rx_dma_id < 8);

	if ( m_rx_dma_id < 4U )
	{
		flag_settings = m_rx_dma_ctrl->LISR & dma_int_flags[m_rx_dma_id];
		m_rx_dma_ctrl->LIFCR |= dma_int_flags[m_rx_dma_id];
	}
	else
	{
		flag_settings = m_rx_dma_ctrl->HISR & dma_int_flags[m_rx_dma_id];
		m_rx_dma_ctrl->HIFCR |= dma_int_flags[m_rx_dma_id];
	}

	if ( m_tx_dma_id < 4U )
	{
		flag_settings |= m_tx_dma_ctrl->LISR & dma_int_flags[m_tx_dma_id];
		m_tx_dma_ctrl->LIFCR |= dma_int_flags[m_tx_dma_id];
	}
	else
	{
		flag_settings |= m_tx_dma_ctrl->HISR & dma_int_flags[m_tx_dma_id];
		m_tx_dma_ctrl->HIFCR |= dma_int_flags[m_tx_dma_id];
	}

	return ( ( ( ( flag_settings & ( DMA_LISR_TEIF0 | DMA_LISR_DMEIF0 |
									 DMA_LISR_TEIF1 | DMA_LISR_DMEIF1 |
									 DMA_LISR_TEIF2 | DMA_LISR_DMEIF2 |
									 DMA_LISR_TEIF3 | DMA_LISR_DMEIF3 ) ) != 0U ) ) ?
			 true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_SPI::SPI_Chip_Configuration( spi_mode mode, uint32_t chip_clock_freq,
											uint8_t assign_chip_id )
{
	uint32_t spi_pclk_freq;
	uint16_t clock_div_counter;

	BF_ASSERT( assign_chip_id < m_total_devices );

	m_slave_config[assign_chip_id].spi_cr1 = 0U;

	if ( 0U != ( mode & 1U ) )
	{
		m_slave_config[assign_chip_id].spi_cr1 |= SPI_CR1_CPHA;
	}

	if ( 0U != ( mode & 2U ) )
	{
		m_slave_config[assign_chip_id].spi_cr1 |= SPI_CR1_CPOL;
	}

	clock_div_counter = 0U;

	spi_pclk_freq =
		( uC_Base::Self() )->Get_PClock_Freq( m_config->periph_def.pclock_reg_index );

	spi_pclk_freq >>= 1U;

	while ( ( clock_div_counter <= MAX_SPI_CLOCK_PRESCALE ) &&
			( spi_pclk_freq > chip_clock_freq ) )
	{
		spi_pclk_freq >>= 1U;
		clock_div_counter += 1U;
	}

	/*  We probably should not use a clock speed faster than the passed in value.*/
	BF_ASSERT( clock_div_counter <= MAX_SPI_CLOCK_PRESCALE );

	m_slave_config[assign_chip_id].spi_cr1 |= ( SPI_CR1_BR
												& static_cast<uint16_t>
												( clock_div_counter << 3U ) );
}

/* lint +e1924
  lint +e1960*/
