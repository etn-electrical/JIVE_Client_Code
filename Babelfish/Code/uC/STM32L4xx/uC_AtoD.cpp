/**
 **********************************************************************************************
 * @file            uC_AtoD.cpp C++ Implementation File for AtoD software module.
 *
 * @brief           The file contains definitions of all the functions required for
 *                  AtoD conversion and which are declared in corresponding header file
 * @details         See header file for module overview.
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
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
#include "uC_AtoD.h"
#include "uC_Base.h"
#include "Ram.h"
#include "uC_IO_Config.h"
#include "stm32l4xx_hal.h"
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
   The STM32F43x/42x uses ADC channel 18 for the temperature sensor and STM32F40x/41x uses ADC
      channel 16
 */
#define ADC_TEMPERATURE_CHANNEL         17U

#define ADC_VREFINT_CHANNEL             0U
#define ADC_DELAY_INTERNAL_REGUL_STAB_US 40	// us
// #define ADC_TEMPERATURE_SAMPLE_TIME     7U  //7 = 239.5 cycles => 17us (min sample time)
// #define ADC_VREFINT_SAMPLE_TIME         7U  //7 = 239.5 cycles => 17us (min sample time)
// #define ADC_SAMPLE_TIME_VAL             3U  //3 = 28.5 cycles.
#define ADC_MAX_CLOCK_FREQUENCY                  40000000UL
#define ADC_CFGR_RES_6BIT                        ( ( uint32_t )0x00000018U )		/*! 6 bit
																					   Resolution */
#define ADC_CFGR_RES_8BIT                        ( ( uint32_t )0x00000010U )		/*! 8 bit
																					   Resolution  */
#define ADC_CFGR_RES_10BIT                       ( ( uint32_t )0x00000008U )		/*! 10 bit
																					   Resolution */
#define ADC_CFGR_RES_12BIT                       ( ( uint32_t )0x00000000U )		/*! 12 bit
																					   Resolution */
// #define ADC_CLOCK_SYNC_PCLK_DIV1    ((uint32_t)((0x1U << ADC_CCR_CKMODE_Pos)))
// #define ADC_CLOCK_SYNC_PCLK_DIV2    ((uint32_t)((0x2U << ADC_CCR_CKMODE_Pos)))
// #define ADC_CLOCK_SYNC_PCLK_DIV4    ((uint32_t)((0x3U << ADC_CCR_CKMODE_Pos)))

#define Get_ADC_SMPR1_Setting( val )                                            \
	( static_cast<uint32_t>( ADC_VREFINT_SAMPLE_TIME ) | \
	  ( static_cast<uint32_t> \
		( val ) << 3U ) | \
	  ( static_cast<uint32_t>( val ) << 6U ) | ( static_cast<uint32_t>( val ) << 9U ) |     \
	  ( static_cast<uint32_t> \
		( val ) << 12U ) | \
	  ( static_cast<uint32_t> \
		( val ) << 15U ) | ( static_cast<uint32_t>( val ) << 18U ) |     \
	  ( static_cast<uint32_t> \
		( val ) << 21U ) | ( static_cast<uint32_t>( val ) << 24U ) | ( static_cast<uint32_t>( val ) << 27U ) )

