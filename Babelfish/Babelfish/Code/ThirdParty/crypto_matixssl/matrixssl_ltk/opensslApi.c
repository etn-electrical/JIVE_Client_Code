/**
 *	@file    opensslApi.c
 *	@version a90e925 (tag: 3-8-3-open)
 *
 *	An OpenSSL interface to MatrixSSL.
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
#include <stdlib.h>
#include <stdio.h>
#include "sockets.h"

// keep this before including opensslApi.h
#define USE_MATRIX_OPENSSL_LAYER

#include "opensslApi.h"
#include "mtxssl_if.h"
#include "matrixssllib.h"

// LTK-4064
// Added function declaration as there was a warning for implicit declaration
// of rtk_set_ciphers_and_options function
extern void rtk_set_ciphers_and_options( SSL*cp, uint16_t const** ciphers, int* cipherSpec_Len,
										 sslCertCb_t** Cert_cb, tlsExtension_t** extensions,
								  sslExtCb_t** extensionCback, sslSessionId_t** resume,
								  sslSessOpts_t* options );

//ssgopenssl start

/******************************************************************************/
/* Always returns 1 */
int SSL_library_init( void )
{
	eeTrace( "Enter SSL_library_init\n" );
	if ( matrixSslOpen() < 0 )
	{
		psTraceInfo( "Error opening MatrixSSL library via SSL_library_init\n" );
	}
	eeTrace( "Exit SSL_library_init\n" );
	return ( 1 );
}

/******************************************************************************/
/* SSL is an sslConn_t of which SSL_CTX (ssl_t) is a member */
SSL* SSL_new( SSL_CTX*ctx )
{
	SSL*cp;

	eeTrace( "Enter SSL_new\n" );

	if ( ( cp = ( SSL* )psMalloc( MATRIX_NO_POOL, sizeof( SSL ) ) ) == NULL )
	{
		psTraceInfo( "Memory allocation error in SSL_new\n" );
		return ( NULL );
	}
	memset( cp, 0x0, sizeof( SSL ) );

	cp->ctx = ctx;
	eeTrace( "Exit SSL_new\n" );
	return ( cp );
}

/******************************************************************************/
/* Counterpart to SSL_new */
void SSL_free( SSL*cp )
{
	//SSL_CTX	*ctx;
	eeTrace( "Enter SSL_free\n" );
	//ctx = cp->ctx;
	if ( cp->ssl )
	{
		matrixSslDeleteSession( cp->ssl );
		cp->ssl = NULL;
	}
	memset( cp, 0x0, sizeof( SSL ) );
	psFree( cp, MATRIX_NO_POOL );

	eeTrace( "Exit SSL_free\n" );
}

/******************************************************************************/
/* A SSL_CTX is NOT quite the same as an ssl_t */
SSL_CTX* SSL_CTX_new( const SSL_METHOD*meth )
{
	SSL_CTX*ctx = NULL;

	eeTrace( "Enter SSL_CTX_new\n" );
	if ( ( ctx = ( SSL_CTX* )psMalloc( MATRIX_NO_POOL, sizeof( SSL_CTX ) ) ) == NULL )
	{
		psTraceInfo( "Memory allocation error in SSL_CTX_new\n" );
	}
	else
	{
		memset( ctx, 0x0, sizeof( SSL_CTX ) );
		eeTrace( "Exit SSL_CTX_new\n" );
	}
	return ( ctx );
}

/******************************************************************************/
int SSL_CTX_New_Keys( SSL_CTX*ctx )
{
	int status = PS_SUCCESS_T;

	if ( matrixSslNewKeys( &ctx->keys, NULL ) < 0 )
	{
		psTraceInfo( "matrixSslNewKeys error in SSL_CTX_new\n" );
		psFree( ctx, MATRIX_NO_POOL );
		status = PS_FAILURE_T;
	}
	return ( status );
}

/******************************************************************************/
void SSL_CTX_free( SSL_CTX*ctx )
{
	eeTrace( "Enter SSL_CTX_free\n" );
	matrixSslDeleteKeys( ctx->keys );
	//if (ctx->ssl) {
	//psTraceInfo("ERROR: ssl_t session not deleted prior to SSL_CTX_free\n");
	//}
	psFree( ctx, MATRIX_NO_POOL );
	eeTrace( "Exit SSL_CTX_free\n" );
}

/* SSL_CTX_set_options() and SSL_set_options() return the new options
    bitmask after adding options. */
long SSL_CTX_set_options( SSL_CTX*ctx, long options )
{
	uTrace( "TODO: SSL_CTX_set_options\n" );
	return ( options );
}

/* SSL_CTX_set_cipher_list() and SSL_set_cipher_list() return 1 if any
    cipher could be selected and 0 on complete failure. */
int SSL_CTX_set_cipher_list( SSL_CTX*ctx, const char*str )
{
	uTrace( "TODO: SSL_CTX_set_cipher_list\n" );
	return ( 1 );
}

/* SSL_CTX_load_verify_locations() specifies the locations for ctx, at which
    CA certificates for verification purposes are located. The certificates
    available via CAfile and CApath are trusted.
 */
int SSL_CTX_load_verify_locations( SSL_CTX*ctx, const char*CAfile,
								   const char*CApath )
{
	uTrace( "TODO: SSL_CTX_load_verify_locations\n" );
	/* 0 on failure.  1 on success */
	return ( 0 );
}

/******************************************************************************/
/*
    0 is failure.  1 is good
 */
int SSL_CTX_use_certificate_file( SSL_CTX*ctx, const char*file, int type )
{
	uTrace( "TODO: SSL_CTX_use_certificate_file\n" );
	return ( 1 );
}

#if 0
X509* SSL_get_certificate( SSL*ssl )
{
	SSL_CTX*ctx = ssl->ctx;

	eeTrace( "Enter SSL_get_certificate\n" );
	if ( ( ctx == NULL ) || ( ctx->keys == NULL ) )
	{
		return ( NULL );
	}
	eeTrace( "Exit SSL_get_certificate\n" );
	return ( ctx->keys->cert );
}


EVP_PKEY* X509_get_pubkey( X509*cert )
{
	eeTrace( "Enter X509_get_pubkey\n" );
	eeTrace( "Exit X509_get_pubkey\n" );
	return ( &cert->publicKey );
}

int EVP_PKEY_copy_parameters( EVP_PKEY*to, const EVP_PKEY*from )
{
	uTrace( "TODO: EVP_PKEY_copy_parameters\n" );
	/* "returns 1 for success and 0 for failure." */
	return ( 0 );
}

EVP_PKEY* EVP_PKEY_new( void )
{
	psPubKey_t*pubKey;

	eeTrace( "Enter EVP_PKEY_new\n" );
	eeTrace( "Exit EVP_PKEY_new\n" );
	if ( psNewPubKey( NULL, PS_NOKEY, &pubKey ) == PS_SUCCESS )
	{
		return ( pubKey );
	}
	else
	{
		return ( NULL );
	}
}

void EVP_PKEY_free( EVP_PKEY*key )
{
	eeTrace( "Enter EVP_PKEY_free\n" );
	psClearPubKey( key );
	psFree( key, NULL );
	eeTrace( "Exit EVP_PKEY_free\n" );
}


EVP_PKEY* SSL_get_privatekey( SSL*ssl )
{
	SSL_CTX*ctx = ssl->ctx;

	eeTrace( "Enter SSL_get_privatekey\n" );
	if ( ( ctx == NULL ) || ( ctx->keys == NULL ) )
	{
		return ( NULL );
	}
	eeTrace( "Exit SSL_get_privatekey\n" );
	return ( &ctx->keys->privKey );
}

#endif


int32 SSL_CTX_Load_Key_Material_RSA( SSL_CTX*ctx, const unsigned char*certBuf,
								 int32 certLen, const unsigned char*privBuf, int32 privLen,
								 const unsigned char*CAbuf, int32 CAlen )
{

	int32 rc = PS_FAILURE;
#ifdef USE_RSA
	rc = matrixSslLoadRsaKeysMem( ctx->keys, certBuf, certLen,
								  privBuf, privLen, CAbuf, CAlen );

	if ( rc != PS_SUCCESS )
	{
		_psTrace( "Certificate material load failure RSA key. Exiting\n" );
		matrixSslDeleteKeys( ctx->keys );
		matrixSslClose();
	}
#endif
	return ( rc );
}

int32 SSL_CTX_Load_Key_Material_ECC( SSL_CTX*ctx, const unsigned char*certBuf,
								 int32 certLen, const unsigned char*privBuf, int32 privLen,
								 const unsigned char*CAbuf, int32 CAlen )
{
	int32 rc = PS_FAILURE;
#ifdef USE_ECC
	rc = matrixSslLoadEcKeysMem( ctx->keys, certBuf, certLen,
								 privBuf, privLen, CAbuf, CAlen );

	if ( rc != PS_SUCCESS )
	{
		_psTrace( "Certificate material load failure ECC key. Exiting\n" );
		matrixSslDeleteKeys( ctx->keys );
		matrixSslClose();
	}
#endif
	return ( rc );
}


/******************************************************************************/
/* FUTURE */
int SSL_CTX_use_PrivateKey_file( SSL_CTX*ctx, const char*file, int type )
{
	uTrace( "TODO: SSL_CTX_use_PrivateKey_file\n" );
	return ( 1 );
}

int SSL_CTX_check_private_key( const SSL_CTX*ctx )
{
	uTrace( "TODO: SSL_CTX_check_private_key\n" );
	return ( 1 );
}

/******************************************************************************/
/* FUTURE */
void SSL_CTX_set_default_passwd_cb( SSL_CTX*ctx, int ( *pem_password_cb )( char*,
																		   int, int, void* ) )
{
	uTrace( "TODO: SSL_CTX_set_default_passwd_cb\n" );
}

/******************************************************************************/
/* FUTURE */
int SSL_CTX_use_certificate_chain_file( SSL_CTX*ctx, const char*file )
{
	uTrace( "TODO: SSL_CTX_use_certificate_chain_file\n" );
	return ( 1 );
}
#if 0
void SSL_CTX_set_verify( SSL_CTX*ctx, int mode,
						 int ( *verify_callback )( int, X509_STORE_CTX* ) )
{
	eeTrace( "Enter SSL_CTX_set_verify\n" );
	ctx->verify_callback = verify_callback;
	eeTrace( "Exit SSL_CTX_set_verify\n" );
}


