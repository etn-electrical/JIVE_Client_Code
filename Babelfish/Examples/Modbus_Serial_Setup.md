\example  Modbus_Serial
\brief    The sample code to setup a Modbus RTU communication on RS485 port using Modbus_UART. It can send and receive modbus frames over serial port on EasyMxProV7 STM32F207 developement board.

\par Board Specific Settings
 \details  Please do the following setting on EasyMXPro Board to run the sample code successfully.
 \n        1. STM32F207 Card is fitted on the board.
 \n        2. In order to use USB_UART A module you must install FTDI drivers on your computer. Drivers can be found on following link -
 \n           http://www.ftdichip.com/Drivers/VCP.htm
 \n        3  Establish connection between USB UART A and USB connector of host computer.
 \n        4. In order to enable USB-UART A communication, push SW12.1, SW12.2 switches to ON position.

\par Pin Configurations
 \details   Correctly configure the UART, GPIO pins in uC_IO_Define_STM32F207.cpp to setup UART communcation.
 \n         const uC_BASE_USART_IO_PORT_STRUCT MODBUS_SERIAL_PIO = { &USART1_TX_PIO_PA9, &USART1_RX_PIO_PA10 };
 \n         #define MODBUS_SERIAL_PORT          uC_BASE_USART_PORT_1
 \n         #define MODBUS_SERIAL_TIMER_CTRL    uC_BASE_TIMER_CTRL_2

\par Dependency
 \details Following DCID needs to be created in Tools\PatB DCI DB Creator.xls for storing serial comm attributes
 \n 1. DCI_MODBUS_SERIAL_COMM_TIMEOUT_DCID
 \n 2. DCI_EXAMPLE_MODBUS_SERIAL_BAUD_DCID
 \n 3. DCI_EXAMPLE_MODBUS_SERIAL_PARITY_DCID
 \n 4. DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_DCID

 \par Sample Code Listing
~~~~{.cpp}

#include "Modbus_Net.h"
#include "Modbus_UART.h"
#include "uC_USART_DMA.h"
#include "Modbus_DCI_Data.h"
#include "Output_Pos.h"


#include "Modbus_Net.h"
#include "Modbus_UART.h"
#include "uC_USART_DMA.h"
#include "Modbus_DCI_Data.h"
#include "Output_Pos.h"


void Modbus_UART_Setup( void )
{
	static const uint8_t MODBUS_SERIAL_DEFAULT_ADDRESS = 1U;
	Modbus_Net* modbus_serial_net;
	uC_USART_DMA* modbus_serial_dma;
	BF_Mbus::Modbus_Display* modbus_serial_display;
	Modbus_UART* modbus_serial;
	DCI_Owner* modbus_serial_baud;
	DCI_Owner* modbus_serial_parity;
	DCI_Owner* modbus_serial_stopbit;
	uint32_t baudrate;
	uint16_t parity;
	uint16_t stopbit;

	modbus_serial_baud = new DCI_Owner( DCI_EXAMPLE_MODBUS_SERIAL_BAUD_DCID );
	modbus_serial_parity = new DCI_Owner( DCI_EXAMPLE_MODBUS_SERIAL_PARITY_DCID );
	modbus_serial_stopbit = new DCI_Owner( DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_DCID );

	modbus_serial_baud->Check_Out_Init( &baudrate );
	modbus_serial_parity->Check_Out_Init( &parity );
	modbus_serial_stopbit->Check_Out_Init( &stopbit );

	// Create instance of uC_USART_DMA to manage data frames in/out over UART.
	modbus_serial_dma = new uC_USART_DMA( MODBUS_SERIAL_PORT, MODBUS_SERIAL_TIMER_CTRL,
		&MODBUS_SERIAL_PIO );
	// Provides the LED indication with respect to initialized hardware port.
	modbus_serial_display = new BF_Mbus::Modbus_Display(
		new BF_Misc::LED( new BF_Lib::Output_Pos( &PIOC_PIN_6_STRUCT ) ) );
	// Create Modbus_UART instance
	//creating object statically to move the buffers which are member varibles of the class
	// to DTCM RAM region which is non cacheable in F767 target
	static Modbus_UART modbus_uart_obj( modbus_serial_dma, modbus_serial_display,
									 new BF_Lib::Output_Pos( &PIOC_PIN_7_STRUCT ) );
	modbus_serial = &modbus_uart_obj;
	// initialize UART channel and setup modbus DCI data
	static Modbus_Net modbus_net_obj( MODBUS_SERIAL_DEFAULT_ADDRESS, modbus_serial,
										&modbus_dci_data_target_info,
										new DCI_Owner( DCI_MODBUS_SERIAL_COMM_TIMEOUT_DCID ) );
	modbus_serial_net = &modbus_net_obj;
	
	// configure UART message settings and enable communcation on UART channel
	modbus_serial_net->Enable_Port( MODBUS_RTU_TX_MODE, (parity_t)parity, baudrate,
		false, (stop_bits_t)stopbit );
}

~~~~
