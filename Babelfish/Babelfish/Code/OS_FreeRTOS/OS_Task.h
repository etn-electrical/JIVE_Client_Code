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
#include "uC_OS_Device_Defines.h"
#include "FreeRTOS.h"
#include "task.h"
#include "OS_Task_Priority.h"

#ifdef OS_TASK_MEASURE_RUN_TIME
	#include "Timers_uSec.h"
#endif



/*
 *****************************************************************************
 *		Typedefs
 *****************************************************************************
 */
typedef void* OS_TASK_PARAM;
typedef void OS_TASK_FUNC ( OS_TASK_PARAM );

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

		uint16_t m_word_stack_size;

		/// The task mon has access to some of our privates.
		friend class OS_Tasker;

		/**
		 * @brief The monitor is our friend.
		 */
		friend class OS_Task_Mon;

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		OS_Task( const OS_Task& object );
		OS_Task & operator =( const OS_Task& object );

	public:
		TaskHandle_t m_task;

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

		/**
		 * @brief Gets stack utilization.
		 * @return Stack utilization percentage.
		 */
		uint8_t Stack_Usage_Percent( void );

		/**
		 * @brief Gets the running task name .
		 * @return task name.
		 */
		inline const uint8_t* Get_Task_Name( void )
		{

#ifdef ESP32_SETUP
			return ( reinterpret_cast<uint8_t*>( pcTaskGetTaskName( m_task ) ) );

#else
			return ( reinterpret_cast<uint8_t*>( pcTaskGetName( m_task ) ) );

#endif


		}

		/**
		 * @brief Gets priority of the task.
		 * @param is_from_isr Set True if this is being called by an ISR,
		 * 						else (default) False for normal task code.
		 * @return priority of the task.
		 */
		inline uint32_t Get_Task_Priority( bool is_from_isr = false )
		{
			if ( is_from_isr )
			{
				return ( static_cast<uint32_t>( uxTaskPriorityGetFromISR( m_task ) ) );
			}
			else
			{
				return ( static_cast<uint32_t>( uxTaskPriorityGet( m_task ) ) );
			}
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
		typedef void context_action_func_t ( context_action_param_t context_param,
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
