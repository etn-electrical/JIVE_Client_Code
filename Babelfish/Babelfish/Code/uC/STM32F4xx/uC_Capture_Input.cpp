/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "uC_Capture_Input.h"
#include "Exception.h"
#include "Ram.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Capture_Input::uC_Capture_Input( uint8_t hw_timer_num, uint8_t num_of_channels,
									uC_PERIPH_TIM_CAPTURE_STRUCT const* channel_config,
									uC_PERIPH_TIM_CONFIG_STRUCT* timer_config,
									uint8_t int_priority ) :
	uC_Timers_HW( hw_timer_num ),
	m_timer_defs( reinterpret_cast<timer_def_t*>( nullptr ) ),
	m_num_timers_chn_avl( num_of_channels ),
	m_tmr_ctrl( reinterpret_cast<uC_BASE_TIMER_IO_STRUCT const*>( nullptr ) ),
	m_chn_ctrl( reinterpret_cast<uC_PERIPH_TIM_CAPTURE_STRUCT const*>( nullptr ) )
{
	m_tmr_ctrl = ( uC_Base::Self() )->Get_Timer_Ctrl( hw_timer_num );

	BF_ASSERT( m_tmr_ctrl != nullptr );
	m_chn_ctrl = channel_config;
	m_num_timers_chn_avl = m_tmr_ctrl->num_compares;
	/// Check the number of capture channel passed is within limits
	if ( ( num_of_channels > m_num_timers_chn_avl ) || ( m_num_timers_chn_avl == 0U ) )
	{
		BF_ASSERT(false);	/// You have tried to use a timer with not enough compare interrupts.
	}
	m_timer_defs =
		reinterpret_cast<timer_def_t*>( Ram::Allocate( sizeof( timer_def_t ) * num_of_channels ) );

	if ( m_timer_defs != nullptr )
	{
		for ( uint8_t i = 0U; i < m_num_timers_chn_avl; i++ )
		{
			m_timer_defs[i].cback_func = &Dummy_Static;
		}
	}
	Attach_CC_Callback( &Int_Handler_Static, this, int_priority, true );

	m_tc->DIER = 0U;/// Disable all interrupts.

	TIMx_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl, timer_config );
	for ( uint_fast8_t i = 0U; i < num_of_channels; i++ )
	{
		uC_IO_Config::Enable_Periph_Input_Pin( channel_config->port_io );
		switch ( channel_config->tim_channel )
		{
			case uC_BASE_TIMER_CAP_COMP_CH1:
				TI1_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl,
							( uC_PERIPH_TIM_CAPTURE_STRUCT* )channel_config );
				break;

			case uC_BASE_TIMER_CAP_COMP_CH2:
				TI2_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl,
							( uC_PERIPH_TIM_CAPTURE_STRUCT* )channel_config );
				break;

			case uC_BASE_TIMER_CAP_COMP_CH3:
				TI3_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl,
							( uC_PERIPH_TIM_CAPTURE_STRUCT* )channel_config );
				break;

			case uC_BASE_TIMER_CAP_COMP_CH4:
				TI4_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl,
							( uC_PERIPH_TIM_CAPTURE_STRUCT* )channel_config );
				break;

			default:
				BF_ASSERT(false);
				break;
		}

		if ( i < num_of_channels )
		{
			channel_config = channel_config + 1;
		}
		Enable_CC_Int();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::TIMx_Config( uC_BASE_TIMER_IO_STRUCT* TIMx,
									uC_PERIPH_TIM_CONFIG_STRUCT* TIM_TimeBaseInitStruct )
{
	uint16_t tmpcr1 = 0U;

	/// Check the parameters
	tmpcr1 = TIMx->reg_ctrl->CR1;

	if ( ( TIMx->reg_ctrl == TIM1 ) || ( TIMx->reg_ctrl == TIM3 ) )
	{
		/// Select the Counter Mode
		tmpcr1 &= ( uint16_t )( ~( ( uint16_t )( TIM_CR1_DIR | TIM_CR1_CMS ) ) );
		tmpcr1 |= ( uint32_t )TIM_TimeBaseInitStruct->tim_counterMode;
	}

	/// Timer6 not supported for STM32f411xe
#ifndef uC_STM32F411_USAGE
	if ( TIMx->reg_ctrl != TIM6 )
	{
		/// Set the clock division
		tmpcr1 &= ( uint16_t )( ~( ( uint16_t )TIM_CR1_CKD ) );
		tmpcr1 |= ( uint32_t )TIM_TimeBaseInitStruct->tim_clockDivision;
	}
#endif

	TIMx->reg_ctrl->CR1 = tmpcr1;

	/// Set the Autoreload value
	TIMx->reg_ctrl->ARR = TIM_TimeBaseInitStruct->tim_period;

	/// Set the Prescaler value
	TIMx->reg_ctrl->PSC = TIM_TimeBaseInitStruct->tim_prescaler;

	/// Timer8 not supported for STM32f411xe
#ifndef uC_STM32F411_USAGE
	if ( ( TIMx->reg_ctrl == TIM1 ) || ( TIMx->reg_ctrl == TIM8 ) )
	{
		/// Set the Repetition Counter value
		TIMx->reg_ctrl->RCR = TIM_TimeBaseInitStruct->tim_repetitionCounter;
	}
#else
	if ( ( TIMx->reg_ctrl == TIM1 ) )
	{
		/// Set the Repetition Counter value
		TIMx->reg_ctrl->RCR = TIM_TimeBaseInitStruct->tim_repetitionCounter;
	}
#endif

	/// Generate an update event to reload the Prescaler and the Repetition counter values immediately
	TIMx->reg_ctrl->EGR = TIM_EGR_UG;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::TI1_Config( uC_BASE_TIMER_IO_STRUCT* TIMx,
								   uC_PERIPH_TIM_CAPTURE_STRUCT* channel1 )
{
	uint16_t tmpccmr1 = 0U, tmpccer = 0U;

	/// Disable the Channel 1: Reset the CC1E Bit
	TIMx->reg_ctrl->CCER &= ( uint16_t ) ~( ( uint16_t )TIM_CCER_CC1E );
	tmpccmr1 = TIMx->reg_ctrl->CCMR1;
	tmpccer = TIMx->reg_ctrl->CCER;
	/// Select the Input and set the filter
	tmpccmr1 &=
		( uint16_t )( ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR1_CC1S ) ) &
					  ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR1_IC1F ) ) );
	tmpccmr1 |=
		( uint16_t )( channel1->tim_icselection |
					  ( uint16_t )( channel1->tim_icfilter << ( uint16_t )INPUT_CAP1_FILTER_POS ) );

	/// Select the Polarity and set the CC1E Bit
	tmpccer &= ( uint16_t ) ~( ( uint16_t )( TIM_CCER_CC1P | TIM_CCER_CC1NP ) );
	tmpccer |= ( uint16_t )( channel1->tim_icpolarity | ( uint16_t )TIM_CCER_CC1E );
	/// Write to TIMx CCMR1 and CCER registers
	TIMx->reg_ctrl->CCMR1 = tmpccmr1;
	TIMx->reg_ctrl->CCER = tmpccer;
	TIMx->reg_ctrl->DIER |= TIM_DIER_CC1IE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::TI2_Config( uC_BASE_TIMER_IO_STRUCT* TIMx,
								   uC_PERIPH_TIM_CAPTURE_STRUCT* channel2 )
{
	uint16_t tmpccmr1 = 0U, tmpccer = 0U, tmp = 0U;

	/// Disable the Channel 2: Reset the CC2E Bit
	TIMx->reg_ctrl->CCER &= ( uint16_t ) ~( ( uint16_t )TIM_CCER_CC2E );
	tmpccmr1 = TIMx->reg_ctrl->CCMR1;
	tmpccer = TIMx->reg_ctrl->CCER;
	tmp = ( uint16_t )( channel2->tim_icpolarity << CC2_OUT_EN );
	/// Select the Input and set the filter
	tmpccmr1 &=
		( uint16_t )( ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR1_CC2S ) ) &
					  ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR1_IC2F ) ) );
	tmpccmr1 |= ( uint16_t )( channel2->tim_icfilter << INPUT_CAP2_FILTER_POS );
	tmpccmr1 |= ( uint16_t )( channel2->tim_icselection << INPUT_CAP_SEL_POS );
	/// Select the Polarity and set the CC2E Bit
	tmpccer &= ( uint16_t ) ~( ( uint16_t )( TIM_CCER_CC2P | TIM_CCER_CC2NP ) );
	tmpccer |= ( uint16_t )( tmp | ( uint16_t )TIM_CCER_CC2E );
	/// Write to TIMx CCMR1 and CCER registers
	TIMx->reg_ctrl->CCMR1 = tmpccmr1;
	TIMx->reg_ctrl->CCER = tmpccer;
	TIMx->reg_ctrl->DIER |= TIM_DIER_CC2IE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::TI3_Config( uC_BASE_TIMER_IO_STRUCT* TIMx,
								   uC_PERIPH_TIM_CAPTURE_STRUCT* channel3 )
{
	uint16_t tmpccmr2 = 0U, tmpccer = 0U, tmp = 0U;

	/// Disable the Channel 3: Reset the CC3E Bit
	TIMx->reg_ctrl->CCER &= ( uint16_t ) ~( ( uint16_t )TIM_CCER_CC3E );
	tmpccmr2 = TIMx->reg_ctrl->CCMR2;
	tmpccer = TIMx->reg_ctrl->CCER;
	tmp = ( uint16_t )( channel3->tim_icpolarity << CC3_OUT_EN );
	/// Select the Input and set the filter
	tmpccmr2 &=
		( uint16_t )( ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR2_CC3S ) ) &
					  ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR2_IC3F ) ) );
	tmpccmr2 |=
		( uint16_t )( channel3->tim_icselection |
					  ( uint16_t )( channel3->tim_icfilter << ( uint16_t )INPUT_CAP1_FILTER_POS ) );
	/// Select the Polarity and set the CC3E Bit
	tmpccer &= ( uint16_t ) ~( ( uint16_t )( TIM_CCER_CC3P | TIM_CCER_CC3NP ) );
	tmpccer |= ( uint16_t )( tmp | ( uint16_t )TIM_CCER_CC3E );
	/// Write to TIMx CCMR2 and CCER registers
	TIMx->reg_ctrl->CCMR2 = tmpccmr2;
	TIMx->reg_ctrl->CCER = tmpccer;
	TIMx->reg_ctrl->DIER |= TIM_DIER_CC3IE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::TI4_Config( uC_BASE_TIMER_IO_STRUCT* TIMx,
								   uC_PERIPH_TIM_CAPTURE_STRUCT* channel4 )
{
	uint16_t tmpccmr2 = 0U, tmpccer = 0U, tmp = 0U;

	/// Disable the Channel 4: Reset the CC4E Bit
	TIMx->reg_ctrl->CCER &= ( uint16_t ) ~( ( uint16_t )TIM_CCER_CC4E );
	tmpccmr2 = TIMx->reg_ctrl->CCMR2;
	tmpccer = TIMx->reg_ctrl->CCER;
	tmp = ( uint16_t )( channel4->tim_icpolarity << CC4_OUT_EN );
	/// Select the Input and set the filter
	tmpccmr2 &=
		( uint16_t )( ( uint16_t )( ~( uint16_t )TIM_CCMR2_CC4S ) &
					  ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR2_IC4F ) ) );
	tmpccmr2 |= ( uint16_t )( channel4->tim_icselection << INPUT_CAP_SEL_POS );
	tmpccmr2 |= ( uint16_t )( channel4->tim_icfilter << INPUT_CAP2_FILTER_POS );
	/// Select the Polarity and set the CC4E Bit
	tmpccer &= ( uint16_t ) ~( ( uint16_t )( TIM_CCER_CC4P | TIM_CCER_CC4NP ) );
	tmpccer |= ( uint16_t )( tmp | ( uint16_t )TIM_CCER_CC4E );
	/// Write to TIMx CCMR2 and CCER registers
	TIMx->reg_ctrl->CCMR2 = tmpccmr2;
	TIMx->reg_ctrl->CCER = tmpccer;
	TIMx->reg_ctrl->DIER |= TIM_DIER_CC4IE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Capture_Input::~uC_Capture_Input( void )
{
	Ram::De_Allocate( m_timer_defs );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::Set_Callback( uint8_t timer_num, cback_func_t func_callback,
									 cback_param_t func_param )
{
	if ( timer_num < m_num_timers_chn_avl )
	{
		if ( ( ( m_chn_ctrl + timer_num )->tim_channel ) == timer_num )
		{
			m_timer_defs[timer_num].cback_func = func_callback;
			m_timer_defs[timer_num].cback_param = func_param;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::Start_Input_Capture( void )
{
	m_tmr_ctrl->reg_ctrl->CR1 |= TIM_CR1_CEN;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::Stop_Input_Capture( void )
{
	m_tmr_ctrl->reg_ctrl->CR1 &= ~( TIM_CR1_CEN );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::Int_Handler( void )
{
	uint16_t status = 0U;
	uint16_t chn_status = 0x1;
	uint32_t timer_ctr = 0U;

	status = m_tc->SR;
	status = m_tc->DIER & status;
	m_tc->SR = ~status;

	status = status >> 1U;

	while ( status != 0U )
	{
		if ( ( status & chn_status ) != 0U )
		{
			( *m_timer_defs[timer_ctr].cback_func )( m_timer_defs[timer_ctr].cback_param );
		}
		status &= ~chn_status;
		chn_status = chn_status << 1U;

		timer_ctr++;
	}
}
