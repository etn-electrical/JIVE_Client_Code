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

static Eth_if* s_ethernet_ctrl = NULL;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Update_Ethernet_Handle( Eth_if* ethernet_ctrl )
{
	s_ethernet_ctrl = ethernet_ctrl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t s_diag_write_cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	// _ASSERT( hams_can_respond(data, length) );

	/* First, output all the headers (except content length)
	 * before beginning dynamic response. */
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
	Http_Server_Print_Dynamic( parser, "{\n" );

	for ( uint8_t i = 0U; i < COUNT_STATS; ++i )
	{
		const struct stats_proto* proto = the_stats[i];
		switch ( i )
		{
			case 0U:
				Http_Server_Print_Dynamic( parser, "\"ip\":{\n" );
				break;

			case 1U:
				Http_Server_Print_Dynamic( parser, ",\n\"tcp\":{\n" );
				break;

			case 2U:
				Http_Server_Print_Dynamic( parser, ",\n\"udp\":{\n" );
				break;

			case 3U:
				Http_Server_Print_Dynamic( parser, ",\n\"link\":{\n" );
				break;

			default:
				break;
		}

		Http_Server_Print_Dynamic( parser,
								   "\t\"xmit\":\bu\n"
								   "\t,\"recv\":\bu\n"
								   "\t,\"fw\":\bu\n"
								   "\t,\"drop\":\bu\n",
								   proto->xmit,
								   proto->recv,
								   proto->fw,
								   proto->drop );

		Http_Server_Print_Dynamic( parser,
								   "\t,\"chkerr\":\bu\n"
								   "\t,\"lenerr\":\bu\n"
								   "\t,\"memerr\":\bu\n"
								   "\t,\"rterr\":\bu\n",
								   proto->chkerr,
								   proto->lenerr,
								   proto->memerr,
								   proto->rterr );

		Http_Server_Print_Dynamic( parser,
								   "\t,\"proterr\":\bu\n"
								   "\t,\"opterr\":\bu\n"
								   "\t,\"err\":\bu\n"
								   "\t,\"cachehit\":\bu\n\t}",
								   proto->proterr,
								   proto->opterr,
								   proto->err,
								   proto->cachehit );
	}
	phy_ports = s_ethernet_ctrl->Read_Port_Count();

	for ( uint8_t idx = 0; idx < phy_ports; idx++ )
	{
		s_phy_err_counter = s_ethernet_ctrl->Read_Diag_Information( idx );
		const uint8_t* const* s_phy_err_name = s_ethernet_ctrl->Read_Phy_Driver_Error_List();

		if ( s_phy_err_counter != NULL )
		{
			Http_Server_Print_Dynamic( parser, ",\n\"phy \bu\":{\n"
									   "\t\"\bs\":\bu\n"
									   "\t,\"\bs\":\bu\n"
									   "\t,\"\bs\":\bu\n"
									   "\t,\"\bs\":\bu\n"
									   "\t,\"\bs\":\bu\n"
									   "\t,\"\bs\":\bu\n"
									   "\t,\"\bs\":\bu\n"
									   "\t,\"\bs\":\bu\n"
									   "\t,\"\bs\":\bu\n\t}",
									   idx,
									   strlen(
										   reinterpret_cast<const char*>( s_phy_err_name[0] ) ),
									   reinterpret_cast<const char*>( s_phy_err_name[0] ),
									   s_phy_err_counter[0U],
									   strlen(
										   reinterpret_cast<const char*>( s_phy_err_name[1] ) ),
									   reinterpret_cast<const char*>( s_phy_err_name[1] ),
									   s_phy_err_counter[1U],
									   strlen(
										   reinterpret_cast<const char*>( s_phy_err_name[2] ) ),
									   reinterpret_cast<const char*>( s_phy_err_name[2] ),
									   s_phy_err_counter[2U],
									   strlen(
										   reinterpret_cast<const char*>( s_phy_err_name[3] ) ),
									   reinterpret_cast<const char*>( s_phy_err_name[3] ),
									   s_phy_err_counter[3U],
									   strlen(
										   reinterpret_cast<const char*>( s_phy_err_name[4] ) ),
									   reinterpret_cast<const char*>( s_phy_err_name[4] ),
									   s_phy_err_counter[4U],
									   strlen(
										   reinterpret_cast<const char*>( s_phy_err_name[5] ) ),
									   reinterpret_cast<const char*>( s_phy_err_name[5] ),
									   s_phy_err_counter[5U],
									   strlen(
										   reinterpret_cast<const char*>( s_phy_err_name[6] ) ),
									   reinterpret_cast<const char*>( s_phy_err_name[6] ),
									   s_phy_err_counter[6U],
									   strlen(
										   reinterpret_cast<const char*>( s_phy_err_name[7] ) ),
									   reinterpret_cast<const char*>( s_phy_err_name[7] ),
									   s_phy_err_counter[7U],
									   strlen(
										   reinterpret_cast<const char*>( s_phy_err_name[8] ) ),
									   reinterpret_cast<const char*>( s_phy_err_name[8] ),
									   s_phy_err_counter[8U] );
		}
	}
	Http_Server_Print_Dynamic( parser, "\n}" );

	if ( 0U != Http_Server_End_Dynamic( parser ) )
	{
		hams_response_header( parser, static_cast<uint32_t>( HTTP_HDR_CONTENT_TYPE ),
							  "application/json", 16U );
		hams_response_header( parser, static_cast<uint32_t>( COUNT_HTTP_HDRS ),
							  reinterpret_cast<void*>( NULL ), 0U );
	}

	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t s_lwip_diag_cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint8_t return_val = 0U;

	/* Skip headers. */
	if ( 0U != hams_check_headers_ended( data, length ) )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		parser->response_code = 200U;

		if ( HTTP_METHOD_OPTIONS == parser->method )
		{
			Validate_Options_Cors_Headers( parser, rs, nullptr );
		}
		/* There should not be any request body.. */
		else if ( 0U != parser->content_length )
		{
			hams_respond_error( parser, 413U );

		}
		/* Request checks out, compose our headers */
		else
		{
			if ( rs->max_auth_exempt_level != 0U )
			{
				parser->user_data_cb = ( s_diag_write_cb );
			}
			else
			{
				hams_respond_error( parser, 401U );
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
								  reinterpret_cast<const void*>( NULL ), 0U );
		}
		return_val = 0U;
	}

	return ( return_val );
}