X509_STORE* SSL_CTX_get_cert_store( const SSL_CTX*ctx )
{
	eeTrace( "Enter SSL_CTX_get_cert_store\n" );
	if ( ctx->keys == NULL )
	{
		return ( NULL );
	}
	eeTrace( "Exit SSL_CTX_get_cert_store\n" );
	return ( ctx->keys->cert );
}

#endif

/******************************************************************************/
void SSL_load_error_strings( void )
{
	uTrace( "TODO: SSL_load_error_strings\n" );
}

/******************************************************************************/
void* SSLv23_server_method( void )
{
	uTrace( "TODO: SSLv23_server_method\n" );
	return ( NULL );
}

/******************************************************************************/
void* SSLv23_client_method( void )
{
	uTrace( "TODO: SSLv23_client_method\n" );
	return ( NULL );
}

void* SSLv2_client_method( void )
{
	uTrace( "DEPRECATED\n" );
	return ( NULL );
}

void* SSLv3_client_method( void )
{
	uTrace( "TODO: SSLv3_client_method\n" );
	return ( NULL );
}

/******************************************************************************/
int SSL_set_fd( SSL*cp, int fd )
{
	eeTrace( "Enter SSL_set_fd\n" );
	cp->fd = fd;
	eeTrace( "Exit SSL_set_fd\n" );
	return ( 1 );
}

/******************************************************************************/
/*
    1 - The TLS/SSL handshake was successfully completed, a TLS/SSL connection
        has been established.
    0 - The TLS/SSL handshake was not successful but was shut down controlled
        and by the specifications of the TLS/SSL protocol. Call SSL_get_error()
        with the return value ret to find out the reason.
    <0 - The TLS/SSL handshake was not successful, because a fatal error
        occurred either at the protocol level or a connection failure occurred.
        The shutdown was not clean.
 */
int SSL_do_handshake( SSL*cp )
{
	int32 rc, transferred, len, done;
	ssl_t*ssl;
	unsigned char*buf;

	eeTrace( "Enter SSL_do_handshake\n" );

	//BK HACK
	//printf("CHANGING SOCKET TO BLOCKING MODE\n");
	//setSocketBlock(cp->fd);
	//END BK HACK

	ssl = cp->ssl;
	done = 0;

WRITE_MORE:
	while ( ( len = matrixSslGetOutdata( ssl, &buf ) ) > 0 )
	{
		transferred = socketWrite( cp->fd, buf, len );
		if ( transferred <= 0 )
		{
			psTraceInfo( "Socket send failed... exiting\n" );
			rc = transferred;
			cp->wouldblock = IS_WOULDBLOCK( transferred )
			;
			goto L_CLOSE_ERR;
		}
		else
		{
			/* Indicate that we've written > 0 bytes of data */
			if ( ( rc = matrixSslSentData( ssl, transferred ) ) < 0 )
			{
				goto L_CLOSE_ERR;
			}
			if ( rc == MATRIXSSL_REQUEST_CLOSE )
			{
				/* We've sent an alert that we are closing */
				eeTrace( "Exit SSL_do_handshake 1\n" );
				return ( 0 ); /* Controlled disconnect */
			}
			if ( rc == MATRIXSSL_HANDSHAKE_COMPLETE )
			{
				/*  Client - resumed handshake success
				    Server - standard handshake success */
				if ( ssl->outlen > 0 )
				{
					/* Issue here is that SentData returns HANDSHAKE_COMPLETE
					    when any part of FINISHED message is sent.  If this
					    was a partial send of the FINISHED message we'll need
					    another pass (or two) to send the remainder */
					done = 1;
				}
				else
				{
					eeTrace( "Exit SSL_do_handshake 2\n" );
					return ( 1 );
				}
			}
			if ( ( rc == MATRIXSSL_SUCCESS ) && ( done == 1 ) && ( ssl->outlen == 0 ) )
			{
				eeTrace( "Exit SSL_do_handshake 3\n" );
				return ( 1 );
			}
			/* SSL_REQUEST_SEND and SSL_SUCCESS are handled by loop logic */
		}
	}
READ_MORE:
	if ( ( len = matrixSslGetReadbuf( ssl, &buf ) ) <= 0 )
	{
		rc = len;
		goto L_CLOSE_ERR;
	}
	if ( ( transferred = socketRead( cp->fd, buf, len ) ) < 0 )
	{
		psTraceInfo( "Socket recv failed... exiting\n" );
		rc = transferred;
		cp->wouldblock = IS_WOULDBLOCK( transferred )
		;
		goto L_CLOSE_ERR;
	}
	/*	If EOF, remote socket closed */
	if ( transferred == 0 )
	{
		rc = 0; /* Consider this a clean shutdown from client perspective */
		goto L_CLOSE_ERR;
	}
	if ( ( rc = matrixSslReceivedData( ssl, ( int32 )transferred, &buf,
									   ( uint32* )&len ) ) < 0 )
	{
		psTraceInfo( "matrixSslReceivedData failure... exiting\n" );
		goto L_CLOSE_ERR;
	}

PROCESS_MORE:
	switch ( rc )
	{
		case MATRIXSSL_HANDSHAKE_COMPLETE:
			eeTrace( "Exit SSL_do_handshake 4\n" );
			return ( 1 ); /* Normal handshake success */

		case MATRIXSSL_APP_DATA:
			if ( !( ssl->flags & SSL_FLAGS_SERVER ) )
			{
				/* Client should never see any app data during connection */
				psTraceInfo( "Got app data during handshake.. exiting\n" );
				rc = PS_UNSUPPORTED_FAIL;
				goto L_CLOSE_ERR;
			}
			/* Case of session resumption with immediate application data
			    tacked on the end of the client's FINISHED message */
			cp->resumedAppDataLen = len;
			cp->appRecLen = len;
#ifdef USE_TLS_1_1
			if ( ssl->flags & SSL_FLAGS_TLS_1_1 )
			{
				/* ReceivedData is giving good lengths and pointers of the
				    record itself but we need to add the explicit IV back
				    in beause we're processing this later */
				cp->outBufOffset = ssl->enBlockSize;
				cp->resumedAppDataLen += ssl->enBlockSize;
				cp->appRecLen += ssl->enBlockSize;
			}
#endif
			eeTrace( "Exit SSL_do_handshake 5\n" );
			return ( 1 );

		case MATRIXSSL_REQUEST_SEND:
			goto WRITE_MORE;

		case MATRIXSSL_REQUEST_RECV:
			goto READ_MORE;

		case MATRIXSSL_RECEIVED_ALERT:
			/* The first byte of the buffer is the level */
			/* The second byte is the description */
			if ( *buf == SSL_ALERT_LEVEL_FATAL )
			{
				psTraceIntInfo( "Fatal SSL alert: %d, closing connection\n",
								*( buf + 1 ) );
				
				// With chrome update 73 we get certificate unknown error ( because we 
				// are using self signed certificate ) with each https request
				// causing server to send certificate on every request. Disabling this 
				// flag here would prevent deleting the session from the session table 
				// so that we can reuse the session for next connection.
				// for more info check LTK-3840 
				if( *( buf + 1 ) == SSL_ALERT_CERTIFICATE_UNKNOWN )
				{
					ssl->flags &= ~SSL_FLAGS_ERROR;
				}
				rc = 0; /* Consider it a clean disconnect */
				goto L_CLOSE_ERR;
			}
			/* Closure alert is normal (and best) way to close */
			if ( *( buf + 1 ) == SSL_ALERT_CLOSE_NOTIFY )
			{
				rc = 0; /* A truly graceful disconnect */
				goto L_CLOSE_ERR;
			}
			psTraceIntInfo( "Warning alert: %d\n", *( buf + 1 ) );
			if ( ( rc = matrixSslProcessedData( ssl, &buf, ( uint32* )&len ) ) == 0 )
			{
				/* No more data in buffer. Might as well read for more. */
				goto READ_MORE;
			}
			goto PROCESS_MORE;

		default:
			/* Will never be hit due to rc < 0 test at ReceivedData level */
			goto L_CLOSE_ERR;
	}

L_CLOSE_ERR:
	eeTrace( "Exit SSL_do_handshake 6\n" );
	//LTK commented(if required, session will be closed in upper interfce)
	//matrixSslDeleteSession(ssl);
	//cp->ssl = NULL;
	return ( rc );
}

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
    Server side.  Accept an incomming SSL connection request.
 */
int32 SSL_accept( SSL*cp )
{
	ssl_t*ssl;
	int32 rc;
	sslSessOpts_t options;

	eeTrace( "Enter SSL_accept\n" );
	memset( &options, 0x0, sizeof( sslSessOpts_t ) );
	options.userPtr = cp->ctx->keys;
#if 0//def USE_CLIENT_AUTH
	if ( ( rc = matrixSslNewServerSession( &ssl, cp->ctx->keys, SSL_cert_auth,
										   &options ) ) < 0 )
	{
		psTraceIntInfo( "matrixSslNewServerSession failed: %d\n", rc );
		return ( rc );
	}
	//ssl->verify_callback = cp->ctx->verify_callback;
#else
	if ( ( rc = matrixSslNewServerSession( &ssl, cp->ctx->keys, NULL, &options ) ) <
		 0 )
	{
		psTraceIntInfo( "matrixSslNewServerSession failed: %d\n", rc );
		return ( rc );
	}
#endif

	cp->ssl = ssl;
	//LTK commented
	//rc = SSL_do_handshake(cp);
	eeTrace( "Exit SSL_accept\n" );
	//LTK modified from rc to 1
	return ( 1 );
}

#endif

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
    Client side SSL handshake

    OpenSSL return codes:

    1 - The TLS/SSL handshake was successfully completed, a TLS/SSL connection
        has been established.
    0 - The TLS/SSL handshake was not successful but was shut down controlled
        and by the specifications of the TLS/SSL protocol. Call SSL_get_error()
        with the return value ret to find out the reason.
    <0 - The TLS/SSL handshake was not successful, because a fatal error
        occurred either at the protocol level or a connection failure occurred.
        The shutdown was not clean.
 */
