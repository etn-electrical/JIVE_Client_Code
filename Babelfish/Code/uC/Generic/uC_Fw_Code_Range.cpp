/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Fw_Code_Range.h"
#include "Stdlib_MV.h"
#include "HAL_Debug.h"

#ifdef ESP32_SETUP
#include "Esp32_Partition.h"


/*Constant values to select Cert store partition*/
static const esp_partition_type_t CERT_STORE_PARTITION_TYPE = ( esp_partition_type_t )0x01;
static const esp_partition_subtype_t CERT_STORE_PARTITION_SUBTYPE = ( esp_partition_subtype_t )0xFD;
#endif
/*
 *****************************************************************************************
 *		Declarations
 *****************************************************************************************
 */
#ifndef ESP32_SETUP
extern "C"
{
extern uint8_t __UBERLOADER_START__;
extern uint8_t __UBERLOADER_END__;
extern uint8_t __WEB_FW_START__;
extern uint8_t __WEB_FW_END__;
extern uint8_t __SCRATCH_AREA_START__;
extern uint8_t __SCRATCH_AREA_END__;
extern uint8_t __UBERLOADER_INFO__;
extern uint8_t __DEVICE_CERT_CERT_START__;
extern uint8_t __DEVICE_CERT_CERT_END__;
extern uint8_t __LANGUAGE_IMAGE_START__;
extern uint8_t __LANGUAGE_IMAGE_END__;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Uberloader_Start_Address( void )
{
	return ( ( uint8_t* ) ( &( __UBERLOADER_START__ ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Uberloader_End_Address( void )
{
	return ( ( uint8_t* ) ( &( __UBERLOADER_END__ ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Web_Fw_Start_Address( void )
{
	return ( ( uint8_t* )&( __WEB_FW_START__ ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Web_Fw_End_Address( void )
{
	return ( ( uint8_t* )&( __WEB_FW_END__ ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Language_Image_Start_Address( void )
{
	return ( ( uint8_t* )&( __LANGUAGE_IMAGE_START__ ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Language_Image_End_Address( void )
{
	return ( ( uint8_t* )&( __LANGUAGE_IMAGE_END__ ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_UberloaderInfo_Start_Address( void )
{
	return ( ( uint8_t* ) ( &( __UBERLOADER_INFO__ ) ) );
}

#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Scratch_Area_Start_Address( void )
{
#ifndef ESP32_SETUP
	return ( ( uint8_t* ) ( &( __SCRATCH_AREA_START__ ) ) );

#else
	esp_partition_t next_partition = { };
	bool ret = ESP32_Partition::Get_Next_Update_Partition( &next_partition, nullptr );
	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get next ota partition address \n" );
	}
	return ( reinterpret_cast<uint8_t*>( next_partition.address ) );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Scratch_Area_End_Address( void )
{
#ifndef ESP32_SETUP
	return ( ( uint8_t* ) ( &( __SCRATCH_AREA_END__ ) ) );

#else
	esp_partition_t next_partition = { };
	bool ret = ESP32_Partition::Get_Next_Update_Partition( &next_partition, nullptr );
	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get next ota partition address \n" );
	}
	return ( reinterpret_cast<uint8_t*>( next_partition.address + ( next_partition.size - 1U ) ) );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Device_Cert_Start_Address( void )
{
#ifndef ESP32_SETUP
	return ( ( uint8_t* ) ( &( __DEVICE_CERT_CERT_START__ ) ) );

#else
	esp_partition_t cert_store_partition = { };
	bool ret = ESP32_Partition::Get_Partition_Details( &cert_store_partition, CERT_STORE_PARTITION_TYPE,
													   CERT_STORE_PARTITION_SUBTYPE, "cert_store" );

	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get cert store partition address \n" );
	}

	return ( reinterpret_cast<uint8_t*>( cert_store_partition.address ) );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_Device_Cert_End_Address( void )
{
#ifndef ESP32_SETUP
	return ( ( uint8_t* ) ( &( __DEVICE_CERT_CERT_END__ ) ) );

#else
	esp_partition_t cert_store_partition = { };
	bool ret = ESP32_Partition::Get_Partition_Details( &cert_store_partition, CERT_STORE_PARTITION_TYPE,
													   CERT_STORE_PARTITION_SUBTYPE, "cert_store" );

	if ( ret != true )
	{
		HAL_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get cert store partition address \n" );
	}

	return ( reinterpret_cast<uint8_t*>( cert_store_partition.address + ( cert_store_partition.size - 1U ) ) );

#endif
}
