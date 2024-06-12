/**
 **********************************************************************************************
 * @file            uC_AtoD.cpp C++ Implementation File for AtoD software module.
 *
 * @brief           The file contains definitions of all the functions required for
 *                  AtoD conversion and which are declared in corresponding header file
 * @details         See header file for module overview.
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
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
#include "stm32wbxx_hal.h"
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
   The STM32WBxx uses ADC channel 17 for the temperature sensor
 */
#define ADC_TEMPERATURE_CHANNEL         17U

#define ADC_VREFINT_CHANNEL             0U
#define ADC_DELAY_INTERNAL_REGUL_STAB_US 40	// us
#define ADC_MAX_CLOCK_FREQUENCY                  64000000UL
#define ADC_CFGR_RES_6BIT                        ( ( uint32_t )0x00000018U )		/*! 6 bit
																					   Resolution */
#define ADC_CFGR_RES_8BIT                        ( ( uint32_t )0x00000010U )		/*! 8 bit
																					   Resolution  */
#define ADC_CFGR_RES_10BIT                       ( ( uint32_t )0x00000008U )		/*! 10 bit
																					   Resolution */
#define ADC_CFGR_RES_12BIT                       ( ( uint32_t )0x00000000U )		/*! 12 bit
																					   Resolution */

/* Maximum channels allowed for SMPR1 register to select sampling time */
#define SMPR1_MAX_CH_COUNT 10U
/* Number of sampling time selection bits for each channel */
#define SMPR_CH_BITS 3U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_AtoD::uC_AtoD( uC_ATOD_RESOLUTION_ENUM resolution, uint32_t active_channels_mask,
				  uC_BASE_ATOD_CTRL_ENUM atod_block,
				  uint16_t time_reqd_to_scan_all_channels_us ) :
	m_adc( nullptr ),
	m_resolution( 0U ),
	m_channel_buffer( nullptr ),
	m_dma_ctrl( nullptr ),
	m_channel_lookup( nullptr ),
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

	/* Disable the ADC (if not already disabled) */
	if ( ( m_adc->reg_ctrl->CR & ADC_CR_ADEN ) != 0U )
	{
		m_adc->reg_ctrl->CR &= ~ADC_CR_ADEN;
	}
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
	BF_ASSERT( ( m_adc->reg_ctrl->CR & ADC_CR_ADVREGEN ) != 0U );

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

			/* Select the sampling time. Some channels are not connected physically. Keep the corresponding SMPx[2:0]
			   setting to the reset value */
			if ( channel_counter < SMPR1_MAX_CH_COUNT )
			{
				m_adc->reg_ctrl->SMPR1 |= ( ADC_SAMPLE_TIME_VAL << ( channel_counter * SMPR_CH_BITS ) );
			}
			else
			{
				m_adc->reg_ctrl->SMPR2 |=
					( ADC_SAMPLE_TIME_VAL << ( ( channel_counter - ( SMPR1_MAX_CH_COUNT + 1U ) ) * SMPR_CH_BITS ) );
			}

			total_channels++;
		}
		channel_mask = channel_mask << 1U;
		channel_counter++;
	}
	// indicate to the ADC how many channels are being used.
	m_adc->reg_ctrl->SQR1 |= ( static_cast<uint32_t>( total_channels ) - 1U );

#if 0	// DMA is disabled, it will be fixed and enabled later in LTK-8345
	if ( m_adc->dma_channel != static_cast<uint8_t>( uC_DMA_CHANNEL_DNE ) )
	{
		// Allocate enough RAM for the number of channels being scanned and no more.
		m_channel_buffer =
			reinterpret_cast<uint16_t*>( Ram::Allocate( static_cast<size_t>( total_channels ) << MULT_BY_2 ) );

		m_dma_channel_mux.DMA_Channel = uC_DMA1_CHANNEL_2;
		m_dma_channel_mux.DMA_Requested_by = m_adc->dma_channel;

		m_dma_ctrl = new uC_DMA( &( m_dma_channel_mux ), uC_DMA_DIR_FROM_PERIPH,
								 static_cast<uint8_t>( sizeof ( m_channel_buffer[0] ) ),
								 uC_DMA_MEM_INC_TRUE,
								 uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_TRUE );

		m_dma_ctrl->Set_Buff( m_channel_buffer,
							  reinterpret_cast<volatile void*>( &m_adc->reg_ctrl->DR ),
							  total_channels );

		m_adc->reg_ctrl->CFGR |= ( ADC_CFGR_DMAEN | ADC_CFGR_DMACFG );

		m_dma_ctrl->Enable();
	}
	else
	{
		m_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
	}
#endif

	m_adc->reg_ctrl->CR |= ADC_CR_ADEN;

	/* Wait till ADC reaches a state where it is ready to accept conversion requests. */
	while ( m_adc->reg_ctrl->ISR & ( 1U << ADC_ISR_ADRDY ) )
	{}

	// Get one good data sample.
	Dump_AtoD_Data();
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

	if ( m_advanced_vect == nullptr )
	{
		m_advanced_vect = new Advanced_Vect();

		m_advanced_vect->Set_Vect( &Int_Handler_Static,
								   static_cast<Advanced_Vect::cback_param_t>( this ) );

		uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_adc->irq_num,
								  int_priority );
		m_adc->reg_ctrl->IER |= ADC_IER_EOCIE;
		m_adc->reg_ctrl->IER |= ADC_IER_OVRIE;
		Enable_Int();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Start_Scan( void ) const
{
	/* Perform ADC enable and conversion start if no conversion is on going */
	if ( ( m_adc->reg_ctrl->CR & ( 1U << ADC_CR_ADSTART ) ) == 0U )
	{
		/* Turn the ADC on. */
		m_adc->reg_ctrl->CR |= ADC_CR_ADEN;

		/* Clear ADC group regular conversion flag and overrun flag to ensure of no unknown state from potential
		   previous ADC operations
		   Note: bit cleared by writing 1 (writing 0 has no effect on any bit of register ISR) */
		m_adc->reg_ctrl->ISR = ( ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_OVR );

		/* Start the conversion of regular channels. */
		m_adc->reg_ctrl->CR |= ADC_CR_ADSTART;
	}
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

#if 0	// DMA is disabled, it will be fixed and enabled later in LTK8345
	return_val = m_channel_buffer[m_channel_lookup[channel]];
#endif
	return_val = m_adc->reg_ctrl->DR;
	return ( return_val );
}
