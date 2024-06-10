/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2011-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/

/*
 *
 *  This file contains
 *  Cryptography Definitions.
 *
  * This file should be verified and/or modified for your target.
 */
#ifndef _TLSVEND_H
#define _TLSVEND_H

#include "tmwtarg.h"

#ifdef TMW_USE_SECURITY

/* User configurable data */

/* Maximum common name string lengths in configuration data structure */
#define TMW_MAX_COMMON_NAME                     1024

#define TMW_CRYPTO_ID_LEN                         1024

/* Time (in milliseconds) that servers should wait for clients to accept session renegotiation.
   When a server requests session renegotiation, if the client does not begin renegotiation
   within this time, the server will terminate the connection
*/
#define TMW_DEFAULT_RENEGOTIATE_REQ_TIMEOUT    60000


/* If these are defined, the default internal versions of the security logging functions
   will be included if they are not explicitly overridden in the security configuration
   structure (TMW_StackSecurityConfiguration_t).
*/
#define TMW_USE_DEFAULT_MMS_SECURITY_LOGGING
#define TMW_USE_DEFAULT_TLS_SECURITY_LOGGING


#define TMW_DEFAULT_CIPHERS "TLSv1:!SSLv2:!aNULL:!eNULL:!CAMELLIA:!EXPORT40:!EXPORT56:@STRENGTH"

/* End of user configurable data */

/* ---------------------------------------------------------------------------- */

typedef enum { tmwMmsSecurityError_SUCCESS,
               tmwMmsSecurityError_NO_PEER_SECURITY,
               tmwMmsSecurityError_CERT_VERIFY,
               tmwMmsSecurityError_SIGNATURE_SIZE,
               tmwMmsSecurityError_SIGNATURE_GEN,
               tmwMmsSecurityError_SHA1_HASH,
               tmwMmsSecurityError_MEMORY_ALLOCATION,
               tmwMmsSecurityError_SSL_MEMORY,
               tmwMmsSecurityError_SSL_BLINDING,
               tmwMmsSecurityError_SIGNATURE_VERIFY,
               tmwMmsSecurityError_CERTIFICATE_FILE_OPEN,
               tmwMmsSecurityError_NO_CONFIGURED_CERTIFICATE,
               tmwMmsSecurityError_CERTIFICATE_FILE_DECODE,
               tmwMmsSecurityError_PRIVATE_KEY_FILE_OPEN,
               tmwMmsSecurityError_NO_CONFIGURED_PRIVATE_KEY,
               tmwMmsSecurityError_PRIVATE_KEY_FILE_DECODE,
               tmwMmsSecurityError_SET_DEFAULT_CA_PATHS,
               tmwMmsSecurityError_LOAD_CA_LOCATIONS,
               tmwMmsSecurityError_CONFIGURATION,
               tmwMmsSecurityError_DECODE_PEER_CERTIFICATE,
               tmwMmsSecurityError_NO_PEER_COMMON_NAME,
               tmwMmsSecurityError_MISMATCHED_COMMON_NAME,
               tmwMmsSecurityError_NO_PEER_PUBLIC_KEY,
               tmwMmsSecurityError_NO_PEER_RSA_PUBLIC_KEY,
               tmwMmsSecurityError_ENCODE_CERTIFICATE_TOO_LARGE,
               tmwMmsSecurityError_ENCODE_CERTIFICATE,
               tmwMmsSecurityError_NO_CONFIGURED_CA,
               tmwMmsSecurityError_NO_CONFIGURED_CRL,
               tmwMmsSecurityError_LOAD_CRL_FILE,
               tmwMmsSecurityError_CONFIGURED_CA_DEPTH,
               tmwMmsSecurityError_NULL_SERVER_INTERFACE
             } tmwMmsSecurityError_t;

