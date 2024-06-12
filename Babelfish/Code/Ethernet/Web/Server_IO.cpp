/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "includes.h"
#include "server.h"
#include "Server_IO.h"
#include "lwip/tcp.h"

server_media_info_t server_media_info[TOTAL_NUM_MEDIA] = {
	{ nullptr, nullptr },
	{ nullptr, nullptr }
};


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Register_Media_On_Server( web_server_media_t media, DCI_Owner* enable_media,
							   SERVER_SEND_CBACK_TD* send_cb )
{
	if ( media < TOTAL_NUM_MEDIA )
	{
		server_media_info[media].send_cb = send_cb;
		server_media_info[media].enable_media = enable_media;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t SERVER_IO_Send_cb( hams_parser* parser, const uint8_t* data, uint32_t length,
							uint32_t copy )
{
	uint32_t return_val = 0U;

	if ( ( parser->https == HTTP_MEDIA ) && ( server_media_info[HTTP_MEDIA].send_cb != nullptr ) )
	{
		return_val = server_media_info[HTTP_MEDIA].send_cb( parser, data, length, copy );
	}
	else if ( ( parser->https == HTTPS_MEDIA ) && ( server_media_info[HTTPS_MEDIA].send_cb != nullptr ) )
	{
		return_val = server_media_info[HTTPS_MEDIA].send_cb( parser, data, length, copy );
	}
	else
	{
		// misra
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Is_Conn_Enabled( web_server_media_t media )
{
	bool http_on = false;

	if ( media < TOTAL_NUM_MEDIA )
	{
		if ( server_media_info[media].send_cb != nullptr )
		{
			if ( server_media_info[media].enable_media != nullptr )
			{
				server_media_info[media].enable_media->Check_Out( http_on );
			}
			else
			{
				http_on = true;
			}
		}
		else
		{
			http_on = false;
		}
	}
	return ( http_on );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Serverif_Send_Size( http_server_req_state* rs )
{
	uint32_t send_size = 0U;

	if ( rs->parser.https != 1U )
	{
		tcp_pcb* pcb = ( reinterpret_cast<tcp_pcb*>( rs->stack_data ) );
		send_size = tcp_sndbuf( pcb );
	}
	else
	{
		send_size = rs->parser.content_length;
	}
	return ( send_size );
}
