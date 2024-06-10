/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BLINK_EXAMPLE_H
#define BLINK_EXAMPLE_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/**
 * @brief			Function to test led blinking on ESP32 Wrover kit
 * @return Void		None
 */
void Blink_LED_Example( void );

#endif	// #ifndef BLINK_EXAMPLE_H
