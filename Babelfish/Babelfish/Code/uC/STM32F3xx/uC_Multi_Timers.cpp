/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_Multi_Timers.h"
#include "Exception.h"
#include "Ram.h"
#include "Babelfish_Assert.h"

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
uC_Multi_Timers::uC_Multi_Timers( uint8_t hw_timer_num,
								  uint8_t num_timers, uint8_t int_priority ) :
	uC_Timers_HW( hw_timer_num ),
	m_timer_defs( reinterpret_cast<timer_def_t*>( nullptr ) ),
	m_num_timers( 0U )
{
	m_num_timers = num_timers;

	BF_ASSERT( m_num_timers <= Num_Compares() );

	m_timer_defs =
		reinterpret_cast<timer_def_t*>( Ram::Allocate( sizeof( timer_def_t ) * num_timers ) );

	for ( uint_fast8_t i = 0U; i < num_timers; i++ )
	{
		m_timer_defs[i].cback_func = &Dummy_Static;
	}

	Attach_Callback( &Int_Handler_Static, this, int_priority, true );

	m_tc->DIER = 0U;// Disable all interrupts.

	Ack_Int();
	Enable();
	Run();
	Enable_Int();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Multi_Timers::~uC_Multi_Timers( void )
{
	Ram::De_Allocate( m_timer_defs );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Set_Min_Frequency( uint32_t desired_frequency )
{
	Set_Timer_Frequency( desired_frequency - 1U );
	Enable();
	Run();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Set_Callback( uint8_t timer_num, cback_func_t func_callback,
									cback_param_t func_param )
{
	if ( timer_num < m_num_timers )
	{
		m_timer_defs[timer_num].cback_func = func_callback;
		m_timer_defs[timer_num].cback_param = func_param;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Set_Timeout( uint8_t timer_num, uint32_t desired_frequency, bool repeat )
{
	if ( timer_num < m_num_timers )
	{
		m_timer_defs[timer_num].timer_counts = Get_Timer_Ticks( desired_frequency );
		m_timer_defs[timer_num].repeat = repeat;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Start_Timeout( uint8_t timer_num )
{
	TICK_TD current_time;
	uint32_t* compare_reg;

	if ( timer_num < m_num_timers )
	{
		compare_reg = const_cast<uint32_t*>( &m_tc->CCR1 );

		current_time = Get_Timer_Value();

		compare_reg[timer_num] = m_timer_defs[timer_num].timer_counts + current_time;

		m_tc->SR = ~static_cast<uint16_t>( TIM_DIER_CC1IE << timer_num );
		m_tc->DIER |= TIM_DIER_CC1IE << timer_num;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Restart_Timeout( uint8_t timer_num )
{
	TICK_TD current_time;
	uint32_t* compare_reg;

	if ( timer_num < m_num_timers )
	{
		compare_reg = const_cast<uint32_t*>( &m_tc->CCR1 );

		current_time = Get_Timer_Value();

		compare_reg[timer_num] = m_timer_defs[timer_num].timer_counts + current_time;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Reload_Timeout( uint8_t timer_num )
{
	uint32_t* compare_reg;

	if ( timer_num < m_num_timers )
	{
		compare_reg = const_cast<uint32_t*>( &m_tc->CCR1 );
		compare_reg += timer_num;

		Push_TGINT();

		*compare_reg = *compare_reg + m_timer_defs[timer_num].timer_counts;

		if ( ( *compare_reg - Get_Timer_Value() ) > m_timer_defs[timer_num].timer_counts )
		{
			*compare_reg = Get_Timer_Value();

			m_tc->EGR = static_cast<uint16_t>( TIM_EGR_CC1G ) << timer_num;
		}
		Pop_TGINT();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Stop_Timeout( uint8_t timer_num )
{
	if ( timer_num < m_num_timers )
	{
		m_tc->DIER &= ~static_cast<uint16_t>( TIM_DIER_CC1IE << timer_num );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Multi_Timers::Timeout_Running( uint8_t timer_num ) const
{
	bool timer_running = false;

	if ( timer_num < m_num_timers )
	{
		if ( ( m_tc->DIER & static_cast<uint16_t>( TIM_DIER_CC1IE << timer_num ) ) != 0U )
		{
			timer_running = true;
		}
	}

	return ( timer_running );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Multi_Timers::Timeout_Pending( uint8_t timer_num ) const
{
	bool timeout_pending = false;
	uint16_t status;

	if ( timer_num < m_num_timers )
	{
		status = m_tc->SR;
		status = m_tc->DIER & status;

		if ( ( status & static_cast<uint16_t>( TIM_SR_CC1IF << timer_num ) ) != 0U )
		{
			timeout_pending = true;
		}
	}

	return ( timeout_pending );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Int_Handler( void )
{
	uint32_t status;
	uint32_t timer_ctr;

	status = m_tc->SR;
	status = m_tc->DIER & status;
	m_tc->SR = static_cast<uint16_t>( ~status );			// & M_COMPARE_INT_BITMASK );

	timer_ctr = 0U;
	status = status >> 1U;		// This makes sense because the start bit we are interested in is
								// second.
	while ( status != 0U )
	{
		if ( ( status & 0x1U ) != 0U )
		{
			if ( m_timer_defs[timer_ctr].repeat == false )
			{
				Stop_Timeout( static_cast<uint8_t>( timer_ctr ) );
			}
			else
			{
				Reload_Timeout( static_cast<uint8_t>( timer_ctr ) );
			}
			( *m_timer_defs[timer_ctr].cback_func )( m_timer_defs[timer_ctr].cback_param );
		}
		status = status >> 1U;		// This makes sense because the bit we are interested in is second.
		timer_ctr++;
	}
}
