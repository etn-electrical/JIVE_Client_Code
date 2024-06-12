/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "OSCR_Mon.h"
#include "OS_Task_Mon.h"
#include "OS_Task.h"
#include "OS_Tasker.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
DCI_Owner* OSCR_Mon::m_cpu_utilization;
DCI_Owner* OSCR_Mon::m_cr_priority_times;
DCI_Owner* OSCR_Mon::m_os_priority_times;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OSCR_Mon::Init( DCI_Owner* cpu_utilization, DCI_Owner* cr_priority_times,
					 DCI_Owner* os_priority_times )
{
	m_cpu_utilization = cpu_utilization;
	m_cr_priority_times = cr_priority_times;
	m_os_priority_times = os_priority_times;

	new CR_Tasker( &Checker_Task, nullptr, CR_TASKER_PRIORITY_0, "OSCR Monitor" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OSCR_Mon::~OSCR_Mon( void )
{
	// destructor not supported for this class.
	BF_ASSERT( false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OSCR_Mon::Checker_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
#ifdef OSCR_MON_PRINT_TASK_DETAILS
#ifdef USE_FULL_RTOS
	OS_Task_Mon::task_report_t os_task_report;
#endif
	CR_Tasker::task_report_t cr_task_report;
	static uint8_t task_index;
#endif
	uint32_t total_time = 0U;
	uint8_t cpu_util_percent;
	static uint32_t total_cr_idle_time;
#ifdef USE_FULL_RTOS
	uint32_t os_priority_run_times[OS_TASK_PRIORITY_MAX];
#endif

	BF_Lib::Unused<CR_TASKER_PARAM>::Okay( param );

	CR_Tasker_Begin( cr_task );

	while ( true )
	{
		// *********** Delay ***********
		CR_Tasker_Delay( cr_task, OSCR_MON_REFRESH_TIME );
#ifdef CR_TASKER_DEBUG
		// Defined here to avoid "Declared but never referenced" warning
		uint32_t cr_priority_run_times[CR_TASKER_MAX_PRIORITIES];

		// Get the latest CR tasker stats.
		CR_Tasker::Tasker_Stats( cr_priority_run_times, total_time );
		if ( m_cr_priority_times != nullptr )
		{
			m_cr_priority_times->Check_In( reinterpret_cast<uint8_t*>( cr_priority_run_times ) );
		}

		// We are going to use this to calculate percent loading of the CPU.
		// We store it temporarily for later.
		total_cr_idle_time = cr_priority_run_times[CR_TASKER_PRIORITY_0];

#ifdef OSCR_MON_PRINT_PRIORITY_LEVEL_TIMES
		Term_Print( "\n\n***** Update - CR_Tasker *****\n" );
		for ( uint8_t priority = CR_TASKER_PRIORITY_0;
			  priority < CR_TASKER_MAX_PRIORITIES; priority++ )
		{
			Term_Print( "Priority %d Time (us): %d\n", priority,
						cr_priority_run_times[priority] );
		}
		Term_Print( "Total Time (us): %d\n", total_time );

		// *********** Delay ***********
		CR_Tasker_Delay( cr_task, REPORT_INTERPRINT_TIME_DELAY );
#endif

#ifdef OSCR_MON_PRINT_TASK_DETAILS
#ifdef OSCR_MON_PRINT_ADVANCED_TASK_DETAILS
		Term_Print(
			"\nTask, Param, Run Cnt, Max(us), Ave(us), Min(us), Total Run Time(us), Priority, Name\n" );
#else
		Term_Print(
			"\nMax(us), Ave(us), Total Run Time(us), Priority, Name\n" );
#endif

		task_index = 0U;
		CR_Tasker::Task_Stats( task_index, cr_task_report );
		while ( cr_task_report.callback != reinterpret_cast<CR_Tasker::CR_TASKER_CBACK>( 0U ) )
		{
#ifdef OSCR_MON_PRINT_ADVANCED_TASK_DETAILS
			Term_Print( "0x%.8X, 0x%.8X, %d, %d, %d, %d, %d, %d, %s\n",
						cr_task_report.callback, cr_task_report.param, cr_task_report.times_run,
						cr_task_report.max_run_time, cr_task_report.filtered_run_time,
						cr_task_report.min_run_time,    cr_task_report.total_run_time,
						cr_task_report.priority, cr_task_report.task_name );
			CR_Tasker_Delay( cr_task, REPORT_INTERPRINT_TIME_DELAY );
#else
			Term_Print( "%d, %d, %d, %d, %s\n",
						cr_task_report.max_run_time, cr_task_report.filtered_run_time,
						cr_task_report.total_run_time, cr_task_report.priority, cr_task_report.task_name );
#endif
			task_index++;
			CR_Tasker::Task_Stats( task_index, cr_task_report );
		}
#endif
#endif

#ifdef USE_FULL_RTOS
		// Get the latest OS tasker stats.
		OS_Task_Mon::Tasker_Stats( os_priority_run_times, total_time );
		if ( m_os_priority_times != nullptr )
		{
			m_os_priority_times->Check_In( reinterpret_cast<uint8_t*>( os_priority_run_times ) );
		}

#ifdef OSCR_MON_PRINT_PRIORITY_LEVEL_TIMES
		Term_Print( "\n***** Update - OS_Tasker *****\n" );
		for ( uint8_t priority = OS_TASK_PRIORITY_LOW_IDLE_TASK;
			  priority < OS_TASK_PRIORITY_MAX; priority++ )
		{
			Term_Print( "Priority %d Time (us): %d\n", priority,
						os_priority_run_times[priority] );
		}

		Term_Print( "Total Time (us): %d\n", total_time );
#endif
#endif

		if ( total_time != 0U )
		{
			total_cr_idle_time = 100U - ( total_cr_idle_time * 100U ) / total_time;
			cpu_util_percent = static_cast<uint8_t>( total_cr_idle_time );

			if ( m_cpu_utilization != nullptr )
			{
				m_cpu_utilization->Check_In( cpu_util_percent );
			}
		}
#ifdef OSCR_MON_PRINT_PRIORITY_LEVEL_TIMES
		Term_Print( "\n***\nCPU Utilization: %d%%\n", cpu_util_percent );
#endif

#ifdef USE_FULL_RTOS
#ifdef OSCR_MON_PRINT_TASK_DETAILS

#ifdef OSCR_MON_PRINT_ADVANCED_TASK_DETAILS
		Term_Print(
			"\nTop Of Stack, Max Stack(%), Run Cnt, Max(us), Ave(us), Min(us), Total Run Time(us), Priority, Name\n" );
#else
		Term_Print(
			"\nMax(us), Ave(us), Total Run Time(us), Priority, Name\n" );
#endif

		CR_Tasker_Delay( cr_task, REPORT_INTERPRINT_TIME_DELAY );

		task_index = 0U;
		OS_Task_Mon::Task_Stats( task_index, os_task_report );
		while ( os_task_report.top_of_stack != nullptr )
		{
#ifdef OSCR_MON_PRINT_ADVANCED_TASK_DETAILS
			Term_Print( "0x%.8X, %14d, %7d, %7d, %7d, %7d, %18d, %8d, %s\n",
						os_task_report.top_of_stack, os_task_report.max_stack_usage, os_task_report.times_run,
						os_task_report.max_run_time, os_task_report.filtered_run_time,
						os_task_report.min_run_time, os_task_report.total_run_time,
						os_task_report.priority, os_task_report.task_name );
			CR_Tasker_Delay( cr_task, REPORT_INTERPRINT_TIME_DELAY );
#else
			Term_Print( "%d, %d, %d, %d, %s\n",
						os_task_report.max_run_time, os_task_report.filtered_run_time,
						os_task_report.total_run_time, os_task_report.priority, os_task_report.task_name );
#endif
			task_index++;
			OS_Task_Mon::Task_Stats( task_index, os_task_report );
		}
#endif
#endif

	}

	CR_Tasker_End();
}
