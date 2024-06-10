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
#include "stm32f7xx_hal.h"
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


#define ADC_TEMPERATURE_CHANNEL         18U
#define ADC_VREFINT_CHANNEL             17U

#define ADC_MAX_CLOCK_FREQUENCY                  18000000UL
#define ADC_CR1_RES_6BIT                        ( ( uint32_t )0x03000000U )			/*! 6 bit
																					   Resolution */
#define ADC_CR1_RES_8BIT                        ( ( uint32_t )0x02000000U )			/*! 8 bit
																					   Resolution  */
#define ADC_CR1_RES_10BIT                       ( ( uint32_t )0x01000000U )			/*! 10 bit
																					   Resolution */
#define ADC_CR1_RES_12BIT                       ( ( uint32_t )0x00000000U )			/*! 12 bit
																					   Resolution */

#define Get_ADC_SMPR1_Setting( val )                                            \
	( ( val ) | \
	  ( static_cast<uint32_t> \
		( val ) << 3U ) | \
	  ( static_cast<uint32_t>( val ) << 6U ) | ( static_cast<uint32_t>( val ) << 9U ) |     \
	  ( static_cast<uint32_t> \
		( val ) << 12U ) | \
	  ( static_cast<uint32_t> \
		( val ) << 15U ) | ( static_cast<uint32_t>( val ) << 18U ) |     \
	  ( static_cast<uint32_t> \
		( ADC_VREFINT_SAMPLE_TIME ) << 21U ) | \
	  ( static_cast<uint32_t>( ADC_TEMPERATURE_SAMPLE_TIME ) << 24U ) )

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
	m_channel_lookup( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_advanced_vect( nullptr ),
	m_cback_param( nullptr ),
	m_cback_func( nullptr )
{
	uint8_t channel_counter;
	uint16_t total_channels;
	uint32_t channel_mask;
	ADC_InitTypeDef ADCClkInit;

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
	if ( ( APB2_PERIPH_CLOCK_FREQ / 2 ) > ADC_MAX_CLOCK_FREQUENCY )
	{
		if ( ( APB2_PERIPH_CLOCK_FREQ / 4 ) > ADC_MAX_CLOCK_FREQUENCY )
		{
			if ( ( APB2_PERIPH_CLOCK_FREQ / 6 ) > ADC_MAX_CLOCK_FREQUENCY )
			{
				ADCClkInit.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
			}
			else
			{
				ADCClkInit.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV6;
			}
		}
		else
		{
			ADCClkInit.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
		}
	}
	else
	{
		ADCClkInit.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	}
	uC_Base::Enable_Periph_Clock( &m_adc->periph_def );
	ADC->CCR |= ADCClkInit.ClockPrescaler;

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

	m_adc->reg_ctrl->CR2 &= ~ADC_CR2_ALIGN;	// right Alignment
	// setting the resolution
	if ( resolution == uC_ATOD_6BIT_RESOLUTION )
	{
		m_adc->reg_ctrl->CR1 |= ADC_CR1_RES_6BIT;
	}
	else if ( resolution == uC_ATOD_8BIT_RESOLUTION )
	{
		m_adc->reg_ctrl->CR1 |= ADC_CR1_RES_8BIT;
	}
	else if ( resolution == uC_ATOD_10BIT_RESOLUTION )
	{
		m_adc->reg_ctrl->CR1 |= ADC_CR1_RES_10BIT;
	}
	else
	{
		m_adc->reg_ctrl->CR1 |= ADC_CR1_RES_12BIT;
		m_adc->reg_ctrl->CR2 |= ADC_CR2_ALIGN;	// Left Alignment for 12bit resolution to make it
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
			if ( ( channel_counter == ADC_TEMPERATURE_CHANNEL ) ||
				 ( channel_counter == ADC_VREFINT_CHANNEL ) )
			{
				ADC->CCR |= ADC_CCR_TSVREFE;
			}
			uC_IO_Config::Enable_Periph_Input_Pin( m_adc->adc_pio[channel_counter] );
			m_channel_lookup[channel_counter] = static_cast<uint8_t>( total_channels );

			total_channels++;
		}
		channel_mask = channel_mask << 1U;
		channel_counter++;
	}
	// The magical number of 20 is the shift necessary to indicate to the ADC
	// how many channels are being used.  The location is 20bits up in the
	// register.
	m_adc->reg_ctrl->SQR1 |= ( static_cast<uint32_t>( total_channels ) - 1U ) << 20U;

	// Allocate enough RAM for the number of channels being scanned and no more.
	m_channel_buffer = reinterpret_cast<uint16_t*>( Ram::Allocate(
														static_cast<size_t>
														( total_channels ) << MULT_BY_2 ) );

	if ( m_adc->dma_channel != static_cast<uint8_t>( uC_DMA_STREAM_DNE ) )
	{
		m_dma_ctrl = new uC_DMA( m_adc->dma_channel, uC_DMA_DIR_FROM_PERIPH,
								 static_cast<uint8_t>( sizeof ( m_channel_buffer[0] ) ),
								 uC_DMA_MEM_INC_TRUE,
								 uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_TRUE );

		m_dma_ctrl->Set_Buff( m_channel_buffer,
							  reinterpret_cast<volatile void*>( &m_adc->reg_ctrl->DR ),
							  total_channels );

		m_adc->reg_ctrl->CR2 |= ( ADC_CR2_DMA | ADC_CR2_DDS );
		m_adc->reg_ctrl->CR1 |= ADC_CR1_SCAN;

		m_dma_ctrl->Enable();
	}
	else
	{
		m_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
	}
	/* End of Conversion selection
	   0 - The EOC bit is set at the end of each sequence of regular conversions
	   1 - The EOC bit is set at the end of each regular conversion
	 */
	m_adc->reg_ctrl->CR1 |= ADC_EACH_REGULAR_CONVERSION;
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
void uC_AtoD::Int_Handler( void )
{
	( *m_cback_func )( m_cback_param );
}

/*******************************************************************************
 *  @fn         Set_Int_Vect
 *  @brief      Configure the ADC interrupt priority.
 *              Declare and pass the call back function and Call back parameter
 *
 *  @param[in]  int_handler => Call back function to be used
 *              param => Call back parameter to be used
 *              priority => priority of the USART interrrupt
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_AtoD::Set_Callback( INT_CALLBACK_FUNC* int_callback, uint8_t int_priority ) const
{
	uC_Interrupt::Set_Vector( int_callback, m_adc->irq_num, int_priority );
	m_adc->reg_ctrl->CR1 |= ADC_CR1_EOCIE;
	m_adc->reg_ctrl->CR1 |= ADC_CR1_OVRIE;
	uC_Interrupt::Enable_Int( m_adc->irq_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
							   uint8_t int_priority )
{
	m_cback_param = func_param;
	m_cback_func = func_callback;

	if ( m_advanced_vect == NULL )
	{
		m_advanced_vect = new Advanced_Vect();

		m_advanced_vect->Set_Vect( &Int_Handler_Static,
								   static_cast<Advanced_Vect::cback_param_t>( this ) );

		uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_adc->irq_num,
								  int_priority );
		m_adc->reg_ctrl->CR1 |= ADC_CR1_EOCIE;
		m_adc->reg_ctrl->CR1 |= ADC_CR1_OVRIE;
		uC_Interrupt::Enable_Int( m_adc->irq_num );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Start_Scan( void ) const
{
	// Set the Continuous conversion bit so that conversions will take place continuously.
	m_adc->reg_ctrl->CR2 &= ~( ADC_CR2_CONT );
	// Turn the ADC on.
	m_adc->reg_ctrl->CR2 |= ADC_CR2_ADON;
	// Start the conversion of regular channels.
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
	// Set the Continuous conversion bit so that conversions will take place continuously.
	m_adc->reg_ctrl->CR2 |= ADC_CR2_CONT;

	// Turn the ADC on.
	m_adc->reg_ctrl->CR2 |= ADC_CR2_ADON;

	// Start the conversion of regular channels.
	m_adc->reg_ctrl->CR2 |= ADC_CR2_SWSTART;
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
