/**
 *****************************************************************************************
 * @file    OSCR_Dog.cpp
 *
 * @brief   Implementation of class OSCR_Dog::.
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "OS_Tasker.h"
#include "OSCR_Dog.h"
#include "Babelfish_Assert.h"
// #include <arm_itm.h>

namespace BF_OS
{

uC_Single_Timer* OSCR_Dog::m_timer = nullptr;
OS_Task* OSCR_Dog::m_cr_task = nullptr;
TaskHandle_t OSCR_Dog::m_oscr_dog_wakeup_task = nullptr;
uint8_t OSCR_Dog::m_elevated = 0U;
OSCR_Dog_Config const* OSCR_Dog::m_config = nullptr;
uint32_t OSCR_Dog::m_elevation_cnt = 0U;
uint32_t OSCR_Dog::m_normal_cnt = 0U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OSCR_Dog::OSCR_Dog( OS_Task* cr_task, OSCR_Dog_Config const* config )
{
	/**
	 * @brief Stack size for the Boost OS Task.  Estimated and not
	 * calculated through experimentation.  Opportunity for optimization.
	 *
	 */
#ifdef ESP32_SETUP
	const uint32_t BOOST_OSTASK_STACK_SIZE = 512;
#else
	const uint32_t BOOST_OSTASK_STACK_SIZE = 100;
#endif
	// We are a singleton.  Trying to create multiples will cause essentially a waste of memory.
	BF_ASSERT( m_timer == nullptr );

	m_cr_task = cr_task;
	m_config = config;

	new CR_Tasker( Feed_Dog_Task, 0, "OSCR_Dog task" );

	// Create a task which will be responsible for boosting the CR Tasker.
	OS_Task* os_task = nullptr;

	os_task = OS_Tasker::Create_Task( Booster_OSTask, BOOST_OSTASK_STACK_SIZE,
									  m_config->m_elevated_priority,
									  reinterpret_cast<uint8_t const*>( "OSCR_Dog Booster" ) );
	BF_ASSERT( os_task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OSCR_Dog::~OSCR_Dog()
{
	// This destructor will need implementation.  It really should never be deleted but
	// this exception will catch the possible happening.
	BF_ASSERT( false );

	m_timer = nullptr;
	m_cr_task = nullptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OSCR_Dog::Booster_OSTask( OS_TASK_PARAM task_param )
{
	uint32_t notify_value;

	m_oscr_dog_wakeup_task = xTaskGetCurrentTaskHandle();
#ifdef ESP32_SETUP
	interrupt_config_t interrupt_config;
	interrupt_config.handler = Timer_Elapsed_Callback;
	interrupt_config.arg = reinterpret_cast<void*>( TIMER_0 );
	interrupt_config.priority = ESP_INTR_FLAG_SHARED;
	interrupt_config.ret_handle = nullptr;
#endif
	m_timer = new uC_Single_Timer( m_config->m_timer_num );
	m_timer->Set_Timeout( m_config->m_timer_freq, true );
#ifdef ESP32_SETUP
	/*
	 * In ESP32, ISR register callback method is different,
	 * so as per interrupt_config_t structure we need to set ISR callback as a handler
	 * to the structure member and pass entire structure to Set_Callback().
	 */
	m_timer->Set_Callback( nullptr, reinterpret_cast<uC_Single_Timer::cback_param_t>( &interrupt_config ), 0 );
#else
	m_timer->Set_Callback( Timer_Elapsed_Callback, nullptr, m_config->m_timer_hwint_priority );
#endif
	m_timer->Start_Timeout();

	// coverity[no_escape]
	for (;;)
	{
		// As before, block to wait for a notification form the ISR.  This
		// time however the first parameter is set to pdTRUE, clearing the task's
		// notification value to 0, meaning each outstanding outstanding deferred
		// interrupt event must be processed before ulTaskNotifyTake() is called
		// again.
		// ITM_EVENT8( 3, 3 );
		notify_value = ulTaskNotifyTake( true, OS_TASKER_INDEFINITE_TIMEOUT );
		if ( notify_value > 0 )
		{
			// ITM_EVENT8( 3, 2 );
			// If we got here then the CR_Tasker was neglected.  We need to boost it up then
			// pend again for the ISR.  Since we are the same priority we should
			// set the priority and run right back into the TaskNotifyTake.
			// elevate priority
			m_elevated = 2;
			m_elevation_cnt++;
			OS_Tasker::Set_Priority( m_config->m_elevated_priority, m_cr_task );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OSCR_Dog::Feed_Dog_Task( CR_TASKER_PARAM param )
{
	// hold off the timer
	m_timer->Restart_Timeout();

	if ( m_elevated != 0 )
	{
		m_elevated--;
		if ( m_elevated == 0 )
		{
			// restore normal priority
			m_normal_cnt++;
			// ITM_EVENT8( 3, 4 );
			OS_Tasker::Set_Priority( m_config->m_normal_priority, m_cr_task );
			// ITM_EVENT8( 3, 0 );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OSCR_Dog::Timer_Elapsed_Callback( uC_Single_Timer::cback_param_t param )
{
	BaseType_t higher_priority_task_woken = false;

#ifdef ESP32_SETUP
	// After the alarm has been triggered we need to enable
	// it again, so it is triggered the next time.
	if ( nullptr != m_timer )
	{
		m_timer->Reload_Timer();
	}
#endif
	if ( m_elevated == 0 )
	{
		// Term_Print( "Elevating Scheduler priority to %u", m_config->m_elevated_priority );

		// Unblock the handling task so the task can perform any processing
		// necessitated by the interrupt.  xHandlingTask is the task's handle, which was
		// obtained when the task was created.  vTaskNotifyGiveFromISR() also increments
		// the receiving task's notification value.
		vTaskNotifyGiveFromISR( m_oscr_dog_wakeup_task, &higher_priority_task_woken );
		// ITM_EVENT8( 3, 1 );
		OS_Tasker::Yield_From_ISR( higher_priority_task_woken );
	}
}

}	/* namespace BF_OS */
