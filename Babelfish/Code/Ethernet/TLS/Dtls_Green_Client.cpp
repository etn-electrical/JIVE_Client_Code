/**
 *	@file    client.c
 *	@version a90e925 (tag: 3-8-3-open)
 *
 *	Simple MatrixSSL blocking client example.
 */



#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
// #include "app.h"
#include "OS_Tasker.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "Eth_if.h"
#include "ssl_connection.h"
#include "matrixssl/matrixsslApi.h"
#include "opensslApi.h"
#include "Device_Cert_ECC.h"
#include "Device_Cert.h"
#include "lwip/sys.h"
#include "Dtls_Green_Client.h"


static enum load_key_type m_dtls_key_type = KEY_ECC_LOAD;

#ifdef __cplusplus
extern "C" {
#endif

extern void dtls_main( void );

#ifdef __cplusplus
}
#endif

#ifdef PX_GREEN_DTLS_SUPPORT
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
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

#define MIN_WAIT_SECS 10
#define MAX_WAIT_SECS 60

#define HELLO_MESSAGE "Hello, I am DTLS Client powered by PX Green"

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
#if 0
static char_t connect_ip[16] = "192.168.1.6";
static uint32_t connect_port = 28000;
static int packet_loss_prob = 0;/* Reciprocal of packet loss probability
								   (i.e. P(packet loss) = 1/x).
								   Default value is 0 (no packet loss). */

static SSL_CTX* m_ctx = nullptr;
static SSL* dtls_obj = nullptr;
static sslSessionId_t* sid = nullptr;
#endif

uint8_t in_buff[256];
uint32_t in_length_size = sizeof( in_buff );

