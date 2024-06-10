/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This contains the implementations for handling the Micro-seconds' tasks.
 *
 *	@details Contains the functionalities for initialization, running and statuses for the
 *	Micro tasks.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *  @deprecated : The micro_task is no longer used and is deprecated.
 *****************************************************************************************
 */
#ifndef MICRO_TASK_H
#define MICRO_TASK_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "uC_Watchdog.h"
#include "OS_Tasker.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is the Micro_Task class contains the implementations for handling
 * the Micro-seconds' tasks.
 * @details TODO
 * @n @b Usage:
 * @n @b 1. Initializes and provides functionalities for Micro Tasks.
 * @n @b 2. It provides public methods to: add the task if the new task, destroy the
 * task, shutdown, to check if the Micro task is running or not,  to get value of the
 * Micro task stack being used.
 ****************************************************************************************
 */
class Micro_Task
{
	public:

		/// The default OS stack size for MICRO task.
		static const uint8_t MICRO_TASK_DEFAULT_OS_STACK_SIZE = 100U;

		/**
		 * Type alias for the type void*, used for the task parameter.
		 */
		typedef void* MICRO_TASK_FUNC_PARAM_TD;

		/**
		 * Type alias for the type void()(void*), takes in the task parameter and does not
		 * return anything.
		 */
		typedef void (MICRO_TASK_FUNC_TD)( MICRO_TASK_FUNC_PARAM_TD param );

		/**
		 * Type alias for the type structure called task, is contains the task function, task
		 * parameter and the pointer to next task.
		 */
		typedef struct task
		{
			MICRO_TASK_FUNC_TD* task_func;
			MICRO_TASK_FUNC_PARAM_TD task_param;
			struct task* next;
		} MICRO_TASK_STRUCT_TD;

		/**
		 *  @brief Constructor
		 *  @details Calls Initialization function which primarily does initialization of the
		 *  next task to be executed, the task function, task parameter,  sets the task
		 *  running as false, initializes the task count to zero. Later calls to add the task
		 *  if the new task is not already in the list. Then the initialization of the OS task
		 *  and the OS fallback task is done which is a fallen into task if the Run Task suspends.
		 *  @param[in] micro_task_os_stack_size: The OS stack size for MICRO task.
		 *  @return None
		 */
		Micro_Task( uint16_t micro_task_os_stack_size =
					MICRO_TASK_DEFAULT_OS_STACK_SIZE );

		/**
		 *  @brief Destructor for the Micro_Task Module.
		 *  @details This will get called when object of Micro_Task goes out of scope.
		 */
		~Micro_Task();

		/**
		 *  @brief
		 *  @details Does initialization of the next task to be executed, the task function, task
		 *  parameter, sets the task running as false, initializes the task count to zero.
		 *  Later calls to add the task.
		 *  @return None
		 */
		static void Init( void );

		/**
		 *  @brief
		 *  @details Calls to add the task if the new task is not already in the list.
		 *  @param[in] task_to_add: The task function.
		 *  @param[in] task_param: The task parameter.
		 *  @return None
		 */
		static void Add_Task( MICRO_TASK_FUNC_TD* task_to_add,
							  MICRO_TASK_FUNC_PARAM_TD task_param = nullptr );

		/**
		 *  @brief
		 *  @details This is basically use to destroy the task.
		 *  @param[in] task_to_destroy: The function of the task to be destroyed.
		 *  @param[in] task_param: The task parameter.
		 *  @return The value of the task count.
		 */
		static uint8_t Destroy_Task( MICRO_TASK_FUNC_TD*
									 task_to_destroy,
									 MICRO_TASK_FUNC_PARAM_TD task_param = nullptr );

		/**
		 *  @brief
		 *  @details Calls the initialization function again to provide basic Micro task
		 *  initialization.
		 *  @return None
		 */
		static void Shutdown( void );

		/**
		 *  @brief
		 *  @details This is basically use to check if the Micro task is running or not.
		 *  @return The status of the Micro task running.
		 *  @retval true: The Micro Task is running.
		 *  @retval false: The Micro Task is not running.
		 */
		static bool Is_uTask_Running( void );

		/**
		 *  @brief
		 *  @details Get the value of the task count, the max task count is updated, then
		 *  depending upon maximum number of Micro tasks, the stack usage value is calculated.
		 *  @return The value of the Micro task stack being used.
		 */
		static uint8_t Get_uTask_Stack_Usage( void );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator
		 * definitions to disallow usage.
		 */
		Micro_Task( const Micro_Task& rhs );
		Micro_Task & operator =( const Micro_Task& object );

		static const uint8_t MICRO_TASK_FALLBACK_STACK_SIZE = 40U;

		static void Run( OS_TASK_PARAM param );

		// This is run if the Run task gets suspended.
		static void Run_Fallback( OS_TASK_PARAM param );

		static void Last_Task( MICRO_TASK_FUNC_PARAM_TD task_param );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Dummy_Task( MICRO_TASK_FUNC_PARAM_TD task_param )
		{}

		static uint8_t Find_Task( MICRO_TASK_FUNC_TD* task_to_find,
								  MICRO_TASK_FUNC_PARAM_TD task_param = nullptr );

		static bool m_tasks_running;
		static MICRO_TASK_STRUCT_TD m_tasks[];
		static uint8_t m_task_count;

		static uC_Watchdog* m_task_watchdog_bone;
		static uint8_t m_task_max;
		static OS_Task* m_os_task;
		// This is fallen into if the Run Task suspends.
		static OS_Task* m_os_fallback_task;
};

}

#endif	/* MICRO_TASK_H */
