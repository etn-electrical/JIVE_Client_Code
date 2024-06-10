/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Single_LPTimer.h"
#include "Exception.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define  LPTIM_IT_CMPM          ( ( uint32_t ) 0x00080000U )
static const uint32_t TMR_CNT_SIZE_2_BYTE = 2U;
static const uint32_t SHIFT_BY_16 = 16U;
static const uint32_t SHIFT_BY_32 = 32U;
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
uC_Single_LPTimer::uC_Single_LPTimer( uint8_t timer_num ) : uC_LPTimers_HW( timer_num )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_LPTimer::Set_Callback( cback_func_t func_callback,
									  cback_param_t func_param, uint8_t int_priority )
{
	Attach_Callback( func_callback, func_param, int_priority, false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Single_LPTimer::Set_Timeout( uint32_t desired_frequency )
{
	uint64_t temp_calc;
	// TICK_TD compare_val;
	TICK_TD count;
	TICK_TD prescaler;


	// enable timer
	m_lptc->CR |= LPTIM_CR_ENABLE;

	temp_calc = Round_Div( static_cast<uint64_t>( m_tclock_freq ), static_cast<uint64_t>( desired_frequency ) );
	if ( m_lptmr_ctrl->cnt_size_bytes == TMR_CNT_SIZE_2_BYTE )
	{
		prescaler = static_cast<TICK_TD>( temp_calc >> SHIFT_BY_16 );
	}
	else
	{
		prescaler = static_cast<TICK_TD>( temp_calc >> SHIFT_BY_32 );
	}
	count = Round_Div( m_tclock_freq, ( prescaler + 1U ) ) / desired_frequency;


	// TODO code a logic to choose LPTIM_CFGR_PRESC w.r.t to prescalar calculated aboove
	// from available 3-bit possibilites 000 , 001 , 010 , 011 , 100  , 101  , 110  , 111
	// switch() 	with 8 cases for 	 DIV1, DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV128
	// default DIV1 prescalar
	m_lptc->CFGR &= ~( LPTIM_CFGR_PRESC_0 | LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_2 );

	// Load the period value in the autoreload register
	m_lptc->ARR = count;

	return ( static_cast<uint32_t>( count ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Single_LPTimer::Set_Wait_Period_Sec( uint32_t desired_period_sec )
{
	uint64_t temp_calc = 0;
	TICK_TD count = 0;
	TICK_TD prescaler = 0;
	uint32_t deisred_Timer_clk = 1;
	uint8_t psc_index = 0;

	static const uint8_t LP_Prescaler[8][2] = {
		{1,     0x0},
		{2,     0x1},
		{4,     0x2},
		{8,     0x3},
		{16,    0x4},
		{32,    0x5},
		{64,    0x6},
		{128,   0x7}
	};

	// enable timer
	m_lptc->CR |= LPTIM_CR_ENABLE;

	/**For reasonable operation limiting the time period **/
	if ( ( desired_period_sec < 10 ) || ( desired_period_sec > 60 ) )
	{
		desired_period_sec = 10;
	}

	// desired count with no PSC
	temp_calc = ( ( static_cast<uint64_t>( m_tclock_freq ) ) * ( static_cast<uint64_t>( desired_period_sec ) ) );

	// is desired count possible? in current CNT register
	if ( ( m_lptmr_ctrl->cnt_size_bytes == TMR_CNT_SIZE_2_BYTE ) && ( temp_calc > 0xFFFFU ) )
	{
		deisred_Timer_clk = ( 0xFFFF / desired_period_sec );
		if ( deisred_Timer_clk )
		{
			prescaler = ( m_tclock_freq / deisred_Timer_clk );
			// prescaler = Round_Div( m_tclock_freq, ( deisred_Timer_clk ) ) ; //TODO might be needed for period <10s
		}
	}
	else
	{
		prescaler = 0;
	}

	// from available 3-bit possibilites 000 , 001 , 010 , 011 , 100  , 101  , 110  , 111
	// DIV1, DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV128
	// default DIV1 prescalar
	for ( psc_index = 0; psc_index < 8;)
	{
		if ( prescaler <= LP_Prescaler[psc_index][0] )
		{
			break;
		}
		else
		{
			psc_index++;
		}
	}

	if ( psc_index < 8 )
	{
		// Choose the Prescaler
		m_lptc->CFGR |= ( LP_Prescaler[psc_index][1] << LPTIM_CFGR_PRESC_Pos );

		// Compute count value
		deisred_Timer_clk = m_tclock_freq / LP_Prescaler[psc_index][0];
	}

	count = deisred_Timer_clk * desired_period_sec;
	// Load the period value in the autoreload register
	m_lptc->ARR = count;

	return ( static_cast<uint32_t>( count ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_LPTimer::Start_Timeout( bool repeat )
{
	if ( repeat == TRUE )
	{
		m_lptc->CR |= LPTIM_CR_CNTSTRT;
	}
	else
	{
		m_lptc->CR |= LPTIM_CR_SNGSTRT;
	}

	if ( !uC_Interrupt::Is_Vector_Empty( m_lptmr_ctrl->irq_num ) )
	{
		Ack_Int();
		m_lptc->IER |= LPTIM_IER_ARRMIE;
		Enable_Int();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_LPTimer::Stop_Timeout( void )
{
	Disable_Int();

	// disable timer
	m_lptc->CR &= ~LPTIM_CR_ENABLE;

	// clear bits of status register
	Ack_Int();
}
