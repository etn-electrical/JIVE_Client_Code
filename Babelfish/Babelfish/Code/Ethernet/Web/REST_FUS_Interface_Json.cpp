/**
 *****************************************************************************************
 * @file REST_FUS_Interface_Json.cpp
 * @details REST-FUS interface file help to get the execute REST operation.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "Base64_Rest.h"
#include "DCI_Defines.h"
#include "httpd_rest_dci.h"
#include "httpd_xml.h"
#include "OS_Tasker.h"
#include "Mem_Check.h"
#include "System_Reset.h"
#include "Proc_Mem_ID_Codes.h"
#include "Timecommon.h"
#include "Prod_Spec_PKI.h"
#include "uC_Fw_Code_Range.h"
#include "Log_Events.h"
#include "StdLib_MV.h"
#include "REST_DCI_Data.h"
#include "REST.h"
#include "Base64_Rest.h"
#include "httpd_common.h"
#include "FUS.h"
#include "FUS_Msg_Structs.h"
#include "server.h"
#include "FUS_Config.h"
#include "Rest_WebUI_Interface.h"
#include "Web_Debug.h"
#include "Babelfish_Assert.h"
#include "FUS_Debug.h"
#include "REST_FUS_Interface_Json.h"
#include "iot_json_parser.h"
#include "Yjson.h"

#define FIRMWARE_OPTIONS   "GET, HEAD, OPTIONS"

#define FW_MIN_ACCESS_LEVEL ( ( PROD_SPEC_FW_UPGRADE_MIN_ACCESS_LEVEL > \
								99U ) ? PROD_SPEC_FW_UPGRADE_MIN_ACCESS_LEVEL : 99U )
#define SESSION_OPTIONS     "POST, DELETE, OPTIONS"
#define PUT_BIN_OPTIONS     "PUT, OPTIONS"

// Used to allocate buffer for parsing FUS address:
const uint32_t FUS_ADDRESS_STRING_MAX_LEN = 16U;

static const uint8_t SESSION_TAG[] = "Session";
static const uint8_t SESSION_TAG_ATTR[] = "SessionID";
static const uint8_t SESSION_TAG_DEF[] = "SessionDef";
static const uint8_t TIMEOUT_TAG_START[] = "TimeoutSec";
static const uint8_t ENCODING_TYPE_TAG[] = "EncodingType";

static const uint8_t COMMIT_TAG_DEF[] = "Commit";
static const uint8_t SESSION_ID_TAG[] = "SessionID";

static const uint8_t ATTR_SESSION_ID = 1U;

static const uint8_t ATTR_ADDRESS_ID = 2U;
static const uint8_t ATTR_SIZE_ID = 3U;
#ifdef ESP32_SETUP
/*
 * Max total chunks required for esp32 is 1535. With ESP32 max app image size as 1536KB.
 */
static const uint16_t MAX_TOTAL_CHUNKS = 1535U;
#else
static const uint16_t MAX_TOTAL_CHUNKS = 1024U;
#endif
static const char_t* BIN_ATTR[ATTR_SIZE_ID] =
{ "SessionID", "address", "size" };

static const char_t FW_BIN_TAG_1[] = "DataPUT";
static const char_t FW_BIN_TAG_2[] = "ChunkTotal";
static const char_t FW_BIN_TAG_3[] = "ChunkNumber";
static const char_t FW_BIN_TAG_4[] = "Data";

static const uint8_t TAG_CHUNK_TOTAL_ID = 2U;
static const uint8_t TAG_CHUNK_NUM_ID = 3U;
static const uint8_t TAG_CHUNK_DATA_ID = 4U;

static const char_t* BIN_TAGS[TAG_CHUNK_DATA_ID] =
{
	FW_BIN_TAG_1,
	FW_BIN_TAG_2,
	FW_BIN_TAG_3,
	FW_BIN_TAG_4
};

/* Session Variables Enums, Constants, Macros */
enum
{
	INIT_STATE = 0U,
	SESSION_TAG_STATE,
	TIMEOUT_TAG_STATE,
	DATA_RX_PREPARE_STATE,
	ENCODE_TYPE,
	DATA_RX_SESSION_ID,
	DATA_END_STATE,
	SESSION_DELETE_REQUEST_COMPLETE,
	MAX_STATE
	/* Dont insert any enum after this */
};

enum
{
	NO_ENCODE = 0U,
	BASE_64_ENCODE,
	MAX_NUMBER_ENCODE
	/* Dont insert any enum after this */
};

/* PUT/bin variable */
enum data_compl_check_t_js
{
	DATA_COMPL_NOT_VERIFIED,
	DATA_COMPL_VERIFY_IN_PROGRESS,
	DATA_COMPL_VERIFIED
} data_compl_check_js;

enum put_bin_state_t_js
{
	BIN_INIT,
	BIN_CHUNK_INIT,
	BIN_INIT_COMPL,
	BIN_BEFORE_TAG_COMPL,
	BIN_DATA_STREAM_COMPL,
	BIN_AFTER_TAG_COMPL,
	BIN_COMPLETE,
	BIN_ERROR,
} put_bin_state_js;

typedef struct put_bin_metadata_s_js
{
	put_bin_state_t_js state;
	uint8_t tag_index;
	uint8_t attr_index;
	data_compl_check_t_js elem_end_chk;
	uint8_t pre_proc_id;
	uint16_t pre_image_id;
	char_t* pre_address;
	uint32_t recd_session_id;
	uint32_t pre_session_id;
	uint32_t pre_chunk_num;
	uint32_t chunk_num;
	uint32_t pre_chunk_total;
	uint32_t chunk_total;
	uint32_t recd_size;
	uint32_t put_bin_len;
	uint32_t total_put_bin_len;
	uint32_t next_req_wait_time;
	bool first_chunk;
} put_bin_metadata_js;

// define the encoded and decoded buffer for memory allocation
static uint8_t* m_encoded_buff = nullptr;
static uint8_t* m_decoded_buff = nullptr;

static User_Session::session_id_t fus_user_sess_id = User_Session::MAX_NUM_SESSIONS;
static BF_FUS::FUS* fus_handle = nullptr;
static uint8_t g_encode_type = NO_ENCODE;
static User_Session* m_fw_upgrade_user_session_handler = nullptr;

static Fw_Upgrade_Audit_Function_Decl_1 m_fw_upgrade_log_audit_capture_cb =
	reinterpret_cast<Fw_Upgrade_Audit_Function_Decl_1>( nullptr );
static Fw_Upgrade_Audit_Function_Decl_2 m_fw_upgrade_log_param_cb =
	reinterpret_cast<Fw_Upgrade_Audit_Function_Decl_2>( nullptr );
static Fw_Upgrade_User_Function_Decl_1 m_get_user_image_info_cb =
	reinterpret_cast<Fw_Upgrade_User_Function_Decl_1>( nullptr );
static Fw_Upgrade_User_Function_Decl_2 m_set_user_image_info_cb =
	reinterpret_cast<Fw_Upgrade_User_Function_Decl_2>( nullptr );

bool pre_pbin_in_progress_js = false;
static uint8_t g_fw_upgrade_user_id_js = 0xFFU;

/*
 *****************************************************************************************
 *		Functions
 *****************************************************************************************
 */
extern void Validate_Options_Cors_Headers( hams_parser* parser, http_server_req_state* rs,
										   hams_data_cb set_user_callback );

/**
 * @brief This function is used to get the details of the processor.
 * @details This function returns the details of the processor when /rs/fw/{x} is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Processor_Details_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Fw_Parse_Image_Id_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief This function is used to get the information for each Image.
 * @details This function returns the information for each Image when rs/fw/{x}/{y} is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Firmware_Details_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief This callback function is for POST session Request.
 * @details This function is for POST session when /rs/fw/session is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Session_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief This callback function is for POST session Request.
 * @details This function is for POST session when /rs/fw/session is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Session_Mgmt_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static void Block_User_Session_Expiration_Json( hams_parser* parser );

static void Unblock_User_Session_Expiration_Json( void );

/**
 * @brief This callback function is for Put Bin Request.
 * @details This function is for Put Bin Request when /rs/fw/x/y/bin is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Init_Fw_Bin_Requst_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief This callback function is for Put Bin Request.
 * @details This function is for Put Bin Request when /rs/fw/x/y/bin is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Parse_Fw_Bin_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief This function is used to save the chunk data.
 * @details This function is used to save the chunk data when /rs/fw/x/y/bin is requested..
 * @param[in] dest_buf_start_index : start index of destination buffer.
 * @param[in] bin_len : length of the data
 * @param[in] src_buff : source buffer
 * @param[in] dest_buff : destination buffer
 * @param[in] dest_buff_size : size of the destination buffer
 * @return bool Returns either true or false
 */
static bool Save_Chunk_Data_Json( uint32_t dest_buf_start_index, uint32_t bin_len,
								  uint8_t const* src_buff, uint8_t* dest_buff,
								  uint16_t dest_buff_size );

