/**
 *****************************************************************************************
 *	@file
 *
 *	@brief BLE buffer interface.
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BLE_DEFINES_H
#define BLE_DEFINES_H

#include "Includes.h"
#include "Ble_User_Management.h"
///< Include ESP32 Stack Files
#ifdef ESP32_SETUP
#include "bt_target.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "Ble_Config_ESP32.h"
#define GATTS_TAG "SEC_GATTS_DEMO"
#endif
#ifdef STM32WB55_WPAN
#include "ble_defs.h"
#endif

// ***********************************************************
/// **** BLE Parity Codes ( parity and stop bit combos )
// ***********************************************************
enum ble_parity_stop_t
{
	BLE_PARITY_EVEN_1_STOP_BIT,	// 0
	BLE_PARITY_ODD_1_STOP_BIT,	// 1
	BLE_PARITY_NONE_2_STOP_BIT,	// 2
	BLE_PARITY_EVEN_2_STOP_BIT,	// 3
	BLE_PARITY_ODD_2_STOP_BIT,	// 4
	BLE_PARITY_NONE_1_STOP_BIT	// 5
};

// ***********************************************************
/// **** BLE Parity Modes
// ***********************************************************
enum ble_parity_t
{
	BLE_PARITY_EVEN,// 0
	BLE_PARITY_ODD,	// 1
	BLE_PARITY_NONE	// 2
};

// ***********************************************************
/// **** BLE Stop Bits
// ***********************************************************
enum ble_stop_bits_t
{
	BLE_1_STOP_BIT,	// 0
	BLE_2_STOP_BIT	// 1
};

// ***********************************************************
/// **** BLE Connection States
// ***********************************************************
enum ble_connection_states_t
{
	BLE_IDLE_OR_DISCONNECTED,	// 0
	BLE_PAIRING,				// 1
	BLE_CONNECTED,				// 2
	BLE_POLLING,				// 3
	BLE_FAULT,					// 4
	BLE_ATTENTION				// 5
};

// ***********************************************************
/// **** BLE Baud Rates
// ***********************************************************
static const uint32_t BLE_BAUD_19200 = 19200U;
static const uint32_t BLE_BAUD_9600 = 9600U;
static const uint32_t BLE_BAUD_38400 = 38400U;
static const uint32_t BLE_BAUD_57600 = 57600U;
static const uint32_t BLE_BAUD_115200 = 115200U;
static const uint32_t BLE_BAUD_230400 = 230400U;
static const uint32_t BLE_BAUD_460800 = 460800U;

/// ***********************************************************
/// **** Ble Error Codes
// ***********************************************************
static const uint8_t BLE_NO_ERROR_CODE = 0U;
static const uint8_t BLE_EXCEEDS_RANGE_ERROR_CODE = 128U;
static const uint8_t BLE_OTHER_DCID_ERROR_CODE = 129U;
static const uint8_t BLE_INVALID_DATA_LENGTH_ERROR_CODE = 130U;
static const uint8_t BLE_INVALID_COMMAND_ERROR_CODE = 131U;
static const uint8_t BLE_RAM_READ_ONLY_ERROR_CODE = 132U;
static const uint8_t BLE_NV_READ_ONLY_ERROR_CODE = 133U;
static const uint8_t BLE_NV_ACCESS_FAILURE_ERROR_CODE = 134U;
static const uint8_t BLE_ACCESS_VIOLATION_ERROR_CODE = 135U;
static const uint8_t BLE_NV_NOT_AVAILABLE_ERROR_CODE = 136U;
static const uint8_t BLE_DEFAULT_NOT_AVAILABLE_ERROR_CODE = 137U;
static const uint8_t BLE_RANGE_NOT_AVAILABLE_ERROR_CODE = 138U;
static const uint8_t BLE_ENUM_NOT_AVAILABLE_ERROR_CODE = 139U;
static const uint8_t BLE_VALUE_LOCKED_ERROR_CODE = 140U;
static const uint8_t BLE_TOTAL_ERRORS_ERROR_CODE = 141U;
static const uint8_t BLE_INVALID_DATA_ID_ERROR_CODE = 142U;

// *******************************************************
// ******     BLE User Management structure and variables
// *******************************************************
extern const DCI_ID_TD ble_user_entry_dcid_list[2];
extern const DCI_ID_TD ble_role_based_dcid_list[4];
extern const DCI_ID_TD ble_user_name_dcid_list[4];
extern const DCI_ID_TD ble_user_password_dcid_list[4];
extern const DCI_ID_TD ble_user_role_dcid_list[4];

extern const Ble_User_Management::ble_user_management_t role;
extern const Ble_User_Management::ble_user_management_t user_entry;
extern const Ble_User_Management::ble_user_management_t user_name;
extern const Ble_User_Management::ble_user_management_t user_pwd;
extern const Ble_User_Management::ble_user_management_t user_role;


#ifdef STM32WB55_WPAN
/* Characteristic properties as per ble_defs.h from STM32WB55 WPAN Ble stack.
 */
static const uint8_t BLE_CHAR_PROP_NONE = CHAR_PROP_NONE;
static const uint8_t BLE_CHAR_PROP_BROADCAST = CHAR_PROP_BROADCAST;
static const uint8_t BLE_CHAR_PROP_READ = CHAR_PROP_READ;
static const uint8_t BLE_CHAR_PROP_WRITE_WITHOUT_RESP = CHAR_PROP_WRITE_WITHOUT_RESP;
static const uint8_t BLE_CHAR_PROP_WRITE = CHAR_PROP_WRITE;
static const uint8_t BLE_CHAR_PROP_NOTIFY = CHAR_PROP_NOTIFY;
static const uint8_t BLE_CHAR_PROP_INDICATE = CHAR_PROP_INDICATE;
static const uint8_t BLE_CHAR_PROP_SIGNED_WRITE = CHAR_PROP_SIGNED_WRITE;
static const uint8_t BLE_CHAR_PROP_EXT = CHAR_PROP_EXT;


static const bool BLE_EVENT_SUCCESS = false;
static const bool BLE_EVENT_ERROR = true;
static const uint8_t ENC_KEY_SIZE = 16U;
static const uint8_t VARIABLE = 1U;

typedef uint8_t APP_BLE_ConnStatus_t;



static const APP_BLE_ConnStatus_t APP_BLE_IDLE = 0U;
static const APP_BLE_ConnStatus_t APP_BLE_FAST_ADV = 1U;
static const APP_BLE_ConnStatus_t APP_BLE_LP_ADV = 2U;
static const APP_BLE_ConnStatus_t APP_BLE_SCAN = 3U;
static const APP_BLE_ConnStatus_t APP_BLE_LP_CONNECTING = 4U;
static const APP_BLE_ConnStatus_t APP_BLE_CONNECTED_SERVER = 5U;
static const APP_BLE_ConnStatus_t APP_BLE_CONNECTED_CLIENT = 6U;

#endif

#endif
