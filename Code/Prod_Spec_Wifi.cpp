/*
 **********************************************************************************************
 * @file            Prod_Spec_Debug_Print.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product.
 *                  Adopter can initialize the Debug print functionality
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Prod_Spec_Wifi.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "Prod_Spec_Debug.h"
#include "Includes.h"
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "Esp32_Station.h"
#include "Esp32_Access_point.h"

/*
 *
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
static const char* TAG = "ESP32 MAIN";

static Esp32_Station station;
static Esp32_Access_Point access_point;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_WIFI_Init( void )
{
	PROD_SPEC_DEBUG_PRINT( DBG_MSG_INFO, "Inside Wifi init" );

#ifdef WIFI_STATION_MODE

	WIFI_Station_Init();

#elif defined WIFI_ACCESS_POINT_MODE

	WIFI_Access_Point_Init();

#endif

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void WIFI_Station_Init( void )
{
	PROD_SPEC_DEBUG_PRINT( DBG_MSG_INFO, "Inside Wifi Station init" );

	/// WiFi Station Mode Initialization
	if ( station.Init() == ESP_OK )
	{
		/// uncomment to view the Available APs and their details
		// station.Scan();

		/// connects to the configured AP in DCI sheet
		if ( station.Connect() != ESP_OK )
		{
			/// connection fails to AP disconnect from Wifi
			ESP_LOGE( TAG, "Connection failed\n" );
			station.Disconnect();
		}
		else
		{
			/// do nothing
		}
	}
	else
	{
		/// if Initialization failed Deinit the Wifi station mode
		ESP_LOGE( TAG, "Init failed\n" );
		station.Deinit();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void WIFI_Access_Point_Init( void )
{
	PROD_SPEC_DEBUG_PRINT( DBG_MSG_INFO, "Inside Wifi Access Point init" );

	/// WiFi Access point Mode Initialization
	if ( access_point.Init() != ESP_OK )
	{
		/// connection fails
		ESP_LOGE( TAG, "Connection failed\n" );
	}
}
