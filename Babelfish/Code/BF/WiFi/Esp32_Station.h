/**
 **********************************************************************************************
 * @file            Esp32_Station.h
 *
 * @brief           This class will be used to WIFI station Mode
 *
 * @details			This class will be used to station Init/Scan/Connect/Disconnect
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef ESP32_STATION_H
   #define ESP32_STATION_H

#include "Wifi_Station.h"

/**
 **********************************************************************************************
 * @brief        Generic Wifi Station class.
 * @details      Contains generic Wifi functions.
 **********************************************************************************************
 */
class Esp32_Station : public Station
{
	public:

		/**
		 * @brief                   Function to Initialize WLAN
		 * @param[in]		        None
		 * @return                  Bool
		 * @retval					0 - Command Passed
		 * @retval	                1 - Command failed
		 */
		bool Init( void );

		/**
		 * @brief                   Function to scan WLAN
		 * @param[in]		        None
		 * @return
		 */
		void Scan( void );

		/**
		 * @brief                   Function to Uninitialize WLAN
		 * @param[in]		        None
		 * @return                  Bool
		 * @retval					0 - Command Passed
		 * @retval	                1 - Command failed
		 */
		bool Deinit( void );

		/**
		 * @brief                   Function to Connect to an AP
		 * @param[in]		        None
		 * @return                  Bool
		 * @retval					0 - Command Passed
		 * @retval	                1 - Command failed
		 */
		bool Connect( void );

		/**
		 * @brief                   Function to Disconnect from AP
		 * @param[in]		        None
		 * @return                  Bool
		 * @nretval				    0 - Command Passed
		 * @retval	                1 - Command failed
		 */
		bool Disconnect( void );

};

#endif