/**
 * @brief This callback function request a validation check from the end device.
 * @details This function request a validation check from the end device when /rs/fw/x/y/validate is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Validate_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief This callback function request used to copy binary into the end memory device
 * @details This function request used to copy binary into the end memory device when /rs/fw/{x}/{y}/commit is
 * requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Commit_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief This callback function request used to copy binary into the end memory device
 * @details This function request used to copy binary into the end memory device when /rs/fw/{x}/{y}/commit is
 * requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Commit_Mgmt_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief This function is used for getting the status of the commit operation. .
 * @details This function is used for getting the status of the commit operation. when /rs/fw/{x}/{y}/status is
 * requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Status_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

void FUS_Update_Audit_logs_Json( uint8_t image_id, uint32_t firm_ver );

/**
 * @brief This callback function is used to parse the command.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Parse_Command_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Firmware_Upgrade_Info_Json( Fw_Upgrade_User_Function_Decl_1 get_user_info_cb,
									  Fw_Upgrade_User_Function_Decl_2 set_user_info_cb )
{
	m_get_user_image_info_cb = get_user_info_cb;
	m_set_user_image_info_cb = set_user_info_cb;
}

static uint32_t Parse_Command_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;

	if ( length > 1U )
	{
		if ( ( length == 7U ) &&
			 ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/status", length ) == 0 ) )
		{
			// status_callback;
			parser->user_data_cb = static_cast<hams_data_cb>( &Status_Cb_Json );
		}
		else if ( ( length == 9U ) &&
				  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/validate", length ) ==
					0 ) )
		{
			// validate_callback;
			parser->user_data_cb = static_cast<hams_data_cb>( &Validate_Cb_Json );
		}
		else if ( ( length == 7U ) &&
				  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/commit", length ) ==
					0 ) )
		{
			// commit_callback
			parser->user_data_cb = static_cast<hams_data_cb>( &Commit_Cb_Json );
		}
		else if ( ( length == 4U ) &&
				  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/bin", length ) == 0 ) )
		{
			// put_bin_callback
			parser->user_data_cb = static_cast<hams_data_cb>( &Init_Fw_Bin_Requst_Cb_Json );
		}
		else
		{
			error_code = ERROR_BAD_REQUEST;
		}

		if ( error_code != RESPONSE_OK )
		{
			hams_respond_error( parser, error_code );
		}
	}
	else
	{
		if ( true == ENDS_WITH_SLASH( data ) )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Firmware_Details_Cb_Json );
		}
	}
	return ( 0 );
}

static uint32_t Init_Fw_Bin_Requst_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;

	if ( length == 0U )
	{
		if ( Firmware_Upgrade_Inprogress() == true )
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			rs->p.fwop.state = 0U;
			if ( ( pre_pbin_in_progress_js == false ) )
			{
				if ( parser->method == HTTP_METHOD_PUT )
				{
					if ( rs->user_id == g_fw_upgrade_user_id_js )
					{
						if ( parser->content_length != 0U )
						{
							if ( parser->content_type == HTTP_CONTENT_TYPE )
							{
								if ( true == Is_Fw_Upgrade_Enabled() )
								{
									/* Allocate ourselves a block for json parsing. */
									rs->p.rest.yjson = Http_Server_Alloc_Json_Parser( parser, 32U );

									if ( rs->p.rest.yjson != NULL )
									{
										BF_ASSERT( m_encoded_buff );
										rs->p.fwop.ptr = m_encoded_buff;

										if ( nullptr != rs->p.fwop.ptr )
										{
											pre_pbin_in_progress_js = true;
											rs->p.fwop.state = 0x55;
											struct yjson_put_json_state* ypx =
												reinterpret_cast<struct yjson_put_json_state*>( rs->p.rest.yjson );

											ypx->flags = 0U;

											parser->user_data_cb = Parse_Fw_Bin_Cb_Json;
										}
										else
										{
											error_code = E503_SCRATCH_SPACE;
										}
									}
									else
									{
										error_code = E503_JSON_PARSER;
									}
								}
								else
								{
									error_code = ERROR_FORBIDDEN;	// upgrade enable flag is diabled.
								}
							}
							else
							{
								error_code = ERROR_UNSUPPORTED_MEDIA_TYPE;
							}
						}
						else
						{
							error_code = ERROR_BAD_REQUEST;
						}
					}
					else
					{
						error_code = ERROR_UNAUTHORIZED;
					}
				}
				else if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					if ( 0U != parser->content_length )
					{
						hams_respond_error( parser, ERROR_UNAUTHORIZED );	/* Request Entity is too large */
					}
					else
					{

						if ( ( 0U == parser->cors_auth_common->origin_length ) ||
							 ( NULL == parser->cors_auth_common->origin_buffer ) )
						{
							hams_respond_error( parser, ERROR_BAD_REQUEST );
						}
						else
						{
							if ( 0U != hams_check_headers_ended( data, length ) )
							{
								parser->response_code = RESPONSE_OK;
								rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
							}
							else
							{
								if ( 0U != hams_can_respond( data, length ) )
								{
									hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
														  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
									hams_response_header( parser, HTTP_HDR_ALLOW,
														  PUT_BIN_OPTIONS,
														  STRLEN( PUT_BIN_OPTIONS ) );
									hams_response_header( parser, COUNT_HTTP_HDRS,
														  ( const void* )nullptr, 0U );
								}
							}
						}
					}
				}
				else
				{
					error_code = ERROR_METHOD_NOT_ALLOWED;
				}
			}
			else
			{
				error_code = 420U;
			}
		}
		else
		{
			error_code = E422_SESSION_NOT_EXIST;
		}
	}
	else if ( length > 1U )
	{
		error_code = ERROR_BAD_REQUEST;
	}
	if ( error_code != RESPONSE_OK )
	{
		hams_respond_error( parser, error_code );
	}
	return ( 0 );
}

static uint32_t Fw_Parse_Image_Id_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;

	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( length > 1U )
		{
			uint16_t val = 0U;
			if ( ERR_PARSING_OK ==
				 INT_ASCII_Conversion::str_to_uint16( ( data ), 1U, length - 1U, &val ) )
			{
				rs->p.fwop.image_id = val;
			}
			else
			{
				error_code = ERROR_BAD_REQUEST;
			}

			if ( data[length] == 0x2FU )	// '/'
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Parse_Command_Cb_Json );
			}
			else
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Firmware_Details_Cb_Json );
			}
			if ( error_code != RESPONSE_OK )
			{
				hams_respond_error( parser, error_code );
			}
		}
		else
		{
			if ( false == IS_EXTRA_SLASH( data ) )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Processor_Details_Cb_Json );
			}
		}
	}
	return ( 0 );
}

static uint32_t Parse_Fw_Bin_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	uint8_t fw_param[User_Account::USER_NAME_LIMIT + 3U] = {0};
	uint32_t decoded_len = 0U;
	BF_FUS::image_write_req_t write_image_in_mem = {0U};
	BF_FUS::image_write_resp_t write_optime = {0U};
	jsmn_parser j_parser;
	int16_t result = 0;
	uint16_t index = 0;
	jsmntok_t tokens[BUFFER_LEN];
	static char_t* p_msg_buffer = nullptr;
	static uint16_t msg_cnt = 1;
	static char_t json_value[JSON_VALUE_LEN];

#ifdef ESP32_SETUP
	static uint32_t image_start_address = 0U;
