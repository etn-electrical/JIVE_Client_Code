/* Timer group-hardware timer example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */


#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "uC_Single_Timer.h"
#include "timer_group_example_main.h"
#include "uC_Interrupt.h"
#include "uC_Base.h"


#define TIMER_DIVIDER         16	// Hardware timer clock divider
#define TIMER_SCALE           ( TIMER_BASE_CLK / TIMER_DIVIDER )	// convert counter value to seconds
#define TIMER_INTERVAL0_SEC   ( 3.4179 )// sample test interval for the first timer
#define TIMER_INTERVAL1_SEC   ( 5.78 )	// sample test interval for the second timer
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

/*
 * A simple helper function to print the raw timer counter value
 * and the counter value converted to seconds
 */
static void inline print_timer_counter( uint64_t counter_value )
{
	printf( "Counter: 0x%08x%08x\n", ( uint32_t ) ( counter_value >> 32 ),
			( uint32_t ) ( counter_value ) );
	printf( "Time   : %.8f s\n", ( double ) counter_value / TIMER_SCALE );
}

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
	int timer_idx = ( int ) para;

	/* Retrieve the interrupt status and the counter value
	   from the timer that reported the interrupt */
	uint32_t intr_status = TIMERG0.int_st_timers.val;

	TIMERG0.hw_timer[timer_idx].update = 1;
	uint64_t timer_counter_value =
		( ( uint64_t ) TIMERG0.hw_timer[timer_idx].cnt_high ) << 32
			| TIMERG0.hw_timer[timer_idx].cnt_low;

	/* Prepare basic event data
	   that will be then sent back to the main program task */
	timer_event_t evt;

	evt.timer_group = 0;
	evt.timer_idx = timer_idx;
	evt.timer_counter_value = timer_counter_value;

	/* Clear the interrupt
	   and update the alarm time for the timer with without reload */
	if ( ( intr_status & BIT( timer_idx ) ) && ( timer_idx == TIMER_0 ) )
	{
		evt.type = TEST_WITHOUT_RELOAD;
		TIMERG0.int_clr_timers.t0 = 1;
		timer_counter_value += ( uint64_t ) ( TIMER_INTERVAL0_SEC * TIMER_SCALE );
		TIMERG0.hw_timer[timer_idx].alarm_high = ( uint32_t ) ( timer_counter_value >> 32 );
		TIMERG0.hw_timer[timer_idx].alarm_low = ( uint32_t ) timer_counter_value;
	}
	else if ( ( intr_status & BIT( timer_idx ) ) && ( timer_idx == TIMER_1 ) )
	{
		evt.type = TEST_WITH_RELOAD;
		TIMERG0.int_clr_timers.t1 = 1;
	}
	else
	{
		evt.type = -1;	// not supported even type
	}

	/* After the alarm has been triggered
	   we need enable it again, so it is triggered the next time */
	TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;

	/* Now just send the event data back to the main program task */
	xQueueSendFromISR( timer_queue, &evt, NULL );
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
	uint32_t freq = 0;
	uC_Single_Timer* temp_timer;
	interrupt_config_t interrupt_config;

	interrupt_config.handler = timer_group0_isr;
	interrupt_config.arg = ( void* ) timer_idx;
	interrupt_config.priority = ESP_INTR_FLAG_IRAM;
	interrupt_config.ret_handle = NULL;

	temp_timer = new uC_Single_Timer( timer_idx );

	temp_timer->Set_Timeout( freq );
	temp_timer->Set_Callback( nullptr, reinterpret_cast<uC_Single_Timer::cback_param_t>( &interrupt_config ), 0 );
	temp_timer->Start_Timeout();
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

		/* Print information that the timer reported an event */
		if ( evt.type == TEST_WITHOUT_RELOAD )
		{
			printf( "\n    Example timer without reload\n" );
		}
		else if ( evt.type == TEST_WITH_RELOAD )
		{
			printf( "\n    Example timer with auto reload\n" );
		}
		else
		{
			printf( "\n    UNKNOWN EVENT TYPE\n" );
		}
		printf( "Group[%d], timer[%d] alarm event\n", evt.timer_group, evt.timer_idx );

		/* Print the timer values passed by event */
		printf( "------- EVENT TIME --------\n" );
		print_timer_counter( evt.timer_counter_value );

		/* Print the timer values as visible by this task */
		printf( "-------- TASK TIME --------\n" );
		uint64_t task_counter_value;
		timer_get_counter_value( ( timer_group_t )evt.timer_group, ( timer_idx_t )evt.timer_idx, &task_counter_value );
		print_timer_counter( task_counter_value );
	}
}

/*
 * In this example, we will test hardware timer0 and timer1 of timer group0.
 */
void timer_test_app_main()
{
	timer_queue = xQueueCreate( 10, sizeof( timer_event_t ) );
	example_tg0_timer_init( TIMER_0, TEST_WITHOUT_RELOAD, TIMER_INTERVAL0_SEC );
	example_tg0_timer_init( TIMER_1, TEST_WITH_RELOAD,    TIMER_INTERVAL1_SEC );
	xTaskCreate( timer_example_evt_task, "timer_evt_task", 2048, NULL, 5, NULL );
}
