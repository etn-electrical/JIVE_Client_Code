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
 * #define CR_TASK_NORMAL_PRIORITY      OS_TASK_PRIORITY_1
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

};

}	/* namespace BF_OS */

#endif	/* OSCR_Dog_H_ */
