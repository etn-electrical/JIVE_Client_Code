/**
 *****************************************************************************************
 *	@file		Esp32_Access_Point.cpp

 *	@details    This file contains wifi access point functionality
                When using ESP32 in soft access point mode, it builds its own WiFi network.
                All devices within the range of this WiFi network can connect to it
                ( using the specified network name and password ). All devices connected
                will act as a station and ESP32 as a soft access point.

 *	@copyright  2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Esp32_Access_Point.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "Esp32_AP_Config.h"

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
static const char* TAG = "wifi softAP";
static uint8_t source_AP_channel = 0U;
static uint8_t source_AP_max_conn = 0U;

/**
 *****************************************************************************************
 * @brief        Wifi event handler function.
 * @details
 *****************************************************************************************
 */
static void wifi_event_handler( void* arg, esp_event_base_t event_base,
								int32_t event_id, void* event_data )
{
	if ( event_id == WIFI_EVENT_AP_STACONNECTED )
	{
		wifi_event_ap_staconnected_t* event = ( wifi_event_ap_staconnected_t* ) event_data;
		ESP_LOGI( TAG, "station " MACSTR " join, AID=%d",
				  MAC2STR( event->mac ), event->aid );
	}
	else if ( event_id == WIFI_EVENT_AP_STADISCONNECTED )
	{
		wifi_event_ap_stadisconnected_t* event = ( wifi_event_ap_stadisconnected_t* ) event_data;
		ESP_LOGI( TAG, "station " MACSTR " leave, AID=%d",
				  MAC2STR( event->mac ), event->aid );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Esp32_Access_Point::Init( void )
{

	ESP_LOGI( TAG, "ESP_WIFI_MODE_AP" );

	ESP_ERROR_CHECK( esp_netif_init() );
	ESP_ERROR_CHECK( esp_event_loop_create_default() );
	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	ESP_ERROR_CHECK( esp_wifi_init( &cfg ) );

	ESP_ERROR_CHECK( esp_event_handler_instance_register( WIFI_EVENT,
														  ESP_EVENT_ANY_ID,
														  &wifi_event_handler,
														  NULL,
														  NULL ) );

	Esp32_AP_Config wconfig;
	wifi_config_t wifi_config;

	/// configure access point mode
	ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_AP ) );
	ESP_ERROR_CHECK( esp_wifi_set_storage( WIFI_STORAGE_RAM ) );

	esp_wifi_get_config( static_cast<wifi_interface_t>( ESP_IF_WIFI_AP ), &wifi_config );

	/// reading Wifi parameters
	wconfig.Read_SSID( reinterpret_cast<char*>( &wifi_config.ap.ssid[0] ) );
	wifi_config.ap.ssid_len = strlen( reinterpret_cast<char*>( &wifi_config.ap.ssid[0] ) );
	wconfig.Read_Password( reinterpret_cast<char*>( &wifi_config.ap.password[0] ) );
	wconfig.Read_Channel( &source_AP_channel );
	wifi_config.ap.channel = source_AP_channel;
	wconfig.Read_Max_Conn( &source_AP_max_conn );
	wifi_config.ap.max_connection = source_AP_max_conn;
	wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;


	/// start the WiFi Access Point mode
	ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_AP ) );
	ESP_ERROR_CHECK( esp_wifi_set_config( static_cast<wifi_interface_t>( ESP_IF_WIFI_AP ), &wifi_config ) );
	ESP_ERROR_CHECK( esp_wifi_start() );
	ESP_LOGI( TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
			  wifi_config.ap.ssid, wifi_config.ap.password, wifi_config.ap.channel );

	return ( ESP_OK );
}
