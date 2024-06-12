/**
 **********************************************************************************************
 *	@file            OS_Tasker.cpp C++ Implementation File for operating system tasking
 *	                 functionality
 *
 *	@brief           The file shall include the definitions of all the functions required for
 *                   Operating system tasking and which are declared in corresponding header file
 *	@details
 *	@copyright       2014 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "Includes.h"
#include "OS_Tasker.h"
#include "uC_OS_Tick.h"
#include "OS_Task_Mon.h"
#include "Exception.h"
#include "uC_OS_Device_Defines.h"

/*
 *****************************************************************************
 *		Variables
 *****************************************************************************
 */
/// Plain old C construct so that assembly can get at it.
OS_Task* OS_TASKER_current_task = nullptr;
extern TaskHandle_t pxCurrentTCB;

// For interrupt Compatibility
/**
 *@remark Coding Standard Deviation:
 *@n MISRA-C++[2008] Required Rule 5-2-6, Suspicious cast
 *@n Justification: Although this cast may raise suspicion, it is required in order for the
 *	code to compile.
 */
/*lint -e{611} # MISRA Deviation */
const void* OS_TASKER_Switch_Context = reinterpret_cast<void*>( &OS_Tasker::Switch_Context );

#if CONFIG_USE_TICK_HOOK == 1U

HOOK_FUNCTION OS_Tasker::m_tick_hook_function = nullptr;

// Disabled vApplicationTickHook function for ESP32
#ifndef ESP32_SETUP

/* Declaration for the FreeRTOS V10.0.1 extern vApplicationTickHook function, implemented
   within this file. */
