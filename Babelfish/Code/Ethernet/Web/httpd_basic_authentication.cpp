/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "DCI_Owner.h"
#include "DCI_Patron.h"
#include "DCI_Data.h"
#include "httpd_common.h"
#include "MD5_Digest.h"
#include "Base64_Rest.h"
#include "http_auth.h"
#include "httpd_basic_authentication.h"

#include "server.h"

UINT8 httpd_basic_authentication::m_buf[MAX_USERNAME_PASSWORD_LENGTH] =
{ 0U };

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
UINT8 httpd_basic_authentication::getRequestLevel( DCI_Patron* httpd_dci_access,
												   UINT8 const* pInput, UINT32 pInput_len )
{
	uint8_t return_val = 0U;
	bool_t loop_break = false;

	UINT16 length_username_password = 0U;
	DCI_ACCESS_ST_TD access_struct =
	{ 0U };
	UINT8 i;
	uint16_t user_valid_bits = 0;

	// access_struct.data_access.length = length;
	access_struct.source_id = REST_DCI_SOURCE_ID;
	access_struct.data_access.data = static_cast<DCI_DATA_PASS_TD*>( &user_valid_bits );
	access_struct.data_access.offset = 0U;
	// access_struct.data_id = DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_DCID;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_id = DCI_VALID_BITS_USERS_DCID;

	access_struct.data_access.length = 0U;
	if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
	{
		// First, decode the Base64
		UINT8 decoded[MAX_USERNAME_PASSWORD_LENGTH];
		UINT32 decoded_len;
		if ( BASE64_DECODING_ERR_OK != Base64::Decode( pInput, pInput_len, decoded, &decoded_len, 0U ) )
		{
			return_val = 0U;
			loop_break = true;
		}

		access_struct.data_access.length = 0U;
		for ( i = 0U;
			  ( ( i < MAX_NUM_OF_AUTH_USERS ) && ( false == loop_break ) );
			  i++ )
		{
			if ( ( user_valid_bits & ( 1 << i ) ) != 0U )
			{
				length_username_password = 0;
				access_struct.data_access.length = length_username_password;
				access_struct.data_access.data = static_cast<DCI_DATA_PASS_TD*>( m_buf );
				access_struct.data_id = USER_MGMT_USER_LIST[i].user_name;
				if ( DCI_ERR_NO_ERROR != httpd_dci_access->Data_Access( &access_struct ) )
				{
					return_val = 0U;
					loop_break = true;
				}
				if ( false == loop_break )
				{
					uint8_t username_offset = 0;
					username_offset = strlen( reinterpret_cast<char_t const*>( m_buf ) );
					m_buf[username_offset] = 0x3AU;	// ':';
					access_struct.data_access.data = static_cast<DCI_DATA_PASS_TD*>( m_buf
																					 + username_offset + 1 );
					access_struct.data_access.offset = 0U;
					access_struct.data_access.length = 0U;
					access_struct.data_id = USER_MGMT_USER_LIST[i].password;
					if ( DCI_ERR_NO_ERROR != httpd_dci_access->Data_Access( &access_struct ) )
					{
						return_val = 0U;
						loop_break = true;
					}
					if ( ( ( strlen( reinterpret_cast<char_t const*>( m_buf ) ) ) ==
						   decoded_len ) && ( false == loop_break ) )
					{
						if ( 0 ==
							 strncmp(
								 reinterpret_cast<char_t const*>( decoded ),
								 ( reinterpret_cast<char_t const*>( m_buf ) ),
								 decoded_len ) )
						{
							return_val = i + 1;
							loop_break = true;
						}
					}
				}
			}
		}
		if ( false == loop_break )
		{
			return_val = 0U;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t httpd_basic_authentication::s_auth_cb( hams_parser* parser,
											   const uint8_t* username, uint32_t len )
{
	uint8_t return_val = 0U;
	uint8_t user_role = 0U;
	DCI_ACCESS_ST_TD access_struct =
	{ 0U };

	// Basic Authentication is Base64::Encode("username:password")
	if ( ( len > MAX_BASE64_ENCODED_USERPASS_LENGTH ) || ( len == 0 ) )
	{
		return_val = 1;
	}
	else
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );

		rs->user_id = getRequestLevel( httpd_dci_access, username, len );
		rs->auth_level = 0U;

		if ( ( rs->user_id > 0U ) && ( rs->user_id <= MAX_NUM_OF_AUTH_USERS ) )
		{
			access_struct.source_id = REST_DCI_SOURCE_ID;
			access_struct.data_access.data = static_cast<DCI_DATA_PASS_TD*>( &user_role );
			access_struct.data_access.offset = 0U;
			access_struct.command = DCI_ACCESS_GET_RAM_CMD;
			access_struct.data_id = USER_MGMT_USER_LIST[rs->user_id - 1].role;
			if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
			{
				rs->auth_level = user_role;
			}
		}
		if ( 0U == rs->auth_level )
		{
			return_val = 1;
		}
		else
		{
			// uint8_t exempt_level = User_Session::Get_Password_Exemption_Level( httpd_dci_access );
			uint8_t exempt_level = 0U;
			rs->flags |= REQ_FLAG_AUTH_VALID;
			rs->max_auth_exempt_level =
				( rs->auth_level > exempt_level ) ? rs->auth_level : exempt_level;
			return_val = 0;
		}
	}
	return ( return_val );
}
