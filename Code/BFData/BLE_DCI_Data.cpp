/**
 *****************************************************************************************
 *   @file
 *   @details See header file for module overview.
 *   @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "BLE_DCI_Data.h"
#include "Ble_Defines.h"


// *******************************************************
// ******     BLE DCI_ID List
// *******************************************************

const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_SNAME_BLE_CHAR_ID = 28U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_VENDOR_BLE_CHAR_ID = 42U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_FAMILY_BLE_CHAR_ID = 44U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_ROLE_BLE_CHAR_ID = 46U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_MODEL_BLE_CHAR_ID = 48U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_LNAME_BLE_CHAR_ID = 50U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_HW_VER_BLE_CHAR_ID = 52U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_SW_VER_BLE_CHAR_ID = 54U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_BLE_CHAR_ID = 56U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_MANUFACTURER_NAME_BLE_CHAR_ID = 58U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_SERIAL_NO_BLE_CHAR_ID = 60U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_IMAGE_URL_BLE_CHAR_ID = 62U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_MIN_VAL_CONN_INTERVAL_BLE_CHAR_ID = 64U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_MAX_VAL_CONN_INTERVAL_BLE_CHAR_ID = 66U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_SLAVE_LATENCY_BLE_CHAR_ID = 68U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_SUPERVISION_TIMEOUT_BLE_CHAR_ID = 70U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_ADMIN_PASSWORD_BLE_CHAR_ID = 72U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_OBSERVER_PASSWORD_BLE_CHAR_ID = 74U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_OPERATOR_PASSWORD_BLE_CHAR_ID = 76U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_ENGINEER_PASSWORD_BLE_CHAR_ID = 78U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USERNAME_BLE_CHAR_ID = 80U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_PASSWORD_BLE_CHAR_ID = 82U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_EDDYSTONE_URL_BLE_CHAR_ID = 84U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_IBEACON_MAJOR_NUMBER_BLE_CHAR_ID = 86U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_IBEACON_MINOR_NUMBER_BLE_CHAR_ID = 88U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_IBEACON_128_BIT_UUID_BLE_CHAR_ID = 90U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER1_NAME_BLE_CHAR_ID = 93U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER1_PASSWORD_BLE_CHAR_ID = 95U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER1_ROLE_BLE_CHAR_ID = 97U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER2_NAME_BLE_CHAR_ID = 99U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER2_PASSWORD_BLE_CHAR_ID = 101U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER2_ROLE_BLE_CHAR_ID = 103U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER3_NAME_BLE_CHAR_ID = 105U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER3_PASSWORD_BLE_CHAR_ID = 107U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER3_ROLE_BLE_CHAR_ID = 109U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER4_NAME_BLE_CHAR_ID = 111U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER4_PASSWORD_BLE_CHAR_ID = 113U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_USER4_ROLE_BLE_CHAR_ID = 115U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_SETTING_1_BLE_CHAR_ID = 117U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_SETTING_2_BLE_CHAR_ID = 119U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_SETTING_3_BLE_CHAR_ID = 121U;
const Ble_DCI::CHAR_ID_TD DCI_SIMPLE_APP_CONTROL_WORD_BLE_CHAR_ID = 123U;
const Ble_DCI::CHAR_ID_TD DCI_SIMPLE_APP_STATUS_WORD_BLE_CHAR_ID = 125U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_COUNTER_BLE_CHAR_ID = 127U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_SETTING_A_BLE_CHAR_ID = 129U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_SETTING_D_BLE_CHAR_ID = 131U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_SETTING_E_BLE_CHAR_ID = 133U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_SETTING_F_BLE_CHAR_ID = 135U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_READING_1_BLE_CHAR_ID = 137U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_SETTING_4_BLE_CHAR_ID = 139U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_READING_2_BLE_CHAR_ID = 141U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_LOG_BLE_CHAR_ID = 143U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_LOG_INDEX_BLE_CHAR_ID = 145U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_PRESENT_STATUS_BLE_CHAR_ID = 147U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_TEST_DEVICE_TIME_BLE_CHAR_ID = 149U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_BLE_CHAR_ID = 152U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_PROFILE_ID_BLE_CHAR_ID = 154U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_DEVICE_BONDING_INFO_BLE_CHAR_ID = 156U;
const Ble_DCI::CHAR_ID_TD DCI_BLE_ACTIVE_ACCESS_LEVEL_BLE_CHAR_ID = 158U;

//*******************************************************
//******     The Ble DCIDS Cross Reference Table.
//*******************************************************

const Ble_DCI::dci_ble_to_dcid_lkup_st_td_t ble_to_dcid_struct[DCI_BLE_DATA_TOTAL_CHAR_ID] =
{
    {
        // model short name of the device
        DCI_BLE_DEVICE_SNAME_BLE_CHAR_ID,
        DCI_BLE_DEVICE_SNAME_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x18, 0x00 },
        { 0x2A, 0x00 },
#endif
    },
    {
        // Vendor ID
        DCI_BLE_DEVICE_VENDOR_BLE_CHAR_ID,
        DCI_BLE_DEVICE_VENDOR_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xba, 0xda, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Family of the BLE device
        DCI_BLE_DEVICE_FAMILY_BLE_CHAR_ID,
        DCI_BLE_DEVICE_FAMILY_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xbb, 0xde, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Role of the BLE device
        DCI_BLE_DEVICE_ROLE_BLE_CHAR_ID,
        DCI_BLE_DEVICE_ROLE_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc0, 0x02, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Model of the BLE device
        DCI_BLE_DEVICE_MODEL_BLE_CHAR_ID,
        DCI_BLE_DEVICE_MODEL_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc0, 0xfc, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Device long name
        DCI_BLE_DEVICE_LNAME_BLE_CHAR_ID,
        DCI_BLE_DEVICE_LNAME_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc2, 0x8c, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // HW_Version of BLE device
        DCI_BLE_DEVICE_HW_VER_BLE_CHAR_ID,
        DCI_BLE_DEVICE_HW_VER_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc3, 0x54, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // SW_Version of BLE device
        DCI_BLE_DEVICE_SW_VER_BLE_CHAR_ID,
        DCI_BLE_DEVICE_SW_VER_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc4, 0x12, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Automatic Login
        DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_BLE_CHAR_ID,
        DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc4, 0xd0, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Manufacturer name string
        DCI_BLE_DEVICE_MANUFACTURER_NAME_BLE_CHAR_ID,
        DCI_BLE_DEVICE_MANUFACTURER_NAME_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc9, 0x3a, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // BLE_Device Serial Number
        DCI_BLE_DEVICE_SERIAL_NO_BLE_CHAR_ID,
        DCI_BLE_DEVICE_SERIAL_NO_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc9, 0xf8, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Device Image URL
        DCI_BLE_DEVICE_IMAGE_URL_BLE_CHAR_ID,
        DCI_BLE_DEVICE_IMAGE_URL_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xcb, 0xd8, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Ble Minimum value for the connection event interval
        DCI_BLE_MIN_VAL_CONN_INTERVAL_BLE_CHAR_ID,
        DCI_BLE_MIN_VAL_CONN_INTERVAL_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xcb, 0xd8, 0xcf, 0x4b, 0x13, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x88, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Ble Maximum value for the connection event interval
        DCI_BLE_MAX_VAL_CONN_INTERVAL_BLE_CHAR_ID,
        DCI_BLE_MAX_VAL_CONN_INTERVAL_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xcb, 0xd8, 0xcf, 0x4b, 0x13, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x98, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Slave Latency of the peripheral
        DCI_BLE_SLAVE_LATENCY_BLE_CHAR_ID,
        DCI_BLE_SLAVE_LATENCY_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xcb, 0xd8, 0xcf, 0x4b, 0x13, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x48, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Supervison Timeout of the peripheral
        DCI_BLE_SUPERVISION_TIMEOUT_BLE_CHAR_ID,
        DCI_BLE_SUPERVISION_TIMEOUT_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xcb, 0xd8, 0xcf, 0x4b, 0x13, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x58, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Ble_Admin_Password
        DCI_BLE_ADMIN_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_ADMIN_PASSWORD_DCID,
        7U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x03 },
        { 0xe9, 0x54, 0x50, 0x8d, 0xc4, 0x45, 0x4a, 0xf9, 0xb0, 0x47, 0xdc, 0x1b, 0x4a, 0x9f, 0xa6, 0x5f },
#endif
    },
    {
        // Ble_Observer_Password
        DCI_BLE_OBSERVER_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_OBSERVER_PASSWORD_DCID,
        7U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x03 },
        { 0xf5, 0xc8, 0x92, 0xd7, 0xdd, 0x27, 0x4c, 0x06, 0xaf, 0xdc, 0x6c, 0x8c, 0xee, 0x15, 0x09, 0x3d },
#endif
    },
    {
        // Ble_Operator_Password
        DCI_BLE_OPERATOR_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_OPERATOR_PASSWORD_DCID,
        7U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x03 },
        { 0x3e, 0x64, 0xf2, 0xf2, 0x89, 0x79, 0x44, 0x02, 0x94, 0xad, 0x2b, 0x30, 0x70, 0x5f, 0xf9, 0x54 },
#endif
    },
    {
        // Ble_Engineer_Password
        DCI_BLE_ENGINEER_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_ENGINEER_PASSWORD_DCID,
        7U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x03 },
        { 0x63, 0x6b, 0x8a, 0xf5, 0x38, 0xbb, 0x46, 0x53, 0xa4, 0xd2, 0x2f, 0x97, 0xee, 0xbc, 0xe5, 0xd5 },
#endif
    },
    {
        // BLE_User_Name
        DCI_BLE_USERNAME_BLE_CHAR_ID,
        DCI_BLE_USERNAME_DCID,
        7U,
        7U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc7, 0x96, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // BLE_Password
        DCI_BLE_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_PASSWORD_DCID,
        7U,
        7U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x03 },
        { 0x53, 0x4a, 0xc8, 0x72, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Ble Eddystone URL Advertisement data Input Uint8
        DCI_BLE_EDDYSTONE_URL_BLE_CHAR_ID,
        DCI_BLE_EDDYSTONE_URL_DCID,
        7U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x03 },
        { 0x63, 0x6b, 0x8a, 0xf5, 0x38, 0xbb, 0x46, 0x53, 0xa4, 0xd2, 0x2f, 0x28, 0xee, 0xbc, 0xe5, 0xd6 },
#endif
    },
    {
        // Ble iBeacon Major Number input Uint8
        DCI_BLE_IBEACON_MAJOR_NUMBER_BLE_CHAR_ID,
        DCI_BLE_IBEACON_MAJOR_NUMBER_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x63, 0x6b, 0x8a, 0xf5, 0x38, 0xbb, 0x46, 0x53, 0xa4, 0xf2, 0x2f, 0x28, 0xee, 0xbc, 0xe5, 0xd7 },
#endif
    },
    {
        // Ble iBeacon Minor Number input Uint8
        DCI_BLE_IBEACON_MINOR_NUMBER_BLE_CHAR_ID,
        DCI_BLE_IBEACON_MINOR_NUMBER_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x63, 0x6b, 0x8a, 0xf5, 0x38, 0xbb, 0x46, 0x53, 0xa4, 0xf6, 0x2f, 0x28, 0xee, 0xbc, 0xe5, 0xd7 },
#endif
    },
    {
        // Ble iBeacon uuid input Uint8
        DCI_BLE_IBEACON_128_BIT_UUID_BLE_CHAR_ID,
        DCI_BLE_IBEACON_128_BIT_UUID_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x03 },
        { 0x63, 0x6b, 0x8a, 0xf5, 0x38, 0xbb, 0x46, 0x66, 0xa4, 0xf6, 0x2f, 0x28, 0xee, 0xbc, 0xe5, 0xd8 },
#endif
    },
    {
        // Ble_User1_Name
        DCI_BLE_USER1_NAME_BLE_CHAR_ID,
        DCI_BLE_USER1_NAME_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x6a, 0x0c, 0xb5, 0xee, 0x4f, 0x8a, 0x45, 0x5d, 0x8b, 0xca, 0x67, 0x5c, 0x6b, 0xd0, 0xf0, 0x3f },
#endif
    },
    {
        // Ble_User1_Password
        DCI_BLE_USER1_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_USER1_PASSWORD_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0xae, 0x02, 0xb7, 0x56, 0xab, 0x1c, 0x40, 0xaf, 0xb6, 0x82, 0xde, 0xf1, 0x8b, 0x05, 0xe1, 0xdb },
#endif
    },
    {
        // Ble_User1_Role
        DCI_BLE_USER1_ROLE_BLE_CHAR_ID,
        DCI_BLE_USER1_ROLE_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0xae, 0x03, 0xb7, 0x56, 0xab, 0x1c, 0x40, 0xaf, 0xb6, 0x82, 0xde, 0xf1, 0x8b, 0x05, 0xe1, 0xdb },
#endif
    },
    {
        // Ble_User2_Name
        DCI_BLE_USER2_NAME_BLE_CHAR_ID,
        DCI_BLE_USER2_NAME_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x5c, 0x0d, 0x82, 0xb5, 0x97, 0xe8, 0x44, 0x98, 0xa3, 0xab, 0x25, 0x04, 0x98, 0x77, 0xf5, 0x37 },
#endif
    },
    {
        // Ble_User2_Password
        DCI_BLE_USER2_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_USER2_PASSWORD_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0xce, 0x9b, 0xb2, 0x13, 0x63, 0x05, 0x44, 0x25, 0x8b, 0xd3, 0x93, 0xa4, 0xc9, 0x4b, 0xb9, 0x0d },
#endif
    },
    {
        // Ble_User2_Role
        DCI_BLE_USER2_ROLE_BLE_CHAR_ID,
        DCI_BLE_USER2_ROLE_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0xce, 0x9b, 0xb2, 0x28, 0x63, 0x05, 0x44, 0x25, 0x8b, 0xd3, 0x93, 0xa4, 0xc9, 0x4b, 0xb9, 0x0d },
#endif
    },
    {
        // Ble_User3_Name
        DCI_BLE_USER3_NAME_BLE_CHAR_ID,
        DCI_BLE_USER3_NAME_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x95, 0xa2, 0x03, 0x6a, 0x92, 0xba, 0x4b, 0x6f, 0x87, 0xdb, 0x61, 0xa6, 0x6f, 0x96, 0x0d, 0x67 },
#endif
    },
    {
        // Ble_User3_Password
        DCI_BLE_USER3_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_USER3_PASSWORD_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0xbe, 0xc9, 0x7f, 0xc7, 0x3e, 0x8e, 0x47, 0x78, 0x85, 0x7d, 0xf5, 0xac, 0x69, 0x7b, 0x34, 0xfe },
#endif
    },
    {
        // Ble_User3_Role
        DCI_BLE_USER3_ROLE_BLE_CHAR_ID,
        DCI_BLE_USER3_ROLE_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0xbe, 0xc9, 0x8f, 0xc8, 0x3e, 0x8e, 0x47, 0x78, 0x85, 0x7d, 0xf5, 0xac, 0x69, 0x7b, 0x34, 0xfe },
#endif
    },
    {
        // Ble_User4_Name
        DCI_BLE_USER4_NAME_BLE_CHAR_ID,
        DCI_BLE_USER4_NAME_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0xe1, 0xf2, 0x61, 0xc2, 0x97, 0x81, 0x43, 0x26, 0xa2, 0xc8, 0x81, 0x22, 0x8b, 0xa3, 0xaf, 0xb3 },
#endif
    },
    {
        // Ble_User4_Password
        DCI_BLE_USER4_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_USER4_PASSWORD_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x50, 0xbe, 0x06, 0x2a, 0xcc, 0x0c, 0x47, 0xb6, 0xa3, 0x18, 0xa4, 0xaf, 0x28, 0x7f, 0x28, 0xba },
#endif
    },
    {
        // Ble_User4_Role
        DCI_BLE_USER4_ROLE_BLE_CHAR_ID,
        DCI_BLE_USER4_ROLE_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x51, 0xbe, 0x06, 0x3a, 0xcc, 0x0c, 0x47, 0xb6, 0xa3, 0x18, 0xa4, 0xaf, 0x28, 0x7f, 0x28, 0xba },
#endif
    },
    {
        // Ble Sample App Input Byte
        DCI_BLE_TEST_SETTING_1_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_1_DCID,
        7U,
        31U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x66, 0x45, 0x9c, 0x91, 0x7b, 0x21, 0xd4, 0xc7, 0xc1, 0x32, 0xbb },
#endif
    },
    {
        // Ble Sample App Input Sint8
        DCI_BLE_TEST_SETTING_2_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_2_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x66, 0x45, 0x43, 0x91, 0x7b, 0x21, 0xd5, 0xc7, 0xc1, 0x32, 0xbb },
#endif
    },
    {
        // Ble Sample App Input Uint8
        DCI_BLE_TEST_SETTING_3_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_3_DCID,
        7U,
        31U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x66, 0x45, 0x93, 0x91, 0x7b, 0x21, 0xd1, 0xc0, 0xc1, 0x22, 0xbb },
#endif
    },
    {
        // Switch to control LED
        DCI_SIMPLE_APP_CONTROL_WORD_BLE_CHAR_ID,
        DCI_SIMPLE_APP_CONTROL_WORD_DCID,
        7U,
        7U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE_WITHOUT_RESP )
        ),
        { 0x50, 0x02 },
        { 0x76, 0x9c, 0x52, 0x80, 0xcd, 0xea, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Switches to display the LED status
        DCI_SIMPLE_APP_STATUS_WORD_BLE_CHAR_ID,
        DCI_SIMPLE_APP_STATUS_WORD_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x02 },
        { 0x76, 0x9c, 0x54, 0x30, 0xcd, 0xea, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Test Counter for logging data
        DCI_BLE_TEST_COUNTER_BLE_CHAR_ID,
        DCI_BLE_TEST_COUNTER_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_NOTIFY )
        ),
        { 0x50, 0x02 },
        { 0xb2, 0x48, 0x01, 0xda, 0xf8, 0x17, 0x11, 0xea, 0xad, 0xc1, 0x02, 0x42, 0xac, 0x12, 0x00, 0x02 },
#endif
    },
    {
        // Ble Sample App Input WORD
        DCI_BLE_TEST_SETTING_A_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_A_DCID,
        7U,
        31U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x76, 0x45, 0x9c, 0x91, 0x7b, 0x21, 0xd5, 0xc7, 0xc1, 0x32, 0xbb },
#endif
    },
    {
        // Ble Sample App Input Uint8
        DCI_BLE_TEST_SETTING_D_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_D_DCID,
        15U,
        15U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x28, 0x45, 0x93, 0x91, 0x7b, 0x21, 0xd5, 0xc7, 0xc1, 0x32, 0xbb },
#endif
    },
    {
        // Ble Sample App Input  Uint16
        DCI_BLE_TEST_SETTING_E_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_E_DCID,
        7U,
        31U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x66, 0x45, 0x28, 0x91, 0x7b, 0x21, 0xd5, 0xc7, 0xc1, 0x32, 0xbb },
#endif
    },
    {
        // Ble Sample App Input  Uint8
        DCI_BLE_TEST_SETTING_F_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_F_DCID,
        31U,
        31U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x66, 0x45, 0x93, 0x91, 0x7b, 0x21, 0xd1, 0xc3, 0xc1, 0x32, 0xbb },
#endif
    },
    {
        // Ble Sample App Input STRING8
        DCI_BLE_TEST_READING_1_BLE_CHAR_ID,
        DCI_BLE_TEST_READING_1_DCID,
        7U,
        31U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x66, 0x10, 0x05, 0x91, 0x7b, 0x21, 0xd1, 0xc3, 0xc1, 0x32, 0xbb },
#endif
    },
    {
        // Ble Sample App Input  BOOL
        DCI_BLE_TEST_SETTING_4_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_4_DCID,
        99U,
        99U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x76, 0x9c, 0x4c, 0x36, 0xcd, 0xea, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Ble Sample App Input FLOAT
        DCI_BLE_TEST_READING_2_BLE_CHAR_ID,
        DCI_BLE_TEST_READING_2_DCID,
        7U,
        31U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x76, 0x9c, 0x50, 0x8c, 0xcd, 0xea, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // BLE Test Log
        DCI_BLE_LOG_BLE_CHAR_ID,
        DCI_BLE_LOG_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x02 },
        { 0x76, 0x9c, 0x71, 0x68, 0xcd, 0xea, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // BLE Test Log Index
        DCI_BLE_LOG_INDEX_BLE_CHAR_ID,
        DCI_BLE_LOG_INDEX_DCID,
        7U,
        7U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x76, 0x9c, 0x81, 0x68, 0xcd, 0xea, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Ble Sample App Input Uint16
        DCI_BLE_TEST_PRESENT_STATUS_BLE_CHAR_ID,
        DCI_BLE_TEST_PRESENT_STATUS_DCID,
        7U,
        31U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x66, 0x10, 0x12, 0x91, 0x7b, 0x21, 0xd1, 0xc3, 0xc1, 0x32, 0xbb },
#endif
    },
    {
        // Ble Sample App Input Uint32
        DCI_BLE_TEST_DEVICE_TIME_BLE_CHAR_ID,
        DCI_BLE_TEST_DEVICE_TIME_DCID,
        15U,
        15U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x02 },
        { 0x2a, 0x0a, 0xda, 0xef, 0x8d, 0x66, 0x45, 0x9c, 0x91, 0x7b, 0x21, 0xd5, 0xc7, 0xc1, 0x32, 0xbb },
#endif
    },
    {
        // Ble Find Target Device 
        DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_BLE_CHAR_ID,
        DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_DCID,
        7U,
        7U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
            | ( BLE_CHAR_PROP_WRITE )
        ),
        { 0x50, 0x05 },
        { 0x76, 0x9c, 0x54, 0x30, 0xcd, 0xea, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0f },
#endif
    },
    {
        // Profile ID
        DCI_BLE_DEVICE_PROFILE_ID_BLE_CHAR_ID,
        DCI_BLE_DEVICE_PROFILE_ID_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x05 },
        { 0x53, 0x4a, 0xb8, 0x28, 0xcf, 0x4b, 0x11, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // BLE Device Bonding info
        DCI_BLE_DEVICE_BONDING_INFO_BLE_CHAR_ID,
        DCI_BLE_DEVICE_BONDING_INFO_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x05 },
        { 0x53, 0x4a, 0xcb, 0xd8, 0xcf, 0x4b, 0x13, 0xe7, 0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a },
#endif
    },
    {
        // Indicate the Active Access Level
        DCI_BLE_ACTIVE_ACCESS_LEVEL_BLE_CHAR_ID,
        DCI_BLE_ACTIVE_ACCESS_LEVEL_DCID,
        7U,
        0U,
#ifdef STM32WB55_WPAN
        (
            ( BLE_CHAR_PROP_READ )
        ),
        { 0x50, 0x05 },
        { 0x9f, 0x20, 0x3d, 0xf7, 0x0a, 0x1b, 0x4d, 0x53, 0xb6, 0x26, 0xcb, 0x37, 0x62, 0x0d, 0x65, 0xaf },
#endif
    },
};

// *******************************************************
// ******     The BLE Target Structure.
// *******************************************************

const Ble_DCI::ble_target_info_st_td_t ble_dci_data_target_info =
{
    DCI_BLE_DATA_TOTAL_CHAR_ID,        //CHAR_ID_TD total_char_id
    ble_to_dcid_struct,        //const dci_ble_to_dcid_lkup_st_td_t* ble_to_dcid_struct
};

// *******************************************************
// ******     The BLE Group Cross Reference Table.
// *******************************************************

static const uint8_t DCI_BLE_DATA_TOTAL_GROUP_ID = 14U;
const Ble_DCI::ble_group_assembly_t ble_dci_group_struct[DCI_BLE_DATA_TOTAL_GROUP_ID] =
{
    {
        15U,
        161U,
    {
        DCI_BLE_DEVICE_VENDOR_BLE_CHAR_ID,
        DCI_BLE_DEVICE_FAMILY_BLE_CHAR_ID,
        DCI_BLE_DEVICE_ROLE_BLE_CHAR_ID,
        DCI_BLE_DEVICE_MODEL_BLE_CHAR_ID,
        DCI_BLE_DEVICE_LNAME_BLE_CHAR_ID,
        DCI_BLE_DEVICE_HW_VER_BLE_CHAR_ID,
        DCI_BLE_DEVICE_SW_VER_BLE_CHAR_ID,
        DCI_BLE_DEVICE_AUTO_LOGIN_SELECT_BLE_CHAR_ID,
        DCI_BLE_DEVICE_MANUFACTURER_NAME_BLE_CHAR_ID,
        DCI_BLE_DEVICE_SERIAL_NO_BLE_CHAR_ID,
        DCI_BLE_DEVICE_IMAGE_URL_BLE_CHAR_ID,
        DCI_BLE_MIN_VAL_CONN_INTERVAL_BLE_CHAR_ID,
        DCI_BLE_MAX_VAL_CONN_INTERVAL_BLE_CHAR_ID,
        DCI_BLE_SLAVE_LATENCY_BLE_CHAR_ID,
        DCI_BLE_SUPERVISION_TIMEOUT_BLE_CHAR_ID,
    },
    },
    {
        4U,
        163U,
    {
        DCI_BLE_ADMIN_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_OBSERVER_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_OPERATOR_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_ENGINEER_PASSWORD_BLE_CHAR_ID,
    },
    },
    {
        6U,
        165U,
    {
        DCI_BLE_USERNAME_BLE_CHAR_ID,
        DCI_BLE_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_EDDYSTONE_URL_BLE_CHAR_ID,
        DCI_BLE_IBEACON_MAJOR_NUMBER_BLE_CHAR_ID,
        DCI_BLE_IBEACON_MINOR_NUMBER_BLE_CHAR_ID,
        DCI_BLE_IBEACON_128_BIT_UUID_BLE_CHAR_ID,
    },
    },
    {
        8U,
        167U,
    {
        DCI_BLE_TEST_SETTING_1_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_2_BLE_CHAR_ID,
        DCI_BLE_TEST_SETTING_3_BLE_CHAR_ID,
        DCI_BLE_TEST_READING_2_BLE_CHAR_ID,
        DCI_BLE_LOG_BLE_CHAR_ID,
        DCI_BLE_LOG_INDEX_BLE_CHAR_ID,
        DCI_BLE_TEST_PRESENT_STATUS_BLE_CHAR_ID,
        DCI_BLE_TEST_DEVICE_TIME_BLE_CHAR_ID,
    },
    },
    {
        6U,
        169U,
    {
        DCI_BLE_USER1_NAME_BLE_CHAR_ID,
        DCI_BLE_USER1_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_USER1_ROLE_BLE_CHAR_ID,
        DCI_BLE_USER2_NAME_BLE_CHAR_ID,
        DCI_BLE_USER2_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_USER2_ROLE_BLE_CHAR_ID
    },
    },
    {
        6U,
        171U,
    {
        DCI_BLE_USER3_NAME_BLE_CHAR_ID,
        DCI_BLE_USER3_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_USER3_ROLE_BLE_CHAR_ID,
        DCI_BLE_USER4_NAME_BLE_CHAR_ID,
        DCI_BLE_USER4_PASSWORD_BLE_CHAR_ID,
        DCI_BLE_USER4_ROLE_BLE_CHAR_ID
    },
    },
    {
        3U,
        173U,
    {
        DCI_SIMPLE_APP_CONTROL_WORD_BLE_CHAR_ID,
        DCI_SIMPLE_APP_STATUS_WORD_BLE_CHAR_ID,
        DCI_BLE_TEST_COUNTER_BLE_CHAR_ID
    },
    },
    {
        1U,
        175U,
    {
        DCI_BLE_TEST_SETTING_A_BLE_CHAR_ID
    },
    },
    {
        1U,
        177U,
    {
        DCI_BLE_TEST_SETTING_D_BLE_CHAR_ID
    },
    },
    {
        1U,
        179U,
    {
        DCI_BLE_TEST_SETTING_E_BLE_CHAR_ID
    },
    },
    {
        1U,
        181U,
    {
        DCI_BLE_TEST_SETTING_F_BLE_CHAR_ID
    },
    },
    {
        1U,
        183U,
    {
        DCI_BLE_TEST_READING_1_BLE_CHAR_ID
    },
    },
    {
        1U,
        185U,
    {
        DCI_BLE_TEST_SETTING_4_BLE_CHAR_ID
    },
    },
    {
        4U,
        187U,
    {
        DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_BLE_CHAR_ID,
        DCI_BLE_DEVICE_PROFILE_ID_BLE_CHAR_ID,
        DCI_BLE_DEVICE_BONDING_INFO_BLE_CHAR_ID,
        DCI_BLE_ACTIVE_ACCESS_LEVEL_BLE_CHAR_ID
    },
    },
};

//*******************************************************
//******     The BLE Group Structure.
//*******************************************************

const Ble_DCI::ble_group_info_st_td_t ble_dci_data_group_info =
{
    DCI_BLE_DATA_TOTAL_GROUP_ID,   //CHAR_ID_TD total_group_id
    ble_dci_group_struct,        //const ble_group_assembly_t* ble_dci_group_struct
};
