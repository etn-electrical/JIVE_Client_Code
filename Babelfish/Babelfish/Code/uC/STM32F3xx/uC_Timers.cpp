/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Timers.h"
#include "uC_Timers_HW.h"
#include "Exception.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "Babelfish_Assert.h"


/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// #define MAX_TIMER_TICKS			0xFFFF

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
uC_Timers::uC_Timers( uint8_t timer_num ) :
	m_tc( reinterpret_cast<TIM_TypeDef*>( nullptr ) ),
	m_tmr_ctrl( reinterpret_cast<uC_BASE_TIMER_IO_STRUCT const*>( nullptr ) ),
	m_tclock_freq( 0U )
{
	m_tmr_ctrl = ( uC_Base::Self() )->Get_Timer_Ctrl( timer_num );
	BF_ASSERT( m_tmr_ctrl != 0U );

	uC_Base::Reset_Periph( &m_tmr_ctrl->periph_def );
	uC_Base::Enable_Periph_Clock( &m_tmr_ctrl->periph_def );

	m_tc = m_tmr_ctrl->reg_ctrl;
	m_tclock_freq = ( uC_Base::Self() )->Get_PClock_Freq( m_tmr_ctrl->pclock_freq_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Set_Callback( INT_CALLBACK_FUNC* int_callback, uint8_t priority ) const
{
	uC_Interrupt::Set_Vector( int_callback, m_tmr_ctrl->irq_num, priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Timers::Set_Timeout( uint32_t desired_frequency, bool repeat )
{
	BF_Lib::SPLIT_UINT32 temp_calc;
	uint32_t prescaler;
	uint32_t count;

	m_tc->CR1 &= ~TIM_CR1_CEN;

	temp_calc.u32 = Round_Div( m_tclock_freq, desired_frequency );
	prescaler = temp_calc.u16[1];
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

	return ( temp_calc.u32 * m_tclock_freq );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Start_Timeout( void )
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
void uC_Timers::Stop_Timeout( void )
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
void uC_Timers::Run( void )
{
	m_tc->EGR = TIM_EGR_UG;
	m_tc->CR1 |= TIM_CR1_CEN;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Stop( void )
{
	m_tc->CR1 &= ~TIM_CR1_CEN;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_TIMERS_TICK_TD uC_Timers::Set_Timer_Frequency( uint32_t desired_frequency )
{
	BF_Lib::SPLIT_UINT32 temp_calc;
	uint32_t prescaler;
	uint32_t count;

	temp_calc.u32 = Round_Div( m_tclock_freq, desired_frequency );
	prescaler = temp_calc.u16[1];
	count = Round_Div( m_tclock_freq, ( prescaler + 1U ) ) / desired_frequency;

	m_tc->PSC = static_cast<uint16_t>( prescaler );

	m_tc->CR1 = 0U;

	return ( static_cast<uC_TIMERS_TICK_TD>( count ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Set_Tick_Frequency( uint32_t desired_frequency )
{
	BF_Lib::SPLIT_UINT32 temp_calc;

	temp_calc.u32 = Round_Div( m_tclock_freq, desired_frequency );
	BF_ASSERT( temp_calc.u16[1] == 0U );

	m_tc->CR1 = 0U;

	m_tc->PSC = temp_calc.u16[0];
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_TIMERS_TICK_TD uC_Timers::Get_Timer_Ticks( uint32_t desired_freq ) const
{
	uC_TIMERS_TICK_TD return_tick_count;
	uC_TIMERS_TICK_TD prescaler_val;

	prescaler_val = m_tc->PSC;
	return_tick_count = Round_Div( static_cast<uC_TIMERS_TICK_TD>( m_tclock_freq ),
								   static_cast<uC_TIMERS_TICK_TD>( prescaler_val + 1U ) );

	if ( desired_freq != 0U )
	{
		return_tick_count = return_tick_count / static_cast<uC_TIMERS_TICK_TD>( desired_freq );
	}

	return ( return_tick_count );
}
