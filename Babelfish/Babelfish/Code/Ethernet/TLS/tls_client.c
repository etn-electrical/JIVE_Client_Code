/**
 *	@file    client.c
 *	@version a90e925 (tag: 3-8-3-open)
 *
 *	Simple MatrixSSL blocking client example.
 */
/*
 *	Copyright (c) 2013-2016 INSIDE Secure Corporation
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software
 *	into proprietary programs.  If you are unable to comply with the GPL, a
 *	commercial license for this software may be purchased from INSIDE at
 *	http://www.insidesecure.com/
 *
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *	See the GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "app.h"
#include "OS_Tasker.h"
#include "sockets.h"
#include "netdb.h"
#include "Ethernet.h"
#include "ssl_connection.h"
#ifndef WIN32
// #include <unistd.h>
#else
#include "XGetopt.h"
#endif
#include "matrixssl/matrixsslApi.h"

#if defined USE_TLS_1_2_AND_ABOVE
 #define USE_TLS_VERSION SSL_FLAGS_TLS_1_2
#elif defined USE_TLS_1_1_AND_ABOVE
 #define USE_TLS_VERSION SSL_FLAGS_TLS_1_1
#elif defined USE_TLS_1_0_AND_ABOVE
 #define USE_TLS_VERSION SSL_FLAGS_TLS_1_0
/** @security undef DISABLE_SSLV3 here if required */
#else
 #error Must define USE_TLS_1_x_AND_ABOVE
#endif

extern int32_t lwip_errno;

#ifdef USE_CLIENT_SIDE_SSL
static const uint16_t g_ciphers[] = {53};
#endif

#define ALLOW_ANON_CONNECTIONS  1
#define SSL_TIMEOUT         6000// 45000	/* In milliseconds */

/********************************** Defines ***********************************/
// char src_ip_addr[16] = "000.000.000.000";

/****************************** Local Functions *******************************/
int32 certCb( ssl_t* ssl, psX509Cert_t* cert, int32 alert );

SOCKET lsocketConnect( const char* host, const char* port, int32* err );

int32_t extensionCb( ssl_t* ssl, uint16_t extType, uint8_t extLen, void* e );


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t extensionCb( ssl_t* ssl, uint16_t extType, uint8_t extLen, void* e )
{
	unsigned char* c;
	short len;
	char proto[128];

	c = ( unsigned char* )e;

	if ( extType == EXT_ALPN )
	{
		memset( proto, 0x0, 128 );
		/* two byte proto list len, one byte proto len, then proto */
		c += 2;	/* Skip proto list len */
		len = *c;
		c++;
		memcpy( proto, c, len );
		Term_Print( "Server agreed to use %s\n", proto );
	}
	return ( PS_SUCCESS );
}

/******************************************************************************/
/*
    Example callback to show possiblie outcomes of certificate validation.
    If this callback is not registered in matrixSslNewClientSession
    the connection will be accepted or closed based on the alert value.
 */
