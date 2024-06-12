/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "User_Management.h"
#include "Log_Events.h"
#include "User_Session.h"
#include "DCI_Enums.h"
#include "StdLib_MV.h"
#include "Web_Debug.h"
#include "REST_Common.h"

extern uint8_t dest_data[];

enum
{
	USERACCOUNT_TAG_STATE = 0U,
	FULLNAME_TAG_STATE,
	USERNAME_TAG_STATE,
	PWD_COMPLEXITY_TAG_STATE,
	PWD_TIMEOUT_TAG_STATE,
	CUR_PWD_TAG_STATE,
	PWD_TAG_STATE,
	ROLE_TAG_STATE,
	PWD_SET_EPOCH_TIME_TAG_STATE,
	MAX_TAG_STATE	/* Dont insert any enum after this */
};

static const uint16_t CHANGE_PWD_MASK =
	( ( 1U << USERACCOUNT_TAG_STATE ) | ( 1U << CUR_PWD_TAG_STATE
										  ) |
	  ( 1U << PWD_TAG_STATE )
	  | ( 1U << PWD_SET_EPOCH_TIME_TAG_STATE ) );

static const uint16_t RECEIVED_TAG_MASK = ( ( 1U << USERACCOUNT_TAG_STATE ) |
											( 1U << FULLNAME_TAG_STATE ) |
											( 1U << USERNAME_TAG_STATE ) |
											( 1U << PWD_COMPLEXITY_TAG_STATE ) |
											( 1U << PWD_TIMEOUT_TAG_STATE ) |
											( 1U << PWD_TAG_STATE ) |
											( 1U << ROLE_TAG_STATE ) |
											( 1U << PWD_SET_EPOCH_TIME_TAG_STATE ) );

static const uint8_t PWD_LIMIT = 20U;
static const uint8_t FULL_NAME_LIMIT = 32U;
static const uint8_t USERNAME_BASE64_SIZE_LIMIT = 28U;
static const uint8_t FULLNAME_BASE64_SIZE_LIMIT = 44U;
static const uint8_t PWD_COMPLEXITY_LIMIT = 3U;
static const uint8_t PWD_TIMEOUT_LIMIT = 5U;
static const uint8_t CUR_PWD_BASE64_SIZE_LIMIT = 28U;
static const uint8_t PWD_BASE64_SIZE_LIMIT = 28U;
static const uint8_t ROLE_SIZE_LIMIT = 20U;
static const uint8_t PWD_SET_EPOCH_TIME_SIZE_LIMIT = 10U;
static const uint8_t MAX_PWD_COMPLEXITY = 3U;
static const uint16_t MAX_PWD_TIMEOUT_DAYS = 65535U;
static const uint8_t ADMIN_USER_INDEX = 0U;

static const uint8_t tag_content_len[MAX_TAG_STATE] =
{
	0U,
	FULLNAME_BASE64_SIZE_LIMIT,
	USERNAME_BASE64_SIZE_LIMIT,
	PWD_COMPLEXITY_LIMIT,
	PWD_TIMEOUT_LIMIT,
	CUR_PWD_BASE64_SIZE_LIMIT,
	PWD_BASE64_SIZE_LIMIT,
	ROLE_SIZE_LIMIT,
	PWD_SET_EPOCH_TIME_SIZE_LIMIT
};

typedef union user_tag_rx_t
{
	uint16_t all;
	struct user_st
	{
		uint8_t user_account_rx : 1U;
		uint8_t full_name_rx : 1U;
		uint8_t user_name_rx : 1U;
		uint8_t pwd_complexity_rx : 1U;
		uint8_t pwd_timeout_rx : 1U;
		uint8_t cur_pwd_rx : 1U;
		uint8_t pwd_rx : 1U;
		uint8_t role_rx : 1U;
		uint8_t pwd_set_epoch_time_rx : 1U;
	} field;

} user_tag_rx_t;

typedef struct
{
	char full_name[FULLNAME_BASE64_SIZE_LIMIT + 1U];
	char user_name[USERNAME_BASE64_SIZE_LIMIT + 1U];
	char cur_password[PWD_BASE64_SIZE_LIMIT + 1U];
	char password[PWD_BASE64_SIZE_LIMIT + 1U];
	char pwd_set_epoch_time[PWD_SET_EPOCH_TIME_SIZE_LIMIT + 1U];
	char role[ROLE_SIZE_LIMIT + 1U];
	uint8_t pwd_complexity;
	uint16_t pwd_timeout;
	uint8_t counter;
	user_tag_rx_t user_tag_rx;
} user_mgmt_data_t;



static const char_t USER_ACCOUNT_TAG_DEF[] = "UserAccount";
static const char_t FULL_NAME_TAG_DEF[] = "FullName";
static const char_t USER_NAME_TAG_DEF[] = "UserName";
static const char_t PWD_COMPLEXITY_TAG_DEF[] = "PwdComplexity";
static const char_t PWD_TIMEOUT_TAG_DEF[] = "PwdTimeoutDays";
static const char_t CUR_PWD_TAG_DEF[] = "LoggedInUserPwd";
static const char_t PWD_TAG_DEF[] = "Pwd";
static const char_t ROLE_TAG_DEF[] = "Role";
static const char_t PWD_SET_EPOCH_TIME_TAG_DEF[] = "PwdSetEpochTime";

static const char_t* User_info_Tags[MAX_TAG_STATE] =
{
	USER_ACCOUNT_TAG_DEF,
	FULL_NAME_TAG_DEF,
	USER_NAME_TAG_DEF,
	PWD_COMPLEXITY_TAG_DEF,
	PWD_TIMEOUT_TAG_DEF,
	CUR_PWD_TAG_DEF,
	PWD_TAG_DEF,
	ROLE_TAG_DEF,
	PWD_SET_EPOCH_TIME_TAG_DEF
};

typedef struct
{
	uint8_t* new_pwd;
	uint8_t* cur_pwd;
	uint8_t* full_name;
	uint8_t* user_name;
	uint8_t pwd_complexity_level;		/* The input level against which the pwd complexity to be verified */
} pwd_complexity_pass;

static const uint8_t LOGIN_TIME_SIZE_LIMIT = 10U;

typedef struct login_date_time_t
{
	char_t login_time[LOGIN_TIME_SIZE_LIMIT + 1U];
	uint16_t counter;
} login_date_time_t;

enum
{
	PUT_CURR_USER_USERACCOUNT_TAG_STATE = 0U,
	PUT_CURR_USER_TIME_TAG_STATE,
	PUT_CURR_USER_COMPLETE_TAG_STATE,
	PUT_CURR_USER_MAX_TAG_STATE	/* Dont insert any enum after this */
};

static const char_t PWD_LOGIN_TIME_TAG_DEF[] = "LastLoginTime";

static const char_t* user_date_time_Tags[PUT_CURR_USER_COMPLETE_TAG_STATE] =
{
	USER_ACCOUNT_TAG_DEF,
	PWD_LOGIN_TIME_TAG_DEF
};

static const uint8_t time_tag_content_len[PUT_CURR_USER_COMPLETE_TAG_STATE] =
{
	0U,
	LOGIN_TIME_SIZE_LIMIT
};

/************************************************************************************
* Below tags and structurs has been used to parse /rs/users/accounts/reset URI
************************************************************************************/
typedef union user_reset_tag_rx_t
{
	uint8_t all;
	struct user_st
	{
		uint8_t user_account_rx : 1U;
		uint8_t cur_pwd_rx : 1U;
	} field;
} user_reset_tag_rx_t;
typedef struct
{
	char cur_password[PWD_BASE64_SIZE_LIMIT + 1U];
	uint8_t counter;
	user_reset_tag_rx_t user_tag_rx;
} user_reset_data_t;
enum reset_user_tags_t
{
	USERACC_TAG_DEF,
	LOGGEDIN_PWD_TAG_DEF,
	MAX_RESET_USER_TAG
};

static const char_t* RESET_USERS_TAGS[MAX_RESET_USER_TAG] =
{
	USER_ACCOUNT_TAG_DEF,
	CUR_PWD_TAG_DEF
};
static const uint8_t RESET_DEFAULT_USER_ACC = 1U;
static const uint8_t RESET_ALL_USERS_ACC = 2U;
/************************************************************************************
************************************************************************************/

static const uint8_t PWD_LEVEL_0_LENGTH = 6U;
static const uint8_t PWD_LEVEL_1_LENGTH = 8U;
static const uint8_t PWD_LEVEL_2_LENGTH = 12U;
static const uint8_t PWD_LEVEL_3_LENGTH = 16U;

static User_Audit_Function_Decl_1 m_user_log_audit_capture_cb =
	reinterpret_cast<User_Audit_Function_Decl_1>( nullptr );
static User_Audit_Function_Decl_2 m_user_log_param_cb =
	reinterpret_cast<User_Audit_Function_Decl_2>( nullptr );

static User_Session* m_user_session_handler = nullptr;
static User_Account* m_user_account_handler = nullptr;

extern uint32_t Get_Epoch_Time( void );

uint32_t Parse_UserMgmt_Resources_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Parse_Accounts_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Parse_Roles_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Parse_Session_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

bool User_Logout( hams_parser* parser );

static uint32_t Users_Response_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Users_Accounts_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Users_Session_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Users_Roles_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Userid_Info_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Put_Post_User_Details_Cb( hams_parser* parser, const uint8_t* data,
										  uint32_t length );

static bool_t Is_UserName_Valid( uint8_t* srcptr, uint8_t length );

static uint32_t User_Delete_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t User_Role_Details_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Users_Accounts_Options( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Parse_Users_Help_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint16_t pwd_complexity_check_true( pwd_complexity_pass* pwd_structure );

static uint32_t Method_Not_Allowed_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Users_Accounts_curruser_Options( hams_parser* parser, const uint8_t* data,
												 uint32_t length );

static uint32_t Curr_User_Put_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Init_Curr_User_Put_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static bool_t Validate_Date_Time_And_Convert_To_Numeric( char_t* str_time );

static uint16_t Validate_Role_Change_or_Delete_Condition( uint8_t req_user_index, uint16_t bitmap );

/**
 * @brief Function to reset the default admin password
 * @details This function will reset the default admin password to default password based on bitmap ( active users list
 * )
 * @param[in] bitmap: Active users list
 * @return error code
 */
uint16_t Reset_Default_User( uint16_t bitmap );

/**
 * @brief Function to reset all users
 * @details This function will reset all users and create default admin
 * @return error code
 */
uint16_t Factory_Reset( void );

/**
 * @brief Function to parse Options URI for /rs/users/accounts/reset/1 {reset id}
 * @details This function will return Options available for this URI
 * @param[in] parser: Hams parser
 * @param[in] data: URI Data
 * @param[in] length: Length of data
 * @return 0
 */
static uint32_t Options_User_Reset_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief Function to parse /rs/users/accounts/reset/1 {reset id} URI
 * @details This function will parse the URI and will store the reset id
 * @param[in] parser: Hams parser
 * @param[in] data: URI Data
 * @param[in] length: Length of data
 * @return 0
 */
static uint32_t Post_User_Reset_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief Function to parse payload of /rs/users/accounts/reset/1 {reset id} URI
 * @details This function will parse payload of the reset URI
 * @param[in] parser: Hams parser
 * @param[in] data: URI Data
 * @param[in] length: Length of data
 * @return 0
 */
static uint32_t User_Reset_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief Function authenticate new password received
 * @details This function authenticate new password received from user as per new California and Oregon law
 * @param[in] user_id: User ID.
 * @param[out] user_mgmt_data: User account data
 * @return error code
 */
static uint32_t Verify_Password( uint8_t user_id, user_mgmt_data_t* user_mgmt_data, uint8_t data_len );

/**
 * @brief Function to create default admin if  user space is avaliable
 * @details This function will create  the default admin password to default password based on bitmap ( active users
 * list )
 * @param[in] bitmap: Active users list
 * @param[in] def_admin_userid
 * @return error code
 */
uint16_t Create_Default_User( uint16_t bitmap, const uint8_t def_admin_userid );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Session_Init_Hanlder( User_Session* user_session )
{
	m_user_session_handler = user_session;
	m_user_account_handler = user_session->m_user_account;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Audit_Log_Assign_Callback( User_Audit_Function_Decl_1 user_audit_log_cb,
									 User_Audit_Function_Decl_2 user_audit_param_cb )
{
	m_user_log_audit_capture_cb = user_audit_log_cb;
	m_user_log_param_cb = user_audit_param_cb;
}

/* /rs/users callback */
uint32_t Parse_UserMgmt_Resources_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	rs->p.usermgmt.req_user_id = 0U;
	rs->p.usermgmt.req_user_role = 0U;
	if ( length != 1U )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( ( HTTP_METHOD_GET == parser->method ) || ( HTTP_METHOD_HEAD == parser->method ) )
			{
				if ( 0U != rs->auth_level )
				{
					parser->response_code = 200U;
					parser->user_data_cb = static_cast<hams_data_cb>( Users_Response_Cb );
				}
				else
				{
					hams_respond_error( parser, 401U );
				}
			}
			else if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				Validate_Options_Cors_Headers( parser, rs, Users_Response_Cb );
			}
			else
			{
				hams_respond_error( parser, 405U );	/* Method not allowed */
			}
		}
		else
		{
			if ( ( length == 9U ) &&
				 ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/accounts", length ) ==
				   0 ) )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( Parse_Accounts_Cb );
			}
			else if ( ( length == 6U ) &&
					  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/roles",
								 length ) == 0 ) )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( Parse_Roles_Cb );
			}
			else if ( ( length == 5U ) &&
					  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/help",
								 length ) == 0 ) )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( Parse_Users_Help_Cb );
			}
			else if ( ( length == 8U ) &&
					  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/session",
								 length ) == 0 ) )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( Parse_Session_Cb );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Users_Response_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	if ( HTTP_METHOD_OPTIONS == parser->method )
	{
		hams_response_header( parser, HTTP_HDR_ALLOW, "GET, HEAD, OPTIONS",
							  STRLEN( "GET, HEAD, OPTIONS" ) );
		hams_response_header( parser, COUNT_HTTP_HDRS, reinterpret_cast<const void*>( NULL ), 0U );
	}
	else
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		if ( 0U != hams_can_respond( data, length ) )
		{
			rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
			Http_Server_Begin_Dynamic( parser );
			Http_Server_Print_Dynamic( parser, XML_DECL XML_RS_USERS );
			if ( rs->flags & REQ_FLAG_NO_DYN_PARSER_SPACE )
			{
				if ( Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0U );
}

/* /rs/users/session Callback */
static uint32_t Parse_Session_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	if ( length != 1U )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			if ( HTTP_METHOD_DELETE == parser->method )
			{
				User_Logout( parser );
				parser->response_code = 202U;
			}
			else if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				Validate_Options_Cors_Headers( parser, rs, Users_Session_Cb );
			}
			else
			{
				hams_respond_error( parser, 405U );
			}
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<void*>( nullptr ), 0U );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}

	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Logout( hams_parser* parser )
{
	/* This function will contain logics for logging out user */
	bool return_val = true;
	Digest_data* digest_data_ptr = parser->cors_auth_common->digest_ptr;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	m_user_session_handler->Logout_Session( digest_data_ptr, ( rs->user_id - 1U ) );
	return ( return_val );
}

