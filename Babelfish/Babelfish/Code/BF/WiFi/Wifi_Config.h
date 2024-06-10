/**
 **********************************************************************************************
 * @file           WiFi_Config.h
 *
 * @brief           Contains the WiFi_Config Class.
 *
 * @details			This class will be used to read/write WIFI parameters
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef WIFI_CONFIG_H
   #define WIFI_CONFIG_H

#include "includes.h"
/**
 **********************************************************************************************
 * @brief                   Wlan Configuration
 * @details                 Contains functions to read/write wlan config.
 * @n						Configurable parameters are SSID, password, security type, channel
 * @n						number and band.
 **********************************************************************************************
 */
class WiFi_Config
{
	public:

		/**
		 * @brief Constructor
		 * @return this
		 */
		WiFi_Config( void ) {}

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		virtual ~WiFi_Config( void ) {}

		/**
		 * @brief                           Function to Read wlan SSID
		 * @param[in] _ssid:				Address (SSID read from DCID to this address)
		 * @return                          Status of read operation
		 * @retval							0 - READ_PASS
		 * @retval							1 - READ_FAIL
		 * @retval                          2 - READ_NOT_SUPPORTED
		 */
		virtual uint8_t Read_SSID( char_t* ssid ) = 0;

		/**
		 * @brief                           Function to Read wlan Password
		 * @param[in] passwd:				Address (Password read from DCID to this address)
		 * @return                          Status of read operation
		 * @retval							0 - READ_PASS
		 * @retval							1 - READ_FAIL
		 * @retval                          2 - READ_NOT_SUPPORTED
		 */

		virtual uint8_t Read_Password( char_t* passwd ) = 0;

		/**
		 * @brief                           Function to Read wlan channel number
		 * @param[in] channel:				Address (channel number read from DCID to this address)
		 * @return                          Status of read operation
		 * @retval							0 - READ_PASS
		 * @retval							1 - READ_FAIL
		 * @retval                          2 - READ_NOT_SUPPORTED
		 */

		virtual uint8_t Read_Channel( uint8_t* channel ) = 0;

		/**
		 * @brief                           Function to update wlan SSID
		 * @param[in] ssid:					Address (SSID written to DCID from this address)
		 * @return                          Status of write operation
		 * @retval							0 - WRITE_PASS
		 * @retval							1 - WRITE_FAIL
		 * @retval                          2 - WRITE_NOT_SUPPORTED
		 */
		virtual uint8_t Write_SSID( char_t* ssid ) = 0;

		/**
		 * @brief                           Function to update wlan Password
		 * @param[in] passwd:				Address (Password written to DCID from this address)
		 * @return                          Status of write operation
		 * @retval							0 - WRITE_PASS
		 * @retval							1 - WRITE_FAIL
		 * @retval                          2 - WRITE_NOT_SUPPORTED
		 */
		virtual uint8_t Write_Password( char_t* passwd ) = 0;

		/**
		 * @brief                           Function to update wlan channel number
		 * @param[in] channel:				Address(channel written to DCID from this address)
		 * @return                          Status of write operation
		 * @retval							0 - WRITE_PASS
		 * @retval							1 - WRITE_FAIL
		 * @retval                          2 - WRITE_NOT_SUPPORTED
		 */
		virtual uint8_t Write_Channel( uint8_t* channel ) = 0;

};

#endif
