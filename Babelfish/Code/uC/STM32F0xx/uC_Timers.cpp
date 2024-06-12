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
#include "uC_Base.h"
#include "uC_IO_Config.h"
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
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Timers::uC_Timers( uint8_t timer_num ) :
	m_advanced_vect( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_cback_param( reinterpret_cast<cback_param_t>( nullptr ) ),
	m_cback_func( reinterpret_cast<cback_func_t>( nullptr ) ),
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

	m_advanced_vect = reinterpret_cast<Advanced_Vect*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Set_Callback( INT_CALLBACK_FUNC* int_callback, uint8_t int_priority ) const
{
	uC_Interrupt::Set_Vector( int_callback, m_tmr_ctrl->irq_num, int_priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
								 uint8_t int_priority, bool raw_interrupt )
{
	m_cback_param = func_param;
	m_cback_func = func_callback;

	if ( m_advanced_vect == NULL )
	{
		m_advanced_vect = new Advanced_Vect();
		if ( raw_interrupt == true )
		{
			m_advanced_vect->Set_Vect( &Raw_Int_Handler_Static,
									   static_cast<Advanced_Vect::cback_param_t>( this ) );
		}
		else
		{
			m_advanced_vect->Set_Vect( &Int_Handler_Static,
									   static_cast<Advanced_Vect::cback_param_t>( this ) );
		}

		uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_tmr_ctrl->irq_num,
								  int_priority );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Attach_CC_Callback( cback_func_t func_callback, cback_param_t func_param,
									uint8_t int_priority, bool raw_interrupt )
{
	m_cback_param = func_param;
	m_cback_func = func_callback;

	if ( m_advanced_vect == NULL )
	{
		m_advanced_vect = new Advanced_Vect();
		if ( raw_interrupt == true )
		{
			m_advanced_vect->Set_Vect( &Raw_Int_Handler_Static,
									   static_cast<Advanced_Vect::cback_param_t>( this ) );
		}
		else
		{
			m_advanced_vect->Set_Vect( &Int_Handler_Static,
									   static_cast<Advanced_Vect::cback_param_t>( this ) );
		}

		uC_Interrupt::Set_Vector(
			m_advanced_vect->Get_Int_Func(), m_tmr_ctrl->irq_num_cc, int_priority );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Int_Handler( void )
{
	Ack_Int();

	( *m_cback_func )( m_cback_param );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Raw_Int_Handler( void )
{
	( *m_cback_func )( m_cback_param );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_TIMERS_TICK_TD uC_Timers::Set_Timeout( uint32_t desired_frequency, bool repeat )
{
	uint64_t temp_calc;
	uC_TIMERS_TICK_TD prescaler;
	uC_TIMERS_TICK_TD count;

	m_tc->CR1 &= ~TIM_CR1_CEN;

	temp_calc =
		Round_Div( static_cast<uint64_t>( m_tclock_freq ),
				   static_cast<uint64_t>( desired_frequency ) );
	prescaler = static_cast<uC_TIMERS_TICK_TD>( temp_calc >> 16U );
	count =
		Round_Div( static_cast<uC_TIMERS_TICK_TD>( m_tclock_freq ),
				   static_cast<uC_TIMERS_TICK_TD>( prescaler + 1U ) ) /
		static_cast<uC_TIMERS_TICK_TD>( desired_frequency );

	m_tc->ARR = count;
	m_tc->PSC = prescaler;

	if ( repeat == true )
	{
		m_tc->CR1 = TIM_CR1_ARPE;
	}
	else
	{
		m_tc->CR1 = TIM_CR1_ARPE | TIM_CR1_OPM;
	}

	return ( static_cast<uC_TIMERS_TICK_TD>( temp_calc * m_tclock_freq ) );
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
bool uC_Timers::Timeout_Running( void ) const
{
	return ( ( m_tc->CR1 & TIM_CR1_CEN ) != 0U );
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
	uint64_t temp_calc;
	uC_TIMERS_TICK_TD prescaler;
	uC_TIMERS_TICK_TD count;

	temp_calc =
		Round_Div( static_cast<uint64_t>( m_tclock_freq ),
				   static_cast<uint64_t>( desired_frequency ) );
	prescaler = static_cast<uC_TIMERS_TICK_TD>( temp_calc >> 16U );
	count =
		Round_Div( static_cast<uC_TIMERS_TICK_TD>( m_tclock_freq ),
				   static_cast<uC_TIMERS_TICK_TD>( prescaler + 1U ) ) /
		static_cast<uC_TIMERS_TICK_TD>( desired_frequency );

	m_tc->PSC = prescaler;

	m_tc->CR1 = 0U;

	return ( count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Set_Tick_Frequency( uint32_t desired_frequency )
{
	uint64_t temp_calc;
	uC_TIMERS_TICK_TD prescaler_check;

	temp_calc =
		Round_Div( static_cast<uint64_t>( m_tclock_freq ),
				   static_cast<uint64_t>( desired_frequency ) );
	prescaler_check = static_cast<uC_TIMERS_TICK_TD>( temp_calc >> 16U );

	// The tick requested is too long.  It cannot be divided down that far.
	BF_ASSERT( prescaler_check == 0U );

	m_tc->CR1 = 0U;

	m_tc->PSC = static_cast<uint16_t>( temp_calc );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_TIMERS_TICK_TD uC_Timers::Get_Timer_Ticks( uint32_t desired_frequency ) const
{
	uC_TIMERS_TICK_TD return_tick_count;
	uC_TIMERS_TICK_TD prescalar;

	prescalar = m_tc->PSC;
	return_tick_count =
		Round_Div( static_cast<uC_TIMERS_TICK_TD>( m_tclock_freq ),
				   static_cast<uC_TIMERS_TICK_TD>( prescalar + 1U ) );

	if ( desired_frequency != 0U )
	{
		return_tick_count = return_tick_count / static_cast<uC_TIMERS_TICK_TD>( desired_frequency );
	}

	return ( return_tick_count );
}
