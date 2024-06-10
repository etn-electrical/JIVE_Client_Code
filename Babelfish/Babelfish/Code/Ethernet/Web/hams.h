/**
 *****************************************************************************************
 *	@file        Hams header file for the webservices.
 *
 *	@brief       This file shall wrap the functions required for webservices using REST protocol.
 *
 *	@details     The LTK REST services are designed primarily for use from the adopting  product web front end for
 * browser based data access. However, they provide an open and standard method for data access from any supporting REST
 * client
 *

 *  @copyright   2015 Eaton Corporation. All Rights Reserved.
 *
 *	@note        Eaton Corporation claims proprietary rights to the material disclosed here in.  This technical
 * information should not be reproduced or used without direct written permission from Eaton Corporation.*
 *
 *
 *
 *
 *
 *
 *****************************************************************************************
 */

#ifndef __HTTP_STREAM_PARSER__
#define __HTTP_STREAM_PARSER__

#include <stdint.h>
#include <string.h>
#include "httpd_common.h"
#include "httpd_digest_authentication.h"
#include "Json_Packetizer.h"


/* HAMS: HTTP Authenticating and Multipart/form-data Support
 *
 * Foundation for server processing and authenticating HTTP requests, as well as parsing multipart/form-data uploads.
 *
 * Features:
 * -Stack agnostic and cross platform
 * -Modular server construction
 * -Minimal code size
 * -Low RAM usage (both heap and stack)
 * -Minimal API
 * */

// #define MAX_URI_DEPTH 8
#define MAX_URI_SEG_LENGTH 80
/* 1MB should be more than enough for all types of Firmwares */
#define MAX_CONTENT_LENGTH 1048576
#define HTTP_ETAG_LEN 32
#define MAX_HTTP_BASIC_AUTH_FIELD 80

/* Reference http://stackoverflow.com/questions/8250154/html-multipart-form-maximum-length-of-boundary-string
 *
 * RFC2616 (HTTP/1.1) section "3.7 Media Types" says that the allowed types in the Content-Type header is defined by
 * RFC1590 (Media Type Registration Procedure). RFC1590 updates RFC-1521(MIME). RFC1521 says that a boundary "must be no
 * longer than 70 characters, not counting the two leading hyphens". The same text also appears in RFC2046 which
 * supposedly obsoletes RFC1521.
 *
 * */
#define MAX_MULTIPART_BOUNDARY_LENGTH 70U

/** @brief Define minimum amount of data user is required to supply for shared output buffer */
#define HAMS_MIN_STAGE_EXT 256

static const uint16_t HTTP_HDR_ALLOW = 0;
static const uint16_t HTTP_HDR_CONTENT_ENCODING = 1;
static const uint16_t HTTP_HDR_LOCATION = 2;
static const uint16_t HTTP_HDR_WWW_AUTHENTICATE = 3;

static const uint16_t HTTP_HDR_X_REQUEST_ACCESS_LEVEL = 4;
static const uint16_t HTTP_HDR_AUTHORIZATION = 5;
static const uint16_t HTTP_HDR_CONNECTION = 6;
static const uint16_t HTTP_HDR_CONTENT_DISPOSITION = 7;

static const uint16_t HTTP_HDR_CONTENT_LENGTH = 8;
static const uint16_t HTTP_HDR_CONTENT_TYPE = 9;
static const uint16_t HTTP_HDR_ETAG = 10;
static const uint16_t HTTP_HDR_IF_NONE_MATCH = 11;
static const uint16_t HTTP_HDR_ORIGIN = 12;
static const uint16_t COUNT_HTTP_HDRS = 13;

static const uint16_t HTTP_AUTH_TYPE_BASIC = 0;
static const uint16_t HTTP_AUTH_TYPE_DIGEST = 1;
static const uint16_t HTTP_AUTH_TYPE_CUSTOM_AUTH = 2;
/* The Custom auth shall be The authorization string shall be used to specify the authorization header which will be
   sent back to client. Generally whenever browser detects 401 response along with Digest or Basic response header, it
   generates popup to enter credentials. However UI developer may want to genarte its own popup in such cases and not to
   use popup provided by browser. In such case the server will send 401 response, however the Auth_header will be custom
   in agreement with UI owner.
 */
static const uint16_t COUNT_HTTP_AUTH_TYPES = 3;

