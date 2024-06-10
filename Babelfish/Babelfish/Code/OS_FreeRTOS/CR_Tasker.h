/**
 **********************************************************************************************
 *  @file            CR_Tasker.h Header File for low toolkit coroutine tasking functionality
 *
 *  @brief           The file shall wrap all the functions which are required for the
 *                   multitasking of coroutines.
 *  @details
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef CR_TASKER_H
	#define CR_TASKER_H

#include "Includes.h"
#include "OS_List.h"
#include "uC_Watchdog.h"

// LTK-4767: TODO: This check should be removed once Timers_uSec.h file is available for ESP32 platform
#ifndef ESP32_SETUP
#include "Timers_uSec.h"
#endif

#include "Bit.hpp"
#include "Filter_TPlate.hpp"

/**
 * @brief List of coroutine priority levels.
 */
typedef uint8_t CR_TASKER_PRIOR_TD;
/** @brief lowest priority - Idle */
static const CR_TASKER_PRIOR_TD CR_TASKER_PRIORITY_0 = 0U;
/** @brief Priority - 1 */
static const CR_TASKER_PRIOR_TD CR_TASKER_PRIORITY_1 = 1U;
/** @brief Priority - 2 */
static const CR_TASKER_PRIOR_TD CR_TASKER_PRIORITY_2 = 2U;
/** @brief Priority - 3 */
static const CR_TASKER_PRIOR_TD CR_TASKER_PRIORITY_3 = 3U;
/** @brief Priority - 4 */
static const CR_TASKER_PRIOR_TD CR_TASKER_PRIORITY_4 = 4U;
/** @brief Priority - 5. Highest Task Priority.*/
static const CR_TASKER_PRIOR_TD CR_TASKER_TMR_PRIORITY = 5U;
/** @brief Priority - 6 Maximum Priorities.*/
static const CR_TASKER_PRIOR_TD CR_TASKER_MAX_PRIORITIES = 6U;

/**
 * @brief Alias for lowest priority.
 */
static const CR_TASKER_PRIOR_TD CR_TASKER_IDLE_PRIORITY = CR_TASKER_PRIORITY_0;

/**
 * @brief Type of coroutine task parameter.
 */
typedef void* CR_TASKER_PARAM;

/**
 * @brief Initial value for coroutine state variable.
 */
static const uint_fast16_t CR_TASKER_START_POINT = 0U;

/*
 *****************************************************************************
 *		PRIVATE Macros
 *		The following macros are private and should not be used.
 *		They are only for macros elsewhere.
 *****************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 6-6-5, 6-4-4, 15-0-3
 * @n PCLint: Note 904: Violates MISRA C++ 2008 Required Rule 6-6-5, A function shall have a single
 * point of exit at the end of the function
 * @n PCLint: Note 1960: Violates MISRA C++ 2008 Required Rule 6-4-4, A switch-label shall only be
 * used when the most closely-enclosing compound
 * @n PCLint: statement is the body of a switch statement
 * @n PCLint: Note 1960: Violates MISRA C++ 2008 Required Rule 15-0-3, Control shall not be
 * transfered into a try or catch block using a goto or a switch statement
 * @n Justification: Implementation of co-routines require this behavior to be exempted
 */
#define CR_Tasker_Set_State0( cr_task )     cr_task->m_state = \
	( static_cast<uint32_t>( __LINE__ ) * 2U );	/*lint -e(904) */ return;															/*lint
																																	   -e(1960,646)
																																	 */  \
	case ( static_cast<uint32_t>( __LINE__ ) * 2U ):
#define CR_Tasker_Set_State1( cr_task )     cr_task->m_state = \
	( ( static_cast<uint32_t>( __LINE__ ) * 2U ) + 1U ); /*lint -e(904) */ return;														/*lint
																																		   -e(1960,646)
																																		 */    \
	case ( ( static_cast<uint32_t>( __LINE__ ) * 2U ) + 1U ):

/*
 *****************************************************************************
 *		PUBLIC Macros
 *		These are the public functions that you should use for coroutines.
 *****************************************************************************
 */

/// Put this at the beginning of the function you wish to have the coroutine preemption on.
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 6-2-3 null statement not in line by itself.
 * @n Justification: Implementation of co-routines require this behavior to be exempted.  It is
 * possible that someone will put a semicolon after the CR_Tasker_Begin() macro.  This is not
 * necessary but makes the appearance better.  We therefore cover for it.
 */
