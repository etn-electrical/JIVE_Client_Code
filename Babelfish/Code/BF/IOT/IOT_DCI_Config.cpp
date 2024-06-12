/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "IOT_DCI_Config.h"
#include "IOT_DCI_Data.h"
#include "String_Sanitizer.hpp"
#include "String_Sanitize_Whitelists.h"
#include "StdLib_MV.h"
#include "Ram.h"

const BF_Lib::String_Sanitizer uuid_whitelist( uuid_sanitize_lookup );
const BF_Lib::String_Sanitizer conn_str_whitelist( conn_str_sanitize_lookup );
const BF_Lib::String_Sanitizer str_whitelist( whitelist_str_sanitize_lookup );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_DCI_Config::IOT_DCI_Config( const iot_config_param_t* config_struct ) :
	m_owner_iot_enable( config_struct->iot_enable ),
	m_owner_iot_connection_status( config_struct->iot_connection_status ),
	m_owner_proxy_enable( config_struct->proxy_enable ),
	m_owner_proxy_server_address( config_struct->proxy_server_address ),
	m_owner_proxy_server_port( config_struct->proxy_server_port ),
	m_owner_proxy_username( config_struct->proxy_username ),
	m_owner_proxy_password( config_struct->proxy_password ),
	m_owner_device_guid( config_struct->device_guid ),
	m_owner_device_profile( config_struct->device_profile ),
	m_owner_iot_connection_string( config_struct->iot_connection_string ),
	m_owner_data_update_rate( config_struct->data_update_rate ),
	m_owner_cadence_update_rate( config_struct->cadence_update_rate ),
	m_owner_iot_conn_status_reason( config_struct->iot_conn_status_reason )
{
	m_owner_iot_enable->Attach_Callback( Update_IOT_Config_CB_static,
										 reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
										 DCI_ACCESS_POST_SET_RAM_CMD_MSK );

	m_owner_proxy_enable->Attach_Callback( Update_IOT_Config_CB_static,
										   reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
										   DCI_ACCESS_POST_SET_RAM_CMD_MSK );

	m_owner_proxy_server_address->Attach_Callback( Update_IOT_Config_CB_static,
												   reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
												   DCI_ACCESS_POST_SET_RAM_CMD_MSK );

	m_owner_proxy_server_port->Attach_Callback( Update_IOT_Config_CB_static,
												reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
												DCI_ACCESS_POST_SET_RAM_CMD_MSK );

	m_owner_proxy_username->Attach_Callback( Update_IOT_Config_CB_static,
											 reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
											 DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK );

	m_owner_proxy_password->Attach_Callback( Update_IOT_Config_CB_static,
											 reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
											 DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK |
											 DCI_ACCESS_GET_RAM_CMD_MSK | DCI_ACCESS_GET_INIT_CMD_MSK |
											 DCI_ACCESS_GET_DEFAULT_CMD_MSK );

	m_owner_device_guid->Attach_Callback( Update_IOT_Config_CB_static,
										  reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
										  DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK );

	m_owner_iot_connection_string->Attach_Callback( Update_IOT_Config_CB_static,
													reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
													DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK |
													DCI_ACCESS_GET_RAM_CMD_MSK | DCI_ACCESS_GET_INIT_CMD_MSK |
													DCI_ACCESS_GET_DEFAULT_CMD_MSK );

	m_owner_device_profile->Attach_Callback( Update_IOT_Config_CB_static,
											 reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
											 DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK );

	memset( &m_connection_options, 0, sizeof( IOT_CONNECTION_OPTIONS ) );

	// Create the arrays to hold the string variables
	m_device_uuid = reinterpret_cast<uint8_t*>( Ram::Allocate( m_owner_device_guid->Get_Length() ) );
	m_device_profile = reinterpret_cast<uint8_t*>( Ram::Allocate( m_owner_device_profile->Get_Length() ) );
	m_connection_string = reinterpret_cast<uint8_t*>( Ram::Allocate( m_owner_iot_connection_string->Get_Length() ) );

	m_proxy_address = nullptr;
	m_proxy_username = nullptr;
	m_proxy_password = nullptr;

	m_DCI_change_flag = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_DCI_Config::~IOT_DCI_Config( void )
{
	Ram::De_Allocate( m_device_uuid );
	Ram::De_Allocate( m_device_profile );
	Ram::De_Allocate( m_connection_string );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD IOT_DCI_Config::Update_IOT_Config_CB_static( DCI_CBACK_PARAM_TD handle,
														   DCI_ACCESS_ST_TD* access_struct )
{
	IOT_DCI_Config* object_ptr = static_cast<IOT_DCI_Config*>( handle );

	return ( object_ptr->Update_IOT_Config_CB( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD IOT_DCI_Config::Update_IOT_Config_CB( DCI_ACCESS_ST_TD* access_struct )
{
    //Ahmed getting connection string
    
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	if ( access_struct->command == DCI_ACCESS_SET_RAM_CMD )
	{
		uint16_t return_val = 0U;
		uint16_t str_len = access_struct->data_access.length;

		switch ( access_struct->data_id )
		{
			case DCI_IOT_PROXY_USERNAME_DCID:
			case DCI_IOT_PROXY_PASSWORD_DCID:
				return_val = str_whitelist.Sanitize(
					reinterpret_cast<char_t const*>( access_struct->data_access.data ), str_len );
				break;

			case DCI_IOT_DEVICE_GUID_DCID:
			case DCI_IOT_DEVICE_PROFILE_DCID:
				return_val = uuid_whitelist.Sanitize(
					reinterpret_cast<char_t const*>( access_struct->data_access.data ), str_len );
				break;

			case DCI_IOT_CONN_STRING_DCID:
				return_val = conn_str_whitelist.Sanitize(
					reinterpret_cast<char_t const*>( access_struct->data_access.data ), str_len );
				break;

			default:
				break;
		}
		if ( return_val != str_len )
		{
			return_status = DCI_CBack_Encode_Error( DCI_ERR_DATA_INCORRECT );
		}
	}
	else if ( access_struct->command == DCI_ACCESS_POST_SET_RAM_CMD )
	{
		m_DCI_change_flag = true;
	}
	else if ( ( access_struct->command == DCI_ACCESS_GET_RAM_CMD ) ||
			  ( access_struct->command == DCI_ACCESS_GET_INIT_CMD ) ||
			  ( access_struct->command == DCI_ACCESS_GET_DEFAULT_CMD ) )
	{
		uint8_t* data_buffer;
		switch ( access_struct->data_id )
		{
			case DCI_IOT_PROXY_PASSWORD_DCID:
			case DCI_IOT_CONN_STRING_DCID:
				data_buffer = reinterpret_cast<uint8_t*>( access_struct->data_access.data );
				memset( data_buffer, '*', access_struct->data_access.length );
				data_buffer[access_struct->data_access.length] = '\0';
				return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
				break;

			default:
				break;
		}
	}
	else
	{
		// misra
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_DCI_Config::Reset_Config_Change_Flag( void )
{
	m_DCI_change_flag = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_DCI_Config::Is_Config_Change_Flag_Set( void )
{
	return ( m_DCI_change_flag );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_DCI_Config::Get_Connection_Status( void )
{
	bool status;

	m_owner_iot_connection_status->Check_Out( status );
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_DCI_Config::Set_Connection_Status( bool set_val )
{
	m_owner_iot_connection_status->Check_In( set_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_DCI_Config::Set_Conn_Status_Reason( dci_enum_uint8_t Conn_Status_Reason )
{
	m_owner_iot_conn_status_reason->Check_In( Conn_Status_Reason );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_DCI_Config::Is_IOT_Enable( void )
{
	bool iot_enable = false;

	m_owner_iot_enable->Check_Out( iot_enable );
	return ( iot_enable );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_CONNECTION_OPTIONS* IOT_DCI_Config::Get_IOT_Connection_Options( void )
{
	bool proxy_enable = false;

	#ifdef IOT_INCLUDE_HTTP
	m_connection_options.protocol = HTTP;
	#else	// Ie, if defined IOT_INCLUDE_MQTT_WEBSOCKETS
	m_connection_options.protocol = MQTT_WEBSOCKETS;
	#endif

	m_connection_options.deviceUUID = Get_Device_Uuid();
	m_owner_iot_connection_string->Check_Out( m_connection_string );
	m_connection_options.connectionString = reinterpret_cast<char*>( m_connection_string );

	m_owner_proxy_enable->Check_Out( proxy_enable );
	if ( proxy_enable )
	{
		uint16_t proxy_port;
		m_owner_proxy_server_port->Check_Out( proxy_port );
		m_connection_options.proxyPort = proxy_port;
		m_connection_options.proxyUser = nullptr;
		m_connection_options.proxyPassword = nullptr;

		/* It matters to the Azure code whether empty fields are null pointers, versus valid
		 * pointers to empty strings; it wants null pointers.
		 * Unfortunately, the Get_Length() function gives the allocation, not actual string length,
		 * so we have to get the string and THEN check its length. */
		if ( m_proxy_address == nullptr )
		{
			m_proxy_address = new uint8_t[m_owner_proxy_server_address->Get_Length()];
		}
		m_owner_proxy_server_address->Check_Out( m_proxy_address );
		char* test_string = reinterpret_cast<char*>( m_proxy_address );
		uint16_t test_string_len = BF_Lib::Get_String_Length( m_proxy_address,
															  m_owner_proxy_server_address->Get_Length() );
		// Using >2 as a simple reality check; must be greater than that, even.
		if ( test_string_len > MIN_STRING_LEN_FOR_TEST )
		{
			m_connection_options.proxyAddress = test_string;

			if ( m_proxy_username == nullptr )
			{
				m_proxy_username = new uint8_t[m_owner_proxy_username->Get_Length()];
			}
			m_owner_proxy_username->Check_Out( m_proxy_username );
			test_string = reinterpret_cast<char*>( m_proxy_username );
			test_string_len = BF_Lib::Get_String_Length( m_proxy_username,
														 m_owner_proxy_username->Get_Length() );
			if ( test_string_len > MIN_STRING_LEN_FOR_TEST )
			{
				m_connection_options.proxyUser = test_string;

				// We don't care about the password unless we have a username
				if ( m_proxy_password == nullptr )
				{
					m_proxy_password = new uint8_t[m_owner_proxy_password->Get_Length()];
				}
				m_owner_proxy_password->Check_Out( m_proxy_password );
				test_string = reinterpret_cast<char*>( m_proxy_password );
				test_string_len = BF_Lib::Get_String_Length( m_proxy_password,
															 m_owner_proxy_password->Get_Length() );
				if ( test_string_len > 0 )
				{
					m_connection_options.proxyPassword = test_string;
				}
				else
				{
					delete[]( m_proxy_password );
					m_proxy_password = nullptr;
				}
			}
			else
			{
				delete[]( m_proxy_username );
				m_proxy_username = nullptr;
			}
		}
		else
		{
			// Nothing else matters if we have no proxy address.
			m_connection_options.proxyAddress = nullptr;
			delete[]( m_proxy_address );
			m_proxy_address = nullptr;
		}
	}
	else
	{
		// Else no proxy, so null out and free anything here
		m_connection_options.proxyAddress = NULL;
		m_connection_options.proxyPort = 0U;
		m_connection_options.proxyUser = NULL;
		m_connection_options.proxyPassword = NULL;
		if ( m_proxy_address != nullptr )
		{
			delete[]( m_proxy_address );
			m_proxy_address = nullptr;
		}
		if ( m_proxy_username != nullptr )
		{
			delete[]( m_proxy_username );
			m_proxy_username = nullptr;
		}
		if ( m_proxy_password != nullptr )
		{
			delete[]( m_proxy_password );
			m_proxy_password = nullptr;
		}
	}
	return ( &m_connection_options );
}

// Getter for the Device UUID (aka GUID).
const char* IOT_DCI_Config::Get_Device_Uuid( void )
{
	m_owner_device_guid->Check_Out( m_device_uuid );
	return ( reinterpret_cast<char*>( m_device_uuid ) );
}

// Getter for the Device's IoT Profile UUID.
const char* IOT_DCI_Config::Get_Device_Profile( void )
{
	m_owner_device_profile->Check_Out( m_device_profile );
	return ( reinterpret_cast<char*>( m_device_profile ) );
}
