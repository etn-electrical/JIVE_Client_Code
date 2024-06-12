/**
 *****************************************************************************************
 *	@file		Esp32_Sta_Config.cpp

 *	@details 	This file contains the function definition of config parameters
 *	@copyright 	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "esp_log.h"
#include "Esp32_Sta_Config.h"
/*
 *********************************************************************************************
 *		Static Variables
 *********************************************************************************************
 */
static const char* TAG = "ESP32 CONFIG";

/**
 **********************************************************************************************
 * @brief
 * @details			This enum used to the return type for config read call
 **********************************************************************************************
 */
enum config_Read_enum_t
{
	READ_PASS = 0,
	READ_FAIL,
	READ_NOT_SUPPORT
};

/**
 **********************************************************************************************
 * @brief
 * @details			This enum used to the return type for config Write call
 **********************************************************************************************
 */
enum config_Write_enum_t
{
	WRITE_PASS = 0,
	WRITE_FAIL,
	WRITE_NOT_SUPPORT
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ESP32_Sta_Config::ESP32_Sta_Config( void )
{
	m_ssid_owner = new DCI_Owner( DCI_CLIENT_AP_SSID_DCID );

	m_passwd_owner = new DCI_Owner( DCI_CLIENT_AP_PASSPHRASE_DCID );

	m_security_owner = new DCI_Owner( DCI_CLIENT_AP_SECURITY_DCID );

	m_band_owner = new DCI_Owner( DCI_CLIENT_AP_BAND_DCID );

	m_channel_owner = new DCI_Owner( DCI_CLIENT_AP_CHANNEL_DCID );

	m_retry_owner = new DCI_Owner( DCI_CLIENT_AP_RETRY_COUNT_DCID );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ESP32_Sta_Config::~ESP32_Sta_Config()
{
	delete m_ssid_owner;
	delete m_passwd_owner;
	delete m_security_owner;
	delete m_band_owner;
	delete m_channel_owner;
	delete m_retry_owner;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Read_SSID( char_t* client_AP_ssid )
{
	m_ssid_owner->Check_Out( client_AP_ssid );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Read_Password( char_t* client_AP_passwd )
{
	m_passwd_owner->Check_Out( client_AP_passwd );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Read_Security( int32_t* client_AP_security )
{
	m_security_owner->Check_Out( *client_AP_security );
	ESP_LOGI( TAG, "configure security value to '0' for to connects open Authentication AP\n" );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

uint8_t ESP32_Sta_Config::Read_Channel( uint8_t* client_AP_channel )
{
	m_channel_owner->Check_Out( *client_AP_channel );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Read_Band( uint8_t* client_AP_band )
{
	m_band_owner->Check_Out( *client_AP_band );
	ESP_LOGI( TAG, "Default configure band for ESP32 is 2.4GHZ\n" );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Read_Retry( uint8_t* client_AP_retry )
{
	m_retry_owner->Check_Out( *client_AP_retry );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Write_SSID( char_t* client_AP_ssid )
{
	uint8_t ret_val = WRITE_FAIL;

	if ( *client_AP_ssid != nullptr )
	{
		m_ssid_owner->Check_In( client_AP_ssid );
		ret_val = WRITE_PASS;
	}
	else
	{
		ESP_LOGE( TAG, "SSID is NULL\n" );
		ret_val = WRITE_FAIL;
	}
	return ( ( uint8_t )ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Write_Password( char_t* client_AP_passwd )
{
	uint8_t ret_val = WRITE_FAIL;

	if ( *client_AP_passwd != nullptr )
	{
		m_passwd_owner->Check_In( client_AP_passwd );
		ret_val = WRITE_PASS;
	}
	else
	{
		ESP_LOGE( TAG, "Password is NULL\n" );
		ret_val = WRITE_FAIL;
	}
	return ( ( uint8_t )ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Write_Security( int32_t* client_AP_security )
{
	ESP_LOGE( TAG, "Write Security Not supported for ESP32\n" );
	return ( ( uint8_t )WRITE_NOT_SUPPORT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Write_Channel( uint8_t* client_AP_channel )
{
	uint8_t ret_val = WRITE_FAIL;

	if ( *client_AP_channel != nullptr )
	{
		m_channel_owner->Check_In( *client_AP_channel );
		ret_val = WRITE_PASS;
	}
	else
	{
		ESP_LOGE( TAG, "Channel is NULL\n" );
		ret_val = WRITE_FAIL;
	}

	return ( ( uint8_t )ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Write_Band( uint8_t* client_AP_band )
{
	ESP_LOGE( TAG, "Write band Not supported for ESP32\n" );
	return ( ( uint8_t )WRITE_NOT_SUPPORT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t ESP32_Sta_Config::Write_Retry( uint8_t* client_AP_retry )
{
	uint8_t ret_val = WRITE_FAIL;

	if ( client_AP_retry != NULL )
	{
		m_channel_owner->Check_In( *client_AP_retry );
		ret_val = WRITE_PASS;
	}
	else
	{
		ESP_LOGE( TAG, "Retry Count is NULL\n" );
		ret_val = WRITE_FAIL;
	}

	return ( ( uint8_t )ret_val );
}
