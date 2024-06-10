/**
 *****************************************************************************************
 * @file	Test_Esp32_Partition.cpp
 * @details This test code is responsible for providing partition information when called.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Test_Esp32_Partition.h"

/*
 *****************************************************************************************
 *	varaibles/pointers
 *****************************************************************************************
 */

void Test_Esp32_Partition( void )
{
	static esp_partition_t next_partition = { };
	static esp_partition_t current_partition = { };

	ESP32_Partition::Get_Running_Partition( &current_partition );
	FUS_DEBUG_PRINT( DBG_MSG_DEBUG, "Running  address [ starting: 0x%x ]  & size = %d \n",
					 ( current_partition.address ), current_partition.size );

	ESP32_Partition::Get_Next_Update_Partition( &next_partition, nullptr );
	FUS_DEBUG_PRINT( DBG_MSG_DEBUG, "Next_Update_Partition  address [ starting: 0x%x ]  & size = %d \n",
					 ( next_partition.address ), next_partition.size );

	bool ret = ESP32_Partition::Ota_Set_Boot_Partition( &next_partition );

	if ( ret != true )
	{
		FUS_DEBUG_PRINT( DBG_MSG_DEBUG, "OTA data partition update is unsuccessful.\r\n" );
	}

	const esp_partition_t* get_partition = esp_ota_get_boot_partition();

	FUS_DEBUG_PRINT( DBG_MSG_DEBUG, "Current OTA data partition [ starting: 0x%x ]  & size = %d \n",
					 ( get_partition->address ), get_partition->size );

}
