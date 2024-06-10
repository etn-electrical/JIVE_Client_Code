/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 *	@file           uC_Timers_HW.cpp implementation file for functionality of timers.
 *
 *	@details        See header file for module overview.
 *
 *	@copyright      2014 Eaton Corporation. All Rights Reserved.
 *
 *	@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here in.  This technical information should not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *
 *****************************************************************************************
 */
#include "uC_Timers_HW.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "Babelfish_Assert.h"

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
uC_Timers_HW::uC_Timers_HW( uint8_t timer_num, uC_BASE_TIMER_IO_STRUCT const* timer_io ) :
	// m_tc(reinterpret_cast<TIM_TypeDef*>(nullptr)),
	m_tclock_freq( 0U ),
	m_advanced_vect( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_cback_param( reinterpret_cast<cback_param_t>( nullptr ) ),
	m_cback_func( reinterpret_cast<cback_func_t>( nullptr ) ),
	m_tmr_ctrl( reinterpret_cast<uC_BASE_TIMER_IO_STRUCT const*>( nullptr ) ),
	m_tc( nullptr )
{
	if ( timer_io == reinterpret_cast<uC_BASE_TIMER_IO_STRUCT const*>( nullptr ) )
	{
		m_tmr_ctrl = ( uC_Base::Self() )->Get_Timer_Ctrl( timer_num );

	}
	else
	{
		m_tmr_ctrl = timer_io;
	}

	BF_ASSERT( m_tmr_ctrl != reinterpret_cast<uC_BASE_TIMER_IO_STRUCT const*>( nullptr ) );
	// switch( timer_num )
	// {
	// case uC_BASE_TIMER_CTRL_0:
	// SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
	// break;
	// case uC_BASE_TIMER_CTRL_1:
	// SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	// break;
	// case uC_BASE_TIMER_CTRL_2:
	// SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
	// break;
	// case uC_BASE_TIMER_CTRL_3:
	// SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;
	// break;
	// default:
	// BF_ASSERT(false);
	// break;
	// }

	uC_Base::Reset_Periph( &m_tmr_ctrl->periph_def );
	uC_Base::Enable_Periph_Clock( &m_tmr_ctrl->periph_def );

	m_tc = m_tmr_ctrl->reg_ctrl;
	m_tclock_freq = ( uC_Base::Self() )->Get_PClock_Freq( m_tmr_ctrl->pclock_freq_index );

	m_cback_func = &Dummy_Vect_Static;
	m_cback_param = this;

	m_advanced_vect = reinterpret_cast<Advanced_Vect*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers_HW::Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
									uint8_t int_priority, bool raw_interrupt )
{
	if ( m_advanced_vect == NULL )
	{
		m_advanced_vect = new Advanced_Vect();
	}

	if ( raw_interrupt == true )
	{
		m_cback_func = &Dummy_Vect_Static;
		m_cback_param = this;

		m_advanced_vect->Set_Vect( func_callback, func_param );
	}
	else
	{
		m_cback_param = func_param;
		m_cback_func = func_callback;

		m_advanced_vect->Set_Vect( &Int_Handler_Static,
								   static_cast<Advanced_Vect::cback_param_t>( this ) );
	}
	uC_Interrupt::Clear_Vector( m_tmr_ctrl->irq_num );
	uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_tmr_ctrl->irq_num, int_priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers_HW::Attach_CC_Callback( cback_func_t func_callback, cback_param_t func_param,
									   uint8_t int_priority, bool raw_interrupt )
{
	if ( m_advanced_vect == NULL )
	{
		m_advanced_vect = new Advanced_Vect();
	}

	if ( raw_interrupt == true )
	{
		m_cback_func = &Dummy_Vect_Static;
		m_cback_param = this;

		m_advanced_vect->Set_Vect( func_callback, func_param );
	}
	else
	{
		m_cback_param = func_param;
		m_cback_func = func_callback;

		m_advanced_vect->Set_Vect( &Int_Handler_Static,
								   static_cast<Advanced_Vect::cback_param_t>( this ) );
	}

	uC_Interrupt::Clear_Vector( m_tmr_ctrl->irq_num );
	uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_tmr_ctrl->irq_num, int_priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers_HW::Int_Handler( void )
{
	Ack_Int();

	( *m_cback_func )( m_cback_param );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Timers_HW::TICK_TD uC_Timers_HW::Set_Timer_Frequency( uint32_t desired_frequency )
{
	TICK_TD prescaler;
	TICK_TD count;
	// uint8_t  prescaler_reg;
	int8_t prescaler_reg;

	// prescaler_reg = 0;
	prescaler_reg = 7;
	// while( prescaler_reg <= 7 )
	while ( ( prescaler_reg >= 0 ) && ( prescaler_reg <= 7 ) )
	{
		prescaler = 1 << prescaler_reg;
		count = Round_Div( m_tclock_freq, prescaler ) / desired_frequency;

		if ( count < ( 1 << ( 8 * m_tmr_ctrl->cnt_size_bytes ) ) )
		{
			break;
		}

		// prescaler_reg++;
		prescaler_reg--;
	}

	// if ( prescaler_reg > 7 )
	// The frequency requested is too long.  It cannot be divided down that far.
	BF_ASSERT( ( prescaler_reg >= 0 ) || ( prescaler_reg <= 7 ) );

	m_tc->SC |= FTM_SC_PS( prescaler_reg );

	m_tc->CNT = 0;	// TODO: check if this counter restart is necessary
	// m_tc->MOD = count; //set overflow point

	return ( count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers_HW::Set_Tick_Frequency( uint32_t desired_frequency )
{
	// unused function, can only set limited prescaler values, use is impractical.
#if !defined ( uC_KINETISK66x_USAGE ) && !defined ( uC_KINETISK60DNx_USAGE )
	BF_ASSERT( false );
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Timers_HW::TICK_TD uC_Timers_HW::Get_Timer_Ticks( uint32_t desired_frequency ) const
{
	TICK_TD return_tick_count = 0U;
	uint32_t timer_freq;
	TICK_TD prescaler_val;

	prescaler_val = 1 << ( m_tc->SC & FTM_SC_PS_MASK );
	timer_freq = Round_Div( m_tclock_freq, prescaler_val );

	if ( desired_frequency != 0U )
	{
		return_tick_count = timer_freq / desired_frequency;
	}

	return ( return_tick_count );
}
