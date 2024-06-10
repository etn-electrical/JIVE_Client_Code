/**
 **********************************************************************************************
 * @file			pwm_test_example.cpp
 * @brief			The following examples demonstrates the testing of PWM module.ESP32 features
 * @n				6 PWM channels with each PWM control block(2 units) with paired channels, where
 * @n				Channel 1 & 2 ,Channel 3 & 4 and Channel 5 & 6. Here for each pair frequency,
 * @n				polarity,counter mode,duty mode will be common.But duty cycle can variable.
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
#include "esp_system.h"
#include "uC_IO_Config.h"
#include "soc/rtc.h"
#include "esp_task_wdt.h"
#include "esp_system.h"
#include "driver/mcpwm.h"
#include "pwm_test_example.h"
#include "uC_IO_Define_ESP32.h"
#include "uC_PWM.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Enable any one of the defines below at a time to test the specified PWM features
#define TEST_PWM_MCPWM_UNIT 0	///< To Test Different PWM units
#define TEST_PWM_DUTY_CYCLE 0	///< To Test Duty cycle periods
#define TEST_PWM_FREQUENCY  0	///< To Test Frequency
#define TEST_PWM_POLARITY  1	///< To Test Polarity


/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

static const char* PWM_TAG = "PWM";

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */



static void MCPWM_PWM_Test( MCPWM_CONFIG pwm_config )
{


	uC_PWM* example_pwm;
	bool pwm_status = false;

	uC_PERIPH_IO_STRUCT port_io;

	port_io.mode = pwm_config.mode;
	port_io.io_num = pwm_config.io_num;


	// create uC_PWM instance, select timer and PWM channel
	example_pwm = new uC_PWM( pwm_config.mcpwm_unit, pwm_config.pwm_channel, &port_io );

	// configure polarity, counter_mode and frequency
	example_pwm->Configure( pwm_config.polarity, pwm_config.counter_mode, pwm_config.frequency );

	// Set PWM duty cycle
	example_pwm->Set_Duty_Cycle_Pct( pwm_config.duty_percent );

	// turn Off PWM Mode
	example_pwm->Turn_Off();

	// check if PWM is turned off
	if ( pwm_status == false )
	{
		// turn On PWM
		example_pwm->Turn_On();
	}

	// read PWM status
	pwm_status = example_pwm->Is_On();

	if ( pwm_status )
	{
		ESP_LOGE( PWM_TAG,
				  "PWM signal started [ MCPWM unit: %d | GPIO pin: %d | Channel :%d | Duty Cycle :%.1f%% | Frequency :%d Hz | polarity : %d | Mode: %d ] \n",
				  pwm_config.mcpwm_unit, pwm_config.io_num, pwm_config.pwm_channel, pwm_config.duty_percent, pwm_config.frequency, pwm_config.polarity,
				  pwm_config.counter_mode );
	}
	else
	{
		ESP_LOGE( PWM_TAG, "NOT able to Start PWM signal" );
	}

}

void uC_PWM_Test_Init_App_Main( void )
{

	MCPWM_CONFIG pwm_config;

#if TEST_PWM_MCPWM_UNIT
	// Test MCPWM_unit 0

	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_1;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 25;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_19;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );

	// Test MCPWM_unit 0
	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_1;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 25;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_18;
	pwm_config.mcpwm_unit = 1;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );

#elif TEST_PWM_DUTY_CYCLE

	// Test Duty Cycle percentages

	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_1;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 25;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_19;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );

	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_2;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 50;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_18;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );


	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_3;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 75;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_21;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );


	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_4;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 75;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_22;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );


	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_5;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 25.5;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_15;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );

	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_6;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 55.5;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_14;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );

#elif  TEST_PWM_FREQUENCY

	/* ESP32 features 6 PWM channels with each PWM control block with paired channels,	where Channel 1 & 2 ,Channel 3 &
	       4 and Channel 5 & 6. Here for each pair
	    frequency,polarity,counter mode,duty mode will be common.But duty cycle can variable.
	 */
	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_1;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 50;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_19;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );

	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_2;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 25;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_18;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );


	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_3;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 50;
	pwm_config.frequency = 500;
	pwm_config.io_num = GPIO_NUM_21;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );


	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_4;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 25;
	pwm_config.frequency = 500;
	pwm_config.io_num = GPIO_NUM_22;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );


	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_5;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 50;
	pwm_config.frequency = 1000;
	pwm_config.io_num = GPIO_NUM_15;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );

	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_6;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 75;
	pwm_config.frequency = 1000;
	pwm_config.io_num = GPIO_NUM_14;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	MCPWM_PWM_Test( pwm_config );


#elif TEST_PWM_POLARITY

	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_1;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 25;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_19;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_1;		// Active low duty

	MCPWM_PWM_Test( pwm_config );

	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_2;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 25;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_18;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;		// Active high duty

	MCPWM_PWM_Test( pwm_config );


	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_3;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 75;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_21;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_1;		// Active Low duty

	MCPWM_PWM_Test( pwm_config );


	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_4;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.duty_percent = 75;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_22;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;		// Active high duty

	MCPWM_PWM_Test( pwm_config );

#endif

}

#ifdef __cplusplus
}
#endif