/*
 *****************************************************************************************
 *		Externs
 *****************************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif
extern void rtk_set_ciphers_and_options( SSL* cp, uint16_t const** ciphers, int* cipherSpec_Len,
										 sslCertCb_t** Cert_cb, tlsExtension_t** extensions,
										 sslExtCb_t** extensionCback,
										 sslSessionId_t** resume, sslSessOpts_t* options );

#ifdef __cplusplus
}
#endif


/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
bool Ssl_Key_Init( SSL_CTX* ctx, const unsigned char* certBuf, int32_t certLen,
				   const unsigned char* privBuf, int32_t privLen,
				   const unsigned char* CAbuf, int32_t CAlen );

#if 0
static void Dtls_Green_Client_Task( void* pParams );

static bool Dtls_Socket_Init( SSL* dtls, uint32_t port, char_t* ip );

static bool Dtls_Ssl_Init( SSL* dtls );

static void Dtls_Connect( SSL* dtls );

static int32_t Dtls_Send( SSL* dtls, const void* inbufPtr, int32 inlen );

static int32_t Dtls_Handshake( SSL* dtls );

static int32_t Dtls_Receive( SSL* dtls, void* userBufPtr, int userBufLen );

void Dtls_Close( SSL* dtls );

static int32 udpSend( SOCKET s, unsigned char* buf, int len,
					  const struct sockaddr* to, int tolen, int flags,
					  int packet_loss_prob, int* drop_rehandshake_cipher_spec );

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ssl_Key_Init( SSL_CTX* ctx, const unsigned char* certBuf, int32_t certLen,
				   const unsigned char* privBuf, int32_t privLen,
				   const unsigned char* CAbuf, int32_t CAlen )
{
	int32_t status = PS_SUCCESS_T;
	int32_t ret = false;

	if ( ctx != nullptr )
	{
		if ( matrixSslNewKeys( &ctx->keys, NULL ) < 0 )
		{
			psTraceInfo( "matrixSslNewKeys error in SSL_CTX_new\n" );
		}
		else
		{
			if ( m_dtls_key_type == KEY_RSA_LOAD )
			{
				status = SSL_CTX_Load_Key_Material_RSA( ctx, certBuf, certLen, privBuf, privLen, CAbuf, CAlen );
			}
			else
			{
				status = SSL_CTX_Load_Key_Material_ECC( ctx, certBuf, certLen, privBuf, privLen, CAbuf, CAlen );
			}
			if ( status != PS_SUCCESS_T )
			{
				psTraceInfo( "SSL_CTX_Load_Key_Material error in key loading\n" );
			}
			else
			{
				ret = true;
			}
		}
	}
	return ( ret );
}

#if 0
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Dtls_Client_Init( enum load_key_type key_type )
{
	m_dtls_key_type = key_type;
	bool init_success = true;

#if 0
	// Create task using "sys_thread_new" to use timeout functionality of lwip/core/sys.c
	if ( sys_thread_new( "PX_Green_DTLS_Client", Dtls_Green_Client_Task, NULL,
						 2000, OS_TASK_PRIORITY_2 ) == NULL )
	{
		init_success = false;
	}
#endif
	return ( init_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void Dtls_Green_Client_Task( void* pParams )
{
	bool status = false;

	Eth_if::Wait_Till_Ip_Configured();

	Device_Cert_Info_t* cert = nullptr;

	if ( m_dtls_key_type == KEY_RSA_LOAD )
	{
		Wait_For_Certificate_Validation_RSA();
		cert = ( Device_Cert_Info_t* )Get_NV_Device_Certificate_RSA();
	}
	else
	{
		Wait_For_Certificate_Validation_ECC();
		cert = ( Device_Cert_Info_t* )Get_NV_Device_Certificate_ECC();
	}

	if ( cert != nullptr )
	{
		// SSL_library_init();

		m_ctx = SSL_CTX_new( nullptr );

		status = Ssl_Key_Init( m_ctx, cert->ss_cert,
							   cert->ss_cert_len,
							   cert->priv_key,
							   cert->priv_key_len,
							   cert->ss_cert,
							   cert->ss_cert_len );

		if ( status == true )
		{
			dtls_obj = reinterpret_cast<SSL*>( psMalloc( NULL, sizeof( SSL ) ) );
			memset( dtls_obj, 0x0, sizeof( SSL ) );

			if ( dtls_obj != nullptr )
			{
				dtls_obj->ctx = m_ctx;

				status = Dtls_Socket_Init( dtls_obj, connect_port, &connect_ip[0] );

				if ( status == true )
				{
					status = Dtls_Ssl_Init( dtls_obj );

					do
					{
						int32_t rc = Dtls_Handshake( dtls_obj );

						if ( rc >= 1 )
						{
							int32_t read = 1;
							int32_t send = 0;
							while ( 1 )
							{
								OS_Tasker::Delay( 200 );
								if ( read > 0 )
								{
									send = Dtls_Send( dtls_obj, HELLO_MESSAGE, strlen( HELLO_MESSAGE ) );

									printf( "Client To Server: %s\n", HELLO_MESSAGE );
								}
								read = Dtls_Receive( dtls_obj, in_buff, in_length_size );

								if ( read > 1 )
								{
									printf( "Server To Client: %s\n", in_buff );
								}
							}
							Dtls_Close( dtls_obj );
						}
						else
						{
							OS_Tasker::Delay( 200 );
						}
					} while( status == false );
				}
			}
		}
	}
	while ( 1 )
	{
		OS_Tasker::Delay( 200 );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Dtls_Socket_Init( SSL* dtls, uint32_t port, char_t* ip )
{
	bool return_status = false;
	SOCKET fd;
	char port_buf[10] = { '\0' };

	fd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if ( fd != INVALID_SOCKET )
	{
		if ( dtls != nullptr )
		{
			dtls->fd = fd;
			memset( &dtls->addr, 0x0, sizeof( struct sockaddr_in ) );
			dtls->addr.sin_family = AF_INET;
			dtls->addr.sin_port = htons( ( short )port );
			dtls->addr.sin_addr.s_addr = inet_addr( ip );

			struct addrinfo addrHint = { 0 };
			struct addrinfo* addrInfo = NULL;

			memset( ( char* )&addrHint, 0x0, sizeof ( addrHint ) );

			addrHint.ai_family = AF_INET;
			addrHint.ai_socktype = SOCK_DGRAM;
			addrHint.ai_protocol = IPPROTO_UDP;

			dtls->host = ip;

			sprintf( port_buf, "%d", port );
			int err = lwip_getaddrinfo( ip, port_buf, &addrHint, &addrInfo );

			if ( err == NULL )
			{

				int rc = 1;
				setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, ( char* )&rc, sizeof ( rc ) );
				setSocketNodelay( fd );

				if ( connect( dtls->fd, addrInfo->ai_addr, addrInfo->ai_addrlen ) < 0 )
				{
					err = SOCKET_ERRNO;
					close( dtls->fd );
					dtls->fd = INVALID_SOCKET;
				}
				else
				{
					// MISRA coding guidelines
					return_status = true;
					dtls->host = ip;
				}
				lwip_freeaddrinfo( addrInfo );
			}

		}
		else
		{
			_psTrace( "Error allocating dtls struct\n" );
		}
	}
	else
	{
		_psTrace( "Error creating INET UDP socket\n" );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Dtls_Ssl_Init( SSL* dtls )
{
	bool status = false;

	const uint16* cipherSpec = NULL;
	int cipherSpec_Len = 0;
	tlsExtension_t* extensions = NULL;
	sslExtCb_t* Extcb = NULL;
	sslCertCb_t* Certcb = NULL;
	ssl_t* ssl;
	sslSessOpts_t options;

	memset( &options, 0x0, sizeof( sslSessOpts_t ) );

	// options.trustedCAindication = 1;

	rtk_set_ciphers_and_options( dtls, &cipherSpec, &cipherSpec_Len, &Certcb, &extensions, &Extcb,
								 &( dtls->resume ), &options );
	options.versionFlag |= SSL_FLAGS_DTLS;
	options.trustedCAindication = 1;
	// ssgopenssl start
	int32_t rc = matrixSslNewClientSession( &ssl, dtls->ctx->keys, dtls->resume, cipherSpec,
											cipherSpec_Len,
											( sslCertCb_t )Certcb, dtls->host, extensions,
											( sslExtCb_t )Extcb, &options );

	if ( extensions != NULL )
	{
		matrixSslDeleteHelloExtension( extensions );
	}
	if ( rc != MATRIXSSL_REQUEST_SEND )
	{
		psTraceInfo( "New client session failed... exiting\n" );
	}
	else
	{
		dtls->ssl = ssl;
		status = true;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Dtls_Connect( SSL* dtls )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Dtls_Handshake( SSL* dtls )
{
	int32 rc, transferred, len, done, val;
	ssl_t* ssl = dtls->ssl;
	unsigned char* buf;
	struct timeval timeout;
	long tSec = MIN_WAIT_SECS;
	struct sockaddr_in from;
	socklen_t fromLen;
	fd_set readfd;

	/*
	    matrixDtlsGetOutdata will always return data to send.  If a new outgoing
	    flight is not waiting to be send a resend of the previous flight will be
	    returned.
	 */