int32 certCb( ssl_t* ssl, psX509Cert_t* cert, int32 alert )
{
#ifndef USE_ONLY_PSK_CIPHER_SUITE
	// psX509Cert_t	*next;

	/* An immediate SSL_ALERT_UNKNOWN_CA alert means we could not find the
	    CA to authenticate the server's certificate */
	return ( SSL_ALLOW_ANON_CONNECTION );

#if 0
	if ( alert == SSL_ALERT_UNKNOWN_CA )
	{
		/* Example to allow anonymous connections based on a define */
		if ( ALLOW_ANON_CONNECTIONS )
		{
			_psTraceStr( "Allowing anonymous connection for: %s.\n",
						 cert->subject.commonName );
			return ( SSL_ALLOW_ANON_CONNECTION );
		}
		_psTrace( "ERROR: No matching CA found.  Terminating connection\n" );
	}

	/*	Check for "major" authentication problems within the server certificate
	    chain.

	    The "alert" is the translation of the very first authentication problem
	    found.  So if we are dealing with a certificate chain we should walk to
	    the parent-most cert to confirm there are no authentication problems
	    that indicate the chain itself did not validate or this client did
	    not find a CA to authenticate the server.

	    Some certificate callback implemenations might choose to ignore some
	    alerts that are considered minor to the use case, so this is an example
	    of how to make sure a minor alert (such as expired date) is not
	    overriding a more serious authentication problem
	 */
	for ( next = cert; next != NULL; next = next->next )
	{
		if ( next->authStatus == PS_CERT_AUTH_FAIL_SIG )
		{
			_psTrace( "Public key signature failure in server cert chain\n" );
			/* This should result in a BAD_CERTIFICATE alert */
			alert = SSL_ALERT_BAD_CERTIFICATE;
			break;
		}
		if ( next->authStatus == PS_CERT_AUTH_FAIL_DN )
		{
			/* A CA file was never located to support this chain */
			_psTrace( "No CA file was found to support server's certificate\n" );
			/* This should result in a SSL_ALERT_UNKNOWN_CA alert */
			alert = SSL_ALERT_UNKNOWN_CA;
			break;
		}
		if ( next->authStatus == PS_CERT_AUTH_FAIL_AUTHKEY )
		{
			/* Subject and Issuer Key Id extension  */
			_psTrace( "Subject and Issuer Key Id mismatch error\n" );
			/* This should be a BAD_CERTIFICATE alert */
			alert = SSL_ALERT_BAD_CERTIFICATE;
			break;
		}
	}

	/*
	    If the expectedName passed to matrixSslNewClientSession does not
	    match any of the server subject name or subjAltNames, we will have
	    the alert below.
	    For security, the expected name (typically a domain name) _must_
	    match one of the certificate subject names, or the connection
	    should not continue.
	    The default MatrixSSL certificates use localhost and 127.0.0.1 as
	    the subjects, so unless the server IP matches one of those, this
	    alert will happen.
	    To temporarily disable the subjet name validation, NULL can be passed
	    as expectedName to matrixNewClientSession.
	 */
	if ( alert == SSL_ALERT_CERTIFICATE_UNKNOWN )
	{
		_psTraceStr( "ERROR: %s not found in cert subject names\n",
					 ssl->expectedName );
	}

	if ( alert == SSL_ALERT_CERTIFICATE_EXPIRED )
	{
#ifdef POSIX
		_psTrace( "ERROR: A cert did not fall within the notBefore/notAfter window\n" );
#else
		_psTrace( "WARNING: Certificate date window validation not implemented\n" );
		alert = 0;
#endif
	}

	if ( alert == SSL_ALERT_ILLEGAL_PARAMETER )
	{
		_psTrace( "ERROR: Found correct CA but X.509 extension details are wrong\n" );
	}

	/* Key usage related problems on chain */
	for ( next = cert; next != NULL; next = next->next )
	{
		if ( next->authStatus == PS_CERT_AUTH_FAIL_EXTENSION )
		{
			if ( next->authFailFlags & PS_CERT_AUTH_FAIL_KEY_USAGE_FLAG )
			{
				_psTrace( "CA keyUsage extension doesn't allow cert signing\n" );
			}
			if ( next->authFailFlags & PS_CERT_AUTH_FAIL_EKU_FLAG )
			{
				_psTrace( "Cert extendedKeyUsage extension doesn't allow TLS\n" );
			}
		}
	}

	if ( alert == SSL_ALERT_BAD_CERTIFICATE )
	{
		/* Should never let a connection happen if this is set.  There was
		    either a problem in the presented chain or in the final CA test */
		_psTrace( "ERROR: Problem in certificate validation.  Exiting.\n" );
	}


	if ( alert == 0 )
	{
		_psTraceStr( "SUCCESS: Validated cert for: %s.\n",
					 cert->subject.commonName );
	}

	return ( alert );

#endif	// 0
#endif	/* !USE_ONLY_PSK_CIPHER_SUITE */
}

#ifdef USE_CLIENT_SIDE_SSL
/** Shorter version of rtk_set_ciphers_and_options, just gets the cipher set and fills out the options structure.
 * @param ssl_ctx_ptr Opaque pointer to our SSL_CTX structure; will cast it to SSL_CTX* type
 * @param[out] ciphers On return, points to the array of cipher(s) supported by our Matrix SSL client.
 * @param[out] cipherSpec_Len The number of ciphers in array ciphers[]
 * @param options The Matrix SSL options structure to be filled in.
 */
void rtk_get_ciphers_and_options( void* ssl_ctx_ptr, const uint16_t** ciphers, int* cipherSpec_Len,
								  sslSessOpts_t* options )
{
	*cipherSpec_Len = g_ciphers_count;
	*ciphers = &( g_ciphers[0] );

	memset( options, 0x0, sizeof( sslSessOpts_t ) );
	// options->ticketResumption = 1;
	options->versionFlag = USE_TLS_VERSION;		// | USE_DTLS_FLAG;

	// The SSL_CTX doesn't have much besides the keys:
	SSL_CTX* ssl_ctx = reinterpret_cast < SSL_CTX * > ( ssl_ctx_ptr );

	options->userPtr = ( void* ) ssl_ctx->keys;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void rtk_set_ciphers_and_options( SSL* cp, uint16_t const** ciphers, int* cipherSpec_Len, sslCertCb_t** Cert_cb,
								  tlsExtension_t** extensions, sslExtCb_t** extensionCback, sslSessionId_t** resume,
								  sslSessOpts_t* options )
{
	unsigned char* ext;
	int32 extLen = 0;

	rtk_get_ciphers_and_options( cp->ctx, ciphers, cipherSpec_Len, options );

	*extensionCback = ( sslExtCb_t* )extensionCb;
	*Cert_cb = ( sslCertCb_t* )certCb;

	// matrixSslNewSessionId(resume, NULL);
	if ( extensions != nullptr )
	{
		matrixSslNewHelloExtension( extensions, NULL );
		matrixSslCreateSNIext( NULL, ( unsigned char* )cp->host, ( uint32 )strlen( cp->host ),
							   &ext, &extLen );
		matrixSslLoadHelloExtension( *extensions, ext, extLen, EXT_SNI );
		psFree( ext, NULL );
	}
}

#endif
