/**
 *****************************************************************************************
 *	@file		Esp32_Ble_Api.cpp
 *	@details    See header file for  overview.
 *	@copyright  2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Esp32_Ble_Api.h"
#include "StdLib_MV.h"
///< Stack files
#include "Ble_Defines.h"
#include "BLE_DCI_DATA.h"
#include "Ble_DCI.h"
#include "Ble_User_Management.h"
#include "DCI_Owner.h"
#include "BLE_Debug.h"
#include "Ble_Api.h"
#include "Single_LED.h"
extern uint8_t ble_state;
Ble_Api* Ble_Api::m_ble_api_obj = nullptr;
int32_t Ble_Api::m_event_mapping_gap[] = { 0 };
int32_t Ble_Api::m_event_mapping_gatts[] = { 0 };


#ifdef CONFIG_BT_ENABLED
/*
 *
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Esp32_Ble_Api::Esp32_Ble_Api( Ble_Advertisement* ble_adv, Ble_DCI* ble_dci, Ble_User_Management* ble_user_mgmt,
							  Single_LED* ble_led ) :
	m_ble_adv( ble_adv ),
	m_ble_dci( ble_dci ),
	m_ble_led( ble_led ),
	m_ble_user_mgmt( ble_user_mgmt )
{
	m_ble_api_obj = this;
	m_ble_display = ble_led;
	Ble_Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Ble_Init()
{
	uint32_t map_index = 0;
	int32_t m_Event_Mapping_Gatts_Esp[GATTS_MAX_EVENT] =
	{
		ESP_GATTS_CREAT_ATTR_TAB_EVT,
		ESP_GATTS_CONNECT_EVT,
		ESP_GATTS_DISCONNECT_EVT,
		ESP_GATTS_READ_EVT,
		ESP_GATTS_WRITE_EVT,
		ESP_GATTS_RESPONSE_EVT,
		ESP_GATTS_RESPONSE_EVT,
		ESP_GATTS_REG_EVT,
	};

	int32_t m_Event_Mapping_Gap_Esp[GAP_MAX_EVENT] =
	{
		-1,		/// Event not applicable for ESP32
		ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT,
		-1,		/// Event not applicable for ESP32
		ESP_GAP_BLE_SEC_REQ_EVT,
		ESP_GAP_BLE_PASSKEY_REQ_EVT,
		ESP_GAP_BLE_OOB_REQ_EVT,
		ESP_GAP_BLE_NC_REQ_EVT,
		ESP_GAP_BLE_AUTH_CMPL_EVT,
		-1,		/// Event not applicable for ESP32
		-1,		/// Event not applicable for ESP32
		ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT
	};

	for ( map_index = 0; map_index < GAP_MAX_EVENT; map_index++ )
	{
		m_event_mapping_gap[map_index] = m_Event_Mapping_Gap_Esp[map_index];
	}
	for ( map_index = 0; map_index < GATTS_MAX_EVENT; map_index++ )
	{
		m_event_mapping_gatts[map_index] = m_Event_Mapping_Gatts_Esp[map_index];
	}

	m_ble_api_obj->Init_Configuration();

	m_ble_api_obj->Register_Callbacks();

	m_ble_api_obj->Ble_Led_Display_Control();

	m_ble_api_obj->Security_Configure();

	m_ble_api_obj->Config_Adv_Data();

	m_ble_api_obj->Config_Raw_Adv_Data();

}

/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Init_Configuration( void )
{
	esp_err_t ret;
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	uint16_t serv_idx = 0;

	ESP_ERROR_CHECK( esp_bt_controller_mem_release( ESP_BT_MODE_CLASSIC_BT ) );
	ret = esp_bt_controller_init( &bt_cfg );

	if ( ret )
	{
		BLE_DEBUG_PRINT( DBG_MSG_ERROR, "%s initialize controller failed: %s\n",  __func__, esp_err_to_name( ret ) );
		return;
	}
	ret = esp_bt_controller_enable( ESP_BT_MODE_BLE );

	if ( ret )
	{
		BLE_DEBUG_PRINT( DBG_MSG_ERROR, "%s enable controller failed: %s\n",  __func__, esp_err_to_name( ret ) );
		return;
	}
	ret = esp_bluedroid_init();

	if ( ret )
	{
		BLE_DEBUG_PRINT( DBG_MSG_ERROR, "%s  init bluetooth failed: %s\n",  __func__, esp_err_to_name( ret ) );
		return;
	}
	ret = esp_bluedroid_enable();

	if ( ret )
	{
		BLE_DEBUG_PRINT( DBG_MSG_ERROR, "%s  enable bluetooth failed: %s\n",  __func__, esp_err_to_name( ret ) );
		return;
	}

	///< Init gatts_if with ESP_GATT_IF_NONE for each service before register the services
	for ( serv_idx = 0; serv_idx < TOTAL_SERVICE; serv_idx++ )
	{
		gatts_profile_tab[serv_idx].gatts_if = ESP_GATT_IF_NONE;
	}
}

/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Config_Raw_Adv_Data( void )
{
	struct advt_data_struct_t
	{
		uint8_t advt_data_len;
		uint8_t advt_data[31];
	};

	advt_data_struct_t advertisement_data_struct = { 0U, 0U };

	advertisement_data_struct.advt_data_len = m_ble_adv->Get_Full_Packet( advertisement_data_struct.advt_data );
	esp_err_t raw_adv_ret =
		esp_ble_gap_config_adv_data_raw( ( uint8_t* )( advertisement_data_struct.advt_data ),
										 ( uint32_t )advertisement_data_struct.advt_data_len );

	if ( raw_adv_ret )
	{
		BLE_DEBUG_PRINT( DBG_MSG_ERROR, "config raw adv data failed, error code = %x ",  raw_adv_ret );
	}

}

/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Register_Callbacks( void )
{
	esp_err_t ret = ESP_FAIL;

	ret = esp_ble_gatts_register_callback( ( esp_gatts_cb_t ) ( Ble_Event_Handler1 ) );

	if ( ret )
	{
		BLE_DEBUG_PRINT( DBG_MSG_ERROR, "gatts register error, error code = %x",   ret );
	}
	else
	{
		ret = esp_ble_gap_register_callback( ( esp_gap_ble_cb_t ) Ble_Event_Handler2 );

		if ( ret )
		{
			BLE_DEBUG_PRINT( DBG_MSG_ERROR, "gap register error, error code = %x",   ret );
		}
	}

	if ( ret )
	{
		return;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Config_Adv_Data( void )
{
	esp_err_t ret;
	config_data_struct_t adv_data;
	static DCI_Owner* m_min_conn_interval_owner = nullptr;
	static DCI_Owner* m_max_conn_interval_owner = nullptr;

	adv_data.set_scan_rsp = true;
	adv_data.include_name = true;
	adv_data.include_txpower = true;
	m_min_conn_interval_owner = new DCI_Owner( DCI_BLE_MIN_VAL_CONN_INTERVAL_DCID );
	m_min_conn_interval_owner->Check_Out( adv_data.min_interval );
	m_max_conn_interval_owner = new DCI_Owner( DCI_BLE_MAX_VAL_CONN_INTERVAL_DCID );
	m_max_conn_interval_owner->Check_Out( adv_data.max_interval );
	adv_data.appearance = 0x00;
	adv_data.manufacturer_len = 0;			/// TEST_MANUFACTURER_DATA_LEN,
	adv_data.p_manufacturer_data = NULL;	/// &test_manufacturer[0],
	adv_data.service_data_len = 0;
	adv_data.p_service_data = NULL;
	adv_data.service_uuid_len = sizeof( sec_service_uuid );
	adv_data.p_service_uuid = sec_service_uuid;
	adv_data.flag = ( ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT );
	esp_ble_adv_data_t* config_data = ( esp_ble_adv_data_t* )( &adv_data );

	ret = esp_ble_gap_config_adv_data( config_data );

	if ( ret )
	{
		BLE_DEBUG_PRINT( DBG_MSG_ERROR, "config adv data failed, error code = %x",   ret );
		ble_state = BLE_FAULT;
	}

}

/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Start_Advertising( void* advertisement_data )
{
	const uint8_t ACCESS_INVALID = 0xFF;
	uint8_t active_access_level = ACCESS_INVALID;

	esp_ble_gap_start_advertising( &esp32_adv_config_params );

	active_access_level = m_ble_user_mgmt->Get_Active_Access_Level();
	BLE_DEBUG_PRINT( DBG_MSG_INFO, "Start_Advertising ,ACTIVE ACCESS LEVEL:%d\n", active_access_level );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Security_Configure( void )
{
	esp_err_t ret;
	uint16_t serv_idx = 0;
	uint32_t passkey = 0U;
	static DCI_Owner* m_passkey_update_owner = nullptr;

	///< set the security iocap & auth_req & key size & init key response key parameters to the stack
	esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;		/// bonding with peer device after
	// authentication
	/// esp_ble_io_cap_t iocap = ESP_IO_CAP_IO; ///ESP_IO_CAP_NONE;                 /// set the IO capability to No
	/// output No input
	esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
	uint8_t key_size = 16;											/// the key size should be 7~16 bytes
	uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;

	m_passkey_update_owner = new DCI_Owner( DCI_BLE_DEVICE_PASSKEY_DCID );
	m_passkey_update_owner->Check_Out( passkey );
	uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE;
	uint8_t oob_support = ESP_BLE_OOB_DISABLE;

	esp_ble_gap_set_security_param( ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof( uint32_t ) );
	esp_ble_gap_set_security_param( ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof( uint8_t ) );
	esp_ble_gap_set_security_param( ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof( uint8_t ) );
	esp_ble_gap_set_security_param( ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof( uint8_t ) );
	esp_ble_gap_set_security_param( ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof( uint8_t ) );
	esp_ble_gap_set_security_param( ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof( uint8_t ) );
	esp_ble_gap_set_security_param( ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof( uint8_t ) );
	esp_ble_gap_set_security_param( ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof( uint8_t ) );

	///< Register application identifier for each service
	for ( serv_idx = 0; serv_idx < TOTAL_SERVICE; serv_idx++ )
	{
		ret = esp_ble_gatts_app_register( serv_idx );
		if ( ret )
		{
			BLE_DEBUG_PRINT( DBG_MSG_ERROR, "gatts app register error, error code = %x", ret );
			ble_state = BLE_ATTENTION;
			return;
		}
		BLE_DEBUG_PRINT( DBG_MSG_INFO, "gatts app register for service ID = %d", serv_idx );
	}

	esp_ble_gap_set_device_name( TEST_DEVICE_NAME );
	esp_ble_gap_config_local_privacy( true );
	ble_state = BLE_PAIRING;

}

/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Set_Encryption( void* param1 )
{
	esp_ble_gatts_cb_param_t* param = ( esp_ble_gatts_cb_param_t* )( param1 );

	BLE_DEBUG_PRINT( DBG_MSG_DEBUG, "Starting security connect event with peer device" );
	///< start security connect with peer device when receive the connect event sent by the master
	esp_ble_set_encryption( param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM );
	ble_state = BLE_CONNECTED;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Set_Bondable_Mode( void* bondable_mode_param )
{
	bondable_mode_param_t* bondable_mode_param_ptr = reinterpret_cast<bondable_mode_param_t*>
		( bondable_mode_param );
	uint16_t* dev_num = bondable_mode_param_ptr->dev_num;
	esp_ble_bond_dev_t* dev_list = bondable_mode_param_ptr->dev_list;

	esp_ble_get_bond_device_list( ( int* ) dev_num, dev_list );
}

/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Connection_Parameter( void* connection_param )
{
	esp_bd_addr_t bd_addr;

	connection_param_struct_t* connection_param_ptr = reinterpret_cast<connection_param_struct_t*>
		( connection_param );
	esp_ble_conn_update_params_t* params = connection_param_ptr->params;

	memcpy( bd_addr, connection_param_ptr->bd_addr, sizeof( esp_bd_addr_t ) );
	uint16_t min_conn_interval = connection_param_ptr->min_interval;
	uint16_t max_conn_interval = connection_param_ptr->max_interval;
	uint16_t slave_latency = connection_param_ptr->latency;
	uint16_t supervision_tout = connection_param_ptr->timeout;

	esp_ble_gap_update_conn_params( params );
	esp_ble_gap_set_prefer_conn_params( bd_addr, min_conn_interval, max_conn_interval, slave_latency,
										supervision_tout );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Read_Response( void* read_response_data )
{
	BLE_DEBUG_PRINT( DBG_MSG_DEBUG, "Read Response\n" );
	ble_state = BLE_POLLING;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Write_Response( void* write_response_data )
{
	BLE_DEBUG_PRINT( DBG_MSG_DEBUG, "Write Response\n" );
	ble_state = BLE_POLLING;
}

/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Gatts_Create_Attr_Table( int gatts_if, void* param )
{
	esp_gatt_if_t esp_gatts_if = ( esp_gatt_if_t )( gatts_if );
	uint16_t serv_id = 0U;

	esp_ble_gatts_cb_param_t* esp_param =
		( esp_ble_gatts_cb_param_t* ) ( param );

	///< If event is register event, store the gatts_if for each profile
	if ( esp_param->reg.status == ESP_GATT_OK )
	{
		gatts_profile_tab[esp_param->reg.app_id].gatts_if = esp_gatts_if;
	}
	else
	{
		BLE_DEBUG_PRINT( DBG_MSG_INFO, "Reg app failed, app_id %04x, status %d\n",
						 esp_param->reg.app_id, esp_param->reg.status );
		return;
	}

	///< Find the Service ID from gatts_if
	serv_id = Find_Service_Id_From_Gatt_If( esp_gatts_if );

	///< Create the service and Characteristics using attribute table */
	esp_ble_gatts_create_attr_tab(
		esp_gatts_attrib_db[serv_id].attrib_tbl, esp_gatts_if,
		esp_gatts_attrib_db[serv_id].total_attrib, serv_id );
	BLE_DEBUG_PRINT( DBG_MSG_ERROR, "Attribute table created for Service %d\n",
					 serv_id );
}

