/**
 **********************************************************************************************
 * @file            BLE-ESP32 configuration file for
 *
 * @brief           This file contains BLE ESP32 configuration.
 *
 * @details
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef BLE_CONFIG_ESP32_H
#define BLE_CONFIG_ESP32_H

#define TEST_DEVICE_NAME "BLE_TEST"

#ifdef ESP32_SETUP
#define MAX_GATTS_ATTIRIBUTE        100			/// Maximum GATTS attribute
#define CHAR_DECLARATION_SIZE       ( sizeof( uint8_t ) )

// *********************************************************************************************
// *****       Define for required byte size for storing UUIDs(16btis, 32bits, 128bits)
// *********************************************************************************************
#define   BYTES_UUID_16BIT                2		/// Required byte size the for store UUIDs 16btis
#define   BYTES_UUID_32BIT                4		/// Required byte size the for store UUIDs 32btis
#define   BYTES_UUID_128BIT               16	/// Required byte size the for store UUIDs 128btis

#endif

#endif	// #ifndef BLE_CONFIG_ESP32_H
