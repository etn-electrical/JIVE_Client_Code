/**
 **********************************************************************************************
 * @file            Esp32_AP_Config.h
 *
 * @brief           Contains the Esp32 Access Point Config Class..
 *
 * @details			This class will be used to Read/Write config of WIFI parameters
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef Esp32_AP_Config_H
   #define Esp32_AP_Config_H

#include "Includes.h"
#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Etn_Types.h"
#include "DCI_Data.h"
#include "DCI_Enums.h"
#include "Wifi_Config.h"

/**
 **********************************************************************************************
 * @brief                  ESP32 Access Point Configuration
 * @details                Contains functions to read/write WIFI configuration.
 **********************************************************************************************
 */
class Esp32_AP_Config : public WiFi_Config
{
	public:
		/**
		 * @brief Constructor
		 * @details
		 * @return this
		 */
		Esp32_AP_Config( void );

		/**
		 *  @brief Destructor
		 *	@details
		 *  @return None
		 */
		~Esp32_AP_Config( void );

		/**
		 * @brief                   Function to Read wlan SSID
		 * @param[in]		        char
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read SSID PASS
		 * @n						1 - CONFIG Read SSID FAIL
		 * @n                       2 - CONFIG READ SSID NOT SUPPORT
		 */
		uint8_t Read_SSID( char_t* source_AP_ssid );

		/**
		 * @brief                   Function to Read Wlan Password
		 * @param[in]		        char
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read Password PASS
		 * @n						1 - CONFIG Read Password FAIL
		 * @n                       2 - CONFIG Read Password NOT SUPPORT
		 */
		uint8_t Read_Password( char_t* source_AP_passwd );

		/**
		 * @brief                   Function to Read Wlan Channel
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read channel PASS
		 * @n						1 - CONFIG Read channel FAIL
		 * @n                       2 - CONFIG Read Channel NOT SUPPORT
		 */
		uint8_t Read_Channel( uint8_t* source_AP_channel );

		/**
		 * @brief                   Function to Update wlan Read max connection
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read Retry Count PASS
		 * @n						1 - CONFIG Read Retry Count FAIL
		 * @n                       2 - CONFIG Read Retry Count NOT SUPPORT
		 */
		uint8_t Read_Max_Conn( uint8_t* source_AP_max_conn );

		/**
		 * @brief                   Function to Update Wlan SSID
		 * @param[in]		        char
		 * @return                  uint8_t
		 * @n						0 - CONFIG Write SSID PASS
		 * @n						1 - CONFIG Write SSID FAIL
		 * @n                       2 - CONFIG Write SSID NOT SUPPORT
		 */
		uint8_t Write_SSID( char_t* source_AP_ssid );

		/**
		 * @brief                   Function to Update Wlan Password
		 * @param[in]		        char
		 * @return                  none
		 * @n						0 - CONFIG Write Password PASS
		 * @n						1 - CONFIG Write Password FAIL
		 * @n                       2 - CONFIG Write Password NOT SUPPORT
		 */
		uint8_t Write_Password( char_t* source_AP_passwd );

		/**
		 * @brief                   Function to Update Wlan Channel
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Write channel PASS
		 * @n						1 - CONFIG Write channel FAIL
		 * @n                       2 - CONFIG Write Channel NOT SUPPORT
		 */
		uint8_t Write_Channel( uint8_t* source_AP_channel );

		/**
		 * @brief                   Function to update max connection
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Write Max connection PASS
		 * @n						1 - CONFIG Write Max connection FAIL
		 * @n                       2 - CONFIG Write Max connection NOT SUPPORT
		 */
		uint8_t Write_Max_Conn( uint8_t* source_AP_max_conn );

	private:
		DCI_Owner* m_ssid_owner;
		DCI_Owner* m_passwd_owner;
		DCI_Owner* m_channel_owner;
		DCI_Owner* m_max_conn_owner;
};

#endif