#define CR_Tasker_Begin( cr_task )  switch ( cr_task->m_state ){/*lint -esym(1960,6-2-3) */ \
		case CR_TASKER_START_POINT:

/// @brief This must be put at the end of the task.
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 6-4-5
 * @n PCLint: Info 825: Violates MISRA C++ 2008 Required Rule 6-4-5, An uncoditional throw or break
 * statement shall terminate every non-empty switch-clause
 * @n Justification: Implementation of co-routines require this behavior to be exempted
 */
#define CR_Tasker_End()		/*lint -e(1960) */  /*lint -e(825) */ default: \
		break;}

/** @brief This function macro allows a task to yield and come back at the next roll through.
 *          Remember that this should only be used at the idle task level since this will cause all
 * other
 *          lower priority tasks to be held off.Not tested yet.
 */
#define CR_Tasker_Yield( cr_task )          CR_Tasker_Set_State0( cr_task )

#define CR_Tasker_Suspend_Reset_Yield( cr_task )    cr_task->Suspend_And_Reset();return;

/** @brief This function macro allows a task to yield and come back to check for a true condition.
 *          Remember that this should only be used at the idle task level since this will cause all
 * other
 *          lower priority tasks to be held off.
 *          Not tested yet.
 */
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 6-6-5
 * @n PCLint: Note 904: Violates MISRA C++ 2008 Required Rule 6-6-5, A function shall have a single
 * point of exit at the end of the function
 * @n Justification: Implementation of co-routines require this behavior to be exempted
 */
#define CR_Tasker_Yield_Until( cr_task, \
							   cond )  cr_task->m_state = \
	( static_cast<uint32_t>( __LINE__ ) * 2U ); \
	case ( static_cast<uint32_t>( __LINE__ ) * 2U ): \
		if ( !( cond ) ){ /*lint -e(904) */ return; }

// Provides a delay.  It actually will pend the task and wake up after the timeout.
/**
 * @remark Coding Standard Deviation:
 * @n PCLint: Note 904: Expected positive indentation from Location.
 * @n Justification: This is a note only and cannot be helped in the context of the
 * place where used.
 */
#define CR_Tasker_Delay( cr_task, ticks_to_delay )	/*lint -e(725) */                            \
	CR_Tasker::Add_To_Delayed_List( ticks_to_delay, nullptr );    \
	CR_Tasker_Set_State0( cr_task );

// This will restart the coroutine state machine.
/**
 * @remark Coding Standard Deviation:
 * @n PCLint: Note 904: Expected positive indentation from Location.
 * @n Justification: This is a note only and cannot be helped in the context of the
 * place where used.
 */
#define CR_Tasker_Restart( cr_task )    \
	/*lint -e(725) */ cr_task->m_state = CR_TASKER_START_POINT

/*
 *****************************************************************************
 *  Class Declaration
 *****************************************************************************
 */
namespace BF_Lib
{
class Timers;
}
/**
 * @brief                   CR_Tasker Class Declaration. The class will include all the
 *                          features of the coroutine tasker
 */
class CR_Tasker
{
	public:

		/**
		 * @brief                    Type of function pointer to coroutine call back function.
		 * @param[in] tasker         Pointer to coroutine task
		 * @param[in] param			 Coroutine task parameter
		 * @note                     The following callback types is for applications where you are
		 *                           planning on using the coroutine functionality
		 */
		typedef void (* CR_TASKER_CBACK)( CR_Tasker* tasker, CR_TASKER_PARAM param );
		/**
		 * @brief                    Type of function pointer to lightweight coroutine callback
		 * function
		 * @param[in] param			 Coroutine task parameter
		 * @note                     The following callback types is for applications where you are
		 *                           NOT planning on using the coroutine functionality and want to
		 *                           simply called back
		 */
		typedef void (* CR_TASKER_LITE_CBACK)( CR_TASKER_PARAM param );
		/**
		 * @brief                    Constructor to create an instance of CR_Tasker class.
		 * @param[in] cr_func_callback Coroutine callback function.
		 * @param[in] param          Coroutine parameter
		 * @param[in] priority       Coroutine task priority. Use enum CR_TASKER_PRIOR_TD.
		 * @param[in] task_name      Name of coroutine task (for debugging)
		 * @return
		 * @note                     Typically we should call the constructor and assign the lowest
		 *                           priority to SysTick, and then call Start_OS_Tick when the OS is
		 *                           set up properly and ready to go.  This constructor provides
		 * full
		 *                           coroutine functionality
		 */
		CR_Tasker( CR_TASKER_CBACK cr_func_callback, CR_TASKER_PARAM param = NULL,
				   uint8_t priority = static_cast<uint8_t>( CR_TASKER_IDLE_PRIORITY ),
				   char_t const* task_name = NULL );

