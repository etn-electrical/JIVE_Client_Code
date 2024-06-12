/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __HTTPD_AUTH_H__
#define __HTTPD_AUTH_H__

#include "DCI_Defines.h"
/******************************************************************************/
// Constants used by http_digest_authentication and http_basic_authentication

// #define NUM_OF_AUTH_LEVELS 7U
#define MAX_USERNAME_LENGTH 20U
#define MAX_PASSWORD_LENGTH 20U

// static const uint8_t HTTPD_AUTH_LEVEL_OPEN_TO_ALL = 0U;
// static const uint8_t HTTPD_AUTH_LEVEL_READ_ONLY = 7U;
// static const uint8_t HTTPD_AUTH_LEVEL_CONTROL = 15U;
// static const uint8_t HTTPD_AUTH_LEVEL_CONFIGURATION = 31U;
// static const uint8_t HTTPD_AUTH_LEVEL_ADMIN_USER = 99U;
// static const uint8_t HTTPD_AUTH_LEVEL_FACTORY = 199U;
// static const uint8_t HTTPD_AUTH_LEVEL_ROOT = 255U;


typedef struct
{
	UINT8 level;
	const uint8_t* level_name;
	uint8_t level_name_length;
} AUTH_REALM_TD;

#endif


