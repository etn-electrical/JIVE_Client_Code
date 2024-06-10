/**
 *****************************************************************************************
 *	@file		Test_OS_Example.cpp
 *	@brief		See header file for module overview.
 *****************************************************************************************
 */

#include <stdio.h>
#include "Includes.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "Test_OS_Example.h"

/* Enable this macro to test critical section handling using below APIs
 * Push_TGINT(), Pop_TGINT()
 * portENTER_CRITICAL( &spinlock ), portEXIT_CRITICAL( &spinlock )
 * Find the details about these APIs at below link in section "Critical Sections & Disabling Interrupts"
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/freertos-smp.html
 * */
// #define CRITICAL_SECTION_TEST

#ifdef CRITICAL_SECTION_TEST
#include "esp_types.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include <unistd.h>

#define TIMER_DIVIDER         16	// Hardware timer clock divider
#define TIMER_SCALE           ( TIMER_BASE_CLK / TIMER_DIVIDER )	// convert counter value to seconds
#define TIMER_INTERVAL1_SEC   ( 0.001 )		// sample test interval for the second timer
#define TEST_WITHOUT_RELOAD   0			// testing will be done without auto reload
#define TEST_WITH_RELOAD      1			// testing will be done with auto reload

/*
 * A sample structure to pass events
 * from the timer interrupt handler to the main program.
 */
typedef struct
{
	int type;	// the type of timer's event
	int timer_group;
	int timer_idx;
	uint64_t timer_counter_value;
} timer_event_t;

xQueueHandle timer_queue;

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
uint32_t interrupt_cntr = 0U;

static void example_tg0_timer_init( int timer_idx,
									bool auto_reload, double timer_interval_sec );

static void timer_example_evt_task( void* arg );

#endif	// CRITICAL_SECTION_TEST

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Task_1( void* arg )
{
	while ( true )
	{
		printf( "\n Inside OS_Task_1" );

#ifdef CRITICAL_SECTION_TEST
		uint32_t interrupt_cntr0 = 0U;
		uint32_t interrupt_cntr1 = 0U;
		// portENTER_CRITICAL( &spinlock );
		Push_TGINT();
		interrupt_cntr0 = interrupt_cntr;
		sleep( 1 );	///< wait for 1 sec, it should be greater than the interrupt interval
		interrupt_cntr1 = interrupt_cntr;
		Pop_TGINT();
		// portEXIT_CRITICAL( &spinlock );

		/* If the difference between interrupt_cntr1 and interrupt_cntr0 is zero
		 * that means interrupt is not serviced during critical section */
		printf( "\n interrupt_cntr0 = %d, interrupt_cntr1 = %d, diff = %d", interrupt_cntr0, interrupt_cntr1,
				( interrupt_cntr1 - interrupt_cntr0 ) );
#endif	// CRITICAL_SECTION_TEST

		vTaskDelay( 2000 / portTICK_RATE_MS );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Task_2( void* arg )
{
	while ( true )
	{
		printf( "\n Inside OS_Task_2 " );
		vTaskDelay( 1000 / portTICK_RATE_MS );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Test_OS_Task( void )
{
	// Create OS Task
	xTaskCreate( OS_Task_1, "OS_Task_Example_1", 2048, NULL, 10, NULL );
	xTaskCreate( OS_Task_2, "OS_Task_Example_2", 2048, NULL, 11, NULL );
#ifdef CRITICAL_SECTION_TEST
	timer_queue = xQueueCreate( 10, sizeof( timer_event_t ) );
	example_tg0_timer_init( TIMER_1, TEST_WITH_RELOAD,    TIMER_INTERVAL1_SEC );
	xTaskCreate( timer_example_evt_task, "timer_evt_task", 2048, NULL, 5, NULL );
#endif	// CRITICAL_SECTION_TEST
}

#ifdef CRITICAL_SECTION_TEST
/*
 * Timer group0 ISR handler
 *
 * Note:
 * We don't call the timer API here because they are not declared with IRAM_ATTR.
 * If we're okay with the timer irq not being serviced while SPI flash cache is disabled,
 * we can allocate this interrupt without the ESP_INTR_FLAG_IRAM flag and use the normal API.
 */
void IRAM_ATTR timer_group0_isr( void* para )
{
	interrupt_cntr++;
	timer_spinlock_take( TIMER_GROUP_0 );
	int timer_idx = ( int ) para;

	/* Retrieve the interrupt status and the counter value
	   from the timer that reported the interrupt */
	uint32_t timer_intr = timer_group_get_intr_status_in_isr( TIMER_GROUP_0 );
	uint64_t timer_counter_value = timer_group_get_counter_value_in_isr( TIMER_GROUP_0, ( timer_idx_t )timer_idx );

	/* Prepare basic event data
	   that will be then sent back to the main program task */
	timer_event_t evt;

	evt.timer_group = 0;
	evt.timer_idx = timer_idx;
	evt.timer_counter_value = timer_counter_value;

	/* Clear the interrupt */
	if ( timer_intr & TIMER_INTR_T1 )
	{
		evt.type = TEST_WITH_RELOAD;
		timer_group_clr_intr_status_in_isr( TIMER_GROUP_0, TIMER_1 );
	}
	else
	{
		evt.type = -1;	// not supported even type
	}

	/* After the alarm has been triggered
	   we need enable it again, so it is triggered the next time */
	timer_group_enable_alarm_in_isr( TIMER_GROUP_0, ( timer_idx_t )timer_idx );

	/* Now just send the event data back to the main program task */
	xQueueSendFromISR( timer_queue, &evt, NULL );
	timer_spinlock_give( TIMER_GROUP_0 );
}

/*
 * Initialize selected timer of the timer group 0
 *
 * timer_idx - the timer number to initialize
 * auto_reload - should the timer auto reload on alarm?
 * timer_interval_sec - the interval of alarm to set
 */
static void example_tg0_timer_init( int timer_idx,
									bool auto_reload, double timer_interval_sec )
{
	/* Select and initialize basic parameters of the timer */
	timer_config_t config = {
		.alarm_en = TIMER_ALARM_EN,
		.counter_en = TIMER_PAUSE,
		.counter_dir = TIMER_COUNT_UP,
		.auto_reload = ( timer_autoreload_t )auto_reload,
		.divider = TIMER_DIVIDER,
	};	// default clock source is APB

	timer_init( TIMER_GROUP_0, ( timer_idx_t )timer_idx, &config );

	/* Timer's counter will initially start from value below.
	   Also, if auto_reload is set, this value will be automatically reload on alarm */
	timer_set_counter_value( TIMER_GROUP_0, ( timer_idx_t )timer_idx, 0x00000000ULL );

	/* Configure the alarm value and the interrupt on alarm. */
	timer_set_alarm_value( TIMER_GROUP_0, ( timer_idx_t )timer_idx, timer_interval_sec * TIMER_SCALE );
	timer_enable_intr( TIMER_GROUP_0, ( timer_idx_t )timer_idx );
	timer_isr_register( TIMER_GROUP_0, ( timer_idx_t )timer_idx, timer_group0_isr,
						( void* ) timer_idx, ESP_INTR_FLAG_IRAM, NULL );

	timer_start( TIMER_GROUP_0, ( timer_idx_t )timer_idx );
}

/*
 * The main task of this example program
 */
static void timer_example_evt_task( void* arg )
{
	while ( 1 )
	{
		timer_event_t evt;
		xQueueReceive( timer_queue, &evt, portMAX_DELAY );

		vTaskDelay( 10 );
	}
}

#endif	// CRITICAL_SECTION_TEST
