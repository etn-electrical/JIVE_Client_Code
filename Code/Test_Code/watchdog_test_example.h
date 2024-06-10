/**
 *****************************************************************************************
 * @file		watchdog_test_example.h
 *
 * @brief		The file contains the watchdog testing details.
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
#include "esp_int_wdt.h"
#include "uC_Watchdog.h"

/**
 * @brief						uC_Watchdog Test Init App Main to test Watchdog.
 * @return void					None
 */
void uC_Watchdog_Test_Init_App_Main();
