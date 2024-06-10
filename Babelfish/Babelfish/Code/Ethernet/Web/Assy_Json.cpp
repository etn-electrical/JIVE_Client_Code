/**
 *****************************************************************************************
 * @file Assy_Json.cpp
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
#include "REST_DCI_Data.h"
#include "server.h"
#include "REST.h"
#include "StdLib_MV.h"
#include "REST_Common.h"
#include "Babelfish_Assert.h"
#include "Assy_Json.h"
#include "REST_Json.h"
#include "iot_json_parser.h"
#include "Yjson.h"

#define HAMS_DEBUG
#ifdef HAMS_DEBUG
// #define DERR(...) fprintf(stderr, __VA_ARGS__)
#define DERR( ... )
#define _ASSERT( x ) do{ if ( !( x ) )while ( 1 ){} \
}while ( 0 )
#else
#define DERR( ... )
#define _ASSERT( x )
#endif

#define  Assy_Get_All  "GET, HEAD, OPTIONS"
#define  Assy_Get_Values  "GET, HEAD, PUT, OPTIONS"
#define  Assy_Get_Param_PREDEF "GET, HEAD, OPTIONS"
#define  Assy_Get_Param_CUSTOM  "GET, HEAD, DELETE, OPTIONS"
#define  Assy_Param_PREDEF     "GET, HEAD, OPTIONS"
#define   Assy_Param_CUSTOM   "GET, HEAD, PUT, POST, OPTIONS"

// assembly types
static const uint8_t ASSY_INVALID = 0U;
static const uint8_t ASSY_PREDEF = 1U;
static const uint8_t ASSY_CUSTOM = 2U;

static const uint16_t CONST_ONE = 1U;

// assembly parse status
enum assy_status_t
{
	NOT_VERIFIED,
	TAG_FOUND,
	VERIFIED,
	FIRST_PARAM_FOUND,
	PARSE_COMPLETE
};

typedef struct
{
	uint16_t start;
	uint16_t end;
	uint16_t param;
	present_t sep[DATA_PRESENT_SIZE];
} assy_param_t;

typedef struct
{
	enum assy_status_t status;
	bool param_value_cmd;
	bool param_found;
	uint8_t max_params;
	uint16_t param_index;
	uint16_t buff_size;
	assy_param_t* assy_data;
} assy_parsed_data_t;

#define ASSY_PARAMS_CONST_DATA_LEN  STRLEN( XML_DECL ) \
	+ STRLEN( XML_ASSY_PARAM_ALL_PREFIX ) \
	+ STRLEN( XML_ASSY_ALL_LINE_SEG2 ) \
	+ STRLEN( XML_LIST_RO ) \
	+ STRLEN( XML_ASSY_ONE ) \
	+ STRLEN( XML_OPEN_SLOTS ) \
	+ STRLEN( XML_ASSY_ZERO ) \
	+ STRLEN( XML_RACCESS ) \
	+ STRLEN( QUOTE_XML_WACCESS ) \
	+ STRLEN( QUOTE_GT_EOL ) \
	+ STRLEN( XML_ASSY_CLOSE_TAG )

/* Function prototypes. */
static uint32_t Assy_Delete_Param_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static void Get_Assy_All_List_Json( hams_parser* parser, http_server_req_state* rs );

static uint32_t Assy_Param_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief Callback for /rs/assy request
 * @details This function is called on "GET /rs/assy{a}" request.
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return Success or failure of requested operation.
 */
static uint32_t Assy_Get_Param_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Assy_Put_Values_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief Callback for /rs/assy request
 * @details This function is called on /rs/assy request, For "PUT /rs/assy" and "POST /rs/assy" request should add and
 * update the parameter from the produced assembly
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return Success or failure of requested operation.
 */
static uint32_t Assy_Put_Parse_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint16_t Put_Params_Into_Assy( const REST_CUSTOM_ASSY_TD* pCustomAssy, http_server_req_state* rs );

static uint16_t Post_Params_Into_Assy( const REST_CUSTOM_ASSY_TD* pCustomAssy, http_server_req_state* rs );

static const REST_PREDEFINED_ASSY_TD* Find_Predefined_Assy_Json( uint16_t aid );

static const REST_CUSTOM_ASSY_TD* Find_Custom_Assy_Json( uint16_t aid );

static bool Is_Param_In_Predefined_Assy( uint16_t pid, const REST_PREDEFINED_ASSY_TD* pPredefinedAssy );

static bool Is_Param_In_Custom_Assy( uint16_t pid, const REST_CUSTOM_ASSY_TD* pCustomAssy );

static uint16_t Get_Custom_Assy_NumOfOpenSlots( uint16_t bitmap );

static uint8_t Get_Valid_Numbers_From_Bitmap( uint16_t bitmap );

static bool Get_Valid_Bitmap( const REST_CUSTOM_ASSY_TD* pCustomAssy, uint16_t* pBitmap );

static void Add_Assy_Line_Json( hams_parser* parser, uint16_t aid, const uint8_t* p_name, uint8_t name_len );

static bool Delete_Param_From_Assy_Json( uint16_t pid, const REST_CUSTOM_ASSY_TD* pCustomAssy );

static uint32_t Assy_Get_Values_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

static bool Get_Custom_Assy_Param_Values_Json( hams_parser* parser, const REST_CUSTOM_ASSY_TD* pCustomAssy );

static bool Get_Predefined_Assy_Param_Values_Json( hams_parser* parser,
												   const REST_PREDEFINED_ASSY_TD* pPredefinedAssy );

static void Get_Predefined_Assy_Params_Json( hams_parser* parser, const REST_PREDEFINED_ASSY_TD* pPredefinedAssy );

static void Get_Custom_Assy_Params_Json( hams_parser* parser, const REST_CUSTOM_ASSY_TD* pCustomAssy );

static const int8_t PARAM_TAG_1[] = "Assy";
static const int8_t PARAM_TAG_2[] = "Param";
static const int8_t PARAM_VAL_TAG_1[] = "ValueList";
static const int8_t PARAM_VAL_TAG_2[] = "Value";

static const int8_t* ASSY_TAGS[2][2] =
{
	{ PARAM_TAG_1,
	  PARAM_TAG_2
	},
	{
		PARAM_VAL_TAG_1,
		PARAM_VAL_TAG_2
	}
};

static DCI_SOURCE_ID_TD httpd_source_id = DCI_SOURCE_IDS_Get();

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Assy_Get_All_Callback_Json( hams_parser* parser,
									 const uint8_t* data, uint32_t length )
{
	uint16_t num_of_assy = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/* Look for all match. */
	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )	// '/'=> ASCII 0x2FU
	{
		parser->user_data_cb = &Assy_Get_All_Callback_Json;
	}
	else
	{
		if ( ( 0U != rs->max_auth_exempt_level ) || ( HTTP_METHOD_OPTIONS == parser->method ) )
		{
			if ( ( HTTP_METHOD_GET == parser->method ) || ( HTTP_METHOD_HEAD == parser->method ) ||
				 ( HTTP_METHOD_OPTIONS == parser->method ) )
			{
				if ( parser->content_length == 0U )
				{
					if ( 0U != hams_check_headers_ended( data, length ) )
					{
						parser->response_code = RESPONSE_OK;
						rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
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
							num_of_assy = httpd_rest_dci::getTotalNumberOfAssemblies();

							Http_Server_Begin_Dynamic( parser );

							json_response_t* json_buffer =
								reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

							if ( json_buffer != nullptr )
							{
								json_buffer->used_buff_len = 0U;
								memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

								char_t formatted_buff[BUFFER_LEN];
								memset( formatted_buff, 0, BUFFER_LEN );

								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, "AssyList", NULL, false );
								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, HREF, "/rs/assy", true );
								Add_Integer_Member( json_buffer, "AssySlotsTotal", num_of_assy, true );
								Add_String_Member( json_buffer, "AssySlotsLeft", "0", true );
								Add_String_Member( json_buffer, "Assy", NULL, false );
								Open_Json_Array( json_buffer );
								Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								Ram::De_Allocate( json_buffer );
							}

							Get_Assy_All_List_Json( parser, rs );
						}
					}
					else
					{
						if ( 0U != hams_can_respond( data, length ) )
						{
							if ( HTTP_METHOD_OPTIONS == parser->method )
							{
								hams_response_header( parser, HTTP_HDR_ALLOW, Assy_Get_All,
													  STRLEN( Assy_Get_All ) );
								hams_response_header( parser, COUNT_HTTP_HDRS,
													  reinterpret_cast<const void*>( NULL ), 0U );
							}
							else if ( 0U != Http_Server_End_Dynamic( parser ) )
							{
								hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
													  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );

								hams_response_header( parser, COUNT_HTTP_HDRS,
													  nullptr, 0U );
							}
							else
							{}
						}
						else
						{
							hams_respond_error( parser, ERROR_BAD_REQUEST );
						}
					}
				}
				else
				{
					hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
			}
		}
		else
		{
			hams_respond_error( parser, ERROR_UNAUTHORIZED );
		}
	}
	return ( 0U );
}

static void Get_Assy_All_List_Json( hams_parser* parser, http_server_req_state* rs )
{
	uint16_t num_predefined;
	uint16_t num_custom;
	uint16_t assy_index = 0U;

	json_response_t* json_buffer =
		reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

		num_predefined = httpd_rest_dci::getNumberofPredefinedAssemblies();
		num_custom = httpd_rest_dci::getNumberofCustomAssemblies();

		while ( assy_index < num_predefined )
		{
			if ( httpd_rest_dci::includedInPredefinedAssyAll( assy_index, rs->max_auth_exempt_level ) )
			{
				Add_Assy_Line_Json( parser, httpd_rest_dci::getPredefinedAssyId( assy_index ),
									httpd_rest_dci::getPredefinedAssyName( assy_index ),
									httpd_rest_dci::getPredefinedAssyNameLen( assy_index ) );

				if ( ( assy_index < ( num_predefined - 1U ) ) || ( num_custom != 0U ) )
				{
					Http_Server_Print_Dynamic( parser, JSON_ADD_COMMA );
				}

			}
			assy_index++;
		}

		assy_index = 0U;

		// finishes predefined, starts custom
		while ( assy_index < num_custom )
		{
			if ( httpd_rest_dci::includedInCustomAssyAll( assy_index, rs->max_auth_exempt_level ) )
			{
				Add_Assy_Line_Json( parser,
									httpd_rest_dci::getCustomAssyId( assy_index ),
									httpd_rest_dci::getCustomAssyName( assy_index ),
									httpd_rest_dci::getCustomAssyNameLen( assy_index ) );

				if ( assy_index < ( num_custom - 1U ) )
				{
					Http_Server_Print_Dynamic( parser, JSON_ADD_COMMA );
				}

			}
			assy_index++;
		}

		Close_Json_Array( json_buffer, false );
		Multiple_Close_Object( json_buffer, 2 );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
		Ram::De_Allocate( json_buffer );
	}
}

static void Add_Assy_Line_Json( hams_parser* parser, uint16_t aid, const uint8_t* p_name,
								uint8_t name_len )
{
	json_response_t* json_buffer =
		reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0, BUFFER_LEN );

		Open_Json_Object( json_buffer );
		Add_Integer_Member( json_buffer, "aid", aid, true );
		Add_String_Member( json_buffer, "name", ( char const* )p_name, true );
		Add_String_Member( json_buffer, "del_supp", "0", true );
		snprintf( formatted_buff, BUFFER_LEN, "/rs/assy/%d", aid );
		Add_String_Member( json_buffer, HREF, formatted_buff, false );
		Close_Json_Object( json_buffer, false );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
		Ram::De_Allocate( json_buffer );
	}
}

