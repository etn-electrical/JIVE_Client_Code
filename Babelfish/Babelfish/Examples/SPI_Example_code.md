@example  uC_SPI
@brief    The sample code to initialize the SPI of STM32F207 controller and sending commands to
          M25P80 flash chip over an SPI
@par Board Specific Settings  

@details  Please do the following setting on EasyMXPro Board to run successfully the sample code
@n        1. STM32F207 Card is fitted on the board.
@n        2. Enable SW13.1, SW13.2, SW13.3 and SW13.8 switches. This connects SPI lines to MCU_MOSI, MCU_MISO, MCU_SCK and PD7 (CS) microcontroller pins.

@par Pin Configurations 
@details   Correctly configure the GPIO pins in uC_IO_Define_STM32F207.cpp as below 
@n        1. const uC_PERIPH_IO_STRUCT SPI3_MOSI_PIO_PC12 = { GPIOC, IO_PIN_12, uC_IO_ALTERNATE_MAP_6 }; //MOSI pin
@n        2. const uC_PERIPH_IO_STRUCT SPI3_MISO_PIO_PC11 = { GPIOC, IO_PIN_11, uC_IO_ALTERNATE_MAP_6 }; //MISO pin
@n        3. const uC_PERIPH_IO_STRUCT SPI3_CLOCK_PIO_PC10 = { GPIOC, IO_PIN_10, uC_IO_ALTERNATE_MAP_6 }; //SCLK pin
@n        4. const uC_USER_IO_STRUCT PIOD_PIN_7_STRUCT = { GPIOD, IO_PIN_7, uC_PIO_INDEX_D }; //Chip Select. General Purpose Output An example code for SPI software module.

@par Dependency
@details Not applicable

@par Sample Code Listing
 
~~~~{.cpp}

#include "uC_SPI.h"
#include "Ram.h"
#include "Ram_Static.h"

#define EXT_MEM_SPI_PORT 2u        //The SPI Channel number of Micro(Channel number starts from CHANNEL 0)
#define NUM_EXT_MEM_CS_CTRLS	1u // Number of SPI compatible slaves
#define FLASH_SPI_CS_IO_CTRL PIOD_PIN_7_STRUCT //Chip Select Pin
uC_USER_IO_STRUCT const* const EXT_MEM_CS_CTRLS[NUM_EXT_MEM_CS_CTRLS] =
{ &FLASH_SPI_CS_IO_CTRL }; // Chip Select Pin structure
const uC_BASE_SPI_IO_PORT_STRUCT EXT_MEM_SPI_PIO =
{ &SPI3_MOSI_PIO_PC12, &SPI3_MISO_PIO_PC11, &SPI3_CLOCK_PIO_PC10, &PERIPH_NULL_PIO };
//Structure which accepts SPI pins.
#define SPI_CLK_FREQ 500000u    //SPI frequency
// Instruction Set of M25P80 flash memory
#define OPCODE_WREN      0x06u // Write Enable
#define OPCODE_WRDI      0x04u // Write Disable
#define OPCODE_RDSR      0x05u // Read Status Register
#define OPCODE_WRSR      0x01u // Write Status Register
#define OPCODE_READ      0x03u // Read Data
#define OPCODE_FAST_READ 0x0Bu // Fast Read
#define OPCODE_PP        0x02u // Page Program
#define OPCODE_SE        0xd8u // block Erase
#define OPCODE_BE        0xc7u // Chip Erase
#define OPCODE_DP        0xb9u // Deep Power-down
#define OPCODE_RES       0xabu // Release from Deep Power-down
#define OPCODE_RES       0x9Fu // Read Identification
#define OPCODE_DEVICEID  0x90u // ManufacturerID
#define DUMMY            0x00u // Dummy Writes as required by some instructions
#define OPCODE_OTPMODE   0x3Au // Enter OTP mode

#define M25P80_ELECTRONIC_SIGNATURE 0x13

/*
 * brief                    Example Code for establishing SPI communication
 * note                     The below function does the following
 *                          1. Initialization of uC_SPI class
 *                          2. Establishing communication with Flash
 *                          3. Reading Identification information from flash memory
 */
void SPITransfer( void )
{
	//Receive Data Buffer
	uint8_t Rx_data[4];
	//Create an instance of uC_SPI class
	uC_SPI* spi_ctrl;
	//Enable peripheral clock of SPI APB1 Enable SCK, MOSI, MISO and NSS GPIO clocks AHB1
	spi_ctrl = new uC_SPI( EXT_MEM_SPI_PORT, TRUE, EXT_MEM_CS_CTRLS,
	NUM_EXT_MEM_CS_CTRLS, &EXT_MEM_SPI_PIO, TRUE );
	//Configure interfacing chip settings 1. SPI mode 2. frequency 3. No of Bytes
	spi_ctrl->Configure_Chip_Select( uC_SPI_MODE_0, 0, SPI_CLK_FREQ,uC_SPI_BIT_COUNT_8, true  );
	// Pull Low the chip select signal
	spi_ctrl->Select_Chip( 0 );
	// Send Read Identification Command to EON25F80
	spi_ctrl->TX_Byte( OPCODE_RES );
	//Wait while instruction byte is being sent
	while ( !spi_ctrl->TX_Data_Reg_Empty() );
	
	//Send Dummy Address 0x00 High
	spi_ctrl->TX_Byte( DUMMY );
	//Wait while byte is being sent
	while ( !spi_ctrl->TX_Data_Reg_Empty() );
	
	//Send Dummy Address 0x00 Middle
	spi_ctrl->TX_Byte( DUMMY );
	//Wait while byte is being sent
	while ( !spi_ctrl->TX_Data_Reg_Empty() );
	
	//Send Dummy Address 0x00 LOW
	spi_ctrl->TX_Byte( DUMMY );
	//Wait while byte is being sent
	while ( !spi_ctrl->TX_Data_Reg_Empty() );
	
	//Response Delay. delay betwenn Tx and Rx
	uC_Delay( 20 );
	// Recieve Manufacturer identification = 0x1C for EON
	Rx_data[0] = spi_ctrl->RX_Byte();
	//receive Memory type = 0x31 for this EON chip
	Rx_data[1] = spi_ctrl->RX_Byte();
	//Receive Memory Capacity = 0x14 for this EON chip
	Rx_data[2] = spi_ctrl->RX_Byte();
	//Receive UID = 0x1C for this EON chip
	Rx_data[3] = spi_ctrl->RX_Byte();
}

~~~~