/*
 *****************************************************************************************
 * Gatts_start_service.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Gatts_Start_Service( int32_t gatts_if, void* param )
{
	esp_gatt_if_t esp_gatts_if = ( esp_gatt_if_t )( gatts_if );
	uint16_t serv_id = Find_Service_Id_From_Gatt_If( esp_gatts_if );
	esp_ble_gatts_cb_param_t* esp_param =
		( esp_ble_gatts_cb_param_t* ) ( param );
	uint16_t attri_idx = 0;

	BLE_DEBUG_PRINT( DBG_MSG_INFO, "[Gatts_Start_Service] gatts_if = %x\n,The number handle = %x",
					 esp_gatts_if, esp_param->add_attr_tab.num_handle );
	if ( esp_param->create.status == ESP_GATT_OK )
	{
		if ( esp_param->add_attr_tab.num_handle ==
			 esp_gatts_attrib_db[serv_id].total_attrib )
		{
			///< Copied/Saved the Handle table generated by esp into local table
			memcpy( gatts_profile_tab[serv_id].attrib_handle_tbl,
					esp_param->add_attr_tab.handles,
					( esp_param->add_attr_tab.num_handle
					  * sizeof( attribute_handle ) ) );
			for ( attri_idx = 0;
				  attri_idx < esp_param->add_attr_tab.num_handle;
				  attri_idx++ )
			{
				BLE_DEBUG_PRINT(
					DBG_MSG_INFO, "Handle = %d\n",
					gatts_profile_tab[serv_id].attrib_handle_tbl[attri_idx] );
			}
			///< Start the GATTS service using service handle number
			esp_ble_gatts_start_service(
				gatts_profile_tab[serv_id].attrib_handle_tbl[0] );			/// Here 0 index point to Service Attribute
		}
		else
		{
			BLE_DEBUG_PRINT( DBG_MSG_INFO,
							 "Create attribute table abnormally, num_handle (%d) doesn't equal to Service(%d)'s Total Attributes(%d)",
							 esp_param->add_attr_tab.num_handle, serv_id,
							 esp_gatts_attrib_db[serv_id].total_attrib );
		}
	}
	else
	{
		BLE_DEBUG_PRINT( DBG_MSG_ERROR, " Create attribute table failed, error code = %x",
						 esp_param->create.status );
	}
}

/*
 *****************************************************************************************
 * Read_Event
 *****************************************************************************************
 */