static uint32_t Assy_Get_Values_Callback_Json( hams_parser* parser, const uint8_t* data,
											   uint32_t length )
{
	/* Look for all match. */
	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )	// '/'=> ASCII 0x2FU
	{
		parser->user_data_cb = &Assy_Get_Values_Callback_Json;
	}
	else
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( parser->content_length == 0U )
			{
				http_server_req_state* rs = Http_Server_Get_State( parser );
				parser->response_code = RESPONSE_OK;
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
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
					Http_Server_Begin_Dynamic( parser );
				}
				if ( HTTP_METHOD_OPTIONS != parser->method )
				{
					if ( rs->p.rest.assy_type == ASSY_PREDEF )
					{
						if ( rs->max_auth_exempt_level <
							 ( reinterpret_cast < REST_PREDEFINED_ASSY_TD * > ( rs->p.rest.assy_ptr ) )
							 ->access_r )
						{
							hams_respond_error( parser, ERROR_UNAUTHORIZED );
						}
						else
						{
							if ( !Get_Predefined_Assy_Param_Values_Json( parser,
																		 reinterpret_cast<REST_PREDEFINED_ASSY_TD*>( rs
																													 ->p
																													 .
																													 rest
																													 .
																													 assy_ptr ) ) )
							{
								hams_respond_error( parser, ERROR_INTERNAL_SERVER );
							}
						}
					}
					else
					{
						if ( rs->max_auth_exempt_level <
							 ( reinterpret_cast < REST_CUSTOM_ASSY_TD * > ( rs->p.rest.assy_ptr ) )
							 ->access_r )
						{
							hams_respond_error( parser, ERROR_UNAUTHORIZED );
						}
						else
						{
							if ( !Get_Custom_Assy_Param_Values_Json( parser,
																	 reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p.rest.
																											 assy_ptr ) ) )
							{
								hams_respond_error( parser, ERROR_INTERNAL_SERVER );
							}
						}

					}
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
			}
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					hams_response_header( parser, HTTP_HDR_ALLOW, Assy_Get_Values,
										  STRLEN( Assy_Get_Values ) );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
				else if ( 0U != Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  nullptr, 0U );
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

static uint32_t Assy_Get_Param_Callback_Json( hams_parser* parser, const uint8_t* data,
											  uint32_t length )
{
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = nullptr;

	uint16_t error_code = RESPONSE_OK;

	/* Look for all match. */
	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )	// '/'=> ASCII 0x2FU
	{
		parser->user_data_cb = &Assy_Get_Param_Callback_Json;
	}
	else
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			if ( parser->content_length == 0U )
			{
				if ( HTTP_METHOD_OPTIONS != parser->method )
				{
					if ( rs->p.rest.assy_type == ASSY_PREDEF )
					{
						if ( rs->max_auth_exempt_level <
							 ( reinterpret_cast < REST_PREDEFINED_ASSY_TD * > ( rs->p.rest.assy_ptr ) )
							 ->access_r )
						{
							error_code = ERROR_UNAUTHORIZED;
						}
					}
					else
					{
						if ( rs->max_auth_exempt_level <
							 ( reinterpret_cast < REST_CUSTOM_ASSY_TD * > ( rs->p.rest.assy_ptr ) )
							 ->access_r )
						{
							error_code = ERROR_UNAUTHORIZED;
						}
					}
				}
				if ( error_code == RESPONSE_OK )
				{
					pDCI = reinterpret_cast<DCI_REST_TO_DCID_LKUP_ST_TD*>( rs->p.rest.ptr );
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						Validate_Options_Cors_Headers( parser, rs, nullptr );
					}
					else
					{
						if ( ( nullptr != pDCI ) &&
							 ( pDCI->access_r <= rs->max_auth_exempt_level ) )
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
								memset( formatted_buff, 0, BUFFER_LEN );

								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, "Param", NULL, false );
								Open_Json_Object( json_buffer );
								Add_Integer_Member( json_buffer, "pid", pDCI->param_id, true );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d", pDCI->param_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, false );
								Multiple_Close_Object( json_buffer, 2 );
								Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								Ram::De_Allocate( json_buffer );
							}
						}
						else
						{
							error_code = ERROR_UNAUTHORIZED;
						}
					}
				}
			}
			else
			{
				error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
			}

			if ( error_code != RESPONSE_OK )
			{
				hams_respond_error( parser, error_code );
			}
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					http_server_req_state* rs = Http_Server_Get_State( parser );
					if ( rs->p.rest.assy_type == ASSY_PREDEF )
					{
						hams_response_header( parser, HTTP_HDR_ALLOW, Assy_Get_Param_PREDEF,
											  STRLEN( Assy_Get_Param_PREDEF ) );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
					else
					{
						hams_response_header( parser, HTTP_HDR_ALLOW, Assy_Get_Param_CUSTOM,
											  STRLEN( Assy_Get_Param_CUSTOM ) );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
				else if ( 0U != Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );

					hams_response_header( parser, COUNT_HTTP_HDRS,
										  nullptr, 0U );
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

static bool Get_Predefined_Assy_Param_Values_Json( hams_parser* parser,
												   const REST_PREDEFINED_ASSY_TD* pPredefinedAssy )
{
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = nullptr;
	bool status = true;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	uint32_t param = rs->p.rest.rest_param;

	json_response_t* json_buffer =
		reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0, BUFFER_LEN );

		Open_Json_Object( json_buffer );
		Add_String_Member( json_buffer, "ValueList", NULL, false );
		Open_Json_Object( json_buffer );
		Add_Integer_Member( json_buffer, "aid", pPredefinedAssy->assy_id, true );
		Add_String_Member( json_buffer, "Value", NULL, false );
		Open_Json_Array( json_buffer );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

		json_buffer->used_buff_len = 0U;
		memset( json_buffer, 0U, JSON_MAXBUFF_LEN );

		for ( uint16_t i = 0U; ( i < pPredefinedAssy->number_of_parameters ) && ( status == true );
			  i++ )
		{
			pDCI = httpd_rest_dci::findIndexREST( pPredefinedAssy->pParams[i] );

			if ( nullptr != pDCI )
			{
				if ( pDCI->access_r <= rs->max_auth_exempt_level )
				{
					status = Get_Json_Param_Single_Item( parser, pDCI, param, 0U );
					if ( i < ( pPredefinedAssy->number_of_parameters - 1U ) )
					{
						Http_Server_Print_Dynamic( parser, JSON_ADD_COMMA );
					}
				}
				else
				{
					hams_respond_error( parser, ERROR_UNAUTHORIZED );
				}
			}
		}
		Close_Json_Array( json_buffer, false );
		Multiple_Close_Object( json_buffer, 2 );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
		Ram::De_Allocate( json_buffer );
	}

	return ( status );
}

static bool Get_Custom_Assy_Param_Values_Json( hams_parser* parser,
											   const REST_CUSTOM_ASSY_TD* pCustomAssy )
{
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = nullptr;
	DCI_ACCESS_ST_TD access_struct;
	uint16_t bitmap = 0U;
	uint16_t pid = 0U;
	bool status = true;

	http_server_req_state* rs = Http_Server_Get_State( parser );
	uint32_t param = rs->p.rest.rest_param;

	json_response_t* json_buffer =
		reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0, BUFFER_LEN );
		bool l_print = false;

		Open_Json_Object( json_buffer );
		Add_String_Member( json_buffer, "ValueList", NULL, false );
		Open_Json_Object( json_buffer );
		Add_Integer_Member( json_buffer, "aid", pCustomAssy->assy_id, true );
		Add_String_Member( json_buffer, "Value", NULL, false );
		Open_Json_Array( json_buffer );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

		json_buffer->used_buff_len = 0U;
		memset( json_buffer, 0U, JSON_MAXBUFF_LEN );

		access_struct.data_access.length = 2U;
		access_struct.source_id = httpd_source_id;
		access_struct.data_access.data = &bitmap;
		access_struct.data_access.offset = 0U;
		access_struct.data_id = pCustomAssy->validBitMapDCID;
		access_struct.command = DCI_ACCESS_GET_RAM_CMD;

		if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
		{
			for ( uint16_t i = 0U; ( i < MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY ) && ( status == true ); i++ )
			{
				if ( ( bitmap & static_cast<uint16_t>( CONST_ONE << i ) ) != 0U )
				{
					access_struct.data_access.data = &pid;
					access_struct.data_access.offset = static_cast<uint16_t>( i << CONST_ONE );
					access_struct.data_id = pCustomAssy->paramListDCID;
					access_struct.command = DCI_ACCESS_GET_RAM_CMD;

					if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
					{
						pDCI = httpd_rest_dci::findIndexREST( pid );

						if ( nullptr != pDCI )
						{
							if ( pDCI->access_r <= rs->max_auth_exempt_level )
							{
								if ( ( i < MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY ) && l_print )
								{
									Http_Server_Print_Dynamic( parser, JSON_ADD_COMMA );
								}
								l_print = true;
								status = Get_Json_Param_Single_Item( parser, pDCI, param, 1U );
							}
							else
							{
								hams_respond_error( parser, ERROR_UNAUTHORIZED );
							}
						}
						else
						{
							status = false;
						}
					}
					else
					{
						status = false;
					}
				}
			}
			Close_Json_Array( json_buffer, false );
			Multiple_Close_Object( json_buffer, 2 );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

		}
		else
		{
			status = false;
		}
		Ram::De_Allocate( json_buffer );
	}

	return ( status );
}