#endif
	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;

	static put_bin_metadata_js pbin =
	{
		BIN_INIT,
		0U,
		0U,
		DATA_COMPL_NOT_VERIFIED,
		0U,
		0U,
		nullptr,
		0U,
		0U,
		0U,
		0U,
		0U,
		0U,
		0U,
		0U,
		0U,
		0U,
		true
	};

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( ( pbin.state == BIN_INIT ) || ( pbin.pre_session_id != fus_handle->Get_Session_Id() ) )
		{
			uint8_t nv_msg = 0U;
			nv_msg = EXIT_TO_APP;
			if ( false == Set_App_Fw_Upgrade_State( nv_msg ) )
			{
				error_code = ERROR_INTERNAL_SERVER;
			}
			pbin.state = BIN_INIT_COMPL;
			pbin.tag_index = 0U;
			pbin.attr_index = 0U;
			pbin.elem_end_chk = DATA_COMPL_NOT_VERIFIED;
			pbin.pre_proc_id = rs->p.fwop.proc_id;
			pbin.pre_image_id = rs->p.fwop.image_id;
			pbin.recd_session_id = 0U;
			pbin.pre_session_id = fus_handle->Get_Session_Id();
			pbin.pre_chunk_num = 0U;
			pbin.chunk_num = 0U;
			pbin.pre_chunk_total = 0U;
			pbin.chunk_total = 0U;
			pbin.recd_size = 0U;
			if ( pbin.pre_address == nullptr )
			{
				pbin.pre_address = new char_t[FUS_ADDRESS_STRING_MAX_LEN];
			}
			*pbin.pre_address = '\0';
			pbin.put_bin_len = 0U;
			pbin.total_put_bin_len = 0U;
			pbin.next_req_wait_time = 0U;
			pbin.first_chunk = true;
#ifdef ESP32_SETUP
			// Read image start address required for flash write address calculation
			image_start_address = Get_Image_Start_Address(
				pbin.pre_proc_id,
				pbin.pre_image_id );
#endif
		}
		else if ( pbin.state == BIN_CHUNK_INIT )
		{
			pbin.state = BIN_INIT_COMPL;
			pbin.tag_index = 0U;
			pbin.attr_index = 0U;
			pbin.elem_end_chk = DATA_COMPL_NOT_VERIFIED;
			pbin.recd_session_id = 0U;
			pbin.chunk_num = 0U;
			pbin.chunk_total = 0U;
			pbin.recd_size = 0U;
			if ( pbin.pre_address == nullptr )
			{
				pbin.pre_address = new char_t[FUS_ADDRESS_STRING_MAX_LEN];
			}
			*pbin.pre_address = '\0';
			pbin.put_bin_len = 0U;
			pbin.next_req_wait_time = 0U;
		}

		if ( msg_cnt == 1 )
		{
			p_msg_buffer = reinterpret_cast<char_t*>( Ram::Allocate( JSON_MAXBUFF_LEN * 3 ) );

			if ( p_msg_buffer != nullptr )
			{
				memset( p_msg_buffer, 0, JSON_MAXBUFF_LEN * 3 );
				memcpy( p_msg_buffer, data, length );
			}
		}
		else
		{
			strncat( p_msg_buffer, ( char_t* )data, length );
			msg_cnt--;
		}

		uint16_t msgLen = strlen( p_msg_buffer );

		jsmn_init( &j_parser );
		jsmntok_t_init( tokens, BUFFER_LEN );

		result =
			jsmn_parse( &j_parser, ( char_t* )p_msg_buffer, msgLen, tokens, sizeof( tokens ) / sizeof( tokens[0] ) );

		if ( result < 0 )
		{
			if ( result == -3 )
			{
				/* Invalid Json string, More bytes expected */
				msg_cnt++;
				return ( 0U );
			}
			else
			{
				/* Failed to parse BIN parameter  */
				error_code = E400_JSON_SYNTAX_ERROR;
			}
		}
		for ( index = 1; index < result; index++ )
		{
			size_t token_size = tokens[index + 1].end - tokens[index + 1].start;
			if ( pbin.state != BIN_BEFORE_TAG_COMPL )
			{
				if ( pbin.state == BIN_DATA_STREAM_COMPL )
				{
					if ( pbin.elem_end_chk == DATA_COMPL_NOT_VERIFIED )
					{
						pbin.elem_end_chk = DATA_COMPL_VERIFY_IN_PROGRESS;
					}
					else if ( pbin.elem_end_chk == DATA_COMPL_VERIFY_IN_PROGRESS )
					{
						pbin.elem_end_chk = DATA_COMPL_VERIFIED;
					}
				}

				if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
							 reinterpret_cast<const char_t*>( BIN_TAGS[1] ) ) == 0 )
				{
					memset( json_value, 0, JSON_VALUE_LEN );
					strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

					pbin.tag_index = TAG_CHUNK_TOTAL_ID;

					if ( pbin.tag_index == TAG_CHUNK_TOTAL_ID )
					{
						uint16_t k = 0;
						while ( json_value[k] != NULL )
						{
							if ( IS_DIGIT( json_value[k] ) )
							{
								pbin.chunk_total *= 10U;
								pbin.chunk_total += ( json_value[k] - 0x30 );

								if ( pbin.chunk_total > MAX_TOTAL_CHUNKS )
								{
									error_code = E422_OUT_OF_ORDER_CHUNK;
								}
							}
							else
							{
								error_code = ERROR_BAD_REQUEST;
							}
							k++;
						}

						if ( pbin.pre_chunk_total == 0U )
						{
							if ( pbin.chunk_total != 0U )
							{
								pbin.pre_chunk_total = pbin.chunk_total;
							}
							else
							{
								error_code = E422_OUT_OF_ORDER_CHUNK;
							}
						}
						else
						{
							if ( pbin.pre_chunk_total != pbin.chunk_total )
							{
								error_code = E422_OUT_OF_ORDER_CHUNK;
							}
						}
					}
				}
				else if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
								  reinterpret_cast<const char_t*>( BIN_TAGS[2] ) ) == 0 )
				{
					memset( json_value, 0, JSON_VALUE_LEN );
					strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

					pbin.tag_index = TAG_CHUNK_NUM_ID;

					if ( pbin.tag_index == TAG_CHUNK_NUM_ID )
					{
						uint16_t k = 0;
						while ( json_value[k] != NULL )
						{
							if ( IS_DIGIT( json_value[k] ) )
							{
								pbin.chunk_num *= 10U;
								pbin.chunk_num += ( json_value[k] - 0x30 );

								if ( pbin.chunk_num >= pbin.chunk_total )
								{
									error_code = E422_OUT_OF_ORDER_CHUNK;
								}
							}
							else
							{
								error_code = ERROR_BAD_REQUEST;
							}
							k++;
						}

						if ( pbin.first_chunk == true )
						{
							if ( pbin.chunk_num != 0U )
							{
								error_code = E422_OUT_OF_ORDER_CHUNK;
							}
						}
						else
						{
							if ( pbin.chunk_num == ( pbin.pre_chunk_num + 1 ) )
							{
								pbin.pre_chunk_num = pbin.chunk_num;
							}
							else
							{
								if ( pbin.chunk_num == pbin.pre_chunk_num )
								{
									error_code = RESPONSE_OK;
								}
								else
								{
									error_code = E422_OUT_OF_ORDER_CHUNK;
								}
							}
						}
					}
				}
				else if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
								  reinterpret_cast<const char_t*>( BIN_ATTR[0] ) ) == 0 )
				{
					memset( json_value, 0, JSON_VALUE_LEN );
					strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

					pbin.attr_index = ATTR_SESSION_ID;

					if ( pbin.attr_index == ATTR_SESSION_ID )
					{
						uint16_t k = 0;
						while ( json_value[k] != NULL )
						{
							if ( IS_DIGIT( json_value[k] ) )
							{
								pbin.recd_session_id *= 10U;
								pbin.recd_session_id += ( json_value[k] - 0x30 );

								if ( ( pbin.recd_session_id > MAX_NUMBER_5_DIGIT ) ||
									 ( pbin.recd_session_id < 1 ) )
								{
									error_code = E422_SESSION_ID_MISMATCH;
								}
							}
							else
							{
								error_code = ERROR_BAD_REQUEST;
							}
							k++;
						}
					}
				}
				else if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
								  reinterpret_cast<const char_t*>( BIN_ATTR[1] ) ) == 0 )
				{
					memset( json_value, 0, JSON_VALUE_LEN );
					strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

					pbin.attr_index = ATTR_ADDRESS_ID;

					if ( pbin.attr_index == ATTR_ADDRESS_ID )
					{
						uint16_t k = 0;
						while ( json_value[k] != NULL )
						{
							if ( pbin.pre_address != nullptr )
							{
								uint8_t i = strlen( pbin.pre_address );
								pbin.pre_address[i++] = json_value[k++];
								pbin.pre_address[i] = '\0';
							}
							else
							{
								error_code = ERROR_BAD_REQUEST;
							}
						}
					}
				}
				else if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
								  reinterpret_cast<const char_t*>( BIN_ATTR[2] ) ) == 0 )
				{
					memset( json_value, 0, JSON_VALUE_LEN );
					strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

					pbin.attr_index = ATTR_SIZE_ID;

					if ( pbin.attr_index == ATTR_SIZE_ID )
					{
						uint16_t k = 0;
						while ( json_value[k] != NULL )
						{
							if ( IS_DIGIT( json_value[k] ) )
							{
								pbin.recd_size *= 10U;
								pbin.recd_size += ( json_value[k] - 0x30 );
								if ( pbin.recd_size > MAX_FW_UPGRADE_ENCODED_PACKET_SIZE )
								{
									error_code = E422_SIZE_MISMATCH;
								}
							}
							else
							{
								error_code = ERROR_BAD_REQUEST;
							}
							k++;
						}
						if ( pbin.recd_size == 0U )
						{
							error_code = ERROR_BAD_REQUEST;
						}
					}
				}
			}
			if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index], JSON_KEY_TEXT ) == 0 )
			{
				char_t* p_text_buff = nullptr;

				p_text_buff = reinterpret_cast<char_t*>( Ram::Allocate( JSON_MAXBUFF_LEN * 2 ) );

				if ( p_text_buff != nullptr )
				{
					memset( p_text_buff, 0, JSON_MAXBUFF_LEN * 2 );
					strncpy( p_text_buff, p_msg_buffer + tokens[index + 1].start, token_size );
				}

				uint16_t bin_len = strlen( p_text_buff );

				if ( ( pbin.put_bin_len + bin_len ) > pbin.recd_size )
				{
					bin_len = pbin.recd_size - pbin.put_bin_len;
				}

				if ( true ==
					 Save_Chunk_Data_Json( pbin.put_bin_len, bin_len, ( uint8_t* )p_text_buff,
										   ( uint8_t* )rs->p.fwop.ptr,
										   MAX_FW_UPGRADE_ENCODED_PACKET_SIZE ) )
				{
					Ram::De_Allocate( p_text_buff );
					pbin.put_bin_len += bin_len;

					if ( pbin.put_bin_len >= pbin.recd_size )
					{
						if ( error_code == RESPONSE_OK )
						{
							pbin.state = BIN_DATA_STREAM_COMPL;
							if ( g_encode_type == NO_ENCODE )
							{
								if ( pbin.put_bin_len & 0x01 )		// value should be an even number
								{
									error_code = E422_SIZE_MISMATCH;
								}
								else
								{
									decoded_len = pbin.put_bin_len >> 1U;	// divide by 2
									if ( decoded_len > ( MAX_FW_UPGRADE_DECODED_PACKET_SIZE ) )
									{
										error_code = E503_SCRATCH_SPACE;	// 503 scratch
									}
									else
									{
										BF_ASSERT( m_decoded_buff );
										if ( ERR_PARSING_OK !=
											 INT_ASCII_Conversion::str_hex_to_Bytes(
												 ( uint8_t* )rs->p.fwop.ptr, 0U,
												 pbin.put_bin_len - 1,
												 m_decoded_buff ) )
										{
											error_code = E422_DECRYPTION_FAILED;	// decryption
										}
									}
								}
							}
							else if ( g_encode_type == BASE_64_ENCODE )		// base 64
							{
								BF_ASSERT( m_decoded_buff );
								if ( BASE64_DECODING_ERR_OK !=
									 Base64::Decode( ( uint8_t* )rs->p.fwop.ptr,
													 pbin.put_bin_len,
													 m_decoded_buff, &decoded_len, 0U ) )
								{
									error_code = E422_DECRYPTION_FAILED;
								}
								else
								{
									if ( decoded_len >
										 ( MAX_FW_UPGRADE_DECODED_PACKET_SIZE ) )
									{
										error_code = E503_SCRATCH_SPACE;	// 503 scratch
									}
								}
							}
							else
							{
								error_code = E422_INVALID_ENCODE_TYPE;
							}
							if ( error_code == RESPONSE_OK )
							{
								if ( ( rs->p.fwop.proc_id <= fus_handle->Get_Component_Count() ) )	// ||
								{
									write_image_in_mem.proc_index = rs->p.fwop.proc_id;
									write_image_in_mem.image_index = rs->p.fwop.image_id;
									write_image_in_mem.session_id = pbin.recd_session_id;
									write_image_in_mem.data = m_decoded_buff;
									write_image_in_mem.data_len = decoded_len;

									write_image_in_mem.address = strtol( pbin.pre_address, NULL, HTTP_HDR_DATA_LEN );
									return_status = fus_handle->Write( &write_image_in_mem, &write_optime );
									if ( BF_FUS::SUCCESS == return_status )
									{
										pbin.next_req_wait_time = write_optime.op_time;
										if ( ( pbin.first_chunk == true ) && ( pbin.chunk_num == 0U ) )
										{
											// add firmware upgrade start log event
											// fw_param[0] to fw_param[20] is user name
											bool status =
												m_fw_upgrade_user_session_handler->m_user_account->Get_User_Name(
													rs->user_id - 1U, &fw_param[0], User_Account::USER_NAME_LIMIT );

											if ( status )
											{
												uint8_t data_len = BF_Lib::Get_String_Length(

													fw_param,
													( User_Account::USER_NAME_LIMIT + 1U ) );
												for ( uint8_t i = data_len;
													  i < ( User_Account::USER_NAME_LIMIT + 1U );
													  i++ )
												{
													fw_param[i] = 0U;
												}
												// fw_param[21] is processor id
												fw_param[User_Account::USER_NAME_LIMIT + 1U] = rs->p.fwop.proc_id;
												// fw_param[22] is image id
												fw_param[User_Account::USER_NAME_LIMIT + 2U] = rs->p.fwop.image_id;

											}
											if ( m_fw_upgrade_log_param_cb != nullptr )
											{
												( *m_fw_upgrade_log_param_cb )(
													LOG_EVENT_FW_UPDATE_STARTED, fw_param );
											}
											if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
											{
												( *m_fw_upgrade_log_audit_capture_cb )(
													LOG_EVENT_FW_UPDATE_STARTED );
											}
										}
									}
									else if ( BF_FUS::REQUEST_INVALID == return_status )
									{
										error_code = STATUS_NOT_FOUND;
									}
									else if ( BF_FUS::BUSY == return_status )
									{
										error_code = 420U;
									}
									else if ( BF_FUS::REJECT == return_status )
									{
										error_code = E422_SIZE_MISMATCH;
									}
									else
									{
										error_code = ERROR_BAD_REQUEST;
									}
								}
								else
								{
									error_code = STATUS_NOT_FOUND;
								}
							}
						}
					}
				}
				else
				{
					error_code = E503_SCRATCH_SPACE;
				}
			}
			if ( error_code == RESPONSE_OK )
			{
				if ( index == result - 1 )
				{
					pbin.state = BIN_COMPLETE;
				}
			}
		}

		if ( ( error_code != RESPONSE_OK ) || ( pbin.state == BIN_COMPLETE ) )
		{
			delete[] pbin.pre_address;
			pbin.pre_address = nullptr;
			msg_cnt = 1;
			Ram::De_Allocate( p_msg_buffer );
			if ( error_code != RESPONSE_OK )
			{
				if ( error_code == 420 )
				{
					pbin.state = BIN_CHUNK_INIT;
				}
				else
				{
					pbin.state = BIN_INIT;
				}
				hams_respond_error( parser, error_code );
			}
			else
			{
				parser->response_code = RESPONSE_OK;
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				Http_Server_Begin_Dynamic( parser );

				json_response_t* js_resp_buffer =
					reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

				if ( js_resp_buffer != nullptr )
				{
					js_resp_buffer->used_buff_len = 0U;
					memset( js_resp_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

					char_t bin_val[BUFFER_LEN];
					memset( bin_val, 0, BUFFER_LEN );

					Open_Json_Object( js_resp_buffer );
					Add_String_Member( js_resp_buffer, "DataPUT", NULL, false );
					Open_Json_Object( js_resp_buffer );
					Add_Integer_Member( js_resp_buffer, "SessionID", pbin.recd_session_id, true );
					Add_Integer_Member( js_resp_buffer, "Wait", pbin.next_req_wait_time, false );
					Multiple_Close_Object( js_resp_buffer, 2 );
					Http_Server_Print_Dynamic( parser, js_resp_buffer->json_buff );
					Ram::De_Allocate( js_resp_buffer );
				}
			}
		}
	}
	else
	{
		if ( pbin.state == BIN_COMPLETE )
		{
			delete[] pbin.pre_address;
			pbin.pre_address = nullptr;
			if ( pbin.chunk_total == ( pbin.chunk_num + 1 ) )
			{
				pbin.state = BIN_INIT;
			}
			else
			{
				pbin.state = BIN_CHUNK_INIT;
				pbin.first_chunk = false;
			}
			if ( ( 0U != hams_can_respond( data, length ) ) && ( parser->response_code != 0U ) )
			{
				if ( 0U != Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );

					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<void*>( nullptr ), 0U );
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
		}
		else
		{
			if ( pbin.state == BIN_BEFORE_TAG_COMPL )
			{
				hams_respond_error( parser, E422_SIZE_MISMATCH );
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
			pbin.state = BIN_INIT;
			delete[] pbin.pre_address;
			pbin.pre_address = nullptr;
			// Http_Server_Free( rs->p.fwop.ptr );
		}
	}
	return ( 0U );
}

