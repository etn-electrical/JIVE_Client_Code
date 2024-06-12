/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_AtoD.h"
#include "uC_Base.h"
#include "Ram.h"
#include "uC_IO_Config.h"
#include "stm32f1xx_hal.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define ADC_STABILIZATION_TIME_uS       5U

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
#define ADC_TEMPERATURE_CHANNEL         16U
#define ADC_VREFINT_CHANNEL             17U
#define ADC_TEMPERATURE_SAMPLE_TIME     7U	// 7 = 239.5 cycles => 17us (min sample time)
#define ADC_VREFINT_SAMPLE_TIME         7U	// 7 = 239.5 cycles => 17us (min sample time)
#define ADC_SAMPLE_TIME_VAL             3U	// 3 = 28.5 cycles.
#define Get_ADC_SMPR1_Setting( val )                                            \
	( ( val ) | \
	  ( static_cast<uint32_t> \
		( val ) << 3 ) | \
	  ( static_cast<uint32_t>( val ) << 6 ) | ( static_cast<uint32_t>( val ) << 9 ) |  \
	  ( static_cast<uint32_t>( val ) << 12 ) | ( static_cast<uint32_t>( val ) << 15 ) | \
	  ( static_cast<uint32_t> \
		( ADC_TEMPERATURE_SAMPLE_TIME ) << 18 ) | \
	  ( static_cast<uint32_t>( ADC_VREFINT_SAMPLE_TIME ) << 21 ) )
#define Get_ADC_SMPR2_Setting( val )                                            \
	( ( val ) | \
	  ( static_cast<uint32_t> \
		( val ) << 3U ) | \
	  ( static_cast<uint32_t>( val ) << 6U ) | ( static_cast<uint32_t>( val ) << 9U ) |   \
	  ( static_cast<uint32_t> \
		( val ) << 12U ) | \
	  ( static_cast<uint32_t>( val ) << 15U ) | ( static_cast<uint32_t>( val ) << 18U ) |  \
	  ( static_cast<uint32_t> \
		( val ) << 21U ) | \
	  ( static_cast<uint32_t>( val ) << 24U ) | ( static_cast<uint32_t>( val ) << 27U ) )