#ifdef JSON_ACTIVE
static const uint16_t HTTP_CONTENT_TYPE_JSON = 0;
static const uint16_t HTTP_CONTENT_TYPE_APPLICATION_OCTET_STREAM = 1;
static const uint16_t HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED = 2;
static const uint16_t HTTP_CONTENT_TYPE_APPLICATION_XML = 3;
static const uint16_t HTTP_CONTENT_TYPE_MULTIPART_FORM = 4;
static const uint16_t HTTP_CONTENT_TYPE_TEXT_PLAIN = 5;
static const uint16_t COUNT_HTTP_CONTENT_TYPES = 6;
#else
static const uint16_t HTTP_CONTENT_TYPE_APPLICATION_OCTET_STREAM = 0;
static const uint16_t HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED = 1;
static const uint16_t HTTP_CONTENT_TYPE_APPLICATION_XML = 2;
static const uint16_t HTTP_CONTENT_TYPE_MULTIPART_FORM = 3;
static const uint16_t HTTP_CONTENT_TYPE_TEXT_PLAIN = 4;
static const uint16_t HTTP_CONTENT_TYPE_JSON = 5;
static const uint16_t COUNT_HTTP_CONTENT_TYPES = 6;
#endif

/* 4XX Error reasons */
static const uint16_t HTTP_ERR_4XX_URI_SLASH = 0;
static const uint16_t HTTP_ERR_4XX_INTR_QUERY = 1;
static const uint16_t HTTP_ERR_4XX_BAD_QUERY_EQ = 2;
static const uint16_t HTTP_ERR_4XX_BAD_QUERY_QUEST = 3;
static const uint16_t HTTP_ERR_4XX_BAD_QUERY_AMP = 4;
static const uint16_t HTTP_ERR_4XX_BAD_HTTP_VER_MAJOR = 5;
static const uint16_t HTTP_ERR_4XX_BAD_HTTP_VER_MINOR = 6;
static const uint16_t HTTP_ERR_4XX_CONTENT_LENGTH_OVERLONG = 7;
static const uint16_t HTTP_ERR_4XX_CONTENT_LENGTH = 8;
static const uint16_t HTTP_ERR_4XX_URI_OVERLONG = 9;
static const uint16_t HTTP_ERR_4XX_BAD_LINE_TERM = 10;
static const uint16_t HTTP_ERR_4XX_CONNECTION_TYPE = 11;
static const uint16_t HTTP_ERR_4XX_BAD_AUTH_TYPE = 12;
static const uint16_t HTTP_ERR_4XX_BAD_AUTH_BASIC_FORMAT = 13;
static const uint16_t HTTP_ERR_4XX_IF_NONE_MATCH = 14;
static const uint16_t HTTP_ERR_4XX_BAD_HTTP_HEADER = 15;
static const uint16_t HTTP_ERR_4XX_BAD_MULTIPART_BOUNDARY = 16;
static const uint16_t HTTP_ERR_4XX_BAD_DISPOSITION = 17;
static const uint16_t HTTP_ERR_4XX_CONTENT_BOUNDARY = 18;
static const uint16_t HTTP_ERR_4XX_CONTENT_UNDERRUN = 19;
static const uint16_t HTTP_ERR_4XX_CONTENT_TYPE_MISSING = 20;
static const uint16_t HTTP_ERR_4XX_BAD_MULTIPART_TYPE = 21;
static const uint16_t HTTP_ERR_4XX_USER_FUNCTION = 22;
static const uint16_t COUNT_HTTP_ERR_4XX = 23;

static const uint16_t HAMS_FLAG_KEEP_ALIVE = 0x01;
static const uint16_t HAMS_FLAG_SENDING = 0x02;
static const uint16_t HAMS_FLAG_STAGE_REQ = 0x04;
static const uint16_t HAMS_FLAG_ERROR = 0x08;
static const uint16_t HAMS_FLAG_FIRST_SKIP = 0x10;
static const uint16_t HAMS_FLAG_CONTENT = 0x20;
static const uint16_t HAMS_FLAG_MULTIPART = 0x40;
static const uint16_t HAMS_FLAG_FIRST_BOUNDARY = 0x80;
static const uint16_t HAMS_FLAG_ECHO_CONTENT = 0x100;
static const uint16_t HAMS_FLAG_HANDLE_ERROR = 0x200;
static const uint16_t HAMS_FLAG_REDIRECT = 0x400;

static const uint16_t HAMS_EXPECT_TX = 0x1;
static const uint16_t HAMS_EXPECT_RX = 0x2;

