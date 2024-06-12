/***********************************************************************************************//**
 * \file   app.c
 * \brief  Event handling and application code for Empty NCP Host application example
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* standard library headers */
#include <string.h>

/* BG stack headers */
#include "bg_types.h"
#include "gecko_bglib.h"
#include "Ble_UART.h"
#include "Ble_DCI.h"
#include "Ble_Advertisement.h"
#include "Ble_User_Management.h"
#include "BLE_DCI_Data.h"
#include "BLE_Debug.h"

/***************************************************************************************************
 Local Variables
 **************************************************************************************************/
static uint8_t _conn_handle = 0xFF;

/***************************************************************************************************
 Global Variables
 **************************************************************************************************/
uint8 ble_state = BLE_IDLE_OR_DISCONNECTED;
extern int32_t DCID_passkey;
extern uint16_t DCID_min_conn_interval,DCID_max_conn_interval,DCID_slave_latency,
				DCID_supervision_timeout;

extern Ble_DCI* Get_Ble_Dci_Obj(void);
extern Ble_User_Management* Get_Ble_User_Management_Obj(void);
extern Ble_Advertisement* Get_Ble_Advertisement_Obj(void);

/***************************************************************************************************
 Constants
 **************************************************************************************************/
const uint16_t DEVICE_ALREADY_BONDED = 1U;
const uint8_t DEVICE_NOT_BONDED = 0xFF;
const uint8_t DEVICE_BONDED_LSB = 1U;
const uint8_t DEVICE_BONDED_MSB = 0U;
const uint8_t DEVICE_CLEAR_BONDING_LSB = 0U;
const uint8_t DEVICE_CLEAR_BONDING_MSB = 0U;

/***************************************************************************************************
 * Public Function Definitions
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Reset variable function.
 *  \param[in] None.
 **************************************************************************************************/
static void reset_variables(void)
{
	_conn_handle = 0xFF;
}

/***********************************************************************************************//**
 *  \brief  Event handler function.
 *  \param[in] evt Event pointer.
 **************************************************************************************************/
