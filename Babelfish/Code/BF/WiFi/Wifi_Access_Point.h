/**
 **********************************************************************************************
 * @file            Wifi_Access_point.h
 *
 * @brief           Contains the Access point Class
 *
 * @details			This class will be used WIFI Access point
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef WIFI_ACCESS_POINT_H
   #define WIFI_ACCESS_POINT_H

/**
 **********************************************************************************************
 * @brief        WIFI Access Point class.
 * @details      Contains the WIFI Access Point functions.
 **********************************************************************************************
 */
class Wifi_Access_Point
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

};

#endif
