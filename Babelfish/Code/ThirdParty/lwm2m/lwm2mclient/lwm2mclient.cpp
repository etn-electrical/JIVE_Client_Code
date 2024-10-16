/*******************************************************************************
*
* Copyright (c) 2013, 2014 Intel Corporation and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at
*    http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    David Navarro, Intel Corporation - initial API and implementation
*    Benjamin Cabé - Please refer to git log
*    Fabien Fleutot - Please refer to git log
*    Simon Bernard - Please refer to git log
*    Julien Vermillard - Please refer to git log
*    Axel Lorente - Please refer to git log
*    Toby Jaffey - Please refer to git log
*    Bosch Software Innovations GmbH - Please refer to git log
*    Pascal Rieux - Please refer to git log
*    Christian Renz - Please refer to git log
*    Ricky Liu - Please refer to git log
*    Gregory Lemercier, Samsung - Please refer to git log
*
*******************************************************************************/

/*
   Copyright (c) 2013, 2014 Intel Corporation

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
   THE POSSIBILITY OF SUCH DAMAGE.

   David Navarro <david.navarro@intel.com>
   Bosch Software Innovations GmbH - Please refer to git log

 */

#include "lwm2mclient.h"
#include "liblwm2m.h"
#include "connection.h"
#include "internals.h"
#include "er-coap-13/er-coap-13.h"
#include "..\..\..\..\..\Code\Prod_Spec_LWM2M\LWM2M_Debug.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
// LTK_4064: Removed this file as we are using lwip/socket.h which is getting 
// included here through "connection.h"
//#include <sockets.h>
   
// LTK_4064: We are using lwip_error.h hence commenting this header file inclusion
//#include <errno.h>
#include <time.h>

#define CLIENT_PORT_RANGE_START    64900
#define CLIENT_PORT_RANGE_END      64999

#define MAX_PACKET_SIZE            1024
#define MAX_CONNECTION_RETRIES     5
#define MAX_NUMBER_CLIENTS         1

typedef struct
{
	coap_protocol_t proto;
	char uri_prefix[16];
	char friendly_name[16];
} coap_uri_protocol;

typedef struct
{
	lwm2m_object_t* securityObjP;
	lwm2m_object_t* serverObject;
	int sock;
	connection_t* connList;
	lwm2m_context_t* lwm2mH;
	struct sockaddr server_addr;
	size_t server_addrlen;
	connection_t*conn;
	bool verify_cert;
	int addressFamily;
	lwm2m_object_t* objArray[LWM2M_OBJ_COUNT];
	coap_protocol_t proto;
	char local_port[16];
	int connection_retries;
	char*root_ca;
	bool use_se;
	int connect_timeout;
} client_data_t;

static coap_uri_protocol protocols[] = {
	{ COAP_UDP_DTLS, "coaps://", "UDP/DTLS" },
	{ COAP_UDP, "coap://", "UDP" },
	{ COAP_TCP, "coap+tcp://", "TCP" },
	{ COAP_TCP_TLS, "coaps+tcp://", "TCP/TLS" }
};

extern lwm2m_object_t* get_server_object( int serverId, const char*binding, int lifetime,
										  bool storing );

extern lwm2m_object_t* get_security_object( int serverId, const char*serverUri,
											uint8_t securityMode,
											char*serverCertificate, char*clientCertificateOrPskId,
											char*psk, uint16_t pskLen, bool isBootstrap );

//extern lwm2m_object_t *get_object_device(object_device_t *default_value);
//extern lwm2m_object_t *get_object_firmware(object_firmware_t *default_value);
//extern lwm2m_object_t *get_object_location(object_location_t *default_value);
//extern lwm2m_object_t *get_object_conn_m(object_conn_monitoring_t *default_value);
extern lwm2m_object_t* get_object_device();

extern lwm2m_object_t* get_object_firmware();

extern lwm2m_object_t* get_object_location();

extern lwm2m_object_t* get_object_conn_m();

extern lwm2m_object_t* get_object_conn_s( void );

extern void clean_security_object( lwm2m_object_t*object );

extern void clean_server_object( lwm2m_object_t*object );

extern void free_object_device( lwm2m_object_t*object );

extern void free_object_firmware( lwm2m_object_t*object );

extern void free_object_location( lwm2m_object_t*object );

extern void free_object_conn_s( lwm2m_object_t*object );

extern void free_object_conn_m( lwm2m_object_t*object );

extern void acl_ctrl_free_object( lwm2m_object_t*object );

extern char* get_server_uri( lwm2m_object_t*object, uint16_t secObjInstID );

extern uint16_t get_server_id( lwm2m_object_t* objectP, uint16_t secObjInstID );

extern lwm2m_object_t* acc_ctrl_create_object( void );

extern bool acc_ctrl_obj_add_inst( lwm2m_object_t*accCtrlObjP, uint16_t instId, uint16_t acObjectId,
								   uint16_t acObjInstId,
								   uint16_t acOwner );

extern bool acc_ctrl_oi_add_ac_val( lwm2m_object_t*accCtrlObjP, uint16_t instId, uint16_t aclResId,
									uint16_t acValue );

