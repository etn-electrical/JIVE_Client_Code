/**
 **********************************************************************************************
 * @file			watchdog_test_example.cpp
 * @brief			The following examples demonstrates the testing of watchdog module.
 **********************************************************************************************
 */
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"
#include "Reset_Test_Example.h"
#include "esp_system.h"
#include "uC_IO_Config.h"
#include "soc/rtc.h"
#include "esp_task_wdt.h"
#include "esp_system.h"
#include "esp_int_wdt.h"
#include "uC_Reset.h"
#include "system_internal.h"
#include "esp_int_wdt.h"
#include "watchdog_test_example.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint32_t RIGHT_SHIFT_BY = 10U;			///< Right shift value.
static const uint32_t ENABLE_VAL = 1U;				///< Enable value.
static const uint32_t DISABLE_VAL = 0U;				///< Disable value.
static const uint32_t TIME_OUT_VALUE_1 = 20U;		///< Time out value 1.
static const uint32_t TIME_OUT_VALUE_2 = 1000U;		///< Time out value 2.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog_Test_Init_App_Main( void )
{

	bool watchdog_reset_worked = false;
	uint32_t counter;

	uC_Watchdog* Watchdog_ctrl;

	Watchdog_ctrl = new uC_Watchdog();

	Watchdog_ctrl->Init( TIME_OUT_VALUE_1 );	///< Setting small interval which will trigger watch dog
	esp_reset_reason_set_hint( ESP_RST_INT_WDT );

	Watchdog_ctrl->Handler();

	uC_RESET_CAUSE_TD ret;
	uC_Reset* Reset_ctrl = new uC_Reset();

	ret = Reset_ctrl->Get_Reset_Trigger();


	if ( ret == ( ENABLE_VALUE << RESET_SOFTWARE_TRIGRD ) )
	{
		watchdog_reset_worked = true;
		printf( "Last Triggered reset Reason : RESET_WATCHDOG_TRIGRD \n" );

	}
	else if ( ret == ( ENABLE_VALUE << RESET_WATCHDOG_TRIGRD ) )
	{
		watchdog_reset_worked = true;
		printf( "Last Triggered reset Reason: RESET_WATCHDOG_TRIGRD \n" );
	}
	else if ( ret == ( ENABLE_VALUE << RESET_WAKEUP_TRIGRD ) )
	{
		watchdog_reset_worked = true;
		printf( "Last  Triggered reset Reason: RESET_WAKEUP_TRIGRD \n" );
	}
	else if ( ret == ENABLE_VALUE )
	{
		watchdog_reset_worked = false;
		printf( " Reset Flag Status : Reset NOT Triggered yet \n" );
	}

	if ( watchdog_reset_worked == true )
	{

		Watchdog_ctrl->Init( TIME_OUT_VALUE_2 );	///< Setting interval for a high value
		counter = DISABLE_VAL;
		while ( counter < ( ( ~( uint32_t )DISABLE_VAL ) >> RIGHT_SHIFT_BY ) )
		{
			counter++;
			Watchdog_ctrl->Feed_Dog();
			Watchdog_ctrl->Handler();
		}

		printf( "Watch Dog reset triggered once \n" );
	}
}

#ifdef __cplusplus
}
#endif
