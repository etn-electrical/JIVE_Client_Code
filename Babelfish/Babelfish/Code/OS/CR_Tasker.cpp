/**
 **********************************************************************************************
 *  @file            CR_Tasker.cpp implementation File for low toolkit coroutine tasking
 *                   functionality
 *
 *  @brief           The file shall include the definitions of all the functions required for
 *                   coroutine tasking functionality and which are declared in corresponding
 *                   header file
 *  @details
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "CR_Tasker.h"
#include "StdLib_MV.h"

/*
 *****************************************************************************
 *		Constants
 *****************************************************************************
 */
// The initial state of the co-routine when it is created.
#define CRT_INITIAL_STATE           0U
// The initial state of the co-routine when it is created.
// This allows us to create very simple tasks to run in the idle priority level.
#define CRT_STATELESS_CONFIG        ~static_cast<uint_fast16_t>( 0U )

// #ifdef CR_TASKER_DEBUG
const char_t CR_Tasker::NULL_TASK_NAME[] = { "Null" };
// #endif

#ifndef CR_TASKER_INIT_TICK_COUNT
// The purpose of this define constant is to set the initial value of the tick counters
// to something close to overflow.  This causes the timers to overflow right away.  Hopefully causing issues to appear
// faster.
	#define CR_TASKER_INIT_TICK_COUNT       static_cast<OS_TICK_TD>( 0 )
#endif

/*
 *****************************************************************************
 *		Static Variables
 *****************************************************************************
 */
OS_List* CR_Tasker::m_ready_cr_lists[CR_TASKER_MAX_PRIORITIES];
OS_List* CR_Tasker::m_pending_cr_ready_list;
OS_List* CR_Tasker::m_delayed_cr_list1;
OS_List* CR_Tasker::m_delayed_cr_list2;
OS_List* CR_Tasker::m_delayed_cr_list;
OS_List* CR_Tasker::m_delayed_overflow_cr_list;
uC_Watchdog* CR_Tasker::m_task_watchdog_bone;
bool CR_Tasker::m_cr_tasker_running = false;

CR_Tasker* CR_Tasker::m_current_coroutine = nullptr;
BF_Lib::MBASE CR_Tasker::m_cr_top_ready_priority;
OS_TICK_TD CR_Tasker::m_cr_tick_count = CR_TASKER_INIT_TICK_COUNT;
OS_TICK_TD CR_Tasker::m_cr_interrupt_tick_count = CR_TASKER_INIT_TICK_COUNT;
OS_TICK_TD CR_Tasker::m_cr_last_tick_count = CR_TASKER_INIT_TICK_COUNT;
OS_TICK_TD CR_Tasker::m_cr_passed_ticks;
CR_Tasker* CR_Tasker::m_last_task;

#ifdef CR_TASKER_DEBUG
CR_Tasker* CR_Tasker::m_debug_task_head = nullptr;
BF_Lib::Timers_uSec::TIME_TD CR_Tasker::m_run_sample_time = 0U;
uint32_t CR_Tasker::m_priority_time[CR_TASKER_MAX_PRIORITIES] = { 0 };
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CR_Tasker::CR_Tasker( CR_TASKER_CBACK cr_func_callback, CR_TASKER_PARAM param,
					  uint8_t priority, char_t const* task_name ) :
	m_state( 0U ),
	m_crt_callback( nullptr ),
	m_param( nullptr ),
	m_generic_list_item( nullptr ),
	m_event_list_item( nullptr ),
	m_priority( 0U )
#ifdef CR_TASKER_DEBUG
	,
	m_task_name( nullptr ),
	m_max_run_time( 0U ),
	m_min_run_time( 0U ),
	m_total_run_time( 0U ),
	m_times_run( 0U ),
	m_debug_task_next( nullptr )
