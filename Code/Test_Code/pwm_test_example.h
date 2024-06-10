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
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "Etn_Types.h"


typedef struct
{
	uint8_t mcpwm_unit;					///< Select Unit 0 or 1
	uint8_t pwm_channel;				///< Channel num 1-6
	float32_t duty_percent;					///< Duty cycle in percentage
	uint32_t frequency;					///< frequency of MCPWM in Hz
	gpio_mode_t mode;					///< GPIO mode: set input/output mode
	gpio_num_t io_num;					///< IO pins
	mcpwm_duty_type_t polarity;			///< Active low/High
	mcpwm_counter_type_t counter_mode;	///< Set  type of MCPWM counter

} MCPWM_CONFIG;

/**
 * @brief	uC_PWM_Test_Init_App_Main
 * @return void					None
 */
void uC_PWM_Test_Init_App_Main();


