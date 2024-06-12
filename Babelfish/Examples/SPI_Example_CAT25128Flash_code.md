
#include "Includes.h"
#include "uC_SPI.h"
#include "NV_Ctrl.h"
#include "CAT25128.h"

void serial_flash_init( void );

void self_test( void );

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
static NV_Ctrl* nv_ctrl_flash;
static NV_Ctrls::CAT25128* flash_ctrl_driver;
static bool_t flash_working_status = false;

const NV_Ctrls::EEPROM_CHIP_CONFIG_TD CAT25128_CFG =
{
	2,				//Address length
	64,             //write page size
	0,				//Start Address
	0x2000,			//Mirror Start Address
	0x2000* 2,		//End of Memory
	10000000		//Max clock freq
};



/* The SPI Channel number of Micro(Channel number starts from CHANNEL 0) */
 #define EXT_MEM_SPI_PORT 2u
/* Number of SPI compatible slaves */
 #define NUM_EXT_MEM_CS_CTRLS   1u
/* Chip Select Pin */
 #define FLASH_SPI_CS_IO_CTRL PIOB_PIN_12_STRUCT
uC_USER_IO_STRUCT const* const EXT_MEM_CS_CTRLS[NUM_EXT_MEM_CS_CTRLS] =
{ &FLASH_SPI_CS_IO_CTRL };  //Chip Select Pin structure


uC_SPI* spi_ctrl_driver;

void serial_flash_init( void )
{

	//Enable peripheral clock of SPI APB1 Enable SCK, MOSI, MISO and NSS GPIO clocks AHB1
	spi_ctrl_driver = new uC_SPI( EXT_MEM_SPI_PORT, TRUE, EXT_MEM_CS_CTRLS,
								  NUM_EXT_MEM_CS_CTRLS, &EXT_MEM_SPI_PIO, FALSE );

	nv_ctrl_flash = new NV_Ctrls::CAT25128( &CAT25128_CFG, 253U, spi_ctrl, 0U );

	flash_ctrl_driver = reinterpret_cast<NV_Ctrls::CAT25128*>( nv_ctrl_flash );

	self_test();
}

bool_t self_test( void )
{
	uint32_t addr = 0x00000000;
	static uint8_t data[] = { 0xAA, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xDF, 0xDE, 0xDD, 0xDC };
	uint8_t read_data[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uint8_t len = sizeof ( data );
	bool_t status = true;
	uint8_t i = 0U;

	// Perform write operation under unlocking and locking guard
	if( CAT25128::Get_Write_Lock() )
	{
		CAT25128::Set_Write_Lock( false );
	}
	while ( ( i < len ) && ( status != false ) )
	{
        status = Writting( &data[i], ( addr + i ), 1U );
		// Wait till write complete
		while ( !Test_Bit( status, CAT25128_STATUS_RDY_BIT ) );
		uC_Delay( 5000U );
		i++;
	}
	if( !CAT25128::Get_Write_Lock() )
	{
		CAT25128::Set_Write_Lock( true );
	}

	i = 0U;

	// Readback the flash data
	while ( ( i < len ) && ( status != false ) )
	{
        status = Read_Now( &read_data[i], ( addr + i ), 1U, false );
		// Wait till flash is ready to read again
		uC_Delay( 10U );

		// Check for read back data with original array sent over SPI
		if ( data[i] != read_data[i] )
		{
			status = false;
			break;
		}
		i++;
	}

	return ( status );
}