typedef enum { tmwTlsError_SUCCESS,
               tmwTlsError_CONNECT_CERT_VERIFY,
               tmwTlsError_CONNECT_NO_CERT,
               tmwTlsError_CONNECT_COMMON_NAME,
               tmwTlsError_ACCEPT_CERT_VERIFY,
               tmwTlsError_ACCEPT_NO_CERT,
               tmwTlsError_ACCEPT_COMMON_NAME,
               tmwTlsError_NULL_CONFIGURATION,
               tmwTlsError_INVALID_CONFIGURATION,
               tmwTlsError_SSL_MEMORY,
               tmwTlsError_NO_PRIVATE_KEY_FILE,
               tmwTlsError_NO_CERTIFICATE,
               tmwTlsError_NO_CONFIGURED_CA,
               tmwTlsError_CONFIGURED_CA_DEPTH,
               tmwTlsError_NO_CONFIGURED_CRL,
               tmwTlsError_LOAD_CRL_FILE,
               tmwTlsError_PRIVATE_KEY_FILE_INVALID,
               tmwTlsError_DH_FILE_OPEN,
               tmwTlsError_DH_FILE_READ,
               tmwTlsError_SET_CIPHERS,
               tmwTlsError_LOAD_VERIFY_LOCATIONS,
               tmwTlsError_CERTIFICATE_FILE,
               tmwTlsError_RSA_PRIVATE_KEY_FILE,
               tmwTlsError_PRIVATE_KEY_FILE,
               tmwTlsError_RENOGOTIATION
             } tmwTlsError_t;

typedef enum { tmwTlsSelectState_NONE,
               tmwTlsSelectState_WANT_READ,
               tmwTlsSelectState_WANT_WRITE,
               tmwTlsSelectState_WANT_CONNECT,
               tmwTlsSelectState_WANT_ACCEPT
             } tmwTlsSelectState_t;

typedef enum { tmwTlsState_IDLE,
               tmwTlsState_CONNECTING,
               tmwTlsState_ACCEPTING,
               tmwTlsState_READY,
               tmwTlsState_READY_WANT_READ,
               tmwTlsState_READY_WANT_WRITE,
               tmwTlsState_RENEGOTIATING,
               tmwTlsState_FAILED,
               tmwTlsState_DISCONNECTING,
               tmwTlsState_DISCONNECTED
             } tmwTlsState_State;

typedef enum { tmwTlsSelectMethod_TLSV1 = 0,     /* default method */
               tmwTlsSelectMethod_TLSV1_1,
               tmwTlsSelectMethod_SSLV2,
               tmwTlsSelectMethod_SSLV3,
               tmwTlsSelectMethod_SSLV23
             } tmwTlsSelectMethod_t;


/* Prototypes for error logging callback functions */
typedef void (* TMW_log_mms_error_fcn)( tmwMmsSecurityError_t errCode, char *pExtraInfo, void *pUserData );
typedef void (* TMW_log_tls_error_fcn)( tmwTlsError_t errCode, char *pExtraInfo, void *pUserData );

/* Functions which translate error codes into human readable text, callable from error logging functions */
TMW_CLIB_API void TMW_get_tls_error_text(tmwTlsError_t errorCode, char *pExtraInfo, char *pBuffer);
TMW_CLIB_API void TMW_get_mms_error_text(tmwMmsSecurityError_t errorCode, char *pExtraInfo, char *pBuffer);


/* One of these goes in each RFC1006 context */
typedef struct TMW_TlsSecurityContext_t {
    void                    *pSSL_CTX;
    int                     nTlsRenegotiationSeconds;          /* Max time (seconds) before forcing cipher renegotiation */
    int                     nTlsRenegotiationCount;            /* Max PDUs befo forcing cipher renegotiation */
    int                     nTlsRenegotiationMsTimeout;        /* Max ms time to wait after requesting renegotiation (Server_Hello) */
    char                    tlsCommonName[TMW_CRYPTO_ID_LEN];  /* Common name to expect on incoming TLS certs (empty string disables) */
    TMW_log_tls_error_fcn   errLogFcn;                         /* Callback function to log security errors */
    void                    *errLogFcnData;                    /* Callback function data to log security errors */
    char                    ciphers[TMW_CRYPTO_ID_LEN];                   /* openSSL cipher suite string */
    int                     bSkipTlsSecurity;

} TMW_TlsSecurityContext_t;

