/*
   @example  uC_I2C Master
   @brief    The sample code to demonstrate the I2C Master functionality of STM32FXXX controller and sending commands to
          I2C Slave (May be I2C Slave sample code of STM32FXXX can be used on the other side)
   @par Pin Configurations
   @details  Please do the following connection to run successfully the sample code.
   @n        1. PB8 (SCL) and PB9 (SDA) shall be connected to I2C Slave respective pins
   @n        2. If these pins already in use, then refer the uC_IO_Define_STM32FXXX.cpp for alternate I2C Ports and
      configure
   @n           the SCL and SDA pin in the below I2C_TEST_IO_PORT structure.

   @par Slave addrees and I2C Mode configurations
   @details  Please do update the following configurations
   @n        1. Slave address configured in this example is 7 bit address and address is 0x02
   @n        2. Transfer mode can be configigurable by using the variable "i2c_transfer_mode" and
   @n			    "i2c_dma_state"
   @n		 3. By default its set to I2C_SYNC_TRANSFER_MODE and ENABLE_DMA mode
   @n        4. I2C Standard mode is configured and tested


   @par Dependency
   @details Not applicable

   @par Sample Code Listing

   ~~~~{.cpp}
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */

#include "uc_I2C.h"
#include "CR_Tasker.h"
#include "stdio.h"
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define I2C_DEVICE_ADDR             0x02U
#define I2C_ASYNC_TRANSFER_MODE    ( 0x0 )
#define I2C_SYNC_TRANSFER_MODE     ( 0x1 )
#define DISABLE_DMA                ( 0x00 )
#define ENABLE_DMA                 ( 0x01 )

// ***********************************************************
// **** I2C Port IO Pins
// ***********************************************************
const uC_BASE_I2C_IO_PORT_STRUCT I2C_TEST_IO_PORT[] =
{
	{&I2C1_SCL_PIO_PB8, &I2C1_SDA_PIO_PB9}
};

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

void I2C_Test_Init( void );

void I2C_Test_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param );

uC_I2C::i2c_status_t I2C_Read_Example( uC_I2C* i2c_ctrl );

uC_I2C::i2c_status_t I2C_Write_Example( uC_I2C* i2c_ctrl );

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
bool_t i2c_transfer_mode = I2C_SYNC_TRANSFER_MODE;
bool_t i2c_dma_state = ENABLE_DMA;
uC_I2C* i2c_ctrl;
bool_t first_time_flag = true;	// This variable set to true to run I2C Write for the first time without depending on
								// the Tx status.
/**
 **********************************************************************************************
 * @brief                             static function for Sample_App thread
 * @param[in] cr_task                 CR_Tasker handle
 * @param[in] cr_param                CR_Tasker Parameter
 * @return[out] none                  none
 * @n
 **********************************************************************************************
 */
static void I2C_App_Thread_Static( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param )
{
	I2C_Test_Task( cr_task, cr_param );
}

/*
 * brief                    I2C class initialization with specific configuration to be tested
 * note                     The below function does the following
 *                          1. Initialization of uC_I2C class
 *                          2. CR Task created to read or write the data to I2C Slave repeatedly.
 */
void I2C_Test_Init( void )
{
	/*Sync with DMA*/
	i2c_ctrl = new uC_I2C( uC_BASE_I2C_CTRL_1, I2C_DEVICE_ADDR, uC_I2C::I2C_STANDARD_MODE,
						   uC_I2C::ADDRESS_7_BIT, i2c_transfer_mode, i2c_dma_state,
						   &I2C_TEST_IO_PORT[uC_BASE_I2C_CTRL_1] );

	new CR_Tasker( &I2C_App_Thread_Static, NULL, CR_TASKER_IDLE_PRIORITY,
				   ( char_t const* )( "I2C_App_Thread_Static" ) );
}

/*
 * brief                    Example Code for establishing I2C communication with any I2C Slace device
 * note                     The below function does the following
 *                          1. Initialization of uC_I2C class
 *                          2. Establishing communication with I2C Slave
 *                          3. Reading and writing info from I2C Slave
 *                          4. It do not use DMA fro I2C byte transfer. The transaction is synchronous
 *                          5. No callbacks are generated. The transfer status is returned in the passed
 *                             argument
 */

