\example  DCI_Exmaple_Flash
\brief    The sample code which tells  how to use the uC_Flash for DCI implementation in STM32F0xx.

\STM32F030 Discovery Board Settings  
		   STM32F030R8T6 MCU

 \par Configurations 
        Enable DCI feature for STM32F030 project.All DCI parameters should be created from RTK DCI DB Creator. 
      

~~~~{.cpp}

#include "NV_Mem.h"
#include "NV_Ctrl.h"
#include "DCI_Owner.h"
#include "uC_Flash.h"
#include "DCI_NV_Mem.h"
#include "NV_Address.h"

// total available flash in STM32F030R8T6 is 64KB ( 0x08000000 to 0x0800FFFF ).
// We will use from 0x0800F400 to 0x0800FFFF region of flash as EEPROM and rest is used for Application firmware.

#define uC_FLASH_START_POINT        0x0800F400                  // Starting point of Flash memory
#define uC_FLASH_PAGE_SIZE          0x400U                      // 1KB is the sector/block size of the flash memory which is the minimum size to erase
#define uC_FLASH_LENGTH             ( uC_FLASH_PAGE_SIZE * 3 )  // Indicating 2 for mirror and 1 for the scratch

/* This is a key which is used to reset all the DCI parameters.  This can be used
   to refresh the NV when the DCI database gets shuffled.  This is mostly used during
   the development phase for when the NV memory is re-arranged.
 */
const uint8_t DCI_NV_FLASH_FACTORY_RESET_KEY = 0x5BU;
const uint8_t DCI_NV_RAM_FACTORY_RESET_KEY = 0x0U;

const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD F0_FLASH_CFG =
{
   uC_FLASH_PAGE_SIZE,                                // page size
   uC_FLASH_START_POINT,                              // start address
   ( uC_FLASH_START_POINT + uC_FLASH_PAGE_SIZE ),     // mirror start address
   ( uC_FLASH_START_POINT + uC_FLASH_LENGTH )         // end of memory
};

static const uint8_t NV_CTRL_MAX_CTRLS = 1U;
static NV_Ctrl* nv_ctrl_list[NV_CTRL_MAX_CTRLS];
uint8_t nv_flush_keys[NV_CTRL_MAX_CTRLS] = { 0 };
static const uint8_t NV_FLASH_CTRL = 0;

DCI_Owner* dci_test1 = nullptr;
DCI_Owner* dci_test2 = nullptr;
bool_t nv_working_status = false;

/**
 **********************************************************************************************
 * @brief                     Example code for Nonvolatile memory Initialization
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_Target_NV_Init( void )
{
	nv_ctrl_list[NV_FLASH_CTRL] = new NV_Ctrls::uC_Flash( &F0_FLASH_CFG, FLASH );
	NV::NV_Mem::Init( nv_ctrl_list, NV_CTRL_MAX_CTRLS, nv_flush_keys );
	nv_working_status = true;
}

/**
 **********************************************************************************************
 * @brief                     Example code of DCI usage
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void DCI_Flash_Example_Code( void )
{
	uint8_t eeprom_Val1,eeprom_Val2;
	dci_test1 = new DCI_Owner( DCI_TEST_ONE );
	dci_test2 = new DCI_Owner( DCI_TEST_TWO );

	eeprom_Val1 = 0x05;
	dci_test1->Check_In_Init( &eeprom_Val1 );
	dci_test1->Check_Out( &eeprom_Val2 );
	if( eeprom_Val1 == eeprom_Val2 )
	{
		//successfully written to flash
	}
	
	eeprom_Val1 = 0xA5;
	dci_test2->Check_In_Init( &eeprom_Val1 );
	dci_test2->Check_Out( &eeprom_Val2 );
	if( eeprom_Val1 == eeprom_Val2 )
	{
		//successfully written to flash
	}	

}

/**
 **********************************************************************************************
 * @brief                     Example code for factory reset of DCID 
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_Factory_Reset_Or_No_FRAM( void )
{
	if ( nv_working_status == false )
	{
		DCI_NV_Mem::Init( DCI_NV_RAM_FACTORY_RESET_KEY );
		for ( UINT16 DCIDsList = 0; DCIDsList < DCI_TOTAL_DCIDS; DCIDsList++ )
		{
			/* SRP: If EasyMxPRO doesn't have FRAM, we will load all the default values to init
			   values everytime */
			DCI::Factory_Reset_DCID( DCIDsList );
		}
	}
	else
	{
		DCI_NV_Mem::Init( DCI_NV_FLASH_FACTORY_RESET_KEY );
	}
}

/**
 **********************************************************************************************
 * @brief                     Example code for DCI initialization and NV initialization 
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_Main_Init( void )
{
	//Initialize the DCI.
	DCI::Init();
	
	PROD_SPEC_Target_NV_Init();
	
	PROD_SPEC_Factory_Reset_Or_No_FRAM();
	
	DCI_Flash_Example_Code();

}

~~~~
