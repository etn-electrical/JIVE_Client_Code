/**
 *****************************************************************************************
 * @file REST.cpp
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
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
#include "REST.h"// LTK
#include "REST_Log.h"
#include "Log_Config.h"
#include "Format_handler.h"
#include "stdio.h"
#include "StdLib_MV.h"
#include "Web_Debug.h"
#include "REST_Common.h"
#include "Babelfish_Assert.h"

#ifdef JSON_ACTIVE
#include "REST_Json.h"
#endif

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

// extern "C"{
#include "server.h"	// LTK
#include "yxml.h"// LTK
#define Param_RO   "GET, HEAD, OPTIONS"
#define Param_W    "GET, HEAD, PUT, OPTIONS"
/* Generic HAMS handler functions.  */
extern uint32_t S_Xmit_Flash_Data( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Assy_Get_All_Callback( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Assy_Index_Callback( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Prod_Info_Spec_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Parse_Fw_Resources_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t Parse_UserMgmt_Resources_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

bool Format_Handler_Check( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
						   uint8_t* dest_data, uint32_t length, uint32_t offset );


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

static const char_t* const s_rest_param_xml_tags[COUNT_REST_PARAMS] =
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

static uint8_t number_charArr[MAX_NUMBER_STRING_LENGTH] =
{ 0U };

#define DEST_BUFFER_SIZE ( REST_DCI_MAX_LENGTH > \
						   MIN_DEST_BUFFER_SIZE ) ? ( REST_DCI_MAX_LENGTH + 1 ) : MIN_DEST_BUFFER_SIZE

uint8_t dest_data[DEST_BUFFER_SIZE];

/* Function prototypes. */
static bool_t getXMLParamDescription( hams_parser* parser,
									  const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI );

static bool_t getXMLParamRange( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI );

static bool_t getXMLParamEnum( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI );

static uint32_t Get_Enum_Description_Cb( hams_parser* parser, const uint8_t* data,
										 uint32_t length );

static uint32_t Get_Enum_Xml_All_Remaining( hams_parser* parser, const uint8_t* data,
											uint32_t length );

static bool_t Get_XML_Param_Bitlist( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI );

static uint32_t Get_Bitfield_Description_Cb( hams_parser* parser, const uint8_t* data,
											 uint32_t length );

static bool_t s_dynamic_write_value( hams_parser* parser, UINT8 type, const UINT8* pData,
									 UINT32 offset );

static uint32_t s_rest_root_handler( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t s_rest_device_all( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t s_rest_param_list_end( hams_parser* parser, const uint8_t* data,
									   uint32_t length );

static uint32_t s_rest_param_index( hams_parser* parser, const uint8_t* data,
									uint32_t length );


uint16_t Print_Rest_Format( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
							uint8_t* data, uint32_t tot_length, uint32_t offset );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t s_rest_root_hdrs_end( hams_parser* parser, const uint8_t* data, unsigned length )
{

	/* There should not be any request body.. */
	if ( 0U != ( parser->content_length ) )
	{
		hams_respond_error( parser, 413U );
	}
	else
	{
		parser->response_code = 200U;
		http_server_req_state* rs = Http_Server_Get_State( parser );
		rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
		Http_Server_Begin_Dynamic( parser );

		Http_Server_Print_Dynamic( parser, XML_DECL XML_RS_START ATTRIB_XMLNS_XLINK );
		Http_Server_Print_Dynamic( parser, " RESTSpecVer=\"\bu.\bu.\bu\" ",
								   REST_SPEC_VER_MAJOR, REST_SPEC_VER_MINOR, REST_SPEC_VER_REV );
		Http_Server_Print_Dynamic( parser, XML_RS_ROOT );

		if ( 0U != Http_Server_End_Dynamic( parser ) )
		{
			hams_response_header( parser, static_cast<uint32_t>( HTTP_HDR_CONTENT_TYPE ),
								  "application/xml", 15U );

			hams_response_header( parser, static_cast<uint32_t>( COUNT_HTTP_HDRS ),
								  reinterpret_cast<void*>( NULL ), 0U );
		}

	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t s_rest_root_handler( hams_parser* parser, const uint8_t* data,
									 uint32_t length )
{
	if ( Firmware_Upgrade_Inprogress() == true )
	{
		hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
	}
	else
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );

		if ( ( 0U == rs->max_auth_exempt_level ) && ( parser->method != HTTP_METHOD_OPTIONS ) )
		{
			hams_respond_error( parser, 401U );
		}
		else if ( 0U != hams_check_headers_ended( data, length ) )
		{
			/* There should not be any request body.. */
			if ( 0U != parser->content_length )
			{
				hams_respond_error( parser, 413U );
			}
			else if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				Validate_Options_Cors_Headers( parser, rs, nullptr );
			}
			else
			{
				/* Redirect to default. */
				parser->response_code = 200U;
#ifdef JSON_ACTIVE
				parser->user_data_cb = static_cast<hams_data_cb>( &S_Rest_Root_Hdrs_End_Json );
#else
				parser->user_data_cb = static_cast<hams_data_cb>( &s_rest_root_hdrs_end );
#endif

			}
		}
		else
		{
			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				hams_response_header( parser, HTTP_HDR_ALLOW, Param_RO, STRLEN( Param_RO ) );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
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
static uint32_t s_rest_device_all( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint8_t return_val = 0U;

	if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
		 ( HTTP_METHOD_OPTIONS != parser->method ) )
	{
		hams_respond_error( parser, 405U );
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
					hams_respond_error( parser, 413U );
					return_val = 0U;
				}
				else
				{
					UINT16 tot_length = 0U;
					if ( DCI_ERR_NO_ERROR ==
						 get_dcid_ram_data( DCI_PRODUCT_NAME_DCID,
											&dest_data, &tot_length, 0U ) )
					{
						parser->response_code = 200U;
						rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
						Http_Server_Begin_Dynamic( parser );
						uint16_t String_Size = strlen( ( char const* )dest_data );

						Http_Server_Print_Dynamic( parser,
												   XML_DECL XML_DEVICE_ALL_PREFIX
												   XML_DEVICE_LINE_SEGMENT0 "0" XML_DEVICE_LINE_SEGMENT1 QUOTE_STR "\bs" QUOTE_STR
												   XML_DEVICE_LINE_SEGMENT2 "0" XML_DEVICE_LINE_SEGMENT3
												   XML_DEVICE_ALL_SUFFIX, String_Size,
												   dest_data );

						return_val = 1U;
					}
					else
					{
						hams_respond_error( parser, 500U );
						return_val = 0U;
					}
				}
			}
			else
			{
				hams_respond_error( parser, 401U );
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
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );

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
static uint32_t s_rest_param_value_put_parse( hams_parser* parser,
											  const uint8_t* data, uint32_t length )
{
	bool_t loop_exit = false;
	bool_t function_end = false;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	struct yxml_put_xml_state* ypx =
		reinterpret_cast<struct yxml_put_xml_state*>( rs->p.rest.yxml );
	yxml_t* yx = rs->p.rest.yxml;
	ReusedRecvBuf* pTarget = static_cast<ReusedRecvBuf*>( rs->p.rest.put_request );
	FormatBuf* format_buff = reinterpret_cast<FormatBuf*>( static_cast<uint8_t*>( rs->p.rest.put_request )
														   + sizeof( ReusedRecvBuf ) );
	bool_t parse_complete = false;

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
			static_cast<const DCI_REST_TO_DCID_LKUP_ST_TD*>( rs->p.rest.ptr );
		const DCI_DATA_BLOCK_TD* data_block = DCI::Get_Data_Block( pDCI->dcid );

		uint32_t error_500 = 0U;

		for ( uint32_t i = 0U;
			  ( ( i < length ) && ( loop_exit == false ) && ( function_end == false ) ); ++i )
		{
			int32_t r = yxml_parse( yx, static_cast<int32_t>( data[i] ) );
			switch ( r )
			{

				/* Expecting the proper tag element. */
				case YXML_ELEMSTART:
					if ( 0 != strcmp( yx->elem, s_rest_param_xml_tags[rs->p.rest.rest_param] ) )
					{
						error_500 = 400U;
						loop_exit = true;
					}
					else if ( pDCI->pFormatData == nullptr )
					{
						ypx->tmp_number = 0U;
						ypx->val_index = 0U;
						if ( ypx->separator == 0x3AU )
						{
							// :
							ypx->flags |= YXML_PUT_IS_HEX;
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
					break;

				/* This is the meat of the PUT request. This case statement is why IAR compiler needs pointers to
				   labels...*/
				case YXML_CONTENT:
				{

					if ( ( pDCI->pFormatData == nullptr ) || ( pDCI->pFormatData->format_name != nullptr
#ifdef REST_SERVER_FORMAT_HANDLING
															   && pDCI->pFormatData->formatter_cback == nullptr
#endif
															   ) )
					{
						const char_t* c = yx->data;
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

								if ( 0U != ( ypx->flags & YXML_PUT_END ) )
								{
									error_500 = 400U;
									loop_exit = true;
								}
								else
								{
									if ( 0U != ( YXML_PUT_IS_BASE64 & ypx->flags ) )
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
											ypx->flags |= YXML_PUT_BASE64_PAD;
											if ( 2U == ypx->base64_bits )
											{
												/* Don't overflow the buffer. */
												if ( ypx->val_index >= *data_block->length_ptr )
												{
													error_500 = 413U;
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
													error_500 = 413U;
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
											ypx->flags |= YXML_PUT_END;
										}
										else
										{
											error_500 = 400U;
											loop_exit = true;
										}

										if ( ( val < 0x80U ) && ( false == loop_exit ) )
										{
											/* Cannot have more B64 chars after pad appears.*/
											if ( 0U != ( ypx->flags & YXML_PUT_BASE64_PAD ) )
											{
												error_500 = 400U;
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
														error_500 = 413U;
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
											error_500 = 413U;
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
												if ( 0U == ( ypx->flags & YXML_PUT_HAS_LEADING_ZERO ) )
												{
													error_500 = 400U;
													loop_exit = true;
												}
												else
												{
													/* Clear leading zero flag. */
													ypx->flags = YXML_PUT_IS_HEX;
													++ypx->put_state;
												}
											}
											else if ( 0x2DU == static_cast<uint8_t>( *c ) )
											{
												// -
												/* No flags should be set. */
												if ( ypx->flags )
												{
													error_500 = 400U;
													loop_exit = true;
												}
												else
												{
													ypx->flags |= YXML_PUT_IS_NEGATIVE;
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
												if ( YXML_PUT_HAS_LEADING_ZERO == ypx->flags )
												{
													/* This just zero, save it. */
													ypx->tmp_number = 0U;
													int32_t ret =
														set_param_numeric( pTarget, ypx,
																		   data_block->datatype );
													if ( -1 == ret )
													{
														error_500 = 413U;
														loop_exit = true;
													}
													else if ( ret < 0 )
													{
														error_500 = 400U;
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

														if ( ypx->separator == 0x3AU )	/* : */
														{
															ypx->flags = YXML_PUT_IS_HEX;
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
														error_500 = 400U;
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
												error_500 = 400U;
												loop_exit = true;
											}
										}
										else
										{
											ypx->flags |= YXML_PUT_HAS_LEADING_ZERO;
										}
									}
									else
									{
										error_500 = 400U;
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
											 ( ( ypx->flags & YXML_PUT_HAS_LEADING_ZERO ) == false ) )
										{
											/* Save the number. */
											int32_t ret =
												set_param_numeric( pTarget, ypx,
																   data_block->datatype );
											if ( -1 == ret )
											{
												error_500 = 413U;
												loop_exit = true;
											}
											else if ( ret < 0 )
											{
												error_500 = 400U;
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
												/* If parsing a MAC address, basically assume we already read the next
												   0x (MAC address excludes hex prefix). */
												if ( ypx->separator == 0x3AU )	/* : */
												{
													ypx->flags = YXML_PUT_IS_HEX;
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
											error_500 = 400U;
											loop_exit = true;
										}
										else if ( v > 0x39U )
										{
											// 9
											if ( ypx->flags & YXML_PUT_IS_HEX )
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
													error_500 = 400U;
													loop_exit = true;
												}
											}
											else
											{
												error_500 = 400U;
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
											if ( ypx->flags & YXML_PUT_IS_HEX )
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
						if ( FORMATTED_BUF_SIZE > format_buff->byte_index )
						{
							// Capture complete formatted string byte by byte
							format_buff->buf[format_buff->byte_index] = data[i];
							format_buff->byte_index++;
						}
						else
						{
							error_500 = 413U;
							loop_exit = true;
						}
					}
				}
				break;

				/* Put request ends when element ends. */
				case YXML_ELEMEND:
				{
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
								error_500 = 413U;
								loop_exit = true;
							}

							if ( PTARGET_DATA_PRESENT( pTarget->present,
													   ( ypx->val_index +
														 ( ypx->num_of_units * ypx->curr_num_list ) ) ) )
							{
								int32_t ret =
									set_param_numeric( pTarget, ypx, data_block->datatype );
								if ( -1 == ret )
								{
									error_500 = 413U;
									loop_exit = true;
								}
								else if ( ret < 0 )
								{
									error_500 = 400U;
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
							error_500 = 413U;
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
						// First terminate string since we reached to state:YXML_ELEMEND
						format_buff->buf[format_buff->byte_index] = '\0';

						uint16_t total_raw_bytes = pDCI->pFormatData->formatter_cback( pTarget->buf.u8_data,
																					   reinterpret_cast<uint16_t>( *
																												   data_block
																												   ->
																												   length_ptr ), format_buff->buf,
																					   FORMATTED_BUF_SIZE,
																					   STRING_TO_RAW,
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
							error_500 = 400U;
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
							error_500 = 401U;
							loop_exit = true;
						}
						else
						{
							uint16_t error_code = putParamValue( pDCI, pTarget, data_block, true,
																 rs->p.rest.rest_param );
							if ( error_code == 200 )
							{
								parse_complete = true;
								parser->response_code = 200U;
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
				break;

				/* TODO make sure pid matches...even though the pid is already determined by the URL... */
				case YXML_ATTRSTART:
					break;

				case YXML_ATTREND:
					break;

				case YXML_ATTRVAL:
					break;

				case YXML_OK:
					break;

				case YXML_EEOF:
					error_500 = E400_XML_EOF_ERROR;
					loop_exit = true;
					break;

				case YXML_EREF:
					error_500 = E400_XML_INVALID_CHAR_ERROR;
					loop_exit = true;
					break;

				case YXML_ECLOSE:
					error_500 = E400_XML_CLOSE_ERROR;
					loop_exit = true;
					break;

				case YXML_ESTACK:
					error_500 = E400_XML_STACK_ERROR;
					loop_exit = true;
					break;

				case YXML_ESYN:
					error_500 = E400_XML_SYNTAX_ERROR;
					loop_exit = true;
					break;

				default:
					break;
			}
		}
		if ( ( parse_complete == true ) || ( function_end == true ) || ( loop_exit == true ) )
		{
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
			hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
			hams_response_header( parser, COUNT_HTTP_HDRS,
								  reinterpret_cast<void*>( nullptr ), 0U );
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
static uint32_t s_rest_param_value_put_start( hams_parser* parser,
											  const uint8_t* data, uint32_t length )
{
	bool_t function_end = false;

	if ( ( false == ENDS_WITH_SLASH( data ) ) && ( false == IS_EXTRA_SLASH( data ) ) )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		/*  */
		if ( hams_has_uri_part( data, length ) )
		{
			hams_respond_error( parser, 404U );
			function_end = true;
		}
		if ( false == function_end )
		{
			// _ASSERT( hams_check_headers_ended(data, length) );

			/* Make sure we have a content body. */
			if ( !parser->content_length )
			{
				hams_respond_error( parser, 411U );
				function_end = true;
			}
			else
			{
				/* Make we are reading the correct type. Form URL Encoded is not the optimal way to do this...
				 * should be using HTTP_CONTENT_TYPE_APPLICATION_XML...oh well */
				if ( parser->content_type != HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED )
				{
					hams_respond_error( parser, 415U );
					function_end = true;
				}
				else
				{
					const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
						( const DCI_REST_TO_DCID_LKUP_ST_TD* )rs->p.rest.ptr;

					/* Verify authority level. */
					if ( rs->max_auth_exempt_level < pDCI->access_w )
					{
						hams_respond_error( parser, 401U );
					}
					else
					{
						/* Sanity check. */
						const DCI_DATA_BLOCK_TD* data_block = DCI::Get_Data_Block( pDCI->dcid );

						// _ASSERT( *data_block->length_ptr <= HTTPD_RECV_BUF_REUSE_DATA_UNION );

						/* Allocate ourselves a block for xml parsing. */
						rs->p.rest.yxml = Http_Server_Alloc_Xml_Parser( parser, 32U );
						if ( !rs->p.rest.yxml )
						{
							hams_respond_error( parser, E503_XML_PARSER );
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
								struct yxml_put_xml_state* ypx = ( struct yxml_put_xml_state* )rs->p
									.rest
									.yxml;

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
									static_cast<hams_data_cb>( &s_rest_param_value_put_parse );
							}
						}
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
static uint32_t s_rest_param_value_read( hams_parser* parser, const uint8_t* data,
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
							hams_respond_error( parser, 401U );
						}
						else
						{
							parser->response_code = 200U;
							Http_Server_Begin_Dynamic( parser );
							if ( REST_PARAM_DESCRIPTION == param )
							{
								if ( 0U == getXMLParamDescription( parser, pDCI ) )
								{
									hams_respond_error( parser, 500U );
								}
							}
							else if ( REST_PARAM_RANGE == param )
							{
								if ( 0U == getXMLParamRange( parser, pDCI ) )
								{
									hams_respond_error( parser, 500U );
								}
							}
							else if ( REST_PARAM_ENUM == param )
							{
								if ( 0U == getXMLParamEnum( parser, pDCI ) )
								{
									hams_respond_error( parser, 500U );
								}
							}
							else if ( REST_PARAM_BITLIST == param )
							{
								if ( 0U == Get_XML_Param_Bitlist( parser, pDCI ) )
								{
									hams_respond_error( parser, 500U );
								}
							}
							else
							{
								if ( !getXMLParamSingleItem( parser, pDCI, param, 1U ) )
								{
									hams_respond_error( parser, 500U );
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
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
			}
			else
			{
				hams_respond_error( parser, 405U );
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
						static_cast<hams_data_cb>( &Get_Enum_Description_Cb );
					rs->p.rest.rest_param = enum_value;
				}
				else
				{
					hams_respond_error( parser, 400U );
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
						static_cast<hams_data_cb>( &Get_Bitfield_Description_Cb );
					rs->p.rest.rest_param = bitfield_value;
				}
				else
				{
					hams_respond_error( parser, 400U );
				}

			}
			else
			{
				hams_respond_error( parser, 404U );
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
static uint32_t s_rest_param_value_all( hams_parser* parser, const uint8_t* data,
										uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/* Drop query variables. */
	if ( !hams_has_uri_part( data, length ) )
	{
		if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
			 ( HTTP_METHOD_OPTIONS != parser->method ) )
		{
			hams_respond_error( parser, 405U );
		}
		else
		{
			if ( hams_check_headers_ended( data, length ) )
			{

				if ( parser->content_length )
				{
					hams_respond_error( parser, 413U );
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
							hams_respond_error( parser, 401U );
						}
						else
						{
							/* First set the response code before emitting dynamic data. If there is not enough space
							   for the whole response the response_code will update automatically.*/
							parser->response_code = 200U;

							uint32_t param_id = pDCI->param_id;
							UINT16 temp_u32 = 0U;
							UINT16 temp_u16 = 0U;
							DCI_DATATYPE_TD temp_u8 = DCI_DTYPE_UINT8;
							const DCI_DATA_BLOCK_TD* data_block = NULL;

							data_block = DCI::Get_Data_Block( pDCI->dcid );

							Http_Server_Begin_Dynamic( parser );

							Http_Server_Print_Dynamic( parser,
													   XML_DECL XML_PARAM_ROOT_PREFIX1 "\bu" XML_PARAM_ROOT_PREFIX2 "\bu" QUOTE_STR,
													   param_id,
													   param_id );
							Http_Server_Print_Dynamic( parser,
													   " r=\"\bu\" w=\"\bu\">\n",
													   pDCI->access_r,
													   pDCI->access_w );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
							uint16_t StringSize = strlen( pDCI->pName );
							Http_Server_Print_Dynamic( parser,
													   "\t<Name>\bs</Name>",
													   StringSize, pDCI->pName );
#endif
							Http_Server_Print_Dynamic( parser,
													   "\n\t<Description"
													   ATTRIB_XLINK_HREF "/rs/param/\bu/description\"/>\n\t<Datatype>",
													   param_id );

							if ( ( DCI_ERR_NO_ERROR ==
								   httpd_dci_access->Get_Datatype( pDCI->dcid,
																   ( DCI_DATATYPE_TD* ) ( &temp_u8 ) ) )
								 &&
								 ( DCI_DTYPE_MAX_TYPES > temp_u8 ) )
							{
								Http_Server_Print_Dynamic( parser, "\bs",
														   DCI_Datatype_length[temp_u8],
														   DCI_Datatype_str[temp_u8] );
							}

							Http_Server_Print_Dynamic( parser, "</Datatype>\n\t<Length>" );
							if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
								 httpd_dci_access->Get_Length( pDCI->dcid, &temp_u32 ) )
							{
								Http_Server_Print_Dynamic( parser, "\bu", temp_u32 );
							}
							Http_Server_Print_Dynamic( parser, "</Length>\n\t<PresentWR>" );

							if ( NULL != data_block )
							{
								temp_u16 =
									Test_Bit( data_block->patron_attrib_info,
											  DCI_PATRON_ATTRIB_RAM_WR_DATA );
								Http_Server_Print_Dynamic( parser, "\bs", 1U,
														   temp_u16 ? "1" : "0" );
							}
							Http_Server_Print_Dynamic( parser, "</PresentWR>" );

							if ( 0U != temp_u16 )
							{
								Http_Server_Print_Dynamic( parser,
														   "\n\t<Value" ATTRIB_XLINK_HREF "/rs/param/\bu/value\"/>",
														   param_id );
							}


							Http_Server_Print_Dynamic( parser, "\n\t<InitRD>" );
							temp_u16 = 0U;
							if ( NULL != data_block )
							{
								temp_u16 =
									Test_Bit( data_block->patron_attrib_info,
											  DCI_PATRON_ATTRIB_NVMEM_DATA );
								Http_Server_Print_Dynamic( parser, "\bs", 1U,
														   temp_u16 ? "1" : "0" );
							}
							Http_Server_Print_Dynamic( parser, "</InitRD>\n\t<InitWR>" );

							if ( NULL != data_block )
							{
								Http_Server_Print_Dynamic( parser, "\bs", 1U,
														   Test_Bit( data_block->patron_attrib_info,
																	 DCI_PATRON_ATTRIB_NVMEM_WR_DATA )
														   ? "1" : "0"
														   );
							}
							Http_Server_Print_Dynamic( parser, "</InitWR>" );

							if ( 0U != temp_u16 )
							{
								Http_Server_Print_Dynamic( parser,
														   "\n\t<Initial" ATTRIB_XLINK_HREF "/rs/param/\bu/initial\"/>",
														   param_id );
							}

							Http_Server_Print_Dynamic( parser, "\n\t<DefaultSupport>" );
							temp_u16 = 0U;
							if ( NULL != data_block )
							{
								temp_u16 =
									Test_Bit( data_block->patron_attrib_info,
											  DCI_PATRON_ATTRIB_DEFAULT_DATA );
								Http_Server_Print_Dynamic( parser, "\bs", 1U,
														   temp_u16 ? "1" : "0" );
							}
							Http_Server_Print_Dynamic( parser, "</DefaultSupport>" );

							if ( 0U != temp_u16 )
							{
								Http_Server_Print_Dynamic( parser,
														   "\n\t<Default" ATTRIB_XLINK_HREF "/rs/param/\bu/default\"/>",
														   param_id );
							}

							Http_Server_Print_Dynamic( parser, "\n\t<RangeSupport>" );
							temp_u16 = 0U;
							if ( NULL != data_block )
							{
								temp_u16 =
									Test_Bit( data_block->patron_attrib_info,
											  DCI_PATRON_ATTRIB_RANGE_DATA );
								Http_Server_Print_Dynamic( parser, "\bs", 1U,
														   temp_u16 ? "1" : "0" );

							}
							Http_Server_Print_Dynamic( parser, "</RangeSupport>" );

							if ( 0U != temp_u16 )
							{
								Http_Server_Print_Dynamic( parser,
														   "\n\t<Range " ATTRIB_XLINK_HREF "/rs/param/\bu/range\"/>",
														   param_id );
							}

							Http_Server_Print_Dynamic( parser, "\n\t<EnumSupport>" );
							temp_u16 = 0;
							if ( NULL != data_block )
							{
								temp_u16 =
									Test_Bit( data_block->patron_attrib_info,
											  DCI_PATRON_ATTRIB_ENUM_DATA );
								Http_Server_Print_Dynamic( parser, "\bs", 1U,
														   temp_u16 ? "1" : "0" );
							}
							Http_Server_Print_Dynamic( parser, "</EnumSupport>" );

							if ( 0U != temp_u16 )
							{
								Http_Server_Print_Dynamic( parser,
														   "\n\t<Enum" ATTRIB_XLINK_HREF "/rs/param/\bu/enum\"/>",
														   param_id );
							}

							Http_Server_Print_Dynamic( parser, "\n\t<BitfieldSupport>" );
							if ( nullptr != pDCI->pBitfieldDes )
							{
								Http_Server_Print_Dynamic( parser, "\bs", 1U, "1" );
							}
							else
							{
								Http_Server_Print_Dynamic( parser, "\bs", 1U, "0" );
							}
							Http_Server_Print_Dynamic( parser, "</BitfieldSupport>" );
							if ( nullptr != pDCI->pBitfieldDes )
							{
								Http_Server_Print_Dynamic( parser,
														   "\n\t<Bitfield" ATTRIB_XLINK_HREF "/rs/param/\bu/bitfield\"/>",
														   param_id );
							}
							uint16_t StringLength = strlen( pDCI->pUnits );
							Http_Server_Print_Dynamic( parser,
													   "\n\t<Units>\bs</Units>",
													   StringLength, pDCI->pUnits );

							uint16_t StringSiz = 0U;
							const char_t* format_name = nullptr;
							if ( pDCI->pFormatData != nullptr )
							{
								StringSiz = strlen( pDCI->pFormatData->format_name );
								format_name = pDCI->pFormatData->format_name;
							}
							Http_Server_Print_Dynamic( parser,
													   "\n\t<Format>\bs</Format>",
													   StringSiz, format_name );
							Http_Server_Print_Dynamic( parser, "\n</Param>" );
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
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
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
static uint32_t s_rest_param_value_check( hams_parser* parser, const uint8_t* data,
										  uint32_t length )
{
	// _ASSERT( hams_has_uri_part(data, length) );
	// if (0U== hams_check_headers_ended( data, length ) )

	if ( IS_EXTRA_SLASH( data ) == false )
	{
		bool_t loop_break = false;
		bool_t function_end = false;
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
			hams_respond_error( parser, 404U );
		}
		else
		{
			parser->read_only_param = 0U;
			switch ( rs->p.rest.rest_param )
			{
				case REST_PARAM_ALL:
					parser->user_data_cb = static_cast<hams_data_cb>( &s_rest_param_value_all );
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
						hams_respond_error( parser, 404U );
						function_end = true;
					}

					parser->read_only_param = 1U;
					break;

				case REST_PARAM_ENUM:
					function_end = true;
					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_ENUM_DATA ) )
					{
						hams_respond_error( parser, 404U );
					}
					parser->read_only_param = 1U;
					parser->user_data_cb = static_cast<hams_data_cb>( &s_rest_param_value_read );
					break;

				case REST_PARAM_RANGE:
					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_RANGE_DATA ) )
					{
						hams_respond_error( parser, 404U );
						function_end = true;
					}

					parser->read_only_param = 1U;
					break;

				case REST_PARAM_INITIAL:
					if ( !Test_Bit( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_NVMEM_DATA ) )
					{
						hams_respond_error( parser, 404U );
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
						hams_respond_error( parser, 404U );
					}
					parser->read_only_param = 1U;
					parser->user_data_cb = static_cast<hams_data_cb>( &s_rest_param_value_read );
					break;

				default:
					hams_respond_error( parser, 400U );
					function_end = true;
			}

		}

		if ( false == function_end )
		{
			if ( HTTP_METHOD_PUT == parser->method )
			{
				if ( 0 != parser->read_only_param )
				{
					hams_respond_error( parser, 405U );
				}
				else
				{
					parser->user_data_cb =
						static_cast<hams_data_cb>( &s_rest_param_value_put_start );
				}
			}
			else
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &s_rest_param_value_read );
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
static uint32_t s_rest_param_all_remaining( hams_parser* parser, const uint8_t* data,
											uint32_t length )
{
	uint32_t i;

	parser->blocks_for_data = 0;

#ifndef REMOVE_REST_PARAM_NAME_TEXT
	uint16_t StringSize;
#endif

	http_server_req_state* rs = Http_Server_Get_State( parser );
	rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
	Http_Server_Begin_Dynamic( parser );

	for ( i = parser->data_transmitted;
		  ( i < rest_dci_data_target_info.number_of_parameters ) &&
		  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) ); i++ )
	{
		/* Transmit the parameter if user has read access */
		if ( rs->auth_level >= rest_dci_data_target_info.param_list[i].access_r )
		{
			Http_Server_Print_Dynamic( parser,
									   XML_PARAM_ROOT_PREFIX1 "\bu",
									   rest_dci_data_target_info.param_list[i].param_id );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
			Http_Server_Print_Dynamic( parser,
									   XML_PARAM_ALL_LINE_SEG2 );
			StringSize = strlen( rest_dci_data_target_info.param_list[i].pName );
			Http_Server_Print_Dynamic( parser, "\bs", StringSize,
									   rest_dci_data_target_info.param_list[i].pName );
#endif
			Http_Server_Print_Dynamic( parser,
									   XML_STRUCT_P_SEGMENT3 "\bu" "\"/>\n",
									   rest_dci_data_target_info.param_list[i].param_id );
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
		Http_Server_Print_Dynamic( parser, XML_PARAM_ALL_SUFFIX );
	}
	Http_Server_End_Dynamic( parser );
	g_hams_demultiplexer->staged_len = 0U;
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t s_rest_param_all( hams_parser* parser, const uint8_t* data,
								  uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( ( 0U == rs->max_auth_exempt_level ) && ( HTTP_METHOD_OPTIONS != parser->method ) )
	{
		hams_respond_error( parser, 401U );
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
				hams_respond_error( parser, 405U );
			}
			else
			{
				if ( hams_check_headers_ended( data, length ) )
				{
					if ( parser->content_length )
					{
						hams_respond_error( parser, 413U );
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

						content_length += STRLEN( XML_PARAM_ALL_PREFIX );

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
							common_content_len = STRLEN( XML_PARAM_ROOT_PREFIX1 );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
							common_content_len += STRLEN( XML_PARAM_ALL_LINE_SEG2 );
#endif
							common_content_len += STRLEN( XML_STRUCT_P_SEGMENT3 );
							common_content_len += STRLEN( "\"/>" );
							common_content_len++;	/*For new line */
							/* Multiply fix string length with total number of parameters accessible to user */
							content_length = content_length + ( common_content_len * param_count );
						}

						content_length += STRLEN( XML_PARAM_ALL_SUFFIX );
						parser->total_content_length = content_length;
						/******************************************************************************************/

						/* First set the response code before emitting dynamic data. If there is not enough space for
						   the whole response the response_code will update automatically.*/

						parser->response_code = 200U;

						Http_Server_Begin_Dynamic( parser );
						Http_Server_Print_Dynamic( parser,
												   XML_PARAM_ALL_PREFIX );

						uint32_t i;

#ifndef REMOVE_REST_PARAM_NAME_TEXT
						uint16_t StringSize;
#endif
						for ( i = parser->data_transmitted;
							  ( i < rest_dci_data_target_info.number_of_parameters ) &&
							  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) );
							  i++ )
						{
							/* Transmit the parameter if user has read access */
							if ( rs->auth_level >= rest_dci_data_target_info.param_list[i].access_r )
							{
								Http_Server_Print_Dynamic( parser,
														   XML_PARAM_ROOT_PREFIX1 "\bu",
														   rest_dci_data_target_info.param_list[i].param_id );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
								Http_Server_Print_Dynamic( parser,
														   XML_PARAM_ALL_LINE_SEG2 );
								StringSize = strlen( rest_dci_data_target_info.param_list[i].pName );
								Http_Server_Print_Dynamic( parser, "\bs", StringSize,
														   rest_dci_data_target_info.param_list[i].pName );
#endif
								Http_Server_Print_Dynamic( parser,
														   XML_STRUCT_P_SEGMENT3 "\bu" "\"/>\n",
														   rest_dci_data_target_info.param_list[i].param_id );
							}
							parser->data_transmitted++;
						}

						if ( parser->data_transmitted < rest_dci_data_target_info.number_of_parameters )
						{
							parser->transmission_continue = true;
							parser->user_data_cb_remaining = s_rest_param_all_remaining;
						}
						else
						{
							parser->data_transmitted = 0;
							parser->transmission_continue = false;
							Http_Server_Print_Dynamic( parser, XML_PARAM_ALL_SUFFIX );
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
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml",
											  15U );
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
static uint32_t s_rest_param_list_end( hams_parser* parser, const uint8_t* data,
									   uint32_t length )
{
	bool_t loop_break = false;

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
					hams_respond_error( parser, 401U );
					loop_break = true;
				}
				if ( false == loop_break )
				{
					/* Write out the last end tag of the XML response. If there is no overflow then set the response
					   code to 200. */
					if ( Http_Server_Print_Dynamic( parser, "\n</Values>" ) )
					{
						parser->response_code = 200U;
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
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
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
static uint32_t s_rest_param_list_element( hams_parser* parser, const uint8_t* data,
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
				parser->user_data_cb = static_cast<hams_data_cb>( &s_rest_param_list_end );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
		else
		{
			const DCI_REST_TO_DCID_LKUP_ST_TD*
				pDCI = httpd_rest_dci::findIndexREST( param );

			if ( NULL == pDCI )
			{
				hams_respond_error( parser, 404U );
			}
			else
			{
				if ( NULL == DCI::Get_Data_Block( pDCI->dcid ) )
				{
					hams_respond_error( parser, 404U );
				}
				else
				{
					if ( rs->p.dyn.rest_list.max_auth < pDCI->access_r )
					{
						rs->p.dyn.rest_list.max_auth = pDCI->access_r;
					}

					Http_Server_Print_Dynamic( parser, "\n" );

					if ( !getXMLParamSingleItem( parser, pDCI, REST_PARAM_VALUE, 0 ) )
					{
						hams_respond_error( parser, 500U );
					}
					else
					{
						++rs->p.dyn.rest_list.count;
						/* This is for rs/param/value/1 */
						if ( '/' != data[length] )
						{
							parser->user_data_cb =
								static_cast<hams_data_cb>( &s_rest_param_list_end );
						}
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
static uint32_t s_rest_param_index( hams_parser* parser, const uint8_t* data,
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
			parser->user_data_cb = static_cast<hams_data_cb>( &s_rest_param_all );
		}
	}
	else
	{
		/* Look for 'value'*/
		if ( !strncmp( ( ( const char_t* )data + 1 ), "value", 5U ) )
		{
			/* Only GET or HEAD is valid. */
			if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
				 ( HTTP_METHOD_OPTIONS != parser->method ) )
			{
				hams_respond_error( parser, 405U );
			}
			else
			{
				if ( data[length] == '/' )
				{
					parser->user_data_cb = static_cast<hams_data_cb>
						( &s_rest_param_list_element );
					rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
					rs->p.dyn.rest_list.max_auth = 0U;
					rs->p.dyn.rest_list.count = 0U;
					Http_Server_Begin_Dynamic( parser );
					Http_Server_Print_Dynamic( parser, XML_DECL "<Values>" );
				}
				else
				{
					hams_respond_error( parser, 404U );
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
				hams_respond_error( parser, 400U );
			}
			else
			{
				const DCI_REST_TO_DCID_LKUP_ST_TD*
					pDCI = httpd_rest_dci::findIndexREST( value );

				if ( NULL == pDCI )
				{
					hams_respond_error( parser, 404U );
				}
				else if ( NULL == DCI::Get_Data_Block( pDCI->dcid ) )
				{
					hams_respond_error( parser, 404U );
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
							( &s_rest_param_value_all );
					}
					else
					{
						parser->user_data_cb =
							static_cast<hams_data_cb>( &s_rest_param_value_check );
					}
				}
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
static uint32_t s_rest_device_index( hams_parser* parser, const uint8_t* data,
									 uint32_t length )
{
	if ( ( 1U == length ) && ( 0x2FU == data[0] ) )
	{
		// '/'=> ASCII 0x2FU
		if ( data[length] != 0x2FU )
		{
			parser->user_data_cb = &s_rest_device_all;
		}
	}
	else
	{
		/* TODO */
		hams_respond_error( parser, 501U );
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t s_rest_other_handler( hams_parser* parser, const uint8_t* data,
									  uint32_t length )
{

	bool_t choice_found = false;

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
						hams_respond_error( parser, 405U );
					}
					else
					{
						parser->response_code = 200U;

						parser->user_data_cb = static_cast<hams_data_cb>
							( &s_rest_root_hdrs_end );

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
							( &s_rest_device_index );

					}
					else if ( ' ' == data[length] )
					{
						parser->user_data_cb = s_rest_device_all;
					}
					else
					{
						/* No '?' mark allowed, bad request TODO If question mark is really a special case then it will
						   be handled later ON*/
						// hams_respond_error( parser, E404_NO_QUERY_ALLOWED );
						hams_respond_error( parser, 400U );
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
						parser->user_data_cb = static_cast<hams_data_cb>( &s_rest_param_index );
					}
					else if ( ' ' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &s_rest_param_all );

					}
					else
					{
						/* No '?' mark allowed, bad request TODO If question mark is really a special case then it will
						   be handled later ON*/
						// hams_respond_error( parser, E404_NO_QUERY_ALLOWED );
						hams_respond_error( parser, 400U );
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
						parser->user_data_cb = static_cast<hams_data_cb>( Assy_Index_Callback );
					}
					else if ( ' ' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>
							( Assy_Get_All_Callback );
					}
					else
					{
						/* No '?' mark allowed, bad request TODO If question mark is really a special case then it will
						   be handled later ON*/
						// hams_respond_error( parser, E404_NO_QUERY_ALLOWED );
						hams_respond_error( parser, 400U );
					}
				}
				break;

			case REST_FW:
				if ( '/' == data[length] )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Parse_Fw_Resources_Cb );

				}
				else if ( ' ' == data[length] )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( Prod_Info_Spec_cb );

				}
				else
				{
					/* No '?' mark allowed, bad request TODO If question mark is really a special case then it will be
					   handled later ON*/
					// hams_respond_error( parser, E404_NO_QUERY_ALLOWED );
					hams_respond_error( parser, 400U );
				}
				break;

			case REST_USER_MGMT:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else if ( ( ' ' == data[length] ) || ( '/' == data[length] ) )
				{
					parser->user_data_cb = static_cast<hams_data_cb>
						( Parse_UserMgmt_Resources_Cb );
				}
				else
				{
					// hams_respond_error( parser, E404_NO_QUERY_ALLOWED );
					hams_respond_error( parser, 400U );
				}
				break;

			case REST_STRUCT:
				/* TODO */
				hams_respond_error( parser, 501U );
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
						parser->user_data_cb = static_cast<hams_data_cb>( &Parse_Log_Resource_Cb );
					}
					else if ( ' ' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb );
					}
					else
					{
						hams_respond_error( parser, 400U );
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
						hams_respond_error( parser, 400U );
					}
				}
				break;

#endif
			default:
				hams_respond_error( parser, 400U );
				break;
		}
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static bool_t getXMLParamDescription( hams_parser* parser,
									  const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI )
{

#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
	uint16_t String_Size = strlen( ( char const* )pDCI->pDescription );

	Http_Server_Print_Dynamic( parser,
							   XML_DECL "<Description pid=\"\bu\"" ATTRIB_XMLNS_XLINK
							   ATTRIB_XLINK_HREF "/rs/param/" "\bu" "/description\">\"\bs\"</Description>",
							   pDCI->param_id, pDCI->param_id, String_Size, pDCI->pDescription );
#endif
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static bool_t getXMLParamRange( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI )
{
	bool_t return_val = false;
	DCI_ACCESS_ST_TD access_struct =
	{ 0U };
	uint8_t type = 0U;
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
			access_struct.data_access.data = dest_data;
			access_struct.data_access.offset = 0U;
			access_struct.data_id = pDCI->dcid;

			Http_Server_Print_Dynamic( parser,
									   XML_DECL "<Range pid=\"\bu\">\n"
									   "\t<Min>",
									   pDCI->param_id );

			access_struct.command = DCI_ACCESS_GET_MIN_CMD;
			if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
				 static_cast<uint32_t>( httpd_dci_access->Data_Access( &access_struct ) ) )
			{
				if ( Format_Handler_Check( parser, pDCI, dest_data, type_length, 0U ) == true )
				{
					Http_Server_Print_Dynamic( parser, "</Min>\n\t<Max>" );
					access_struct.command = DCI_ACCESS_GET_MAX_CMD;
					if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
						 static_cast<uint32_t>
						 ( httpd_dci_access->Data_Access( &access_struct ) ) )
					{
						if ( Format_Handler_Check( parser, pDCI, dest_data, type_length, 0U ) == true )
						{
							Http_Server_Print_Dynamic( parser, "</Max>\n</Range>" );
							return_val = true;
						}
					}
				}
				else
				{
					if ( false != s_dynamic_write_value( parser, type, dest_data, 0U ) )
					{
						Http_Server_Print_Dynamic( parser, "</Min>\n\t<Max>" );
						access_struct.command = DCI_ACCESS_GET_MAX_CMD;
						if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
							 static_cast<uint32_t>
							 ( httpd_dci_access->Data_Access( &access_struct ) ) )
						{
							if ( false != s_dynamic_write_value( parser, type, dest_data, 0U ) )
							{
								Http_Server_Print_Dynamic( parser, "</Max>\n</Range>" );
								return_val = true;
							}
						}

					}
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
static bool_t getXMLParamEnum( hams_parser* parser,
							   const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI )
{
	bool_t return_val = true;
	uint16_t enum_count = 0U;
	uint32_t content_length = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	bool_t status = false;

	if ( nullptr != pDCI->pEnumDes )
	{
		uint16_t i;
		enum_val_t enum_val = 0U;
		enum_count = pDCI->pEnumDes->total_enums;
		/* calculating content length of the response */
		content_length += STRLEN( XML_ENUM_PREFIX_1 );
		content_length += STRLEN( XML_ENUM_PREFIX_2 );
		content_length += STRLEN( XML_ENUM_PREFIX_3 );
		content_length += ( 2U
							* Find_Base_Log_Ten(
								pDCI->param_id ) );	// multipling by 2 as we are printing it two times
		content_length += STRLEN( XML_ENUM_TOTAL_PREFIX );
		content_length += STRLEN( XML_ENUM_TOTAL_SUFFIX );
		content_length += Find_Base_Log_Ten( enum_count );
		content_length += ( enum_count * ( STRLEN( XML_ENUM_VALUE_1 ) ) );
		content_length += ( enum_count * ( STRLEN( XML_ENUM_VALUE_2 ) ) );
		content_length += ( enum_count * ( STRLEN( XML_ENUM_VALUE_3 ) ) );
		content_length += ( enum_count * ( STRLEN( XML_ENUM_VALUE_4 ) ) );
		content_length += ( enum_count * ( Find_Base_Log_Ten( pDCI->param_id ) ) );


		for ( i = 0U; i < enum_count; i++ )
		{
			enum_val = pDCI->pEnumDes->enum_list[i].enum_val;
			if ( enum_val < ( static_cast < enum_val_t > ( 0 ) ) )
			{
				content_length += 1U;	// for negative sign
				enum_val *= -1;	// to convert negative number to positive to get digit count
			}
			content_length += Find_Base_Log_Ten( enum_val );
			content_length += Find_Base_Log_Ten( i );
		}

		content_length += STRLEN( XML_ENUM_SUFFIX );
		parser->total_content_length = content_length;
		parser->response_code = 200U;

		Http_Server_Print_Dynamic( parser, XML_ENUM_PREFIX_1 "\bu" XML_ENUM_PREFIX_2 "\bu" XML_ENUM_PREFIX_3,
								   pDCI->param_id, pDCI->param_id );
		Http_Server_Print_Dynamic( parser, XML_ENUM_TOTAL_PREFIX "\bu" XML_ENUM_TOTAL_SUFFIX, enum_count );

		for ( i = 0U; ( i < enum_count ) &&
			  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) ); i++ )
		{
			Http_Server_Print_Dynamic( parser, XML_ENUM_VALUE_1 "\bu" XML_ENUM_VALUE_2
									   "\bu" XML_ENUM_VALUE_3, pDCI->param_id, i );
			enum_val = pDCI->pEnumDes->enum_list[i].enum_val;
			if ( enum_val < ( static_cast < enum_val_t > ( 0 ) ) )
			{
				uint8_t type = 0U;
				if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) !=
					 httpd_dci_access->Get_Datatype( pDCI->dcid, ( DCI_DATATYPE_TD* ) ( &type ) ) )
				{
					hams_respond_error( parser, 400U );
				}

				status = s_dynamic_write_value( parser, type, reinterpret_cast<uint8_t*>( &enum_val ), 0U );
				if ( false == status )
				{
					REST_DEBUG_PRINT( DBG_MSG_ERROR, "Error in s_dynamic_write_value" );
				}
			}
			else
			{
				Http_Server_Print_Dynamic( parser, "\bu", enum_val );
			}
			Http_Server_Print_Dynamic( parser, XML_ENUM_VALUE_4 );
		}

		if ( true == return_val )
		{

			parser->data_transmitted = i;
			parser->general_use_reg = reinterpret_cast<uint32_t>( pDCI );
			if ( i < enum_count )
			{
				parser->transmission_continue = true;
				parser->user_data_cb_remaining = Get_Enum_Xml_All_Remaining;
			}
			else
			{
				parser->data_transmitted = 0U;
				parser->transmission_continue = false;
				Http_Server_Print_Dynamic( parser, XML_ENUM_SUFFIX );
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
static uint32_t Get_Enum_Xml_All_Remaining( hams_parser* parser, const uint8_t* data,
											uint32_t length )
{
	uint16_t i;
	uint16_t enum_count = 0U;
	enum_val_t enum_val = 0U;
	bool_t status = false;


	parser->blocks_for_data = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;

	const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = ( const DCI_REST_TO_DCID_LKUP_ST_TD* )( parser->general_use_reg );

	enum_count = pDCI->pEnumDes->total_enums;

	Http_Server_Begin_Dynamic( parser );
	for ( i = parser->data_transmitted; ( i < enum_count ) &&
		  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) ); i++ )
	{
		Http_Server_Print_Dynamic( parser, XML_ENUM_VALUE_1 "\bu" XML_ENUM_VALUE_2
								   "\bu" XML_ENUM_VALUE_3, pDCI->param_id, i );
		enum_val = pDCI->pEnumDes->enum_list[i].enum_val;
		if ( enum_val < ( static_cast < enum_val_t > ( 0 ) ) )
		{
			uint8_t type = 0U;
			if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) !=
				 httpd_dci_access->Get_Datatype( pDCI->dcid, ( DCI_DATATYPE_TD* ) ( &type ) ) )
			{
				hams_respond_error( parser, 400U );
			}
			status = s_dynamic_write_value( parser, type, reinterpret_cast<uint8_t*>( &enum_val ), 0U );
			if ( false == status )
			{
				REST_DEBUG_PRINT( DBG_MSG_ERROR, "Error in s_dynamic_write_value" );
			}
		}
		else
		{
			Http_Server_Print_Dynamic( parser, "\bu", enum_val );
		}
		Http_Server_Print_Dynamic( parser, XML_ENUM_VALUE_4 );
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
		Http_Server_Print_Dynamic( parser, XML_ENUM_SUFFIX );
	}
	Http_Server_End_Dynamic( parser );
	g_hams_demultiplexer->staged_len = 0U;
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Get_Enum_Description_Cb( hams_parser* parser, const uint8_t* data,
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
				uint32_t StringSize = 0U;
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
							parser->response_code = 200U;
							rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
							Http_Server_Begin_Dynamic( parser );
							Http_Server_Print_Dynamic( parser, XML_ENUM_DESCRIPTION_PREFIX,
													   pDCI->param_id, pDCI->param_id, enum_index );
							StringSize = strlen( pDCI->pEnumDes->enum_list[enum_index].description );
							Http_Server_Print_Dynamic( parser, XML_ENUM_DESCRIPTION, StringSize,
													   pDCI->pEnumDes->enum_list[enum_index].description );
							Http_Server_Print_Dynamic( parser, XML_ENUM_DESCRIPTION_SUFFIX );
						}
					}
					else
					{
						hams_respond_error( parser, 405U );
					}
				}
				else
				{
					hams_respond_error( parser, 404U );
				}
			}
			else
			{
				hams_respond_error( parser, 404U );
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
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
		}
	}
	else
	{
		if ( ( true == ENDS_WITH_SLASH( data ) ) || ( true == IS_EXTRA_SLASH( data ) ) )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Get_Enum_Description_Cb );
		}
		else
		{
			hams_respond_error( parser, 404U );
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static bool_t Get_XML_Param_Bitlist( hams_parser* parser,
									 const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI )
{
	bool_t return_val = true;
	bitfield_val_t bitfield_count = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( nullptr != pDCI->pBitfieldDes )
	{
		bitfield_count = pDCI->pBitfieldDes->total_bitfields;

		Http_Server_Print_Dynamic( parser, XML_BITFIELD_PREFIX, pDCI->param_id, pDCI->param_id );
		Http_Server_Print_Dynamic( parser, XML_BITFIELD_TOTAL, bitfield_count );

		for ( bitfield_val_t i = 0U; i < bitfield_count; i++ )
		{
			Http_Server_Print_Dynamic( parser, XML_BITFIELD_BIT_VALUE, pDCI->param_id,
									   pDCI->pBitfieldDes->bitfield_list[i].bitfield_val,
									   pDCI->pBitfieldDes->bitfield_list[i].bitfield_val );
		}
		Http_Server_Print_Dynamic( parser, XML_BITFIELD_SUFFIX );
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
static uint32_t Get_Bitfield_Description_Cb( hams_parser* parser, const uint8_t* data,
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
						parser->response_code = 200U;
						rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
						Http_Server_Begin_Dynamic( parser );
						Http_Server_Print_Dynamic( parser, XML_BITFIELD_DESCRIPTION_PREFIX,
												   pDCI->param_id, pDCI->param_id, bitfield_requested );
						bool_t loop_break = false;
						uint32_t StringSize = 0U;
						for ( bitfield_val_t i = 0U; ( i < bitfield_count ) && ( loop_break == false ); i++ )
						{
							if ( pDCI->pBitfieldDes->bitfield_list[i].bitfield_val == bitfield_requested )
							{
								StringSize = strlen( pDCI->pBitfieldDes->bitfield_list[i].description );
								Http_Server_Print_Dynamic( parser, XML_BITFIELD_DESCRIPTION, StringSize,
														   pDCI->pBitfieldDes->bitfield_list[i].description );
								loop_break = true;
							}
						}
						if ( false == loop_break )
						{
							hams_respond_error( parser, 404U );
						}
						Http_Server_Print_Dynamic( parser, XML_BITFIELD_DESCRIPTION_SUFFIX );
					}
					else
					{
						hams_respond_error( parser, 404U );
					}
				}
			}
			else
			{
				hams_respond_error( parser, 405U );
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
				hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml", 15U );
				hams_response_header( parser, COUNT_HTTP_HDRS,
									  reinterpret_cast<const void*>( NULL ), 0U );
			}
		}
	}
	else
	{
		if ( ( true == ENDS_WITH_SLASH( data ) ) || ( true == IS_EXTRA_SLASH( data ) ) )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Get_Bitfield_Description_Cb );
		}
		else
		{
			hams_respond_error( parser, 404U );
		}
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t getXMLParamSingleItem( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
							  uint32_t param, uint32_t verbose )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );
	bool_t return_val = false;
	bool_t function_end = false;
	UINT16 tot_length = 0U;
	DCI_DATATYPE_TD type = DCI_DTYPE_UINT8;
	UINT8 type_length = 0U;
	DCI_ACCESS_ST_TD access_struct =
	{ 0U };

	/* Write out XML tag name and REST parameter id */
	const char_t* tag = s_rest_param_xml_tags[param];

	if ( verbose )
	{
		Http_Server_Print_Dynamic( parser,
								   "<\bs pid=\"\bu\"" ATTRIB_XMLNS_XLINK,
								   strlen( tag ), tag, pDCI->param_id );

		/* Write out URI reference */
		Http_Server_Print_Dynamic( parser,
								   ATTRIB_XLINK_HREF "/rs/param/\bu\bs\">",	// Sada
								   pDCI->param_id,
								   strlen( s_rest_param_types[param] ),
								   s_rest_param_types[param] );
	}
	else
	{
		Http_Server_Print_Dynamic( parser, "<\bs pid=\"\bu\">",
								   strlen( tag ), tag, pDCI->param_id );
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
			access_struct.data_access.data = dest_data;
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
				if ( Format_Handler_Check( parser, pDCI, dest_data, tot_length, 0U ) == false )
				{
					// DCID does not attach with any format type
					if ( DCI_DTYPE_STRING8 == type )
					{
						Http_Server_Print_Dynamic( parser, "\bX",
												   strnlen( ( const char_t* )dest_data,
															tot_length ), dest_data );
					}
					else
					{
						Print_Rest_Format( parser, pDCI, dest_data, tot_length, 0U );
					}
				}

				Http_Server_Print_Dynamic( parser, "</\bs>", strlen( tag ), tag );
				Http_Server_Print_Dynamic( parser, "\n" );					// Sada
				return_val = true;
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
int32_t set_param_numeric( ReusedRecvBuf* pTarget,
						   const struct yxml_put_xml_state* ypx,
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
				if ( ( ypx->flags & YXML_PUT_IS_NEGATIVE ) &&
					 ( ypx->tmp_number > VALID_SINT8_MAX ) )
				{
					return_val = -2;
				}
				else if ( !( ypx->flags & YXML_PUT_IS_NEGATIVE ) &&
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
					if ( 0 != ( ypx->flags & YXML_PUT_IS_NEGATIVE ) )
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
				if ( 0U != ( ypx->flags & YXML_PUT_IS_NEGATIVE ) )
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
				if ( ( ypx->flags & YXML_PUT_IS_NEGATIVE ) &&
					 ( ypx->tmp_number > VALID_SINT16_MAX ) )
				{
					return_val = -4;
				}
				else if ( !( ypx->flags & YXML_PUT_IS_NEGATIVE ) &&
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
					if ( 0 != ( ypx->flags & YXML_PUT_IS_NEGATIVE ) )
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
				if ( 0U != ( ypx->flags & YXML_PUT_IS_NEGATIVE ) )
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
				if ( ( ypx->flags & YXML_PUT_IS_NEGATIVE ) &&
					 ( ypx->tmp_number > VALID_SINT32_MAX ) )
				{
					return_val = -6;
				}
				else if ( !( ypx->flags & YXML_PUT_IS_NEGATIVE ) &&
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
					if ( ypx->flags & YXML_PUT_IS_NEGATIVE )
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
				if ( 0U != ( ypx->flags & YXML_PUT_IS_NEGATIVE ) )
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
				if ( ( ypx->flags & YXML_PUT_IS_NEGATIVE ) &&
					 ( ypx->tmp_number > VALID_SINT64_MAX ) )
				{
					return_val = -8;
				}
				else if ( !( ypx->flags & YXML_PUT_IS_NEGATIVE ) &&
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
					if ( ypx->flags & YXML_PUT_IS_NEGATIVE )
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
					 ( 0U != ( ypx->flags & YXML_PUT_IS_NEGATIVE ) ) )
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t s_dynamic_write_value( hams_parser* parser, UINT8 type,
							  const UINT8* pData, UINT32 offset )
{

	bool_t return_val = true;
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
uint16_t putParamValue( const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
						ReusedRecvBuf* pTarget, const DCI_DATA_BLOCK_TD* data_block,
						bool_t realWrite, uint32_t param_type )
{
	int32_t return_val = static_cast<int32_t>( DCI_ERR_NO_ERROR );
	bool_t function_end = false;
	DCI_ACCESS_ST_TD access_struct =
	{ 0U };
	UINT32 num_of_units = *( data_block->length_ptr )
		/ ( DCI::Get_Datatype_Size( data_block->datatype ) );
	UINT32 i = 0U;
	UINT32 j = 0U;
	UINT32 data_limit = 0U;
	UINT16 array_index = 0U;
	UINT8 data = 0U;

	// _ASSERT( REST_PARAM_VALUE == param_type
	// || REST_PARAM_PRESENTVAL == param_type
	// || REST_PARAM_INITIAL == param_type );
	data_limit = *( data_block->length_ptr );

	if ( DCI_DTYPE_BOOL == data_block->datatype )
	{
		while ( array_index < data_limit )
		{
			data = pTarget->buf.u8_data[array_index];
			if ( ( data == true ) || ( data == false ) )
			{
				return_val = DCI_ERR_NO_ERROR;
			}
			else
			{
				return_val = DCI_ERR_EXCEEDS_RANGE;
				break;
			}
			array_index++;
		}
	}

	if ( pTarget->buf.s8_data[data_limit] != '\0' )
	{
		return_val = DCI_ERR_EXCEEDS_RANGE;
	}
	else
	{
		if ( DCI_ERR_NO_ERROR == return_val )
		{
			i = 0U;
			function_end = false;

			if ( DCI_DTYPE_STRING8 == data_block->datatype )
			{
				num_of_units = 1U;
				PTARGET_SET_DATA_PRESENT( pTarget->present, 0 );
			}
			while ( ( i < num_of_units ) && ( false == function_end ) )
			{
				if ( !PTARGET_DATA_PRESENT( pTarget->present, i ) )
				{
					i++;
				}
				else
				{
					// now we know that the i-th data is available
					j = num_of_units;
					access_struct.data_id = data_block->dcid;
					access_struct.source_id = REST_DCI_SOURCE_ID;
					if ( DCI_DTYPE_STRING8 == data_block->datatype )
					{
						access_struct.data_access.length = *( data_block->length_ptr );
						// Exception_Assert( access_struct.data_access.length <= DEST_BUFFER_SIZE );
						access_struct.data_access.data = &pTarget->buf.u8_data[0];
						access_struct.data_access.offset = 0U;
					}
					else
					{
						access_struct.data_access.length = ( j - i )
							* ( DCI::Get_Datatype_Size( data_block->datatype ) );
						// Exception_Assert( access_struct.data_access.length <= DEST_BUFFER_SIZE );
						access_struct.data_access.data = &pTarget->buf.u8_data[i
																			   * ( DCI::
																				   Get_Datatype_Size(
																					   data_block
																					   ->datatype ) )
							];
						access_struct.data_access.offset = i
							* ( DCI::Get_Datatype_Size( data_block->datatype ) );
					}
					if ( REST_PARAM_VALUE == param_type )
					{
						if ( Test_Bit( data_block->patron_attrib_info,
									   DCI_PATRON_ATTRIB_RAM_WR_DATA ) )
						{
							access_struct.command = DCI_ACCESS_SET_RAM_CMD;
							int32_t ret = static_cast<int32_t>( httpd_dci_access->Data_Access(
																	&access_struct ) );
							if ( static_cast<int32_t>( DCI_ERR_NO_ERROR ) != ret )
							{
								return_val = ret;
								function_end = true;
							}
						}
						else
						{
							return_val = DCI_ERR_RAM_READ_ONLY;
							function_end = true;
						}
						if ( ( Test_Bit( data_block->patron_attrib_info,
										 DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) ) &&
							 ( false == function_end ) )
						{
							access_struct.command = DCI_ACCESS_SET_INIT_CMD;

							int32_t ret = static_cast<int32_t>( httpd_dci_access->Data_Access(
																	&access_struct ) );
							if ( static_cast<int32_t>( DCI_ERR_NO_ERROR ) != ret )
							{
								return_val = ret;
								function_end = true;
							}
						}
					}
					else if ( REST_PARAM_INITIAL == param_type )
					{
						access_struct.command = DCI_ACCESS_SET_INIT_CMD;
						int32_t ret = static_cast<int32_t>( httpd_dci_access->Data_Access(
																&access_struct ) );
						if ( static_cast<int32_t>( DCI_ERR_NO_ERROR ) != ret )
						{
							return_val = ret;
							function_end = true;
						}
					}
					else if ( REST_PARAM_PRESENTVAL == param_type )
					{
						access_struct.command = DCI_ACCESS_SET_RAM_CMD;
						int32_t ret = static_cast<int32_t>( httpd_dci_access->Data_Access(
																&access_struct ) );
						if ( static_cast<int32_t>( DCI_ERR_NO_ERROR ) != ret )
						{
							return_val = ret;
							function_end = true;
						}
					}
					else
					{
						// MISRA Suppress
					}
					/*
					   if (REST_PARAM_VALUE == param_type)
					   {
					   access_struct.command = DCI_ACCESS_SET_INIT_CMD;
					   }
					   if (DCI_ERR_NO_ERROR != httpd_dci_access->Data_Access(&access_struct))
					   {
					   return false;
					   }
					 */
					i = j;
				}
			}
		}
	}
	uint16_t error_code = 0;

	switch ( return_val )
	{
		case DCI_ERR_NO_ERROR:
			/* Nothing more to do, just report success. */
			error_code = 200U;
			break;

		case DCI_ERR_INVALID_COMMAND:
			error_code = E500_RV_INVALID_COMMAND;
			break;

		case DCI_ERR_INVALID_DATA_ID:
			error_code = 404U;
			break;

		case DCI_ERR_ACCESS_VIOLATION:
			error_code = E500_RV_ACCESS_VIOLATION;
			break;

		case DCI_ERR_EXCEEDS_RANGE:
			error_code = 400U;
			break;

		case DCI_ERR_RAM_READ_ONLY:
			error_code = 405U;
			break;

		case DCI_ERR_NV_NOT_AVAILABLE:
			error_code = E500_RV_NV_VALUE_INACCESSIBLE;
			break;

		case DCI_ERR_NV_ACCESS_FAILURE:
			error_code = E500_RV_NV_ACCESS_FAILURE;
			break;

		case DCI_ERR_NV_READ_ONLY:
			error_code = E500_RV_NV_READ_ONLY;
			break;

		case DCI_ERR_DEFAULT_NOT_AVAILABLE:
			error_code = E500_RV_DEFAULT_NOT_AVAILABLE;
			break;

		case DCI_ERR_RANGE_NOT_AVAILABLE:
			error_code = E500_RV_RANGE_NOT_AVAILABLE;
			break;

		case DCI_ERR_ENUM_NOT_AVAILABLE:
			error_code = E500_RV_ENUM_NOT_AVAILABLE;
			break;

		case DCI_ERR_INVALID_DATA_LENGTH:
			error_code = E500_RV_INVALID_DATA_LENGTH;
			break;

		case DCI_ERR_VALUE_LOCKED:
			error_code = E500_RV_VALUE_LOCKED;
			break;

		case DCI_ERR_DATA_INCORRECT:
			error_code = 400;
			break;

		default:
			error_code = 500U;
			break;
	}
	return ( error_code );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t rest_root_handler( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	return ( s_rest_root_handler( parser, data, length ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t rest_other_handler( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	return ( s_rest_other_handler( parser, data, length ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Find_Base_Log_Ten( uint16_t number )
{
	if ( number < 10U )
	{
		number = 1U;
	}
	else if ( number < 100U )
	{
		number = 2U;
	}
	else if ( number < 1000U )
	{
		number = 3U;
	}
	else if ( number < 10000 )
	{
		number = 4U;
	}
	else
	{
		number = 5U;
	}
	return ( number );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Print_Rest_Format( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
							uint8_t* data, uint32_t tot_length, uint32_t offset )
{
	uint16_t arr_cnt = 0U;

	if ( Format_Handler_Check( parser, pDCI, data, tot_length, offset ) == false )
	{
		bool_t first = true;
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

			if ( false == s_dynamic_write_value( parser, datatype, data, offset ) )
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
void Initialize_pTarget_Buffer( ReusedRecvBuf* pTarget )
{
	/* this function should be called only if pTarget buffer is being re-used.
	 * first time Http_Server_Alloc_Scratch function initialize whole buffer to zero already.
	 */
	memset( ( void* )pTarget->present, 0, DATA_PRESENT_SIZE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Clone_pTarget_Present_Data( present_t* present_dest, const uint8_t* present_src,
								 uint16_t present_dest_len )
{
	if ( present_dest_len <= DATA_PRESENT_SIZE )
	{
		memcpy( present_dest, present_src, present_dest_len );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Format_Handler_Check( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
						   uint8_t* dest_data, uint32_t length, uint32_t offset )
{
	bool status = false;
	DCI_DATATYPE_TD type = DCI_DTYPE_UINT8;

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
			// Format Type = CRED()
			if ( DCI_DTYPE_STRING8 == type )
			{
				Http_Server_Print_Dynamic( parser, "\bX",
										   strnlen( ( const char_t* )dest_data,
													length ), dest_data + offset );
			}
			else
			{
				Http_Server_Print_Dynamic( parser, ( const char* )dest_data + offset );
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
					Http_Server_Print_Dynamic( parser, ( const char* )formatted_string );
					status = true;
				}
				Http_Server_Free( formatted_string );
			}
		}
	}
#endif
	return ( status );
}
