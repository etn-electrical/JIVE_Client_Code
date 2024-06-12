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
#ifndef OS_TASK_H
#define OS_TASK_H

#include "OS_List_Item.h"
#include "Bit.hpp"
#include "Filter_TPlate.hpp"

#ifdef OS_TASK_MEASURE_RUN_TIME
	#include "Timers_uSec.h"
#endif

// The higher the task priority number the higher the priority.
static const uint8_t OS_TASK_PRIORITY_LOW_IDLE_TASK = 0U;
static const uint8_t OS_TASK_PRIORITY_1 = 1U;
static const uint8_t OS_TASK_PRIORITY_2 = 2U;
static const uint8_t OS_TASK_PRIORITY_3 = 3U;
static const uint8_t OS_TASK_PRIORITY_4 = 4U;
static const uint8_t OS_TASK_PRIORITY_5 = 5U;
static const uint8_t OS_TASK_PRIORITY_6 = 6U;
static const uint8_t OS_TASK_PRIORITY_7 = 7U;
static const uint8_t OS_TASK_PRIORITY_MAX = 8U;
// Not a valid task priority.


/*
 *****************************************************************************
 *		Typedefs
 *****************************************************************************
 */
typedef void* OS_TASK_PARAM;
typedef void OS_TASK_FUNC( OS_TASK_PARAM );

/*
 *****************************************************************************
 *		Constants
 *****************************************************************************
 */

/*
 *****************************************************************************
 *  Class Declaration
   -----------------------------------------------------------------------------
 */
class OS_Task
{
	private:
		// It is imperative that this item show up FIRST in the
		// the class declaration.  The context switch looks for the top of
		// stack to be right where the task pointer is.  Since a class is a
		// structure and the current task points at the task object(structure)
		// this will guarantee that the stack is the first thing.
		volatile OS_STACK_TD* m_top_of_stack;

		OS_STACK_TD* m_start_of_stack;	// high address
		OS_STACK_TD* m_stack_threshold;
		OS_STACK_TD* m_end_of_stack;// low address

		uint16_t m_word_stack_size;
		const uint8_t* m_task_name;

		/**
		 * @brief Task priority.
		 */
		BF_Lib::MBASE m_priority;
#if CONFIG_USE_MUTEXES == 1
		/**
		 * @brief Base priority.
		 */
		BF_Lib::MBASE m_base_priority;
#endif
		/**
		 * @brief General list item.
		 */
		OS_List_Item* m_general_list_item;

		/**
		 * @brief Event list item.
		 */
		OS_List_Item* m_event_list_item;

		/**
		 * The ARM libraries require an 8byte aligned stack.
		 * This mask is applied to the top of the stack and
		 * assures that the stack for a task is 8byte aligned.
		 */
		static const BF_Lib::MBASE CORTEX_M_STACK_ALIGNMENT_MASK =
			static_cast<BF_Lib::MBASE>( ~static_cast<BF_Lib::MBASE>( 0x7 ) );

		// Stack size is in words (processor native variable size).
		void Init_Stack_Space( OS_TASK_FUNC* task_func, uint16_t stack_size_in_words,
							   OS_TASK_PARAM init_task_param );

		/// The task mon has access to some of our privates.
		friend class OS_Tasker;

		/**
		 * @brief The monitor is our friend.
		 */
		friend class OS_Task_Mon;

#ifdef OS_TASK_MEASURE_RUN_TIME
		/**
		 * @brief Process runtime statistics.
		 * @param run_time: Time.
		 */
		void Process_Run_Time( BF_Lib::Timers_uSec::TIME_TD run_time );

		/**
		 * @brief Run time.
		 */
		Filter_TPlate<uint16_t, uint32_t> m_run_time;

		/**
		 * @brief Maximum run time.
		 */
		uint16_t m_max_run_time;

		/**
		 * @brief Minimum runtime.
		 */
		uint16_t m_min_run_time;

		/**
		 * @brief Total runtime.
		 */
		uint32_t m_total_run_time;

		/**
		 * @brief Total runtime.
		 */
		uint32_t m_times_run;

		/**
		 *  Array of 32bit times which are the sum of the tasks in each priority level.
		 *  Measured in microseconds.
		 */
		static uint32_t m_priority_time[OS_TASK_PRIORITY_MAX];

		/**
		 *  Max Stack usage by the task
		 */
		uint8_t m_max_stack_usage_percent;
#endif

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		OS_Task( const OS_Task& object );
		OS_Task & operator =( const OS_Task& object );

	public:
		/**
		 * @brief Constructs an instance of the class.
		 * @param task_func: Task callback function.
		 * @param stack_size_in_words: Stack size (words, not bytes).
		 * @param priority: Priority.
		 * @param task_name: Task name string.
		 * @param init_task_param: Initial task parameter.
		 */
		OS_Task( OS_TASK_FUNC* task_func, uint16_t stack_size_in_words,
				 uint8_t priority, uint8_t const* task_name = reinterpret_cast<uint8_t const*>( "UNDEFINED" ),
				 OS_TASK_PARAM init_task_param = nullptr );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_Task();

		bool Are_We_Suspended( void );

		/**
		 * @brief Gets stack utilization.
		 * @return Stack utilization percentage.
		 */
		uint8_t Stack_Usage_Percent( void );

		/**
		 * @brief Tests whether stack is healthy.
		 * @return True if stack is healthy, false otherwise.
		 */
		bool Stack_Good( void ) const;

		/**
		 * @brief Gets the running task name .
		 * @return task name.
		 */
		inline const uint8_t* Get_Task_Name( void )
		{
			return ( m_task_name );
		}

		/**
		 * @brief Gets priority of the task.
		 * @return priority of the task.
		 */
		inline uint32_t Get_Task_Priority( void )
		{
			return ( m_priority );
		}

		enum context_action_t
		{
			ENTERING,	///< The task is being switched to.  Entering or reentering.
			EXITING		///< The task is being switched out of.  Exiting to another task.
		};

		/**
		 * Callback definitions for what gets called when a context switch happens.
		 * Remember that these should be very very quick.  The callbacks are
		 * associated with each task.  Each task can have its own callback.
		 */
		typedef void* context_action_param_t;
		typedef void context_action_func_t( context_action_param_t context_param,
											context_action_t context_action );

		/**
		 * @brief Provides the task owner the ability to know when the task is
		 * activated or inactivated.  When the tasker enters back into the task
		 * or exits out of the task, the tasker will call this function to indicate
		 * that the task is being activated.
		 * @param context_cback:  The callback to be called on context switch to/from this
		 * task.
		 * @param context_param:  The parameter which can be typically used to store
		 * the this pointer.
		 */
		void Context_Action_Cback( context_action_func_t* context_cback,
								   context_action_param_t context_param = nullptr );

	private:
		/**
		 * Associated with the task context switch pre/post behavior.
		 */
		context_action_param_t m_context_param;
		context_action_func_t* m_context_cback;

};

#endif
