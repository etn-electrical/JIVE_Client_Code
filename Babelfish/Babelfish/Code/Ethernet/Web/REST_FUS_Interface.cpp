/**
 *****************************************************************************************
 * @file
 * @details REST-FUS interface file help to get the execute REST operation.
 * @copyright 2019 Eaton Corporation. All Rights Reserved.
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
#include "yxml.h"
#include "Base64_Rest.h"
#include "httpd_common.h"
#include "REST_FUS_Interface.h"
#include "FUS.h"
#include "FUS_Msg_Structs.h"
#include "server.h"
#include "FUS_Config.h"
#include "Rest_WebUI_Interface.h"
#include "Web_Debug.h"
#include "Babelfish_Assert.h"
#include "FUS_Debug.h"

#define FIRMWARE_OPTIONS   "GET, HEAD, OPTIONS"
// extern "C"{

/*
 *****************************************************************************************
 *		Functions
 *****************************************************************************************
 */
extern void Validate_Options_Cors_Headers( hams_parser* parser, http_server_req_state* rs,
										   hams_data_cb set_user_callback );

// --------- REST Parser Function ------------------

static uint32_t Fw_Parse_Image_Id_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

// --------- Get Product Info  --------------------
uint32_t Prod_Info_Spec_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

// ---------- Get Component Info -------------------
static uint32_t Processor_Details_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

// ------------ Get Image Info -------------------
static uint32_t Firmware_Details_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

////////////session Functions //////////////////////

static uint32_t Session_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Session_Mgmt_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static void Block_User_Session_Expiration( hams_parser* parser );

static void Unblock_User_Session_Expiration( void );

// ------------ Put Bin Request Function ------------
static uint32_t Init_Fw_Bin_Requst_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Parse_Fw_Bin_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static bool Save_Chunk_Data( uint32_t dest_buf_start_index, uint32_t bin_len,
							 uint8_t const* src_buff, uint8_t* dest_buff,
							 uint16_t dest_buff_size );

// -------------- Validate the image ----------------
static uint32_t Validate_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

// -------------- Commit the Image Function ---------
static uint32_t Commit_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Commit_Mgmt_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

// ---------------- Status Request Function ----------
static uint32_t Status_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

// this handled in prod_spec_fw_upgrade
int32_t Fw_Reboot( void );

void Assign_Web_Pages( uint8_t ui_type, NV_Ctrl* web_ui_nv_ctrl );

void FUS_Update_Audit_logs( uint8_t image_id, uint32_t firm_ver );

static Fw_Upgrade_Audit_Function_Decl_1 m_fw_upgrade_log_audit_capture_cb =
	reinterpret_cast<Fw_Upgrade_Audit_Function_Decl_1>( nullptr );
static Fw_Upgrade_Audit_Function_Decl_2 m_fw_upgrade_log_param_cb =
	reinterpret_cast<Fw_Upgrade_Audit_Function_Decl_2>( nullptr );
static Fw_Upgrade_User_Function_Decl_1 m_get_user_image_info_cb =
	reinterpret_cast<Fw_Upgrade_User_Function_Decl_1>( nullptr );
static Fw_Upgrade_User_Function_Decl_2 m_set_user_image_info_cb =
	reinterpret_cast<Fw_Upgrade_User_Function_Decl_2>( nullptr );

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

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

// static const uint8_t TAG_DATAPUT_ID = 1U;
static const uint8_t TAG_CHUNK_TOTAL_ID = 2U;
static const uint8_t TAG_CHUNK_NUM_ID = 3U;
// static const uint8_t TAG_MONO_DATA_ID = 2U;
static const uint8_t TAG_CHUNK_DATA_ID = 4U;

static const char_t* BIN_TAGS[TAG_CHUNK_DATA_ID] =
{
	FW_BIN_TAG_1,
	FW_BIN_TAG_2,
	FW_BIN_TAG_3,
	FW_BIN_TAG_4
};

////////////Session Variables Enums, Constants, Macros//////////////////////
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

///////////////////////////////////////////////////
/////////////PUT/bin variable /////////////////////

enum data_compl_check_t
{
	DATA_COMPL_NOT_VERIFIED,
	DATA_COMPL_VERIFY_IN_PROGRESS,
	DATA_COMPL_VERIFIED
} data_compl_check;

enum put_bin_state_t
{
	BIN_INIT,
	BIN_CHUNK_INIT,
	BIN_INIT_COMPL,
	BIN_BEFORE_TAG_COMPL,
	BIN_DATA_STREAM_COMPL,
	BIN_AFTER_TAG_COMPL,
	BIN_COMPLETE,
	BIN_ERROR,
} put_bin_state;

typedef struct put_bin_metadata_s
{
	put_bin_state_t state;
	uint8_t tag_index;
	uint8_t attr_index;
	data_compl_check_t elem_end_chk;
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
} put_bin_metadata;

// define the encoded and decoded buffer for memory allocation
static uint8_t* m_encoded_buff = nullptr;
static uint8_t* m_decoded_buff = nullptr;

