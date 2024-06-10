/**
 *****************************************************************************************
 *	@file		Reset_Test_Example.h
 *
 *	@brief		The file contains the Reset testing details
 *
 *****************************************************************************************
 */

#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "esp_system.h"
#include "uC_Reset.h"

// #define ESP32_TEST_CODE		///< To enable the testing of ESP32 resets

#ifdef ESP32_TEST_CODE

void wdt_reset_test_app_main();

void soft_reset_test_app_main();

void power_on_reset_test_app_main();

#else

void wdt_reset_test_app_main();

void soft_reset_test_app_main();

void power_on_reset_test_app_main();

void brownout_reset_test_app_main();

#endif
