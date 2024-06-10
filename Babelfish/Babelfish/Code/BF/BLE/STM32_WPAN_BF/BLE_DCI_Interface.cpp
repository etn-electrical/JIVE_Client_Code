/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "includes.h"
#include "ble_gatt_aci.h"
#include "BLE_DCI_Interface.h"
#include "Change_Tracker.h"
#include "StdLib_MV.h"
#include "app_conf.h"


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BLE_DCI_Interface::BLE_DCI_Interface( Ble_DCI* ble_dci ) :
	m_ble_dci( ble_dci )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
tBleStatus BLE_DCI_Interface::Init( void )
{
	tBleStatus return_status = BLE_STATUS_SUCCESS;
	Service_UUID_t uuidsrv;
	Service_UUID_t uuidsrv_j;
	Char_UUID_t uuidch;
	uint8_t uuid_type = UUID_TYPE_128;
	uint8_t uuid_type_j = UUID_TYPE_128;
	uint8_t char_property = 0U;
	uint8_t notification_property = 0U;
	uint16_t Char_Value_Length = 0U;

	// Assign Service Handles for all BLE parameters
	// The purpose is to fill the service_handles array for each BLE parameter.
	// If 2 BLE parameters have same service UUID, aci_gatt_add_service() function is called only once but the
	// same handle number will be filled in the service_handles array using below logic.
	// Starting from 1 to skip the devcie name property which is added at the time of stack initilization.
	for ( uint_fast16_t i = 1U; ( i < DCI_BLE_DATA_TOTAL_CHAR_ID ) && ( return_status == BLE_STATUS_SUCCESS ); i++ )
	{
		// Find total attributes of particular service.
		uint32_t length = 0U;
		Ble_DCI::CHAR_ID_TD char_attr_count = 1U;

		if ( service_handles[i] == 0U )
		{
			Load_Service_UUID( &uuidsrv, i, &uuid_type );
			for ( uint_fast16_t j = i + 1U; j < DCI_BLE_DATA_TOTAL_CHAR_ID; j++ )
			{
				// Calculate total characteristics for this service.
				Load_Service_UUID( &uuidsrv_j, j, &uuid_type_j );
				if ( uuid_type == uuid_type_j )
				{
					if ( uuid_type == UUID_TYPE_16 )
					{
						length = 2U;
					}
					else
					{
						length = 16U;
					}
					if ( BF_Lib::Compare_Data( ble_dci_data_target_info.ble_to_dcid_struct[i].service_uuid,
											   ble_dci_data_target_info.ble_to_dcid_struct[j].service_uuid, length ) )
					{
						char_attr_count++;
					}
				}
			}
			return_status = aci_gatt_add_service( uuid_type, &uuidsrv, PRIMARY_SERVICE,
												  ( char_attr_count * 3U + 1U ), &service_handles[i] );
			if ( return_status == BLE_STATUS_SUCCESS )
			{
				// Fill up Service handles of same service.
				uint32_t length = 0U;
				for ( uint_fast16_t j = i + 1U; j < DCI_BLE_DATA_TOTAL_CHAR_ID; j++ )
				{
					if ( uuid_type == UUID_TYPE_16 )
					{
						length = 2U;
					}
					else
					{
						length = 16U;
					}
					if ( BF_Lib::Compare_Data( ble_dci_data_target_info.ble_to_dcid_struct[i].service_uuid,
											   ble_dci_data_target_info.ble_to_dcid_struct[j].service_uuid, length ) )
					{
						// Fill the service handle array of parameters which have same service uuid.
						service_handles[j] = service_handles[i];
					}
				}
			}
		}
	}
	// Add GATT characteristic for all BLE parameters
	if ( return_status == BLE_STATUS_SUCCESS )
	{
		for ( uint_fast16_t i = 1U; ( i < DCI_BLE_DATA_TOTAL_CHAR_ID ) && ( return_status == BLE_STATUS_SUCCESS ); i++ )
		{
			Load_Char_UUID( &uuidch, i, &uuid_type );
			char_property = ble_dci_data_target_info.ble_to_dcid_struct[i].ble_attrib;
			notification_property = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
			if ( ( char_property & CHAR_PROP_WRITE ) == CHAR_PROP_WRITE )
			{
				notification_property |= GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP;
			}
			else if ( ( char_property & CHAR_PROP_WRITE_WITHOUT_RESP ) == CHAR_PROP_WRITE_WITHOUT_RESP )
			{
				notification_property |= GATT_NOTIFY_ATTRIBUTE_WRITE;
			}

			Char_Value_Length = m_ble_dci->Get_Length( ble_dci_data_target_info.ble_to_dcid_struct[i].dcid );

			return_status = aci_gatt_add_char( service_handles[i], uuid_type, &uuidch, Char_Value_Length, char_property,
											   ATTR_PERMISSION, notification_property, ENC_KEY_SIZE, VARIABLE,
											   &characteristic_handles[i] );
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BLE_DCI_Interface::Load_Service_UUID( Service_UUID_t* uuidsrv, Ble_DCI::CHAR_ID_TD index, uint8_t* uuid_type )
{
	uint_fast8_t j = 0U;
	uint_fast8_t k = 0U;

	if ( Check_Zero( &ble_dci_data_target_info.ble_to_dcid_struct[index].service_uuid[2U] ) == true )
	{
		uuidsrv->Service_UUID_16 = ble_dci_data_target_info.ble_to_dcid_struct[index].service_uuid[1U];
		uuidsrv->Service_UUID_16 |= ( ble_dci_data_target_info.ble_to_dcid_struct[index].service_uuid[0U] << 8U );
		*uuid_type = UUID_TYPE_16;
	}
	else
	{
		for ( j = 0U, k = 15U; j < 16U; j++, k-- )
		{
			uuidsrv->Service_UUID_128[j] = ble_dci_data_target_info.ble_to_dcid_struct[index].service_uuid[k];
		}
		*uuid_type = UUID_TYPE_128;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BLE_DCI_Interface::Load_Char_UUID( Char_UUID_t* uuidchar, Ble_DCI::CHAR_ID_TD index, uint8_t* uuid_type )
{
	uint8_t j = 0U;
	uint8_t k = 0U;

	if ( Check_Zero( &ble_dci_data_target_info.ble_to_dcid_struct[index].char_uuid[2U] ) == true )
	{
		uuidchar->Char_UUID_16 = ble_dci_data_target_info.ble_to_dcid_struct[index].char_uuid[1U];
		uuidchar->Char_UUID_16 |= ( ble_dci_data_target_info.ble_to_dcid_struct[index].char_uuid[0U] << 8 );
		*uuid_type = UUID_TYPE_16;
	}
	else
	{
		for ( j = 0U, k = 15U; j < 16U; j++, k-- )
		{
			uuidchar->Char_UUID_128[j] = ble_dci_data_target_info.ble_to_dcid_struct[index].char_uuid[k];
		}
		*uuid_type = UUID_TYPE_128;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool BLE_DCI_Interface::Find_Char_Attribute_Index( uint16_t char_attribute, Ble_DCI::CHAR_ID_TD* index_ret )
{
	bool char_attrib_found = false;
	Ble_DCI::CHAR_ID_TD index = 0U;

	for ( index = 0U; ( index < DCI_BLE_DATA_TOTAL_CHAR_ID ) && ( char_attrib_found == false ); index++ )
	{
		if ( characteristic_handles[index] == char_attribute )
		{
			char_attrib_found = true;
		}
	}
	*index_ret = index - 1U;
	return ( char_attrib_found );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool BLE_DCI_Interface::Find_Index_From_Dcid( DCI_ID_TD dcid, Ble_DCI::CHAR_ID_TD* index_ret )
{
	bool dcid_found = false;
	Ble_DCI::CHAR_ID_TD index = 0U;

	for ( index = 0U; ( index < DCI_BLE_DATA_TOTAL_CHAR_ID ) && ( dcid_found == false ); index++ )
	{
		if ( ( ble_dci_data_target_info.ble_to_dcid_struct[index].dcid ) == dcid )
		{
			dcid_found = true;
			*index_ret = index;
		}
	}
	return ( dcid_found );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool BLE_DCI_Interface::Check_Zero( const uint8_t* str )
{
	bool zero_flag = true;

	for ( uint_fast8_t i = 0U; ( i < 14U ) && ( zero_flag == true ); i++ )
	{
		if ( str[i] != 0U )
		{
			zero_flag = false;
		}
	}
	return ( zero_flag );
}