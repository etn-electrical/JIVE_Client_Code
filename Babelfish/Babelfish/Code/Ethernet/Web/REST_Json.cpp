/**
 *****************************************************************************************
 * @file REST_Json.cpp
 * @details See header file for module overview.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "httpd_rest_dci.h"
#include "httpd_xml.h"
#include "httpd_common.h"
#include "BIN_ASCII_Conversion.h"
#include "ASCII_Conversion.h"
#include "REST_DCI_Data.h"
#include "REST.h"
#include "REST_Log_Json.h"
#include "Log_Config.h"
#include "Format_handler.h"
#include "stdio.h"
#include "StdLib_MV.h"
#include "Web_Debug.h"
#include "REST_Common.h"
#include "Babelfish_Assert.h"
#include "server.h"
#include "REST_Json.h"
#include "iot_json_parser.h"
#include "Ram.h"
#include "hams.h"
#include "Assy_Json.h"
#include "Yjson.h"

static bool g_flag = false;
static bool from_rest = false;

#define HAMS_DEBUG
#ifdef HAMS_DEBUG
// #define DERR(...) fprintf(stderr, __VA_ARGS__)
#define DERR( ... )
#define _ASSERT( x ) do {if ( !( x ) ){while ( 1 ){;} \
						 } \
} while ( 0 )
#else
#define DERR( ... )
#define _ASSERT( x )
#endif

#define Param_RO   "GET, HEAD, OPTIONS"
#define Param_W    "GET, HEAD, PUT, OPTIONS"

extern uint32_t S_Xmit_Flash_Data( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Prod_Info_Spec_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Parse_Fw_Resources_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Assy_Get_All_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Assy_Index_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Parse_UserMgmt_Resources_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Get_Cert_Store_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static const char_t* const s_rest_param_types[COUNT_REST_PARAMS] =
{
	"/",
	"/default",
	"/description",
	"/enum",
	"/initial",
	"/presentval",
	"/range",
	"/value",
	"/bitfield"
};

static const char_t* const s_rest_param_json_tags[COUNT_REST_PARAMS] =
{
	static_cast<const char_t*>( NULL ),
	"Default",
	"Description",
	"Enum",
	"Initial",
	"PresentVal",
	"Range",
	"Value",
	"Bitfield"
};

static const uint8_t REST_SPEC_VER_MAJOR = 4U;
static const uint8_t REST_SPEC_VER_MINOR = 0U;
static const uint8_t REST_SPEC_VER_REV = 18U;

static uint8_t number_charArr[MAX_NUMBER_STRING_LENGTH] = { 0U };

#define DEST_BUFFER_SIZE ( REST_DCI_MAX_LENGTH > \
						   MIN_DEST_BUFFER_SIZE ) ? ( REST_DCI_MAX_LENGTH + 1 ) : MIN_DEST_BUFFER_SIZE

uint8_t j_dest_data[DEST_BUFFER_SIZE];

uint16_t Print_Rest_Format_Json( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
								 uint8_t* data, uint32_t tot_length, uint32_t offset );

bool Format_Handler_Check_Json( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
								uint8_t* dest_data, uint32_t length, uint32_t offset );

static bool S_Dynamic_Write_Value_Json( hams_parser* parser, UINT8 type, const UINT8* pData,
										UINT32 offset );

static uint32_t S_Rest_Device_All_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t S_Rest_Param_Index_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static bool Get_Json_Param_Description( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI );

static bool Get_Json_Param_Range( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI );

static bool Get_Json_Param_Enum( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI );

static bool Get_Json_Param_Bitlist( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI );

static uint32_t Get_Enum_Json_All_Remaining( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Get_Enum_Description_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Get_Bitfield_Description_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t S_Rest_Param_List_End_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t S_Rest_Device_Index_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t S_Rest_Param_All_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t S_Rest_Other_Handler_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t S_Rest_Root_Hdrs_End_Json( hams_parser* parser, const uint8_t* data, unsigned length )
{
	char_t verVal[BUFFER_LEN];

	/* There should not be any request body.. */
	if ( 0U != ( parser->content_length ) )
	{
		hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
	}
	else
	{
		parser->response_code = RESPONSE_OK;
		http_server_req_state* rs = Http_Server_Get_State( parser );
		rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
		Http_Server_Begin_Dynamic( parser );

		json_response_t* json_buffer = reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

		if ( json_buffer != nullptr )
		{
			json_buffer->used_buff_len = 0U;
			memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, "RS", NULL, false );
			Open_Json_Object( json_buffer );
			memset( verVal, 0U, BUFFER_LEN );
			snprintf( verVal, BUFFER_LEN, "%d.%d.%d", REST_SPEC_VER_MAJOR, REST_SPEC_VER_MINOR,
					  REST_SPEC_VER_REV );
			Add_String_Member( json_buffer, "RESTSpecVer", verVal, true );
			Add_String_Member( json_buffer, HREF, "/rs", true );
			Add_String_Member( json_buffer, "DeviceList", NULL, false );
			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, HREF, "/rs/device", false );
			Close_Json_Object( json_buffer, true );
			Add_String_Member( json_buffer, "ParamList", NULL, false );
			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, HREF, "/rs/param", false );
			Close_Json_Object( json_buffer, true );
			Add_String_Member( json_buffer, "AssyList", NULL, false );
			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, HREF, "/rs/assy", false );
			Close_Json_Object( json_buffer, true );
			Add_String_Member( json_buffer, "FwList", NULL, false );
			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, HREF, "/rs/fw", false );
			Close_Json_Object( json_buffer, true );
			Add_String_Member( json_buffer, "UsersList", NULL, false );
			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, HREF, "/rs/users", false );
			Close_Json_Object( json_buffer, true );
			Add_String_Member( json_buffer, "LogList", NULL, false );
			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, HREF, "/rs/log", false );
			Close_Json_Object( json_buffer, true );
			Add_String_Member( json_buffer, "Langlist", NULL, false );
			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, HREF, "/rs/lang", false );
			Multiple_Close_Object( json_buffer, 3U );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
			Ram::De_Allocate( json_buffer );
		}

		if ( 0U != Http_Server_End_Dynamic( parser ) )
		{
			hams_response_header( parser, static_cast<uint32_t>( HTTP_HDR_CONTENT_TYPE ),
								  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );

			hams_response_header( parser, static_cast<uint32_t>( COUNT_HTTP_HDRS ),
								  reinterpret_cast<void*>( NULL ), 0U );
		}

	}
	return ( 0U );
}

/**
 * @brief This function is for /rs/device request
 * @details This function is called on /rs/device request request, For "GET /rs/device" request it returns device list
 * in response
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return Success or failure of requested operation.
 */
