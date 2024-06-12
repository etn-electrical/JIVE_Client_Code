/**
*****************************************************************************************
*   @file
*
*   @brief
*
*   @details
*
*   @version
*   C++ Style Guide Version 1.0
*
*   @copyright 2021 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#ifndef BLE_DCI_DATA_H
#define BLE_DCI_DATA_H

#include "DCI.h"
#include "DCI_Data.h"
#include "Ble_DCI.h"


#include "Ble_Defines.h"

// *******************************************************
// ******     Standard Function UUID.
// *******************************************************

#define TOTAL_SERVICE 5
#define SERVICE_1_APP_ID 0
#define SERVICE_2_APP_ID 1
#define SERVICE_3_APP_ID 2
#define SERVICE_4_APP_ID 3
#define SERVICE_5_APP_ID 4

// *******************************************************
// ******     Variables and constants.
// *******************************************************

 static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE; 
 static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE; 
 static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
 static const uint8_t char_prop_notify = ESP_GATT_CHAR_PROP_BIT_NOTIFY; 
 static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ; 
 static const uint8_t char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_WRITE |  ESP_GATT_CHAR_PROP_BIT_READ; 

///< UUID : Services and Characteristics UUID 

/* Services and Characteristics UUID variables 
 LSB <--------------------------------------------------------------------------------> MSB */

///< GATTS_DEVICE_UUID DCI_BLE_DEVICE_SNAME
static const uint8_t serv1_svc_uuid[ BYTES_UUID_16BIT ] = 
        { 0x00, 0x18 };
///< GATTS_SERV1_CHAR1_VAL_UUID 0x2A00 
static const uint8_t serv1_char1_val_uuid[ BYTES_UUID_16BIT ] = 
        { 0x00, 0x2A };
static const uint8_t serv2_svc_uuid[BYTES_UUID_16BIT] = 
        { 0x03, 0x50 };
