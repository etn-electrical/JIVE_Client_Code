/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "uC_Base_HW_IDs_STM32F030.h"
#include "uC_Capture_Input.h"
#include "Ram.h"
#include "Babelfish_Assert.h"

extern HAL_StatusTypeDef HAL_TIM_IC_Init( TIM_HandleTypeDef* htim );

TIM_HandleTypeDef timerStruct;
TIM_IC_InitTypeDef counterStruct;

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

uC_Capture_Input::uC_Capture_Input( uint8_t hw_timer_num, uint8_t num_Of_Channels,
									uC_PERIPH_TIM_CAPTURE_STRUCT const* Channel_Config,
									uC_PERIPH_TIM_CONFIG_STRUCT* Timer_Config,
									uint8_t int_priority ) :
	uC_Timers_HW( hw_timer_num ),
	m_timer_defs( reinterpret_cast<timer_def_t*>( nullptr ) ),
	m_num_timers_Chn_Avl( num_Of_Channels ),
	m_tmr_ctrl( reinterpret_cast<uC_BASE_TIMER_IO_STRUCT const*>( nullptr ) ),
	m_chn_ctrl( reinterpret_cast<uC_PERIPH_TIM_CAPTURE_STRUCT const*>( nullptr ) )
{
	m_tmr_ctrl = ( uC_Base::Self() )->Get_Timer_Ctrl( hw_timer_num );

	BF_ASSERT( m_tmr_ctrl != nullptr );

	m_chn_ctrl = Channel_Config;
	m_num_timers_Chn_Avl = m_tmr_ctrl->num_compares;
	/* Check the number of capture channel passed is within limits */
	if ( ( num_Of_Channels > m_num_timers_Chn_Avl ) || ( m_num_timers_Chn_Avl == 0 ) )			// Num_Compares()
																								// )
	{
		BF_ASSERT(false);	// You have tried to use a timer with not enough compare
										// interrupts.
	}
	m_timer_defs =
		reinterpret_cast<timer_def_t*>( Ram::Allocate( sizeof( timer_def_t ) * num_Of_Channels ) );

	for ( uint8_t i = 0U; i < m_num_timers_Chn_Avl; i++ )
	{
		m_timer_defs[i].cback_func = &Dummy_Static;
	}
	Attach_CC_Callback( &Int_Handler_Static, this, int_priority, true );

	m_tc->DIER = 0U;// Disable all interrupts.

	TIMx_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl, Timer_Config );
	for ( uint_fast8_t i = 0U; i < num_Of_Channels; i++ )
	{
		uC_IO_Config::Enable_Periph_Input_Pin( Channel_Config->port_io );
		switch ( Channel_Config->TIM_Channel )
		{
			case uC_BASE_TIMER_CAP_COMP_CH1:
				TI1_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl,
							( uC_PERIPH_TIM_CAPTURE_STRUCT* )Channel_Config );
				break;

			case uC_BASE_TIMER_CAP_COMP_CH2:
				TI2_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl,
							( uC_PERIPH_TIM_CAPTURE_STRUCT* )Channel_Config );
				break;

			case uC_BASE_TIMER_CAP_COMP_CH3:
				TI3_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl,
							( uC_PERIPH_TIM_CAPTURE_STRUCT* )Channel_Config );
				break;

			case uC_BASE_TIMER_CAP_COMP_CH4:
				TI4_Config( ( uC_BASE_TIMER_IO_STRUCT* )m_tmr_ctrl,
							( uC_PERIPH_TIM_CAPTURE_STRUCT* )Channel_Config );
				break;
		}
		if ( i != ( num_Of_Channels - 1 ) )
		{
			Channel_Config = Channel_Config + 1;
		}
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
	uint16_t tmpcr1 = 0;

	/* Check the parameters */

	tmpcr1 = TIMx->reg_ctrl->CR1;

	if ( ( TIMx->reg_ctrl == TIM1 ) || ( TIMx->reg_ctrl == TIM3 ) )
	{
		/* Select the Counter Mode */
		tmpcr1 &= ( uint16_t )( ~( ( uint16_t )( TIM_CR1_DIR | TIM_CR1_CMS ) ) );
		tmpcr1 |= ( uint32_t )TIM_TimeBaseInitStruct->TIM_CounterMode;
	}

	if ( TIMx->reg_ctrl != TIM6 )
	{
		/* Set the clock division */
		tmpcr1 &= ( uint16_t )( ~( ( uint16_t )TIM_CR1_CKD ) );
		tmpcr1 |= ( uint32_t )TIM_TimeBaseInitStruct->TIM_ClockDivision;
	}

	TIMx->reg_ctrl->CR1 = tmpcr1;

	/* Set the Autoreload value */
	TIMx->reg_ctrl->ARR = TIM_TimeBaseInitStruct->TIM_Period;

	/* Set the Prescaler value */
	TIMx->reg_ctrl->PSC = TIM_TimeBaseInitStruct->TIM_Prescaler;

	if ( ( TIMx->reg_ctrl == TIM1 ) || ( TIMx->reg_ctrl == TIM15 ) || ( TIMx->reg_ctrl == TIM16 ) ||
		 ( TIMx->reg_ctrl == TIM17 ) )
	{
		/* Set the Repetition Counter value */
		TIMx->reg_ctrl->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
	}

	/* Generate an update event to reload the Prescaler and the Repetition counter
	   values immediately */
	TIMx->reg_ctrl->EGR = TIM_EGR_UG;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::TI1_Config( uC_BASE_TIMER_IO_STRUCT* TIMx,
								   uC_PERIPH_TIM_CAPTURE_STRUCT* Channel1 )
{
	uint16_t tmpccmr1 = 0, tmpccer = 0;

	/* Disable the Channel 1: Reset the CC1E Bit */
	TIMx->reg_ctrl->CCER &= ( uint16_t ) ~( ( uint16_t )TIM_CCER_CC1E );
	tmpccmr1 = TIMx->reg_ctrl->CCMR1;
	tmpccer = TIMx->reg_ctrl->CCER;
	/* Select the Input and set the filter */
	tmpccmr1 &=
		( uint16_t )( ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR1_CC1S ) ) &
					  ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR1_IC1F ) ) );
	tmpccmr1 |=
		( uint16_t )( Channel1->TIM_ICSelection |
					  ( uint16_t )( Channel1->TIM_ICFilter << ( uint16_t )4 ) );

	/* Select the Polarity and set the CC1E Bit */
	tmpccer &= ( uint16_t ) ~( ( uint16_t )( TIM_CCER_CC1P | TIM_CCER_CC1NP ) );
	tmpccer |= ( uint16_t )( Channel1->TIM_ICPolarity | ( uint16_t )TIM_CCER_CC1E );
	/* Write to TIMx CCMR1 and CCER registers */
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
								   uC_PERIPH_TIM_CAPTURE_STRUCT* Channel2 )
{
	uint16_t tmpccmr1 = 0, tmpccer = 0, tmp = 0;

	/* Disable the Channel 2: Reset the CC2E Bit */
	TIMx->reg_ctrl->CCER &= ( uint16_t ) ~( ( uint16_t )TIM_CCER_CC2E );
	tmpccmr1 = TIMx->reg_ctrl->CCMR1;
	tmpccer = TIMx->reg_ctrl->CCER;
	tmp = ( uint16_t )( Channel2->TIM_ICPolarity << 4 );
	/* Select the Input and set the filter */
	tmpccmr1 &=
		( uint16_t )( ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR1_CC2S ) ) &
					  ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR1_IC2F ) ) );
	tmpccmr1 |= ( uint16_t )( Channel2->TIM_ICFilter << 12 );
	tmpccmr1 |= ( uint16_t )( Channel2->TIM_ICSelection << 8 );
	/* Select the Polarity and set the CC2E Bit */
	tmpccer &= ( uint16_t ) ~( ( uint16_t )( TIM_CCER_CC2P | TIM_CCER_CC2NP ) );
	tmpccer |= ( uint16_t )( tmp | ( uint16_t )TIM_CCER_CC2E );
	/* Write to TIMx CCMR1 and CCER registers */
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
								   uC_PERIPH_TIM_CAPTURE_STRUCT* Channel3 )
{
	uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

	/* Disable the Channel 3: Reset the CC3E Bit */
	TIMx->reg_ctrl->CCER &= ( uint16_t ) ~( ( uint16_t )TIM_CCER_CC3E );
	tmpccmr2 = TIMx->reg_ctrl->CCMR2;
	tmpccer = TIMx->reg_ctrl->CCER;
	tmp = ( uint16_t )( Channel3->TIM_ICPolarity << 8 );
	/* Select the Input and set the filter */
	tmpccmr2 &=
		( uint16_t )( ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR2_CC3S ) ) &
					  ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR2_IC3F ) ) );
	tmpccmr2 |=
		( uint16_t )( Channel3->TIM_ICSelection |
					  ( uint16_t )( Channel3->TIM_ICFilter << ( uint16_t )4 ) );
	/* Select the Polarity and set the CC3E Bit */
	tmpccer &= ( uint16_t ) ~( ( uint16_t )( TIM_CCER_CC3P | TIM_CCER_CC3NP ) );
	tmpccer |= ( uint16_t )( tmp | ( uint16_t )TIM_CCER_CC3E );
	/* Write to TIMx CCMR2 and CCER registers */
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
								   uC_PERIPH_TIM_CAPTURE_STRUCT* Channel4 )
{
	uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

	/* Disable the Channel 4: Reset the CC4E Bit */
	TIMx->reg_ctrl->CCER &= ( uint16_t ) ~( ( uint16_t )TIM_CCER_CC4E );
	tmpccmr2 = TIMx->reg_ctrl->CCMR2;
	tmpccer = TIMx->reg_ctrl->CCER;
	tmp = ( uint16_t )( Channel4->TIM_ICPolarity << 12 );
	/* Select the Input and set the filter */
	tmpccmr2 &=
		( uint16_t )( ( uint16_t )( ~( uint16_t )TIM_CCMR2_CC4S ) &
					  ( ( uint16_t ) ~( ( uint16_t )TIM_CCMR2_IC4F ) ) );
	tmpccmr2 |= ( uint16_t )( Channel4->TIM_ICSelection << 8 );
	tmpccmr2 |= ( uint16_t )( Channel4->TIM_ICFilter << 12 );
	/* Select the Polarity and set the CC4E Bit */
	tmpccer &= ( uint16_t ) ~( ( uint16_t )( TIM_CCER_CC4P | TIM_CCER_CC4NP ) );
	tmpccer |= ( uint16_t )( tmp | ( uint16_t )TIM_CCER_CC4E );
	/* Write to TIMx CCMR2 and CCER registers */
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
	if ( timer_num < m_num_timers_Chn_Avl )
	{
		if ( ( ( m_chn_ctrl + timer_num )->TIM_Channel ) == timer_num )
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
void uC_Capture_Input::Init_Input_Capture( TIM_HandleTypeDef* htim )
{

	HAL_TIM_IC_Init( htim );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Capture_Input::Deinit_Input_Capture( uC_BASE_TIMER_IO_STRUCT* htim )
{

	m_tmr_ctrl->reg_ctrl->CR1 &= ~( TIM_CR1_CEN );

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
	uint16_t status;
	uint16_t chn_status = 0x1;
	uint32_t timer_ctr;

	status = m_tc->SR;
	status = m_tc->DIER & status;
	m_tc->SR = ~status;		// & M_COMPARE_INT_BITMASK );

	timer_ctr = 0U;
	status = status >> 1U;		// This makes sense because the start bit we are interested in is
								// second.
	while ( status != 0U )
	{
		if ( ( status & chn_status ) != 0U )
		{
			( *m_timer_defs[timer_ctr].cback_func )( m_timer_defs[timer_ctr].cback_param );
		}
		status &= ~chn_status;
		chn_status = chn_status << 1U;		// This makes sense because the bit we are interested in is
											// second.
		timer_ctr++;
	}
}