static bool Save_Chunk_Data_Json( uint32_t dest_buf_start_index, uint32_t bin_len,
								  uint8_t const* src_buff,
								  uint8_t* dest_buff, uint16_t dest_buff_size )
{
	bool ret_val = true;

	if ( ( dest_buf_start_index + bin_len ) <= dest_buff_size )
	{
		for ( uint_fast32_t id = 0U; id < bin_len; id++ )
		{
			dest_buff[dest_buf_start_index] = *src_buff;
			src_buff++;
			dest_buf_start_index++;
		}
	}
	else
	{
		ret_val = false;
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Prod_Info_Spec_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );
	uint32_t boot_spec_version = 0U;
	uint16_t boot_spec_ver_major = 0U;
	uint16_t boot_spec_ver_minor = 0U;
	uint16_t boot_spec_rev = 0U;
	uint32_t boot_version = 0U;
	uint16_t boot_ver_major = 0U;
	uint16_t boot_ver_minor = 0U;
	uint16_t boot_ver = 0U;
	char_t formatted_buff[BUFFER_LEN];

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	if ( Firmware_Upgrade_Inprogress() == true )
	{
		hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
	}
	else
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				Validate_Options_Cors_Headers( parser, rs, nullptr );
			}
			else if ( 0U == rs->max_auth_exempt_level )
			{
				hams_respond_error( parser, ERROR_UNAUTHORIZED );
			}
			/* There should not be any request body.. */
			else if ( 0U != parser->content_length )
			{
				hams_respond_error( parser, ERROR_UNAUTHORIZED );
			}
			else if ( ( HTTP_METHOD_GET == parser->method ) ||
					  ( HTTP_METHOD_HEAD == parser->method ) )
			{
				BF_FUS::prod_info_t prod_info = { };
				uint8_t prod_name[PROD_NAME_MAX_LENGTH] = "";
				prod_info.name = prod_name;

				if ( BF_FUS::SUCCESS == fus_handle->Product_Info( &prod_info ) )
				{
					parser->response_code = RESPONSE_OK;
					http_server_req_state* rs = Http_Server_Get_State( parser );
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
					Http_Server_Begin_Dynamic( parser );

					json_response_t* json_buffer =
						reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

					if ( json_buffer != nullptr )
					{
						json_buffer->used_buff_len = 0U;
						memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

						Open_Json_Object( json_buffer );
						Add_String_Member( json_buffer, "Product", NULL, false );
						Open_Json_Object( json_buffer );
						Add_String_Member( json_buffer, HREF, "/rs/fw", true );
						Add_String_Member( json_buffer, "Name", ( char* )( prod_info.name ), true );

						memset( formatted_buff, 0U, BUFFER_LEN );
						Add_Integer_Member( json_buffer, "ProductCode",  prod_info.code, true );
						Add_Integer_Member( json_buffer, "ProductGuid", prod_info.guid, true );

						boot_spec_version = prod_info.boot_version;
						boot_spec_ver_major = ( boot_spec_version >> 24 );
						boot_spec_ver_minor = ( ( boot_spec_version >> 16 ) & 0xFF );
						boot_spec_rev = ( boot_spec_version & 0xFFFF );

						boot_version = prod_info.boot_spec_version;
						boot_ver_major = ( boot_version >> 24 );
						boot_ver_minor = ( ( boot_version >> 16 ) & 0xFF );
						boot_ver = ( boot_version & 0xFFFF );

						snprintf( formatted_buff, BUFFER_LEN, "%d.%d.%d", boot_spec_ver_major, boot_spec_ver_minor,
								  boot_spec_rev );
						Add_String_Member( json_buffer, "BootSpecVer", formatted_buff, true );

						snprintf( formatted_buff, BUFFER_LEN, "%d.%d.%d", boot_ver_major, boot_ver_minor, boot_ver );
						Add_String_Member( json_buffer, "BootVer", formatted_buff, true );
						Add_Integer_Member( json_buffer, "SerialNum", prod_info.serial_num, true );
						Add_Integer_Member( json_buffer, "ProcessorCount", prod_info.component_count, true );
						Add_String_Member( json_buffer, "ProcessorList", NULL, false );
						Open_Json_Object( json_buffer );
						Add_String_Member( json_buffer, "Processor", NULL, false );
						Open_Json_Array( json_buffer );
						for ( uint8_t proc_id = 0; proc_id < prod_info.component_count; proc_id++ )
						{
							Open_Json_Object( json_buffer );
							snprintf( formatted_buff, BUFFER_LEN, "/rs/fw/%d", proc_id );
							Add_String_Member( json_buffer, HREF, formatted_buff, false );
							if ( proc_id != ( prod_info.component_count - 1U ) )
							{
								Close_Json_Object( json_buffer, true );
							}
							else
							{
								Close_Json_Object( json_buffer, false );
							}
						}
						Close_Json_Array( json_buffer, false );
						Multiple_Close_Object( json_buffer, 3 );
						Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
						Ram::De_Allocate( json_buffer );
					}
				}
				else
				{
					hams_respond_error( parser, ERROR_INTERNAL_SERVER );
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
			}
		}
		else
		{
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				hams_response_header( parser, HTTP_HDR_ALLOW, FIRMWARE_OPTIONS,
									  STRLEN( FIRMWARE_OPTIONS ) );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else if ( 0U != Http_Server_End_Dynamic( parser ) )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ),
									  0U );
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
uint32_t Parse_Fw_Resources_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;

	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( length > 1U )
		{
			if ( ( length == 8U ) &&
				 ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/session", length ) ==
				   0 ) )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Session_Cb_Json );
			}
			else
			{
				uint16_t val = 0U;
				if ( ERR_PARSING_OK ==
					 INT_ASCII_Conversion::str_to_uint16( ( data ), 1U, length - 1U, &val ) )
				{
					if ( val < fus_handle->Get_Component_Count() )
					{
						rs->p.fwop.proc_id = val;
					}
					else
					{
						error_code = STATUS_NOT_FOUND;
					}

				}
				else
				{
					error_code = ERROR_BAD_REQUEST;
				}

				if ( data[length] == 0x2FU )	// '/'
				{
					parser->user_data_cb = static_cast<hams_data_cb>( &Fw_Parse_Image_Id_Cb_Json );
				}
				else
				{
					parser->user_data_cb = static_cast<hams_data_cb>( &Processor_Details_Cb_Json );
				}
			}
			if ( error_code != RESPONSE_OK )
			{
				hams_respond_error( parser, error_code );
			}
		}
		else
		{
			if ( false == IS_EXTRA_SLASH( data ) )
			{
				parser->user_data_cb = Prod_Info_Spec_Cb_Json;
			}
		}
	}
	return ( 0 );
}