extern void conn_s_updateRxStatistic( lwm2m_object_t* objectP, uint16_t rxDataByte, bool smsBased );

extern void prv_firmware_register_callback( lwm2m_object_t* objectP,
											enum lwm2m_execute_callback_type type,
											lwm2m_exe_callback callback, void*param );

extern void prv_device_register_callback( lwm2m_object_t* objectP,
										  enum lwm2m_execute_callback_type type,
										  lwm2m_exe_callback callback, void*param );

extern uint8_t device_change_object( lwm2m_data_t* dataArray, lwm2m_object_t*object );

extern uint8_t firmware_change_object( lwm2m_data_t*dataArray, lwm2m_object_t*object );

extern uint8_t location_change_object( lwm2m_data_t*dataArray, lwm2m_object_t*object );

extern uint8_t connectivity_moni_change( lwm2m_data_t*dataArray, lwm2m_object_t*object );

void* lwm2m_connect_server( uint16_t secObjInstID, void* userData )
{
	client_data_t* dataP = NULL;
	char* uri = NULL;
	char* host = NULL;
	char* port = NULL;
	lwm2m_list_t* instance = NULL;
	int i = 0;
	coap_protocol_t protocol = COAP_PROTOCOL_MAX;
	lwm2m_object_t*securityObj = NULL;
	connection_t*conn = NULL;

	dataP = ( client_data_t* )userData;
	securityObj = dataP->securityObjP;

	uri = get_server_uri( dataP->securityObjP, secObjInstID );

	if ( uri == NULL )
	{
		return ( NULL );
	}

	//parse uri in the form "coaps://[host]:[port]"
	for ( i = 0; i < sizeof( protocols ) / sizeof( coap_uri_protocol ); i++ )
	{
		if ( 0 == strncmp( uri, protocols[i].uri_prefix, strlen( protocols[i].uri_prefix ) ) )
		{
			host = uri + strlen( protocols[i].uri_prefix );
			protocol = protocols[i].proto;
			break;
		}
	}

	port = strrchr( host, ':' );
	if ( port == NULL )
	{
		goto exit;
	}
	//remove brackets
	if ( host[0] == '[' )
	{
		host++;
		if ( *( port - 1 ) == ']' )
		{
			*( port - 1 ) = 0;
		}
		else
		{
			goto exit;
		}
	}
	//split strings
	*port = 0;
	port++;

#ifdef WITH_LOGS
//	LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "\r\nOpening connection to server at %s:%s with timeout %d ms\r\n",
//				host, port, timeout );
#endif
	dataP->proto = protocol;
	//If secure connection, make sure we have a security object
	instance = LWM2M_LIST_FIND( dataP->securityObjP->instanceList, secObjInstID );
	if ( instance == NULL )
	{
		goto exit;
	}

	conn = connection_create( protocol, dataP->root_ca, dataP->verify_cert,
							  dataP->use_se, dataP->sock, host, dataP->local_port, port,
							  dataP->addressFamily, securityObj, instance->id,
							  dataP->connect_timeout );
	if ( !conn )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Connection creation failed.\r\n" );
#endif
		goto exit;
	}

	memcpy( &dataP->server_addr, &conn->addr, conn->addrLen );
	dataP->server_addrlen = conn->addrLen;
	dataP->conn = conn;

	dataP->connList = ( connection_t* )conn;

exit:
	lwm2m_free( uri );
	return ( ( void* )conn );
}

void lwm2m_close_connection( void* sessionH,
							 void* userData )
{
	client_data_t*app_data = ( client_data_t* )userData;
	connection_t*targetP = ( connection_t* )sessionH;

	if ( targetP == app_data->connList )
	{
		app_data->connList = targetP->next;
		if ( targetP == app_data->conn )
		{
			app_data->conn = targetP->next;
		}

		lwm2m_free( targetP );

	}
	else
	{
		connection_t*parentP = app_data->connList;
		while ( ( parentP != NULL ) && ( parentP->next != targetP ) )
		{
			parentP = parentP->next;
		}
		if ( parentP != NULL )
		{
			parentP->next = targetP->next;
			lwm2m_free( targetP );
		}
	}
}

