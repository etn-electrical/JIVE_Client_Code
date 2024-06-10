/**
 *****************************************************************************************
 * @file      Test_Ext_Int.cpp
 * @details   Contains the test application for external interrupt
 *            configured for gpio's.
 *            There are two gpio's configured as an interrupt. By default one is enabled.
 *            Second gpio configuration can be enabled by un-commenting "TWO_EXT_INTERRUPT"
 *            On pressing push button( boot button ) on board, interrupt will be triggered
 *            and log will be available on terminal.
 *
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Test_Ext_Int.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "uC_IO_Config.h"
#include "uC_Ext_Int.h"

/*
 *****************************************************************************************
 *		Defines
 *****************************************************************************************
 */

/*
 * Un-comment below macro TWO_EXT_INTERRUPT to enable one more external interrupt support
 * To test this interrupt,
 * Connect GPIO18 with GPIO4, That will generate interrupt
 */

// #define TWO_EXT_INTERRUPT

#define PUSH_BUTTON_GPIO    0		///< gpio pin for push button( BOOT button on board )
#define GPIO_INPUT_IO_0     4		///< gpio test pin
#define GPIO_OUTPUT_IO_0    18		///< gpio test pin

#define GPIO_TASK_QUEUE_LENGTH   10
#define GPIO_TASK_STACK_SIZE     2048
#define GPIO_TASK_PRIORITY       10
#define ONE_SEC_DELAY            1000

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
static xQueueHandle gpio_evt_queue = nullptr;

/*
 ********************************************************************************
 *		IO Definitions
 ********************************************************************************
 */
uC_USER_IO_STRUCT push_button_io = {GPIO_MODE_INPUT, PUSH_BUTTON_GPIO, GPIO_PIN_INTR_ANYEDGE};
uC_USER_IO_STRUCT test_input_gpio = {GPIO_MODE_INPUT, GPIO_INPUT_IO_0, GPIO_PIN_INTR_ANYEDGE};

/**
 ******************************************************************************************
 * @brief			: Interrupt handler
 * @in              : arg to get gpio number for which interrupt is registered
 * @return Void		: None
 ******************************************************************************************
 */
static void IRAM_ATTR Ext_Int_Callback( void* arg )
{
	uint32_t gpio_num = reinterpret_cast<uint32_t>( arg );

	xQueueSendFromISR( gpio_evt_queue, &gpio_num, NULL );
}

/**
 ******************************************************************************************
 * @brief			: GPIO Task to handle data received from ISR
 * @in              : arg not used
 * @return Void		: None
 ******************************************************************************************
 */
static void gpio_task_example( void* arg )
{
	uint32_t io_num;

	while ( true )
	{
		if ( xQueueReceive( gpio_evt_queue, &io_num, portMAX_DELAY ) )
		{
			printf( "GPIO[%d] intr, val: %d\n", io_num, gpio_get_level( static_cast<gpio_num_t>( io_num ) ) );
		}
	}
}

/**
 ******************************************************************************************
 * @brief			: This function will attach and initialize the interrupts
 * @return Void		: None
 ******************************************************************************************
 */
void Test_Ext_Int_App_Main( void )
{
	uint32_t cnt = 0;

	/*
	 * GPIO configuration is initialized in the constructor of uC_IO_Config
	 * This is test code hence added leaked_storage check
	 */
	// coverity[leaked_storage]
	new uC_IO_Config();

	/* Push button( BOOT button ) configuration as an external interrupt */
	uC_Ext_Int* Push_Button = new uC_Ext_Int( &push_button_io );

	/* Fill interrupt config structure */
	interrupt_config_t interrupt_config;

	interrupt_config.source = ETS_GPIO_INTR_SOURCE;
	interrupt_config.handler = Ext_Int_Callback;
	interrupt_config.arg = reinterpret_cast<void*>( PUSH_BUTTON_GPIO );

	/*
	 * Attach_IRQ(): This method does the following things
	 * 1. Attaches vector callback as per allocated handler
	 * 2. Install/allocate interrupt
	 * 3. Defines triggering method(rising edge, falling edge or level trigger)
	 */
	Push_Button->Attach_IRQ( reinterpret_cast<INT_CALLBACK_FUNC*>( &interrupt_config ), GPIO_INTR_ANYEDGE,
							 static_cast<uC_IO_INPUT_CFG_EN>( 0 ), 0 );

#ifdef TWO_EXT_INTERRUPT
	/* gpio configuration as an interrupt */
	uC_Ext_Int* Test_Ext_Int_2 = new uC_Ext_Int( &test_input_gpio );

	interrupt_config.source = ETS_GPIO_INTR_SOURCE;
	interrupt_config.handler = Ext_Int_Callback;
	interrupt_config.arg = reinterpret_cast<void*>( GPIO_INPUT_IO_0 );

	Test_Ext_Int_2->Attach_IRQ( reinterpret_cast<INT_CALLBACK_FUNC*>( &interrupt_config ), GPIO_INTR_ANYEDGE,
								uC_IO_IN_CFG_HIZ, 0 );
#endif	// TWO_EXT_INTERRUPT

	/* Create a queue to handle gpio event from isr */
	gpio_evt_queue = xQueueCreate( GPIO_TASK_QUEUE_LENGTH, sizeof( uint32_t ) );

	/* Start gpio task */
	xTaskCreate( gpio_task_example, "gpio_task_example", GPIO_TASK_STACK_SIZE, NULL,
				 GPIO_TASK_PRIORITY, NULL );

	while ( true )
	{
		printf( "cnt: %d\n", cnt++ );
		vTaskDelay( ONE_SEC_DELAY / portTICK_RATE_MS );
#ifdef TWO_EXT_INTERRUPT
		uC_IO_Config::Set_GPIO_Level( static_cast<gpio_num_t>( GPIO_OUTPUT_IO_0 ), cnt % 2 );
#endif
	}
}
