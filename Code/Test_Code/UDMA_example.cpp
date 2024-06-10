/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/* UART with DMA Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
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
#include "uC_USART_HW.h"
#include "UDMA_example.h"


#define UART1_RX_PIN     ( 22 )
#define UART1_TX_PIN     ( 23 )


typedef struct
{
	uhci_num_t uhci_num;
	uint32_t uart_num;
	uint32_t rx_time_out;
	uint32_t tx_time_out;
	uint32_t intr_mask;
} uhci_config_t;


uint8_t buf[50];

uC_USART_Buff* usart_ctrl;
const uC_BASE_USART_IO_PORT_STRUCT USART_DMA_PIO = {NULL, NULL};
#define USART_DMA_TIMER_CTRL    1



void uart_test_app_main()
{

	volatile int count = 0;

	/* Create object for USART DMA */
	usart_ctrl = new uC_USART_DMA( UART_NUM_1, USART_DMA_TIMER_CTRL, &USART_DMA_PIO );

	/* Configure RX & Tx pins */
	uart_set_pin( UART_NUM_1, UART1_TX_PIN, UART1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE );

	/*  UART hardware configuration */
	usart_ctrl->Config_Port( 115200, uC_USART_Buff::CHAR_SIZE_8BIT,
							 uC_USART_Buff::PARITY_NONE, uC_USART_Buff::STOP_BIT_1 );


	while ( 1 )
	{

		/* Read test */

		memset( buf, 0, 50 );
		usart_ctrl->Start_RX( buf, 50 );
		printf( "count =%d ,read: %s  \n", count, buf );

		usart_ctrl->Start_TX( buf, 50 );

		/* Write Test */
		vTaskDelay( 1500 / portTICK_RATE_MS );
		count++;
	}

}

#ifdef __cplusplus
}
#endif