static uint32_t Processor_Details_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	if ( Firmware_Upgrade_Inprogress() == true )
	{
		hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
	}
	else
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );

		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				Validate_Options_Cors_Headers( parser, rs, nullptr );
			}
			else if ( 0U == rs->max_auth_exempt_level )
			{
				hams_respond_error( parser, ERROR_UNAUTHORIZED );
			}
			else if ( 0U != parser->content_length )
			{
				hams_respond_error( parser, ERROR_UNAUTHORIZED );
			}
			else if ( ( HTTP_METHOD_GET == parser->method ) ||
					  ( HTTP_METHOD_HEAD == parser->method ) )
			{
				uint16_t spec_ver_major = 0U;
				uint16_t spec_ver_minor = 0U;
				uint16_t spec_build_ver = 0U;
				uint32_t proc_version = 0U;
				parser->response_code = RESPONSE_OK;

				http_server_req_state* rs = Http_Server_Get_State( parser );
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				Http_Server_Begin_Dynamic( parser );

				BF_FUS::comp_info_req_t comp_req;
				comp_req.comp_id = rs->p.fwop.proc_id;
				BF_FUS::comp_info_resp_t comp_resp = {0U};
				uint8_t proc_name[COMP_NAME_MAX_LENGTH] = "";
				comp_resp.name = proc_name;

				if ( BF_FUS::SUCCESS == fus_handle->Component( &comp_req, &comp_resp ) )
				{
					json_response_t* json_buffer =
						reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

					if ( json_buffer != nullptr )
					{
						json_buffer->used_buff_len = 0U;
						memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

						char_t formatted_buff[BUFFER_LEN];
						memset( formatted_buff, 0U, BUFFER_LEN );

						Open_Json_Object( json_buffer );
						Add_String_Member( json_buffer, "Processor", NULL, false );
						Open_Json_Object( json_buffer );
						snprintf( formatted_buff, BUFFER_LEN, "/rs/fw/%d", rs->p.fwop.proc_id );
						Add_String_Member( json_buffer, HREF, formatted_buff, true );

						Add_String_Member( json_buffer, "Name", ( char* )comp_resp.name, true );
						proc_version = comp_resp.firmware_version;
						spec_ver_major = ( proc_version & 0x00FF );
						spec_ver_minor = ( ( proc_version >> 8 ) & 0xFF );
						spec_build_ver = ( ( proc_version >> 16 ) & 0xFFFFU );

						snprintf( formatted_buff, BUFFER_LEN, "%d.%d.%d", spec_ver_major, spec_ver_minor,
								  spec_build_ver );
						Add_String_Member( json_buffer, "Ver", formatted_buff, true );
						Add_Integer_Member( json_buffer, "Guid", comp_resp.guid, true );
						Add_Integer_Member( json_buffer, "SerialNum", comp_resp.serial_num, true );

						proc_version = comp_resp.hardware_version;
						spec_ver_major = ( proc_version & 0xFF );
						spec_ver_minor = ( ( proc_version >> 8 ) & 0xFF );
						spec_build_ver = ( ( proc_version >> 16 ) & 0xFFFFU );

						snprintf( formatted_buff, BUFFER_LEN, "%d.%d.%d", spec_ver_major, spec_ver_minor,
								  spec_build_ver );
						Add_String_Member( json_buffer, "HardwareVer", formatted_buff, true );
						Add_Integer_Member( json_buffer, "ImageCount", comp_resp.image_count, true );
						Add_String_Member( json_buffer, "ImageList", NULL, false );
						Open_Json_Object( json_buffer );
						Add_String_Member( json_buffer, "Image", NULL, false );
						Open_Json_Array( json_buffer );
						for ( uint8_t image_id = 0U; image_id < comp_resp.image_count; image_id++ )
						{
							Open_Json_Object( json_buffer );
							snprintf( formatted_buff, BUFFER_LEN, "/rs/fw/%d/%d", rs->p.fwop.proc_id, image_id );
							Add_String_Member( json_buffer, HREF, formatted_buff, false );
							if ( image_id != ( comp_resp.image_count - 1U ) )
							{
								Close_Json_Object( json_buffer, true );
							}
							else
							{
								Close_Json_Object( json_buffer, false );
							}
						}
						Close_Json_Array( json_buffer, false );
						Multiple_Close_Object( json_buffer, 3 );
						Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
						Ram::De_Allocate( json_buffer );
					}
				}
				else
				{
					hams_respond_error( parser, ERROR_INTERNAL_SERVER );
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
			}
		}
		else
		{
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				hams_response_header( parser, HTTP_HDR_ALLOW, FIRMWARE_OPTIONS,
									  STRLEN( FIRMWARE_OPTIONS ) );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );

			}
			else if ( 0U != Http_Server_End_Dynamic( parser ) )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ),
									  0U );
			}
		}
	}
	return ( 0U );
}

static uint32_t Firmware_Details_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	char_t formatted_buff[BUFFER_LEN];

	http_server_req_state* rs = Http_Server_Get_State( parser );

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();

	if ( Firmware_Upgrade_Inprogress() == true )
	{
		hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
	}
	else
	{
		/*
		 * In case data is nullptr there is no need to check for slash. If we go for check then system crashes due to
		 * nullptr
		 */
		if ( ( nullptr == data ) || ( ( false == ENDS_WITH_SLASH( data ) ) &&
									  ( false == IS_EXTRA_SLASH( data ) ) ) )
		{
			if ( 0U == hams_has_uri_part( data, length ) )
			{
				if ( hams_check_headers_ended( data, length ) )
				{
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						Validate_Options_Cors_Headers( parser, rs, nullptr );
					}
					else if ( 0U == rs->max_auth_exempt_level )
					{
						hams_respond_error( parser, ERROR_UNAUTHORIZED );
					}
					/* There should not be any request body.. */
					else if ( 0U != parser->content_length )
					{
						hams_respond_error( parser, ERROR_UNAUTHORIZED );
					}
					else if ( ( HTTP_METHOD_GET == parser->method ) ||
							  ( HTTP_METHOD_HEAD == parser->method ) )
					{
						parser->response_code = RESPONSE_OK;
						http_server_req_state* rs = Http_Server_Get_State( parser );
						rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
						Http_Server_Begin_Dynamic( parser );
						BF_FUS::image_info_req_t image_info;
						image_info.proc_id = rs->p.fwop.proc_id;
						image_info.image_id = rs->p.fwop.image_id;
						uint32_t firm_version = 0U;
						uint16_t firm_ver_major = 0U;
						uint16_t firm_ver_minor = 0U;
						uint16_t firm_build_ver = 0U;
						BF_FUS::image_info_resp_t image_resp = { 0 };
						uint8_t img_name[IMAGE_NAME_MAX_LENGTH] = " ";
						image_resp.name = img_name;
						if ( BF_FUS::SUCCESS == fus_handle->FUS::Image( &image_info, &image_resp ) )
						{
							json_response_t* json_buffer =
								reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

							if ( json_buffer != nullptr )
							{
								json_buffer->used_buff_len = 0U;
								memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

								memset( formatted_buff, 0U, BUFFER_LEN );

								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, "Image", NULL, false );
								Open_Json_Object( json_buffer );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/fw/%d/%d", image_info.proc_id,
										  image_info.image_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, true );
								Add_String_Member( json_buffer, "Name", ( char* )image_resp.name, true );
								Add_Integer_Member( json_buffer, "Guid", image_resp.guid, true );

								firm_version = image_resp.version;
								firm_ver_major = ( firm_version & 0x00FF );
								firm_ver_minor = ( ( firm_version >> 8 ) & 0xFF );
								firm_build_ver = ( ( firm_version >> 16 ) & 0xFFFF );

								snprintf( formatted_buff, BUFFER_LEN, "%d.%d.%d", firm_ver_major, firm_ver_minor,
										  firm_build_ver );
								Add_String_Member( json_buffer, "Ver", formatted_buff, true );
								Add_Integer_Member( json_buffer, "Compatibility", image_resp.compatibility_num, true );

								uint8_t last_upgrade_user[DCID_STRING_SIZE] = "";
								image_upgrade_info_t image_upgrade_info = { 0 };
								image_upgrade_info.last_user = last_upgrade_user;
								if ( m_get_user_image_info_cb != nullptr )
								{
									m_get_user_image_info_cb( image_info.image_id, &image_upgrade_info );
								}
								Add_String_Member( json_buffer, "UpdateHistory", NULL, false );
								Open_Json_Object( json_buffer );

								Add_String_Member( json_buffer, "Who", ( char* )image_upgrade_info.last_user, true );
								Add_Integer_Member( json_buffer, "When", image_upgrade_info.last_time, false );
								Close_Json_Object( json_buffer, true );

								Add_String_Member( json_buffer, "Bin", NULL, false );
								Open_Json_Object( json_buffer );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/fw/%d/%d/bin", image_info.proc_id,
										  image_info.image_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, false );
								Close_Json_Object( json_buffer, true );

								Add_String_Member( json_buffer, "Valid", NULL, false );
								Open_Json_Object( json_buffer );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/fw/%d/%d/validate", image_info.proc_id,
										  image_info.image_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, false );
								Close_Json_Object( json_buffer, true );

								Add_String_Member( json_buffer, "Commit", NULL, false );
								Open_Json_Object( json_buffer );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/fw/%d/%d/commit", image_info.proc_id,
										  image_info.image_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, false );
								Close_Json_Object( json_buffer, true );

								Add_String_Member( json_buffer, "Status", NULL, false );
								Open_Json_Object( json_buffer );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/fw/%d/%d/status", image_info.proc_id,
										  image_info.image_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, false );
								Multiple_Close_Object( json_buffer, 3 );

								Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								Ram::De_Allocate( json_buffer );
							}
						}
						else
						{
							hams_respond_error( parser, ERROR_INTERNAL_SERVER );
						}
					}
					else
					{
						hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
					}
				}
				else
				{
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						hams_response_header( parser, HTTP_HDR_ALLOW, FIRMWARE_OPTIONS,
											  STRLEN( FIRMWARE_OPTIONS ) );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );

					}
					else if ( 0U != Http_Server_End_Dynamic( parser ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
											  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
			}
		}
	}
	return ( 0U );
}

static uint32_t Session_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	rs->p.fwop.session_timeout = 0U;
	rs->p.fwop.session_id = 0U;

	if ( HTTP_METHOD_POST == parser->method )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( rs->auth_level < FW_MIN_ACCESS_LEVEL )
			{
				hams_respond_error( parser, ERROR_UNAUTHORIZED );
			}
			else
			{
				/* There should be request body.. */
				if ( 0U == parser->content_length )
				{
					hams_respond_error( parser, ERROR_BAD_REQUEST );
				}
				else
				{
					/* create a fresh session */
					if ( Firmware_Upgrade_Inprogress() == false )
					{
						g_fw_upgrade_user_id_js = rs->user_id;
						parser->user_data_cb = static_cast<hams_data_cb>( Session_Mgmt_Cb_Json );
						rs->p.rest.yjson = Http_Server_Alloc_Json_Parser( parser, 32U );
					}
					else
					{
						/* Session already created. Request to create another rejected*/
						hams_respond_error( parser, 430U );
					}
				}
			}
		}
	}
	else if ( HTTP_METHOD_DELETE == parser->method )
	{

		if ( 0U == parser->content_length )
		{
			hams_respond_error( parser, ERROR_BAD_REQUEST );
		}
		else
		{
			if ( Firmware_Upgrade_Inprogress() == true )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( Session_Mgmt_Cb_Json );
				rs->p.rest.yjson = Http_Server_Alloc_Json_Parser( parser, 32U );
			}
			else
			{
				hams_respond_error( parser, E422_DELETE_NON_EXIST_RESOURCE );
				/* Session is not created already. Still Delete command received. So Bad request */
			}
		}
	}
	else if ( HTTP_METHOD_OPTIONS == parser->method )
	{
		if ( 0U != parser->content_length )
		{
			hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );	/* Request Entity is too large */
		}
		else
		{
			if ( ( 0U == parser->cors_auth_common->origin_length ) ||
				 ( NULL == parser->cors_auth_common->origin_buffer ) )
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
			else
			{
				if ( 0U != hams_check_headers_ended( data, length ) )
				{
					parser->response_code = RESPONSE_OK;
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				}
				else
				{
					if ( 0U != hams_can_respond( data, length ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
											  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
						hams_response_header( parser, HTTP_HDR_ALLOW, SESSION_OPTIONS,
											  STRLEN( SESSION_OPTIONS ) );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  ( const void* )nullptr, 0U );
					}
				}
			}
		}
	}
	else
	{
		hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
	}

	return ( 0U );
}