int32 SSL_connect( SSL*cp )
{
	int32 rc;
	ssl_t*ssl;
	sslSessOpts_t options;
	//int ret_err;
	int cipherSpec_Len = 0;
	const uint16* cipherSpec = NULL;

	tlsExtension_t* extensions = NULL;
	sslExtCb_t* Extcb = NULL;
	sslCertCb_t* Certcb = NULL;

	eeTrace( "Enter SSL_connect\n" );
	if ( cp->ssl != NULL )
	{
		if ( matrixSslHandshakeIsComplete( cp->ssl ) )
		{
			if ( matrixSslEncodeRehandshake( cp->ssl, NULL, NULL, 0, 0, 0 ) <
				 0 )
			{
				psTraceInfo( "matrixSslEncodeRehandshake failed\n" );
				return ( -1 );
			}
		}
		else
		{
			psTraceInfo( "ERROR: SSL_connect called on existing SSL\n" );
			return ( -1 );
		}
	}
	else
	{
		memset( &options, 0x0, sizeof( sslSessOpts_t ) );
		rtk_set_ciphers_and_options( cp, &cipherSpec, &cipherSpec_Len, &Certcb, &extensions, &Extcb,
									 &( cp->resume ), &options );

		//ssgopenssl start
		rc = matrixSslNewClientSession( &ssl, cp->ctx->keys, cp->resume, cipherSpec, cipherSpec_Len,
										( sslCertCb_t )Certcb, cp->host, extensions,
										( sslExtCb_t )Extcb, &options );
		if ( extensions != NULL )
		{
			matrixSslDeleteHelloExtension( extensions );
		}
		if ( rc != MATRIXSSL_REQUEST_SEND )
		{
			psTraceInfo( "New client session failed... exiting\n" );
			return ( 0 );
		}
		cp->ssl = ssl;
	}

	//if ((rc = SSL_do_handshake(cp)) <= 0)
	//{
	//cp->ssl = NULL;
	//}
	eeTrace( "Exit SSL_connect\n" );
	return ( rc );

}

#endif

/******************************************************************************/
/*
    OpenSSL return codes

    >0	The read operation was successful; the return value is the number
        of bytes actually read from the TLS/SSL connection.

    0	The read operation was not successful. The reason may either be a clean
        shutdown due to a ``close notify'' alert sent by the peer. It is also
        possible, that the peer simply shut down the underlying transport and
        the shutdown is incomplete.

   <0	The read operation was not successful, because either an error occurred
        or action must be taken by the calling process.
 */
int32 SSL_get_data( SSL*cp, unsigned char**ptBuf, int*ptBufLen )
{
	ssl_t*ssl;
	unsigned char*ctBuf;
	int32 readLen, rc;
	int transferred;

	eeTrace( "Enter SSL_get_data\n" );
	ssl = cp->ssl;
	/*
	    There are a few ways data might already be available to read for a
	    server here.  They are triggered on whether there is any length for
	    incoming data.
	 */

	if ( cp->resumedAppDataLen )
	{
		/* This case is when a session resumption handshake takes place
			and the client sends application data in the same flight as
			the FINISHED message.  In this case, the application data has
			already been decrypted and is just waiting to be picked off */
		*ptBuf = ssl->inbuf + ssl->deBlockSize;
		*ptBufLen = cp->resumedAppDataLen - ssl->deBlockSize;//ssl->inlen;
		//ssl->inlen = 0;
		cp->resumedAppDataLen = 0;
		eeTrace( "Exit SSL_get_data 1\n" );
		return ( *ptBufLen );
	}
	else if (ssl->inlen > 0 )
	{
		/* This case is the Chrome false start where application data has
			been sent in the middle of the handshake.  MatrixSSL has not
			yet decrypted the data in this case so we do that now. */
		rc = matrixSslReceivedData( ssl, 0, ptBuf, ( uint32* )ptBufLen );
		goto PROCESS_MORE;
	}

READ_MORE:

	if ( ( readLen = matrixSslGetReadbuf( ssl, &ctBuf ) ) <= 0 )
	{
		return ( readLen );
	}
	if ( ( transferred = socketRead( cp->fd, ctBuf, readLen ) ) < 0 )
	{
		psTraceInfo( "Socket recv failed... exiting\n" );
		cp->wouldblock = IS_WOULDBLOCK( transferred )
		;
		return ( transferred );
	}
	/*	If EOF, remote socket closed */
	if ( transferred == 0 )
	{
		psTraceInfo( "Server disconnected\n" );
		eeTrace( "Exit SSL_get_data 2\n" );
		return ( 0 ); /* Consider this a clean shutdown from client perspective */
	}
	if ( ( rc = matrixSslReceivedData( ssl, ( int32 )transferred, ptBuf,
									   ( uint32* )ptBufLen ) ) < 0 )
	{
		psTraceInfo( "matrixSslReceivedData failure... exiting\n" );
		return ( rc );
	}

PROCESS_MORE:
	switch ( rc )
	{
		case MATRIXSSL_APP_DATA:
			if ( *ptBufLen == 0 )
			{
				/* Eat any zero length records that come across. OpenSSL server
				    is likely sending some */
				rc = matrixSslProcessedData( ssl, ptBuf, ( uint32* )ptBufLen );
				goto PROCESS_MORE;
			}
			eeTrace( "Exit SSL_get_data 3\n" );
			return ( *ptBufLen );

		case MATRIXSSL_REQUEST_SEND:
			SSL_do_handshake( cp );
			psTraceInfo( "Peer requested a re-handshake\n" );
			goto READ_MORE;

		case MATRIXSSL_REQUEST_RECV:
			goto READ_MORE;

		case MATRIXSSL_RECEIVED_ALERT:
			/* The first byte of the buffer is the level */
			/* The second byte is the description */
			if ( **ptBuf == SSL_ALERT_LEVEL_FATAL )
			{
				psTraceIntInfo( "Fatal SSL alert: %d, closing connection\n",
								*( *ptBuf + 1 ) );
				eeTrace( "Exit SSL_get_data 4\n" );
				return ( 0 ); /* Actually a clean disconnect */
			}
			/* Closure alert is normal (and best) way to close */
			if ( *( *ptBuf + 1 ) == SSL_ALERT_CLOSE_NOTIFY )
			{
				eeTrace( "Exit SSL_get_data 5\n" );
				return ( 0 ); /* Graceful disconnect */
			}
			psTraceIntInfo( "Warning alert: %d\n", *( *ptBuf + 1 ) );
			if ( ( rc = matrixSslProcessedData( ssl, ptBuf, ( uint32* )ptBufLen ) ) ==
				 0 )
			{
				/* No more data in buffer. Might as well read for more. */
				goto READ_MORE;
			}
			goto PROCESS_MORE;

		default:
			/* MATRIXSSL_HANDSHAKE_COMPLETE */
			/* Will never be hit due to rc < 0 test at ReceivedData level */
			return ( -1 );
	}

	eeTrace( "Exit SSL_get_data 6\n" );
}

/******************************************************************************/
/*	Not an OpenSSL function */
/* TODO: return codes.  <0 is failure */
int SSL_processed_data( SSL*cp, unsigned char**ptBuf, int*ptBufLen )
{
	int32 rc;

	eeTrace( "Enter SSL_processed_data\n" );
	rc = matrixSslProcessedData( cp->ssl, ptBuf, ( uint32* )ptBufLen );

PROCESS_MORE:
	/* MATRIXSSL_SUCCESS or MATRIXSSL_APP_DATA are expected */
	if ( rc == MATRIXSSL_RECEIVED_ALERT )
	{
		if ( *( *ptBuf + 1 ) == SSL_ALERT_CLOSE_NOTIFY )
		{
			psTraceInfo( "Received close_notify alert from server\n" );
		}
		else
		{
			psTraceInfo( "Received unexpected alert from server\n" );
		}
		rc = matrixSslProcessedData( cp->ssl, ptBuf, ( uint32* )ptBufLen );
		goto PROCESS_MORE;
	}
	else if ( rc == MATRIXSSL_REQUEST_RECV )
	{
		if ( SSL_get_data( cp, ptBuf, ptBufLen ) <= 0 )
		{
			psTraceInfo( "Unable to get next record\n" );
			return ( PS_FAILURE );
		}
		if ( ptBuf == NULL )
		{
			/* always expecting a full record out of here */
			psTraceInfo( "Is this a possible case??\n" );
		}
	}
	else if ( ( rc != MATRIXSSL_SUCCESS ) && ( rc != MATRIXSSL_APP_DATA ) )
	{
		psTraceIntInfo( "!!!!!!  Handle this case %d  !!!!!! \n", rc );
		return ( PS_UNSUPPORTED_FAIL );
	}
	eeTrace( "Exit SSL_processed_data\n" );
	return ( *ptBufLen );
}

/* Test if SSL_read has buffered data */
int SSL_pending( const SSL*ssl )
{
	eeTrace( "Enter SSL_pending\n" );
	/* Three places data could be waiting */
	/* Anything in the ssl_t inlen means there is data */
	if ( ssl->ssl->inlen > 0 )
	{
		eeTrace( "Exit SSL_pending 1\n" );
		return ( ssl->ssl->inlen );
	}
	/* Corner case of app data following a FINISHED message that doesn't
	    get pulled out in SSL_do_handshake */
	if ( ssl->resumedAppDataLen > 0 )
	{
		eeTrace( "Exit SSL_pending 2\n" );
		return ( ssl->resumedAppDataLen );
	}
	/* Partial record still sitting in inbuf */
	eeTrace( "Exit SSL_pending 3\n" );
	return ( ssl->appRecLen - ssl->outBufOffset );
}

/******************************************************************************/
/*
    OpenSSL return codes

    >0	The read operation was successful; the return value is the number
        of bytes actually read from the TLS/SSL connection.

    0	The read operation was not successful. The reason may either be a clean
        shutdown due to a ``close notify'' alert sent by the peer. It is also
        possible, that the peer simply shut down the underlying transport and
        the shutdown is incomplete.

   <0	The read operation was not successful, because either an error occurred
        or action must be taken by the calling process.
 */