static uint32_t S_Rest_Device_All_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint8_t return_val = 0U;

	if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
		 ( HTTP_METHOD_OPTIONS != parser->method ) )
	{
		hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
		return_val = 0U;
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
			else if ( rs->max_auth_exempt_level != 0U )
			{
				if ( 0U != parser->content_length )
				{
					hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
					return_val = 0U;
				}
				else
				{
					UINT16 tot_length = 0U;
					if ( DCI_ERR_NO_ERROR ==
						 get_dcid_ram_data( DCI_PRODUCT_NAME_DCID,
											&j_dest_data, &tot_length, 0U ) )
					{
						parser->response_code = RESPONSE_OK;
						rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
						Http_Server_Begin_Dynamic( parser );

						json_response_t* json_buffer =
							reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

						if ( json_buffer != nullptr )
						{
							json_buffer->used_buff_len = 0;
							memset( json_buffer->json_buff, 0, JSON_MAXBUFF_LEN );
							Open_Json_Object( json_buffer );
							Add_String_Member( json_buffer, "DeviceList", NULL, false );
							Open_Json_Object( json_buffer );
							Add_String_Member( json_buffer, HREF, "/rs/device", true );
							Add_String_Member( json_buffer, "Device", NULL, false );
							Open_Json_Object( json_buffer );
							Add_String_Member( json_buffer, "dev_id", "0", true );
							Add_String_Member( json_buffer, "name", ( char_t const* )j_dest_data, true );
							Add_String_Member( json_buffer, HREF, "/rs/device/0", false );
							Multiple_Close_Object( json_buffer, 3U );
							Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
							Ram::De_Allocate( json_buffer );
						}

						return_val = 1U;
					}
					else
					{
						hams_respond_error( parser, ERROR_INTERNAL_SERVER );
						return_val = 0U;
					}
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_UNAUTHORIZED );
			}
		}
		else
		{
			return_val = 1U;
			// _ASSERT( hams_can_respond(data, length) );
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				hams_response_header( parser, HTTP_HDR_ALLOW, Param_RO, STRLEN( Param_RO ) );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else
			{
				if ( 0U != Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );

					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
		}
	}

	return ( return_val );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t S_Rest_Param_Value_Put_Parse_Json( hams_parser* parser,
											const uint8_t* data, uint32_t length )
{
	bool loop_exit = false;
	bool function_end = false;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yjson_put_json_state* ypx =
		reinterpret_cast<struct yjson_put_json_state*>( rs->p.rest.yjson );

	ReusedRecvBuf* pTarget = static_cast<ReusedRecvBuf*>( rs->p.rest.put_request );
	FormatBuf* format_buff = reinterpret_cast<FormatBuf*>( static_cast<uint8_t*>( rs->p.rest.put_request )
														   + sizeof( ReusedRecvBuf ) );
	bool parse_complete = false;
	jsmn_parser j_parser;
	int16_t result = 0;
	static int16_t cp_result = 0;
	uint16_t index = 0;
	jsmntok_t tokens[BUFFER_LEN];
	static char_t* p_msg_buffer = nullptr;
	static uint16_t msg_cnt = 1;
	static char_t json_value[JSON_VALUE_LEN];

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
			static_cast<const DCI_REST_TO_DCID_LKUP_ST_TD*>( rs->p.rest.ptr );
		const DCI_DATA_BLOCK_TD* data_block = DCI::Get_Data_Block( pDCI->dcid );

		uint32_t error_500 = 0U;

		if ( msg_cnt == 1 )
		{
			p_msg_buffer = reinterpret_cast<char_t*>( Ram::Allocate( JSON_MAXBUFF_LEN ) );

			if ( p_msg_buffer != nullptr )
			{
				memset( p_msg_buffer, 0, JSON_MAXBUFF_LEN );
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
			jsmn_parse( &j_parser, ( char_t* ) p_msg_buffer, msgLen, tokens, sizeof( tokens ) / sizeof( tokens[0] ) );

		if ( result < 0 )
		{
			if ( result == -3 )
			{
				/* Invalid Json string, More bytes expected */
				cp_result = result;
				msg_cnt++;
				return ( 0U );
			}
			else
			{
				/* Failed to parse param parameter  */
				error_500 = E400_JSON_SYNTAX_ERROR;
				loop_exit = true;
			}
		}

		for ( index = 1; ( ( index < result ) && ( loop_exit == false ) && ( function_end == false ) ); index++ )
		{
			size_t token_size = tokens[index + 1].end - tokens[index + 1].start;

			if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
						 ( s_rest_param_json_tags[rs->p.rest.rest_param] ) ) == 0 )
			{
				if ( pDCI->pFormatData == nullptr )
				{
					ypx->tmp_number = 0U;
					ypx->val_index = 0U;
					if ( ypx->separator == 0x3AU )
					{
						// :
						ypx->flags |= YJSON_PUT_IS_HEX;
						ypx->put_state = 1U;
					}
					else
					{
						ypx->put_state = 0U;
					}
				}
				else
				{
					/* Reset format buffer index. */
					format_buff->byte_index = 0;
				}
			}
			else if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index], JSON_KEY_TEXT ) == 0 )
			{
				uint16_t k = 0;
				memset( json_value, 0, JSON_VALUE_LEN );
				strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );

				if ( ( pDCI->pFormatData == nullptr ) || ( pDCI->pFormatData->format_name != nullptr
#ifdef REST_SERVER_FORMAT_HANDLING
														   && pDCI->pFormatData->formatter_cback == nullptr
#endif
														   ) )
				{
					const char_t* c = json_value;
					while ( ( 0x00U != static_cast<uint8_t>( *c ) ) && ( false == loop_exit ) )
					{
						if ( DCI_DTYPE_STRING8 == data_block->datatype )
						{
							if ( 0U == ypx->put_state )
							{
								++ypx->put_state;
								PTARGET_SET_DATA_PRESENT( pTarget->present, 0U );
								ypx->base64_bits = 0U;
								ypx->base64.all = 0U;
							}

							if ( 0U != ( ypx->flags & YJSON_PUT_END ) )
							{
								error_500 = ERROR_BAD_REQUEST;
								loop_exit = true;
							}
							else
							{
								if ( 0U != ( YJSON_PUT_IS_BASE64 & ypx->flags ) )
								{

									uint8_t val = 0x80U;
									if ( ( ( static_cast<uint8_t>( *c ) >= 0x41U ) &&
										   ( static_cast<uint8_t>( *c ) <= 0x5AU ) ) )
									{
										// A...Z
										val = static_cast<uint8_t>( *c ) - 0x41U;
									}
									else if ( ( ( static_cast<uint8_t>( *c ) >= 0x61U ) &&
												( static_cast<uint8_t>( *c ) <= 0x7AU ) ) )
									{
										// a..z
										val = ( static_cast<uint8_t>( *c ) - 0x61U ) + 26U;
									}
									else if ( ( ( static_cast<uint8_t>( *c ) >= 0x30U ) &&
												( static_cast<uint8_t>( *c ) <= 0x39U ) ) )
									{
										val = ( static_cast<uint8_t>( *c ) - 0x30U ) + 52U;
									}
									else if ( 0x2BU == static_cast<uint8_t>( *c ) )
									{
										// +
										val = 62U;
									}
									else if ( 0x2FU == static_cast<uint8_t>( *c ) )
									{
										///
										val = 63U;
									}
									else if ( 0x3DU == static_cast<uint8_t>( *c ) )
									{
										// =
										// No-op
										ypx->flags |= YJSON_PUT_BASE64_PAD;
										if ( 2U == ypx->base64_bits )
										{
											/* Don't overflow the buffer. */
											if ( ypx->val_index >= *data_block->length_ptr )
											{
												error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
												loop_exit = true;
											}
											else
											{
												uint32_t compose =
													( ( static_cast<uint32_t>
														( ypx->base64.buff[0] ) )
														<< 18 )
													| ( ( static_cast<uint32_t>( ypx->base64
																				 .buff[1] )
														  << 12 ) );
												ypx->base64.all = compose;

												pTarget->buf.u8_data[ypx->val_index] =
													ypx->base64.buff[2];
												ypx->val_index++;
											}
										}
										else if ( 3U == ypx->base64_bits )
										{
											/* Don't overflow the buffer. */
											if ( ( ypx->val_index + 1U ) >=
												 static_cast<uint8_t>
												 ( *data_block->length_ptr ) )
											{
												error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
												loop_exit = true;
											}
											else
											{
												uint32_t compose =
													( static_cast<uint32_t>
													  ( ypx->base64.buff[0] )
														<< 18 )
													| ( static_cast<uint32_t>( ypx->base64
																			   .buff[1] ) <<
														12 )
													| ( static_cast<uint32_t>( ypx->base64
																			   .buff[2] ) <<
														6 );
												ypx->base64.all = compose;

												pTarget->buf.u8_data[ypx->val_index] =
													ypx->base64.buff[2];
												ypx->val_index++;
												pTarget->buf.u8_data[ypx->val_index] =
													ypx->base64.buff[1];
												ypx->val_index++;
											}
										}
										else
										{
											// MISRA Suppress
										}
										if ( false == loop_exit )
										{
											ypx->base64_bits = 0U;
										}
									}
									else if ( 0x22U == static_cast<uint8_t>( *c ) )
									{
										// '"'
										/* Found the end without any padding..
										 * must not have any further characters.*/
										ypx->flags |= YJSON_PUT_END;
									}
									else
									{
										error_500 = ERROR_BAD_REQUEST;
										loop_exit = true;
									}

									if ( ( val < 0x80U ) && ( false == loop_exit ) )
									{
										/* Cannot have more B64 chars after pad appears.*/
										if ( 0U != ( ypx->flags & YJSON_PUT_BASE64_PAD ) )
										{
											error_500 = ERROR_BAD_REQUEST;
											loop_exit = true;
										}
										else
										{

											ypx->base64.buff[ypx->base64_bits] =
												static_cast<char_t>( val );
											++ypx->base64_bits;
											if ( 4U == ypx->base64_bits )
											{
												if ( ( ypx->val_index + 2U ) >=
													 static_cast<uint8_t>( *data_block
																		   ->length_ptr ) )
												{
													error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
													loop_exit = true;
												}
												else
												{
													uint32_t compose =
														( static_cast<uint32_t>( ypx->base64
																				 .buff[0] )
															<< 18 )
														| ( static_cast<uint32_t>( ypx
																				   ->base64.
																				   buff[1] )
															<< 12 )
														| ( static_cast<uint32_t>( ypx
																				   ->base64.
																				   buff[2] )
															<< 6 )
														| ( static_cast<uint32_t>( ypx
																				   ->base64.
																				   buff[3] )
															<< 0 );
													ypx->base64.all = compose;

													pTarget->buf.u8_data[ypx->val_index]
														=
															ypx->base64.buff[2];
													ypx->val_index++;
													pTarget->buf.u8_data[ypx->val_index]
														=
															ypx->base64.buff[1];
													ypx->val_index++;
													pTarget->buf.u8_data[ypx->val_index]
														=
															ypx->base64.buff[0];
													ypx->val_index++;
													ypx->base64_bits = 0U;
													ypx->base64.all = 0U;
												}
											}
										}
									}
								}
								else
								{
									if ( ypx->val_index >= *data_block->length_ptr )
									{
										error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
										loop_exit = true;
									}
									else
									{
										/* Append character data to */
										pTarget->buf.u8_data[ypx->val_index] =
											static_cast<uint8_t>( *c );
										ypx->val_index++;
									}
								}
							}
						}
						else
						{
							/* This is the branch for handling numeric values. */
							if ( 0U == ypx->put_state )
							{
								if ( ypx->curr_num_list < ypx->max_num_list )
								{
									PTARGET_SET_DATA_PRESENT( pTarget->present,
															  ( ypx->val_index
																+ ( ypx->num_of_units *
																	ypx->curr_num_list ) ) );
									if ( 0x30U != static_cast<uint8_t>( *c ) )
									{
										if ( ( 0x58U == static_cast<uint8_t>( *c ) ) ||
											 ( 0x78U == static_cast<uint8_t>( *c ) ) )
										{
											/* x.. X */
											if ( 0U == ( ypx->flags & YJSON_PUT_HAS_LEADING_ZERO ) )
											{
												error_500 = ERROR_BAD_REQUEST;
												loop_exit = true;
											}
											else
											{
												/* Clear leading zero flag. */
												ypx->flags = YJSON_PUT_IS_HEX;
												++ypx->put_state;
											}
										}
										else if ( 0x2DU == static_cast<uint8_t>( *c ) )
										{
											// -
											/* No flags should be set. */
											if ( ypx->flags )
											{
												error_500 = ERROR_BAD_REQUEST;
												loop_exit = true;
											}
											else
											{
												ypx->flags |= YJSON_PUT_IS_NEGATIVE;
												++ypx->put_state;
											}
										}
										else if ( ( 0x31U <= static_cast<uint8_t>( *c ) ) &&
												  ( static_cast<uint8_t>( *c ) <= 0x39U ) )
										{
											// 1..9
											ypx->tmp_number = static_cast<uint8_t>
												( *c ) - 0x30U;
											// 0
											++ypx->put_state;
										}
										else if ( ( static_cast<uint8_t>( *c ) ==
													ypx->separator ) ||
												  ( ( static_cast<uint8_t>( *c ) ==
													  GLOBAL_ARR_SEPARATOR ) &&
													( ypx->max_num_list > ypx->curr_num_list ) &&
													( ( ypx->val_index + 1 ) ==
													  ypx->num_of_units ) ) )
										{
											/* Check if this is just zero value. */
											if ( YJSON_PUT_HAS_LEADING_ZERO == ypx->flags )
											{
												/* This just zero, save it. */
												ypx->tmp_number = 0U;
												int32_t ret =
													Set_Param_Numeric_Json( pTarget, ypx,
																			data_block->datatype );
												if ( -1 == ret )
												{
													error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
													loop_exit = true;
												}
												else if ( ret < 0 )
												{
													error_500 = ERROR_BAD_REQUEST;
													loop_exit = true;
												}
												else
												{
													++ypx->val_index;

													if ( ( static_cast<uint8_t>( *c ) ==
														   GLOBAL_ARR_SEPARATOR ) &&
														 ( ypx->max_num_list > 1 ) )
													{
														ypx->val_index = 0;
														ypx->curr_num_list++;
													}

													if ( ypx->separator == 0x3AU )												/*
																																   :
																																 */
													{
														ypx->flags = YJSON_PUT_IS_HEX;
													}
													else
													{
														ypx->flags = 0U;
														ypx->put_state = 0U;
													}
												}
											}
											else
											{
												if ( ( static_cast<uint8_t>( *c ) ==
													   GLOBAL_ARR_SEPARATOR ) &&
													 ( ypx->max_num_list > 1 ) )
												{
													ypx->val_index = 0;
													ypx->curr_num_list++;
												}
												/* No flags should be set. */
												if ( 0U != ypx->flags )
												{
													error_500 = ERROR_BAD_REQUEST;
													loop_exit = true;
												}
												else
												{
													/* empty value. */
													PTARGET_CLEAR_DATA_PRESENT( pTarget->present,
																				( ypx->val_index
																				  + ( ypx->num_of_units
																					  * ypx->curr_num_list ) ) );
													++ypx->val_index;
												}
											}
										}
										else
										{
											error_500 = ERROR_BAD_REQUEST;
											loop_exit = true;
										}
									}
									else
									{
										ypx->flags |= YJSON_PUT_HAS_LEADING_ZERO;
									}
								}
								else
								{
									error_500 = ERROR_BAD_REQUEST;
									loop_exit = true;
								}
							}
							else
							{
								PTARGET_SET_DATA_PRESENT( pTarget->present,
														  ( ypx->val_index +
															( ypx->num_of_units * ypx->curr_num_list ) ) );
								if ( ( static_cast<uint8_t>( *c ) == ypx->separator ) ||
									 ( ( static_cast<uint8_t>( *c ) == GLOBAL_ARR_SEPARATOR ) &&
									   ( ypx->max_num_list > ypx->curr_num_list ) &&
									   ( ( ypx->val_index + 1 ) == ypx->num_of_units ) ) )
								{
									if ( ( 0U != ypx->tmp_number ) ||
										 ( ypx->flags & YJSON_PUT_HAS_LEADING_ZERO ) )
									{
										/* Save the number. */
										int32_t ret =
											Set_Param_Numeric_Json( pTarget, ypx,
																	data_block->datatype );
										if ( -1 == ret )
										{
											error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
											loop_exit = true;
										}
										else if ( ret < 0 )
										{
											error_500 = ERROR_BAD_REQUEST;
											loop_exit = true;
										}
										else
										{
											++ypx->val_index;

											if ( ( static_cast<uint8_t>( *c ) ==
												   GLOBAL_ARR_SEPARATOR ) &&
												 ( ypx->max_num_list > 1 ) )
											{
												ypx->val_index = 0;
												ypx->curr_num_list++;
											}
											/* TODO limit check on number of units... */

											/* go onto next value. */
											ypx->tmp_number = 0U;
											/* If parsing a MAC address, basically assume we already read the next 0x
											   (MAC address excludes hex prefix). */
											if ( ypx->separator == 0x3AU )										/* : */
											{
												ypx->flags = YJSON_PUT_IS_HEX;
											}
											else
											{
												ypx->flags = 0U;
												ypx->put_state = 0U;
											}
										}
									}
									else
									{
										++ypx->val_index;
										if ( ( static_cast<uint8_t>( *c ) ==
											   GLOBAL_ARR_SEPARATOR ) &&
											 ( ypx->max_num_list > 1 ) )
										{
											ypx->val_index = 0;
											ypx->curr_num_list++;
										}
									}
								}
								else
								{
									uint8_t v = static_cast<uint8_t>( *c );
									if ( v < 0x30U )
									{
										// 0
										error_500 = ERROR_BAD_REQUEST;
										loop_exit = true;
									}
									else if ( v > 0x39U )
									{
										// 9
										if ( ypx->flags & YJSON_PUT_IS_HEX )
										{
											if ( ( ( v >= 0x61U ) && ( v <= 0x66U ) ) )
											{
												// a..f
												v -= ( 0x61U - 10U );
											}
											else if ( ( v >= 0x41U ) && ( v <= 0x46U ) )
											{
												// A..F
												v -= ( 0x41U - 10U );
											}
											else
											{
												error_500 = ERROR_BAD_REQUEST;
												loop_exit = true;
											}
										}
										else
										{
											error_500 = ERROR_BAD_REQUEST;
											loop_exit = true;
										}
									}
									else
									{
										v -= 0x30U;
										// 0
									}
									if ( false == loop_exit )
									{
										if ( ypx->flags & YJSON_PUT_IS_HEX )
										{
											ypx->tmp_number <<= 4;
											ypx->tmp_number |= v;
										}
										else
										{
											ypx->tmp_number *= 10U;
											ypx->tmp_number += v;
										}
									}
								}
							}
						}
						if ( false == loop_exit )
						{
							++c;
						}
					}
				}
				else
				{
					while ( json_value[k] != NULL )
					{
						if ( FORMATTED_BUF_SIZE > format_buff->byte_index )
						{
							// Capture complete formatted string byte by byte
							format_buff->buf[format_buff->byte_index] = json_value[k];
							format_buff->byte_index++;
							k++;
						}
						else
						{
							error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
							loop_exit = true;
						}
					}
				}

				if ( ( pDCI->pFormatData == nullptr ) || ( pDCI->pFormatData->format_name != nullptr
#ifdef REST_SERVER_FORMAT_HANDLING
														   && pDCI->pFormatData->formatter_cback == nullptr
#endif
														   ) )
				{
					if ( DCI_DTYPE_STRING8 != data_block->datatype )
					{
						/* ypx->val_index is a count of separators. Can only have 1 less than number of units.*/
						if ( ypx->val_index >= ypx->num_of_units )
						{
							error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
							loop_exit = true;
						}

						if ( PTARGET_DATA_PRESENT( pTarget->present,
												   ( ypx->val_index +
													 ( ypx->num_of_units * ypx->curr_num_list ) ) ) )
						{
							int32_t ret =
								Set_Param_Numeric_Json( pTarget, ypx, data_block->datatype );
							if ( -1 == ret )
							{
								error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
								loop_exit = true;
							}
							else if ( ret < 0 )
							{
								error_500 = ERROR_BAD_REQUEST;
								loop_exit = true;
							}
						}
						if ( false == loop_exit )
						{
							++ypx->val_index;
						}
					}
				}
				else if ( ( STRLEN( "CRED" ) == strlen( pDCI->pFormatData->format_name ) ) &&
						  ( strncmp( pDCI->pFormatData->format_name, "CRED", strlen( "CRED" ) ) == 0 ) )
				{
					// Format type = CRED, thus we do not need format handler support.
					format_buff->buf[format_buff->byte_index] = '\0';

					const uint16_t DCI_LENGTH = reinterpret_cast<uint16_t>( *data_block->length_ptr );
					uint16_t filtered_length = strnlen( const_cast<const char*>
														( reinterpret_cast<char*>( format_buff->buf ) ),
														DCI_LENGTH );

					if ( DCI_LENGTH <= filtered_length )
					{
						error_500 = ERROR_REQUEST_ENTITY_TOO_LARGE;
						loop_exit = true;
					}
					else
					{
						// Copy raw data into pTarget
						BF_Lib::Copy_String( pTarget->buf.u8_data, format_buff->buf, filtered_length );

						// All CRED data has string data type.
						// Thus write 1 in present data structure indicating that one element
						// received.
						PTARGET_SET_DATA_PRESENT( pTarget->present, 0 );
					}
				}