///< GATTS_SERV2_CHAR_UUID ( 5003 ) 
static const uint8_t serv2_char1_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0xda, 0xba, 0x4a, 0x53 };
static const uint8_t serv2_char2_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0xde, 0xbb, 0x4a, 0x53 };
static const uint8_t serv2_char3_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0x02, 0xc0, 0x4a, 0x53 };
static const uint8_t serv2_char4_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0xfc, 0xc0, 0x4a, 0x53 };
static const uint8_t serv2_char5_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0x8c, 0xc2, 0x4a, 0x53 };
static const uint8_t serv2_char6_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0x54, 0xc3, 0x4a, 0x53 };
static const uint8_t serv2_char7_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0x12, 0xc4, 0x4a, 0x53 };
static const uint8_t serv2_char8_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0xd0, 0xc4, 0x4a, 0x53 };
static const uint8_t serv2_char9_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0x3a, 0xc9, 0x4a, 0x53 };
static const uint8_t serv2_char10_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0xf8, 0xc9, 0x4a, 0x53 };
static const uint8_t serv2_char11_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0xd8, 0xcb, 0x4a, 0x53 };
static const uint8_t serv2_char12_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x88, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x13, 0x4b, 0xcf, 0xd8, 0xcb, 0x4a, 0x53 };
static const uint8_t serv2_char13_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x98, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x13, 0x4b, 0xcf, 0xd8, 0xcb, 0x4a, 0x53 };
static const uint8_t serv2_char14_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x48, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x13, 0x4b, 0xcf, 0xd8, 0xcb, 0x4a, 0x53 };
static const uint8_t serv2_char15_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x58, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x13, 0x4b, 0xcf, 0xd8, 0xcb, 0x4a, 0x53 };
static const uint8_t serv2_char16_val_uuid[BYTES_UUID_128BIT] = 
{ 0x5f, 0xa6, 0x9f, 0x4a, 0x1b, 0xdc, 0x47, 0xb0, 0xf9, 0x4a, 0x45, 0xc4, 0x8d, 0x50, 0x54, 0xe9 };
static const uint8_t serv2_char17_val_uuid[BYTES_UUID_128BIT] = 
{ 0x3d, 0x09, 0x15, 0xee, 0x8c, 0x6c, 0xdc, 0xaf, 0x06, 0x4c, 0x27, 0xdd, 0xd7, 0x92, 0xc8, 0xf5 };
static const uint8_t serv2_char18_val_uuid[BYTES_UUID_128BIT] = 
{ 0x54, 0xf9, 0x5f, 0x70, 0x30, 0x2b, 0xad, 0x94, 0x02, 0x44, 0x79, 0x89, 0xf2, 0xf2, 0x64, 0x3e };
static const uint8_t serv2_char19_val_uuid[BYTES_UUID_128BIT] = 
{ 0xd5, 0xe5, 0xbc, 0xee, 0x97, 0x2f, 0xd2, 0xa4, 0x53, 0x46, 0xbb, 0x38, 0xf5, 0x8a, 0x6b, 0x63 };
static const uint8_t serv2_char20_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0x96, 0xc7, 0x4a, 0x53 };
static const uint8_t serv2_char21_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0x72, 0xc8, 0x4a, 0x53 };
static const uint8_t serv2_char22_val_uuid[BYTES_UUID_128BIT] = 
{ 0xd6, 0xe5, 0xbc, 0xee, 0x28, 0x2f, 0xd2, 0xa4, 0x53, 0x46, 0xbb, 0x38, 0xf5, 0x8a, 0x6b, 0x63 };
static const uint8_t serv2_char23_val_uuid[BYTES_UUID_128BIT] = 
{ 0xd7, 0xe5, 0xbc, 0xee, 0x28, 0x2f, 0xf2, 0xa4, 0x53, 0x46, 0xbb, 0x38, 0xf5, 0x8a, 0x6b, 0x63 };
static const uint8_t serv2_char24_val_uuid[BYTES_UUID_128BIT] = 
{ 0xd7, 0xe5, 0xbc, 0xee, 0x28, 0x2f, 0xf6, 0xa4, 0x53, 0x46, 0xbb, 0x38, 0xf5, 0x8a, 0x6b, 0x63 };
static const uint8_t serv2_char25_val_uuid[BYTES_UUID_128BIT] = 
{ 0xd8, 0xe5, 0xbc, 0xee, 0x28, 0x2f, 0xf6, 0xa4, 0x66, 0x46, 0xbb, 0x38, 0xf5, 0x8a, 0x6b, 0x63 };
static const uint8_t serv3_svc_uuid[BYTES_UUID_16BIT] = 
        { 0x02, 0x50 };
