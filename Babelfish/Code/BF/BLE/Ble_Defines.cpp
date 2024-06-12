/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Ble_UART.h"
#include "Ble_Defines.h"
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// *******************************************************
// ******     The BLE User Management Structure and variables.
// *******************************************************

const DCI_ID_TD ble_user_entry_dcid_list[2] =
{
	DCI_BLE_USERNAME_DCID,
	DCI_BLE_PASSWORD_DCID,
};

const DCI_ID_TD ble_role_based_dcid_list[4] =
{
	DCI_BLE_OBSERVER_PASSWORD_DCID,
	DCI_BLE_OPERATOR_PASSWORD_DCID,
	DCI_BLE_ENGINEER_PASSWORD_DCID,
	DCI_BLE_ADMIN_PASSWORD_DCID,
};

const DCI_ID_TD ble_user_name_dcid_list[4] =
{
	DCI_BLE_USER1_NAME_DCID,
	DCI_BLE_USER2_NAME_DCID,
	DCI_BLE_USER3_NAME_DCID,
	DCI_BLE_USER4_NAME_DCID,
};

const DCI_ID_TD ble_user_password_dcid_list[4] =
{
	DCI_BLE_USER1_PASSWORD_DCID,
	DCI_BLE_USER2_PASSWORD_DCID,
	DCI_BLE_USER3_PASSWORD_DCID,
	DCI_BLE_USER4_PASSWORD_DCID,
};

const DCI_ID_TD ble_user_role_dcid_list[4] =
{
	DCI_BLE_USER1_ROLE_DCID,
	DCI_BLE_USER2_ROLE_DCID,
	DCI_BLE_USER3_ROLE_DCID,
	DCI_BLE_USER4_ROLE_DCID,
};


const Ble_User_Management::ble_user_management_t user_entry =
{
	2U,
	ble_user_entry_dcid_list,
};


const Ble_User_Management::ble_user_management_t role =
{
	4U,
	ble_role_based_dcid_list,
};

const Ble_User_Management::ble_user_management_t user_name =
{
	4U,
	ble_user_name_dcid_list,
};

const Ble_User_Management::ble_user_management_t user_pwd =
{
	4U,
	ble_user_password_dcid_list,
};

const Ble_User_Management::ble_user_management_t user_role =
{
	4U,
	ble_user_role_dcid_list,
};




