/**
 *****************************************************************************************
 *	@file uC_Code_Range.cpp
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Code_Range.h"
#include "Stdlib_MV.h"
#include "Esp32_Partition.h"
#include "HAL_Debug.h"


static const esp_partition_type_t NV_CONFIG_PARTITION_TYPE = ( esp_partition_type_t )0x40;
static const esp_partition_subtype_t NV_CONFIG_PARTITION_SUBTYPE = ( esp_partition_subtype_t )0xFC;

/*Constant values to select web image partition*/
static const esp_partition_type_t WEB_IMAGE_PARTITION_TYPE = ESP_PARTITION_TYPE_DATA;
// 0xFB is a partition subtype created for web_image. Please refer partitions.csv for more details
static const esp_partition_subtype_t WEB_IMAGE_PARTITION_SUBTYPE = ( esp_partition_subtype_t )0xFB;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_App_Fw_Start_Address( void )
{
	esp_partition_t current_partition = {  };
	bool ret = ESP32_Partition::Get_Running_Partition( &current_partition );

	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get current ota partition address \n" );
	}
	return ( reinterpret_cast<uint8_t*>( current_partition.address ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_DCI_NV_Start_Address( void )
{
	esp_partition_t nv_config_partition = { };
	bool ret = ESP32_Partition::Get_Partition_Details( &nv_config_partition, NV_CONFIG_PARTITION_TYPE,
													   NV_CONFIG_PARTITION_SUBTYPE, "nv_config" );

	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get current nv config partition address \n" );
	}

	return ( reinterpret_cast<uint8_t*>( nv_config_partition.address ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_App_Fw_End_Address( void )
{
	esp_partition_t current_partition = { };
	bool ret = ESP32_Partition::Get_Running_Partition( &current_partition );

	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get current ota partition address \n" );
	}
	return ( reinterpret_cast<uint8_t*>( current_partition.address + ( current_partition.size - 1U ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_DCI_NV_End_Address( void )
{
	esp_partition_t nv_config_partition = { };
	bool ret = ESP32_Partition::Get_Partition_Details( &nv_config_partition, NV_CONFIG_PARTITION_TYPE,
													   NV_CONFIG_PARTITION_SUBTYPE, "nv_config" );

	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get current nv config partition address \n" );
	}

	return ( reinterpret_cast<uint8_t*>( nv_config_partition.address + ( nv_config_partition.size - 1U ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Web_Fw_Start_Address( void )
{
	esp_partition_t web_image_partition = { };

	bool ret = ESP32_Partition::Get_Partition_Details( &web_image_partition, WEB_IMAGE_PARTITION_TYPE,
													   WEB_IMAGE_PARTITION_SUBTYPE, "web_image" );

	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get web image partition start address \n" );
	}

	return ( reinterpret_cast<uint8_t*>( web_image_partition.address ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Web_Fw_End_Address( void )
{
	esp_partition_t web_image_partition = { };

	bool ret = ESP32_Partition::Get_Partition_Details( &web_image_partition, WEB_IMAGE_PARTITION_TYPE,
													   WEB_IMAGE_PARTITION_SUBTYPE, "web_image" );

	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get web image partitions end address \n" );
	}

	return ( reinterpret_cast<uint8_t*>( web_image_partition.address + ( web_image_partition.size - 1U ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_Create_Partition( size_t offset, size_t size, const char* label )
{
	/*
	 * Disadvantages:
	 * 1. offset ( starting address ) which we are using here, we don't have memory check for overlapping.
	 *    So, we need to take extra care here.
	 * 2. Once partition is created, it is not seen on terminal like other partitions in partition table.
	 *
	 */

	esp_partition_type_t type;
	esp_partition_subtype_t subtype;
	esp_err_t err = ESP_OK;

	/*
	 * Add label check as per need in else condition
	 * similar to web_image
	 */
	if ( strncmp( label, "web_image", strlen( label ) ) == 0 )
	{
		type = WEB_IMAGE_PARTITION_TYPE;
		subtype = WEB_IMAGE_PARTITION_SUBTYPE;
	}
	else
	{
		err = ESP_ERR_NOT_SUPPORTED;
	}

	/*
	 * Check if we have found correct partition label
	 */
	if ( ESP_ERR_NOT_SUPPORTED != err )
	{
		err = ESP32_Partition::Create_Partition( offset, size, label, type, subtype );
	}

	if ( ESP_OK == err )
	{
		HAL_DEBUG_PRINT( DBG_MSG_INFO, "Partition is created successfully \n" );
	}
	else if ( ESP_ERR_INVALID_ARG == err )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Partition with given label is already available \n" );
	}
	else
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to create partition \n" );
	}

	return ( err );
}
