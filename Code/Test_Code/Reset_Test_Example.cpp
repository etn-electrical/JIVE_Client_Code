/**
 **********************************************************************************************
 * @file			reset_test_example.cpp
 * @brief			The following examples demonstrates the testing of rest module
 **********************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include "Reset_Test_Example.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/twai.h"
#include "uC_IO_Config.h"
#include "soc/rtc.h"
#include "system_internal.h"
extern "C"
{
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "brownout.h"
}

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const char* EXAMPLE_TAG = "Reset test";
static const uint8_t RESET_WAIT_COUNT = 5;

#ifdef ESP32_TEST_CODE

/**
 * @brief					Function used to perform the watchdog reset of ESP32
 * @return Void				None
 */
void wdt_reset_test_app_main()
{
	esp_task_wdt_init( 1, true );		///< Initialization of watch dog timer reset
	esp_task_wdt_add( xTaskGetIdleTaskHandleForCPU( 0 ) );

	ESP_LOGI( EXAMPLE_TAG, "Watch dog timer reset" );

	for ( int i = RESET_WAIT_COUNT; i >= 0; i-- )
	{
		printf( "Restarting in %d seconds...\n", i );
		vTaskDelay( 1000 / portTICK_PERIOD_MS );
	}

	while ( 1 )
	{}
}

/**
 * @brief					Function used to perform the software reset of ESP32
 * @return Void				None
 */
void soft_reset_test_app_main()
{
	ESP_LOGI( EXAMPLE_TAG, "software reset" );

	for ( int i = RESET_WAIT_COUNT; i >= 0; i-- )
	{
		printf( "Restarting in %d seconds...\n", i );
		vTaskDelay( 1000 / portTICK_PERIOD_MS );
	}

	printf( "Restarting now.\n" );
	esp_restart();
}

/**
 * @brief					Function used to perform the Power on reset of ESP32
 * @return Void				None
 */
void power_on_reset_test_app_main()
{
	ESP_LOGI( EXAMPLE_TAG, "Power on reset" );

	for ( int i = 20 * RESET_WAIT_COUNT; i >= 0; i-- )
	{
		printf( "Press EN button of ESP32 board to perform poweron reset...\n" );
		vTaskDelay( 1000 / portTICK_PERIOD_MS );
	}
}

#else

/**
 * @brief					Function used to perform the watchdog reset of mapped PX green reset code
 * @return Void				None
 */
void wdt_reset_test_app_main()
{
	uint32_t temp_ret;

	esp_task_wdt_init( 1, true );		///< Initialization of watch dog timer reset
	esp_task_wdt_add( xTaskGetIdleTaskHandleForCPU( 0 ) );
	esp_reset_reason_set_hint( ESP_RST_TASK_WDT );

	ESP_LOGI( EXAMPLE_TAG, "Watch dog timer reset" );

	for ( int i = RESET_WAIT_COUNT; i >= 0; i-- )
	{
		printf( "Restarting in %d seconds...\n", i );
		vTaskDelay( 1000 / portTICK_PERIOD_MS );
	}

	vTaskDelay( 500 / portTICK_PERIOD_MS );
	temp_ret = uC_Reset::Get_Reset_Trigger();

	if ( !( uC_Reset::Get_Bit( temp_ret, RESET_TASK_WDT ) ) )
	{
		printf( "Restarting now.\n" );
		uC_Reset::Watchdog_Reset();
	}
	else
	{
		printf( "Watch dog reset is not successful...\n" );
	}
}

/**
 * @brief					Function used to perform the software reset of mapped PX green reset code
 * @return Void				None
 */
void soft_reset_test_app_main()
{
	ESP_LOGI( EXAMPLE_TAG, "software reset" );

	for ( int i = RESET_WAIT_COUNT; i >= 0; i-- )
	{
		printf( "Restarting in %d seconds...\n", i );
		vTaskDelay( 500 / portTICK_PERIOD_MS );
	}

	vTaskDelay( 500 / portTICK_PERIOD_MS );

	printf( "Restarting now.\n" );
	uC_Reset::Soft_Reset();
}

/**
 * @brief					Function used to perform the Power on reset of mapped PX green reset code
 * @return Void				None
 */
void power_on_reset_test_app_main()
{
	ESP_LOGI( EXAMPLE_TAG, "Power on reset" );

	for ( int i = 20 * RESET_WAIT_COUNT; i >= 0; i-- )
	{
		printf( "Press RST button of ESP32 board to perform poweron reset...\n" );
		vTaskDelay( 1000 / portTICK_PERIOD_MS );
	}
}

/**
 * @brief					Function used to perform the brownout reset of mapped PX green reset code
 * @return Void				None
 */
void brownout_reset_test_app_main()
{
	esp_brownout_init();

	ESP_LOGI( EXAMPLE_TAG, "Brownout reset" );

	for ( int i = RESET_WAIT_COUNT; i >= 0; i-- )
	{
		printf( "Restarting in %d seconds...\n", i );
		vTaskDelay( 500 / portTICK_PERIOD_MS );
	}

	vTaskDelay( 500 / portTICK_PERIOD_MS );
	printf( "Restarting now. \n" );
	uC_Reset::Soft_Reset();
}

#endif
