/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __HTTPD_COMMON_H__
#define __HTTPD_COMMON_H__

#include <stdint.h>
#include "DCI_Patron.h"
#include "httpd_rest_dci.h"
#include "Etn_Types.h"
#include "Rest_Config.h"
#include "REST_DCI_Data.h"

#ifdef __DIGEST_ACCESS_AUTH__
#define HTTP_RESP_MAJOR_VER      1
#define HTTP_RESP_MINOR_VER      0
#endif

#if 1 != HTTP_RESP_MAJOR_VER
#error HTTP Response Major Version has to be 1
#endif

#if 0 == HTTP_RESP_MINOR_VER
#define HTTP_RESP_VER   "HTTP/1.0 "
#elif 1 == HTTP_RESP_MINOR_VER
#define HTTP_RESP_VER_STRING   "HTTP/1.1 "
#else
#error minor version supported: 0 or 1
#endif
/******************************************************************************/

/******************************************************************************/

#define CONCAT_INT_8( s1, s2, s3, s4, s5, s6, s7, s8 ) \
	s1 ## s2 ## s3 ## s4 ## s5 ## s6 ## s7 ## s8
#define CONCAT_MACRO_8( s1, s2, s3, s4, s5, s6, s7, s8 ) \
	CONCAT_INT_8( s1, s2, s3, s4, s5, s6, s7, s8 )
/******************************************************************************/

/******************************************************************************/
// character classification, applied to char 'c', only valid within [0, 127]
// based on RFC 2616 (HTTP/1.1), Section 2.2, p12

#define IS_SPACE( c )    ( ( c ) == 0x20U )	/* Space character */
#define IS_PERIOD( c )    ( ( c ) == 0x2EU )/* Period (.) character */
#define IS_UNDERSCORE( c )    ( ( c ) == 0x5FU )/* Underscore (_) character */
#define IS_APOSTROPHE( c )    ( ( c ) == 0x27U )/* Apostrophe (') character */
#define IS_DASH( c )    ( ( c ) == 0x2DU )	/* Dash (-) character */
#define IS_PWD_SPECIAL_CHAR( c )      ( ( ( c ) >= 0x20 && ( c ) <= 0x2FU ) || ( ( c ) >= 0x3A && ( c ) <= 0x40U ) || \
										( ( c ) >= 0x5B && ( c ) <= 0x60U ) || ( ( c ) >= 0x7B && ( c ) <= 0x7EU ) )
#define IS_CHAR( c )      ( ( c ) >= 0 && ( c ) <= 127U )
#define IS_CTL_CHAR( c )  ( ( c ) >= 0 && ( c ) < 32 || ( c ) == 127U )
#define IS_UPALPHA( c )   ( ( ( c ) >= 0x41U ) && ( ( c ) <= 0x5AU ) )	/* A...Z */
#define IS_LOALPHA( c )   ( ( ( c ) >= 0x61U ) && ( ( c ) <= 0x7AU ) )	/* a...z */
#define IS_ALPHA( c )     ( IS_UPALPHA( c ) || IS_LOALPHA( c ) )
#define IS_DIGIT( c )     ( ( ( c ) >= 0x30U ) && ( ( c ) <= 0x39U ) )	/* 0...9 */
#define IS_HEX( c )       ( IS_DIGIT( c ) || ( ( c ) >= 0x41U && ( c ) <= 0x46U ) \
							|| ( ( c ) >= 0x61U && c <= 0x66U ) )	/* a..f or A .. F */
#define IS_SP_OR_HT( c )  ( ( c ) == ' ' || ( c ) == '\t' )
#define IS_NEW_LINE( c )  ( ( c ) == '\n' )
#define IS_TOKEN_CHAR( c )   ( IS_ALPHA( c ) || IS_DIGIT( c ) \
							   || ( ( c ) >= 0x23 && ( c ) <= 0x27 ) \
							   || ( ( c ) >= 0x2A && ( c ) <= 0x2E && ( c ) != 0x2C ) \
							   || ( ( c ) >= 0x5E && ( c ) <= 0x60 ) \
							   || ( ( c ) == '!' ) \
							   || ( ( c ) == '~' ) )
