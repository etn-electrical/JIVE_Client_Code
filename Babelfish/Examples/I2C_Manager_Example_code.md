@example  I2C_Manager
@brief    The sample code to initialize the I2C of STM32Fx07 controller and sending commands to
		  24AA01 EEPROM chip over an I2C
@par Board Specific Settings  
@details  Please do the following setting on EasyMXPro Board to run successfully the sample code.
@n        1. STM32Fx07 Card is fitted on the board.
@n        2. Enable SW14.1, SW14.2 switches. This connects I2C lines of controller to that of EEPROM chip.

@par Pin Configurations 
@details   Correctly configure the GPIO pins for I2C1(alternate function 4) in uC_IO_Define_STM32F207.cpp as below 
@n        1. const uC_PERIPH_IO_STRUCT 	I2C1_SCL_PIO_PB6    = { GPIOB, IO_PIN_6, uC_IO_ALTERNATE_MAP_4 }; //SCL pin
@n        2. const uC_PERIPH_IO_STRUCT 	I2C1_SDA_PIO_PB7   = { GPIOB, IO_PIN_7, uC_IO_ALTERNATE_MAP_4 }; //SDA Pin

@par Dependency
@details Not applicable

@par Sample Code Listing
 
~~~~{.cpp}

#include "I2C_Manager.h"
#include "Ram.h"
#include "Ram_Static.h"

void I2C_Manager_Class_Sync_DMA( void );

/* The bytes to be written on EEPROM */
uint8_t transmit_byte[8];
/* The bytes Received from EEPROM */
uint8_t receive_byte[8];
//* Macro which denotes the I2C address of the chip
#define MEMORY_ADDRESS 0x50U

/*
 * brief                    Example Code for establishing I2C communication with EEPROM
 * note                     The below function does the following
 *                          1. Initialization of I2C_Manager class
 *                          2. Establishing communication with EEPROM
 *                          3. Reading and writing info from EEPROM
 *                          4. It uses DMA for I2C byte transfer. The transaction is synchronous using OS functionalitys
 *                          5. Note this class(I2C_Manager) can only be used in OS based applications.
 *                          6. The class uses OS functionality and e.g. below function(I2C_Manager_Class_Sync_DMA) should be a
 *                           task under OS_Takser
 */
void I2C_Manager_Class_Sync_DMA( void )
{
	/* Data transmission status */
    uC_I2C::i2c_status_t tx_status = uC_I2C::NO_ERROR_INIT;
	/* Data reception status/error */
	uC_I2C::i2c_status_t rx_status = uC_I2C::NO_ERROR_INIT;
	/* The byte address from/to which data to be read/written */
	uint8_t byte_address;
	/* Amount of data to be read/written */
	uint8_t data_length;
	/* The bytes to be written on EEPROM */
	uint8_t transmit_byte[8];
	/* The bytes Received from EEPROM */
	uint8_t receive_byte[8];
	I2C_Manager* i2c_control;

	/* lets try to perform read/write operation on location 0x10.
	 available locations are 0 to 127(0x7F) */
	byte_address = 0x10U;
	/* The buffer to be written over an EEPROM */
	transmit_byte[0] = 0x78U;
	transmit_byte[1] = 0x66U;
	transmit_byte[2] = 0x77U;
	transmit_byte[3] = 0x88U;
	transmit_byte[4] = 0x99U;
	transmit_byte[5] = 0xAAU;
	transmit_byte[6] = 0xBBU;
	transmit_byte[7] = 0xEEU;

	/* length of data to be transmitted */
	data_length = 0x08U;
                      
	/* Initialize the I2C1 of microcontroller. EEPROM is connected on I2C1
	 Set standard mode/100KHz bit rate. EEPROM addressing is 7bit and DMA transfer is not used
	 */
	i2c_control = new uC_I2C( uC_BASE_I2C_CTRL_1, MEMORY_ADDRESS, uC_I2C::I2C_STANDARD_MODE,
		uC_I2C::ADDRESS_7_BIT);
	/* Transmit the data to slave EEPROM */
	i2c_control->Write( byte_address, data_length, &transmit_byte[0], &tx_status );


	if ( uC_I2C::I2C_FINISH_TX == tx_status )
	{
		/* Page write time for EEPROM */
		uC_Delay( 6500U );
		i2c_control->Read( byte_address, data_length, &receive_byte[0], &rx_status );
	}
	else
	{
	//Error
	}
}
~~~~