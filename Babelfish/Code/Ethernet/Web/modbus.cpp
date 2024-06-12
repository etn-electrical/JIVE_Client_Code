/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "httpd_rs_struct.h"
#include "httpd_rest_dci.h"

#include "httpd_xml.h"

#include "BIN_ASCII_Conversion.h"
#include "Modbus_Net.h"
#include "Babelfish_Assert.h"

#define HAMS_DEBUG
#ifdef HAMS_DEBUG
// #define DERR(...) fprintf(stderr, __VA_ARGS__)
#define DERR( ... )
#define _ASSERT( x ) do { if ( !( x ) )while ( 1 ){} \
} while ( 0 )
#else
#define DERR( ... )
#define _ASSERT( x )
#endif
#define MODBUS   "POST, OPTIONS"
// extern "C"{
#include "server.h"
#include "rest.h"
/* Generic HAMS handler functions.  */
extern uint32_t S_Xmit_Dynamic_Data( hams_parser* parser, const uint8_t* data,
									 uint32_t length );

// }

inline static uint8_t s_hex( uint8_t v )
{
	uint8_t return_val;

	if ( ( v >= 0x30U ) && ( v <= 0x39U ) )
	{
		return_val = ( v - 0x30U );
	}
	else if ( ( v >= 0x41U ) && ( v <= 0x46U ) )
	{
		return_val = ( v - 0x41U ) + 10U;
	}
	else
	{
		return_val = 0x10U;
	}

	return ( return_val );
}

extern Modbus_Net* modbus_tcp;

static uint32_t s_modbus_tx_request( hams_parser* parser, const uint8_t* data, uint32_t length );

