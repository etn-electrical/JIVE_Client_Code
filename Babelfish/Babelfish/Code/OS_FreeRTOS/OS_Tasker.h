/**
 **********************************************************************************************
 *	@file            OS_Tasker.h Header File for low toolkit operating system functionality
 *
 *	@brief           The file shall wrap all the functions which are required for the
 *                   multitasking of small footprint real time operating system(RTOS)
 *	@details
 *	@copyright      2014 Eaton Corporation. All Rights Reserved.
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef OS_TASKER_H
   #define OS_TASKER_H

// configuration is mostly done here:
#include "Bit.hpp"
#include "OS_Config.h"
#include "OS_List.h"
#include "OS_Task.h"

/**
 * @brief Task Time structure
 */
typedef struct
{
	/** @brief Overflow Ticks Snapshot */
	BF_Lib::MSBASE overflow_ticks_snapshot;
	/** @brief Current Ticks Snapshot */
	OS_TICK_TD current_ticks_snapshot;
} OS_TASKER_TIME_STRUCT;

/**
 * @brief Timeout value for waiting indefinitely (forever)
 */
static const OS_TICK_TD OS_TASKER_INDEFINITE_TIMEOUT =
	static_cast<OS_TICK_TD>( ~static_cast<OS_TICK_TD>( 0U ) );

/**
 *@remark Coding Standard Deviation:
 *@n MISRA-C++[2008] Required Rule 5-2-6, Suspicious cast
 *@n Justification: Although this cast may raise suspicion, it is required in order for the
 *	code to compile.
 */
/*lint -e{611} # MISRA Deviation */
extern const void* OS_TASKER_Switch_Context;
/*
 * @brief externed only for the class default section - Don't Touch!
 */
/**
 * @brief The pointer which holds the currently executing task
 */
extern OS_Task* OS_TASKER_current_task;

/**
 * @brief Idle task stack size.
 */
#define OS_TASKER_IDLE_TASK_STACK_SIZE  CONFIG_MINIMAL_STACK_SIZE

/**
 * @brief Hook function pointer type.
 */
typedef void (* HOOK_FUNCTION)( void );

/**
 * @brief Operating System Tasker States
 */

/**
 * @brief                   OS_Tasker Class Declaration. The class will include all the
 *                          features of the real time operating system
 */
class OS_Tasker
{
	public:
		/**
		 * @brief                    Constructor to create instance of OS_Tasker class.
		 * @param[in] void           None
		 * @n
		 * @return[out] None         None
		 */
		OS_Tasker( void );

		/**
		 * @brief                   Destructor to delete the OS_Tasker instance when it goes out of
		 * scope
		 *                          scope.
		 * @param[in] void          none
		 * @return[out] none        none
		 */
		~OS_Tasker();

		/**
		 * @brief                   Returns the state of the tasker
		 * @param[in] void          none
		 * @return[out] uint8_t     Three possible return values
		 * @n                       0 : Tasker is not started yet
		 * @n                       1 : Tasker is in suspended state
		 * @n                       2 : Tasker is running
		 * @note
		 */
		static uint8_t Tasker_Running( void );

		/**
		 * @brief                    Creates the operating system task and add it in OS scheduler.
		 * @param[in] task_func      The function pointer which holds task callback function.
		 * @param[in] stack_size_in_words  Stack size in words.
		 * @param[in] priority       Priority of task
		 * @param[in] task_name      Task name string
		 * @param[in] init_parameter Task initial Parameter
		 * @return[out] OS_Task*     Pointer to created task object. If error occurs in creating
		 * task,
		 * @                         it returns NULL pointer
		 * @note
		 */
		static OS_Task* Create_Task( OS_TASK_FUNC* task_func,
									 uint16_t stack_size_in_words, uint8_t priority,// Where words
																					// is 32bit
																					// bytes
									 uint8_t const* task_name = reinterpret_cast<uint8_t const*>( "UNDEFINED" ),
									 OS_TASK_PARAM init_parameter = nullptr );