#ifdef REST_SERVER_FORMAT_HANDLING
				else if ( pDCI->pFormatData->formatter_cback != nullptr )
				{
					// Call Format handler for STRING_TO_RAW conversion only if
					// 1. When format handler call back is attached
					// 2. When format type is other than CRED
					// First terminate string since we reached to state:YJSON_ELEMEND
					format_buff->buf[format_buff->byte_index] = '\0';

					uint16_t total_raw_bytes = pDCI->pFormatData->formatter_cback( pTarget->buf.u8_data,
																				   reinterpret_cast<uint16_t>( *
																											   data_block
																											   ->
																											   length_ptr ), format_buff->buf,
																				   FORMATTED_BUF_SIZE, STRING_TO_RAW,
																				   pDCI->pFormatData->config_struct );

					if ( total_raw_bytes > 0 )
					{
						if ( DCI_DTYPE_STRING8 != data_block->datatype )
						{
							uint16_t datatype_size = DCI::Get_Datatype_Size( data_block->datatype );

							// Generate nuber of 1's equal to number of elements
							for ( uint16_t i = 0; i < total_raw_bytes / datatype_size; i++ )
							{
								PTARGET_SET_DATA_PRESENT( pTarget->present, i );
							}
						}
						else
						{
							// Only 1 element received since data type is string
							PTARGET_SET_DATA_PRESENT( pTarget->present, 0 );
						}
					}
					else
					{
						// Format conversion failed
						error_500 = ERROR_BAD_REQUEST;
						loop_exit = true;
					}
				}
#endif
				else
				{
					// Control comes here when format attached with DCI parameter
					// without creating Format_Handler()
					BF_ASSERT( false );
				}
				if ( ( false == function_end ) && ( false == loop_exit ) )
				{
					// We can modify data only if following condition meet.
					if ( ( rs->max_auth_exempt_level < pDCI->access_w )
						 ||
						 ( ( pDCI->pFormatData != nullptr ) &&
						   ( STRLEN( "$WAIVER" ) == strlen( pDCI->pFormatData->format_name ) ) &&
						   ( 0 == strncmp( pDCI->pFormatData->format_name, "WAIVER",
										   strlen( pDCI->pFormatData->format_name ) ) )
						   &&
						   ( rs->auth_level != USER_ROLE_ADMIN ) )
						  )
					{
						error_500 = ERROR_UNAUTHORIZED;
						loop_exit = true;
					}
					else
					{
						uint16_t error_code = putParamValue( pDCI, pTarget, data_block, true,
															 rs->p.rest.rest_param );
						if ( error_code == 200 )
						{
							parse_complete = true;
							parser->response_code = RESPONSE_OK;
						}
						else
						{
							hams_respond_error( parser, error_code );
						}
					}
				}
				if ( false == loop_exit )
				{
					function_end = true;
				}
			}
			else
			{
				/* MISRA Suppress */
			}
		}

		if ( ( parse_complete == true ) || ( function_end == true ) || ( loop_exit == true ) )
		{
			msg_cnt = 1;
			Ram::De_Allocate( p_msg_buffer );

			if ( rs->p.rest.put_request != NULL )
			{
				Http_Server_Free( rs->p.rest.put_request );
			}
			if ( loop_exit == true )
			{
				hams_respond_error( parser, error_500 );
			}
		}
	}
	else
	{
		if ( ( 0U != hams_can_respond( data, length ) ) && ( parser->response_code != 0U ) )
		{
			hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );

			hams_response_header( parser, COUNT_HTTP_HDRS, reinterpret_cast<void*>( nullptr ), 0U );
		}
		else
		{
			if ( cp_result == -3 )
			{
				cp_result = 0;
				msg_cnt = 1;
				Ram::De_Allocate( p_msg_buffer );
				hams_respond_error( parser, E400_JSON_SYNTAX_ERROR );
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
		}
	}
	return ( 0U );
}

static uint32_t S_Rest_Param_Value_Put_Start_Json( hams_parser* parser,
												   const uint8_t* data, uint32_t length )
{
	bool function_end = false;

	if ( ( false == ENDS_WITH_SLASH( data ) ) && ( false == IS_EXTRA_SLASH( data ) ) )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		/*  */
		if ( hams_has_uri_part( data, length ) )
		{
			hams_respond_error( parser, STATUS_NOT_FOUND );
			function_end = true;
		}
		if ( false == function_end )
		{
			// _ASSERT( hams_check_headers_ended(data, length) );

			/* Make sure we have a content body. */
			if ( !parser->content_length )
			{
				hams_respond_error( parser, ERROR_LENGTH_REQUIRED );
				function_end = true;
			}
			else
			{
				/* Make we are reading the correct type. Form URL Encoded is not the optimal way to do this...
				 * should be using HTTP_CONTENT_TYPE_APPLICATION_JSON...oh well */
				if ( parser->content_type != HTTP_CONTENT_TYPE )
				{
					hams_respond_error( parser, ERROR_UNSUPPORTED_MEDIA_TYPE );
					function_end = true;
				}
				else
				{
					const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
						( const DCI_REST_TO_DCID_LKUP_ST_TD* )rs->p.rest.ptr;

					/* Verify authority level. */
					if ( rs->max_auth_exempt_level < pDCI->access_w )
					{
						hams_respond_error( parser, ERROR_UNAUTHORIZED );
					}
					else
					{
						/* Sanity check. */
						const DCI_DATA_BLOCK_TD* data_block = DCI::Get_Data_Block( pDCI->dcid );

						// _ASSERT( *data_block->length_ptr <= HTTPD_RECV_BUF_REUSE_DATA_UNION );

						/* Allocate ourselves a block for json parsing. */
						rs->p.rest.yjson = Http_Server_Alloc_Json_Parser( parser, 32U );
						if ( !rs->p.rest.yjson )
						{
							hams_respond_error( parser, E503_JSON_PARSER );
							function_end = true;
						}
						else
						{
							/* Allocate ourselves a block for put request data parsing. */
							uint32_t total_buf_size = RECV_TOTAL_BUF_SIZE;
							if ( pDCI->pFormatData != nullptr )
							{
								total_buf_size = RECV_TOTAL_BUF_SIZE + FORMATTED_TOTAL_BUF_SIZE;
							}
							uint8_t total_num_blocks = ( total_buf_size + ( HTTP_POOL_BLOCK_SIZE - 1U ) ) /
								HTTP_POOL_BLOCK_SIZE;
							rs->p.rest.put_request = Http_Server_Alloc_Scratch( parser, total_num_blocks );
							if ( !rs->p.rest.put_request )
							{
								hams_respond_error( parser, E503_SCRATCH_SPACE );
								function_end = true;
							}
							else
							{
								/* Initialize separator */
								struct yjson_put_json_state* ypx = ( struct yjson_put_json_state* )rs->p
									.rest
									.yjson;

								ypx->flags = 0U;

								if ( DCI_DTYPE_STRING8 != data_block->datatype )
								{
									ypx->separator = 0x2CU;	/* , */
									ypx->num_of_units = ( *( data_block->length_ptr ) )
										/ ( DCI::Get_Datatype_Size( data_block->datatype ) );
								}
								else
								{
									ypx->num_of_units = 1U;
									ypx->separator = 0x00U;	/* '\0' */
									ypx->max_num_list = 1U;
								}

								if ( DCI_DTYPE_STRING8 != data_block->datatype )
								{
									ypx->max_num_list = ( ( *( data_block->length_ptr ) )
														  / ( DCI::Get_Datatype_Size( data_block->
																					  datatype ) ) )
										/ ypx->num_of_units;
								}
								ypx->curr_num_list = 0;

								// for ( uint32_t i = 0U;
								// i < Data_Present_Size; i++ )
								// {
								// Initialize_pTarget_Buffer(pTarget);
								////CLEAR_DATA_PRESENT( pTarget, i );
								// }
								parser->user_data_cb =
									static_cast<hams_data_cb>( &S_Rest_Param_Value_Put_Parse_Json );
							}
						}
					}
				}
			}
		}
	}
	return ( 0U );
}

/**
 * @brief This function to read the value of parameters.
 * @details This function to read the value of parameters When URI is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data.
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t S_Rest_Param_Value_Read_Json( hams_parser* parser, const uint8_t* data,
											  uint32_t length )
{
	/* Ignore query variables, etc. When requests headers are done processing we'll be called with "", 0 Then we start
	   response headers. */

	/*
	 * In case data is nullptr there is no need to check for slash. If we go for check then system crashes due to
	 * nullptr
	 */

	if ( ( nullptr == data ) || ( ( false == ENDS_WITH_SLASH( data ) ) &&
								  ( false == IS_EXTRA_SLASH( data ) ) ) )
	{
		if ( 0U == hams_has_uri_part( data, length ) )
		{
			if ( ( HTTP_METHOD_GET == parser->method ) ||
				 ( HTTP_METHOD_HEAD == parser->method ) ||
				 ( HTTP_METHOD_OPTIONS == parser->method ) )
			{
				if ( hams_check_headers_ended( data, length ) )
				{
					http_server_req_state* rs = Http_Server_Get_State( parser );

					/* Dynamic output will clobber our rs->p union; hold onto the param and DCI. */
					uint32_t param = rs->p.rest.rest_param;
					const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
						( const DCI_REST_TO_DCID_LKUP_ST_TD* )rs->p.rest.ptr;
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						Validate_Options_Cors_Headers( parser, rs, nullptr );
					}
					else
					{
						if ( ( rs->max_auth_exempt_level < pDCI->access_r ) )
						{
							hams_respond_error( parser, ERROR_UNAUTHORIZED );
						}
						else
						{
							parser->response_code = RESPONSE_OK;
							Http_Server_Begin_Dynamic( parser );
							if ( REST_PARAM_DESCRIPTION == param )
							{
								if ( 0U == Get_Json_Param_Description( parser, pDCI ) )
								{
									hams_respond_error( parser, ERROR_INTERNAL_SERVER );
								}
							}
							else if ( REST_PARAM_RANGE == param )
							{
								if ( 0U == Get_Json_Param_Range( parser, pDCI ) )
								{
									hams_respond_error( parser, ERROR_INTERNAL_SERVER );
								}
							}
							else if ( REST_PARAM_ENUM == param )
							{
								if ( 0U == Get_Json_Param_Enum( parser, pDCI ) )
								{
									hams_respond_error( parser, ERROR_INTERNAL_SERVER );
								}
							}
							else if ( REST_PARAM_BITLIST == param )
							{
								if ( 0U == Get_Json_Param_Bitlist( parser, pDCI ) )
								{
									hams_respond_error( parser, ERROR_INTERNAL_SERVER );
								}
							}
							else
							{
								g_flag = false;
								from_rest = true;
								Http_Server_Print_Dynamic( parser, OPEN_JSON_OBJECT );
								if ( !Get_Json_Param_Single_Item( parser, pDCI, param, 1U ) )
								{
									hams_respond_error( parser, ERROR_INTERNAL_SERVER );
								}
								from_rest = false;
							}
						}
					}
				}
				else
				{
					// _ASSERT( hams_can_respond(data, length) );
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						// uint32_t read_only=0U ;
						if ( 0 != parser->read_only_param )
						{
							hams_response_header( parser, HTTP_HDR_ALLOW, Param_RO,
												  STRLEN( Param_RO ) );
							hams_response_header( parser, COUNT_HTTP_HDRS,
												  reinterpret_cast<const void*>( NULL ), 0U );

						}
						else
						{
							hams_response_header( parser, HTTP_HDR_ALLOW, Param_W,
												  STRLEN( Param_W ) );
							hams_response_header( parser, COUNT_HTTP_HDRS,
												  reinterpret_cast<const void*>( NULL ), 0U );

						}
					}
					else if ( Http_Server_End_Dynamic( parser ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
			}
		}
		else
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			uint32_t param = rs->p.rest.rest_param;
			if ( REST_PARAM_ENUM == param )
			{
				uint16_t enum_value = 0U;
				http_server_req_state* rs = Http_Server_Get_State( parser );
				if ( ERR_PARSING_OK ==
					 INT_ASCII_Conversion::str_to_uint16( ( const uint8_t* )data, 1U, length - 1U, &enum_value ) )
				{
					parser->user_data_cb =
						static_cast<hams_data_cb>( &Get_Enum_Description_Cb_Json );
					rs->p.rest.rest_param = enum_value;
				}
				else
				{
					hams_respond_error( parser, ERROR_BAD_REQUEST );
				}
			}
			else if ( REST_PARAM_BITLIST == param )
			{
				uint8_t bitfield_value = 0U;
				http_server_req_state* rs = Http_Server_Get_State( parser );
				if ( ERR_PARSING_OK ==
					 INT_ASCII_Conversion::str_to_uint8( ( const uint8_t* )data, 1U, length - 1U, &bitfield_value ) )
				{
					parser->user_data_cb =
						static_cast<hams_data_cb>( &Get_Bitfield_Description_Cb_Json );
					rs->p.rest.rest_param = bitfield_value;
				}
				else
				{
					hams_respond_error( parser, ERROR_BAD_REQUEST );
				}

			}
			else
			{
				hams_respond_error( parser, STATUS_NOT_FOUND );
			}
		}
	}
	return ( 0U );
}