/* Expecting a modbus request in the user content data. */
// extern "C"
uint32_t s_modbus_rx_request( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	bool_t end_function = false;
	uint8_t modbus_num_bocks = ( MODBUS_ON_REST_BUFFER_SIZE + ( HTTP_POOL_BLOCK_SIZE - 1 ) ) / HTTP_POOL_BLOCK_SIZE;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( ( false == end_function ) && ( 0U != hams_check_headers_ended( data, length ) ) )
	{
		if ( parser->method != HTTP_METHOD_OPTIONS )
		{
			/* Make sure we have a content body. */
			if ( 0U == rs->max_auth_exempt_level )
			{
				hams_respond_error( parser, 401U );
				end_function = true;
			}
			else if ( 0U == parser->content_length )
			{
				hams_respond_error( parser, 411U );
				end_function = true;
			}
			/* Only accept */
#ifdef JSON_ACTIVE
			else if ( parser->content_type != HTTP_CONTENT_TYPE_JSON )
#else
			else if ( parser->content_type != HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED )
#endif
			{
				hams_respond_error( parser, 415U );
				end_function = true;
			}
			else
			{
				/* This must be 0; as initialized in hams_parser_init. */
				// _ASSERT( 0 == parser->ext.multipart_boundary_length );
				/* Abusing the multipart area for storing the modbus request, so don't use the multipart area for
				   forming a dynamic response. */
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
			}
		}
		else
		{
			parser->response_code = 200U;
			http_server_req_state* rs = Http_Server_Get_State( parser );
			rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
			end_function = true;

			if ( ( 0U == parser->cors_auth_common->origin_length ) ||
				 ( NULL == parser->cors_auth_common->origin_buffer ) )
			{
				hams_respond_error( parser, 400U );
			}
			else
			{
				parser->user_data_cb = ( s_modbus_tx_request );
			}
		}
	}

	if ( false == end_function )
	{
		/* Abuse the multipart boundary area for storing the modbus request. */
		if ( 0U != ( data && length ) )
		{
			if ( ( parser->ext.multipart_boundary_length + length ) >
				 MAX_MULTIPART_BOUNDARY_LENGTH )
			{
				hams_respond_error( parser, 413U );
				end_function = true;
			}
			else
			{
				memcpy( parser->ext.multipart_boundary +
						parser->ext.multipart_boundary_length, data, length );
				parser->ext.multipart_boundary_length += length;
			}
		}

		if ( ( 0 == data ) && ( false == end_function ) )
		{

			/* Look for odd number of digits. */
			if ( ( 0U != ( parser->ext.multipart_boundary_length & 1U ) ) &&
				 !( ( IS_SINGLE_EXTRA_SLASH( parser->ext.multipart_boundary ) ) ) )
			{
				hams_respond_error( parser, 403U );
				end_function = true;
			}
			else
			{
				/* Convert the input to binary. */
				uint8_t* o = parser->ext.multipart_boundary;

				uint8_t prefix_count = 0;
				if ( ( IS_EXTRA_SLASH( o ) ) )
				{
					prefix_count += 2;
				}
				else if ( ( IS_SINGLE_EXTRA_SLASH( o ) ) )
				{
					prefix_count += 1;
				}
				if ( ( parser->ext.multipart_boundary
					   [prefix_count] != 0x30U ) || ( parser->ext.multipart_boundary
													  [prefix_count + 1] != 0x78U ) )
				{
					hams_respond_error( parser, 403U );
					end_function = true;
				}
				else
				{
					/* Skip over 0x */
					const uint8_t* src = parser->ext.multipart_boundary + 2 + prefix_count;
					const uint8_t* end =
						parser->ext.multipart_boundary + parser->ext.multipart_boundary_length;

					while ( ( src != end ) && ( false == end_function ) )
					{
						*o = s_hex( src[0] );
						if ( *o >= 0x10U )
						{
							hams_respond_error( parser, 400U );
							end_function = true;
						}
						if ( false == end_function )
						{
							*o <<= 4U;
							uint8_t low = s_hex( src[1] );
							if ( low >= 0x10U )
							{
								hams_respond_error( parser, 400U );
								end_function = true;
							}
							else
							{
								*o |= low;
								++o;
								src += 2U;
							}
						}
					}
				}
				if ( false == end_function )
				{
					/* Reached end of body...Process the modbus request. */

					MB_FRAME_STRUCT rx;
					MB_FRAME_STRUCT tx;
					rx.data = parser->ext.multipart_boundary;
					rx.data_length = ( parser->ext.multipart_boundary_length - 2U - prefix_count )
						/ 2U;

					/* Allocate MODBUS_ON_REST_BUFFER_SIZE bytes of space for POST request defined in RestConfig.h */
					tx.data = reinterpret_cast<uint8_t*>( Http_Server_Alloc_Scratch( parser, modbus_num_bocks ) );
					if ( tx.data == nullptr )
					{
						hams_respond_error( parser, E503_SCRATCH_SPACE );
						end_function = true;
					}
					else
					{
						/* Only allow reads. Writes are unauthorized. */
						if ( 0x03U != rx.data[1] )
						{
							hams_respond_error( parser, 403U );
							end_function = true;
						}
						else
						{
							if ( modbus_tcp == nullptr )
							{
								hams_respond_error( parser, 503U );
								end_function = true;
							}
							else
							{
								/* If an error then assuming bad response. */
								uint32_t mb_err = modbus_tcp->Process_Msg( &rx, &tx );

								/* scratch allocation of 128 bytes is not sufficient so increased the space allocation
								   for the POST request by increasing the value of MODBUS_ON_REST_BUFFER_SIZE as
								   multiple of 128
								 */
								BF_ASSERT( tx.data_length <= MODBUS_ON_REST_BUFFER_SIZE );

								if ( ( MB_NO_ERROR_CODE != mb_err ) || ( 0U == tx.data_length ) )
								{
									uint32_t err_code = 503U;
									switch ( mb_err )
									{
										case MB_ILLEGAL_FUNCTION_ERROR_CODE:
											err_code = E400_MB_ILLEGAL_FUNCTION_CODE;
											break;

										case MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE:
											err_code = E400_MB_DATA_ADDRESS_ERROR;
											break;

										case MB_ILLEGAL_DATA_VALUE_ERROR_CODE:
											err_code = E400_MB_DATA_VALUE_ERROR;
											break;

										case MB_SLAVE_DEVICE_FAILURE_ERROR_CODE:
											err_code = E503_MB_DEVICE_FAILURE_ERROR;
											break;

										case MB_SLAVE_DEVICE_ACK_ERROR_CODE:
											err_code = E503_MB_DEVICE_ACK_ERROR;
											break;

										case MB_SLAVE_DEVICE_BUSY_ERROR_CODE:
											err_code = E503_MB_DEVICE_BUSY_ERROR;
											break;

										case MB_GENERAL_RECEIVE_ERROR_CODE:
											err_code = E503_MB_GENERAL_RECEIVE_ERROR;
											break;

										case MB_DEST_STRING_SIZE_WRONG_ERROR_CODE:
											err_code = E400_MB_STRING_SIZE_WRONG;
											break;

										case MB_INVALID_INTERCOM_BLOCK_ERROR_CODE:
											err_code = E503_MB_INTERCOM_BLOCK_ERROR;
											break;

										case MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE:
											err_code = E503_MB_NO_SLAVE_RESPONSE;
											break;

										default:
											break;
									}
									hams_respond_error( parser, err_code );
									end_function = true;
								}
								else
								{
									parser->response_code = 200U;
									http_server_req_state* rs = Http_Server_Get_State( parser );
									rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;

									Http_Server_Begin_Dynamic( parser );

									Http_Server_Print_Dynamic( parser, "0x" );
									for ( uint32_t i = 0U; i < tx.data_length; ++i )
									{
										Http_Server_Print_Dynamic( parser, "\bb", tx.data[i] );
									}
									parser->user_data_cb = ( s_modbus_tx_request );
								}
							}
						}
						Http_Server_Free( tx.data );
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
static uint32_t s_modbus_tx_request( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	// _ASSERT( hams_can_respond(data, length) );
	if ( ( HTTP_METHOD_OPTIONS == parser->method ) && ( length != 1U ) )
	{
		hams_response_header( parser, HTTP_HDR_ALLOW, MODBUS, STRLEN( MODBUS ) );
		hams_response_header( parser, COUNT_HTTP_HDRS, reinterpret_cast<const void*>( NULL ), 0U );
	}
	else if ( ( HTTP_METHOD_OPTIONS != parser->method ) && ( 0U != Http_Server_End_Dynamic( parser ) ) )
	{
		hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "text/plain", 10U );
		hams_response_header( parser, COUNT_HTTP_HDRS, ( reinterpret_cast<const void*>( NULL ) ),
							  0U );
	}
	return ( 0U );
}
