/**
 *****************************************************************************************
 *	@file		Ble_Api.cpp
 *
 *	@brief 		This files contains the details of BLE Event Handler functions
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Ble_Api.h"
#include "BLE_Debug.h"

/***************************************************************************************************
   Global Variables
 **************************************************************************************************/
#ifdef ESP32_SETUP
uint8_t ble_state = BLE_IDLE_OR_DISCONNECTED;
#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void Ble_Api::Ble_Event_Handler1( int32_t evt_id, int32_t gatts_if, void* param )	/// gatts
{
	int32_t map_index = 0;


	BLE_DEBUG_PRINT( DBG_MSG_INFO, "gatts event: %d \n", evt_id );
	for ( map_index = 0; map_index < GATTS_MAX_EVENT; map_index++ )
	{
		if ( evt_id == m_event_mapping_gatts[map_index] )
		{
			break;
		}
	}
	/* Handle events */
	switch ( map_index )
	{
		case GATTS_ATTRIBUTE_TABLE_CREATED:
			m_ble_api_obj->Gatts_Start_Service( gatts_if, param );
			break;

		case GATTS_CONNECT:
			m_ble_api_obj->Set_Encryption( param );
			break;

		case GATTS_DISCONNECT:
			m_ble_api_obj->Start_Advertising( param );
#ifdef ESP32_SETUP
			ble_state = BLE_IDLE_OR_DISCONNECTED;
#endif
			break;

		case GATTS_READ:
			m_ble_api_obj->Read_Event( gatts_if, param );
			break;

		case GATTS_WRITE:
			m_ble_api_obj->Write_Event( gatts_if, param );
			break;

		case GATTS_READ_RESPONSE_COMPLETE:
			m_ble_api_obj->Read_Response( param );
			break;

		case GATTS_WRITE_RESPONSE_COMPLETE:
			m_ble_api_obj->Write_Response( param );
			break;

		case GATTS_INTERFACE_CREATED:
			m_ble_api_obj->Gatts_Create_Attr_Table( gatts_if, param );
			break;

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * For function description, Please refer to the Ble_Api.h file.
 *****************************************************************************************
 */

void Ble_Api::Ble_Event_Handler2( int32_t evt_id, void* param )	/// gap
{
	uint32_t map_index = 0;

	BLE_DEBUG_PRINT( DBG_MSG_INFO, "gap event: %d \n", evt_id );
	for ( map_index = 0; map_index < GAP_MAX_EVENT; map_index++ )
	{
		if ( evt_id == m_event_mapping_gap[map_index] )
		{
			break;
		}
	}

	switch ( map_index )
	{
		/* start event */
		case SYSTEM_BOOT:
			break;

		case GAP_SET_ADV_DATA_COMPLETE:
			m_ble_api_obj->Start_Advertising( param );
			break;

		case GAP_SECURITY_CONFIGURE_COMPLETE:	/// NA
			break;

		case GAP_SECURITY_REQUEST:
			/// TODO:
			break;

		case GAP_NUMERIC_COMPARISION_REQUEST:
			/// TODO:
			break;

		case GAP_PASSKEY_REQUEST:
			/// TODO:
			break;

		case GAP_OOB_REQUEST:
			/// TODO:
			break;

		case GAP_BONDING_SUCCESS:
			/// TODO:
			/// ble_wrapper_obj->abcd( );
			break;

		case GAP_BONDING_FAILURE:
			/// TODO:
			/// ble_wrapper_obj->abcd( );
			break;

		case GAP_REMOVE_BONDED_DEV_LIST:
			/// TODO:
			break;

		case GAP_SET_RAW_ADV_DATA_COMPLETE:
			m_ble_api_obj->Start_Advertising( param );
			break;

		case GAP_SET_RAW_SCAN_RSP_DATA_COMPLETE:
			m_ble_api_obj->Start_Advertising( param );
			break;

		case GAP_AUTH_COMPLETE:
			m_ble_api_obj->Authentication_Complete( param );
			break;

		default:
			break;
	}
}