static void poll_lwm2m_sockets( client_data_t**clients, int number_clients, int timeout )
{
	int ret = 0;
	int numBytes = 0;
	uint8_t buffer[MAX_PACKET_SIZE];
	struct sockaddr addr;
	socklen_t addrLen = sizeof( addr );
	int i = 0; /*, j = 0;*/

	if ( number_clients == 0 )
	{
		return;
	}

	for ( i = 0; i < number_clients; i++ )
	{
		client_data_t*data = clients[i];
		/*TODO: Future Ready for Multiple servers*/
		/*for (j = 0; j < number_clients; j++)
		   {
		    if (clients[j]->sock == pfds[i].fd)
		    {
		        data = clients[j];
		        break;
		    }
		   }*/

		if ( data == NULL )
		{
			continue;
		}

		numBytes = connection_read( data->conn, buffer, MAX_PACKET_SIZE );

		memcpy( &addr, &data->server_addr, data->server_addrlen );
		addrLen = data->server_addrlen;

		if ( numBytes > 0 )
		{
			char s[INET6_ADDRSTRLEN];
			in_port_t port = 0;

			if ( AF_INET == addr.sa_family )
			{
				struct sockaddr_in*saddr = ( struct sockaddr_in* )&addr;
				inet_ntop( saddr->sin_family, &saddr->sin_addr, s, INET6_ADDRSTRLEN );
				port = saddr->sin_port;
			}
			else if ( AF_INET6 == addr.sa_family )
			{
				/*struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
				   inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
				   port = saddr->sin6_port;*/
			}
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs( port ) );
			/*output_buffer(stdout, buffer, numBytes, 0);*/
#endif

			connection_t*conn = connection_find( ( connection_t* )data->connList, (sockaddr_storage *)&addr, addrLen );
			if ( conn )
			{
				lwm2m_handle_packet( data->lwm2mH, buffer, numBytes, conn );
				conn_s_updateRxStatistic( data->objArray[LWM2M_OBJ_CONN_STAT], numBytes, false );
			}
		}
		else
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "server has closed the connection\r\n" );
#endif
			if ( numBytes == -1 )
			{
				/*	Close connection only in case of error.
				    Keep connection open if the SSL_Read is returned after timeout.*/
				close( data->sock );
				data->sock = -1;
			}
		}
	}
}

client_handle_t* lwm2m_client_start( object_container_t*init_val, char*root_ca, bool use_se )
{
	int result;
	int i;
	coap_protocol_t protocol = COAP_PROTOCOL_MAX;
	client_data_t* data = NULL;
	lwm2m_context_t*ctx = NULL;
	uint16_t pskLen = -1;
	char* pskBuffer = NULL;
	char* token = init_val->server->token;
	char* uri = init_val->server->serverUri;
	int serverId = init_val->server->serverId;
	client_handle_t*handle = NULL;
	int instId = 0;

	data = ( client_data_t* )lwm2m_malloc( sizeof( client_data_t ) );
	handle = ( client_handle_t* )lwm2m_malloc( sizeof( client_handle_t ) );
	if ( !data || !handle )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to allocate memory for client data\r\n" );
#endif
		goto error;
	}

	memset( data, 0, sizeof( client_data_t ) );
	memset( handle, 0, sizeof( client_handle_t ) );
	handle->client = data;

	/* Figure out protocol from the URI prefix */
	for ( i = 0; i < sizeof( protocols ) / sizeof( coap_uri_protocol ); i++ )
	{
		if ( 0 == strncmp( uri, protocols[i].uri_prefix, strlen( protocols[i].uri_prefix ) ) )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Connecting to server over %s\r\n", protocols[i].friendly_name );
#endif
			protocol = protocols[i].proto;
			break;
		}
	}

	if ( protocol == ( coap_protocol_t )-1 )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Unknown protocol, should be one of: " );
		for ( i = 0; i < sizeof( protocols ) / sizeof( coap_uri_protocol ); i++ )
		{
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "%s, ", protocols[i].uri_prefix );
		}
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "\r\n" );
#endif
		goto error;
	}

	data->proto = protocol;

	if ( data->proto == COAP_TCP_TLS )
	{
		data->verify_cert = init_val->server->verifyCert;
	}

	/* Default to IPV4, should add parameter to allow IPV6 */
	data->addressFamily = AF_INET;

	/*
	 * The liblwm2m library is now initialized with the functions that will be in
	 * charge of communication
	 */
	ctx = lwm2m_init( data );
	if ( !ctx )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_init() failed\r\n" );
#endif
		goto error;
	}

	data->lwm2mH = ctx;
	data->root_ca = root_ca;
	data->use_se = use_se;
	data->connect_timeout = init_val->server->connect_timeout;

	/*
	 * If valid local port is not provided, then randomize one based on a predefined range.
	 * Depending on the range it should be enough to avoid reusing
	 * twice the same port across the TIME_WAIT period after
	 * closing the socket.
	 */
	if ( init_val->server->localPort > 0 )
	{
		snprintf( data->local_port, 16, "%d", init_val->server->localPort );
	}
	else
	{
		snprintf( data->local_port, 16, "%d",
				  ( rand() % ( CLIENT_PORT_RANGE_END - CLIENT_PORT_RANGE_START ) ) +
				  CLIENT_PORT_RANGE_START );
	}

#ifdef WITH_LOGS
	LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Trying to bind LWM2M Client to port %s\r\n", data->local_port );
#endif

	data->sock = create_socket( data->proto, data->local_port, data->addressFamily );
	if ( data->sock < 0 )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to open socket: %d %s\r\n", errno, strerror( errno ) );
#endif
		goto error;
	}

	/*
	 * Now the main function fill an array with each object, this list will be later passed to
	 *liblwm2m.
	 * Those functions are located in their respective object file.
	 */
	if ( ( token != NULL ) &&
		 ( init_val->server->securityMode == LWM2M_SECURITY_MODE_PRE_SHARED_KEY ) )
	{
		pskLen = strlen( token ) / 2;
		pskBuffer = ( char* )lwm2m_malloc( pskLen );

		if ( NULL == pskBuffer )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create PSK binary buffer\r\n" );
#endif
			goto error;
		}
		//Hex string to binary
		char*h = token;
		char*b = pskBuffer;
		char xlate[] = "0123456789ABCDEF";