int32 SSL_read( SSL*cp, void*userBufPtr, int userBufLen )
{
	ssl_t*ssl;
	unsigned char*readBuf, *ptBuf, *userBuf;
	int32 readLen, leftover, bytes, rc;
	uint32 nextRecLen, ptBufLen;
	int transferred;

	eeTrace( "Enter SSL_read\n" );
	ssl = cp->ssl;
	userBuf = ( unsigned char* )userBufPtr;

	/*
	    There are a few ways data might already be available to read for a
	    server here.  Two are triggered on whether there is any length for
	    incoming data.
	 */
	if ( cp->resumedAppDataLen )
	{
		/* This case is when a session resumption handshake takes place
			and the client sends application data in the same flight as
			the FINISHED message.  In this case, the application data has
			already been decrypted and is just waiting to be picked off.
			The ssl->inlen > 0 indicates the is also more unencrypted
			data after this decrypted record.  Seen to happen in TLS 1.0
			when 1 byte BEAST workaround record is sent */
		cp->appRecLen = cp->resumedAppDataLen;
		cp->outBufOffset = 0;
		cp->resumedAppDataLen = 0;
	}
	else if ( ssl->inlen > 0 )
	{
		/* This case is the Chrome false start where application data has
			been sent in the middle of the handshake.  MatrixSSL has not
			yet decrypted the data in this case so we do that now. */
		rc = matrixSslReceivedData( ssl, 0, &ptBuf, &ptBufLen );
		goto PROCESS_MORE;
	}

	/*
	    Always send any remainder first
	 */
	leftover = cp->appRecLen - cp->outBufOffset;
	if ( leftover > 0 )
	{
		bytes = ( int32 )min( userBufLen, leftover );
		memcpy( userBuf, ssl->inbuf + ssl->deBlockSize + cp->outBufOffset, bytes );
		cp->outBufOffset += bytes;
		if ( cp->outBufOffset == cp->appRecLen )
		{
			cp->outBufOffset = cp->appRecLen = 0;
			SSL_processed_data( cp, &ptBuf, ( int* )&ptBufLen );
			if ( ptBufLen > 0 )
			{
				/* Make this the next 'leftover' */
				cp->appRecLen = ptBufLen;
			}
		}
		eeTrace( "Exit SSL_read 1\n" );
		return ( bytes );
	}

READ_MORE:
	if ( ( readLen = matrixSslGetReadbuf( ssl, &readBuf ) ) <= 0 )
	{
		return ( readLen );
	}
	if ( ( transferred = socketRead( cp->fd, readBuf, readLen ) ) < 0 )
	{
		psTraceInfo( "Socket recv failed... exiting\n" );
		cp->wouldblock = IS_WOULDBLOCK( transferred )
		;
		return ( transferred );
	}
	/*	If EOF, remote socket closed */
	if ( transferred == 0 )
	{
		psTraceInfo( "Server disconnected\n" );
		eeTrace( "Exit SSL_read 2\n" );
		return ( 0 ); /* Consider this a clean shutdown from client perspective */
	}
	if ( ( rc = matrixSslReceivedData( ssl, ( int32 )transferred, &ptBuf,
									   &ptBufLen ) ) < 0 )
	{
		psTraceInfo( "matrixSslReceivedData failure... exiting\n" );
		return ( rc );
	}

PROCESS_MORE:
	switch ( rc )
	{
		case MATRIXSSL_APP_DATA:

			if ( ptBufLen == 0 )
			{
				/* Eat any zero length records that come across. OpenSSL server
				   is likely sending some */
				rc = matrixSslProcessedData( ssl, &ptBuf, &ptBufLen );
				goto PROCESS_MORE;
			}
			/* Always a full app data record here but need to know if user
			    can accept the whole thing in one pass */
			if ( ptBufLen <= userBufLen )
			{
				memcpy( userBuf, ptBuf, ptBufLen );
				/* Have a full record so update that it's processed */
				SSL_processed_data( cp, &ptBuf, ( int* )&nextRecLen );

#ifdef USE_REHANDSHAKING
				if ( cp->ssl->hsState != SSL_HS_DONE )
				{
					cp->rehandshake = 1;
				}
#endif
				if ( nextRecLen > 0 )
				{
					cp->appRecLen = nextRecLen;
				}
				eeTrace( "Exit SSL_read 3\n" );
				return ( ptBufLen );
			}
			else
			{
				cp->appRecLen = ptBufLen;
				memcpy( userBuf, ptBuf, userBufLen );
				cp->outBufOffset += userBufLen;
				eeTrace( "Exit SSL_read 4\n" );
				return ( userBufLen );
			}

		case MATRIXSSL_REQUEST_SEND:
			/* If the library is telling us there is data to send, this must
			    be a re-handshake. (this could also be an alert that is being
			    sent but SSL_do_handshake will handle this scenario as well) */
#ifdef USE_REHANDSHAKING
			cp->rehandshake = 1;
			return ( -1 );

#else
			if ( SSL_do_handshake( cp ) <= 0 )
			{
				return ( -1 );
			}
#endif
			goto READ_MORE;

		case MATRIXSSL_REQUEST_RECV:
			goto READ_MORE;

		case MATRIXSSL_RECEIVED_ALERT:
			/* The first byte of the buffer is the level */
			/* The second byte is the description */
			if ( *ptBuf == SSL_ALERT_LEVEL_FATAL )
			{
				psTraceIntInfo( "Fatal SSL alert: %d, closing connection\n",
								*( ptBuf + 1 ) );
				eeTrace( "Exit SSL_read 5\n" );
				return ( 0 ); /* Actually a clean disconnect */
			}
			/* Closure alert is normal (and best) way to close */
			if ( *( ptBuf + 1 ) == SSL_ALERT_CLOSE_NOTIFY )
			{
				eeTrace( "Exit SSL_read 6\n" );
				return ( 0 ); /* Graceful disconnect */
			}
			psTraceIntInfo( "Warning alert: %d\n", *( ptBuf + 1 ) );
			if ( ( rc = matrixSslProcessedData( ssl, &ptBuf, &ptBufLen ) ) == 0 )
			{
				/* No more data in buffer. Might as well read for more. */
				goto READ_MORE;
			}
			goto PROCESS_MORE;

		default:
			/* MATRIXSSL_HANDSHAKE_COMPLETE */
			/* Will never be hit due to rc < 0 test at ReceivedData level */
			return ( -1 );
	}

	eeTrace( "Exit SSL_read 7\n" );
}

/******************************************************************************/
/*
    OpenSSL return codes

    >0	The write operation was successful, the return value is the number of
        bytes actually written to the TLS/SSL connection.
    0	The write operation was not successful. Probably the underlying
        connection was closed
    <0	The write operation was not successful, because either an error
        occurred or action must be taken by the calling process.
 */
int32 SSL_write( SSL*cp, const void*inbufPtr, int32 inlen )
{
	ssl_t*ssl;
	unsigned char*buf;
	int transferred;
	const char*inbuf;
	int32 len, bytesSent = 0;

	eeTrace( "Enter SSL_write\n" );
	inbuf = ( char* )inbufPtr;
	ssl = cp->ssl;

	if ( inbuf != NULL )
	{ /* A NULL inbuf means internal buffer needs to flush */
		/* Test if inbuf falls without our buffer to determine
		  what option they have choosen */
		if ( ( ( int32 )inbuf >= ( int32 )ssl->outbuf ) &&
			 ( ( int32 )inbuf < ( int32 ) ( ssl->outbuf + ssl->outsize ) ) )
		{
			matrixSslEncodeWritebuf( ssl, inlen );

		}
		else if ( matrixSslEncodeToOutdata( ssl, ( unsigned char* )inbuf, inlen ) < 0 )
		{
			return ( -1 ); //LTK modified, return -1 if failed in buffer allocation or encoding
		}

	}

	while ( ( len = matrixSslGetOutdata( ssl, &buf ) ) > 0 )
	{
		//LTK modified,
		if ( inbuf != NULL )
		{
			len = (len / MAX_ENCOD_TEXT_IN_OUT_BUF) * MAX_ENCOD_TEXT_IN_OUT_BUF;
		}

		if ( len )
		{
			transferred = socketWrite( cp->fd, buf, len );
			if ( transferred <= 0 )
			{
				psTraceInfo( "Socket send failed... exiting\n" );
				cp->wouldblock = IS_WOULDBLOCK( transferred )
				;
				return ( transferred );
			}
			else
			{
				bytesSent += transferred;
				/* Indicate that we've written > 0 bytes of data */
				if ( matrixSslSentData( ssl, transferred ) < 0 )
				{
					return ( -1 );
				}
				/* SSL_REQUEST_SEND and SSL_SUCCESS are handled by loop logic */
			}
		}
		else
		{
			return ( 1 );
		}
	}
	eeTrace( "Exit SSL_write\n" );
	return ( bytesSent );
}

/******************************************************************************/
/*
    Send a CLOSE_NOTIFY alert and delete a session that was opened with
    SSL_accept or SSL_connect
 */
void SSL_shutdown( SSL*cp )
{
	ssl_t*ssl;
	unsigned char*buf;
	int32 len;

	eeTrace( "Enter SSL_shutdown\n" );
	ssl = cp->ssl;

	if ( ( ssl != NULL ) && ( cp->fd > 0 ) )
	{

		setSocketNonblock( cp->fd );
		/* Quick attempt to send a closure alert, don't worry about failure */
		if ( matrixSslEncodeClosureAlert( ssl ) >= 0 )
		{
			if ( ( len = matrixSslGetOutdata( ssl, &buf ) ) > 0 )
			{
				if ( ( len = socketWrite( cp->fd, buf, len ) ) > 0 )
				{
					matrixSslSentData( ssl, len );
				}
			}
		}
		matrixSslDeleteSession( ssl );
		cp->ssl = NULL;
	}
	cp->fd = 0;
	eeTrace( "Exit SSL_shutdown\n" );
}

/* "SSL_peek - Copy the data in the SSL buffer into the buffer
       passed to this API" */
int SSL_peek( SSL*s, void*buf, int num )
{
	uTrace( "TODO: SSL_peek\n" );
	return ( 0 );
}

/* "SSL_set_connect_state() sets ssl to work in client mode.

    SSL_set_accept_state() sets ssl to work in server mode." */
void SSL_set_connect_state( SSL*ssl )
{
	uTrace( "TODO: SSL_set_connect_state\n" );
}

/* SSL_set_session() sets session to be used when the TLS/SSL connection is
    to be established. SSL_set_session() is only useful for TLS/SSL clients.
    When the session is set, the reference count of session is incremented by
    1. If the session is not reused, the reference count is decremented again
    during SSL_connect(). Whether the session was reused can be queried with
    the SSL_session_reused call.

    If there is already a session set inside ssl (because it was set with
    SSL_set_session() before or because the same ssl was already used for a
    connection), SSL_SESSION_free() will be called for that session. */
int SSL_set_session( SSL*ssl, SSL_SESSION*session )
{
	uTrace( "TODO: SSL_set_session\n" );
	/* 0 is failure.  1 is success */
	return ( 1 );
}

void SSL_SESSION_free( SSL_SESSION*session )
{
	uTrace( "TODO: SSL_SESSION_free\n" );
}