static uint32_t Assy_Put_Parse_Callback_Json( hams_parser* parser,
											  const uint8_t* data, uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yjson_put_json_state* ypx = reinterpret_cast<struct yjson_put_json_state*>( rs->p.rest.yjson );
	const DCI_DATA_BLOCK_TD* data_block = nullptr;
	uint16_t error_code = RESPONSE_OK;
	jsmn_parser j_parser;
	int16_t result = 0;
	static int16_t cp_result = 0;
	uint16_t index = 0;
	jsmntok_t tokens[BUFFER_LEN];
	static char_t* p_msg_buffer = nullptr;
	static uint16_t msg_cnt = 1;
	static char_t json_value[JSON_VALUE_LEN];
	bool json_attr_value = 0;
	bool json_attr_end = 0;
	bool json_content = 0;
	bool json_end = 0;
	bool jAid = 0;

	/* Make we are reading the correct type. Form URL Encoded is not the optimal way to do this...
	 * should be using HTTP_CONTENT_TYPE_APPLICATION_JSON...oh well */

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		assy_parsed_data_t* assy_p_data =
			reinterpret_cast<assy_parsed_data_t*>( rs->p.rest.put_request );
		uint8_t* p_data = reinterpret_cast<uint8_t*>( rs->p.rest.put_request )
			+ ( ( assy_p_data->max_params * sizeof( assy_param_t ) ) + sizeof( assy_parsed_data_t ) );

		ReusedRecvBuf* pTarget = ( ReusedRecvBuf* )( p_data + assy_p_data->buff_size );

		FormatBuf* format_buff =
			reinterpret_cast<FormatBuf*>( p_data + assy_p_data->buff_size + RECV_TOTAL_BUF_SIZE );
		const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = reinterpret_cast<DCI_REST_TO_DCID_LKUP_ST_TD*>( rs->p.rest.ptr );

		if ( parser->content_type == HTTP_CONTENT_TYPE )
		{

			if ( length > 0U )
			{
				if ( ( assy_p_data->param_value_cmd == true ) &&
					 ( assy_p_data->status == FIRST_PARAM_FOUND ) )
				{
					if ( rs->p.rest.ptr != NULL )
					{
						pDCI = reinterpret_cast<DCI_REST_TO_DCID_LKUP_ST_TD*>( rs->p.rest.ptr );
						if ( nullptr != pDCI )
						{
							data_block = DCI::Get_Data_Block( pDCI->dcid );
							if ( data_block == NULL )
							{
								error_code = STATUS_NOT_FOUND;
							}
						}
						else
						{
							error_code = STATUS_NOT_FOUND;
						}
					}
					else
					{
						error_code = STATUS_NOT_FOUND;
					}
				}

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
					jsmn_parse( &j_parser, ( char_t* ) p_msg_buffer, msgLen, tokens,
								sizeof( tokens ) / sizeof( tokens[0] ) );

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
						error_code = E400_JSON_SYNTAX_ERROR;
					}
				}

				for ( index = 1; ( ( index < result ) && ( error_code == 200 ) ); index++ )
				{
					size_t token_size = tokens[index + 1].end - tokens[index + 1].start;

					if ( assy_p_data->status < TAG_FOUND )
					{
						if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index],
									 ( reinterpret_cast<const char_t*>( ASSY_TAGS[assy_p_data->param_value_cmd][0] ) ) )
							 ==
							 0 )
						{
							assy_p_data->status = TAG_FOUND;
						}
						if ( error_code == RESPONSE_OK )
						{
							assy_p_data->param_found = true;

							assy_p_data->assy_data[assy_p_data->param_index].param = 0U;

							if ( assy_p_data->param_value_cmd == true )
							{
								if ( assy_p_data->param_index == 0U )
								{
									assy_p_data->assy_data[assy_p_data->param_index].start = 0U;
								}
								else
								{
									assy_p_data->assy_data[assy_p_data->param_index].start =
										assy_p_data->assy_data[assy_p_data->param_index - 1U].end;
								}
							}
						}
					}
					else
					{
						if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index], JSON_KEY_AID ) == 0 )
						{
							memset( json_value, 0, JSON_VALUE_LEN );
							strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );
							json_attr_value = 1;
							json_attr_end = 1;
							json_content = 0;
							jAid = 1;
						}
						if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index], JSON_KEY_PID ) == 0 )
						{
							memset( json_value, 0, JSON_VALUE_LEN );
							strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );
							json_attr_value = 1;
							json_attr_end = 1;
							json_content = 0;

							if ( jAid )
							{
								json_end = 0;
							}
							else
							{
								json_end = 1;
							}

							if ( error_code == RESPONSE_OK )
							{
								assy_p_data->param_found = true;

								assy_p_data->assy_data[assy_p_data->param_index].param = 0U;

								if ( assy_p_data->param_value_cmd == true )
								{
									if ( assy_p_data->param_index == 0U )
									{
										assy_p_data->assy_data[assy_p_data->param_index].start = 0U;
									}
									else
									{
										assy_p_data->assy_data[assy_p_data->param_index].start =
											assy_p_data->assy_data[assy_p_data->param_index - 1U].end;
									}
								}
							}
						}
						if ( jsoneq( ( char_t* )p_msg_buffer, &tokens[index], JSON_KEY_TEXT ) == 0 )
						{
							memset( json_value, 0, JSON_VALUE_LEN );
							strncpy( json_value, p_msg_buffer + tokens[index + 1].start, token_size );
							json_content = 1;
							json_end = 1;
							json_attr_value = 0;
							json_attr_end = 1;
						}
					}

					if ( json_content == 1 )
					{
						json_content = 0;
						uint16_t k = 0;

						if ( ( pDCI->pFormatData == nullptr ) || ( pDCI->pFormatData->format_name != nullptr
#ifdef REST_SERVER_FORMAT_HANDLING
																   && pDCI->pFormatData->formatter_cback == nullptr
#endif
																   ) )
						{
							const uint8_t* c = reinterpret_cast<uint8_t*>( json_value );
							while ( ( *c != 0U ) && ( error_code == RESPONSE_OK ) )
							{
								if ( data_block != nullptr )
								{
									if ( DCI_DTYPE_STRING8 == data_block->datatype )
									{
										if ( ypx->put_state == 0U )
										{
											++ypx->put_state;
											PTARGET_SET_DATA_PRESENT( pTarget->present, 0U );
											ypx->base64_bits = 0U;
											ypx->base64.all = 0U;
										}

										if ( ( ypx->flags & YJSON_PUT_END ) != 0U )
										{
											error_code = ERROR_BAD_REQUEST;
										}
										else
										{
											if ( ( YJSON_PUT_IS_BASE64 & ypx->flags ) != 0U )
											{
												uint8_t val = 0x80U;
												if ( IS_UPALPHA( *c ) )
												{
													val = *c - 0x41U;

												}
												else if ( IS_LOALPHA( *c ) )
												{
													val = ( *c - 0x61U ) + 26U;
												}
												else if ( IS_DIGIT( *c ) )
												{
													val = ( *c - 0x30U ) + 52U;
												}
												else if ( 0x2BU == *c )
												{
													val = 62U;
												}
												else if ( 0x2FU == *c )
												{
													val = 63U;
												}
												else if ( 0x3DU == *c )
												{
													/* No-op. */
													ypx->flags |= YJSON_PUT_BASE64_PAD;
													if ( 2U == ypx->base64_bits )
													{
														/* Don't overflow the buffer. */
														if ( ypx->val_index >=
															 *data_block->length_ptr )
														{
															error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
														}
														else
														{
															uint32_t compose =
																( static_cast<uint32_t>( ypx
																						 ->base64.buff[0] )
																	<< 18U )
																| ( static_cast<uint32_t>( ypx
																						   ->base64
																						   .buff[1] )
																	<< 12U );
															ypx->base64.all = compose;

															pTarget->buf.u8_data[ypx
																				 ->val_index] =
																ypx->base64.buff[2];
															ypx->val_index++;
														}
													}
													else if ( 3U == ypx->base64_bits )
													{
														/* Don't overflow the buffer. */
														if ( ( ypx->val_index + CONST_ONE ) >=
															 static_cast<uint16_t>( *data_block
																					->length_ptr ) )
														{
															error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
														}
														else
														{
															uint32_t compose =
																( static_cast<uint32_t>( ypx
																						 ->base64.buff[0] )
																	<< 18U )
																| ( static_cast<uint32_t>( ypx
																						   ->base64
																						   .buff[1] )
																	<< 12U )
																| ( static_cast<uint32_t>( ypx
																						   ->base64
																						   .buff[2] )
																	<< 6U );
															ypx->base64.all = compose;

															pTarget->buf.u8_data[ypx
																				 ->val_index] =
																ypx->base64.buff[2];
															ypx->val_index++;
															pTarget->buf.u8_data[ypx
																				 ->val_index] =
																ypx->base64.buff[1];
															ypx->val_index++;
														}
													}
													else
													{
														// comment for Misrafication
													}
													ypx->base64_bits = 0U;
												}
												else if ( 0x22U == *c )
												{
													/* Found the end without any padding..
													 * must not have any further characters.*/
													ypx->flags |= YJSON_PUT_END;
												}
												else
												{
													error_code = ERROR_BAD_REQUEST;
												}
												if ( error_code == RESPONSE_OK )
												{
													if ( val < 0x80U )
													{
														/* Cannot have more B64 chars after pad appears.*/
														if ( ( ypx->flags
															   & YJSON_PUT_BASE64_PAD ) !=
															 0U )
														{
															error_code = ERROR_BAD_REQUEST;
														}
														else
														{
															ypx->base64.buff[ypx
																			 ->base64_bits] =
																val;
															++ypx->base64_bits;
															if ( 4U == ypx->base64_bits )
															{
																if ( ( ypx->val_index
																	   + static_cast<uint16_t>( 2U ) ) >=
																	 *data_block
																	 ->length_ptr )
																{
																	error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
																}
																else
																{
																	uint32_t compose =
																		( static_cast<uint32_t>( ypx
																								 ->base64
																								 .buff[0] )
																			<< 18U )
																		| ( static_cast<uint32_t>( ypx
																								   ->base64
																								   .buff[1] )
																			<< 12U )
																		| ( static_cast<uint32_t>( ypx
																								   ->base64
																								   .buff[2] )
																			<< 6U )
																		| ( static_cast<uint32_t>( ypx
																								   ->base64
																								   .buff[3] ) );
																	ypx->base64.all =
																		compose;

																	pTarget->buf
																	.u8_data[ypx
																			 ->val_index] =
																		ypx->base64.buff[2];
																	ypx->val_index++;
																	pTarget->buf
																	.u8_data[ypx
																			 ->val_index] =
																		ypx->base64.buff[1];
																	ypx->val_index++;
																	pTarget->buf
																	.u8_data[ypx
																			 ->val_index] =
																		ypx->base64.buff[0];
																	ypx->val_index++;
																	ypx->base64_bits = 0U;
																	ypx->base64.all = 0U;
																}
															}
														}
													}
												}
											}
											else
											{
												if ( ypx->val_index >= *data_block->length_ptr )
												{
													error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
												}
												else
												{
													/* Append character data to */
													pTarget->buf.u8_data[ypx->val_index] =
														*c;
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
											PTARGET_SET_DATA_PRESENT( pTarget->present,
																	  ypx->val_index
																	  + ( ypx->num_of_units *
																		  ypx->curr_num_list ) );
											if ( 0x30U != *c )													// '0'
											{
												if ( ( 0x78U == *c ) || ( 0x58U == *c ) )	/* 'x' || 'X'	*/
												{
													if ( ( ypx->flags
														   & YJSON_PUT_HAS_LEADING_ZERO ) ==
														 0U )
													{
														error_code = ERROR_BAD_REQUEST;
													}
													else
													{
														/* Clear leading zero flag. */
														ypx->flags = YJSON_PUT_IS_HEX;
														++ypx->put_state;
													}
												}
												else if ( 0x2DU == *c )	// '-'
												{
													/* No flags should be set. */
													if ( ypx->flags != 0U )
													{
														error_code = ERROR_BAD_REQUEST;
													}
													else
													{
														ypx->flags |= YJSON_PUT_IS_NEGATIVE;
														++ypx->put_state;
													}
												}
												else if ( ( 0x31U <= *c ) && ( *c <= 0x39U ) )	// '1' && '9'
												{
													ypx->tmp_number =
														static_cast<uint64_t>( *c )
														- 0x30U;
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
													if ( YJSON_PUT_HAS_LEADING_ZERO ==
														 ypx->flags )
													{
														/* This just zero, save it. */
														ypx->tmp_number = 0U;
														int32_t ret =
															Set_Param_Numeric_Json( pTarget, ypx,
																					static_cast<uint32_t>( data_block
																										   ->datatype ) );
														if ( -1 == ret )
														{
															error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
														}
														else if ( ret < 0 )
														{
															error_code = ERROR_BAD_REQUEST;
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
															if ( ypx->separator == 0x3AU )
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
														if ( ypx->flags != 0U )
														{
															error_code = ERROR_BAD_REQUEST;
														}
														else
														{
															/* empty value. */
															PTARGET_CLEAR_DATA_PRESENT(
																pTarget->present,
																ypx->val_index
																+ ( ypx->num_of_units
																	* ypx->curr_num_list ) );
															++ypx->val_index;
														}
													}
												}
												else
												{
													error_code = ERROR_BAD_REQUEST;
												}
											}
											else
											{
												ypx->flags |= YJSON_PUT_HAS_LEADING_ZERO;
											}
										}
										else
										{
											PTARGET_SET_DATA_PRESENT( pTarget->present,
																	  ypx->val_index
																	  + ( ypx->num_of_units *
																		  ypx->curr_num_list ) );
											if ( ( static_cast<uint8_t>( *c ) == ypx->separator ) ||
												 ( ( static_cast<uint8_t>( *c ) ==
													 GLOBAL_ARR_SEPARATOR ) &&
												   ( ypx->max_num_list > ypx->curr_num_list ) &&
												   ( ( ypx->val_index + 1 ) == ypx->num_of_units ) ) )
											{
												if ( ( ypx->tmp_number > 0U ) ||
													 ( ( ypx->flags
														 & YJSON_PUT_HAS_LEADING_ZERO ) >
													   0U ) )
												{
													/* Save the number. */
													int32_t ret =
														Set_Param_Numeric_Json( pTarget, ypx,
																				static_cast<uint32_t>( data_block
																									   ->datatype ) );
													if ( -1 == ret )
													{
														error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
													}
													else if ( ret < 0 )
													{
														error_code = ERROR_BAD_REQUEST;
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
														/* If parsing a MAC address, basically assume we already read
														   the next 0x (MAC address excludes hex prefix). */
														if ( ypx->separator == 0x3AU )
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
												uint8_t v = *c;
												if ( v < 0x30U )	// '0'
												{
													error_code = ERROR_BAD_REQUEST;
												}
												else if ( v > 0x39U )	// '9'
												{
													if ( ( ypx->flags & YJSON_PUT_IS_HEX ) > 0U )
													{
														/* HEX LOWER CASE */
														if ( ( v >= 0x61U ) && ( v <= 0x66U ) )
														{
															v -= 0x61U - 10U;
														}
														else if ( ( v >= 0x41U ) && ( v <= 0x46U ) )/* HEX UPPER CASE */
														{
															v -= 0x41U - 10U;
														}
														else
														{
															error_code = ERROR_BAD_REQUEST;
														}
													}
													else
													{
														error_code = ERROR_BAD_REQUEST;
													}
												}
												else
												{
													v -= 0x30U;	// '0'
												}

												if ( error_code == RESPONSE_OK )
												{
													if ( ( ypx->flags & YJSON_PUT_IS_HEX ) > 0U )
													{
														ypx->tmp_number <<= 4U;
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
									++c;
								}
								else
								{
									error_code = STATUS_NOT_FOUND;
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
									error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
								}
							}
						}
					}

					if ( json_attr_value == 1 )
					{
						json_attr_value = 0;
						assy_p_data->param_found = true;
						if ( assy_p_data->param_found == true )
						{
							uint32_t param = assy_p_data->assy_data[assy_p_data->param_index].param;

							uint16_t k = 0;
							while ( json_value[k] != NULL )
							{
								if ( IS_DIGIT( json_value[k] ) )
								{
									param *= 10U;
									param += ( json_value[k] - 0x30 );

									if ( param > 0xFFFF )
									{
										error_code = ERROR_BAD_REQUEST;
									}
									else
									{
										assy_p_data->assy_data[assy_p_data->param_index].param =
											static_cast<uint16_t>( param );
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

					if ( json_attr_end == 1 )
					{
						json_attr_end = 0;
						if ( assy_p_data->param_found == true )
						{
							assy_p_data->param_found = false;

							if ( assy_p_data->param_index >=
								 assy_p_data->max_params )
							{
								assy_p_data->param_index = 0U;
								error_code = ERROR_FORBIDDEN;
							}
							else
							{
								if ( assy_p_data->param_value_cmd == true )
								{
									if ( assy_p_data->status < VERIFIED )
									{
										if ( rs->p.rest.assy_type == ASSY_PREDEF )
										{
											if ( ( reinterpret_cast<REST_PREDEFINED_ASSY_TD*>
												   ( rs->p.rest.assy_ptr ) )->assy_id ==
												 assy_p_data->assy_data[assy_p_data->param_index].param )
											{
												assy_p_data->status = VERIFIED;
											}
											else
											{
												error_code = ERROR_BAD_REQUEST;
											}
										}
										else
										{
											if ( ( reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p
																						   .rest.assy_ptr ) )->
												 assy_id ==
												 assy_p_data->assy_data[assy_p_data->param_index].param )
											{
												assy_p_data->status = VERIFIED;
											}
											else
											{
												error_code = ERROR_BAD_REQUEST;
											}
										}
									}
									else
									{
										bool param_in_assy = false;
										// first check and see if it's part of the assy
										if ( rs->p.rest.assy_type == ASSY_PREDEF )
										{
											param_in_assy =
												Is_Param_In_Predefined_Assy(
													assy_p_data->assy_data[assy_p_data
																		   ->param_index].param,
													reinterpret_cast<REST_PREDEFINED_ASSY_TD*>( rs
																								->p
																								.rest.assy_ptr ) );
										}
										else
										{
											param_in_assy = Is_Param_In_Custom_Assy(
												assy_p_data->assy_data[assy_p_data
																	   ->param_index].param,
												reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p
																						.rest
																						.assy_ptr ) );
										}

										if ( param_in_assy == true )
										{
											pDCI = httpd_rest_dci::findIndexREST(
												assy_p_data->assy_data[assy_p_data
																	   ->param_index].param );
											rs->p.rest.ptr = ( void* )pDCI;			/* not taking reinterpret+cast */
											assy_p_data->status = FIRST_PARAM_FOUND;

											if ( pDCI != nullptr )
											{
												if ( ( pDCI->pFormatData == nullptr ) ||
													 ( pDCI->pFormatData->format_name != nullptr
#ifdef REST_SERVER_FORMAT_HANDLING
													   && pDCI->pFormatData->formatter_cback == nullptr
#endif
													 ) )
												{
													data_block = DCI::Get_Data_Block( pDCI->dcid );
													ypx->flags = 0U;

													if ( DCI_DTYPE_STRING8 !=
														 data_block->datatype )
													{
														ypx->separator = 0x2CU;
														ypx->num_of_units =
															static_cast<uint8_t>( *( data_block
																					 ->length_ptr )
																				  / ( DCI::Get_Datatype_Size(
																						  data_block
																						  ->
																						  datatype ) ) );
													}
													else
													{
														ypx->num_of_units = 1U;
														ypx->separator = 0x0U;
														ypx->max_num_list = 1U;
													}

													if ( DCI_DTYPE_STRING8 != data_block->datatype )
													{
														ypx->max_num_list = ( ( *( data_block
																				   ->length_ptr ) )
																			  / ( DCI::Get_Datatype_Size(
																					  data_block->datatype ) ) )
															/ ypx->num_of_units;
													}
													ypx->curr_num_list = 0;

													ypx->tmp_number = 0U;
													ypx->val_index = 0U;
													if ( ypx->separator == 0x3AU )
													{
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
													/* Reset format buffer index */
													format_buff->byte_index = 0;
												}
											}
											Initialize_pTarget_Buffer( pTarget );
										}
										else
										{
											error_code = STATUS_NOT_FOUND;
										}
									}
								}
							}
						}
					}

					if ( json_end == 1 )
					{
						json_end = 0;

						if ( assy_p_data->param_value_cmd == true )
						{
							if ( ( ( pDCI != nullptr ) && ( pDCI->pFormatData == nullptr ) ) ||
								 ( pDCI->pFormatData->format_name != nullptr
#ifdef REST_SERVER_FORMAT_HANDLING
								   && pDCI->pFormatData->formatter_cback == nullptr
#endif
								 ) )
							{
								if ( DCI_DTYPE_STRING8 == data_block->datatype )
								{
									if ( ( assy_p_data->assy_data[assy_p_data->param_index]
										   .start + ypx->val_index ) < assy_p_data->buff_size )
									{
										assy_p_data->assy_data[assy_p_data->param_index].end =
											assy_p_data->assy_data[assy_p_data->param_index]
											.start + ypx->val_index;
									}
									else
									{
										error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
									}
								}
								else
								{
									/* ypx->val_index is a count of separators. Can only have 1 less than number of
									   units.*/
									if ( ypx->val_index >= ypx->num_of_units )
									{
										error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
									}
									else
									{
										if ( PTARGET_DATA_PRESENT( pTarget->present,
																   ypx->val_index
																   + ( ypx->num_of_units *
																	   ypx->curr_num_list ) ) )
										{
											int32_t ret =
												Set_Param_Numeric_Json( pTarget, ypx,
																		static_cast<uint32_t>( data_block->datatype ) );
											if ( -1 == ret )
											{
												error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
											}
											else if ( ret < 0 )
											{
												error_code = ERROR_BAD_REQUEST;
											}
											else
											{
												// comments to avoid misra warning
											}
										}
										if ( error_code == RESPONSE_OK )
										{
											++ypx->val_index;

											uint8_t data_type_size = DCI::Get_Datatype_Size(
												data_block->datatype );

											assy_p_data->assy_data[assy_p_data->param_index].end =
												assy_p_data->assy_data[assy_p_data->param_index].start
												+ ( static_cast<uint16_t>( ypx->val_index +
																		   ( ypx->num_of_units *
																			 ypx->curr_num_list ) ) * data_type_size );
										}
									}
								}

								if ( error_code == RESPONSE_OK )
								{
									if ( assy_p_data->assy_data[assy_p_data->param_index].end >
										 assy_p_data->buff_size )
									{
										error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
									}
									else
									{
										uint16_t j = 0U;
										for ( uint16_t k =
												  assy_p_data->assy_data[assy_p_data->param_index]
												  .start;
											  k <
											  assy_p_data->assy_data[assy_p_data
																	 ->param_index].end; k++ )
										{
											p_data[k] = pTarget->buf
												.u8_data[j];
											j++;
										}

										Clone_pTarget_Present_Data( assy_p_data->assy_data[assy_p_data->
																						   param_index].sep,
																	pTarget->present, DATA_PRESENT_SIZE );

										for ( uint32_t k = 0U;
											  k < ( ypx->num_of_units * ypx->max_num_list );
											  k++ )
										{
											PTARGET_CLEAR_DATA_PRESENT( pTarget->present, k );
										}
									}
								}
							}
							else if ( ( pDCI != nullptr ) && ( pDCI->pFormatData != nullptr ) )
							{
								// First terminate string since we reached to state:YJSON_ELEMEND
								format_buff->buf[format_buff->byte_index] = '\0';
								data_block = DCI::Get_Data_Block( pDCI->dcid );

								if ( ( STRLEN( "CRED" ) == strlen( pDCI->pFormatData->format_name ) ) &&
									 ( strncmp( pDCI->pFormatData->format_name, "CRED",
												strlen( "CRED" ) ) == 0 ) )
								{
									// Format type = CRED, thus we do not need format handler support.
									const uint16_t DCI_LENGTH =
										reinterpret_cast<uint16_t>( *data_block->length_ptr );
									uint16_t filtered_length =
										strnlen( const_cast<const char*>( reinterpret_cast<char*>( format_buff->buf ) ),
												 DCI_LENGTH );

									if ( ( DCI_LENGTH <= filtered_length ) ||
										 ( assy_p_data->buff_size <
										   ( assy_p_data->assy_data[assy_p_data->param_index].start +
											 format_buff->byte_index ) ) )
									{
										error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
									}
									else
									{
										// Copy raw data into pTarget
										BF_Lib::Copy_String( p_data +
															 assy_p_data->assy_data[assy_p_data->param_index].start,
															 format_buff->buf, filtered_length );

										assy_p_data->assy_data[assy_p_data->param_index].end =
											assy_p_data->assy_data[assy_p_data->param_index].start +
											format_buff->byte_index;

										// Thus write 1 in present data structure indicating that one element
										// received.
										PTARGET_SET_DATA_PRESENT( assy_p_data->assy_data[assy_p_data->
																						 param_index].sep,
																  0 );
									}
								}
#ifdef REST_SERVER_FORMAT_HANDLING
								else if ( pDCI->pFormatData->formatter_cback != nullptr )
								{
									// Call Format handler for STRING_TO_RAW conversion only if
									// 1. When format handler call back is attached
									// 2. When format type is other than CRED
									// First terminate string since we reached to state:YJSON_ELEMEND
									uint16_t total_raw_bytes = pDCI->pFormatData->formatter_cback(
										p_data + assy_p_data->assy_data[assy_p_data->param_index].start,
										reinterpret_cast<uint16_t>( *data_block->length_ptr ),
										format_buff->buf, FORMATTED_BUF_SIZE,
										STRING_TO_RAW, pDCI->pFormatData->config_struct );

									if ( total_raw_bytes > 0 )
									{
										if ( DCI_DTYPE_STRING8 != data_block->datatype )
										{
											uint16_t datatype_size = DCI::Get_Datatype_Size(
												data_block->datatype );

											// Generate nuber of 1's equal to number of elements
											for ( uint16_t i = 0; i < total_raw_bytes / datatype_size; i++ )
											{
												PTARGET_SET_DATA_PRESENT( assy_p_data->
																		  assy_data[assy_p_data->param_index].sep,
																		  i );
											}
										}
										else
										{
											// Only 1 element received since data type is string
											PTARGET_SET_DATA_PRESENT( assy_p_data->
																	  assy_data[assy_p_data->param_index].sep,
																	  0 );
										}

										if ( ( assy_p_data->buff_size >=
											   assy_p_data->assy_data[assy_p_data->param_index].start +
											   total_raw_bytes ) )
										{
											assy_p_data->assy_data[assy_p_data->param_index].end =
												assy_p_data->assy_data[assy_p_data->param_index].start +
												total_raw_bytes;
											// assy_p_data->param_index++;
										}
										else
										{
											error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
										}
									}
									else
									{
										// Format conversion failed
										error_code = ERROR_BAD_REQUEST;
									}
								}
#endif
								else
								{
									// Control comes here when format attached with DCI parameter
									// without creating Format_Handler()
									BF_ASSERT( false );
								}
							}
							else
							{
								error_code = ERROR_BAD_REQUEST;
							}
						}
						assy_p_data->param_index++;
					}
					if ( index == result - 1 )
					{
						assy_p_data->status = PARSE_COMPLETE;
						jAid = 0;
					}
				}

				if ( error_code == RESPONSE_OK )
				{
					if ( assy_p_data->status == PARSE_COMPLETE )
					{
						parser->response_code = RESPONSE_OK;

						// check for duplicate parameters in received content
						for ( uint16_t k = 0U;
							  ( k < assy_p_data->param_index ) && ( error_code == RESPONSE_OK ); k++ )
						{
							for ( uint16_t j = k + 1U;
								  ( j < assy_p_data->param_index ) && ( error_code == RESPONSE_OK );
								  j++ )
							{
								if ( assy_p_data->assy_data[k].param == assy_p_data->assy_data[j].param )
								{
									error_code = ERROR_BAD_REQUEST;
								}
							}
						}

						if ( error_code == RESPONSE_OK )
						{
							if ( parser->method == HTTP_METHOD_PUT )
							{
								if ( assy_p_data->param_value_cmd == true )
								{
									for ( uint16_t ind = 0U;
										  ( ind < assy_p_data->param_index ) &&
										  ( error_code == RESPONSE_OK );
										  ind++ )
									{
										pDCI = httpd_rest_dci::findIndexREST(
											assy_p_data->assy_data[ind].param );
										if ( nullptr == pDCI )
										{
											error_code = STATUS_NOT_FOUND;
										}
										else
										{
											if ( pDCI->access_w <= rs->max_auth_exempt_level )
											{
												data_block = DCI::Get_Data_Block( pDCI->dcid );

												// now we can try to write into the parameter
												if ( NULL != data_block )
												{
													if ( assy_p_data->assy_data[ind].end <=
														 assy_p_data->assy_data[ind].start )
													{
														error_code = ERROR_BAD_REQUEST;
													}
													else
													{
														pDCI = httpd_rest_dci::findIndexREST(
															assy_p_data->assy_data[ind].param );

														Clone_pTarget_Present_Data( pTarget->present,
																					assy_p_data->assy_data[ind].sep,
																					DATA_PRESENT_SIZE );

														uint16_t j = 0U;
														for ( uint16_t k = assy_p_data->assy_data[ind].start;
															  k < assy_p_data->assy_data[ind].end; k++ )
														{
															pTarget->buf.u8_data[j] = p_data[k];
															j++;
														}
														pTarget->buf.u8_data[j] = 0x0U;

														error_code = putParamValue( pDCI, pTarget,
																					data_block, true,
																					rs->p.rest.rest_param );
													}
												}
												else
												{
													error_code = STATUS_NOT_FOUND;
												}
											}
											else
											{
												error_code = ERROR_UNAUTHORIZED;
											}
										}
									}
								}
								else
								{
									error_code =
										Put_Params_Into_Assy(
											reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p.rest
																					.assy_ptr ), rs );
								}
							}
							else
							{
								error_code = Post_Params_Into_Assy(
									reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p.rest.assy_ptr ),
									rs );
							}
						}
					}
				}
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
		else
		{
			error_code = ERROR_UNSUPPORTED_MEDIA_TYPE;
		}

		if ( ( error_code != RESPONSE_OK ) || ( assy_p_data->status == PARSE_COMPLETE ) )
		{
			msg_cnt = 1;
			Ram::De_Allocate( p_msg_buffer );
			Http_Server_Free( rs->p.rest.put_request );
			if ( error_code != RESPONSE_OK )
			{
				hams_respond_error( parser, error_code );
			}
		}
	}
	else
	{
		if ( ( 0U != hams_can_respond( data, length ) ) && ( parser->response_code != 0U ) )
		{
			hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
			hams_response_header( parser, COUNT_HTTP_HDRS, nullptr, 0U );
		}
		else
		{
			hams_respond_error( parser, ERROR_BAD_REQUEST );
		}
	}

	return ( 0U );
}

static uint16_t Put_Params_Into_Assy( const REST_CUSTOM_ASSY_TD* pCustomAssy,
									  http_server_req_state* rs )
{
	DCI_ACCESS_ST_TD access_struct = { 0U };
	uint16_t bitmap = 0U;
	uint16_t i = 0U;
	uint16_t pid = 0U;
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = nullptr;
	uint16_t error_code = RESPONSE_OK;
	bool loop_break = false;

	access_struct.data_access.length = 2U;
	access_struct.source_id = httpd_source_id;
	access_struct.data_access.data = &bitmap;
	access_struct.data_access.offset = 0U;
	access_struct.data_id = pCustomAssy->validBitMapDCID;
	access_struct.command = DCI_ACCESS_SET_RAM_CMD;

	if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
	{
		access_struct.command = DCI_ACCESS_SET_INIT_CMD;

		if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
		{
			assy_parsed_data_t* assy_p_data = reinterpret_cast<assy_parsed_data_t*>( rs->p.rest
																					 .put_request );

			for ( i = 0U; ( i < assy_p_data->param_index ) && ( loop_break == false ); i++ )
			{
				pid = assy_p_data->assy_data[i].param;

				pDCI = httpd_rest_dci::findIndexREST( pid );

				if ( nullptr != pDCI )
				{
					if ( rs->max_auth_exempt_level >= pDCI->access_r )
					{
						access_struct.data_access.data = &pid;
						access_struct.data_access.offset = static_cast<uint16_t>( i << CONST_ONE );
						access_struct.data_id = pCustomAssy->paramListDCID;
						access_struct.command = DCI_ACCESS_SET_RAM_CMD;

						if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
						{
							access_struct.command = DCI_ACCESS_SET_INIT_CMD;

							if ( DCI_ERR_NO_ERROR ==
								 httpd_dci_access->Data_Access( &access_struct ) )
							{
								bitmap |= static_cast<uint16_t>( CONST_ONE << i );
							}
							else
							{
								loop_break = true;
							}
						}
						else
						{
							loop_break = true;
						}
					}
					else
					{
						loop_break = true;
						error_code = ERROR_UNAUTHORIZED;
					}
				}
				else
				{
					error_code = STATUS_NOT_FOUND;
				}
			}

			if ( loop_break == false )
			{
				access_struct.data_access.data = &bitmap;
				access_struct.data_access.offset = 0U;
				access_struct.data_id = pCustomAssy->validBitMapDCID;
				access_struct.command = DCI_ACCESS_SET_RAM_CMD;

				if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
				{
					access_struct.command = DCI_ACCESS_SET_INIT_CMD;

					if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
					{}
				}
			}
		}
	}
	return ( error_code );
}

static uint16_t Post_Params_Into_Assy( const REST_CUSTOM_ASSY_TD* pCustomAssy,
									   http_server_req_state* rs )
{
	uint8_t max_allowed = 0U;
	uint8_t used = 0U;
	uint16_t bitmap = 0U;
	uint16_t new_bitmap = 0U;
	uint16_t i = 0U;
	uint16_t j = 0U;
	uint16_t pid = 0U;
	DCI_ACCESS_ST_TD access_struct = { 0U };
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = nullptr;
	bool duplicate = false;
	bool findSlot = false;
	uint16_t error_code = RESPONSE_OK;
	bool loop_break = false;

	if ( true == Get_Valid_Bitmap( pCustomAssy, &bitmap ) )
	{
		max_allowed =
			httpd_rest_dci::getRestStruct()->max_number_of_paramters_in_custom_assembly;
		used = Get_Valid_Numbers_From_Bitmap( bitmap );

		if ( max_allowed <
			 ( used
			   + ( reinterpret_cast < assy_parsed_data_t * > ( rs->p.rest.put_request )->param_index ) ) )
		{
			error_code = ERROR_FORBIDDEN;
		}
		else
		{
			new_bitmap = bitmap;

			assy_parsed_data_t* assy_p_data = reinterpret_cast<assy_parsed_data_t*>( rs->p.rest
																					 .put_request );

			for ( i = 0U; ( i < assy_p_data->param_index ) && ( loop_break == false ); i++ )
			{
				access_struct.data_access.length = 2U;
				access_struct.source_id = httpd_source_id;
				access_struct.data_access.data = &pid;
				access_struct.data_access.offset = 0U;
				access_struct.data_id = pCustomAssy->paramListDCID;
				access_struct.command = DCI_ACCESS_GET_RAM_CMD;

				duplicate = false;
				// check for duplicates
				for ( j = 0U; ( j < MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY ) && ( duplicate == false );
					  j++ )
				{
					if ( ( bitmap & static_cast<uint16_t>( CONST_ONE << j ) ) != 0U )
					{
						access_struct.data_access.offset = static_cast<uint16_t>( j << CONST_ONE );
						if ( DCI_ERR_NO_ERROR ==
							 httpd_dci_access->Data_Access( &access_struct ) )
						{
							if ( pid == assy_p_data->assy_data[i].param )
							{
								duplicate = true;
							}
						}
					}
				}

				if ( duplicate == false )
				{
					pDCI = httpd_rest_dci::findIndexREST( assy_p_data->assy_data[i].param );

					if ( nullptr != pDCI )
					{
						if ( rs->max_auth_exempt_level >= pDCI->access_r )
						{
							// find empty slots
							findSlot = false;
							for ( j = 0U; j < MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY; j++ )
							{
								if ( ( new_bitmap & static_cast<uint16_t>( CONST_ONE << j ) ) ==
									 0U )
								{
									access_struct.data_access.offset = static_cast<uint16_t>( j
																							  << CONST_ONE );
									access_struct.data_access.data =
										&assy_p_data->assy_data[i].param;
									access_struct.command = DCI_ACCESS_SET_RAM_CMD;

									if ( DCI_ERR_NO_ERROR ==
										 httpd_dci_access->Data_Access( &access_struct ) )
									{
										access_struct.command = DCI_ACCESS_SET_INIT_CMD;
										if ( DCI_ERR_NO_ERROR ==
											 httpd_dci_access->Data_Access( &access_struct ) )
										{
											new_bitmap |= static_cast<uint16_t>( CONST_ONE << j );
											findSlot = true;
											break;
										}
									}
								}
							}
							if ( findSlot == false )
							{
								loop_break = true;
							}
						}
						else
						{
							error_code = ERROR_UNAUTHORIZED;
							loop_break = true;
						}
					}
					else
					{
						error_code = STATUS_NOT_FOUND;
					}
				}
			}

			if ( ( error_code == RESPONSE_OK ) && ( loop_break == false ) )
			{
				access_struct.data_access.offset = 0U;
				access_struct.data_access.data = &new_bitmap;
				access_struct.command = DCI_ACCESS_SET_RAM_CMD;
				access_struct.data_id = pCustomAssy->validBitMapDCID;

				if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
				{
					access_struct.command = DCI_ACCESS_SET_INIT_CMD;
					if ( DCI_ERR_NO_ERROR != httpd_dci_access->Data_Access( &access_struct ) )
					{}
				}
			}
		}
	}
	return ( error_code );
}

static uint32_t Assy_Param_Values_Callback_Json( hams_parser* parser, const uint8_t* data,
												 uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;

	/* Look for root match. */
	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )	// '/'=> ASCII 0x2FU
	{
		if ( data[length] != 0x2FU )
		{
			parser->user_data_cb = &Assy_Param_Callback_Json;
		}
		else
		{
			parser->user_data_cb = &Assy_Param_Values_Callback_Json;
		}
	}
	else
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );

		/* Look for 'values'*/
		if ( ( length == 7U ) &&
			 ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/values", length ) == 0 ) )
		{
			/* Only GET or HEAD is valid. */
			if ( ( HTTP_METHOD_GET == parser->method ) ||
				 ( HTTP_METHOD_HEAD == parser->method ) ||
				 ( HTTP_METHOD_OPTIONS == parser->method ) )
			{
				rs->p.rest.rest_param = REST_PARAM_VALUE;
				parser->user_data_cb = &Assy_Get_Values_Callback_Json;
			}
			else if ( HTTP_METHOD_PUT == parser->method )
			{
				rs->p.rest.rest_param = REST_PARAM_VALUE;
				parser->user_data_cb = &Assy_Put_Values_Callback_Json;
			}
			else
			{
				error_code = ERROR_METHOD_NOT_ALLOWED;
			}
		}
		else
		{
			/* Expecting a decimal integer, 0-65535 */
			uint16_t val = 0U;
			if ( ERR_PARSING_OK
				 ==
				 INT_ASCII_Conversion::str_to_uint16( ( data ), 1U,
													  length - 1U, &val ) )
			{
				if ( ( HTTP_METHOD_GET == parser->method ) ||
					 ( HTTP_METHOD_HEAD == parser->method ) ||
					 ( HTTP_METHOD_DELETE == parser->method ) ||
					 ( HTTP_METHOD_OPTIONS == parser->method ) )
				{
					if ( rs->p.rest.assy_type == ASSY_CUSTOM )
					{
						if ( !Is_Param_In_Custom_Assy( val,
													   reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p.rest.assy_ptr ) ) )
						{
							error_code = STATUS_NOT_FOUND;
						}
					}
					else
					{
						if ( HTTP_METHOD_DELETE != parser->method )
						{
							if ( !Is_Param_In_Predefined_Assy( val,
															   reinterpret_cast<REST_PREDEFINED_ASSY_TD*>( rs->p.rest.
																										   assy_ptr ) ) )
							{
								error_code = STATUS_NOT_FOUND;
							}
						}
						else
						{
							error_code = ERROR_METHOD_NOT_ALLOWED;
						}
					}

					if ( error_code == RESPONSE_OK )
					{
						const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = httpd_rest_dci::findIndexREST(
							val );

						if ( pDCI != nullptr )
						{
							rs->p.rest.ptr = ( void* )pDCI;	// not taking interpret_cast

							if ( HTTP_METHOD_DELETE == parser->method )
							{
								parser->user_data_cb = &Assy_Delete_Param_Callback_Json;
							}
							else
							{
								parser->user_data_cb = &Assy_Get_Param_Callback_Json;
							}
						}
						else
						{
							error_code = STATUS_NOT_FOUND;
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
				error_code = ERROR_BAD_REQUEST;
			}
		}
		if ( error_code != RESPONSE_OK )
		{
			hams_respond_error( parser, error_code );
		}
	}
	return ( 0U );
}

static uint32_t Assy_Param_Callback_Json( hams_parser* parser, const uint8_t* data,
										  uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;

	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )	// '/'=> ASCII 0x2FU
	{
		parser->user_data_cb = &Assy_Param_Callback_Json;
	}
	else
	{
		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( rs->p.rest.assy_type == ASSY_PREDEF )
			{
				if ( parser->method != HTTP_METHOD_OPTIONS )
				{
					if ( rs->max_auth_exempt_level <
						 ( reinterpret_cast < REST_PREDEFINED_ASSY_TD * > ( rs->p.rest.assy_ptr ) )
						 ->access_r )
					{
						error_code = ERROR_UNAUTHORIZED;
					}
				}
			}
			else if ( rs->p.rest.assy_type == ASSY_CUSTOM )
			{
				if ( parser->method != HTTP_METHOD_OPTIONS )
				{
					if ( rs->max_auth_exempt_level <
						 ( reinterpret_cast < REST_CUSTOM_ASSY_TD * > ( rs->p.rest.assy_ptr ) )
						 ->access_r )
					{
						error_code = ERROR_UNAUTHORIZED;
					}
				}
			}
			else
			{
				error_code = ERROR_BAD_REQUEST;
			}

			if ( error_code == RESPONSE_OK )
			{
				if ( ( HTTP_METHOD_GET == parser->method ) ||
					 ( HTTP_METHOD_HEAD == parser->method ) ||
					 ( HTTP_METHOD_OPTIONS == parser->method ) )
				{
					if ( parser->content_length != 0U )
					{
						error_code = ERROR_REQUEST_ENTITY_TOO_LARGE;
					}
					else
					{
						parser->response_code = RESPONSE_OK;
						rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
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
							parser->blocks_for_data = 0U;
							parser->data_transmitted = 0U;
							Http_Server_Begin_Dynamic( parser );

							if ( rs->p.rest.assy_type == ASSY_PREDEF )
							{
								Get_Predefined_Assy_Params_Json( parser,
																 reinterpret_cast<REST_PREDEFINED_ASSY_TD*>( rs->p.rest.
																											 assy_ptr ) );
							}
							else
							{
								Get_Custom_Assy_Params_Json( parser,
															 reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p.rest.assy_ptr ) );
							}
						}

					}
				}
				else if ( ( ( HTTP_METHOD_PUT == parser->method ) ||
							( HTTP_METHOD_POST == parser->method ) ) &&
						  ( rs->p.rest.assy_type == ASSY_CUSTOM ) )
				{
					if ( parser->content_length != 0U )
					{
						/* Make we are reading the correct type. Form URL Encoded is not the optimal way to do this...
						 * should be using HTTP_CONTENT_TYPE_APPLICATION_JSON...oh well */
						if ( parser->content_type == HTTP_CONTENT_TYPE )
						{
							uint8_t max_param_allowed = rest_dci_data_target_info
								.max_number_of_paramters_in_custom_assembly;

							/* Allocate ourselves a block for json parsing. */
							rs->p.rest.yjson = Http_Server_Alloc_Json_Parser( parser, 32U );

							if ( rs->p.rest.yjson != NULL )
							{
								uint8_t number_of_blocks = ( ( ( max_param_allowed
																 * sizeof( assy_param_t ) ) +
															   sizeof( assy_parsed_data_t ) )
															 + ( HTTP_POOL_BLOCK_SIZE - 1U ) ) / HTTP_POOL_BLOCK_SIZE;

								assy_parsed_data_t* meta_data =
									reinterpret_cast<assy_parsed_data_t*>( Http_Server_Alloc_Scratch( parser,
																									  number_of_blocks ) );

								if ( meta_data != NULL )
								{
									struct yjson_put_json_state* ypx =
										reinterpret_cast<struct yjson_put_json_state*>( rs->p.rest.yjson );

									ypx->flags = 0U;

									rs->p.rest.put_request = ( void* )meta_data;
									meta_data->param_value_cmd = false;
									meta_data->status = NOT_VERIFIED;
									meta_data->param_found = false;
									meta_data->param_index = 0U;
									meta_data->buff_size = 0U;
									meta_data->max_params = max_param_allowed;
									meta_data->assy_data =
										reinterpret_cast<assy_param_t*>( reinterpret_cast<uint8_t*>( meta_data )
																		 + sizeof( assy_parsed_data_t ) );
									parser->user_data_cb = &Assy_Put_Parse_Callback_Json;
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
					error_code = ERROR_METHOD_NOT_ALLOWED;
				}
			}
			if ( error_code != RESPONSE_OK )
			{
				hams_respond_error( parser, error_code );
			}
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					if ( rs->p.rest.assy_type == ASSY_PREDEF )
					{
						hams_response_header( parser, HTTP_HDR_ALLOW, Assy_Param_PREDEF,
											  STRLEN( Assy_Param_PREDEF ) );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
					else
					{
						hams_response_header( parser, HTTP_HDR_ALLOW, Assy_Param_CUSTOM,
											  STRLEN( Assy_Param_CUSTOM ) );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
				else if ( 0U != Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  nullptr, 0U );
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
uint32_t Assy_Index_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;

	/* Look for all match. */
	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )	// '/'=> ASCII 0x2FU
	{
		if ( data[length] == 0x2FU )
		{
			parser->user_data_cb = &Assy_Index_Callback_Json;
		}
		else
		{
			parser->user_data_cb = &Assy_Get_All_Callback_Json;
		}
	}
	else
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );

		rs->p.rest.assy_type = ASSY_INVALID;

		/* Expecting a decimal integer, 0-65535 */
		uint16_t value = 0U;
		if ( ERR_PARSING_OK
			 !=
			 INT_ASCII_Conversion::str_to_uint16( ( data ), 1U,
												  length - static_cast<uint16_t>( 1U ), &value ) )
		{
			error_code = ERROR_BAD_REQUEST;
		}
		else
		{
			rs->p.rest.assy_ptr = ( void* )Find_Predefined_Assy_Json( value );

			if ( rs->p.rest.assy_ptr == NULL )
			{
				rs->p.rest.assy_ptr = ( void* )Find_Custom_Assy_Json( value );

				if ( rs->p.rest.assy_ptr == NULL )
				{
					error_code = STATUS_NOT_FOUND;
				}
				else
				{
					rs->p.rest.assy_type = ASSY_CUSTOM;
				}
			}
			else
			{
				rs->p.rest.assy_type = ASSY_PREDEF;
			}

			if ( error_code == RESPONSE_OK )
			{
				if ( 0x2FU != data[length] )
				{
					parser->user_data_cb = &Assy_Param_Callback_Json;
				}
				else
				{
					parser->user_data_cb = &Assy_Param_Values_Callback_Json;
				}
			}
		}
		if ( error_code != RESPONSE_OK )
		{
			hams_respond_error( parser, error_code );
		}
	}
	return ( 0U );
}

static uint32_t Get_Predefined_Assy_Params_Remaining( hams_parser* parser, const uint8_t* data,
													  uint32_t length )
{
	uint16_t pid;
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI;

	http_server_req_state* rs = Http_Server_Get_State( parser );
	const REST_PREDEFINED_ASSY_TD* pPredefinedAssy =
		reinterpret_cast<const REST_PREDEFINED_ASSY_TD*>( rs->p.rest.assy_ptr );

	parser->blocks_for_data = 0U;
	rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
	Http_Server_Begin_Dynamic( parser );

	json_response_t* json_buffer =
		reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

		char formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0, BUFFER_LEN );

		for (; parser->data_transmitted < pPredefinedAssy->number_of_parameters &&
			 !( Check_Block_Overflow_And_Datablock_Limit( parser ) );
			 parser->data_transmitted++ )
		{
			pid = pPredefinedAssy->pParams[parser->data_transmitted];
			pDCI = httpd_rest_dci::findIndexREST( pid );
			if ( pDCI != nullptr )
			{
				if ( pDCI->access_r <= rs->max_auth_exempt_level )
				{
					Open_Json_Object( json_buffer );
					Add_Integer_Member( json_buffer, "pid", pid, true );
					snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d", pid );
					Add_String_Member( json_buffer, HREF, formatted_buff, false );
					if ( parser->data_transmitted < ( pPredefinedAssy->number_of_parameters - 1U ) )
					{
						Close_Json_Object( json_buffer, true );
					}
					else
					{
						Close_Json_Object( json_buffer, false );
					}
					Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

					json_buffer->used_buff_len = 0U;
					memset( json_buffer, 0U, JSON_MAXBUFF_LEN );
				}
				else
				{
					hams_respond_error( parser, ERROR_UNAUTHORIZED );
				}
			}
		}
		if ( parser->data_transmitted >= pPredefinedAssy->number_of_parameters )
		{
			Close_Json_Array( json_buffer, false );
			Multiple_Close_Object( json_buffer, 2 );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

			parser->data_transmitted = 0U;
			parser->transmission_continue = false;
		}
		else
		{
			parser->transmission_continue = true;
		}
		Http_Server_End_Dynamic( parser );
		g_hams_demultiplexer->staged_len = 0U;

		Ram::De_Allocate( json_buffer );
	}

	return ( 0U );
}

static void Get_Predefined_Assy_Params_Json( hams_parser* parser,
											 const REST_PREDEFINED_ASSY_TD* pPredefinedAssy )
{
	uint16_t content_length = 0U;
	uint16_t pid;
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI;
	uint16_t total_params = 0U;

	http_server_req_state* rs = Http_Server_Get_State( parser );

	json_response_t* json_buffer =
		reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0, BUFFER_LEN );

		Open_Json_Object( json_buffer );
		Add_String_Member( json_buffer, "Assy", NULL, false );
		Open_Json_Object( json_buffer );
		Add_Integer_Member( json_buffer, "aid", pPredefinedAssy->assy_id, true );
		Add_String_Member( json_buffer, "name", pPredefinedAssy->assy_name, true );
		Add_String_Member( json_buffer, "list_ro", "1", true );
		Add_String_Member( json_buffer, "open_slots", "0", true );
		Add_Integer_Member( json_buffer, "r", pPredefinedAssy->access_r, true );
		Add_Integer_Member( json_buffer, "w", pPredefinedAssy->access_w, true );
		Add_String_Member( json_buffer, "Param", NULL, false );
		Open_Json_Array( json_buffer );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

		json_buffer->used_buff_len = 0U;
		memset( json_buffer, 0U, JSON_MAXBUFF_LEN );

		for ( uint16_t i = 0U; i < pPredefinedAssy->number_of_parameters; i++ )
		{
			pid = pPredefinedAssy->pParams[i];
			pDCI = httpd_rest_dci::findIndexREST( pid );
			if ( nullptr != pDCI )
			{
				if ( pDCI->access_r <= rs->max_auth_exempt_level )
				{
					if ( false == Check_Block_Overflow_And_Datablock_Limit( parser ) )
					{
						Open_Json_Object( json_buffer );
						Add_Integer_Member( json_buffer, "pid", pid, true );
						snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d", pid );
						Add_String_Member( json_buffer, HREF, formatted_buff, false );
						if ( i < ( pPredefinedAssy->number_of_parameters - 1U ) )
						{
							Close_Json_Object( json_buffer, true );
						}
						else
						{
							Close_Json_Object( json_buffer, false );
						}
						Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

						json_buffer->used_buff_len = 0U;
						memset( json_buffer, 0U, JSON_MAXBUFF_LEN );

						parser->data_transmitted++;
					}

					total_params++;

					content_length += ( 2U * Find_Base_Log_Ten( pid ) );
				}
				else
				{
					hams_respond_error( parser, ERROR_UNAUTHORIZED );
				}
			}
		}

		if ( parser->data_transmitted >= pPredefinedAssy->number_of_parameters )
		{
			Close_Json_Array( json_buffer, false );
			Multiple_Close_Object( json_buffer, 2 );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

			parser->data_transmitted = 0U;
			parser->transmission_continue = false;
		}
		else
		{
			content_length += STRLEN( JSON_ASSY_PARAMS_CONST_DATA );

			content_length += pPredefinedAssy->assy_name_len;

			content_length += Find_Base_Log_Ten( pPredefinedAssy->assy_id );

			content_length += Find_Base_Log_Ten( pPredefinedAssy->access_r );

			content_length += Find_Base_Log_Ten( pPredefinedAssy->access_w );

			content_length += STRLEN( JSON_RS_PARAM_SUFFIX );

			uint16_t const_content_len = 0U;

			const_content_len += STRLEN( JSON_RS_PARAM_FIX_STRING );

			content_length += ( const_content_len * total_params );

			parser->total_content_length = content_length;

			parser->user_data_cb_remaining = &Get_Predefined_Assy_Params_Remaining;
			parser->transmission_continue = true;
		}

		Ram::De_Allocate( json_buffer );
	}
}

static const REST_PREDEFINED_ASSY_TD* Find_Predefined_Assy_Json( uint16_t aid )
{
	uint16_t i = 0U;
	const REST_PREDEFINED_ASSY_TD* pPredefinedAssy = nullptr;
	bool break_loop = false;

	uint16_t num_of_predefined_assy = httpd_rest_dci::getPredefinedAssyNumber();

	for ( i = 0U; ( i < num_of_predefined_assy ) && ( break_loop == false ); i++ )
	{
		pPredefinedAssy = httpd_rest_dci::getPredefinedAssy( i );
		if ( aid == pPredefinedAssy->assy_id )
		{
			break_loop = true;
		}
	}
	if ( break_loop == false )
	{
		pPredefinedAssy = nullptr;
	}
	return ( pPredefinedAssy );
}

static const REST_CUSTOM_ASSY_TD* Find_Custom_Assy_Json( uint16_t aid )
{
	uint16_t i = 0U;
	const REST_CUSTOM_ASSY_TD* pCustomAssy = nullptr;
	bool break_loop = false;
	uint16_t num_of_custom_assy = httpd_rest_dci::getCustomAssyNumber();

	for ( i = 0U; ( i < num_of_custom_assy ) && ( break_loop == false ); i++ )
	{
		pCustomAssy = httpd_rest_dci::getCustomAssy( i );
		if ( aid == pCustomAssy->assy_id )
		{
			break_loop = true;
		}
	}
	if ( break_loop == false )
	{
		pCustomAssy = nullptr;
	}
	return ( pCustomAssy );
}

static void Get_Custom_Assy_Params_Json( hams_parser* parser, const REST_CUSTOM_ASSY_TD* pCustomAssy )
{
	uint16_t bitmap = 0U;
	DCI_ACCESS_ST_TD access_struct;
	uint16_t pid = 0U;
	uint16_t num_of_open_slots = 0U;
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI;

	access_struct.data_access.length = 2U;
	access_struct.source_id = httpd_source_id;
	access_struct.data_access.data = &pid;
	access_struct.data_access.offset = 0U;
	access_struct.data_id = pCustomAssy->paramListDCID;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;

	if ( true == Get_Valid_Bitmap( pCustomAssy, &bitmap ) )
	{
		num_of_open_slots = Get_Custom_Assy_NumOfOpenSlots( bitmap );
	}
	else
	{
		num_of_open_slots = 0U;
	}

	http_server_req_state* rs = Http_Server_Get_State( parser );

	json_response_t* json_buffer =
		reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );

	if ( json_buffer != nullptr )
	{
		json_buffer->used_buff_len = 0U;
		memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

		char_t formatted_buff[BUFFER_LEN];
		memset( formatted_buff, 0, BUFFER_LEN );
		bool l_print = false;

		Open_Json_Object( json_buffer );
		Add_String_Member( json_buffer, "Assy", NULL, false );
		Open_Json_Object( json_buffer );
		Add_Integer_Member( json_buffer, "aid", pCustomAssy->assy_id, true );
		Add_String_Member( json_buffer, "name", pCustomAssy->assy_name, true );
		Add_String_Member( json_buffer, "list_ro", "0", true );
		Add_Integer_Member( json_buffer, "open_slots", num_of_open_slots, true );
		Add_Integer_Member( json_buffer, "r", pCustomAssy->access_r, true );
		Add_Integer_Member( json_buffer, "w", pCustomAssy->access_w, true );
		Add_String_Member( json_buffer, "Param", NULL, false );
		Open_Json_Array( json_buffer );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

		json_buffer->used_buff_len = 0U;
		memset( json_buffer, 0U, JSON_MAXBUFF_LEN );

		for ( uint16_t i = 0U; i < MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY; i++ )
		{
			if ( ( bitmap & static_cast<uint16_t>( CONST_ONE << i ) ) != 0U )
			{
				access_struct.data_access.offset = static_cast<uint16_t>( i << CONST_ONE );
				if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
				{
					pDCI = httpd_rest_dci::findIndexREST( pid );
					if ( nullptr != pDCI )
					{
						if ( pDCI->access_r <= rs->max_auth_exempt_level )
						{
							if ( l_print == true )
							{
								Http_Server_Print_Dynamic( parser, JSON_ADD_COMMA );
							}
							Open_Json_Object( json_buffer );
							Add_Integer_Member( json_buffer, "pid", pid, true );
							snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d", pid );
							Add_String_Member( json_buffer, HREF, formatted_buff, false );
							Close_Json_Object( json_buffer, false );
							Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

							json_buffer->used_buff_len = 0U;
							memset( json_buffer, 0U, JSON_MAXBUFF_LEN );
							l_print = true;
						}
						else
						{
							hams_respond_error( parser, ERROR_UNAUTHORIZED );
						}
					}

				}
			}
		}

		Close_Json_Array( json_buffer, false );
		Multiple_Close_Object( json_buffer, 2 );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
		Ram::De_Allocate( json_buffer );
	}
}

static bool Get_Valid_Bitmap( const REST_CUSTOM_ASSY_TD* pCustomAssy,
							  uint16_t* pBitmap )
{
	DCI_ACCESS_ST_TD access_struct;
	uint16_t bitmap = 0U;
	bool status = false;

	*pBitmap = 0U;
	access_struct.data_access.length = 2U;
	access_struct.source_id = httpd_source_id;
	access_struct.data_access.data = &bitmap;
	access_struct.data_access.offset = 0U;
	access_struct.data_id = pCustomAssy->validBitMapDCID;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
	{
		*pBitmap = bitmap;
		status = true;
	}
	return ( status );
}

static uint16_t Get_Custom_Assy_NumOfOpenSlots( uint16_t bitmap )
{
	uint16_t maxAllowed;
	uint8_t used;
	uint16_t open_slots = 0U;

	maxAllowed =
		httpd_rest_dci::getRestStruct()->max_number_of_paramters_in_custom_assembly;
	used = Get_Valid_Numbers_From_Bitmap( bitmap );
	if ( used <= maxAllowed )
	{
		open_slots = ( maxAllowed - used );
	}
	return ( open_slots );
}

static uint8_t Get_Valid_Numbers_From_Bitmap( uint16_t bitmap )
{
	uint8_t res = 0U;

	for ( uint8_t i = 0U; i < MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY; i++ )
	{
		if ( 0U != ( bitmap & ( 1UL << i ) ) )
		{
			res++;
		}
	}
	return ( res );
}

static bool Is_Param_In_Predefined_Assy( uint16_t pid,
										 const REST_PREDEFINED_ASSY_TD* pPredefinedAssy )
{
	bool status = false;

	for ( uint16_t i = 0U; ( i < pPredefinedAssy->number_of_parameters ) && ( status == false ); i++ )
	{
		if ( pPredefinedAssy->pParams[i] == pid )
		{
			status = true;
		}
	}
	return ( status );
}

static bool Is_Param_In_Custom_Assy( uint16_t pid,
									 const REST_CUSTOM_ASSY_TD* pCustomAssy )
{
	DCI_ACCESS_ST_TD access_struct;
	uint16_t bitmap = 0U;
	uint16_t pid_read = 0U;
	uint16_t i = 0U;
	bool status = false;
	bool break_loop = false;

	access_struct.data_access.length = 2U;
	access_struct.source_id = httpd_source_id;
	access_struct.data_access.data = &bitmap;
	access_struct.data_access.offset = 0U;
	access_struct.data_id = pCustomAssy->validBitMapDCID;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;

	if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
	{
		for ( i = 0U; ( i < MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY ) && ( break_loop == false ); i++ )
		{
			if ( ( bitmap & static_cast<uint16_t>( CONST_ONE << i ) ) != 0U )
			{
				access_struct.data_access.data = &pid_read;
				access_struct.data_access.offset = static_cast<uint16_t>( i << CONST_ONE );
				access_struct.data_id = pCustomAssy->paramListDCID;
				access_struct.command = DCI_ACCESS_GET_RAM_CMD;

				if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
				{
					if ( pid_read == pid )
					{
						break_loop = true;
						status = true;
					}
				}
				else
				{
					break_loop = true;
					status = false;
				}
			}
		}
	}
	return ( status );
}

static uint32_t Assy_Put_Values_Callback_Json( hams_parser* parser, const uint8_t* data,
											   uint32_t length )
{
	uint16_t error_code = RESPONSE_OK;
	uint16_t formatted_param_count = 0;
	bool status = false;

	/* Look for all match. */
	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )	// '/'=> ASCII 0x2FU
	{
		parser->user_data_cb = &Assy_Put_Values_Callback_Json;
	}
	else
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );

		if ( 0U != hams_check_headers_ended( data, length ) )
		{
			if ( parser->content_length != 0U )
			{
				/* Make we are reading the correct type. Form URL Encoded is not the optimal way to do this...
				 * should be using HTTP_CONTENT_TYPE_APPLICATION_JSON...oh well */
				if ( parser->content_type == HTTP_CONTENT_TYPE )
				{
					const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI;
					const DCI_DATA_BLOCK_TD* data_block;
					uint16_t total_value_len = 0U;
					uint16_t pid = 0U;
					uint16_t max_param_allowed = 0U;

					if ( rs->p.rest.assy_type == ASSY_PREDEF )
					{
						if ( rs->max_auth_exempt_level >=
							 ( reinterpret_cast<REST_PREDEFINED_ASSY_TD*>
							   ( rs->p.rest.assy_ptr ) )->access_r )
						{
							max_param_allowed = ( reinterpret_cast<REST_PREDEFINED_ASSY_TD*>
												  ( rs->p.rest.assy_ptr ) )->number_of_parameters;
							for ( uint16_t i = 0U; i < max_param_allowed; i++ )
							{
								pid = ( reinterpret_cast<REST_PREDEFINED_ASSY_TD*>
										( rs->p.rest.assy_ptr ) )->pParams[i];
								pDCI = httpd_rest_dci::findIndexREST( pid );

								if ( pDCI != NULL )
								{
									data_block = DCI::Get_Data_Block( pDCI->dcid );

									if ( data_block != NULL )
									{
										total_value_len += *data_block->length_ptr;
									}
									if ( pDCI->pFormatData != nullptr )
									{
										formatted_param_count++;
									}
								}
							}
							// Make total length align with 8 byte step size
							total_value_len = ALIGN_U64( total_value_len );
						}
						else
						{
							// Internal server error
							error_code = ERROR_INTERNAL_SERVER;
						}
					}
					else
					{
						if ( rs->max_auth_exempt_level >=
							 ( reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p.rest.assy_ptr ) )
							 ->access_r )
						{
							uint16_t bitmap = 0U;
							DCI_ACCESS_ST_TD access_struct;

							max_param_allowed = rest_dci_data_target_info
								.max_number_of_paramters_in_custom_assembly;

							access_struct.data_access.length = 2U;
							access_struct.source_id = httpd_source_id;
							access_struct.data_access.data = &pid;
							access_struct.data_access.offset = 0U;
							access_struct.data_id = ( reinterpret_cast<REST_CUSTOM_ASSY_TD*>
													  ( rs->p.rest.assy_ptr ) )->paramListDCID;
							access_struct.command = DCI_ACCESS_GET_RAM_CMD;

							status = Get_Valid_Bitmap(
								reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p.rest.assy_ptr ),
								&bitmap );

							if ( true == status )
							{
								for ( uint16_t i = 0U; i < max_param_allowed; i++ )
								{
									if ( ( bitmap & static_cast<uint16_t>( CONST_ONE << i ) ) != 0U )
									{
										access_struct.data_access.offset = static_cast<uint16_t>( i
																								  << CONST_ONE );
										if ( DCI_ERR_NO_ERROR ==
											 httpd_dci_access->Data_Access( &access_struct ) )
										{
											pDCI = httpd_rest_dci::findIndexREST( pid );
											if ( nullptr != pDCI )
											{
												data_block = DCI::Get_Data_Block( pDCI->dcid );
												if ( data_block != NULL )
												{
													total_value_len += *data_block->length_ptr;
												}
												if ( pDCI->pFormatData != nullptr )
												{
													formatted_param_count++;
												}
											}
										}
									}
								}
							}
							else
							{
								// Internal server error
								error_code = ERROR_INTERNAL_SERVER;
							}
							// Make total length align with 8 byte step size
							total_value_len = ALIGN_U64( total_value_len );
						}
						else
						{
							error_code = ERROR_UNAUTHORIZED;
						}
					}
					if ( error_code == RESPONSE_OK )
					{
						rs->p.rest.yjson = Http_Server_Alloc_Json_Parser( parser, 32U );

						if ( rs->p.rest.yjson != NULL )
						{
							uint16_t buf_size = ( max_param_allowed * sizeof( assy_param_t ) )
								+ sizeof( assy_parsed_data_t ) + total_value_len
								+ RECV_TOTAL_BUF_SIZE;
							// Reserve space for format buffer if assembly contains formatted parameter.
							if ( formatted_param_count > 0 )
							{
								buf_size += FORMATTED_TOTAL_BUF_SIZE;
							}
							uint8_t num_blocks = ( buf_size + ( HTTP_POOL_BLOCK_SIZE - 1U ) ) / HTTP_POOL_BLOCK_SIZE;

							/* Allocate ourselves a block for json parsing. */
							assy_parsed_data_t* meta_data =
								reinterpret_cast<assy_parsed_data_t*>( Http_Server_Alloc_Scratch(
																		   parser, num_blocks ) );

							if ( meta_data != NULL )
							{
								rs->p.rest.put_request = ( void* )meta_data;
								meta_data->param_value_cmd = true;
								meta_data->buff_size = total_value_len;
								meta_data->status = NOT_VERIFIED;
								meta_data->param_found = false;
								meta_data->param_index = 0U;
								meta_data->max_params = max_param_allowed;
								meta_data->assy_data =
									reinterpret_cast<assy_param_t*>( reinterpret_cast<uint8_t*>( meta_data )
																	 + sizeof( assy_parsed_data_t ) );

								struct yjson_put_json_state* ypx =
									reinterpret_cast<struct yjson_put_json_state*>( rs->p.rest.yxml );

								ypx->flags = 0U;

								parser->user_data_cb = &Assy_Put_Parse_Callback_Json;
							}
							else
							{
								error_code = E503_JSON_PARSER;
							}
						}
						else
						{
							error_code = E503_SCRATCH_SPACE;
						}
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
			if ( error_code != RESPONSE_OK )
			{
				hams_respond_error( parser, error_code );
			}
		}
		else
		{
			if ( 0U != hams_can_respond( data, length ) )
			{
				if ( 0U != Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  nullptr, 0U );
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

static uint32_t Assy_Delete_Param_Callback_Json( hams_parser* parser, const uint8_t* data,
												 uint32_t length )
{
	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = nullptr;
	uint16_t error_code = RESPONSE_OK;

	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )	// '/'=> ASCII 0x2FU
	{
		parser->user_data_cb = &Assy_Delete_Param_Callback_Json;
	}
	else
	{
		if ( parser->content_length == 0U )
		{
			if ( 0U != hams_check_headers_ended( data, length ) )
			{
				http_server_req_state* rs = Http_Server_Get_State( parser );

				if ( ( reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p.rest.assy_ptr ) )->access_w <=
					 rs->max_auth_exempt_level )
				{
					pDCI = reinterpret_cast<DCI_REST_TO_DCID_LKUP_ST_TD*>( rs->p.rest.ptr );

					if ( pDCI->access_r <= rs->max_auth_exempt_level )
					{
						if ( !Delete_Param_From_Assy_Json( pDCI->param_id,
														   reinterpret_cast<REST_CUSTOM_ASSY_TD*>( rs->p.rest.assy_ptr ) ) )
						{
							error_code = ERROR_INTERNAL_SERVER;
						}
						else
						{
							parser->response_code = RESPONSE_OK;
						}
					}
					else
					{
						error_code = ERROR_UNAUTHORIZED;
					}
				}
				else
				{
					error_code = ERROR_UNAUTHORIZED;
				}

				if ( error_code != RESPONSE_OK )
				{
					hams_respond_error( parser, error_code );
				}
			}
			else
			{
				if ( 0U != hams_can_respond( data, length ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  nullptr, 0U );
				}
				else
				{
					hams_respond_error( parser, ERROR_BAD_REQUEST );
				}
			}
		}
		else
		{
			hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
		}
	}
	return ( 0U );
}

static bool Delete_Param_From_Assy_Json( uint16_t pid,
										 const REST_CUSTOM_ASSY_TD* pCustomAssy )
{
	DCI_ACCESS_ST_TD access_struct;
	uint16_t bitmap = 0U;
	uint16_t pid_read = 0U;
	uint16_t i = 0U;
	bool status = false;
	bool loop_break = false;

	access_struct.data_access.length = 2U;
	access_struct.source_id = httpd_source_id;
	access_struct.data_access.data = &bitmap;
	access_struct.data_access.offset = 0U;
	access_struct.data_id = pCustomAssy->validBitMapDCID;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;

	if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
	{
		for ( i = 0U; ( i < MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY ) && ( loop_break == false ); i++ )
		{
			if ( ( bitmap & static_cast<uint16_t>( CONST_ONE << i ) ) != 0U )
			{
				access_struct.data_access.data = &pid_read;
				access_struct.data_access.offset = static_cast<uint16_t>( i << 1U );
				access_struct.data_id = pCustomAssy->paramListDCID;
				access_struct.command = DCI_ACCESS_GET_RAM_CMD;

				if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
				{
					if ( pid_read == pid )
					{
						bitmap &= static_cast<uint16_t>( ~( 1UL << i ) );
						access_struct.data_access.data = &bitmap;
						access_struct.data_access.offset = 0U;
						access_struct.data_id = pCustomAssy->validBitMapDCID;
						access_struct.command = DCI_ACCESS_SET_RAM_CMD;

						if ( DCI_ERR_NO_ERROR == httpd_dci_access->Data_Access( &access_struct ) )
						{
							access_struct.command = DCI_ACCESS_SET_INIT_CMD;

							if ( DCI_ERR_NO_ERROR ==
								 httpd_dci_access->Data_Access( &access_struct ) )
							{
								status = true;
								loop_break = true;
							}
							else
							{
								status = false;
								loop_break = true;
							}
						}
						else
						{
							status = false;
							loop_break = true;
						}
					}
				}
				else
				{
					status = false;
					loop_break = true;
				}
			}
		}
	}
	return ( status );
}
