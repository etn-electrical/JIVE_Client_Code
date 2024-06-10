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
*    Gregory Lemercier, Samsung Semiconductor - support for TCP/TLS
*    David Navarro, Intel Corporation - initial API and implementation
*    Pascal Rieux - Please refer to git log
*
*******************************************************************************/
#include "includes.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// LTK_4064: Commented because of Warning[Pa181]: incompatible redefinition
// Commented as we are using error codes mentioned in lwip_error.h
//#include <errno.h>
#include <sockets.h>
#include <string.h>
#include "connection.h"
#include "lwip/sys.h"
#include "inet.h"
#include <netdb.h>
#include "mtxssl_if.h"
#include "OS_Tasker.h"
#include "..\..\..\..\..\Code\Prod_Spec_LWM2M\LWM2M_Debug.h"

#define errno lwip_errno
/* Needed for Mac OS X */
#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif
// LTK_4064: commented this as AI_OASSIVE is already defined in lwip/netdb.h
// Also in this file usage of AI_PASSIVE is commented.
//#define AI_PASSIVE      1
#define INET6_ADDRSTRLEN    6
static lwm2m_dtls_info_t*dtlsinfo_list = NULL;
static Ssl_Context* ssl_context_handle = nullptr;
extern char* strndup( const char*s, size_t n );

static char* security_get_public_id( lwm2m_object_t* obj, int instanceId, int* length );

static char* security_get_secret_key( lwm2m_object_t* obj, int instanceId, int* length );

void Attach_Lwm2m_Dtls_SSL_Context( Ssl_Context* ssl_context );

static unsigned int psk_client_cb( SSL*ssl, const char*hint, char*identity,
								   unsigned int max_identity_len,
								   unsigned char*psk,
								   unsigned int max_psk_len )
{
	int keyLen = 0;
	lwm2m_dtls_info_t*dtls = dtlsinfo_list;
	char*id = NULL, *key = NULL;

	//Look up DTLS info based on SSL pointer
	while ( dtls != NULL )
	{
		if ( ssl == dtls->connection->ssl )
		{
			id = dtls->identity;
			key = dtls->key;
			keyLen = dtls->key_length;
			break;
		}

		dtls = dtls->next;
	}

	if ( !id || !key )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Could not find DTLS credentials\n" );
#endif
		return ( 0 );
	}

	if ( strlen( id ) > max_identity_len )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "PSK identity is too long\n" );
#endif
		return ( 0 );
	}

	strncpy( identity, id, max_identity_len );

	if ( keyLen > max_psk_len )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "PSK key is too long\n" );
#endif
		return ( 0 );
	}

	memcpy( psk, key, keyLen );

#ifdef WITH_LOGS
	{
		int i = 0;
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "id: %s\n", identity );
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Key:" );
		for ( i = 0; i < keyLen; i++ )
		{
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "%02x", psk[i] );
		}
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "\n" );
	}
#endif

	return ( keyLen );
}

static int security_get_security_mode( lwm2m_object_t* obj, int instanceId )
{
	int size = 1;
	lwm2m_data_t* dataP = lwm2m_data_new( size );

	dataP->id = LWM2M_SECURITY_SECURITY_ID;//Ori-2; // security mode

	obj->readFunc( instanceId, &size, &dataP, obj );
	if ( ( dataP != NULL ) &&
		 ( dataP->type == LWM2M_TYPE_INTEGER ) )
	{
		int val = dataP->value.asInteger;
		lwm2m_free( dataP );
		return ( val );
	}
	else
	{
		lwm2m_free( dataP );
		return ( -1 );
	}
}

static char* security_get_server_public( lwm2m_object_t* obj, int instanceId, int* length )
{
	int size = 1;
	lwm2m_data_t* dataP = lwm2m_data_new( size );

	dataP->id = 4; //server public key or id

	obj->readFunc( instanceId, &size, &dataP, obj );
	if ( ( dataP != NULL ) &&
		 ( dataP->type == LWM2M_TYPE_OPAQUE ) )
	{
		char*val = ( char* )dataP->value.asBuffer.buffer;
		*length = dataP->value.asBuffer.length;
		lwm2m_free( dataP );
		return ( val );
	}
	else
	{
		lwm2m_free( dataP );
		return ( NULL );
	}
}