static User_Session::session_id_t fus_user_sess_id = User_Session::MAX_NUM_SESSIONS;
static BF_FUS::FUS* fus_handle = nullptr;
static uint8_t g_encode_type = NO_ENCODE;
static User_Session* m_fw_upgrade_user_session_handler = nullptr;
bool pre_pbin_in_progress = false;
uint8_t g_fw_upgrade_user_id = 0xFFU;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Firmware_Upgrade_Info( Fw_Upgrade_User_Function_Decl_1 get_user_info_cb,
								 Fw_Upgrade_User_Function_Decl_2 set_user_info_cb )
{
	m_get_user_image_info_cb = get_user_info_cb;
	m_set_user_image_info_cb = set_user_info_cb;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Assign_Web_Pages( uint8_t ui_type, NV_Ctrl* web_ui_nv_ctrl )
{
	const fw_www_header* www = reinterpret_cast<const fw_www_header*>( NULL );

	if ( RUN_WEB_UI == ui_type )
	{

		www = Get_Fw_WWW_Header( web_ui_nv_ctrl );
	}

	WEBSERVER_DEBUG_PRINT( DBG_MSG_INFO, "Setting HTTP Server Page" );
	Http_Server_Set_Pages( www, ui_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Web_Page_Init( NV_Ctrl* web_ui_nv_ctrl )
{
	uint16_t server_flag = Get_Server_Flags();

	switch ( server_flag )
	{
		case HTTP_SERVER_FLAG_NOT_READY:
			Assign_Web_Pages( RUN_DEFAULT_UI, web_ui_nv_ctrl );
			break;

		case HTTP_SERVER_FLAG_MAINTENANCE_PAGES:
			Assign_Web_Pages( RUN_MAINTENANCE_UI, web_ui_nv_ctrl );
			break;

		case HTTP_SERVER_FLAG_WEB_PAGES:
			Assign_Web_Pages( RUN_WEB_UI, web_ui_nv_ctrl );
			break;

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Fus_Upgrade_Session_Init_Handler( User_Session* user_session )
{
	m_fw_upgrade_user_session_handler = user_session;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

int32_t Fw_Reboot( void )
{
	BF::System_Reset::Reset_Device( BF::System_Reset::SOFT_RESET );
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Fw_Upgrade_Audit_Log_Assign_Callback( Fw_Upgrade_Audit_Function_Decl_1 fw_audit_log_cb,
										   Fw_Upgrade_Audit_Function_Decl_2 fw_audit_param_cb )
{
	m_fw_upgrade_log_audit_capture_cb = fw_audit_log_cb;
	m_fw_upgrade_log_param_cb = fw_audit_param_cb;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Parse_Fw_Resources_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = 200U;

	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( length > 1U )
		{
			if ( ( length == 8U ) &&
				 ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/session", length ) ==
				   0 ) )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Session_cb );
			}
			else
			{
				uint16_t val = 0U;
				if ( ERR_PARSING_OK ==
					 INT_ASCII_Conversion::str_to_uint16( ( data ), 1U,
														  length - 1U, &val ) )
				{
					if ( val < fus_handle->Get_Component_Count() )
					{
						rs->p.fwop.proc_id = val;
					}
					else
					{
						error_code = 404U;
					}

				}
				else
				{
					error_code = 400U;
				}

				if ( data[length] == 0x2FU )	// '/'
				{
					parser->user_data_cb = static_cast<hams_data_cb>
						( &Fw_Parse_Image_Id_Cb );
				}
				else
				{
					parser->user_data_cb = static_cast<hams_data_cb>
						( &Processor_Details_Cb );
				}
			}
			if ( error_code != 200U )
			{
				hams_respond_error( parser, error_code );
			}
		}
		else
		{
			if ( false == IS_EXTRA_SLASH( data ) )
			{
				parser->user_data_cb = Prod_Info_Spec_cb;
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
uint32_t Parse_Command_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = 200U;

	if ( length > 1U )
	{
		if ( ( length == 7U ) &&
			 ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/status", length ) == 0 ) )
		{
			// status_callback;
			parser->user_data_cb = static_cast<hams_data_cb>( &Status_cb );
		}
		else if ( ( length == 9U ) &&
				  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/validate", length ) ==
					0 ) )
		{
			// validate_callback;
			parser->user_data_cb = static_cast<hams_data_cb>( &Validate_cb );
		}
		else if ( ( length == 7U ) &&
				  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/commit", length ) ==
					0 ) )
		{
			// commit_callback
			parser->user_data_cb = static_cast<hams_data_cb>( &Commit_cb );
		}
		else if ( ( length == 4U ) &&
				  ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/bin", length ) == 0 ) )
		{
			// put_bin_callback
			parser->user_data_cb = static_cast<hams_data_cb>( &Init_Fw_Bin_Requst_Cb );
		}
		else
		{
			error_code = 400U;
		}

		if ( error_code != 200U )
		{
			hams_respond_error( parser, error_code );
		}
	}
	else
	{
		if ( true == ENDS_WITH_SLASH( data ) )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Firmware_Details_Cb );
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Init_Fw_Bin_Requst_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = 200U;

	if ( length == 0U )
	{
		if ( Firmware_Upgrade_Inprogress() == true )
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			rs->p.fwop.state = 0U;
			if ( ( pre_pbin_in_progress == false ) )
			{
				if ( parser->method == HTTP_METHOD_PUT )
				{
					if ( rs->user_id == g_fw_upgrade_user_id )
					{
						if ( parser->content_length != 0U )
						{
							if ( parser->content_type == HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED )
							{
								if ( true == Is_Fw_Upgrade_Enabled() )
								{
									/* Allocate ourselves a block for xml parsing. */
									rs->p.rest.yxml = Http_Server_Alloc_Xml_Parser( parser,
																					32U );

									if ( rs->p.rest.yxml != NULL )
									{
										BF_ASSERT( m_encoded_buff );
										rs->p.fwop.ptr = m_encoded_buff;

										if ( nullptr != rs->p.fwop.ptr )
										{
											pre_pbin_in_progress = true;
											rs->p.fwop.state = 0x55;
											struct yxml_put_xml_state* ypx =
												reinterpret_cast<struct yxml_put_xml_state*>( rs
																							  ->
																							  p
																							  .
																							  rest
																							  .
																							  yxml );

											ypx->flags = 0U;

											parser->user_data_cb = Parse_Fw_Bin_Cb;
										}
										else
										{
											error_code = E503_SCRATCH_SPACE;
										}
									}
									else
									{
										error_code = E503_XML_PARSER;
									}
								}
								else
								{
									error_code = 403U;	// upgrade enable flag is diabled.
								}
							}
							else
							{
								error_code = 415U;
							}
						}
						else
						{
							error_code = 400U;
						}
					}
					else
					{
						error_code = 401U;
					}
				}
				else if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					if ( 0U != parser->content_length )
					{
						hams_respond_error( parser, 413U );	/* Request Entity is too large */
					}
					else
					{

						if ( ( 0U == parser->cors_auth_common->origin_length ) ||
							 ( NULL == parser->cors_auth_common->origin_buffer ) )
						{
							hams_respond_error( parser, 400U );
						}
						else
						{
							if ( 0U != hams_check_headers_ended( data, length ) )
							{
								parser->response_code = 200U;
								rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
							}
							else
							{
								if ( 0U != hams_can_respond( data, length ) )
								{
									hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
														  "application/xml", 15U );
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
					error_code = 405U;
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
uint32_t Fw_Parse_Image_Id_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = 200U;

	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( length > 1U )
		{
			uint16_t val = 0U;
			if ( ERR_PARSING_OK ==
				 INT_ASCII_Conversion::str_to_uint16( ( data ), 1U,
													  length - 1U, &val ) )
			{
				rs->p.fwop.image_id = val;
			}
			else
			{
				error_code = 400U;
			}

			if ( data[length] == 0x2FU )// '/'
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Parse_Command_Cb );
			}
			else
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Firmware_Details_Cb );
			}
			if ( error_code != 200U )
			{
				hams_respond_error( parser, error_code );
			}
		}
		else
		{
			if ( false == IS_EXTRA_SLASH( data ) )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Processor_Details_Cb );
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
static uint32_t Parse_Fw_Bin_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = 200U;
	uint16_t i = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	yxml_t* yx = rs->p.rest.yxml;
	uint8_t fw_param[User_Account::USER_NAME_LIMIT + 3U] = {0};
	uint32_t decoded_len = 0U;
	BF_FUS::image_write_req_t write_image_in_mem = {0U};
	BF_FUS::image_write_resp_t write_optime = {0U};

#ifdef ESP32_SETUP
	static uint32_t image_start_address = 0U;
#endif
	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;

	static put_bin_metadata pbin =
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
				error_code = 500U;
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

		while ( pbin.state < BIN_COMPLETE && ( i < length ) && error_code == 200U )
		{
			if ( pbin.state != BIN_BEFORE_TAG_COMPL )
			{
				int32_t r = yxml_parse( yx, static_cast<int32_t>( data[i] ) );

				if ( pbin.state == BIN_DATA_STREAM_COMPL )
				{
					if ( pbin.elem_end_chk == DATA_COMPL_NOT_VERIFIED )
					{
						pbin.elem_end_chk = DATA_COMPL_VERIFY_IN_PROGRESS;
						if ( data[i] != '<' )
						{
							error_code = 400U;
						}
					}
					else if ( pbin.elem_end_chk == DATA_COMPL_VERIFY_IN_PROGRESS )
					{
						if ( data[i] != '/' )
						{
							error_code = 400U;
						}
						pbin.elem_end_chk = DATA_COMPL_VERIFIED;
					}
				}
				switch ( r )
				{
					/* Expecting the proper tag element. */
					case YXML_ELEMSTART:
					{
						if ( 0 ==
							 strncmp( yx->elem,
									  reinterpret_cast<const char_t*>( BIN_TAGS[pbin
																				.tag_index] ),
									  ( strlen(
											reinterpret_cast<const char_t*>( BIN_TAGS[pbin
																					  .tag_index] ) )
										- 1U ) ) )
						{
							pbin.tag_index++;
						}
						else
						{
							error_code = 400U;
						}
						break;
					}

					case YXML_CONTENT:
						if ( pbin.tag_index == TAG_CHUNK_DATA_ID )
						{
							pbin.state = BIN_BEFORE_TAG_COMPL;
						}
						else if ( pbin.tag_index == TAG_CHUNK_TOTAL_ID )
						{
							if ( IS_DIGIT( yx->data[0] ) )
							{
								pbin.chunk_total *= 10U;
								pbin.chunk_total += ( yx->data[0] - 0x30 );

								if ( pbin.chunk_total > MAX_TOTAL_CHUNKS )
								{
									error_code = E422_OUT_OF_ORDER_CHUNK;
								}
							}
							else
							{
								error_code = 400U;
							}
						}
						else if ( pbin.tag_index == TAG_CHUNK_NUM_ID )
						{
							if ( IS_DIGIT( yx->data[0] ) )
							{
								pbin.chunk_num *= 10U;
								pbin.chunk_num += ( yx->data[0] - 0x30 );

								if ( pbin.chunk_num >= pbin.chunk_total )
								{
									error_code = E422_OUT_OF_ORDER_CHUNK;
								}
							}
							else
							{
								error_code = 400U;
							}
						}
						break;

					case YXML_ELEMEND:
						if ( pbin.tag_index == TAG_CHUNK_DATA_ID )
						{
							if ( pbin.state == BIN_DATA_STREAM_COMPL )
							{
								pbin.state = BIN_AFTER_TAG_COMPL;
							}
							else if ( pbin.state < BIN_BEFORE_TAG_COMPL )
							{
								error_code = 400U;
							}
						}
						else if ( pbin.tag_index == TAG_CHUNK_TOTAL_ID )
						{
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
						else if ( pbin.tag_index == TAG_CHUNK_NUM_ID )
						{
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
										error_code = 200U;
									}
									else
									{
										error_code = E422_OUT_OF_ORDER_CHUNK;
									}
								}
							}
						}
						if ( error_code == 200U )
						{
							if ( *yx->elem == static_cast<char_t>( 0U ) )
							{
								if ( ( ( ( length - i ) == 1U ) && ( data[i] == 0x3EU ) ) ||
									 ( ( ( length - i ) > 1U ) && ( data[i] == 0x3EU ) &&
									   ( IS_SP_OR_HT( data[i + 1] ) ||
										 IS_NEW_LINE( data[i + 1] ) ) ) )
								{
									pbin.state = BIN_COMPLETE;
								}
								else
								{
									error_code = 400U;
								}
							}
						}
						break;

					case YXML_ATTRSTART:
						if ( 0 ==
							 strncmp( yx->attr,
									  reinterpret_cast<const char_t*>( BIN_ATTR[pbin.attr_index] ),
									  ( strlen(
											reinterpret_cast<const char_t*>( BIN_ATTR[pbin.
																					  attr_index] ) )
										- 1U ) ) )
						{
							pbin.attr_index++;
						}
						else
						{
							error_code = 400U;
						}
						break;

					case YXML_ATTREND:
						if ( pbin.attr_index == ATTR_SESSION_ID )
						{}
						else if ( pbin.attr_index == ATTR_ADDRESS_ID )
						{}
						else if ( pbin.attr_index == ATTR_SIZE_ID )
						{
							if ( pbin.recd_size == 0U )
							{
								error_code = 400U;
							}
						}
						break;

					case YXML_ATTRVAL:
						if ( pbin.attr_index == ATTR_SESSION_ID )
						{
							if ( IS_DIGIT( yx->data[0] ) )
							{
								pbin.recd_session_id *= 10U;
								pbin.recd_session_id += ( yx->data[0] - 0x30 );

								if ( ( pbin.recd_session_id > MAX_NUMBER_5_DIGIT ) ||
									 ( pbin.recd_session_id < 1 ) )
								{
									error_code = E422_SESSION_ID_MISMATCH;
								}
							}
							else
							{
								error_code = 400U;
							}

						}
						else if ( pbin.attr_index == ATTR_ADDRESS_ID )
						{
							if ( pbin.pre_address != nullptr )
							{
								uint8_t i = strlen( pbin.pre_address );
								pbin.pre_address[i++] = yx->data[0];
								pbin.pre_address[i] = '\0';
							}
							else
							{
								error_code = 400U;
							}
						}
						else if ( pbin.attr_index == ATTR_SIZE_ID )
						{
							if ( IS_DIGIT( yx->data[0] ) )
							{
								pbin.recd_size *= 10U;
								pbin.recd_size += ( yx->data[0] - 0x30 );
								if ( pbin.recd_size > MAX_FW_UPGRADE_ENCODED_PACKET_SIZE )
								{
									error_code = E422_SIZE_MISMATCH;
								}
							}
							else
							{
								error_code = 400U;
							}
						}
						break;

					case YXML_OK:
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

					case YXML_PISTART:
					case YXML_PICONTENT:
					case YXML_PIEND:
					default:
						break;
				}
			}
			if ( ( BIN_BEFORE_TAG_COMPL == pbin.state ) && ( error_code == 200U ) )
			{
				uint16_t bin_len = ( length - i );

				if ( ( pbin.put_bin_len + bin_len ) > pbin.recd_size )
				{
					bin_len = pbin.recd_size - pbin.put_bin_len;
				}

				if ( true ==
					 Save_Chunk_Data( pbin.put_bin_len, bin_len, &data[i],
									  ( uint8_t* )rs->p.fwop.ptr,
									  MAX_FW_UPGRADE_ENCODED_PACKET_SIZE ) )
				{
					pbin.put_bin_len += bin_len;

					if ( pbin.put_bin_len >= pbin.recd_size )
					{
						if ( error_code == 200U )
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
									decoded_len = pbin.put_bin_len >> 1U;		// divide by 2
									if ( decoded_len >
										 ( MAX_FW_UPGRADE_DECODED_PACKET_SIZE ) )
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
							if ( error_code == 200U )
							{
								if ( ( rs->p.fwop.proc_id <= fus_handle->Get_Component_Count() ) )		// ||
								{
									write_image_in_mem.proc_index = rs->p.fwop.proc_id;
									write_image_in_mem.image_index = rs->p.fwop.image_id;
									write_image_in_mem.session_id = pbin.recd_session_id;
									write_image_in_mem.data = m_decoded_buff;
									write_image_in_mem.data_len = decoded_len;

#ifdef ESP32_SETUP
									write_image_in_mem.address = image_start_address
										+ strtol( pbin.pre_address, NULL, 16U );
#else
									write_image_in_mem.address = strtol( pbin.pre_address, NULL, 16U );
#endif

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
										error_code = 404U;
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
										error_code = 400U;
									}
								}
								else
								{
									error_code = 404U;
								}
							}
						}
					}
				}
				else
				{
					error_code = E503_SCRATCH_SPACE;
				}
				i += ( bin_len - 1 );
			}
			i++;
		}

		if ( ( error_code != 200U ) || ( pbin.state == BIN_COMPLETE ) )
		{
			delete[] pbin.pre_address;
			pbin.pre_address = nullptr;
			if ( error_code != 200U )
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
				parser->response_code = 200U;
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				Http_Server_Begin_Dynamic( parser );
				Http_Server_Print_Dynamic( parser,
										   "<DataPUT SessionID=" QUOTE_STR "\bu" QUOTE_STR ">\n\t",
										   pbin.recd_session_id );
				Http_Server_Print_Dynamic( parser,
										   "<Wait>" "\bu" "</Wait>\n", pbin.next_req_wait_time );	// In
																									// MS
				Http_Server_Print_Dynamic( parser,
										   "</DataPUT>" );
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
		else
		{
			if ( pbin.state == BIN_BEFORE_TAG_COMPL )
			{
				hams_respond_error( parser, E422_SIZE_MISMATCH );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
			pbin.state = BIN_INIT;
			delete[] pbin.pre_address;
			pbin.pre_address = nullptr;
			// Http_Server_Free( rs->p.fwop.ptr );
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static bool Save_Chunk_Data( uint32_t dest_buf_start_index, uint32_t bin_len,
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
uint32_t Prod_Info_Spec_cb( hams_parser* parser, const uint8_t* data, uint32_t length )
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
				hams_respond_error( parser, 401U );
			}
			/* There should not be any request body.. */
			else if ( 0U != parser->content_length )
			{
				hams_respond_error( parser, 413U );
			}
			else if ( ( HTTP_METHOD_GET == parser->method ) ||
					  ( HTTP_METHOD_HEAD == parser->method ) )
			{
				BF_FUS::prod_info_t prod_info = { };
				uint8_t prod_name[PROD_NAME_MAX_LENGTH] = "";
				prod_info.name = prod_name;

				if ( BF_FUS::SUCCESS == fus_handle->Product_Info( &prod_info ) )
				{
					parser->response_code = 200U;
					http_server_req_state* rs = Http_Server_Get_State( parser );
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
					Http_Server_Begin_Dynamic( parser );
					Http_Server_Print_Dynamic( parser,
											   XML_DECL XML_PRODUCT_PREFIX_XLINK XML_NAME_START "\bs" XML_NAME_END,
											   prod_info.name_len, prod_info.name );

					Http_Server_Print_Dynamic( parser,
											   XML_PRODUCT_CODE_START "\bu" XML_PRODUCT_CODE_END,
											   prod_info.code );

					Http_Server_Print_Dynamic( parser, XML_PROD_GUID_START );
					Http_Server_Print_Dynamic( parser, "\bu" XML_PROD_GUID_END, prod_info.guid );

					boot_spec_version = prod_info.boot_version;
					boot_spec_ver_major = ( boot_spec_version >> 24 );
					boot_spec_ver_minor = ( ( boot_spec_version >> 16 ) & 0xFF );
					boot_spec_rev = ( boot_spec_version & 0xFFFF );

					boot_version = prod_info.boot_spec_version;
					boot_ver_major = ( boot_version >> 24 );
					boot_ver_minor = ( ( boot_version >> 16 ) & 0xFF );
					boot_ver = ( boot_version & 0xFFFF );

					Http_Server_Print_Dynamic( parser,
											   "\n\t<BootSpecVer>\bu.\bu.\bu</BootSpecVer>", boot_spec_ver_major,
											   boot_spec_ver_minor, boot_spec_rev );
					Http_Server_Print_Dynamic( parser,
											   "\n\t<BootVer>\bu.\bu.\bu</BootVer>", boot_ver_major,
											   boot_ver_minor, boot_ver );
					Http_Server_Print_Dynamic( parser,
											   XML_SERIAL_NUM_START "\bu" XML_SERIAL_NUM_END,
											   prod_info.serial_num );
					Http_Server_Print_Dynamic( parser,
											   "\n\t<ProcessorCount>" "\bu" "</ProcessorCount>",
											   prod_info.component_count );
					Http_Server_Print_Dynamic( parser, "\n\t<ProcessorList>" );
					for ( uint8_t proc_id = 0; proc_id < prod_info.component_count; proc_id++ )
					{
						Http_Server_Print_Dynamic( parser,
												   XML_PROCESSOR_XLINK "\bu" XML_DEVICE_LINE_SEGMENT3,
												   proc_id );
					}
					Http_Server_Print_Dynamic( parser, "\n\t</ProcessorList>" );
					Http_Server_Print_Dynamic( parser, "\n</Product>" );
				}
				else
				{
					hams_respond_error( parser, 500U );
				}
			}
			else
			{
				hams_respond_error( parser, 405U );
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
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
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
uint32_t Processor_Details_Cb( hams_parser* parser, const uint8_t* data,
							   uint32_t length )
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
				hams_respond_error( parser, 401U );
			}
			else if ( 0U != parser->content_length )
			{
				hams_respond_error( parser, 413U );
			}
			else if ( ( HTTP_METHOD_GET == parser->method ) ||
					  ( HTTP_METHOD_HEAD == parser->method ) )
			{
				uint16_t spec_ver_major = 0U;
				uint16_t spec_ver_minor = 0U;
				uint16_t spec_build_ver = 0U;
				uint32_t proc_version = 0U;
				parser->response_code = 200U;

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
					Http_Server_Print_Dynamic( parser,
											   XML_DECL XML_PROCESSOR_PREFIX_XLINK "\bu" QUOTE_STR GT_STR,
											   rs->p.fwop.proc_id );
					Http_Server_Print_Dynamic( parser,
											   XML_NAME_START "\bs" XML_NAME_END, comp_resp.name_len,
											   comp_resp.name );
					proc_version = comp_resp.firmware_version;
					spec_ver_major = ( proc_version & 0x00FF );
					spec_ver_minor = ( ( proc_version >> 8 ) & 0xFF );
					spec_build_ver = ( ( proc_version >> 16 ) & 0xFFFFU );
					Http_Server_Print_Dynamic( parser,
											   XML_VERSION_START "\bu.\bu.\bu" XML_VERSION_END,
											   spec_ver_major, spec_ver_minor, spec_build_ver );
					Http_Server_Print_Dynamic( parser, "\t" XML_GUID_START );
					Http_Server_Print_Dynamic( parser, "\bu" XML_GUID_END, comp_resp.guid );
					Http_Server_Print_Dynamic( parser,
											   XML_SERIAL_NUM_START "\bu" XML_SERIAL_NUM_END,
											   comp_resp.serial_num );
					proc_version = comp_resp.hardware_version;
					spec_ver_major = ( proc_version & 0xFF );
					spec_ver_minor = ( ( proc_version >> 8 ) & 0xFF );
					spec_build_ver = ( ( proc_version >> 16 ) & 0xFFFFU );
					Http_Server_Print_Dynamic( parser,
											   "\n\t<HardwareVer>" "\bu.\bu.\bu" "</HardwareVer>",
											   spec_ver_major, spec_ver_minor, spec_build_ver );
					Http_Server_Print_Dynamic( parser, "\n\t<ImageCount>\bu</ImageCount>",
											   comp_resp.image_count );
					Http_Server_Print_Dynamic( parser, "\n\t<ImageList>" );
					for ( uint8_t image_id = 0U; image_id < comp_resp.image_count; image_id++ )
					{
						Http_Server_Print_Dynamic( parser,
												   XML_IMAGE_XLINK "\bu" SLASH_STR "\bu" XML_DEVICE_LINE_SEGMENT3,
												   rs->p.fwop.proc_id, image_id );
					}
					Http_Server_Print_Dynamic( parser, "\n\t</ImageList>" );
					Http_Server_Print_Dynamic( parser, "\n</Processor>" );
				}
				else
				{
					hams_respond_error( parser, 500U );
				}
			}
			else
			{
				hams_respond_error( parser, 405U );
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
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
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
uint32_t Firmware_Details_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
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
						hams_respond_error( parser, 401U );
					}
					/* There should not be any request body.. */
					else if ( 0U != parser->content_length )
					{
						hams_respond_error( parser, 413U );
					}
					else if ( ( HTTP_METHOD_GET == parser->method ) ||
							  ( HTTP_METHOD_HEAD == parser->method ) )
					{
						parser->response_code = 200U;
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
							Http_Server_Print_Dynamic( parser,
													   XML_DECL XML_IMAGE_PREFIX_XLINK "\bu" SLASH_STR "\bu" QUOTE_STR GT_STR,
													   image_info.proc_id, image_info.image_id );

							Http_Server_Print_Dynamic( parser,
													   XML_NAME_START "\bs" XML_NAME_END,
													   image_resp.name_len, image_resp.name );
							Http_Server_Print_Dynamic( parser, "\t" XML_GUID_START );
							Http_Server_Print_Dynamic( parser, "\bu" XML_GUID_END, image_resp.guid );

							firm_version = image_resp.version;
							firm_ver_major = ( firm_version & 0x00FF );
							firm_ver_minor = ( ( firm_version >> 8 ) & 0xFF );
							firm_build_ver = ( ( firm_version >> 16 ) & 0xFFFF );
							Http_Server_Print_Dynamic( parser,
													   XML_VERSION_START "\bu.\bu.\bu" XML_VERSION_END, firm_ver_major,
													   firm_ver_minor,
													   firm_build_ver );

							Http_Server_Print_Dynamic( parser,
													   XML_COMPATIBILITY_START "\bu" XML_COMPATIBILITY_END,
													   image_resp.compatibility_num );

							uint8_t last_upgrade_user[DCID_STRING_SIZE] = "";
							image_upgrade_info_t image_upgrade_info = { 0 };
							image_upgrade_info.last_user = last_upgrade_user;
							if ( m_get_user_image_info_cb != nullptr )
							{
								m_get_user_image_info_cb( image_info.image_id, &image_upgrade_info );
							}
							Http_Server_Print_Dynamic( parser, "\n\t<UpdateHistory>" );
							Http_Server_Print_Dynamic( parser,
													   "\n\t\t<Who>" "\bs" "</Who>", image_upgrade_info.user_len,
													   image_upgrade_info.last_user );
							Http_Server_Print_Dynamic( parser,
													   "\n\t\t<When>" "\bu" "</When>" "\n\t</UpdateHistory>",
													   image_upgrade_info.last_time );

							Http_Server_Print_Dynamic( parser,
													   XML_BIN_XLINK "\bu" SLASH_STR "\bu" "/bin" QUOTE_STR SLASH_GT,
													   image_info.proc_id,
													   image_info.image_id );
							Http_Server_Print_Dynamic( parser,
													   XML_VALID_XLINK "\bu" SLASH_STR "\bu" "/validate" QUOTE_STR SLASH_GT,
													   image_info.proc_id,
													   image_info.image_id );
							Http_Server_Print_Dynamic( parser,
													   XML_COMMIT_XLINK "\bu" SLASH_STR "\bu" "/commit" QUOTE_STR SLASH_GT,
													   image_info.proc_id,
													   image_info.image_id );
							Http_Server_Print_Dynamic( parser,
													   XML_STATUS_XLINK "\bu" SLASH_STR "\bu" "/status" QUOTE_STR SLASH_GT,
													   image_info.proc_id,
													   image_info.image_id );

							Http_Server_Print_Dynamic( parser, "\n</Image>" );
						}
						else
						{
							hams_respond_error( parser, 500U );
						}
					}
					else
					{
						hams_respond_error( parser, 405U );
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
											  "application/xml", 15U );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
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

uint32_t Session_cb( hams_parser* parser, const uint8_t* data, uint32_t length )
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
				hams_respond_error( parser, 401U );
			}
			else
			{
				/* There should be request body.. */
				if ( 0U == parser->content_length )
				{
					hams_respond_error( parser, 400U );
				}
				else
				{
					/* create a fresh session */
					if ( Firmware_Upgrade_Inprogress() == false )
					{
						g_fw_upgrade_user_id = rs->user_id;
						parser->user_data_cb = static_cast<hams_data_cb>( Session_Mgmt_cb );
						rs->p.rest.yxml = Http_Server_Alloc_Xml_Parser( parser, 32U );
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
			hams_respond_error( parser, 400U );
		}
		else
		{
			if ( Firmware_Upgrade_Inprogress() == true )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( Session_Mgmt_cb );
				rs->p.rest.yxml = Http_Server_Alloc_Xml_Parser( parser, 32U );
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
			hams_respond_error( parser, 413U );	/* Request Entity is too large */
		}
		else
		{
			if ( ( 0U == parser->cors_auth_common->origin_length ) ||
				 ( NULL == parser->cors_auth_common->origin_buffer ) )
			{
				hams_respond_error( parser, 400U );
			}
			else
			{
				if ( 0U != hams_check_headers_ended( data, length ) )
				{
					parser->response_code = 200U;
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				}
				else
				{
					if ( 0U != hams_can_respond( data, length ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
											  "application/xml", 15U );
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
		hams_respond_error( parser, 405U );
	}

	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Session_Mgmt_cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint32_t counter;
	uint16_t error_code = 200;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yxml_put_xml_state* ypx =
		reinterpret_cast<struct yxml_put_xml_state*>( rs->p.rest.yxml );
	yxml_t* yx = rs->p.rest.yxml;
	BF_FUS::session_info_resp_t session_resp = { 0U };
	BF_FUS::session_info_req_t session_req = {0U};
	BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( parser->content_type != HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED )
		{
			hams_respond_error( parser, 415U );
		}
		else
		{
			for ( counter = 0U; ( counter < length ) && ( error_code == 200U ); ++counter )
			{
				int32_t r = yxml_parse( yx, static_cast<int32_t>( data[counter] ) );
				switch ( r )
				{
					/* Expecting the proper tag element. */
					case YXML_ELEMSTART:
						if ( HTTP_METHOD_POST == parser->method )
						{
							if ( INIT_STATE == ypx->put_state )
							{
								if ( 0U !=
									 strcmp( yx->elem,
											 reinterpret_cast<const char_t*>( SESSION_TAG_DEF ) ) )
								{
									error_code = 400U;
								}
								else
								{
									ypx->put_state = SESSION_TAG_STATE;
								}
							}
							else
							{
								if ( SESSION_TAG_STATE == ypx->put_state )
								{
									if ( 0U !=
										 strcmp( yx->elem,
												 reinterpret_cast<const char_t*>( TIMEOUT_TAG_START ) ) )
									{
										error_code = 400U;
									}
									else
									{
										ypx->put_state = TIMEOUT_TAG_STATE;
										session_req.timeout_sec = 0U;
									}
								}
								else if ( ENCODE_TYPE == ypx->put_state )
								{
									if ( 0U !=
										 strcmp( yx->elem,
												 reinterpret_cast<const char_t*>( ENCODING_TYPE_TAG ) ) )
									{
										error_code = 400U;
									}
								}
								else
								{
									error_code = 0x4044U;
								}
							}
						}
						else if ( HTTP_METHOD_DELETE == parser->method )
						{
							if ( INIT_STATE == ypx->put_state )
							{
								if ( 0U != strcmp( yx->elem,
												   reinterpret_cast<const char_t*>( SESSION_TAG ) ) )
								{
									error_code = 400U;
								}
								else
								{
									ypx->put_state = DATA_RX_PREPARE_STATE;
								}
							}
						}
						else
						{
							error_code = 405U;	// MISRA Suppress
						}
						break;

					case YXML_CONTENT:
						if ( HTTP_METHOD_POST == parser->method )
						{
							if ( TIMEOUT_TAG_STATE == ypx->put_state )
							{
								if ( IS_DIGIT( yx->data[0] ) )
								{
									rs->p.fwop.session_timeout *= 10U;
									rs->p.fwop.session_timeout += ( yx->data[0] - 0x30 );
									if ( rs->p.fwop.session_timeout > MAX_SESSION_TIMEOUT_SEC )
									{
										error_code = E422_INVALID_RESOURCE_TIMEOUT;
									}
								}
								else
								{
									error_code = 400U;
								}
							}
							else if ( ENCODE_TYPE == ypx->put_state )
							{

								if ( IS_DIGIT( yx->data[0] ) )
								{
									g_encode_type *= 10U;
									g_encode_type += ( yx->data[0] - 0x30 );
									if ( g_encode_type > MAX_NUMBER_ENCODE )
									{
										error_code = E422_INVALID_ENCODE_TYPE;
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
						else
						{
							// MISRA Suppress
						}
						break;

					case YXML_ELEMEND:
						if ( HTTP_METHOD_POST == parser->method )
						{
							if ( TIMEOUT_TAG_STATE == ypx->put_state )
							{
								ypx->put_state = ENCODE_TYPE;
								g_encode_type = 0U;
							}
							else if ( ENCODE_TYPE == ypx->put_state )
							{
								if ( g_encode_type < MAX_NUMBER_ENCODE )
								{
									ypx->put_state = DATA_END_STATE;/* Here we have received full query */
								}
								else
								{
									error_code = E422_INVALID_ENCODE_TYPE;
								}
							}
							else if ( DATA_END_STATE == ypx->put_state )
							{
								if ( ( ( ( length - counter ) == 1U ) &&
									   ( data[counter] == 0x3EU ) ) ||
									 ( ( ( length - counter ) > 1U ) &&
									   ( data[counter] == 0x3EU ) &&
									   ( IS_SP_OR_HT( data[counter + 1] ) ||
										 IS_NEW_LINE( data[counter + 1] ) ) ) )
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
												Block_User_Session_Expiration( parser );
												parser->response_code = 201U;
												rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
												Http_Server_Begin_Dynamic( parser );
												Http_Server_Print_Dynamic( parser, SESSION_RESP_PREFIX );
												Http_Server_Print_Dynamic( parser,
																		   SESSION_ID_PREFIX "\bu" SESSION_ID_SUFFIX,
																		   fus_handle->Get_Session_Id() );
												Http_Server_Print_Dynamic( parser, WAIT_PREFIX "\bu" WAIT_SUFFIX,
																		   session_resp.wait_time );
												Http_Server_Print_Dynamic( parser, SESSION_RESP_SUFFIX );
											}
											else if ( BF_FUS::REJECT == return_status )
											{
												error_code = 403U;
											}
											else if ( BF_FUS::REQUEST_INVALID == return_status )
											{
												error_code = E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS;
											}
											else
											{
												error_code = 400U;
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
									error_code = 400U;
								}
							}
							else
							{
								// MISRA Suppress
							}
						}
						else if ( HTTP_METHOD_DELETE == parser->method )
						{
							if ( g_fw_upgrade_user_id != rs->user_id )
							{
								hams_respond_error( parser, 401U );
							}
							else
							{
								if ( SESSION_DELETE_REQUEST_COMPLETE == ypx->put_state )
								{
									session_req.session_id = rs->p.fwop.session_id;
									return_status = fus_handle->Exit_FUM( &session_req, &session_resp );
									if ( BF_FUS::SUCCESS == return_status )
									{
										Unblock_User_Session_Expiration();
										parser->response_code = 200U;
										rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
										Http_Server_Begin_Dynamic( parser );
										Http_Server_Print_Dynamic( parser,
																   SESSION_PREFIX );
										Http_Server_Print_Dynamic( parser,
																   WAIT_PREFIX "\bu" WAIT_SUFFIX,
																   session_resp.wait_time );
										Http_Server_Print_Dynamic( parser,
																   SESSION_SUFFIX );
									}
									else if ( BF_FUS::REQUEST_INVALID == return_status )
									{
										error_code = E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS;
									}
									else
									{
										error_code = 400U;
									}
								}
								else
								{
									error_code = 0x4044U;
								}
							}
						}
						else
						{
							error_code = 405U;
						}
						break;

					case YXML_ATTRSTART:
						if ( HTTP_METHOD_DELETE == parser->method )
						{
							if ( DATA_RX_PREPARE_STATE == ypx->put_state )
							{
								if ( 0U !=
									 strcmp( yx->attr,
											 reinterpret_cast<const char_t*>( SESSION_TAG_ATTR ) ) )
								{
									error_code = 400U;
								}
								else
								{
									ypx->put_state = DATA_RX_SESSION_ID;
									rs->p.fwop.session_id = 0U;
								}
							}
							else
							{
								error_code = 400U;
							}
						}
						break;

					case YXML_ATTREND:
						if ( HTTP_METHOD_DELETE == parser->method )
						{
							if ( DATA_RX_SESSION_ID != ypx->put_state )
							{
								error_code = 400U;
							}
							else
							{
								if ( fus_handle->Is_Session_Alive() != true )
								{
									error_code = E422_SESSION_NOT_EXIST;
								}
								else if ( rs->p.fwop.session_id != Firmware_Session_Id() )
								{
									error_code = E422_SESSION_ID_MISMATCH;
								}
								else
								{
									ypx->put_state = SESSION_DELETE_REQUEST_COMPLETE;
								}
							}
						}
						break;

					case YXML_ATTRVAL:
						if ( HTTP_METHOD_DELETE == parser->method )
						{
							if ( DATA_RX_SESSION_ID == ypx->put_state )
							{
								if ( IS_DIGIT( yx->data[0] ) )
								{
									rs->p.fwop.session_id *= 10U;
									rs->p.fwop.session_id += ( yx->data[0] - 0x30 );
									if ( ( rs->p.fwop.session_id > MAX_NUMBER_5_DIGIT ) ||
										 ( rs->p.fwop.session_id < 1 ) )
									{
										error_code = E422_SESSION_ID_MISMATCH;
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
						else
						{
							error_code = 400U;
						}
						break;

					case YXML_OK:
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
						if ( HTTP_METHOD_POST == parser->method )
						{
							Exit_firmware_Session();
							/* XML format is not proper */
						}
						else if ( HTTP_METHOD_DELETE == parser->method )
						{
							/* Session should not get deleted in case of xml error*/
						}
						error_code = E400_XML_SYNTAX_ERROR;
						break;

					case YXML_PISTART:

					case YXML_PICONTENT:
					case YXML_PIEND:

					default:
						break;
				}
				if ( 200U != error_code )
				{
					hams_respond_error( parser, error_code );
				}
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
										  "application/xml", 15U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
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

void Unblock_User_Session_Expiration( void )
{
	pre_pbin_in_progress = false;
	if ( fus_user_sess_id != User_Session::MAX_NUM_SESSIONS )
	{
		m_fw_upgrade_user_session_handler->Unblock_Session_Expiration( fus_user_sess_id );
		fus_user_sess_id = User_Session::MAX_NUM_SESSIONS;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Status_cb( hams_parser* parser, const uint8_t* data, uint32_t length )
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
				hams_respond_error( parser, 405U );
			}
			else
			{
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				parser->response_code = 200U;
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					if ( ( 0U == parser->cors_auth_common->origin_length ) ||
						 ( NULL == parser->cors_auth_common->origin_buffer ) )
					{
						hams_respond_error( parser, 400U );
					}
				}
				else
				{
					if ( ( Firmware_Upgrade_Inprogress() == true ) &&
						 ( g_fw_upgrade_user_id != rs->user_id ) )
					{
						hams_respond_error( parser, 401U );
					}
					else if ( ( Firmware_Upgrade_Inprogress() == false ) &&
							  ( 0U == rs->user_id ) )
					{
						hams_respond_error( parser, 401U );
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
								Http_Server_Print_Dynamic( parser, "<Status>\n\t<State>" );
								status_req.proc_index = rs->p.fwop.proc_id;
								status_req.image_index = rs->p.fwop.image_id;
								return_status = fus_handle->Status( &status_req, &status_resp );
								if ( BF_FUS::SUCCESS == return_status )
								{
									if ( status_resp.status == BF_FUS::FUS_COMMIT_DONE )
									{
										Http_Server_Print_Dynamic( parser, "Idle</State>\n\t<Wait>" );
										Http_Server_Print_Dynamic( parser, "\bu", status_resp.remaining_op_time );
										Http_Server_Print_Dynamic( parser, "</Wait>\n</Status>" );
										if ( ( ( g_fw_upgrade_user_id > 0U ) &&
											   ( g_fw_upgrade_user_id <= MAX_NUM_OF_AUTH_USERS ) ) &&
											 ( m_set_user_image_info_cb != nullptr ) )
										{
											m_set_user_image_info_cb( status_req.image_index, g_fw_upgrade_user_id );
										}
										BF_FUS::image_info_req_t image_req;
										image_req.proc_id = rs->p.fwop.proc_id;
										image_req.image_id = rs->p.fwop.image_id;
										BF_FUS::image_info_resp_t image_resp;
										if ( BF_FUS::SUCCESS == fus_handle->FUS::Image( &image_req, &image_resp ) )
										{
											FUS_Update_Audit_logs( image_req.image_id, image_resp.version );
										}
									}
									else if ( status_resp.status == BF_FUS::FUS_PENDING_CALL_BACK )
									{
										Http_Server_Print_Dynamic( parser, "Busy</State>\n\t<Wait>" );
										Http_Server_Print_Dynamic( parser, "\bu", status_resp.remaining_op_time );
										Http_Server_Print_Dynamic( parser, "</Wait>\n</Status>" );
									}
									else
									{
										Http_Server_Print_Dynamic( parser, "Error</State>\n\t<Wait>" );
										Http_Server_Print_Dynamic( parser, "\bu", status_resp.remaining_op_time );
										Http_Server_Print_Dynamic( parser, "</Wait>\n</Status>" );
									}
								}
								else if ( BF_FUS::REQUEST_INVALID == return_status )
								{
									hams_respond_error( parser, 404U );
								}
								else
								{
									hams_respond_error( parser, 400U );
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
										  "application/xml", 15U );
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
void Fw_Upgrade_Session_Init_Handler( User_Session* user_session )
{
	m_fw_upgrade_user_session_handler = user_session;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Block_User_Session_Expiration( hams_parser* parser )
{
	pre_pbin_in_progress = false;
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Validate_cb( hams_parser* parser, const uint8_t* data, uint32_t length )
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
				hams_respond_error( parser, 405U );
			}
			else
			{
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				parser->response_code = 200U;
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					if ( ( 0U == parser->cors_auth_common->origin_length ) ||
						 ( NULL == parser->cors_auth_common->origin_buffer ) )
					{
						hams_respond_error( parser, 400U );
					}
				}
				else
				{
					if ( ( Firmware_Upgrade_Inprogress() == true ) &&
						 ( g_fw_upgrade_user_id != rs->user_id ) )
					{
						hams_respond_error( parser, 401U );
					}
					else if ( ( Firmware_Upgrade_Inprogress() == false ) &&
							  ( 0U == rs->user_id ) )
					{
						hams_respond_error( parser, 401U );
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

							Http_Server_Print_Dynamic( parser, "<Validate>\n\t<Integrity>" );
							return_status = fus_handle->Validate( &validate_req, &validate_resp );
							if ( BF_FUS::SUCCESS == return_status )
							{
								if ( validate_resp.status == BF_FUS::FUS_VALIDATION_IN_PROGRESS )
								{
									Http_Server_Print_Dynamic( parser,
															   "Checking</Integrity>\n\t<Wait>" );
								}
								else if ( validate_resp.status == BF_FUS::FUS_VALID_IMAGE )
								{
									Http_Server_Print_Dynamic( parser,
															   "Valid</Integrity>\n\t<Wait>" );
								}
								else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_CHECKSUM_FAILED )
								{
									Http_Server_Print_Dynamic( parser, "Invalid</Integrity>\n\t<Wait>" );
									if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
									{
										( *m_fw_upgrade_log_audit_capture_cb )( LOG_EVENT_CHECKSUM_FAILURE );
									}
								}
								else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_SIGNATURE_FAILED )
								{
									Http_Server_Print_Dynamic( parser, "Invalid</Integrity>\n\t<Wait>" );
									if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
									{
										( *m_fw_upgrade_log_audit_capture_cb )( LOG_EVENT_SIGNATURE_VERIFICATION_FAILURE );
									}
								}
								else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_PRODUCT_CODE_FAILED )
								{
									Http_Server_Print_Dynamic( parser, "Invalid</Integrity>\n\t<Wait>" );
									// ToDo: Not defined log event code for product code failure.
									// if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
									// {
									// ( *m_fw_upgrade_log_audit_capture_cb )( LOG_EVENT_PRODUCT_CODE_FAILURE );
									// }
								}
								else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_VERSION_ROLLBACK_FAILED )
								{
									Http_Server_Print_Dynamic( parser, "Invalid</Integrity>\n\t<Wait>" );
									if ( m_fw_upgrade_log_audit_capture_cb != nullptr )
									{
										( *m_fw_upgrade_log_audit_capture_cb )( LOG_EVENT_VERSION_ROLLBACK_FAILURE );
									}
								}

								Http_Server_Print_Dynamic( parser, "\bu", validate_resp.op_time );
								Http_Server_Print_Dynamic( parser,
														   "</Wait>\n</Validate>" );
							}
							else if ( BF_FUS::REQUEST_INVALID == return_status )
							{
								hams_respond_error( parser, 404U );
							}
							else
							{
								hams_respond_error( parser, 400U );
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
										  "application/xml", 15U );
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
uint32_t Commit_cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();

	if ( HTTP_METHOD_POST == parser->method )
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( rs->auth_level < FW_MIN_ACCESS_LEVEL )
			{
				hams_respond_error( parser, 401U );
			}
			else
			{
				/* There should be request body.. */
				if ( 0U == parser->content_length )
				{
					hams_respond_error( parser, 400U );
				}
				else
				{
					/* create a fresh session */
					if ( Firmware_Upgrade_Inprogress() == true )
					{
						rs->p.fwop.session_id = 0U;
						parser->user_data_cb = static_cast<hams_data_cb>( Commit_Mgmt_cb );
						rs->p.rest.yxml = Http_Server_Alloc_Xml_Parser( parser, 32U );
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
			hams_respond_error( parser, 413U );	/* Request Entity is too large */
		}
		else
		{
			if ( ( 0U == parser->cors_auth_common->origin_length ) ||
				 ( NULL == parser->cors_auth_common->origin_buffer ) )
			{
				hams_respond_error( parser, 400U );
			}
			else
			{
				if ( 0U != hams_check_headers_ended( data, length ) )
				{
					parser->response_code = 200U;
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				}
				else
				{
					if ( 0U != hams_can_respond( data, length ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
											  "application/xml", 15U );
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
		hams_respond_error( parser, 405U );
	}

	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Commit_Mgmt_cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint32_t counter;
	uint16_t error_code = 200;
	BF_FUS::image_commit_resp_t commit_resp = { };
	BF_FUS::image_commit_req_t commit_req = { };
	BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;

	commit_req.session_id = 0U;

	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yxml_put_xml_state* ypx =
		reinterpret_cast<struct yxml_put_xml_state*>( rs->p.rest.yxml );
	yxml_t* yx = rs->p.rest.yxml;

	commit_req.comp_id = rs->p.fwop.proc_id;
	commit_req.image_id = rs->p.fwop.image_id;

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( parser->content_type != HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED )
		{
			hams_respond_error( parser, 415U );
		}
		else
		{
			for ( counter = 0U; ( counter < length ) && ( error_code == 200U ); ++counter )
			{
				int32_t r = yxml_parse( yx, static_cast<int32_t>( data[counter] ) );
				switch ( r )
				{
					/* Expecting the proper tag element. */
					case YXML_ELEMSTART:
						if ( HTTP_METHOD_POST == parser->method )
						{
							if ( INIT_STATE == ypx->put_state )
							{
								if ( 0U !=
									 strcmp( yx->elem,
											 reinterpret_cast<const char_t*>( COMMIT_TAG_DEF ) ) )
								{
									error_code = 400U;
								}
								else
								{
									ypx->put_state = SESSION_TAG_STATE;
									rs->p.fwop.session_id = 0;
								}
							}
							else
							{
								if ( SESSION_TAG_STATE == ypx->put_state )
								{
									if ( 0U !=
										 strcmp( yx->elem,
												 reinterpret_cast<const char_t*>( SESSION_ID_TAG ) ) )
									{
										error_code = 400U;
									}
									else
									{
										ypx->put_state = TIMEOUT_TAG_STATE;
									}
								}
								else
								{
									error_code = 0x4044U;
								}
							}
						}
						else
						{
							error_code = 503U;
						}
						break;

					case YXML_CONTENT:
						if ( HTTP_METHOD_POST == parser->method )
						{
							if ( TIMEOUT_TAG_STATE == ypx->put_state )
							{
								if ( IS_DIGIT( yx->data[0] ) )
								{
									rs->p.fwop.session_id *= 10U;
									rs->p.fwop.session_id += ( yx->data[0] - 0x30 );
									if ( ( rs->p.fwop.session_id > MAX_NUMBER_5_DIGIT ) ||
										 ( rs->p.fwop.session_id < 1 ) )
									{
										error_code = E422_SESSION_ID_MISMATCH;
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
						else
						{}
						break;

					case YXML_ELEMEND:
						if ( HTTP_METHOD_POST == parser->method )
						{
							if ( TIMEOUT_TAG_STATE == ypx->put_state )
							{
								ypx->put_state = DATA_END_STATE;
							}
							else if ( DATA_END_STATE == ypx->put_state )
							{
								if ( ( ( ( length - counter ) == 1U ) &&
									   ( data[counter] == 0x3EU ) ) ||
									 ( ( ( length - counter ) > 1U ) &&
									   ( data[counter] == 0x3EU ) &&
									   ( IS_SP_OR_HT( data[counter + 1] ) ||
										 IS_NEW_LINE( data[counter + 1] ) ) ) )
								{
									commit_req.session_id = rs->p.fwop.session_id;
									return_status = fus_handle->Commit( &commit_req, &commit_resp );
									if ( BF_FUS::SUCCESS == return_status )
									{
										parser->response_code = 200U;
										rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
										Http_Server_Begin_Dynamic( parser );
										Http_Server_Print_Dynamic( parser, "<CommitResp>\n" );
										Http_Server_Print_Dynamic( parser, WAIT_PREFIX "\bu" WAIT_SUFFIX,
																   commit_resp.op_time );
										Http_Server_Print_Dynamic( parser, "</CommitResp>" );
									}
									else if ( BF_FUS::REJECT == return_status )
									{
										error_code = E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS;
									}
									else if ( BF_FUS::REQUEST_INVALID == return_status )
									{
										error_code = 404U;
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
							else
							{
								error_code = E400_CONTENT_LENGTH_OVERLONG;
							}
						}
						else
						{
							error_code = 503U;
						}
						break;

					case YXML_ATTRSTART:
						break;

					case YXML_ATTREND:
						break;

					case YXML_ATTRVAL:
						break;

					case YXML_OK:
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

					case YXML_PISTART:

					case YXML_PICONTENT:
					case YXML_PIEND:

					default:
						break;
				}
				if ( 200U != error_code )
				{
					hams_respond_error( parser, error_code );
				}
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
										  "application/xml", 15U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
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
bool Firmware_Upgrade_Inprogress( void )
{
	bool return_status = false;

	if ( nullptr != fus_handle )
	{
		return_status = fus_handle->Is_Session_Alive();
	}
	else
	{
		fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
		return_status = fus_handle->Is_Session_Alive();
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Exit_firmware_Session( void )
{
	fus_handle->Delete_Session();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Firmware_Session_Id( void )
{
	return ( fus_handle->Get_Session_Id() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Allocate_Rest_FUS_Buffer( void )
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
void Deallocate_Rest_FUS_Buffer( void )
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
void FUS_Update_Audit_logs( uint8_t image_id, uint32_t firm_ver )
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
