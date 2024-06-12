/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *   @deprecated : The micro_task is no longer used and is deprecated.
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Watchdog.h"
#include "Micro_Task.h"
#include "Babelfish_Assert.h"
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define MICRO_TASK_FALLBACK_STACK_SIZE      40

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

bool Micro_Task::m_tasks_running = false;

MICRO_TASK_STRUCT_TD Micro_Task::m_tasks[MAX_NUMBER_OF_MICRO_TASKS];
uint8_t Micro_Task::m_task_count = 0U;
uC_Watchdog* Micro_Task::m_task_watchdog_bone;
uint8_t Micro_Task::m_task_max;
#ifdef OPERATING_SYSTEM
OS_Task* Micro_Task::m_os_task = 0U;
OS_Task* Micro_Task::m_os_fallback_task = 0U;
#endif

extern bool reset_now;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Micro_Task::Micro_Task( uint16_t micro_task_os_stack_size )
{
	Init();

#ifdef OPERATING_SYSTEM
	m_os_task = OS_Tasker::Create_Task( Run, micro_task_os_stack_size,
										OS_TASK_PRIORITY_1, "Micro Task", NULL );
	m_os_fallback_task = OS_Tasker::Create_Task( Run_Fallback, MICRO_TASK_FALLBACK_STACK_SIZE,
												 OS_TASK_PRIORITY_LOW_IDLE_TASK, "Micro Fallback", NULL );
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Init( void )
{
	m_tasks_running = false;

	for ( uint_fast8_t i = 0U; i < MAX_NUMBER_OF_MICRO_TASKS; i++ )
	{
		m_tasks[i].next = &m_tasks[0];
		m_tasks[i].task_func = Dummy_Task;
		m_tasks[i].task_param = NULL;
	}
	m_task_count = 0U;

	Add_Task( Last_Task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Add_Task( MICRO_TASK_FUNC_TD* task_to_add, MICRO_TASK_FUNC_PARAM_TD task_param )
{
#ifdef OPERATING_SYSTEM
	OS_Tasker::Enter_Critical();
#else
	GINT_TDEF temp_gint;

	Push_GINT( temp_gint );
#endif

	// In other words - is the new task already in the list?  If not then add it.
	if ( Find_Task( task_to_add, task_param ) >= m_task_count )
	{
		if ( ( m_task_count < MAX_NUMBER_OF_MICRO_TASKS ) &&
			 ( m_task_count != 0 ) )
		{
			m_tasks[m_task_count - 1].next = &m_tasks[m_task_count];
		}
		BF_ASSERT( m_task_count < MAX_NUMBER_OF_MICRO_TASKS );

		m_tasks[m_task_count].task_func = task_to_add;
		m_tasks[m_task_count].task_param = task_param;
		m_tasks[m_task_count].next = &m_tasks[0];

		if ( m_task_count > m_task_max )
		{
			m_task_max = m_task_count;
		}

		m_task_count++;
	}

#ifdef OPERATING_SYSTEM
	OS_Tasker::Exit_Critical();
#else
	Pop_GINT( temp_gint );
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Last_Task( MICRO_TASK_FUNC_PARAM_TD task_param )
{
	m_task_watchdog_bone->Feed_Dog();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Micro_Task::Destroy_Task( MICRO_TASK_FUNC_TD* task_to_destroy, MICRO_TASK_FUNC_PARAM_TD task_param )
{
	uint_fast8_t i;

#ifdef OPERATING_SYSTEM
	OS_Tasker::Enter_Critical();
#else
	GINT_TDEF temp_gint;

	Push_GINT( temp_gint );
#endif

	i = Find_Task( task_to_destroy, task_param );

	// make sure this is our task
	if ( i < m_task_count )
	{
		while ( i < ( m_task_count - 1 ) )
		{
			m_tasks[i].task_func = m_tasks[i + 1].task_func;
			m_tasks[i].task_param = m_tasks[i + 1].task_param;
			i++;
		}
		m_tasks[i - 1].next = &m_tasks[0];
		m_tasks[i].next = &m_tasks[0];
		m_tasks[i].task_func = Dummy_Task;
		m_tasks[i].task_param = NULL;
		m_task_count--;
	}

#ifdef OPERATING_SYSTEM
	OS_Tasker::Exit_Critical();
#else
	Pop_GINT( temp_gint );
#endif

	return ( m_task_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Run( OS_TASK_PARAM param )
{
	static MICRO_TASK_STRUCT_TD* current_task;
	MICRO_TASK_FUNC_TD* task;
	MICRO_TASK_FUNC_PARAM_TD task_param;

#ifndef OPERATING_SYSTEM
	GINT_TDEF temp_gint;
#endif

	m_tasks_running = true;
	m_task_watchdog_bone = new uC_Watchdog();

	current_task = &m_tasks[0];
	task = current_task->task_func;
	task_param = current_task->task_param;

	while ( true )
	{	// ******************* BEGIN of MAIN LOOP ************************
		// (*current_task->task_func)( current_task->task_param );
		( *task )( task_param );
#ifdef OPERATING_SYSTEM
		OS_Tasker::Enter_Critical();
#else
		Push_GINT( temp_gint );
#endif
		current_task = ( MICRO_TASK_STRUCT_TD* )current_task->next;
		task = current_task->task_func;
		task_param = current_task->task_param;
#ifdef OPERATING_SYSTEM
		OS_Tasker::Exit_Critical();
#else
		Pop_GINT( temp_gint );
#endif
	}	// ******************** END of MAIN LOOP *************************
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Run_Fallback( OS_TASK_PARAM param )
{
	while ( true )
	{
		Last_Task( 0 );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Shutdown( void )
{
	Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Micro_Task::Find_Task( MICRO_TASK_FUNC_TD* task_to_find, MICRO_TASK_FUNC_PARAM_TD task_param )
{
	uint_fast8_t i = 0U;
	MICRO_TASK_STRUCT_TD* temp_task_ptr = m_tasks;

	while ( ( i < m_task_count ) &&
			!( ( temp_task_ptr->task_func == task_to_find ) &&
			   ( temp_task_ptr->task_param == task_param ) ) )
	{
		temp_task_ptr++;
		i++;
	}

	return ( i );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Micro_Task::Is_uTask_Running( void )
{
	return ( m_tasks_running );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Micro_Task::Get_uTask_Stack_Usage( void )
{
	SPLIT_UINT16 task_usage_percent;

	task_usage_percent.u16 = m_task_max;
	task_usage_percent.u16 *= 100;
	task_usage_percent.u16 = task_usage_percent.u16 / MAX_NUMBER_OF_MICRO_TASKS;

	return ( task_usage_percent.u8[0] );
}