static SSL_CTX* ssl_configure_certificate_mode( connection_t*conn )
{
	SSL_CTX* ctx = NULL;

	if ( ( conn->protocol == COAP_UDP_DTLS ) && ( ssl_context_handle != NULL ) )
	{
		ctx = reinterpret_cast<SSL_CTX*>( ssl_context_handle->Get_Context() );
	}

	return ( ctx );
}

static SSL_CTX* ssl_configure_pre_shared_key( connection_t*conn )
{
	SSL_CTX*ctx = NULL;

	if ( ( conn->protocol == COAP_UDP_DTLS ) && conn->sec_obj )
	{
		char*id = NULL, *psk = NULL;
		int len = 0;
		lwm2m_dtls_info_t*dtls = NULL;

		if ( !conn->sec_obj )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "No security object provided\n" );
#endif
			return ( NULL );
		}
		//Retrieve ID/PSK from security object for DTLS handshake
		dtls = ( lwm2m_dtls_info_t* )lwm2m_malloc( sizeof( lwm2m_dtls_info_t ) );

		if ( !dtls )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to allocate memory for DTLS security info\n" );
#endif
			lwm2m_free( dtls );
			return ( NULL );
		}

		memset( dtls, 0, sizeof( lwm2m_dtls_info_t ) );

		id = security_get_public_id( conn->sec_obj, conn->sec_inst, &len );
		if ( len > MAX_DTLS_INFO_LEN )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Public ID is too long\n" );
#endif
			lwm2m_free( dtls );
			return ( NULL );
		}

		memcpy( dtls->identity, id, len );

		psk = security_get_secret_key( conn->sec_obj, conn->sec_inst, &len );
		if ( len > MAX_DTLS_INFO_LEN )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Secret key is too long\n" );
#endif
			lwm2m_free( dtls );
			return ( NULL );
		}

		memcpy( dtls->key, psk, len );
		dtls->key_length = len;
		dtls->connection = conn;
		dtls->id = lwm2m_list_newId( ( lwm2m_list_t* )dtlsinfo_list );
		dtlsinfo_list =
			( lwm2m_dtls_info_t* )LWM2M_LIST_ADD( ( lwm2m_list_t* )dtlsinfo_list,
												  ( lwm2m_list_t* )dtls );

		ctx = SSL_CTX_new( nullptr );//TODO: Pass DTLS_client_method
	}
	return ( ctx );
}

static bool ssl_init( connection_t* conn )
{
	static SSL_CTX*m_ctx = NULL;
	SSL*ssl = NULL;
	int ret = 0;
   uint8_t securityMode = 0U;

	OpenSSL_add_all_algorithms();
	ERR_clear_error();
	SSL_load_error_strings();

	if ( SSL_library_init() < 0 )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to initialize OpenSSL\n" );
#endif
		goto error;
	}

	securityMode = security_get_security_mode( conn->sec_obj, conn->sec_inst );
	if ( securityMode == 0 )
	{ /* Pre shared key mode */
		if ( m_ctx == NULL )
		{
			m_ctx = ssl_configure_pre_shared_key( conn );
		}
	}
	else if ( securityMode == 2 )
	{ /* Certificate mode */
		if ( m_ctx == NULL )
		{
			m_ctx = ssl_configure_certificate_mode( conn );
		}
	}
	else
	{
		return ( NULL );
	}

	if ( m_ctx == NULL )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create SSL context\n" );
#endif
		goto error;
	}

	ssl = SSL_new( m_ctx );
	if ( !ssl )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to allocate SSL connection\n" );
#endif
		goto error;
	}

	if ( conn->protocol == COAP_UDP_DTLS )
	{
	  /*X509* peer_cert = NULL;
		X509* server_cert = NULL;*/
		struct sockaddr peer;
		int peerlen = sizeof ( struct sockaddr );
		struct timeval timeout;
		int ret;
		int handshake_retries = 2;
	  /*char*cert = NULL;
		int len;*/
		int addrlen;
		if ( getsockname( conn->sock, &peer, ( socklen_t* )&peerlen ) < 0 )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "getsockname failed (%s)\n", strerror( errno ) );
#endif
		}



		if ( securityMode == 2 )
		{ /* Certificate mode
			 cert = security_get_server_public(conn->sec_obj, conn->sec_inst, &len);
			 if (len < 0) {
			 #ifdef WITH_LOGS
			    LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG,"Failed to get server certificate\n");
			 #endif
			    goto error;
			 }

			 server_cert = d2i_X509(NULL, (const unsigned char **)&cert, len);
			 if (!server_cert) {
			 #ifdef WITH_LOGS
			    LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG,"Failed to parse server certificate\n");
			 #endif
			    goto error;
			 }*/
		}
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		conn->ssl = ssl;
		conn->ssl->ctx->addr = ( struct sockaddr* )( &conn->addr );
		conn->ssl->fd = conn->sock;
		conn->ssl->host = conn->host;
		ret = SSL_connect_Dtls( conn->ssl );
		if ( ret < 1 )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "%s: SSL handshake failed\n", __func__ );
