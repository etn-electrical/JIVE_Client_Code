/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2017 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "mtxssl_if.h"
#include "crypto/cryptoApi.h"
#include "matrixsslApi.h"
#include "lwip/arch.h"
#include "opensslApi.h"
#include "matrixssllib.h"


const uint16_t CERT_ALGORITHM = ALG_SHA256;
const int16_t PS_SUCCESS_T = PS_SUCCESS;
const int16_t PS_FAILURE_T = PS_FAILURE;
const int16_t PS_PLATFORM_FAIL_T = PS_PLATFORM_FAIL;

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

#ifdef USE_DTLS
#define USE_DTLS_FLAG SSL_FLAGS_DTLS
#else
#define USE_DTLS_FLAG 0
#endif

// static const uint16_t RSA_KEY_SIZE = 2048U;
// static const uint8_t BASIC_CONSTRAINT_CA = 0; //value can be 0 or 1
// static const uint16_t BASIC_CONSTRAINT_PATH_LEN = 0U;
// static const char_t* KEY_USAGE_1 = "keyEncipherment";  //and/or "keyCertSign"
// static const char_t* KEY_USAGE_2 = "digitalSignature";
// static const char_t* EXTENDED_KEY_USAGE_1 = "serverAuth";
// static const char_t* EXTENDED_KEY_USAGE_2 = "clientAuth";
// static char_t* m_common_name;

#ifdef USE_CLIENT_SIDE_SSL
static const uint16_t g_ciphers[] = {
#if defined USE_TLS_RSA_WITH_AES_256_CBC_SHA
	0x35,
#endif
#if defined USE_TLS_RSA_WITH_AES_128_CBC_SHA
	0x2f,
#endif
#if defined USE_TLS_RSA_WITH_AES_128_CBC_SHA256
	0x3C,
#endif
#if defined USE_TLS_RSA_WITH_AES_128_GCM_SHA256
	0x9C,
#endif
#if defined USE_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256
	0xc023,
#endif
#if defined USE_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
	0xc02B,
#endif
//Ahmed Tom Recommendation
#if defined USE_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
    0xC02F,
#endif


};

static const uint16_t g_ciphers_count = sizeof( g_ciphers ) / sizeof( uint16_t );

#endif	// USE_CLIENT_SIDE_SSL

#define ALLOW_ANON_CONNECTIONS          1
#define SSL_TIMEOUT         6000// 45000	/* In milliseconds */

/*************************Function Declaration**********************************************/

int32 certCb( ssl_t* ssl, psX509Cert_t* cert, int32 alert );

int32_t extensionCb( ssl_t* ssl, uint16_t extType, uint8_t extLen, void* e );

/*******************************************************************************************/

void* mtxssl_if_SSL_new( void* sslContext, int8_t fd )
{
	SSL* ctx = SSL_new( ( SSL_CTX* )sslContext );

	ctx->fd = fd;
#ifdef USE_REHANDSHAKING
	ctx->rehandshake = 0;
#endif
	return ( ( void* )ctx );
}

bool mtxssl_if_SSL_library_init()
{
	bool return_val = false;

	if ( SSL_library_init() == 1U )
	{
		return_val = true;
	}

	return ( return_val );
}

void* mtxssl_if_SSL_CTX_new( void* ptr )
{

	return ( SSL_CTX_new( ptr ) );
}

void mtxssl_if_SSL_CTX_free( void* sslContext )
{
	SSL_CTX* ctx = ( SSL_CTX* )sslContext;

	SSL_CTX_free( ctx );
}

int mtxssl_if_SSL_CTX_New_Keys( void* ptr )
{
	SSL_CTX* ctx = ( SSL_CTX* )ptr;
	int return_status = PS_SUCCESS_T;

	if ( matrixSslNewKeys( &ctx->keys, NULL ) < 0 )
	{
		psTraceInfo( "matrixSslNewKeys error in SSL_CTX_new\n" );
		psFree( ctx, MATRIX_NO_POOL );
		return_status = PS_FAILURE_T;
	}
	return ( return_status );
}