/* One of these goes in each RFC1006 connection */
typedef struct TMW_TlsSecurityData_t {
    tmwTlsState_State         state;                              /* TLS state machine */
    int                       last_error;                         /* Last error from OpenSSL */
    int                       rfc1006_state_when_error_occurred;  /* Diagnostic */
    tmwTlsSelectState_t       select_state;                       /* State of select that is needed to proceed */
    int                       socket_state;                       /* RFC1006_SOCKET_SUCCESS, BLOCKED, or FAILED */
    TMW_ms_time               renegotiation_timer;                /* Times out at nTlsRenegotiationSeconds */
    int                       renegotiation_counter;              /* Counts TPDUs up to nTlsRenegotiationCount */
    int                       renegotiation_requested;            /* True if Server_Hello is pending */
    TMW_ms_time               renegotiation_request_timeout;      /* Used to timout pending renegotiation requests (Server_Hellos) */
    int                       pending_write;                      /* Non-zero if we are in the middle of sending a PDU */
    void                      *pSSL;                              /* OpenSSL 'connection' */
    TMW_TlsSecurityContext_t  *pTlsSecurityContext;               /* Pointer to TLS security context */
} TMW_TlsSecurityData_t;

typedef struct TMW_MmsSecurityContext_t {
    void                    *pCaStore;                         /* Certificate Authority validation */
    char                    *pMmsCertCommonName;               /* Common name of this MMS context */
    void                    *pPrKey;                           /* Private key for this MMS context */
    void                    *pMmsCert;                         /* Certificate for this MMS context */
    TMW_log_mms_error_fcn   errLogFcn;                         /* Callback function to log security errors */
    void                    *errLogFcnData;                    /* Callback function data to log security errors */
    int	          	    siscoCompatibility;                /* SISCO's implementation does not exactly follow 62351 */
} TMW_MmsSecurityContext_t;

typedef void *TMW_CryptoContextHandle_t;

/* Tls Context data structure pointer is actually OpenSSL SSL_CTX * */
#define tmwTlsState_SECURITY_CONTEXT    TMW_TlsSecurityContext_t
#define tmwTlsState_SECURITY_DATA       TMW_TlsSecurityData_t

#define TMW_IS_SECURE_RFC_CONNECTION( pRfcConnection )   ( (pRfcConnection)->securityData.pSSL != NULL )
#define TMW_IS_SECURE_RFC_CONTEXT( pRfcContext )         ( (pRfcContext)->securityContext.pSSL_CTX != NULL )

/* Called once at startup */
void TMW_InitializeSslLibrary( void );

/* Called once at shutdown */
void TMW_TerminateSslLibrary( void );

/* Called from RFC1006 at shutdown of each RFC1006 context */
int TMW_RemoveTlsSecurity( TMW_TP_Context *pTpContext );

/* Called from RFC1006 state machine (client side) */
int TMW_TlsConnect( TMW_TP_Connection   *pTpConnection );

/* Called from RFC1006 state machine (server side) */
int TMW_TlsAccept( TMW_TP_Connection    *pTpConnection );

/* Called from RFC1006 state machine (both sides) */
int TMW_TlsRead( unsigned char          *pBufferToFill,
                 int                    nLengthOfData,
                 TMW_TP_Connection      *pTpConnection );

/* Called from RFC1006 state machine (both sides) */
int TMW_TlsWrite( unsigned char          *pDataToSend,
                  int                    nLengthOfData,
                  TMW_TP_Connection      *pTpConnection,
                  int                    *pMustBeRetried );

/* Called from RFC1006 state machine (both sides) */
int TMW_TlsShutdown( TMW_TP_Connection    *pTpConnection );

/* Called from RFC1006 when connection fully disconnected */
void TMW_TlsCleanupConnection( TMW_TP_Connection *pTpConnection );

/* Called from RFC1006 state machine (both sides) */
int TMW_TlsGetSocketStatus( TMW_TP_Connection *pTpConnection );

/* Called from RFC1006 to periodically */
void TMW_TlsCheckForRenegotiation( TMW_TP_Connection *pTpConnection );

/* Called from RFC1006 for each TPDU received */
void TMW_TlsHaveIsoPdu( TMW_TP_Connection *pTpConnection );