static uint32_t Session_Mgmt_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yjson_put_json_state* ypx =
		reinterpret_cast<struct yjson_put_json_state*>( rs->p.rest.yjson );

	BF_FUS::session_info_resp_t session_resp = { 0U };
	BF_FUS::session_info_req_t session_req = {0U};
	BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;

	jsmn_parser j_parser;
	int16_t result = 0;
	static int16_t cp_result = 0;
	uint16_t index = 0;
	jsmntok_t tokens[BUFFER_LEN];
	static char_t* p_msg_buffer = nullptr;
	static uint16_t s_msg_cnt = 1;
	static char_t json_value[JSON_VALUE_LEN];
	json_response_t* rs_json_buffer = nullptr;
	char_t formatted_buff[BUFFER_LEN];

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( parser->content_type != HTTP_CONTENT_TYPE )
		{
			hams_respond_error( parser, ERROR_UNSUPPORTED_MEDIA_TYPE );
		}
		else
		{
			if ( s_msg_cnt == 1 )
			{
				p_msg_buffer = reinterpret_cast<char_t*>( Ram::Allocate( JSON_MAXBUFF_LEN * sizeof( char_t ) ) );
				if ( p_msg_buffer != nullptr )
				{
					memset( p_msg_buffer, 0, JSON_MAXBUFF_LEN );
					memcpy( p_msg_buffer, data, length );
				}
			}
			else
			{
				strncat( p_msg_buffer, ( char_t* )data, length );
				s_msg_cnt--;
			}

			uint16_t msgLen = strlen( p_msg_buffer );

			jsmn_init( &j_parser );
			jsmntok_t_init( tokens, BUFFER_LEN );

			result =
				jsmn_parse( &j_parser, ( char_t* ) p_msg_buffer, msgLen, tokens,
							sizeof( tokens ) / sizeof( tokens[0] ) );

			if ( result < 0 )
			{
				if ( result == -3 )
				{
					/* Invalid Json string, More bytes expected */
					cp_result = result;
					s_msg_cnt++;
					return ( 0U );
				}
				else
				{
					/* Failed to parse Session parameter  */

					if ( HTTP_METHOD_POST == parser->method )
					{
						Exit_firmware_Session_Json();
					}
					else if ( HTTP_METHOD_DELETE == parser->method )
					{
						/* Session should not get deleted in case of JSON error*/
					}
					error_code = E400_JSON_SYNTAX_ERROR;
				}
			}

			for ( index = 1; ( ( index < result ) && ( error_code == 200 ) ); index++ )
			{
				size_t token_size = tokens[index + 1].end - tokens[index + 1].start;

				if ( HTTP_METHOD_POST == parser->method )
				{
					if ( INIT_STATE == ypx->put_state )
					{
						if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
									 ( reinterpret_cast<const char_t*>( SESSION_TAG_DEF ) ) ) == 0 )
						{
							ypx->put_state = SESSION_TAG_STATE;
						}
					}
					else
					{
						if ( SESSION_TAG_STATE == ypx->put_state )
						{
							if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
										 ( reinterpret_cast<const char_t*>( TIMEOUT_TAG_START ) ) ) == 0 )
							{
								memset( json_value, 0, JSON_VALUE_LEN );
								strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

								ypx->put_state = TIMEOUT_TAG_STATE;
								session_req.timeout_sec = 0U;

								if ( TIMEOUT_TAG_STATE == ypx->put_state )
								{
									uint16_t k = 0;
									while ( json_value[k] != NULL )
									{
										if ( IS_DIGIT( json_value[k] ) )
										{
											rs->p.fwop.session_timeout *= 10U;
											rs->p.fwop.session_timeout += ( json_value[k] - 0x30 );
											if ( rs->p.fwop.session_timeout > MAX_SESSION_TIMEOUT_SEC )
											{
												error_code = E422_INVALID_RESOURCE_TIMEOUT;
											}
										}
										else
										{
											error_code = ERROR_BAD_REQUEST;
										}
										k++;
									}

								}

								ypx->put_state = ENCODE_TYPE;
								g_encode_type = 0U;
							}
						}
						else
						{
							if ( ENCODE_TYPE == ypx->put_state )
							{
								if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
											 ( reinterpret_cast<const char_t*>( ENCODING_TYPE_TAG ) ) ) == 0 )
								{
									memset( json_value, 0, JSON_VALUE_LEN );
									strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

									uint16_t k = 0;
									while ( json_value[k] != NULL )
									{
										if ( IS_DIGIT( json_value[k] ) )
										{
											g_encode_type *= 10U;
											g_encode_type += ( json_value[k] - 0x30 );
											if ( g_encode_type > MAX_NUMBER_ENCODE )
											{
												error_code = E422_INVALID_ENCODE_TYPE;
											}
										}
										else
										{
											error_code = ERROR_BAD_REQUEST;
										}
										k++;
									}

									if ( g_encode_type < MAX_NUMBER_ENCODE )
									{
										ypx->put_state = DATA_END_STATE;	/* Here we have received full query */
									}
									else
									{
										error_code = E422_INVALID_ENCODE_TYPE;
									}
								}
							}
							else if ( DATA_END_STATE == ypx->put_state )
							{
								if ( index == result - 1 )
								{
									session_req.timeout_sec = rs->p.fwop.session_timeout;
									if ( session_req.timeout_sec == 0U )
									{
										error_code = E422_INVALID_RESOURCE_TIMEOUT;
									}
									else
									{
										if ( g_encode_type < MAX_NUMBER_ENCODE )
										{
											session_req.interface = fus_if_t::REST_FUS;
											return_status = fus_handle->Goto_FUM( &session_req, &session_resp );
											if ( BF_FUS::SUCCESS == return_status )
											{
												Block_User_Session_Expiration_Json( parser );
												parser->response_code = 201U;
												rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
												Http_Server_Begin_Dynamic( parser );

												rs_json_buffer =
													reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof(
																										   json_response_t ) ) );

												if ( rs_json_buffer != nullptr )
												{
													rs_json_buffer->used_buff_len = 0U;
													memset( rs_json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

													memset( formatted_buff, 0U, BUFFER_LEN );

													Open_Json_Object( rs_json_buffer );
													Add_String_Member( rs_json_buffer, "SessionResp", NULL, false );
													Open_Json_Object( rs_json_buffer );

													Add_Integer_Member( rs_json_buffer, "SessionID",
																		fus_handle->Get_Session_Id(), true );
													Add_Integer_Member( rs_json_buffer, "Wait", session_resp.wait_time,
																		false );
													Multiple_Close_Object( rs_json_buffer, 2 );
													Http_Server_Print_Dynamic( parser, rs_json_buffer->json_buff );
													Ram::De_Allocate( rs_json_buffer );
												}

											}
											else if ( BF_FUS::REJECT == return_status )
											{
												error_code = ERROR_FORBIDDEN;
											}
											else if ( BF_FUS::REQUEST_INVALID == return_status )
											{
												error_code = E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS;
											}
											else
											{
												error_code = ERROR_BAD_REQUEST;
											}
										}
										else
										{
											error_code = E422_INVALID_ENCODE_TYPE;
										}
									}
								}
								else
								{
									error_code = ERROR_BAD_REQUEST;
								}
							}
						}
					}
				}
				else if ( HTTP_METHOD_DELETE == parser->method )
				{
					if ( INIT_STATE == ypx->put_state )
					{
						if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
									 ( reinterpret_cast<const char_t*>( SESSION_TAG ) ) ) == 0 )
						{
							ypx->put_state = DATA_RX_PREPARE_STATE;
						}
					}

					if ( DATA_RX_PREPARE_STATE == ypx->put_state )
					{
						if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
									 ( reinterpret_cast<const char_t*>( SESSION_TAG_ATTR ) ) ) == 0 )
						{
							memset( json_value, 0, JSON_VALUE_LEN );
							strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

							ypx->put_state = DATA_RX_SESSION_ID;
							rs->p.fwop.session_id = 0U;

							if ( DATA_RX_SESSION_ID == ypx->put_state )
							{
								uint16_t k = 0;

								while ( json_value[k] != NULL )
								{
									if ( IS_DIGIT( json_value[k] ) )
									{
										rs->p.fwop.session_id *= 10U;
										rs->p.fwop.session_id += ( json_value[k] - 0x30 );
										if ( ( rs->p.fwop.session_id > MAX_NUMBER_5_DIGIT ) ||
											 ( rs->p.fwop.session_id < 1 ) )
										{
											error_code = E422_SESSION_ID_MISMATCH;
										}
									}
									else
									{
										error_code = ERROR_BAD_REQUEST;
									}
									k++;
								}
							}

							if ( DATA_RX_SESSION_ID == ypx->put_state )
							{
								if ( fus_handle->Is_Session_Alive() != true )
								{
									error_code = E422_SESSION_NOT_EXIST;
								}
								else if ( rs->p.fwop.session_id != Firmware_Session_Id_Json() )
								{
									error_code = E422_SESSION_ID_MISMATCH;
								}
								else
								{
									ypx->put_state = SESSION_DELETE_REQUEST_COMPLETE;
								}
							}
							if ( g_fw_upgrade_user_id_js != rs->user_id )
							{
								hams_respond_error( parser, ERROR_UNAUTHORIZED );
							}
							else
							{
								if ( SESSION_DELETE_REQUEST_COMPLETE == ypx->put_state )
								{
									session_req.session_id = rs->p.fwop.session_id;
									return_status = fus_handle->Exit_FUM( &session_req, &session_resp );
									if ( BF_FUS::SUCCESS == return_status )
									{
										Unblock_User_Session_Expiration_Json();
										parser->response_code = RESPONSE_OK;
										rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
										Http_Server_Begin_Dynamic( parser );

										rs_json_buffer =
											reinterpret_cast<json_response_t*>( Ram::Allocate(
																					sizeof( json_response_t ) ) );

										if ( rs_json_buffer != nullptr )
										{
											rs_json_buffer->used_buff_len = 0U;
											memset( rs_json_buffer, 0U, JSON_VALUE_LEN );

											char_t formatted_buff[BUFFER_LEN];
											memset( formatted_buff, 0U, BUFFER_LEN );

											Open_Json_Object( rs_json_buffer );
											Add_String_Member( rs_json_buffer, "Session", NULL, false );
											Open_Json_Object( rs_json_buffer );
											Add_Integer_Member( rs_json_buffer, "Wait", session_resp.wait_time, false );
											Multiple_Close_Object( rs_json_buffer, 2 );
											Http_Server_Print_Dynamic( parser, rs_json_buffer->json_buff );
											Ram::De_Allocate( rs_json_buffer );
										}
									}
									else if ( BF_FUS::REQUEST_INVALID == return_status )
									{
										error_code = E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS;
									}
									else
									{
										error_code = ERROR_BAD_REQUEST;
									}
								}
								else
								{
									error_code = 0x4044U;
								}
							}
						}
					}
				}
				else
				{
					error_code = ERROR_METHOD_NOT_ALLOWED;
				}
			}
			if ( RESPONSE_OK != error_code )
			{
				s_msg_cnt = 1;
				Ram::De_Allocate( p_msg_buffer );
				hams_respond_error( parser, error_code );
			}
		}
	}
	else
	{
		if ( ( 0U != hams_can_respond( data, length ) ) && ( parser->response_code != 0U ) )
		{
			if ( rs->flags & REQ_FLAG_NO_DYN_PARSER_SPACE )
			{
				if ( Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
										  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
		}
		else
		{
			if ( cp_result == -3 )
			{
				cp_result = 0;
				s_msg_cnt = 1;
				Ram::De_Allocate( p_msg_buffer );
				hams_respond_error( parser, E400_JSON_SYNTAX_ERROR );
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
		}
	}
	return ( 0 );
}

static uint32_t Status_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( 0U == hams_has_uri_part( data, length ) )
	{
		if ( hams_check_headers_ended( data, length ) )
		{
			// Status_callback;
			if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
				 ( HTTP_METHOD_OPTIONS != parser->method ) )
			{
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
			}
			else
			{
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				parser->response_code = RESPONSE_OK;
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					if ( ( 0U == parser->cors_auth_common->origin_length ) ||
						 ( NULL == parser->cors_auth_common->origin_buffer ) )
					{
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
				}
				else
				{
					if ( ( Firmware_Upgrade_Inprogress() == true ) &&
						 ( g_fw_upgrade_user_id_js != rs->user_id ) )
					{
						hams_respond_error( parser, ERROR_UNAUTHORIZED );
					}
					else if ( ( Firmware_Upgrade_Inprogress() == false ) &&
							  ( 0U == rs->user_id ) )
					{
						hams_respond_error( parser, ERROR_UNAUTHORIZED );
					}
					else
					{
						Http_Server_Begin_Dynamic( parser );
						if ( HTTP_METHOD_GET == parser->method )
						{
							BF_FUS::image_status_req_t status_req = {0U};
							BF_FUS::image_status_resp_t status_resp = {0U};
							BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;
							if ( Firmware_Upgrade_Inprogress() == true )
							{
								json_response_t* json_buffer =
									reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

								if ( json_buffer != nullptr )
								{
									json_buffer->used_buff_len = 0U;
									memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

									char_t formatted_buff[BUFFER_LEN];
									memset( formatted_buff, 0U, BUFFER_LEN );

									Open_Json_Object( json_buffer );
									Add_String_Member( json_buffer, "Status", NULL, false );
									Open_Json_Object( json_buffer );

									status_req.proc_index = rs->p.fwop.proc_id;
									status_req.image_index = rs->p.fwop.image_id;
									return_status = fus_handle->Status( &status_req, &status_resp );
									if ( BF_FUS::SUCCESS == return_status )
									{
										if ( status_resp.status == BF_FUS::FUS_COMMIT_DONE )
										{
											Add_String_Member( json_buffer, "State", "Idle", true );
											Add_Integer_Member( json_buffer, "Wait", status_resp.remaining_op_time,
																false );
											Multiple_Close_Object( json_buffer, 2 );

											if ( ( ( g_fw_upgrade_user_id_js > 0U ) &&
												   ( g_fw_upgrade_user_id_js <= MAX_NUM_OF_AUTH_USERS ) ) &&
												 ( m_set_user_image_info_cb != nullptr ) )
											{
												m_set_user_image_info_cb( status_req.image_index,
																		  g_fw_upgrade_user_id_js );
											}
											BF_FUS::image_info_req_t image_req;
											image_req.proc_id = rs->p.fwop.proc_id;
											image_req.image_id = rs->p.fwop.image_id;
											BF_FUS::image_info_resp_t image_resp;
											if ( BF_FUS::SUCCESS == fus_handle->FUS::Image( &image_req, &image_resp ) )
											{
												FUS_Update_Audit_logs_Json( image_req.image_id, image_resp.version );
											}
										}
										else if ( status_resp.status == BF_FUS::FUS_PENDING_CALL_BACK )
										{
											Add_String_Member( json_buffer, "State", "Busy", true );
											Add_Integer_Member( json_buffer, "Wait", status_resp.remaining_op_time,
																false );
											Multiple_Close_Object( json_buffer, 2 );
										}
										else
										{
											Add_String_Member( json_buffer, "State", "Error", true );
											Add_Integer_Member( json_buffer, "Wait", status_resp.remaining_op_time,
																false );
											Multiple_Close_Object( json_buffer, 2 );
										}
									}
									else if ( BF_FUS::REQUEST_INVALID == return_status )
									{
										hams_respond_error( parser, STATUS_NOT_FOUND );
									}
									else
									{
										hams_respond_error( parser, ERROR_BAD_REQUEST );
									}
									Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
									Ram::De_Allocate( json_buffer );
								}
							}
							else
							{
								hams_respond_error( parser, 422U );
							}
						}
					}
				}
			}
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					hams_response_header( parser, HTTP_HDR_ALLOW, FIRMWARE_OPTIONS,
										  STRLEN( FIRMWARE_OPTIONS ) );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
				else if ( Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
										  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );

					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
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
void Fw_Upgrade_Session_Init_Handler_Json( User_Session* user_session )
{
	m_fw_upgrade_user_session_handler = user_session;
}

static void Block_User_Session_Expiration_Json( hams_parser* parser )
{
	pre_pbin_in_progress_js = false;
	bool status = false;

	if ( parser->cors_auth_common != nullptr )
	{
		if ( parser->cors_auth_common->digest_ptr != nullptr )
		{
			if ( parser->cors_auth_common->digest_ptr->rx_nonce_buf != nullptr )
			{
				User_Session::session_id_t sesson_id = 0U;
				if ( m_fw_upgrade_user_session_handler != nullptr )
				{

					status = m_fw_upgrade_user_session_handler->Block_Session_Expiration(
						parser->cors_auth_common->digest_ptr->rx_nonce_buf,
						&sesson_id );
					if ( status == true )
					{
						fus_user_sess_id = sesson_id;
					}
				}
			}
		}
	}
}

static void Unblock_User_Session_Expiration_Json( void )
{
	pre_pbin_in_progress_js = false;
	if ( fus_user_sess_id != User_Session::MAX_NUM_SESSIONS )
	{
		m_fw_upgrade_user_session_handler->Unblock_Session_Expiration( fus_user_sess_id );
		fus_user_sess_id = User_Session::MAX_NUM_SESSIONS;
	}
}

static uint32_t Validate_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;

	if ( 0U == hams_has_uri_part( data, length ) )
	{
		if ( hams_check_headers_ended( data, length ) )
		{
			// validate_callback;
			if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
				 ( HTTP_METHOD_OPTIONS != parser->method ) )
			{
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
			}
			else
			{
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				parser->response_code = RESPONSE_OK;
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					if ( ( 0U == parser->cors_auth_common->origin_length ) ||
						 ( NULL == parser->cors_auth_common->origin_buffer ) )
					{
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
				}
				else
				{
					if ( ( Firmware_Upgrade_Inprogress() == true ) &&
						 ( g_fw_upgrade_user_id_js != rs->user_id ) )
					{
						hams_respond_error( parser, ERROR_UNAUTHORIZED );
					}
					else if ( ( Firmware_Upgrade_Inprogress() == false ) &&
							  ( 0U == rs->user_id ) )
					{
						hams_respond_error( parser, ERROR_UNAUTHORIZED );
					}
					else
					{
						Http_Server_Begin_Dynamic( parser );
						if ( HTTP_METHOD_GET == parser->method )
						{
							BF_FUS::image_validate_req_t validate_req;
							BF_FUS::image_validate_resp_t validate_resp = {0U};
							validate_req.comp_index = rs->p.fwop.proc_id;
							validate_req.image_index = rs->p.fwop.image_id;

							json_response_t* json_buffer =
								reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

							if ( json_buffer != nullptr )
							{
								json_buffer->used_buff_len = 0U;
								memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

								char formatted_buff[BUFFER_LEN];
								memset( formatted_buff, 0U, BUFFER_LEN );

								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, "Validate", NULL, false );
								Open_Json_Object( json_buffer );

								return_status = fus_handle->Validate( &validate_req, &validate_resp );
								if ( BF_FUS::SUCCESS == return_status )
								{
									if ( validate_resp.status == BF_FUS::FUS_VALIDATION_IN_PROGRESS )
									{
										Add_String_Member( json_buffer, "Integrity", "Checking", true );
									}
									else if ( validate_resp.status == BF_FUS::FUS_VALID_IMAGE )
									{
										Add_String_Member( json_buffer, "Integrity", "Valid", true );
									}
									else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_CHECKSUM_FAILED )
									{
										Add_String_Member( json_buffer, "Integrity", "Invalid", true );
										if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
										{
											( *m_fw_upgrade_log_audit_capture_cb )( LOG_EVENT_CHECKSUM_FAILURE );
										}
									}
									else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_SIGNATURE_FAILED )
									{
										Add_String_Member( json_buffer, "Integrity", "Invalid", true );
										if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
										{
											( *m_fw_upgrade_log_audit_capture_cb )(
												LOG_EVENT_SIGNATURE_VERIFICATION_FAILURE );
										}
									}
									else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_PRODUCT_CODE_FAILED )
									{
										Add_String_Member( json_buffer, "Integrity", "Invalid", true );
										// ToDo: Not defined log event code for product code failure.
										// if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
										// {
										// ( *m_fw_upgrade_log_audit_capture_cb )( LOG_EVENT_PRODUCT_CODE_FAILURE );
										// }
									}
									else if ( validate_resp.status ==
											  BF_FUS::FUS_INVALID_IMAGE_VERSION_ROLLBACK_FAILED )
									{
										Add_String_Member( json_buffer, "Integrity", "Invalid", true );
										if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
										{
											( *m_fw_upgrade_log_audit_capture_cb )( LOG_EVENT_VERSION_ROLLBACK_FAILURE );
										}
									}

									Add_Integer_Member( json_buffer, "Wait", validate_resp.op_time, false );
									Multiple_Close_Object( json_buffer, 2 );
									Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								}
								else if ( BF_FUS::REQUEST_INVALID == return_status )
								{
									hams_respond_error( parser, STATUS_NOT_FOUND );
								}
								else
								{
									hams_respond_error( parser, ERROR_BAD_REQUEST );
								}
								Ram::De_Allocate( json_buffer );
							}
						}
					}
				}
			}
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					hams_response_header( parser, HTTP_HDR_ALLOW, FIRMWARE_OPTIONS,
										  STRLEN( FIRMWARE_OPTIONS ) );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
				else if ( Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
										  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );

					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
		}
	}
	return ( 0U );
}

