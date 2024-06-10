/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "OS_Tasker.h"
#include "OS_Task_Mon.h"
#include "Ram.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
OS_Task_Mon::task_list_t* OS_Task_Mon::m_task_list = nullptr;
OS_Task* OS_Task_Mon::m_fragile_task = nullptr;
uint8_t OS_Task_Mon::m_high_stack_usage = 0U;
BF_Lib::Timers* OS_Task_Mon::m_mon_timer = nullptr;

#ifdef OS_TASK_MON_USE_LOGGING
OS_Task_Mon::log_list_t OS_Task_Mon::m_task_log[TASK_LOG_LEN];
uint8_t OS_Task_Mon::m_current_log_entry;
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Task_Mon::Init( void )
{
	m_mon_timer = new BF_Lib::Timers( &Checker_Task, nullptr,
									  BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY, "OS Task Mon" );
	m_mon_timer->Start( OS_TASK_MON_REFRESH_TIME, true );

#ifdef OS_TASK_MON_USE_LOGGING
	m_current_log_entry = 0U;
	for ( uint_fast16_t i = 0U; i < TASK_LOG_LEN; i++ )
	{
		m_task_log[i].task = nullptr;
		m_task_log[i].entries = 0U;
		m_task_log[i].name = nullptr;
	}
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Task_Mon::~OS_Task_Mon( void )
{
	// Destructor is not supported.
	BF_ASSERT( false );
	// task_list_t* walker;
	//
	// walker = m_task_list;
	// while ( walker != NULL )
	// {
	// Ram::De_Allocate( walker );
	// walker = walker->next;
	// }
	//
	// delete ( m_mon_timer );
}

/*
 *****************************************************************************************
 *		Quick calculation for incrementing the log place.
 *****************************************************************************************
 */
inline uint8_t OS_Task_Mon::Inc_Log_Entry( uint8_t current_count )
{
	current_count++;
	if ( current_count >= TASK_LOG_LEN )
	{
		current_count = 0;
	}

	return ( current_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t OS_Task_Mon::Fragile_Task_Stack_Usage( void )
{
	return ( m_high_stack_usage );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t const* OS_Task_Mon::Fragile_Task_Name( void )
{
	return ( m_fragile_task->m_task_name );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Task_Mon::Register_Task( OS_Task* new_task )
{
	task_list_t* task_mon_list_walker = m_task_list;
	task_list_t* new_item;

	new_item = reinterpret_cast<task_list_t*>( Ram::Allocate( sizeof( task_list_t ) ) );

	new_item->percent_used = 0U;
	new_item->task = new_task;
	new_item->next = nullptr;

	if ( m_task_list == nullptr )
	{
		m_task_list = new_item;
	}
	else
	{
		while ( task_mon_list_walker->next != NULL )
		{
			task_mon_list_walker = task_mon_list_walker->next;
		}
		task_mon_list_walker->next = new_item;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Task_Mon::Unregister_Task( OS_Task* old_task )
{
	task_list_t* task_mon_list_walker = m_task_list;
	task_list_t* item_to_delete;

	if ( m_task_list != NULL )
	{
		if ( m_task_list->task == old_task )
		{
			item_to_delete = m_task_list;
			m_task_list = m_task_list->next;
			Ram::De_Allocate( item_to_delete );
		}
		else
		{
			while ( ( task_mon_list_walker->next != NULL ) && ( task_mon_list_walker->next->task != old_task ) )
			{
				task_mon_list_walker = task_mon_list_walker->next;
			}
			if ( ( task_mon_list_walker->next != NULL ) && ( task_mon_list_walker->next->task == old_task ) )
			{
				item_to_delete = task_mon_list_walker->next;
				task_mon_list_walker->next = task_mon_list_walker->next->next;
				Ram::De_Allocate( item_to_delete );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Task_Mon::Tasker_Stats( uint32_t* priority_run_times, uint32_t& total_time )
{
#ifdef OS_TASK_MEASURE_RUN_TIME
	total_time = 0U;

	for ( uint8_t priority = OS_TASK_PRIORITY_LOW_IDLE_TASK;
		  priority < OS_TASK_PRIORITY_MAX; priority++ )
	{
		total_time += OS_Task::m_priority_time[priority];
		priority_run_times[priority] = OS_Task::m_priority_time[priority];

		OS_Task::m_priority_time[priority] = 0U;
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
void OS_Task_Mon::Task_Stats( uint8_t task_num, task_report_t& task_report )
{
#ifdef OS_TASK_MEASURE_RUN_TIME
	task_list_t* walker;
	OS_Task* task;
	uint_fast8_t task_counter = 0U;

	walker = m_task_list;
	while ( ( task_counter < task_num ) && ( walker != nullptr ) )
	{
		walker = walker->next;
		task_counter++;
	}

	if ( walker != nullptr )
	{
		task = walker->task;
		task_report.top_of_stack = const_cast<OS_STACK_TD*>( task->m_top_of_stack );
		task_report.times_run = task->m_times_run;
		task_report.max_run_time = task->m_max_run_time;
		task_report.filtered_run_time = task->m_run_time.Value();
		task_report.min_run_time = task->m_min_run_time;
		task_report.total_run_time = task->m_total_run_time;
		task_report.priority = task->m_priority;
		task_report.task_name = reinterpret_cast<char const*>( task->m_task_name );
		task_report.max_stack_usage = task->m_max_stack_usage_percent;
		task->m_min_run_time = static_cast<uint16_t>( UINT16_MAX );
		task->m_max_run_time = 0U;
		task->m_times_run = 0U;
		task->m_total_run_time = 0U;
		task->m_run_time.Re_Init();
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#ifdef OS_TASK_MON_USE_LOGGING
void OS_Task_Mon::Log_Task_Switch( OS_Task* new_task )
{
	if ( new_task == m_task_log[m_current_log_entry].task )
	{
		m_task_log[m_current_log_entry].entries++;
	}
	else
	{
		m_task_log[m_current_log_entry].task = new_task;
		m_task_log[m_current_log_entry].entries = 1U;
		m_task_log[m_current_log_entry].name = new_task->m_task_name;
		m_current_log_entry = Inc_Log_Entry( m_current_log_entry );
	}
}

#endif


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Task_Mon::Checker_Task( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	task_list_t* task_walker;

	BF_Lib::Unused<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>::Okay( param );

	task_walker = m_task_list;
	while ( task_walker != NULL )
	{
		task_walker->percent_used = task_walker->task->Stack_Usage_Percent();
		if ( task_walker->percent_used > m_high_stack_usage )
		{
			m_high_stack_usage = task_walker->percent_used;
			m_fragile_task = task_walker->task;
		}
		task_walker = task_walker->next;
	}
}