#endif
{
	Init_CRT( cr_func_callback, param, priority, CRT_INITIAL_STATE, task_name );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CR_Tasker::CR_Tasker( CR_TASKER_LITE_CBACK cr_func_callback,
					  CR_TASKER_PARAM param, char_t const* task_name ) :
	m_state( 0U ),
	m_crt_callback( nullptr ),
	m_param( nullptr ),
	m_generic_list_item( nullptr ),
	m_event_list_item( nullptr ),
	m_priority( 0U )
#ifdef CR_TASKER_DEBUG
	,
	m_task_name( nullptr ),
	m_max_run_time( 0U ),
	m_min_run_time( 0U ),
	m_total_run_time( 0U ),
	m_times_run( 0U ),
	m_debug_task_next( nullptr )
#endif
{
	Init_CRT( reinterpret_cast<CR_TASKER_CBACK>( cr_func_callback ), param,
			  CR_TASKER_IDLE_PRIORITY, CRT_STATELESS_CONFIG, task_name );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::Init_CRT( CR_TASKER_CBACK cr_func_callback, CR_TASKER_PARAM param,
						  uint8_t priority, uint_fast16_t init_state, char_t const* task_name )
{
	Push_TGINT();
	// If m_current_coroutine is NULL then this is the first co-routine to
	// be created and the co-routine data structures needs initializing.
	if ( m_current_coroutine == NULL )
	{
		m_current_coroutine = this;

		for ( uint_fast8_t priority_index = 0U; priority_index < CR_TASKER_MAX_PRIORITIES;
			  priority_index++ )
		{
			m_ready_cr_lists[priority_index] = new OS_List();

		}

		m_delayed_cr_list1 = new OS_List();

		m_delayed_cr_list2 = new OS_List();

		m_pending_cr_ready_list = new OS_List();

		m_delayed_cr_list = m_delayed_cr_list1;
		m_delayed_overflow_cr_list = m_delayed_cr_list2;

		m_task_watchdog_bone = new uC_Watchdog();

		// Yes - I know that the following are recursiveish.
		new CR_Tasker( &Last_Task, nullptr,
					   CR_TASKER_IDLE_PRIORITY, "CR Tasker Last Task" );

#ifdef CR_TASKER_DEBUG
		for ( uint_fast8_t i = 0U; i < CR_TASKER_MAX_PRIORITIES; i++ )
		{
			m_priority_time[i] = 0U;
		}
#endif
	}
	Pop_TGINT();

	// Check the priority is within limits.
	if ( priority >= CR_TASKER_MAX_PRIORITIES )
	{
		priority = static_cast<uint8_t>( CR_TASKER_MAX_PRIORITIES - 1U );
	}

	// Fill out the co-routine control block from the function parameters.
	m_state = init_state;
	m_priority = priority;
	m_param = param;
	m_crt_callback = cr_func_callback;

	// Initialise all the other co-routine control block parameters.
	m_generic_list_item = new OS_List_Item();

	m_event_list_item = new OS_List_Item();

	// Set the co-routine control block as a link back from the OS_List_Item.
	// This is so we can get back to the containing CRCB from a generic item
	// in a list.
	m_generic_list_item->Set_Owner( this );
	m_event_list_item->Set_Owner( this );

	// Event lists are always in priority order.
	m_event_list_item->Set_Value(
		static_cast<OS_LIST_VALUE_TD>( CR_TASKER_MAX_PRIORITIES ) - m_priority );

	Push_TGINTS();		// using the same variable as before.
	// Now the co-routine has been initialised it can be added to the ready
	// list at the correct priority.
	// We had an issue here where a CR_Tasker could be constructed from an interrupt/RTOS task level.
	// This would call Add to ready queueu which was a nono
	// because it would add to the ready queue.  So we create a list of tasks to be added into the
	// ready list for later when the CR_Tasker is ready to run.
	// Add_To_Ready_Queue( this );
	m_pending_cr_ready_list->Insert( m_event_list_item );
	Pop_TGINT();

	if ( task_name != NULL )
	{
		m_task_name = task_name;
	}
	else
	{
		m_task_name = NULL_TASK_NAME;
	}

#ifdef CR_TASKER_DEBUG

	m_max_run_time = 0U;
	m_min_run_time = static_cast<uint16_t>( UINT16_MAX );
	m_total_run_time = 0U;
	m_times_run = 0U;

	if ( m_debug_task_head == NULL )
	{
		m_debug_task_head = this;
	}
	else
	{
		CR_Tasker* walker = m_debug_task_head;
		while ( walker->m_debug_task_next != NULL )
		{
			walker = walker->m_debug_task_next;
		}
		walker->m_debug_task_next = this;
	}
	m_debug_task_next = reinterpret_cast<CR_Tasker*>( nullptr );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CR_Tasker::~CR_Tasker( void )
{
	m_crt_callback = nullptr;
	m_param = nullptr;
	delete m_generic_list_item;
	delete m_event_list_item;

#ifdef CR_TASKER_DEBUG
	if ( m_debug_task_head == this )
	{
		m_debug_task_head = m_debug_task_next;
	}
	else
	{
		CR_Tasker* task_itr = m_debug_task_head;
		while ( task_itr->m_debug_task_next != this )
		{
			task_itr = task_itr->m_debug_task_next;
		}
		task_itr->m_debug_task_next = m_debug_task_next;
	}
	m_debug_task_next = reinterpret_cast<CR_Tasker*>( nullptr );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::Add_To_Delayed_List( OS_TICK_TD time_to_delay, OS_List* event_list )
{
	OS_TICK_TD time_to_wake;

	// If we do not have a time to delay then just get out of here.
	// This should effectively cause it to run on the next loop through.
	if ( time_to_delay > 0U )
	{
		/* Calculate the time to wake - this may overflow but this is
		   not a problem. */
		time_to_wake = m_cr_tick_count + time_to_delay;

		/* We must remove ourselves from the ready list before adding
		   ourselves to the blocked list as the same list item is used for
		   both lists. */
		Remove_From_Any_List( m_current_coroutine->m_generic_list_item );

		/* The list item will be inserted in wake time order. */
		m_current_coroutine->m_generic_list_item->Set_Value( time_to_wake );

		if ( time_to_wake < m_cr_tick_count )
		{
			/* Wake time has overflowed.  Place this item in the
			   overflow list. */
			m_delayed_overflow_cr_list->Insert( m_current_coroutine->m_generic_list_item );
		}
		else
		{
			/* The wake time has not overflowed, so we can use the
			   current block list. */
			m_delayed_cr_list->Insert( m_current_coroutine->m_generic_list_item );
		}

		if ( event_list != nullptr )
		{
			/* Also add the co-routine to an event list.  If this is done then the
			   function must be called with interrupts disabled. */
			event_list->Insert( m_current_coroutine->m_event_list_item );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::Check_Pending_Ready_List( void )
{
	CR_Tasker* unblocked_cr;

	/* Are there any co-routines waiting to get moved to the ready list?  These
	   are co-routines that have been readied by an ISR.  The ISR cannot access
	   the	ready lists itself. */
	while ( !m_pending_cr_ready_list->Is_Empty() )
	{
		/* The pending ready list can be accessed by an ISR. */
		Push_TGINT();

		unblocked_cr = static_cast<CR_Tasker*>( m_pending_cr_ready_list->Get_Head_Entry() );
		Remove_From_Any_List( unblocked_cr->m_event_list_item );

		Pop_TGINT();

		Remove_From_Any_List( unblocked_cr->m_generic_list_item );
		Add_To_Ready_Queue( unblocked_cr );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::Check_Delayed_List( void )
{
	CR_Tasker* temp_task;
	OS_List* temp_list;

	m_cr_passed_ticks = Get_Interrupt_Tick_Count() - m_cr_last_tick_count;
	while ( m_cr_passed_ticks > 0U )
	{
		m_cr_tick_count++;
		m_cr_passed_ticks--;

		/* If the tick count has overflowed we need to swap the ready lists. */
		if ( m_cr_tick_count == 0U )
		{
			/* Tick count has overflowed so we need to swap the delay lists.  If there are
			   any items in delayed_cr_list here then there is an error! */
			temp_list = m_delayed_cr_list;
			m_delayed_cr_list = m_delayed_overflow_cr_list;
			m_delayed_overflow_cr_list = temp_list;
		}

		/* See if this tick has made a timeout expire. */
		while ( m_delayed_cr_list->Is_Empty() == false )
		{
			temp_task = reinterpret_cast<CR_Tasker*>( m_delayed_cr_list->Get_Head_Entry() );

			if ( m_cr_tick_count < temp_task->m_generic_list_item->Get_Value() )
			{
				// I don't typically like a break out of a while loop like this.
				// In this case though it may be better to follow the lead of FreeRTOS.
				// What is happening here is if we find that the first task in the list
				// is not expired yet then we just jump out from here (of the while loop at least).
				// A timeout has not yet expired.
				break;
			}

			Push_TGINT();
			/* The event could have occurred just before this critical
			   section.  If this is the case then the generic list item will
			   have been moved to the pending ready list and the following
			   line is still valid.  Also the pvContainer parameter will have
			   been set to NULL so the following lines are also valid. */
			Remove_From_Any_List( temp_task->m_generic_list_item );

			/* Is the co-routine waiting on an event also? */
			if ( temp_task->m_event_list_item->Get_Container() != nullptr )
			{
				Remove_From_Any_List( temp_task->m_event_list_item );
			}
			Pop_TGINT();

			Add_To_Ready_Queue( temp_task );
		}
	}

	m_cr_last_tick_count = m_cr_tick_count;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */ ahmed test
void CR_Tasker::Tick_Event( void )
{
	// We disable interrupts in the situation where this might be called from a regular
	// task or non-interrupt operation.
	Push_TGINT();
	m_cr_interrupt_tick_count++;
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::Scheduler( CR_TASKER_PARAM param )
{
	m_cr_tasker_running = true;
	BF_Lib::Unused<CR_TASKER_PARAM>::Okay( param );

	while ( true )
	{
		RunReadyCR();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::RunReadyCR( void )
{
	/* See if any co-routines readied by events need moving to the ready lists. */
	Check_Pending_Ready_List();

	/* See if any delayed co-routines have timed out. In the past this was
	 * handled by being a dedicated CR_Task.  Now it is just called directly.
	 * Saving some RAM for a task as well as a small bit of processing time.*/
	Check_Delayed_List();

	/* Find the highest priority queue that contains ready co-routines. */
	while ( m_ready_cr_lists[m_cr_top_ready_priority]->Is_Empty() )
	{
		if ( m_cr_top_ready_priority == 0 )
		{
			// We end up here if there are no CR_Tasks.  This is almost incredibly
			// unlikely to happen because even the CR_Tasker has a task in it.  This is to
			// catch it in the unlikely occurrence that it does happen.
			// We just return.
			return;
		}
		--m_cr_top_ready_priority;
	}

#ifdef CR_TASKER_DEBUG
	Push_TGINT();
	m_current_coroutine->Process_Run_Time(
		BF_Lib::Timers_uSec::Time_Passed( m_run_sample_time ) );
	m_run_sample_time = BF_Lib::Timers_uSec::Get_Time();
	Pop_TGINT();
#endif

	/* listGET_OWNER_OF_NEXT_ENTRY walks through the list, so the co-routines
	   of the	same priority get an equal share of the processor time. */
	m_current_coroutine = reinterpret_cast<CR_Tasker*>(
		m_ready_cr_lists[m_cr_top_ready_priority]->Get_Next() );

	// If the coroutine is not going to use the state stuff.
	if ( m_current_coroutine->m_state != CRT_STATELESS_CONFIG )
	{
		/* Call the co-routine. */
		( *m_current_coroutine->m_crt_callback )( m_current_coroutine,
												  m_current_coroutine->m_param );
	}
	else
	{
		/* Call the co-routine. */
		( *( reinterpret_cast<CR_TASKER_LITE_CBACK>( m_current_coroutine->m_crt_callback ) ) )(
			m_current_coroutine->m_param );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CR_Tasker::Remove_From_Event_List( OS_List* event_list )
{
	CR_Tasker* unblocked_cr;
	bool return_status = false;

	/* This function is called from within an interrupt.  It can only access
	   event lists and the pending ready list.  This function assumes that a
	   check has already been made to ensure pxEventList is not empty. */
	unblocked_cr = reinterpret_cast<CR_Tasker*>( event_list->Get_Head_Entry() );

	if ( unblocked_cr != nullptr )
	{
		Remove_From_Any_List( unblocked_cr->m_event_list_item );

		m_pending_cr_ready_list->Insert( unblocked_cr->m_event_list_item );

		if ( unblocked_cr->m_priority >= m_current_coroutine->m_priority )
		{
			return_status = true;
		}
		else
		{
			return_status = false;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CR_Tasker* CR_Tasker::Current_CRTask( void )
{
	CR_Tasker* return_task = reinterpret_cast<CR_Tasker*>( nullptr );

	if ( m_cr_tasker_running == true )
	{
		return_task = m_current_coroutine;
	}

	return ( return_task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::Last_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	BF_Lib::Unused<CR_Tasker*>::Okay( cr_task );
	BF_Lib::Unused<CR_TASKER_PARAM>::Okay( param );

	m_task_watchdog_bone->Feed_Dog();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CR_Tasker::CR_TASKER_CBACK CR_Tasker::Current_CRTask_Callback( void )
{
	CR_TASKER_CBACK return_task_callback = nullptr;

	if ( m_cr_tasker_running == true )
	{
		return_task_callback = m_current_coroutine->m_crt_callback;
	}

	return ( return_task_callback );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CR_TASKER_PARAM CR_Tasker::Current_CRTask_Param( void )
{
	CR_TASKER_PARAM return_task_parameter = nullptr;

	if ( m_cr_tasker_running == true )
	{
		return_task_parameter = m_current_coroutine->m_param;
	}

	return ( return_task_parameter );
}

#ifdef CR_TASKER_DEBUG
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::Process_Run_Time( BF_Lib::Timers_uSec::TIME_TD run_time )
{
	if ( m_times_run != UINT32_MAX )
	{
		m_times_run++;
	}

	m_total_run_time += run_time;

	if ( run_time > m_max_run_time )
	{
		m_max_run_time = static_cast<uint16_t>( run_time );
	}
	if ( run_time < m_min_run_time )
	{
		m_min_run_time = static_cast<uint16_t>( run_time );
	}

	m_run_time.Value( static_cast<uint16_t>( run_time ) );

	if ( m_priority < CR_TASKER_MAX_PRIORITIES )
	{
		m_priority_time[m_priority] += run_time;
	}
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::Tasker_Stats( uint32_t* priority_run_times, uint32_t& total_time )
{
#ifdef CR_TASKER_DEBUG
	total_time = 0U;

	for ( uint8_t priority = CR_TASKER_PRIORITY_0;
		  priority < CR_TASKER_MAX_PRIORITIES; priority++ )
	{
		total_time += m_priority_time[priority];
		priority_run_times[priority] = m_priority_time[priority];

		m_priority_time[priority] = 0;
	}
#else
	BF_Lib::Unused<uint32_t*>::Okay( priority_run_times );
	BF_Lib::Unused<uint32_t>::Okay( total_time );
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CR_Tasker::Task_Stats( uint8_t task_num, task_report_t& task_report )
{
#ifdef CR_TASKER_DEBUG
	CR_Tasker* walker;
	uint_fast8_t task_counter = 0U;

	walker = m_debug_task_head;
	while ( ( task_counter < task_num ) && ( walker != nullptr ) )
	{
		walker = walker->m_debug_task_next;
		task_counter++;
	}

	if ( walker != nullptr )
	{
		task_report.callback = walker->m_crt_callback;
		task_report.param = walker->m_param;
		task_report.times_run = walker->m_times_run;
		task_report.max_run_time = walker->m_max_run_time;
		task_report.filtered_run_time = walker->m_run_time.Value();
		task_report.min_run_time = walker->m_min_run_time;
		task_report.total_run_time = walker->m_total_run_time;
		task_report.priority = walker->m_priority;
		task_report.task_name = walker->m_task_name;

		walker->m_min_run_time = static_cast<uint16_t>( UINT16_MAX );
		walker->m_max_run_time = 0U;
		walker->m_times_run = 0U;
		walker->m_total_run_time = 0U;
		walker->m_run_time.Re_Init();
	}
	else
	{
		BF_Lib::Copy_Val_To_String( reinterpret_cast<uint8_t*>( &task_report ),
									0U, sizeof( task_report_t ) );
	}
#else
	BF_Lib::Unused<uint8_t>::Okay( task_num );
	BF_Lib::Unused<task_report_t>::Okay( task_report );
#endif
}