// based on RFC 2396 and RFC 2732
#define IS_ALPHANUM( c )  ( IS_ALPHA( c ) || IS_DIGIT( c ) )
#define IS_VALID_FIRST_USRNAME_CHAR( c )  ( IS_UNDERSCORE( c ) || IS_APOSTROPHE( c ) || IS_DASH( c ) || \
											IS_ALPHANUM( c ) )
#define IS_VALID_USRNAME_CHAR( c )  ( IS_VALID_FIRST_USRNAME_CHAR( c ) || IS_PERIOD( c ) )
#define ALIGN_U64( addr ) ( ( ( addr ) + 7U ) & ( ~7U ) )
// to deal with case-insensitive comparison in RFC 2616
// RFC 2616 token is "case-insensitive" by default with exceptions:

// header names are case-INsensitive
// URIs are case-SENSITIVE,
// although host name and scheme name are case-INsensitive
// HTTP-date is case-SENSITIVE
// methods are case-SENSITIVE

#define TO_UPPER( c )     ( IS_LOALPHA( c ) ? ( ( c ) - 32U ) : ( c ) )
#define TO_LOWER( c )     ( IS_UPALPHA( c ) ? ( ( c ) + 32U ) : ( c ) )

#define CR      '\r'
#define LF      '\n'
#define CRLF    "\r\n"
#define COLON_SPACE ": "
#define COMMA   ","

#define GLOBAL_ARR_SEPARATOR ','

// RFC 2396, unreserved = alphanum | mark
// mark     =       "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
#define IS_UNRESERVED( c ) ( IS_ALPHANUM( c ) || '-' == ( c ) || '_' == ( c ) \
							 || '.' == ( c ) || '!' == ( c ) || '~' == ( c ) \
							 || ( '\'' <= ( c ) && '*' >= ( c ) ) )
#define IS_EXCLUDED_IN_URI( c ) ( ' ' >= ( c ) || 127 <= ( c ) \
								  || '<' == ( c ) || '>' == ( c ) || '#' == ( c ) || '\"' == ( c ) \
								  || ( ( '{' <= ( c ) ) && ( '}' >= ( c ) ) ) \
								  || '\\' == ( c ) || '^' == ( c ) || '`' == ( c ) )

#define DECODE_HEX( c ) ( IS_DIGIT( c ) ? ( ( c ) - '0' ) : ( ( TO_UPPER( c ) ) - 'A' + 10 ) )
/******************************************************************************/

/******************************************************************************/
#define HTTP_POOL_BLOCK_SIZE  ( 128U )
#define HTTP_POOL_BLOCK_COUNT  ( HTTP_POOL_TOTAL_BUFFER_SIZE / HTTP_POOL_BLOCK_SIZE )

typedef uint8_t present_t;

#define PRESENT_TYPE_SIZE               ( sizeof( present_t ) * 8 )
#define DATA_PRESENT_SIZE               ( ( REST_DCI_MAX_ARRAY_COUNT + ( PRESENT_TYPE_SIZE - 1 ) ) / PRESENT_TYPE_SIZE )
#define RECV_TOTAL_BUF_SIZE             ( DATA_PRESENT_SIZE + REST_DCI_MAX_LENGTH )
#define RECV_BUF_BLOCK_NUM              ( ( RECV_TOTAL_BUF_SIZE + ( HTTP_POOL_BLOCK_SIZE - 1 ) ) / \
										  HTTP_POOL_BLOCK_SIZE )

/*  Formatted buffer will get allocated in PUT/GET rest request if DCI has format information attached.
    "Format_Handler module" will use this space to cature payload string in PUT request.
    "Format_Handler module" will use this space to generate formatted string in GET request. FORMAT_HANDLER_BUFFER_SIZE
       can be configured in Rest_Config.h. Adopter should configure this value as per max payload requirement in PUT
       request of REST or ASSY resources.
 */
#define FORMATTED_BUF_BLOCK_NUM         ( ( FORMAT_HANDLER_BUFFER_SIZE + sizeof( uint16_t ) + \
											( HTTP_POOL_BLOCK_SIZE - 1 ) ) / HTTP_POOL_BLOCK_SIZE )
#define FORMATTED_TOTAL_BUF_SIZE        ( ( FORMATTED_BUF_BLOCK_NUM * HTTP_POOL_BLOCK_SIZE ) / ( sizeof( uint8_t ) ) )
#define FORMATTED_BUF_SIZE              ( FORMATTED_TOTAL_BUF_SIZE - sizeof( uint16_t ) )

