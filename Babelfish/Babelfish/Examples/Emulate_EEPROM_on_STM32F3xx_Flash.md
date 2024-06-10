@example  EEPROM emulation example
@brief    The sample code to demostrate data recovery mechanism in STM32F3xx controller. 
          The emulation method uses two Flash memory pages as a scratch spage.

@par Development Board  
@details  STM32 Nucleo-64 board, microcontroller - STM32F303RE                                 	        

@par Usage
@details Add EEPROM_Emulation_Test() prototype in a suitable header file and add the below code in source file

@copyright       2020 Eaton Corporation. All Rights Reserved.
@note            Eaton Corporation claims proprietary rights to the material disclosed
                 here on.  This technical information may not be reproduced or used
                 without direct written permission from Eaton Corporation.

@par Sample Code Listing
 
~~~~{.cpp}
		
/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Flash.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
/* Flash size of STM32F303RE is 512KB ( 0x08000000 to 0x0807FFFF ).
  0x08000000 to 0x0803FFFF region of flash is used for Application firmware.
  To define a scratch memory (4K) region other than end of flash, change uC_FLASH_SCRATCH_PAGE_ADDRESS in (uC_Flash.cpp) 
  as shown in below example,
	  #define uC_FLASH_SCRATCH_PAGE_ADDRESS (static_cast<uint32_t>(0x08040000) )
*/

#define uC_FLASH_START_POINT        0x08040000                  // Starting point of Flash memory
#define uC_FLASH_START_END          0x0807FFFF                  // Starting point of Flash memory

const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD F3_FLASH_CONFIG =
{
   uC_FLASH_START_POINT,   // start address
   uC_FLASH_START_END,     // mirror start address
   uC_FLASH_START_END      // end of memory, 4KB memory (2 pages) at the end will be used as scratch space
};

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
static NV_Ctrls::uC_Flash* flash_handle;


/*
 *****************************************************************************************
 *		Function definition
 *****************************************************************************************
 */

bool EEPROM_Emulation_Test( void ) 
{	
    uint32_t data_w = 0x12345678;///< data to be written
    uint32_t data_w_len = 4U;
    const uint8_t data_r_len = 50U;
    uint8_t data_r[data_r_len] = {0U};
    uint8_t i = 0U;
    bool status = false;
    NV_Ctrl::nv_status_t return_status ;
    
    flash_handle = new NV_Ctrls::uC_Flash( &F3_FLASH_CONFIG, FLASH );
    
    uint32_t write_addr = 0x08040000;///< Do not use scratch memory address
    uint32_t read_addr = 0x08040000;
    
    /* When Use_Protection value is true, mirror copy of the page (to be written) is created 
       which is considered as a good copy and can be used in case of data corruption, 
       checksum calculation is done in that case.
       Use_Protection = true, is not covered in this example code.
    */
    bool use_protection_val = false;

    /************************************************************************************
    Check whether 1st value is retained after writing 2nd value
    *************************************************************************************/
    
    if ( flash_handle->Read( data_r, read_addr, data_r_len, use_protection_val ) 
                                                           == NV_Ctrl::SUCCESS ) ///< Read the memory before write call
    {
        if ( flash_handle->Write( (uint8_t*)&data_w, write_addr, data_w_len, use_protection_val ) == NV_Ctrl::SUCCESS )
        {
            if ( flash_handle->Read( data_r, read_addr, data_r_len, use_protection_val ) == NV_Ctrl::SUCCESS )
            {
                /* Verify the data in data_r to confirm that data is written successfully */
                status = true;
            }
        }
    }
    
    if ( status == true )
    {
        status = false;
        data_w =  0x11111111;
        write_addr = write_addr + 10U;
        
        if ( flash_handle->Write( (uint8_t*)&data_w, write_addr, data_w_len, use_protection_val ) == NV_Ctrl::SUCCESS )
        {
            if ( flash_handle->Read( data_r, read_addr, data_r_len, use_protection_val ) == NV_Ctrl::SUCCESS )
            {
                /* Analyze data_r to verify that previous data (0x12345678) is present at 0x08040000, hence old data is retained.
                   Softreset/hardreset the device and read 0x08040000 to check whether data is retained after reset.
                */
                status = true;
            }
        }
    }
    
    /************************************************************************************
    Check whether old data is retained when memory is not clear i.e FF
    *************************************************************************************/
    write_addr = 0x08040000;
    read_addr = 0x08040000;
    
    /* Fill the memory with some data  */
    for(i = 0U; i < data_r_len; i++)
    {
        return_status = flash_handle->Write( (uint8_t*)&i, write_addr + i, 1U, use_protection_val );
    }
    
    if ( return_status == NV_Ctrl::SUCCESS )
    {
        status = false;
        if ( flash_handle->Read( data_r, read_addr, data_r_len, use_protection_val ) == NV_Ctrl::SUCCESS ) 
        {
            /* write 4 byte data */
            data_w =  0x22222222;
            if ( flash_handle->Write( (uint8_t*)&data_w, write_addr, data_w_len, use_protection_val ) 
                                                                                == NV_Ctrl::SUCCESS )
            {
                if ( flash_handle->Read( data_r, read_addr, data_r_len, use_protection_val ) == NV_Ctrl::SUCCESS )
                {
                    /* Verify the data in data_r to confirm that 4 byte data is written successfully, 
                        also old data is retained */
                    status = true;
                }
            }
        }
        
        if ( status == true )
        {
            status = false;
            data_w =  0x33333333;
            write_addr = write_addr + 15U;
            
            if ( flash_handle->Write( (uint8_t*)&data_w, write_addr, data_w_len, use_protection_val ) 
                                                                                == NV_Ctrl::SUCCESS )
            {
                if ( flash_handle->Read( data_r, read_addr, data_r_len, use_protection_val ) == NV_Ctrl::SUCCESS )
                {
                    /* Analyze data_r to verify that old data is retained accurately */
                    status = true;
                }
            }
        }
    }
    
    return ((bool)return_status);
}