/* Configuration containing all security for both MMS and TLS */
typedef struct TMW_StackSecurityConfiguration_t {
    char tlsRsaPrivateKeyFile[TMW_CRYPTO_ID_LEN];       /* File containing the private key for RSA TLS ciphers */
    char tlsRsaPrivateKeyPassPhrase[TMW_CRYPTO_ID_LEN]; /* PassPhrase for decrypting the private key for RSA TLS ciphers */
    char tlsRsaCertificateId[TMW_CRYPTO_ID_LEN];        /* File containing the certificate for key for RSA TLS ciphers */

    char tlsDsaPrivateKeyFile[TMW_CRYPTO_ID_LEN];       /* File containing the private key for DSA TLS ciphers */
    char tlsDsaPrivateKeyPassPhrase[TMW_CRYPTO_ID_LEN]; /* PassPhrase for decrypting the private key for DSA TLS ciphers */
    char tlsDsaCertificateId[TMW_CRYPTO_ID_LEN];        /* File containing the certificate for key for DSA TLS ciphers */

    char tlsCommonName[TMW_CRYPTO_ID_LEN];              /* Common name to expect on incoming TLS certs (empty string disables) */

    int  nTlsRenegotiationSeconds;                      /* Max time (seconds) before forcing cipher renegotiation */
    int  nTlsRenegotiationCount;                        /* Max PDUs befo forcing cipher renegotiation */
    int  nTlsRenegotiationMsTimeout;                    /* Max time to wait for client to respond to renegotiation request */

    char dhFileName[TMW_CRYPTO_ID_LEN];                 /* File containing DH parameters for TLS cipher suites */

    char mmsPrivateKeyFile[TMW_CRYPTO_ID_LEN];          /* File containing the private key for signing ACSE Authentication */
    char mmsPrivateKeyPassPhrase[TMW_CRYPTO_ID_LEN];    /* PassPhrase for decrypting the private key ACSE Authentication */
    char mmsCertificateId[TMW_CRYPTO_ID_LEN];           /* File containing the certificate for the ACSE private key */
    char mmsCommonName[TMW_CRYPTO_ID_LEN];              /* Common name to expect on incoming MMS certs (empty string disables) */

    char clientCertificatePath[TMW_CRYPTO_ID_LEN];      /* Path to list of possible Client-side Certificates */

    char caFileName[TMW_CRYPTO_ID_LEN];                 /* File containing Certificate Authority Certificates */
    char caPathName[TMW_CRYPTO_ID_LEN];                 /* Path to Certificate Authority Certificates (instead of file) */
    char caCrlFileName[TMW_CRYPTO_ID_LEN];              /* File containing Certificate Authority Certificates */
    int  nCaVerifyDepth;                                /* Depth of certificate chaining verification */

    char ciphers[TMW_CRYPTO_ID_LEN];                   /* openSSL cipher suite string */

    TMW_log_mms_error_fcn   pMmsErrLogFcn;              /* Callback function to log security errors */
    void                    *pMmsErrLogFcnData;         /* Callback function data to log security errors */
    TMW_log_tls_error_fcn   pTlsErrLogFcn;              /* Callback function to log security errors */
    void                    *pTlsErrLogFcnData;         /* Callback function data to log security errors */
    int						siscoCompatibility;         /* flag to use SISCO hack rather than spec */
    tmwTlsSelectMethod_t    openSSLmethod;				/* Crypto method for openSSL - used only for negative testing */
	long sslOptions;                                    /* openSSL cipher suite string */
    int                     bSkipTlsSecurity;           /* Can be set to allow MMS security without TLS  */

} TMW_StackSecurityConfiguration_t;

/* Called by user to read TMW_StackSecurityConfiguration_t from file */
int TMW_Crypto_configure( char *fileName, TMW_StackSecurityConfiguration_t *config );

/* Checks basic validity (has cert references, etc.) - but doesn't actually checks files */
int TMW_Crypto_validate_tls_configuration( TMW_StackSecurityConfiguration_t *pConfig );

/* Checks basic validity (has cert references, etc.) - but doesn't actually checks files */
int TMW_Crypto_validate_mms_security_configuration( TMW_StackSecurityConfiguration_t *pConfig );

/* 62351 requires that the implementation be capable of checking the local CRL at a configurable interval,
   and also states that the management of the CRL is a "local issue".
   It is up to some process outside TMW code to retreive the CRL file from the CA as needed.
   When the CRL is updated this function must be called by your application in order to reload the CRL.
   An open connection will be checked against the new CRL on the next renegotiation,
   which is based on the limits in the security config file.
*/
TMW_CLIB_API int TMW_Crypto_tls_reload_crl( TMW_StackSecurityConfiguration_t *pCryptoConfig, TMW_TP_Context *pTpContext  );
TMW_CLIB_API int TMW_Crypto_mms_reload_crl(TMW_StackSecurityConfiguration_t *pCryptoConfig, TMW_MmsSecurityContext_t  *pMmsSecurityContext);