WRITE_MORE:
	while ( ( len = matrixDtlsGetOutdata( ssl, &buf ) ) > 0 )
	{
		transferred = udpSend( dtls->fd, buf, len, ( struct sockaddr* ) &dtls->addr,
							   sizeof( dtls->addr ), ( int ) tSec,
							   packet_loss_prob, NULL );

		if ( transferred <= 0 )
		{
			goto L_CLOSE_ERR;
		}
		else
		{
			/* Always indicate the entire datagram was sent as there is no
			   way for DTLS to handle partial records.  Resends and timeouts will
			   handle any problems */
			if ( ( rc = matrixDtlsSentData( ssl, len ) ) < 0 )
			{
				goto L_CLOSE_ERR;
			}
			if ( rc == MATRIXSSL_REQUEST_CLOSE )
			{
				/* This is a 'graceful' exit so return success */
				return ( -1 );
			}
			if ( rc == MATRIXSSL_HANDSHAKE_COMPLETE )
			{
				_psTrace( "handshake complete\n" );
				return ( 1 );
			}
			/* SSL_REQUEST_SEND is handled by loop logic */
		}
	}

READ_MORE:
	/*  Select is the DTLS timeout/resend mechanism */
	FD_ZERO( &readfd );
	FD_SET( dtls->fd, &readfd );
	timeout.tv_sec = tSec;

	if ( ( val = select( dtls->fd + 1, &readfd, NULL, NULL, &timeout ) ) < 0 )
	{
		if ( SOCKET_ERRNO != EINTR )
		{
			psTraceIntDtls( "unhandled error %d from select", SOCKET_ERRNO );
			goto L_CLOSE_ERR;
		}
		goto READ_MORE;
	}
	/* Check for timeout or select wake-up */
	if ( !FD_ISSET( dtls->fd, &readfd ) )
	{
		/* Timed out.  For good? */
		psTraceIntDtls( "select timed out %d\n", val );
		if ( tSec == MAX_WAIT_SECS )
		{
			psTraceDtls( "Max Timeout.  Leaving\n" );
			goto L_CLOSE_ERR;
		}
		tSec *= 2;

		/* Normal timeout case is to resend flight */
		goto WRITE_MORE;
	}

	/* Select woke.  Reset timeout */
	tSec = MIN_WAIT_SECS;

	if ( ( len = matrixSslGetReadbuf( ssl, &buf ) ) <= 0 )
	{
		goto L_CLOSE_ERR;
	}
	fromLen = sizeof( struct sockaddr_in );
	if ( ( transferred = ( int32 ) recvfrom( dtls->fd, buf, len, MSG_DONTWAIT,
											 ( struct sockaddr* ) &from, &fromLen ) ) < 0 )
	{
		goto L_CLOSE_ERR;
	}

	# ifdef USE_DTLS_DEBUG_TRACE
	/* nice for debugging */
	psTraceIntDtls( "Read %d bytes from server\n", transferred );
	# endif

	/*  If EOF, remote socket closed. But we haven't received the HTTP response
	    so we consider it an error in the case of an HTTP client */
	if ( transferred == 0 )
	{
		goto L_CLOSE_ERR;
	}
	if ( ( rc = matrixSslReceivedData( ssl, ( int32 ) transferred, &buf,
									   ( uint32* ) &len ) ) < 0 )
	{
		goto L_CLOSE_ERR;
	}