#endif
			goto error;
		}

		do
		{
			mtxssl_if_clear_wouldblock( conn->ssl );
			ret = SSL_do_handshake_Dtls( conn->ssl );
			if ( ret <= 0 )
			{
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "SSL handshake time error %d\n", ret );
				/*switch (SSL_get_error(conn->ssl, ret))
				{
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
						break;
					default:
#ifdef WITH_LOGS
						LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG,"%s: SSL error: %s\n", __func__,
						ERR_error_string(SSL_get_error(conn->ssl, ret), NULL));
#endif
						goto error;
				}*/
				OS_Tasker::Delay( 100U );
				if ( handshake_retries-- <= 0 )
				{
#ifdef WITH_LOGS
					LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "%s: SSL handshake timed out\n", __func__ );
#endif
					goto error;
				}
			}
		} while ( ret != 1 );
	}
	return ( true );

error:
	//if (conn->ssl->ctx)
	//SSL_CTX_free(conn->ssl->ctx);
	if ( conn->ssl )
	{
		SSL_free( conn->ssl );
		conn->ssl = NULL;
	}

	return ( false );
}

static char* security_get_public_id( lwm2m_object_t* obj, int instanceId, int* length )
{
	int size = 1;
	lwm2m_data_t* dataP = lwm2m_data_new( size );

	dataP->id = 3; //public key or id

	obj->readFunc( instanceId, &size, &dataP, obj );
	if ( ( dataP != NULL ) &&
		 ( dataP->type == LWM2M_TYPE_OPAQUE ) )
	{
		char*val = ( char* )dataP->value.asBuffer.buffer;
		*length = dataP->value.asBuffer.length;
		lwm2m_free( dataP );
		return ( val );
	}
	else
	{
		lwm2m_free( dataP );
		return ( NULL );
	}
}

static char* security_get_secret_key( lwm2m_object_t* obj, int instanceId, int* length )
{
	int size = 1;
	lwm2m_data_t* dataP = lwm2m_data_new( size );

	dataP->id = 5; //secret key

	obj->readFunc( instanceId, &size, &dataP, obj );
	if ( ( dataP != NULL ) &&
		 ( dataP->type == LWM2M_TYPE_OPAQUE ) )
	{
		char*val = ( char* )dataP->value.asBuffer.buffer;
		*length = dataP->value.asBuffer.length;
		lwm2m_free( dataP );
		return ( val );
	}
	else
	{
		lwm2m_free( dataP );
		return ( NULL );
	}
}

int connection_restart( connection_t*conn )
{
	int sock;
	connection_t*newConn = NULL;

	conn->connected = false;

	/* Close previous connection */
	close( conn->sock );

	//if (conn->ssl->ctx)
	//{
	//SSL_CTX_free(conn->ssl->ctx);
	//conn->ssl->ctx = NULL;
	//}

	if ( conn->ssl )
	{
		SSL_free( conn->ssl );
		conn->ssl = NULL;
	}

	sock = create_socket( conn->protocol, conn->local_port, conn->address_family );
	if ( sock < 0 )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create new socket\n" );
#endif
		return ( -1 );
	}

	newConn = connection_create( conn->protocol,
								 conn->root_ca,
								 conn->verify_cert,
								 conn->use_se,
								 sock,
								 conn->host,
								 conn->local_port,
								 conn->remote_port,
								 conn->address_family,
								 conn->sec_obj,
								 conn->sec_inst,
								 conn->timeout );

	if ( !newConn )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to create new connection\n" );