static uint32_t Commit_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();

	if ( HTTP_METHOD_POST == parser->method )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( rs->auth_level < FW_MIN_ACCESS_LEVEL )
			{
				hams_respond_error( parser, ERROR_UNAUTHORIZED );
			}
			else
			{
				/* There should be request body.. */
				if ( 0U == parser->content_length )
				{
					hams_respond_error( parser, ERROR_BAD_REQUEST );
				}
				else
				{
					/* create a fresh session */
					if ( Firmware_Upgrade_Inprogress() == true )
					{
						rs->p.fwop.session_id = 0U;
						parser->user_data_cb = static_cast<hams_data_cb>( Commit_Mgmt_Cb_Json );
						rs->p.rest.yjson = Http_Server_Alloc_Json_Parser( parser, 32U );
					}
					else
					{
						/* Session not created.*/
						hams_respond_error( parser, E422_SESSION_ID_MISMATCH );
					}
				}
			}
		}
	}
	else if ( HTTP_METHOD_OPTIONS == parser->method )
	{
		if ( 0U != parser->content_length )
		{
			hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );	/* Request Entity is too large */
		}
		else
		{
			if ( ( 0U == parser->cors_auth_common->origin_length ) ||
				 ( NULL == parser->cors_auth_common->origin_buffer ) )
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
			else
			{
				if ( 0U != hams_check_headers_ended( data, length ) )
				{
					parser->response_code = RESPONSE_OK;
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				}
				else
				{
					if ( 0U != hams_can_respond( data, length ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
											  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
						hams_response_header( parser, HTTP_HDR_ALLOW, SESSION_OPTIONS,
											  STRLEN( SESSION_OPTIONS ) );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  ( const void* )nullptr, 0U );
					}
				}
			}
		}

	}
	else
	{
		hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
	}

	return ( 0U );
}

