@example  uC_USART_DMA
@brief    The sample code to configure USART port to perform DMA  data RX and data Tx functions on STM32F207 controller. 
           ModbusMAT 1.1 tool is used to verify that the data transmit and recive functions using USART DMA are successfull.

@par Board Specific Settings  
@details  Please do the following setting on EasyMXPro Board to run successfully the sample code
@n        1. STM32F207 Card is fitted on the board.
@n        2. Connect a USB to port USB UART A
@n        3. Enable SW12.1 and SW12.2. This connects USART transmit (USART1_TX_PIO_PA9)and receive 
		    (USART1_TX_PIO_PA9)lines to microcontroller GPIO pins.                                  	        

@par Pin Configurations 
@details   Correctly configure the USART port, Timer and GPIO pins in APP_IO_CONFIG_STM32F207.h to initialize the uC_USART_DMA object as below 
@n        1. #define USART_DMA_PORT	uC_BASE_USART_PORT_1
@n        2. const uC_BASE_USART_IO_PORT_STRUCT USART_DMA_PIO =
		         { &USART1_TX_PIO_PA9, &USART1_RX_PIO_PA10 };
@n        3. #define USART_DMA_TIMER_CTRL	uC_BASE_TIMER_CTRL_2

@par Dependency
@details Not applicable

@par Sample Code Listing
 
~~~~{.cpp}

#include "Ram.h"
#include "Ram_Static.h"
#include "uC_USART_DMA.h"
#include "APP_IO_CONFIG_STM32F207.h"

uC_USART_DMA* usart_dma;
const uint16_t USART_DMA_BUFF_LEN = 8;
uint8_t usart_dma_tx_buff[USART_DMA_BUFF_LEN];
uint8_t usart_dma_rx_buff[USART_DMA_BUFF_LEN];

/**
 * @brief	Example Code for using uC_USART_DMA class
 * @n       The below function does the following
 *          1. Initializes uC_USART_DMA object
 *          2. Configures USART port to perform
 *		       DMA Rx and Tx
 *		    3. Receives data sent from ModbusMAT 1.1
 *		       tool adds 1 to each byte and transmits it back
 */

void uC_USART_DMA_RxTx_Example( void )
{
	usart_dma = new uC_USART_DMA( USART_DMA_PORT, USART_DMA_TIMER_CTRL,
		&USART_DMA_PIO );
	usart_dma->Configure( 19200U, uC_USART_HW::CHAR_SIZE_8BIT,
		uC_USART_HW::PARITY_EVEN, uC_USART_HW::STOP_BIT_1 );
	usart_dma->Start_RX( usart_dma_rx_buff, USART_DMA_BUFF_LEN );

	for ( uint_fast16_t i = 0U; i < USART_DMA_BUFF_LEN; i++ )
	{
		usart_dma_tx_buff[i] = 0U;
		usart_dma_tx_buff[i] = usart_dma_rx_buff[i] + 0x01U;
	}

	usart_dma->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );
}

/**
 * @brief	Example Code for using uC_USART_DMA class
 * @n       The below function does the following
 *          1. Initializes uC_USART_DMA object
 *          2. Configures USART port to perform
 *		       DMA Rx and Tx
 *		    3. Transmits data continuously using CR Tasker class
 *		       Data can be seen in the ModbusMAT 1.1 tool
 */

void uC_USART_DMA_Tx_Example( void )
{
	usart_dma = new uC_USART_DMA( USART_DMA_PORT, USART_DMA_TIMER_CTRL,
		&USART_DMA_PIO );
	usart_dma->Configure( 19200U, uC_USART_HW::CHAR_SIZE_8BIT,
		uC_USART_HW::PARITY_EVEN, uC_USART_HW::STOP_BIT_1 );
	for ( uint_fast16_t i = 0U; i < USART_DMA_BUFF_LEN; i++ )
	{
		usart_dma_tx_buff[i] = 0x01U;
	}
	new CR_Tasker( uC_USART_DMA_Tx, reinterpret_cast<CR_TASKER_PARAM>( NULL ) );
}

/**
 * @brief	Example Code for using uC_USART_DMA class
 * @n       The below function does the following
 *          1. Initializes uC_USART_DMA object
 *          2. Configures USART port to perform
 *		       DMA Rx and Tx
 *		    3. Receives data sent from ModbusMAT 1.1
 *		       tool.
 */

void uC_USART_DMA_Rx_Example( void )
{
	usart_dma = new uC_USART_DMA( USART_DMA_PORT, USART_DMA_TIMER_CTRL,
		&USART_DMA_PIO );
	usart_dma->Configure( 19200U, uC_USART_HW::CHAR_SIZE_8BIT,
		uC_USART_HW::PARITY_EVEN, uC_USART_HW::STOP_BIT_1 );
	new CR_Tasker( uC_USART_DMA_Rx, reinterpret_cast<CR_TASKER_PARAM>( NULL ) );
}

/**
 * @brief	Example Code for using uC_USART_DMA class
 * @n       The below function does the following
 *          1. Calls receive function of DMA class
 *          2. Adds delay after each receive
 */

void uC_USART_DMA_Rx( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	usart_dma->Start_RX( usart_dma_rx_buff, USART_DMA_BUFF_LEN );
	uC_Delay( 50000U );
}

/**
 * @brief	Example Code for using uC_USART_DMA class
 * @n       The below function does the following
 *          1. Calls transmit function of DMA class
 *          2. Adds delay after each transmit
 */

void uC_USART_DMA_Tx( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	usart_dma->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );
	uC_Delay( 50000U );
}

~~~~