#endif
		close( sock );
		return ( -1 );
	}

	if ( conn->protocol == COAP_UDP_DTLS )
	{
		lwm2m_dtls_info_t*dtls = dtlsinfo_list;

		/* Delete old connection's DTLS info */
		while ( dtls != NULL )
		{
			if ( conn == dtls->connection )
			{
				lwm2m_dtls_info_t*node;
				dtlsinfo_list =
					( lwm2m_dtls_info_t* )LWM2M_LIST_RM( dtlsinfo_list, dtls->id, &node );
				lwm2m_free( node );
				break;
			}
			dtls = dtls->next;
		}

		/* Replace connection pointer in new DTLS info */
		dtls = dtlsinfo_list;
		while ( dtls != NULL )
		{
			if ( newConn == dtls->connection )
			{
				dtls->connection = newConn;
				break;
			}
			dtls = dtls->next;
		}
	}

	/*
	 * Copy new connection on top of the old one to keep same pointer,
	 * then dispose of the newly allocated memory
	 */
	lwm2m_free( conn->host );
	memcpy( conn, newConn, sizeof( connection_t ) );
	lwm2m_free( newConn );

	return ( 0 );
}

int create_socket( coap_protocol_t protocol, const char* portStr, int addressFamily )
{
	int s = -1;

	/*struct addrinfo hints;
	   struct addrinfo *res;
	   struct addrinfo *p;

	   memset(&hints, 0, sizeof hints);
	   hints.ai_family = addressFamily;
	   hints.ai_socktype = SOCK_DGRAM;
	   //hints.ai_flags = AI_PASSIVE;

	   if (0 != getaddrinfo(NULL, portStr, &hints, &res))
	   {
	    return -1;
	   }

	   for(p = res ; p != NULL && s == -1 ; p = p->ai_next)
	   {
	    s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	    if (s >= 0)
	    {
	        if (-1 == bind(s, p->ai_addr, p->ai_addrlen))
	        {
	            close(s);
	            s = -1;
	        }
	    }
	   }

	   freeaddrinfo(res);*/

	s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( s >= 0 )
	{
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_port = htons( atoi( portStr ) );
		addr.sin_addr.s_addr = INADDR_ANY;

		if ( -1 == bind( s, ( sockaddr* ) &addr, sizeof( addr ) ) )
		{
			close( s );
			s = -1;
		}
	}

	return ( s );
}

connection_t* connection_find( connection_t* connList,
							   struct sockaddr_storage* addr,
							   size_t addrLen )
{
	connection_t* connP;

	connP = connList;
	while ( connP != NULL )
	{
		if ( ( connP->addrLen == addrLen ) &&
			 ( memcmp( &( connP->addr ), addr, addrLen ) == 0 ) )
		{
			return ( connP );
		}
		connP = connP->next;
	}

	return ( connP );
}

