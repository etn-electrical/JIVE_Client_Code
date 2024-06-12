/**
 *****************************************************************************************
 *   @file
 *
 *   @brief      Declaration of class OSCR_Dog::.
 *
 *   @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OSCR_Dog_H_
#define OSCR_Dog_H_

#include "OS_Task.h"
#include "uC_Single_Timer.h"
#include "CR_Tasker.h"

namespace BF_OS
{

/**
 * @brief Represents configuration constants for class OSCR_Dog.
 */
struct OSCR_Dog_Config
{
	uint32_t m_timer_freq;
	uint8_t m_timer_num;
	uint8_t m_timer_hwint_priority;
	uint8_t m_normal_priority;
	uint8_t m_elevated_priority;
};


/**
 * @brief Represents a watchdog for the co-routine task scheduler.
 *
 * @details Oscar, the friendly watchdog, monitors the performance of the OS task in which
 * the CR co-routine task scheduler runs using a hardware timer peripheral. If the hardware
 * timer elapses prior to being fed, then the priority of the OS task in which the CR
 * scheduler runs will be elevated. The priority is returned to normal when the CR scheduler
 * is able to make several passes through its idle level tasks.
 *
 * Example usage:
 * #define OSCR_DOG_TIMER				uC_BASE_TIMER_CTRL_1
 * #define OSCR_DOG_FREQUENCY           200U
 * #define OSCR_DOG_HW_INT_PRIORITY		uC_INT_HW_PRIORITY_8
 * #define CR_TASK_NORMAL_PRIORITY      OS_TASK_PRIORITY_LOW_IDLE_TASK
 * #define CR_TASK_ELEVATED_PRIORITY    OS_TASK_PRIORITY_4
 *
 * const BF_OS::OSCR_Dog_Config g_OSCR_Dog_Config =
 * {
 *  200U,
 *  uC_BASE_TIMER_CTRL_1,
 *  uC_INT_HW_PRIORITY_8,
 *  CR_TASK_NORMAL_PRIORITY,
 *  CR_TASK_ELEVATED_PRIORITY
 * };
 *
 * void SNIPPET_FORM_PROD_SPEC( void )
 * {
 *  OS_Task* cr_task = OS_Tasker::Create_Task(
 *      &CR_Tasker::Scheduler,
 *      COROUTINE_OS_TASK_STACK_SIZE,
 *      static_cast<uint8_t>(CR_TASK_NORMAL_PRIORITY),
 *      static_cast<uint8_t const*>("Co-Routine Task"),
 *      reinterpret_cast<OS_TASK_PARAM>(nullptr)
 *      );
 *
 *  new BF_OS::OSCR_Dog( cr_task, &g_OSCR_Dog_Config );
 * }
 */
class OSCR_Dog
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 */
		OSCR_Dog( OS_Task* cr_task, OSCR_Dog_Config const* config );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OSCR_Dog();

	private:
		/**
		 * @brief Reference to a hardware timer.
		 */
		static uC_Single_Timer* m_timer;

		/**
		 * @brief Pointer to the OS task in which the CR scheduler runs..
		 */
		static OS_Task* m_cr_task;

		/**
		 * @brief This is the task we will wake up to elevate CR_Tasker.
		 */
		static TaskHandle_t m_oscr_dog_wakeup_task;

		/**
		 * @brief Count of invocations of Feed_Dog. If value is non-zero, we are elevated.
		 */
		static uint8_t m_elevated;

		/**
		 * @brief Pointer to config struct.
		 */
		static OSCR_Dog_Config const* m_config;

		/**
		 * @brief Diagnostic count of elevations.
		 */
		static uint32_t m_elevation_cnt;

		/**
		 * @brief Diagnostic count of returns to normal.
		 */
		static uint32_t m_normal_cnt;

		/**
		 * @brief Handle the dog barking.
		 */
		static void Timer_Elapsed_Callback( uC_Single_Timer::cback_param_t param );

		/**
		 * @brief CR task to feed the dog.
		 */
		static void Feed_Dog_Task( CR_TASKER_PARAM param );

		/**
		 * @brief This is an OS Task which will suspend and remain suspended
		 * unless the CR_Tasker is neglected and the Feed_Dog_Task is not called
		 * in an appropriate amount of time.  In general:  This task will run and immediately
		 * go to sleep (Suspend).  Under normal situations the CR_Tasker should be calling Feed_Dog_Task.
		 * Inside Feed_Dog_Task we are resetting the hardware timer.  If the hardware timer interrupt
		 * actually triggers due to the CR_Tasker being neglected, this Booster task will
		 * be resumed from that timer interrupt using ResumeFromISR.  This Booster task is
		 * running at the target OS priority level of the CR_Tasker.  That means that after the interrupt
		 * the Booster Task should resume.  The Booster task will then elevate the CR_Tasker task priority and
		 * suspend itself again.  The elevated level of the CR_Tasker will be the same as the Booster Task which
		 * means that the Booster task will suspend and then hand off to the CR_Tasker.  The CR_Tasker
		 * will run two complete loops through then de-elevate itself back down.
		 */
		static void Booster_OSTask( OS_TASK_PARAM task_param );

};

}	/* namespace BF_OS */

#endif	/* OSCR_Dog_H_ */
