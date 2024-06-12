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
#include "stm32f0xx_hal.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define ADC_STABILIZATION_TIME_uS       5U
// void Callback_Interupt( void );
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
#define ADC_TEMPERATURE_CHANNEL         16U
#define ADC_VREFINT_CHANNEL             17U
#define ADC_TEMPERATURE_SAMPLE_TIME     7U	// 7 = 239.5 cycles => 17us (min sample time)
#define ADC_VREFINT_SAMPLE_TIME         7U	// 7 = 239.5 cycles => 17us (min sample time)

#define Get_ADC_SMPR_Setting( val )                                         \
	( ( val ) | \
	  ( static_cast<uint32_t>( val ) << 3 ) | ( static_cast<uint32_t>( val ) << 6 ) | \
	  ( static_cast<uint32_t>( val ) << 9 ) |  \
	  ( static_cast<uint32_t>( val ) << 12 ) | ( static_cast<uint32_t>( val ) << 15 ) | \
	  ( static_cast<uint32_t>( ADC_TEMPERATURE_SAMPLE_TIME ) << 18 ) | \
	  ( static_cast<uint32_t>( ADC_VREFINT_SAMPLE_TIME ) << 21 ) )
#define ADC_RES_6BIT                        ( ( uint32_t )0x00000018U )			/*! 6 bit Resolution
																				 */
#define ADC_RES_8BIT                        ( ( uint32_t )0x00000010U )			/*! 8 bit Resolution
																				 */
#define ADC_RES_10BIT                       ( ( uint32_t )0x00000008U )			/*! 10 bit Resolution
																				 */
#define ADC_RES_12BIT                       ( ( uint32_t )0x00000000U )			/*! 12 bit Resolution
																				 */

