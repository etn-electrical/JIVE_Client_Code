/**
 **********************************************************************************************
 *	@file            OS_Tasker.cpp C++ Implementation File for operating system tasking
 *	                 functionality
 *
 *	@brief           The file shall include the definitions of all the functions required for
 *                   Operating system tasking and which are declared in corresponding header file
 *	@details
 *	@copyright       2014 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "Includes.h"
#include "OS_Tasker.h"
#include "uC_OS_Tick.h"
#include "OS_Priorities.h"
#include "OS_Task_Mon.h"
#include "uC_OS_Interrupt.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************
 *		Variables
 *****************************************************************************
 */
extern "C"
{
void OS_Start_First_Task_Handler( void );

#ifdef FPU_SUPPORT
void asmEnableVFP( void );

#endif
};

/// Plain old C construct so that assembly can get at it.
OS_Task* OS_TASKER_current_task = reinterpret_cast<OS_Task*>( nullptr );

/// Number of tasks
volatile BF_Lib::MBASE OS_Tasker::m_num_tasks = 0U;

/// Top priority of ready tasks
volatile BF_Lib::MBASE OS_Tasker::m_top_ready_priority = 0U;

/// Top used priority
BF_Lib::MBASE OS_Tasker::m_top_used_priority = 0U;

/// Scheduler running or not
volatile bool OS_Tasker::m_scheduler_running = false;

/// Scheduler Suspended
volatile BF_Lib::MBASE OS_Tasker::m_scheduler_suspend = 0U;
bool OS_Tasker::m_yield_missed = false;

OS_TICK_TD OS_Tasker::m_current_tick = 0U;
volatile OS_TICK_TD OS_Tasker::m_tick_overflows = 0U;
volatile OS_TICK_TD OS_Tasker::m_missed_ticks = 0U;

OS_List* OS_Tasker::m_ready_lists[OS_TASK_PRIORITY_MAX];
OS_List* /* volatile */ OS_Tasker::m_delayed_task_list = reinterpret_cast<OS_List*>( nullptr );
OS_List* /* volatile */ OS_Tasker::m_overflow_delayed_task_list =
	reinterpret_cast<OS_List*>( nullptr );
OS_List* OS_Tasker::m_pending_ready_list = reinterpret_cast<OS_List*>( nullptr );

#if CONFIG_INCLUDE_TASK_SUSPEND == 1U
OS_List* OS_Tasker::m_suspended_list = reinterpret_cast<OS_List*>( nullptr );
#endif

OS_List* OS_Tasker::m_delayed_list_0 = reinterpret_cast<OS_List*>( nullptr );
OS_List* OS_Tasker::m_delayed_list_1 = reinterpret_cast<OS_List*>( nullptr );

#if CONFIG_INCLUDE_TASK_DELETE == 1U
OS_List* OS_Tasker::m_waiting_termination_list = reinterpret_cast<OS_List*>( nullptr );
volatile BF_Lib::MBASE OS_Tasker::m_num_tasks_to_delete = 0U;
CR_Tasker* OS_Tasker::m_delete_task = reinterpret_cast<CR_Tasker*>( nullptr );
#endif

// For interrupt Compatibility
/**
 *@remark Coding Standard Deviation:
 *@n MISRA-C++[2008] Required Rule 5-2-6, Suspicious cast
 *@n Justification: Although this cast may raise suspicion, it is required in order for the
 *	code to compile.
 */
/*lint -e{611} # MISRA Deviation */
const void* OS_TASKER_Switch_Context = reinterpret_cast<void*>( &OS_Tasker::Switch_Context );
// const void*   OS_TASKER_Tick_Handler = (void*)OS_Tasker::Tick_Handler;
/// Pick a number that will not roll over but will also not dec too far.
/// If it were to roll over you would have bigger problems.
BF_Lib::MBASE OS_INT_critical_nesting = 0U;

