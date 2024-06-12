/**
 *****************************************************************************************
 *	@file           uC_LPTimers_HW.cpp implementation file for functionality of LPtimers.
 *
 *	@details        See header file for module overview.
 *
 *	@copyright      2020 Eaton Corporation. All Rights Reserved.
 *
 *	@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here in.  This technical information should not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *
 *****************************************************************************************
 */
#include "uC_LPTimers_HW.h"
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
uC_LPTimers_HW::uC_LPTimers_HW( uint8_t timer_num, uC_BASE_LPTIMER_IO_STRUCT const* lptimer_io ) :
	m_lptc( nullptr ),
	m_tclock_freq( 0U ),
	m_advanced_vect( nullptr ),
	m_cback_param( nullptr ),
	m_cback_func( nullptr ),
	m_lptmr_ctrl( nullptr )
{
	if ( lptimer_io == nullptr )
	{
		m_lptmr_ctrl = ( uC_Base::Self() )->Get_LPTimer_Ctrl( timer_num );
	}
	else
	{
		m_lptmr_ctrl = lptimer_io;
	}

	BF_ASSERT( m_lptmr_ctrl != nullptr );
	uC_Base::Reset_Periph( &m_lptmr_ctrl->periph_def );
	uC_Base::Enable_Periph_Clock( &m_lptmr_ctrl->periph_def );

	m_lptc = m_lptmr_ctrl->reg_ctrl;
	m_tclock_freq = ( uC_Base::Self() )->Get_PClock_Freq( m_lptmr_ctrl->pclock_freq_index );// TODO add LSE
																							// definition of 32.768
																							// KHz
	m_tclock_freq = 32768;	// TODO add LSE definition of 32.768 KHz
	m_cback_func = &Dummy_Vect_Static;
	m_cback_param = this;

	m_advanced_vect = reinterpret_cast<Advanced_Vect*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_LPTimers_HW::Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
									  uint8_t int_priority, bool raw_interrupt )
{
	if ( m_advanced_vect == nullptr )
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
	uC_Interrupt::Clear_Vector( m_lptmr_ctrl->irq_num );
	uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_lptmr_ctrl->irq_num, int_priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_LPTimers_HW::Attach_CC_Callback( cback_func_t func_callback, cback_param_t func_param,
										 uint8_t int_priority, bool raw_interrupt )
{
	if ( m_advanced_vect == nullptr )
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

	uC_Interrupt::Clear_Vector( m_lptmr_ctrl->irq_num_cc );
	uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(), m_lptmr_ctrl->irq_num_cc, int_priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_LPTimers_HW::Int_Handler( void )
{
	Ack_Int();

	( *m_cback_func )( m_cback_param );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_LPTimers_HW::TICK_TD uC_LPTimers_HW::Set_Timer_Frequency( uint32_t desired_frequency )
{
	TICK_TD compare_val;

	// stop timer
	m_lptc->CR = 0U;

	// Timeout = (Compare + 1) / LPTIM_Clock
	compare_val = static_cast<TICK_TD>( ( m_tclock_freq * desired_frequency ) - 1 );

	// Load the Timeout value in the compare register
	m_lptc->CMP = compare_val;

	return ( compare_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_LPTimers_HW::Set_Tick_Frequency( uint32_t desired_frequency )
{
	TICK_TD compare_val;

	// stop timer
	m_lptc->CR = 0U;

	// Timeout = (Compare + 1) / LPTIM_Clock
	compare_val = static_cast<TICK_TD>( ( m_tclock_freq * desired_frequency ) - 1 );

	// Load the Timeout value in the compare register
	// cmp = (desired frequency * LE clock frequency ) -1;
	m_lptc->CMP = compare_val;


}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_LPTimers_HW::TICK_TD uC_LPTimers_HW::Get_Timer_Ticks( uint32_t desired_frequency ) const
{
	TICK_TD return_tick_count;
	TICK_TD prescaler_val;

	prescaler_val = m_lptc->CMP;

	return_tick_count = prescaler_val + 1U;

	if ( desired_frequency != 0U )
	{
		return_tick_count = return_tick_count / desired_frequency;
	}

	return ( return_tick_count );
}