int32 mtxssl_if_SSL_CTX_load_key_material_ECC( void* ctx, const unsigned char* certBuf,
											   int32 certLen, const unsigned char* privBuf,
											   int32 privLen,
											   const unsigned char* CAbuf, int32 CAlen )
{
	int status = PS_FAILURE_T;

#ifdef USE_ECC
	status = mtxssl_if_SSL_CTX_New_Keys( ctx );

	if ( status == PS_SUCCESS_T )
	{
		status = SSL_CTX_Load_Key_Material_ECC( ( SSL_CTX* )ctx, certBuf, certLen, privBuf, privLen,
												CAbuf, CAlen );
	}
#endif
	return ( status );
}

int32 mtxssl_if_SSL_CTX_load_key_material_RSA( void* ctx, const unsigned char* certBuf,
											   int32 certLen, const unsigned char* privBuf,
											   int32 privLen,
											   const unsigned char* CAbuf, int32 CAlen )
{
	int status = PS_FAILURE_T;

#ifdef USE_RSA
	status = mtxssl_if_SSL_CTX_New_Keys( ctx );

	if ( status == PS_SUCCESS_T )
	{
		status = SSL_CTX_Load_Key_Material_RSA( ( SSL_CTX* )ctx, certBuf, certLen, privBuf, privLen,
												CAbuf, CAlen );
	}
#endif
	return ( status );
}

void mtxssl_if_update_timeout( void* ptr, int32_t timeout, int32_t idle_state_timeout )
{
	SSL* ctx = ( SSL* )ptr;

	ctx->timeout = timeout;
	ctx->idle_state_timeout = idle_state_timeout;
	psGetTime( &ctx->time, NULL );
}

uint8_t mtxssl_if_timeout_status( void* ptr )
{
	SSL* ctx = ( SSL* )ptr;
	psTime_t now;

	psGetTime( &now, NULL );
	return ( ctx->timeout && ( psDiffMsecs( ctx->time, now, NULL ) > ( int32 )ctx->timeout ) );
}

bool mtxssl_if_idle_state_timeout_status( void* ptr )
{
	SSL* ctx = ( SSL* )ptr;
	psTime_t now;
	bool timeout_status = false;

	if ( ctx->idle_state_timeout > 0U )
	{
		psGetTime( &now, NULL );
		int32 time_laps = psDiffMsecs( ctx->time, now, NULL );

		if ( ( uint32 )( time_laps ) > ( ctx->idle_state_timeout ) )
		{
			timeout_status = true;
		}
	}

	return ( timeout_status );
}

int mtxssl_if_accept( void* ptr )
{
	SSL* ctx = ( SSL* )ptr;

	return ( SSL_accept( ctx ) );
}

int mtxssl_if_write( void* ptr, char* data, int length )
{
	SSL* ctx = ( SSL* )ptr;

	return ( SSL_write( ctx, data, length ) );
}

int32_t mtxssl_if_matrixSslGetOutdata( void* ptr, unsigned char** buf )
{
	SSL* ctx = ( SSL* )ptr;

	return ( matrixSslGetOutdata( ctx->ssl, buf ) );
}

void mtxssl_if_clear_wouldblock( void* ptr )
{
	SSL* ctx = ( SSL* )ptr;

	ctx->wouldblock = 0;
}

uint8_t mtxssl_if_get_wouldblock( void* ptr )
{
	SSL* ctx = ( SSL* )ptr;

	return ( ctx->wouldblock );
}

int32_t mtxssl_if_do_handshake( void* ptr )
{
	SSL* ctx = ( SSL* )ptr;

	return ( SSL_do_handshake( ctx ) );
}

void mtxssl_if_SSL_free( void* ptr )
{
	SSL* ctx = ( SSL* )ptr;

	SSL_free( ctx );
}

void mtxssl_if_set_hostname( void* ptr, const char* hostname )
{
	SSL* ctx = ( SSL* )ptr;

	ctx->host = ( char* )hostname;
}

int mtxssl_if_SSL_connect( void* ptr )
{
	SSL* ctx = ( SSL* )ptr;

	return ( SSL_connect( ctx ) );
}

