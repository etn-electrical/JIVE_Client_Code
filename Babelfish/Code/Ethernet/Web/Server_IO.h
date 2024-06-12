/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __SERVER_IO_H__
#define __SERVER_IO_H__


#include "hams.h"

typedef uint32_t SERVER_SEND_CBACK_TD ( hams_parser* parser, const uint8_t* data, uint32_t length,
										uint32_t copy );

struct server_media_info_t
{
	SERVER_SEND_CBACK_TD* send_cb;
	DCI_Owner* enable_media;
};

void Register_Media_On_Server( web_server_media_t media, DCI_Owner* enable_media,
							   SERVER_SEND_CBACK_TD* send_cb );

bool_t Is_Conn_Enabled( web_server_media_t media );

uint32_t SERVER_IO_Send_cb( hams_parser* parser, const uint8_t* data, uint32_t length,
							uint32_t copy );

uint32_t Serverif_Send_Size( http_server_req_state* rs );

#endif