connection_t* connection_create( coap_protocol_t protocol,
								 char*root_ca,
								 bool verify_cert,
								 bool use_se,
								 int sock,
								 char*host,
								 char*local_port,
								 char*remote_port,
								 int addressFamily,
								 lwm2m_object_t* sec_obj,
								 int sec_inst,
								 int timeout )
{
	struct addrinfo hints;
	struct addrinfo*servinfo = NULL;
	struct addrinfo*p;
	int s, ret;
	struct sockaddr*sa;
	socklen_t sl;
	connection_t* connP = NULL;
	long arg = 0;
	fd_set rset, wset;
	struct timeval ts;

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = addressFamily;

	switch ( protocol )
	{
		case COAP_TCP:
		case COAP_TCP_TLS:
			hints.ai_socktype = SOCK_STREAM;
			break;

		case COAP_UDP:
		case COAP_UDP_DTLS:
			hints.ai_socktype = SOCK_DGRAM;
			break;

		default:
			break;
	}

	if ( ( 0 != getaddrinfo( host, remote_port, &hints, &servinfo ) ) || ( servinfo == NULL ) )
	{
		return ( NULL );
	}

	//we test the various addresses
	s = -1;
	for ( p = servinfo; p != NULL && s == -1; p = p->ai_next )
	{
		s = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
		if ( s >= 0 )
		{
			sa = p->ai_addr;
			sl = p->ai_addrlen;

#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Try to connect to server with timeout %d ms\n", timeout );
#endif
			ret = connect( s, p->ai_addr, p->ai_addrlen );
			if ( ret < 0 )
			{
				if ( errno != EINPROGRESS )
				{
#ifdef WITH_LOGS
					LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Connect to socket failed (err=%d)\n", errno );
#endif
					goto fail;
				}

				FD_ZERO( &rset );
				FD_ZERO( &wset );
				FD_SET( s, &rset );
				FD_SET( s, &wset );
				ts.tv_sec = timeout / 1000;
				ts.tv_usec = ( timeout - ( ts.tv_sec * 1000 ) ) * 1000;
				ret = select( s + 1, &rset, &wset, NULL, ( timeout ) ? &ts : NULL );
				if ( ret <= 0 )
				{
#ifdef WITH_LOGS
					LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Waiting for socket failed (err=%d)\n", ret );
#endif
					goto fail;
				}

				if ( !FD_ISSET( s, &rset ) && !FD_ISSET( s, &wset ) )
				{
#ifdef WITH_LOGS
					LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "No fd was set\n" );
#endif
					goto fail;
				}
			}

			continue;
fail:
			close( s );
			s = -1;
		}
	}

	if ( s >= 0 )
	{
		if ( protocol != COAP_UDP )
		{
			if ( connect( sock, sa, sl ) < 0 )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to connect to socket: %s\n", strerror( errno ) );
#endif
				close( sock );
				return ( NULL );
			}
		}

		/* Allocate and fill up connection structure */
		connP = ( connection_t* )lwm2m_malloc( sizeof( connection_t ) );
		if ( connP == NULL )
		{
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to allocate memory for connection\n" );
#endif
			return ( NULL );
		}

		memset( connP, 0, sizeof( connection_t ) );
		connP->sock = sock;
		connP->protocol = protocol;
		connP->verify_cert = verify_cert;

		if ( root_ca )
		{
			connP->root_ca = strdup( root_ca );
		}

		memcpy( &( connP->addr ), sa, sl );
		connP->host = strndup( host, strlen( host ) );
		connP->addrLen = sl;
		strncpy( connP->local_port, local_port, 16 );
		strncpy( connP->remote_port, remote_port, 16 );
		connP->address_family = addressFamily;
		connP->sec_obj = sec_obj;
		connP->sec_inst = sec_inst;
		connP->use_se = use_se;
		connP->timeout = timeout;

		if ( ( protocol == COAP_TCP_TLS ) ||
			 ( protocol == COAP_UDP_DTLS ) )
		{
			if ( !ssl_init( connP ) )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to initialize SSL session\n" );
#endif
				goto error;
			}
		}
		close( s );
	}
	else
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Failed to find responsive server\n" );
#endif
		goto error;
	}

	if ( NULL != servinfo )   //As per latest stack
	{
		freeaddrinfo( servinfo );
	}

	connP->connected = true;

	return ( connP );

error:
	if ( NULL != servinfo )
	{
		freeaddrinfo( servinfo );
	}
	close( s );
	if ( connP )
	{
		lwm2m_free( connP->host );
		lwm2m_free( connP );
		connP = NULL;
	}

	return ( NULL );
}

void connection_free( connection_t* connList )
{
	while ( connList != NULL )
	{
		connection_t* nextP;
		lwm2m_dtls_info_t*dtls = dtlsinfo_list;

		//Free DTLS info if any
		while ( dtls != NULL )
		{
			if ( connList == dtls->connection )
			{
				lwm2m_dtls_info_t*node;
				dtlsinfo_list =
					( lwm2m_dtls_info_t* )LWM2M_LIST_RM( dtlsinfo_list, dtls->id, &node );
				lwm2m_free( node );
				break;
			}

			dtls = dtls->next;
		}

		nextP = connList->next;
		if ( connList->host )
		{
			lwm2m_free( connList->host );
			connList->host = NULL;
		}
		if ( connList->root_ca )
		{
			lwm2m_free( connList->root_ca );
			connList->root_ca = NULL;
		}

		//if (connList->ssl->ctx)
		//{
		//SSL_CTX_free(connList->ssl->ctx);
		//}

		if ( connList->ssl )
		{
			SSL_free( connList->ssl );
			connList->ssl = NULL;
		}

		lwm2m_free( connList );

		connList = nextP;
	}
}

