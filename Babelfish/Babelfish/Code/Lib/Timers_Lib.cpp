/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Timers_Lib.h"

namespace BF_Lib
{
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
Timers::Timers( TIMERS_CBACK_TD callback_func, TIMERS_CBACK_PARAM_TD param,
				CR_TASKER_PRIOR_TD priority, char_t const* task_name ) :
	m_flags( 0U ),
	m_timeout( 0U ),
	m_overrun_adjust( 0U ),
	m_overrun_time_sample( 0U ),
	m_cback( callback_func ),
	// reinterpret_cast<TIMERS_CBACK_TD>( nullptr ) ),
	m_cback_param( param ),
	// reinterpret_cast<TIMERS_CBACK_PARAM_TD>( nullptr ) ),
	m_cr_task( reinterpret_cast<CR_Tasker*>( nullptr ) )
{
	m_cback = callback_func;
	m_cback_param = param;
	m_timeout = CR_Tasker::Get_CR_Tick_Count();
	m_flags = 0U;

	m_cr_task = new CR_Tasker( &Task, this, priority, task_name );

	Stop();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Timers::Attach_Callback( TIMERS_CBACK_TD callback_func, TIMERS_CBACK_PARAM_TD param )
{
	Push_TGINT();

	m_cback = callback_func;
	m_cback_param = param;

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Timers::Task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	Timers* me;
	bool temp_bit_test = false;

	me = reinterpret_cast<Timers*>( param );

	CR_Tasker_Begin( cr_task );
	{
		me->m_overrun_adjust = 0U;
		while ( true )
		{
			// The following lines were added to compensate for when the timer is offset for a
			// long time.
			if ( me->m_timeout <= me->m_overrun_adjust )
			{
				me->m_overrun_adjust = me->m_timeout;
			}
			me->m_overrun_time_sample = Get_Time();
			CR_Tasker_Delay( cr_task, me->m_timeout );		// - me->m_overrun_adjust );
			me->m_overrun_adjust = me->m_timeout - ( Get_Time() - me->m_overrun_time_sample );

			( *me->m_cback )( me->m_cback_param );

			Push_TGINT();
			temp_bit_test = Bit::Test( me->m_flags, TIMER_WAS_RESET );
			if ( Bit::Test( me->m_flags, TIMER_STOPPED ) )
			{
				me->m_overrun_adjust = 0U;

				Pop_TGINT();

				CR_Tasker_Yield( cr_task );
			}
			else if ( ( !Bit::Test( me->m_flags, TIMER_RELOAD ) ) &&
					  ( !temp_bit_test ) )
			{
				me->m_overrun_adjust = 0U;

				me->Stop();
				Pop_TGINT();

				CR_Tasker_Yield( cr_task );
			}
			else
			{
				/// Else deliberately left out.
			}
			Pop_TGINT();
			Bit::Clr( me->m_flags, TIMER_WAS_RESET );
		}
	}
	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Timers::Start( TIMERS_TIME_TD time_value, bool auto_reload )
{
	Push_TGINT();

	Bit::Clr( m_flags, TIMER_STOPPED );

	CR_Tasker::Remove_From_Any_List( m_cr_task->m_generic_list_item );
	CR_Tasker::Remove_From_Any_List( m_cr_task->m_event_list_item );
	CR_Tasker_Restart( m_cr_task );
	Bit::Clr( m_cr_task->m_priority, m_cr_task->CR_TASKER_SUSPEND_BIT );
	if ( CR_Tasker::Current_CRTask() == m_cr_task )
	{
		Bit::Set( m_flags, TIMER_WAS_RESET );
	}
	else
	{
		m_cr_task->m_pending_cr_ready_list->Insert( m_cr_task->m_event_list_item );
	}

	if ( auto_reload == true )
	{
		Bit::Set( m_flags, TIMER_RELOAD );
	}
	else
	{
		Bit::Clr( m_flags, TIMER_RELOAD );
	}

	m_timeout = time_value;

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Timers::Stop( void )
{
	Push_TGINT();

	Bit::Set( m_cr_task->m_priority, m_cr_task->CR_TASKER_SUSPEND_BIT );
	Bit::Set( m_flags, TIMER_STOPPED );

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Timers::Is_Dead( void ) const
{
	bool dead;

	Push_TGINT();
	dead = Bit::Test( m_flags, TIMER_STOPPED );
	Pop_TGINT();

	return ( dead );
}

}	/* end namespace BF_Lib for this module */
