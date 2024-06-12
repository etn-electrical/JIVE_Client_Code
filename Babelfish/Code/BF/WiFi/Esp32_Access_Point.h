/**
 **********************************************************************************************
 * @file            Esp32_Access_Point.h
 *
 * @brief           This class will be used to WIFI Access_Point Mode
 *
 * @details			This class will be used to implement wifi access point functionality
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef ESP32_ACCESS_POINT_H
   #define ESP32_ACCESS_POINT_H

#include "Wifi_Access_Point.h"

/**
 **********************************************************************************************
 * @brief        Generic Wifi Access Point class.
 * @details      Contains generic Wifi functions.
 **********************************************************************************************
 */
class Esp32_Access_Point : public Wifi_Access_Point
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

};

#endif