void mtxssl_if_setSocketNodelay( int8_t fd )
{
	setSocketNodelay( fd );
}

void mtxssl_if_setSocketNonblock( int8_t fd )
{
	setSocketNonblock( fd );
}

int mtxssl_if_SSL_processed_data( void* ptr, unsigned char** data, int* length )
{
	SSL* ctx = ( SSL* )ptr;

	return ( SSL_processed_data( ctx, data, length ) );
}

int mtxssl_if_SSL_get_error( void* ptr, int ret_val )
{
	SSL* ctx = ( SSL* )ptr;

	return ( SSL_get_error( ctx, ret_val ) );
}

int mtxssl_if_SSL_read( void* ptr, char* data, int length )
{
	SSL* ctx = ( SSL* )ptr;

	return ( SSL_read( ctx, data, length ) );
}

int32_t mtxssl_if_SSL_get_data( void* ptr, unsigned char** ptBuf, int* ptBufLen )
{
	SSL* ctx = ( SSL* )ptr;

	return ( SSL_get_data( ctx, ptBuf, ptBufLen ) );
}

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
		// Term_Print("Server agreed to use %s\n", proto);
	}
	return ( PS_SUCCESS );
}

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
	options->ticketResumption = 1;
	options->versionFlag = USE_TLS_VERSION;		// | USE_DTLS_FLAG;
	// The SSL_CTX doesn't have much besides the keys:
	SSL_CTX* ssl_ctx = ( SSL_CTX* ) ssl_ctx_ptr;

	options->userPtr = ( void* ) ssl_ctx->keys;
}

void rtk_set_ciphers_and_options( SSL* cp, uint16_t const** ciphers, int* cipherSpec_Len,
								  sslCertCb_t** Cert_cb, tlsExtension_t** extensions,
								  sslExtCb_t** extensionCback, sslSessionId_t** resume,
								  sslSessOpts_t* options )
{
	unsigned char* ext;
	int32 extLen = 0;

	rtk_get_ciphers_and_options( cp->ctx, ciphers, cipherSpec_Len, options );

	*extensionCback = ( sslExtCb_t* )extensionCb;
	*Cert_cb = ( sslCertCb_t* )certCb;

	// matrixSslNewSessionId(resume, NULL);
	if ( extensions != NULL )
	{
		matrixSslNewHelloExtension( extensions, NULL );
		matrixSslCreateSNIext( NULL, ( unsigned char* )cp->host, ( uint32 )strlen( cp->host ),
							   &ext, &extLen );
		matrixSslLoadHelloExtension( *extensions, ext, extLen, EXT_SNI );
		psFree( ext, NULL );
	}
}

#endif

extern int socketConnect( char* ip, short port, int* err );

int mtxssl_if_socketConnect( char* hostname, short port, int* err )
{
	return ( socketConnect( hostname, port, err ) );
}

void mtxssl_if_eeTrace( const char* str )
{
	eeTrace( str );
}

void mtxssl_if_psTraceInfo( const char* str )
{
	psTraceInfo( str );
}

void mtxssl_if_psTraceIntInfo( const char* str, int err )
{
	psTraceIntInfo( str, err );
}

void mtxssl_if_DLListInsertTail( void* connsTmp, void* list )
{
	DLListInsertTail( ( DLListEntry* )connsTmp, ( DLListEntry* )list );
}

void mtxssl_if_DmatrixSslClose()
{
	matrixSslClose();
}

void mtxssl_if_ssl_rehandshake_disable( void* ptr )
{
#ifdef USE_REHANDSHAKING
	SSL* ctx = ( SSL* )ptr;
	ctx->rehandshake = 0;
#endif
}

uint8_t mtxssl_if_rehandshake_status( void* ptr )
{
	uint8_t status = 0;

#ifdef USE_REHANDSHAKING
	SSL* ctx = ( SSL* )ptr;
	if ( ctx->rehandshake > 0 )
	{
		status = 1;
	}
	else
	{
		// misra
	}
#endif
	return ( status );
}

void mtxssl_if_psGetTime( uint32_t* now )
{
	psGetTime( now, NULL );
}
