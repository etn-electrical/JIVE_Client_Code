@example  uC_I2C
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

#include "uc_I2C.h"
#include "Ram.h"
#include "Ram_Static.h"

uC_I2C* i2c_ctrl;
EEPROM_Manager* i2c_ctrl_test2;
EEPROM_Manager* i2c_ctrl_test3;
void I2C_Simple_Class_Async_DMA( void );
void I2C_Simple_Class_Async_NO_DMA( void );
void I2C_Simple_Class_Sync_NO_DMA( void );

/* These variables will become 1 when transaction is finished */
/* These variables are set o value 1 inside callbak function */
volatile uint8_t write_finish = 0;
volatile uint8_t read_finish = 0;
volatile bool_t transfer_finish = false;
/* The bytes to be written on EEPROM */
uint8_t transmit_byte[8];
/* The bytes Received from EEPROM */
uint8_t receive_byte[8];
//* Macro which denotes the I2C address of the chip
#define MEMORY_ADDRESS 0x50U

class EEPROM_Manager
{
	public:

          EEPROM_Manager( uint8_t channel, uint16_t device_address, uC_I2C::i2c_mode_t speed_mode,
                            uC_I2C::address_type_t addr_mode, bool_t synchronus_trnasfer, bool_t enable_dma,
                            uC_I2C::duty_type_t fastmode_duty_cycle = uC_I2C::FASTMODE_DUTYCYCLE_16_9  );

		void Read_Data( uint16_t address, uint16_t length, uint8_t* datasource );

		void Write_Data( uint16_t address, uint16_t length, uint8_t* datasource );
                
        static volatile bool_t m_transfer_finish;

	private:

		uC_I2C* m_i2c_control;
                
        void Transfer_Finish(void);
                
		static void Transfer_Finish_Static( uC_I2C::cback_param_t param )
        {
                  ( (EEPROM_Manager*)param )->Transfer_Finish();
        }

};


EEPROM_Manager::EEPROM_Manager( uint8_t channel, uint16_t device_address, uC_I2C::i2c_mode_t speed_mode,
                            uC_I2C::address_type_t addr_mode, bool_t synchronus_trnasfer, bool_t enable_dma,
                            uC_I2C::duty_type_t fastmode_duty_cycle )
{
	m_i2c_control = new uC_I2C( channel, device_address, speed_mode, addr_mode,
		synchronus_trnasfer, enable_dma, fastmode_duty_cycle );
	m_i2c_control->Attach_Callback( &EEPROM_Manager::Transfer_Finish_Static, this );
}


void EEPROM_Manager::Read_Data( uint16_t address, uint16_t length,
	uint8_t* datasource )
{
	m_i2c_control->Read( address, length, datasource );
}


void EEPROM_Manager::Write_Data( uint16_t address, uint16_t length,
	uint8_t* datasource )
{
	m_i2c_control->Write( address, length, datasource);
}


void EEPROM_Manager::Transfer_Finish( void )
{
	EEPROM_Manager::m_transfer_finish = true;
}

/*
 * brief                    Example Code for establishing I2C communication with EEPROM
 * note                     The below function does the following
 *                          1. Initialization of uC_I2C class
 *                          2. Establishing communication with EEPROM
 *                          3. Reading and writing info from EEPROM
 *                          4. It do not use DMA. Howvever the trasfer is asynchronous.
 *                          5. Application will get callback for each I2C transfer completion.
 */
void I2C_Simple_Class_Async_NO_DMA(void)
{
	/* The byte address from/to which data to be read/written */
	uint8_t byte_address;
	/* Amount of data to be read/written */
	uint8_t data_length;

	/* lets try to perform read/write operation on location 0x10.
	 available locations are 0 to 127(0x7F) */
	byte_address = 0x10U;
	/* The buffer to be written over an EEPROM */
	transmit_byte[0] = 0x66U;
	transmit_byte[1] = 0x66U;
	transmit_byte[2] = 0x77U;
	transmit_byte[3] = 0x88U;
	transmit_byte[4] = 0x99U;
	transmit_byte[5] = 0xAAU;
	transmit_byte[6] = 0xBBU;
	transmit_byte[7] = 0xEEU;
    EEPROM_Manager::m_transfer_finish = false;
                      //Advanced_Vect::m_cback_funcs_counter = 2U;

	/* length od data to be transmitted */
	data_length = 0x08U;

	/* Initialize the I2C1 of microcontroller. EEPROM is connected on I2C1
	 Set standard mode/100KHz bit rate. EEPROM addressing is 7bit and DMA transfer is not used
	 */
    i2c_ctrl_test3 = new EEPROM_Manager( uC_BASE_I2C_CTRL_1, MEMORY_ADDRESS, uC_I2C::I2C_STANDARD_MODE,
		uC_I2C::ADDRESS_7_BIT,false, false );
	/* Transmit the data to slave EEPROM */
                      
	i2c_ctrl_test3->Write_Data( byte_address, data_length, &transmit_byte[0] );
                      
    while(EEPROM_Manager::m_transfer_finish == false);
                      /* Wait while callback is not generated */

    EEPROM_Manager::m_transfer_finish = false;
	/* Page write time for EEPROM */
	uC_Delay( 6500U );

	i2c_ctrl_test3->Read_Data( byte_address, data_length, &receive_byte[0] );
    while(EEPROM_Manager::m_transfer_finish == false);              
}
/* async with interrupt*/
/*
 * brief                    Example Code for establishing I2C communication with EEPROM
 * note                     The below function does the following
 *                          1. Initialization of uC_I2C class
 *                          2. Establishing communication with EEPROM
 *                          3. Reading and writing info from EEPROM
 *                          4. It uses DMA. Howvever the trasfer is asynchronous.
 *                          5. Application will get callback for each I2C transfer completion.
 */