static const uint16_t E404_NO_QUERY_ALLOWED = 0x4000;
static const uint16_t E404_BAD_QUERY_VARIABLE = 0x4001;
static const uint16_t E404_QUERY_REQUIRED = 0x4002;
static const uint16_t E400_URI_SLASH = 0x4003;
static const uint16_t E400_INTR_QUERY = 0x4004;
static const uint16_t E400_BAD_QUERY_EQUALS = 0x4005;
static const uint16_t E400_BAD_QUERY_QUESTION_MARK = 0x4006;
static const uint16_t E400_BAD_QUERY_AMPERSAND = 0x4007;
static const uint16_t E400_BAD_HTTP_VER_MAJOR = 0x4008;
static const uint16_t E400_BAD_HTTP_VER_MINOR = 0x4009;
static const uint16_t E400_CONTENT_LENGTH_OVERLONG = 0x400A;
static const uint16_t E400_BAD_CONTENT_LENGTH = 0x400B;
static const uint16_t E400_BAD_LINE_TERM = 0x400C;
static const uint16_t E400_CONNECTION_TYPE = 0x400D;
static const uint16_t E400_BAD_AUTH_TYPE = 0x400E;
static const uint16_t E400_BAD_AUTH_FORMAT = 0x400F;
static const uint16_t E400_IF_NONE_MATCH = 0x4010;
static const uint16_t E400_BAD_HTTP_HEADER = 0x4011;
static const uint16_t E400_BAD_MULTIPART_BOUNDARY = 0x4012;
static const uint16_t E400_BAD_DISPOSITION = 0x4013;
static const uint16_t E400_CONTENT_BOUNDARY = 0x4014;
static const uint16_t E400_CONTENT_UNDERRUN = 0x4015;
static const uint16_t E400_CONTENT_TYPE_MISSING = 0x4016;
static const uint16_t E400_BAD_MULTIPART_TYPE = 0x4017;
static const uint16_t E400_BAD_ORIGIN = 0x4018;

static const uint16_t E503_DYNAMIC_SPACE = 0x5000;
static const uint16_t E503_XML_PARSER = 0x5001;
static const uint16_t E503_SCRATCH_SPACE = 0x5002;

/* For saving (R)EST (V)alues. */
static const uint16_t E500_RV_INVALID_COMMAND = 0x5003;
static const uint16_t E500_RV_ACCESS_VIOLATION = 0x5004;
static const uint16_t E500_RV_EXCEEDS_RANGE = 0x5005;
static const uint16_t E500_RV_READ_ONLY = 0x5006;
static const uint16_t E500_RV_NV_VALUE_INACCESSIBLE = 0x5007;
static const uint16_t E500_RV_NV_ACCESS_FAILURE = 0x5008;
static const uint16_t E500_RV_NV_READ_ONLY = 0x5009;
static const uint16_t E500_RV_DEFAULT_NOT_AVAILABLE = 0x500A;
static const uint16_t E500_RV_RANGE_NOT_AVAILABLE = 0x500B;
static const uint16_t E500_RV_ENUM_NOT_AVAILABLE = 0x500C;
static const uint16_t E500_RV_INVALID_DATA_LENGTH = 0x500D;
static const uint16_t E500_RV_VALUE_LOCKED = 0x500E;

static const uint16_t E400_MB_ILLEGAL_FUNCTION_CODE = 0x4030;
static const uint16_t E400_MB_DATA_ADDRESS_ERROR = 0x4031;
static const uint16_t E400_MB_DATA_VALUE_ERROR = 0x4032;
static const uint16_t E400_MB_STRING_SIZE_WRONG = 0x4033;

static const uint16_t E400_XML_EOF_ERROR = 0x4040;
static const uint16_t E400_XML_INVALID_CHAR_ERROR = 0x4041;
static const uint16_t E400_XML_CLOSE_ERROR = 0x4042;
static const uint16_t E400_XML_STACK_ERROR = 0x4043;
static const uint16_t E400_XML_SYNTAX_ERROR = 0x4044;

static const uint16_t E503_MB_DEVICE_FAILURE_ERROR = 0x5030;
static const uint16_t E503_MB_DEVICE_ACK_ERROR = 0x5031;
static const uint16_t E503_MB_DEVICE_BUSY_ERROR = 0x5032;
static const uint16_t E503_MB_GENERAL_RECEIVE_ERROR = 0x5033;
static const uint16_t E503_MB_INTERCOM_BLOCK_ERROR = 0x5034;
static const uint16_t E503_MB_NO_SLAVE_RESPONSE = 0x5035;

