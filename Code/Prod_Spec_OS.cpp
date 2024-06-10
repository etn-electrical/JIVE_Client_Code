/**
 **********************************************************************************************
 * @file            Prod_Spec_OS.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to OS component.
 *
 *
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
// #define USE_FULL_RTOS 1

#include "Includes.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "Prod_Spec_OS.h"
#include "CR_Tasker.h"
#include "DCI_Owner.h"
#include "OSCR_Mon.h"
#include "OSCR_Dog.h"
#include "OS_Task.h"
#ifdef USE_FULL_RTOS
#include "OS_Tasker.h"
#endif
#include "uC_Tick.h"

#include "uC_Watchdog.h"
#include "uC_Interrupt.h"
#include "esp_freertos_hooks.h"

#include "Breaker.h"

extern DRAM_ATTR Device_Info_str DeviceInfo;

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#ifdef USE_FULL_RTOS
const uint16_t COROUTINE_OS_TASK_STACK_SIZE = 4096U;
#endif

#ifndef USE_FULL_RTOS
/* CR_Tasker tick interval in milliseconds.  Not used if RTOS.*/
const uint32_t CR_TASKER_TICK_FREQUENCY = 1000U;
#endif

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
OS_Task* g_cr_task = nullptr;

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

void Context_Action_Cback( OS_Task::context_action_param_t context_param,
						   OS_Task::context_action_t context_action );

/**
 **********************************************************************************************
 * @brief                     Product Specific Target OS Initialization
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_Target_OS_Init( void )
{

	// Disabled Watchdog task as we dont have Watchdog working as of now
	// coverity[leaked_storage]
	new CR_Tasker( uC_Watchdog::Handler, NULL, "uC_Watchdog" );

#ifdef USE_FULL_RTOS

	// coverity[leaked_storage]
	new OS_Tasker();

	g_cr_task = OS_Tasker::Create_Task( CR_Tasker::Scheduler,
										COROUTINE_OS_TASK_STACK_SIZE,
//Ahmed_DPS_ADU come back here
										OS_TASK_PRIORITY_1,
                                        //OS_TASK_PRIORITY_LOW_IDLE_TASK,
										reinterpret_cast<uint8_t const*>( "Co-Routine Task" ), NULL );
	if ( g_cr_task == NULL )
	{
		printf( "\n Unable to create CR_Tasker::Scheduler OS_Task" );
	}

	// Register Hook callback mechanism using ESP32 API
	// This mechanism works and we are able to create and run multiple CR Tasker
	esp_err_t register_tick_hook = esp_register_freertos_tick_hook( CR_Tasker::Tick_Event );
	if ( register_tick_hook != ESP_OK )
	{
		printf( "\n Error in registering tick hook: %d", register_tick_hook );
	}


#if defined( OS_TASK_MEASURE_RUN_TIME ) && defined( CR_TASKER_DEBUG )
	g_cr_task->Context_Action_Cback( Context_Action_Cback );
#endif

#else
	uC_Tick* puCTick = new uC_Tick( CR_Tasker::Tick_Event,
									CR_TASKER_TICK_FREQUENCY, uC_INT_HW_PRIORITY_7 );
	if ( puCTick == NULL )
	{
		printf( "\n Unable to create CR_Tasker::Tick_Event OS_Task" );
	}

#endif
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {
	printf( "\n Initialized CR Task functionality" );
    }

}

/**
 **********************************************************************************************
 * @brief                     Initialization of  Operating system for product specific tasks
 * @param[in]                 none
 * @return[out] none          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_Target_Start_OS( void )
{
	/* LTK-4719: OSCR_Dog timer interrupt handler elevates CR_Tasker priority. This elevation should happen
	   after os start event. In initialization scratch area gets cleared after fw upgrade which
	   takes significant time. Thus OSCR_Dog timer should start at end of initialization phase */
	//Ahmed PROD_SPEC_OS_Oscr_Dog_Init();
}

/**
 **********************************************************************************************
 * @brief                     Idle Task in the OS Scheduler
 * @param[in]                 OS task parameter
 * @return[out] none          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_OS_Oscr_Dog_Init( void )
{
#if defined ( HTTPS_SUPPORT ) || defined ( PX_GREEN_IOT_SUPPORT ) || defined ( PX_GREEN_DTLS_SUPPORT )

	static const BF_OS::OSCR_Dog_Config g_OSCR_Dog_Config =
	{
		OSCR_DOG_FREQUENCY,
		OSCR_DOG_TIMER,
		OSCR_DOG_HW_INT_PRIORITY,
		CR_TASK_NORMAL_PRIORITY,
		CR_TASK_ELEVATED_PRIORITY
	};
	/* Oscr Dog is required to initialize prior to Device Certificate generation */
	new BF_OS::OSCR_Dog( g_cr_task, &g_OSCR_Dog_Config );
#endif
}

/**
 **********************************************************************************************
 * @brief                     Monitor CPU utilisation
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_OS_Cpu_Usage_Monitor( void )
{
	// coverity[leaked_storage]
	DCI_Owner* cr_priority_times = new DCI_Owner( DCI_CR_PRIORITY_TIMING_DCID );
	// coverity[leaked_storage]
	DCI_Owner* os_priority_times = new DCI_Owner( DCI_OS_PRIORITY_TIMING_DCID );
	// coverity[leaked_storage]
	DCI_Owner* cpu_util = new DCI_Owner( DCI_CPU_UTILIZATION_DCID );

	// This function can also be used without the DCI parameters.
	// coverity[leaked_storage]
	OSCR_Mon::Init( cpu_util, cr_priority_times, os_priority_times );
}

/**
 **********************************************************************************************
 * @brief Provides a method of pausing the CR_Tasker task timing monitors so
 * that we do not take into account the time spent in the RTOS in the CR_Tasker.
 * @param context_param: The parameter which was passed in.  Not used in this function.
 * @param context_action: Whether the task is entering or exiting the active RTOS task level.
 **********************************************************************************************
 */
void Context_Action_Cback( OS_Task::context_action_param_t context_param,
						   OS_Task::context_action_t context_action )
{
	BF_Lib::Unused<OS_Task::context_action_param_t>::Okay( context_param );

	if ( context_action == OS_Task::ENTERING )
	{
		CR_Tasker::Tasker_Resumed();
	}
	else
	{
		CR_Tasker::Tasker_Paused();
	}
}