void I2C_Simple_Class_Async_DMA( void )
{
	/* The byte address from/to which data to be read/written */
	uint8_t byte_address;
	/* Amount of data to be read/written */
	uint8_t data_length;

	/* lets try to perform read/write operation on location 0x10.
	 available locations are 0 to 127(0x7F) */
	byte_address = 0x10U;
	/* The buffer to be written over an EEPROM */
	transmit_byte[0] = 0x66U;
	transmit_byte[1] = 0x66U;
	transmit_byte[2] = 0x77U;
	transmit_byte[3] = 0x88U;
	transmit_byte[4] = 0x99U;
	transmit_byte[5] = 0xAAU;
	transmit_byte[6] = 0xBBU;
	transmit_byte[7] = 0xEEU;
    EEPROM_Manager::m_transfer_finish = false;
                      //Advanced_Vect::m_cback_funcs_counter = 2U;

	/* length od data to be transmitted */
	data_length = 0x08U;

	/* Initialize the I2C1 of microcontroller. EEPROM is connected on I2C1
	 Set standard mode/100KHz bit rate. EEPROM addressing is 7bit and DMA transfer is not used
	 */
    i2c_ctrl_test2 = new EEPROM_Manager( uC_BASE_I2C_CTRL_1, MEMORY_ADDRESS, uC_I2C::I2C_STANDARD_MODE,
		uC_I2C::ADDRESS_7_BIT,false, true );
	/* Transmit the data to slave EEPROM */
                      
	i2c_ctrl_test2->Write_Data( byte_address, data_length, &transmit_byte[0] );
                      
    while(EEPROM_Manager::m_transfer_finish == false);
                      /* Wait while callback is not generated */

    EEPROM_Manager::m_transfer_finish = false;
	/* Page write time for EEPROM */
	uC_Delay( 6500U );

	i2c_ctrl_test2->Read_Data( byte_address, data_length, &receive_byte[0] );
    while(EEPROM_Manager::m_transfer_finish == false);  

}

/*
 * brief                    Example Code for establishing I2C communication with EEPROM
 * note                     The below function does the following
 *                          1. Initialization of uC_I2C class
 *                          2. Establishing communication with EEPROM
 *                          3. Reading and writing info from EEPROM
 *                          4. It do not use DMA fro I2C byte transfer. The transaction is synchronous
 *                          5. No callbacks are generated. The transfer status is returned in the passed
 *                             argument
 */
void I2C_Simple_Class_Sync_NO_DMA( void )
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

	/* length od data to be transmitted */
	data_length = 0x08U;
                      
	/* Initialize the I2C1 of microcontroller. EEPROM is connected on I2C1
	 Set standard mode/100KHz bit rate. EEPROM addressing is 7bit and DMA transfer is not used
	 */
	i2c_ctrl = new uC_I2C( uC_BASE_I2C_CTRL_1, MEMORY_ADDRESS, uC_I2C::I2C_STANDARD_MODE,
		uC_I2C::ADDRESS_7_BIT,true,false );
		
	i2c_ctrl->Write( byte_address, data_length, &transmit_byte[0], &tx_status );


	if ( uC_I2C::I2C_FINISH_TX == tx_status )
	{
		/* Page write time for EEPROM */
		uC_Delay( 6500U );

		i2c_ctrl->Read( byte_address, data_length, &receive_byte[0], &rx_status );
	}
	else
	{
		//Error
	}
}
~~~~