#ifdef __cplusplus
extern "C" {
#endif
void vApplicationTickHook( void );

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ESP32_SETUP

#endif	// CONFIG_USE_TICK_HOOK == 1U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Tasker::OS_Tasker( void )
{
	OS_Task_Mon::Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t OS_Tasker::Tasker_Running( void )
{
	return ( static_cast<uint8_t>( xTaskGetSchedulerState() ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Task* OS_Tasker::Create_Task( OS_TASK_FUNC* task_func, uint16_t stack_size_in_words,
								 uint8_t priority, uint8_t const* task_name,
								 OS_TASK_PARAM init_parameter )
{
	// This variable is required only if we are using ESP32, hence added a check
#ifdef ESP32_SETUP
	portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#endif

	OS_Task* task;

	task = new OS_Task( task_func, stack_size_in_words, priority, task_name, init_parameter );

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 0-1-9, Operator '!=' always evaluates to True
	 *@n Justification: This if check is needed to check for null pointer.
	 */
	/* lint -e{948} # MISRA Deviation
	   lint -e{774} # MISRA Deviation*/
	if ( task != nullptr )
	{
		/* Commented this as we don't want to use register functionality */
		// OS_Task_Mon::Register_Task( task );

		// Added check as ESP32 notation for this function is different
#ifdef ESP32_SETUP
		Enter_Critical( &mux );
#else
		Enter_Critical();
#endif

		if ( ( Get_Num_Tasks() ) == 1U )
		{
			OS_TASKER_current_task = task;
		}
		else
		{
			if ( ( Tasker_Running() ) == 0U )
			{
				if ( ( OS_TASKER_current_task->Get_Task_Priority() ) <= priority )
				{
					OS_TASKER_current_task = task;
				}
			}
		}

		// Added check as ESP32 notation for this function is different
#ifdef ESP32_SETUP
		Exit_Critical( &mux );
#else
		Exit_Critical();
#endif

	}
	return ( task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_DELETE == 1U
void OS_Tasker::Delete_Task( OS_Task* task )
{
	if ( task != nullptr )
	{
		vTaskDelete( task->m_task );
	}
	else
	{
		vTaskDelete( pxCurrentTCB );
	}
}

#endif	// CONFIG_INCLUDE_TASK_DELETE == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_DELAY == 1U

void OS_Tasker::Delay( OS_TICK_TD ticks_to_delay )
{
	vTaskDelay( ticks_to_delay );
}

#endif	// CONFIG_INCLUDE_TASK_DELAY == 1U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_DELAY_UNTIL == 1U

void OS_Tasker::Delay_Until( OS_TICK_TD* const p_previous_wake_time,
							 OS_TICK_TD time_increment )
{
	vTaskDelayUntil( p_previous_wake_time, time_increment );
}

#endif	// CONFIG_INCLUDE_TASK_DELAY_UNTIL == 1U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Resume_Scheduler( void )
{
	bool return_state;

	if ( xTaskResumeAll() == pdPASS )
	{
		return_state = true;
	}
	else
	{
		return_state = false;
	}
	return ( return_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_PRIORITY_GET == 1U
BF_Lib::MBASE OS_Tasker::Get_Priority( OS_Task* task )
{
	if ( task != nullptr )
	{
		return ( uxTaskPriorityGet( task->m_task ) );
	}
	else
	{
		return ( uxTaskPriorityGet( pxCurrentTCB ) );
	}
}

#endif	// CONFIG_INCLUDE_TASK_PRIORITY_GET == 1
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_PRIORITY_SET == 1U
void OS_Tasker::Set_Priority( BF_Lib::MBASE new_priority, OS_Task* task, bool is_from_isr,
							  BaseType_t* const pxHigherPriorityTaskWoken )
{
	// if ( is_from_isr )
	// {
	// vTaskPrioritySetFromISR( task->m_task, new_priority, pxHigherPriorityTaskWoken );
	// }
	// else
	{
		vTaskPrioritySet( task->m_task, new_priority );
	}
}

#endif	// CONFIG_INCLUDE_TASK_PRIORITY_SET == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_INCLUDE_TASK_SUSPEND == 1U
void OS_Tasker::Suspend( OS_Task* task )
{
	if ( task != nullptr )
	{
		vTaskSuspend( task->m_task );
	}
	else
	{
		vTaskSuspend( pxCurrentTCB );
	}
}

#endif	// CONFIG_INCLUDE_TASK_SUSPEND == 1
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Resume( OS_Task* task )
{
	vTaskResume( task->m_task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Resume_From_ISR( OS_Task* task )
{
	bool return_state;

	if ( xTaskResumeFromISR( task->m_task ) == pdPASS )
	{
		return_state = true;
		OS_Tasker::Yield_From_ISR( ( BaseType_t )return_state );
	}
	else
	{
		return_state = false;
	}
	return ( return_state );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Start_Scheduler( void )
{
	uC_OS_Tick::Start_OS_Tick();
	vTaskStartScheduler();

	return ( false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Switch_Context( void )
{
	vTaskSwitchContext();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker::Add_To_Event_List( OS_List* list, OS_TICK_TD ticks_to_delay )
{
	vTaskPlaceOnEventList( reinterpret_cast<List_t*>( list ), ticks_to_delay );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Remove_From_Event_List( OS_List* list )
{
	bool return_state;

	if ( xTaskRemoveFromEventList( reinterpret_cast<List_t*>( list ) ) == pdPASS )
	{
		return_state = true;
	}
	else
	{
		return_state = false;
	}
	return ( return_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Tasker::Check_For_Timeout( OS_TASKER_TIME_STRUCT* timer_struct,
								   OS_TICK_TD* ticks_to_wait )
{
	bool return_state;

	if ( xTaskCheckForTimeOut( ( reinterpret_cast<TimeOut_t*>( timer_struct ) ), ticks_to_wait ) == pdPASS )
	{
		return_state = true;
	}
	else
	{
		return_state = false;
	}
	return ( return_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_USE_MUTEXES == 1U

void OS_Tasker::PriorityInherit( OS_Task* const p_mutex_holder )
{
	xTaskPriorityInherit( p_mutex_holder );
}

#endif	// CONFIG_USE_MUTEXES == 1
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_USE_MUTEXES == 1U

void OS_Tasker::PriorityDisinherit( OS_Task* const p_mutex_holder )
{
	xTaskPriorityDisinherit( p_mutex_holder );
}

#endif	// CONFIG_USE_MUTEXES == 1

/*
 *****************************************************************************************
 * Defination for the FreeRTOS V10.0.1 extern vApplicationTickHook function.
 *****************************************************************************************
 */
#if CONFIG_USE_TICK_HOOK == 1U

// This is required for ESP32 as if we disable legacy hooks and compile the code we get
// error: "Multiple definations of esp_vApplication as one definition is already
// defined in freertos_hooks.c and in portmacro.h vApplicationTickHook calls esp_vApplication function
#ifndef ESP32_SETUP

void vApplicationTickHook( void )
{
	OS_Tasker::m_tick_hook_function();
}

#endif	// #ifndef ESP32_SETUP

#endif	// CONFIG_USE_TICK_HOOK == 1U