static const uint16_t E422_DELETE_NON_EXIST_RESOURCE = 0x4220U;
static const uint16_t E422_SESSION_ID_MISMATCH = 0x4221U;
static const uint16_t E422_INVALID_RESOURCE_TIMEOUT = 0x4222U;
static const uint16_t E422_INVALID_ENCODE_TYPE = 0x4223U;
static const uint16_t E422_DECRYPTION_FAILED = 0x4224U;
static const uint16_t E422_SESSION_NOT_EXIST = 0x4225U;
static const uint16_t E422_SIZE_MISMATCH = 0x4226U;
static const uint16_t E422_OUT_OF_ORDER_CHUNK = 0x4227U;
static const uint16_t E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS = 0x4228U;
static const uint16_t E422_HOST_NOT_READY_TO_ACCEPT_REQUEST = 0x4229U;
static const uint16_t E422_HOST_FW_UPLOAD_ERROR = 0x4230U;

static const uint16_t E422_ONE_ADMIN_MANDATE_ERROR = 0x4240;
static const uint16_t E422_USER_SELF_DELETE_ERROR = 0x4241;
static const uint16_t E422_USER_ALREADY_EXISTS = 0x4242;
static const uint16_t E422_USER_DATABASE_FULL = 0x4243;
static const uint16_t E422_PWD_COMPLEXITY_VIOLATION = 0x4244;
static const uint16_t E422_PWD_MATCHES_WITH_EXISTING_NAME_OR_PASSWORD = 0x4245;

static const uint16_t E423_USER_LOCKED_TEMPORARILY = 0x4260;
static const uint16_t E401_USER_INVALID_CRED = 0x4261;

/* Match multipart boundary prefix of \r\n-- */
#define BOUND_MATCH_PREFIX 0x2D2D0A0D

/* Forward declaration. */
struct hams_parser_s;

/** @brief This callback type handles all*/
typedef unsigned (* hams_data_cb)( struct hams_parser_s* parser,
								   const uint8_t* data, unsigned length );

struct hams_extended
{
	uint32_t boundary_match[3];
	uint8_t multipart_boundary[MAX_MULTIPART_BOUNDARY_LENGTH];
	uint8_t multipart_boundary_length;
};

/** @brief Structure for CORS common authorization*/
typedef struct cors_auth_common_s
{
	uint8_t* origin_buffer;
	uint8_t origin_length;
#ifdef __DIGEST_ACCESS_AUTH__
	Digest_data* digest_ptr;
#endif
} cors_auth_common_s;
/** @brief Structure for Hams parser*/
typedef struct hams_parser_s
{
	/* User settable. */
	hams_data_cb user_data_cb;

	uint16_t response_code;
	/* Read only by user. */
	uint16_t flags;

	unsigned content_length;
	unsigned int https : 1;
	unsigned int method : 4;
	unsigned int content_type : 3;
	unsigned int http_auth_type : 2;
	unsigned int eating : 2;
	unsigned int cur_header : 6;
	unsigned int transmission_continue : 1;
	unsigned int origin_header_received : 1;
	unsigned int read_only_param : 1;
	unsigned int redir_custom_uri : 1;
	/* Only read by the multiplexer. */
	uint8_t buffer_expect;

	/* Private. */
	uint8_t buffer_size;
	uint8_t cur_label;
	uint8_t present;

	/* Number of data blocks allocated for large data service (like rs/param, assembly, language file) */
	uint8_t blocks_for_data;

	/* data_transmitted represents,
	   - Number of parameters transmitted in case of "GET rs/param" and "GET rs/assy" URI
	   - Number of data chunks transmitted while transmitting language file content*/
	uint32_t data_transmitted;

	uint32_t total_content_length;		///< Total length of the data to be tranmitted
	uint32_t general_use_reg;		///< Used for Ip address and pDCI ptr in enum
	uint16_t remote_port;

	hams_data_cb user_data_cb_remaining;

	cors_auth_common_s* cors_auth_common;
	struct hams_extended ext;

} hams_parser;

/** @brief Callback into user code when HAMS wants to transmit.
 *  @param[in] parser Parser instance
 *  @param[in] data Pointer to data to send; If NULL, hang up the sending connection
 *  @param[in] length How much data to send
 *  @param[in]copy 1 to make a local copy of the data, 0 to not copy (eg readonly)
 *  @return[out]0 on success; 1 if a retry is needed later; otherwise unrecoverable error. */
typedef unsigned (* hams_send_cb)( hams_parser* parser, const uint8_t* data, unsigned length, unsigned copy );

/** @brief Called when comparing a request's etag to the URL's etag. */
typedef void (* hams_etag_cb)( hams_parser* parser,
							   const uint8_t* etag, unsigned length );

/** @brief Callback into user code when HAMS authenticates
 *  @param[in] user_data User data associated with stream
 *  @param[in] base64_encoded Base64 encoded username:password
 *  @param[in] len number of base64_encoded bytes
 *  @return[out] 0 if acceptable, 1 if not. */
