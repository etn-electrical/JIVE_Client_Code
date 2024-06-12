#ifndef MATRIXSSL_INTERFACE_H
#define MATRIXSSL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif
//stdbool included for bool type
#include <stdbool.h>
#include "matrixos.h"

#ifndef min
  #define min( a, b )    ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

extern const uint16_t KEY_TYPE;
extern const uint16_t CERT_ALGORITHM;
extern const int16_t PS_SUCCESS_T;
extern const int16_t PS_FAILURE_T;
extern const int16_t PS_PLATFORM_FAIL_T;

#define MAC_ADDR_LEN 6U
#define MAX_PRIV_KEY_LEN 1300U
#define MAX_CERT_LEN 1024U

#define IS_WOULDBLOCK( return_val ) ( return_val == \
									  SOCKET_ERROR ) ? ( ( getSocketError() == EWOULDBLOCK ) || \
														 ( getSocketError() == EINTR ) ) : 0;



extern int32_t mtxssl_if_generate_device_certificate_RSA( uint8_t mac_id[MAC_ADDR_LEN],
													  char_t* common_name,
													  uint8_t cn_name_len );
int32_t mtxssl_if_SSL_CTX_load_key_material_RSA( void*ctx, const unsigned char*certBuf,
											 int32_t certLen, const unsigned char*privBuf,
											 int32_t privLen,
											 const unsigned char*CAbuf, int32_t CAlen );

int32_t mtxssl_if_SSL_CTX_load_key_material_ECC( void*ctx, const unsigned char*certBuf,
											 int32_t certLen, const unsigned char*privBuf,
											 int32_t privLen,
											 const unsigned char*CAbuf, int32_t CAlen );

void* mtxssl_if_SSL_new( void*sslContext, int8_t fd );

bool mtxssl_if_SSL_library_init();

void* mtxssl_if_SSL_CTX_new( void* ptr );

int32_t mtxssl_if_SSL_CTX_load_rsa_key_material( void*sslContext, const unsigned char*certBuf,
												 int32_t certLen, const unsigned char*privBuf,
												 int32_t privLen,
												 const unsigned char*CAbuf, int32_t CAlen );

void mtxssl_if_SSL_CTX_free( void*sslContext );

void mtxssl_if_update_timeout( void*ptr, int32_t timeout, int32_t idle_state_timeout );

uint8_t mtxssl_if_timeout_status( void*ptr );

bool mtxssl_if_idle_state_timeout_status( void*ptr );

int mtxssl_if_accept( void*ptr );

int mtxssl_if_write( void*ptr, char* data, int length );

int32_t mtxssl_if_matrixSslGetOutdata( void*ptr, unsigned char**buf );

void mtxssl_if_clear_wouldblock( void*ptr );

uint8_t mtxssl_if_get_wouldblock( void*ptr );

int32_t mtxssl_if_do_handshake( void*ptr );

void mtxssl_if_SSL_free( void*ptr );

void mtxssl_if_set_hostname( void*ptr, const char*hostname );

int mtxssl_if_SSL_connect( void*ptr );

void mtxssl_if_setSocketNodelay( int8_t fd );

void mtxssl_if_setSocketNonblock( int8_t fd );

int mtxssl_if_SSL_processed_data( void*ptr, unsigned char**data, int*length );

int mtxssl_if_SSL_get_error( void*ptr, int ret_val );

int mtxssl_if_SSL_read( void*ptr, char* data, int length );

int32_t mtxssl_if_SSL_get_data( void*ptr, unsigned char**ptBuf, int*ptBufLen );

int mtxssl_if_socketConnect( char*hostname, short port, int*err );

void mtxssl_if_eeTrace( const char*str );

void mtxssl_if_psTraceInfo( const char*str );

void mtxssl_if_psTraceIntInfo( const char*str, int err );

void mtxssl_if_DLListInsertTail( void*connsTmp, void*list );

void mtxssl_if_DmatrixSslClose( void );

void mtxssl_if_ssl_rehandshake_disable( void*ptr );

uint8_t mtxssl_if_rehandshake_status( void*ptr );

void mtxssl_if_psGetTime( uint32_t*now );

int mtxssl_if_SSL_CTX_New_Keys( void* ctx );

#ifdef __cplusplus
}
#endif

#endif //MATRIXSSL_INTERFACE_H