		/**
		 * @brief                    Deleting the task from OS scheduling
		 * @param[in] task           Task to be deleted from operating system scheduler.
		 * @return[out] void         None
		 * @note
		 */
		static void Delete_Task( OS_Task* task = nullptr );

		/**
		 * @brief                    Delay the execution of current task by number of ticks.
		 * @param[in] ticks_to_delay The number of ticks by which current task to be delayed.
		 * @return[out] void         None
		 * @note
		 */
#if CONFIG_INCLUDE_TASK_DELAY == 1

		static void Delay( OS_TICK_TD ticks_to_delay );

#endif

		/**
		 * @brief                    Delay the execution of current task.
		 * @details                  This function can be used for more precise timing than provided
		 * by
		 * @                         Delay().
		 * @param[in] pPreviousWakeTime  Pointer to previous wake time stamp.
		 * @param[in] timeIncrement  Time to delay
		 * @return[out] void         None
		 * @note
		 */
#if CONFIG_INCLUDE_TASK_DELAY_UNTIL == 1

		static void Delay_Until( OS_TICK_TD* const p_previous_wake_time,
								 OS_TICK_TD time_increment );

#endif

		/**
		 * @brief                    Get the priority of the task.
		 * @param[in] task           The task whose priority is to be returned.
		 * @param[in] timeIncrement  Time to delay
		 * @return[out] BF_Lib::MBASE The priority of task
		 * @note
		 */
#if CONFIG_INCLUDE_TASK_PRIORITY_GET == 1

		static BF_Lib::MBASE Get_Priority( OS_Task* task = nullptr );

#endif

		/**
		 * @brief                    Set the priority of the task.
		 * @param[in] new_priority   The new priority of the task.
		 * @param[in] task           The task whose priority is to be changed
		 * @param[in] is_from_isr	 Set True if this is called from within an ISR,
		 * 								Else False (default) if regular task code
		 * @return[out] void         None
		 * @note
		 */
#if CONFIG_INCLUDE_TASK_PRIORITY_SET == 1

		static void Set_Priority( BF_Lib::MBASE new_priority, OS_Task* task, bool is_from_isr = false,
								  BaseType_t* const pxHigherPriorityTaskWoken = NULL );

#endif

		/**
		 * @brief                    Suspend the task
		 * @param[in] task           The task which is to be suspended
		 * @return[out] void         None
		 * @note
		 */
		static void Suspend( OS_Task* task = nullptr );


		/**
		 * @brief                    Resumes the suspended task
		 * @param[in] task           The task which is to be resumed
		 * @return[out] void         None
		 * @note
		 */
		static void Resume( OS_Task* task );

		/**
		 * @brief                    Resumes the suspended task from interrupt subroutine
		 * @details                  Resumes a suspended task.  For use inside ISR .
		 * @param[in] task           The task which is to be resumed
		 * @return[out] bool_t       Two possible values
		 * @n                        TRUE  - Task in resumed**
		 * @n                        FALSE - Task is not resumed
		 * @note
		 */
		static bool Resume_From_ISR( OS_Task* task );

		/**
		 * @brief                    Starts the operating system Scheduler
		 * @details
		 * @param[in] void           None
		 * @return[out] bool_t       Two possible values
		 * @n                        TRUE  - Scheduler is started
		 * @n                        FALSE - Error in starting Scheduler
		 * @note
		 */
		static bool Start_Scheduler( void );

		/**
		 * @brief                    Suspend the operating system Scheduler
		 * @details
		 * @param[in] void           None
		 * @return[out] void         None
		 * @note                     Implemented as an inline function
		 */
		inline static void Suspend_Scheduler( void )
		{
			vTaskSuspendAll();
		}

		/**
		 * @brief                    Resume the operating system Scheduler
		 * @param[in] void           None
		 * @return[out] bool_t       Two possible values
		 * @n                        TRUE : Scheduler is resumed
		 * @n                        FALSE : Scheduler is not resumed
		 */
		static bool Resume_Scheduler( void );