///< GATTS_SERV3_CHAR_UUID ( 5002 ) 
static const uint8_t serv3_char1_val_uuid[BYTES_UUID_128BIT] = 
{ 0x3f, 0xf0, 0xd0, 0x6b, 0x5c, 0x67, 0xca, 0x8b, 0x5d, 0x45, 0x8a, 0x4f, 0xee, 0xb5, 0x0c, 0x6a };
static const uint8_t serv3_char2_val_uuid[BYTES_UUID_128BIT] = 
{ 0xdb, 0xe1, 0x05, 0x8b, 0xf1, 0xde, 0x82, 0xb6, 0xaf, 0x40, 0x1c, 0xab, 0x56, 0xb7, 0x02, 0xae };
static const uint8_t serv3_char3_val_uuid[BYTES_UUID_128BIT] = 
{ 0xdb, 0xe1, 0x05, 0x8b, 0xf1, 0xde, 0x82, 0xb6, 0xaf, 0x40, 0x1c, 0xab, 0x56, 0xb7, 0x03, 0xae };
static const uint8_t serv3_char4_val_uuid[BYTES_UUID_128BIT] = 
{ 0x37, 0xf5, 0x77, 0x98, 0x04, 0x25, 0xab, 0xa3, 0x98, 0x44, 0xe8, 0x97, 0xb5, 0x82, 0x0d, 0x5c };
static const uint8_t serv3_char5_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0d, 0xb9, 0x4b, 0xc9, 0xa4, 0x93, 0xd3, 0x8b, 0x25, 0x44, 0x05, 0x63, 0x13, 0xb2, 0x9b, 0xce };
static const uint8_t serv3_char6_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0d, 0xb9, 0x4b, 0xc9, 0xa4, 0x93, 0xd3, 0x8b, 0x25, 0x44, 0x05, 0x63, 0x28, 0xb2, 0x9b, 0xce };
static const uint8_t serv3_char7_val_uuid[BYTES_UUID_128BIT] = 
{ 0x67, 0x0d, 0x96, 0x6f, 0xa6, 0x61, 0xdb, 0x87, 0x6f, 0x4b, 0xba, 0x92, 0x6a, 0x03, 0xa2, 0x95 };
static const uint8_t serv3_char8_val_uuid[BYTES_UUID_128BIT] = 
{ 0xfe, 0x34, 0x7b, 0x69, 0xac, 0xf5, 0x7d, 0x85, 0x78, 0x47, 0x8e, 0x3e, 0xc7, 0x7f, 0xc9, 0xbe };
static const uint8_t serv3_char9_val_uuid[BYTES_UUID_128BIT] = 
{ 0xfe, 0x34, 0x7b, 0x69, 0xac, 0xf5, 0x7d, 0x85, 0x78, 0x47, 0x8e, 0x3e, 0xc8, 0x8f, 0xc9, 0xbe };
static const uint8_t serv3_char10_val_uuid[BYTES_UUID_128BIT] = 
{ 0xb3, 0xaf, 0xa3, 0x8b, 0x22, 0x81, 0xc8, 0xa2, 0x26, 0x43, 0x81, 0x97, 0xc2, 0x61, 0xf2, 0xe1 };
static const uint8_t serv3_char11_val_uuid[BYTES_UUID_128BIT] = 
{ 0xba, 0x28, 0x7f, 0x28, 0xaf, 0xa4, 0x18, 0xa3, 0xb6, 0x47, 0x0c, 0xcc, 0x2a, 0x06, 0xbe, 0x50 };
static const uint8_t serv3_char12_val_uuid[BYTES_UUID_128BIT] = 
{ 0xba, 0x28, 0x7f, 0x28, 0xaf, 0xa4, 0x18, 0xa3, 0xb6, 0x47, 0x0c, 0xcc, 0x3a, 0x06, 0xbe, 0x51 };
static const uint8_t serv3_char13_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x32, 0xc1, 0xc7, 0xd4, 0x21, 0x7b, 0x91, 0x9c, 0x45, 0x66, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv3_char14_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x32, 0xc1, 0xc7, 0xd5, 0x21, 0x7b, 0x91, 0x43, 0x45, 0x66, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv3_char15_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x22, 0xc1, 0xc0, 0xd1, 0x21, 0x7b, 0x91, 0x93, 0x45, 0x66, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv3_char16_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0xea, 0xcd, 0x80, 0x52, 0x9c, 0x76 };
static const uint8_t serv3_char17_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0xea, 0xcd, 0x30, 0x54, 0x9c, 0x76 };
static const uint8_t serv3_char18_val_uuid[BYTES_UUID_128BIT] = 
{ 0x02, 0x00, 0x12, 0xac, 0x42, 0x02, 0xc1, 0xad, 0xea, 0x11, 0x17, 0xf8, 0xda, 0x01, 0x48, 0xb2 };
static const uint8_t serv3_char19_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x32, 0xc1, 0xc7, 0xd5, 0x21, 0x7b, 0x91, 0x9c, 0x45, 0x76, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv3_char20_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x32, 0xc1, 0xc7, 0xd5, 0x21, 0x7b, 0x91, 0x93, 0x45, 0x28, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv3_char21_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x32, 0xc1, 0xc7, 0xd5, 0x21, 0x7b, 0x91, 0x28, 0x45, 0x66, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv3_char22_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x32, 0xc1, 0xc3, 0xd1, 0x21, 0x7b, 0x91, 0x93, 0x45, 0x66, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv3_char23_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x32, 0xc1, 0xc3, 0xd1, 0x21, 0x7b, 0x91, 0x05, 0x10, 0x66, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv3_char24_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0xea, 0xcd, 0x36, 0x4c, 0x9c, 0x76 };
static const uint8_t serv3_char25_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0xea, 0xcd, 0x8c, 0x50, 0x9c, 0x76 };
static const uint8_t serv3_char26_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0xea, 0xcd, 0x68, 0x71, 0x9c, 0x76 };
static const uint8_t serv3_char27_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0xea, 0xcd, 0x68, 0x81, 0x9c, 0x76 };
static const uint8_t serv3_char28_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x32, 0xc1, 0xc3, 0xd1, 0x21, 0x7b, 0x91, 0x12, 0x10, 0x66, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv3_char29_val_uuid[BYTES_UUID_128BIT] = 
{ 0xbb, 0x32, 0xc1, 0xc7, 0xd5, 0x21, 0x7b, 0x91, 0x9c, 0x45, 0x66, 0x8d, 0xef, 0xda, 0x0a, 0x2a };
static const uint8_t serv4_svc_uuid[BYTES_UUID_16BIT] = 
        { 0x05, 0x50 };
