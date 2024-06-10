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
	Attach_Callback( &Int_Handler_Static, this, int_priority, true );

	m_tc->SC &= ~FTM_SC_TOIE_MASK;	// Disable timer overflow int.

	Ack_Int();
	Enable();
	// Run();
	Enable_Int();

	m_cback_func = func_callback;
	m_cback_param = func_param;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Single_Timer::Set_Timeout( uint32_t desired_frequency, bool repeat )
{
	m_tc->SC &= ~FTM_SC_TOIE_MASK;	// Disable timer overflow int.

	Set_Timer_Frequency( desired_frequency - 1 );

	timer_counts = Get_Timer_Ticks( desired_frequency );
	Get_Timer_Ticks( desired_frequency );
	// set to compare mode
	m_tc->CONTROLS->CnSC = ( FTM_CnSC_ELSA_MASK | FTM_CnSC_MSA_MASK );

	repeat_tmr = repeat;

	return ( static_cast<uint32_t>( timer_counts ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Start_Timeout( void )
{
	TICK_TD current_time;
	volatile uint16_t new_counts;
	volatile uint32_t dummy_read;

	Push_TGINT();

	// set mode to compare (modified to set to compare when the timeout is set up)
	// m_tc->CONTROLS[timer_num].CnSC |= (FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK);
	current_time = Get_Timer_Value();

	// calculate the timeout
	new_counts = ( 0xFFFF ) & ( timer_counts + current_time );
	// dummy read to make sure the flag gets reset when we write to it
	dummy_read = m_tc->CONTROLS->CnSC;
	m_tc->CONTROLS->CnSC &= ~FTM_CnSC_CHF_MASK;
	// turn on the interrupt (yes this has to be done before loading the counts, not sure why)
	m_tc->CONTROLS->CnSC |= FTM_CnSC_CHIE_MASK;
	dummy_read = m_tc->CONTROLS->CnSC;
	// load the counts for the timeout
	m_tc->CONTROLS->CnV = new_counts;
	dummy_read = m_tc->CONTROLS->CnV;

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Restart_Timeout( void )
{
	Push_TGINT();

	TICK_TD current_time;

	current_time = Get_Timer_Value();

	m_tc->CONTROLS->CnV = timer_counts + current_time;

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Stop_Timeout( void )
{
	volatile uint32_t dummy_read;

	// have to read status to clear the flag, might as well do it.
	dummy_read = m_tc->CONTROLS->CnSC;
	// turn off the channel
	// m_tc->CONTROLS[timer_num].CnSC = 0x00000000;

	// turn off the interrupt instead of turning off the channel
	m_tc->CONTROLS->CnSC &= ~FTM_CnSC_CHIE_MASK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Single_Timer::Timeout_Running( void ) const
{
	bool timeout_pending = false;
	uint32_t status;

	status = m_tc->CONTROLS->CnSC;

	if ( ( status & FTM_CnSC_CHF_MASK ) != 0 )
	{
		timeout_pending = true;
	}

	return ( timeout_pending );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Int_Handler( void )
{
	uint32_t status;

	status = m_tc->STATUS;
	m_tc->STATUS = ~status;	// ack the interrupts

	while ( status != 0 )
	{
		if ( ( status & 0x1U ) != 0U )
		{
			if ( repeat_tmr == false )
			{
				Stop_Timeout();
			}
			else
			{
				Restart_Timeout();
			}
			( *m_cback_func )( m_cback_param );
		}
		status = status >> 1U;		// This makes sense because the bit we are interested in is second.
	}
}

/*lint +e1924 */