/**
 * @brief This function to read all the value of parameters.
 * @details This function returns the list of attribute of parameters when /rs/param/x is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t S_Rest_Param_Value_All_Json( hams_parser* parser, const uint8_t* data,
											 uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/* Drop query variables. */
	if ( !hams_has_uri_part( data, length ) )
	{
		if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
			 ( HTTP_METHOD_OPTIONS != parser->method ) )
		{
			hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
		}
		else
		{
			if ( hams_check_headers_ended( data, length ) )
			{

				if ( parser->content_length )
				{
					hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
				}
				else
				{
					const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
						( const DCI_REST_TO_DCID_LKUP_ST_TD* )rs->p.rest.ptr;
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						Validate_Options_Cors_Headers( parser, rs, nullptr );
					}
					else
					{
						if ( ( rs->max_auth_exempt_level < pDCI->access_r ) )
						{
							hams_respond_error( parser, ERROR_UNAUTHORIZED );
						}
						else
						{
							/* First set the response code before emitting dynamic data. If there is not enough space
							   for the whole response the response_code will update automatically.*/
							parser->response_code = RESPONSE_OK;

							uint32_t param_id = pDCI->param_id;
							UINT16 temp_u32 = 0U;
							UINT16 temp_u16 = 0U;
							UINT8 temp_u8 = 0U;
							const DCI_DATA_BLOCK_TD* data_block = NULL;

							json_response_t* json_buffer =
								reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

							if ( json_buffer != nullptr )
							{
								json_buffer->used_buff_len = 0;
								memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
								char_t formatted_buff[BUFFER_LEN];
								memset( formatted_buff, 0U, BUFFER_LEN );

								data_block = DCI::Get_Data_Block( pDCI->dcid );

								Http_Server_Begin_Dynamic( parser );

								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, "Param", NULL, false );
								Open_Json_Object( json_buffer );

								Add_Integer_Member( json_buffer, JSON_KEY_PID, param_id, true );

								snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d", param_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, true );

								Add_Integer_Member( json_buffer, "r", pDCI->access_r, true );

								Add_Integer_Member( json_buffer, "w", pDCI->access_w, true );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
								Add_String_Member( json_buffer, "Name", pDCI->pName, true );
#endif
								Add_String_Member( json_buffer, "Description", NULL, false );
								Open_Json_Object( json_buffer );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/description", param_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, false );
								Close_Json_Object( json_buffer, true );

								if ( ( DCI_ERR_NO_ERROR ==
									   httpd_dci_access->Get_Datatype( pDCI->dcid,
																	   ( DCI_DATATYPE_TD* ) ( &temp_u8 ) ) )
									 &&
									 ( DCI_DTYPE_MAX_TYPES > temp_u8 ) )
								{
									Add_String_Member( json_buffer, "Datatype", DCI_Datatype_str[temp_u8], true );
								}

								if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
									 httpd_dci_access->Get_Length( pDCI->dcid, &temp_u32 ) )
								{
									Add_Integer_Member( json_buffer, "Length", temp_u32, true );
								}

								if ( NULL != data_block )
								{
									temp_u16 =
										Test_Bit( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_RAM_WR_DATA );
									Add_String_Member( json_buffer, "PresentWR", temp_u16 ? "1" : "0", true );
								}

								if ( 0U != temp_u16 )
								{
									Add_String_Member( json_buffer, "Value", NULL, false );
									Open_Json_Object( json_buffer );
									snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/value", param_id );
									Add_String_Member( json_buffer, HREF, formatted_buff, false );
									Close_Json_Object( json_buffer, true );
								}

								temp_u16 = 0U;
								if ( NULL != data_block )
								{
									temp_u16 =
										Test_Bit( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_NVMEM_DATA );
									Add_String_Member( json_buffer, "InitRD", temp_u16 ? "1" : "0", true );
								}

								if ( NULL != data_block )
								{
									Add_String_Member( json_buffer, "InitWR",
													   ( Test_Bit( data_block->patron_attrib_info,
																   DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) ? "1" : "0" ),
													   true );
								}

								if ( 0U != temp_u16 )
								{
									Add_String_Member( json_buffer, "Initial", NULL, false );
									Open_Json_Object( json_buffer );
									snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/initial", param_id );
									Add_String_Member( json_buffer, HREF, formatted_buff, false );
									Close_Json_Object( json_buffer, true );
								}

								temp_u16 = 0U;
								if ( NULL != data_block )
								{
									temp_u16 =
										Test_Bit( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_DEFAULT_DATA );
									Add_String_Member( json_buffer, "DefaultSupport", temp_u16 ? "1" : "0", true );
								}

								if ( 0U != temp_u16 )
								{
									Add_String_Member( json_buffer, "Default", NULL, false );
									Open_Json_Object( json_buffer );
									snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/default", param_id );
									Add_String_Member( json_buffer, HREF, formatted_buff, false );
									Close_Json_Object( json_buffer, true );
								}

								temp_u16 = 0U;
								if ( NULL != data_block )
								{
									temp_u16 =
										Test_Bit( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_RANGE_DATA );
									Add_String_Member( json_buffer, "RangeSupport", temp_u16 ? "1" : "0", true );

								}

								if ( 0U != temp_u16 )
								{
									Add_String_Member( json_buffer, "Range", NULL, false );
									Open_Json_Object( json_buffer );
									snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/range", param_id );
									Add_String_Member( json_buffer, HREF, formatted_buff, false );
									Close_Json_Object( json_buffer, true );
								}

								temp_u16 = 0;
								if ( NULL != data_block )
								{
									temp_u16 =
										Test_Bit( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_ENUM_DATA );
									Add_String_Member( json_buffer, "EnumSupport", temp_u16 ? "1" : "0", true );
								}

								if ( 0U != temp_u16 )
								{
									Add_String_Member( json_buffer, "Enum", NULL, false );
									Open_Json_Object( json_buffer );
									snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/enum", param_id );
									Add_String_Member( json_buffer, HREF, formatted_buff, false );
									Close_Json_Object( json_buffer, true );
								}

								if ( nullptr != pDCI->pBitfieldDes )
								{
									Add_String_Member( json_buffer, "BitfieldSupport", "1", true );
								}
								else
								{
									Add_String_Member( json_buffer, "BitfieldSupport", "0", true );
								}
								if ( nullptr != pDCI->pBitfieldDes )
								{
									Add_String_Member( json_buffer, "Bitfield", NULL, false );
									Open_Json_Object( json_buffer );
									snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/bitfield", param_id );
									Add_String_Member( json_buffer, HREF, formatted_buff, false );
									Close_Json_Object( json_buffer, true );
								}

								Add_String_Member( json_buffer, "Units", pDCI->pUnits, true );

								const char_t* format_name = nullptr;
								if ( pDCI->pFormatData != nullptr )
								{
									format_name = pDCI->pFormatData->format_name;
								}
								if ( format_name )
								{
									Add_String_Member( json_buffer, "Format", format_name, false );
								}
								else
								{
									Add_String_Member( json_buffer, "Format", "", false );
								}
								Multiple_Close_Object( json_buffer, 2U );
								Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								Ram::De_Allocate( json_buffer );
							}
						}
					}
				}
			}
			else
			{
				// _ASSERT( hams_can_respond(data, length) );
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{

					hams_response_header( parser, HTTP_HDR_ALLOW, Param_RO, STRLEN( Param_RO ) );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
				else if ( 0U != Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
		}
	}
	return ( 0U );
}

static uint32_t S_Rest_Param_Value_Check_Json( hams_parser* parser, const uint8_t* data,
											   uint32_t length )
{
	// _ASSERT( hams_has_uri_part(data, length) );
	// if (0U== hams_check_headers_ended( data, length ) )

	if ( IS_EXTRA_SLASH( data ) == false )
	{
		bool loop_break = false;
		bool function_end = false;
		http_server_req_state* rs = Http_Server_Get_State( parser );
		rs->p.rest.rest_param = COUNT_REST_PARAMS;
		for ( uint32_t i = 0U; ( ( i < COUNT_REST_PARAMS ) && ( false == loop_break ) ); ++i )
		{
			uint32_t len = strlen( reinterpret_cast<const char_t*>( s_rest_param_types[i] ) );
			if ( ( length == len ) &&
				 ( !strncmp( reinterpret_cast<const char_t*>( data ), s_rest_param_types[i],
							 length ) ) )
			{
				rs->p.rest.rest_param = i;
				loop_break = true;
			}
		}

		const DCI_DATA_BLOCK_TD* data_block = reinterpret_cast<const DCI_DATA_BLOCK_TD*>( NULL );
		const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
			reinterpret_cast<const DCI_REST_TO_DCID_LKUP_ST_TD*>( rs->p.rest.ptr );

		data_block = DCI::Get_Data_Block( pDCI->dcid );
		if ( !data_block )
		{
			hams_respond_error( parser, STATUS_NOT_FOUND );
		}
		else
		{
			parser->read_only_param = 0U;
			switch ( rs->p.rest.rest_param )
			{
				case REST_PARAM_ALL:
					parser->user_data_cb = static_cast<hams_data_cb>( &S_Rest_Param_Value_All_Json );
					function_end = true;
					break;

				/* These fields are read-only. */
				case REST_PARAM_DESCRIPTION:
					parser->read_only_param = 1U;
					break;

				case REST_PARAM_DEFAULT:
					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_DEFAULT_DATA ) )
					{
						hams_respond_error( parser, STATUS_NOT_FOUND );
						function_end = true;
					}

					parser->read_only_param = 1U;
					break;

				case REST_PARAM_ENUM:
					function_end = true;
					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_ENUM_DATA ) )
					{
						hams_respond_error( parser, STATUS_NOT_FOUND );
					}
					parser->read_only_param = 1U;
					parser->user_data_cb = static_cast<hams_data_cb>( &S_Rest_Param_Value_Read_Json );
					break;

				case REST_PARAM_RANGE:
					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_RANGE_DATA ) )
					{
						hams_respond_error( parser, STATUS_NOT_FOUND );
						function_end = true;
					}

					parser->read_only_param = 1U;
					break;

				case REST_PARAM_INITIAL:
					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_NVMEM_DATA ) )
					{
						hams_respond_error( parser, STATUS_NOT_FOUND );
						function_end = true;
					}

					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
					{
						parser->read_only_param = 1U;
					}

					break;

				case REST_PARAM_PRESENTVAL:
					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_RAM_WR_DATA ) )
					{
						parser->read_only_param = 1U;
					}
					break;

				case REST_PARAM_VALUE:
					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_RAM_WR_DATA ) &&
						 !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
					{
						parser->read_only_param = 1U;
					}
					break;

				case REST_PARAM_BITLIST:
					function_end = true;
					if ( nullptr == pDCI->pBitfieldDes )
					{
						hams_respond_error( parser, STATUS_NOT_FOUND );
					}
					parser->read_only_param = 1U;
					parser->user_data_cb = static_cast<hams_data_cb>( &S_Rest_Param_Value_Read_Json );
					break;

				default:
					hams_respond_error( parser, ERROR_BAD_REQUEST );
					function_end = true;
			}

		}

		if ( false == function_end )
		{
			if ( HTTP_METHOD_PUT == parser->method )
			{
				if ( 0 != parser->read_only_param )
				{
					hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
				}
				else
				{
					parser->user_data_cb =
						static_cast<hams_data_cb>( &S_Rest_Param_Value_Put_Start_Json );
				}
			}
			else
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &S_Rest_Param_Value_Read_Json );
			}
		}
	}
	return ( 0U );
}

/**
 * @brief This function to get remaining parameters.
 * @details This function returns the remaining parameters when /rs/param is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t S_Rest_Param_All_Remaining_Json( hams_parser* parser, const uint8_t* data,
												 uint32_t length )
{
	uint32_t i;

	parser->blocks_for_data = 0;

	http_server_req_state* rs = Http_Server_Get_State( parser );

	rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
	Http_Server_Begin_Dynamic( parser );

	json_response_t* json_buffer = reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0U, BUFFER_LEN );

		for ( i = parser->data_transmitted;
			  ( i < rest_dci_data_target_info.number_of_parameters ) &&
			  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) ); i++ )
		{
			/* Transmit the parameter if user has read access */
			if ( rs->auth_level >= rest_dci_data_target_info.param_list[i].access_r )
			{
				Open_Json_Object( json_buffer );
				Add_Integer_Member( json_buffer, JSON_KEY_PID, rest_dci_data_target_info.param_list[i].param_id, true );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
				Add_String_Member( json_buffer, "name", rest_dci_data_target_info.param_list[i].pName, true );
#endif
				snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d",
						  rest_dci_data_target_info.param_list[i].param_id );
				Add_String_Member( json_buffer, HREF, formatted_buff, false );
				if ( i < ( rest_dci_data_target_info.number_of_parameters - 1U ) )
				{
					Close_Json_Object( json_buffer, true );
				}
				else
				{
					Close_Json_Object( json_buffer, false );
				}
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
			}
			parser->data_transmitted++;
		}

		if ( parser->data_transmitted < rest_dci_data_target_info.number_of_parameters )
		{
			parser->transmission_continue = true;
		}
		else
		{
			parser->data_transmitted = 0U;
			parser->transmission_continue = false;
			Close_Json_Array( json_buffer, false );
			Multiple_Close_Object( json_buffer, 2U );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
		}
		Ram::De_Allocate( json_buffer );
	}
	Http_Server_End_Dynamic( parser );
	g_hams_demultiplexer->staged_len = 0U;
	return ( 0U );
}

