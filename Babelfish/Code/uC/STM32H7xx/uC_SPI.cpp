/**
 **********************************************************************************************
 *	@file            uC_SPI.cpp C++ Implementation File for SPI software module.
 *
 *	@brief           The file shall include the definitions of all the functions required for
 *                   SPI communication and which are declared in corresponding header file
 *	@details
 *	@copyright       2014 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "Includes.h"
#include "uC_SPI.h"
#include "uC_Base.h"
#include "RAM.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// These work out to be the shifts - 1.  So a prescaler of 0 = divby2, 1 = divby4 etc.
#define TOTAL_SPI_CLOCK_PRESC       8U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_SPI::uC_SPI( uint8_t spi_select, bool master, uC_USER_IO_STRUCT const* const* chip_sel_ctrls,
				uint8_t total_chip_sel, uC_BASE_SPI_IO_PORT_STRUCT const* io_ctrl,
				bool enable_dma ) :
	m_config( reinterpret_cast<uC_BASE_SPI_IO_STRUCT const*>( nullptr ) ),
	m_spi( reinterpret_cast<SPI_TypeDef*>( nullptr ) ),
	m_chip_sel_ctrls( reinterpret_cast<uC_USER_IO_STRUCT const* const*>( nullptr ) ),
	m_total_chip_sel( 0U ),
	m_chip_sel_cfg( reinterpret_cast<CHIP_SELECT_CFG_STTD*>( nullptr ) ),
	m_rx_dma( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_tx_dma( reinterpret_cast<uC_DMA*>( nullptr ) ),
	first_Tranmit( 0U )
{
	m_config = ( uC_Base::Self() )->Get_SPI_Ctrl( spi_select );
	BF_ASSERT( m_config != reinterpret_cast<uC_BASE_SPI_IO_STRUCT const*>( nullptr ) );
	m_spi = m_config->reg_ctrl;

	uC_Base::Reset_Periph( &m_config->periph_def );
	uC_Base::Enable_Periph_Clock( &m_config->periph_def );

	// SPI will always keep control of all associated GPIOs
	m_spi->CFG2 |= SPI_CFG2_AFCNTR;

	if ( master == true )
	{


		// SS signal input is managed by software (SSM=1, SSI=1)
		m_spi->CR1 |= SPI_CR1_SSI;
		m_spi->CFG2 |= SPI_CFG2_SSM;

		m_spi->CFG2 |= SPI_CFG2_MASTER;

		m_chip_sel_cfg = reinterpret_cast<CHIP_SELECT_CFG_STTD*>( Ram::Allocate(
																	  ( sizeof(
																			CHIP_SELECT_CFG_STTD )
																		* total_chip_sel ) ) );
		m_total_chip_sel = total_chip_sel;
		m_chip_sel_ctrls = chip_sel_ctrls;
	}
	else
	{
		m_chip_sel_cfg = reinterpret_cast<CHIP_SELECT_CFG_STTD*>( nullptr );
		m_total_chip_sel = 0U;
		m_chip_sel_ctrls = reinterpret_cast<uC_USER_IO_STRUCT const* const*>( nullptr );
	}

#ifdef uC_SPI_OS_SEMAPHORE_SUPPORT
	m_semaphore = new OS_Semaphore( true );
#else
	m_spi_busy_status = false;
#endif
	if ( io_ctrl == NULL )
	{
		io_ctrl = m_config->default_port_io;
	}

	if ( master == true )
	{
		uC_IO_Config::Enable_Periph_Input_Pin( io_ctrl->miso_pio );
		uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->mosi_pio, true );
		uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->clock_pio, true );
	}
	else
	{
		uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->miso_pio );
		uC_IO_Config::Enable_Periph_Input_Pin( io_ctrl->mosi_pio );
		uC_IO_Config::Enable_Periph_Input_Pin( io_ctrl->clock_pio );
		uC_IO_Config::Enable_Periph_Input_Pin( io_ctrl->ss_in_pio );
	}
	m_spi->CR1 |= SPI_CR1_SPE;

	if ( ( m_config->rx_dma_channel != static_cast<uint8_t>( uC_DMA_STREAM_DNE ) ) &&
		 ( m_config->tx_dma_channel != static_cast<uint8_t>( uC_DMA_STREAM_DNE ) ) &&
		 ( enable_dma == true ) )
	{
		m_rx_dma = new uC_DMA( m_config->rx_dma_channel, uC_DMA_DIR_FROM_PERIPH,
							   static_cast<uint8_t>( sizeof( uint8_t ) ),
							   uC_DMA_MEM_INC_TRUE, uC_DMA_PERIPH_INC_FALSE,
							   uC_DMA_CIRC_FALSE );

		m_tx_dma = new uC_DMA( m_config->tx_dma_channel, uC_DMA_DIR_FROM_MEMORY,
							   static_cast<uint8_t>( sizeof( uint8_t ) ),
							   uC_DMA_MEM_INC_TRUE, uC_DMA_PERIPH_INC_FALSE,
							   uC_DMA_CIRC_FALSE );

		m_spi->CFG1 |= SPI_CFG1_TXDMAEN;
		m_spi->CFG1 |= SPI_CFG1_RXDMAEN;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_SPI::~uC_SPI( void )
{
	delete m_rx_dma;
	delete m_tx_dma;
#ifdef uC_SPI_OS_SEMAPHORE_SUPPORT
	delete m_semaphore;
#endif
	if ( m_chip_sel_cfg != nullptr )
	{
		Ram::De_Allocate( m_chip_sel_cfg );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Configure_Chip_Select( uint8_t spi_mode, uint8_t chip_select,
									uint32_t desired_spi_clock_freq,
									uint8_t bits_to_transfer, bool manual_chip_select )
{
	uint32_t spi_clock_freq;
	uint_fast16_t clock_div_counter;

	BF_ASSERT( chip_select < m_total_chip_sel );
	m_chip_sel_cfg[chip_select].cr1 = ( m_spi->CR1 & ~SPI_CR1_SPE );
	m_chip_sel_cfg[chip_select].cr2 = m_spi->CR2;
	m_chip_sel_cfg[chip_select].cfg1 = m_spi->CFG1;
	m_chip_sel_cfg[chip_select].cfg2 = m_spi->CFG2;

	switch ( spi_mode )
	{
		case uC_SPI_MODE_0:
			break;

		case uC_SPI_MODE_1:
			m_chip_sel_cfg[chip_select].cfg2 |= static_cast<uint16_t>( SPI_CFG2_CPHA );
			break;

		case uC_SPI_MODE_2:
			m_chip_sel_cfg[chip_select].cfg2 |= static_cast<uint16_t>( SPI_CFG2_CPOL );
			break;

		case uC_SPI_MODE_3:
			m_chip_sel_cfg[chip_select].cfg2 |= static_cast<uint16_t>( SPI_CFG2_CPOL
																	   | SPI_CFG2_CPHA );
			break;

		default:
			break;
	}

	m_chip_sel_cfg[chip_select].manual_pin_ctrl = manual_chip_select;

	if ( ( bits_to_transfer > 3U ) && ( bits_to_transfer <= 16U ) )
	{
		bits_to_transfer = bits_to_transfer - 1U;
		m_chip_sel_cfg[chip_select].cfg1 &= ~SPI_CFG1_DSIZE_Msk;
		m_chip_sel_cfg[chip_select].cfg1 |=
			( static_cast<uint32_t>( bits_to_transfer ) << SPI_CFG1_DSIZE_Pos );

		if ( bits_to_transfer > 8U )
		{
			m_chip_sel_cfg[chip_select].cfg1 |= SPI_CFG1_FTHLV_0;
		}
		else
		{
			m_chip_sel_cfg[chip_select].cfg1 &= ~( SPI_CFG1_FTHLV_Msk );
		}
	}
	else
	{
		BF_ASSERT( false );
	}

	spi_clock_freq = ( uC_Base::Self() )->Get_PClock_Freq(
		m_config->periph_def.pclock_reg_index );
	clock_div_counter = 1U;
	while ( ( clock_div_counter <= TOTAL_SPI_CLOCK_PRESC ) &&
			( ( spi_clock_freq >> clock_div_counter ) > desired_spi_clock_freq ) )
	{
		clock_div_counter++;
	}

	BF_ASSERT( clock_div_counter <= TOTAL_SPI_CLOCK_PRESC );

	clock_div_counter--;	// Due to the fact that 0 = a div by 2.
	m_chip_sel_cfg[chip_select].cfg1 |= ( SPI_CFG1_MBR & ( clock_div_counter << SPI_CFG1_MBR_Pos ) );

	uC_IO_Config::Set_Pin_To_Output( m_chip_sel_ctrls[chip_select],
									 static_cast<bool>( uC_IO_OUT_INIT_HIGH ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Select_Chip( uint8_t chip_select )
{
	uint32_t temp_cr1;
	GINT_TDEF temp_gint;

	Push_GINT( temp_gint );


	// Push the SPI enable register so we can choose to properly enable it
	temp_cr1 = ( m_spi->CR1 & static_cast<uint32_t>( SPI_CR1_SPE ) );
	m_spi->CR1 &= ~( SPI_CR1_SPE );			// Disable the SPI because some of these changes can do damage.

	m_spi->CR1 = m_chip_sel_cfg[chip_select].cr1;
	m_spi->CR2 = m_chip_sel_cfg[chip_select].cr2;
	m_spi->CFG1 = m_chip_sel_cfg[chip_select].cfg1;
	m_spi->CFG2 = m_chip_sel_cfg[chip_select].cfg2;

	m_spi->CR1 |= temp_cr1;		// Re-enable the SPI if it was enabled before all this config mess.
	m_spi->CR1 |= SPI_CR1_CSTART;

	uC_IO_Config::Clr_Out( m_chip_sel_ctrls[chip_select] );

	Pop_GINT( temp_gint );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::De_Select_Chip( uint8_t chip_select ) const
{
	uC_IO_Config::Set_Out( m_chip_sel_ctrls[chip_select] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Set_TX_DMA( uint8_t* data, uint16_t length )
{
	m_tx_dma->Disable();
	m_tx_dma->Set_Buff( data, &m_spi->TXDR, length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Enable_TX_DMA( void )
{
	Push_TGINT();
	m_tx_dma->Enable();
	m_spi->CFG1 |= SPI_CFG1_TXDMAEN;
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Disable_TX_DMA( void )
{
	Push_TGINT();
	m_spi->CFG1 &= ~SPI_CFG1_TXDMAEN;
	m_tx_dma->Disable();
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_SPI::TX_DMA_Busy( void )
{
	return ( m_tx_dma->Busy() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Set_RX_DMA( uint8_t* data, uint16_t length )
{
	m_rx_dma->Disable();
	m_rx_dma->Set_Buff( data, &m_spi->RXDR, length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Enable_RX_DMA( void )
{
	Push_TGINT();
	m_rx_dma->Enable();
	m_spi->CFG1 |= SPI_CFG1_RXDMAEN;
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Disable_RX_DMA( void )
{
	Push_TGINT();
	m_spi->CFG1 &= ~SPI_CFG1_RXDMAEN;
	m_rx_dma->Disable();
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_SPI::RX_DMA_Busy( void )
{
	return ( m_rx_dma->Busy() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Set_XChange_DMA( uint8_t* tx_data, uint8_t* rx_data, uint16_t length )
{
	Set_RX_DMA( rx_data, length );
	Set_TX_DMA( tx_data, length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Enable_XChange_DMA( void )
{
	Enable_RX_DMA();
	Enable_TX_DMA();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Disable_XChange_DMA( void )
{
	Push_TGINT();
	m_rx_dma->Disable();
	m_tx_dma->Disable();
	m_spi->CFG1 &= ~( SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN );
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_SPI::XChange_DMA_Busy( void )
{
	return ( m_rx_dma->Busy() );
}
