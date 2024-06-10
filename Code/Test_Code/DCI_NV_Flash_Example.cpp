/**
 *****************************************************************************************
 *	@file		DCI_NV_Flash_Example.cpp
 *	@brief		See header file for module overview.
 *****************************************************************************************
 */

#include <stdio.h>
#include "Includes.h"
#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Etn_Types.h"
#include "uC_Flash.h"
#include "DCI_NV_Flash_Example.h"

#ifdef __cplusplus
extern "C"
{
#endif

namespace NV_Ctrls
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Check_In_Init_Check_Out_Init_Data_Variables_Test( void )
{
	uint32_t val = 0x01;		///< Data value
	uint32_t val_read = 0;
	DCI_Owner* uint8_data_test_owner;
	uint32_t sector = 273;
	uC_Flash* Temp_uC_Flash = new uC_Flash( nullptr, nullptr, true, NULL, NULL, false );

	/// DCI_Owner for uint_8 datatype
	uint8_data_test_owner = new DCI_Owner( DCI_BLE_USER1_ROLE_DCID );

	while ( val < 0xFF )
	{
		if ( Temp_uC_Flash->Erase_Sector( sector ) != uC_FLASH_COMPLETE )
		{
			printf( "Erase failed\r\n" );
		}

		uint8_data_test_owner->Check_In_Init( ( uint8_t* ) &val );
		uint8_data_test_owner->Check_Out_Init( ( uint8_t* ) &val_read );

		if ( val_read != val )
		{
			printf( "Read invalid value=%08x of sector=%d\r\n", val_read, sector );
			break;
		}
		else
		{
			val++;
			printf( "Read value=%08x of SECTOR =%d\r\n", val_read, sector );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_NV_Flash_Test_App_Main()
{
	DCI_Check_In_Init_Check_Out_Init_Data_Variables_Test();
}

}
#ifdef __cplusplus
}
#endif