/**
 * @brief This function to get the list of parameters.
 * @details This function returns the list of param when /rs/param is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t S_Rest_Param_All_Json( hams_parser* parser, const uint8_t* data,
									   uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( ( 0U == rs->max_auth_exempt_level ) && ( HTTP_METHOD_OPTIONS != parser->method ) )
	{
		hams_respond_error( parser, ERROR_UNAUTHORIZED );
	}
	else
	{
		/* Drop query variables. */
		if ( !hams_has_uri_part( data, length ) )
		{
			rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
			rs->p.dyn.rest_list.max_auth = 0;
			rs->p.dyn.rest_list.count = 0;
			parser->blocks_for_data = 0;
			if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
				 ( HTTP_METHOD_OPTIONS != parser->method ) )
			{
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
			}
			else
			{
				if ( hams_check_headers_ended( data, length ) )
				{
					if ( parser->content_length )
					{
						hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
					}
					else if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						Validate_Options_Cors_Headers( parser, rs, nullptr );
					}
					else
					{
						/***********************Calculate Total Content
						   Length******************************************/
						uint32_t content_length = 0U;
						uint32_t param_count = 0U;
						uint32_t param_id;
						uint32_t common_content_len;///< common content length for each parameter

						content_length += STRLEN( JSON_RS_PARAM_PREFIX );

						/* Calculate total number of parameters accessible to user and content length of each parameter
						 */
						for ( param_id = 0U;
							  param_id < rest_dci_data_target_info.number_of_parameters;
							  param_id++ )
						{
							if ( rs->auth_level >= rest_dci_data_target_info.param_list[param_id].access_r )
							{
								content_length += ( 2U
													* Find_Base_Log_Ten(
														rest_dci_data_target_info.param_list[param_id].
														param_id ) );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
								content_length += strlen(
									rest_dci_data_target_info.param_list[param_id].pName );
#endif
								param_count++;
							}
						}

						if ( param_count != 0U )
						{
							/* Calculate the fix string length */
							common_content_len = STRLEN( JSON_RS_PARAM_FIX_STRING );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
							common_content_len += STRLEN( JSON_RS_PARAM_NAME );
#endif
							content_length = content_length + ( common_content_len * param_count );
						}

						content_length += STRLEN( JSON_RS_PARAM_SUFFIX );
						parser->total_content_length = content_length;
						/******************************************************************************************/

						/* First set the response code before emitting dynamic data. If there is not enough space for
						   the whole response the response_code will update automatically.*/

						parser->response_code = RESPONSE_OK;

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
							Add_String_Member( json_buffer, "ParamList", NULL, false );
							Open_Json_Object( json_buffer );
							Add_String_Member( json_buffer, HREF, "/rs/param", true );
							Add_String_Member( json_buffer, "Param", NULL, false );
							Open_Json_Array( json_buffer );

							uint32_t i;

							for ( i = parser->data_transmitted;
								  ( i < rest_dci_data_target_info.number_of_parameters ) &&
								  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) );
								  i++ )
							{
								/* Transmit the parameter if user has read access */
								if ( rs->auth_level >= rest_dci_data_target_info.param_list[i].access_r )
								{
									Open_Json_Object( json_buffer );
									Add_Integer_Member( json_buffer, JSON_KEY_PID,
														rest_dci_data_target_info.param_list[i].param_id, true );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
									Add_String_Member( json_buffer, "name",
													   rest_dci_data_target_info.param_list[i].pName, true );
#endif
									snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d",
											  rest_dci_data_target_info.param_list[i].param_id );
									Add_String_Member( json_buffer, HREF, formatted_buff, false );
									if ( i < ( rest_dci_data_target_info.number_of_parameters - 1U ) )
									{
										Close_Json_Object( json_buffer, true );
									}
									else
									{
										Close_Json_Object( json_buffer, false );
									}
									Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
									json_buffer->used_buff_len = 0U;
									memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
								}
								parser->data_transmitted++;
							}

							if ( parser->data_transmitted < rest_dci_data_target_info.number_of_parameters )
							{
								parser->transmission_continue = true;
								parser->user_data_cb_remaining = S_Rest_Param_All_Remaining_Json;
							}
							else
							{
								parser->data_transmitted = 0;
								parser->transmission_continue = false;
								Close_Json_Array( json_buffer, false );
								Multiple_Close_Object( json_buffer, 2U );
								Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
							}
							Ram::De_Allocate( json_buffer );
						}
					}
				}
				else
				{
					// _ASSERT( hams_can_respond(data, length) );
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{

						hams_response_header( parser, HTTP_HDR_ALLOW, Param_RO,
											  STRLEN( Param_RO ) );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );

					}
					else if ( Http_Server_End_Dynamic( parser ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE,
											  HTTP_HDR_DATA_LEN );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
			}
		}
	}
	return ( 0U );
}

/**
 * @brief This function to get the values for list of parameters.
 * @details This function returns the values for list of parameters when /rs/param/value/x1/x2/x3 is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t S_Rest_Param_List_End_Json( hams_parser* parser, const uint8_t* data,
											uint32_t length )
{
	bool loop_break = false;

	/* Skip over query parameters. */
	if ( !hams_has_uri_part( data, length ) )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );

		if ( hams_check_headers_ended( data, length ) )
		{
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				Validate_Options_Cors_Headers( parser, rs, nullptr );
			}
			else
			{
				if ( rs->max_auth_exempt_level < rs->p.dyn.rest_list.max_auth )
				{
					hams_respond_error( parser, ERROR_UNAUTHORIZED );
					loop_break = true;
				}
				if ( false == loop_break )
				{
					/* Write out the last end tag of the JSON response. If there is no overflow then set the response
					   code to 200. */
					if ( g_flag == true )
					{
						g_flag = false;
					}
					if ( Http_Server_Print_Dynamic( parser, CLOSE_JSON_ARRAY CLOSE_JSON_OBJECT CLOSE_JSON_OBJECT ) )
					{
						parser->response_code = RESPONSE_OK;
					}
				}
			}
		}
		else
		{
			// _ASSERT( hams_can_respond(data, length) );
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				hams_response_header( parser, HTTP_HDR_ALLOW, Param_RO, STRLEN( Param_RO ) );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else if ( Http_Server_End_Dynamic( parser ) )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
		}
	}

	return ( 0U );
}

static uint32_t S_Rest_Param_List_Element_Json( hams_parser* parser, const uint8_t* data,
												uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	// _ASSERT( rs->p.dyn.rest_list.count <= 0xFFFFU );

	if ( IS_EXTRA_SLASH( data ) == false )
	{
		/* Expecting a decimal integer, 0-65535 */
		uint16_t param = 0U;
		if ( ERR_PARSING_OK !=
			 INT_ASCII_Conversion::str_to_uint16( ( const uint8_t* )data, 1, length - 1, &param ) )
		{
			if ( ( true == ENDS_WITH_SLASH( data ) ) && ( rs->p.dyn.rest_list.count > 0U ) )
			{
				// '/'
				// and
				// 'space'
				parser->user_data_cb = static_cast<hams_data_cb>( &S_Rest_Param_List_End_Json );
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
		}
		else
		{
			const DCI_REST_TO_DCID_LKUP_ST_TD*
				pDCI = httpd_rest_dci::findIndexREST( param );

			if ( NULL == pDCI )
			{
				hams_respond_error( parser, STATUS_NOT_FOUND );
			}
			else
			{
				if ( NULL == DCI::Get_Data_Block( pDCI->dcid ) )
				{
					hams_respond_error( parser, STATUS_NOT_FOUND );
				}
				else
				{
					if ( rs->p.dyn.rest_list.max_auth < pDCI->access_r )
					{
						rs->p.dyn.rest_list.max_auth = pDCI->access_r;
					}

					// Http_Server_Print_Dynamic( parser, "\n" );
					if ( g_flag == false )
					{
						Http_Server_Print_Dynamic( parser, OPEN_JSON_OBJECT );
					}
					from_rest = true;

					if ( !Get_Json_Param_Single_Item( parser, pDCI, REST_PARAM_VALUE, 0 ) )
					{
						hams_respond_error( parser, ERROR_INTERNAL_SERVER );
					}
					else
					{
						from_rest = false;
						++rs->p.dyn.rest_list.count;
						/* This is for rs/param/value/1 */
						if ( '/' != data[length] )
						{
							parser->user_data_cb =
								static_cast<hams_data_cb>( &S_Rest_Param_List_End_Json );
						}
						else
						{
							Http_Server_Print_Dynamic( parser, JSON_ADD_COMMA );
						}
					}
				}
			}
		}
	}

	return ( 0U );
}

static uint32_t S_Rest_Param_Index_Json( hams_parser* parser, const uint8_t* data,
										 uint32_t length )
{
	/* Must be data. */
	// _ASSERT( hams_has_uri_part(data, length) );
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/* Look for all match. */
	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )	// '/' => 0x2F
	{
		if ( data[length] != 0x2FU )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &S_Rest_Param_All_Json );
		}
	}
	else
	{
		/* Look for 'value' */
		if ( !strncmp( ( ( const char_t* )data + 1 ), "value", 5U ) )
		{
			/* Only GET or HEAD is valid. */
			if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
				 ( HTTP_METHOD_OPTIONS != parser->method ) )
			{
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
			}
			else
			{
				if ( data[length] == '/' )
				{
					parser->user_data_cb = static_cast<hams_data_cb>
						( &S_Rest_Param_List_Element_Json );
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
					rs->p.dyn.rest_list.max_auth = 0U;
					rs->p.dyn.rest_list.count = 0U;
					Http_Server_Begin_Dynamic( parser );

					json_response_t* json_buffer =
						reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

					if ( json_buffer != nullptr )
					{
						json_buffer->used_buff_len = 0U;
						memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
						Open_Json_Object( json_buffer );
						Add_String_Member( json_buffer, "Values", NULL, false );
						g_flag = false;
						Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
						Ram::De_Allocate( json_buffer );
					}
				}
				else
				{
					hams_respond_error( parser, STATUS_NOT_FOUND );
				}
			}
		}
		else
		{
			/* Expecting a decimal integer, 0-65535 */
			uint16_t value = 0U;
			if ( ERR_PARSING_OK
				 !=
				 INT_ASCII_Conversion::str_to_uint16( ( const uint8_t* )data, 1, length - 1U,
													  &value ) )
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
			else
			{
				const DCI_REST_TO_DCID_LKUP_ST_TD*
					pDCI = httpd_rest_dci::findIndexREST( value );

				if ( NULL == pDCI )
				{
					hams_respond_error( parser, STATUS_NOT_FOUND );
				}
				else if ( NULL == DCI::Get_Data_Block( pDCI->dcid ) )
				{
					hams_respond_error( parser, STATUS_NOT_FOUND );
				}
				else
				{
					rs->p.rest.ptr = ( void* )pDCI;
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
					rs->p.dyn.rest_list.max_auth = 0U;
					rs->p.dyn.rest_list.count = 0U;
					if ( '/' != data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>
							( &S_Rest_Param_Value_All_Json );
					}
					else
					{
						parser->user_data_cb =
							static_cast<hams_data_cb>( &S_Rest_Param_Value_Check_Json );
					}
				}
			}
		}
	}
	return ( 0 );
}

static uint32_t S_Rest_Device_Index_Json( hams_parser* parser, const uint8_t* data,
										  uint32_t length )
{
	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )
	{
		// '/'=> ASCII 0x2FU
		if ( data[length] != 0x2FU )
		{
			parser->user_data_cb = &S_Rest_Device_All_Json;
		}
	}
	else
	{
		/* TODO */
		hams_respond_error( parser, ERROR_NOT_IMPLEMENTED );
	}
	return ( 0U );
}

/** User defined functions required for REST services to function.
 * @brief       This function is used to write the JSON object name and REST parameter id.
 * @param[in]   parser : Pointer to parser structure
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return Success or failure of requested operation.
 */
static uint32_t S_Rest_Other_Handler_Json( hams_parser* parser, const uint8_t* data,
										   uint32_t length )
{
	bool choice_found = false;

	if ( IS_EXTRA_SLASH( data ) == false )
	{
		uint32_t choice = COUNT_REST;
		for ( uint32_t i = 0U; ( ( i < COUNT_REST ) && ( choice_found == false ) ); ++i )
		{
			uint32_t len = BF_Lib::Get_String_Length( const_cast<uint8_t*>( s_rest_paths[i] ),
													  REST_PATH_MAX_LEN );
			if ( ( length == len ) && ( BF_Lib::Compare_Data( data, s_rest_paths[i], length ) == true ) )
			{
				choice = i;
				choice_found = true;
			}
		}

		switch ( choice )
		{
			case REST_ROOT:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					if ( ( HTTP_METHOD_GET != parser->method ) &&
						 ( HTTP_METHOD_HEAD != parser->method ) )
					{
						hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
					}
					else
					{
						parser->response_code = RESPONSE_OK;
						parser->user_data_cb = static_cast<hams_data_cb>
							( &S_Rest_Root_Hdrs_End_Json );
					}
				}
				break;

			case REST_DEVICE:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					if ( '/' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>
							( &S_Rest_Device_Index_Json );
					}
					else if ( ' ' == data[length] )
					{
						parser->user_data_cb = S_Rest_Device_All_Json;
					}
					else
					{
						/* No '?' mark allowed, bad request TODO If question mark is really a special case then it will
						   be handled later ON*/
						// hams_respond_error( parser, E404_NO_QUERY_ALLOWED );
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
				}
				break;

			case REST_PARAM:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					if ( '/' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &S_Rest_Param_Index_Json );
					}
					else if ( ' ' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &S_Rest_Param_All_Json );
					}
					else
					{
						/* No '?' mark allowed, bad request TODO If question mark is really a special case then it will
						   be handled later ON*/
						// hams_respond_error( parser, E404_NO_QUERY_ALLOWED );
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
				}
				break;

			/* Unimplemented: */
			case REST_ASSY:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					if ( '/' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( Assy_Index_Callback_Json );
					}
					else if ( ' ' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( Assy_Get_All_Callback_Json );
					}
					else
					{
						/* No '?' mark allowed, bad request TODO If question mark is really a special case then it will
						   be handled later ON*/
						// hams_respond_error( parser, E404_NO_QUERY_ALLOWED );
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
				}
				break;

			case REST_FW:
				if ( '/' == data[length] )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Parse_Fw_Resources_Cb_Json );

				}
				else if ( ' ' == data[length] )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Prod_Info_Spec_Cb_Json );

				}
				else
				{
					/* No '?' mark allowed, bad request TODO If question mark is really a special case then it will be
					   handled later ON*/
					// hams_respond_error( parser, E404_NO_QUERY_ALLOWED );
					hams_respond_error( parser, ERROR_BAD_REQUEST );
				}
				break;

			case REST_USER_MGMT:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else if ( ( ' ' == data[length] ) || ( '/' == data[length] ) )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Parse_UserMgmt_Resources_Cb_Json );
				}
				else
				{
					hams_respond_error( parser, ERROR_BAD_REQUEST );
				}
				break;

			case REST_STRUCT:
				/* TODO */
				hams_respond_error( parser, ERROR_NOT_IMPLEMENTED );
				break;

