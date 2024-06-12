
#include "Includes.h"
#include "uC_SPI.h"
#include "NV_Ctrl.h"
#include "AT25DF.h"

void serial_flash_init( void );

void spi_read_write_driver( void );

bool_t Verify_Flash_Chip( void );

void page_program_driver( void );

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
static NV_Ctrl* nv_ctrl_flash;
static NV_Ctrls::AT25DF* flash_ctrl_driver;
static bool_t flash_working_status = false;
#define OPCODE_WREN      0x06u //Write Enable

const NV_Ctrls::AT25DF_CHIP_CONFIG_TD AT25DF_CFG =
{
	3U,
	/* Address length*/
	4096U,
	/*erase_page_size*/
	256U,
	/*write_page_size*/
	0U,
	/* NV_DATA_START_NVCTRL0_NVADD,- Start Address */
	( 0x100000 / 2 ),
	/* NV_DATA_CTRL0_RANGE_SIZE,	- Mirror Start Address */
	0x0FFFFF,
	/* NV_DATA_CTRL0_RANGE_SIZE * 2 - End of Memory */
	25000000u
	/* Max clock freq */
};



/* The SPI Channel number of Micro(Channel number starts from CHANNEL 0) */
 #define EXT_MEM_SPI_PORT 2u
/* Number of SPI compatible slaves */
 #define NUM_EXT_MEM_CS_CTRLS   1u
/* Chip Select Pin */
 #define FLASH_SPI_CS_IO_CTRL PIOD_PIN_7_STRUCT
uC_USER_IO_STRUCT const* const EXT_MEM_CS_CTRLS[NUM_EXT_MEM_CS_CTRLS] =
{ &FLASH_SPI_CS_IO_CTRL };  //Chip Select Pin structure




const uint8_t Flash_Memory_Capacity_ID = 0x14;


void spi_read_write_driver( void )
{
	serial_flash_init();
}

uC_SPI* spi_ctrl_driver;

void serial_flash_init( void )
{

	//Enable peripheral clock of SPI APB1 Enable SCK, MOSI, MISO and NSS GPIO clocks AHB1
	spi_ctrl_driver = new uC_SPI( EXT_MEM_SPI_PORT, TRUE, EXT_MEM_CS_CTRLS,
								  NUM_EXT_MEM_CS_CTRLS, &EXT_MEM_SPI_PIO, TRUE );

	nv_ctrl_flash = new NV_Ctrls::AT25DF( spi_ctrl_driver, 0U, &AT25DF_CFG );

	flash_ctrl_driver = reinterpret_cast<NV_Ctrls::AT25DF*>( nv_ctrl_flash );

	flash_working_status = Verify_Flash_Chip();

	if ( flash_working_status == true )
	{

		page_program_driver();

	}
}

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

void page_program_driver( void )
{
	uint8_t status_test = 0U;
	uint8_t write_buffer1[200U], write_buffer2[267U], read_buffer1[200U], read_buffer2[267U],
			read_buffer_erase1[4097U], write_buffer_erase2[4097U];
	uint32_t i, k;

	for ( i = 0U; i < 200U; i++ )
	{
		write_buffer1[i] = i;
		read_buffer1[i] = 0U;
	}

	for ( i = 0U; i < 4097U; i++ )
	{
		write_buffer_erase2[i] = k;
		read_buffer_erase1[i] = 0U;
		k++;
		if ( k > 250U )
		{
			k = 1U;
		}
	}

	for ( i = 0U; i < 267U; i++ )
	{
		write_buffer2[i] = i;
		read_buffer2[i] = 0U;
	}

	flash_ctrl_driver->Erase( 0x000000, 4096U );

	flash_ctrl_driver->Read( read_buffer_erase1, 0x000000, 4096U, true );
	//Writing 256 length data into sector-0
	flash_ctrl_driver->Write( write_buffer1, 0x000000, 200U, true );

	//Reading 256 length data from sector-0

	flash_ctrl_driver->Read( read_buffer1, 0, 200U, true );


	//Writing 267 length data into sector-1
	flash_ctrl_driver->Write( write_buffer2, 0x000000, 267U, true );

	//Reading 267 length data from sector-1

	flash_ctrl_driver->Read( read_buffer2, 0x00000, 267U, true );

	//flash_ctrl_driver-> Erase (0, 4096);
	flash_ctrl_driver->Write( write_buffer2, 0x010000, 4097U, true );
	//Reading after Erase i.e checking Erased or not

	flash_ctrl_driver->Read( read_buffer_erase1, 0x010000, 4097U, true );  //block 1 and Sector-1

}