#if 0
SSL_SESSION* SSL_get_session( const SSL*ssl )
{
	eeTrace( "Enter SSL_get_session\n" );
	eeTrace( "Exit SSL_get_session\n" );
	return ( ssl );
}

SSL_CIPHER* SSL_get_cipher( const SSL*ssl )
{
	ssl_t*lssl = ssl->ssl;

	eeTrace( "Enter SSL_get_cipher\n" );
	if ( lssl == NULL )
	{
		return ( NULL );
	}
	eeTrace( "Exit SSL_get_cipher\n" );
	return ( lssl->cipher );
}

X509* SSL_get_peer_cert_chain( const SSL*s )
{
	ssl_t*lssl = s->ssl;

	eeTrace( "Enter SSL_get_peer_cert_chain\n" );
	if ( lssl == NULL )
	{
		return ( NULL );
	}
	eeTrace( "Exit SSL_get_peer_cert_chain\n" );
	return ( lssl->sec.cert );
}

X509* SSL_get_peer_certificate( const SSL*s )
{
	ssl_t*lssl = s->ssl;

	eeTrace( "Enter SSL_get_peer_certificate\n" );
	if ( lssl == NULL )
	{
		return ( NULL );
	}
	eeTrace( "Exit SSL_get_peer_certificate\n" );
	return ( lssl->sec.cert );
}

/* SSL_get_verify_result() returns the result of the verification of the X509
    certificate presented by the peer, if any. */
long SSL_get_verify_result( const SSL*ssl )
{
	ssl_t*lssl = ssl->ssl;

	eeTrace( "Enter SSL_get_verify_result\n" );

	/* X509_V_OK if verification succeeded or no peer cert presented */

	eeTrace( "Exit SSL_get_verify_result\n" );
	return ( lssl->sec.cert->authStatus );
}

#endif
/* X509_verify_cert_error_string() returns a human readable error string for
    verification error n. */
const char* X509_verify_cert_error_string( long n )
{
	/* Return string of authStatus (n) */
	uTrace( "TODO: X509_verify_cert_error_string\n" );
	return ( NULL );
}

uint16_t SSL_version( SSL* ssl )
{
	ssl_t* lssl = ssl->ssl;
    uint16_t ret;
    
	eeTrace( "Enter SSL_version\n" );
    
    ret = psEncodeVersion(GET_ACTV_VER(lssl));
    
    eeTrace( "Exit SSL_version\n" );
    
    return ret;
}

/******************************************************************************/
/*
    MatrixSSL err

    "SSL_get_error() returns a result code (suitable for the C "switch"
    statement) for a preceding call to SSL_connect(), SSL_accept(),
    SSL_do_handshake(), SSL_read(), SSL_peek(), or SSL_write() on ssl. The
    value returned by that TLS/SSL I/O function must be passed to
    SSL_get_error() in parameter ret.

    In addition to ssl and ret, SSL_get_error() inspects the current thread's
    OpenSSL error queue. Thus, SSL_get_error() must be used in the same thread
    that performed the TLS/SSL I/O operation, and no other OpenSSL function
    calls should appear in between. The current thread's error queue must be
    empty before the TLS/SSL I/O operation is attempted, or SSL_get_error()
    will not work reliably."

    These are what callers will be expecting!!

 #define SSL_ERROR_NONE				0
 #define SSL_ERROR_SSL				1
 #define SSL_ERROR_WANT_READ			2
 #define SSL_ERROR_WANT_WRITE        3
 #define SSL_ERROR_WANT_X509_LOOKUP  4
 #define SSL_ERROR_SYSCALL			5
 #define SSL_ERROR_ZERO_RETURN       6
 #define SSL_ERROR_WANT_CONNECT      7
 #define SSL_ERROR_WANT_ACCEPT       8
 */
int SSL_get_error( const SSL*cp, int ret )
{
	eeTrace( "Enter SSL_get_error\n" );
	if ( cp->ssl )
	{
		eeTrace( "Exit SSL_get_error\n" );
		return ( cp->ssl->err );
	}
	return ( 0 );
}
#if 0
/* Invoke the users registered callback if exists */
int32 SSL_cert_auth( ssl_t*ssl, psX509Cert_t*cert, int32 alert )
{
	eeTrace( "Enter SSL_cert_auth\n" );
	if ( ssl->verify_callback )
	{
		alert = ssl->verify_callback( alert, cert );
	}
	eeTrace( "Exit SSL_cert_auth\n" );

	printf( "HARD CODE SUCCESS RETURN FROM CERT CBACK\n" );
	return ( 0 );
	//return alert;
}
#endif
/******************************************************************************/
/* Mongoose web server wants */

int CRYPTO_num_locks( void )
{
	uTrace( "TODO: CRYPTO_num_locks\n" );
	return ( 1 );
}

void CRYPTO_set_locking_callback( void ( *cb )( int, int, const char*, int ) )
{
	uTrace( "TODO: CRYPTO_set_locking_callback\n" );
}

extern void CRYPTO_set_id_callback( unsigned long ( *cb )( void ) )
{
	uTrace( "TODO: CRYPTO_set_id_callback\n" );
}

unsigned long ERR_get_error( void )
{
	uTrace( "TODO: ERR_get_error\n" );
	return ( 0 );
}

char* ERR_error_string( unsigned long x, char*y )
{
	uTrace( "TODO: ERR_error_string\n" );
	return ( ( char* ) "unimplemented" );
}

/* End Mongoose */

void ERR_free_strings( void )
{
	uTrace( "TODO: ERR_free_strings\n" );
}

void ERR_remove_state( unsigned long pid )
{
	uTrace( "TODO: ERR_remove_state\n" );
}

void ERR_clear_error( void )
{
	uTrace( "TODO: ERR_clear_error\n" );
}

unsigned long ERR_peek_error( void )
{
	uTrace( "TODO: ERR_peek_error\n" );
	return ( 0 );
}

/******************************************************************************/
/* cURL wants */

/* Uses 'max_bytes' from the file to add to the PRNG.  There is no context
    parameter here so we'd need to create a global one if this family of
    APIs are implemented.   It looks like maybe RAND_set_rand_engine could be
    used as an initialization point, but it looks like there is also a default
    RAND engine so couldn't rely on that necessarily */
int RAND_load_file( const char*filename, long max_bytes )
{
	uTrace( "TODO: RAND_load_file\n" );
	return ( ( int )max_bytes );
}

void RAND_add( const void*buf, int num, int entropy )
{
	uTrace( "TODO: RAND_add\n" );
}

/* "generates a default path for the random seed file. buf points to a buffer
    of size num in which to store the filename. The seed file is $RANDFILE if
    that environment variable is set, $HOME/.rnd otherwise. If $HOME is not
    set either, or num is too small for the path name, an error occurs. */
const char* RAND_file_name( char*buf, size_t num )
{
	uTrace( "TODO: RAND_file_name\n" );
	return ( NULL );
}

int RAND_status( void )
{
	uTrace( "TODO: RAND_status\n" );
	return ( 1 );
}

int RAND_bytes( unsigned char*buf, int num )
{
	int rc;

	eeTrace( "Enter RAND_bytes\n" );
	rc = ( int )psGetEntropy( buf, ( int )num, NULL );
	eeTrace( "Exit RAND_bytes\n" );
	return ( rc );
}

/*
    typedef struct {
        sslBuf_t	*buf;
        int			type;
    } BIO_METHOD;

    typedef struct {
        BIO_METHOD	*method;
    } BIO;
 */
#if 0
BIO* BIO_new( BIO_METHOD*type )
{
	BIO*p_new;

	eeTrace( "Enter BIO_new\n" );
	p_new = ( BIO* )psMalloc( NULL, sizeof( BIO ) );
	if ( p_new == NULL )
	{
		return ( NULL );
	}
	p_new->method = type;
	eeTrace( "Exit BIO_new\n" );
	return ( p_new );
}

int BIO_free( BIO*a )
{
	BIO_METHOD*method;

	eeTrace( "Enter BIO_free\n" );
	if ( a == NULL )
	{
		return ( 0 );
	}
	method = a->method;
	if ( method->buf )
	{
		if ( method->buf->buf )
		{
			psFree( method->buf->buf, NULL );
		}
		psFree( method->buf, NULL );
	}
	psFree( method, NULL );
	memset_s( method, sizeof( BIO ), 0x0, sizeof( BIO_METHOD ) );

	psFree( a, NULL );
	eeTrace( "Exit BIO_free\n" );
	return ( 1 );
}

BIO_METHOD* BIO_s_mem( void )
{
	BIO_METHOD*p_new;
	psBuf_t*buf;

	eeTrace( "Enter BIO_s_mem\n" );

	p_new = ( BIO_METHOD* )psMalloc( NULL, sizeof( BIO_METHOD ) );
	if ( p_new == NULL )
	{
		return ( NULL );
	}
	buf = ( psBuf_t* )psMalloc( NULL, sizeof( psBuf_t ) );
	if ( buf == NULL )
	{
		psFree( p_new, NULL );
		return ( NULL );
	}
	buf->start = buf->end = buf->buf = ( unsigned char* )psMalloc( NULL, DEFAULT_BIO_BUF_LEN );
	if ( buf->start == NULL )
	{
		psFree( p_new, NULL );
		psFree( buf, NULL );
		return ( NULL );
	}
	buf->size = DEFAULT_BIO_BUF_LEN;

	p_new->type = BIO_S_MEM_TYPE;
	p_new->buf = buf;
	eeTrace( "Exit BIO_s_mem\n" );
	return ( p_new );
}

void BIO_get_mem_ptr( BIO*b, BUF_MEM**pp )
{
	eeTrace( "Enter BIO_get_mem_ptr\n" );
	*pp = NULL;
	if ( b == NULL )
	{
		return;
	}
	( *pp )->data = b->method->buf->start;
	( *pp )->length = ( b->method->buf->end - b->method->buf->start );
	eeTrace( "Exit BIO_get_mem_ptr\n" );
}

int PEM_write_bio_X509( BIO*bp, X509*x )
{
	/* Copy x.509 to BIO */
	uTrace( "TODO:  PEM_write_bio_X509\n" );
	return ( 0 );
}

int X509_NAME_print_ex( BIO*out, X509_NAME*nm, int indent, unsigned long flags )
{
	/* Copy x.509 name to BIO */
	uTrace( "TODO:  X509_NAME_print_ex\n" );
	return ( 0 );
}