PROCESS_MORE:
	switch ( rc )
	{
		case MATRIXSSL_HANDSHAKE_COMPLETE:
			return ( 1 );// MATRIXSSL_SUCCESS;

		case MATRIXSSL_APP_DATA:
			/*
			            Would process app data here
			 */
			return ( -123 );

			if ( ( rc = matrixSslProcessedData( ssl, &buf, ( uint32* ) &len ) ) == 0 )
			{
				goto READ_MORE;
			}
			goto PROCESS_MORE;

		case MATRIXSSL_REQUEST_SEND:
			goto WRITE_MORE;

		case MATRIXSSL_REQUEST_RECV:
			goto READ_MORE;

		case MATRIXSSL_RECEIVED_ALERT:
			/* The first byte of the buffer is the level
			   The second byte is the description*/
			if ( *buf == SSL_ALERT_LEVEL_FATAL )
			{
				psTraceIntInfo( "Fatal alert: %d, closing connection.\n",
								*( buf + 1 ) );
				goto L_CLOSE_ERR;
			}
			/* Closure alert is normal (and best) way to close */
			if ( *( buf + 1 ) == SSL_ALERT_CLOSE_NOTIFY )
			{
				// closeConn(dtlsCtx, fd);
				return ( -1 );	// MATRIXSSL_SUCCESS;
			}
			psTraceIntInfo( "Warning alert: %d\n", *( buf + 1 ) );
			if ( ( rc = matrixSslProcessedData( ssl, &buf, ( uint32* ) &len ) ) == 0 )
			{
				/* No more data in buffer. Might as well read for more. */
				goto READ_MORE;
			}
			goto PROCESS_MORE;

		default:
			/* If rc <= 0 we fall here */
			goto L_CLOSE_ERR;
	}