#define ADC_MAX_CLOCK_FREQUENCY         14000000UL
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_AtoD::uC_AtoD( uC_ATOD_RESOLUTION_ENUM resolution, uint32_t active_channels_mask,
				  uC_BASE_ATOD_CTRL_ENUM atod_block ) :
	m_adc( reinterpret_cast<uC_BASE_ATOD_IO_STRUCT const*>( nullptr ) ),
	m_resolution( 0U ),
	m_channel_buffer( reinterpret_cast<uint16_t*>( nullptr ) ),
	m_dma_ctrl( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_channel_lookup( reinterpret_cast<uint8_t*>( nullptr ) )
{
	uint8_t channel_counter;
	uint16_t total_channels;
	uint32_t channel_mask;

	RCC_PeriphCLKInitTypeDef PeriphClkInit;

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
	if ( APB2_PERIPH_CLOCK_FREQ / 2 > ADC_MAX_CLOCK_FREQUENCY )
	{
		if ( APB2_PERIPH_CLOCK_FREQ / 4 > ADC_MAX_CLOCK_FREQUENCY )
		{
			if ( APB2_PERIPH_CLOCK_FREQ / 6 > ADC_MAX_CLOCK_FREQUENCY )
			{
				PeriphClkInit.AdcClockSelection = RCC_CFGR_ADCPRE_DIV8;
			}
			else
			{
				PeriphClkInit.AdcClockSelection = RCC_CFGR_ADCPRE_DIV6;
			}
		}
		else
		{
			PeriphClkInit.AdcClockSelection = RCC_CFGR_ADCPRE_DIV4;
		}
	}
	else
	{
		PeriphClkInit.AdcClockSelection = RCC_CFGR_ADCPRE_DIV2;
	}
	// RCC->CFGR |= PeriphClkInit.AdcClockSelection<<4;
	__HAL_RCC_ADC_CONFIG( PeriphClkInit.AdcClockSelection );
	uC_Base::Enable_Periph_Clock( &m_adc->periph_def );

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers,
	 * not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	m_resolution = static_cast<uint8_t>( resolution );

	m_adc->reg_ctrl->CR2 = ADC_CR2_ADON;// Initial turn on.  Requires some stabilization time
										// defined by tstab.

	uC_Delay( ADC_STABILIZATION_TIME_uS )

	m_adc->reg_ctrl->CR2 &= ~ADC_CR2_ADON;	// Turn off the ADC.

	m_adc->reg_ctrl->CR2 = ADC_CR2_CAL;		// Begin a calibration cycle.
	m_adc->reg_ctrl->CR2 |= ADC_CR2_ADON;	// Run the calibration.
	while ( ( m_adc->reg_ctrl->CR2 & ADC_CR2_CAL ) != 0U )
	{}
	m_adc->reg_ctrl->CR2 &= ~ADC_CR2_ADON;	// Turn off the ADC.

	m_adc->reg_ctrl->CR2 |= ADC_CR2_ALIGN;	// left Alignment for 12 bit resolution
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
			if ( total_channels < 6U )
			{
				m_adc->reg_ctrl->SQR3 |= ( static_cast<uint32_t>( channel_counter )
										   << ( 5U * total_channels ) );
			}
			else if ( total_channels < 12U )
			{
				m_adc->reg_ctrl->SQR2 |= ( static_cast<uint32_t>( channel_counter )
										   << ( 5U * ( total_channels - 6U ) ) );
			}
			else
			{
				m_adc->reg_ctrl->SQR1 |= ( static_cast<uint32_t>( channel_counter )
										   << ( 5U * ( total_channels - 12U ) ) );
			}
			if ( channel_counter == ADC_TEMPERATURE_CHANNEL )
			{
				m_adc->reg_ctrl->CR2 |= ADC_CR2_TSVREFE;
			}
			uC_IO_Config::Enable_Periph_Input_Pin( m_adc->adc_pio[channel_counter] );
			m_channel_lookup[channel_counter] = static_cast<uint8_t>( total_channels );

			total_channels++;
		}
		channel_mask = channel_mask << 1U;
		channel_counter++;
	}
	m_adc->reg_ctrl->SQR1 |= ( static_cast<uint32_t>( total_channels ) - 1U ) << 20U;	// This
																						// magical
																						// number
																						// could
																						// do
																						// with
																						// some
																						// refinement.

	m_channel_buffer = reinterpret_cast<uint16_t*>( Ram::Allocate(
														static_cast<size_t>
														( total_channels ) << MULT_BY_2 ) );

	if ( m_adc->dma_channel != static_cast<uint8_t>( uC_DMA_CHANNEL_DNE ) )
	{
		m_dma_ctrl = new uC_DMA( m_adc->dma_channel, uC_DMA_DIR_FROM_PERIPH,
								 static_cast<uint8_t>( sizeof ( m_channel_buffer[0] ) ),
								 uC_DMA_MEM_INC_TRUE,
								 uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_TRUE );

		m_dma_ctrl->Set_Buff( m_channel_buffer, &m_adc->reg_ctrl->DR, total_channels );

		m_adc->reg_ctrl->CR2 |= ADC_CR2_DMA;
		m_adc->reg_ctrl->CR1 |= ADC_CR1_SCAN;

		m_dma_ctrl->Enable();
	}
	else
	{
		m_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
	}

	// Get one good data sample.
	Dump_AtoD_Data();
	Start_Scan();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Start_Scan( void ) const
{
	m_adc->reg_ctrl->CR2 |= ADC_CR2_ADON;
	m_adc->reg_ctrl->CR2 |= ADC_CR2_SWSTART;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_AtoD::Scan_Done( void ) const
{
	return ( ( m_adc->reg_ctrl->SR & ADC_SR_EOC ) != 0U );
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
	m_adc->reg_ctrl->CR2 |= ADC_CR2_CONT;
	m_adc->reg_ctrl->CR2 |= ADC_CR2_ADON;
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
