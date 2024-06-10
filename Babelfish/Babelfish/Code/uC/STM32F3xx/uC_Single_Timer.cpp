/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Single_Timer.h"
#include "Exception.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"


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
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Single_Timer::uC_Single_Timer( uint8_t timer_num ) : uC_Timers_HW( timer_num )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Set_Callback( cback_func_t func_callback,
									cback_param_t func_param, uint8_t int_priority )
{
	Attach_Callback( func_callback, func_param, int_priority, false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Single_Timer::Set_Timeout( uint32_t desired_frequency, bool repeat )
{
	uint64_t temp_calc;
	TICK_TD prescaler;
	TICK_TD count;

	m_tc->CR1 &= ~TIM_CR1_CEN;

	temp_calc = Round_Div( static_cast<uint64_t>( m_tclock_freq ), static_cast<uint64_t>( desired_frequency ) );
	if ( m_tmr_ctrl->cnt_size_bytes == 2U )
	{
		prescaler = static_cast<TICK_TD>( temp_calc >> 16 );
	}
	else
	{
		prescaler = static_cast<TICK_TD>( temp_calc >> 32 );
	}
	count = Round_Div( m_tclock_freq, ( prescaler + 1U ) ) / desired_frequency;

	m_tc->ARR = count;
	m_tc->PSC = static_cast<uint16_t>( prescaler );

	if ( repeat == true )
	{
		m_tc->CR1 = TIM_CR1_ARPE;
	}
	else
	{
		m_tc->CR1 = TIM_CR1_ARPE | TIM_CR1_OPM;
	}

	return ( static_cast<uint32_t>( temp_calc * m_tclock_freq ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Start_Timeout( void )
{
	m_tc->EGR = TIM_EGR_UG;
	m_tc->CR1 |= TIM_CR1_CEN;
	if ( !uC_Interrupt::Is_Vector_Empty( m_tmr_ctrl->irq_num ) )
	{
		Ack_Int();
		m_tc->DIER = TIM_DIER_UIE;
		Enable_Int();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Restart_Timeout( void )
{
	Push_TGINT();
	m_tc->CR1 |= TIM_CR1_UDIS;
	m_tc->EGR = TIM_EGR_UG;
	m_tc->CR1 &= ~TIM_CR1_UDIS;
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Stop_Timeout( void )
{
	Disable_Int();
	m_tc->CR1 &= ~TIM_CR1_CEN;
	m_tc->DIER = 0U;
	Ack_Int();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Single_Timer::Timeout_Running( void ) const
{
	return ( ( m_tc->CR1 & TIM_CR1_CEN ) != 0U );
}