#if 1
		for (; *h; h += 2, ++b )
		{
			char*l = strchr( xlate, toupper( *h ) );
			char*r = strchr( xlate, toupper( *( h + 1 ) ) );

			if ( !r || !l )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to parse Pre-Shared-Key HEXSTRING\r\n" );
#endif
				lwm2m_free( pskBuffer );
				goto error;
			}

			*b = ( ( l - xlate ) << 4 ) + ( r - xlate );
		}
#endif
	}
	else
	{
		pskBuffer = init_val->server->privateKey;
	}

#ifdef WITH_LOGS
	LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, " Server Uri =  %s\n", uri );
#endif

	if ( init_val->server )
	{
		if ( pskBuffer )
		{
			data->objArray[LWM2M_OBJ_SECURITY] =
				get_security_object( serverId, uri, init_val->server->securityMode,
									 init_val->server->serverCertificate,
									 init_val->server->clientCertificateOrPskId,
									 pskBuffer, pskLen, false );
			if ( NULL == data->objArray[LWM2M_OBJ_SECURITY] )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create security object\r\n" );
#endif
				if ( init_val->server->securityMode == LWM2M_SEC_MODE_PSK )
				{
					lwm2m_free( pskBuffer );
				}

				goto error;
			}

			if ( init_val->server->securityMode == LWM2M_SEC_MODE_PSK )
			{
				lwm2m_free( pskBuffer );
			}

			data->securityObjP = data->objArray[LWM2M_OBJ_SECURITY];
		}

		switch ( data->proto )
		{
			case COAP_UDP:
			case COAP_UDP_DTLS:
				data->objArray[LWM2M_OBJ_SERVER] = get_server_object( serverId, "U",
																	  init_val->server->lifetime,
																	  false );
				strncpy( init_val->device->binding_mode, "U", LWM2M_MAX_STR_LEN );
				break;

			case COAP_TCP:
			case COAP_TCP_TLS:
				data->objArray[LWM2M_OBJ_SERVER] = get_server_object( serverId, "T",
																	  init_val->server->lifetime,
																	  false );
				strncpy( init_val->device->binding_mode, "T", LWM2M_MAX_STR_LEN );
				break;

			default:
				break;
		}

		if ( NULL == data->objArray[LWM2M_OBJ_SERVER] )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create server object\r\n" );
#endif
			goto error;
		}
	}

	if ( init_val->device )
	{
		//data->objArray[LWM2M_OBJ_DEVICE] = get_object_device(init_val->device);
		data->objArray[LWM2M_OBJ_DEVICE] = get_object_device();
		if ( NULL == data->objArray[LWM2M_OBJ_DEVICE] )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create Device object\r\n" );
#endif
			goto error;
		}
	}

	if ( init_val->firmware )
	{
		//data->objArray[LWM2M_OBJ_FIRMWARE] = get_object_firmware(init_val->firmware);
		data->objArray[LWM2M_OBJ_FIRMWARE] = get_object_firmware();
		if ( NULL == data->objArray[LWM2M_OBJ_FIRMWARE] )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create Firmware object\r\n" );
#endif
			goto error;
		}
	}

	if ( init_val->location )
	{
		//data->objArray[LWM2M_OBJ_LOCATION] = get_object_location(init_val->location);
		data->objArray[LWM2M_OBJ_LOCATION] = get_object_location();
		if ( NULL == data->objArray[LWM2M_OBJ_LOCATION] )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create location object\r\n" );
#endif
			goto error;
		}
	}

	if ( init_val->monitoring )
	{
		//data->objArray[LWM2M_OBJ_CONN_MON] = get_object_conn_m(init_val->monitoring);
		data->objArray[LWM2M_OBJ_CONN_MON] = get_object_conn_m();
		if ( NULL == data->objArray[LWM2M_OBJ_CONN_MON] )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create connectivity monitoring object\r\n" );
#endif
			goto error;
		}
	}

	data->objArray[LWM2M_OBJ_CONN_STAT] = get_object_conn_s();
	if ( NULL == data->objArray[LWM2M_OBJ_CONN_STAT] )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create connectivity statistics object\r\n" );
#endif
		goto error;
	}


	data->objArray[LWM2M_OBJ_ACL] = acc_ctrl_create_object();
	if ( NULL == data->objArray[LWM2M_OBJ_ACL] )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create Access Control object\r\n" );
#endif
		goto error;
	}
	else if ( acc_ctrl_obj_add_inst( data->objArray[LWM2M_OBJ_ACL], instId, 3, 0,
									 serverId ) == false )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create Access Control object instance\r\n" );
#endif
		goto error;
	}
	else if ( acc_ctrl_oi_add_ac_val( data->objArray[LWM2M_OBJ_ACL], instId, 0,
									  0b000000000001111 ) == false )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create Access Control ACL default resource\r\n" );
#endif
		goto error;
	}
	else if ( acc_ctrl_oi_add_ac_val( data->objArray[LWM2M_OBJ_ACL], instId, 999,
									  0b000000000000001 ) == false )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create Access Control ACL resource for serverId: 999\r\n" );
