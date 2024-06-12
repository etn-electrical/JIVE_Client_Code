/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_Timers_LP_HW.h"
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
uC_Timers_LP_HW::uC_Timers_LP_HW( uint8_t timer_num )
{
	m_tmr_ctrl = ( uC_Base::Self() )->Get_LP_Timer_Ctrl( timer_num );
	BF_ASSERT( m_tmr_ctrl != NULL );
	// enable the clock to the LPTMR
	// SIM->SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

	uC_Base::Reset_Periph( &m_tmr_ctrl->periph_def );
	uC_Base::Enable_Periph_Clock( &m_tmr_ctrl->periph_def );

	// set up module variables
	m_tc = m_tmr_ctrl->reg_ctrl;
	m_tclock_freq = m_tmr_ctrl->clock_freq;

	// set up for free running, reset on overflow
	m_tc->CSR = LPTMR_CSR_TFC_MASK;
	// set to use the selected clock, clear other prescale settings
	m_tc->PSR = LPTMR_PSR_PCS( m_tmr_ctrl->clock_sel );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Timers_LP_HW::TICK_TD uC_Timers_LP_HW::Set_Timer_Frequency( uint32_t desired_frequency )
{
	TICK_TD prescaler;
	TICK_TD count;
	uint8_t prescaler_reg;

	prescaler_reg = 0;
	while ( prescaler_reg <= 15 )
	{
		// prescaler setting of 0b0000 = div by 2
		prescaler = 1 << ( prescaler_reg + 1 );
		count = Round_Div( m_tclock_freq, prescaler ) / desired_frequency;

		if ( count < ( 1 << ( 8 * m_tmr_ctrl->cnt_size_bytes ) ) )
		{
			break;
		}

		prescaler_reg++;
	}

	BF_ASSERT( prescaler_reg <= 15 );

	m_tc->PSR |= LPTMR_PSR_PRESCALE( prescaler_reg );

	m_tc->CMR = count;	// set compare point

	return ( count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Timers_LP_HW::Set_Tick_Frequency( uint32_t desired_frequency )
{
	uint64_t temp_calc;
	TICK_TD prescaler_check;
	TICK_TD prescaler;

	temp_calc = Round_Div( m_tclock_freq, desired_frequency );
	if ( m_tmr_ctrl->cnt_size_bytes == 2 )
	{
		prescaler_check = temp_calc >> 16;
	}
	else
	{
		prescaler_check = temp_calc >> 32;
	}
	BF_ASSERT( prescaler_check == 0 );

	// already should equal zero, no need to re-set it to zero
	while ( prescaler_check <= 15 )
	{
		// prescaler setting of 0b0000 = div by 2
		prescaler = 1 << ( prescaler_check + 1 );

		if ( temp_calc == prescaler )
		{
			break;
		}

		prescaler_check++;
	}
	BF_ASSERT( prescaler_check <= 15 );

	m_tc->PSR |= LPTMR_PSR_PRESCALE( prescaler_check );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Timers_LP_HW::TICK_TD uC_Timers_LP_HW::Get_Timer_Ticks( uint32_t desired_frequency )
{
	TICK_TD return_tick_count = 0U;
	uint32_t timer_freq;
	TICK_TD prescaler_val;

	prescaler_val = 1 << ( ( ( m_tc->PSR & LPTMR_PSR_PRESCALE_MASK ) >> 3 ) + 1 );
	timer_freq = Round_Div( m_tclock_freq, prescaler_val );

	if ( desired_frequency != 0 )
	{
		return_tick_count = timer_freq / desired_frequency;
	}

	return ( return_tick_count );
}