///< GATTS_SERV4_CHAR_UUID ( 5005 ) 
static const uint8_t serv4_char1_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0f, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0xea, 0xcd, 0x30, 0x54, 0x9c, 0x76 };
static const uint8_t serv4_char2_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x11, 0x4b, 0xcf, 0x28, 0xb8, 0x4a, 0x53 };
static const uint8_t serv4_char3_val_uuid[BYTES_UUID_128BIT] = 
{ 0x0a, 0xb5, 0xb6, 0x78, 0xc2, 0xce, 0xc4, 0xab, 0xe7, 0x13, 0x4b, 0xcf, 0xd8, 0xcb, 0x4a, 0x53 };
static const uint8_t serv4_char4_val_uuid[BYTES_UUID_128BIT] = 
{ 0xaf, 0x65, 0x0d, 0x62, 0x37, 0xcb, 0x26, 0xb6, 0x53, 0x4d, 0x1b, 0x0a, 0xf7, 0x3d, 0x20, 0x9f };
static const uint8_t serv5_svc_uuid[BYTES_UUID_16BIT] = 
        { 0x04, 0x50 };
///< GATTS_SERV5_CHAR_UUID ( 5004 ) 
static const uint8_t serv5_char1_val_uuid[BYTES_UUID_128BIT] = 
{ 0x7d, 0x35, 0xb4, 0x27, 0x86, 0xad, 0xc1, 0x83, 0xd5, 0x44, 0x38, 0xf7, 0x2e, 0xfe, 0x43, 0xe0 };
static const uint8_t serv5_char2_val_uuid[BYTES_UUID_128BIT] = 
{ 0x7d, 0x35, 0xb4, 0x27, 0x86, 0xcd, 0xc1, 0x83, 0xd5, 0x44, 0x38, 0xf7, 0x2e, 0xfe, 0x43, 0xe0 };
static const uint8_t serv5_char3_val_uuid[BYTES_UUID_128BIT] = 
{ 0x7d, 0x35, 0xb4, 0xc7, 0x86, 0xcd, 0xc1, 0x83, 0xd5, 0x44, 0x38, 0xf7, 0x2e, 0xfe, 0x43, 0xe0 };
static const uint8_t serv5_char4_val_uuid[BYTES_UUID_128BIT] = 
{ 0x62, 0x24, 0x11, 0x94, 0x1d, 0x3c, 0xfc, 0x9a, 0xa9, 0x42, 0x64, 0x73, 0x4e, 0x70, 0xec, 0xdd };
static const uint8_t serv5_char5_val_uuid[BYTES_UUID_128BIT] = 
{ 0x7c, 0x35, 0xb4, 0x27, 0x86, 0xad, 0xc1, 0x83, 0xd5, 0x44, 0x38, 0xf7, 0x2e, 0xfe, 0x43, 0xe0 };
static const uint8_t serv5_char6_val_uuid[BYTES_UUID_128BIT] = 
{ 0x2c, 0x01, 0xb1, 0x27, 0x86, 0xad, 0xc1, 0x83, 0xd5, 0x44, 0x38, 0xf7, 0x2e, 0xfe, 0x43, 0xe0 };
static const uint8_t serv5_char7_val_uuid[BYTES_UUID_128BIT] = 
{ 0x8d, 0x56, 0xad, 0xba, 0x1c, 0xec, 0x20, 0xb2, 0xdc, 0x42, 0xfa, 0x74, 0x0b, 0xe0, 0xbf, 0x17 };
static const uint8_t serv5_char8_val_uuid[BYTES_UUID_128BIT] = 
{ 0x6b, 0x5c, 0x76, 0x32, 0xa7, 0x06, 0xa3, 0xa7, 0x99, 0x48, 0xf1, 0x59, 0x65, 0x74, 0x88, 0x1d };
static const uint8_t serv5_char9_val_uuid[BYTES_UUID_128BIT] = 
{ 0xe5, 0xc7, 0x84, 0x50, 0x64, 0xd5, 0x07, 0x83, 0xf2, 0x4f, 0x69, 0x09, 0x96, 0x7c, 0x24, 0xfc };
static const uint8_t serv5_char10_val_uuid[BYTES_UUID_128BIT] = 
{ 0x2e, 0x59, 0x2c, 0xef, 0xaf, 0x51, 0xc9, 0xa3, 0x58, 0x40, 0x75, 0x6d, 0x6f, 0x5e, 0x31, 0x96 };
static const uint8_t serv5_char11_val_uuid[BYTES_UUID_128BIT] = 
{ 0x40, 0xf3, 0x65, 0x3a, 0xd4, 0x83, 0xe3, 0x81, 0x31, 0x42, 0xa4, 0xd7, 0x63, 0xf3, 0x17, 0x20 };
static const uint8_t serv5_char12_val_uuid[BYTES_UUID_128BIT] = 
{ 0x7c, 0x95, 0x8f, 0xf4, 0x51, 0x49, 0x70, 0x81, 0x65, 0x40, 0xce, 0x6f, 0x57, 0x90, 0x45, 0x92 };
static const uint8_t serv5_char13_val_uuid[BYTES_UUID_128BIT] = 
{ 0xe6, 0x5a, 0x1f, 0x9a, 0x6e, 0x79, 0xac, 0xaf, 0x1a, 0x43, 0x36, 0x39, 0x3e, 0xb6, 0x02, 0x38 };
static const uint8_t serv5_char14_val_uuid[BYTES_UUID_128BIT] = 
{ 0xea, 0x5a, 0x1f, 0x9a, 0x6e, 0x79, 0xac, 0xaf, 0x1a, 0x43, 0x36, 0x39, 0x3e, 0xb6, 0x02, 0x38 };

