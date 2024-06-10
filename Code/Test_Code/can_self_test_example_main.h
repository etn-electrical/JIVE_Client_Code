/**
 *****************************************************************************************
 *	@file		can_self_test_example_main.h
 *
 *	@brief		The file contains the CAN testing details
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
#include "uC_CAN.h"

void can_self_test_app_main();

void can_self_test_app_main_250kbps();

void can_self_test_app_main_500kbps();

void can_self_test_app_main_1000kbps();

void can_test_with_PCAN_View();
