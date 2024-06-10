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
#if CONFIG_INCLUDE_TASK_DELETE == 1
#include "CR_Tasker.h"
#endif
/**
 * @brief Task Time structure
 */
typedef struct
{
	/** @brief Current Ticks Snapshot */
	OS_TICK_TD current_ticks_snapshot;
	/** @brief Overflow Ticks Snapshot */
	BF_Lib::MSBASE overflow_ticks_snapshot;
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
 * @brief Critical nesting count.
 */
extern BF_Lib::MBASE OS_INT_critical_nesting;

/**
 * @brief Tick frequency in hertz.
 */
#define OS_TASKER_OS_TICK_FREQUENCY     1000

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
enum
{
	/** @brief Tasker in not started State. Tasker Ready. */
	OS_TASKER_NOT_STARTED,
	/** @brief Tasker in suspended state */
	OS_TASKER_SUSPENDED,
	/** @brief Tasker in running state */
	OS_TASKER_RUNNING
};

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
									 OS_TASK_PARAM init_parameter =
									 reinterpret_cast<OS_TASK_PARAM>( nullptr ) );

		/**
		 * @brief                    Deleting the task from OS scheduling
		 * @param[in] task           Task to be deleted from operating system scheduler.
		 * @return[out] void         None
		 * @note
		 */
		static void Delete_Task( OS_Task* task = OS_TASKER_current_task );

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
		static void Delay_Until( OS_TICK_TD* const pPreviousWakeTime,
								 OS_TICK_TD timeIncrement );

#endif

		/**
		 * @brief                    Get the priority of the task.
		 * @param[in] task           The task whose priority is to be returned.
		 * @param[in] timeIncrement  Time to delay
		 * @return[out] BF_Lib::MBASE The priority of task
		 * @note
		 */
#if CONFIG_INCLUDE_TASK_PRIORITY_GET == 1
		static BF_Lib::MBASE Get_Priority( OS_Task* task = OS_TASKER_current_task );

#endif

		/**
		 * @brief                    Set the priority of the task.
		 * @param[in] new_priority   The new priority of the task.
		 * @param[in] task           The task whose priority is to be changed
		 * @return[out] void         None
		 * @note
		 */
#if CONFIG_INCLUDE_TASK_PRIORITY_SET == 1
		static void Set_Priority( BF_Lib::MBASE new_priority, OS_Task* task );

#endif

		/**
		 * @brief                    Suspend the task
		 * @param[in] task           The task which is to be suspended
		 * @return[out] void         None
		 * @note
		 */
		static void Suspend( OS_Task* task = OS_TASKER_current_task );

		/**
		 * @brief                    Tests if the task is suspended or not
		 * @param[in] task           The task whose status is to be asked.
		 * @return[out] bool_t       Two possible values
		 * @n                        TRUE  - Task in suspended state
		 * @n                        FALSE - Task is not in a suspended state
		 * @note
		 */
		static bool Is_Task_Suspended( OS_Task* task );

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
			m_scheduler_suspend++;
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

		/**
		 * @brief                    Scheduler Tick handler
		 * @param[in] void           None
		 * @return[out] void         None
		 * @remark                   This function kept public for purpose of interrupt conformance.
		 */
		static void Tick_Handler( void );

		/**
		 * @brief                    Enter in the critical section of the code
		 * @param[in] void           None
		 * @return[out] void         None
		 * @remark                   Inline function
		 */
		inline static void Enter_Critical( void )
		{
			OS_Disable_Interrupts();
			// Debug_1_On();
			OS_INT_critical_nesting++;
		}

		/**
		 * @brief                    Exit the critical section of the code
		 * @param[in] void           None
		 * @return[out] void         None
		 * @remark                   Inline function
		 */
		inline static void Exit_Critical( void )
		{
			OS_INT_critical_nesting--;
			if ( OS_INT_critical_nesting == 0 )
			{
				// Debug_1_Off();
				OS_Enable_Interrupts();
			}
		}

		/**
		 * @brief                    Idle task
		 * @param[in] task_param     The task parameter
		 * @return[out] void         None
		 */
		static void Idle_Maintenance_Task( void );

		/**
		 * @brief                    Interrupt Service Entry
		 * @param[in] void           None
		 * @return[out] void         None
		 */
		inline static void Regular_Int_Enter( void )
		{
			OS_INT_critical_nesting = 0x7FFF;
		}

		/**
		 * @brief                    Interrupt Service Exit
		 * @param[in] void           None
		 * @return[out] void         None
		 */
		inline static void Regular_Int_Exit( void )
		{
			OS_INT_critical_nesting = 0;
		}

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
			m_yield_missed = true;
		}

		/**
		 * @brief                    Yield Process
		 * @param[in] void           None
		 * @return[out]void          None
		 * @remark                   For use by friend classes only.
		 */
		inline static void Yield( void )
		{
			// IntPendSet(FAULT_PENDSV);
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
		}

		/**
		 * @brief                    Get current time info.
		 * @param[in] timer_struct   Pointer to structure to receive values
		 * @return[out]void          None
		 * @note
		 */
		static void Get_Current_Time( OS_TASKER_TIME_STRUCT* timer_struct )
		{
			timer_struct->current_ticks_snapshot = m_current_tick;
			timer_struct->overflow_ticks_snapshot = m_tick_overflows;
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
			return ( OS_TASKER_current_task );
		}

		/**
		 * @brief                    Get the tick count
		 * @param[in] void           None
		 * @return[out]OS_TICK_TD    Number of ticks/Tick count
		 * @note
		 */
		static OS_TICK_TD Get_Tick_Count( void )
		{
			OS_TICK_TD return_ticks;

			Enter_Critical();
			return_ticks = m_current_tick;
			Exit_Critical();
			return ( return_ticks );
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
			return ( m_num_tasks );
		}

		/**
		 * @brief                    Inherit priority
		 * @param[in] pMutexHolder   Task which holds mutex.
		 * @return[out]void          None
		 * @note
		 */
