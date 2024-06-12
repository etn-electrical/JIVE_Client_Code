@example  uC_USART_Soft_DMA
@brief    The sample code to configure USART port to perform Soft DMA  data RX and data Tx functions on STM32F407 controller. 
           ModbusMAT 1.1 tool is used to verify that the data transmit and recive functions using USART DMA are successfull.

@par Board Specific Settings  
@details  Please do the following setting on EasyMXPro Board to run successfully the sample code
@n        1. STM32F407 Card is fitted on the board.
@n        2. Connect a USB to port USB UART A
@n        3. Enable SW12.1 and SW12.2. This connects USART transmit (USART1_TX_PIO_PA9)and receive 
		    (USART1_TX_PIO_PA9)lines to microcontroller GPIO pins.                                  	        

@par Pin Configurations 
@details   Correctly configure the USART port, Timer and GPIO pins in APP_IO_CONFIG_STM32F407.h to initialize the uC_USART_SOFT_DMA object as below 
@n        1. #define USART_DMA_PORT	uC_BASE_USART_PORT_1
@n        2. const uC_BASE_USART_IO_PORT_STRUCT USART_PIO =
		         { &USART1_TX_PIO_PA9, &USART1_RX_PIO_PA10 };
@n        3. #define USART_TIMER_CTRL	uC_BASE_TIMER_CTRL_2

@par Dependency
@details Not applicable

@par Sample Code Listing
 
~~~~{.cpp}

#include "Ram.h"
#include "Ram_Static.h"
#include "uC_USART_SOFT_DMA.h"
#include "APP_IO_CONFIG_STM32F407.h"

uC_USART_Soft_DMA* usart_soft_dma;
const uint16_t USART_DMA_BUFF_LEN = 8;
uint8_t usart_dma_tx_buff[USART_DMA_BUFF_LEN];
uint8_t usart_dma_rx_buff[USART_DMA_BUFF_LEN];

//This is merely the bit times for an EOF RX Idle interrupt.
//Time calc is boosted by a value which boosts things into a microsecond type of framework.
//Calculation is microsecond boost * bits_per_char ( 8 bits of data + 1 parity + 1 stop bit + 1 start bit = 11)
const uint32_t RX_DONE_TIME_CALC = static_cast <uint32_t> ( 1000000.0 * 11 );
const uint32_t TX_HOLDOFF_TIME_CALC = static_cast <uint32_t> ( 1000000.0 * 11 );
static uint8_t Read_Data = 0;
void uC_USART_Soft_DMA_Tx( CR_Tasker* cr_task, CR_TASKER_PARAM param );
static void UART_Callback_Static( uC_USART_Buff::cback_param_t param,
			uC_USART_Buff::cback_status_t status );

/**
 * @brief	Example Code for using uC_USART_DMA class
 * @n       The below function does the following
 *          1. Initializes uC_USART_SOFT_DMA object
 *          2. Configures USART port to perform
 *		       DMA Rx and Tx
 *		    3. Sets dma req bits which determines reason we want interrupt to occur		       
 */
void uC_USART_Soft_DMA_Tx_Example( void )
{
	uint32_t rx_done_time;
    uint32_t tx_holdoff_time;
	uC_USART_Buff::cback_status_t usart_dma_req_bits;

	usart_soft_dma = new uC_USART_Soft_DMA( uC_BASE_USART_PORT_1, USART_TIMER_CTRL,
		&USART_PIO );
		
		/**
		 * @brief Configures the port.  Can be executed at any time.
		 * @param1 baud_rate - Baud rate in bps. 
		 * @param2 char_size - Controls the number of bits per character. 
			available character size
				CHAR_SIZE_5BIT = 5,
				CHAR_SIZE_6BIT = 6,
				CHAR_SIZE_7BIT = 7,
				CHAR_SIZE_8BIT = 8,
				CHAR_SIZE_9BIT = 9
		 * @param3 parity - Parity of the uart.
			available parity options
				PARITY_EVEN,
				PARITY_ODD,
				PARITY_SPACE,
				PARITY_MARK,
				PARITY_NONE,
		 * @param4 stop_bit_count - Stop bit count.
			available stop bit options
				STOP_BIT_1,
				STOP_BIT_1p5,
				STOP_BIT_2
		 * @return success - True if the configuration succeeded.False if it failed to config.
		 */
	usart_soft_dma->Configure( 19200U, uC_USART_HW::CHAR_SIZE_8BIT,
		uC_USART_HW::PARITY_EVEN, uC_USART_HW::STOP_BIT_1 );
		
		

	usart_dma_req_bits = 0U; // these bits describe which interrupts are needed
	rx_done_time = (uint32_t)RX_DONE_TIME_CALC / 19200U; // we have to divide the calc constant with baud rate to get time for a rx to complete
    tx_holdoff_time = (uint32_t)TX_HOLDOFF_TIME_CALC / 19200U;

	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::RX_BYTE_TIMEOUT ); // will ensure interrupt is hit on rx_byte_timeout
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::RX_BUFF_FULL );// will ensure interrupt is hit on rx_buff_full
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::TX_COMPLETE );// will ensure interrupt is hit on tx_complete
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::TXRX_HOLDOFF_COMPLETE );// will ensure interrupt is hit on tx and rx hold off time is expired
	
	//we configure the callback to be called on interrupt and reason for interrupt.
	usart_soft_dma->Config_Callback( &UART_Callback_Static, usart_soft_dma, usart_dma_req_bits, rx_done_time, tx_holdoff_time );

	usart_soft_dma->Start_RX( usart_dma_rx_buff, USART_DMA_BUFF_LEN );
}

/**
 * @brief	Callback function called when any interrupt is triggered
 * @n       The below function does the following
 *          1. Checks the state of the rx
 *          2. if its not idle receive the data 
 *		    3. data received is sent back adding 1 to the received data
 */
 static void UART_Callback_Static( uC_USART_Buff::cback_param_t param,
			uC_USART_Buff::cback_status_t status )
{
	if ( Test_Bit( status, uC_USART_Buff::RX_BYTE_TIMEOUT ) )
    {
		for ( uint_fast16_t i = 0U; i < USART_DMA_BUFF_LEN; i++ )
		{
			usart_dma_tx_buff[i] = 0U;
			usart_dma_tx_buff[i] = usart_dma_rx_buff[i] + 0x01U;
		}
		usart_soft_dma->Stop_RX();
		usart_soft_dma->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );

		usart_soft_dma->Start_RX( usart_dma_rx_buff, USART_DMA_BUFF_LEN );
    }

    if ( Test_Bit( status, uC_USART_Buff::TX_COMPLETE ) )
    {
        // logic to execute when transimission complete interrupt occurs
    }

    if ( Test_Bit( status, uC_USART_Buff::TXRX_HOLDOFF_COMPLETE ) )
    {
       // logic to execute when holdoff time is expired and interrupt occurs
    }
}
~~~~