		// This will attach a callback to the coroutine system.  This constructor is
		// used for the more traditional task which has no intention of using the
		// coroutine capabilities (pend, delay, etc).  This is for only idle task functionality.
		/**
		 * @brief Constructs an instance of the class.
		 * @param cr_func_callback: Lightweight coroutine callback function.
		 * @param param: Coroutine parameter.
		 * @param task_name: Name of coroutine task (for debugging).
		 */
		CR_Tasker( CR_TASKER_LITE_CBACK cr_func_callback, CR_TASKER_PARAM param = NULL,
				   char_t const* task_name = NULL );

		/**
		 * @brief                   Destructor to delete the CR_Tasker instance when it goes out of
		 * scope
		 * @param[in] void          none
		 * @return[out] none        none
		 */
		~CR_Tasker( void );
		/**
		 * @brief                    Will put a task to sleep in that it will no longer execute.
		 *                           It will also reset the state machine back to the beginning if
		 * it
		 *                           has a BEGIN/END and yields.
		 * @param[in] void           None
		 * @return
		 * @note                     This function can be used in a task which has no BEGIN/END
		 * construct.
		 */
		void Suspend_And_Reset( void )
		{
			Push_TGINT();
			if ( !BF_Lib::Bit::Test( m_priority, CR_TASKER_SUSPENDED_PRI_BIT ) )
			{
				BF_Lib::Bit::Set( m_priority, CR_TASKER_SUSPEND_BIT );
				m_state = CR_TASKER_START_POINT;
			}
			Pop_TGINT();
		}

		/**
		 * @brief                    Will put a task to sleep in that it will no longer execute.
		 * @param[in] void           None
		 * @return
		 * @note                     This function can be used in a task which has no BEGIN/END
		 * construct.
		 */
		void Suspend( void )
		{
			Push_TGINT();
			if ( !BF_Lib::Bit::Test( m_priority, CR_TASKER_SUSPENDED_PRI_BIT ) )
			{
				BF_Lib::Bit::Set( m_priority, CR_TASKER_SUSPEND_BIT );
			}
			Pop_TGINT();
		}

		/**
		 * @brief                    Resume the coroutine.
		 *                           Will put wake a task that had previously been suspended.
		 * @param[in] void           None
		 * @return
		 * @note                     This function can be used in a task which has no BEGIN/END
		 * construct.
		 */
		void Resume( void )
		{
			Push_TGINT();
			if ( BF_Lib::Bit::Test( m_priority, CR_TASKER_SUSPENDED_PRI_BIT ) )
			{
				Remove_From_Any_List( m_generic_list_item );
				Remove_From_Any_List( m_event_list_item );
				m_pending_cr_ready_list->Insert( m_event_list_item );
				BF_Lib::Bit::Clr( m_priority, CR_TASKER_SUSPENDED_PRI_BIT );
			}
			else
			{
				BF_Lib::Bit::Clr( m_priority, CR_TASKER_SUSPEND_BIT );
			}

			Pop_TGINT();
		}

		/**
		 * @brief                    This is the main task handler. It does not exit. It schedules
		 * the
		 *                           tasks and dispatches them for running purpose.
		 *                           Designed to either run as it is or be sent to the Operating
		 * system
		 *                           task manager.
		 *                           Will put wake a task that had previously been suspended.
		 * @param[in] param          The coroutine task scheduler
		 * @return
		 * @note
		 */
		static void Scheduler( CR_TASKER_PARAM param );

		/**
		 * @brief                    This is the CR dispatcher called by the Scheduler operation.
		 *                           It dispatches any CR that is ready to run and then returns
		 * @param[in] void           None
		 * @return
		 * @note
		 */
		static void RunReadyCR( void );

		/**
		 * @brief                    Handles a tick event. This is optimally called externally from
		 *                           some tick generator. This should be put in a place where the
		 * tick
		 *                           is generated ideally every 1ms.
		 * @param[in] void           None
		 * @return
		 * @note
		 */
		static void Tick_Event( void );

