/**
 **********************************************************************************************
 * @file            WiFi_Station.h
 *
 * @brief           Contains the Station Class..
 *
 * @details			This class will be used WIFI station mode init/scan/connect/disconnect
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef WIFI_STATION_H
   #define WIFI_STATION_H

/**
 **********************************************************************************************
 * @brief        WIFI Station class.
 * @details      Contains the WIFI staion mode init/scan/connect/disconnect functions.
 **********************************************************************************************
 */
class Station
{
	public:

		/**
		 * @brief                   Function to Initialize WLAN
		 * @param[in]		        None
		 * @return                  Bool
		 * @n						0 - Initialization successful
		 * @n                       1 - Initialization Failed
		 */
		virtual bool Init( void ) = 0;

		/**
		 * @brief                   Function to scan WLAN
		 * @param[in]		        None
		 * @return
		 */
		virtual void Scan( void ) = 0;

		/**
		 * @brief                   Function to Uninitialize WLAN
		 * @param[in]		        None
		 * @return                  Bool
		 * @n						0 - De-initialization successful
		 * @n                       1 - De-initialization failed
		 */
		virtual bool Deinit( void ) = 0;

		/**
		 * @brief                   Function to Connect to an AP
		 * @param[in]		        None
		 * @return                  Bool
		 * @n						0 - Connection successful
		 * @n                       1 - Connection Failed
		 */
		virtual bool Connect( void ) = 0;

		/**
		 * @brief                   Function to Disconnect from AP
		 * @param[in]		        None
		 * @return                  Bool
		 * @n						0 - Disconnection successful
		 * @n                       1 - Disconnection failed
		 */
		virtual bool Disconnect( void ) = 0;

};

#endif