/* /rs/users/accounts Callback */
static uint32_t Parse_Accounts_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	if ( length != 1U )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( ( HTTP_METHOD_GET == parser->method ) || ( HTTP_METHOD_HEAD == parser->method ) )
			{
				if ( 0U != rs->auth_level )
				{
					parser->response_code = 200U;
					parser->user_data_cb = static_cast<hams_data_cb>( Users_Accounts_Cb );
				}
				else
				{
					hams_respond_error( parser, 401U );
				}
			}
			else if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				Validate_Options_Cors_Headers( parser, rs, Users_Accounts_Cb );
			}
			else if ( ( ( HTTP_METHOD_POST == parser->method ) &&
						( rs->auth_level == USER_ROLE_ADMIN ) ) ||
					  ( ( HTTP_METHOD_PUT == parser->method ) &&
						( rs->p.usermgmt.req_user_id != 0U ) &&
						( rs->user_id != 0U ) ) )
			{
				if ( parser->content_length != 0U )
				{
					parser->user_data_cb = static_cast<hams_data_cb>
						( Put_Post_User_Details_Cb );
					rs->p.usermgmt.yxml = Http_Server_Alloc_Xml_Parser( parser, 32U );
					uint8_t num_blocks = ( ( sizeof( user_mgmt_data_t ) )
										   + ( HTTP_POOL_BLOCK_SIZE - 1U ) ) / HTTP_POOL_BLOCK_SIZE;
					/* Allocate ourselves a block for xml parsing. */
					user_mgmt_data_t* meta_data =
						reinterpret_cast<user_mgmt_data_t*>( Http_Server_Alloc_Scratch( parser,
																						num_blocks ) );
					if ( meta_data != NULL )
					{
						rs->p.usermgmt.put_request = ( void* )meta_data;
					}
					else
					{
						hams_respond_error( parser, E503_SCRATCH_SPACE );
					}
				}
				else
				{
					hams_respond_error( parser, 411U );
				}
			}
			else
			{
				if ( ( HTTP_METHOD_POST == parser->method ) || ( rs->user_id == 0U ) )
				{
					hams_respond_error( parser, 401U );
				}
				else
				{
					hams_respond_error( parser, 405U );	/* Method not allowed */
				}
			}
		}
		else
		{
			uint8_t user_id = 0U;
			if ( ERR_PARSING_OK ==
				 INT_ASCII_Conversion::str_to_uint8( ( data ), 1U, length - 1U, &user_id ) )
			{
				if ( ( user_id < 1U ) || ( user_id > MAX_NUM_OF_AUTH_USERS ) )
				{
					hams_respond_error( parser, 400U );
				}
				else
				{
					rs->p.usermgmt.req_user_id = user_id;
					if ( HTTP_METHOD_DELETE == parser->method )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( User_Delete_Cb );
					}
					else if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						parser->user_data_cb = static_cast<hams_data_cb>
							( Users_Accounts_Options );
					}
					else if ( ( HTTP_METHOD_GET == parser->method ) ||
							  ( HTTP_METHOD_HEAD == parser->method ) )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( Userid_Info_Cb );
					}
					else if ( HTTP_METHOD_PUT != parser->method )
					{
						parser->user_data_cb = static_cast<hams_data_cb>
							( Method_Not_Allowed_Cb );
					}
					else
					{	/* Misra Suppress */
					}
				}
			}
			else if ( ( length == 9U ) &&
					  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/curruser",
								 length ) ==
						0U ) )
			{
				if ( ( HTTP_METHOD_GET == parser->method ) ||
					 ( HTTP_METHOD_HEAD == parser->method ) )
				{	/* magic number for curr_user to be verified in next callback */
					rs->p.usermgmt.req_user_id = 0xFEU;
					parser->user_data_cb = static_cast<hams_data_cb>( Userid_Info_Cb );
				}
				else if ( HTTP_METHOD_PUT == parser->method )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Init_Curr_User_Put_Cb );
				}
				else if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					parser->user_data_cb =
						static_cast<hams_data_cb>( Users_Accounts_curruser_Options );
				}
				else
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Method_Not_Allowed_Cb );
				}
			}
			else if ( ( length == 6U ) &&
					  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/reset", length ) == 0U ) )
			{
				// Check if this is a POST Method call
				if ( HTTP_METHOD_POST == parser->method )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Post_User_Reset_Cb );
				}
				else if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					parser->user_data_cb =
						static_cast<hams_data_cb>( Options_User_Reset_Cb );
				}
				else
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Method_Not_Allowed_Cb );
				}
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Method_Not_Allowed_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	hams_respond_error( parser, 405U );
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Users_Accounts_Options( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	if ( length != 1U )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			Validate_Options_Cors_Headers( parser, rs, nullptr );
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				hams_response_header( parser, HTTP_HDR_ALLOW, "GET, HEAD, PUT, DELETE, OPTIONS",
									  STRLEN( "GET, HEAD, PUT, DELETE, OPTIONS" ) );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Users_Accounts_curruser_Options( hams_parser* parser, const uint8_t* data,
												 uint32_t length )
{
	if ( length != 1U )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			Validate_Options_Cors_Headers( parser, rs, nullptr );
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				hams_response_header( parser, HTTP_HDR_ALLOW, "GET, HEAD, PUT, OPTIONS",
									  STRLEN( "GET, HEAD, PUT, OPTIONS" ) );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Audit_Log_Params_Update( hams_parser* parser )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	uint8_t log_param[User_Account::USER_NAME_LIMIT + 6U] = {0U};
	uint16_t data_len = 0U;
	ip_address_setting_t ip_address_setting;
	uint16_t error_code = 200U;

	ip_address_setting.ip_address = parser->general_use_reg;
	ip_address_setting.port = parser->remote_port;
	log_param[User_Account::USER_NAME_LIMIT + 1U] = ip_address_setting.ip_address & 0xffU;
	log_param[User_Account::USER_NAME_LIMIT + 2U] = ( ip_address_setting.ip_address & 0xff00U ) >> 8U;
	log_param[User_Account::USER_NAME_LIMIT + 3U] = ( ip_address_setting.ip_address & 0xff0000U ) >> 16U;
	log_param[User_Account::USER_NAME_LIMIT + 4U] = ( ip_address_setting.ip_address & 0xff000000U ) >> 24U;
	if ( parser->https == HTTPS_MEDIA )
	{
		log_param[User_Account::USER_NAME_LIMIT + 5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTPS;
	}
	else
	{
		log_param[User_Account::USER_NAME_LIMIT + 5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTP;
	}
	if ( ( m_user_account_handler->Get_User_Name( rs->user_id - 1U, &log_param[0],
												  User_Account::USER_NAME_LIMIT + 1U ) ) )
	{
		error_code = 200U;
	}
	else
	{
		error_code = 400U;
	}

	data_len = BF_Lib::Get_String_Length( log_param, ( User_Account::USER_NAME_LIMIT + 1U ) );
	for ( uint8_t i = data_len; i < ( User_Account::USER_NAME_LIMIT + 1U ); i++ )
	{
		log_param[i] = 0U;
	}

	if ( m_user_log_param_cb != nullptr )
	{
		( *m_user_log_param_cb )( log_param );
	}
	if ( error_code != 200U )
	{
		hams_respond_error( parser, error_code );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Users_Accounts_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t bitmap = 0U;
	uint16_t len = 0U;
	uint8_t user_loc = 0U;
	uint8_t user_count = 0U;
	uint8_t total_users = 1U;

	if ( HTTP_METHOD_OPTIONS == parser->method )
	{
		hams_response_header( parser, HTTP_HDR_ALLOW, "GET, HEAD, POST, OPTIONS",
							  STRLEN( "GET, HEAD, POST, OPTIONS" ) );
		hams_response_header( parser, COUNT_HTTP_HDRS, reinterpret_cast<const void*>( NULL ), 0U );
	}
	else
	{
		if ( 0U != hams_can_respond( data, length ) )
		{
			parser->response_code = 200U;
			http_server_req_state* rs = Http_Server_Get_State( parser );
			rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
			Http_Server_Begin_Dynamic( parser );
			if ( USER_ROLE_ADMIN == rs->auth_level )
			{
				bitmap = m_user_account_handler->Get_User_Account_Valid_Bit_List();
				if ( bitmap != User_Account::UNKNOWN_ERROR )
				{
					for ( user_loc = 0U; ( user_loc < MAX_NUM_OF_AUTH_USERS ); user_loc++ )
					{
						if ( ( ( bitmap >> user_loc ) & 0x01U ) )
						{
							user_count += 1U;
						}
					}
				}
				total_users = user_count;
			}
			Http_Server_Print_Dynamic( parser, XML_DECL USER_ACCOUNT_PREFIX );
			Http_Server_Print_Dynamic( parser, TOTAL_USERS_PREFIX "\bu" TOTAL_USERS_SUFFIX,
									   total_users );
			Http_Server_Print_Dynamic( parser, MAX_USERS_PREFIX "\bu" MAX_USERS_SUFFIX,
									   MAX_NUM_OF_AUTH_USERS );
			if ( USER_ROLE_ADMIN == rs->auth_level )
			{
				if ( 0U != user_count )
				{
					for ( user_loc = 0U; ( user_loc < MAX_NUM_OF_AUTH_USERS ); user_loc++ )
					{
						if ( ( ( bitmap >> user_loc ) & 0x01U ) )
						{
							len = 0U;
							if ( m_user_account_handler->Get_User_Name( user_loc, dest_data,
																		( static_cast<uint8_t>
																		  ( len ) ) ) )
							{
								Http_Server_Print_Dynamic( parser, USER_NAME_TAG,
														   strlen(
															   ( const char* )dest_data ), dest_data,
														   ( user_loc + 1 ) );
							}
						}
					}
				}
			}
			else
			{
				len = 0U;
				if ( m_user_account_handler->Get_User_Name( rs->user_id - 1, dest_data,
															( static_cast<uint8_t>( len ) ) ) )
				{
					Http_Server_Print_Dynamic( parser, USER_NAME_TAG,
											   strlen( ( const char* )dest_data ), dest_data,
											   ( rs->user_id ) );
				}
			}
			Http_Server_Print_Dynamic( parser,
									   USERNAME_SIZE_LIMIT_PREFIX "\bu" USERNAME_SIZE_LIMIT_SUFFIX,
									   User_Account::USER_NAME_LIMIT );
			Http_Server_Print_Dynamic( parser,
									   FULLNAME_SIZE_LIMIT_PREFIX "\bu" FULLNAME_SIZE_LIMIT_SUFFIX,
									   FULL_NAME_LIMIT );
			Http_Server_Print_Dynamic( parser, USER_ACCOUNT_SUFFIX );
			if ( rs->flags & REQ_FLAG_NO_DYN_PARSER_SPACE )
			{
				if ( Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Users_Session_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	if ( HTTP_METHOD_OPTIONS == parser->method )
	{
		hams_response_header( parser, HTTP_HDR_ALLOW, "DELETE, OPTIONS",
							  STRLEN( "DELETE, OPTIONS" ) );
		hams_response_header( parser, COUNT_HTTP_HDRS, reinterpret_cast<const void*>( NULL ), 0U );
	}
	else
	{}

	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Put_Post_User_Details_Cb( hams_parser* parser, const uint8_t* data,
										  uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yxml_put_xml_state* ypx = reinterpret_cast<struct yxml_put_xml_state*>( rs->p.usermgmt
																				   .yxml );
	yxml_t* yx = rs->p.usermgmt.yxml;
	uint8_t counter;
	uint16_t error_code = 200U;
	pwd_complexity_pass complexity_structure = {0};

	uint8_t log_param[User_Account::USER_NAME_LIMIT + 6U] = {0U};

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		user_mgmt_data_t* usermgmt_p_data = reinterpret_cast<user_mgmt_data_t*>( rs->p.usermgmt
																				 .put_request );
		if ( parser->content_type != HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED )
		{
			error_code = 415U;
		}
		else
		{
			for ( counter = 0U; ( counter < length ) && ( error_code == 200U ); ++counter )
			{
				int32_t r = yxml_parse( yx, static_cast<int32_t>( data[counter] ) );
				switch ( r )
				{	/* Expecting the proper tag element. */
					case YXML_ELEMSTART:
						usermgmt_p_data->counter = 0U;
						if ( usermgmt_p_data->user_tag_rx.field.user_account_rx != 1U )
						{
							if ( 0U ==
								 strcmp( yx->elem,
										 reinterpret_cast<const char_t*>( User_info_Tags[
																			  USERACCOUNT_TAG_STATE] ) ) )
							{
								ypx->put_state = USERACCOUNT_TAG_STATE;
								usermgmt_p_data->user_tag_rx.field.user_account_rx = 1U;
							}
							else
							{
								error_code = 400U;
							}
						}
						else
						{
							error_code = 400U;
							for ( uint32_t i = 1U;
								  ( ( i < MAX_TAG_STATE ) && ( error_code == 400U ) ); i++ )
							{
								if ( 0U ==
									 strcmp( yx->elem,
											 reinterpret_cast<const char_t*>( User_info_Tags[i] ) ) )
								{
									if ( ( usermgmt_p_data->user_tag_rx.all & ( 1U << i ) ) == 0U )
									{
										error_code = 200U;
										ypx->put_state = i;
										usermgmt_p_data->user_tag_rx.all |= 1U << i;
									}
									else
									{
										i = MAX_TAG_STATE;
									}
								}
							}
						}
						break;

					case YXML_CONTENT:
						if ( FULLNAME_TAG_STATE == ypx->put_state )
						{
							usermgmt_p_data->full_name[usermgmt_p_data->counter] = yx->data[0];
						}
						else if ( USERNAME_TAG_STATE == ypx->put_state )
						{
							usermgmt_p_data->user_name[usermgmt_p_data->counter] = yx->data[0];
						}
						else if ( PWD_COMPLEXITY_TAG_STATE == ypx->put_state )
						{
							if ( IS_DIGIT( yx->data[0] ) )
							{
								uint32_t pwd_complexity = usermgmt_p_data->pwd_complexity;
								pwd_complexity *= 10U;
								pwd_complexity += ( yx->data[0] - 0x30U );
								if ( pwd_complexity > MAX_PWD_COMPLEXITY )
								{
									error_code = 400U;
								}
								else
								{
									usermgmt_p_data->pwd_complexity = pwd_complexity;
								}
							}
							else
							{
								error_code = 400U;
							}
						}
						else if ( PWD_TIMEOUT_TAG_STATE == ypx->put_state )
						{
							if ( IS_DIGIT( yx->data[0] ) )
							{
								uint32_t pwd_timeout = usermgmt_p_data->pwd_timeout;
								pwd_timeout *= 10U;
								pwd_timeout += ( yx->data[0] - 0x30U );
								if ( pwd_timeout > MAX_PWD_TIMEOUT_DAYS )
								{
									error_code = 400U;
								}
								else
								{
									usermgmt_p_data->pwd_timeout = pwd_timeout;
								}
							}
							else
							{
								error_code = 400U;
							}
						}
						else if ( CUR_PWD_TAG_STATE == ypx->put_state )
						{
							usermgmt_p_data->cur_password[usermgmt_p_data->counter] = yx->data[0];
						}
						else if ( PWD_TAG_STATE == ypx->put_state )
						{
							usermgmt_p_data->password[usermgmt_p_data->counter] = yx->data[0];
						}
						else if ( ROLE_TAG_STATE == ypx->put_state )
						{
							usermgmt_p_data->role[usermgmt_p_data->counter] = yx->data[0];
						}
						else if ( PWD_SET_EPOCH_TIME_TAG_STATE == ypx->put_state )
						{
							usermgmt_p_data->pwd_set_epoch_time[usermgmt_p_data->counter] =
								yx->data[0];
						}
						else
						{	/* Misra supress */
						}
						if ( ++usermgmt_p_data->counter > tag_content_len[ypx->put_state] )
						{
							error_code = 400U;
						}
						break;

					case YXML_ELEMEND:
						if ( *yx->elem == static_cast<char_t>( 0U ) )
						{
							if ( ( ( ( length - counter ) == 1U ) && ( data[counter] == 0x3EU ) ) ||
								 ( ( ( length - counter ) > 1U ) && ( data[counter] == 0x3EU ) &&
								   ( IS_SP_OR_HT( data[counter + 1U] ) ||
									 IS_NEW_LINE( data[counter + 1U] ) ) ) )
							{
								ypx->put_state = MAX_TAG_STATE;
							}
							else
							{
								error_code = 400U;
							}
						}
						if ( MAX_TAG_STATE == ypx->put_state )
						{
							uint32_t decoded_length = 0U;
							uint16_t bitmap = 0U;
							uint8_t user_loc = 0U;
							bool_t empty_found = false;
							uint16_t buffer_length = 0U;
							uint16_t data_len = 0U;
							bitmap = m_user_account_handler->Get_User_Account_Valid_Bit_List();
							if ( bitmap != User_Account::UNKNOWN_ERROR )
							{
								if ( parser->method == HTTP_METHOD_POST )
								{
									for ( user_loc = 0U;
										  ( user_loc < MAX_NUM_OF_AUTH_USERS ) &&
										  ( false == empty_found ); user_loc++ )
									{
										if ( 0U == ( ( bitmap >> user_loc ) & 0x01U ) )
										{
											empty_found = true;
										}
									}
									if ( empty_found == false )
									{
										error_code = E422_USER_DATABASE_FULL;
									}
									else
									{
										if ( ( usermgmt_p_data->user_tag_rx.all &
											   RECEIVED_TAG_MASK ) !=
											 RECEIVED_TAG_MASK )
										/* all tags not received except current password */
										{
											error_code = 400U;
										}
										else
										{
											user_loc--;
										}
									}
								}
								else
								{
									if ( ( bitmap && ( 1U << ( rs->p.usermgmt.req_user_id - 1U ) ) ) ==
										 0U )
									{
										error_code = 404U;
									}
									else
									{
										user_loc = rs->p.usermgmt.req_user_id - 1U;
										if ( ( usermgmt_p_data->user_tag_rx.field.user_name_rx ==
											   1U ) ||
											 ( usermgmt_p_data->user_tag_rx.field
											   .pwd_complexity_rx == 1U ) ||
											 ( usermgmt_p_data->user_tag_rx.field.pwd_rx == 1U ) ||
											 ( usermgmt_p_data->user_tag_rx.field.
											   pwd_set_epoch_time_rx ==
											   1U ) )
										{
											if ( error_code == 200U )
											{
												if ( ( usermgmt_p_data->user_tag_rx.all
													   & CHANGE_PWD_MASK ) != CHANGE_PWD_MASK )
												{
													error_code = 400U;
												}
												else if ( ( rs->auth_level != USER_ROLE_ADMIN ) &&
														  ( ( rs->p.usermgmt.req_user_id !=
															  rs->user_id ) ||
															( usermgmt_p_data->user_tag_rx.all !=
															  CHANGE_PWD_MASK ) ) )
												{
													error_code = 401U;
												}
												/* Check if current password matches with logged in user's password */
												else if ( ( usermgmt_p_data->user_tag_rx.field.
															pwd_rx == 1U ) && ( usermgmt_p_data->
																				user_tag_rx.field.
																				cur_pwd_rx == 1U ) )
												{
													data_len = 0U;
													uint8_t cur_pwd[CUR_PWD_BASE64_SIZE_LIMIT] =
													{'\0'};

													m_user_account_handler->Get_Password( ( rs->
																							user_id
																							- 1U ),
																						  cur_pwd,
																						  static_cast<uint8_t>
																						  ( data_len ) );
													uint16_t string_length = BF_Lib
														::
														Get_String_Length(
														reinterpret_cast
														<
															uint8_t
															*>(
															usermgmt_p_data
															->
															cur_password ),
														(
															PWD_BASE64_SIZE_LIMIT
															+
															1U ) );

													if ( BASE64_DECODING_ERR_OK != Base64::Decode( (
																									   uint8_t
																									   * )
																								   usermgmt_p_data
																								   ->
																								   cur_password,
																								   string_length,
																								   dest_data,
																								   &
																								   decoded_length,
																								   0U ) )
													{
														error_code = E422_DECRYPTION_FAILED;
													}
													else
													{
														if ( ( decoded_length !=
															   strlen( reinterpret_cast<const char_t
																						*>( cur_pwd ) ) )
															 ||
															 ( 0U !=
															   strncmp( reinterpret_cast<char_t*>(
																			cur_pwd ),
																		reinterpret_cast<
																			char_t*>( dest_data ),
																		BF_Lib::
																		Get_String_Length(
																			reinterpret_cast<uint8_t
																							 *>(
																				cur_pwd ),
																			(
																				CUR_PWD_BASE64_SIZE_LIMIT ) ) ) ) )
														{
															error_code = 403U;
														}
													}
												}
											}
										}
										else
										{
											if ( rs->auth_level != USER_ROLE_ADMIN )
											{
												error_code = 401U;
											}
										}
									}
								}
								if ( error_code == 200U )
								{
									if ( usermgmt_p_data->user_tag_rx.field.full_name_rx == 1U )
									{
										uint16_t str_len = BF_Lib::Get_String_Length( reinterpret_cast<uint8_t
																									   *>(
																						  usermgmt_p_data->
																						  full_name ),
																					  (
																						  FULLNAME_BASE64_SIZE_LIMIT
																						  + 1U ) );
										if ( BASE64_DECODING_ERR_OK !=
											 Base64::Decode(
												 ( uint8_t* )usermgmt_p_data->full_name,
												 str_len,
												 dest_data,
												 &decoded_length, 0U ) )
										{
											error_code = E422_DECRYPTION_FAILED;
										}
										else
										{
											if ( decoded_length <= FULL_NAME_LIMIT )
											{
												if ( IS_ALPHA( dest_data[0] ) )
												{
													memcpy( usermgmt_p_data->full_name, dest_data,
															decoded_length );
													usermgmt_p_data->full_name[decoded_length] = 0U;/*
																									   null character
																									 */
												}
												else
												{
													error_code = 400U;
												}
											}
											else
											{
												error_code = 400U;
											}
										}
									}
									if ( ( error_code == 200U ) &&
										 ( usermgmt_p_data->user_tag_rx.field.user_name_rx == 1U ) )
									{
										if ( BASE64_DECODING_ERR_OK !=
											 Base64::Decode(
												 ( uint8_t* )usermgmt_p_data->user_name,
												 BF_Lib::Get_String_Length( reinterpret_cast<uint8_t
																							 *>(
																				usermgmt_p_data->
																				user_name ),
																			(
																				USERNAME_BASE64_SIZE_LIMIT
																				+ 1U ) ),
												 dest_data,
												 &decoded_length, 0U ) )
										{
											error_code = E422_DECRYPTION_FAILED;
										}
										else
										{
											if ( decoded_length <= User_Account::USER_NAME_LIMIT )
											{
												if ( Is_UserName_Valid( dest_data,
																		decoded_length ) )
												{
													memcpy( usermgmt_p_data->user_name, dest_data,
															decoded_length );
													usermgmt_p_data->user_name[decoded_length] = 0U;/*
																									   null character
																									 */
												}
												else
												{
													error_code = 400U;
												}
											}
											else
											{
												error_code = 400U;
											}
										}
									}
									if ( ( error_code == 200U ) &&
										 ( usermgmt_p_data->user_tag_rx.field.pwd_rx == 1U ) )
									{
										if ( BASE64_DECODING_ERR_OK !=
											 Base64::Decode( ( uint8_t* )usermgmt_p_data->password,
															 BF_Lib::Get_String_Length(
																 reinterpret_cast<uint8_t*>(
																	 usermgmt_p_data->password ),
																 ( PWD_BASE64_SIZE_LIMIT + 1U ) ),
															 dest_data,
															 &decoded_length, 0U ) )
										{
											error_code = E422_DECRYPTION_FAILED;
										}
										else
										{
											if ( decoded_length <= PWD_LIMIT )
											{
												memcpy( usermgmt_p_data->password, dest_data,
														decoded_length );
												usermgmt_p_data->password[decoded_length] = 0U;	/*
																								   null character
																								 */
											}
											else
											{
												error_code = 400U;
											}
										}
									}
									if ( ( error_code == 200U ) &&
										 ( usermgmt_p_data->user_tag_rx.field.role_rx == 1U ) )
									{
										error_code = 400U;
										for ( uint32_t temp_counter = 0U;
											  ( ( temp_counter < NUM_OF_AUTH_LEVELS ) &&
												( error_code == 400U ) ); temp_counter++ )
										{
											if ( !memcmp( auth_struct[temp_counter].role_name,
														  ( uint8_t* )usermgmt_p_data->role,
														  strlen( auth_struct[temp_counter].
																  role_name ) ) )
											{
												rs->p.usermgmt.req_user_role =
													auth_struct[temp_counter].role_level;
												error_code = 200U;
												if ( ( rs->p.usermgmt.req_user_role !=
													   USER_ROLE_ADMIN ) &&
													 ( parser->method == HTTP_METHOD_PUT ) )
												{
													error_code =
														Validate_Role_Change_or_Delete_Condition(
															rs->p.usermgmt.req_user_id - 1U,
															bitmap );
												}
											}
										}
									}
									if ( ( error_code == 200U ) &&
										 ( usermgmt_p_data->user_tag_rx.field.pwd_set_epoch_time_rx
										   ==
										   1U ) )
									{	/* Expecting a decimal integer, in DD:MM:YYYY format */
										if ( false ==
											 Validate_Date_Time_And_Convert_To_Numeric(
												 usermgmt_p_data->pwd_set_epoch_time ) )
										{
											error_code = 400U;
										}
									}
									if ( error_code == 200U )
									{
										if ( usermgmt_p_data->user_tag_rx.field.user_name_rx == 1U )
										{
											for ( uint8_t user_id = 0U;
												  ( user_id < MAX_NUM_OF_AUTH_USERS ) &&
												  ( 200U == error_code ); user_id++ )
											{
												if ( ( ( bitmap >> user_id ) & 0x01U ) )
												{
													buffer_length = 0U;
													if ( m_user_account_handler->Get_User_Name(
															 user_id, dest_data,
															 ( static_cast<uint8_t>
															   ( buffer_length ) ) ) )
													{
														if ( !strcmp( usermgmt_p_data->user_name,
																	  ( char_t* )dest_data ) )
														{
															if ( ( parser->method ==
																   HTTP_METHOD_POST ) ||
																 ( ( rs->p.usermgmt.req_user_id
																	 - 1U ) != user_id ) )
															{
																error_code =
																	E422_USER_ALREADY_EXISTS;
															}
														}
													}
												}
											}
										}
										if ( error_code == 200U )
										{
											uint16_t data_len = 0U;
											if ( 1U == usermgmt_p_data->user_tag_rx.field.pwd_rx )
											{	/* Perform password complexity check first */
												uint8_t cur_pwd[USERNAME_BASE64_SIZE_LIMIT] = {'\0'};
												if ( 0U ==
													 usermgmt_p_data->user_tag_rx.field
													 .full_name_rx )
												{
													data_len = 0U;
													if ( m_user_account_handler->Get_User_Fullname(
															 user_loc,
															 reinterpret_cast<uint8_t*>(
																 usermgmt_p_data->
																 full_name ),
															 ( static_cast<uint8_t>
															   ( data_len ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}
												}
												complexity_structure.full_name =
													( uint8_t* )usermgmt_p_data->full_name;
												if ( ( error_code == 200U ) && ( 0U ==
																				 usermgmt_p_data->
																				 user_tag_rx.field
																				 .user_name_rx ) )
												{
													data_len = 0U;
													if ( m_user_account_handler->Get_User_Name(
															 user_loc,
															 reinterpret_cast<uint8_t*>(
																 usermgmt_p_data->
																 user_name ),
															 ( static_cast<uint8_t>
															   ( data_len ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}
												}
												complexity_structure.user_name =
													( uint8_t* )usermgmt_p_data->user_name;
												complexity_structure.new_pwd =
													( uint8_t* )usermgmt_p_data->password;
												if ( parser->method == HTTP_METHOD_POST )
												{
													complexity_structure.cur_pwd = NULL;
												}
												else
												{
													data_len = 0U;
													if ( m_user_account_handler->Get_Password(
															 user_loc, cur_pwd,
															 ( static_cast<uint8_t>
															   ( data_len ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}

													complexity_structure.cur_pwd = &cur_pwd[0];
												}
												if ( ( error_code == 200U ) &&
													 ( usermgmt_p_data->user_tag_rx.field
													   .pwd_complexity_rx ==
													   1U ) )
												{
													complexity_structure.pwd_complexity_level =
														usermgmt_p_data->pwd_complexity;
												}
												else
												{
													data_len = 0U;
													if ( m_user_account_handler->
														 Get_Password_Complexity( user_loc,
																				  &
																				  complexity_structure
																				  .
																				  pwd_complexity_level,
																				  ( static_cast<
																						uint8_t>
																					( data_len ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}
												}

												if ( 200U == error_code )
												{
													// coverity[var_deref_model]
													error_code = pwd_complexity_check_true( &complexity_structure );
												}
											}
											if ( 200U == error_code )
											{
												if ( usermgmt_p_data->user_tag_rx.field.full_name_rx
													 ==
													 1U )
												{
													data_len = BF_Lib::Get_String_Length(
														reinterpret_cast<uint8_t*>(
															usermgmt_p_data->
															full_name ),
														( FULLNAME_BASE64_SIZE_LIMIT + 1U ) );
													if ( ( data_len != 0U ) &&
														 ( m_user_account_handler->Set_User_Fullname(
															   user_loc,
															   reinterpret_cast
															   <
																   uint8_t
																   *>( usermgmt_p_data->
																	   full_name ),
															   (
																   static_cast<uint8_t>
																   ( data_len ) ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}
												}
												if ( ( error_code == 200U ) &&
													 ( usermgmt_p_data->user_tag_rx.field.
													   user_name_rx
													   ==
													   1U ) )
												{
													data_len = BF_Lib::Get_String_Length(
														reinterpret_cast<uint8_t*>(
															usermgmt_p_data->
															user_name ),
														( USERNAME_BASE64_SIZE_LIMIT + 1U ) );
													if ( ( data_len != 0U ) &&
														 ( m_user_account_handler->Set_User_Name(
															   user_loc,
															   reinterpret_cast
															   <uint8_t*>( usermgmt_p_data->
																		   user_name ),
															   (
																   static_cast<uint8_t>
																   ( data_len ) ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}
												}
												if ( ( error_code == 200U ) &&
													 ( usermgmt_p_data->user_tag_rx.field
													   .pwd_complexity_rx ==
													   1U ) )
												{
													data_len = 0U;
													if ( m_user_account_handler->
														 Set_Password_Complexity( user_loc,
																				  reinterpret_cast<
																					  uint8_t*>( &
																								 usermgmt_p_data
																								 ->
																								 pwd_complexity ),
																				  ( static_cast<
																						uint8_t>
																					( data_len ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}
												}
												if ( ( error_code == 200U ) &&
													 ( usermgmt_p_data->user_tag_rx.field
													   .pwd_timeout_rx ==
													   1U ) )
												{
													data_len = 0U;
													if ( m_user_account_handler->Set_Password_Aging(
															 user_loc,
															 reinterpret_cast<uint8_t*>( &
																						 usermgmt_p_data
																						 ->
																						 pwd_timeout ),
															 ( static_cast<uint8_t>
															   ( data_len ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}
												}
												if ( ( error_code == 200U ) &&
													 ( usermgmt_p_data->user_tag_rx.field.pwd_rx ==
													   1U ) )
												{
													data_len =
														BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>(
																					   usermgmt_p_data->
																					   password ),
																				   ( PWD_BASE64_SIZE_LIMIT +
																					 1U ) );

													if ( data_len != 0U )
													{
														error_code = Verify_Password( user_loc, usermgmt_p_data,
																					  data_len );
														if ( 200U == error_code )
														{
															/* Clear all sessions of this user */
															m_user_session_handler->Clear_User_Session( user_loc );
														}
													}
													else
													{
														error_code = 400U;
													}
													/* user password change event logging */
													if ( ( error_code == 200U ) &&
														 ( complexity_structure.cur_pwd != NULL ) )
													{
														ip_address_setting_t ip_address_setting;
														ip_address_setting.ip_address =
															parser->general_use_reg;
														ip_address_setting.port =
															parser->remote_port;
														log_param[User_Account::USER_NAME_LIMIT +
																  1U] =
															ip_address_setting.ip_address & 0xffU;
														log_param[User_Account::USER_NAME_LIMIT +
																  2U] =
															( ip_address_setting.ip_address &
															  0xff00U ) >>
															8U;
														log_param[User_Account::USER_NAME_LIMIT +
																  3U] =
															( ip_address_setting.ip_address &
															  0xff0000U ) >>
															16U;
														log_param[User_Account::USER_NAME_LIMIT +
																  4U] =
															( ip_address_setting.ip_address &
															  0xff000000U ) >> 24U;
														if ( parser->https == HTTPS_MEDIA )
														{
															log_param[User_Account::USER_NAME_LIMIT
																	  + 5U] =
																DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTPS;
														}
														else
														{
															log_param[User_Account::USER_NAME_LIMIT
																	  + 5U] =
																DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTP;
														}
														/*
														    check that logged user is admin or some other role based
														       user
														 */
														if ( rs->user_id - 1U == ( ADMIN_USER_INDEX ) )
														{
															if ( ( m_user_account_handler->
																   Get_User_Name(
																	   ADMIN_USER_INDEX,
																	   &log_param[0],
																	   User_Account::USER_NAME_LIMIT
																	   + 1U ) ) )
															{
																error_code = 200U;
															}
															else
															{
																error_code = 400U;
															}


															if ( m_user_log_param_cb != nullptr )
															{
																( *m_user_log_param_cb )( log_param );
															}
															if ( m_user_log_audit_capture_cb !=
																 nullptr )
															{
																( *m_user_log_audit_capture_cb )(
																	LOG_EVENT_ADMIN_USER );
															}
														}
														else
														{
															// MISRA compatible
														}

														if ( ( m_user_account_handler->Get_User_Name(
																   user_loc,
																   &log_param[0],
																   User_Account::USER_NAME_LIMIT +
																   1U ) ) )
														{
															error_code = 200U;
														}
														else
														{
															error_code = 400U;
														}
														data_len = BF_Lib::Get_String_Length(
															log_param,
															( User_Account::USER_NAME_LIMIT +
															  1U ) );
														for ( uint8_t i = data_len;
															  i <
															  ( User_Account::USER_NAME_LIMIT +
																1U );
															  i++ )
														{
															log_param[i] = 0;
														}
														if ( m_user_log_param_cb != nullptr )
														{
															( *m_user_log_param_cb )( log_param );
														}
														/*
														   log events based on whether admin changes pwd of role based
														      user or role based user does it for itself
														 */
														if ( rs->user_id - 1U == ( ADMIN_USER_INDEX ) )
														{
															if ( m_user_log_audit_capture_cb !=
																 nullptr )
															{
																( *m_user_log_audit_capture_cb )(
																	LOG_EVENT_OTHER_USER_PWD_CHANGED );
															}
														}
														else
														{
															if ( m_user_log_audit_capture_cb !=
																 nullptr )
															{
																( *m_user_log_audit_capture_cb )(
																	LOG_EVENT_SELF_USER_PWD_CHANGED );
															}
														}
													}
													else
													{
														// MISRA compatible
													}
												}
												if ( ( error_code == 200U ) &&
													 ( usermgmt_p_data->user_tag_rx.field.role_rx ==
													   1U ) )
												{
													data_len =
														sizeof( rs->p.usermgmt.req_user_role );
													if ( m_user_account_handler->Set_User_Role(
															 user_loc,
															 &rs->p.usermgmt.req_user_role,
															 ( static_cast<uint8_t>
															   ( data_len ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}
												}
												if ( ( error_code == 200U ) &&
													 ( usermgmt_p_data->user_tag_rx.field
													   .
													   pwd_set_epoch_time_rx == 1U ) )
												{
													data_len = 0U;
													/* set epoch time for password created*/
													if ( m_user_account_handler->
														 Set_Password_Epoch_Time( user_loc,
																				  reinterpret_cast<
																					  uint8_t*>( &
																								 usermgmt_p_data
																								 ->
																								 pwd_set_epoch_time ),
																				  ( static_cast<
																						uint8_t>
																					( data_len ) ) ) )
													{
														error_code = 200U;
													}
													else
													{
														error_code = 400U;
													}
												}

												if ( error_code == 200U )
												{
													rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
													if ( parser->method == HTTP_METHOD_POST )
													{
														ip_address_setting_t ip_address_setting;
														ip_address_setting.ip_address =
															parser->general_use_reg;
														ip_address_setting.port =
															parser->remote_port;
														/* resetting the last login date, time and pwd set date value to
														   default value for newly created user */
														uint32_t time = 0U;
														m_user_account_handler->
														m_user_account_owner_list[user_loc].
														last_login_epoch_time->Check_Out_Default(
															&time );
														if ( ( error_code == 200U ) &&
															 m_user_account_handler->
															 Set_Last_Login_Epoch_Time( user_loc,
																						reinterpret_cast
																						<uint8_t*>(
																							&time ),
																						sizeof( time ) ) )
														{
															error_code = 200U;
														}
														else
														{
															error_code = 400U;
														}
														if ( ( error_code == 200U ) &&
															 m_user_account_handler->
															 Set_User_Account_Valid_Bit_List( bitmap,
																							  user_loc ) )
														{
															error_code = 200U;
														}
														else
														{
															error_code = 400U;
														}
														if ( error_code == 200U )
														{
															parser->response_code = 201U;
															/*user created event log*/


															if ( ( m_user_account_handler->
																   Get_User_Name(
																	   rs->user_id - 1U,
																	   &log_param[0],
																	   User_Account::USER_NAME_LIMIT
																	   + 1U ) ) )
															{
																error_code = 200U;
															}
															else
															{
																error_code = 400U;
															}
															log_param[User_Account::USER_NAME_LIMIT
																	  + 1U] =
																ip_address_setting.ip_address &
																0xffU;
															log_param[User_Account::USER_NAME_LIMIT
																	  + 2U] =
																( ip_address_setting.ip_address &
																  0xff00U ) >> 8U;
															log_param[User_Account::USER_NAME_LIMIT
																	  + 3U] =
																( ip_address_setting.ip_address &
																  0xff0000U ) >> 16U;
															log_param[User_Account::USER_NAME_LIMIT
																	  + 4U] =
																( ip_address_setting.ip_address &
																  0xff000000U ) >> 24U;
															if ( parser->https == HTTPS_MEDIA )
															{
																log_param[User_Account::
																		  USER_NAME_LIMIT + 5U] =
																	DCI_LOG_PORT_NUMBER_ENUM::
																	PORT_HTTPS;
															}
															else
															{
																log_param[User_Account::
																		  USER_NAME_LIMIT + 5U] =
																	DCI_LOG_PORT_NUMBER_ENUM::
																	PORT_HTTP;
															}

															if ( m_user_log_param_cb != nullptr )
															{
																( *m_user_log_param_cb )( log_param );
															}
															if ( m_user_log_audit_capture_cb !=
																 nullptr )
															{
																( *m_user_log_audit_capture_cb )(
																	LOG_EVENT_ADMIN_USER );
															}

															if ( ( m_user_account_handler->
																   Get_User_Name(
																	   user_loc,
																	   &log_param[0],
																	   User_Account::USER_NAME_LIMIT
																	   + 1U ) ) )
															{
																error_code = 200U;
															}
															else
															{
																error_code = 400U;
															}

															data_len = BF_Lib::Get_String_Length(
																log_param,
																( User_Account::USER_NAME_LIMIT
																  + 1U ) );
															for ( uint8_t i = data_len;
																  i <
																  ( User_Account::USER_NAME_LIMIT +
																	1U );
																  i++ )
															{
																log_param[i] = 0U;
															}

															if ( m_user_log_param_cb != nullptr )
															{
																( *m_user_log_param_cb )( log_param );
															}

															if ( m_user_log_audit_capture_cb !=
																 nullptr )
															{
																( *m_user_log_audit_capture_cb )(
																	LOG_EVENT_USER_CREATED );
															}
														}

													}
													else
													{
														parser->response_code = 200U;
													}
												}
											}
										}
									}
								}
							}
						}
						else if ( usermgmt_p_data->counter == 0U )
						{
							error_code = 400U;
						}
						else
						{	/* misra supress */
						}
						break;

					case YXML_EEOF:
						error_code = E400_XML_EOF_ERROR;
						break;

					case YXML_EREF:
						error_code = E400_XML_INVALID_CHAR_ERROR;
						break;

					case YXML_ECLOSE:
						error_code = E400_XML_CLOSE_ERROR;
						break;

					case YXML_ESTACK:
						error_code = E400_XML_STACK_ERROR;
						break;

					case YXML_ESYN:
						error_code = E400_XML_SYNTAX_ERROR;
						break;

					case YXML_ATTREND:
					case YXML_ATTRVAL:
					case YXML_OK:
					case YXML_PISTART:
					case YXML_PICONTENT:
					case YXML_PIEND:
					default:
						break;
				}
			}
		}
		if ( ( error_code != 200U ) || ( MAX_TAG_STATE == ypx->put_state ) )
		{
			Http_Server_Free( rs->p.usermgmt.put_request );
			if ( error_code != 200U )
			{
				hams_respond_error( parser, error_code );
			}
		}
	}
	else
	{
		if ( MAX_TAG_STATE == ypx->put_state )
		{
			if ( ( 0U != hams_can_respond( data, length ) ) && ( parser->response_code != 0U ) )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
		else
		{
			hams_respond_error( parser, 400U );
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Userid_Info_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = 200U;
	uint16_t bitmap = 0U;
	uint16_t buffer_length = 0U;
	uint8_t pwd_complexity = 0U;
	uint8_t role_id = 0U;
	uint16_t pwd_aging = 0U;
	date_time_format_t date_format;

	date_format.date_time = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( length != 1U )
	{
		if ( length == 0U )
		{
			if ( 0U != hams_check_headers_ended( data, length ) )
			{	/* magic number for curr_user to be verified and update req_user_id */
				rs->p.usermgmt.req_user_id =
					( rs->p.usermgmt.req_user_id == 0xFE ) ? rs->user_id :
					rs->p.usermgmt.req_user_id;
				if ( ( ( USER_ROLE_ADMIN == rs->auth_level ) ||
					   ( rs->p.usermgmt.req_user_id == rs->user_id ) ) && ( rs->user_id != 0U ) )
				{
					if ( parser->content_length == 0U )
					{
						bitmap = m_user_account_handler->Get_User_Account_Valid_Bit_List();
						if ( bitmap != User_Account::UNKNOWN_ERROR )
						{
							if ( ( bitmap
								   & static_cast<uint16_t>
								   ( 1U << ( rs->p.usermgmt.req_user_id - 1U ) ) ) !=
								 0U )
							{
								parser->response_code = 200U;
								rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
								Http_Server_Begin_Dynamic( parser );
								Http_Server_Print_Dynamic( parser, XML_DECL USER_ACCOUNT_ID_PREFIX,
														   rs->p.usermgmt.req_user_id,
														   rs->p.usermgmt.req_user_id );
								if ( m_user_account_handler->Get_User_Fullname( rs->p.usermgmt.
																				req_user_id - 1U,
																				dest_data,
																				( static_cast<
																					  uint8_t>
																				  ( buffer_length ) ) ) )
								{
									error_code = 200U;
									Http_Server_Print_Dynamic( parser,
															   FULL_NAME_PREFIX "\bs" FULL_NAME_SUFFIX,
															   strlen(
																   ( char const* )dest_data ),
															   dest_data );
								}
								else
								{
									error_code = 400U;
								}

								buffer_length = 0U;
								if ( ( error_code == 200U ) &&
									 m_user_account_handler->Get_User_Name( rs->p.usermgmt.
																			req_user_id -
																			1U, dest_data,
																			( static_cast<
																				  uint8_t>
																			  ( buffer_length ) ) ) )
								{
									error_code = 200U;
									Http_Server_Print_Dynamic( parser,
															   USER_NAME_PREFIX "\bs" USER_NAME_SUFFIX,
															   strlen(
																   ( char const* )dest_data ),
															   dest_data );
								}
								else
								{
									error_code = 400U;
								}

								buffer_length = 0U;
								if ( ( error_code == 200U ) &&
									 m_user_account_handler->Get_Password_Complexity( rs->p.usermgmt.req_user_id - 1U,
																					  &pwd_complexity,
																					  ( static_cast<uint8_t>(
																							buffer_length ) ) ) )
								{
									error_code = 200U;

									// Reset password change and warning bit
									RESET_PWD_CHANGE( pwd_complexity );
									RESET_PWD_WARNING( pwd_complexity );

									Http_Server_Print_Dynamic( parser,
															   PWD_COMPLEXITY_PREFIX "\bu" PWD_COMPLEXITY_SUFFIX,
															   pwd_complexity );
								}
								else
								{
									error_code = 400U;
								}

								buffer_length = 0U;
								if ( ( error_code == 200U ) &&
									 m_user_account_handler->Get_Password_Aging( rs->p.usermgmt.
																				 req_user_id - 1U,
																				 reinterpret_cast<
																					 uint8_t*>( &
																								pwd_aging ),
																				 ( static_cast<
																					   uint8_t>
																				   ( buffer_length ) ) ) )
								{
									error_code = 200U;
									Http_Server_Print_Dynamic( parser,
															   PWD_TOUTDAYS_PREFIX "\bu" PWD_TOUTDAYS_SUFFIX,
															   pwd_aging );
								}
								else
								{
									error_code = 400U;
								}
								buffer_length = 0U;
								if ( error_code == 200U )
								{
									role_id = m_user_account_handler->Get_User_Role(
										rs->p.usermgmt.req_user_id - 1U );
									if ( role_id != User_Account::UNKNOWN_ERROR )
									{
										bool_t found = false;
										for ( uint8_t counter = 0U;
											  ( ( counter < NUM_OF_AUTH_LEVELS ) &&
												( found == false ) );
											  counter++ )
										{
											if ( role_id == auth_struct[counter].role_level )
											{
												Http_Server_Print_Dynamic( parser, ROLE_TAG,
																		   counter + 1,
																		   strlen( auth_struct[
																					   counter].
																				   role_name ),
																		   auth_struct[counter].role_name );
												found = true;
											}
										}
									}
									else
									{
										error_code = 400U;
									}
								}
								buffer_length = 0U;
								/* get use of epoch time to get */
								if ( ( error_code == 200U ) &&
									 m_user_account_handler->Get_Password_Set_Epoch_Time( rs->p.
																						  usermgmt.
																						  req_user_id
																						  - 1U,
																						  reinterpret_cast
																						  <uint8_t*>(
																							  &
																							  date_format
																							  .
																							  date_time ),
																						  (
																							  static_cast
																							  <
																								  uint8_t
																							  >(
																								  buffer_length ) ) ) )
								{
									Http_Server_Print_Dynamic( parser,
															   PWD_SET_EPOCH_TIME_PREFIX "\bu" PWD_SET_EPOCH_TIME_SUFFIX,
															   date_format.date_time );
								}
								else
								{
									error_code = 400U;
								}

								buffer_length = 0U;
								if ( ( error_code == 200U ) &&
									 m_user_account_handler->Get_Last_Login_Epoch_Time( rs->p.
																						usermgmt.
																						req_user_id
																						- 1U,
																						reinterpret_cast
																						<uint8_t*>(
																							&
																							date_format
																							.
																							date_time ),
																						(
																							static_cast
																							<
																								uint8_t
																							>(
																								buffer_length ) ) ) )
								{
									Http_Server_Print_Dynamic( parser,
															   LAST_LOGIN_TIME_PREFIX "\bu" LAST_LOGIN_TIME_SUFFIX,
															   date_format.date_time );

									Http_Server_Print_Dynamic( parser,
															   FAILED_ATTEMPT_COUNT_PREFIX "\bu"
															   FAILED_ATTEMPT_COUNT_SUFFIX,
															   m_user_session_handler->Get_Failed_Attempt_Count( rs->p.
																												 usermgmt
																												 .
																												 req_user_id
																												 - 1U ) );

									// Get Trigger Change password value and add in response
									uint8_t trigger_pwd_change = m_user_account_handler->Get_Pwd_Change_State(
										rs->p.usermgmt.req_user_id - 1U );

									Http_Server_Print_Dynamic( parser,
															   TRIGGER_CHANGE_PWD_PREFIX "\bu"
															   TRIGGER_CHANGE_PWD_SUFFIX,
															   trigger_pwd_change );

									Http_Server_Print_Dynamic( parser, USER_ACCOUNT_ID_SUFFIX );
								}
								else
								{
									error_code = 400U;
								}

							}
							else
							{
								error_code = 404U;
							}
						}
						else
						{
							error_code = 500U;
						}
					}
					else
					{
						error_code = 413U;	/* Request Entity is too large */
					}
					if ( error_code != 200U )
					{
						hams_respond_error( parser, error_code );
					}
				}
				else
				{
					hams_respond_error( parser, 401U );	/* Authorization required */
				}
			}
			else
			{
				if ( 0U != hams_can_respond( data, length ) )
				{
					if ( Http_Server_End_Dynamic( parser ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml",
											  15U );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<void*>( nullptr ), 0U );
					}
				}
				else
				{
					hams_respond_error( parser, 400U );
				}
			}
		}
		else
		{
			hams_respond_error( parser, 400U );
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Parse_Roles_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( length != 1U )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( ( HTTP_METHOD_GET == parser->method ) || ( HTTP_METHOD_HEAD == parser->method ) )
			{
				if ( 0U != rs->auth_level )
				{
					parser->response_code = 200U;
					parser->user_data_cb = static_cast<hams_data_cb>( Users_Roles_Cb );
				}
				else
				{
					hams_respond_error( parser, 401U );
				}
			}
			else if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				Validate_Options_Cors_Headers( parser, rs, Users_Roles_Cb );
			}
			else
			{
				hams_respond_error( parser, 405U );	/* Method not allowed */
			}
		}
		else
		{
			uint8_t user_role = 0U;
			if ( ERR_PARSING_OK ==
				 INT_ASCII_Conversion::str_to_uint8( ( data ), 1U, length - 1U, &user_role ) )
			{
				if ( ( user_role < 1U ) || ( user_role > NUM_OF_AUTH_LEVELS ) )
				{
					hams_respond_error( parser, 400U );
				}
				else
				{
					rs->p.usermgmt.req_user_role = user_role;
					parser->user_data_cb = static_cast<hams_data_cb>( User_Role_Details_Cb );
				}
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Users_Roles_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	if ( HTTP_METHOD_OPTIONS == parser->method )
	{
		hams_response_header( parser, HTTP_HDR_ALLOW, "GET, HEAD, OPTIONS",
							  STRLEN( "GET, HEAD, OPTIONS" ) );
		hams_response_header( parser, COUNT_HTTP_HDRS, reinterpret_cast<const void*>( NULL ), 0U );
	}
	else
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		parser->response_code = 200U;
		if ( 0U != hams_can_respond( data, length ) )
		{
			rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
			Http_Server_Begin_Dynamic( parser );
			Http_Server_Print_Dynamic( parser, XML_DECL ROLES_PREFIX );
			for ( uint8_t role_id = 0U; role_id < NUM_OF_AUTH_LEVELS; role_id++ )
			{
				Http_Server_Print_Dynamic( parser, ROLE_NAME_TAG,
										   strlen( auth_struct[role_id].role_name ),
										   auth_struct[role_id].role_name,
										   role_id + 1U );
			}
			Http_Server_Print_Dynamic( parser, ROLES_SUFFIX );
			if ( rs->flags & REQ_FLAG_NO_DYN_PARSER_SPACE )
			{
				if ( Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t User_Role_Details_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( length != 1U )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( ( HTTP_METHOD_GET == parser->method ) || ( HTTP_METHOD_HEAD == parser->method ) )
			{
				if ( 0U != rs->auth_level )
				{
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
					parser->response_code = 200U;
					Http_Server_Begin_Dynamic( parser );
					Http_Server_Print_Dynamic( parser, XML_DECL USER_ROLE_PREFIX,
											   rs->p.usermgmt.req_user_role,
											   rs->p.usermgmt.req_user_role );
					Http_Server_Print_Dynamic( parser, XML_NAME_START "\bs" XML_NAME_END,
											   strlen( auth_struct[rs->p.usermgmt.req_user_role -
																   1].role_name ),
											   auth_struct[rs->p.usermgmt.req_user_role -
														   1].role_name );
					Http_Server_Print_Dynamic( parser, "\n\t<RoleLevel>" "\bu" "</RoleLevel>",
											   auth_struct[rs->p.usermgmt.req_user_role -
														   1].role_level );
					Http_Server_Print_Dynamic( parser, "\n\t<Description>" "\bs" "</Description>",
											   strlen( auth_struct[rs->p.usermgmt.req_user_role -
																   1].role_desc ),
											   auth_struct[rs->p.usermgmt.req_user_role -
														   1].role_desc );
					Http_Server_Print_Dynamic( parser, "\n</UserRole>" );
				}
				else
				{
					hams_respond_error( parser, 401U );
				}
			}
			else if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				Validate_Options_Cors_Headers( parser, rs, nullptr );
			}
			else
			{
				hams_respond_error( parser, 405U );
			}
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					hams_response_header( parser, HTTP_HDR_ALLOW, "GET, HEAD, OPTIONS",
										  STRLEN( "GET, HEAD, OPTIONS" ) );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
				else if ( 0U != Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<void*>( nullptr ), 0U );
				}
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t User_Delete_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = 200U;
	uint16_t bitmap = 0U;
	uint8_t log_param[User_Account::USER_NAME_LIMIT + 6U] = {0U};

	if ( length != 1U )
	{
		if ( length == 0U )
		{
			if ( 0U != hams_check_headers_ended( data, length ) )
			{
				http_server_req_state* rs = Http_Server_Get_State( parser );
				if ( rs->auth_level == USER_ROLE_ADMIN )
				{
					if ( parser->content_length == 0U )
					{
						bitmap = m_user_account_handler->Get_User_Account_Valid_Bit_List();
						if ( bitmap != User_Account::UNKNOWN_ERROR )
						{
							if ( ( bitmap
								   & static_cast<uint16_t>
								   ( 1U << ( rs->p.usermgmt.req_user_id - 1U ) ) ) !=
								 0U )
							{
								if ( rs->p.usermgmt.req_user_id != rs->user_id )
								{
									error_code = Validate_Role_Change_or_Delete_Condition(
										rs->p.usermgmt.req_user_id - 1, bitmap );

									if ( error_code == 200U )
									{
										if ( m_user_account_handler->
											 Clear_User_Account_Valid_Bit_List( bitmap,
																				rs->p.usermgmt.
																				req_user_id -
																				1U ) == false )
										{
											error_code = 500U;
										}
										else
										{
											m_user_session_handler->Clear_User_Session(
												rs->p.usermgmt.req_user_id - 1U );
											ip_address_setting_t ip_address_setting;
											ip_address_setting.ip_address = parser->general_use_reg;
											ip_address_setting.port = parser->remote_port;
											parser->response_code = 200U;
											log_param[User_Account::USER_NAME_LIMIT +
													  1U] = ip_address_setting.ip_address & 0xffU;
											log_param[User_Account::USER_NAME_LIMIT + 2U] =
												( ip_address_setting.ip_address & 0xff00U ) >> 8U;
											log_param[User_Account::USER_NAME_LIMIT + 3U] =
												( ip_address_setting.ip_address & 0xff0000U ) >>
												16U;
											log_param[User_Account::USER_NAME_LIMIT + 4U] =
												( ip_address_setting.ip_address & 0xff000000U ) >>
												24U;
											if ( parser->https == HTTPS_MEDIA )
											{
												log_param[User_Account::USER_NAME_LIMIT + 5U] =
													DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTPS;
											}
											else
											{
												log_param[User_Account::USER_NAME_LIMIT +
														  5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTP;
											}
											/*
											    Getting admin user name
											 */

											if ( ( m_user_account_handler->Get_User_Name(
													   rs->user_id - 1U,
													   &log_param[0],
													   User_Account::USER_NAME_LIMIT + 1U ) ) )
											{
												error_code = 200U;
											}
											else
											{
												error_code = 400U;
											}


											if ( m_user_log_param_cb != nullptr )
											{
												( *m_user_log_param_cb )( log_param );
											}
											if ( m_user_log_audit_capture_cb !=
												 nullptr )
											{
												( *m_user_log_audit_capture_cb )(
													LOG_EVENT_ADMIN_USER );
											}

											/* user name of other role based user which is deleted by admin */
											if ( ( m_user_account_handler->Get_User_Name(
													   rs->p.usermgmt.req_user_id - 1U,
													   &log_param[0],
													   User_Account::USER_NAME_LIMIT + 1U ) ) )
											{
												error_code = 200U;
											}
											else
											{
												error_code = 400U;
											}
											uint8_t data_len = 0U;
											data_len = BF_Lib::Get_String_Length(
												log_param,
												( User_Account::USER_NAME_LIMIT + 1U ) );
											for ( uint8_t i = data_len;
												  i < ( User_Account::USER_NAME_LIMIT + 1U );
												  i++ )
											{
												log_param[i] = 0;
											}
											if ( m_user_log_param_cb != nullptr )
											{
												( *m_user_log_param_cb )( log_param );
											}
										}

									}
								}
								else
								{
									error_code = E422_USER_SELF_DELETE_ERROR;
								}
							}
							else
							{
								error_code = E422_DELETE_NON_EXIST_RESOURCE;
							}
						}
						else
						{
							error_code = 500U;
						}
					}
					else
					{
						error_code = 413U;	/* Request Entity is too large */
					}
				}
				else
				{
					error_code = 401U;
				}
				if ( error_code != 200U )
				{
					hams_respond_error( parser, error_code );
				}
			}
			else
			{
				if ( 0U != hams_can_respond( data, length ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<void*>( nullptr ), 0U );
					/* user delete log event to be logged */
					if ( m_user_log_audit_capture_cb != nullptr )
					{
						( *m_user_log_audit_capture_cb )( LOG_EVENT_USER_DELETED );
					}
				}
				else
				{
					hams_respond_error( parser, 400U );
				}
			}
		}
		else
		{
			hams_respond_error( parser, 400U );
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static bool_t Is_UserName_Valid( uint8_t* srcptr, uint8_t length )
{
	uint8_t i = 0U;
	bool_t return_val = true;

	if ( IS_VALID_FIRST_USRNAME_CHAR( srcptr[0] ) )	/* UserName should start with a..z and A..Z */
	{
		for ( i = 1U; ( i < length ) && ( true == return_val ); i++ )
		{
			if ( !( IS_VALID_USRNAME_CHAR( srcptr[i] ) ) )	/* Username shall contain only a-z, A-Z, 0-9 and
															   underscore(_) */
			{
				return_val = false;
			}

			if ( ( IS_PERIOD( srcptr[i] ) ) && ( IS_PERIOD( srcptr[i - 1U] ) ) )
			{
				return_val = false;
			}
		}
	}
	else
	{
		return_val = false;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Parse_Users_Help_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	if ( length != 1U )
	{
		if ( length == 0U )
		{
			if ( 0U != hams_check_headers_ended( data, length ) )
			{
				http_server_req_state* rs = Http_Server_Get_State( parser );
				if ( ( HTTP_METHOD_GET == parser->method ) ||
					 ( HTTP_METHOD_HEAD == parser->method ) )
				{
					if ( 0U != rs->auth_level )
					{
						parser->response_code = 200U;
						rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
						Http_Server_Begin_Dynamic( parser );
						Http_Server_Print_Dynamic( parser, XML_DECL XML_RS_USERS_HELP );
					}
					else
					{
						hams_respond_error( parser, 401U );
					}
				}
				else if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					Validate_Options_Cors_Headers( parser, rs, Users_Roles_Cb );
				}
				else
				{
					hams_respond_error( parser, 405U );	/* Method not allowed */
				}
			}
			else
			{
				if ( Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
		}
		else
		{
			hams_respond_error( parser, 400U );
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Init_Curr_User_Put_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = 200U;

	if ( length == 0U )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		if ( rs->user_id != 0U )
		{
			rs->p.usermgmt.req_user_id = rs->user_id;
			if ( parser->content_type == HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED )
			{
				if ( parser->content_length != 0U )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Curr_User_Put_Cb );
					rs->p.usermgmt.yxml = Http_Server_Alloc_Xml_Parser( parser, 32U );
					uint8_t num_blocks = ( ( sizeof( login_date_time_t ) )
										   + ( HTTP_POOL_BLOCK_SIZE - 1U ) ) / HTTP_POOL_BLOCK_SIZE;/*
																									   Allocate
																									      ourselves a
																									      block for xml
																									      parsing.
																									 */
					login_date_time_t* meta_data =
						reinterpret_cast<login_date_time_t*>( Http_Server_Alloc_Scratch( parser,
																						 num_blocks ) );
					if ( meta_data != NULL )
					{
						rs->p.usermgmt.put_request = ( void* )meta_data;
					}
					else
					{
						error_code = E503_SCRATCH_SPACE;
					}
				}
				else
				{
					error_code = 411U;
				}
			}
			else
			{
				error_code = 415U;
			}
		}
		else
		{
			error_code = 401U;
		}
	}
	else if ( length > 1U )
	{
		error_code = 400U;
	}
	if ( error_code != 200U )
	{
		hams_respond_error( parser, error_code );
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Curr_User_Put_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yxml_put_xml_state* ypx = reinterpret_cast<struct yxml_put_xml_state*>( rs->p.usermgmt
																				   .yxml );
	yxml_t* yx = rs->p.usermgmt.yxml;
	uint8_t counter;
	uint16_t error_code = 200U;

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		login_date_time_t* usermgmt_p_data = reinterpret_cast<login_date_time_t*>( rs->p.usermgmt
																				   .put_request );
		for ( counter = 0U; ( counter < length ) && ( error_code == 200U ); ++counter )
		{
			int32_t r = yxml_parse( yx, static_cast<int32_t>( data[counter] ) );
			switch ( r )
			{	/* Expecting the proper tag element. */
				case YXML_ELEMSTART:
					usermgmt_p_data->counter = 0U;
					if ( ypx->put_state < PUT_CURR_USER_COMPLETE_TAG_STATE )
					{
						if ( 0U ==
							 strcmp( yx->elem,
									 reinterpret_cast<const char_t*>( user_date_time_Tags[ypx->
																						  put_state] ) ) )
						{
							ypx->put_state++;
						}
						else
						{
							error_code = 400U;
						}
					}
					break;

				case YXML_CONTENT:
					if ( PUT_CURR_USER_TIME_TAG_STATE == ( ypx->put_state - 1 ) )
					{
						usermgmt_p_data->login_time[usermgmt_p_data->counter] = yx->data[0];
					}
					else
					{
						error_code = 400U;
					}
					if ( error_code == 200U )
					{
						if ( ++usermgmt_p_data->counter >
							 time_tag_content_len[ypx->put_state - 1] )
						{
							error_code = 400U;
						}
					}
					break;

				case YXML_ELEMEND:
					if ( *yx->elem == static_cast<char_t>( 0U ) )
					{
						if ( ( ( ( length - counter ) == 1U ) && ( data[counter] == 0x3EU ) ) ||
							 ( ( ( length - counter ) > 1U ) && ( data[counter] == 0x3EU ) &&
							   ( IS_SP_OR_HT( data[counter + 1] ) ||
								 IS_NEW_LINE( data[counter + 1] ) ) ) )
						{
							if ( PUT_CURR_USER_COMPLETE_TAG_STATE == ypx->put_state )
							{
								ypx->put_state = PUT_CURR_USER_MAX_TAG_STATE;
								uint16_t bitmap = 0U;
								uint8_t user_loc = 0U;
								bitmap = m_user_account_handler->Get_User_Account_Valid_Bit_List();
								if ( bitmap != User_Account::UNKNOWN_ERROR )
								{
									if ( ( bitmap && ( 1U << ( rs->p.usermgmt.req_user_id - 1U ) ) ) ==
										 0U )
									{
										error_code = 404U;
									}
									else
									{
										user_loc = rs->p.usermgmt.req_user_id - 1U;
									}
									if ( error_code == 200U )
									{
										if ( true ==
											 Validate_Date_Time_And_Convert_To_Numeric(
												 usermgmt_p_data->login_time ) )
										{
											uint16_t data_len = 0U;
											data_len = 0U;
											if ( ( error_code == 200U ) &&
												 ( m_user_account_handler->Set_Last_Login_Epoch_Time(
													   user_loc,
													   reinterpret_cast
													   <
														   uint8_t
														   *>( &usermgmt_p_data->login_time ),
													   (
														   static_cast<uint8_t>
														   ( data_len ) ) ) == true ) )
											{
												error_code = 200U;
											}
											else
											{
												error_code = 400U;
											}
											rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
											parser->response_code = 200U;
										}
										else
										{
											error_code = 400U;
										}
									}
								}
								else
								{
									error_code = 500U;
								}
							}
							else
							{
								error_code = 400U;
							}
						}
						else
						{
							error_code = 400U;
						}
					}
					else if ( usermgmt_p_data->counter == 0U )
					{
						error_code = 400U;
					}
					break;

				case YXML_EEOF:
					error_code = E400_XML_EOF_ERROR;
					break;

				case YXML_EREF:
					error_code = E400_XML_INVALID_CHAR_ERROR;
					break;

				case YXML_ECLOSE:
					error_code = E400_XML_CLOSE_ERROR;
					break;

				case YXML_ESTACK:
					error_code = E400_XML_STACK_ERROR;
					break;

				case YXML_ESYN:
					error_code = E400_XML_SYNTAX_ERROR;
					break;

				case YXML_ATTREND:
				case YXML_ATTRVAL:
				case YXML_OK:
				case YXML_PISTART:
				case YXML_PICONTENT:
				case YXML_PIEND:
				default:
					break;
			}
		}
		if ( ( error_code != 200U ) || ( PUT_CURR_USER_MAX_TAG_STATE == ypx->put_state ) )
		{
			Http_Server_Free( rs->p.usermgmt.put_request );
			if ( error_code != 200U )
			{
				hams_respond_error( parser, error_code );
			}
		}
	}
	else
	{
		if ( PUT_CURR_USER_MAX_TAG_STATE == ypx->put_state )
		{
			if ( ( 0U != hams_can_respond( data, length ) ) && ( parser->response_code != 0U ) )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
		else
		{
			hams_respond_error( parser, 400U );
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Validate_Date_Time_And_Convert_To_Numeric( char_t* str_time )
{
	uint32_t epoch_time = 0U;

	bool_t valid_status = false;

	if ( str_time != nullptr )
	{
#ifndef USE_ONBOARD_RTC

		valid_status = false;
		uint8_t login_time_size = BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>( str_time ),
															 ( PWD_SET_EPOCH_TIME_SIZE_LIMIT +
															   1U ) );
		if ( ERR_PARSING_OK ==
			 INT_ASCII_Conversion::str_to_uint32(
				 ( ( const uint8_t* )&str_time[0] ), 1U, login_time_size - 1, &epoch_time,
				 true ) )
		{
			valid_status = true;
		}
#else
		/* call epoch time API */
		epoch_time = Get_Epoch_Time();
#endif
		/* beginning of epoch time and End of Unix time */
		if ( ( epoch_time > 0U ) && ( epoch_time <= 0x7FFFFFFF ) )
		{
			valid_status = true;
			*( reinterpret_cast<uint32_t*>( str_time ) ) = epoch_time;
		}
		else
		{
			valid_status = false;
			epoch_time = 0xFFFFFFFF;
			*( reinterpret_cast<uint32_t*>( str_time ) ) = epoch_time;
		}

	}

	return ( valid_status );
}

/*
   Password Complexity Requirements-

   Password complexity level – 0 1. It should be at least 6 characters long 2. It should not match with user name or
      full name 3. It should not match with existing password

   Password complexity level – 1 1. It should be at least 8 characters long 2. It should not match with user name or
      full name 3. It should not match with existing password 4. It should contain at least 1 alphabetic and 1 numeric
      character

   Password complexity level – 2 1. It should be at least 12 characters long 2. It should not match with user name or
      full name 3. It should not match with existing password 4. It should contain at least 1 alphabetic and 1 numeric
      character 5. It should contain at least 1 special character 6. It should contain at least 1 upper case alphabetic
      character

   Password complexity level – 3 1. It should be at least 16 characters long 2. It should not match with user name or
      full name 3. It should not match with existing password 4. It should contain at least 2 alphabetic character and 1
      numeric character 5. It should contain at least 2 special character 6. It should contain at least 1 upper case
      alphabetic character 7. It should contain at least 1 lower case alphabetic character
 */
uint16_t pwd_complexity_check_true( pwd_complexity_pass* pwd_structure )
{
	uint16_t return_val = E422_PWD_COMPLEXITY_VIOLATION;
	uint8_t new_pwd_len = 0U;
	uint8_t cur_pwd_len = 0U;
	uint8_t user_name_len = 0U;
	uint8_t full_name_len = 0U;
	uint8_t counter = 0U;
	uint8_t no_numeric_present = 0U;
	uint8_t no_special_char_present = 0U;
	uint8_t no_alphabetic_upper_present = 0U;
	uint8_t no_alphabetic_lower_present = 0U;

	user_name_len = BF_Lib::Get_String_Length(
		reinterpret_cast<uint8_t*>( pwd_structure->user_name ),
		( USERNAME_BASE64_SIZE_LIMIT + 1U ) );
	full_name_len = BF_Lib::Get_String_Length(
		reinterpret_cast<uint8_t*>( pwd_structure->full_name ),
		( FULLNAME_BASE64_SIZE_LIMIT + 1U ) );
	new_pwd_len =
		BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>( pwd_structure->new_pwd ),
								   ( PWD_BASE64_SIZE_LIMIT + 1U ) );
	cur_pwd_len =
		BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>( pwd_structure->cur_pwd ),
								   ( PWD_BASE64_SIZE_LIMIT + 1U ) );
	if ( ( user_name_len != 0U ) && ( full_name_len != 0U ) && ( new_pwd_len != 0U ) )
	{
		for ( counter = 0U; counter < new_pwd_len; counter++ )
		{
			if ( IS_UPALPHA( pwd_structure->new_pwd[counter] ) )
			{
				no_alphabetic_upper_present += 1U;
			}
			else if ( IS_LOALPHA( pwd_structure->new_pwd[counter] ) )
			{
				no_alphabetic_lower_present += 1U;
			}
			else if ( IS_DIGIT( pwd_structure->new_pwd[counter] ) )
			{
				no_numeric_present += 1U;
			}
			else if ( IS_PWD_SPECIAL_CHAR( pwd_structure->new_pwd[counter] ) )
			{
				no_special_char_present += 1U;
			}
			else
			{}
		}
		if ( !( ( ( cur_pwd_len == new_pwd_len ) &&
				  ( 0U == strcmp( ( char const* )pwd_structure->new_pwd,
								  ( char const* )pwd_structure->cur_pwd ) ) ) ||
				( ( new_pwd_len == full_name_len ) &&
				  ( 0U == strcmp( ( char const* )pwd_structure->new_pwd,
								  ( char const* )pwd_structure->full_name ) ) ) ||
				( ( new_pwd_len == user_name_len ) &&
				  ( 0U == strcmp( ( char const* )pwd_structure->new_pwd,
								  ( char const* )pwd_structure->user_name ) ) ) ) )
		{
			if ( !( ( IS_SPACE( pwd_structure->new_pwd[0] ) ) ||
					( IS_SPACE( pwd_structure->new_pwd[new_pwd_len - 1U] ) ) ) )
			{
				// Reset password complexity bit
				RESET_PWD_CHANGE( pwd_structure->pwd_complexity_level );
				RESET_PWD_WARNING( pwd_structure->pwd_complexity_level );

				switch ( pwd_structure->pwd_complexity_level )
				{
					case 0:	/* Pwd Complexity level 0 */
						if ( new_pwd_len >= PWD_LEVEL_0_LENGTH )
						{
							return_val = 200U;
						}
						break;

					case 1:	/* Pwd Complexity level 1 */
						if ( ( new_pwd_len >= PWD_LEVEL_1_LENGTH ) &&
							 ( ( no_alphabetic_upper_present + no_alphabetic_lower_present ) >=
							   1U ) &&
							 ( no_numeric_present >= 1U ) )
						{
							return_val = 200U;
						}
						break;

					case 2:	/* Pwd Complexity level 2 */
						if ( ( new_pwd_len >= PWD_LEVEL_2_LENGTH ) &&
							 ( no_numeric_present >= 1U ) &&
							 ( no_alphabetic_upper_present >= 1U ) &&
							 ( no_special_char_present >= 1U ) )
						{
							return_val = 200U;
						}
						break;

					case 3:	/* Pwd Complexity level 3 */
						if ( ( new_pwd_len >= PWD_LEVEL_3_LENGTH ) &&
							 ( no_numeric_present >= 1U ) &&
							 ( no_alphabetic_upper_present >= 1U ) &&
							 ( no_alphabetic_lower_present >= 1U ) &&
							 ( no_special_char_present >= 2U ) )
						{
							return_val = 200U;
						}
						break;

					default:
						return_val = E422_PWD_COMPLEXITY_VIOLATION;	/* The complexity level is invalid and is not
																	   impelemnetd
																	 */
						break;
				}
			}
		}
		else
		{
			return_val = E422_PWD_MATCHES_WITH_EXISTING_NAME_OR_PASSWORD;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint16_t Validate_Role_Change_or_Delete_Condition( uint8_t req_user_index, uint16_t bitmap )
{
	uint16_t error_code = 200;
	uint8_t role = 0U;
	uint16_t len = 0U;
	uint8_t num_admin_user = 0U;

	if ( DCI_ERR_NO_ERROR ==
		 get_dcid_ram_data(
			 USER_MGMT_USER_LIST[req_user_index].role, &role,
			 &len, 0U ) )
	{
		if ( role == USER_ROLE_ADMIN )
		{
			for ( uint32_t i = 0U; ( i < MAX_NUM_OF_AUTH_USERS ) && ( error_code == 200U ); i++ )
			{
				if ( ( bitmap & static_cast<uint16_t>( 1U << i ) ) != 0U )
				{
					len = 0U;
					role = 0U;
					if ( ( role = m_user_account_handler->Get_User_Role( i ) ) !=
						 User_Account::UNKNOWN_ERROR )
					{
						if ( role == USER_ROLE_ADMIN )
						{
							num_admin_user++;
						}
					}
					else
					{
						error_code = 500U;
					}
				}
			}
		}
		else
		{
			num_admin_user = 2U;
		}
	}
	else
	{
		error_code = 500U;
	}
	if ( ( error_code == 200U ) && ( num_admin_user < 2U ) )
	{
		error_code = E422_ONE_ADMIN_MANDATE_ERROR;
	}
	return ( error_code );
}

/*
 *****************************************************************************************
 * See function deceleration for function definition.
 *****************************************************************************************
 */
static uint32_t Verify_Password( uint8_t user_loc, user_mgmt_data_t* usermgmt_p_data, uint8_t data_len )
{
	bool ret_status = false;
	uint8_t pwd_complexity = 0U;
	uint8_t def_pwd[PWD_BASE64_SIZE_LIMIT + 1U] = { '\0' };
	uint16_t error_code = 400U;

	// Get Default user name for this user
	ret_status = m_user_account_handler->Get_Default_User_Name( user_loc, dest_data, 0U );
	if ( true == ret_status )
	{
		// Check if user name is empty, this is to check if this user is default user or not
		// If this is default user, default password need to be read and check if the current received
		// password is same as default received
		// If this is not the default user, we can continue for password setting
		ret_status = BF_Lib::String_Compare( dest_data, reinterpret_cast<const uint8_t*>( "" ), 1U );
		if ( false == ret_status )
		{
			ret_status = m_user_account_handler->Get_User_Default_Password( def_pwd );
			if ( true == ret_status )
			{
				ret_status = BF_Lib::String_Compare( def_pwd, reinterpret_cast<uint8_t*>( usermgmt_p_data->password ),
													 static_cast<uint32_t>( data_len ) );
				if ( true == ret_status )
				{
					error_code = E422_PWD_COMPLEXITY_VIOLATION;
				}
				else
				{
					// Default password and current password are not same, continue for password setting
					error_code = 200U;
				}
			}
		}
		else
		{
			// This is not the default user, continue for setting password
			error_code = 200U;
		}
	}

	// Execute this for all the users and also for default admin if ret_status from above is false i.e. new password for
	// default admin doesn't match
	// with default password
	if ( 200U == error_code )
	{
		// Set new password as received from user
		ret_status = m_user_account_handler->Set_Password( user_loc,
														   reinterpret_cast<uint8_t*>( usermgmt_p_data->password ),
														   static_cast<uint8_t>( data_len ) );

		if ( true == ret_status )
		{
			// Below code changes are required as while setting new password, if MSB of password complexity
			// variable is set for this particular user, we should reset it.
			// We are using MSB of password complexity variable to trigger password reset for the user it is
			// set as per new California and Oregon law
			ret_status = m_user_account_handler->Get_Password_Complexity( user_loc,
																		  reinterpret_cast<uint8_t*>( &pwd_complexity ),
																		  static_cast<uint8_t>( data_len ) );
			if ( true == ret_status )
			{
				RESET_PWD_CHANGE( pwd_complexity );
				RESET_PWD_WARNING( pwd_complexity );

				// Set the password complexity value
				ret_status = m_user_account_handler->Set_Password_Complexity( user_loc,
																			  reinterpret_cast<uint8_t*>( &
																										  pwd_complexity ),
																			  static_cast<uint8_t>( data_len ) );
				if ( true == ret_status )
				{
					error_code = 200U;
				}
			}
		}
	}

	return ( error_code );
}

/*
 *****************************************************************************************
 * See function deceleration for function definition.
 *****************************************************************************************
 */
static uint32_t Options_User_Reset_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	if ( length != 1U )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			Validate_Options_Cors_Headers( parser, rs, nullptr );
		}
		else
		{
			// This length is for parameters after /reset
			// Read the data and come back to this callback for response of Options command
			if ( length >= 2U )
			{
				uint8_t recv_reset_val = 0U;
				if ( ERR_PARSING_OK ==
					 INT_ASCII_Conversion::str_to_uint8( ( data ), 1U, length - 1U, &recv_reset_val ) )
				{
					// Added range check for reset value
					if ( ( recv_reset_val < RESET_DEFAULT_USER_ACC ) ||
						 ( recv_reset_val > RESET_ALL_USERS_ACC ) )
					{
						hams_respond_error( parser, 400U );
					}
				}
				else
				{
					hams_respond_error( parser, 400U );
				}
			}
			else
			{
				if ( 0U != hams_can_respond( data, length ) )
				{
					hams_response_header( parser, HTTP_HDR_ALLOW, "POST, OPTIONS",
										  STRLEN( "POST, OPTIONS" ) );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
				else
				{
					hams_respond_error( parser, 400U );
				}
			}
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See function deceleration for function definition.
 *****************************************************************************************
 */
static uint32_t Post_User_Reset_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	if ( length != 1U )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( rs->user_id != 0U )
			{
				if ( parser->content_length != 0U )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( User_Reset_Cb );

					rs->p.usermgmt.yxml = Http_Server_Alloc_Xml_Parser( parser, 32U );
					uint8_t num_blocks = ( ( sizeof( user_reset_data_t ) ) + ( HTTP_POOL_BLOCK_SIZE - 1U ) ) /
						HTTP_POOL_BLOCK_SIZE;

					// Allocate ourselves a block for xml parsing.
					user_reset_data_t* meta_data =
						reinterpret_cast<user_reset_data_t*>( Http_Server_Alloc_Scratch( parser, num_blocks ) );
					if ( meta_data != nullptr )
					{
						rs->p.usermgmt.put_request = ( void* )meta_data;
					}
					else
					{
						hams_respond_error( parser, E503_SCRATCH_SPACE );
					}
				}
				else
				{
					hams_respond_error( parser, 411U );
				}
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
		else
		{
			uint8_t recv_reset_val = 0U;
			if ( ERR_PARSING_OK != INT_ASCII_Conversion::str_to_uint8( ( data ), 1U, length - 1U, &recv_reset_val ) )
			{
				hams_respond_error( parser, 400U );
			}
			else
			{
				rs->p.usermgmt.reserved = recv_reset_val;
			}
		}
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See function deceleration for function definition.
 *****************************************************************************************
 */
static uint32_t User_Reset_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint8_t counter = 0U;
	uint16_t error_code = 200U;
	uint8_t log_param[User_Account::USER_NAME_LIMIT + 6U] = {0U};
	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yxml_put_xml_state* ypx = reinterpret_cast<struct yxml_put_xml_state*>( rs->p.usermgmt.yxml );
	yxml_t* yx = rs->p.usermgmt.yxml;

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		user_reset_data_t* usermgmt_p_data = reinterpret_cast<user_reset_data_t*>( rs->p.usermgmt.put_request );

		if ( parser->content_type != HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED )
		{
			error_code = 415U;
		}
		else
		{
			for ( counter = 0U; ( counter < length ) && ( error_code == 200U ); ++counter )
			{
				int32_t r = yxml_parse( yx, static_cast<int32_t>( data[counter] ) );
				switch ( r )
				{
					// Check for Tag <UserAccount>
					case YXML_ELEMSTART:
					{
						usermgmt_p_data->counter = 0U;
						if ( usermgmt_p_data->user_tag_rx.field.user_account_rx != 1U )
						{
							if ( 0U ==
								 strcmp( yx->elem,
										 reinterpret_cast<const char_t*>( RESET_USERS_TAGS[USERACC_TAG_DEF] ) ) )
							{
								// Next tag state
								ypx->put_state = USERACC_TAG_DEF;
								usermgmt_p_data->user_tag_rx.field.user_account_rx = 1U;
							}
							else
							{
								error_code = 400U;
							}
						}
						else
						{
							error_code = 400U;
							for ( uint32_t i = 1U; ( ( i < MAX_RESET_USER_TAG ) && ( error_code == 400U ) ); i++ )
							{
								if ( 0U == strcmp( yx->elem, reinterpret_cast<const char_t*>( RESET_USERS_TAGS[i] ) ) )
								{
									if ( ( usermgmt_p_data->user_tag_rx.all & ( 1U << i ) ) == 0U )
									{
										error_code = 200U;
										ypx->put_state = i;
										usermgmt_p_data->user_tag_rx.all |= 1U << i;
									}
									else
									{
										i = MAX_RESET_USER_TAG;
									}
								}
							}
						}
						break;

							case YXML_CONTENT:
							{
								// Read logged in user password
								if ( LOGGEDIN_PWD_TAG_DEF == ypx->put_state )
								{
									usermgmt_p_data->cur_password[usermgmt_p_data->counter] = yx->data[0];
								}

								if ( ++usermgmt_p_data->counter > tag_content_len[ypx->put_state] )
								{
									error_code = 400U;
								}
								break;
							}
							break;

							case YXML_ELEMEND:
							{
								if ( *yx->elem == static_cast<char_t>( 0U ) )
								{
									if ( ( ( ( length - counter ) == 1U ) && ( data[counter] == 0x3EU ) ) ||
										 ( ( ( length - counter ) > 1U ) && ( data[counter] == 0x3EU ) &&
										   ( IS_SP_OR_HT( data[counter + 1U] ) ||
											 IS_NEW_LINE( data[counter + 1U] ) ) ) )
									{
										ypx->put_state = MAX_RESET_USER_TAG;
									}
									else
									{
										error_code = 400U;
									}
								}

								if ( MAX_RESET_USER_TAG == ypx->put_state )
								{
									uint16_t bitmap = 0U;
									uint16_t data_len = 0U;
									uint32_t decoded_length = 0U;

									bitmap = m_user_account_handler->Get_User_Account_Valid_Bit_List();
									if ( bitmap != User_Account::UNKNOWN_ERROR )
									{
										if ( ( bitmap && ( 1U << ( rs->user_id - 1U ) ) ) == 0U )
										{
											error_code = 404U;
										}
										else
										{
											if ( usermgmt_p_data->user_tag_rx.field.cur_pwd_rx == 1U )
											{
												if ( error_code == 200U )
												{
													// Check for current user role
													uint8_t role =
														m_user_account_handler->Get_User_Role( rs->user_id - 1U );
													if ( USER_ROLE_ADMIN == role )
													{
														data_len = 0U;
														uint8_t cur_pwd[CUR_PWD_BASE64_SIZE_LIMIT] = {'\0'};
														m_user_account_handler->Get_Password( ( rs->user_id - 1U ),
																							  cur_pwd,
																							  static_cast<uint8_t>(
																								  data_len ) );
														if ( BASE64_DECODING_ERR_OK !=
															 Base64::Decode( ( uint8_t* ) usermgmt_p_data->cur_password,
																			 BF_Lib::
																			 Get_String_Length( reinterpret_cast<uint8_t
																												 *>(
																									usermgmt_p_data->
																									cur_password ),
																								( PWD_BASE64_SIZE_LIMIT
																								  +
																								  1U ) ),
																			 dest_data,
																			 &decoded_length,
																			 0U ) )
														{
															error_code = E422_DECRYPTION_FAILED;
														}
														else
														{
															if ( ( decoded_length !=
																   strlen( reinterpret_cast<const char_t*>( cur_pwd ) ) )
																 ||
																 ( 0U != strncmp( reinterpret_cast<char_t*>( cur_pwd ),
																				  reinterpret_cast<char_t*>( dest_data ),
																				  BF_Lib::Get_String_Length(
																					  reinterpret_cast<uint8_t*>(
																						  cur_pwd ),
																					  (
																						  CUR_PWD_BASE64_SIZE_LIMIT ) ) ) ) )
															{
																error_code = 403U;
															}
														}
													}
													else
													{
														// If User role is not admin
														error_code = 401U;
													}
												}
											}
										}

										// Check for reset value received in URI
										if ( ( error_code == 200U ) &&
											 ( RESET_DEFAULT_USER_ACC == rs->p.usermgmt.reserved ) )
										{
											// Function call to reset default user password to default password
											error_code = Reset_Default_User( bitmap );
											if ( 200U == error_code )
											{
												// Set response code
												rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
												parser->response_code = error_code;
											}
										}
										// Check for reset value received in URI
										else if ( ( error_code == 200U ) &&
												  ( RESET_ALL_USERS_ACC == rs->p.usermgmt.reserved ) )
										{
											// Function call to reset default user password to default password
											error_code = Factory_Reset();
											if ( 200U == error_code )
											{
												// Set response code
												rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
												parser->response_code = error_code;
											}
										}

										// Perform Logging operation
										if ( error_code == 200U )
										{
											// Perform logging operation
											ip_address_setting_t ip_address_setting;
											ip_address_setting.ip_address = parser->general_use_reg;
											ip_address_setting.port = parser->remote_port;
											log_param[User_Account::USER_NAME_LIMIT +
													  1U] = ip_address_setting.ip_address &
												0xffU;
											log_param[User_Account::USER_NAME_LIMIT +
													  2U] = ( ip_address_setting.ip_address & 0xff00U ) >> 8U;
											log_param[User_Account::USER_NAME_LIMIT +
													  3U] = ( ip_address_setting.ip_address & 0xff0000U ) >> 16U;
											log_param[User_Account::USER_NAME_LIMIT +
													  4U] = ( ip_address_setting.ip_address & 0xff000000U ) >> 24U;
											if ( parser->https == HTTPS_MEDIA )
											{
												log_param[User_Account::USER_NAME_LIMIT +
														  5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTPS;
											}
											else
											{
												log_param[User_Account::USER_NAME_LIMIT +
														  5U] = DCI_LOG_PORT_NUMBER_ENUM::PORT_HTTP;
											}

											// Get the current logged in user id
											if ( ( m_user_account_handler->Get_User_Name( ( rs->user_id - 1U ),
																						  &log_param[0],
																						  User_Account::USER_NAME_LIMIT
																						  +
																						  1U ) ) )
											{
												error_code = 200U;
											}
											else
											{
												error_code = 400U;
											}

											if ( m_user_log_param_cb != nullptr )
											{
												( *m_user_log_param_cb )( log_param );
											}
											if ( m_user_log_audit_capture_cb != nullptr )
											{
												if ( RESET_DEFAULT_USER_ACC == rs->p.usermgmt.reserved )
												{
													( *m_user_log_audit_capture_cb )( LOG_EVENT_RESET_DEFAULT_ADMIN_USER );
												}
												else if ( RESET_ALL_USERS_ACC == rs->p.usermgmt.reserved )
												{
													( *m_user_log_audit_capture_cb )( LOG_EVENT_RESET_ALL_ACCOUNT_USERS );
												}
											}
										}
									}
									else
									{
										error_code = 500U;
									}
								}
								else if ( usermgmt_p_data->counter == 0U )
								{
									error_code = 400U;
								}
							}
							break;
					}

					case YXML_EEOF:
						error_code = E400_XML_EOF_ERROR;
						break;

					case YXML_EREF:
						error_code = E400_XML_INVALID_CHAR_ERROR;
						break;

					case YXML_ECLOSE:
						error_code = E400_XML_CLOSE_ERROR;
						break;

					case YXML_ESTACK:
						error_code = E400_XML_STACK_ERROR;
						break;

					case YXML_ESYN:
						error_code = E400_XML_SYNTAX_ERROR;
						break;

					case YXML_ATTREND:
					case YXML_ATTRVAL:
					case YXML_OK:
					case YXML_PISTART:
					case YXML_PICONTENT:
					case YXML_PIEND:
					default:
						break;
				}
			}
		}

		if ( ( error_code != 200U ) || ( MAX_RESET_USER_TAG == ypx->put_state ) )
		{
			Http_Server_Free( rs->p.usermgmt.put_request );
			if ( error_code != 200U )
			{
				hams_respond_error( parser, error_code );
			}
		}
	}
	else
	{
		if ( MAX_RESET_USER_TAG == ypx->put_state )
		{
			if ( ( 0U != hams_can_respond( data, length ) ) && ( parser->response_code != 0U ) )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
		else
		{
			hams_respond_error( parser, 400U );
		}
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See function deceleration for function definition.
 *****************************************************************************************
 */
uint16_t Reset_Default_User( uint16_t bitmap )
{
	// Initialize error code to "NOT Found" as we are going to search for default admin
	// If default admin is found, error code will be changed to 200 OK
	uint16_t error_code = 500U;
	uint8_t cur_user_id = 0U;
	uint8_t cur_user_name[User_Account::USER_NAME_LIMIT + 1U] = "\0";
	uint8_t def_admin_userid = 0U;
	bool found_def_user_id = false;

	// Identify Default Admin user id
	for ( cur_user_id = 0U; cur_user_id < MAX_NUM_OF_AUTH_USERS; cur_user_id++ )
	{
		if ( ( found_def_user_id == false ) &&
			 ( m_user_account_handler->Get_Default_User_Name( cur_user_id, dest_data, 0U ) ) )
		{
			// Check if user name exists for this user
			if ( false == BF_Lib::String_Compare( dest_data, reinterpret_cast<const uint8_t*>( "" ), 1U ) )
			{
				def_admin_userid = cur_user_id;
				found_def_user_id = true;
			}
		}

		// Added check, if default user id is not found, there is no need to reading the current user name
		if ( ( true == found_def_user_id ) &&
			 ( nullptr != m_user_account_handler->Get_User_Name( cur_user_id, cur_user_name, 0U ) ) )
		{

			if ( true ==
				 BF_Lib::String_Compare( cur_user_name, dest_data,
										 static_cast<uint32_t>( User_Account::USER_NAME_LIMIT ) ) )
			{
				if ( BF_Lib::Bit::Test( bitmap, cur_user_id ) )
				{
					error_code = 200U;
				}
				break;
			}
		}
	}

	if ( error_code == 200U )
	{
		uint8_t def_pwd[PWD_BASE64_SIZE_LIMIT] = {'\0'};

		// Get default admin password
		if ( true == m_user_account_handler->Get_User_Default_Password( def_pwd ) )
		{
			uint16_t pwd_len = BF_Lib::Get_String_Length( def_pwd, ( PWD_BASE64_SIZE_LIMIT + 1U ) );
			if ( false == m_user_account_handler->Set_Password( cur_user_id, def_pwd, pwd_len ) )
			{
				error_code = 400U;
			}
			else
			{
				uint8_t pwd_complexity = 0U;
				// Get the password complexity of default admin to append the trigger password change window bit
				bool ret_status =
					m_user_account_handler->Get_Password_Complexity( cur_user_id,
																	 reinterpret_cast<uint8_t*>( &pwd_complexity ),
																	 0U );

				// Check for password scheme
				uint8_t device_pwd_scheme = m_user_account_handler->Get_Device_Password_Scheme();

				// Check for the password scheme
				if ( ( device_pwd_scheme == DCI_DEVICE_PWD_SETTING_ENUM::DCI_COMMON_PWD_CHANGE ) ||
					 ( device_pwd_scheme == DCI_DEVICE_PWD_SETTING_ENUM::DCI_UNIQUE_PWD_CHANGE ) )
				{
					SET_PWD_CHANGE( pwd_complexity );
				}
				else
				{
					SET_PWD_WARNING( pwd_complexity );
				}

				( void ) m_user_account_handler->Set_Password_Complexity( cur_user_id, &pwd_complexity, 0U );

				// Clear all user session for default user
				m_user_session_handler->Clear_User_Session( cur_user_id );
			}
		}
	}
	else
	{
		error_code = Create_Default_User( bitmap, def_admin_userid );
	}

	return ( error_code );
}

/*
 *****************************************************************************************
 * See function deceleration for function definition.
 *****************************************************************************************
 */
uint16_t Create_Default_User( uint16_t bitmap, const uint8_t def_admin_userid )
{
	bool ret_status = false;
	uint8_t empty_found = false;
	uint8_t user_loc = 0U;
	uint8_t def_pwd[PWD_BASE64_SIZE_LIMIT] = {'\0'};
	uint16_t error_code = 500U;
	uint16_t string_len = 0U;

	for ( user_loc = 0U; ( ( user_loc < MAX_NUM_OF_AUTH_USERS ) && ( false == empty_found ) ); user_loc++ )
	{
		// Finding empty location to create a new admin new user
		if ( 0U == ( BF_Lib::Bit::Test( bitmap, user_loc ) ) )
		{
			empty_found = true;
			USER_SESS_PRINT( DBG_MSG_INFO, "user loc which is empty is %d", user_loc );
			break;
		}
	}

	// Create a default user if an empty location is found
	if ( true == empty_found )
	{
		// Get the default user fullname of default admin
		if ( true == m_user_account_handler->Get_Default_User_Fullname( def_admin_userid, dest_data, 0U ) )
		{
			string_len = BF_Lib::Get_String_Length( dest_data, ( FULL_NAME_LIMIT + 1U ) );
			ret_status = m_user_account_handler->Set_User_Fullname( user_loc, dest_data, string_len );
			if ( true == ret_status )
			{
				ret_status = m_user_account_handler->Get_Default_User_Name( def_admin_userid, dest_data, 0U );
				if ( true == ret_status )
				{
					string_len = BF_Lib::Get_String_Length( dest_data, ( USERNAME_BASE64_SIZE_LIMIT + 1U ) );
					ret_status = m_user_account_handler->Set_User_Name( user_loc, dest_data, string_len );
					if ( true == ret_status )
					{
						// Get default admin password from the Device Default password DCID
						// Note: Here we are NOT reading default password from any user
						ret_status = m_user_account_handler->Get_User_Default_Password( def_pwd );
						if ( true == ret_status )
						{
							string_len = BF_Lib::Get_String_Length( def_pwd, ( PWD_BASE64_SIZE_LIMIT + 1U ) );
							ret_status = m_user_account_handler->Set_Password( user_loc, def_pwd, string_len );
							if ( true == ret_status )
							{

								uint8_t pwd_complexity = 0U;
								ret_status = m_user_account_handler->Get_Password_Complexity( def_admin_userid,
																							  reinterpret_cast<uint8_t*>(
																								  &pwd_complexity ),
																							  0U );
								if ( true == ret_status )
								{
									uint8_t device_pwd_scheme = m_user_account_handler->Get_Device_Password_Scheme();

									// Check for the password scheme
									if ( ( device_pwd_scheme == DCI_DEVICE_PWD_SETTING_ENUM::DCI_COMMON_PWD_CHANGE ) ||
										 ( device_pwd_scheme == DCI_DEVICE_PWD_SETTING_ENUM::DCI_UNIQUE_PWD_CHANGE ) )
									{
										SET_PWD_CHANGE( pwd_complexity );
									}
									else
									{
										SET_PWD_WARNING( pwd_complexity );
									}

									ret_status = m_user_account_handler->Set_Password_Complexity( user_loc,
																								  &pwd_complexity, 0U );
									if ( true == ret_status )
									{
										uint16_t pwd_aging = 0U;
										ret_status = m_user_account_handler->Get_Default_Password_Aging(
											def_admin_userid,
											(
												reinterpret_cast<uint8_t*>( &pwd_aging ) ),
											0U );
										if ( true == ret_status )
										{
											ret_status = m_user_account_handler->Set_Password_Aging( user_loc,
																									 ( reinterpret_cast<
																										   uint8_t*>( &
																													  pwd_aging ) ),
																									 0U );
											if ( true == ret_status )
											{
												uint8_t user_role = m_user_account_handler->Get_Default_User_Role(
													def_admin_userid );
												ret_status = m_user_account_handler->Set_User_Role( user_loc,
																									( reinterpret_cast<
																										  uint8_t*>( &
																													 user_role ) ),
																									sizeof( user_role ) );
												if ( true == ret_status )
												{
													ret_status =
														m_user_account_handler->Set_User_Account_Valid_Bit_List( bitmap,
																												 user_loc );
													if ( true == ret_status )
													{
														// Set Error code to OK
														error_code = 200U;
													}
												}
												else
												{
													USER_SESS_PRINT( DBG_MSG_ERROR,
																	 " Unable to Set User Role for user %d", user_loc );
												}
											}
											else
											{
												USER_SESS_PRINT( DBG_MSG_ERROR,
																 " Unable to Set Password Aging for user %d",
																 user_loc );
											}
										}
										else
										{
											USER_SESS_PRINT( DBG_MSG_ERROR,
															 " Unable to Get Default Password Aging from user %d",
															 def_admin_userid );
										}
									}
									else
									{
										USER_SESS_PRINT( DBG_MSG_ERROR,
														 " Unable to Set Password Complexity for user %d", user_loc );
									}
								}
								else
								{
									USER_SESS_PRINT( DBG_MSG_ERROR,
													 " Unable to Get Default Password Complexity from user %d",
													 def_admin_userid );
								}
							}
							else
							{
								USER_SESS_PRINT( DBG_MSG_ERROR, " Unable to Set Password for user %d", user_loc );
							}
						}
						else
						{
							USER_SESS_PRINT( DBG_MSG_ERROR, " Unable to Get Default Password from user %d",
											 def_admin_userid );
						}
					}
					else
					{
						USER_SESS_PRINT( DBG_MSG_ERROR, " Unable to Set User name for user %d", user_loc );
					}
				}
				else
				{
					USER_SESS_PRINT( DBG_MSG_ERROR, " Unable to Get Default User name for user %d", def_admin_userid );
				}
			}
			else
			{
				USER_SESS_PRINT( DBG_MSG_ERROR, " Unable to Set Full User name for user %d", user_loc );
			}
		}
		else
		{
			USER_SESS_PRINT( DBG_MSG_ERROR, " Unable to Get Default Full User name from user %d", def_admin_userid );
		}
	}
	else
	{
		error_code = E422_USER_DATABASE_FULL;
		USER_SESS_PRINT( DBG_MSG_ERROR, " User database full, not creating default admin user" );
	}

	return ( error_code );
}

/*
 *****************************************************************************************
 * See function deceleration for function definition.
 *****************************************************************************************
 */
uint16_t Factory_Reset( void )
{
	uint16_t error_code = 200U;

	if ( m_user_account_handler->
		 Clear_All_User_Account_Valid_Bit_List() == false )
	{
		error_code = 500U;
	}
	else
	{
		for ( uint8_t user_id = 0U; ( user_id < MAX_NUM_OF_AUTH_USERS ); user_id++ )
		{
			m_user_session_handler->Clear_User_Session( user_id );
		}
	}
	error_code = Reset_Default_User( 0U );
	return ( error_code );
}
