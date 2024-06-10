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
#include "BLE_Stack_Interface.h"
#include "Change_Tracker.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BLE_Stack_Interface::BLE_Stack_Interface( BLE_STAT_CALLBACK_FUNC* get_server_connection_status_cb,
										  BLE_DCI_Interface* ble_dci_interface_handle ) :
	m_change_tracker( new Change_Tracker( &Change_Track_Handler_Static, this, true ) ),
	m_get_server_connection_status_cb( get_server_connection_status_cb ),
	m_ble_dci_interface_handle( ble_dci_interface_handle )
{
	uint8_t char_property = 0U;

	for ( uint_fast16_t i = 1U; i < DCI_BLE_DATA_TOTAL_CHAR_ID; i++ )
	{
		char_property = ble_dci_data_target_info.ble_to_dcid_struct[i].ble_attrib;
		if ( ( char_property & CHAR_PROP_NOTIFY ) == CHAR_PROP_NOTIFY )
		{
			m_change_tracker->Track_ID( ble_dci_data_target_info.ble_to_dcid_struct[i].dcid );
			notification_enable[i] = true;	// TODO: enable or disable at run time as per client's request.
											// Problem is we don't get event when user enables the notification
											// button on mobile.
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BLE_Stack_Interface::Change_Track_Handler( DCI_ID_TD dcid )
{

	Ble_DCI::CHAR_ID_TD index = 0U;
	DCI_LENGTH_TD length = 0U;
	DCI_LENGTH_TD offset = 0U;

	if ( m_ble_dci_interface_handle->Find_Index_From_Dcid( dcid, &index ) )
	{
		if ( notification_enable[index] == true )
		{
			m_ble_dci_interface_handle->m_ble_dci->Get_Dcid_Data( dcid, &length, dest_data_change_track, offset );
			Ble_Char_Value_Update( m_ble_dci_interface_handle->service_handles[index],
								   m_ble_dci_interface_handle->characteristic_handles[index], offset, length,
								   dest_data_change_track );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
tBleStatus BLE_Stack_Interface::Ble_Char_Value_Update( uint16_t serv_handle, uint16_t char_handle,
													   DCI_LENGTH_TD offset, DCI_LENGTH_TD length, uint8_t* data )
{
	tBleStatus ret = BLE_STATUS_PENDING;

	if ( m_get_server_connection_status_cb() == APP_BLE_CONNECTED_SERVER )
	{
		ret = aci_gatt_update_char_value( serv_handle, char_handle, offset, length, data );
		if ( ret != BLE_STATUS_SUCCESS )
		{
			ret = BLE_STATUS_ERROR;
		}
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool BLE_Stack_Interface::Read_Event( uint16_t char_attribute, uint16_t offset, tBleStatus* err_code )
{
	bool return_status = BLE_EVENT_SUCCESS;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ID_TD dcid = 0U;
	static DCI_LENGTH_TD full_length = 0U;	// retaining the length
	DCI_LENGTH_TD length = 0U;
	Ble_DCI::CHAR_ID_TD index = 0U;
	bool char_attrib_found = false;

	char_attrib_found = m_ble_dci_interface_handle->Find_Char_Attribute_Index( char_attribute, &index );

	if ( char_attrib_found == true )
	{
		dcid = ble_dci_data_target_info.ble_to_dcid_struct[index].dcid;

		if ( offset == 0U )
		{
			dci_error = m_ble_dci_interface_handle->m_ble_dci->Get_Dcid_Data( dcid, &length, dest_data_read, offset );
			full_length = length;
			if ( dci_error != DCI_ERR_NO_ERROR )
			{
				*err_code = m_ble_dci_interface_handle->m_ble_dci->Interpret_DCI_Error( dci_error );
				return_status = BLE_EVENT_ERROR;
			}
		}
		else
		{
			if ( offset >= full_length )
			{
				// error detected in offset calculation.
				*err_code = BLE_INVALID_DATA_ID_ERROR_CODE;
				return_status = BLE_EVENT_ERROR;
			}
			else
			{
				length = full_length - offset;
			}
		}
		if ( return_status == BLE_EVENT_SUCCESS )
		{
	#ifdef DEBUG
			BF_ASSERT( length <= MAX_BUFFER );
	#endif
			*err_code = Ble_Char_Value_Update( m_ble_dci_interface_handle->service_handles[index],
											   m_ble_dci_interface_handle->characteristic_handles[index], 0U, length,
											   &dest_data_read[offset] );
			if ( *err_code != BLE_STATUS_SUCCESS )
			{
				return_status = BLE_EVENT_ERROR;
			}
		}
	}
	else
	{
		*err_code = BLE_OTHER_DCID_ERROR_CODE;
		return_status = BLE_EVENT_ERROR;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool BLE_Stack_Interface::Write_Event( uint16_t char_attribute, uint8_t* data, tBleStatus* err_code )
{
	bool return_status = BLE_EVENT_SUCCESS;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ID_TD dcid = 0U;
	DCI_LENGTH_TD length = 0U;
	Ble_DCI::CHAR_ID_TD index = 0U;
	bool char_attrib_found = false;

	char_attrib_found = m_ble_dci_interface_handle->Find_Char_Attribute_Index( char_attribute, &index );

	if ( char_attrib_found == true )
	{
		dcid = ble_dci_data_target_info.ble_to_dcid_struct[index].dcid;
		length = m_ble_dci_interface_handle->m_ble_dci->Get_Length( dcid );
		dci_error = m_ble_dci_interface_handle->m_ble_dci->Set_Dcid_Data( dcid, length, data );
		if ( dci_error != DCI_ERR_NO_ERROR )
		{
			*err_code = m_ble_dci_interface_handle->m_ble_dci->Interpret_DCI_Error( dci_error );
			return_status = BLE_EVENT_ERROR;
		}
	}
	else
	{
		*err_code = BLE_OTHER_DCID_ERROR_CODE;
		return_status = BLE_EVENT_ERROR;
	}
	return ( return_status );
}