#endif
		goto error;
	}

	/*
	 * We configure the liblwm2m library with the name of the client - which shall be unique for
	 *each client -
	 * the number of objects we will be passing through and the objects array
	 */
	result = lwm2m_configure( data->lwm2mH, init_val->server->client_name, NULL, NULL,
							  LWM2M_OBJ_COUNT, data->objArray );
	if ( result != 0 )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_configure() failed: 0x%X\r\n", result );
#endif
		goto error;
	}

	return ( handle );

error:
	if ( ctx )
	{
		lwm2m_close( ctx );
	}

	if ( data )
	{
		if ( data->objArray[LWM2M_OBJ_SECURITY] )
		{
			clean_security_object( data->objArray[LWM2M_OBJ_SECURITY] );
		}

		if ( data->objArray[LWM2M_OBJ_SECURITY] )
		{
			lwm2m_free( data->objArray[LWM2M_OBJ_SECURITY] );
		}

		if ( data->objArray[LWM2M_OBJ_SERVER] )
		{
			clean_server_object( data->objArray[LWM2M_OBJ_SERVER] );
			lwm2m_free( data->objArray[LWM2M_OBJ_SERVER] );
		}

		if ( data->objArray[LWM2M_OBJ_DEVICE] )
		{
			free_object_device( data->objArray[LWM2M_OBJ_DEVICE] );
		}

		if ( data->objArray[LWM2M_OBJ_FIRMWARE] )
		{
			free_object_firmware( data->objArray[LWM2M_OBJ_FIRMWARE] );
		}

		if ( data->objArray[LWM2M_OBJ_LOCATION] )
		{
			free_object_location( data->objArray[LWM2M_OBJ_LOCATION] );
		}

		if ( data->objArray[LWM2M_OBJ_CONN_MON] )
		{
			free_object_conn_m( data->objArray[LWM2M_OBJ_CONN_MON] );
		}

		if ( data->objArray[LWM2M_OBJ_CONN_STAT] )
		{
			free_object_conn_s( data->objArray[LWM2M_OBJ_CONN_STAT] );
		}

		if ( data->objArray[LWM2M_OBJ_ACL] )
		{
			acl_ctrl_free_object( data->objArray[LWM2M_OBJ_ACL] );
		}

		lwm2m_free( data );
	}

	if ( handle )
	{
		lwm2m_free( handle );
	}

	return ( NULL );
}

void lwm2m_client_stop( client_handle_t* handle )
{
	client_data_t*data = NULL;

	if ( !handle )
	{
		return;
	}

	data = ( client_data_t* )handle->client;
	if ( !data )
	{
		return;
	}

	if ( data->lwm2mH )
	{
		lwm2m_close( data->lwm2mH );
	}

	close( data->sock );

	if ( data->connList )
	{
		connection_free( data->connList );
	}

	if ( data->objArray[LWM2M_OBJ_SECURITY] )
	{
		clean_security_object( data->objArray[LWM2M_OBJ_SECURITY] );
	}

	if ( data->objArray[LWM2M_OBJ_SECURITY] )
	{
		lwm2m_free( data->objArray[LWM2M_OBJ_SECURITY] );
	}

	if ( data->objArray[LWM2M_OBJ_SERVER] )
	{
		clean_server_object( data->objArray[LWM2M_OBJ_SERVER] );
		lwm2m_free( data->objArray[LWM2M_OBJ_SERVER] );
	}

	if ( data->objArray[LWM2M_OBJ_DEVICE] )
	{
		free_object_device( data->objArray[LWM2M_OBJ_DEVICE] );
	}

	if ( data->objArray[LWM2M_OBJ_FIRMWARE] )
	{
		free_object_firmware( data->objArray[LWM2M_OBJ_FIRMWARE] );
	}

	if ( data->objArray[LWM2M_OBJ_LOCATION] )
	{
		free_object_location( data->objArray[LWM2M_OBJ_LOCATION] );
	}

	if ( data->objArray[LWM2M_OBJ_CONN_MON] )
	{
		free_object_conn_m( data->objArray[LWM2M_OBJ_CONN_MON] );
	}

	if ( data->objArray[LWM2M_OBJ_CONN_STAT] )
	{
		free_object_conn_s( data->objArray[LWM2M_OBJ_CONN_STAT] );
	}

	if ( data->objArray[LWM2M_OBJ_ACL] )
	{
		acl_ctrl_free_object( data->objArray[LWM2M_OBJ_ACL] );
	}

	lwm2m_free( data );
	lwm2m_free( handle );
}

int lwm2m_client_service( client_handle_t*handle, int timeout_s )
{
	time_t timeout = lwm2m_clients_service( &handle, MAX_NUMBER_CLIENTS, timeout_s );

	if ( handle->error != 0 )
	{
		return ( handle->error );
	}
	else
	{
		return ( timeout );
	}
}