void Esp32_Ble_Api::Read_Event( int32_t gatts_if, void* param )
{
	esp_gatt_if_t esp_gatts_if = ( esp_gatt_if_t ) ( gatts_if );
	esp_ble_gatts_cb_param_t* esp_param = ( esp_ble_gatts_cb_param_t* ) ( param );
	esp_gatt_status_t esp_gatt_status = ESP_GATT_OK;
	esp_err_t response_err = ESP_OK;
	esp_gatt_rsp_t rsp;
	uint16_t rsp_attr_value_index = 0;
	uint16_t dest_data_read_index = 0;
	uint8_t* dest_data_ptr = dest_data_read;
	///< DCI handle variables
	Ble_DCI::CHAR_ID_TD ble_unique_id = 0U;
	const uint8_t ACCESS_INVALID = 0xFF;
	uint8_t active_access_level = ACCESS_INVALID;
	const uint8_t NO_ACCESS = 0;
	DCI_ID_TD dcid = 0U;
	uint8_t read_access = 0U;
	uint8_t write_access = 0U;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	static DCI_LENGTH_TD full_length = 0U;	/// retaining the length
	DCI_LENGTH_TD length = 0U;

	ble_state = BLE_POLLING;
	uint16_t ble_unique_id_each_par = 0U;
	uint8_t group_index_count = 0U, index = 0U;
	uint8_t group_assem_check = false;

	///< Fetching the current access level of the user
	active_access_level = m_ble_user_mgmt->Get_Active_Access_Level();
	BLE_DEBUG_PRINT( DBG_MSG_INFO, "Read_Event : gatts_if %d, Read Handle = %d offset = %d \n",
					 esp_gatts_if, esp_param->read.handle, esp_param->read.offset );

	ble_unique_id = ( Ble_DCI::CHAR_ID_TD ) esp_param->read.handle;
	BLE_DEBUG_PRINT( DBG_MSG_INFO, " Ble_unique_id = %d \n", ble_unique_id );

	if ( ble_unique_id >= ( m_ble_dci->m_group_profile->ble_dci_group_struct[0].group_dcid ) )
	{
		group_index_count =
			( ( ble_unique_id - ( m_ble_dci->m_group_profile->ble_dci_group_struct[0].group_dcid ) ) / 2U );

		///< This condition will cross verify the ble_unique_id and group_dcid are same or not.
		if ( m_ble_dci->m_group_profile->ble_dci_group_struct[group_index_count].group_dcid == ble_unique_id )
		{
			group_assem_check = true;

			/* This loop will run till the total parameters inside the assembly/group
			   taken from the structure based on the group index count.*/
			for ( int i = 0;
				  i < m_ble_dci->m_group_profile->ble_dci_group_struct[group_index_count].total_parameters;
				  i++ )
			{
				ble_unique_id_each_par =
					m_ble_dci->m_group_profile->ble_dci_group_struct[group_index_count].ble_dcid[i];
				BLE_DEBUG_PRINT( DBG_MSG_INFO, "ble_unique_id_each_par is =%d\r\n", ble_unique_id_each_par );
				dcid = m_ble_dci->Find_Ble_Mod_DCID( ble_unique_id_each_par, &read_access, &write_access );
				BLE_DEBUG_PRINT( DBG_MSG_INFO, "Read Access level %d\r\n", read_access );
				if ( dcid != DCI_ID_UNDEFINED )
				{
					if ( ( ( read_access != NO_ACCESS ) && ( read_access != ACCESS_INVALID ) ) &&
						 ( active_access_level >= read_access ) )
					{
						dci_error = m_ble_dci->Get_Dcid_Data( dcid, reinterpret_cast<DCI_LENGTH_TD*>( &length ),
															  reinterpret_cast<uint8_t*>( m_ble_dci->
																						  m_individual_readvalue ) );
						BLE_DEBUG_PRINT( DBG_MSG_INFO, "DCID is = %d,Access level %d\r\n", dcid,
										 active_access_level );
						if ( dci_error != DCI_ERR_NO_ERROR )
						{
							BLE_DEBUG_PRINT( DBG_MSG_DEBUG, "Read Access Not Permitted\r\n" );
							dci_error = DCI_ERR_NO_ERROR;
						}
						else
						{
							BF_Lib::Copy_String( dest_data_ptr + index, m_ble_dci->m_individual_readvalue, length );
							index += length;

						}
					}
					else
					{
						BLE_DEBUG_PRINT( DBG_MSG_INFO,
										 "Current Access level is %d\n\rRead Access Not Permitted for current access level\n",
										 active_access_level );
						dci_error = DCI_ERR_ACCESS_VIOLATION;
					}
				}
				else
				{
					BLE_DEBUG_PRINT( DBG_MSG_INFO, "DCID is invalid\n" );
					dci_error = DCI_ERR_INVALID_DATA_ID;
				}
			}
		}
		else
		{
			dci_error = DCI_ERR_INVALID_DATA_ID;

		}
	}
	else
	{
		if ( esp_param->read.offset == 0U )
		{
			///< If offset is zero means Read Event comes first time, then get data from DCI

			ble_unique_id = ( Ble_DCI::CHAR_ID_TD ) esp_param->read.handle;
			///< Get the DCID from CHAR_ID
			dcid = m_ble_dci->Find_Ble_Mod_DCID( ble_unique_id, &read_access, &write_access );
			BLE_DEBUG_PRINT( DBG_MSG_INFO, "Active access level is %d and Read Access level is %d\r\n",
							 active_access_level, read_access );
			if ( dcid == DCI_ID_UNDEFINED )
			{
				BLE_DEBUG_PRINT( DBG_MSG_ERROR, "Read_Event : Find_Ble_Mod_DCID : Error Undefined dcid %d\n", dcid );
				esp_gatt_status = ESP_GATT_NOT_FOUND;
			}
			else
			{
				BLE_DEBUG_PRINT( DBG_MSG_INFO, "Read_Event : Find_Ble_Mod_DCID : dcid %d\n", dcid );
				///< Clear the data buffer
				memset( dest_data_read, 0, BLE_ADV_BUF_MAX_SIZE );
				///< Read Data from DCI using DCID into data buffer
				if ( ( ( read_access != NO_ACCESS ) && ( read_access != ACCESS_INVALID ) ) &&
					 ( active_access_level >= read_access ) )
				{
					dci_error = m_ble_dci->Get_Dcid_Data( dcid, &length, dest_data_read );
					if ( dci_error != DCI_ERR_NO_ERROR )
					{
						BLE_DEBUG_PRINT( DBG_MSG_DEBUG, "Read Access Not Permitted\r\n" );
						BLE_DEBUG_PRINT( DBG_MSG_ERROR, "Read_Event : Error while Get_Dcid_Data %d\n", dci_error );
						esp_gatt_status = Interpret_Ble_Error( m_ble_dci->Interpret_DCI_Error( dci_error ) );
					}
					else
					{
						/* Save the DCID data length into static variable full_length for sending data into chunk if
						   data is
						   long. */
						full_length = length;
						BLE_DEBUG_PRINT( DBG_MSG_INFO, "Read_Event : Get_Dcid_Data : length  %d\n", length );
					}
				}
				else
				{
					BLE_DEBUG_PRINT( DBG_MSG_DEBUG, "Read Access Not Permitted\r\nAccess level %d\r\n",
									 active_access_level );
					dci_error = DCI_ERR_ACCESS_VIOLATION;
				}
			}
		}
		else
		{
			///< If Offset is non-zero means Read Event comes for sending data into multiple chunks

			if ( esp_param->read.offset >= full_length )
			{
				///< error detected in offset calculation.
				BLE_DEBUG_PRINT( DBG_MSG_ERROR, "Read_Event : error detected in offset calculation\n" );
				esp_gatt_status = ESP_GATT_INVALID_OFFSET;
			}
			else
			{
				length = full_length - esp_param->read.offset;
			}

		}
	}
	///< Prepare the GATT remote read request response
	memset( &rsp, 0, sizeof( esp_gatt_rsp_t ) );
	rsp.attr_value.handle = esp_param->read.handle;

	///< If no error then copied data from data buffer into esp
	if ( esp_gatt_status == ESP_GATT_OK )
	{
		rsp.attr_value.len = length;

		///< Compare length with GATT maximum attribute length
		if ( length > ESP_GATT_MAX_ATTR_LEN )
		{
			length = ESP_GATT_MAX_ATTR_LEN;
		}

		if ( group_assem_check == true )
		{
			dest_data_read_index = index;
			for ( rsp_attr_value_index = 0; rsp_attr_value_index < dest_data_read_index; rsp_attr_value_index++ )
			{
				rsp.attr_value.value[rsp_attr_value_index] = dest_data_read[rsp_attr_value_index];
				BLE_DEBUG_PRINT( DBG_MSG_INFO, "response attr value is = %d\r\n",
								 rsp.attr_value.value[rsp_attr_value_index] );
			}

		}
		else
		{
			///< Copy data into Read Request Response attribute value buffer
			for ( rsp_attr_value_index = 0, dest_data_read_index = esp_param->read.offset;
				  rsp_attr_value_index < length; rsp_attr_value_index++, dest_data_read_index++ )
			{
				rsp.attr_value.value[rsp_attr_value_index] =
					dest_data_read[dest_data_read_index];
				BLE_DEBUG_PRINT( DBG_MSG_INFO, "response attr value is = %d\r\n",
								 rsp.attr_value.value[rsp_attr_value_index] );
			}
		}
	}

	/* Sent prepared GATT remote Read Request Response */
	response_err = esp_ble_gatts_send_response( esp_gatts_if, esp_param->read.conn_id,
												esp_param->read.trans_id, esp_gatt_status,
												&rsp );
	if ( response_err != ESP_OK )
	{
		BLE_DEBUG_PRINT( DBG_MSG_ERROR, "Read_Event: Send gatts response error\n" );
	}
}