L_CLOSE_ERR:
	_psTrace( "DTLS Connection FAIL.  Exiting\n" );
	// closeConn(dtlsCtx, fd);
	return ( MATRIXSSL_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Dtls_Send( SSL* dtls, const void* inbufPtr, int32 inlen )
{
	ssl_t* ssl;
	unsigned char* buf;
	int transferred;
	const char* inbuf;
	int32 len, bytesSent = 0;

	eeTrace( "Enter SSL_write\n" );
	inbuf = ( char* )inbufPtr;
	ssl = dtls->ssl;

	if ( inbuf != NULL )
	{	/* A NULL inbuf means internal buffer needs to flush */
		/* Test if inbuf falls without our buffer to determine
		   what option they have choosen */
		if ( ( ( int32 )inbuf >= ( int32 )ssl->outbuf ) &&
			 ( ( int32 )inbuf < ( int32 ) ( ssl->outbuf + ssl->outsize ) ) )
		{
			matrixSslEncodeWritebuf( ssl, inlen );

		}
		else if ( matrixSslEncodeToOutdata( ssl, ( unsigned char* )inbuf, inlen ) < 0 )
		{
			return ( -1 );	// LTK modified, return -1 if failed in buffer allocation or encoding
		}

	}

	while ( ( len = matrixDtlsGetOutdata( ssl, &buf ) ) > 0 )// Sada
	{
		// LTK modified,
		if ( inbuf != NULL )
		{
			// len = (len / TCP_MSS) * TCP_MSS;
		}

		if ( len )
		{
			transferred =
				( int32 ) sendto( dtls->fd, buf, len, 0, ( struct sockaddr* )&dtls->addr, sizeof( dtls->addr ) );

			if ( transferred <= 0 )
			{
				psTraceInfo( "Socket send failed... exiting\n" );
				dtls_obj->wouldblock = IS_WOULDBLOCK( transferred )
				;
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Dtls_Receive( SSL* dtls, void* userBufPtr, int userBufLen )
{

	ssl_t* ssl;
	unsigned char* readBuf, * ptBuf, * userBuf;
	int32 readLen, leftover, bytes, rc;
	uint32 nextRecLen, ptBufLen;
	int transferred;
	int32 val, weAreDone;
	long tSec;
	struct timeval timeout;
	fd_set readfd;
	int16 rehandshakeTestDone, resumedRehandshakeTestDone;
	socklen_t fromLen;
	struct sockaddr_in from;
	static int g_port, g_new, g_resumed, g_ciphers;
	int32 read_len = 0;	// len, done;

	// unsigned char*buf;


	eeTrace( "Enter SSL_read\n" );
	ssl = dtls->ssl;
	SSL* cp = dtls;

	userBuf = ( unsigned char* )userBufPtr;

	/*
	    There are a few ways data might already be available to read for a
	    server here.  Two are triggered on whether there is any length for
	    incoming data.
	 */
	if ( ssl->inlen > 0 )
	{
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
		else
		{
			/* This case is the Chrome false start where application data has
			    been sent in the middle of the handshake.  MatrixSSL has not
			    yet decrypted the data in this case so we do that now. */
			rc = matrixSslReceivedData( ssl, 0, &ptBuf, &ptBufLen );
			goto PROCESS_MORE;
		}
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


	/*	Select is the DTLS timeout/resend mechanism */
	FD_ZERO( &readfd );
	FD_SET( cp->fd, &readfd );
	timeout.tv_sec = tSec;

	if ( ( val = select( cp->fd + 1, &readfd, NULL, NULL, &timeout ) ) < 0 )
	{
		if ( SOCKET_ERRNO != EINTR )
		{
			psTraceIntDtls( "unhandled error %d from select", SOCKET_ERRNO );
			return ( -1 );
		}
		return ( 0 );
		// goto READ_MORE;
	}
	/* Check for timeout or select wake-up */
	if ( !FD_ISSET( cp->fd, &readfd ) )
	{
		/* Timed out.  For good? */
		psTraceIntDtls( "select timed out %d\n", val );
		if ( tSec == MAX_WAIT_SECS )
		{
			psTraceDtls( "Max Timeout.  Leaving\n" );
			return ( -1 );
		}
		tSec *= 2;
		/* Normal timeout case is to resend flight */
		// goto WRITE_MORE;
	}

	/* Select woke.  Reset timeout */
	tSec = MIN_WAIT_SECS;

	if ( ( read_len = matrixSslGetReadbuf( ssl, &readBuf ) ) <= 0 )
	{
		return ( read_len );
	}
	fromLen = sizeof( struct sockaddr_in );
	if ( ( transferred = ( int32 )recvfrom( dtls->fd, readBuf, read_len, MSG_DONTWAIT,
											( struct sockaddr* )&from, &fromLen ) ) < 0 )
	{
		if ( SOCKET_ERRNO = EWOULDBLOCK )
		{
			return ( 0 );
		}
		else
		{
			return ( -1 );
		}
	}

#ifdef USE_DTLS_DEBUG_TRACE
	/* nice for debugging */
	psTraceIntDtls( "Read %d bytes from server\n", transferred );
#endif

	/*	If EOF, remote socket closed. But we haven't received the HTTP response
	    so we consider it an error in the case of an HTTP client */
	if ( transferred == 0 )
	{
		psTraceInfo( "Server disconnected\n" );
		eeTrace( "Exit SSL_read 2\n" );
		return ( 0 );	/* Consider this a clean shutdown from client perspective */
	}
	if ( ( rc = matrixSslReceivedData( ssl, ( int32 )transferred, &ptBuf,
									   ( uint32* )&ptBufLen ) ) < 0 )
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
			/* The first byte of the buffer is the level
			   The second byte is the description*/
			if ( *ptBuf == SSL_ALERT_LEVEL_FATAL )
			{
				psTraceIntInfo( "Fatal SSL alert: %d, closing connection\n",
								*( ptBuf + 1 ) );
				eeTrace( "Exit SSL_read 5\n" );
				return ( 0 );	/* Actually a clean disconnect */
			}
			/* Closure alert is normal (and best) way to close */
			if ( *( ptBuf + 1 ) == SSL_ALERT_CLOSE_NOTIFY )
			{
				eeTrace( "Exit SSL_read 6\n" );
				return ( 0 );	/* Graceful disconnect */
			}
			psTraceIntInfo( "Warning alert: %d\n", *( ptBuf + 1 ) );
			if ( ( rc = matrixSslProcessedData( ssl, &ptBuf, &ptBufLen ) ) == 0 )
			{
				/* No more data in buffer. Might as well read for more. */
				goto READ_MORE;
			}
			goto PROCESS_MORE;

		default:
			/* MATRIXSSL_HANDSHAKE_COMPLETE
			   Will never be hit due to rc < 0 test at ReceivedData level*/
			return ( -1 );
	}

L_CLOSE_ERR:
	eeTrace( "Exit SSL_do_handshake_dtls 6\n" );
	return ( -1 );	/* How to get here? */
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Dtls_Close( SSL* dtls )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32 udpSend( SOCKET s, unsigned char* buf, int len,
			   const struct sockaddr* to, int tolen, int flags,
			   int packet_loss_prob, int* drop_rehandshake_cipher_spec )
{
	int32 sent;

	if ( ( sent = ( int32 ) sendto( s, buf, len, 0, to, tolen ) ) < 0 )
	{
		return ( -1 );
	}
	psTraceIntDtls( "Sent %d bytes\n", sent );

	return ( len );
}

#endif

#endif
