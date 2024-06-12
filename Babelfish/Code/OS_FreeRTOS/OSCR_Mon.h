/**
 *****************************************************************************************
 *	@file Holds the OS and CR Tasker monitor code.
 *
 *	@brief Provides monitoring of the OS and CR Tasker timing.
 *
 *	@details
 *	In some cases it is also advised to pause the timing of the CR Tasker when it is put
 *	to sleep by the OS.  To support this we have a hook which can be set in the OS to
 *	pause the CR_Tasker.
 *
 *	Add the following function call.  g_cr_task is the pointer to the OS Task which
 *	is holding the CR_Tasker scheduler.  The context action Cback function is also below.
 *	The following function call should be put in the OS Init code of prod spec.
 *
 #if defined( OS_TASK_MEASURE_RUN_TIME ) && defined( CR_TASKER_DEBUG )
    g_cr_task->Context_Action_Cback( Context_Action_Cback );
 #endif
 *
 *	This function should be put in prod spec somewhere.
 *
 #if defined( OS_TASK_MEASURE_RUN_TIME ) && defined( CR_TASKER_DEBUG )
 **********************************************************************************************
   @brief Provides a method of pausing the CR_Tasker task timing monitors so
   that we do not take into account the time spent in the RTOS in the CR_Tasker.
   @param context_param: The parameter which was passed in.  Not used in this function.
   @param context_action: Whether the task is entering or exiting the active RTOS task level.
 **********************************************************************************************
   void Context_Action_Cback( OS_Task::context_action_param_t context_param,
        OS_Task::context_action_t context_action )
   {
    BF_Lib::Unused<OS_Task::context_action_param_t>::Okay( context_param );

    if ( context_action == OS_Task::ENTERING )
    {
        CR_Tasker::Tasker_Resumed();
    }
    else
    {
        CR_Tasker::Tasker_Paused();
    }
   }
 #endif
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OSCR_MON_H
   #define OSCR_MON_H

#include "CR_Tasker.h"
#include "DCI_Owner.h"

/**
 * @brief Class represents task monitor.
 */
class OSCR_Mon
{
	public:
		/**
		 * @brief Initializes the static class OSCR_Mon.  This will cause a CR Task to be
		 * created which is delayed by a delay timer.  See OSCR_MON_REFRESH_TIME.
		 * A typical use of this class is to display to the terminal details about
		 * a task (CR_Task/OS_Task).  Details include task min/max/ave run time, priority,
		 * priority level run time and CPU utlization.  The utilization is calculated
		 * using the idle time of the CR_Tasker since it is the true idle of the system.
		 * DCI paramaters can also optionally be passed in.
		 * @param cpu_utilization: DCI to store the CPU utilization in percent.
		 * @param cr_priority_times: This must be a 32Bit DCI Array of CR_TASK_MAX_PRIORITIES SIZE.
		 * A DCI which is an array of CR_TASK_MAX_PRIORITIES long.
		 * The amount of time spent in each priority level in the given period are stored here.
		 * @param os_priority_times: This must be a 32Bit DCI Array of CR_TASK_MAX_PRIORITIES SIZE.
		 * The DCI which is an array of OS_TASK_MAX_PRIORITIES long.
		 * This also stores the times spent in each priority level in an array.
		 * index 0 contains the lowest priority.
		 *
		 * @note: Example DCI Definitions in the workbook.
		 * @b CR Tasker priority timing list	DCI_CR_PRIORITY_TIMING	DCI_DTYPE_UINT32	4	6	24
		 * @b OS Tasker priority timing list	DCI_OS_PRIORITY_TIMING	DCI_DTYPE_UINT32	4	8	32
		 * @b CPU utilization percentage		DCI_CPU_UTILIZATION		DCI_DTYPE_UINT8		1		1
		 */
		static void Init( DCI_Owner* cpu_utilization = nullptr,
						  DCI_Owner* cr_priority_times = nullptr,
						  DCI_Owner* os_priority_times = nullptr );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		OSCR_Mon( const OSCR_Mon& rhs );
		OSCR_Mon & operator =( const OSCR_Mon& object );

		OSCR_Mon( void );
		~OSCR_Mon( void );

		/**
		 * DCI_Owners for various containers for metrics.
		 */
		static DCI_Owner* m_cpu_utilization;
		static DCI_Owner* m_cr_priority_times;
		static DCI_Owner* m_os_priority_times;

		/// Time in milliseconds between each status retrieval.
		static const OS_TICK_TD OSCR_MON_REFRESH_TIME = 10000U;

		/// This is the time between each print.
		static const OS_TICK_TD REPORT_INTERPRINT_TIME_DELAY = 1U;

		/**
		 * @brief Task for performing the mentioned checks.
		 * I apologize that the function is an ifdef nightmare.  It was done to save space when
		 * not debugging.  Several dependencies were created when these things turn on and off.
		 * @param param: Callback parameter.  Not used.
		 */
		static void Checker_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param );

};



#endif