#ifdef __DIGEST_ACCESS_AUTH__
typedef uint8_t (* hams_auth_cb)( uint8_t* auth_level, uint8_t* user_id, uint8_t* max_auth_exempt_level,
								  Digest_data* p );
#endif

/** @brief Structure for managing multiple HTTP streams. */
extern struct hams_demultiplexer_s
{

	/** @brief Initial callback for handler. Expect the user_data parameter to be NULL. Should populate the user
	   parameter. */
	hams_data_cb init_cb;

	/** @brief Data sending function. */
	hams_send_cb send_cb;

	/** @brief ETAG handling function. If NULL then not handling ETAGs.*/
	hams_etag_cb etag_cb;

	/** @brief Authentication function. If NULL then server never authenticates. */
	hams_auth_cb auth_cb;

	/** @brief Start of shared output staging buffer. */
	uint8_t* output_buffer;

	/** @brief Number of bytes written to output buffer.. */
	unsigned staged_len;

	/** @brief Pointer to server hostname, most likely the IP address. */
	const char* hostname;

}* g_hams_demultiplexer;

/* Called by the server layer. */

/** @brief Initialize HAMS. Space must be allocated for g_hams_demultiplexer */
void hams_init( void );

/** @brief Initialize HAMS parser
 *  @param[in]parser Parser structure
 *  */
void hams_parser_init( hams_parser* parser, bool_t https_conn );

/** @brief Receive data from remote.
 *  @param[in] stream Opaque stream object associated with connection.
 *  @param[in]data Data from remote; if NULL then no more data.
 *  @param[in] length How much data; if data == NULL then irrelevant
 *  @return[out] Bitmask of HAMS_EXPECT_* values. */
unsigned hams_recv( hams_parser* stream, const uint8_t* data, unsigned length );

/** @brief           Start HAMS response
 *  @param[in]parser Parser structure
 **/
void hams_start_response( hams_parser* parser );

/** @brief Use this callback for echoing; expects query variables to parse. */
unsigned hams_echo_query( hams_parser* parser, const uint8_t* data,
						  unsigned length );

/** @brief Check Hams Integrity*/
uint8_t hams_integrity_check( const hams_parser* parser );

/* Called by the user callbacks */

/** @brief Write header line */
void hams_response_header( hams_parser* parser, uint32_t header_id,
						   const void* data, unsigned data_size );

/** @brief     Function for custom headers
 * @param[in]  Response and data size */
uint8_t hams_custom_headers( hams_parser* parser, const char* response, unsigned data_size );

#ifdef __DIGEST_ACCESS_AUTH__
int s_auth_cb( void );

#endif

/* A support function to avoid errors while using Http_Server_Get_State */
uint8_t Get_Auth_Level( hams_parser* parser );

/** @brief Called when sending an response code >=400 and no custom headers or body required.
 * (Note: 404 responses to file requests are not going to use this function, since those respond with an html body) */
static inline void hams_respond_error( hams_parser* parser, unsigned code )
{
	if ( code == 401U )
	{
		uint8_t val;
		val = Get_Auth_Level( parser );
		if ( val != 0U )
		{
			/* user has valid credentials but not authorized to access the resource. return 403 forbidden errorcode. */
			code = 403U;
		}
	}
	parser->response_code = code;
	parser->user_data_cb = NULL;
	parser->flags |= HAMS_FLAG_ERROR;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static inline unsigned hams_has_uri_part( const uint8_t* data, unsigned length )
{
	return ( data && length );
}

/**@brief Function to check if headers have been completeley parsed.
 * @param[in] data
 * @param[in] length
 */
static inline unsigned hams_check_headers_ended( const uint8_t* data, unsigned length )
{
	return ( data && *data == '\0' && ( 0 == length ) );
}

/**@brief  Function to check if hams can respond
 * @param[in]  data
 * @param[in]  length
 */
static inline unsigned hams_can_respond( const uint8_t* data, unsigned length )
{
	return ( !data && !length );
}

/**@brief  Function to check for hams comm errors.*/
static inline unsigned hams_check_comm_error( const uint8_t* data, unsigned length )
{
	return ( !data && 1 == length );
}

/** String constants of various http methods supported (useful to the user). */
extern const char* s_http_methods[HTTP_METHOD_MAX + 1];

/** Interface functions supplied by user. */

/** @brief Extend the length of the shared output buffer.
 *  @param extended_len New length of the output buffer. If 0 then frees the memory.
 *  @return Actual amount of memory available. If return value < extended_len then allocation failure (unable to extend)
 * If return value >= extended_len then success. */
extern unsigned Hamsif_Extend_Output( hams_parser* parser, unsigned extended_len );

char_t* read_login_user( hams_parser* parser );

#endif