#define HTTPD_RECV_BUF_REUSE_U8_PRESENCE    ( ( ( RECV_BUF_BLOCK_NUM * HTTP_POOL_BLOCK_SIZE ) - DATA_PRESENT_SIZE ) / \
											  ( sizeof( present_t ) * ( sizeof( uint8_t ) ) ) )
#define HTTPD_RECV_BUF_REUSE_U16_PRESENCE   ( ( ( RECV_BUF_BLOCK_NUM * HTTP_POOL_BLOCK_SIZE ) - DATA_PRESENT_SIZE ) / \
											  ( sizeof( present_t ) * ( sizeof( uint16_t ) ) ) )
#define HTTPD_RECV_BUF_REUSE_U32_PRESENCE   ( ( ( RECV_BUF_BLOCK_NUM * HTTP_POOL_BLOCK_SIZE ) - DATA_PRESENT_SIZE ) / \
											  ( sizeof( present_t ) * ( sizeof( uint32_t ) ) ) )
#define HTTPD_RECV_BUF_REUSE_U64_PRESENCE   ( ( ( RECV_BUF_BLOCK_NUM * HTTP_POOL_BLOCK_SIZE ) - DATA_PRESENT_SIZE ) / \
											  ( sizeof( present_t ) * ( sizeof( uint64_t ) ) ) )



struct ReusedRecvBuf
{
	present_t present[DATA_PRESENT_SIZE];
	union
	{
		uint8_t u8_data[HTTPD_RECV_BUF_REUSE_U8_PRESENCE];
		int8_t s8_data[HTTPD_RECV_BUF_REUSE_U8_PRESENCE];
		uint16_t u16_data[HTTPD_RECV_BUF_REUSE_U16_PRESENCE];
		int16_t s16_data[HTTPD_RECV_BUF_REUSE_U16_PRESENCE];
		uint32_t u32_data[HTTPD_RECV_BUF_REUSE_U32_PRESENCE];
		int32_t s32_data[HTTPD_RECV_BUF_REUSE_U32_PRESENCE];
		uint64_t u64_data[HTTPD_RECV_BUF_REUSE_U64_PRESENCE];
		int64_t s64_data[HTTPD_RECV_BUF_REUSE_U64_PRESENCE];
	} buf;
};

struct FormatBuf
{
	uint16_t byte_index;
	uint8_t buf[FORMATTED_BUF_SIZE];
};

static const uint32_t PTARGET_BUFF_SIZE = ( RECV_BUF_BLOCK_NUM * HTTP_POOL_BLOCK_SIZE );




/******************************************************************************/

/******************************************************************************/
/*
    HTTP method table, RFC 2616, 5.1.1, p24 sorted from the most frequently used to the least frequently used a
       preprocessor trick to assure consistency of the table of methods
 #define METHOD_TABLE METHOD_EXPAND(GET), \
       METHOD_EXPAND(HEAD), \
       METHOD_EXPAND(OPTIONS), \
       METHOD_EXPAND(POST), \
       METHOD_EXPAND(PUT), \
    //
    enum {
    HTTP_METHOD_DELETE = 0U,
 #undef  METHOD_EXPAND
 #define METHOD_EXPAND(method) HTTP_METHOD_##method METHOD_TABLE HTTP_METHOD_MAX, HTTP_METHOD_UNRECOGNIZED =
   HTTP_METHOD_MAX, HTTP_METHOD_UNDEFINED,
    } ;
 */
static const uint16_t HTTP_METHOD_DELETE = 0;
static const uint16_t HTTP_METHOD_GET = 1;
static const uint16_t HTTP_METHOD_HEAD = 2;
static const uint16_t HTTP_METHOD_OPTIONS = 3;
static const uint16_t HTTP_METHOD_POST = 4;
static const uint16_t HTTP_METHOD_PUT = 5;
static const uint16_t HTTP_METHOD_MAX = 6;
static const uint16_t HTTP_METHOD_UNRECOGNIZED = HTTP_METHOD_MAX;
static const uint16_t HTTP_METHOD_UNDEFINED = HTTP_METHOD_UNRECOGNIZED + 1;