#ifdef REST_LOGGING
			case REST_LOG:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					if ( '/' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &Parse_Log_Resource_Cb_Json );
					}
					else if ( ' ' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb_Json );
					}
					else
					{
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
				}
				break;

#endif
#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
			case REST_CERT:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					if ( ( ' ' == data[length] ) || ( '/' == data[length] ) )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &Get_Cert_Store_Cb );
					}
					else
					{
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
				}
				break;

#endif
			default:
				hams_respond_error( parser, ERROR_BAD_REQUEST );
				break;
		}
	}

	return ( 0 );
}

/**
 * @brief This function to get the description of parameters.
 * @details This function returns the description of parameters when a /rs/param/x/description is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return bool Returns either true or false
 */
static bool Get_Json_Param_Description( hams_parser* parser,
										const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI )
{

#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
	uint16_t String_Size = strlen( ( char const* )pDCI->pDescription );
	String_Size = String_Size + 1;

	json_response_t* json_buffer = reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0U, BUFFER_LEN );
		Open_Json_Object( json_buffer );
		Add_String_Member( json_buffer, "Description", NULL, false );
		Open_Json_Object( json_buffer );
		Add_Integer_Member( json_buffer, JSON_KEY_PID, pDCI->param_id, true );
		snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/description", pDCI->param_id );
		Add_String_Member( json_buffer, HREF, formatted_buff, true );

		char_t* pDescriptionBuffer = reinterpret_cast<char_t*>( Ram::Allocate( String_Size ) );

		if ( pDescriptionBuffer != nullptr )
		{
			memset( pDescriptionBuffer, 0U, String_Size );

			strncpy( pDescriptionBuffer, pDCI->pDescription, String_Size );

			for ( uint16_t j = 0U; ( pDescriptionBuffer[j] != '\0' ); j++ )
			{
				if ( pDescriptionBuffer[j] == 0x0AU )					// if '\n' character
				{
					pDescriptionBuffer[j] = 0x20U;						// replaced it with space
				}
			}

			Add_String_Member( json_buffer, JSON_KEY_TEXT, pDescriptionBuffer, false );
			Ram::De_Allocate( pDescriptionBuffer );
		}

		Multiple_Close_Object( json_buffer, 2U );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
		Ram::De_Allocate( json_buffer );
	}
#endif
	return ( true );
}

/**
 * @brief This function to get the range of parameters.
 * @details This function returns the range of parameters when a /rs/param/x/range is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return bool Returns either true or false
 */
static bool Get_Json_Param_Range( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI )
{
	bool return_val = false;
	DCI_ACCESS_ST_TD access_struct =
	{ 0U };
	uint32_t type = 0U;
	UINT8 type_length = 0U;

	if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
		 httpd_dci_access->Get_Datatype( pDCI->dcid, ( DCI_DATATYPE_TD* ) ( &type ) ) )
	{

		if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
			 httpd_dci_access->Get_Datatype_Length( pDCI->dcid, &type_length ) )
		{

			// Exception_Assert( type_length <= DEST_BUFFER_SIZE );

			access_struct.data_access.length = type_length;
			access_struct.source_id = REST_DCI_SOURCE_ID;
			access_struct.data_access.data = j_dest_data;
			access_struct.data_access.offset = 0U;
			access_struct.data_id = pDCI->dcid;

			json_response_t* json_buffer = reinterpret_cast<json_response_t*>( Ram::Allocate(
																				   sizeof( json_response_t ) ) );

			if ( json_buffer != nullptr )
			{
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
				char_t formatted_buff[BUFFER_LEN];
				memset( formatted_buff, 0U, BUFFER_LEN );
				Open_Json_Object( json_buffer );
				Add_String_Member( json_buffer, "Range", NULL, false );
				Open_Json_Object( json_buffer );
				Add_Integer_Member( json_buffer, JSON_KEY_PID, pDCI->param_id, true );
				Add_String_Member( json_buffer, "Min", NULL, false );
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
				Http_Server_Print_Dynamic( parser, "\"" );

				access_struct.command = DCI_ACCESS_GET_MIN_CMD;
				if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
					 static_cast<uint32_t>( httpd_dci_access->Data_Access( &access_struct ) ) )
				{
					if ( Format_Handler_Check_Json( parser, pDCI, j_dest_data, type_length, 0U ) == true )
					{
						Http_Server_Print_Dynamic( parser, "</Min>\n\t<Max>" );
						access_struct.command = DCI_ACCESS_GET_MAX_CMD;
						if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
							 static_cast<uint32_t>
							 ( httpd_dci_access->Data_Access( &access_struct ) ) )
						{
							if ( Format_Handler_Check_Json( parser, pDCI, j_dest_data, type_length, 0U ) == true )
							{
								Http_Server_Print_Dynamic( parser, "</Max>\n</Range>" );
								return_val = true;
							}
						}
					}
					else
					{
						if ( false != S_Dynamic_Write_Value_Json( parser, type, j_dest_data, 0U ) )
						{
							Http_Server_Print_Dynamic( parser, "\"," );
							Http_Server_Print_Dynamic( parser, "\"Max\" : \"" );
							access_struct.command = DCI_ACCESS_GET_MAX_CMD;
							if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
								 static_cast<uint32_t>
								 ( httpd_dci_access->Data_Access( &access_struct ) ) )
							{
								if ( false != S_Dynamic_Write_Value_Json( parser, type, j_dest_data, 0U ) )
								{
									Http_Server_Print_Dynamic( parser, "\"" );
									return_val = true;
								}
							}

						}
					}
				}
				Multiple_Close_Object( json_buffer, 2U );
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				Ram::De_Allocate( json_buffer );
			}
		}
	}

	return ( return_val );
}

/**
 * @brief This function to get the enum of parameters.
 * @details This function returns the enum of parameters when a /rs/param/x/enum is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] pDCI : Pointer to DCID lookup structure consisting of REST parameter
 * @return bool Returns either true or false
 */
static bool Get_Json_Param_Enum( hams_parser* parser,
								 const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI )
{
	bool return_val = true;
	uint16_t enum_count = 0U;
	uint32_t content_length = 0U;
	bool status = false;

	json_response_t* json_buffer = reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0U, BUFFER_LEN );

		if ( nullptr != pDCI->pEnumDes )
		{
			uint16_t i;
			enum_val_t enum_val = 0U;
			enum_count = pDCI->pEnumDes->total_enums;
			/* calculating content length of the response */
			content_length += STRLEN( JSON_ENUM_PREFIX );

			content_length += ( 2U
								* Find_Base_Log_Ten(
									pDCI->param_id ) );									// multipling by 2 as we are
																						// printing it two times
			content_length += STRLEN( JSON_ENUM_SUFFIX );

			content_length += Find_Base_Log_Ten( enum_count );
			content_length += ( enum_count * ( STRLEN( JSON_ENUM_FIX_STRING ) ) );

			content_length += ( enum_count * ( Find_Base_Log_Ten( pDCI->param_id ) ) );


			for ( i = 0U; i < enum_count; i++ )
			{
				enum_val = pDCI->pEnumDes->enum_list[i].enum_val;
				if ( enum_val < ( static_cast < enum_val_t > ( 0 ) ) )
				{
					content_length += 1U;				// for negative sign
					enum_val *= -1;					// to convert negative number to positive to get digit count
				}
				content_length += Find_Base_Log_Ten( enum_val );
				content_length += Find_Base_Log_Ten( i );
			}

			content_length += STRLEN( XML_ENUM_SUFFIX );
			parser->total_content_length = content_length;
			parser->response_code = RESPONSE_OK;
			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, "Enum", NULL, false );
			Open_Json_Object( json_buffer );
			Add_Integer_Member( json_buffer, JSON_KEY_PID, pDCI->param_id, true );
			snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/enum", pDCI->param_id );
			Add_String_Member( json_buffer, HREF, formatted_buff, true );
			Add_Integer_Member( json_buffer, "Total", enum_count, true );
			Add_String_Member( json_buffer, "Val", NULL, false );
			Open_Json_Array( json_buffer );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
			json_buffer->used_buff_len = 0U;
			memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

			for ( i = 0U; ( i < enum_count ) &&
				  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) ); i++ )
			{
				Open_Json_Object( json_buffer );
				snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/enum/%d", pDCI->param_id, i );
				Add_String_Member( json_buffer, HREF, formatted_buff, true );
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

				enum_val = pDCI->pEnumDes->enum_list[i].enum_val;
				if ( enum_val < ( static_cast < enum_val_t > ( 0 ) ) )
				{
					uint8_t type = 0U;
					if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) !=
						 httpd_dci_access->Get_Datatype( pDCI->dcid, ( DCI_DATATYPE_TD* ) ( &type ) ) )
					{
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
					Http_Server_Print_Dynamic( parser, "\"text\" : \"" );
					status = S_Dynamic_Write_Value_Json( parser, type, reinterpret_cast<uint8_t*>( &enum_val ), 0U );
					if ( false == status )
					{
						REST_DEBUG_PRINT( DBG_MSG_ERROR, "Error in S_Dynamic_Write_Value_Json" );
					}
					Http_Server_Print_Dynamic( parser, "\"" );
				}
				else
				{
					Add_Integer_Member( json_buffer, JSON_KEY_TEXT, enum_val, false );
				}
				if ( i < ( enum_count - 1U ) )
				{
					Close_Json_Object( json_buffer, true );
				}
				else
				{
					Close_Json_Object( json_buffer, false );
				}
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
			}

			if ( true == return_val )
			{

				parser->data_transmitted = i;
				parser->general_use_reg = reinterpret_cast<uint32_t>( pDCI );
				if ( i < enum_count )
				{
					parser->transmission_continue = true;
					parser->user_data_cb_remaining = Get_Enum_Json_All_Remaining;
				}
				else
				{
					parser->data_transmitted = 0U;
					parser->transmission_continue = false;
					Close_Json_Array( json_buffer, false );
					Multiple_Close_Object( json_buffer, 2U );
					Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				}
			}
		}
		else
		{
			return_val = false;
		}
		Ram::De_Allocate( json_buffer );
	}

	return ( return_val );
}

/**
 * @brief This function to get the enum of parameters.
 * @details This function returns the enum of parameters when a /rs/param/x/enum is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Get_Enum_Json_All_Remaining( hams_parser* parser, const uint8_t* data,
											 uint32_t length )
{
	uint16_t i;
	uint16_t enum_count = 0U;
	enum_val_t enum_val = 0U;
	bool status = false;

	parser->blocks_for_data = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;

	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = ( const DCI_REST_TO_DCID_LKUP_ST_TD* )( parser->general_use_reg );

	enum_count = pDCI->pEnumDes->total_enums;

	Http_Server_Begin_Dynamic( parser );

	json_response_t* json_buffer = reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0U, BUFFER_LEN );

		for ( i = parser->data_transmitted; ( i < enum_count ) &&
			  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) ); i++ )
		{
			Open_Json_Object( json_buffer );
			snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/enum/%d", pDCI->param_id, i );
			Add_String_Member( json_buffer, HREF, formatted_buff, true );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
			json_buffer->used_buff_len = 0U;
			memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

			enum_val = pDCI->pEnumDes->enum_list[i].enum_val;
			if ( enum_val < ( static_cast < enum_val_t > ( 0 ) ) )
			{
				uint8_t type = 0U;
				if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) !=
					 httpd_dci_access->Get_Datatype( pDCI->dcid, ( DCI_DATATYPE_TD* ) ( &type ) ) )
				{
					hams_respond_error( parser, ERROR_BAD_REQUEST );
				}
				Http_Server_Print_Dynamic( parser, "\"text\" : \"" );
				status = S_Dynamic_Write_Value_Json( parser, type, reinterpret_cast<uint8_t*>( &enum_val ), 0U );
				if ( false == status )
				{
					REST_DEBUG_PRINT( DBG_MSG_ERROR, "Error in S_Dynamic_Write_Value_Json" );
				}
				Http_Server_Print_Dynamic( parser, "\"" );
			}
			else
			{
				Add_Integer_Member( json_buffer, JSON_KEY_TEXT, enum_val, false );
			}
			if ( i < ( enum_count - 1U ) )
			{
				Close_Json_Object( json_buffer, true );
			}
			else
			{
				Close_Json_Object( json_buffer, false );
			}
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
			json_buffer->used_buff_len = 0U;
			memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
		}
		parser->data_transmitted = i;
		if ( i < enum_count )
		{
			parser->transmission_continue = true;
		}
		else
		{
			parser->data_transmitted = 0U;
			parser->transmission_continue = false;
			Close_Json_Array( json_buffer, false );
			Multiple_Close_Object( json_buffer, 2U );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
		}
		Ram::De_Allocate( json_buffer );
	}
	Http_Server_End_Dynamic( parser );
	g_hams_demultiplexer->staged_len = 0U;
	return ( 0U );
}

/**
 * @brief Callback function to get the description of enum of parameters.
 * @details This function returns the description of enum of parameters when /rs/param/x/enum/y is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Get_Enum_Description_Cb_Json( hams_parser* parser, const uint8_t* data,
											  uint32_t length )
{
	uint16_t enum_count = 0U;
	uint16_t enum_index = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	enum_index = rs->p.rest.rest_param;
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
		( const DCI_REST_TO_DCID_LKUP_ST_TD* )rs->p.rest.ptr;

	if ( !hams_has_uri_part( data, length ) )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		if ( hams_check_headers_ended( data, length ) )
		{
			if ( nullptr != pDCI->pEnumDes )
			{
				enum_count = pDCI->pEnumDes->total_enums;
				if ( enum_index < enum_count )
				{
					if ( ( HTTP_METHOD_GET == parser->method ) ||
						 ( HTTP_METHOD_HEAD == parser->method ) ||
						 ( HTTP_METHOD_OPTIONS == parser->method ) )
					{
						if ( HTTP_METHOD_OPTIONS == parser->method )
						{
							Validate_Options_Cors_Headers( parser, rs, nullptr );
						}
						else
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
								Add_String_Member( json_buffer, "Val", NULL, false );
								Open_Json_Object( json_buffer );
								Add_Integer_Member( json_buffer, JSON_KEY_PID, pDCI->param_id, true );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/enum/%d", pDCI->param_id,
										  enum_index );
								Add_String_Member( json_buffer, HREF, formatted_buff, true );
								Add_String_Member( json_buffer, "Desc",
												   pDCI->pEnumDes->enum_list[enum_index].description, false );
								Multiple_Close_Object( json_buffer, 2U );
								Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								Ram::De_Allocate( json_buffer );
							}
						}
					}
					else
					{
						hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
					}
				}
				else
				{
					hams_respond_error( parser, STATUS_NOT_FOUND );
				}
			}
			else
			{
				hams_respond_error( parser, STATUS_NOT_FOUND );
			}
		}
		else
		{
			// _ASSERT( hams_can_respond(data, length) );
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				hams_response_header( parser, HTTP_HDR_ALLOW, Param_RO, STRLEN( Param_RO ) );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else if ( Http_Server_End_Dynamic( parser ) )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
		}
	}
	else
	{
		if ( ( true == ENDS_WITH_SLASH( data ) ) || ( true == IS_EXTRA_SLASH( data ) ) )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Get_Enum_Description_Cb_Json );
		}
		else
		{
			hams_respond_error( parser, STATUS_NOT_FOUND );
		}
	}
	return ( 0U );
}

/**
 * @brief This function to get the values in the bitfield.
 * @details This function returns the values in the bitfield when a /rs/param/x/bitfield is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] pDCI : Pointer to DCID lookup structure consisting of REST parameter
 * @return bool Returns either true or false
 */