#if CONFIG_USE_MUTEXES == 1
		static void PriorityInherit( OS_Task* const pMutexHolder );

#endif	// CONFIG_USE_MUTEXES == 1

		/**
		 * @brief DisInherit priority. return to base priority
		 * @param[in] pMutexHolder Task which holds mutex.
		 * @return[out]void None
		 * @note
		 */
#if CONFIG_USE_MUTEXES == 1
		static void PriorityDisinherit( OS_Task* const pMutexHolder );

#endif	// CONFIG_USE_MUTEXES == 1

	private:
		/**
		 * @brief                    Adds the task to ready list
		 * @param[in] task           The task to be added in reday list
		 * @return[out]void          None
		 * @note
		 */
		inline static void Add_To_Ready_List( OS_Task* task )
		{
			if ( task->m_priority > m_top_ready_priority )
			{
				m_top_ready_priority = task->m_priority;
			}
			m_ready_lists[task->m_priority]->Add_To_End( task->m_general_list_item );
		}

		/**
		 * @brief                    Remove the item from any list containing it
		 * @param[in] list_item      Given item.
		 * @return[out]void          None
		 * @note
		 */
		inline static void Remove_From_Any_List( OS_List_Item* list_item )
		{
			if ( ( OS_List* )list_item->Get_Container() != nullptr )
			{
				( ( OS_List* )list_item->Get_Container() )->Remove( list_item );
			}
		}

#if CONFIG_INCLUDE_TASK_DELETE == 1

		/**
		 * @brief                    This CR task deletes the OS tasks which are in
		 * 							 the delete list
		 * @param[in] cr_task		 CR Task object.
		 * @param[in] handle		 CR Task parameters.
		 * @return[out]void          None
		 * @note
		 */
		static void Delete_Task_Thread( CR_Tasker* cr_task, CR_TASKER_PARAM handle );

#endif

		/**
		 * @brief                    Check Delayed task
		 * @param[in] void           None
		 * @return[out]void          None
		 * @note
		 */
		static void Check_Delayed_Tasks( void );

		/**
		 * @brief                    Check tasks waiting termination
		 * @param[in] void           None
		 * @return[out]void          None
		 * @note
		 */
		static void Check_Tasks_Waiting_Termination( void );

		/**
		 * @brief                    Idle task
		 * @param[in] task_param     Idle task parameter.
		 * @return[out]void          None
		 * @note
		 */
		static void Idle_Task( OS_TASK_PARAM task_param );

		/**
		 * @brief Number of tasks.
		 */
		static volatile BF_Lib::MBASE m_num_tasks;

		/**
		 * @brief Top ready priority.
		 */
		static volatile BF_Lib::MBASE m_top_ready_priority;

		/**
		 * @brief Top used priority.
		 */
		static BF_Lib::MBASE m_top_used_priority;

		/**
		 * @brief Flag indicating whether scheduler is running.
		 */
		static volatile bool m_scheduler_running;

		/**
		 * @brief Scheduler suspend count.
		 */
		static volatile BF_Lib::MBASE m_scheduler_suspend;

		/**
		 * @brief Flag indicating whether yield was missed.
		 */
		static bool m_yield_missed;

		/**
		 * @brief Current tick count.
		 */
		static OS_TICK_TD m_current_tick;

		/**
		 * @brief Tick overflows.
		 */
		static volatile OS_TICK_TD m_tick_overflows;

		/**
		 * @brief Missed ticks.
		 */
		static volatile OS_TICK_TD m_missed_ticks;

		/**
		 * @brief Array of lists of ready tasks (one list per priority level).
		 */
		static OS_List* m_ready_lists[OS_TASK_PRIORITY_MAX];

		/**
		 * @brief List of delayed tasks.
		 */
		static OS_List*	/* volatile */ m_delayed_task_list;

		/**
		 * @brief List of overflowed-delayed tasks.
		 */
		static OS_List*	/* volatile */ m_overflow_delayed_task_list;

		/**
		 * @brief List of pending-ready tasks.
		 */
		static OS_List* m_pending_ready_list;

#if CONFIG_INCLUDE_TASK_SUSPEND == 1
		/**
		 * @brief List of suspended tasks.
		 */
		static OS_List* m_suspended_list;
#endif

		/**
		 * @brief List of delayed tasks.
		 */
		static OS_List* m_delayed_list_0;

		/**
		 * @brief Another list of delayed tasks.
		 */
		static OS_List* m_delayed_list_1;

#if CONFIG_INCLUDE_TASK_DELETE == 1
		/**
		 * @brief List of tasks awaiting termination.
		 */
		static /* volatile */ OS_List* m_waiting_termination_list;

		/**
		 * @brief count of tasks to delete.
		 */
		static volatile BF_Lib::MBASE m_num_tasks_to_delete;
#endif

#if CONFIG_USE_TICK_HOOK == 1
		/**
		 * @brief Tick hook function.
		 */
		static HOOK_FUNCTION m_tick_hook_function;
#endif

#if CONFIG_INCLUDE_TASK_DELETE == 1
		static CR_Tasker* m_delete_task;
#endif
};

#endif
