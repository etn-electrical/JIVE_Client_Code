/**
 *****************************************************************************************
 *	@file		Esp32_AP_Config.cpp

 *	@details 	This file contains the function definition of config parameters
 *	@copyright 	2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "esp_log.h"
#include "Esp32_AP_Config.h"
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
Esp32_AP_Config::Esp32_AP_Config( void )
{
	m_ssid_owner = new DCI_Owner( DCI_SOURCE_AP_SSID_DCID );

	m_passwd_owner = new DCI_Owner( DCI_SOURCE_AP_PASSPHRASE_DCID );

	m_channel_owner = new DCI_Owner( DCI_SOURCE_AP_CHANNEL_DCID );

	m_max_conn_owner = new DCI_Owner( DCI_SOURCE_AP_MAX_CONN_DCID );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Esp32_AP_Config::~Esp32_AP_Config()
{
	delete m_ssid_owner;
	delete m_passwd_owner;
	delete m_channel_owner;
	delete m_max_conn_owner;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Esp32_AP_Config::Read_SSID( char_t* source_AP_ssid )
{
	m_ssid_owner->Check_Out( source_AP_ssid );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Esp32_AP_Config::Read_Password( char_t* source_AP_passwd )
{
	m_passwd_owner->Check_Out( source_AP_passwd );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

uint8_t Esp32_AP_Config::Read_Channel( uint8_t* source_AP_channel )
{
	m_channel_owner->Check_Out( *source_AP_channel );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Esp32_AP_Config::Read_Max_Conn( uint8_t* source_AP_max_conn )
{
	m_max_conn_owner->Check_Out( *source_AP_max_conn );
	return ( ( uint8_t )READ_PASS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Esp32_AP_Config::Write_SSID( char_t* source_AP_ssid )
{
	uint8_t ret_val = WRITE_FAIL;

	if ( *source_AP_ssid != nullptr )
	{
		m_ssid_owner->Check_In( source_AP_ssid );
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
uint8_t Esp32_AP_Config::Write_Password( char_t* source_AP_passwd )
{
	uint8_t ret_val = WRITE_FAIL;

	if ( *source_AP_passwd != nullptr )
	{
		m_passwd_owner->Check_In( source_AP_passwd );
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
uint8_t Esp32_AP_Config::Write_Channel( uint8_t* source_AP_channel )
{
	uint8_t ret_val = WRITE_FAIL;

	if ( *source_AP_channel != nullptr )
	{
		m_channel_owner->Check_In( *source_AP_channel );
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
uint8_t Esp32_AP_Config::Write_Max_Conn( uint8_t* source_AP_max_conn )
{
	uint8_t ret_val = WRITE_FAIL;

	if ( *source_AP_max_conn != nullptr )
	{
		m_channel_owner->Check_In( *source_AP_max_conn );
		ret_val = WRITE_PASS;
	}
	else
	{
		ESP_LOGE( TAG, "Max Connection is NULL\n" );
		ret_val = WRITE_FAIL;
	}

	return ( ( uint8_t )ret_val );
}