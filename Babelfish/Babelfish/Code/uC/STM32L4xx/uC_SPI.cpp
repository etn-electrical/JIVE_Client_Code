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
#include "Exception.h"
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
	m_tx_dma( reinterpret_cast<uC_DMA*>( nullptr ) )
{
	m_config = ( uC_Base::Self() )->Get_SPI_Ctrl( spi_select );
	BF_ASSERT( m_config != reinterpret_cast<uC_BASE_SPI_IO_STRUCT const*>( nullptr ) );
	m_spi = m_config->reg_ctrl;

	uC_Base::Reset_Periph( &m_config->periph_def );
	uC_Base::Enable_Periph_Clock( &m_config->periph_def );

	if ( master == true )
	{
		m_spi->CR1 = ( SPI_CR1_MSTR );
		m_spi->CR2 = ( SPI_CR2_SSOE );
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

	if ( ( m_config->rx_dma_channel != static_cast<uint8_t>( uC_DMA_CHANNEL_DNE ) ) &&
		 ( m_config->tx_dma_channel != static_cast<uint8_t>( uC_DMA_CHANNEL_DNE ) ) &&
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

		m_spi->CR2 |= SPI_CR2_TXDMAEN;
		m_spi->CR2 |= SPI_CR2_RXDMAEN;
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
	m_chip_sel_cfg[chip_select].cr1 = ( m_spi->CR1
										& static_cast<uint16_t>
										( ~( static_cast<uint16_t>( SPI_CR1_SPE ) ) ) );
	m_chip_sel_cfg[chip_select].cr2 = m_spi->CR2;

	switch ( spi_mode )
	{
		case uC_SPI_MODE_0:
			break;

		case uC_SPI_MODE_1:
			m_chip_sel_cfg[chip_select].cr1 |= static_cast<uint16_t>( SPI_CR1_CPHA );
			break;

		case uC_SPI_MODE_2:
			m_chip_sel_cfg[chip_select].cr1 |= static_cast<uint16_t>( SPI_CR1_CPOL );
			break;

		case uC_SPI_MODE_3:
			m_chip_sel_cfg[chip_select].cr1 |= static_cast<uint16_t>( SPI_CR1_CPOL
																	  | SPI_CR1_CPHA );
			break;

		default:
			break;
	}

	m_chip_sel_cfg[chip_select].manual_pin_ctrl = manual_chip_select;

	BF_ASSERT( ( bits_to_transfer > 3U ) && ( bits_to_transfer <= 16U ) );
	bits_to_transfer = bits_to_transfer - 1U;
	m_chip_sel_cfg[chip_select].cr2 &= ~SPI_CR2_DS_Msk;
	m_chip_sel_cfg[chip_select].cr2 |=
		( static_cast<uint16_t>( bits_to_transfer ) << SPI_CR2_DS_Pos );
	if ( bits_to_transfer > 8U )
	{
		m_chip_sel_cfg[chip_select].cr2 &= ~( SPI_CR2_FRXTH );
	}
	else
	{
		m_chip_sel_cfg[chip_select].cr2 |= SPI_CR2_FRXTH;
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
	m_chip_sel_cfg[chip_select].cr1 |= static_cast<uint16_t>( SPI_CR1_BR
															  & ( clock_div_counter << 3U ) );

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
	uint16_t temp_cr1;
	GINT_TDEF temp_gint;

	Push_GINT( temp_gint );


	// Push the SPI enable register so we can choose to properly enable it
	temp_cr1 = static_cast<uint16_t>( m_spi->CR1 & static_cast<uint32_t>( SPI_CR1_SPE ) );
	m_spi->CR1 &= ~( SPI_CR1_SPE );			// Disable the SPI because some of these changes can do
											// damage.

	m_spi->CR1 = m_chip_sel_cfg[chip_select].cr1;
	m_spi->CR2 = m_chip_sel_cfg[chip_select].cr2;

	m_spi->CR1 |= temp_cr1;		// Re-enable the SPI if it was enabled before all this config mess.

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
	m_tx_dma->Set_Buff( data, &m_spi->DR, length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Set_RX_DMA( uint8_t* data, uint16_t length )
{
	m_rx_dma->Disable();
	m_rx_dma->Set_Buff( data, &m_spi->DR, length );
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

#if 0
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define MAX_SPI_CLOCK_DIVISOR                       ( 0xFF )

#define uC_SPI_SLAVE_CHIP_SELECT_CHANNEL        ( 0 )

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_SPI::uC_SPI( uint8_t spi_select, bool loopback, bool master )
{
	m_config = ( uC_Base::Self() )->Get_SPI_Ctrl( spi_select );
	BF_ASSERT( m_config != 0 );
	m_spi = m_config->reg_ctrl;

	uC_Base::Enable_PMC( m_config->pid );

	m_spi->SPI_CR = AT91C_SPI_SWRST;
	m_spi->SPI_IDR = USINT32_BIT_MASK;
	if ( master == true )
	{
		m_spi->SPI_MR = ( AT91C_SPI_MSTR | AT91C_SPI_MODFDIS );
	}

	if ( loopback == true )
	{
		m_spi->SPI_MR |= AT91C_SPI_LLB;
	}

	m_semaphore = new OS_Semaphore( true );

	// m_semaphore->Post();

	Enable();
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
	volatile uint32_t dump_reg;

	BF_ASSERT( chip_select < m_config->slave_select_pin_count );
	Set_SPI_CSR_Bits( spi_mode, bits_to_transfer, chip_select, manual_chip_select );
	Set_SPI_Clock( desired_spi_clock_freq, chip_select );

	Activate_SPI_IO( chip_select, ( ( m_spi->SPI_MR & AT91C_SPI_MSTR ) != 0 ),
					 manual_chip_select );

	while ( m_spi->SPI_SR & AT91C_SPI_RDRF )
	{
		dump_reg = m_spi->SPI_RDR;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Exchange_Data( uint8_t* tx_data, uint8_t* rx_data, uint16_t length )
{
	m_spi->SPI_RNPR = 0;
	m_spi->SPI_TNPR = 0;
	m_spi->SPI_RNCR = 0;
	m_spi->SPI_TNCR = 0;

	m_spi->SPI_RPR = ( uint32_t )rx_data;
	m_spi->SPI_TPR = ( uint32_t )tx_data;
	m_spi->SPI_RCR = length;
	m_spi->SPI_TCR = length;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_SPI::Set_Int_Vect( INT_CALLBACK_FUNC* int_handler, uint8_t priority )
{
	return ( uC_Interrupt::Set_Vector( int_handler, m_config->pid,
									   priority, uC_INT_PERIPH_HIGH_LEVEL ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_SPI::Set_OS_Int_Vect( OS_INT_CALLBACK_FUNC* int_handler, uint8_t priority )
{
	return ( uC_OS_Interrupt::Set_OS_Vect( int_handler, m_config->pid,
										   priority, uC_INT_PERIPH_HIGH_LEVEL ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Set_SPI_Clock( uint32_t desired_spi_clock_freq, uint8_t channel )
{
	uint32_t temp_clock_div;

	temp_clock_div = MASTER_CLOCK_FREQUENCY / desired_spi_clock_freq;
	if ( temp_clock_div > MAX_SPI_CLOCK_DIVISOR )
	{
		temp_clock_div = MAX_SPI_CLOCK_DIVISOR;
	}

	BF_ASSERT( temp_clock_div <= MAX_SPI_CLOCK_DIVISOR );
	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to
	 * underlying type for sub-integers
	 *@n Justification: Generic #define is used here, recasting in that #define would have
	 * adverse effects throughout.
	 */
	/*lint -e{1960} # MISRA Deviation */
	Set_Mask( m_spi->SPI_CSR[channel], AT91C_SPI_SCBR, ( temp_clock_div << 8 ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Set_TX_DMA( uint8_t* data, uint16_t length )
{
	m_spi->SPI_TPR = ( uint32_t )data;
	m_spi->SPI_TCR = length;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SPI::Set_RX_DMA( uint8_t* data, uint16_t length )
{
	m_spi->SPI_RPR = ( uint32_t )data;
	m_spi->SPI_RCR = length;
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************

   void uC_SPI::TX_Byte( uint8_t data )
   {
   m_spi->SPI_TDR = data;
   }
 */

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************

   uint8_t uC_SPI::RX_Byte( void )
   {
   m_spi->SPI_TDR = 0;
   while ( !RX_Data_Reg_Full() )
   {};
   return ( m_spi->SPI_RDR );
   }
 */

#endif