		/**
		 * @brief                    Trigger the context switching
		 * @param[in] context_switch_required
		 *                           Enable context switching flag
		 * @n                        TRUE : Enable context switching
		 * @n                        FALSE : Context switching not required
		 * @return[out] void         None
		 */
		static void Context_Switch_Trig( bool context_switch_required )
		{
			if ( context_switch_required )
			{
				Switch_Context();
			}
		}

		/**
		 * @brief                    Perform the context switching
		 * @param[in] void           None
		 * @return[out] void         None
		 * @remark                   This function shall be called from  Context_Switch_Trig().
		 *                           However it is kept public for purpose of interrupt conformance.
		 */
		static void Switch_Context( void );

#ifdef ESP32_SETUP

		/**
		 * @brief                   Exit the critical section of the code.For multiprocessing in EPS32 there are
		 * @n						difference.First of all, disabling the interrupts doesn't stop the tasks or ISRs on
		 * @n						other processors meddling with our CPU. For tasks, this is solved by adding a
		 * @n						spinlock to portENTER_CRITICAL macro.A task running on the other CPU accessing
		 * @n						the same data will spinlock in the portENTER_CRITICAL code until the first CPU is
		 * @n						done.For more reference see 'portmacro.h' in esp32 freertos folder.
		 * @param[in] void          Port spinlock
		 * @return[out] void        None
		 * @remark                  Inline function
		 */
		inline static void Enter_Critical( portMUX_TYPE* mux )
		{
			portENTER_CRITICAL( mux );
		}

#else
		/**
		 * @brief                    Enter in the critical section of the code
		 * @param[in] void           None
		 * @return[out] void         None
		 * @remark                   Inline function
		 */
		inline static void Enter_Critical( void )
		{
			portENTER_CRITICAL();
		}

#endif	// #ifdef ESP32_SETUP


#ifdef ESP32_SETUP

		/**
		 * @brief                   Exit the critical section of the code.For multiprocessing in EPS32 there are
		 * @n						difference.First of all, disabling the interrupts doesn't stop the tasks or ISRs on
		 * @n						other processors meddling with our CPU. For tasks, this is solved by adding a
		 * @n						spinlock to portENTER_CRITICAL macro.A task running on the other CPU accessing
		 * @n						the same data will spinlock in the portENTER_CRITICAL code until the first CPU is
		 * @n						done.For more reference see 'portmacro.h' in esp32 freertos folder.
		 * @param[in] void          Port spinlock
		 * @return[out] void        None
		 * @remark                  Inline function
		 */
		inline static void Exit_Critical( portMUX_TYPE* mux )
		{
			portEXIT_CRITICAL( mux );
		}

#else
		/**
		 * @brief                    Exit the critical section of the code
		 * @param[in] void           None
		 * @return[out] void         None
		 * @remark                   Inline function
		 */
		inline static void Exit_Critical( void )
		{
			portEXIT_CRITICAL();
		}

#endif
		/**
		 * @brief                    Idle task
		 * @param[in] task_param     The task parameter
		 * @return[out] void         None
		 */
		static void Idle_Maintenance_Task( void );

		/**
		 * @brief                    Add to event list.
		 * @param[in] list           Given list
		 * @param[in] ticks_to_delay Delay time
		 * @return[out]void          None
		 * @remark                   For use by friend classes only.
		 */
		static void Add_To_Event_List( OS_List* list,
									   OS_TICK_TD ticks_to_delay );

		/**
		 * @brief                    Remove from event list.
		 * @param[in] list           Given list
		 * @return[out]bool_t        Two possible values
		 * @n                        TRUE : Successful
		 * @n                        FALSE : Otherwise
		 * @remark                   For use by friend classes only.
		 */
		static bool Remove_From_Event_List( OS_List* list );

		/**
		 * @brief                    Sets yield missed flag
		 * @param[in] void           None
		 * @return[out]void          None
		 * @remark                   For use by friend classes only.
		 */
		inline static void Yield_Missed( void )
		{
			vTaskMissedYield();
		}