		/** @brief Coroutine state */
		uint_fast16_t m_state;
		/**
		 * @brief                    Add given list to the delayed list for given time.
		 *                           It dispatches any CR that is ready to run and then returns
		 * @param[in] time_to_delay  The time in ticks to delay.
		 * @param[in] event_list     The given list
		 * @return
		 * @note
		 */
		static void Add_To_Delayed_List( OS_TICK_TD time_to_delay, OS_List* event_list );

		/**
		 * @brief                    This function will return the CR_Tasker base ticks.  This tick count
		 * is utilized by the delay code.  This tick can change by large steps if it falls behind the
		 * m_cr_interrupt_tick_counter.  This tick counter will have to catch up with the interrupt tick if
		 * a task takes too much processing time and the delay evaluation task fails to run in the base tick time.
		 * @param[in] void           None
		 * @return OS_TICK_TD        Number of ticks.
		 * @note
		 */
		static OS_TICK_TD Get_CR_Tick_Count( void )
		{
			volatile OS_TICK_TD return_ticks;
                        //Ahmed come back to this one 
			Push_TGINT();
			return_ticks = m_cr_tick_count;
			Pop_TGINT();
			return ( return_ticks );
		}

		/**
		 * @brief                    This returns the actual interrupt based tick count.  Since
		 * the CR Tick can lag if a task hogs the processing this tick will provide a good time but is not
		 * the tick used to trigger timed events in the CR_Tasker.  If you want the CR_Tasker time tick
		 * utilize the Get_CR_Tick_Count function instead.
		 * @param[in] void           None
		 * @return OS_TICK_TD        Number of ticks.
		 * @note
		 */
		static OS_TICK_TD Get_Interrupt_Tick_Count( void )
		{
			volatile OS_TICK_TD return_ticks;
                        //Ahmed come back to this one 
			Push_TGINT();
			return_ticks = m_cr_interrupt_tick_count;
			Pop_TGINT();
			return ( return_ticks );
		}

		// TODO clean up crufty comment
		// Used to hide the implementation of the co-routine control block.  The
		// control block structure however has to be included in the header due to
		// the macro implementation of the co-routine functionality.
		// Defines the prototype to which co-routine functions must conform.
		/**
		 * @brief Coroutine queues are friends.
		 */
		friend class CR_Queue;
		/**
		 * @brief Timers are our friend
		 */
		friend class ::BF_Lib::Timers;
		/**
		 * @brief                    Gets the currently executing coroutine task.
		 * @param[in] void           None
		 * @return[out] CR_Tasker*   Pointer to Current coroutine task.
		 * @note
		 */
		static CR_Tasker* Current_CRTask( void );

		/**
		 * @brief Returns the function pointer to the task which is currently being run.
		 * This is used for diagnostic purposes only.
		 * @return a UINT32 pointer for the function which is the current CRTask.
		 */
		static CR_TASKER_CBACK Current_CRTask_Callback( void );

		/**
		 * @brief Returns the function pointer to the task parameter (typically the pointer to the
		 * object).
		 * This is used for diagnostic purposes only.
		 * @return a UINT32 pointer for the function parameter.
		 */
		static CR_TASKER_PARAM Current_CRTask_Param( void );

		/**
		 * The following structure is used by the Task_Stat function.
		 * It provides various diagnostic information for each task.
		 */
		struct task_report_t
		{
			CR_TASKER_CBACK callback;	///< The function used in the task.
			CR_TASKER_PARAM param;		///< Typically the this pointer.
			uint32_t times_run;			///< Number of times the task has run since last ask.
			uint16_t filtered_run_time;	///< Rough average of run time over the period.
			uint16_t max_run_time;		///< Maximum time for the task to run.
			uint16_t min_run_time;		///< Minimum time the task has run.
			uint32_t total_run_time;	///< Total accumulated time the task has run.
			char_t const* task_name;	///< The name of the task.
			uint8_t priority;			///< The task priority.
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
		 * function callback is nullptr then it is an indication of running to the end
		 * of the linked list and you have no more tasks to get.
		 */
		static void Task_Stats( uint8_t task_num, task_report_t& task_report );