/*
 *****************************************************************************************
 * Write_Event
 *****************************************************************************************
 */
void Esp32_Ble_Api::Write_Event( int32_t gatts_if, void* param )
{
	esp_gatt_if_t esp_gatts_if = ( esp_gatt_if_t ) ( gatts_if );
	esp_ble_gatts_cb_param_t* esp_param = ( esp_ble_gatts_cb_param_t* ) ( param );
	esp_gatt_status_t esp_gatt_status = ESP_GATT_OK;
	esp_err_t response_err = ESP_OK;
	esp_gatt_rsp_t rsp;
	static uint16_t full_length = 0U;	/// retaining the length

	const uint8_t ACCESS_INVALID = 0xFF;
	uint8_t active_access_level = ACCESS_INVALID;
	const uint8_t NO_ACCESS = 0;

	///< DCI handle variables
	DCI_ID_TD dcid = 0U;
	Ble_DCI::CHAR_ID_TD ble_unique_id = ( Ble_DCI::CHAR_ID_TD ) esp_param->write.handle;
	uint8_t read_access;
	uint8_t write_access;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;

	ble_state = BLE_POLLING;
	///< Fetching the current access level of the user
	active_access_level = m_ble_user_mgmt->Get_Active_Access_Level();
	BLE_DEBUG_PRINT( DBG_MSG_INFO, "Write_Event : gatts_if %d, Read Handle = %d offset = %d \n", esp_gatts_if,
					 esp_param->write.handle, esp_param->write.offset );
	BLE_DEBUG_PRINT( DBG_MSG_DEBUG, "conn_id:%d, trans_id:%d, handle: %d, offset:%d, need_rsp:%d, is_prep:%d, len:%d\n",
					 esp_param->write.conn_id, esp_param->write.trans_id, esp_param->write.handle,
					 esp_param->write.offset, esp_param->write.need_rsp, esp_param->write.is_prep,
					 esp_param->write.len );
	///< Check offset value validation
	if ( esp_param->write.offset > BLE_ADV_BUF_MAX_SIZE )
	{
		BLE_DEBUG_PRINT( DBG_MSG_INFO, "Write_Event: ESP_GATT_INVALID_OFFSET\n" );
		esp_gatt_status = ESP_GATT_INVALID_OFFSET;
	}
	else if ( ( esp_param->write.offset + esp_param->write.len ) > BLE_ADV_BUF_MAX_SIZE )
	{
		BLE_DEBUG_PRINT( DBG_MSG_INFO, "Write_Event: ESP_GATT_INVALID_ATTR_LEN\n" );
		esp_gatt_status = ESP_GATT_INVALID_ATTR_LEN;
	}

	if ( esp_param->write.need_rsp )
	{
		if ( esp_param->write.is_prep )
		{
			/// TODO: Fix this Implement this case from gatts_demo.c example
			rsp.attr_value.len = esp_param->write.len;
			rsp.attr_value.handle = esp_param->write.handle;
			rsp.attr_value.offset = esp_param->write.offset;
			rsp.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
			memcpy( rsp.attr_value.value, esp_param->write.value, esp_param->write.len );
			if ( esp_gatt_status != ESP_GATT_OK )
			{
				///< Clear the data buffer
				memset( dest_data_read, 0, BLE_ADV_BUF_MAX_SIZE );
				///< Copy the data from ESP BLE to data buffer
				memcpy( dest_data_read + esp_param->write.offset, esp_param->write.value,
						esp_param->write.len );
				///< Update the full_length with ESP param length
				full_length += esp_param->write.len;
			}

			///< Sent prepared GATT remote Read Request Response
			response_err = esp_ble_gatts_send_response( esp_gatts_if, esp_param->write.conn_id,
														esp_param->write.trans_id,
														esp_gatt_status, &rsp );
			if ( response_err != ESP_OK )
			{
				BLE_DEBUG_PRINT( DBG_MSG_ERROR, "Write_Event: Send response error\n" );
			}
		}
		else
		{
			///< Write data into DCI data.
			///< Clear the data buffer
			memset( dest_data_read, 0, BLE_ADV_BUF_MAX_SIZE );
			///< Copy the data from ESP BLE to data buffer
			memcpy( dest_data_read + esp_param->write.offset, esp_param->write.value,
					esp_param->write.len );

			///< Get the DCID from CHAR_ID
			dcid = m_ble_dci->Find_Ble_Mod_DCID( ble_unique_id, &read_access, &write_access );
			BLE_DEBUG_PRINT( DBG_MSG_INFO, "Active access level %d\r\nWrite access level %d\r\n", active_access_level,
							 write_access );
			if ( dcid == DCI_ID_UNDEFINED )
			{
				BLE_DEBUG_PRINT( DBG_MSG_ERROR, "Write_Event : Find_Ble_Mod_DCID : Error Undefined dcid %d\n", dcid );
				esp_gatt_status = ESP_GATT_NOT_FOUND;
			}
			else
			{
				BLE_DEBUG_PRINT( DBG_MSG_INFO, "Write_Event : Find_Ble_Mod_DCID : dcid %d\n", dcid );
				if ( ( ( write_access != NO_ACCESS ) && ( write_access != ACCESS_INVALID ) ) &&
					 ( active_access_level >= write_access ) )
				{
					///< Get the DCI DATA length
					full_length = m_ble_dci->Get_Length( dcid );

					///< Write buffer data to DCI using Set_Dcid_Data api
					dci_error = m_ble_dci->Set_Dcid_Data( dcid, full_length, dest_data_read );
					if ( dci_error != DCI_ERR_NO_ERROR )
					{
						BLE_DEBUG_PRINT( DBG_MSG_ERROR, "Write_Event : Error while Set_Dcid_Data %d\n",
										 dci_error );
						esp_gatt_status = Interpret_Ble_Error( m_ble_dci->Interpret_DCI_Error( dci_error ) );
					}
					full_length = 0;
				}
				else
				{
					BLE_DEBUG_PRINT( DBG_MSG_INFO, "Write Access Not Permitted\n" );
				}

				esp_ble_gatts_send_response( esp_gatts_if, esp_param->write.conn_id,
											 esp_param->write.trans_id, esp_gatt_status, NULL );
				if ( response_err != ESP_OK )
				{
					BLE_DEBUG_PRINT( DBG_MSG_ERROR, "Write_Event: Send response error\n" );
				}
			}
		}
	}
}