// extern void (*m_ram_vector[])();
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
	m_channel_lookup( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_advanced_vect( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_cback_param( nullptr ),
	m_cback_func( nullptr )
{
	uint8_t channel_counter;
	uint16_t total_channels;
	uint32_t channel_mask;

	m_adc = ( uC_Base::Self() )->Get_AtoD_Ctrl( static_cast<uint32_t>( atod_block ) );

	BF_ASSERT( ( m_adc != 0U ) && ( active_channels_mask != 0U ) );

	/*Clock selection*/
	uC_Base::Reset_Periph( &m_adc->periph_def );
	uC_Base::Enable_Periph_Clock( &m_adc->periph_def );
	if ( ADC_CLOCK_MODE_SYNC_ASYNC == 1 )
	{
		if ( ( APB2_PERIPH_CLOCK_FREQ / 2 ) > ADC_PERIPH_CLOCK_MAX_FREQ )
		{
			m_adc->reg_ctrl->CFGR2 |= ADC_CFGR2_CKMODE_1;
		}
		else
		{
			m_adc->reg_ctrl->CFGR2 |= ADC_CFGR2_CKMODE_0;
		}
	}
	else
	{
		m_adc->reg_ctrl->CFGR2 &= ~( ADC_CFGR2_CKMODE );
	}

	/*Resolution and Data Alignment*/
	m_resolution = static_cast<uint8_t>( resolution );
	Set_AtoD_Resolution( m_adc, resolution );

	/*Sample time selection*/
	m_adc->reg_ctrl->SMPR |= ADC_SAMPLE_TIME_VAL;

	/*Intial Turn on*/
	m_adc->reg_ctrl->CR |= ADC_CR_ADEN;	// Initial turn on.  Requires some stabilization time
										// defined by tstab.
	while ( ( m_adc->reg_ctrl->ISR & ADC_ISR_ADRDY ) == 0 )
	{}

	/* Calibration sequence */
	m_adc->reg_ctrl->CR |= ADC_CR_ADCAL;	// Begin a calibration cycle.
	m_adc->reg_ctrl->CR |= ADC_CR_ADSTART;	// Run the calibration
	while ( ( m_adc->reg_ctrl->CR & ADC_CR_ADCAL ) != 0U )
	{}
	if ( ( m_adc->reg_ctrl->CR & ADC_CR_ADSTART ) != 0U )
	{
		m_adc->reg_ctrl->CR |= ADC_CR_ADSTP;
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

	/* channel selection */
	while ( channel_counter < static_cast<uint8_t>( uC_ATOD_CHANNEL_MAX ) )
	{
		if ( ( channel_mask & active_channels_mask ) != 0U )
		{

			m_adc->reg_ctrl->CHSELR |= ( 1 << channel_counter );

			if ( channel_counter == ADC_TEMPERATURE_CHANNEL )
			{
				ADC1_COMMON->CCR |= ADC_CCR_TSEN;
			}
			if ( channel_counter == ADC_VREFINT_CHANNEL )
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

	/* DMA based ADC */
	m_channel_buffer = reinterpret_cast<uint16_t*>( Ram::Allocate(
														static_cast<size_t>
														( total_channels ) <<
															MULT_BY_2 ) );

	if ( m_adc->dma_channel != static_cast<uint8_t>( uC_DMA_CHANNEL_DNE ) )
	{
		m_dma_ctrl = new uC_DMA( m_adc->dma_channel, uC_DMA_DIR_FROM_PERIPH,
								 static_cast<uint8_t>( sizeof ( m_channel_buffer[0] ) ),
								 uC_DMA_MEM_INC_TRUE,
								 uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_TRUE );

		m_dma_ctrl->Set_Buff( m_channel_buffer, &m_adc->reg_ctrl->DR, total_channels );

		m_adc->reg_ctrl->CFGR1 |= ADC_CFGR1_DMAEN;
		m_adc->reg_ctrl->CFGR1 |= ADC_CFGR1_DMACFG;

		m_dma_ctrl->Enable();
	}
	else
	{
		m_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
	}

	/* Enable this for Interrupt based ADC*/
	/*
	   uC_Interrupt::Set_Vector( &Callback_Interupt, m_adc->irq_num, uC_INT_HW_PRIORITY_6 );
	   m_adc->reg_ctrl->IER |= ADC_IER_EOSIE;
	   m_adc->reg_ctrl->CFGR1 |= ADC_CFGR1_SCANDIR;
	 */
	// Get one good data sample.
	Dump_AtoD_Data();
	// Start_Scan();
}

/*******************************************************************************
 *  @fn         Set_Int_Vect
 *  @brief      Configure the USARTx interrupt priority.
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
	m_adc->reg_ctrl->IER |= ADC_IER_EOSIE;
	m_adc->reg_ctrl->CFGR1 &= ~( ADC_CFGR1_SCANDIR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Int_Handler( void )
{
	Clear_Int_Status();
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

	if ( m_advanced_vect == NULL )
	{
		m_advanced_vect = new Advanced_Vect();

		m_advanced_vect->Set_Vect( &Int_Handler_Static,
								   static_cast<Advanced_Vect::cback_param_t>( this ) );

		uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_adc->irq_num,
								  int_priority );
		m_adc->reg_ctrl->IER |= ADC_IER_EOSIE;
		m_adc->reg_ctrl->CFGR1 &= ~( ADC_CFGR1_SCANDIR );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Start_Scan( void ) const
{
	m_adc->reg_ctrl->CR |= ADC_CR_ADEN;
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
	m_adc->reg_ctrl->CFGR1 |= ADC_CFGR1_CONT;
	m_adc->reg_ctrl->CR |= ADC_CR_ADEN;
	m_adc->reg_ctrl->CR |= ADC_CR_ADSTART;
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Set_AtoD_Resolution( uC_BASE_ATOD_IO_STRUCT const* m_adc,
								   uC_ATOD_RESOLUTION_ENUM resolution ) const
{
	if ( resolution == uC_ATOD_6BIT_RESOLUTION )
	{
		m_adc->reg_ctrl->CFGR1 |= ADC_RES_6BIT;
	}
	else if ( resolution == uC_ATOD_8BIT_RESOLUTION )
	{
		m_adc->reg_ctrl->CFGR1 |= ADC_RES_8BIT;
	}
	else if ( resolution == uC_ATOD_10BIT_RESOLUTION )
	{
		m_adc->reg_ctrl->CFGR1 |= ADC_RES_10BIT;
	}
	else
	{
		m_adc->reg_ctrl->CFGR1 |= ADC_RES_12BIT;
		m_adc->reg_ctrl->CFGR1 |= ADC_CFGR1_ALIGN;	// Left Alignment for 12bit resolution to make it
													// appear 16bit regardless of actual bit
													// resolution
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// void Callback_Interupt( void )
// {
// uint16_t adc_value;
// ADC1->ISR &= ~ADC_ISR_EOS ;
// adc_value = ADC1->DR ;
// }
