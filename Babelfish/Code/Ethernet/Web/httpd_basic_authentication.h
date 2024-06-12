/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __HTTPD_BASIC_AUTHENTICATION_H__
#define __HTTPD_BASIC_AUTHENTICATION_H__

#include "hams.h"
#include "http_auth.h"
// max length = user name + pass + colon + null character
#define MAX_USERNAME_PASSWORD_LENGTH  ( MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 2 )
#define MAX_BASE64_ENCODED_USERPASS_LENGTH  ( ( ( MAX_USERNAME_PASSWORD_LENGTH + 2 ) / 3 ) * 4 )

class httpd_basic_authentication
{
	public:
		static UINT8 getRequestLevel( DCI_Patron* httpd_dci_access,
									  UINT8 const* pInput,
									  UINT32 pInput_len );

		static const AUTH_REALM_TD* getRealm( UINT8 level );

		static uint8_t s_auth_cb( hams_parser* parser,
								  const uint8_t* username, uint32_t len );

	private:
		static UINT8 m_buf[MAX_USERNAME_PASSWORD_LENGTH];
};


#endif	/* __HTTPD_BASIC_AUTHENTICATION_H__ */
