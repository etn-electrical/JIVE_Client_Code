@example  uC_USART interupt example
@brief    The sample code to configure USART port to perform Soft DMA  data RX and data Tx functions on STM32F407 controller. 
           ModbusMAT 1.1 tool or Docklight is used to verify that the data transmit and recive functions using USART DMA are successfull.

@par Board Specific Settings  
@details  Please do the following setting on EasyMXPro Board to run successfully the sample code
@n        1. STM32F407 Card is fitted on the board.
@n        2. Connect a USB to port USB UART A
@n        3. Enable SW12.1 and SW12.2. This connects USART transmit (USART1_TX_PIO_PA9)and receive 
		    (USART1_TX_PIO_PA9)lines to microcontroller GPIO pins.                                  	        

@par Pin Configurations 
@details   Correctly configure the USART port, Timer and GPIO pins in APP_IO_CONFIG_STM32F407.h to initialize the uC_USART_HW object as below 
@n        1. #define USART_EXAMPLE_PORT	uC_BASE_USART_PORT_1
@n        2. const uC_BASE_USART_IO_PORT_STRUCT USART_EXAMPLE_PIO =
		         { &USART1_TX_PIO_PA9, &USART1_RX_PIO_PA10 };

@par Dependency
@details Not applicable

@par Sample Code Listing
 
~~~~{.cpp}

#include "Ram.h"
#include "Ram_Static.h"
#include "uC_USART_HW.h"
#include "APP_IO_CONFIG_STM32F407.h"

static uint8_t Read_Data = 0;
uC_USART_HW* usart1;
void usart_txRx( uC_USART_HW::cback_param_t param );

/**
 * @brief	Example Code for using uC_USART_HW class with interrupt
 * @n       The below function does the following
 *          1. Initializes uC_USART_HW object
 *          2. Configures USART port to perform
 *		       Rx and Tx
 *		    3. Interrupt is configured. when we receive data from modmat or docklight interrupt is hit.
 */

void Usart_Enable( void )
{
	usart1 = new uC_USART_HW( USART_EXAMPLE_PORT, &USART_EXAMPLE_PIO, false ); // third argument is for using dma. we set it false to not use dma.
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
	usart1->Configure( 19200, uC_USART_HW::CHAR_SIZE_8BIT,
					   uC_USART_HW::PARITY_NONE, uC_USART_HW::STOP_BIT_1 );
	usart1->Set_Int_Vect( &usart_txRx, usart1, uC_INT_HW_PRIORITY_1 );
	
	usart1->Enable_Int();
	usart1->Enable_RX_Ready_Int();
	usart1->Enable_TX();
	usart1->Enable_RX();
	usart1->Set_TX_Data( 0x31 ); //random data just to start tx
}

/**
 * @brief	Interrupt service routine Code for using uC_USART_HW class with interrupt
 * @n       The below function does the following
 *          1. Checks if interrupt is enabled
 *          2. Reads the received data
 *		    3. Replies back with same received data.
 */
void usart_txRx( uC_USART_HW::cback_param_t param )
{
	bool int_enabled = usart1->Int_Enabled();

	Read_Data = usart1->Get_RX_Data();

	usart1->Set_TX_Data( Read_Data );

}
