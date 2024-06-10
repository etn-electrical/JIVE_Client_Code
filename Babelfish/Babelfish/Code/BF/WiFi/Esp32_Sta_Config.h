/**
 **********************************************************************************************
 * @file            Esp32_Sta_Config.h
 *
 * @brief           Contains the Esp32 Station Config Class..
 *
 * @details			This class will be used to Read/Write config of WIFI parameters
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef ESP32_STA_CONFIG_H
   #define ESP32_STA_CONFIG_H

#include "Includes.h"
#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Etn_Types.h"
#include "DCI_Data.h"
#include "DCI_Enums.h"
#include "Wifi_Config.h"

/**
 **********************************************************************************************
 * @brief                  ESP32 Station Configuration
 * @details                Contains functions to read/write WIFI configuration.
 **********************************************************************************************
 */
class ESP32_Sta_Config : public WiFi_Config
{
	public:
		/**
		 * @brief Constructor
		 * @details
		 * @return this
		 */
		ESP32_Sta_Config( void );

		/**
		 *  @brief Destructor
		 *	@details
		 *  @return None
		 */
		~ESP32_Sta_Config( void );

		/**
		 * @brief                   Function to Read wlan SSID
		 * @param[in]		        char
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read SSID PASS
		 * @n						1 - CONFIG Read SSID FAIL
		 * @n                       2 - CONFIG READ SSID NOT SUPPORT
		 */
		uint8_t Read_SSID( char_t* client_AP_ssid );

		/**
		 * @brief                   Function to Read Wlan Password
		 * @param[in]		        char
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read Password PASS
		 * @n						1 - CONFIG Read Password FAIL
		 * @n                       2 - CONFIG Read Password NOT SUPPORT
		 */
		uint8_t Read_Password( char_t* client_AP_passwd );

		/**
		 * @brief                   Function to Read Wlan Security
		 * @param[in]		        int32_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read security PASS
		 * @n						1 - CONFIG Read security FAIL
		 * @n                       2 - CONFIG Read security NOT SUPPORT
		 */
		uint8_t Read_Security( int32_t* client_AP_security );

		/**
		 * @brief                   Function to Read Wlan Channel
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read channel PASS
		 * @n						1 - CONFIG Read channel FAIL
		 * @n                       2 - CONFIG Read Channel NOT SUPPORT
		 */
		uint8_t Read_Channel( uint8_t* client_AP_channel );

		/**
		 * @brief                   Function to Read Wlan Band
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read BAND PASS
		 * @n						1 - CONFIG Read BAND FAIL
		 * @n                       2 - CONFIG Read BAND NOT SUPPORT
		 */
		uint8_t Read_Band( uint8_t* client_AP_band );

		/**
		 * @brief                   Function to Update Wlan retry_count
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Read Retry Count PASS
		 * @n						1 - CONFIG Read Retry Count FAIL
		 * @n                       2 - CONFIG Read Retry Count NOT SUPPORT
		 */
		uint8_t Read_Retry( uint8_t* client_AP_retry );

		/**
		 * @brief                   Function to Update Wlan SSID
		 * @param[in]		        char
		 * @return                  uint8_t
		 * @n						0 - CONFIG Write SSID PASS
		 * @n						1 - CONFIG Write SSID FAIL
		 * @n                       2 - CONFIG Write SSID NOT SUPPORT
		 */
		uint8_t Write_SSID( char_t* client_AP_ssid );

		/**
		 * @brief                   Function to Update Wlan Password
		 * @param[in]		        char
		 * @return                  none
		 * @n						0 - CONFIG Write Password PASS
		 * @n						1 - CONFIG Write Password FAIL
		 * @n                       2 - CONFIG Write Password NOT SUPPORT
		 */
		uint8_t Write_Password( char_t* client_AP_passwd );

		/**
		 * @brief                   Function to Update Wlan Security
		 * @param[in]		        int32_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Write Security PASS
		 * @n						1 - CONFIG Write Security FAIL
		 * @n                       2 - CONFIG Write Security NOT SUPPORT
		 */
		uint8_t Write_Security( int32_t* client_AP_security );

		/**
		 * @brief                   Function to Update Wlan Channel
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Write channel PASS
		 * @n						1 - CONFIG Write channel FAIL
		 * @n                       2 - CONFIG Write Channel NOT SUPPORT
		 */
		uint8_t Write_Channel( uint8_t* client_AP_channel );

		/**
		 * @brief                   Function to Update Wlan Band
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Write BAND PASS
		 * @n						1 - CONFIG Write BAND FAIL
		 * @n                       2 - CONFIG Write BAND NOT SUPPORT
		 */
		uint8_t Write_Band( uint8_t* client_AP_band );

		/**
		 * @brief                   Function to Update Wlan retry_count
		 * @param[in]		        uint8_t
		 * @return                  uint8_t
		 * @n						0 - CONFIG Write Retry Count PASS
		 * @n						1 - CONFIG Write Retry Count FAIL
		 * @n                       2 - CONFIG Write Retry Count NOT SUPPORT
		 */
		uint8_t Write_Retry( uint8_t* client_AP_retry );

	private:
		DCI_Owner* m_ssid_owner;
		DCI_Owner* m_passwd_owner;
		DCI_Owner* m_security_owner;
		DCI_Owner* m_channel_owner;
		DCI_Owner* m_band_owner;
		DCI_Owner* m_retry_owner;
};

#endif
