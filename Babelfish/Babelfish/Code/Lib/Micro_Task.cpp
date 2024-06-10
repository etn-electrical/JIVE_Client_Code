/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *  @deprecated The micro_task is no longer used and is deprecated.
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Watchdog.h"
#include "Micro_Task.h"
#include "Babelfish_Assert.h"
namespace BF_Lib
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#ifndef MAX_NUMBER_OF_MICRO_TASKS
#define MAX_NUMBER_OF_MICRO_TASKS       10
#endif

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

bool Micro_Task::m_tasks_running = false;

Micro_Task::MICRO_TASK_STRUCT_TD Micro_Task::m_tasks[MAX_NUMBER_OF_MICRO_TASKS];
uint8_t Micro_Task::m_task_count = 0U;
uC_Watchdog* Micro_Task::m_task_watchdog_bone;
uint8_t Micro_Task::m_task_max;
OS_Task* Micro_Task::m_os_task = reinterpret_cast<OS_Task*>( nullptr );
OS_Task* Micro_Task::m_os_fallback_task = reinterpret_cast<OS_Task*>( nullptr );

// extern bool reset_now; //unused
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Micro_Task::Micro_Task( uint16_t micro_task_os_stack_size )
{
	Init();

	m_os_task = OS_Tasker::Create_Task( &Run, micro_task_os_stack_size,
										static_cast<uint8_t>( OS_TASK_PRIORITY_1 ),
										reinterpret_cast<uint8_t const*>( "Micro Task" ),
										reinterpret_cast<OS_TASK_PARAM>( nullptr ) );
	m_os_fallback_task = OS_Tasker::Create_Task( &Run_Fallback, MICRO_TASK_FALLBACK_STACK_SIZE,
												 static_cast<uint8_t>
												 ( OS_TASK_PRIORITY_LOW_IDLE_TASK ),
												 reinterpret_cast<uint8_t const*>(
													 "Micro Fallback" ),
												 reinterpret_cast<OS_TASK_PARAM>( nullptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Init( void )
{
	m_tasks_running = false;

	for ( uint_fast8_t i = 0U;
		  i < static_cast<uint_fast8_t>( MAX_NUMBER_OF_MICRO_TASKS );
		  i++ )
	{
		m_tasks[i].next = &m_tasks[0];
		m_tasks[i].task_func = &Dummy_Task;
		m_tasks[i].task_param = reinterpret_cast<Micro_Task::MICRO_TASK_STRUCT_TD*>( nullptr );
	}
	m_task_count = 0U;

	Add_Task( &Last_Task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Add_Task( MICRO_TASK_FUNC_TD* task_to_add,
						   MICRO_TASK_FUNC_PARAM_TD task_param )
{
	Push_TGINT();

	// In other words - is the new task already in the list?  If not then add it.
	if ( Find_Task( task_to_add, task_param ) >= m_task_count )
	{
		BF_ASSERT( m_task_count < static_cast<uint8_t>( MAX_NUMBER_OF_MICRO_TASKS ) );
		if ( ( m_task_count < static_cast<uint8_t>( MAX_NUMBER_OF_MICRO_TASKS ) ) &&
			 ( m_task_count != 0U ) )
		{
			m_tasks[m_task_count - 1U].next = &m_tasks[m_task_count];
		}
		else
		{	/* do nothing*/
		}

		if ( m_task_count < static_cast<uint8_t>( MAX_NUMBER_OF_MICRO_TASKS ) )
		{
			m_tasks[m_task_count].task_func = task_to_add;
			m_tasks[m_task_count].task_param = task_param;
			m_tasks[m_task_count].next = &m_tasks[0];
		}

		if ( m_task_count > m_task_max )
		{
			m_task_max = m_task_count;
		}

		m_task_count++;
	}

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Last_Task( MICRO_TASK_FUNC_PARAM_TD task_param )
{
	m_task_watchdog_bone->Feed_Dog();

	// Since we stole the idle task we need to give the OS_Tasker some love back.  We need to
	// Call its maintenance task.
	// This task promises not to block.
	OS_Tasker::Idle_Maintenance_Task();
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'task_param' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Micro_Task::Destroy_Task( MICRO_TASK_FUNC_TD* task_to_destroy,
								  MICRO_TASK_FUNC_PARAM_TD task_param )
{
	uint_fast8_t i;

	Push_TGINT();

	i = Find_Task( task_to_destroy, task_param );

	// make sure this is our task
	if ( i < m_task_count )
	{
		while ( i < ( static_cast<uint_fast8_t>( m_task_count ) - 1U ) )
		{
			m_tasks[i].task_func = m_tasks[i + 1U].task_func;
			m_tasks[i].task_param = m_tasks[i + 1U].task_param;
			i++;
		}
		m_tasks[i - 1U].next = &m_tasks[0];
		m_tasks[i].next = &m_tasks[0];
		m_tasks[i].task_func = &Dummy_Task;
		m_tasks[i].task_param = reinterpret_cast<Micro_Task::MICRO_TASK_STRUCT_TD*>( nullptr );
		m_task_count--;
	}

	Pop_TGINT();

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
	MICRO_TASK_FUNC_TD* temp_task;
	MICRO_TASK_FUNC_PARAM_TD task_param;

	m_tasks_running = true;
	m_task_watchdog_bone = new uC_Watchdog();

	current_task = &m_tasks[0];
	temp_task = current_task->task_func;
	task_param = current_task->task_param;

	// coverity[no_escape]
	while ( true )
	{		// ******************* BEGIN of MAIN LOOP ************************
		// Debug_2_On();
		// Debug_2_On();
		// Debug_2_On();
		////		(*current_task->task_func)( current_task->task_param );
		( *temp_task )( task_param );
		// Debug_2_Off();
		// Debug_2_Off();
		// Debug_2_Off();
		Push_TGINT();

		current_task = static_cast<MICRO_TASK_STRUCT_TD*>( current_task->next );
		temp_task = current_task->task_func;
		task_param = current_task->task_param;

		Pop_TGINT();
	}		// ******************** END of MAIN LOOP *************************
			/**
			 * @remark Coding Standard Deviation:
			 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'param' not referenced
			 * @n PCLint:
			 * @n Justification: In some cases we are not using a variable.  Rather than create
			 * confusing code where we assign the param to itself which could imply an error or
			 * peppering the code with lint exceptions we mark a variable as being unused using this
			 * template which clearly indicates our intention.
			 */
			/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Micro_Task::Run_Fallback( OS_TASK_PARAM param )
{
	// coverity[no_escape]
	while ( true )
	{
		Last_Task( reinterpret_cast<MICRO_TASK_FUNC_PARAM_TD>( 0U ) );
	}
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'param' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
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
uint8_t Micro_Task::Find_Task( MICRO_TASK_FUNC_TD* task_to_find,
							   MICRO_TASK_FUNC_PARAM_TD task_param )
{
	uint_fast8_t i = 0U;
	MICRO_TASK_STRUCT_TD* temp_task_ptr = m_tasks;

	while ( ( i < m_task_count ) &&
			( !( ( temp_task_ptr->task_func == task_to_find ) &&
				 ( temp_task_ptr->task_param == task_param ) ) ) )
	{
		temp_task_ptr++;
		i++;
	}

	return ( static_cast<uint8_t>( i ) );
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
	BF_Lib::SPLIT_UINT16 task_usage_percent;

	task_usage_percent.u16 = m_task_max;
	task_usage_percent.u16 *= 100U;
	task_usage_percent.u16 = task_usage_percent.u16
		/ static_cast<uint16_t>( MAX_NUMBER_OF_MICRO_TASKS );

	return ( task_usage_percent.u8[0] );
}

}	/* end namespace BF_Lib for this module */
