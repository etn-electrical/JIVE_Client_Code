/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
@example SPI_Example_Flash2Click_code
@brief The sample code to initialize the SPI of STM32F427 controller and sending commands to
SST26VF flash chip over an SPI to verify the Erase, read and write operation.
@par Board Specific Settings
Connect the Flash2 - Click board with STM32F427 Nuclo board as per the gpio
configuration given in the APP_IO_Config_STM32F427.h file

@Flash - 2 Click board link: -https:// www.mikroe.com/flash-2-click

For testing purpose only on the Arduino UNO click board connect the Serial flash at slot - 1
define the below lines in the APP_IO_Config_STM32F427.h file.

#define SERIAL_FLASH_SPI_CS_IO_CTRL             PIOD_PIN_14_STRUCT
#define SERIAL_FLASH_SPI_PORT                   uC_BASE_SPI_CTRL_1
#define SERIAL_FLASH_SPI_CS_NUM                 0

const uC_BASE_SPI_IO_PORT_STRUCT SERIAL_FLASH_SPI_PIO =
{ &SPI1_MOSI_PIO_PB5, &SPI1_MISO_PIO_PA6, &SPI1_CLOCK_PIO_PA5, &PERIPH_NULL_PIO };


#include "Includes.h"
#include "uC_SPI.h"
#include "NV_Ctrl.h"
#include "SST26VF_Block.h"

void Serial_Flash_Init( void );

bool_t Verify_Flash_Chip( void );

void Page_Program_Driver( void );

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
static NV_Ctrl* nv_ctrl_flash;
static NV_Ctrls::SST26VF_Block* flash_ctrl_driver;
static bool_t flash_working_status = false;
uC_SPI* serial_flash_ctrl;

const uint8_t Flash_Memory_Capacity_ID = 0x43;

const NV_Ctrls::SST26VF_BLOCK_CHIP_CONFIG_TD SST26VF_CFG =
{
	3U,
	/* Address length*/
	4096U,
	/*erase_page_size*/
	256U,
	/*write_page_size*/
	0U,
	/* NV_DATA_START_NVCTRL0_NVADD,- Start Address */
	0x3FFFFFU,
	/* NV_DATA_CTRL0_RANGE_SIZE,	- Mirror Start Address */
	0x7FFFFFU,
	/* NV_DATA_CTRL0_RANGE_SIZE * 2 - End of Memory */
	70000000u
	/* Max clock freq */
};


/*
 * brief                    Example Code for for initializing the external serial flash chip
 * note                     The below function does the following
 *                          1. Initialization of uC_SPI class
 *                          2. Establishing communication with Flash
 *                          3. Reading Identification information from flash memory
 */

void Serial_Flash_Init( void )
{

	static uC_USER_IO_STRUCT const* const SERIAL_FLASH_CS_CTRLS[1U] = { &SERIAL_FLASH_SPI_CS_IO_CTRL };

	// Enable peripheral clock of SPI APB1 Enable SCK, MOSI, MISO and NSS GPIO clocks AHB1
	serial_flash_ctrl = new uC_SPI( SERIAL_FLASH_SPI_PORT, TRUE, & SERIAL_FLASH_CS_CTRLS[0],
									1U, & SERIAL_FLASH_SPI_PIO, TRUE );

	nv_ctrl_flash = new NV_Ctrls::SST26VF_Block( serial_flash_ctrl, SERIAL_FLASH_SPI_CS_NUM,
												 reinterpret_cast < NV_Ctrls::SST26VF_BLOCK_CHIP_CONFIG_TD const* > ( &SST26VF_CFG ) );

	flash_ctrl_driver = reinterpret_cast < NV_Ctrls::SST26VF_Block * > ( nv_ctrl_flash );

	flash_working_status = Verify_Flash_Chip();

	if ( flash_working_status == TRUE )
	{

		page_program_driver();

	}
}

/*
 * brief                    Function to varify the flash chip
 * note                     The below function does the following
 *                          1. Read the device ID into the rx_data buffer
 *                          2. Compare the received buffer with constant device ID
 *                          3. Return true if device ID matches else return false.
 */

bool_t Verify_Flash_Chip( void )
{
	uint8_t Rx_data[4] = {0, 0, 0, 0};
	bool_t flash_working = false;

	flash_ctrl_driver->Read_Device_ID( Rx_data );

	if ( Rx_data[2] == Flash_Memory_Capacity_ID )
	{
		flash_working = true;
	}

	return ( flash_working );
}