X509* X509_STORE_CTX_get_current_cert( X509_STORE_CTX*ctx )
{
	eeTrace( "Enter X509_STORE_CTX_get_current_cert\n" );
	eeTrace( "Exit X509_STORE_CTX_get_current_cert\n" );
	return ( ctx );
}

X509_NAME* X509_get_subject_name( X509*a )
{
	eeTrace( "Enter X509_get_subject_name\n" );
	eeTrace( "Exit X509_get_subject_name\n" );
	return ( &a->subject );
}

char* X509_NAME_oneline( X509_NAME*a, char*buf, int size )
{
	uTrace( "TODO:  X509_NAME_oneline\n" );
	return ( NULL );
}

int sk_X509_num( X509*certs )
{
	eeTrace( "Enter sk_X509_num\n" );
	X509*next = certs;
	int i = 1;

	while ( next->next )
	{
		i++;
		next = next->next;
	}
	eeTrace( "Exit sk_X509_num\n" );
	return ( i );
}

X509* sk_X509_value( X509*cert, int i )
{
	X509*cur = cert;

	eeTrace( "Enter sk_X509_value\n" );
	/* TODO: this is using a zero based count */
	while ( i > 0 )
	{
		cur = cur->next;
		i++;
	}
	eeTrace( "Exit sk_X509_value\n" );
	return ( cur );
}

/* Related to STACK_OF.  Our extension structure isn't a list.  It's just
    one big struct that holds all the ones we care about */
int sk_X509_EXTENSION_num( X509_EXTENSION*ext )
{
	uTrace( "TODO:  sk_X509_EXTENSION_num\n" );
	return ( 1 ); /* Never a list */
}

X509_EXTENSION* sk_X509_EXTENSION_value( X509_EXTENSION*ext, int i )
{
	uTrace( "TODO:  sk_X509_EXTENSION_value\n" );
	return ( ext );
}

ASN1_OBJECT* X509_EXTENSION_get_object( X509_EXTENSION*ex )
{
	/* Hmm... how would we map our current extension to an "object" */
	uTrace( "TODO:  X509_EXTENSION_get_object\n" );
	return ( NULL );
}

int X509_EXTENSION_get_critical( X509_EXTENSION*ex )
{
	eeTrace( "Enter X509_EXTENSION_get_critical\n" );
	eeTrace( "Exit X509_EXTENSION_get_critical\n" );
	return ( ex->critFlags );
}

int X509V3_EXT_print( BIO*out, X509_EXTENSION*ext, unsigned long flag,
					  int indent )
{
	uTrace( "TODO:  X509V3_EXT_print\n" );
	return ( 0 );
}

/********/
void OPENSSL_load_builtin_modules( void )
{
	uTrace( "TODO:  OPENSSL_load_builtin_modules\n" );
}

int CONF_modules_load_file( const char*filename, const char*appname,
							unsigned long flags )
{
	psTraceInfo( "There is no configuration file for MatrixSSL\n" );
	return ( 0 );
}

void EVP_cleanup( void )
{
	uTrace( "TODO:  EVP_cleanup\n" );
}

void* X509_get_ext_d2i( X509*x, int nid, int*crit, int*idx )
{
	uTrace( "TODO:  X509_get_ext_d2i\n" );
	return ( NULL );
}

/* X509_NAME_get_index_by_NID() and X509_NAME_get_index_by_OBJ() retrieve the
    next index matching nid or obj after lastpos. lastpos should initially be
    set to -1. If there are no more entries -1 is returned. */
int X509_NAME_get_index_by_NID( X509_NAME*name, int nid, int lastpos )
{
	/* NID_commonName is nid */
	uTrace( "TODO:  X509_NAME_get_index_by_NID\n" );
	return ( 0 );
}

X509_NAME_ENTRY* X509_NAME_get_entry( X509_NAME*name, int loc )
{
	uTrace( "TODO:  X509_NAME_get_entry\n" );
	return ( NULL );
}

ASN1_STRING* X509_NAME_ENTRY_get_data( X509_NAME_ENTRY*ne )
{
	uTrace( "TODO:  X509_NAME_ENTRY_get_data\n" );
	return ( NULL );
}

X509_LOOKUP* X509_STORE_add_lookup( X509_STORE*v, X509_LOOKUP_METHOD*m )
{
	uTrace( "TODO:  X509_STORE_add_lookup\n" );
	return ( NULL );
}

X509_LOOKUP_METHOD* X509_LOOKUP_file( void )
{
	uTrace( "TODO:  X509_LOOKUP_file\n" );
	return ( NULL );
}

int X509_STORE_set_flags( X509_STORE*ctx, unsigned long flags )
{
	uTrace( "TODO:  X509_STORE_set_flags\n" );
	return ( 0 );
}

int X509_load_crl_file( X509_LOOKUP*ctx, const char*file, int type )
{
	uTrace( "TODO:  X509_load_crl_file\n" );
	return ( 0 );
}

void GENERAL_NAMES_free( GENERAL_NAME*name )
{
	uTrace( "TODO:  GENERAL_NAMES_free\n" );
}

const GENERAL_NAME* sk_GENERAL_NAME_value( GENERAL_NAME*altnames, int i )
{
	uTrace( "TODO:  sk_GENERAL_NAME_value\n" );
	return ( NULL );
}

int sk_GENERAL_NAME_num( GENERAL_NAME*names )
{
	uTrace( "TODO:  sk_GENERAL_NAME_num\n" );
	return ( 0 );
}

unsigned char* ASN1_STRING_data( ASN1_STRING*x )
{
	eeTrace( "Enter ASN1_STRING_data\n" );
	eeTrace( "Exit ASN1_STRING_data\n" );
	return ( x->data );
}

size_t ASN1_STRING_length( ASN1_STRING*x )
{
	eeTrace( "Enter ASN1_STRING_length\n" );
	eeTrace( "Exit ASN1_STRING_length\n" );
	return ( x->length );
}

int ASN1_STRING_type( ASN1_STRING*x )
{
	eeTrace( "Enter ASN1_STRING_type\n" );
	eeTrace( "Exit ASN1_STRING_type\n" );
	return ( x->type );
}

int ASN1_STRING_to_UTF8( unsigned char**out, ASN1_STRING*in )
{
	eeTrace( "Enter ASN1_STRING_to_UTF8\n" );
	eeTrace( "Exit ASN1_STRING_to_UTF8\n" );
	*out = in->data;
	return ( 0 );
}

int ASN1_STRING_print( BIO*bp, const ASN1_STRING*v )
{
	uTrace( "TODO:  ASN1_STRING_print\n" );
	return ( 0 );
}

/* i2t_ASN1_OBJECT tries to find the LN (long name) associated with the
    ASN1_OBJECT a and puts it in the buffer buf, or up to buf_len bytes of it,
    at any rate. If no LN can be found, then the OID (a->data) is written out
    into buf instead, in "##.##.##" format, where the ## are digits. If there
    is room, the string written into buf is null-termnated. The number of
    bytes written into buf is returned. */
int i2t_ASN1_OBJECT( char*buf, int buf_len, ASN1_OBJECT*a )
{
	uTrace( "TODO:  i2t_ASN1_OBJECT\n" );
	memcpy( buf, a, buf_len );
	return ( buf_len );
}

#endif // #if 0

