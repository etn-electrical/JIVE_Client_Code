\example  Modbus_Master
\brief    The sample code to setup a Modbus Master RTU communication on RS485 port using Modbus_UART. This example uses OS_Tasker.

\par Board Specific Settings
 \details  This example is tested on below hardware:
 \n        1. Nucleo STM32F429 board.
 \n		   2. Nucleo STM32F767 board.
 \n		Note: For F7 target move this file rw object to DTCM RAM Region which is non cacheble region in linker file.
 \n		Example:Add "place at start of DTCMRAM_region	{ rw object Modbus_Master_Example.o };" in RTK-stm32f767_flash_With_Uberloader.icf
\par Pin Configurations
 \details   Correctly configure the UART, GPIO pins in uC_IO_Define_STM32F427.cpp to setup UART communcation.
 \n         const uC_BASE_USART_IO_PORT_STRUCT MODBUS_SERIAL_PIO = { &USART2_TX_PIO_PD5, &USART2_RX_PIO_PD6 };
 \n         #define MODBUS_SERIAL_PORT          uC_BASE_USART_PORT_2
 \n         #define MODBUS_SERIAL_TIMER_CTRL    uC_BASE_TIMER_CTRL_2
 \n			Ensure that this port is not used by any other module like Modbus Slave. Comment Modbus Slave Initilization.
 

 \par Sample Code Listing
~~~~{.cpp}
#include "Includes.h"
#include "Prod_Spec_LTK_STM32F.h"
#include "Prod_Spec_ModbusUART.h"
#include "Modbus_UART.h"
#include "uC_USART_DMA.h"
#include "Modbus_DCI_Data.h"
#include "Output_Null.h"
#include "Output_Pos.h"
#include "Modbus_Master.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint16_t MODBUS_MASTER_OS_STACK_SIZE = 500U;
static const uint16_t MODBUS_MASTER_RESPONSE_TIMEOUT = 250;		//in milliseconds
static const uint16_t MODBUS_MASTER_MAX_MSG_RETRIES = 3U;



uC_USART_DMA* modbus_serial_dma = nullptr;
BF_Mbus::Modbus_Display* modbus_serial_display = nullptr;
Modbus_UART* modbus_serial = nullptr;
BF_Mbus::Modbus_Master *modbus_master = nullptr;

void Modbus_Master_Task( OS_TASK_PARAM handle );

void Modbus_Master_Init( void )
{
	modbus_serial_dma = new uC_USART_DMA( MODBUS_SERIAL_PORT, MODBUS_SERIAL_TIMER_CTRL,
										  &MODBUS_SERIAL_PIO );
	modbus_serial_display = new BF_Mbus::Modbus_Display(
			new BF_Misc::LED( new BF_Lib::Output_Null() ) );
#ifdef RS_485_ENABLE
	/*If RS485 is used, asign a port pin for direction control*/
	BF_Lib::Output_Pos *rs485_tx_enable = new BF_Lib::Output_Pos( &RS485_SLAVE_TXEN_PIO );
#else
	/*else use output null*/
	BF_Lib::Output_Null*rs485_tx_enable = new BF_Lib::Output_Null();
#endif
	static Modbus_UART modbus_uart_obj( modbus_serial_dma, modbus_serial_display,
									 rs485_tx_enable );
	modbus_serial = &modbus_uart_obj;

	static BF_Mbus::Modbus_Master Modbus_Master_obj( modbus_serial, MODBUS_MASTER_RESPONSE_TIMEOUT );

	modbus_master = &Modbus_Master_obj;

	modbus_master->Enable_Port( MODBUS_RTU_TX_MODE, MODBUS_PARITY_EVEN,
		MODBUS_BAUD_19200,
		TRUE, MODBUS_1_STOP_BIT );
						
	OS_Tasker::Create_Task( &Modbus_Master_Task,
					   MODBUS_MASTER_OS_STACK_SIZE,
					   OS_TASK_PRIORITY_2,
					   reinterpret_cast<uint8_t const* >("Modbus Master Task"),
					   nullptr);
}

void Modbus_Master_Task( OS_TASK_PARAM handle )
{
	uint8_t device_address = 1U;
	uint16_t read_reg = 1U;
	uint16_t write_reg = 10U;
	uint16_t byte_count = 2U;
	uint8_t read_data[256U];
	uint8_t write_data[2U] = {0x0U};
	uint8_t attribute = 0U;
	uint16_t counter_data = 0U;
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint16_t m_inter_message_delay_ms = 100U;
	uint8_t error_count = 0U;
	
	while (1)
	{
		//Read register query :
		//Slave device address = 1; Register address = 1; Read Register Count = 1;
		error_count = 0;
		do
		{
			OS_Tasker::Delay( m_inter_message_delay_ms );
			return_status = modbus_master->Get_Reg(device_address, read_reg, byte_count, read_data, attribute);
			error_count++;
		} while ( ( error_count <= MODBUS_MASTER_MAX_MSG_RETRIES ) && ( return_status != MB_NO_ERROR_CODE ) );

		//Write register query:
		//Slave device address = 1; Register address = 10; Read Register Count = 1;
		counter_data++;
		write_data[0U] = static_cast <uint8_t> (counter_data);
		write_data[1U] = static_cast <uint8_t> (counter_data >> 8U);
				
		modbus_master->Set_Reg(device_address, write_reg, byte_count, write_data, attribute);
		error_count = 0;
		do
		{
			OS_Tasker::Delay( m_inter_message_delay_ms );
			return_status = modbus_master->Set_Reg(device_address, write_reg, byte_count, write_data, attribute);
			error_count++;
		} while ( ( error_count <= MODBUS_MASTER_MAX_MSG_RETRIES ) && ( return_status != MB_NO_ERROR_CODE ) );
		OS_Tasker::Delay(1000U);
	}
}
~~~~