/* Checks if it includes TLS at all */
int TMW_Crypto_tls_configured( TMW_StackSecurityConfiguration_t *pSecurityConfiguration );

/* Not used yet - future */
int TMW_Crypto_compare_mms_configurations( TMW_StackSecurityConfiguration_t *config1, TMW_StackSecurityConfiguration_t *config2 );

/* Called from STACK.C to verify that stacks having same listen port, IP addresses have same TLS configuration */
int TMW_Crypto_compare_tls_configurations( TMW_StackSecurityConfiguration_t *pConfig1, TMW_StackSecurityConfiguration_t *pConfig2 );

/* Called from STACK.C to create SSL context in TP context */
int TMW_Crypto_initialize_tls_security( TMW_StackSecurityConfiguration_t *pCryptoConfig, TMW_TP_Context *pTpContext );

/* Called from STACK.C to create MMS security context in MMS context */
int TMW_Crypto_initialize_mms_security( TMW_StackSecurityConfiguration_t *pSecurityConfig, void *pMmsContext );

/* Called from STACK.C to delete MMS security context in MMS context */
int TMW_Crypto_remove_mms_security( void *pMmsContext );

#if defined(TMW_USE_CLIENT) || defined(USE_TASE2_CLIENT)

/* ----------------------------

    The use of security on client outbound connections is based on the MMSd_RemoteServer upper layer address
    structure.  This structure contains (along with the upper-layer addresses) a parameter called AUTH_enabled.

    The AUTH_enabled can take on one of three values:

        MMSd_Authentication_None:   no authentication
        MMSd_Authentication_Weak:   password authentication (password in specified in upper layer addresses)
        MMSd_Authentication_Strong: full certificate-based authentication

    If MMSd_Authentication_Strong is used, additional security parameters are required. These security parameters
    are configured using the TMW_StackSecurityConfiguration_t data structure which includes the certificate and key
    files and other parameters required for authentication.  The TMW_StackSecurityConfiguration_t data structure
    can be passed to TMW_Crypto_new_mms_security_context (below) to create an MMS security context, which can be
    installed in the upper-layer address structure for any connections requiring those parameters.  It is the
    responsibility of the user to free the MMS security context when all of the remote servers with that MMS
    security context are done. The routine TMW_Crypto_remove_client_mms_security (below) may be used to remove
    the security context from an upper-layer address structure if the remote server containing it needs to be
    re-used with either different parameters or for an unsecured connection.

    If MMSd_Authentication_Strong is used but no security parameters have been installed in the upper-layer address
    structure, the library will attempt to find security parameters from the MMS context associated with the client
    context.  This will succeed if the client context was created using TMW_create_client_stack_context or
    TMW_create_full_address_client_context and both an MMS context and a TMW_StackSecurityConfiguration_t was
    included.  This means that the stack will use the same security parameters for the outgoing connection as
    it would use for incoming connections.

    Note that in all cases, the security parameters for the transport layer (TLS) must also have been specified
    when the stack was created. The TLS parameters are taken from the same TMW_StackSecurityConfiguration_t data
    structure at stack creation time.

---------------------------- */

/* Called by user to retrieve MMS security context for connections based on configuration */
TMW_CryptoContextHandle_t TMW_Crypto_new_mms_security_context( TMW_StackSecurityConfiguration_t *pSecurityConfig );

/* Called by user to install MMS security context created by TMW_Crypto_new_mms_security_context into connection address */
int TMW_Crypto_initialize_client_mms_security( TMW_CryptoContextHandle_t pSecurityHandle, void *pUpperAddress );

/* Called by user to remove (but not free) MMS security context connection address */
int TMW_Crypto_remove_client_mms_security( void *pUpperAddress );

/* Called by user to free MMS security context created by TMW_Crypto_new_mms_security_context */
void TMW_Crypto_free_mms_security_context( TMW_CryptoContextHandle_t pHandle );

/* Called from CLI_REQ.C to attempt to get the security parameters from the MMS context associated with the client context */
/*  This is only done for new connections when ATH_enabled is STRONG authentication and no signature handlers are installed. */
int TMW_Crypto_retrieve_mms_security( void *pVoidClientContext, void *pVoidUpperAddress );

#endif
#endif

#endif /* _TLSVEND_H */
