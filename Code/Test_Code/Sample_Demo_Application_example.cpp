/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/* UART with DMA Example
 */
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/uart.h"
#include "esp32/rom/cache.h"
#include "uC_UHCI.h"
#include "uC_Base.h"
#include "uC_USART_DMA.h"
#include "uC_USART_Buff.h"
#include "esp_types.h"
#include "freertos/task.h"
#include "soc/timer_group_struct.h"
#include "driver/timer.h"
#include "uC_Single_Timer.h"
#include "Sample_Demo_Application_example.h"

// UART input & output pins
#define UART1_RX_PIN     ( 22 )
#define UART1_TX_PIN     ( 23 )


#define TIMER_DIVIDER         16									///< Hardware timer clock divider
#define TIMER_SCALE           ( TIMER_BASE_CLK / TIMER_DIVIDER )	///< convert counter value to seconds
#define TIMER_INTERVAL0_SEC   ( 3.4179 )							///< sample test interval for the first timer
#define TIMER_INTERVAL1_SEC   ( 5.78 )								///< sample test interval for the second timer
#define TEST_WITHOUT_RELOAD   0										///< testing will be done without auto reload
#define TEST_WITH_RELOAD      1										///< testing will be done with auto reload

/**
 * @brief		A sample structure to pass events
 *				from the timer interrupt handler to the main program.
 */
struct timer_event_t
{
	int type;	///< the type of timer's event
	int timer_group;
	int timer_idx;
	uint64_t timer_counter_value;
};

xQueueHandle timer_queue;

/**
 * @brief		A sample structure of UHCI
 */
struct uhci_config_t
{
	uhci_num_t uhci_num;
	uint32_t uart_num;
	uint32_t rx_time_out;
	uint32_t tx_time_out;
	uint32_t intr_mask;
};

uint8_t buf[50];
char buf1[50];
uC_USART_Buff* usart_ctrl;

const uC_BASE_USART_IO_PORT_STRUCT USART_DMA_PIO = {NULL, NULL};

#define USART_DMA_TIMER_CTRL    1

/**
 * @brief		UART hardware init configuration
 */
static void uart_init( void )
{
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_RTS,
		.rx_flow_ctrl_thresh = 100,
		.use_ref_tick = false
	};

	uart_param_config( UART_NUM_1, &uart_config );
}

/**
 * @brief		A simple helper function to print the raw timer counter value
 *              and the counter value converted to seconds
 */
static void inline print_timer_counter( uint64_t counter_value )
{
	printf( "Counter: 0x%08x%08x\n", ( uint32_t ) ( counter_value >> 32 ),
			( uint32_t ) ( counter_value ) );
	printf( "Time   : %.8f s\n", ( double ) counter_value / TIMER_SCALE );
}

/*
 * @brief		Timer group0 ISR handler
 *              We don't call the timer API here because they are not declared with IRAM_ATTR.
 *              If we're okay with the timer irq not being serviced while SPI flash cache is disabled,
 *              we can allocate this interrupt without the ESP_INTR_FLAG_IRAM flag and use the normal API.
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
		evt.type = -1;	///< not supported even type
	}

	/* After the alarm has been triggered
	   we need enable it again, so it is triggered the next time */
	TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;

	// Now just send the event data back to the main program task
	xQueueSendFromISR( timer_queue, &evt, NULL );
}

/*
 * @brief			Initialize selected timer of the timer group
 *                  timer_idx - the timer number to initialize
 *                  auto_reload - should the timer auto reload on alarm?
 *					 timer_interval_sec - the interval of alarm to set
 */
static void example_tg0_timer_init( int timer_idx,
									bool auto_reload, double timer_interval_sec )
{
	uint32_t freq = 0;
	uC_Single_Timer* temp_timer;

	temp_timer = new uC_Single_Timer( timer_idx );

	temp_timer->Set_Timeout( freq );
	timer_isr_register( TIMER_GROUP_0, ( timer_idx_t )timer_idx, timer_group0_isr,
						( void* ) timer_idx, ESP_INTR_FLAG_IRAM, NULL );
	temp_timer->Start_Timeout();
}

/*
 * @brief		The main task of this example program
 */
static void timer_example_evt_task( void* arg )
{
	while ( 1 )
	{
		timer_event_t evt;
		xQueueReceive( timer_queue, &evt, portMAX_DELAY );

		// Print information that the timer reported an event
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

		// Print the timer values passed by event
		printf( "------- EVENT TIME --------\n" );
		print_timer_counter( evt.timer_counter_value );

		// Print the timer values as visible by this task
		printf( "-------- TASK TIME --------\n" );
		uint64_t task_counter_value;
		timer_get_counter_value( ( timer_group_t )evt.timer_group, ( timer_idx_t )evt.timer_idx, &task_counter_value );
		print_timer_counter( task_counter_value );

		// Write through UART TX pin
		memset( buf, 0, 50 );
		sprintf( buf1, "0x%08x%08x\n", ( uint32_t ) ( task_counter_value >> 32 ),
				 ( uint32_t ) ( task_counter_value ) );
		usart_ctrl->Start_TX( ( uint8_t* ) buf1, 50 );
		vTaskDelay( 1500 / portTICK_RATE_MS );

		// Read through UART RX pin
		usart_ctrl->Start_RX( buf, 50 );
	}
}

/*
 * @brief		 In this example, we will test hardware timer0 and timer1 of timer group0.
 */
void phase1_sample_demo_app_main()
{
	// Initialise UART hardware configuration
	uart_init();
	// Configure RX & Tx pins
	uart_set_pin( UART_NUM_1, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE );
	usart_ctrl = new uC_USART_DMA( UART_NUM_1, USART_DMA_TIMER_CTRL, &USART_DMA_PIO );

	timer_queue = xQueueCreate( 10, sizeof( timer_event_t ) );
	example_tg0_timer_init( TIMER_0, TEST_WITHOUT_RELOAD, TIMER_INTERVAL0_SEC );
	xTaskCreate( timer_example_evt_task, "timer_evt_task", 2048, NULL, 5, NULL );
}

#ifdef __cplusplus
}
#endif
