/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 * all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

#include "Includes.h"
#include "uC_Timers.h"
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
uC_Timers::uC_Timers( uint8_t timer_num ) :
	m_advanced_vect( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_cback_param( reinterpret_cast<cback_param_t>( nullptr ) ),
	m_cback_func( reinterpret_cast<cback_func_t>( nullptr ) ),
	m_tc( reinterpret_cast<TIM_TypeDef*>( nullptr ) ),
	m_tmr_ctrl( reinterpret_cast<uC_BASE_TIMER_IO_STRUCT const*>( nullptr ) ),
	m_tclock_freq( 0U )
{
	m_tmr_ctrl = ( uC_Base::Self() )->Get_Timer_Ctrl( timer_num );
	if ( m_tmr_ctrl != 0U )
	{
		uC_Base::Enable_Periph_Clock( &m_tmr_ctrl->periph_def );
		uC_Base::Reset_Periph( &m_tmr_ctrl->periph_def );

		m_tc = m_tmr_ctrl->reg_ctrl;
		m_tclock_freq = ( uC_Base::Self() )->Get_PClock_Freq( m_tmr_ctrl->pclock_freq_index );

		m_advanced_vect = reinterpret_cast<Advanced_Vect*>( nullptr );
	}
	else
	{
		BF_Lib::Exception::Stop_Here();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers::Set_Callback( INT_CALLBACK_FUNC* int_callback, uint8_t int_priority )
{
	m_vector_set = uC_Interrupt::Set_Vector( int_callback, m_tmr_ctrl->irq_num, int_priority );
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
		m_vector_set = uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_tmr_ctrl->irq_num, int_priority );
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
		m_vector_set =
			uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_tmr_ctrl->irq_num_cc, int_priority );
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
uint32_t uC_Timers::Set_Timeout( uint32_t desired_frequency, bool repeat )
{
	uint64_t temp_calc;
	uC_TIMERS_TICK_TD prescaler;
	uC_TIMERS_TICK_TD count;

	m_tc->CR1 &= ~TIM_CR1_CEN;

	temp_calc = Round_Div( static_cast<uint64_t>( m_tclock_freq ), static_cast<uint64_t>( desired_frequency ) );
	if ( m_tmr_ctrl->cnt_size_bytes == 2U )
	{
		prescaler = static_cast<uC_TIMERS_TICK_TD>( temp_calc >> 16U );
	}
	else
	{
		prescaler = static_cast<uC_TIMERS_TICK_TD>( temp_calc >> 32U );
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
void uC_Timers::Start_Timeout( void )
{
	m_tc->EGR = TIM_EGR_UG;
	m_tc->CR1 |= TIM_CR1_CEN;
	if ( m_vector_set == true )
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

	temp_calc = Round_Div( static_cast<uint64_t>( m_tclock_freq ), static_cast<uint64_t>( desired_frequency ) );
	if ( m_tmr_ctrl->cnt_size_bytes == 2U )
	{
		prescaler = static_cast<uC_TIMERS_TICK_TD>( temp_calc >> 16U );
	}
	else
	{
		prescaler = static_cast<uC_TIMERS_TICK_TD>( temp_calc >> 32U );
	}
	count = Round_Div( m_tclock_freq, ( prescaler + 1U ) ) / desired_frequency;

	m_tc->PSC = static_cast<uint16_t>( prescaler );

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

	temp_calc = Round_Div( static_cast<uint64_t>( m_tclock_freq ), static_cast<uint64_t>( desired_frequency ) );
	if ( m_tmr_ctrl->cnt_size_bytes == 2U )
	{
		prescaler_check = static_cast<uint32_t>( temp_calc >> 16U );
	}
	else
	{
		prescaler_check = static_cast<uint32_t>( temp_calc >> 32U );
	}

	if ( prescaler_check != 0U )
	{
		// The tick requested is too long.  It cannot be divided down that far.
		BF_Lib::Exception::Stop_Here();
	}

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
	uint32_t prescalar;

	prescalar = m_tc->PSC;
	return_tick_count = Round_Div( m_tclock_freq, ( prescalar + 1U ) );

	if ( desired_frequency != 0U )
	{
		return_tick_count = return_tick_count / desired_frequency;
	}

	return ( return_tick_count );
}

/*lint +e1924 */