# ifdef USE_DTLS
int SSL_do_handshake_Dtls( SSL*cp )
{
	int32 rc = -1, transferred = 0, len, rec_len = 0;
	ssl_t*ssl;
	unsigned char*buf;
	int32 val, weAreDone;
	long tSec;
	struct timeval timeout;
	fd_set readfd;
	socklen_t fromLen;
	struct sockaddr_in from;
	static int packet_loss_prob = 0; /* Reciprocal of packet loss probability
	                                    (i.e. P(packet loss) = 1/x).
	                                    Default value is 0 (no packet loss). */

	eeTrace( "Enter SSL_do_handshake_dtls\n" );
	memset( &from, 0x0, sizeof( struct sockaddr_in ) );
	ssl = cp->ssl;
	weAreDone = 0; /* resumed handshake exit hint mechanism */
	tSec = MIN_WAIT_SECS;
	timeout.tv_usec = 0;

WRITE_MORE:
	while ( ( len = matrixDtlsGetOutdata( ssl, &buf ) ) > 0 )
	{
		transferred = ( int32 ) sendto( cp->fd, buf, len, 0,
										( struct sockaddr* )( cp->ctx->addr ),
										sizeof( struct sockaddr_in ) );

		psTraceIntDtls( "\n transferred1 = %d", transferred );

		if ( transferred <= 0 )
		{
			psTraceInfo( "Socket send failed... exiting\n" );
			rc = transferred;
			cp->wouldblock = IS_WOULDBLOCK( transferred );
			goto L_CLOSE_ERR;
		}
		else
		{
			/* Always indicate the entire datagram was sent as there is no
			   way for DTLS to handle partial records.  Resends and timeouts will
			   handle any problems */
			if ( ( rc = matrixDtlsSentData( ssl, transferred ) ) < 0 )
			{
				cp->wouldblock = IS_WOULDBLOCK( transferred );
				goto L_CLOSE_ERR;
			}
			if ( rc == MATRIXSSL_REQUEST_CLOSE )
			{
				/* This is a 'graceful' exit so return success */
				//closeConn(dtlsCtx, fd);
				return ( MATRIXSSL_SUCCESS );
			}
			if ( rc == MATRIXSSL_HANDSHAKE_COMPLETE )
			{
				if ( ssl->outlen > 0 )
				{
					/* This is the resumed handshake endgame case and is a bit more
					   problematic than the standard case because we are the last
					   to send and we can't be sure the server received our FINISHED
					   message.  Let's handle this in our example by waiting another
					   timeout to see if the server is trying to resend */
					_psTrace( "Resumed handshake case. Pausing to check " );
					//printf("server received final FINISHED message\n");
					tSec = 4;
					timeout.tv_usec = 0;
					weAreDone = 1;
				}
				else
				{
					eeTrace( "Exit SSL_do_handshake_Dtls 2\n" );
					return ( 1 );
				}
			}
			if ( ( rc == MATRIXSSL_SUCCESS ) && ( weAreDone == 1 ) && ( ssl->outlen == 0 ) )
			{
				eeTrace( "Exit SSL_do_handshake_Dtls with return 1  3\n" );
				return ( 1 );
			}
			/* SSL_REQUEST_SEND and SSL_SUCCESS are handled by loop logic */
			/* SSL_REQUEST_SEND is handled by loop logic */
		}
	}
READ_MORE:
	if ( ( len = matrixSslGetReadbuf( ssl, &buf ) ) <= 0 )
	{
		rc = len;
		goto L_CLOSE_ERR;
	}

	/*	Select is the DTLS timeout/resend mechanism */
	FD_ZERO( &readfd );
	FD_SET( cp->fd, &readfd );
	timeout.tv_sec = tSec;

	if ( ( val = select( cp->fd + 1, &readfd, NULL, NULL, &timeout ) ) < 0 )
	{
		if ( SOCKET_ERRNO != EINTR )
		{
			psTraceIntDtls( "unhandled error %d from select", SOCKET_ERRNO );
			cp->wouldblock = IS_WOULDBLOCK( val );
			rc = val;
			goto L_CLOSE_ERR;
		}
		goto READ_MORE;
	}
	/* Check for timeout or select wake-up */
	if ( !FD_ISSET( cp->fd, &readfd ) )
	{
		/* Timed out.  For good? */
		psTraceIntDtls( "select timed out %d\n", val );
		if ( tSec == MAX_WAIT_SECS )
		{
			eeTrace( "Max Timeout.  Leaving\n" );
			cp->wouldblock = IS_WOULDBLOCK( val );
			goto L_CLOSE_ERR;
		}
		tSec *= 2;
		/* Our special exit case for resumed handshakes was simply to wait
		   and see if the server was resending its FINISHED message.  If not,
		   safe to assume we are connected.  Send app data and leave */
		if ( weAreDone == 1 )
		{
			return ( MATRIXSSL_SUCCESS );
		}
		/* Normal timeout case is to resend flight */
		goto WRITE_MORE;
	}

	/* Select woke.  Reset timeout */
	tSec = MIN_WAIT_SECS;

	//if ((len = matrixSslGetReadbuf(ssl, &buf)) <= 0) {
	//goto L_CLOSE_ERR;
	//}
	fromLen = sizeof( struct sockaddr_in );
	if ( ( rec_len =
			   ( int32 )recvfrom( cp->fd, buf, len, MSG_DONTWAIT, ( struct sockaddr* )&from,
								  &fromLen ) ) < 0 )
	{
		rc = rec_len;
		goto L_CLOSE_ERR;
	}

#ifdef USE_DTLS_DEBUG_TRACE
	/* nice for debugging */
	//printf("Read %d bytes from server\n", transferred);
#endif

	/*	If EOF, remote socket closed. But we haven't received the HTTP response
	    so we consider it an error in the case of an HTTP client */
	if ( rec_len == 0 )
	{
		rc = 0; /* Consider this a clean shutdown from client perspective */
		goto L_CLOSE_ERR;
	}
	if ( ( rc = matrixSslReceivedData( ssl, ( int32 )rec_len, &buf,
									   ( uint32* )&len ) ) < 0 )
	{
		cp->wouldblock = IS_WOULDBLOCK( rec_len );
		goto L_CLOSE_ERR;
	}

PROCESS_MORE:
	switch ( rc )
	{
		case MATRIXSSL_HANDSHAKE_COMPLETE:
			eeTrace( "Exit SSL_do_handshake_Dtls 4\n" );
			return ( 1 ); /* Normal handshake success */

		case MATRIXSSL_APP_DATA:
			if ( !( ssl->flags & SSL_FLAGS_SERVER ) )
			{
				/* Client should never see any app data during connection */
				psTraceInfo( "Got app data during handshake.. exiting\n" );
				rc = PS_UNSUPPORTED_FAIL;
				goto L_CLOSE_ERR;
			}
			/* Case of session resumption with immediate application data
			    tacked on the end of the client's FINISHED message */
			cp->resumedAppDataLen = len;
			cp->appRecLen = len;
#ifdef USE_TLS_1_1
			if ( ssl->flags & SSL_FLAGS_TLS_1_1 )
			{
				/* ReceivedData is giving good lengths and pointers of the
				    record itself but we need to add the explicit IV back
				    in beause we're processing this later */
				cp->outBufOffset = ssl->enBlockSize;
				cp->resumedAppDataLen += ssl->enBlockSize;
				cp->appRecLen += ssl->enBlockSize;
			}
#endif
			eeTrace( "Exit SSL_do_handshake_Dtls 5\n" );
			return ( 1 );

		case MATRIXSSL_REQUEST_SEND:
			goto WRITE_MORE;

		case MATRIXSSL_REQUEST_RECV:
			goto READ_MORE;

		case MATRIXSSL_RECEIVED_ALERT:
			/* The first byte of the buffer is the level */
			/* The second byte is the description */
			if ( *buf == SSL_ALERT_LEVEL_FATAL )
			{
				psTraceIntInfo( "Fatal alert: %d, closing connection.\n",
								*( buf + 1 ) );
				
				// With chrome update 73 we get certificate unknown error ( because we 
				// are using self signed certificate ) with each https request
				// causing server to send certificate on every request. Disabling this 
				// flag here would prevent deleting the session from the session table 
				// so that we can reuse the session for next connection.
				// for more info check LTK-3840 
				if( *( buf + 1 ) == SSL_ALERT_CERTIFICATE_UNKNOWN )
				{
					ssl->flags &= ~SSL_FLAGS_ERROR;
				}								
				rc = 0; /* Consider it a clean disconnect */
				goto L_CLOSE_ERR;
			}
			/* Closure alert is normal (and best) way to close */
			if ( *( buf + 1 ) == SSL_ALERT_CLOSE_NOTIFY )
			{
				rc = 0; /* A truly graceful disconnect */
				goto L_CLOSE_ERR;

			}
			psTraceIntInfo( "Warning alert: %d\n", *( buf + 1 ) );
			if ( ( rc = matrixSslProcessedData( ssl, &buf, ( uint32* )&len ) ) == 0 )
			{
				/* No more data in buffer. Might as well read for more. */
				goto READ_MORE;
			}
			goto PROCESS_MORE;

		default:
			/* If rc <= 0 we fall here */
			cp->wouldblock = IS_WOULDBLOCK( rc );
			goto L_CLOSE_ERR;
	}

L_CLOSE_ERR:
	eeTrace( "Exit SSL_do_handshake_dtls 6\n" );
	//LTK commented(if required, session will be closed in upper interfce)
	//matrixSslDeleteSession(ssl);
	//cp->ssl = NULL;
	return ( rc );
}

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
    Client side SSL handshake

    OpenSSL return codes:

    1 - The TLS/SSL handshake was successfully completed, a TLS/SSL connection
        has been established.
    0 - The TLS/SSL handshake was not successful but was shut down controlled
        and by the specifications of the TLS/SSL protocol. Call SSL_get_error()
        with the return value ret to find out the reason.
    <0 - The TLS/SSL handshake was not successful, because a fatal error
        occurred either at the protocol level or a connection failure occurred.
        The shutdown was not clean.
 */

int32 SSL_connect_Dtls( SSL*cp )
{
	int32 rc = -1;
	ssl_t*ssl;
	sslSessOpts_t options;
	int cipherSpec_Len = 0;
	const uint16* cipherSpec = NULL;

	tlsExtension_t* extensions = NULL;
	sslExtCb_t* Extcb = NULL;
	sslCertCb_t* Certcb = NULL;

	eeTrace( "Enter SSL_connect\n" );
	if ( cp->ssl != NULL )
	{
		if ( matrixSslHandshakeIsComplete( cp->ssl ) )
		{
			if ( matrixSslEncodeRehandshake( cp->ssl, NULL, NULL, 0, 0, 0 ) < 0 )
			{
				psTraceInfo( "matrixSslEncodeRehandshake failed\n" );
				return ( -1 );
			}
		}
		else
		{
			psTraceInfo( "ERROR: SSL_connect called on existing SSL\n" );
			return ( -1 );
		}
	}
	else
	{
		memset( &options, 0x0, sizeof( sslSessOpts_t ) );
		rtk_set_ciphers_and_options( cp, &cipherSpec, &cipherSpec_Len, &Certcb, &extensions, &Extcb,
									 &( cp->resume ), &options );
		options.versionFlag |= SSL_FLAGS_DTLS;
		options.trustedCAindication = 1;

		//ssgopenssl start
		rc = matrixSslNewClientSession( &ssl, cp->ctx->keys, cp->resume, cipherSpec, cipherSpec_Len,
										( sslCertCb_t )Certcb, cp->host, extensions,
										( sslExtCb_t )Extcb, &options );
		if ( extensions != NULL )
		{
			matrixSslDeleteHelloExtension( extensions );
		}
		if ( rc != MATRIXSSL_REQUEST_SEND )
		{
			psTraceInfo( "New client session failed... exiting\n" );
			return ( 0 );
		}
		cp->ssl = ssl;
	}

	//if ((rc = SSL_do_handshake_Dtls(cp)) <= 0)
	//{
	//cp->ssl = NULL;
	//}
	eeTrace( "Exit SSL_connect\n" );
	return ( rc );

}

#endif



/******************************************************************************/
/*
    OpenSSL return codes

    >0	The read operation was successful; the return value is the number
        of bytes actually read from the TLS/SSL connection.

    0	The read operation was not successful. The reason may either be a clean
        shutdown due to a ``close notify'' alert sent by the peer. It is also
        possible, that the peer simply shut down the underlying transport and
        the shutdown is incomplete.

   <0	The read operation was not successful, because either an error occurred
        or action must be taken by the calling process.
 */
