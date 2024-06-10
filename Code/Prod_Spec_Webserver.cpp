/**
 **********************************************************************************************
 * @file            Prod_Spec_Webserver.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product.
 *                  Adopter can initialize the webserver
 *
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "Includes.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "Prod_Spec_Webserver.h"
#include "Crypt_Aes_CBC_Matrix.h"
#include "User_Session.h"
#include "User_Account.h"
#include "uC_Fw_Code_Range.h"
#include "httpd.h"
#include "Services.h"
#include "NV_Ctrl.h"
#include "Prod_Spec_MEM.h"
#include "server.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/* Crypt object ( which is going to use these keys ) is located at /Babelfish/Lib/Crypt.
   These keys used for encryption/decryption of nonce used in session management.
   Note: Following key values are randomly choosen. Adopter can use any key values. */

// 16 Bytes AES 128 Key
static const uint8_t crypt_aes_key_sess[16U] = { 0xA0, 0x09, 0xBA, 0xFF, 0x9C, 0x53, 0x85, 0xA1, 0xD1,
												 0x11, 0x05, 0x41, 0x63, 0xE7, 0xAC, 0xBF };
// 16 Bytes Initialization vector
static const uint8_t crypt_aes_iv_sess[16U] = { 0x28, 0x9F, 0x0C, 0x8F, 0xCA, 0xFF, 0xF0, 0x3A, 0xC7,
												0x27, 0xD6, 0x75, 0x3B, 0x0A, 0xC3, 0xA0 };

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
// Instance of User Account class
static User_Account* m_user_account = nullptr;

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
void Web_Server_Setup( void );


extern bool_t Https_Init( void );

/**
 **********************************************************************************************
 * @brief                     Ethernet/EMAC initialization
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_WEBSERVER_Init( void )
{
#ifdef WEB_SERVER_SUPPORT
	Web_Server_Setup();

#ifdef HTTP_SUPPORT
	httpd_init();
#endif

#ifdef HTTPS_SUPPORT
	Https_Init();
#endif
#endif
}

/**
 **********************************************************************************************
 * @brief                     Web Server Setup function
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void Web_Server_Setup( void )
{
	NV_Ctrl* web_ui_nv_ctrl = nullptr;

	/* Crypto init for User Session */
	Crypt* crypt_obj_sess =
		new Crypt_Aes_CBC_Matrix( const_cast<uint8_t const*>( &crypt_aes_key_sess[0] ),
								  Crypt::AES_128_KEY,
								  const_cast<uint8_t const*>( &crypt_aes_iv_sess[0] ),
								  Crypt::AES_IV_SIZE_KEY );

	User_Session::user_session_config_t user_session_config;

	user_session_config.enable_session_takeover = new DCI_Owner( DCI_ENABLE_SESSION_TAKEOVER_DCID );
	user_session_config.max_concurrent_sessions = new DCI_Owner( DCI_MAX_CONCURRENT_SESSION_DCID );
	user_session_config.inactivity_timeout_sec = new DCI_Owner( DCI_USER_INACTIVITY_TIMEOUT_DCID );
	user_session_config.absolute_timeout_sec = new DCI_Owner( DCI_ABSOLUTE_TIMEOUT_SEC_DCID );
	user_session_config.user_lock_time_sec = new DCI_Owner( DCI_USER_LOCK_TIME_SEC_DCID );
	user_session_config.max_num_failed_login_attempts = new DCI_Owner(
		DCI_MAX_FAILED_LOGIN_ATTEMPTS_DCID );
	user_session_config.max_num_session = 6U;
	user_session_config.max_num_stored_logout_sessions = user_session_config.max_num_session * 2U;
	user_session_config.password_waiver_level = new DCI_Owner(
		DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_DCID );
	DCI_Owner* user_valid_account_bit_list = new DCI_Owner( DCI_VALID_BITS_USERS_DCID );
	DCI_Owner* device_default_password = new DCI_Owner( DCI_DEVICE_DEFAULT_PWD_DCID );

	m_user_account =
		new User_Account( reinterpret_cast<const User_Account::dci_user_mgmt_t*>( USER_MGMT_USER_LIST ),
						  MAX_NUM_OF_AUTH_USERS,
						  reinterpret_cast<const User_Account::auth_struct_t*>( auth_struct ),
						  NUM_OF_AUTH_LEVELS,
						  user_valid_account_bit_list,
						  device_default_password );

	// Attach callback for setting device unique password
	device_default_password->Attach_Callback( &Prod_Spec_Device_Default_Pwd_CB,
											  nullptr,
											  ( DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_GET_RAM_CMD_MSK ) );

	User_Session* user_session = new User_Session( m_user_account, &user_session_config,
												   crypt_obj_sess );

	PROD_SPEC_MEM_Get_WEB_NV_Handle( &web_ui_nv_ctrl );

	Web_Server_Init( &rest_dci_data_target_info, user_session, web_ui_nv_ctrl );
	// coverity[leaked_storage]
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Prod_Spec_Device_Default_Pwd_CB( DCI_CBACK_PARAM_TD param, DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	if ( BF_Lib::Services::MFG_Access_Unlocked() )
	{
		if ( access_struct->command == DCI_ACCESS_SET_RAM_CMD )
		{
			// API to set device unique password
			return_status =
				m_user_account->Set_Device_Unique_Password( reinterpret_cast<uint8_t const*>( access_struct->data_access
																							  .
																							  data ),
															access_struct->data_access.length );
		}
	}
	else
	{
		switch ( access_struct->command )
		{
			case DCI_ACCESS_SET_RAM_CMD:
			{
				return_status = DCI_CBack_Encode_Error( DCI_ERR_RAM_READ_ONLY );
			}
			break;

			case DCI_ACCESS_GET_RAM_CMD:
			{
				return_status = DCI_CBack_Encode_Error( DCI_ERR_ACCESS_VIOLATION );
			}
			break;

			default:
			{
				return_status = DCI_CBack_Encode_Error( DCI_ERR_INVALID_COMMAND );
			}
			break;
		}
	}
	return ( return_status );
}
