	
	/**Example code for Flash **/
	
	/**
	*
	* 	add these header files 
	*	#include "uC_Flash.h"
	*	#include "uC_Fw_Code_Range.h"
	*	#include "uC_Code_Range.h"
	*
	*	add the internal_flash_test() in a suitable header file
	*	and add the below code in source file
	*
	*	make sure to use the code (in a CR tasker) after OS initialization is done
	*	as there is a flash integartiy check through CRC and
	*	executing this example code before it may cause
	*	exception in CRC mismatch and eventually a watchdoh reset
	*	
	**/
		
	
//defines macors consts  variables

const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD APP_IMAGE_CONFIG =
{
reinterpret_cast<uint32_t>( uC_App_Fw_Start_Address() ),
reinterpret_cast<uint32_t>( uC_App_Fw_End_Address() ),
reinterpret_cast<uint32_t>( uC_App_Fw_End_Address() ),
};

const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD SCRATCH_IMAGE_CONFIG =
{
	reinterpret_cast<uint32_t>( uC_Scratch_Area_Start_Address() ),
	reinterpret_cast<uint32_t>( uC_Scratch_Area_End_Address() ),
	reinterpret_cast<uint32_t>( uC_Scratch_Area_End_Address() ),
};
	
static NV_Ctrls::uC_Flash* flash_ex_handle;



bool internal_flash_test() 
{	uint32_t data_32 =  ((uint32_t)0x12345678);
	NV_Ctrl::nv_status_t return_status ;
	
	uint32_t flash_addr = 0x0805FFFA;
	uint32_t len = 4;
	bool status = false;
	//create uC_Flash object
	flash_ex_handle = new NV_Ctrls::uC_Flash( &SCRATCH_IMAGE_CONFIG, FLASH );
	
	//call write
	return_status = flash_ex_handle->Write( (uint8_t*)&data_32, flash_addr, len, status );
	
	return ((bool)return_status);
}


	/**Example code end for Flash **/