int connection_send( connection_t*connP,
					 uint8_t* buffer,
					 size_t length )
{
	int nbSent;
	size_t offset;

	if ( !connP->connected )
	{
		return ( -1 );
	}

#ifdef WITH_LOGS
	char s[INET6_ADDRSTRLEN];
	uint16_t port;

	s[0] = 0;

	if ( AF_INET == connP->addr.sin_family )
	{
		struct sockaddr_in*saddr = ( struct sockaddr_in* )&connP->addr;
		inet_ntop( saddr->sin_family, &saddr->sin_addr, s, 4 );
		port = saddr->sin_port;
	}
	/*else if (AF_INET6 == connP->addr.sin6_family)
	   {
	    struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&connP->addr;
	    inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
	    port = saddr->sin6_port;
	   }*/

	LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Connection Send: %lu bytes to [%s]:%hu\r\n", length, s, ntohs( port ) );
	//output_buffer(stderr, buffer, length, 0);
#endif

	offset = 0;
	while ( offset < length )
	{
		switch ( connP->protocol )
		{
			case COAP_UDP_DTLS:
			case COAP_TCP_TLS:
				mtxssl_if_clear_wouldblock( connP->ssl );
				nbSent = SSL_write_Dtls( connP->ssl, buffer + offset, length - offset );
				if ( nbSent < 1 )
				{
#ifdef WITH_LOGS
					LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "SSL Send error: %s\n",
								ERR_error_string( SSL_get_error( connP->ssl, nbSent ), NULL ) );
#endif
					if ( ( nbSent == -1 ) && ( connP->ssl->wouldblock == 1 ) )
					{
						nbSent = SSL_write_Dtls( connP->ssl, buffer, 0U );
					}
					return ( -1 );
				}
				break;

			case COAP_TCP:
				nbSent = send( connP->sock, buffer + offset, length - offset, 0 );
				if ( nbSent == -1 )
				{
#ifdef WITH_LOGS
					LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Send error: %s\n", strerror( errno ) );
#endif
					return ( -1 );
				}
				break;

			case COAP_UDP:
				nbSent =
					sendto( connP->sock, buffer + offset, length - offset, 0,
							( struct sockaddr* )&( connP->addr ), connP->addrLen );
				if ( nbSent == -1 )
				{
#ifdef WITH_LOGS
					LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Send error: %s\n", strerror( errno ) );
#endif
					return ( -1 );
				}
				break;

			default:
				break;
		}

		offset += nbSent;
	}
	return ( 0 );
}

uint8_t lwm2m_buffer_send( void* sessionH,
						   uint8_t* buffer,
						   size_t length,
						   void* userdata )
{
	connection_t* connP = ( connection_t* ) sessionH;

	if ( connP == NULL )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "#> failed sending %lu bytes, missing connection\r\n", length );
#endif
		return ( COAP_500_INTERNAL_SERVER_ERROR );
	}

	if ( -1 == connection_send( connP, buffer, length ) )
	{
#ifdef WITH_LOGS
		LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "#> failed sending %lu bytes, try reconnecting\r\n", length );
#endif
		connP->connected = false;
		return ( COAP_500_INTERNAL_SERVER_ERROR );
	}

	return ( COAP_NO_ERROR );
}

bool lwm2m_session_is_equal( void* session1,
							 void* session2,
							 void* userData )
{
	return ( session1 == session2 );
}

int connection_read( connection_t*connP, uint8_t* buffer, size_t size ) {
	int numBytes = -1;

	switch ( connP->protocol )
	{
		case COAP_UDP:
			numBytes = recvfrom( connP->sock, buffer, size, 0, NULL, NULL );
			if ( numBytes < 0 )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Error in recvfrom(): %d %s\r\n", errno, strerror( errno ) );
#endif
				return ( 0 );
			}
			break;

		case COAP_TCP:
			numBytes = recv( connP->sock, buffer, size, 0 );
			if ( numBytes < 0 )
			{
#ifdef WITH_LOGS
				LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Error in recv(): %d %s\r\n", errno, strerror( errno ) );
#endif
				return ( 0 );
			}
			break;

		case COAP_UDP_DTLS:
		case COAP_TCP_TLS:
			mtxssl_if_clear_wouldblock( connP->ssl );
			numBytes = SSL_read_Dtls( connP->ssl, buffer, size );
			if ( ( numBytes == -1 ) && ( connP->ssl->wouldblock == 1 ) )
			{
				mtxssl_if_clear_wouldblock( connP->ssl );
				return ( 0 );
			}
			break;

		default:
#ifdef WITH_LOGS
			LWM2M_DEBUG_PRNT( DBG_MSG_DEBUG, "Error protocol = %d is not supported.\r\n", connP->protocol );
#endif
			return ( 0 );
	}

	return ( numBytes );
}

void inet_ntop( u8_t af, in_addr* src, char* dst, u8_t size )
{
	dst = inet_ntoa( *src );
}

void Attach_Lwm2m_Dtls_SSL_Context( Ssl_Context* ssl_context )
{
	if ( ssl_context != nullptr )
	{
		ssl_context_handle = ssl_context;
	}
}

Ssl_Context* Get_Lwm2m_Dtls_SSL_Context( void )
{
	return( ssl_context_handle );
}
