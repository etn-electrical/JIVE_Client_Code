/**
 *****************************************************************************************
 * @file    Diag_Json.cpp
 * @details See header file for module overview.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Json_Packetizer.h"
#include "iot_json_parser.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "httpd_rs_struct.h"
#include "httpd_rest_dci.h"
#include "httpd_xml.h"
#include "server.h"
#include "BIN_ASCII_Conversion.h"
#include "Eth_if.h"
#include "REST_Common.h"

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
#define COUNT_STATS 4U
#define Diag_LWIP  "GET, HEAD, OPTIONS"

static Eth_if* s_ethernet_ctrl = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Update_Ethernet_Handle_Json( Eth_if* ethernet_ctrl )
{
	s_ethernet_ctrl = ethernet_ctrl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Diag_Write_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	// _ASSERT( hams_can_respond(data, length) );

	/* First, output all the headers (except content length) before beginning dynamic response. */
	uint32_t const* s_phy_err_counter;
	uint8_t phy_ports;

	static const struct stats_proto* the_stats[COUNT_STATS] =
	{
		&lwip_stats.ip,
		&lwip_stats.tcp,
		&lwip_stats.udp,
		&lwip_stats.link
	};

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
		Open_Json_Object( json_buffer );

		for ( uint8_t i = 0U; i < COUNT_STATS; ++i )
		{
			const struct stats_proto* proto = the_stats[i];
			switch ( i )
			{
				case 0U:
					Add_String_Member( json_buffer, "ip", nullptr, false );
					Open_Json_Object( json_buffer );
					break;

				case 1U:
					Add_Comma( json_buffer );
					Add_String_Member( json_buffer, "tcp", nullptr, false );
					Open_Json_Object( json_buffer );
					break;

				case 2U:
					Add_Comma( json_buffer );
					Add_String_Member( json_buffer, "udp", nullptr, false );
					Open_Json_Object( json_buffer );
					break;

				case 3U:
					Add_Comma( json_buffer );
					Add_String_Member( json_buffer, "link", nullptr, false );
					Open_Json_Object( json_buffer );
					break;

				default:
					break;
			}

			Add_Integer_Member( json_buffer, "xmit", proto->xmit, true );
			Add_Integer_Member( json_buffer, "recv", proto->recv, true );
			Add_Integer_Member( json_buffer, "fw", proto->fw, true );
			Add_Integer_Member( json_buffer, "drop", proto->drop, true );

			Add_Integer_Member( json_buffer, "chkerr", proto->chkerr, true );
			Add_Integer_Member( json_buffer, "lenerr", proto->lenerr, true );
			Add_Integer_Member( json_buffer, "memerr", proto->memerr, true );
			Add_Integer_Member( json_buffer, "rterr", proto->rterr, true );

			Add_Integer_Member( json_buffer, "proterr", proto->proterr, true );
			Add_Integer_Member( json_buffer, "opterr", proto->opterr, true );
			Add_Integer_Member( json_buffer, "err", proto->err, true );
			Add_Integer_Member( json_buffer, "cachehit", proto->cachehit, false );
			Close_Json_Object( json_buffer, false );
			Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

			json_buffer->used_buff_len = 0U;
			memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
		}
		phy_ports = s_ethernet_ctrl->Read_Port_Count();

		for ( uint8_t idx = 0; idx < phy_ports; idx++ )
		{
			s_phy_err_counter = s_ethernet_ctrl->Read_Diag_Information( idx );
			const uint8_t* const* s_phy_err_name = s_ethernet_ctrl->Read_Phy_Driver_Error_List();

			if ( s_phy_err_counter != nullptr )
			{
				Add_Comma( json_buffer );
				snprintf( formatted_buff, BUFFER_LEN, "phy %d", idx );
				Add_String_Member( json_buffer, formatted_buff, nullptr, false );
				Open_Json_Object( json_buffer );
				Add_Integer_Member( json_buffer, ( char_t* )s_phy_err_name[0], s_phy_err_counter[0], true );
				Add_Integer_Member( json_buffer, ( char_t* )s_phy_err_name[1], s_phy_err_counter[1], true );
				Add_Integer_Member( json_buffer, ( char_t* )s_phy_err_name[2], s_phy_err_counter[2], true );
				Add_Integer_Member( json_buffer, ( char_t* )s_phy_err_name[3], s_phy_err_counter[3], true );
				Add_Integer_Member( json_buffer, ( char_t* )s_phy_err_name[4], s_phy_err_counter[4], true );
				Add_Integer_Member( json_buffer, ( char_t* )s_phy_err_name[5], s_phy_err_counter[5], true );
				Add_Integer_Member( json_buffer, ( char_t* )s_phy_err_name[6], s_phy_err_counter[6], true );
				Add_Integer_Member( json_buffer, ( char_t* )s_phy_err_name[7], s_phy_err_counter[7], true );
				Add_Integer_Member( json_buffer, ( char_t* )s_phy_err_name[8], s_phy_err_counter[8], false );

				Close_Json_Object( json_buffer, false );
				Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

				json_buffer->used_buff_len = 0U;
				memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
			}
		}
		Close_Json_Object( json_buffer, false );
		Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
		Ram::De_Allocate( json_buffer );
	}

	if ( 0U != Http_Server_End_Dynamic( parser ) )
	{
		hams_response_header( parser, static_cast<uint32_t>( HTTP_HDR_CONTENT_TYPE ),
							  HTTP_HDR_DATA_TYPE, HTTP_HDR_DATA_LEN );
		hams_response_header( parser, static_cast<uint32_t>( COUNT_HTTP_HDRS ),
							  reinterpret_cast<void*>( nullptr ), 0U );
	}

	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t S_Lwip_Diag_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint32_t return_val = 0U;

	/* Skip headers. */
	if ( 0U != hams_check_headers_ended( data, length ) )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		parser->response_code = RESPONSE_OK;

		if ( HTTP_METHOD_OPTIONS == parser->method )
		{
			Validate_Options_Cors_Headers( parser, rs, nullptr );
		}
		/* There should not be any request body.. */
		else if ( 0U != parser->content_length )
		{
			hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );

		}
		/* Request checks out, compose our headers */
		else
		{
			if ( rs->max_auth_exempt_level != 0U )
			{
				parser->user_data_cb = ( S_Diag_Write_Cb_Json );
			}
			else
			{
				hams_respond_error( parser, ERROR_UNAUTHORIZED );
			}
		}
		return_val = 0U;
	}
	/* There should not be any request body.. */
	else
	{
		if ( HTTP_METHOD_OPTIONS == parser->method )
		{
			hams_response_header( parser, HTTP_HDR_ALLOW, Diag_LWIP, STRLEN( Diag_LWIP ) );
			hams_response_header( parser, COUNT_HTTP_HDRS,
								  reinterpret_cast<const void*>( nullptr ), 0U );
		}
		return_val = 0U;
	}

	return ( return_val );
}