/*
 * brief                    Example Code for testing the Flash memory
 * note                     The below function does the following
 *                          1. Initialize the buffer with some default data
 *                          2. Erase entire chip
 *                          3. Read and write data into the flash
 */
void Page_Program_Driver( void )
{
	NV_Ctrl::nv_status_t m_status = NV_Ctrl::SUCCESS;
	bool return_status = false;

	uint8_t write_buffer1[200U], write_buffer2[267U], read_buffer1[200U], read_buffer2[267U],
			read_buffer_erase1[4097U], write_buffer_erase1[4097U];
	uint32_t i = 0;

	for ( i = 0U; i < 200U; i++ )
	{
		write_buffer1[i] = 0x0A;
		read_buffer1[i] = 0U;
	}

	for ( i = 0U; i < 267U; i++ )
	{
		write_buffer2[i] = 0x0B;
		read_buffer2[i] = 0U;
	}

	for ( i = 0U; i < 4097U; i++ )
	{
		write_buffer_erase1[i] = 0x0C;
		read_buffer_erase1[i] = 0U;
	}

	// Erase Full chip
	m_status = flash_ctrl_driver->Erase_All();
	BF_ASSERT( m_status == NV_Ctrl::SUCCESS );
	// Check Weather Data read after Erase is 0xFF or not
	m_status = flash_ctrl_driver->Read( read_buffer_erase1, 0, 4096U, true );
	BF_ASSERT( m_status == NV_Ctrl::SUCCESS );

	// write 200 bytes of data
	m_status = flash_ctrl_driver->Write( write_buffer1, 0, 200U, true );
	m_status = flash_ctrl_driver->Read( read_buffer1, 0, 200U, true );
	return_status =
		BF_Lib::String_Compare( reinterpret_cast < uint8_t const* > ( &write_buffer1 ),
								reinterpret_cast < uint8_t const* > ( read_buffer1 ), 200 );
	BF_ASSERT( return_status == TRUE );


	// Write 267 bytes of data without erase to check false condition
	m_status = flash_ctrl_driver->Write( write_buffer2, 0, 267U, true );
	m_status = flash_ctrl_driver->Read( read_buffer2, 0, 267U, true );
	return_status =
		BF_Lib::String_Compare( reinterpret_cast < uint8_t const* > ( &write_buffer2 ),
								reinterpret_cast < uint8_t const* > ( read_buffer2 ), 267 );
	BF_ASSERT( return_status == FALSE );

	// Erase 4K bytes of memory
	m_status = flash_ctrl_driver->Erase( 0, 4096U );
	BF_ASSERT( m_status == NV_Ctrl::SUCCESS );

	// Write 4K bytes of data at address 0x00
	m_status = flash_ctrl_driver->Write( write_buffer_erase1, 0, 4096U, true );
	m_status = flash_ctrl_driver->Read( read_buffer_erase1, 0, 4096U, true );
	return_status =
		BF_Lib::String_Compare( reinterpret_cast < uint8_t const* > ( &write_buffer_erase1 ),
								reinterpret_cast < uint8_t const* > ( read_buffer_erase1 ), 4096 );
	BF_ASSERT( return_status == TRUE );

	// Erase 4K bytes of memory from address 0x010000
	m_status = flash_ctrl_driver->Erase( 0x010000, 4096U );
	BF_ASSERT( m_status == NV_Ctrl::SUCCESS );

	// Write 4K bytes of data at address 0x010000
	m_status = flash_ctrl_driver->Write( write_buffer_erase1, 0x010000, 4096U, true );
	m_status = flash_ctrl_driver->Read( read_buffer_erase1, 0x010000, 4096U, true );
	return_status =
		BF_Lib::String_Compare( reinterpret_cast < uint8_t const* > ( &write_buffer_erase1 ),
								reinterpret_cast < uint8_t const* > ( read_buffer_erase1 ), 4096 );
	BF_ASSERT( return_status == TRUE );

	// Erase 8K bytes of memory
	m_status = flash_ctrl_driver->Erase( 0x010000, 8192U );
	BF_ASSERT( m_status == NV_Ctrl::SUCCESS );

	// write 4097 Bytes data at address 0x010000
	m_status = flash_ctrl_driver->Write( write_buffer_erase1, 0x010000, 4097U, true );
	m_status = flash_ctrl_driver->Read( read_buffer_erase1, 0x010000, 4097U, true );
	return_status =
		BF_Lib::String_Compare( reinterpret_cast < uint8_t const* > ( &write_buffer_erase1 ),
								reinterpret_cast < uint8_t const* > ( read_buffer_erase1 ), 4097 );
	BF_ASSERT( return_status == TRUE );

}