static bool Get_Json_Param_Bitlist( hams_parser* parser,
									const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI )
{
	bool return_val = true;
	bitfield_val_t bitfield_count = 0U;

	json_response_t* json_buffer = reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0U, BUFFER_LEN );

		if ( nullptr != pDCI->pBitfieldDes )
		{
			bitfield_count = pDCI->pBitfieldDes->total_bitfields;

			Open_Json_Object( json_buffer );
			Add_String_Member( json_buffer, "Bitfield", NULL, false );
			Open_Json_Object( json_buffer );
			Add_Integer_Member( json_buffer, JSON_KEY_PID, pDCI->param_id, true );
			snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/bitfield", pDCI->param_id );
			Add_String_Member( json_buffer, HREF, formatted_buff, true );
			Add_Integer_Member( json_buffer, "Total", bitfield_count, true );
			Add_String_Member( json_buffer, "Bit", NULL, false );
			Open_Json_Array( json_buffer );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
			json_buffer->used_buff_len = 0U;
			memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

			for ( bitfield_val_t i = 0U; i < bitfield_count; i++ )
			{
				Open_Json_Object( json_buffer );
				snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/bitfield/%d", pDCI->param_id,
						  pDCI->pBitfieldDes->bitfield_list[i].bitfield_val );
				Add_String_Member( json_buffer, HREF, formatted_buff, true );
				Add_Integer_Member( json_buffer, JSON_KEY_TEXT, pDCI->pBitfieldDes->bitfield_list[i].bitfield_val,
									false );
				if ( i < ( bitfield_count - 1U ) )
				{
					Close_Json_Object( json_buffer, true );
				}
				else
				{
					Close_Json_Object( json_buffer, false );
				}
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
			}
			Close_Json_Array( json_buffer, false );
			Multiple_Close_Object( json_buffer, 2U );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
		}
		else
		{
			return_val = false;
		}
		Ram::De_Allocate( json_buffer );
	}
	return ( return_val );
}