/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Ble_Api::Authentication_Complete( void* param1 )
{
	esp_bd_addr_t bd_addr;
	esp_ble_gap_cb_param_t* param = ( esp_ble_gap_cb_param_t* )( param1 );

	memcpy( bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof( esp_bd_addr_t ) );
	BLE_DEBUG_PRINT( DBG_MSG_INFO, "remote BD_ADDR: %08x%04x,address type = %d,pair status = %s\n", \
					 ( bd_addr[0] << 24 ) + ( bd_addr[1] << 16 ) + ( bd_addr[2] << 8 ) + bd_addr[3],
					 ( bd_addr[4] << 8 ) + bd_addr[5], param->ble_security.auth_cmpl.addr_type,
					 param->ble_security.auth_cmpl.success ? "success" : "fail" );

	if ( !param->ble_security.auth_cmpl.success )
	{
		BLE_DEBUG_PRINT( DBG_MSG_INFO, "fail reason = 0x%x", param->ble_security.auth_cmpl.fail_reason );
		ble_state = BLE_ATTENTION;
	}
	else
	{
		BLE_DEBUG_PRINT( DBG_MSG_INFO, "auth mode = %s",
						 Esp_Auth_Req_To_Str( param->ble_security.auth_cmpl.auth_mode ) );
	}
	ble_state = BLE_PAIRING;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
char* Esp32_Ble_Api::Esp_Auth_Req_To_Str( esp_ble_auth_req_t auth_req )
{
	char* auth_str = NULL;

	switch ( auth_req )
	{
		case ESP_LE_AUTH_NO_BOND:
			auth_str = ( char* ) "ESP_LE_AUTH_NO_BOND";
			break;

		case ESP_LE_AUTH_BOND:
			auth_str = ( char* ) "ESP_LE_AUTH_BOND";
			break;

		case ESP_LE_AUTH_REQ_MITM:
			auth_str = ( char* ) "ESP_LE_AUTH_REQ_MITM";
			break;

		case ESP_LE_AUTH_REQ_BOND_MITM:
			auth_str = ( char* ) "ESP_LE_AUTH_REQ_BOND_MITM";
			break;

		case ESP_LE_AUTH_REQ_SC_ONLY:
			auth_str = ( char* ) "ESP_LE_AUTH_REQ_SC_ONLY";
			break;

		case ESP_LE_AUTH_REQ_SC_BOND:
			auth_str = ( char* ) "ESP_LE_AUTH_REQ_SC_BOND";
			break;

		case ESP_LE_AUTH_REQ_SC_MITM:
			auth_str = ( char* ) "ESP_LE_AUTH_REQ_SC_MITM";
			break;

		case ESP_LE_AUTH_REQ_SC_MITM_BOND:
			auth_str = ( char* ) "ESP_LE_AUTH_REQ_SC_MITM_BOND";
			break;

		default:
			auth_str = ( char* ) "INVALID BLE AUTH REQ";
			break;
	}
	return ( auth_str );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Esp32_Ble_Api::Find_Service_Id_From_Gatt_If(
	esp_gatt_if_t gatts_if )
{
	uint16_t serv_index = 0;

	for ( serv_index = 0; serv_index < TOTAL_SERVICE; serv_index++ )
	{
		if ( gatts_profile_tab[serv_index].gatts_if == gatts_if )
		{
			return ( serv_index );
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_gatt_status_t Esp32_Ble_Api::Interpret_Ble_Error( uint8_t ble_error )
{
	esp_gatt_status_t return_esp_gatt_status;

	switch ( ble_error )
	{
		case BLE_NO_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_OK;
			break;

		case BLE_EXCEEDS_RANGE_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_OUT_OF_RANGE;
			break;

		case BLE_OTHER_DCID_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_UNKNOWN_ERROR;
			break;

		case BLE_INVALID_DATA_LENGTH_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_INVALID_ATTR_LEN;
			break;

		case BLE_INVALID_COMMAND_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_ILLEGAL_PARAMETER;
			break;

		case BLE_RAM_READ_ONLY_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_WRITE_NOT_PERMIT;
			break;

		case BLE_NV_READ_ONLY_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_WRITE_NOT_PERMIT;
			break;

		case BLE_NV_ACCESS_FAILURE_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_BUSY;
			break;

		case BLE_ACCESS_VIOLATION_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_ILLEGAL_PARAMETER;
			break;

		case BLE_NV_NOT_AVAILABLE_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_NOT_FOUND;
			break;

		case BLE_DEFAULT_NOT_AVAILABLE_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_NOT_FOUND;
			break;

		case BLE_RANGE_NOT_AVAILABLE_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_OUT_OF_RANGE;
			break;

		case BLE_ENUM_NOT_AVAILABLE_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_NOT_FOUND;
			break;

		case BLE_VALUE_LOCKED_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_AUTH_FAIL;
			break;

		case BLE_TOTAL_ERRORS_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_UNKNOWN_ERROR;
			break;

		case BLE_INVALID_DATA_ID_ERROR_CODE:
			return_esp_gatt_status = ESP_GATT_ILLEGAL_PARAMETER;
			break;

		default:
			return_esp_gatt_status = ESP_GATT_UNKNOWN_ERROR;
			break;
	}

	return ( return_esp_gatt_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_Api::Ble_Led_Display_Control( void )
{

	if ( m_ble_display != nullptr )
	{
		switch ( ble_state )
		{
			case BLE_IDLE_OR_DISCONNECTED:
				m_ble_display->Device_Disconnected();
				break;

			case BLE_PAIRING:
				m_ble_display->Pairing_Mode();
				break;

			case BLE_CONNECTED:
				m_ble_display->Device_Connected();
				break;

			case BLE_POLLING:
				m_ble_display->Data_Operations();
				break;

			case BLE_FAULT:
				m_ble_display->Device_Faulted();
				break;

			case BLE_ATTENTION:
				m_ble_display->Attention();
				break;
		}
	}
}

#endif	// #ifdef CONFIG_BT_ENABLED