static uint32_t Commit_Mgmt_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;
	BF_FUS::image_commit_resp_t commit_resp = { };
	BF_FUS::image_commit_req_t commit_req = { };
	BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;

	jsmn_parser j_parser;
	int16_t result = 0;
	uint16_t index = 0;
	jsmntok_t tokens[BUFFER_LEN];
	static char_t* p_msg_buffer = nullptr;
	static uint16_t c_msg_cnt = 1;
	static char_t json_value[JSON_VALUE_LEN];

	commit_req.session_id = 0U;

	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yjson_put_json_state* ypx =
		reinterpret_cast<struct yjson_put_json_state*>( rs->p.rest.yjson );

	commit_req.comp_id = rs->p.fwop.proc_id;
	commit_req.image_id = rs->p.fwop.image_id;

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( parser->content_type != HTTP_CONTENT_TYPE )
		{
			hams_respond_error( parser, ERROR_UNSUPPORTED_MEDIA_TYPE );
		}
		else
		{
			if ( c_msg_cnt == 1 )
			{
				p_msg_buffer = reinterpret_cast<char_t*>( Ram::Allocate( JSON_MAXBUFF_LEN * sizeof( char_t ) ) );

				if ( p_msg_buffer != nullptr )
				{
					memset( p_msg_buffer, 0, JSON_MAXBUFF_LEN );
					memcpy( p_msg_buffer, data, length );
				}
			}
			else
			{
				strncat( p_msg_buffer, ( char_t* )data, length );
				c_msg_cnt--;
			}

			uint16_t msgLen = strlen( p_msg_buffer );

			jsmn_init( &j_parser );
			jsmntok_t_init( tokens, BUFFER_LEN );

			result =
				jsmn_parse( &j_parser, ( char_t* )p_msg_buffer, msgLen, tokens,
							sizeof( tokens ) / sizeof( tokens[0] ) );

			if ( result < 0 )
			{
				if ( result == -3 )
				{
					/* Invalid Json string, More bytes expected */
					c_msg_cnt++;
					return ( 0U );
				}
				else
				{
					/* Failed to parse Commit parameter */
					error_code = E400_JSON_SYNTAX_ERROR;
				}
			}

			for ( index = 1; ( ( index < result ) && ( error_code == 200 ) ); index++ )
			{
				size_t token_size = tokens[index + 1].end - tokens[index + 1].start;

				if ( HTTP_METHOD_POST == parser->method )
				{
					if ( INIT_STATE == ypx->put_state )
					{
						if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
									 reinterpret_cast<const char_t*>( COMMIT_TAG_DEF ) ) == 0 )
						{
							ypx->put_state = SESSION_TAG_STATE;
							rs->p.fwop.session_id = 0;
						}
					}
					else
					{
						if ( SESSION_TAG_STATE == ypx->put_state )
						{
							if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
										 reinterpret_cast<const char_t*>( SESSION_ID_TAG ) ) == 0 )
							{
								memset( json_value, 0, JSON_VALUE_LEN );
								strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

								ypx->put_state = TIMEOUT_TAG_STATE;

								if ( TIMEOUT_TAG_STATE == ypx->put_state )
								{
									uint16_t k = 0;
									while ( json_value[k] != NULL )
									{
										if ( IS_DIGIT( json_value[k] ) )
										{
											rs->p.fwop.session_id *= 10U;
											rs->p.fwop.session_id += ( json_value[k] - 0x30 );
											if ( ( rs->p.fwop.session_id > MAX_NUMBER_5_DIGIT ) ||
												 ( rs->p.fwop.session_id < 1 ) )
											{
												error_code = E422_SESSION_ID_MISMATCH;
											}
										}
										else
										{
											error_code = ERROR_BAD_REQUEST;
										}
										k++;
									}
								}
								if ( TIMEOUT_TAG_STATE == ypx->put_state )
								{
									ypx->put_state = DATA_END_STATE;
								}
							}
						}
					}

					if ( DATA_END_STATE == ypx->put_state )
					{
						if ( index == result - 1 )
						{
							commit_req.session_id = rs->p.fwop.session_id;
							return_status = fus_handle->Commit( &commit_req, &commit_resp );
							if ( BF_FUS::SUCCESS == return_status )
							{
								parser->response_code = RESPONSE_OK;
								rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
								Http_Server_Begin_Dynamic( parser );


								json_response_t* json_buffer =
									reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

								if ( json_buffer != nullptr )
								{
									json_buffer->used_buff_len = 0U;
									memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

									char_t formatted_buff[BUFFER_LEN];
									memset( formatted_buff, 0U, BUFFER_LEN );

									Open_Json_Object( json_buffer );
									Add_String_Member( json_buffer, "CommitResp", NULL, false );
									Open_Json_Object( json_buffer );
									Add_Integer_Member( json_buffer, "Wait", commit_resp.op_time, false );
									Multiple_Close_Object( json_buffer, 2 );
									Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								}
							}
							else if ( BF_FUS::REJECT == return_status )
							{
								error_code = E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS;
							}
							else if ( BF_FUS::REQUEST_INVALID == return_status )
							{
								error_code = STATUS_NOT_FOUND;
							}
							else
							{
								error_code = ERROR_BAD_REQUEST;
							}
						}
					}
				}
			}
			if ( RESPONSE_OK != error_code )
			{
				c_msg_cnt = 1;
				Ram::De_Allocate( p_msg_buffer );
				hams_respond_error( parser, error_code );
			}
		}
	}
	else
	{
		if ( ( 0U != hams_can_respond( data, length ) ) && ( parser->response_code != 0U ) )
		{
			if ( rs->flags & REQ_FLAG_NO_DYN_PARSER_SPACE )
			{
				if ( Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
										  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );

					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
		}
		else
		{
			hams_respond_error( parser, ERROR_BAD_REQUEST );
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Exit_firmware_Session_Json( void )
{
	fus_handle->Delete_Session();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Firmware_Session_Id_Json( void )
{
	return ( fus_handle->Get_Session_Id() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Allocate_Rest_FUS_Buffer_Json( void )
{
	bool return_status = false;

	// Allocate the size of the m_decoded_buff and m_encoded_buff
	m_decoded_buff = new uint8_t[MAX_FW_UPGRADE_DECODED_PACKET_SIZE];
	if ( m_decoded_buff != nullptr )
	{
		m_encoded_buff = new uint8_t[MAX_FW_UPGRADE_ENCODED_PACKET_SIZE];
		if ( m_encoded_buff != nullptr )
		{
			return_status = true;
		}
		else
		{
			delete[] m_decoded_buff;
			m_decoded_buff = nullptr;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Deallocate_Rest_FUS_Buffer_Json( void )
{
	// free the decoded and encoded buffer
	if ( m_decoded_buff != nullptr )
	{
		delete ( m_decoded_buff );
		m_decoded_buff = nullptr;
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Decoded buffer value is nullptr already, deallocation failed " );
	}
	if ( m_encoded_buff != nullptr )
	{
		delete ( m_encoded_buff );
		m_encoded_buff = nullptr;
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Encoded buffer value is nullptr already, deallocation failed" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FUS_Update_Audit_logs_Json( uint8_t image_id, uint32_t firm_ver )
{
	uint8_t fw_param[5] = {0};
	uint32_t rev_num = 0U;

	rev_num = firm_ver;
	if ( m_fw_upgrade_log_param_cb != nullptr )
	{
		fw_param[2] = ( ( rev_num ) & 0x000000ff );
		fw_param[3] = ( ( rev_num ) & 0x0000ff00 ) >> 8;
		fw_param[1] = ( ( rev_num ) & 0x00ff0000 ) >> 16;
		fw_param[0] = ( ( rev_num ) & 0xff000000 ) >> 24;

		fw_param[4] = image_id & 0x00FF;
		( *m_fw_upgrade_log_param_cb )( LOG_EVENT_FW_UPDATE_COMPLETE,
										fw_param );
	}
	if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
	{
		( *m_fw_upgrade_log_audit_capture_cb )( LOG_EVENT_FW_UPDATE_COMPLETE );
	}
}