#if CONFIG_USE_TICK_HOOK == 1U
HOOK_FUNCTION OS_Tasker::m_tick_hook_function = reinterpret_cast<HOOK_FUNCTION>( nullptr );
#endif


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Tasker::OS_Tasker( void )
{

	new uC_OS_Interrupt();

	for ( uint8_t i = 0U; i < OS_TASK_PRIORITY_MAX; i++ )
	{
		m_ready_lists[i] = new OS_List();

	}

	m_pending_ready_list = new OS_List();

#if CONFIG_INCLUDE_TASK_SUSPEND == 1U
	m_suspended_list = new OS_List();

#endif

	m_delayed_list_0 = new OS_List();

	m_delayed_list_1 = new OS_List();

#if CONFIG_INCLUDE_TASK_DELETE == 1U
	m_waiting_termination_list = new OS_List();

#endif

	m_delayed_task_list = m_delayed_list_0;
	m_overflow_delayed_task_list = m_delayed_list_1;

	m_top_ready_priority = OS_TASK_PRIORITY_LOW_IDLE_TASK;

	OS_Task_Mon::Init();

	uC_OS_Interrupt::Set_SVC_Handler_Vector();

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not
	 * enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	new uC_OS_Tick( Tick_Handler );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t OS_Tasker::Tasker_Running( void )
{
	uint8_t running;

	if ( m_scheduler_running == false )
	{
		running = static_cast<uint8_t>( OS_TASKER_NOT_STARTED );
	}
	else
	{
		if ( m_scheduler_suspend == 0U )
		{
			running = static_cast<uint8_t>( OS_TASKER_RUNNING );
		}
		else
		{
			running = static_cast<uint8_t>( OS_TASKER_SUSPENDED );
		}
	}

	return ( running );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Task* OS_Tasker::Create_Task( OS_TASK_FUNC* task_func, uint16_t stack_size_in_words,
								 uint8_t priority, uint8_t const* task_name,
								 OS_TASK_PARAM init_parameter )
{
	OS_Task* task;

	if ( priority >= OS_TASK_PRIORITY_MAX )
	{
		priority = OS_TASK_PRIORITY_MAX - 1U;
	}

	task = new OS_Task( task_func, stack_size_in_words, priority, task_name, init_parameter );

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 0-1-9, Operator '!=' always evaluates to True
	 *@n Justification: This if check is needed to check for null pointer.
	 */
	/* lint -e{948} # MISRA Deviation
	  lint -e{774} # MISRA Deviation*/
	if ( task != NULL )
	{
		OS_Task_Mon::Register_Task( task );

		Enter_Critical();

		m_num_tasks++;

		if ( m_num_tasks == 1U )
		{
			OS_TASKER_current_task = task;
		}
		else
		{
			if ( m_scheduler_running == false )
			{
				if ( OS_TASKER_current_task->m_priority <= priority )
				{
					OS_TASKER_current_task = task;
				}
			}
		}

		if ( task->m_priority > m_top_used_priority )
		{
			m_top_used_priority = task->m_priority;
		}

		task->m_event_list_item->Set_Value(
			static_cast<OS_LIST_VALUE_TD>( OS_TASK_PRIORITY_MAX ) - priority );
		Add_To_Ready_List( task );

		Exit_Critical();
	}

	if ( m_scheduler_running )
	{
		if ( OS_TASKER_current_task->m_priority < priority )
		{
			Yield();
		}
	}

	return ( task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_DELETE == 1U
void OS_Tasker::Delete_Task( OS_Task* task )
{
	// Suspend( task );

	Enter_Critical();

	Remove_From_Any_List( task->m_general_list_item );

	Remove_From_Any_List( task->m_event_list_item );

	m_waiting_termination_list->Add_To_End( task->m_general_list_item );

	m_num_tasks_to_delete++;

	if ( m_delete_task != nullptr )
	{
		m_delete_task->Resume();
	}

	Exit_Critical();

	if ( m_scheduler_running )
	{
		if ( task == OS_TASKER_current_task )
		{
			Yield();
		}
	}

}

#endif	// CONFIG_INCLUDE_TASK_DELETE == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_DELAY == 1U
void OS_Tasker::Delay( OS_TICK_TD ticks_to_delay )
{
	bool already_yielded = false;
	OS_TICK_TD time_to_wake;

	if ( ticks_to_delay > 0U )
	{
		Suspend_Scheduler();

		time_to_wake = m_current_tick + ticks_to_delay;

		Remove_From_Any_List( OS_TASKER_current_task->m_general_list_item );

		OS_TASKER_current_task->m_general_list_item->Set_Value( time_to_wake );

		// time_to_wake overflow
		if ( time_to_wake < m_current_tick )
		{
			m_overflow_delayed_task_list->Insert(
				OS_TASKER_current_task->m_general_list_item );
		}
		else// time_to_wake no overflow
		{
			m_delayed_task_list->Insert(
				OS_TASKER_current_task->m_general_list_item );
		}
		already_yielded = Resume_Scheduler();
	}

	if ( already_yielded == false )
	{
		Yield();
	}
}

#endif	// CONFIG_INCLUDE_TASK_DELAY == 1U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_DELAY_UNTIL == 1U
void OS_Tasker::Delay_Until( OS_TICK_TD* const pPreviousWakeTime,
							 OS_TICK_TD timeIncrement )
{
	bool already_yielded;
	bool should_delay = false;
	OS_TICK_TD time_to_wake;

	Suspend_Scheduler();

	time_to_wake = *pPreviousWakeTime + timeIncrement;

	if ( m_current_tick < *pPreviousWakeTime )
	{
		if ( ( time_to_wake < *pPreviousWakeTime ) && ( time_to_wake >
														m_current_tick ) )
		{
			should_delay = true;
		}
	}
	else
	{
		if ( ( time_to_wake < *pPreviousWakeTime ) || ( time_to_wake >
														m_current_tick ) )
		{
			should_delay = true;
		}
	}

	*pPreviousWakeTime = time_to_wake;

	if ( should_delay )
	{
		Remove_From_Any_List( OS_TASKER_current_task->m_general_list_item );

		OS_TASKER_current_task->m_general_list_item->Set_Value( time_to_wake );

		if ( time_to_wake < m_current_tick )
		{
			m_overflow_delayed_task_list->Insert(
				OS_TASKER_current_task->m_general_list_item );
		}
		else
		{
			m_delayed_task_list->Insert(
				OS_TASKER_current_task->m_general_list_item );
		}
		/*already_yielded = */ Resume_Scheduler();
		// TODO:  I think something is fishy here.  To resume the scheduler in two places.
		// At this time I don't believe this function is even used.  If it is used at some
		// point it would be good to reverify.  The exception stop here is to catch someone using
		// it and have them evaluate it.
		BF_ASSERT( false );
	}

	already_yielded = Resume_Scheduler();

	if ( !already_yielded )
	{
		Yield();
	}
}

#endif	// CONFIG_INCLUDE_TASK_DELAY == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Resume_Scheduler( void )
{
	bool already_yielded = false;
	bool yield_required = false;
	OS_Task* temp_task;

	Enter_Critical();

	m_scheduler_suspend--;

	if ( m_scheduler_suspend == 0U )
	{
		if ( m_num_tasks > 0U )
		{
			temp_task = reinterpret_cast<OS_Task*>( m_pending_ready_list->Get_Head_Entry() );
			while ( temp_task != NULL )
			{
				Remove_From_Any_List( temp_task->m_general_list_item );
				Remove_From_Any_List( temp_task->m_event_list_item );
				Add_To_Ready_List( temp_task );

				if ( temp_task->m_priority >= OS_TASKER_current_task->m_priority )
				{
					yield_required = true;
				}
				temp_task = reinterpret_cast<OS_Task*>( m_pending_ready_list->Get_Head_Entry() );
			}

			if ( m_missed_ticks > 0U )
			{
				while ( m_missed_ticks > 0U )
				{
					Tick_Handler();
					m_missed_ticks--;
				}

				yield_required = true;
			}

			if ( ( yield_required ) || ( m_yield_missed ) )
			{
				already_yielded = true;
				m_yield_missed = false;
				Yield();
			}
		}
	}
	Exit_Critical();

	return ( already_yielded );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Tick_Handler( void )
{
	if ( m_scheduler_suspend == 0U )
	{
		m_current_tick++;
		if ( m_current_tick == 0U )
		{
			OS_List* temp_list;

			temp_list = m_delayed_task_list;
			m_delayed_task_list = m_overflow_delayed_task_list;
			m_overflow_delayed_task_list = temp_list;
			m_tick_overflows++;
		}

		Check_Delayed_Tasks();
	}
	else
	{
		m_missed_ticks++;
		/* The tick hook gets called at regular intervals, even if the
		   scheduler is locked. */

#if CONFIG_USE_TICK_HOOK == 1U
		if ( m_tick_hook_function != NULL )
		{
			m_tick_hook_function();
		}
#endif
	}

	/* Guard against the tick hook being called when the missed tick
	   count is being unwound (when the scheduler is being unlocked. */
#if CONFIG_USE_TICK_HOOK == 1U
	if ( ( m_missed_ticks == 0U ) && ( m_tick_hook_function != NULL ) )
	{
		m_tick_hook_function();
	}
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_PRIORITY_GET == 1U
BF_Lib::MBASE OS_Tasker::Get_Priority( OS_Task* task )
{
	BF_Lib::MBASE return_priority;

	Enter_Critical();
	return_priority = task->m_priority;
	Exit_Critical();

	return ( return_priority );
}

#endif	// CONFIG_INCLUDE_TASK_PRIORITY_GET == 1
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_PRIORITY_SET == 1U
void OS_Tasker::Set_Priority( BF_Lib::MBASE new_priority, OS_Task* task )
{
	bool yield_required = false;

	BF_Lib::MBASE old_priority;

	if ( new_priority >= OS_TASK_PRIORITY_MAX )
	{
		new_priority = OS_TASK_PRIORITY_MAX - 1U;
	}

	Enter_Critical();

#if CONFIG_USE_MUTEXES == 1U
	old_priority = task->m_base_priority;
#else
	old_priority = task->m_priority;
#endif

	if ( old_priority != new_priority )
	{
		if ( new_priority > old_priority )	// Gotta seriously look at this.
		{
			if ( task != OS_TASKER_current_task )
			{
				yield_required = true;
			}
		}
		else if ( task == OS_TASKER_current_task )
		{
			yield_required = true;
		}
		else	// do nothing
		{}

		// TODO: add MUTEX stuff here!
#if CONFIG_USE_MUTEXES == 1U
		if ( task->m_base_priority == task->m_priority )
		{
			task->m_priority = new_priority;
		}

		task->m_base_priority = new_priority;
#else
		task->m_priority = new_priority;
#endif

		task->m_event_list_item->Set_Value( OS_TASK_PRIORITY_MAX - new_priority );

		if ( m_ready_lists[old_priority]->Contained_Within(
				 task->m_general_list_item ) )
		{
			m_ready_lists[old_priority]->Remove( task->m_general_list_item );
			Add_To_Ready_List( task );
		}

		if ( yield_required )
		{
			Yield();
		}
	}

	Exit_Critical();

}

#endif	// CONFIG_INCLUDE_TASK_PRIORITY_SET == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_SUSPEND == 1U
void OS_Tasker::Suspend( OS_Task* task )
{
	Enter_Critical();

	Remove_From_Any_List( task->m_general_list_item );
	Remove_From_Any_List( task->m_event_list_item );

	m_suspended_list->Add_To_End( task->m_general_list_item );

	Exit_Critical();

	if ( task == OS_TASKER_current_task )
	{
		Yield();
	}
}

#endif	// CONFIG_INCLUDE_TASK_SUSPEND == 1
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Resume( OS_Task* task )
{
	if ( ( task != NULL ) && ( task != OS_TASKER_current_task ) )
	{
		Enter_Critical();

		if ( Is_Task_Suspended( task ) )
		{
			Remove_From_Any_List( task->m_general_list_item );
			Add_To_Ready_List( task );

			if ( task->m_priority >= OS_TASKER_current_task->m_priority )
			{
				Yield();
			}
		}
		Exit_Critical();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Resume_From_ISR( OS_Task* task )
{
	bool yield_required = false;

	if ( Is_Task_Suspended( task ) )
	{
		if ( m_scheduler_suspend == 0U )
		{
			yield_required = ( task->m_priority >=
							   OS_TASKER_current_task->m_priority );
			Remove_From_Any_List( task->m_general_list_item );
			Add_To_Ready_List( task );
		}
		else
		{
			m_pending_ready_list->Add_To_End( task->m_event_list_item );
		}
	}

	return ( yield_required );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Start_Scheduler( void )
{
	// Even if there are other tasks in the same lowest priority we still want
	// to create the idle task. Reasons are: 1. other tasks may block;
	// 2. need idle task to accomodate the idle hook function
	if ( m_ready_lists[OS_TASK_PRIORITY_LOW_IDLE_TASK]->Is_Empty() )
	{
		Create_Task( &Idle_Task, OS_TASKER_IDLE_TASK_STACK_SIZE, OS_TASK_PRIORITY_LOW_IDLE_TASK,
					 reinterpret_cast<uint8_t const*>( "Idle Task" ),
					 reinterpret_cast<OS_TASK_PARAM>( nullptr ) );
	}

#if CONFIG_INCLUDE_TASK_DELETE == 1
	m_delete_task = new CR_Tasker( &Delete_Task_Thread, NULL, CR_TASKER_IDLE_PRIORITY,
								   "Delete Task Thread" );
	if ( m_delete_task != nullptr )
	{
		m_delete_task->Suspend();
	}
#endif

	OS_Disable_Interrupts();

	m_scheduler_running = true;

	uC_OS_Tick::Start_OS_Tick();

#ifdef FPU_SUPPORT
	/* Ensure the VFP is enabled - it should be anyway. */
	asmEnableVFP();
#endif
	OS_Start_First_Task_Handler();

	return ( false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Switch_Context( void )
{
	OS_Task* next_task;

#ifdef OS_TASK_MEASURE_RUN_TIME
	static BF_Lib::Timers_uSec::TIME_TD run_time_sample = 0U;
	BF_Lib::Timers_uSec::TIME_TD time_passed;
#endif

	if ( m_scheduler_suspend != 0U )
	{
		m_yield_missed = true;
	}
	else
	{
		while ( m_ready_lists[m_top_ready_priority]->Is_Empty() )
		{
			m_top_ready_priority--;
		}

#ifdef OS_TASKER_VERIFY_TASK_STACK
		BF_ASSERT( OS_TASKER_current_task->Stack_Good() != false );
#endif

		next_task = reinterpret_cast<OS_Task*>( m_ready_lists[m_top_ready_priority]->Get_Next() );

#ifdef OS_TASK_MEASURE_RUN_TIME
		time_passed = BF_Lib::Timers_uSec::Time_Passed( run_time_sample );

		run_time_sample = BF_Lib::Timers_uSec::Get_Time();

		OS_TASKER_current_task->Process_Run_Time( time_passed );

		// Here we can callback a function if they want to be notified when they are being reactivated.
		if ( next_task->m_context_cback != nullptr )
		{
			next_task->m_context_cback( next_task->m_context_param,
										OS_Task::ENTERING );
		}

		if ( OS_TASKER_current_task->m_context_cback != nullptr )
		{
			OS_TASKER_current_task->m_context_cback( OS_TASKER_current_task->m_context_param,
													 OS_Task::EXITING );
		}
#endif

		OS_TASKER_current_task = next_task;

#ifdef OS_TASKER_VERIFY_TASK_STACK
		OS_Task_Mon::Log_Task_Switch( OS_TASKER_current_task );

		if ( ( OS_TASKER_current_task == NULL ) || !OS_TASKER_current_task->Stack_Good() )
		{
			BF_ASSERT(false);
		}
#endif

		if ( m_top_ready_priority == OS_TASK_PRIORITY_1 )
		{
			// Debug_3_On();
		}
		else
		{
			// Debug_3_Off();
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Idle_Task( OS_TASK_PARAM task_param )
{
	if ( task_param != NULL )
	{}
	while ( true )
	{
		Idle_Maintenance_Task();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Idle_Maintenance_Task( void )
{
#if CONFIG_INCLUDE_TASK_DELETE == 1U
	Check_Tasks_Waiting_Termination();
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Add_To_Event_List( OS_List* list, OS_TICK_TD ticks_to_delay )
{
	OS_TICK_TD time_to_wake;

	list->Insert( OS_TASKER_current_task->m_event_list_item );
	Remove_From_Any_List( OS_TASKER_current_task->m_general_list_item );

#if CONFIG_INCLUDE_TASK_SUSPEND == 1U
	if ( ticks_to_delay == OS_TASKER_INDEFINITE_TIMEOUT )
	{
		m_suspended_list->Add_To_End(
			OS_TASKER_current_task->m_general_list_item );
	}
	else
#endif
	{
		time_to_wake = m_current_tick + ticks_to_delay;
		OS_TASKER_current_task->m_general_list_item->Set_Value( time_to_wake );

		if ( time_to_wake < m_current_tick )
		{
			// will wake up after overflow
			m_overflow_delayed_task_list->Insert(
				OS_TASKER_current_task->m_general_list_item );
		}
		else
		{
			// will wake up before overflow
			m_delayed_task_list->Insert(
				OS_TASKER_current_task->m_general_list_item );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Remove_From_Event_List( OS_List* list )
{
	bool priority_change = false;
	OS_Task* unblocked_task;

	unblocked_task = reinterpret_cast<OS_Task*>( list->Get_Head_Entry() );

	if ( unblocked_task != nullptr )
	{
		Remove_From_Any_List( unblocked_task->m_event_list_item );

		if ( m_scheduler_suspend == 0U )
		{
			Remove_From_Any_List( unblocked_task->m_general_list_item );
			Add_To_Ready_List( unblocked_task );
		}
		else
		{
			m_pending_ready_list->Add_To_End( unblocked_task->m_event_list_item );
		}

		if ( unblocked_task->m_priority >= OS_TASKER_current_task->m_priority )
		{
			priority_change = true;
		}
		else
		{
			priority_change = false;
		}
	}

	return ( priority_change );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Check_For_Timeout( OS_TASKER_TIME_STRUCT* timer_struct,
								   OS_TICK_TD* ticks_to_wait )
{
	bool timeout;

	Enter_Critical();
	if ( *ticks_to_wait == OS_TASKER_INDEFINITE_TIMEOUT )
	{
		timeout = false;
	}
	else if ( ( m_tick_overflows != static_cast<OS_TICK_TD>
				( timer_struct->overflow_ticks_snapshot ) ) &&
			  ( m_current_tick >= timer_struct->current_ticks_snapshot ) )
	{
		timeout = true;
	}
	else if ( ( m_current_tick - timer_struct->current_ticks_snapshot ) <
			  *ticks_to_wait )
	{
		*ticks_to_wait -= ( m_current_tick
							- timer_struct->current_ticks_snapshot );
		Get_Current_Time( timer_struct );
		timeout = false;
	}
	else
	{
		timeout = true;
	}

	Exit_Critical();
	return ( timeout );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_SUSPEND == 1U
bool OS_Tasker::Is_Task_Suspended( OS_Task* task )
{
	bool task_suspended = false;

	/* Is the task we are attempting to resume actually in the
	   suspended list? */
	if ( m_suspended_list->Contained_Within( task->m_general_list_item ) != false )
	{
		/* Has the task already been resumed from within an ISR? */
		if ( !m_pending_ready_list->Contained_Within( task->m_event_list_item ) )
		{
			/* Is it in the suspended list because it is in the
			   Suspended state?  It is possible to be in the suspended
			   list because it is blocked on a task with no timeout
			   specified. */
			if ( m_pending_ready_list->Not_Contained( task->m_event_list_item ) )
			{
				task_suspended = true;
			}
		}
	}

	return ( task_suspended );
}

#endif	// CONFIG_INCLUDE_TASK_SUSPEND == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Check_Delayed_Tasks( void )
{
	OS_Task* temp_task = nullptr;

	temp_task = reinterpret_cast<OS_Task*>( m_delayed_task_list->Get_Head_Entry() );

	while ( ( temp_task != NULL ) && !( m_current_tick < ( temp_task->m_general_list_item->Get_Value() ) ) )
	// Rely on the shortcircuit of the compiler.
	{
		Remove_From_Any_List( temp_task->m_general_list_item );
		Remove_From_Any_List( temp_task->m_event_list_item );

		Add_To_Ready_List( temp_task );

		temp_task = reinterpret_cast<OS_Task*>( m_delayed_task_list->Get_Head_Entry() );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_USE_MUTEXES == 1U
void OS_Tasker::PriorityInherit( OS_Task* const pMutexHolder )
{
	OS_Task* const pHolderTask = pMutexHolder;

	if ( pHolderTask->m_priority < OS_TASKER_current_task->m_priority )
	{
		pHolderTask->m_event_list_item->Set_Value( OS_TASK_PRIORITY_MAX
												   - OS_TASKER_current_task->m_priority );
		if ( m_ready_lists[pHolderTask->m_priority]->Contained_Within(
				 pHolderTask->m_general_list_item ) )
		{
			Remove_From_Any_List( pHolderTask->m_general_list_item );
			pHolderTask->m_priority = OS_TASKER_current_task->m_priority;
			Add_To_Ready_List( pHolderTask );
		}
		else
		{
			pHolderTask->m_priority = OS_TASKER_current_task->m_priority;
		}
	}
}

#endif	// CONFIG_USE_MUTEXES == 1
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_USE_MUTEXES == 1U
void OS_Tasker::PriorityDisinherit( OS_Task* const pMutexHolder )
{
	OS_Task* const pHolderTask = pMutexHolder;

	if ( pMutexHolder != NULL )
	{
		if ( pHolderTask->m_priority != pHolderTask->m_base_priority )
		{
			Remove_From_Any_List( pHolderTask->m_general_list_item );

			pMutexHolder->m_priority = pMutexHolder->m_base_priority;
			pHolderTask->m_event_list_item->Set_Value( OS_TASK_PRIORITY_MAX
													   - pHolderTask->m_priority );
			Add_To_Ready_List( pHolderTask );

		}
	}
}

#endif	// CONFIG_USE_MUTEXES == 1
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_DELETE == 1U
void OS_Tasker::Check_Tasks_Waiting_Termination( void )
{
	if ( m_num_tasks_to_delete > 0U )
	{
		bool is_empty;

		Suspend_Scheduler();

		is_empty = m_waiting_termination_list->Is_Empty();

		Resume_Scheduler();

		if ( !is_empty )
		{
			OS_Task* task_to_delete;

			Enter_Critical();

			task_to_delete =
				reinterpret_cast<OS_Task*>( m_waiting_termination_list->Get_Head_Entry() );

			Remove_From_Any_List( task_to_delete->m_general_list_item );

			m_num_tasks--;

			m_num_tasks_to_delete--;

			OS_Task_Mon::Unregister_Task( task_to_delete );

			Exit_Critical();

			delete task_to_delete;
		}
	}
}

#endif	// CONFIG_INCLUDE_TASK_DELETE == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_DELETE == 1

void OS_Tasker::Delete_Task_Thread( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
{
	Check_Tasks_Waiting_Termination();
	if ( m_delete_task != nullptr )
	{
		m_delete_task->Suspend();
	}
}

#endif