		/**
		 * @brief This is to be called when the CR_Tasker task is currently suspended.
		 * Events which would cause this are things like interrupts or OS Tasks which
		 * have preempted the operation of this task.
		 * This is primarily to benefit the timing analysis of the tasks.
		 */
		static void Tasker_Paused( void )
		{
#ifdef CR_TASKER_DEBUG
			m_current_coroutine->Process_Run_Time(
				BF_Lib::Timers_uSec::Time_Passed( m_run_sample_time ) );
#endif
		}

		/**
		 * @brief This is to be called when the CR_Tasker task is resumed.
		 * Events which would cause this are things like interrupts or OS Tasks which
		 * have preempted the operation of this task and have now returned control
		 * back to the CR_Tasker.  This is primarily to benefit the timing analysis
		 * of the tasks.
		 */
		static void Tasker_Resumed( void )
		{
#ifdef CR_TASKER_DEBUG
			m_run_sample_time = BF_Lib::Timers_uSec::Get_Time();
#endif
		}

		/**
		 * @brief Gets the running task name .
		 * @return task name.
		 */
		inline char_t const* Get_Task_Name( void )
		{
			return ( m_task_name );
		}

	private:
		/*
		 * @brief Bit number used to build mask to indicate task to remove from ready list
		 */
		static const uint8_t CR_TASKER_SUSPEND_BIT = 6U;
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		CR_Tasker( const CR_Tasker& rhs );
		CR_Tasker & operator =( const CR_Tasker& object );

		/**
		 * @brief Bit number used to build mask to indicate task suspended state.
		 */
		static const uint8_t CR_TASKER_SUSPENDED_PRI_BIT = 7U;

		/*
		 * Co-routines that are readied by an interrupt cannot be placed directly into
		 * the ready lists (there is no mutual exclusion).  Instead they are placed in
		 * in the pending ready list in order that they can later be moved to the ready
		 * list by the co-routine scheduler.
		 */
		/**
		 * @brief                    Checks the Pending Ready List
		 * @param[in] void           None
		 * @return
		 * @note                     Co-routines that are readied by an interrupt cannot be placed
		 *                           directly into the ready lists (there is no mutual exclusion).
		 *                           Instead they are placed in the pending ready list in order that
		 *                           they can later be moved to the ready list by the co-routine
		 *                           scheduler.
		 */
		static void Check_Pending_Ready_List( void );

		/**
		 * @brief                    Checks the Delayed List
		 * @return					 None
		 * @note                     Function that looks at the list of co-routines that are
		 * currently
		 *                           delayed to see if any require waking.
		 *                           Co-routines are stored in the queue in the order of their wake
		 *                           time meaning once one co-routine has been found whose timer
		 *                           has not expired. We need not look any further down the list.
		 */
		static void Check_Delayed_List( void );

		/**
		 * @brief                    Basic idle task behavior
		 * @param[in] cr_task        Coroutine task
		 * @param[in] param          Coroutine task parameter
		 * @return
		 * @note                     Function that handles some very basic idle task behavior.This
		 * task
		 *                           also allows us to make some efficiency changes to the task
		 * switching.
		 */
		static void Last_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		/**
		 * @brief                    Adds coroutine to ready list
		 * @param[in] cr_task        Coroutine task
		 * @return
		 * @note                     Place the co-routine represented by pxCRCB into the appropriate
		 *                           ready queue for the priority. It is inserted at the end of the
		 * list.
		 *                           This macro accesses the co-routine ready lists and therefore
		 * must
		 *                           not be used from within an ISR.
		 */
		inline static void Add_To_Ready_Queue( CR_Tasker* cr_task )
		{
			uint8_t priority = 0U;

			priority = cr_task->m_priority;
			BF_Lib::Bit::Clr( priority, CR_TASKER_SUSPEND_BIT );
			BF_Lib::Bit::Clr( priority, CR_TASKER_SUSPENDED_PRI_BIT );
			if ( priority > m_cr_top_ready_priority )
			{
				m_cr_top_ready_priority = priority;
			}
			m_ready_cr_lists[priority]->Insert( cr_task->m_generic_list_item );
		}

		/**
		 * @brief                    Removes item from any list which contains it
		 * @param[in] list_item      The given item
		 * @return
		 * @note
		 */
		inline static void Remove_From_Any_List( OS_List_Item* list_item )
		{
			if ( ( OS_List* )list_item->Get_Container() != NULL )
			{
				( ( OS_List* )list_item->Get_Container() )->Remove( list_item );
			}
		}