int lwm2m_clients_service( client_handle_t**handles, int number_handles, int timeout_s )
{
	time_t min_timeout = 30;
	client_data_t*clients[MAX_NUMBER_CLIENTS];
	int number_clients = 0;
	int i = 0;
	uint8_t lwm2mh_state = STATE_REG_FAILED;
	client_data_t*data = NULL;

	if ( !handles || ( number_handles <= 0 ) || ( number_handles > MAX_NUMBER_CLIENTS ) )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_clients_service: wrong parameters\r\n" );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	for ( i = 0; i < number_handles; i++ )
	{
		int result;
		time_t timeout = timeout_s;
		data = ( client_data_t* )handles[i]->client;
		result = lwm2m_step( data->lwm2mH, &timeout );
		min_timeout = min_timeout > timeout ? timeout : min_timeout;
		handles[i]->error = 0;

		if ( !data->conn )
		{
			handles[i]->error = LWM2M_CLIENT_ERROR;
			continue;
		}
		lwm2mh_state = registration_getStatus( data->lwm2mH );
		if ( ( result != 0 ) || ( lwm2mh_state == STATE_REG_FAILED ) )
		{
			/* Try reconnecting */
			if ( result != 0 )
			{
				data->conn->connected = false;
			}
			else
			{
				if (data->connection_retries > MAX_CONNECTION_RETRIES)
				{
#ifdef WITH_LOGS
					LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG,"Failed to register after %d retries, exiting...\r\n", MAX_CONNECTION_RETRIES);
#endif
					data->lwm2mH->state = STATE_REGISTER_REQUIRED;
					data->conn->connected = false;
					data->connection_retries = 0;
				}
				else
				{
					data->connection_retries++;
					LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG,"Retries++ =%d\n",data->connection_retries);
				}				
			}
		}
		else if ( lwm2mh_state == STATE_REGISTERED )
		{
			data->connection_retries = 0;
		}

		if ( !data->conn->connected )
		{
			data->lwm2mH->state = STATE_REGISTER_REQUIRED;
			/*if (data->connection_retries > MAX_CONNECTION_RETRIES)
			   {
			 #ifdef WITH_LOGS
			    LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG,"Failed to reconnect %d times, exiting...\r\n", MAX_CONNECTION_RETRIES);
			 #endif
			    handles[i]->error = LWM2M_CLIENT_ERROR;
			    continue;
			   }*/
			connection_restart( data->conn );
			min_timeout = 0;
			if ( data->conn->connected == true )
			{
				/*	As the connection is restarted attempt the register query by executing
					lwm2m_step() again for same "data".*/
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG,"lwm2m_clients_service: Restarted for re-register\n");
				i--;
			}
			continue;
		}

		clients[i] = data;
		//number_clients++;
	}
	number_clients = 1;

	min_timeout = min_timeout < timeout_s || timeout_s == 0 ? min_timeout : timeout_s;

	if ( ( handles[0]->error == LWM2M_CLIENT_OK ) && ( data->conn->connected == true ) )
	{
		poll_lwm2m_sockets( clients, number_clients, min_timeout );
	}

	/* Check if some connections got closed, set error accordingly */
	for ( i = 0; i < number_handles; i++ )
	{
		client_data_t*data = ( client_data_t* )handles[i]->client;

		if ( data->sock < 0 ) //Ori:(data->sock <= 0)
		{
			handles[i]->error = LWM2M_CLIENT_ERROR;
		}
	}

	return ( min_timeout );
}

void lwm2m_register_callback( client_handle_t* handle, enum lwm2m_execute_callback_type type,
							  lwm2m_exe_callback callback, void*param )
{
	
	if ( !handle || !callback || ( type >= LWM2M_EXE_COUNT ) )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_register_callback: wrong parameters\r\n" );
#endif
		return;
	}

#if 0	
	switch ( type )
	{
		case LWM2M_EXE_FIRMWARE_UPDATE:
			prv_firmware_register_callback( data->objArray[LWM2M_OBJ_FIRMWARE], type,
											callback, param );
			break;

		case LWM2M_EXE_FACTORY_RESET:
		case LWM2M_EXE_DEVICE_REBOOT:
			prv_device_register_callback( data->objArray[LWM2M_OBJ_DEVICE], type,
										  callback, param );
			break;

		case LWM2M_NOTIFY_RESOURCE_CHANGED:
			prv_device_register_callback( data->objArray[LWM2M_OBJ_DEVICE], type,
										  callback, param );
			prv_firmware_register_callback( data->objArray[LWM2M_OBJ_FIRMWARE], type,
											callback, param );
			break;

		default:
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_register_callback: unsupported callback\r\n" );
#endif
			break;
	}
#endif
}

void lwm2m_unregister_callback( client_handle_t* handle, enum lwm2m_execute_callback_type type )
{
	client_data_t*data = NULL;

	if ( !handle || ( type >= LWM2M_EXE_COUNT ) )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_unregister_callback: wrong parameters\r\n" );
#endif
		return;
	}

	data = ( client_data_t* )handle->client;

	switch ( type )
	{
		case LWM2M_EXE_FIRMWARE_UPDATE:
			prv_firmware_register_callback( data->objArray[LWM2M_OBJ_FIRMWARE], type, NULL, NULL );
			break;

		case LWM2M_EXE_FACTORY_RESET:
		case LWM2M_EXE_DEVICE_REBOOT:
			prv_device_register_callback( data->objArray[LWM2M_OBJ_DEVICE], type, NULL, NULL );
			break;

		case LWM2M_NOTIFY_RESOURCE_CHANGED:
			prv_device_register_callback( data->objArray[LWM2M_OBJ_DEVICE], type, NULL, NULL );
			prv_firmware_register_callback( data->objArray[LWM2M_OBJ_FIRMWARE], type, NULL, NULL );
			break;

		default:
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_register_callback: unsupported callback\r\n" );
#endif
			break;
	}
}