#define Get_ADC_SMPR2_Setting( val )                                            \
	( ( val ) | \
	  ( static_cast<uint32_t> \
		( val ) << 3U ) | \
	  ( static_cast<uint32_t>( val ) << 6U ) | ( static_cast<uint32_t>( val ) << 9U ) |   \
	  ( static_cast<uint32_t> \
		( val ) << 12U ) | \
	  ( static_cast<uint32_t>( val ) << 15U ) | ( static_cast<uint32_t>( val ) << 18U ) |  \
	  ( static_cast<uint32_t> \
		( ADC_TEMPERATURE_SAMPLE_TIME ) << 21U ) | \
	  ( static_cast<uint32_t>( val ) << 24U ) )

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_AtoD::uC_AtoD( uC_ATOD_RESOLUTION_ENUM resolution, uint32_t active_channels_mask,
				  uC_BASE_ATOD_CTRL_ENUM atod_block,
				  uint16_t time_reqd_to_scan_all_channels_us ) :
	m_adc( reinterpret_cast<uC_BASE_ATOD_IO_STRUCT const*>( nullptr ) ),
	m_resolution( 0U ),
	m_channel_buffer( reinterpret_cast<uint16_t*>( nullptr ) ),
	m_dma_ctrl( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_channel_lookup( reinterpret_cast<uint8_t*>( nullptr ) )
{
	uint8_t channel_counter;
	uint16_t total_channels;
	uint32_t channel_mask;
	ADC_InitTypeDef ADCClkInit;
	__IO uint32_t wait_loop_index = 0U;

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not
	 * enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	m_adc = ( uC_Base::Self() )->Get_AtoD_Ctrl( static_cast<uint32_t>( atod_block ) );

	BF_ASSERT( ( m_adc != 0U ) && ( active_channels_mask != 0U ) );
	uC_Base::Reset_Periph( &m_adc->periph_def );
	if ( ( AHB2_PERIPH_CLOCK_FREQ ) > ADC_MAX_CLOCK_FREQUENCY )
	{
		if ( ( AHB2_PERIPH_CLOCK_FREQ / 2 ) > ADC_MAX_CLOCK_FREQUENCY )
		{
			ADCClkInit.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
		}
		else
		{
			ADCClkInit.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
		}
	}
	else
	{
		ADCClkInit.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
	}
	uC_Base::Enable_Periph_Clock( &m_adc->periph_def );
	ADC1_COMMON->CCR |= ADCClkInit.ClockPrescaler;

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers,
	 * not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	m_resolution = static_cast<uint8_t>( resolution );

	m_adc->reg_ctrl->CR &= ~ADC_CR_DEEPPWD;
	m_adc->reg_ctrl->CR |= ADC_CR_ADVREGEN;	// enable ADC voltage regulator before enable ADC
	uC_Delay( ADC_VOLTAGE_REGULATOR_START_TIME_uS )
	// calibration
	m_adc->reg_ctrl->CR &= ~ADC_CR_ADCALDIF;
	m_adc->reg_ctrl->CR |= ADC_CR_ADCAL;
	while ( m_adc->reg_ctrl->CR & ADC_CR_ADCAL != 0 )
	{}
	uC_Delay( ADC_STABILIZATION_TIME_uS )
	m_adc->reg_ctrl->CR |= ADC_CR_ADEN;	// Initial turn on.  Requires some stabilization time
										// defined by tstab.

	uC_Delay( ADC_STABILIZATION_TIME_uS )

	m_adc->reg_ctrl->CR &= ~ADC_CR_ADEN;	// Turn off the ADC.

	m_adc->reg_ctrl->CFGR &= ~ADC_CFGR_ALIGN;	// right Alignment
	// setting the resolution
	if ( resolution == uC_ATOD_6BIT_RESOLUTION )
	{
		m_adc->reg_ctrl->CFGR |= ADC_CFGR_RES_6BIT;
	}
	else if ( resolution == uC_ATOD_8BIT_RESOLUTION )
	{
		m_adc->reg_ctrl->CFGR |= ADC_CFGR_RES_8BIT;
	}
	else if ( resolution == uC_ATOD_10BIT_RESOLUTION )
	{
		m_adc->reg_ctrl->CFGR |= ADC_CFGR_RES_10BIT;
	}
	else
	{
		m_adc->reg_ctrl->CFGR |= ADC_CFGR_RES_12BIT;
		m_adc->reg_ctrl->CFGR |= ADC_CFGR_ALIGN;// Left Alignment for 12bit resolution to make it
												// appear 16bit regardless of actual bit resolution
	}

	m_adc->reg_ctrl->SMPR1 = Get_ADC_SMPR1_Setting( ADC_SAMPLE_TIME_VAL );
	m_adc->reg_ctrl->SMPR2 = Get_ADC_SMPR2_Setting( ADC_SAMPLE_TIME_VAL );

	// This is to provide a reverse lookup for when someone wants to reference the adc value by
	// channel id.
	// The DMA loads up the buffer with the data based on how many channels.  We need to
	// reference this
	// to make the channel get go faster.
	m_channel_lookup = reinterpret_cast<uint8_t*>( Ram::Allocate( m_adc->max_channels ) );

	channel_mask = 1U;
	total_channels = 0U;
	channel_counter = 0U;
	while ( channel_counter < static_cast<uint8_t>( uC_ATOD_CHANNEL_MAX ) )
	{
		if ( ( channel_mask & active_channels_mask ) != 0U )
		{
			if ( total_channels < 4U )
			{
				m_adc->reg_ctrl->SQR1 |= ( static_cast<uint32_t>( channel_counter )
										   << ( 6U * total_channels + 6U ) );
			}
			else if ( total_channels < 9U )
			{
				m_adc->reg_ctrl->SQR2 |= ( static_cast<uint32_t>( channel_counter )
										   << ( 6U * ( total_channels - 4U ) ) );
			}
			else if ( total_channels < 14U )
			{
				m_adc->reg_ctrl->SQR3 |= ( static_cast<uint32_t>( channel_counter )
										   << ( 6U * ( total_channels - 9U ) ) );
			}
			else
			{
				m_adc->reg_ctrl->SQR4 |= ( static_cast<uint32_t>( channel_counter )
										   << ( 6U * ( total_channels - 14U ) ) );
			}
			if ( channel_counter == ADC_TEMPERATURE_CHANNEL )
			{
				ADC1_COMMON->CCR |= ADC_CCR_TSEN;
			}
			else if ( channel_counter == ADC_VREFINT_CHANNEL )
			{
				ADC1_COMMON->CCR |= ADC_CCR_VREFEN;
			}
			uC_IO_Config::Enable_Periph_Input_Pin( m_adc->adc_pio[channel_counter] );
			m_channel_lookup[channel_counter] = static_cast<uint8_t>( total_channels );

			total_channels++;
		}
		channel_mask = channel_mask << 1U;
		channel_counter++;
	}
	// indicate to the ADC how many channels are being used.
	m_adc->reg_ctrl->SQR1 |= ( static_cast<uint32_t>( total_channels ) - 1U );

	// Allocate enough RAM for the number of channels being scanned and no more.
	m_channel_buffer =
		reinterpret_cast<uint16_t*>( Ram::Allocate( static_cast<size_t>( total_channels ) << MULT_BY_2 ) );

	if ( m_adc->dma_channel != static_cast<uint8_t>( uC_DMA_CHANNEL_DNE ) )
	{
		m_dma_ctrl = new uC_DMA( m_adc->dma_channel, uC_DMA_DIR_FROM_PERIPH,
								 static_cast<uint8_t>( sizeof ( m_channel_buffer[0] ) ),
								 uC_DMA_MEM_INC_TRUE,
								 uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_TRUE );

		m_dma_ctrl->Set_Buff( m_channel_buffer,
							  reinterpret_cast<volatile void*>( &m_adc->reg_ctrl->DR ),
							  total_channels );

		m_adc->reg_ctrl->CFGR |= ( ADC_CFGR_DMAEN | ADC_CFGR_DMACFG );
		// m_adc->reg_ctrl->CR1 |= ADC_CR1_SCAN; //I can't find the corresponding register in L451

		m_dma_ctrl->Enable();
	}
	else
	{
		m_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
	}

	// Get one good data sample.
	Dump_AtoD_Data();
	// Start_Scan();
	BF_ASSERT( time_reqd_to_scan_all_channels_us != 0U );
	uC_Delay( time_reqd_to_scan_all_channels_us );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Start_Scan( void ) const
{
	// Turn the ADC on.
	m_adc->reg_ctrl->CR |= ADC_CR_ADEN;

	// Start the conversion of regular channels.
	m_adc->reg_ctrl->CR |= ADC_CR_ADSTART;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_AtoD::Scan_Done( void ) const
{
	return ( ( m_adc->reg_ctrl->ISR & ADC_ISR_EOC ) != 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Dump_AtoD_Data( void ) const
{
	volatile uint32_t temp_dump;

	temp_dump = m_adc->reg_ctrl->DR;
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-4: Symbol 'temp_dump' not accessed
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
void uC_AtoD::Start_Repeat_Scan( void ) const
{
	// Set the Continuous conversion bit so that conversions will take place continuously.
	m_adc->reg_ctrl->CFGR |= ADC_CFGR_CONT;

	Start_Scan();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t uC_AtoD::Get_Channel( uint8_t channel ) const
{
	uint16_t return_val;

	return_val = m_channel_buffer[m_channel_lookup[channel]];

	return ( return_val );
}

/* lint +e1924
   lint +e1960*/
