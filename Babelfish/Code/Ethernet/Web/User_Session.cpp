/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "User_Session.h"
#include "Log_Events.h"
#include "uC_Rand_Num.h"
#include "DCI_Enums.h"
#include "StdLib_MV.h"
#include "hams.h"
#include "Web_Debug.h"

extern uint32_t Get_Epoch_Time( void );

/*
 *****************************************************************************************
 *		Static Variable
 *****************************************************************************************
 */
uint32_t User_Session::m_fr_counter = 0U;
static Session_Audit_Function_Decl_1 m_session_log_audit_capture_cb =
	reinterpret_cast<Session_Audit_Function_Decl_1>( nullptr );
static Session_Audit_Function_Decl_2 m_session_log_param_cb =
	reinterpret_cast<Session_Audit_Function_Decl_2>( nullptr );
uint16_t User_Session::m_auth_req_counter = 0U;
uint8_t User_Session::m_magic_num[User_Session::MAGIC_NUMBER_SIZE] = { 0U };
const uint8_t User_Session::LOGIN_URI[] = "GET:/rs/users/accounts/curruser";
const uint8_t User_Session::LOGOUT_URI[] = "DELETE:/rs/users/session";
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
User_Session::User_Session( User_Account* user_acc,
							const user_session_config_t* user_session_config,
							Crypt* crypt_obj_sess ) :
	m_user_account( user_acc ),
	m_crypt_obj( crypt_obj_sess ),
	m_enable_sess_takeover_owner( user_session_config->enable_session_takeover ),
	m_max_concurr_sess_owner( user_session_config->max_concurrent_sessions ),
	m_inactivity_timeout_owner( user_session_config->inactivity_timeout_sec ),
	m_absolute_timeout_owner( user_session_config->absolute_timeout_sec ),
	m_user_lock_time_owner( user_session_config->user_lock_time_sec ),
	m_max_failed_login_owner( user_session_config->max_num_failed_login_attempts ),
	m_max_sessions( user_session_config->max_num_session ),
	m_max_logout_sessions( user_session_config->max_num_stored_logout_sessions ),
	m_web_access_password_waiver_level_owner( user_session_config->password_waiver_level ),
	m_sessions( nullptr ),
	m_user_lock_info( nullptr ),
	m_logout_sessions( nullptr )
{
	/* Allocating memory for maximum number of session object */
	m_sessions = new session_object_t[m_max_sessions];
	/* Allocating memory for logged out session object. */
	m_logout_sessions = new logout_session_list_t[m_max_logout_sessions];

	/* Allocating memory to capture failure attempt. */
	m_user_lock_info = new user_lock_info_t[m_user_account->Get_Total_User_Count()];

	Initialize_Global_Session_Constants();

	/* A CR task for handling logout and timeout */
	// coverity[leaked_storage]
	new CR_Tasker( &Checker_Task_Static, static_cast<CR_TASKER_PARAM>( this ),
				   CR_TASKER_IDLE_PRIORITY,
				   "User session task" );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Session::Create_Nonce( char_t* nonce, char_t* opaque, uint32_t ip_address,
								 uint8_t port )
{
	bool return_val = false;
	uint8_t encrypted_buf[NONCE_LENGTH] = { 0U };
	nonce_element_t nonce_element;

	nonce_element.auth_counter = ++m_auth_req_counter;
	nonce_element.fr_counter = m_fr_counter;
	memcpy( nonce_element.magic_number, m_magic_num, MAGIC_NUMBER_SIZE );
	nonce_element.ip_address = ip_address;
	nonce_element.port = port;

	if ( true == Encrypt_Nonce( &nonce_element, sizeof( nonce_element ),
								reinterpret_cast<char_t*>( encrypted_buf ) ) )
	{
		httpd_digest_authentication::From_16Bytes_To_32Hex( encrypted_buf,
															reinterpret_cast<uint8_t*>( nonce ) );
		memcpy( opaque, httpd_digest_authentication::OPAQUE, OPAQUE_LENGTH );
		return_val = true;

		USER_SESS_PRINT( DBG_MSG_INFO, "Nonce created for Session: %u, from IP: %X", m_fr_counter,
						 nonce_element.ip_address );
	}
	else
	{
		/* Misra */
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t User_Session::Is_Nonce_Active( char_t* nonce )
{
	uint8_t return_val = m_max_sessions;
	uint_fast8_t counter;

	for ( counter = 0U; counter < m_max_sessions; counter++ )
	{
		/* Check if nonce is present in active session list */
		if ( ( true == m_sessions[counter].is_active ) && ( 0U == strncmp( nonce,
																		   m_sessions[counter].nonce,
																		   NONCE_LENGTH ) ) )
		{
			return_val = counter;
			break;
		}
		else
		{
			/* Nonce is not found in active session list */
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Session::Create_Session( uint8_t session_index, uint8_t user_id, char_t* nonce,
								   uint32_t ip_address, uint8_t port )
{
	bool return_val = false;
	uint8_t log_param[User_Account::USER_NAME_LIMIT + 6U] = {0U};

	if ( m_curr_logout_sessions < m_max_logout_sessions )
	{
		m_sessions[session_index].user_id = user_id;
		m_sessions[session_index].last_activity_time = m_fr_counter;
		m_sessions[session_index].session_create_time = m_fr_counter;
		memcpy( m_sessions[session_index].nonce, nonce, NONCE_LENGTH );
		m_sessions[session_index].epoch_time = Get_Epoch_Time();
		m_sessions[session_index].is_active = true;

		/* user session creation to be logged */
		if ( ( m_user_account->Get_User_Name(
				   user_id,
				   &log_param[0],
				   User_Account::USER_NAME_LIMIT + 1U ) ) )
		{
			uint8_t data_len = BF_Lib::Get_String_Length(
				log_param,
				( User_Account::USER_NAME_LIMIT + 1U ) );
			for ( uint8_t i = data_len; i < ( User_Account::USER_NAME_LIMIT + 1U ); i++ )
			{
				log_param[i] = 0;
			}
			log_param[User_Account::USER_NAME_LIMIT + 1U] = ip_address & 0xffU;
			log_param[User_Account::USER_NAME_LIMIT + 2U] = ( ip_address & 0xff00U ) >> 8U;
			log_param[User_Account::USER_NAME_LIMIT + 3U] = ( ip_address & 0xff0000U ) >> 16U;
			log_param[User_Account::USER_NAME_LIMIT + 4U] = ( ip_address & 0xff000000U ) >> 24U;
			if ( port == HTTPS_MEDIA )
			{
				log_param[User_Account::USER_NAME_LIMIT +
						  5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTPS;
			}
			else
			{
				log_param[User_Account::USER_NAME_LIMIT + 5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTP;
			}
			if ( m_session_log_param_cb != nullptr )
			{
				( *m_session_log_param_cb )( log_param );
			}

			if ( m_session_log_audit_capture_cb != nullptr )
			{
				( *m_session_log_audit_capture_cb )(
					LOG_EVENT_LOG_IN_USER_X );
			}

			USER_SESS_PRINT( DBG_MSG_INFO, "Session Created for User ID: %u, IP: %X, Port: %u", user_id, ip_address,
							 port );
			return_val = true;
		}
	}
	else
	{
		USER_SESS_PRINT( DBG_MSG_ERROR, "Unable to create session, Max log session count: %u", m_max_logout_sessions );
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Session::Logout_Session( Digest_data* p, uint8_t user_id )
{
	bool return_val = false;
	uint_fast8_t counter;

	for ( counter = 0; counter < m_max_sessions; counter++ )
	{
		if ( ( true == m_sessions[counter].is_active ) &&
			 ( ( m_sessions[counter].user_id == user_id ) &&
			   ( 0U == memcmp( p->rx_nonce_buf, m_sessions[counter].nonce, NONCE_LENGTH ) ) ) )
		{
			Delete_Session( counter );
			return_val = true;
			break;
		}
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Session::Delete_Session( uint8_t session_index )
{
	uint32_t epoch_time = 0U;
	nonce_element_t nonce_element = {0, 0, 0, 0, 0};
	uint_fast8_t counter;
	bool return_status = false;

	/* Find if nonce is still fresh, if yes then store it in the logout session list */
	if ( true == Decrypt_Nonce( reinterpret_cast<uint8_t*>( m_sessions[session_index].nonce ),
								&nonce_element ) )
	{
		if ( true == Is_Fresh_Nonce_Valid( &nonce_element ) )
		{
			if ( m_curr_logout_sessions < m_max_logout_sessions )
			{
				for ( counter = 0; counter < m_max_logout_sessions; counter++ )
				{
					if ( m_logout_sessions[counter].auth_counter == 0U )
					{
						m_logout_sessions[counter].auth_counter =
							nonce_element.auth_counter;
						m_logout_sessions[counter].fr_expiry_value =
							nonce_element.fr_counter;

						++m_curr_logout_sessions;
						break;
					}
				}
			}
		}
	}

	/* Delete the session */
	m_sessions[session_index].is_active = false;

	USER_SESS_PRINT( DBG_MSG_INFO, "Session deleted : %u", session_index );

	/* Update last login Epoch time */
	epoch_time = Get_Epoch_Time();

	return_status = m_user_account->Set_Last_Login_Epoch_Time( m_sessions[session_index].user_id,
															   reinterpret_cast<uint8_t*>( &epoch_time ),
															   sizeof( epoch_time ) );
	uint8_t log_param[User_Account::USER_NAME_LIMIT + 6U] = {0U};

	if ( ( return_status == true ) && ( m_user_account->Get_User_Name(
											m_sessions[session_index].user_id,
											&log_param[0],
											User_Account::USER_NAME_LIMIT + 1U ) ) )
	{
		uint8_t data_len = BF_Lib::Get_String_Length(
			log_param,
			( User_Account::USER_NAME_LIMIT + 1U ) );
		for ( uint8_t i = data_len; i < ( User_Account::USER_NAME_LIMIT + 1U ); i++ )
		{
			log_param[i] = 0;
		}
		log_param[User_Account::USER_NAME_LIMIT + 1U] = nonce_element.ip_address & 0xffU;
		log_param[User_Account::USER_NAME_LIMIT + 2U] = ( nonce_element.ip_address & 0xff00U ) >>
			8U;
		log_param[User_Account::USER_NAME_LIMIT + 3U] = ( nonce_element.ip_address & 0xff0000U ) >>
			16U;
		log_param[User_Account::USER_NAME_LIMIT +
				  4U] = ( nonce_element.ip_address & 0xff000000U ) >> 24U;
		if ( nonce_element.port == HTTPS_MEDIA )
		{
			log_param[User_Account::USER_NAME_LIMIT + 5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTPS;
		}
		else
		{
			log_param[User_Account::USER_NAME_LIMIT + 5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTP;
		}
		if ( m_session_log_param_cb != nullptr )
		{
			( *m_session_log_param_cb )( log_param );
		}

		if ( m_session_log_audit_capture_cb !=
			 nullptr )
		{
			( *m_session_log_audit_capture_cb )(
				LOG_EVENT_LOG_OUT_USER_X );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
USER_SESSION_ERR_TD User_Session::Authenticate_User( Digest_data* p, uint8_t* user_id,
													 uint8_t* auth_level, uint16_t auth_type )
{
	USER_SESSION_ERR_TD return_val = USER_SESS_NO_ERR;
	uint8_t session;
	nonce_element_t nonce_element = {0, 0, 0, 0, 1};
	uint8_t new_session_index = 0U;
	uint8_t password_len = m_user_account->Get_Max_Password_Length();
	uint8_t* password_buf = new uint8_t[password_len];

	if ( password_buf != nullptr )
	{
		*user_id = m_user_account->Verify_User_Name( p->rx_user_name, p->rx_user_name_length );
		if ( *user_id != User_Account::UNKNOWN_ERROR )
		{
			/* Check if nonce is active or not */
			session = Is_Nonce_Active( reinterpret_cast<char_t*>( p->rx_nonce_buf ) );
			if ( session != m_max_sessions )
			{
				/* Check if the user and user linked to this session is same */
				if ( *user_id == m_sessions[session].user_id )
				{
					/* Get the user's password */
					if ( true ==
						 m_user_account->Get_Password( *user_id, password_buf, password_len ) )
					{
						/* Is authorization successful */
						if ( true ==
							 httpd_digest_authentication::Verify_User_Digest( p, password_buf,
																			  password_len ) )
						{
							*auth_level = m_user_account->Get_User_Role( *user_id );
							m_sessions[session].last_activity_time = m_fr_counter;		///< Capture
							// inactivity
							// time
						}
						else
						{
							if ( true ==
								 Decrypt_Nonce( p->rx_nonce_buf, &nonce_element ) )
							{
								/* Increase user's max failed attempt by 1 */
								return_val = Capture_Failed_Attempt( *user_id,
																	 nonce_element.ip_address,
																	 nonce_element.port,
																	 auth_type,
																	 p->rx_md5_uri );
								if ( USER_SESS_AUTH_REQUIRED_ERR == return_val )
								{
									return_val = USER_SESS_INVALID_CRED_ERR;
								}
							}
							else
							{
								/* Assuming Decrypt_Nonce fails(which is rare) and at this point
								*  we know that nonce is correct so returning this error code */
								return_val = USER_SESS_INTERNAL_ERR;
							}
						}
					}
					else
					{
						return_val = USER_SESS_INTERNAL_ERR;
					}
				}
				else
				{
					if ( true == Decrypt_Nonce( p->rx_nonce_buf, &nonce_element ) )
					{
						/* Increase user's max failed attempt by 1 */
						return_val = Capture_Failed_Attempt( *user_id, nonce_element.ip_address,
															 nonce_element.port,
															 auth_type,
															 p->rx_md5_uri );
						if ( USER_SESS_AUTH_REQUIRED_ERR == return_val )
						{
							return_val = USER_SESS_INVALID_CRED_ERR;
						}
					}
					else
					{
						/* Assuming Decrypt_Nonce fails(which is rare) and at this point we
						 * know that nonce is correct so returning this error code */
						return_val = USER_SESS_INTERNAL_ERR;
					}
				}
			}
			else
			{
				/* The nonce is not in the active session list so it should be a new login
				   request */
				if ( false == Is_User_Locked( *user_id ) )
				{
					/* Decrypt the nonce */
					if ( true == Decrypt_Nonce( p->rx_nonce_buf, &nonce_element ) )
					{
						/* Check if nonce is still fresh */
						if ( true == Is_Fresh_Nonce_Valid( &nonce_element ) )
						{
							/* Check if authorization is successful or not */
							if ( true == m_user_account->Get_Password( *user_id, password_buf,
																	   password_len ) )
							{
								if ( true == httpd_digest_authentication::Verify_User_Digest( p,
																							  password_buf,
																							  password_len ) )
								{
									/* Allocate session to the user */
									new_session_index = Find_New_Session_Index( *user_id );
									if ( new_session_index != m_max_sessions )
									{
										if ( true ==
											 Create_Session( new_session_index, *user_id,
															 reinterpret_cast<char_t*>( p->
																						rx_nonce_buf ),
															 nonce_element.ip_address,
															 nonce_element.port ) )
										{
											*auth_level = m_user_account->Get_User_Role(
												*user_id );
											/* Convert failed_attempt_counter to 0 on successful
											   login */
											if ( m_user_lock_info[*user_id].login_attempt ==
												 true )
											{
												m_user_lock_info[*user_id].
												failed_attempt_counter =
													0U;
											}
											else
											{
												m_user_lock_info[*user_id].login_attempt = true;
											}
										}
										else
										{
											return_val = USER_SESS_NO_SESSION_AVAILABLE_ERR;
										}
									}
									else
									{
										/* Can not find a session. Reply with error */
										return_val = USER_SESS_NO_SESSION_AVAILABLE_ERR;
									}
								}
								else
								{
									return_val = Capture_Failed_Attempt( *user_id,
																		 nonce_element.ip_address,
																		 nonce_element.port,
																		 auth_type,
																		 p->rx_md5_uri );
									if ( USER_SESS_AUTH_REQUIRED_ERR == return_val )
									{
										return_val = USER_SESS_INVALID_CRED_ERR;
									}
								}
							}
							else
							{
								return_val = USER_SESS_INTERNAL_ERR;
							}
						}
						else
						{
							return_val = Capture_Failed_Attempt( *user_id,
																 nonce_element.ip_address,
																 nonce_element.port,
																 auth_type,
																 p->rx_md5_uri );
						}
					}
					else
					{
						return_val = Capture_Failed_Attempt( *user_id, nonce_element.ip_address,
															 nonce_element.port, auth_type,
															 p->rx_md5_uri );
					}
				}
				else
				{
					/* Respond with user account locked error */
					return_val = USER_SESS_USER_LOCKED_ERR;
				}
			}

			*user_id += 1U;	///< Increasing by 1 since client handles user_id from 1 to max_user_id
		}
		else
		{
			/* Respond with 401 error */
			return_val = USER_SESS_USER_NOT_AVAILABLE;
			USER_SESS_PRINT( DBG_MSG_INFO, "User not available" );
		}

		delete[] password_buf;
	}
	else
	{
		return_val = USER_SESS_INTERNAL_ERR;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t User_Session::Get_Max_Auth_Role_Exemption_Level( uint8_t auth_level )
{
	uint8_t max_auth_level = 0U;
	uint8_t res = 0U;

	// coverity[var_deref_model]
	m_web_access_password_waiver_level_owner->Check_Out( &res );
	max_auth_level = ( auth_level > res ) ? auth_level : res;
	return ( max_auth_level );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Session::Clear_User_Session( uint8_t user_id )
{
	bool return_val = false;
	uint_fast8_t counter;

	for ( counter = 0U; counter < m_max_sessions; counter++ )
	{
		if ( ( true == m_sessions[counter].is_active ) &&
			 ( m_sessions[counter].user_id == user_id ) )
		{
			Delete_Session( counter );
			return_val = true;
		}
		else
		{
			/* Misra Suppression */
		}
	}
	/* Clearing all user data */
	m_user_lock_info[user_id].is_lock = false;
	m_user_lock_info[user_id].unlock_time = 0U;
	m_user_lock_info[user_id].failed_attempt_counter = 0U;
	m_user_lock_info[user_id].login_attempt = false;

	USER_SESS_PRINT( DBG_MSG_INFO, "All Sessions cleared for User: %u", user_id );

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Session::Initialize_Global_Session_Constants( void )
{
	/* Initialize all parameters */
	m_fr_counter = 0U;
	m_auth_req_counter = 0U;
	uC_Rand_Num::Get( m_magic_num, MAGIC_NUMBER_SIZE );
	m_curr_logout_sessions = 0U;

	uint8_t total_users = m_user_account->Get_Total_User_Count();

	/* Clearing user lock information */
	memset( m_user_lock_info, 0, total_users * sizeof( user_lock_info_t ) );
	/* Clearing session information */
	memset( m_sessions, 0, m_max_sessions * sizeof( session_object_t ) );
	/* Clearing logout sessions information */
	memset( m_logout_sessions, 0, m_max_logout_sessions * sizeof( logout_session_list_t ) );

	httpd_digest_authentication::Generate_Opaque();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Session::Is_Fresh_Nonce_Valid( nonce_element_t* nonce_element )
{
	bool return_val = false;
	uint_fast8_t counter;

	/* Check if nonce generation time is less than FRESH_NONCE_TIMEOUT time */
	if ( ( FRESH_NONCE_TIMEOUT > ( m_fr_counter - nonce_element->fr_counter ) ) &&
		 ( 0U == memcmp( nonce_element->magic_number, m_magic_num, MAGIC_NUMBER_SIZE ) ) )
	{
		return_val = true;
		/* Check if nonce is in the logout list */
		for ( counter = 0; counter < m_max_logout_sessions; counter++ )
		{
			if ( m_logout_sessions[counter].auth_counter == nonce_element->auth_counter )
			{
				return_val = false;
			}
		}
	}

	if ( false == return_val )
	{
		USER_SESS_PRINT( DBG_MSG_CRITICAL, "Nonce not Valid" );
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t User_Session::Is_Session_Timed_Out( uint8_t session_index,
										   const uint16_t inactivity_timeout,
										   const uint32_t absolute_timeout )
{
	bool return_val = false;

	if ( ( ( m_fr_counter - m_sessions[session_index].last_activity_time ) > inactivity_timeout ) ||
		 ( ( m_fr_counter - m_sessions[session_index].session_create_time ) > absolute_timeout ) )
	{
		if ( m_sessions[session_index].timeout_expiry == SESSION_EXPIRY_UNBLOCK )
		{
			return_val = true;
		}
	}
	else
	{
		/* Misra suppresion */
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Session::Encrypt_Nonce( nonce_element_t* nonce_element, uint8_t nonce_element_size,
								  char_t* nonce )
{
	bool return_val = false;

	if ( m_crypt_obj->Encrypt( reinterpret_cast<uint8_t*>( nonce_element ), nonce_element_size,
							   reinterpret_cast<uint8_t*>( nonce ) ) == Crypt::SUCCESS )
	{
		return_val = true;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Session::Decrypt_Nonce( uint8_t* nonce, nonce_element_t* nonce_element )
{
	bool return_val = false;

	if ( m_crypt_obj->Decrypt( nonce, NONCE_LENGTH, reinterpret_cast<uint8_t*>( nonce_element )
							   ) == Crypt::SUCCESS )
	{
		if ( 0U == memcmp( nonce_element->magic_number, m_magic_num, MAGIC_NUMBER_SIZE ) )
		{
			return_val = true;
		}
		else
		{
			/* Misra */
		}
	}
	else
	{
		/* Misra */
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Session::Checker_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
{
	( ( User_Session* )handle )->Checker_Task( cr_task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Session::Checker_Task( CR_Tasker* cr_task )
{
	uint_fast8_t counter;
	uint16_t inactivity_timeout = 0U;
	uint32_t absolute_timeout = 0U;
	uint16_t lock_period = 0U;

	CR_Tasker_Begin( cr_task );

	for (;;)
	{
		m_fr_counter++;

		/* Logic for absolute and inactivity timeout check */
		if ( ( m_inactivity_timeout_owner->Check_Out( &inactivity_timeout ) ) &&
			 ( m_absolute_timeout_owner->Check_Out( &absolute_timeout ) ) )
		{
			for ( counter = 0; counter < m_max_sessions; counter++ )
			{
				if ( ( true == m_sessions[counter].is_active ) &&
					 ( true ==
					   Is_Session_Timed_Out( counter, inactivity_timeout, absolute_timeout ) ) )
				{
					USER_SESS_PRINT( DBG_MSG_INFO, "User Session deleted because of Inactivity: %u", counter );
					Delete_Session( counter );
				}
				else
				{
					/* Misra suppression */
				}
			}
		}

		/* Unlocking locked user after user_lock_time*/
		if ( m_user_lock_time_owner->Check_Out( &lock_period ) )
		{
			for ( counter = 0; counter < m_user_account->Get_Total_User_Count(); counter++ )
			{
				if ( true == m_user_lock_info[counter].is_lock )///< Check if user is locked
				{
					if ( lock_period < ( m_fr_counter - m_user_lock_info[counter].unlock_time ) )
					{
						/* Unlock user since max lock time complete*/
						m_user_lock_info[counter].is_lock = false;
						m_user_lock_info[counter].unlock_time = 0U;

						USER_SESS_PRINT( DBG_MSG_INFO, "User Session Unlocked: %u", counter );
					}
					else
					{
						/* Misra suppression*/
					}
				}
				else
				{
					/* Misra suppression*/
				}
			}
		}

		/* Removing entry from m_logout_sessions after FRESH_NONCE_TIMEOUT */
		if ( m_curr_logout_sessions > 0U )
		{
			for ( counter = 0; counter < m_max_logout_sessions; counter++ )
			{
				if ( ( 0U != m_logout_sessions[counter].auth_counter ) &&
					 ( FRESH_NONCE_TIMEOUT <
					   ( m_fr_counter - m_logout_sessions[counter].fr_expiry_value ) ) )
				{
					m_logout_sessions[counter].auth_counter = 0U;
					--m_curr_logout_sessions;
					USER_SESS_PRINT( DBG_MSG_INFO, "Nonce expiry value: %u",
									 m_logout_sessions[counter].fr_expiry_value );
				}
			}
		}

		CR_Tasker_Delay( cr_task, FR_TIMER_PERIOD_1000_MS );
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t User_Session::Find_Hijackable_Session( uint8_t user_index, bool_t is_concur_session )
{
	/* The default value of sess_delete is the number of available session because default value
	   must be something which will never be a session index. If number of available session is 6
	   then session index will be something between 0-5 so 6 is a safe choice for default value. */
	uint8_t sess_delete = m_max_sessions;
	bool_t session_takeover = false;

	if ( ( m_enable_sess_takeover_owner->Check_Out( &session_takeover ) ) &&
		 ( session_takeover != false ) )
	{
		uint_fast8_t counter;
		uint8_t user_role;
		uint8_t sess_user_role;
		uint32_t inactivity_time = 4294967295U;

		user_role = m_user_account->Get_User_Role( user_index );

		for ( counter = 0; counter < m_max_sessions; counter++ )
		{
			if ( true == m_sessions[counter].is_active )
			{
				/* Get the role of the user to whom this session is attached*/
				sess_user_role = m_user_account->Get_User_Role( m_sessions[counter].user_id );

				/* Find the least active session of the same user */
				if ( true == is_concur_session )
				{
					if ( ( m_sessions[counter].user_id == user_index ) &&
						 ( m_sessions[counter].last_activity_time < inactivity_time ) )
					{
						inactivity_time = m_sessions[counter].last_activity_time;
						sess_delete = counter;
					}
				}
				/* Find the session having lesser role and least active */
				else
				{
					if ( sess_user_role < user_role )
					{
						user_role = sess_user_role;
						inactivity_time = m_sessions[counter].last_activity_time;
						sess_delete = counter;
					}
					else if ( ( sess_user_role == user_role ) &&
							  ( m_sessions[counter].last_activity_time < inactivity_time ) )
					{
						inactivity_time = m_sessions[counter].last_activity_time;
						sess_delete = counter;
					}
					else
					{
						/* Misra Suppression */
					}
				}
			}
		}
	}
	else
	{
		/* Misra Suppression */
	}

	return ( sess_delete );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t User_Session::Find_New_Session_Index( uint8_t user_index )
{
	uint8_t active_sessions = 0U;
	uint8_t active_concur_sessions = 0U;
	uint8_t max_concur_sessions = 0U;
	uint8_t available_session_index = 0U;
	uint_fast8_t counter;
	bool available_index_found = false;

	/* Find number of all active sessions and concurrent sessions for user_index */
	for ( counter = 0; counter < m_max_sessions; counter++ )
	{
		if ( true == m_sessions[counter].is_active )
		{
			++active_sessions;
			if ( m_sessions[counter].user_id == user_index )
			{
				++active_concur_sessions;
			}
		}
		else
		{
			if ( available_index_found == false )
			{
				available_session_index = counter;
				available_index_found = true;
			}
		}
	}

	/* Check if active sessions for user_index is equal to max_concurrent_session */
	if ( ( m_max_concurr_sess_owner->Check_Out( &max_concur_sessions ) ) &&
		 ( active_concur_sessions >= max_concur_sessions ) )
	{
		/* Find and delete the least active session and return the value of deleted session */
		available_session_index = Find_Hijackable_Session( user_index, true );
		Delete_Session( available_session_index );
		USER_SESS_PRINT( DBG_MSG_INFO, "Session Hijacked because of least activity: %u", available_session_index );
	}
	/* If all sessions are full then delete one session based on the role and inactivity time */
	else if ( active_sessions >= m_max_sessions )
	{
		available_session_index = Find_Hijackable_Session( user_index, false );
		/* If found a session then delete it and if not found then return too many request
		   or server busy error */
		if ( available_session_index != m_max_sessions )
		{
			Delete_Session( available_session_index );
			USER_SESS_PRINT( DBG_MSG_INFO, "Session Hijacked because of Lowest role / Inactivity: %u",
							 available_session_index );
		}
		else
		{
			/* Misra Suppression */
		}
	}
	/* Session available. return with session index */
	else
	{
		/* Misra Suppression */
	}

	return ( available_session_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
USER_SESSION_ERR_TD User_Session::Capture_Failed_Attempt( uint8_t user_index, uint32_t ip_address,
														  uint8_t port, uint16_t auth_type,
														  uint8_t* uri )
{
	USER_SESSION_ERR_TD status = USER_SESS_AUTH_REQUIRED_ERR;
	uint16_t max_failed_count = 0U;
	uint8_t log_param[User_Account::USER_NAME_LIMIT + 6U] = {0U};

	if ( user_index < m_user_account->Get_Total_User_Count() )
	{
		if ( m_user_lock_info[user_index].login_attempt == true )
		{
			m_user_lock_info[user_index].failed_attempt_counter = 0U;
		}

		// coverity[var_deref_model]
		m_max_failed_login_owner->Check_Out( &max_failed_count );

		if ( m_user_lock_info[user_index].failed_attempt_counter != MAX_FAIL_ATTEMPT_COUNTER )
		{
			if ( auth_type == HTTP_AUTH_TYPE_DIGEST )
			{
				m_user_lock_info[user_index].failed_attempt_counter++;
				USER_SESS_PRINT( DBG_MSG_ERROR, "Failed login attempt counter: %u",
								 m_user_lock_info[user_index].failed_attempt_counter );
			}
			else if ( auth_type == HTTP_AUTH_TYPE_CUSTOM_AUTH )
			{
				/* Checking if URI received is Login URI or not */
				if ( ( uri != nullptr ) && ( !strncmp( reinterpret_cast<char_t*>( uri ),
													   reinterpret_cast<char_t*>( const_cast<
																					  uint8_t*>
																				  ( LOGIN_URI ) ),
													   BF_Lib::Get_String_Length( ( const_cast<
																						uint8_t*>
																					( LOGIN_URI ) ),
																				  AUTH_BUF_URI_FIELD_LENGTH ) ) ) )
				{
					m_user_lock_info[user_index].failed_attempt_counter++;
					USER_SESS_PRINT( DBG_MSG_ERROR, "Failed login attempt for UserId: %u, Counter: %u, from IP: %X",
									 user_index, m_user_lock_info[user_index].failed_attempt_counter, ip_address );
				}
			}
			else
			{}

			/* if the URI is Logout URI then error will be 202(Accepted). I know it may trick the
			   user/script.
			 * We are still incrementing the failed attempt counter */
			if ( ( uri != nullptr ) &&
				 ( !strncmp( reinterpret_cast<char_t*>( uri ),
							 reinterpret_cast<char_t*>( const_cast<uint8_t*>( LOGOUT_URI ) ),
							 BF_Lib::Get_String_Length( ( const_cast<uint8_t*>( LOGOUT_URI ) ),
														AUTH_BUF_URI_FIELD_LENGTH ) ) ) )
			{
				status = USER_SESS_ACCEPTED_ERR;
			}
		}

		m_user_lock_info[user_index].login_attempt = false;

		if ( ( m_user_lock_info[user_index].failed_attempt_counter > 0U ) &&
			 ( ( max_failed_count != 0U ) &&
			   ( ( m_user_lock_info[user_index].failed_attempt_counter % max_failed_count ) ==
				 0U ) ) )
		{
			m_user_lock_info[user_index].is_lock = true;
			m_user_lock_info[user_index].unlock_time = m_fr_counter;

			USER_SESS_PRINT( DBG_MSG_ERROR, "This User has been locked: %u, UserId: %u", m_fr_counter, user_index );

			/*Also We need to take log*/
			if ( ( m_user_account->Get_User_Name(
					   user_index,
					   &log_param[0],
					   User_Account::USER_NAME_LIMIT + 1U ) ) )
			{
				uint8_t data_len = BF_Lib::Get_String_Length(
					log_param,
					( User_Account::USER_NAME_LIMIT + 1U ) );
				for ( uint8_t i = data_len; i < ( User_Account::USER_NAME_LIMIT + 1U ); i++ )
				{
					log_param[i] = 0;
				}
				log_param[User_Account::USER_NAME_LIMIT + 1U] = ip_address & 0xffU;
				log_param[User_Account::USER_NAME_LIMIT + 2U] = ( ip_address & 0xff00U ) >> 8U;
				log_param[User_Account::USER_NAME_LIMIT + 3U] = ( ip_address & 0xff0000U ) >> 16U;
				log_param[User_Account::USER_NAME_LIMIT + 4U] = ( ip_address & 0xff000000U ) >> 24U;
				if ( port == HTTPS_MEDIA )
				{
					log_param[User_Account::USER_NAME_LIMIT +
							  5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTPS;
				}
				else
				{
					log_param[User_Account::USER_NAME_LIMIT +
							  5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTP;
				}
				if ( m_session_log_param_cb != nullptr )
				{
					( *m_session_log_param_cb )( log_param );
				}

				if ( m_session_log_audit_capture_cb != nullptr )
				{
					( *m_session_log_audit_capture_cb )(
						LOG_EVENT_LOG_LOCKED_USER_X );
				}
			}
			/* We need to take log */
			status = USER_SESS_USER_LOCKED_ERR;
		}
		else
		{
			/* Misra */
		}
	}
	else
	{
		/* Misra */
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Session::Block_Session_Expiration( uint8_t* nonce, session_id_t* session_id )
{
	bool status = false;

	if ( nonce != nullptr )
	{
		for ( uint16_t id = 0; id < m_max_sessions; id++ )
		{
			if ( m_sessions[id].is_active == true )
			{
				if ( BF_Lib::String_Compare( nonce,
											 reinterpret_cast<uint8_t const*>( m_sessions[id].nonce ),
											 static_cast<uint32_t>( NONCE_LENGTH ) ) == true )
				{
					m_sessions[id].timeout_expiry = SESSION_EXPIRY_BLOCK;
					*session_id = id;
					id = m_max_sessions;
					status = true;
				}
			}
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Session::Unblock_Session_Expiration( session_id_t session_id )
{
	if ( session_id < m_max_sessions )
	{
		m_sessions[session_id].timeout_expiry = SESSION_EXPIRY_UNBLOCK;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Session_Audit_Log_Assign_Callback( Session_Audit_Function_Decl_1 session_audit_log_cb,
										Session_Audit_Function_Decl_2 session_audit_param_cb )
{
	m_session_log_audit_capture_cb = session_audit_log_cb;
	m_session_log_param_cb = session_audit_param_cb;
}