uC_I2C::i2c_status_t I2C_Read_Example( uC_I2C* i2c_ctrl )
{
	/* Data reception status/error */
	uC_I2C::i2c_status_t rx_status = uC_I2C::NO_ERROR_INIT;
	/* The byte address from which data to be read*/
	uint16_t byte_address;
	/* Amount of data to be read */
	const uint16_t data_length = 12;

	static uint8_t read_buffer[data_length];

	byte_address = I2C_DEVICE_ADDR;

	/*To read data from the I2C slave*/
	i2c_ctrl->Read( byte_address, data_length, read_buffer, &rx_status );

	return ( rx_status );
}

/*
 * brief                    Example Code for establishing I2C communication with any I2C Slace device
 * note                     The below function does the following
 *                          1. Initialization of uC_I2C class
 *                          2. Establishing communication with I2C Slave
 *                          3. Reading and writing info from I2C Slave
 *                          4. Transfer mode can be configigured by using the variable "i2c_transfer_mode" and
 *								"i2c_dma_state"
 *                          5. No callbacks are generated. The transfer status is returned in the passed
 *                             argument
 */

uC_I2C::i2c_status_t I2C_Write_Example( uC_I2C* i2c_ctrl )
{
	/* Data transmission status */
	uC_I2C::i2c_status_t tx_status = uC_I2C::NO_ERROR_INIT;
	/* The byte address to which data to be written */
	uint16_t byte_address;
	/* Amount of data to be written */
	uint16_t data_length = 12;
	/* The bytes to be written on I2C Slave */
	// static uint8_t transmit_byte[12];

	/*The string need to be transmitted to test Tx with I2C Slave device*/
	static char const* sample_string = "Hello World";

	byte_address = I2C_DEVICE_ADDR;

	/*Below byte byte by byte Hex also shall be reffered to fill the transmit bytes*/

	/* length of data to be transmitted */
	// data_length = 12U;

	/* The buffer to be written over an I2C Slave device */
	// transmit_byte[0] = 0x48U; // H
	// transmit_byte[1] = 0x45U; // E
	// transmit_byte[2] = 0x4CU; // L
	// transmit_byte[3] = 0x4CU; // L
	// transmit_byte[4] = 0x4FU; // O
	// transmit_byte[5] = 0x20U; // SPACE
	// transmit_byte[6] = 0x57U; // W
	// transmit_byte[7] = 0x4FU; // O
	// transmit_byte[8] = 0x52U; // R
	// transmit_byte[9] = 0x4CU; // L
	// transmit_byte[10] = 0x44U; // D
	// transmit_byte[11] = 0x21U; // !

	/*To write test string to the I2C slave*/
	i2c_ctrl->Write( byte_address, data_length, ( uint8_t* ) sample_string, &tx_status );

	/*To write the sample buffer filled byte by byte*/
	// i2c_ctrl->Write( byte_address, data_length, &transmit_byte[0], &tx_status );

	return ( tx_status );
}

/*
 * brief                    Sample I2C CR Task to demonstrate continuous read or write operation with I2C Slave
 * note                     The below function does the following
 *                          1. Initialized uC_I2C class shall be passed for testing Read and Write operation
 */
void I2C_Test_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	/** @brief Holds the status of I2C data transmisson/reception */
	volatile uC_I2C::i2c_status_t m_transfer_finish = uC_I2C::NO_ERROR_INIT;

	CR_Tasker_Begin( cr_task );
	for (;;)
	{
		CR_Tasker_Delay( cr_task, 1000 );
		m_transfer_finish = i2c_ctrl->I2C_Transfer_Status();
		if ( ( ( m_transfer_finish == uC_I2C::I2C_FINISH_RX ) ) || ( first_time_flag == true ) )
		{
			uC_Delay( 10000U );
			I2C_Write_Example( i2c_ctrl );
			first_time_flag = false;
		}
		else if ( ( m_transfer_finish == uC_I2C::I2C_FINISH_TX ) )
		{
			uC_Delay( 10000U );
			I2C_Read_Example( i2c_ctrl );
		}
	}
	CR_Tasker_End();
}