int lwm2m_write_resource( client_handle_t* handle, lwm2m_resource_t*res )
{
	int ret;
	client_data_t*client = NULL;
	lwm2m_uri_t uri_t;

	if ( !res || !handle )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_write_resource: wrong parameters\r\n" );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	client = ( client_data_t* )handle->client;

	ret = lwm2m_stringToUri( res->uri, strlen( res->uri ), &uri_t );
	if ( ret == 0 )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_stringToUri() failed: 0x%X", ret );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	if ( res->buffer && res->length )
	{
		/* Change the value */
		lwm2m_object_t*object = ( lwm2m_object_t* )lwm2m_list_find(
				( lwm2m_list_t* )client->lwm2mH->objectList, uri_t.objectId );

		if ( object )
		{
			int result = COAP_405_METHOD_NOT_ALLOWED;
			lwm2m_data_t data;

			data.id = uri_t.resourceId;
			lwm2m_data_encode_nstring( ( const char* )res->buffer, res->length, &data );

			if ( object->writeFunc )
			{
				result = object->writeFunc( uri_t.instanceId, 1, &data, object );
			}

			/*
			 * If property is not writable, we can still try to change it
			 * locally for objects that support it
			 */
			if ( result == COAP_405_METHOD_NOT_ALLOWED )
			{
#if 0
				switch ( uri_t.objectId )
				{
					case LWM2M_DEVICE_OBJECT_ID:
						//result = device_change_object(&data, object);
						break;

					case LWM2M_CONN_MONITOR_OBJECT_ID:
						result = connectivity_moni_change( &data, object );
						break;

					case LWM2M_FIRMWARE_UPDATE_OBJECT_ID:
						//result = firmware_change_object(&data, object);
						break;

					case LWM2M_LOCATION_OBJECT_ID:
						//result = location_change_object(&data, object);
						break;

					default:
						break;
				}
#endif
			}

			if ( result != COAP_204_CHANGED )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_write_resource: failed (%d)\r\n", result );
#endif
				return ( LWM2M_CLIENT_ERROR );
			}
		}
		else
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_write_resource: object not found\r\n" );
#endif
			return ( LWM2M_CLIENT_ERROR );
		}
	}

	lwm2m_resource_value_changed( client->lwm2mH, &uri_t );

	return ( LWM2M_CLIENT_OK );
}

static int encode_data( lwm2m_data_t*data, uint8_t**buffer )
{
	int size = 0;

	switch ( data->type )
	{
		case LWM2M_TYPE_STRING:
			size = data[0].value.asBuffer.length;
			*buffer = ( uint8_t* )lwm2m_malloc( size );
			if ( !buffer )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "encode_data: failed to allocate memory\r\n" );
#endif
				return ( LWM2M_CLIENT_ERROR );
			}
			memcpy( *buffer, data[0].value.asBuffer.buffer, size );
			break;

		case LWM2M_TYPE_INTEGER:
			size = utils_int64ToPlainText( data[0].value.asInteger, buffer );
			break;

		case LWM2M_TYPE_FLOAT:
			size = utils_float64ToPlainText( data[0].value.asFloat, buffer );
			break;

		case LWM2M_TYPE_BOOLEAN:
			size = utils_boolToPlainText( data[0].value.asBoolean, buffer );
			break;

		case LWM2M_TYPE_OBJECT_LINK:
		case LWM2M_TYPE_OPAQUE:
		case LWM2M_TYPE_UNDEFINED:
		default:
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "encode_data: unsupported type (%d)\r\n", data[0].type );
#endif
			size = LWM2M_CLIENT_ERROR;
			break;
	}

	return ( size );
}