void appHandleEvents(struct gecko_cmd_packet *evt)
{
   
	const uint8_t ACCESS_INVALID = 0xFF;
	const uint8_t NO_ACCESS = 0;
	DCI_ERROR_TD status = DCI_ERR_NO_ERROR;
	uint8_t tempvalue[2],bleerrstatus;
	uint8_t adv_data_length,active_access_level = 0,read_access = ACCESS_INVALID,
			write_access = ACCESS_INVALID;
        //uint8_t loop_var, access_length;
	
	static uint8_t readvalue[256],adv_data[31];
	uint16_t dcid = 0,length = 0,tempreadvalue = 0,tempreadmsbvalue = 0;
	memset(adv_data, 0, sizeof(adv_data));
	memset(readvalue, 0, sizeof(readvalue));
	
	static Ble_DCI* ble_dci_ref;	
    static Ble_User_Management* ble_usr_mgmt_ref;
    static Ble_Advertisement* ble_adv_ref;
    
    // Getting the ble objects from prod spec file.
    ble_dci_ref = Get_Ble_Dci_Obj();
    ble_usr_mgmt_ref = Get_Ble_User_Management_Obj();
    ble_adv_ref = Get_Ble_Advertisement_Obj();
    
	//Fetching the current access level of the user
	ble_usr_mgmt_ref->m_ble_active_access_level_owner->Check_Out(active_access_level);
	
	/* Handle events */
	switch (BGLIB_MSG_ID(evt->header))
	{
		/* This boot event is generated when the system boots up after reset.
		 * Here the system is set to start advertising immediately after boot procedure. */
		case gecko_evt_system_boot_id:
			reset_variables();
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Configuring Security manager in Progress\r\n");
			gecko_cmd_sm_configure(3, sm_io_capability_displayonly); /* Numeric comparison */
			break;
			
	    case gecko_rsp_le_gap_set_adv_data_id:
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Setting the Advertisement data success\r\n");
			break;
			
		case gecko_rsp_sm_configure_id:
			if(evt->data.rsp_sm_configure.result == bg_err_success)
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Successfully security manager configured\r\n");
				gecko_cmd_sm_set_bondable_mode(1);
			}
			break;
			
		case gecko_rsp_sm_set_bondable_mode_id:
			if(evt->data.rsp_sm_set_bondable_mode.result == bg_err_success)
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Bondable configured\r\n");
				/* Start general advertising and enable connections. */
				gecko_cmd_le_gap_set_mode(le_gap_user_data,
					le_gap_undirected_connectable);
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Advertising and Discoverable \n");
			}
			break;
			
		case gecko_cmd_le_gap_set_mode_id:
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Setting the Passkey \r\n");
			gecko_cmd_sm_set_passkey(DCID_passkey);
			break;
			
		case gecko_evt_sm_passkey_display_id:
			ble_state = BLE_PAIRING;
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Passkey_displayed \r\n");
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Passkey: %d\n", (int)evt->data.evt_sm_passkey_display.passkey);
			break;
			
		case gecko_rsp_sm_set_passkey_id:
			if(evt->data.rsp_sm_set_passkey.result ==  bg_err_success)
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Setting the Passkey success \r\n");
			}
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Setting user data in the advertisement packet\r\n");
			adv_data_length = ble_adv_ref-> Get_Full_Packet(adv_data);
			gecko_cmd_le_gap_set_adv_data(0, adv_data_length, adv_data);
			break;
			
		/* Connection opened event */
		case gecko_evt_le_connection_opened_id:
			ble_state = BLE_PAIRING;
			dcid = ble_dci_ref->Find_Ble_Mod_DCID( DCI_DEVICE_BONDING_INFO_BLE_CHAR_ID,&read_access,&write_access);
			if ( dcid != DCI_ID_UNDEFINED )
			{
				status = ble_dci_ref->Get_Dcid_Data(dcid,
					reinterpret_cast<DCI_LENGTH_TD*>(&length),readvalue);
			}
			else
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "DCID is invalid\n");
				bleerrstatus = BLE_INVALID_DATA_ID_ERROR_CODE;
			}
			
			if( status == DCI_ERR_NO_ERROR)
			{
				tempreadvalue = readvalue[0];
				tempreadmsbvalue = readvalue[1];
				tempreadvalue |= tempreadmsbvalue << 8;
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Bonding Read success @le_connection : %d\n",tempreadvalue);
			}
			else
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Bonding Read Error\n");
			}
			
			if(tempreadvalue != DEVICE_ALREADY_BONDED)
			{
				tempvalue[0] = DEVICE_CLEAR_BONDING_LSB;
				tempvalue[1] = DEVICE_CLEAR_BONDING_MSB;
				if ( dcid != DCI_ID_UNDEFINED )
				{
					status = ble_dci_ref->Set_Dcid_Data(dcid,2,
										reinterpret_cast<uint8_t*>(tempvalue));
					bleerrstatus = ble_dci_ref->Interpret_DCI_Error(status);
				}
				else
				{
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "DCID is invalid\n");
					bleerrstatus = BLE_INVALID_DATA_ID_ERROR_CODE;
				}
				
				if(bleerrstatus == BLE_NO_ERROR_CODE)
				{
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Bonding info stored @le_connection\n");
				}
				else
				{
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Bonding info not stored @le_connection\n");
				}				
			}
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "<-- Received event:\n\tgecko_evt_le_connection_opened(%02X:%02X:%02X:%02X:%02X:%02X, %d, %d, %d, 0x%02X)\n",
											evt->data.evt_le_connection_opened.address.addr[5],
											evt->data.evt_le_connection_opened.address.addr[4],
											evt->data.evt_le_connection_opened.address.addr[3],
											evt->data.evt_le_connection_opened.address.addr[2],
											evt->data.evt_le_connection_opened.address.addr[1],
											evt->data.evt_le_connection_opened.address.addr[0],
											evt->data.evt_le_connection_opened.address_type,
											evt->data.evt_le_connection_opened.master,
											evt->data.evt_le_connection_opened.connection,
											evt->data.evt_le_connection_opened.bonding
											);
			_conn_handle = evt->data.evt_le_connection_opened.connection;
			
			if(evt->data.evt_le_connection_opened.bonding == DEVICE_NOT_BONDED)
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Device is not yet bonded");
			}
			else
			{
				tempvalue[0] = DEVICE_BONDED_LSB;
				tempvalue[1] = DEVICE_BONDED_MSB;
                if ( dcid != DCI_ID_UNDEFINED )
                {
					status = ble_dci_ref->Set_Dcid_Data(dcid,2,
										  reinterpret_cast<uint8_t*>(tempvalue));
					bleerrstatus = ble_dci_ref->Interpret_DCI_Error(status);
                }
				else
                {
                	BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "DCID is invalid\n");
                                    bleerrstatus = BLE_INVALID_DATA_ID_ERROR_CODE;
                }
				
				if(bleerrstatus == BLE_NO_ERROR_CODE)
				{
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Bonding info stored @sm_bonded\n");
				}
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Device is already Bonded \n");
			}
			gecko_cmd_sm_increase_security(evt->data.evt_le_connection_opened.connection);
			break;
			
		case gecko_rsp_sm_delete_bonding_id:
			ble_state = BLE_FAULT;
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Last bonding deleted for the device \n");
			break;
			
		case gecko_rsp_sm_delete_bondings_id:
			ble_state = BLE_FAULT;
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Previous Bondings deleted for the device \n");
			break;
			
		case gecko_rsp_sm_increase_security_id:
			ble_state = BLE_PAIRING;
			if(evt->data.rsp_sm_increase_security.result ==  bg_err_success)
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "security increaded \n");
				/* Set connection parameters. 
				 * The first parameter is the connection handle of the active connection
				 * The next two parameters are minimum and maximum connection event interval,both in
				 * units of (value * 1.25 milliseconds).
				 * The fourth parameter '4' sets the latency, This parameter defines 
				 * how many connection intervals the slave can skip if it has no data to send 
				 * The fifth parameter is Supervision timeout Supervision timeout.this defines for 
				 * how long the connection is maintained despite the devices being unable to
				 * communicate at the currently configured connection intervals.
				 * uints of (value * 10 milliseconds)
				 */
				gecko_cmd_le_connection_set_parameters(_conn_handle, DCID_min_conn_interval,
				DCID_max_conn_interval, DCID_slave_latency, DCID_supervision_timeout);
			}
			break;
			
		case gecko_evt_le_connection_closed_id:
			ble_state = BLE_IDLE_OR_DISCONNECTED;
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Disconnected \n");
			reset_variables();
			gecko_cmd_le_gap_set_mode(le_gap_user_data, le_gap_undirected_connectable);
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Reinitializing... \n");
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Advertising and Discoverable \n");
			break;
			
		case gecko_evt_sm_passkey_request_id:
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Passkey request\r\n");
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Enter the passkey you see on the tablet\r\n");
			break;
			
		case gecko_evt_sm_confirm_passkey_id:
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Confirm passkey\r\n");
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Are you see the same passkey on the tablet: %d (y/n)?\r\n",
					  evt->data.evt_sm_confirm_passkey.passkey);
			break;
			
		case gecko_evt_sm_confirm_bonding_id:
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "gecko_evt_sm_confirm_bonding_id\r\n");
			break;
			
		case gecko_evt_sm_bonded_id:
			ble_state = BLE_CONNECTED;
			/* The bonding/pairing was successful*/
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "sm bonded\n");
			tempvalue[0] = DEVICE_BONDED_LSB;
			tempvalue[1] = DEVICE_BONDED_MSB;
			dcid = ble_dci_ref->Find_Ble_Mod_DCID( DCI_DEVICE_BONDING_INFO_BLE_CHAR_ID,&read_access,&write_access);
			if ( dcid != DCI_ID_UNDEFINED )
			{
				status = ble_dci_ref->Set_Dcid_Data(dcid,2,
						reinterpret_cast<uint8_t*>(tempvalue));
				bleerrstatus = ble_dci_ref->Interpret_DCI_Error(status);
			}
			else
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "DCID is invalid\n");
				bleerrstatus = BLE_INVALID_DATA_ID_ERROR_CODE;
			}
			
            if(bleerrstatus == BLE_NO_ERROR_CODE)
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Bonding info stored @sm_bonded \n");
			}
			break;
			
		case gecko_rsp_le_connection_set_parameters_id:
			if(evt->data.rsp_le_gap_set_conn_parameters.result ==  bg_err_success)
			{
				//Do nothing just to wait for the event #gecko_evt_le_connection_parameters_id
				//this case is added
			}
			break;
				  
		case gecko_evt_le_connection_parameters_id:
			ble_state = BLE_POLLING;
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Conn.parameters: connection_handle %u ,interval %u , latency %u , timeout %u , security_mode %u , txsize %u\r\n",
			evt->data.evt_le_connection_parameters.connection, 
			evt->data.evt_le_connection_parameters.interval, 
			evt->data.evt_le_connection_parameters.latency, 
			evt->data.evt_le_connection_parameters.timeout, 
			evt->data.evt_le_connection_parameters.security_mode, 
			evt->data.evt_le_connection_parameters.txsize
			);
			break;
			
		case gecko_evt_sm_bonding_failed_id:
			ble_state = BLE_FAULT;
			dcid = ble_dci_ref->Find_Ble_Mod_DCID( DCI_DEVICE_BONDING_INFO_BLE_CHAR_ID,&read_access,&write_access);
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Bonding Error : 0x%X\r\n",evt->data.evt_sm_bonding_failed.reason);
			tempvalue[0] = (evt->data.evt_sm_bonding_failed.reason & 0xFF);
			tempvalue[1] = (evt->data.evt_sm_bonding_failed.reason & 0xFF00) >> 8;
            if ( dcid != DCI_ID_UNDEFINED )
            {
				status = ble_dci_ref->Set_Dcid_Data(dcid,2,
									  reinterpret_cast<uint8_t*>(tempvalue));
				bleerrstatus = ble_dci_ref->Interpret_DCI_Error(status);
            }
			else
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "DCID is invalid\n");
				bleerrstatus = BLE_INVALID_DATA_ID_ERROR_CODE;
			}
			
			if(bleerrstatus == BLE_NO_ERROR_CODE)
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "bonding_failed info stored\n");
			}
			else
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Error in storing bonding info\n");
			}
			
			switch(evt->data.evt_sm_bonding_failed.reason)
			{
				case bg_err_smp_passkey_entry_failed:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "The user input of passkey failed\r\n");
					break;
					
				case bg_err_smp_oob_not_available:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Out of Band data is not available for authentication\r\n");
					break;
					
				case bg_err_smp_authentication_requirements:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "The pairing procedure cannot be performed as authentication \
						requirements cannot be met due to IO capabilities of one or both devices\r\n");
					break;
					
				case bg_err_smp_confirm_value_failed:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "The confirm value does not match the calculated compare value\r\n");
					break;
					
				case bg_err_smp_pairing_not_supported:
					gecko_cmd_sm_delete_bondings();
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Pairing is not supported bonding already exist for this device\r\n");
					break;
					
				case bg_err_smp_encryption_key_size:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "The resultant encryption key size is insufficient for the security \
							   requirements of this device\r\n");
					break;
					
				case bg_err_smp_command_not_supported:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "The SMP command received is not supported on this device\r\n");
					break;
					
				case bg_err_smp_unspecified_reason:
					gecko_cmd_sm_delete_bondings();
					break;
					
				case bg_err_smp_repeated_attempts:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Pairing or authentication procedure is disallowed because too little  \
							  time has elapsed since last pairing request or security request\r\n");
					break;
					
				case bg_err_smp_invalid_parameters:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Invalid Parameters\r\n");
					break;
					
				case bg_err_smp_dhkey_check_failed:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "The bonding does not exist\r\n");
					break;
					
				case bg_err_bt_pin_or_key_missing:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Pairing failed because of missing PIN, or authentication 	\
							  failed because of missing Key\r\n");
					break;
					
				case bg_err_timeout:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Command or Procedure failed due to timeout\r\n");
					break;
					
				default:
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Unknown error: 0x%X\r\n",evt->data.evt_sm_bonding_failed.reason);
					break;
			}
			break;
			
		case gecko_evt_gatt_server_user_write_request_id:
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Write Request for %d \n",
					  evt->data.evt_gatt_server_user_write_request.characteristic);
			ble_state = BLE_POLLING;
			dcid = ble_dci_ref->Find_Ble_Mod_DCID( evt->data.evt_gatt_server_user_write_request.characteristic,&read_access,&write_access);

			if ( dcid != DCI_ID_UNDEFINED )
			{
				if(((write_access!= NO_ACCESS) && (write_access != ACCESS_INVALID)) && (active_access_level >= write_access))
				{
					status = ble_dci_ref->Set_Dcid_Data(dcid,
								evt->data.evt_gatt_server_user_write_request.value.len,
								reinterpret_cast<uint8_t*>(evt->data.evt_gatt_server_user_write_request.value.data));
					bleerrstatus = ble_dci_ref->Interpret_DCI_Error(status);
				}
				else
				{
					BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Write Access Not Permitted\n");
					bleerrstatus = BLE_ACCESS_VIOLATION_ERROR_CODE;
				}
			}
			else
			{
				BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "DCID is invalid\n");
				bleerrstatus = BLE_INVALID_DATA_ID_ERROR_CODE;
			}
			gecko_cmd_gatt_server_send_user_write_response(
				evt->data.evt_gatt_server_user_write_request.connection,
				evt->data.evt_gatt_server_user_write_request.characteristic,
				bleerrstatus);
			
			break;
			
		case gecko_evt_gatt_server_user_read_request_id:
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Read Request for %d \n",
					  evt->data.evt_gatt_server_user_read_request.characteristic);
			ble_state = BLE_POLLING;
			status = ble_dci_ref->Group_Parameters_Assembly(evt->data.evt_gatt_server_user_read_request.characteristic, 
				  reinterpret_cast<DCI_LENGTH_TD*>(&length),readvalue,active_access_level);
			
			bleerrstatus = ble_dci_ref->Interpret_DCI_Error(status);

			gecko_cmd_gatt_server_send_user_read_response(
				evt->data.evt_gatt_server_user_read_request.connection,
				evt->data.evt_gatt_server_user_read_request.characteristic,
				bleerrstatus,length,readvalue);
			break;
			
		case gecko_rsp_gatt_server_send_user_read_response_id:
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Read Response \n");
			ble_state = BLE_CONNECTED;
			break;
			
		case gecko_rsp_gatt_server_send_user_write_response_id:
			BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Write Response \n");
			ble_state = BLE_CONNECTED;
			break;
	  		
		default:
			//BLE_Print("Default break : 0x%04x\n", BGLIB_MSG_ID(evt->header));
			break;
	}
}