		/**
		 * @brief                    Yield Process
		 * @param[in] void           None
		 * @return[out]void          None
		 * @remark                   For use by friend classes only.
		 */
		inline static void Yield( void )
		{
			taskYIELD();
		}

		/**
		 * @brief                    Get current time info.
		 * @param[in] timer_struct   Pointer to structure to receive values
		 * @return[out]void          None
		 * @note
		 */
		static void Get_Current_Time( OS_TASKER_TIME_STRUCT* timer_struct )
		{

#ifdef ESP32_SETUP
			vTaskSetTimeOutState( reinterpret_cast<TimeOut_t*>( timer_struct ) );
#else
			vTaskInternalSetTimeOutState( reinterpret_cast<TimeOut_t*>( timer_struct ) );
#endif	// #ifdef ESP32_SETUP
		}

		/**
		 * @brief                    Check for timeout
		 * @param[in] timer_struct   Pointer to timer structure
		 * @param[in] ticks_to_wait  Number of ticks to wait
		 * @return[out]bool_t        True if timed out, false otherwise.
		 * @note
		 */
		static bool Check_For_Timeout( OS_TASKER_TIME_STRUCT* timer_struct,
									   OS_TICK_TD* ticks_to_wait );

		/**
		 * @brief                    Get Current Task. Returns the pointer to current task
		 * @param[in] void           None
		 * @return[out]OS_Task       pointer to current task.
		 * @note
		 */
		inline static OS_Task* Get_Current_Task( void )
		{
			return ( reinterpret_cast<OS_Task*>( xTaskGetCurrentTaskHandle() ) );
		}

		/**
		 * @brief                    Get the tick count
		 * @param[in] void           None
		 * @return[out]OS_TICK_TD    Number of ticks/Tick count
		 * @note
		 */
		static OS_TICK_TD Get_Tick_Count( void )
		{
			return ( static_cast<OS_TICK_TD>( xTaskGetTickCount() ) );
		}

		/**
		 * @brief                    Set Tick hook function
		 * @param[in] HOOK_FUNCTION  The function which is to be added in HOOK function list
		 * @return[out]void          None
		 * @note
		 */
#if CONFIG_USE_TICK_HOOK == 1

		inline static void Set_Tick_Hook( HOOK_FUNCTION hook_function )
		{
			m_tick_hook_function = hook_function;
		}

#endif

		/**
		 * @brief                    Get the count of tasks.
		 * @param[in] void           None
		 * @return[out]uint32_t      Number of tasks.
		 * @note
		 */
		inline static uint32_t Get_Num_Tasks( void )
		{
			return   ( uxTaskGetNumberOfTasks() );
		}

		/**
		 * @brief                    Inherit priority
		 * @param[in] p_mutexholder   Task which holds mutex.
		 * @return[out]void          None
		 * @note
		 */
#if CONFIG_USE_MUTEXES == 1
		static void PriorityInherit( OS_Task* const p_mutex_holder );

#endif	// CONFIG_USE_MUTEXES == 1

		/**
		 * @brief DisInherit priority. return to base priority
		 * @param[in] p_mutex_holder Task which holds mutex.
		 * @return[out]void None
		 * @note
		 */
#if CONFIG_USE_MUTEXES == 1
		static void PriorityDisinherit( OS_Task* const p_mutex_holder );

#endif	// CONFIG_USE_MUTEXES == 1

#if CONFIG_USE_TICK_HOOK == 1
		/**
		 * @brief Tick hook function.
		 */
		static HOOK_FUNCTION m_tick_hook_function;
#endif

		/**
		 * @brief                    Function to request a context switch from an ISR
		 * @param[in] higher_priority_task_woken        Parameter to indicate if the higher priority task needs to be
		 * woken up
		 * @return[out] void         None
		 * @remark                   Inline function
		 */
		inline static void Yield_From_ISR( BaseType_t higher_priority_task_woken )
		{
#ifdef ESP32_SETUP
			if ( higher_priority_task_woken == TRUE )
			{
				portYIELD_FROM_ISR();
			}
#else
			portYIELD_FROM_ISR( higher_priority_task_woken );
#endif
		}

};

#endif