int32 SSL_read_Dtls( SSL*cp, void*userBufPtr, int userBufLen )
{
	ssl_t* ssl;
	char_t*readBuf, *ptBuf, *userBuf;
	int32 leftover, bytes, rc = -1;
	uint32 nextRecLen, ptBufLen;
	int32 val;
	uint32 tSec;
	struct timeval timeout;
	fd_set readfd;
	socklen_t fromLen;
	struct sockaddr_in from;
	uint32_t freeBufLen, recvLen;

	memset( &from, 0x0, sizeof( struct sockaddr_in ) );
	ssl = cp->ssl;
	tSec = MIN_WAIT_SECS;
	timeout.tv_usec = 0;
	userBuf = ( char_t* )userBufPtr;
#ifdef USE_DTLS_DEBUG_TRACE
	eeTrace( "Enter SSL_read_Dtls\n" );
#endif

	/*
	    There are a few ways data might already be available to read for a
	    server here.  Two are triggered on whether there is any length for
	    incoming data.
	 */
	if ( cp->resumedAppDataLen )
	{
		/* This case is when a session resumption handshake takes place
		    and the client sends application data in the same flight as
		    the FINISHED message.  In this case, the application data has
		    already been decrypted and is just waiting to be picked off.
		    The ssl->inlen > 0 indicates the is also more unencrypted
		    data after this decrypted record.  Seen to happen in TLS 1.0
		    when 1 byte BEAST workaround record is sent */
		cp->appRecLen = cp->resumedAppDataLen;
		cp->outBufOffset = 0;
		cp->resumedAppDataLen = 0;
	}
	else if ( ssl->inlen > 0 )
	{
		/* This case is the Chrome false start where application data has
		    been sent in the middle of the handshake.  MatrixSSL has not
		    yet decrypted the data in this case so we do that now. */
		rc = matrixSslReceivedData( ssl, 0, &ptBuf, &ptBufLen );
		goto PROCESS_MORE;
	}

	/*
	    Always send any remainder first
	 */
	leftover = cp->appRecLen - cp->outBufOffset;
	if ( leftover > 0 )
	{
		bytes = ( int32 )min( userBufLen, leftover );
		memcpy( userBuf, ssl->inbuf + ssl->deBlockSize + cp->outBufOffset, bytes );
		cp->outBufOffset += bytes;
		if ( cp->outBufOffset == cp->appRecLen )
		{
			cp->outBufOffset = cp->appRecLen = 0;
			SSL_processed_data( cp, &ptBuf, ( int* )&ptBufLen );
			if ( ptBufLen > 0 )
			{
				/* Make this the next 'leftover' */
				cp->appRecLen = ptBufLen;
			}
		}
		eeTrace( "Exit SSL_read_Dtls 1\n" );
		return ( bytes );
	}

READ_MORE:

	/*	Select is the DTLS timeout/resend mechanism */
	FD_ZERO( &readfd );
	FD_SET( cp->fd, &readfd );
	timeout.tv_sec = tSec;

	if ( ( val = select( cp->fd + 1, &readfd, NULL, NULL, &timeout ) ) < 0 )
	{
		if ( SOCKET_ERRNO != EINTR )
		{
			psTraceIntDtls( "unhandled error %d from select", SOCKET_ERRNO );
			goto L_CLOSE_ERR;
		}
		goto READ_MORE;
	}
	/* Check for timeout or select wake-up */
	if ( !FD_ISSET( cp->fd, &readfd ) )
	{
		/* Timed out.  For good? */
#ifdef USE_DTLS_DEBUG_TRACE
		psTraceDtls( "select timed out %d\n", val );
#endif
		if ( tSec == MAX_WAIT_SECS )
		{
#ifdef USE_DTLS_DEBUG_TRACE
			psTraceDtls( "Max Timeout.  Leaving\n" );
#endif
			//goto L_CLOSE_ERR;
			return ( 0 );
		}
		tSec *= 2;

		/* Normal timeout case is to resend flight */
		goto READ_MORE;
	}
	/* Select woke.  Reset timeout */
	tSec = MIN_WAIT_SECS;

	if ( ( freeBufLen = matrixSslGetReadbuf( ssl, &readBuf ) ) <= 0 )
	{
		return ( freeBufLen );
	}
	fromLen = sizeof( struct sockaddr_in );
	if ( ( recvLen = ( int32 )recvfrom( cp->fd, readBuf, freeBufLen, MSG_DONTWAIT,
										( struct sockaddr* )&from, &fromLen ) ) < 0 )
	{
		psTraceInfo( "Socket recv failed... exiting\n" );
		cp->wouldblock = IS_WOULDBLOCK( recvLen );
		goto L_CLOSE_ERR;
	}
#ifdef USE_DTLS_DEBUG_TRACE
	/* nice for debugging */
	psTraceDtls( "Read %d bytes from server\n", recvLen );
	psTraceDtls( "Encoded Meassage : %s\n", readBuf );

#endif

	/*	Notify SSL state machine that we've received more data into the
	   ssl buffer retreived with matrixSslGetReadbuf. */
	if ( ( rc = matrixSslReceivedData( ssl, recvLen, &ptBuf, ( uint32* )&ptBufLen ) ) < 0 )
	{
		psTraceInfo( "matrixSslReceivedData failure3... exiting\n" );
		return ( rc );
	}
#ifdef USE_DTLS_DEBUG_TRACE
	psTraceDtls( "Decoded Meassage: %s\n", ptBuf );
#endif
	/*	If EOF, remote socket closed. But we haven't received the HTTP response
	    so we consider it an error in the case of an HTTP client */

PROCESS_MORE:
	switch ( rc )
	{
		case MATRIXSSL_APP_DATA:
			if ( ptBufLen == 0 )
			{
				/* Eat any zero length records that come across. OpenSSL server
				   is likely sending some */
				rc = matrixSslProcessedData( ssl, &ptBuf, &ptBufLen );
				goto PROCESS_MORE;
			}
			/* Always a full app data record here but need to know if user
			    can accept the whole thing in one pass */
			if ( ptBufLen <= userBufLen )
			{
				memcpy( userBuf, ptBuf, ptBufLen );
				/* Have a full record so update that it's processed */
				SSL_processed_data( cp, &ptBuf, ( int* )&nextRecLen );

#ifdef USE_REHANDSHAKING
				if ( cp->ssl->hsState != SSL_HS_DONE )
				{
					cp->rehandshake = 1;
				}
#endif
				if ( nextRecLen > 0 )
				{
					cp->appRecLen = nextRecLen;
				}
				eeTrace( "Exit SSL_read_Dtls 3\n" );
				return ( ptBufLen );
			}
			else
			{
				cp->appRecLen = ptBufLen;
				memcpy( userBuf, ptBuf, userBufLen );
				cp->outBufOffset += userBufLen;
				eeTrace( "Exit SSL_read_Dtls 4\n" );
				return ( userBufLen );
			}

		case MATRIXSSL_REQUEST_SEND:
			//goto WRITE_MORE;
			/* If the library is telling us there is data to send, this must
			    be a re-handshake. (this could also be an alert that is being
			    sent but SSL_do_handshake_Dtls will handle this scenario as well) */
#ifdef USE_REHANDSHAKING
			cp->rehandshake = 1;
			return ( -1 );

#else
			if ( SSL_do_handshake_Dtls( cp ) <= 0 )
			{
				return ( -1 );
			}
#endif
			goto READ_MORE;

		case MATRIXSSL_REQUEST_RECV:
			goto READ_MORE;

		case MATRIXSSL_RECEIVED_ALERT:
			/* The first byte of the buffer is the level */
			/* The second byte is the description */
			if ( *ptBuf == SSL_ALERT_LEVEL_FATAL )
			{
				psTraceIntInfo( "Fatal alert: %d, closing connection.\n",
								*( ptBuf + 1 ) );
				eeTrace( "Exit SSL_read_Dtls 5\n" );
				return ( 0 ); /* Actually a clean disconnect */
			}
			/* Closure alert is normal (and best) way to close */
			if ( *( ptBuf + 1 ) == SSL_ALERT_CLOSE_NOTIFY )
			{
				//closeConn(dtlsCtx, fd);
				eeTrace( "Exit SSL_read 6\n" );
				return ( MATRIXSSL_SUCCESS );/* Graceful disconnect */
			}
			psTraceIntInfo( "Warning alert: %d\n", *( ptBuf + 1 ) );
			if ( ( rc = matrixSslProcessedData( ssl, &ptBuf, ( uint32* )&ptBufLen ) ) == 0 )
			{
				/* No more data in buffer. Might as well read for more. */
				goto READ_MORE;
			}
			goto PROCESS_MORE;

		default:
			/* MATRIXSSL_HANDSHAKE_COMPLETE */
			/* Will never be hit due to rc < 0 test at ReceivedData level */
			/* If rc <= 0 we fall here */
			//goto L_CLOSE_ERR;
			cp->wouldblock = IS_WOULDBLOCK( rc );
			return ( -1 );
	}

L_CLOSE_ERR:
	eeTrace( "Exit SSL_read_Dtls 7\n" );
	return ( -1 ); /* How to get here? */
}

/******************************************************************************/
/*
    OpenSSL return codes

    >0	The write operation was successful, the return value is the number of
        bytes actually written to the TLS/SSL connection.
    0	The write operation was not successful. Probably the underlying
        connection was closed
    <0	The write operation was not successful, because either an error
        occurred or action must be taken by the calling process.
 */
int32 SSL_write_Dtls( SSL*cp, const void*inbufPtr, int32 inlen )
{
	ssl_t*ssl;
	unsigned char*buf;
	int transferred;
	const char*inbuf;
	int32 len, bytesSent = 0;

	eeTrace( "Enter SSL_write_Dtls\n" );
	inbuf = ( char* )inbufPtr;
	ssl = cp->ssl;

	if ( inbuf != NULL )
	{ /* A NULL inbuf means internal buffer needs to flush */
		/* Test if inbuf falls without our buffer to determine
		 what option they have choosen */
		if ( ( ( int32 )inbuf >= ( int32 )ssl->outbuf ) &&
			 ( ( int32 )inbuf < ( int32 ) ( ssl->outbuf + ssl->outsize ) ) )
		{
			matrixSslEncodeWritebuf( ssl, inlen );

		}
		else if ( matrixSslEncodeToOutdata( ssl, ( unsigned char* )inbuf, inlen ) < 0 )
		{
			return ( -1 ); //LTK modified, return -1 if failed in buffer allocation or encoding
		}
		else
		{}
	}

	while ( ( len = matrixDtlsGetOutdata( ssl, &buf ) ) > 0 )
	{
		//LTK modified,
		if ( inbuf != NULL )
		{
			//len = (len / TCP_MSS) * TCP_MSS;
		}

		if ( len )
		{
			transferred = ( int32 ) sendto( cp->fd, buf, len, 0,
											( struct sockaddr* )( cp->ctx->addr ),
											sizeof( struct sockaddr_in ) );
			if ( transferred <= 0 )
			{
				psTraceInfo( "Socket send failed... exiting\n" );
				cp->wouldblock = IS_WOULDBLOCK( transferred );
				return ( transferred );
			}
			else
			{
				bytesSent += transferred;
				/* Indicate that we've written > 0 bytes of data */
				if ( matrixDtlsSentData( ssl, transferred ) < 0 )
				{
					return ( -1 );
				}
				/* SSL_REQUEST_SEND and SSL_SUCCESS are handled by loop logic */
			}
		}
		else
		{
			return ( 1 );
		}
	}
	eeTrace( "Exit SSL_write\n" );
	return ( bytesSent );
}
#endif