/**
 * @brief Callback function to get the description for bitfield of parameters.
 * @details This function sends the description for bitfield of parameters when /rs/param/x/bitfield/y is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
static uint32_t Get_Bitfield_Description_Cb_Json( hams_parser* parser, const uint8_t* data,
												  uint32_t length )
{
	bitfield_val_t bitfield_count = 0U;
	bitfield_val_t bitfield_requested = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	bitfield_requested = rs->p.rest.rest_param;
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
		( const DCI_REST_TO_DCID_LKUP_ST_TD* )rs->p.rest.ptr;

	if ( !hams_has_uri_part( data, length ) )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );

		if ( hams_check_headers_ended( data, length ) )
		{
			if ( ( HTTP_METHOD_GET == parser->method ) ||
				 ( HTTP_METHOD_HEAD == parser->method ) ||
				 ( HTTP_METHOD_OPTIONS == parser->method ) )
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					Validate_Options_Cors_Headers( parser, rs, nullptr );
				}
				else
				{
					if ( nullptr != pDCI->pBitfieldDes )
					{
						bitfield_count = pDCI->pBitfieldDes->total_bitfields;
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
							Add_String_Member( json_buffer, "Bit", NULL, false );
							Open_Json_Object( json_buffer );
							Add_Integer_Member( json_buffer, JSON_KEY_PID, pDCI->param_id, true );
							snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d/bitfield/%d", pDCI->param_id,
									  bitfield_requested );
							Add_String_Member( json_buffer, HREF, formatted_buff, true );
							Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
							json_buffer->used_buff_len = 0U;
							memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

							bool loop_break = false;

							for ( bitfield_val_t i = 0U; ( i < bitfield_count ) && ( loop_break == false ); i++ )
							{
								if ( pDCI->pBitfieldDes->bitfield_list[i].bitfield_val == bitfield_requested )
								{
									Add_String_Member( json_buffer, "Desc",
													   pDCI->pBitfieldDes->bitfield_list[i].description, false );
									loop_break = true;
								}
							}
							if ( false == loop_break )
							{
								hams_respond_error( parser, STATUS_NOT_FOUND );
							}
							Multiple_Close_Object( json_buffer, 2U );
							Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
							Ram::De_Allocate( json_buffer );
						}
					}
					else
					{
						hams_respond_error( parser, STATUS_NOT_FOUND );
					}
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
			}
		}
		else
		{
			// _ASSERT( hams_can_respond(data, length) );
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				hams_response_header( parser, HTTP_HDR_ALLOW, Param_RO, STRLEN( Param_RO ) );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
			else if ( Http_Server_End_Dynamic( parser ) )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
		}
	}
	else
	{
		if ( ( true == ENDS_WITH_SLASH( data ) ) || ( true == IS_EXTRA_SLASH( data ) ) )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Get_Bitfield_Description_Cb_Json );
		}
		else
		{
			hams_respond_error( parser, STATUS_NOT_FOUND );
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Get_Json_Param_Single_Item( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
								 uint32_t param, uint32_t verbose )
{
	bool return_val = false;
	bool function_end = false;
	UINT16 tot_length = 0U;
	UINT8 type = 0U;
	UINT8 type_length = 0U;
	DCI_ACCESS_ST_TD access_struct =
	{ 0U };
	/* Write out json tag name and REST parameter id */
	const char_t* tag = s_rest_param_json_tags[param];

	json_response_t* json_buffer = reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
		char_t formatted_buff[BUFFER_LEN];

		memset( formatted_buff, 0U, BUFFER_LEN );

		if ( verbose )
		{
			if ( from_rest == true )
			{
				Add_String_Member( json_buffer, tag, NULL, false );
				Open_Json_Object( json_buffer );
				snprintf( formatted_buff, BUFFER_LEN, "%d", pDCI->param_id );
				Add_String_Member( json_buffer, JSON_KEY_PID, formatted_buff, true );
				snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d%s", pDCI->param_id, s_rest_param_types[param] );
				Add_String_Member( json_buffer, HREF, formatted_buff, true );
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
			}
			else
			{
				Open_Json_Object( json_buffer );
				snprintf( formatted_buff, BUFFER_LEN, "%d", pDCI->param_id );
				Add_String_Member( json_buffer, JSON_KEY_PID, formatted_buff, true );
				snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d%s", pDCI->param_id, s_rest_param_types[param] );
				Add_String_Member( json_buffer, HREF, formatted_buff, true );
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
				if ( !g_flag )
				{
					g_flag = true;
				}

			}
		}
		else
		{
			if ( from_rest == true )
			{
				if ( g_flag == false )
				{
					Add_String_Member( json_buffer, tag, NULL, false );
					g_flag = true;
					Open_Json_Array( json_buffer );
				}
				Open_Json_Object( json_buffer );
				snprintf( formatted_buff, BUFFER_LEN, "%d", pDCI->param_id );
				Add_String_Member( json_buffer, JSON_KEY_PID, formatted_buff, true );
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
			}
			else
			{
				Open_Json_Object( json_buffer );
				snprintf( formatted_buff, BUFFER_LEN, "%d", pDCI->param_id );
				Add_String_Member( json_buffer, JSON_KEY_PID, formatted_buff, true );
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
				if ( !g_flag )
				{
					g_flag = true;
				}
			}
		}

		switch ( param )
		{
			case REST_PARAM_VALUE:
			case REST_PARAM_PRESENTVAL:
				access_struct.command = DCI_ACCESS_GET_RAM_CMD;
				break;

			case REST_PARAM_INITIAL:
				access_struct.command = DCI_ACCESS_GET_INIT_CMD;
				break;

			case REST_PARAM_DEFAULT:
				access_struct.command = DCI_ACCESS_GET_DEFAULT_CMD;
				break;

			// case REST_PARAM_ENUM:
			// access_struct.command = DCI_ACCESS_GET_ENUM_CMD;
			// break;

			default:
				function_end = true;
				return_val = false;
		}

		// do DCI Access here
		if ( false == function_end )
		{
			if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) !=
				 httpd_dci_access->Get_Datatype( pDCI->dcid, ( DCI_DATATYPE_TD* ) ( &type ) ) )
			{
				return_val = false;
			}
			else if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) !=
					  httpd_dci_access->Get_Datatype_Length( pDCI->dcid, &type_length ) )
			{
				return_val = false;
			}
			else if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) !=
					  httpd_dci_access->Get_Length( pDCI->dcid, &tot_length ) )
			{
				return_val = false;
			}
			else
			{
				// Exception_Assert( tot_length <= DEST_BUFFER_SIZE );

				access_struct.data_access.length = tot_length;
				access_struct.source_id = REST_DCI_SOURCE_ID;
				access_struct.data_access.data = j_dest_data;
				access_struct.data_access.offset = 0U;
				access_struct.data_id = pDCI->dcid;

				if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) !=
					 static_cast<uint32_t>( httpd_dci_access->Data_Access( &access_struct ) ) )
				{
					return_val = false;
				}
				else
				{
					// Check if DCID supports any format type
					if ( Format_Handler_Check_Json( parser, pDCI, j_dest_data, tot_length, 0U ) == false )
					{
						// DCID does not attach with any format type
						if ( DCI_DTYPE_STRING8 == type )
						{
							Add_String_Member( json_buffer, JSON_KEY_TEXT, ( char const* )j_dest_data, false );
							if ( g_flag == true )
							{
								Close_Json_Object( json_buffer, false );
							}
							Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
							json_buffer->used_buff_len = 0U;
							memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
						}
						else
						{
							Add_String_Member( json_buffer, JSON_KEY_TEXT, NULL, false );
							Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
							json_buffer->used_buff_len = 0U;
							memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
							Http_Server_Print_Dynamic( parser, "\"" );
							Print_Rest_Format_Json( parser, pDCI, j_dest_data, tot_length, 0U );
							Http_Server_Print_Dynamic( parser, "\"" );
							if ( g_flag == true )
							{
								Http_Server_Print_Dynamic( parser, CLOSE_JSON_OBJECT );
							}
						}
					}
					if ( g_flag == false )
					{
						Multiple_Close_Object( json_buffer, 2U );
						Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
					}
					return_val = true;
				}
			}
		}
		Ram::De_Allocate( json_buffer );
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Print_Rest_Format_Json( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
								 uint8_t* data, uint32_t tot_length, uint32_t offset )
{
	uint16_t arr_cnt = 0U;

	if ( Format_Handler_Check_Json( parser, pDCI, data, tot_length, offset ) == false )
	{
		bool first = true;
		const DCI_DATA_BLOCK_TD* dci_block = DCI::Get_Data_Block( pDCI->dcid );
		DCI_DATATYPE_TD datatype = dci_block->datatype;
		uint8_t datatype_size = DCI::Get_Datatype_Size( datatype );
		while ( tot_length > 0U )
		{
			if ( !first )
			{
				Http_Server_Print_Dynamic( parser, "," );
			}
			else
			{
				first = false;
			}

			if ( false == S_Dynamic_Write_Value_Json( parser, datatype, data, offset ) )
			{
				hams_respond_error( parser, E503_DYNAMIC_SPACE );
			}
			else
			{
				if ( tot_length >= datatype_size )
				{
					tot_length -= datatype_size;
					offset += datatype_size;
					arr_cnt = arr_cnt + datatype_size;
				}
				else
				{
					tot_length = 0U;
				}
			}
		}
	}
	return ( arr_cnt );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Format_Handler_Check_Json( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
								uint8_t* dest_data, uint32_t length, uint32_t offset )
{
	bool status = false;
	uint8_t type = 0U;
	json_response_t* json_buffer = reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	httpd_dci_access->Get_Datatype( pDCI->dcid, ( DCI_DATATYPE_TD* ) ( &type ) );

#ifdef REST_SERVER_FORMAT_HANDLING
	// Check if DCID supports any format type
	if ( ( pDCI->pFormatData != nullptr ) &&
		 ( pDCI->pFormatData->formatter_cback != nullptr ) )
	{
		// CRED type data should be hidden for GET request. Currently DCI callback
		// is returning '*' asterisk string instead of real data.
		// When control comes here, DCI callback has alrready done its job.

		if ( ( STRLEN( "CRED" ) == strlen( pDCI->pFormatData->format_name ) ) &&
			 ( strncmp( pDCI->pFormatData->format_name, "CRED", strlen( "CRED" ) ) == 0 ) )
		{

			if ( json_buffer != nullptr )
			{
				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
				uint16_t size = strnlen( ( const char_t* )dest_data, length );

				// Format Type = CRED()
				if ( size )
				{
					Add_String_Member( json_buffer, JSON_KEY_TEXT, ( const char* )dest_data + offset, false );
				}
				else
				{
					Add_String_Member( json_buffer, JSON_KEY_TEXT, "", false );

				}
				if ( g_flag == true )
				{
					Close_Json_Object( json_buffer, false );
				}
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
			}

			status = true;
		}
		else
		{
			// Format Type = Other than "CRED"
			uint8_t* formatted_string = static_cast<uint8_t*>( Http_Server_Alloc_Scratch(
																   parser, FORMATTED_BUF_BLOCK_NUM ) );
			if ( formatted_string != nullptr )
			{
				uint16_t bytes_used = pDCI->pFormatData->formatter_cback( dest_data + offset, length, formatted_string,
																		  FORMAT_HANDLER_BUFFER_SIZE,
																		  RAW_TO_STRING,
																		  pDCI->pFormatData->config_struct );
				if ( bytes_used > 0 )
				{

					if ( json_buffer != nullptr )
					{
						json_buffer->used_buff_len = 0U;
						memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
						Add_String_Member( json_buffer, JSON_KEY_TEXT, ( const char* )formatted_string, false );
						if ( g_flag == true )
						{
							Close_Json_Object( json_buffer, false );
						}
						Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
					}

					status = true;
				}
				Http_Server_Free( formatted_string );
			}
		}
	}
	Ram::De_Allocate( json_buffer );
#endif
	return ( status );
}

static bool S_Dynamic_Write_Value_Json( hams_parser* parser, UINT8 type,
										const UINT8* pData, UINT32 offset )
{
	bool return_val = true;
	SINT32 s;
	UINT32 v;

	switch ( type )
	{
		case DCI_DTYPE_SINT8:
			s = *( reinterpret_cast<const SINT8*>( &pData[offset] ) );
			if ( s >= 0 )
			{
				Http_Server_Print_Dynamic( parser, "\bu", s );
			}
			else
			{
				Http_Server_Print_Dynamic( parser, "-\bu", -s );
			}
			break;

		case DCI_DTYPE_SINT16:
			s = *( reinterpret_cast<const SINT16*>( &pData[offset] ) );
			if ( s >= 0 )
			{
				Http_Server_Print_Dynamic( parser, "\bu", s );
			}
			else
			{
				Http_Server_Print_Dynamic( parser, "-\bu", -s );
			}
			break;

		case DCI_DTYPE_SINT32:
			s = *( reinterpret_cast<const SINT32*>( &pData[offset] ) );

			if ( s >= 0 )
			{
				Http_Server_Print_Dynamic( parser, "\bu", s );
			}
			else
			{
				Http_Server_Print_Dynamic( parser, "-\bu", -s );
			}
			break;

		case DCI_DTYPE_BOOL:
		case DCI_DTYPE_UINT8:
			v = *( reinterpret_cast<const UINT8*>( &pData[offset] ) );
			Http_Server_Print_Dynamic( parser, "\bu", v );
			break;

		case DCI_DTYPE_UINT16:
			v = *( reinterpret_cast<const UINT16*>( &pData[offset] ) );
			Http_Server_Print_Dynamic( parser, "\bu", v );
			break;

		case DCI_DTYPE_UINT32:
			v = *( reinterpret_cast<const UINT32*>( &pData[offset] ) );

			Http_Server_Print_Dynamic( parser, "\bu", v );
			break;

		case DCI_DTYPE_UINT64:
		{
			UINT64 temp_uint64 = *( reinterpret_cast<const UINT64*>( &pData[offset] ) );
			v = INT_ASCII_Conversion::uint64_to_str( temp_uint64, number_charArr );
			Http_Server_Print_Dynamic( parser, "\bs", v, number_charArr );
			break;
		}

		case DCI_DTYPE_SINT64:
		{
			SINT64 temp_sint64 = *( reinterpret_cast<const SINT64*>( &pData[offset] ) );
			v = INT_ASCII_Conversion::sint64_to_str( temp_sint64, number_charArr );
			Http_Server_Print_Dynamic( parser, "\bs", v, number_charArr );
			break;
		}

		case DCI_DTYPE_FLOAT:
		{
			v = BIN_ASCII_Conversion::float_to_str(
				reinterpret_cast<const uint8_t*>( &pData[offset] ),
				number_charArr );
			Http_Server_Print_Dynamic( parser, "\bs", v, number_charArr );
			break;
		}

		case DCI_DTYPE_DFLOAT:
			v = BIN_ASCII_Conversion::dfloat_to_str(
				reinterpret_cast<const uint8_t*>( &pData[offset] ),
				number_charArr );
			Http_Server_Print_Dynamic( parser, "\bs", v, number_charArr );
			break;

		case DCI_DTYPE_BYTE:
			v = BIN_ASCII_Conversion::byte_to_str(
				reinterpret_cast<const uint8_t*>( &pData[offset] ),
				number_charArr );
			Http_Server_Print_Dynamic( parser, "\bs", v, number_charArr );
			break;

		case DCI_DTYPE_WORD:
			v = BIN_ASCII_Conversion::word_to_str(
				reinterpret_cast<const uint8_t*>( &pData[offset] ),
				number_charArr );
			Http_Server_Print_Dynamic( parser, "\bs", v, number_charArr );
			break;

		case DCI_DTYPE_DWORD:
			v = BIN_ASCII_Conversion::dword_to_str(
				reinterpret_cast<const uint8_t*>( &pData[offset] ),
				number_charArr );
			Http_Server_Print_Dynamic( parser, "\bs", v, number_charArr );
			break;

		case DCI_DTYPE_STRING8:
			Http_Server_Print_Dynamic( parser, "'\bs'", 1, pData + offset );
			break;

		default:
			return_val = false;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Set_Param_Numeric_Json( ReusedRecvBuf* pTarget,
								const struct yjson_put_json_state* ypx,
								uint32_t datatype )
{

	int32_t return_val = 0;

	/* Bounds check the index. */
	if ( ypx->val_index >= ypx->num_of_units )
	{
		return_val = -1;
	}
	else
	{
		switch ( datatype )
		{
			case DCI_DTYPE_SINT8:
				if ( ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) &&
					 ( ypx->tmp_number > VALID_SINT8_MAX ) )
				{
					return_val = -2;
				}
				else if ( !( ypx->flags & YJSON_PUT_IS_NEGATIVE ) &&
						  ( ypx->tmp_number > ( VALID_SINT8_MAX - 1U ) ) )
				{
					return_val = -2;
				}
				else if ( ( ypx->val_index + ( ypx->num_of_units * ypx->curr_num_list ) ) >=
						  ( HTTPD_RECV_BUF_REUSE_U8_PRESENCE ) )
				{
					return_val = -1;
				}
				else
				{
					pTarget->buf.s8_data[( ypx->val_index
										   + ( ypx->num_of_units *
											   ypx->curr_num_list ) )] = ypx->tmp_number;
					if ( 0 != ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) )
					{
						pTarget->buf.s8_data[( ypx->val_index
											   + ( ypx->num_of_units *
												   ypx->curr_num_list ) )] *= -1;
					}
					return_val = 0;
				}
				break;

			case DCI_DTYPE_BOOL:
			case DCI_DTYPE_UINT8:
			case DCI_DTYPE_BYTE:
				if ( 0U != ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) )
				{
					return_val = -2;
				}
				else if ( ypx->tmp_number > VALID_UINT8_MAX )
				{
					return_val = -3;
				}
				else if ( ( ypx->val_index + ( ypx->num_of_units * ypx->curr_num_list ) ) >=
						  ( HTTPD_RECV_BUF_REUSE_U8_PRESENCE ) )
				{
					return_val = -1;
				}
				else
				{
					pTarget->buf.u8_data[( ypx->val_index
										   + ( ypx->num_of_units *
											   ypx->curr_num_list ) )] = ypx->tmp_number;
					return_val = 0;
				}
				break;

			case DCI_DTYPE_SINT16:
				if ( ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) &&
					 ( ypx->tmp_number > VALID_SINT16_MAX ) )
				{
					return_val = -4;
				}
				else if ( !( ypx->flags & YJSON_PUT_IS_NEGATIVE ) &&
						  ( ypx->tmp_number > ( VALID_SINT16_MAX - 1U ) ) )
				{
					return_val = -4;
				}
				else if ( ( ypx->val_index + ( ypx->num_of_units * ypx->curr_num_list ) ) >=
						  ( ( HTTPD_RECV_BUF_REUSE_U16_PRESENCE ) ) )
				{
					return_val = -1;
				}
				else
				{
					pTarget->buf.s16_data[( ypx->val_index
											+ ( ypx->num_of_units *
												ypx->curr_num_list ) )] = static_cast<
						int16_t>( ypx
								  ->
								  tmp_number );
					if ( 0 != ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) )
					{
						pTarget->buf.s16_data[( ypx->val_index
												+ ( ypx->num_of_units *
													ypx->curr_num_list ) )] *= -1;
					}
					return_val = 0;
				}
				break;

			case DCI_DTYPE_UINT16:
			case DCI_DTYPE_WORD:
				if ( 0U != ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) )
				{
					return_val = -2;
				}
				else if ( ypx->tmp_number > VALID_UINT16_MAX )
				{
					return_val = -5;
				}
				else if ( ( ypx->val_index + ( ypx->num_of_units * ypx->curr_num_list ) ) >=
						  ( ( HTTPD_RECV_BUF_REUSE_U16_PRESENCE ) ) )
				{
					return_val = -1;
				}
				else
				{
					pTarget->buf.u16_data[( ypx->val_index
											+ ( ypx->num_of_units *
												ypx->curr_num_list ) )] = ypx->tmp_number;
					return_val = 0;
				}
				break;

			case DCI_DTYPE_SINT32:
				if ( ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) &&
					 ( ypx->tmp_number > VALID_SINT32_MAX ) )
				{
					return_val = -6;
				}
				else if ( !( ypx->flags & YJSON_PUT_IS_NEGATIVE ) &&
						  ( ypx->tmp_number > ( VALID_SINT32_MAX - 1U ) ) )
				{
					return_val = -6;
				}
				else if ( ( ypx->val_index + ( ypx->num_of_units * ypx->curr_num_list ) ) >=
						  ( ( HTTPD_RECV_BUF_REUSE_U32_PRESENCE ) ) )
				{
					return_val = -1;
				}
				else
				{
					pTarget->buf.s32_data[( ypx->val_index
											+ ( ypx->num_of_units *
												ypx->curr_num_list ) )] = ypx->tmp_number;
					if ( ypx->flags & YJSON_PUT_IS_NEGATIVE )
					{
						pTarget->buf.s32_data[( ypx->val_index
												+ ( ypx->num_of_units *
													ypx->curr_num_list ) )] *= -1;
					}
					return_val = 0;
				}
				break;

			case DCI_DTYPE_UINT32:
			case DCI_DTYPE_DWORD:
				if ( 0U != ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) )
				{
					return_val = -2;
				}
				else if ( ypx->tmp_number > VALID_UINT32_MAX )
				{
					return_val = -7;
				}
				else if ( ( ypx->val_index + ( ypx->num_of_units * ypx->curr_num_list ) ) >=
						  HTTPD_RECV_BUF_REUSE_U32_PRESENCE )
				{
					return_val = -1;
				}
				else
				{
					pTarget->buf.u32_data[( ypx->val_index
											+ ( ypx->num_of_units *
												ypx->curr_num_list ) )] = ypx->tmp_number;
					return_val = 0;
				}
				break;

			case DCI_DTYPE_SINT64:
				if ( ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) &&
					 ( ypx->tmp_number > VALID_SINT64_MAX ) )
				{
					return_val = -8;
				}
				else if ( !( ypx->flags & YJSON_PUT_IS_NEGATIVE ) &&
						  ( ypx->tmp_number > ( VALID_SINT64_MAX - 1U ) ) )
				{
					return_val = -8;
				}
				else if ( ( ypx->val_index + ( ypx->num_of_units * ypx->curr_num_list ) ) >=
						  HTTPD_RECV_BUF_REUSE_U64_PRESENCE )
				{
					return_val = -1;
				}
				else
				{
					pTarget->buf.s64_data[( ypx->val_index
											+ ( ypx->num_of_units *
												ypx->curr_num_list ) )] = ypx->tmp_number;
					if ( ypx->flags & YJSON_PUT_IS_NEGATIVE )
					{
						pTarget->buf.s64_data[( ypx->val_index
												+ ( ypx->num_of_units *
													ypx->curr_num_list ) )] *= -1;
					}
					return_val = 0;
				}
				break;

			case DCI_DTYPE_FLOAT:
			case DCI_DTYPE_DFLOAT:
			case DCI_DTYPE_UINT64:
				if ( ( datatype == DCI_DTYPE_UINT64 ) &&
					 ( 0U != ( ypx->flags & YJSON_PUT_IS_NEGATIVE ) ) )
				{
					return_val = -2;
				}
				else if ( ( ypx->val_index + ( ypx->num_of_units * ypx->curr_num_list ) ) >=
						  static_cast<uint8_t>( ( HTTPD_RECV_BUF_REUSE_U64_PRESENCE ) ) )
				{
					return_val = -1;
				}
				else
				{
					pTarget->buf.u64_data[( ypx->val_index
											+ ( ypx->num_of_units *
												ypx->curr_num_list ) )] = ypx->tmp_number;
					return_val = 0;
				}
				break;

			default:
				// _ASSERT( 0 );
				break;
		}
	}

	return ( return_val );
}

uint32_t Rest_Other_Handler_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	return ( S_Rest_Other_Handler_Json( parser, data, length ) );
}