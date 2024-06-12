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
 *all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

#include "uC_Multi_Timers.h"
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

	// You have tried to use a timer with not enough compare interrupts.
	BF_ASSERT( m_num_timers <= Num_Compares() );

	m_timer_defs = reinterpret_cast<timer_def_t*>( Ram::Allocate( sizeof( timer_def_t ) * num_timers ) );

	for ( uint_fast8_t i = 0U; i < num_timers; i++ )
	{
		m_timer_defs[i].cback_func = &Dummy_Static;
	}

	Attach_CC_Callback( &Int_Handler_Static, this, int_priority, true );

	m_tc->SC &= ~FTM_SC_TOIE_MASK;		// Disable timer overflow int.

	Ack_Int();
	Enable();
	// Run();
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
	Disable();
	Set_Timer_Frequency( desired_frequency - 1 );
	Enable();
	// Run();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Set_Callback( uint8_t timer_num,
									cback_func_t func_callback,
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
		// set to compare mode
		m_tc->CONTROLS[timer_num].CnSC = ( FTM_CnSC_ELSA_MASK | FTM_CnSC_MSA_MASK );
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
	volatile uint16_t new_counts;
	volatile uint32_t dummy_read;

	if ( timer_num < m_num_timers )
	{
		Push_TGINT();

		// set mode to compare (modified to set to compare when the timeout is set up)
		// m_tc->CONTROLS[timer_num].CnSC |= (FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK);
		current_time = Get_Timer_Value();

		// calculate the timeout
		new_counts = ( 0xFFFF ) & ( m_timer_defs[timer_num].timer_counts + current_time );
		// dummy read to make sure the flag gets reset when we write to it
		dummy_read = m_tc->CONTROLS[timer_num].CnSC;
		m_tc->CONTROLS[timer_num].CnSC &= ~FTM_CnSC_CHF_MASK;
		// turn on the interrupt (yes this has to be done before loading the counts, not sure why)
		m_tc->CONTROLS[timer_num].CnSC |= FTM_CnSC_CHIE_MASK;
		dummy_read = m_tc->CONTROLS[timer_num].CnSC;
		// load the counts for the timeout
		m_tc->CONTROLS[timer_num].CnV = new_counts;
		dummy_read = m_tc->CONTROLS[timer_num].CnV;

		Pop_TGINT();
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

	if ( timer_num < m_num_timers )
	{
		current_time = Get_Timer_Value();

		m_tc->CONTROLS[timer_num].CnV = m_timer_defs[timer_num].timer_counts + current_time;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers::Reload_Timeout( uint8_t timer_num )
{
	if ( timer_num < m_num_timers )
	{
		Push_TGINT();

		m_tc->CONTROLS[timer_num].CnV = m_tc->CONTROLS[timer_num].CnV + m_timer_defs[timer_num].timer_counts;
		// *compare_reg = *compare_reg + m_timer_defs[timer_num].timer_counts;

		if ( ( m_tc->CONTROLS[timer_num].CnV - Get_Timer_Value() ) > m_timer_defs[timer_num].timer_counts )
		{
			m_tc->CONTROLS[timer_num].CnV = Get_Timer_Value() + 1;

			// m_tc->EGR = TIM_EGR_CC1G<<timer_num; this was to force a reload of the counter value for the stm32. not
			// sure if there is a matching thing in k60
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
	volatile uint32_t dummy_read;

	if ( timer_num < m_num_timers )
	{
		// have to read status to clear the flag, might as well do it.
		dummy_read = m_tc->CONTROLS[timer_num].CnSC;
		// turn off the channel
		// m_tc->CONTROLS[timer_num].CnSC = 0x00000000;

		// turn off the interrupt instead of turning off the channel
		m_tc->CONTROLS[timer_num].CnSC &= ~FTM_CnSC_CHIE_MASK;
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
	uint32_t status;

	if ( timer_num < m_num_timers )
	{
		status = m_tc->CONTROLS[timer_num].CnSC;
		if ( ( status & FTM_CnSC_CHIE_MASK ) != 0 )
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
	uint32_t status;

	if ( timer_num < m_num_timers )
	{
		status = m_tc->CONTROLS[timer_num].CnSC;

		if ( ( status & FTM_CnSC_CHF_MASK ) != 0 )
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

	status = m_tc->STATUS;
	m_tc->STATUS = ~status;	// ack the interrupts

	timer_ctr = 0U;
	while ( ( status != 0 ) && ( timer_ctr < m_num_timers ) )
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

/*lint +e1924 */