int lwm2m_read_resource( client_handle_t* handle, lwm2m_resource_t*res )
{
	int ret;
	client_data_t*client = NULL;
	lwm2m_uri_t uri_t;
	lwm2m_object_t*object;

	if ( !res || !handle )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_read_resource: wrong parameters\r\n" );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	client = ( client_data_t* )handle->client;

	ret = lwm2m_stringToUri( res->uri, strlen( res->uri ), &uri_t );
	if ( ret == 0 )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_stringToUri() failed: 0x%X", ret );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	object = ( lwm2m_object_t* )lwm2m_list_find(
			( lwm2m_list_t* )client->lwm2mH->objectList, uri_t.objectId );

	if ( object )
	{
		if ( object->readFunc )
		{
			int result = COAP_405_METHOD_NOT_ALLOWED;
			lwm2m_data_t*data;
			int dnum = 1;

			data = lwm2m_data_new( dnum );
			if ( !data )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_read_resource: failed to allocate memory\r\n" );
#endif
				return ( LWM2M_CLIENT_ERROR );
			}

			data[0].id = uri_t.resourceId;
			result = object->readFunc( uri_t.instanceId, &dnum, &data, object );
			if ( result != COAP_205_CONTENT )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_read_resource: failed (%d)\r\n", result );
#endif
				lwm2m_free( data );
				return ( LWM2M_CLIENT_ERROR );
			}
			else
			{
				if ( data[0].type == LWM2M_TYPE_MULTIPLE_RESOURCE )
				{
					int i = 0, idx = 0;
					int num = data[0].value.asChildren.count;
					lwm2m_data_t*array = data[0].value.asChildren.array;
					lwm2m_resource_t*resarray = NULL;

					resarray = ( lwm2m_resource_t* )lwm2m_malloc( num * sizeof( *resarray ) );
					if ( !resarray )
					{
#ifdef WITH_LOGS
						LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_read_resource: failed to allocate memory\r\n" );
#endif
						lwm2m_data_free( num, data );
						return ( LWM2M_CLIENT_ERROR );
					}

					/* Retrieve the buffers from all children and compute total size */
					res->length = 0;
					for ( i = 0; i < num; i++ )
					{
						resarray[i].length = encode_data( &array[i], &resarray[i].buffer );
						res->length += resarray[i].length + 3;
					}

					/* Generate a single buffer out of the parsed ones */
					res->buffer = ( uint8_t* )lwm2m_malloc( res->length );
					if ( !res->length )
					{
#ifdef WITH_LOGS
						LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_read_resource: failed to allocate memory\r\n" );
#endif
						lwm2m_data_free( num, data );
						lwm2m_free( resarray );
						return ( LWM2M_CLIENT_ERROR );
					}

					memset( res->buffer, 0, res->length );
					for ( i = 0; i < num; i++ )
					{
						char prefix[5];
						char suffix[] = ",";
						snprintf( prefix, 5, "%d=", i );
						memcpy( res->buffer + idx, prefix, strlen( prefix ) );
						idx += strlen( prefix );
						memcpy( res->buffer + idx, resarray[i].buffer, resarray[i].length );
						idx += resarray[i].length;
						if ( i < ( num - 1 ) )
						{
							memcpy( res->buffer + idx, ( uint8_t* )suffix, 1 );
							idx++;
						}
					}

					lwm2m_free( resarray );
				}
				else
				{
					res->length = encode_data( &data[0], &res->buffer );
					if ( res->length < 0 )
					{
#ifdef WITH_LOGS
						LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_read_resource: failed to encode data\r\n" );
#endif
						return ( LWM2M_CLIENT_ERROR );

						lwm2m_free( data );
					}
				}
				lwm2m_free( data );
			}
		}
		else
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_read_resource: object not readable\r\n" );
#endif
			return ( LWM2M_CLIENT_ERROR );
		}
	}
	else
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_read_resource: object not found\r\n" );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	return ( LWM2M_CLIENT_OK );
}

int lwm2m_serialize_tlv_string( int num, char**strs, lwm2m_resource_t* res )
{
	lwm2m_data_t*array = NULL;
	int i = 0;

	array = ( lwm2m_data_t* )lwm2m_malloc( num * sizeof( *array ) );
	if ( !array )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_serialize_tlv_string: failed to allocate memory\r\n" );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	for ( i = 0; i < num; i++ )
	{
		array[i].type = LWM2M_TYPE_STRING;
		array[i].value.asBuffer.length = strlen( strs[i] );
		array[i].value.asBuffer.buffer = ( uint8_t* )strs[i];
	}

	res->length = tlv_serialize( true, num, array, &res->buffer );
	if ( !res->length )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_serialize_tlv_string: failed to serialize TLV\r\n" );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	return ( LWM2M_CLIENT_OK );
}

int lwm2m_serialize_tlv_int( int num, int*ints, lwm2m_resource_t* res )
{
	lwm2m_data_t*array = NULL;
	int i = 0;

	array = ( lwm2m_data_t* )lwm2m_malloc( num * sizeof( *array ) );
	if ( !array )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_serialize_tlv_int: failed to allocate memory\r\n" );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	for ( i = 0; i < num; i++ )
	{
		array[i].type = LWM2M_TYPE_INTEGER;
		array[i].value.asInteger = ( int64_t )ints[i];
	}

	res->length = tlv_serialize( true, num, array, &res->buffer );
	if ( !res->length )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "lwm2m_tlv_string: failed to serialize TLV\r\n" );
#endif
		return ( LWM2M_CLIENT_ERROR );
	}

	return ( LWM2M_CLIENT_OK );
}

#if 0
time_t lwm2m_last_succesful_registration( client_handle_t* handle )
{
	client_data_t*client = NULL;
	uint16_t id;

	if ( !handle )
	{
		return ( 0 );
	}

	client = ( client_data_t* )handle->client;

	/* Get instance 0 of the instance list */
	id = get_server_id( client->objArray[LWM2M_OBJ_SECURITY], 0 );

	if ( id == LWM2M_MAX_ID )
	{
		return ( 0 );
	}

	return ( lwm2m_last_registration( client->lwm2mH, id ) );
}

#endif
