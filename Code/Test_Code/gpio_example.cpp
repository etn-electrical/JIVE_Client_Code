/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "uC_IO_Config.h"

#define GPIO_OUTPUT_IO_0    18
#define GPIO_OUTPUT_IO_1    19

#define GPIO_INPUT_IO_0     4
#define GPIO_INPUT_IO_1     5
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle gpio_evt_queue = NULL;

// extern void call_to_babelfish();

static void IRAM_ATTR gpio_isr_handler( void* arg )
{
	uint32_t gpio_num = ( uint32_t ) arg;

	xQueueSendFromISR( gpio_evt_queue, &gpio_num, NULL );
}

static void gpio_task_example( void* arg )
{
	uint32_t io_num;

	while ( true )
	{
		if ( xQueueReceive( gpio_evt_queue, &io_num, portMAX_DELAY ) )
		{
			printf( "GPIO[%d] intr, val: %d\n", io_num, gpio_get_level( ( gpio_num_t )io_num ) );
		}
	}
}

void test_GPIO_app_main()
{
	int cnt = 0;

	// GPIO Configuartion is initialized in the constructor of uC_IO_Config
	// This is test code hence added leaked_storage check
	// coverity[leaked_storage]
	new uC_IO_Config();

	gpio_set_intr_type( ( gpio_num_t )GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE );
	gpio_set_intr_type( ( gpio_num_t )GPIO_INPUT_IO_1, GPIO_INTR_ANYEDGE );

	// create a queue to handle gpio event from isr
	gpio_evt_queue = xQueueCreate( 10, sizeof( uint32_t ) );
	// start gpio task
	xTaskCreate( gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL );

	// install gpio isr service
	gpio_install_isr_service( ESP_INTR_FLAG_DEFAULT );
	// hook isr handler for specific gpio pin
	gpio_isr_handler_add( ( gpio_num_t )GPIO_INPUT_IO_0, gpio_isr_handler, ( void* ) GPIO_INPUT_IO_0 );
	// hook isr handler for specific gpio pin
	gpio_isr_handler_add( ( gpio_num_t )GPIO_INPUT_IO_1, gpio_isr_handler, ( void* ) GPIO_INPUT_IO_1 );

	// remove isr handler for gpio number.
	gpio_isr_handler_remove( ( gpio_num_t )GPIO_INPUT_IO_0 );
	// hook isr handler for specific gpio pin again
	gpio_isr_handler_add( ( gpio_num_t )GPIO_INPUT_IO_0, gpio_isr_handler, ( void* ) GPIO_INPUT_IO_0 );

	while ( true )
	{
		printf( "cnt: %d\n", cnt++ );
		vTaskDelay( 1000 / portTICK_RATE_MS );
		gpio_set_level( ( gpio_num_t )GPIO_OUTPUT_IO_0, cnt % 2 );
		gpio_set_level( ( gpio_num_t )GPIO_OUTPUT_IO_1, cnt % 2 );
	}
}
