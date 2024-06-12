/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OS_TASK_MON_H
   #define OS_TASK_MON_H

#include "OS_Task.h"
#include "OS_Tasker.h"
#include "Timers_Lib.h"


/**
 * @brief Class represents task monitor.
 */
class OS_Task_Mon
{
	public:
		static void Init( void );

		/**
		 * @brief Calculates the stack usage.
		 * @details Will return the percentage of stack used by the highest stack usage task.
		 * @return Stack utilization in percent.
		 */
		static uint8_t Fragile_Task_Stack_Usage( void );

		/**
		 * @brief Gets task name.
		 * @warning Do not modify the task name string!
		 * @return Task name.
		 */
		static uint8_t const* Fragile_Task_Name( void );

		/**
		 * @brief Register a task to be monitored.
		 * @param new_task: Given task.
		 */
		static void Register_Task( OS_Task* new_task );

		/**
		 * @brief Unregister a task.
		 * @param old_task: Given task.
		 */
		static void Unregister_Task( OS_Task* old_task );


		struct task_report_t
		{
			OS_STACK_TD* top_of_stack;
			char_t const* task_name;
			uint32_t times_run;
			uint16_t filtered_run_time;
			uint16_t max_run_time;
			uint16_t min_run_time;
			uint32_t total_run_time;
			uint8_t priority;
			uint8_t max_stack_usage;
		};

		/**
		 * @brief Provides the statistics for the tasker.  This is a list of time
		 * spent in each priority level.  Total time spent in all CR Tasks is also returned.
		 * The total time is a summation of all priority task times.
		 * @param priority_run_times: Shall be a pointer to an array of size
		 * CR_TASKER_MAX_PRIORITIES.  This array will be filled with the times starting
		 * with priority 0 up to the highest priority.  Times are in microseconds.
		 * @param total_time: Sum of all priorities.  In microseconds.
		 */
		static void Tasker_Stats( uint32_t* priority_run_times, uint32_t& total_time );

		/**
		 * @brief Provides the statistics for a specific task.  The tasks are
		 * actually contained in a long linked list.  The index then just walks that
		 * number of times through the linked list and returns the stats for that
		 * task.
		 * @param task_num: The number of the task you want from the head.
		 * @param task_report: The destination structure for the task stats.  If the
		 * top_of_stack is nullptr then it is an indication of running to the end
		 * of the linked list and you have no more tasks to get.
		 */
		static void Task_Stats( uint8_t task_num, task_report_t& task_report );


#ifdef OS_TASK_MON_USE_LOGGING
		/**
		 * @brief Log task switch.
		 * @param new_task: Given task.
		 */
		static void Log_Task_Switch( OS_Task* new_task );

#else
		/**
		 * @brief Log task switch.
		 * @param new_task: Given task.
		 */
		inline static void Log_Task_Switch( OS_Task* new_task )
		{}

#endif

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		OS_Task_Mon( const OS_Task_Mon& rhs );
		OS_Task_Mon & operator =( const OS_Task_Mon& object );

		OS_Task_Mon( void );
		~OS_Task_Mon( void );

		/**
		 * @brief Task monitor structure.
		 */
		struct task_list_t
		{
			OS_Task* task;			///< Task.
			uint8_t percent_used;	///< Utilization percentage.
			task_list_t* next;	///< Next structure in list.
		};

		/**
		 * @brief Task monitor log structure.
		 */
		struct log_list_t
		{
			OS_Task* task;
			uint32_t entries;
			uint8_t const* name;
		};

		/**
		 * @brief Task for performing monitor checks.
		 * @param param: Callback parameter.
		 */
		static void Checker_Task( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

		/**
		 * @brief Quick calculation for incrementing the log place.
		 */
		static uint8_t Inc_Log_Entry( uint8_t current_count );

		/**
		 * @brief Number of tasks to trace in the log.
		 */
		static const uint8_t TASK_LOG_LEN = 8U;

		/**
		 * Pick some uneven number so we avoid timer overlap.  Used to measure the
		 * stack at some interval.
		 */
		static const BF_Lib::Timers::TIMERS_TIME_TD OS_TASK_MON_REFRESH_TIME = 977U;

		/**
		 * @brief List of tasks to monitor.
		 */
		static task_list_t* m_task_list;

		/**
		 * @brief Fragile task.
		 */
		static OS_Task* m_fragile_task;

		/**
		 * @brief High water mark for stack utilization.
		 */
		static uint8_t m_high_stack_usage;

		/**
		 * @brief Timer for monitoring.
		 */
		static BF_Lib::Timers* m_mon_timer;
#ifdef OS_TASK_MON_USE_LOGGING
		/**
		 * @brief Array of task monitor logs.
		 */
		static log_list_t m_task_log[TASK_LOG_LEN];

		/**
		 * @brief Index into log array.
		 */
		static uint8_t m_current_log_entry;
#endif
};



#endif