#define ALLOW_ORIGIN
#ifdef ALLOW_ORIGIN
#define ACCESS_CONTROL_ALLOW_ORIGIN "Access-Control-Allow-Origin: *\r\n"
#define ACCESS_CONTROL_ALLOW_METHODS "Access-Control-Allow-Methods: GET, OPTIONS"
#define ACCESS_CONTROL_ALLOW_METHODS_WRITE ", POST, PUT"
#define ACCESS_CONTROL_ALLOW_HEADERS "Access-Control-Allow-Headers: accept, authorization\r\n"
#endif
#define ACCESS_CONTROL_MAX_AGE "Access-Control-Max-Age: 1728000\r\n"
#define NO_CACHE_HEADERS \
	"Cache-Control: max-age=0,no-cache,no-store,post-check=0,pre-check=0\r\nExpires: Mon, 26 Jul 1997 05:00:00 GMT\r\nPragma: no-cache\r\n"
#define CONNECTION_CLOSE "Connection: close\r\n"
#define X_REQUEST_ACCESS_LEVEL "X-Request-Access-Level:"
/******************************************************************************/

static const uint16_t REST_ROOT = 0U;
static const uint16_t REST_ASSY = 1U;
static const uint16_t REST_DEVICE = 2U;
static const uint16_t REST_PARAM = 3U;
static const uint16_t REST_STRUCT = 4U;
static const uint16_t REST_FW = 5U;
static const uint16_t REST_USER_MGMT = 6U;
static const uint16_t REST_LOG = 7U;
static const uint16_t REST_CERT = 8U;
static const uint16_t COUNT_REST = 9U;

static const uint8_t REST_PARAM_ALL = 0U;
static const uint8_t REST_PARAM_DEFAULT = 1U;
static const uint8_t REST_PARAM_DESCRIPTION = 2U;
static const uint8_t REST_PARAM_ENUM = 3U;
static const uint8_t REST_PARAM_INITIAL = 4U;
static const uint8_t REST_PARAM_PRESENTVAL = 5U;
static const uint8_t REST_PARAM_RANGE = 6U;
static const uint8_t REST_PARAM_VALUE = 7U;
static const uint8_t REST_PARAM_BITLIST = 8U;
static const uint8_t COUNT_REST_PARAMS = 9U;

static const uint16_t REST_STRUCT_UNSPECIFIED = 0U;
static const uint16_t REST_STRUCT_ALL = 1U;
static const uint16_t REST_STRUCT_ROOT = 2U;
static const uint16_t REST_STRUCT_ERR = 3U;
static const uint16_t REST_STRUCT_MAX = REST_STRUCT_ERR;

/******************************************************************************/


/******************************************************************************/
#define SINT32_MAX 0x7FFFFFFFL
#define UINT32_MAX_ALL_SP   "          "
#define UINT32_MAX_STRING   "4294967295"
#define SINT32_MIN_STRING   "-2147483648"
#define MAX_NUMBER_STRING_LENGTH sizeof( "+18446744073709551615" )
#define MAX_IPV4_STR_LEN  STRLEN( "255.255.255.255" )
#define MAX_IPV4_STR_BUF_SIZE  ( MAX_IPV4_STR_LEN + 1U )
/******************************************************************************/

/******************************************************************************/
static const uint8_t VALID_SINT8_MAX = 0x80U;
static const uint16_t VALID_SINT16_MAX = 0x8000U;
static const uint32_t VALID_SINT32_MAX = 0x80000000LLU;
static const uint64_t VALID_SINT64_MAX = 0x8000000000000000LLU;
static const uint8_t VALID_UINT8_MAX = 0xFFU;
static const uint16_t VALID_UINT16_MAX = 0xFFFFU;
static const uint32_t VALID_UINT32_MAX = 0xFFFFFFFFLLU;
/******************************************************************************/
extern DCI_Patron* httpd_dci_access;
static const bool_t VALID = static_cast<bool_t>( 1U );
static const bool_t INVALID = static_cast<bool_t>( 0U );

typedef struct
{
	uint32_t ip_address;
	uint8_t port;
} ip_address_setting_t;

enum web_server_media_t
{
	HTTP_MEDIA = 0,
	HTTPS_MEDIA,
	TOTAL_NUM_MEDIA
};

#endif  /* __HTTPD_COMMON_H__ */