static uint8_t sec_service_uuid[ BYTES_UUID_128BIT ] = {
      /// first uuid, 16bit, [12],[13] is the value
      0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, serv1_svc_uuid[0], serv1_svc_uuid[1], 0x00,
      0x00,
};
///< Define Total Number of Attributes in each Services 
#define TOTAL_ATTRIB_OF_SERV1             3
#define TOTAL_ATTRIB_OF_SERV2             51
#define TOTAL_ATTRIB_OF_SERV3             59
#define TOTAL_ATTRIB_OF_SERV4             9
#define TOTAL_ATTRIB_OF_SERV5             29

/**
 * @brief GATTS attr database table structure
 */
typedef struct
{
    uint8_t total_attrib;
    esp_gatts_attr_db_t attrib_tbl[ MAX_GATTS_ATTIRIBUTE ]; 
} esp_gatts_attrib_table_t;

const esp_gatts_attrib_table_t esp_gatts_attrib_db[ TOTAL_SERVICE ] =
{
   {
      TOTAL_ATTRIB_OF_SERV1,           /// Total number of attributes included in service 3
       {
           {
               { ESP_GATT_RSP_BY_APP },
               {
                  ESP_UUID_LEN_16,
                  ( uint8_t * )&primary_service_uuid,
                  ESP_GATT_PERM_READ,
                  sizeof( uint16_t ),
                  sizeof( serv1_svc_uuid ),
                  ( uint8_t * )&serv1_svc_uuid 
                }
           },

            /* Service 1800 Characteristic DCI_BLE_DEVICE_SNAME
               Service 1 Characteristic 1 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 1800 Characteristic DCI_BLE_DEVICE_SNAME
               Service 1 Characteristic 1 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv1_char1_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
       }
   },
   {
      TOTAL_ATTRIB_OF_SERV2,           /// Total number of attributes included in service 51
       {
           {
               { ESP_GATT_RSP_BY_APP },
               {
                  ESP_UUID_LEN_16,
                  ( uint8_t * )&primary_service_uuid,
                  ESP_GATT_PERM_READ,
                  sizeof( uint16_t ),
                  sizeof( serv2_svc_uuid ),
                  ( uint8_t * )&serv2_svc_uuid 
                }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_VENDOR
               Service 2 Characteristic 1 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_VENDOR
               Service 2 Characteristic 1 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char1_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_FAMILY
               Service 2 Characteristic 2 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_FAMILY
               Service 2 Characteristic 2 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char2_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_ROLE
               Service 2 Characteristic 3 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_ROLE
               Service 2 Characteristic 3 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char3_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_MODEL
               Service 2 Characteristic 4 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_MODEL
               Service 2 Characteristic 4 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char4_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_LNAME
               Service 2 Characteristic 5 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_LNAME
               Service 2 Characteristic 5 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char5_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_HW_VER
               Service 2 Characteristic 6 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_HW_VER
               Service 2 Characteristic 6 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char6_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_SW_VER
               Service 2 Characteristic 7 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_SW_VER
               Service 2 Characteristic 7 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char7_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_AUTO_LOGIN_SELECT
               Service 2 Characteristic 8 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_AUTO_LOGIN_SELECT
               Service 2 Characteristic 8 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char8_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_MANUFACTURER_NAME
               Service 2 Characteristic 9 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_MANUFACTURER_NAME
               Service 2 Characteristic 9 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char9_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_SERIAL_NO
               Service 2 Characteristic 10 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_SERIAL_NO
               Service 2 Characteristic 10 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char10_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_DEVICE_IMAGE_URL
               Service 2 Characteristic 11 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_DEVICE_IMAGE_URL
               Service 2 Characteristic 11 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char11_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_MIN_VAL_CONN_INTERVAL
               Service 2 Characteristic 12 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_MIN_VAL_CONN_INTERVAL
               Service 2 Characteristic 12 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char12_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_MAX_VAL_CONN_INTERVAL
               Service 2 Characteristic 13 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_MAX_VAL_CONN_INTERVAL
               Service 2 Characteristic 13 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char13_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_SLAVE_LATENCY
               Service 2 Characteristic 14 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_SLAVE_LATENCY
               Service 2 Characteristic 14 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char14_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_SUPERVISION_TIMEOUT
               Service 2 Characteristic 15 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_SUPERVISION_TIMEOUT
               Service 2 Characteristic 15 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char15_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_ADMIN_PASSWORD
               Service 2 Characteristic 16 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_ADMIN_PASSWORD
               Service 2 Characteristic 16 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char16_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_OBSERVER_PASSWORD
               Service 2 Characteristic 17 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_OBSERVER_PASSWORD
               Service 2 Characteristic 17 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char17_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_OPERATOR_PASSWORD
               Service 2 Characteristic 18 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_OPERATOR_PASSWORD
               Service 2 Characteristic 18 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char18_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_ENGINEER_PASSWORD
               Service 2 Characteristic 19 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5003 Characteristic DCI_BLE_ENGINEER_PASSWORD
               Service 2 Characteristic 19 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char19_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_USERNAME
               Service 2 Characteristic 20 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5003 Characteristic DCI_BLE_USERNAME
               Service 2 Characteristic 20 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char20_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_PASSWORD
               Service 2 Characteristic 21 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5003 Characteristic DCI_BLE_PASSWORD
               Service 2 Characteristic 21 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char21_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_EDDYSTONE_URL
               Service 2 Characteristic 22 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5003 Characteristic DCI_BLE_EDDYSTONE_URL
               Service 2 Characteristic 22 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char22_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_IBEACON_MAJOR_NUMBER
               Service 2 Characteristic 23 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5003 Characteristic DCI_BLE_IBEACON_MAJOR_NUMBER
               Service 2 Characteristic 23 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char23_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_IBEACON_MINOR_NUMBER
               Service 2 Characteristic 24 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5003 Characteristic DCI_BLE_IBEACON_MINOR_NUMBER
               Service 2 Characteristic 24 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char24_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5003 Characteristic DCI_BLE_IBEACON_128_BIT_UUID
               Service 2 Characteristic 25 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5003 Characteristic DCI_BLE_IBEACON_128_BIT_UUID
               Service 2 Characteristic 25 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv2_char25_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
       }
   },
   {
      TOTAL_ATTRIB_OF_SERV3,           /// Total number of attributes included in service 59
       {
           {
               { ESP_GATT_RSP_BY_APP },
               {
                  ESP_UUID_LEN_16,
                  ( uint8_t * )&primary_service_uuid,
                  ESP_GATT_PERM_READ,
                  sizeof( uint16_t ),
                  sizeof( serv3_svc_uuid ),
                  ( uint8_t * )&serv3_svc_uuid 
                }
           },

            /* Service 5002 Characteristic DCI_BLE_USER1_NAME
               Service 3 Characteristic 1 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER1_NAME
               Service 3 Characteristic 1 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char1_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER1_PASSWORD
               Service 3 Characteristic 2 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER1_PASSWORD
               Service 3 Characteristic 2 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char2_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER1_ROLE
               Service 3 Characteristic 3 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER1_ROLE
               Service 3 Characteristic 3 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char3_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER2_NAME
               Service 3 Characteristic 4 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER2_NAME
               Service 3 Characteristic 4 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char4_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER2_PASSWORD
               Service 3 Characteristic 5 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER2_PASSWORD
               Service 3 Characteristic 5 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char5_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER2_ROLE
               Service 3 Characteristic 6 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER2_ROLE
               Service 3 Characteristic 6 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char6_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER3_NAME
               Service 3 Characteristic 7 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER3_NAME
               Service 3 Characteristic 7 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char7_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER3_PASSWORD
               Service 3 Characteristic 8 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER3_PASSWORD
               Service 3 Characteristic 8 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char8_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER3_ROLE
               Service 3 Characteristic 9 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER3_ROLE
               Service 3 Characteristic 9 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char9_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER4_NAME
               Service 3 Characteristic 10 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER4_NAME
               Service 3 Characteristic 10 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char10_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER4_PASSWORD
               Service 3 Characteristic 11 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER4_PASSWORD
               Service 3 Characteristic 11 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char11_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_USER4_ROLE
               Service 3 Characteristic 12 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5002 Characteristic DCI_BLE_USER4_ROLE
               Service 3 Characteristic 12 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char12_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_1
               Service 3 Characteristic 13 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_1
               Service 3 Characteristic 13 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char13_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_2
               Service 3 Characteristic 14 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_2
               Service 3 Characteristic 14 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char14_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_3
               Service 3 Characteristic 15 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_3
               Service 3 Characteristic 15 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char15_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_SIMPLE_APP_CONTROL_WORD
               Service 3 Characteristic 16 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_SIMPLE_APP_CONTROL_WORD
               Service 3 Characteristic 16 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char16_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_SIMPLE_APP_STATUS_WORD
               Service 3 Characteristic 17 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5002 Characteristic DCI_SIMPLE_APP_STATUS_WORD
               Service 3 Characteristic 17 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char17_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_COUNTER
               Service 3 Characteristic 18 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_COUNTER
               Service 3 Characteristic 18 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char18_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_A
               Service 3 Characteristic 19 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_A
               Service 3 Characteristic 19 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char19_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_D
               Service 3 Characteristic 20 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_D
               Service 3 Characteristic 20 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char20_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_E
               Service 3 Characteristic 21 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_E
               Service 3 Characteristic 21 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char21_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_F
               Service 3 Characteristic 22 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_F
               Service 3 Characteristic 22 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char22_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_READING_1
               Service 3 Characteristic 23 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_READING_1
               Service 3 Characteristic 23 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char23_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_4
               Service 3 Characteristic 24 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_SETTING_4
               Service 3 Characteristic 24 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char24_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_READING_2
               Service 3 Characteristic 25 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_READING_2
               Service 3 Characteristic 25 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char25_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_LOG
               Service 3 Characteristic 26 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5002 Characteristic DCI_BLE_LOG
               Service 3 Characteristic 26 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char26_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_LOG_INDEX
               Service 3 Characteristic 27 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_LOG_INDEX
               Service 3 Characteristic 27 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char27_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_PRESENT_STATUS
               Service 3 Characteristic 28 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_PRESENT_STATUS
               Service 3 Characteristic 28 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char28_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5002 Characteristic DCI_BLE_TEST_DEVICE_TIME
               Service 3 Characteristic 29 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5002 Characteristic DCI_BLE_TEST_DEVICE_TIME
               Service 3 Characteristic 29 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv3_char29_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
       }
   },
   {
      TOTAL_ATTRIB_OF_SERV4,           /// Total number of attributes included in service 9
       {
           {
               { ESP_GATT_RSP_BY_APP },
               {
                  ESP_UUID_LEN_16,
                  ( uint8_t * )&primary_service_uuid,
                  ESP_GATT_PERM_READ,
                  sizeof( uint16_t ),
                  sizeof( serv4_svc_uuid ),
                  ( uint8_t * )&serv4_svc_uuid 
                }
           },

            /* Service 5005 Characteristic DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL
               Service 4 Characteristic 1 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5005 Characteristic DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL
               Service 4 Characteristic 1 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv4_char1_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5005 Characteristic DCI_BLE_DEVICE_PROFILE_ID
               Service 4 Characteristic 2 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5005 Characteristic DCI_BLE_DEVICE_PROFILE_ID
               Service 4 Characteristic 2 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv4_char2_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5005 Characteristic DCI_BLE_DEVICE_BONDING_INFO
               Service 4 Characteristic 3 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read_write
               }
           },

            /* Service 5005 Characteristic DCI_BLE_DEVICE_BONDING_INFO
               Service 4 Characteristic 3 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv4_char3_val_uuid,
                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5005 Characteristic DCI_BLE_ACTIVE_ACCESS_LEVEL
               Service 4 Characteristic 4 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5005 Characteristic DCI_BLE_ACTIVE_ACCESS_LEVEL
               Service 4 Characteristic 4 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv4_char4_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
       }
   },
   {
      TOTAL_ATTRIB_OF_SERV5,           /// Total number of attributes included in service 29
       {
           {
               { ESP_GATT_RSP_BY_APP },
               {
                  ESP_UUID_LEN_16,
                  ( uint8_t * )&primary_service_uuid,
                  ESP_GATT_PERM_READ,
                  sizeof( uint16_t ),
                  sizeof( serv5_svc_uuid ),
                  ( uint8_t * )&serv5_svc_uuid 
                }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 1 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 1 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char1_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 2 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 2 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char2_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 3 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 3 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char3_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 4 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 4 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char4_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 5 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 5 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char5_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 6 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 6 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char6_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 7 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 7 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char7_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 8 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 8 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char8_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 9 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 9 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char9_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 10 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 10 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char10_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 11 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 11 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char11_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 12 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 12 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char12_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 13 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 13 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char13_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
            /* Service 5004 Characteristic 
               Service 5 Characteristic 14 Declaration */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_16,
                   ( uint8_t * )&character_declaration_uuid,
                   ESP_GATT_PERM_READ,
                   CHAR_DECLARATION_SIZE,
                   CHAR_DECLARATION_SIZE,
                   ( uint8_t * )&char_prop_read
               }
           },

            /* Service 5004 Characteristic 
               Service 5 Characteristic 14 Value */
           {
               { ESP_GATT_RSP_BY_APP },
               {
                   ESP_UUID_LEN_128,
                   ( uint8_t * )&serv5_char14_val_uuid,
                   ESP_GATT_PERM_READ,
                   0,
                   0,
                   NULL
               }
           },
       }
   },
};
//*******************************************************
//******     BLE structures and constants.
//*******************************************************

extern const Ble_DCI::ble_target_info_st_td_t ble_dci_data_target_info;
extern const Ble_DCI::ble_group_info_st_td_t ble_dci_data_group_info;

// Ble bonding flag dcid will be accessed from app.c to retrieve and update the bonding info.
const Ble_DCI::CHAR_ID_TD DCI_DEVICE_BONDING_INFO_BLE_CHAR_ID = 156U;

static const uint32_t BLE_TOTAL_DCID_RAM_SIZE = 838U;

// ******************************************************* 
// ******     The Ble characteristic Id Counts.
// *******************************************************
static const Ble_DCI::CHAR_ID_TD DCI_BLE_DATA_TOTAL_CHAR_ID = 59U;
#endif