		/**
		 * @brief                    Remove ourselves from given event list.
		 * @param[in] event_list     The given event list
		 * @return bool              Two Possible values
		 * @n                        TRUE - if success
		 * @n                        TRUE - if failed
		 * @note
		 */
		static bool Remove_From_Event_List( OS_List* event_list );

		/**
		 * @brief                    Initializes the CoRoutine tasker.
		 * @param[in] cr_func_callback  Coroutine callback function.
		 * @param[in]param           Coroutine parameter
		 * @param[in]priority        Priority
		 * @param[in]init_state      Initial State
		 * @param[in]task_name       Task Name
		 * @return
		 * @note
		 */
		void Init_CRT( CR_TASKER_CBACK cr_func_callback, CR_TASKER_PARAM param,
					   uint8_t priority,
					   uint_fast16_t init_state, char_t const* task_name );

		/**
		 * @brief Our callback function.
		 */
		CR_TASKER_CBACK m_crt_callback;
		/** @brief Our callback function parameter.*/
		CR_TASKER_PARAM m_param;
		/** @brief Generic list item. */
		OS_List_Item* m_generic_list_item;

		/** @brief Event list item. */
		OS_List_Item* m_event_list_item;

		/** @brief Our priority. */
		uint8_t m_priority;

		char_t const* m_task_name;

#ifdef CR_TASKER_DEBUG
		/**
		 * @brief Process runtime statistics.  Primarily the time.
		 * Also accumulates the total time for a priority level.
		 * @param[in] run_time Run time in microseconds.
		 */
		void Process_Run_Time( BF_Lib::Timers_uSec::TIME_TD run_time );


		/** @brief Run time. */
		Filter_TPlate<uint16_t, uint32_t> m_run_time;
		/** @brief Maximum run time. */
		uint16_t m_max_run_time;
		/** @brief Minimum run time. */
		uint16_t m_min_run_time;
		/** @brief Total run time since the last print output.*/
		uint32_t m_total_run_time;
		/** @brief Number of times task has run*/
		uint32_t m_times_run;
		/** @brief Index into debug task list */
		CR_Tasker* m_debug_task_next;

		/** @brief Head of debug task list.*/
		static CR_Tasker* m_debug_task_head;

		/**
		 * Time sample for measuring a task timing.  Sampled at the beginning
		 * of every task run and then measured at the end of a task.
		 */
		static BF_Lib::Timers_uSec::TIME_TD m_run_sample_time;

		/**
		 *  Array of 32bit timers which are the sum of the tasks in each priority level.
		 *  Measured in microseconds.
		 */
		static uint32_t m_priority_time[CR_TASKER_MAX_PRIORITIES];
#endif
		/** @brief Name of the null task. */
		static const char NULL_TASK_NAME[];
		/** @brief Array of ready lists, by priority.*/
		static OS_List* m_ready_cr_lists[CR_TASKER_MAX_PRIORITIES];
		/** @brief Pending-ready list.*/
		static OS_List* m_pending_cr_ready_list;
		/** @brief Delayed list 1. */
		static OS_List* m_delayed_cr_list1;
		/** @brief Delayed list 2. */
		static OS_List* m_delayed_cr_list2;
		/** @brief Delayed list*/
		static OS_List* m_delayed_cr_list;
		/** @brief Delayed overflow list.*/
		static OS_List* m_delayed_overflow_cr_list;
		/** @brief Currently executing coroutine */
		static CR_Tasker* m_current_coroutine;
		/** @brief Top ready priority*/
		static BF_Lib::MBASE m_cr_top_ready_priority;
		/** @brief Tick count*/
		static OS_TICK_TD m_cr_tick_count;
		/** @brief Tick count from the timer tick ISR or background task.  Eventually feeds into the m_cr_tick_count.
		 * in check delayed task*/
		static OS_TICK_TD m_cr_interrupt_tick_count;
		/** @brief Last tick count. */
		static OS_TICK_TD m_cr_last_tick_count;
		/** @brief Passed ticks. */
		static OS_TICK_TD m_cr_passed_ticks;
		/** @brief Last task.*/
		static CR_Tasker* m_last_task;
		/** @brief Flag to indicate whether coroutine tasker is running. */
		static bool m_cr_tasker_running;
		/** @brief Watchdog bone. */
		static uC_Watchdog* m_task_watchdog_bone;

	private:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		bool Is_Suspended( void )
		{
			return ( Test_Bit( m_priority, CR_TASKER_SUSPENDED_PRI_BIT ) );
		}

};

#endif
