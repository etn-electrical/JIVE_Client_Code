/**
 *****************************************************************************************
 *	@file		Esp32_Station.cpp

 *	@details    This file contains wifi station mode init/scan/connect functionality
 *	@copyright  2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Esp32_Station.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "Esp32_Sta_Config.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_sntp.h"

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
static const char* TAG = "ESP32 STATION";

uint8_t* client_AP_channel = nullptr;
uint8_t* client_AP_retry = nullptr;
int32_t* client_AP_Security = nullptr;
wifi_ap_record_t* list = nullptr;

/**
 *****************************************************************************************
 * @brief        Wifi event handler function.
 * @details
 *****************************************************************************************
 */
static esp_err_t event_handler( void* ctx, system_event_t* event )
{
	switch ( event->event_id )
	{
		case SYSTEM_EVENT_STA_GOT_IP:
			break;

		case SYSTEM_EVENT_STA_DISCONNECTED:

			esp_wifi_connect();

			ESP_LOGE( TAG, "connect to the AP failed...\n" );
			break;

		case SYSTEM_EVENT_SCAN_DONE:
		{
			uint16_t apCount = 0;
			int32_t return_check = ESP_FAIL;
			esp_wifi_scan_get_ap_num( &apCount );
			printf( "Number of access points found: %d\n", event->event_info.scan_done.number );
			list = reinterpret_cast<wifi_ap_record_t*>
				( Ram::Allocate( sizeof ( wifi_ap_record_t ) * apCount ) );

			return_check = esp_wifi_scan_get_ap_records( &apCount, list );
			if ( return_check == ESP_OK )
			{
				ESP_LOGI( TAG, "Get AP records successful...\n" );
			}
			else
			{
				ESP_LOGE( TAG, "Get AP records FAILED...\n" );
			}

			printf( "===================================================================================\n" );
			printf( "             SSID            |    RSSI    |   CHANNEL  |         AUTH              \n" );
			printf( "===================================================================================\n" );
			for ( int i = 0; i < apCount; i++ )
			{
				switch ( list[i].authmode )
				{
					case WIFI_AUTH_OPEN:
						printf( "%25s    |% 4d        |%5d        |%20s\n", list[i].ssid, list[i].rssi, list[i].primary,
								"WIFI_AUTH_OPEN" );
						break;

					case WIFI_AUTH_WEP:
						printf( "%25s    |% 4d        |%5d        |%20s\n", list[i].ssid, list[i].rssi, list[i].primary,
								"WIFI_AUTH_WEP" );
						break;

					case WIFI_AUTH_WPA_PSK:
						printf( "%25s    |% 4d        |%5d        |%20s\n", list[i].ssid, list[i].rssi, list[i].primary,
								"WIFI_AUTH_WPA_PSK" );
						break;

					case WIFI_AUTH_WPA2_PSK:
						printf( "%25s    |% 4d        |%5d        |%20s\n", list[i].ssid, list[i].rssi, list[i].primary,
								"WIFI_AUTH_WPA2_PSK" );
						break;

					case WIFI_AUTH_WPA_WPA2_PSK:
						printf( "%25s    |% 4d        |%5d        |%20s\n", list[i].ssid, list[i].rssi, list[i].primary,
								"WIFI_AUTH_WPA_WPA2_PSK" );
						break;

					default:
						printf( "%25s    |% 4d        |%5d        |%20s\n", list[i].ssid, list[i].rssi, list[i].primary,
								"unknown" );
						break;
				}
			}
			printf( "\n\n" );
		}
		break;

		default:
			break;
	}

	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Esp32_Station::Init( void )
{
	/// Network Adaptor Initialization
	tcpip_adapter_init();
	ESP_ERROR_CHECK( esp_event_loop_init( event_handler, NULL ) );

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	ESP_ERROR_CHECK( esp_wifi_init( &cfg ) );

	ESP32_Sta_Config wconfig;
	wifi_config_t wifi_config;

	/// pointer to read config parameter
	client_AP_channel = reinterpret_cast<uint8_t*>( Ram::Allocate( sizeof( uint8_t ) ) );
	client_AP_retry = reinterpret_cast<uint8_t*>( Ram::Allocate( sizeof( uint8_t ) ) );
	client_AP_Security = reinterpret_cast<int32_t*>( Ram::Allocate( sizeof( uint16_t ) ) );

	/// configure station mode
	ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_STA ) );
	ESP_ERROR_CHECK( esp_wifi_set_storage( WIFI_STORAGE_RAM ) );

	esp_wifi_get_config( static_cast<wifi_interface_t>( ESP_IF_WIFI_STA ), &wifi_config );

	/// reading Wifi parameters
	wconfig.Read_SSID( reinterpret_cast<char*>( &wifi_config.sta.ssid[0] ) );

	wconfig.Read_Security( reinterpret_cast<int32_t*>( client_AP_Security ) );

	if ( *client_AP_Security != WIFI_AUTH_OPEN )
	{
		wconfig.Read_Password( reinterpret_cast<char*>( &wifi_config.sta.password[0] ) );
	}

	wconfig.Read_Channel( client_AP_channel );
	wifi_config.sta.channel = *client_AP_channel;

	/// start the WiFi station mode
	ESP_ERROR_CHECK( esp_wifi_set_config( static_cast<wifi_interface_t>( ESP_IF_WIFI_STA ), &wifi_config ) );
	ESP_ERROR_CHECK( esp_wifi_start() );
	ESP_LOGI( TAG, "WIFI_init_station finished...\n" );

	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Esp32_Station::Deinit( void )
{
	int32_t return_check = ESP_FAIL;
	bool_t ret_val = true;

	ESP_LOGI( TAG, "Deinitialize WIFI driver...\n" );
	return_check = esp_wifi_stop();
	if ( return_check == ESP_OK )
	{
		return_check = esp_wifi_deinit();
		if ( return_check == ESP_OK )
		{
			ESP_LOGI( TAG, "Deinitialize Successful...\n" );
			ret_val = ESP_OK;
		}
		else
		{
			ESP_LOGI( TAG, "Deinitialize Failed...\n" );
		}
	}
	else
	{
		ESP_LOGI( TAG, "Failed to stop WiFi Mode...\n" );
	}

	// RAM deallocation
	Ram::De_Allocate( client_AP_channel );
	Ram::De_Allocate( client_AP_retry );
	Ram::De_Allocate( client_AP_Security );
	Ram::De_Allocate( list );

	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Station::Scan( void )
{
	int32_t return_check = ESP_FAIL;

	ESP_LOGI( TAG, "WIFI_Scan started...\n" );
	wifi_scan_config_t Scanconfig;

	Scanconfig.ssid = NULL;
	Scanconfig.bssid = NULL;
	Scanconfig.channel = 0;
	Scanconfig.show_hidden = true;
	Scanconfig.scan_type = WIFI_SCAN_TYPE_ACTIVE;

	return_check = esp_wifi_scan_start( &Scanconfig, true );

	if ( return_check == ESP_OK )
	{
		ESP_LOGI( TAG, "WIFI_Scan completed...\n" );
		esp_wifi_scan_stop();
	}
	else
	{
		ESP_LOGE( TAG, "WIFI_Scan failure... \n" );
		esp_wifi_scan_stop();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Esp32_Station::Connect( void )
{
	int32_t return_check = ESP_FAIL;
	bool_t ret_val = true;

	ESP_LOGI( TAG, "WIFI_connecting....\n \n" );
	return_check = esp_wifi_connect();

	if ( return_check == ESP_OK )
	{
		ret_val = ESP_OK;
	}

	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Esp32_Station::Disconnect()
{
	int32_t return_check = ESP_FAIL;
	bool_t ret_val = true;

	ESP_LOGI( TAG, "WIFI Disconnect...\n\n" );
	return_check = esp_wifi_disconnect();

	if ( return_check == ESP_OK )
	{
		ESP_LOGI( TAG, "Disconnection Successful..." );
		ret_val = ESP_OK;
	}
	else
	{
		ESP_LOGE( TAG, "Disconnection Failed...\n" );
	}

	return ( ret_val );
}
