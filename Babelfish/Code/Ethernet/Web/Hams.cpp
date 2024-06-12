/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "hams.h"
#include "stdio.h"
#include "HttpsConfig.h"
#include "server.h"
#include "stdlib.h"
#include "Server_IO.h"
#include "User_Session.h"
#include "Eth_if.h"
#include "Change_Tracker.h"
#include "DCI_Enums.h"
#include "StdLib_MV.h"
#include "Ethernet_Debug.h"

#ifdef HAMS_DEBUG
#include <assert.h>
#include <stdio.h>
#define DERR( ... ) fprintf( stderr, __VA_ARGS__ )
#define _ASSERT( x ) assert( x )
#else
#define DERR( ... )
#define _ASSERT( x ) do {if ( !( x ) ){while ( 1 ){;} \
						 } \
} while ( 0 )
#endif

// #define CORS_ALLOW_ALL_ORIGIN    //define when need to have Access-Control-Allow-Origin :*; in the response

/* HACK to get around C++ not liking zero length arrays. */
struct real_hams_parser_s
{
	struct hams_parser_s p;
	char_t buffer[4];
};

enum
{
	EAT_NONE
	/* Dropping all space characters */
	, EAT_SPACE
	/* Dropping all characters until the end of the line. */
	, EAT_EOL,
	COUNT_EATS
};

// enum digest_data_parser_state
// {
#ifdef __DIGEST_ACCESS_AUTH__
/** @brief  Here we extract the user name */
static const uint8_t USERNAME_STATE = 1U;
/** @brief  Here we extract realm */
static const uint8_t REALM_STATE = 2U;
/** @brief  Here we extract Nonce */
static const uint8_t NONCE_STATE = 3U;
/** @brief  Here we extract algorithm type */
static const uint8_t ALGO_STATE = 5U;
/** @brief  Here we extract encoded response */
static const uint8_t RESPONSE_STATE = 6U;
/** @brief  Here we extract opaque */
static const uint8_t OPAQUE_STATE = 7U;
/** @brief  Here we extract qop */
static const uint8_t QOP_STATE = 8U;
/** @brief  Here we extract NC */
static const uint8_t NC_STATE = 9U;
/** @brief  Here we extract cnonce */
static const uint8_t CNONCE_STATE = 10U;
/* * @brief Max state reached
 * @brief  Here we extract URI*/
static const uint8_t URI_STATE = 4U;
static const uint8_t MAX_STATE = 11U;
#endif
/** Do not add any enum after this */
// };
/* Internal enums user does not care about. */
enum
{
	HTTP_CONNECTION_TYPE_CLOSE,
	HTTP_CONNECTION_TYPE_KEEP_ALIVE,
	COUNT_HTTP_CONNECTION_TYPES
};

/* String tables. */
const char_t* s_http_methods[HTTP_METHOD_MAX + 1] =
{
	"DELETE ",
	"GET ",
	"HEAD ",
	"OPTIONS ",
	"POST ",
	"PUT ",
	""
};
#define S_MAX_METHOD_LENGTH sizeof( "OPTIONS " )

enum
{
	CF_LWS = 0x01,
	CF_SP = 0x02,
	CF_TERM = 0x04,
	CF_URI_SEP = 0x10,
	CF_TOK_SEP = 0x10
};

/* NOTE: Response-only headers are properly CAPITALIZED because comparisons for request headers occur only in lower
   case. */
static const char_t* const s_http_hdrs[COUNT_HTTP_HDRS + 1] =
{
	"Access-Control-Allow-Methods:",
	"Content-Encoding:",
	"Location:",
	"WWW-Authenticate:"

	, "X-Request-Access-Level:",
	"authorization:",
	"connection:",
	"content-disposition:"

	, "content-length:",
	"content-type:",
	"etag:",
	"if-none-match:"

	, "origin:",
	"l"
};
#define S_MAX_HEADER_LENGTH 40
// #define S_MAX_HEADER_VALUE_LENGTH  64

#define MIN_MULTIPART_BOUNDARY_LENGTH 4
#define MIN_MPFD_BUFFLEN( x ) ( ( x ) + 8 )

static const uint8_t s_expected_lengths[COUNT_HTTP_HDRS] =
{
	0,
	0,
	0,
	0

	, 0,
	36,
	16,
	32

	, 8,
	70 + 17,
	HTTP_ETAG_LEN + 4,
	HTTP_ETAG_LEN + 4,
	0x15
};

static const char_t* const s_http_connection_types[COUNT_HTTP_CONNECTION_TYPES + 1] =
{
	"close",
	"keep-alive",
	"l"
};

#ifdef JSON_ACTIVE
static const char_t* const s_http_content_types[COUNT_HTTP_CONTENT_TYPES + 1] =
{
	"application/json",
	"application/octet-stream",
	"application/x-www-form-urlencoded",
	"application/xml",
	"multipart/form-data",
	"text/plain",
	"l"
};
#else
static const char_t* const s_http_content_types[COUNT_HTTP_CONTENT_TYPES + 1] =
{
	"application/octet-stream",
	"application/x-www-form-urlencoded",
	"application/xml",
	"multipart/form-data",
	"text/plain",
	"application/json",
	"l"
};
#endif

/* These strings are prefixed by the binary value of the http code, for easy lookup. Note that we can map custom error
   codes to HTTP codes + custom text so 0x8000 could map to "503 Not enough memory"
 * */

static const char_t* const s_http_response_codes[] =
{
	/* Special flavor HTTP code to "gently" show the developer that he/she forgot to set the http response code. */
	"\x00\x00 555 Programmer Did Not Set Response"

	, "\x64\x00 100 Continue",
	"\x65\x00 101 Switching Protocols",
	"\xC8\x00 200 OK",
	"\xC9\x00 201 Created",
	"\xCA\x00 202 Accepted",
	"\xCB\x00 203 Non-Authoritative Information",
	"\xCC\x00 204 No Content",
	"\xCD\x00 205 Reset Content",
	"\xCE\x00 206 Partial Content",
	"\x20\x01 404 Not Found"

	/* 300 */
	, "\x2C\x01 300 Redirection",
	"\x2D\x01 301 Moved Permanently",
	"\x2E\x01 302 Found",
	"\x2F\x01 303 See Other",
	"\x30\x01 304 Not Modified",
	"\x31\x01 305 Use Proxy"
	/* 306 is reserved. */
	, "\x33\x01 307 Temporary Redirect"

	/* 400 */
	, "\x90\x01 400 Bad Request",
	"\x91\x01 401 Authorization Required",
	"\x92\x01 402 Payment Required",
	"\x93\x01 403 Forbidden",
	"\x94\x01 404 Not Found",
	"\x95\x01 405 Method Not Allowed",
	"\x96\x01 406 Not Acceptable",
	"\x97\x01 407 Proxy Authentication Required",
	"\x98\x01 408 Request Timeout",
	"\x99\x01 409 Conflict",
	"\x9A\x01 410 Gone",
	"\x9B\x01 411 Length Required",
	"\x9C\x01 412 Precondition Failed",
	"\x9D\x01 413 Request Entity Too Large",
	"\x9E\x01 414 Request-URI Too Long",
	"\x9F\x01 415 Unsupported Media Type",
	"\xA0\x01 416 Requested Range Not Satisfiable",
	"\xA1\x01 417 Expectation Failed",
	"\xA2\x01 418 I'm a teapot",			// Why not...
	"\xA3\x01 419 Authentication Timeout",
	"\xA4\x01 420 Made a request when busy",
	"\xAD\x01 429 Too Many Requests",
	"\xAE\x01 430 Resource Already Exists",
	"\xAF\x01 431 Request Header Fields Too Large"

	/* 500 */
	, "\xF4\x01 500 Internal Server Error",
	"\xF5\x01 501 Not Implemented",
	"\xF6\x01 502 Bad Gateway",
	"\xF7\x01 503 Service Unavailable",
	"\xF8\x01 504 Gateway Timeout",
	"\xF9\x01 505 HTTP Version Not Supported"

	/* Custom error signatures (using same response code, different messages) */
	, "\x00\x40 404 No Query Allowed",
	"\x01\x40 404 Bad Query Variable",
	"\x02\x40 404 Query Required",
	"\x03\x40 400 Bad Query /",
	"\x04\x40 400 Query Interrupted by space char",
	"\x05\x40 400 Bad Query =",
	"\x06\x40 400 Bad Query ?",
	"\x07\x40 400 Bad Query &",
	"\x08\x40 400 Bad HTTP Major Version",
	"\x09\x40 400 Bad HTTP Minor Version",
	"\x0A\x40 400 Content Length Overlong",
	"\x0B\x40 400 Unparseable Content Length",
	"\x0C\x40 400 Bad Line Termination",
	"\x0D\x40 400 Bad Connection-Type",
	"\x0E\x40 400 Bad Auth Type",
	"\x0F\x40 400 Bad Basic Auth Format",
	"\x10\x40 400 Bad If-None-Match",
	"\x11\x40 400 Bad HTTP Header",
	"\x12\x40 400 Bad Multipart Boundary",
	"\x13\x40 400 Bad Disposition",
	"\x14\x40 400 Bad Content Boundary",
	"\x15\x40 400 Content Underrun",
	"\x16\x40 400 Content-Type Missing",
	"\x17\x40 400 Bad Multipart Type",
	"\x18\x40 400 Bad Origin"

	, "\x30\x40 400 MB Illegal Function Code",
	"\x31\x40 400 MB Data Address Error",
	"\x32\x40 400 MB Data Value Error",
	"\x33\x40 400 MB String Size Wrong"

	, "\x40\x40 400 XML EOF Error",
	"\x41\x40 400 XML Invalid Char",
	"\x42\x40 400 XML Bad Close Tag",
	"\x43\x40 400 XML Stack Error",
#ifdef JSON_ACTIVE
	"\x44\x40 400 JSON Syntax Error"
#else
	"\x44\x40 400 XML Syntax Error"
#endif

	, "\x30\x50 503 MB Device Failure Error",
	"\x31\x50 503 MB Device ACK Error",
	"\x32\x50 503 MB Device Busy Error",
	"\x33\x50 503 MB Intercom Block Error",
	"\x34\x50 503 MB No Slave Response"

	, "\x00\x50 503 Dynamic Output Alloc Error",
#ifdef JSON_ACTIVE
	"\x01\x50 503 JSON Parser Alloc Error",
#else
	"\x01\x50 503 XML Parser Alloc Error",
#endif
	"\x02\x50 503 Scratch Space Alloc Error"

	, "\x03\x50 500 REST Invalid command requested",
	"\x04\x50 500 REST Access violation",
	"\x05\x50 500 REST Exceeds range",
	"\x06\x50 500 REST Read Only",
	"\x07\x50 500 REST NV Value Inaccessible",
	"\x08\x50 500 REST NV Access Failure",
	"\x09\x50 500 REST NV Value Read Only",
	"\x0A\x50 500 REST Default not available",
	"\x0B\x50 500 REST Range not available",
	"\x0C\x50 500 REST Enum not available",
	"\x0D\x50 500 REST Invalid data length",
	"\x0E\x50 500 REST Value locked"

	/* Overloaded 422 Session Responses */
	, "\x20\x42 422 Trying to Delete NonExistant Resource",
	"\x21\x42 422 Session Id Mismatch",
	"\x22\x42 422 Invalid Session Timeout Value",
	"\x23\x42 422 Invalid Encoding Type",
	"\x24\x42 422 Decryption Failed",
	"\x25\x42 422 Session Does not exist",
	"\x26\x42 422 Size Mismatch",
	"\x27\x42 422 Out of Order Chunk",
	"\x28\x42 422 SFU Session in Progress",
	"\x29\x42 422 Host Not Ready To Accept Request",
	"\x30\x42 422 Host firmware Upload Error",
	"\x40\x42 422 One Admin Role is manadatory in System",
	"\x41\x42 422 User Self-Deletion Not Permitted",
	"\x42\x42 422 Username Already Exists",
	"\x43\x42 422 User Database Full",
	"\x44\x42 422 Password Security Violation",
	"\x45\x42 422 Password Matches With Name Or Existing Password"

	/* User_Session Related Response */
	, "\x60\x42 423 User Locked Temporarily",
	"\x61\x42 401 Invalid User Credentials"

};
#define VARY_FIELD "Vary: Origin\r\n"
#ifdef CORS_ALLOW_ALL_ORIGIN
#define ACCESS_CONTROL_ALLOW_ORIGIN_STR     "Access-Control-Allow-Origin: http://localhost:9001\r\n"
#else
#define ACCESS_CONTROL_ALLOW_ORIGIN_STR     "Access-Control-Allow-Origin: "
#endif
#define CORS_COMMON_FIELD "Access-Control-Allow-Credentials: true\r\n" \
	"Access-Control-Expose-Headers: WWW-Authenticate\r\n"

#define CORS_OPTIONS_FIELD \
	"Access-Control-Allow-Headers: accept, authorization, content-type, WWW-Authenticate\r\n" \
	"Access-Control-Max-Age: 1728000\r\n"

/* Switch labels for hams_parser_feed */
enum
{
	ST_FEED_INITIAL,
	ST_URI_PROTOCOL,
	ST_URI_COMPONENT,
	ST_HTTP_VERSION,
	ST_HEADER_FIELD,
	ST_HEADER_VALUE,
	ST_DISPOSITION_NAME,
	ST_CONTENT_TYPE_BOUNDARY,
	ST_MPFD_FIND_BOUNDARY,
	ST_USER_CONTENT,
	ST_DISCARD_CASE,
	COUNT_ST_LABELS
};

/* This field is basically used for resetting all the incomplete flags. */
static const char_t s_boiler_plate[] =
	"Server: HAMS_BF/0.1\r\n"
	"Content-Security-Policy: script-src 'self' 'unsafe-inline'; font-src 'self' data:; img-src 'self' data:;object-src 'none'; worker-src blob:;\r\n"
	"X-Content-Type-Options: nosniff\r\n"
	"X-XSS-Protection: 1; mode=block\r\n"
	"X-Frame-Options: SAMEORIGIN\r\n"
	"Cache-Control: max-age=0,no-cache,no-store,post-check=0,pre-check=0\r\n"
	"Expires: Mon, 26 Jul 1997 05:00:00 GMT\r\n"
	"Pragma: no-cache\r\n";

/* HTTPS server will keep SSL connection open for given timeout ( Bug LTK-3621 ) */
static const char_t s_keep_alive_plate[] =
	"Connection: Keep-Alive\r\n"
	"Keep-Alive: timeout=005, max=100\r\n";

/* This field is used for forming http response header for non-persisting connection. Keep-Alive disabled. */
static const char_t s_connection_close_plate[] =
	"Connection: close\r\n";

static const uint8_t s_char_flags[256] =
{
	/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
	0, 0, 0, 0, 0, 0, 0, 0
	/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
	, 0, CF_LWS, CF_TERM, 0, 0, CF_TERM, 0, 0
	/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
	, 0, 0, 0, 0, 0, 0, 0, 0
	/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
	, 0, 0, 0, 0, 0, 0, 0, 0
	/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
	, CF_LWS | CF_SP | CF_URI_SEP, 0, 0, 0, 0, 0, CF_URI_SEP, 0

	/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
	, CF_TOK_SEP, CF_TOK_SEP, 0, 0, CF_TOK_SEP, 0, 0, CF_URI_SEP
	/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
	, 0, 0, 0, 0, 0, 0, 0, 0
	/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
	, 0, 0, CF_TOK_SEP, CF_TOK_SEP, CF_TOK_SEP, CF_URI_SEP | CF_TOK_SEP, CF_TOK_SEP,
	CF_URI_SEP
	| CF_TOK_SEP
	/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
	, CF_TOK_SEP, 0, 0, 0, 0, 0, 0, 0
	/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
	, 0, 0, 0, 0, 0, 0, 0, 0
	/*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
	, 0, 0, 0, 0, 0, 0, 0, 0
	/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
	, 0, 0, 0, 0, CF_TOK_SEP, 0, 0, 0
	/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
	, 0, 0, 0, 0, 0, 0, 0, 0
	/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
	, 0, 0, 0, 0, 0, 0, 0, 0
	/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
	, 0, 0, 0, 0, 0, 0, 0, 0
	/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
	, 0, 0, 0, CF_TOK_SEP, 0, CF_TOK_SEP, 0, 0
};

#define HTTP_STR "http://"
#define HTTPS_STR "https://"

const uint8_t* const protocol_str[2] =
{
	reinterpret_cast<const uint8_t*>( HTTP_STR ),
	reinterpret_cast<const uint8_t*>( HTTPS_STR )
};
const uint8_t protocol_str_length[2] = { STRLEN( HTTP_STR ), STRLEN( HTTPS_STR ) };

static DCI_Owner* cors_origin_type;
extern DCI_Owner* active_ip_address_owner;
static uint32_t s_hams_end_response( hams_parser* parser, const uint8_t* data,
									 uint32_t length );

#ifdef __DIGEST_ACCESS_AUTH__
static uint8_t Digest_Data_Extract( const uint8_t* x, uint8_t* rx_buffer, uint8_t* rx_length,
									bool_t* receive_flag, uint32_t buffer_length,
									uint8_t* digest_state );

#endif
static User_Session* m_user_session_handler = nullptr;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Session_Init( User_Session* user_session )
{
	m_user_session_handler = user_session;
}

/* Do a binary search over a range of alphabetically sorted keys. */
static uint8_t s_util_match_key( const char_t* const* key_set, uint32_t key_count,
								 const uint8_t* datum, uint32_t datum_len )
{
	bool_t return_flag = INVALID;
	uint8_t return_val = INVALID;
	/* Supremum is the least element GREATER than the datum. */
	uint32_t maximum = key_count - 1;
	/* Minimum is the greatest element LESS THAN OR EQUAL to the datum. */
	uint32_t minimum = 0;

	const uint8_t* x = datum;
	const uint8_t* end = datum + datum_len;

	while ( ( x != end ) && ( INVALID == return_flag ) )
	{

		/* Check for an exact match. */
		if ( '\0' == key_set[minimum][x - datum] )
		{
			return_val = ( uint8_t )minimum;
			return_flag = VALID;
		}
		else
		{
			/* Set alphabetic character to lower case if appropriate. */
			uint8_t test = *x;
			if ( key_set[key_count][0] == 'l' )
			{
				if ( ( test >= 'A' ) && ( test <= 'Z' ) )
				{
					test |= 0x20;
				}
			}

			while ( minimum <= maximum && test > key_set[minimum][x - datum] )
			{
				++minimum;
			}

			if ( test != key_set[minimum][x - datum] )
			{
				return_val = ( uint8_t )( key_count );
				return_flag = VALID;
			}
			else
			{
				while ( minimum <= maximum && test < key_set[maximum][x - datum] )
				{
					--maximum;
				}
				/* Go onto next character. */
				++x;
			}
		}
	}
	if ( INVALID == return_flag )
	{
		/* At this point, we reached the end of the datum. However if there are more characters in the key then this is
		   NOT a match. */
		if ( key_set[minimum][x - datum] )
		{
			return_val = ( uint8_t )( key_count );
		}
		else
		{
			return_val = ( uint8_t )( minimum );
		}
	}
	return ( return_val );
}

/* @param buffer_length Must be at least parser->ext.multipart_boundary_length + 6
 * @return 0 on error, 1 if terminating boundary; 2 if boundary indicates content follows. */
static uint8_t s_check_multipart_boundary( hams_parser* parser,
										   const uint8_t* buffer_data, uint32_t buffer_length )
{
	bool_t return_flag = INVALID;
	uint8_t return_val;

	/* Check for premature termination. */
	if ( ( parser->ext.multipart_boundary_length > ( ( buffer_length + 4 ) + 4 ) ) ||
		 ( *( const uint32_t* ) ( buffer_data ) != BOUND_MATCH_PREFIX ) ||
		 ( memcmp( parser->ext.multipart_boundary,
				   buffer_data + 4, parser->ext.multipart_boundary_length ) ) )
	{
		return_val = 0;
	}
	else
	{
		/* Assume this is not a final boundary. */
		return_val = 2;

		/* Next two chars must be '--' xor '\r\n' */
		const uint8_t* x = ( ( buffer_data + parser->ext.multipart_boundary_length ) + 4 );
		if ( *x == '-' )
		{
			++x;
			if ( *x != '-' )
			{
				return_flag = VALID;
				return_val = 0U;
			}
			else
			{
				++x;
				return_val = 1U;
			}
		}

		if ( INVALID == return_flag )
		{
			/* Look for ending \r\n */
			if ( *x == '\r' )
			{
				return_val = ( *( x + 1 ) == '\n' ) ? return_val : 0;
			}
			else
			{
				return_val = 0;
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void hams_parser_init( hams_parser* parser, bool_t https_conn )
{
	parser->flags = 0;
	parser->https = ( https_conn == true ) ? HTTPS_MEDIA : HTTP_MEDIA;
	parser->redir_custom_uri = 0U;
	parser->method = HTTP_METHOD_MAX;
	parser->content_length = 0;
	parser->content_type = COUNT_HTTP_CONTENT_TYPES;
	parser->http_auth_type = COUNT_HTTP_AUTH_TYPES;
	parser->response_code = 0;
	parser->eating = EAT_NONE;
	parser->present = 0;

	parser->ext.multipart_boundary_length = 0;

	parser->cur_label = ST_FEED_INITIAL;
	parser->cur_header = COUNT_HTTP_HDRS;

	/* Expecting METHOD as first field. */
	parser->buffer_expect = S_MAX_METHOD_LENGTH;
	parser->transmission_continue = INVALID;
	parser->blocks_for_data = 0;
	parser->data_transmitted = 0;
	parser->cors_auth_common = NULL;
	parser->origin_header_received = 0U;
	parser->read_only_param = 0;
	parser->cors_auth_common = new
		cors_auth_common_s;
	if ( NULL != parser->cors_auth_common )
	{
		parser->cors_auth_common->origin_buffer = NULL;
		parser->cors_auth_common->origin_length = 0;

#ifdef __DIGEST_ACCESS_AUTH__
		parser->cors_auth_common->digest_ptr = NULL;
		parser->cors_auth_common->digest_ptr = new Digest_data;
		if ( NULL != parser->cors_auth_common->digest_ptr )
		{
			parser->cors_auth_common->digest_ptr->state = MAX_STATE;
		}
#endif
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t hams_integrity_check( const hams_parser* parser )
{
	uint8_t return_value;

	if ( parser->method > HTTP_METHOD_MAX )
	{
		return_value = 0U;
	}
	else if ( parser->cur_label >= COUNT_ST_LABELS )
	{
		return_value = 0U;
	}
	else if ( parser->eating >= COUNT_EATS )
	{
		return_value = 0U;
	}
	else
	{
		return_value = 1U;
	}
	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static int32_t hams_parser_feed( hams_parser* parser, const uint8_t* buffer_data,
								 uint32_t buffer_length )
{
	int32_t return_val = 0;
	bool_t function_end = INVALID;
	bool_t content_type_boundary = INVALID;
	bool_t disposition_type = INVALID;
	bool_t discard = INVALID;

	/* If eating spaces, then seek until non space encountered. */
	switch ( parser->eating )
	{
		case EAT_SPACE:
		{
			const uint8_t* x = buffer_data;
			const uint8_t* end = x + buffer_length;

			/* If there is no space, then not eating. */
			if ( *x != ' ' )
			{
				parser->eating = EAT_NONE;
			}
			else
			{

				/* Look for first non-space character. */
				while ( ( x != end ) && ( INVALID == function_end ) )
				{
					if ( *x != ' ' )
					{
						parser->eating = EAT_NONE;
						// DERR("EAT_SPACE %lu\n", x - buffer_data);
						return_val = ( x - buffer_data );
						function_end = VALID;
					}
					else
					{
						++x;
					}
				}
				if ( INVALID == function_end )
				{
					// DERR("EAT_SPACE %u\n", buffer_length);
					return_val = ( int32_t )( buffer_length );
					function_end = VALID;
				}
			}
		}
		break;

		case EAT_EOL:
		{
			const uint8_t* x = ( uint8_t* ) ( memchr( buffer_data, '\n', buffer_length ) );	// SP

			if ( x )
			{
				parser->eating = EAT_NONE;
				// DERR("EAT_EOL TERM %lu, REMAIN %.*s\n", x - buffer_data + 1
				// , buffer_length - (x - buffer_data), x + 1);
				return_val = ( ( x - buffer_data ) + 1 );
				function_end = VALID;
			}
			else
			{

				// DERR("EAT_EOL %u\n", buffer_length);
				return_val = ( int32_t )( buffer_length );
				function_end = VALID;
			}

		}
		break;

		default:
		case EAT_NONE:
			break;
	}

	if ( INVALID == function_end )
	{

		/* Optimizing for code size, so no unwieldy switch tables here. Instead we go back to the same rights computing
		   pioneers had:
		 * Setting the PC directly with a value. This is just the GCC way to do it.
		 *
		 * Known as label variables in GCC This may not work with other compilers though.... */
		switch ( parser->cur_label )
		{
			/* Direct jumps; because these labels are in a subcase... */
			case ST_CONTENT_TYPE_BOUNDARY:
				content_type_boundary = VALID;
				break;

			case ST_DISPOSITION_NAME:
				disposition_type = VALID;
				break;

			case ST_FEED_INITIAL:
				/* Label is not set, so we are in the initial state. */
				parser->method = s_util_match_key( s_http_methods, HTTP_METHOD_MAX,
												   buffer_data, buffer_length );

				/* If method is unrecognized return 501 */
				if ( HTTP_METHOD_MAX <= parser->method )
				{
					hams_respond_error( parser, 501 );
					discard = VALID;
				}
				else
				{

					/* Method is recognized, parse URI protocol and IP next.
					 * only expect http://IP0.IP1.IP2.IP3:PORT */
					parser->cur_label = ST_URI_PROTOCOL;
					parser->buffer_expect = MAX_URI_SEG_LENGTH;
					discard = INVALID;
					return_val = strlen( ( const char_t* )s_http_methods[parser->method] );
				}
				function_end = VALID;
				break;

			case ST_URI_PROTOCOL:
				DERR( "ST_URI_PROTOCOL\n" );
				if ( '*' == buffer_data[0] )
				{
					/* next character must be space. */
					if ( ' ' == buffer_data[1] )
					{
						/* TODO */
					}
					hams_respond_error( parser, 404 );
					discard = VALID;
				}
				else
				{

					if ( '/' == buffer_data[0] )
					{
						parser->cur_label = ST_URI_COMPONENT;
						discard = INVALID;
						return_val = 0;
					}
					else
					{
						if ( memcmp( buffer_data, "http:/", 6 ) || ( '/' != buffer_data[6] ) )
						{
							hams_respond_error( parser, 404 );
							discard = VALID;
						}
						else
						{
							/* Strip off http:/ */
							parser->cur_label = ST_URI_COMPONENT;
							discard = INVALID;
							return_val = 6;
						}
					}
				}
				function_end = VALID;
				break;

			case ST_URI_COMPONENT:
			{
				// _ASSERT( s_char_flags[buffer_data[0]] & CF_URI_SEP );

				/* Find end of URI segment */
				const uint8_t* x = buffer_data + 1;
				if ( ( parser->https == HTTP_MEDIA ) && ( false == Is_Conn_Enabled( HTTP_MEDIA ) ) )
				{
					if ( parser->redir_custom_uri == 1 )
					{
						// This is not first Iteration and we want to copy all data
						x = buffer_data;
					}
					// if server want to redirect to https parse and save the URI at one go.
					while ( ( x != ( buffer_data + buffer_length ) ) && ( *x != ' ' ) )
					{
						++x;
					}
					if ( x < ( buffer_data + buffer_length ) )
					{
						/* HTTP version will be upcoming. */
						parser->cur_label = ST_HTTP_VERSION;
					}
					else
					{
						// hack: keep one valid chararcter in buffer to be read next time along with
						// space character, Fix the problem when URI and buffer_length is same.
						x--;
					}
					if ( true == Is_Conn_Enabled( HTTPS_MEDIA ) )
					{
						if ( 0U ==
							 Redirect_To_Https_Server( parser, buffer_data, x - buffer_data ) )
						{
							return_val = ( x - buffer_data );
						}
						else
						{
							discard = VALID;
						}
					}
					else
					{
						hams_respond_error( parser, 503U );
						discard = VALID;
					}
				}
				else
				{
					while ( x != buffer_data + buffer_length &&
							!( s_char_flags[*x] & CF_URI_SEP ) )
					{
						++x;
					}
					DERR( "ST_URI_COMPONENT \"%.*s\"\n", x - buffer_data, buffer_data );

					/* URI component is too long. */
					if ( x == buffer_data + buffer_length )
					{
						hams_respond_error( parser, 414 );
						discard = VALID;
					}
					else
					{

						if ( '/' == *x )
						{
							/* Only a '/' may precede this token. */
							if ( buffer_data[0] != '/' )
							{
								hams_respond_error( parser, E400_URI_SLASH );
								discard = VALID;
							}
						}
						else if ( ' ' == *x )
						{
							/* Cannot interrupt a query variable. */
							if ( ( buffer_data[0] == '?' ) || ( buffer_data[0] == '&' ) )
							{
								hams_respond_error( parser, E400_INTR_QUERY );
								discard = VALID;
							}
							if ( INVALID == discard )
							{
								/* HTTP version will be upcoming. */
								parser->cur_label = ST_HTTP_VERSION;
							}
						}
						else if ( '=' == *x )
						{
							/* Query variable value, only ? or & can precede. */
							if ( ( buffer_data[0] != '?' ) && ( buffer_data[0] != '&' ) )
							{
								hams_respond_error( parser, E400_BAD_QUERY_EQUALS );
								discard = VALID;
							}

							/* TODO If processing _method, make sure this is a valid HTTP method. */

						}
						else
						{
							if ( '?' == *x )
							{
								/* Only a '/' may precede this token. */
								if ( buffer_data[0] != '/' )
								{
									hams_respond_error( parser, E400_BAD_QUERY_QUESTION_MARK );
									discard = VALID;
								}
							}
							else
							{
								/* This is the '&' character, must be preceded by '=' */
								if ( buffer_data[0] != '=' )
								{
									hams_respond_error( parser, E400_BAD_QUERY_AMPERSAND );
									discard = VALID;
								}
							}

							/* TODO Check for _method. */
						}
						if ( INVALID == discard )
						{
							parser->user_data_cb( parser, buffer_data, x - buffer_data );

							if ( parser->response_code >= 400 )
							{
								DERR( "URI ERR %u\n", parser->response_code );
								discard = VALID;
							}
							else
							{
								return_val = ( x - buffer_data );
							}
						}
					}
				}
			}
				function_end = VALID;
				break;

			case ST_HTTP_VERSION:
				// DERR("ST_HTTP_VERSION\n");
			{
				if ( memcmp( buffer_data, " HTTP/1.", 8 ) )
				{
					hams_respond_error( parser, E400_BAD_HTTP_VER_MAJOR );
					discard = VALID;
				}
				else
				{

					if ( ( '0' != buffer_data[8] ) && ( '1' != buffer_data[8] ) )
					{
						hams_respond_error( parser, E400_BAD_HTTP_VER_MINOR );
						discard = VALID;
					}
					else
					{

						parser->buffer_expect = S_MAX_HEADER_LENGTH;
						parser->cur_label = ST_HEADER_FIELD;

						/* Line must terminate. */
						if ( ( '\r' != buffer_data[9] ) || ( '\n' != buffer_data[10] ) )
						{
							hams_respond_error( parser, E400_BAD_LINE_TERM );
							discard = VALID;
						}
						else
						{

							/* Don't consume terminators here; we want to go into header eating mode.*/
							parser->eating = EAT_EOL;
							return_val = 9;
						}
					}
				}
			}
				function_end = VALID;
				break;

			case ST_HEADER_FIELD:
				DERR( "ST_HEADER_FIELD %u\n", buffer_length );
				{
					if ( buffer_length > 1U )
					{
						if ( ( ( '\r' == buffer_data[0] ) && ( buffer_data[1] == '\n' ) ) )
						{
							DERR( "HEADER COMPLETE\n" );
							if ( !( parser->flags & HAMS_FLAG_MULTIPART ) )
							{
								/* If there is content, mark it here. */
								if ( parser->content_length )
								{
									parser->flags |= HAMS_FLAG_CONTENT;
								}

								/* Inform user that headers have ended. */
								http_server_req_state* rs = Http_Server_Get_State( parser );
								rs->max_auth_exempt_level =
									m_user_session_handler->Get_Max_Auth_Role_Exemption_Level( rs->auth_level );
								parser->user_data_cb( parser, reinterpret_cast<const uint8_t*>( "" ), 0 );
								if ( parser->response_code >= 400 )
								{
									function_end = VALID;
									discard = VALID;
								}
								else
								{

									/* If not parsing multibody, let the user do the parsing.
									   drop the line terminators.*/
									if ( parser->content_type != HTTP_CONTENT_TYPE_MULTIPART_FORM )
									{
										parser->cur_label = ST_USER_CONTENT;
										parser->buffer_expect = 1U;

										/* HACK, layer above will subtract content length so add here to compensate.*/
										if ( parser->content_length &&
											 !( parser->flags & HAMS_FLAG_ECHO_CONTENT ) )
										{
											parser->content_length += 2;
										}
										function_end = VALID;
										return_val = 2U;
									}
									else
									{
										DERR( "SET UP FOR MULTIPART\n" );

										/* Multipart data to parse, initialize parsing state here.
										 */
										const uint32_t u = *( const uint32_t* ) ( parser->ext
																				  .
																				  multipart_boundary );
										parser->ext.boundary_match[0] = u;
										parser->ext.boundary_match[0] <<= 24;
										parser->ext.boundary_match[0] |=
											( BOUND_MATCH_PREFIX >> 8 );

										parser->ext.boundary_match[1] = u;
										parser->ext.boundary_match[1] <<= 16;
										parser->ext.boundary_match[1] |=
											( BOUND_MATCH_PREFIX >> 16 );

										parser->ext.boundary_match[2] = u;
										parser->ext.boundary_match[2] <<= 8;
										parser->ext.boundary_match[2] |=
											( BOUND_MATCH_PREFIX >> 24 );

										parser->flags |= HAMS_FLAG_MULTIPART
											| HAMS_FLAG_FIRST_BOUNDARY;

										/* Add two the content length, since we are counting final
										 * \r\n to the headers as part of the content when we parse.*/
										parser->content_length += 2;
									}
								}
							}
							if ( INVALID == function_end )
							{
								parser->cur_label = ST_MPFD_FIND_BOUNDARY;
								parser->buffer_expect =
									MIN_MPFD_BUFFLEN( parser->ext.multipart_boundary_length );

								/* If we got here, then consuming multipart data. Since we need leading \r\n for generic
								   case parsing to work, leave them in.*/
								/* FIXME */
								return_val = 0;
								function_end = VALID;
							}
						}
						else
						{

							/* Every header should end in a ':' */
							const uint8_t* p = ( uint8_t* ) ( memchr( buffer_data, ':',
																	  buffer_length ) );
							if ( !p )
							{
								// DERR("BAD FIELD '%.*s'\n", buffer_length, buffer_data);
								// hams_respond_error( parser, E400_BAD_HTTP_HEADER );
								function_end = VALID;
								// discard = VALID;
								return_val = 0;
							}
							else
							{
								uint32_t len = ( uint32_t )( p - buffer_data );
								uint32_t id = s_util_match_key( s_http_hdrs, COUNT_HTTP_HDRS,
																buffer_data, len + 1 );

								/* Don't process etags unless supPorted. */
								if ( ( HTTP_HDR_ETAG == id ) || ( HTTP_HDR_IF_NONE_MATCH == id ) )
								{
									if ( !g_hams_demultiplexer->etag_cb )
									{
										id = COUNT_HTTP_HDRS;
									}
								}

								if ( COUNT_HTTP_HDRS == id )
								{
									/* Unrecognized header, Just drop the rest of the line. */
									parser->eating = EAT_EOL;
								}
								else
								{
									parser->cur_header = id;
									parser->cur_label = ST_HEADER_VALUE;
									parser->eating = EAT_SPACE;
									parser->buffer_expect = s_expected_lengths[id];
								}

								DERR( "HEADER_NAME %u %.*s\n", id, p - buffer_data, buffer_data );

								return_val = ( int32_t )( len + 1 );
								function_end = VALID;
							}
						}
					}
					else
					{
						return_val = 0;
						function_end = VALID;
					}
				}
				break;

			case ST_HEADER_VALUE:
				DERR( "ST_HEADER_VALUE %u %.*s\n", buffer_length, buffer_length, buffer_data );
				{
					switch ( parser->cur_header )
					{
						case HTTP_HDR_AUTHORIZATION:
						{
							switch ( parser->http_auth_type )
							{
#ifdef __DIGEST_ACCESS_AUTH__
								case HTTP_AUTH_TYPE_DIGEST:
								case HTTP_AUTH_TYPE_CUSTOM_AUTH:
								{
									discard = INVALID;
									const uint8_t* x = buffer_data;
									Digest_data* digest_data_ptr =
										parser->cors_auth_common->digest_ptr;
									if ( !memcmp( buffer_data, "username=", 9 ) )
									{
										digest_data_ptr->state = USERNAME_STATE;
										return_val = 9;
										digest_data_ptr->rx_user_name_length = 0;
										function_end = VALID;
									}
									else if ( !memcmp( x, "realm=", 6 ) )
									{
										digest_data_ptr->state = REALM_STATE;
										return_val = 6;
										function_end = VALID;
									}
									else if ( !memcmp( x, "nonce=", 6 ) )
									{
										digest_data_ptr->state = NONCE_STATE;
										return_val = 6;
										function_end = VALID;
									}
									else if ( !memcmp( x, "uri=", 4 ) )	// URI parsing
									{
										digest_data_ptr->state = URI_STATE;
										return_val = 4;
										digest_data_ptr->rx_md5_uri_length = 0;
										function_end = VALID;
									}
									else if ( !memcmp( x, "algorithm=", 10 ) )	// algorithm type
																				// parsing
									{
										digest_data_ptr->state = ALGO_STATE;
										return_val = 10;
										function_end = VALID;
									}
									else if ( !memcmp( x, "response=", 9 ) )// Response parsing
									{
										digest_data_ptr->state = RESPONSE_STATE;
										return_val = 9;
										digest_data_ptr->rx_response_length = 0;
										function_end = VALID;
									}
									else if ( !memcmp( x, "opaque=", 7 ) )
									{
										digest_data_ptr->state = OPAQUE_STATE;
										return_val = 7;
										function_end = VALID;
									}
									else if ( !memcmp( x, "qop=", 4 ) )
									{
										digest_data_ptr->state = QOP_STATE;
										return_val = 4;
										function_end = VALID;
									}
									else if ( !memcmp( x, "nc=", 3 ) )
									{
										digest_data_ptr->state = NC_STATE;
										return_val = 3;
										function_end = VALID;
									}
									else if ( !memcmp( x, "cnonce=", 7 ) )
									{
										digest_data_ptr->state = CNONCE_STATE;
										return_val = 7;
										digest_data_ptr->rx_cnonce_length = 0;
										function_end = VALID;
									}
									uint8_t* loc_state = &digest_data_ptr->state;
									if ( INVALID == function_end )
									{
										bool_t data_received = INVALID;
										uint8_t data_length = 0U;
										switch ( digest_data_ptr->state )
										{
											case USERNAME_STATE:
											{
												return_val = Digest_Data_Extract( x, digest_data_ptr->rx_user_name,
																				  &digest_data_ptr->rx_user_name_length,
																				  &data_received, buffer_length,
																				  loc_state );
												if ( data_received == VALID )
												{
													digest_data_ptr->digest_flags.flag.user_name = VALID;
												}
												function_end = VALID;
											}

											break;

											case REALM_STATE:
											{
												uint8_t temp_realm_buf[WEB_REALM_LENGTH] = {0U};
												return_val = Digest_Data_Extract( x,
																				  temp_realm_buf, &data_length,
																				  &data_received, buffer_length,
																				  loc_state );
												if ( ( data_received == VALID ) && ( data_length == WEB_REALM_LENGTH ) )
												{
													digest_data_ptr->digest_flags.flag.realm = VALID;
													if ( 0U !=
														 strncmp( ( reinterpret_cast<char_t*>( temp_realm_buf ) ),
																  WEB_REALM, WEB_REALM_LENGTH ) )
													{
														digest_data_ptr->digest_flags.flag.realm = INVALID;
													}
												}
												function_end = VALID;
											}

											break;

											case NONCE_STATE:
											{
												uint8_t temp_nonce_buf[OUTPUT_NONCE_LENGTH] = {0U};
												return_val = Digest_Data_Extract( x,
																				  temp_nonce_buf, &data_length,
																				  &data_received, buffer_length,
																				  loc_state );
												if ( ( data_received == VALID ) &&
													 ( data_length == OUTPUT_NONCE_LENGTH ) )
												{
													digest_data_ptr->digest_flags.flag.nonce = VALID;
													if ( false ==
														 httpd_digest_authentication::From_32Hex_To_16Bytes(
															 temp_nonce_buf,
															 digest_data_ptr
															 ->
															 rx_nonce_buf ) )
													{
														digest_data_ptr->digest_flags.flag.nonce = INVALID;
													}
												}
												function_end = VALID;
											}
											break;

											case URI_STATE:
											{
												if ( buffer_length > MAX_URI_SEG_LENGTH )
												{

													if ( 0U == digest_data_ptr->rx_md5_uri_length )
													{
														uint8_t uri_len;
														uri_len =
															strlen( s_http_methods[parser->method] );
														memcpy( &digest_data_ptr->rx_md5_uri[0],
																s_http_methods[parser->method],
																( uri_len - 1 ) );
														digest_data_ptr->rx_md5_uri[uri_len - 1] = ':';
														digest_data_ptr->rx_md5_uri_length = uri_len;
													}
													return_val = Digest_Data_Extract( x,
																					  digest_data_ptr->rx_md5_uri,
																					  &digest_data_ptr->rx_md5_uri_length,
																					  &data_received,
																					  buffer_length,
																					  loc_state );
												}
												else
												{
													return_val = 0;
													data_received = INVALID;
												}
												if ( data_received == VALID )
												{
													digest_data_ptr->digest_flags.flag.uri = VALID;
												}
												function_end = VALID;
											}
											break;

											case ALGO_STATE:
											{
												return_val = Digest_Data_Extract( x, NULL,
																				  &data_length, &data_received,
																				  buffer_length,
																				  loc_state );
												if ( ( data_received == VALID ) && ( data_length == ALGORITHM_LENGTH ) )
												{
													digest_data_ptr->digest_flags.flag.algo = VALID;
												}
												function_end = VALID;
											}
											break;

											case RESPONSE_STATE:
											{

												return_val = Digest_Data_Extract( x,
																				  digest_data_ptr->rx_responce_encode,
																				  &digest_data_ptr->rx_response_length,
																				  &data_received,
																				  buffer_length,
																				  loc_state );
												if ( data_received == VALID )
												{
													digest_data_ptr->digest_flags.flag.response = VALID;
												}
												function_end = VALID;
											}
											break;

											case OPAQUE_STATE:
											{

												uint8_t temp_opaque_buf[OPAQUE_LENGTH] = {0U};
												return_val = Digest_Data_Extract( x,
																				  temp_opaque_buf, &data_length,
																				  &data_received, buffer_length,
																				  loc_state );

												if ( ( data_received == VALID ) && ( data_length == OPAQUE_LENGTH ) )
												{
													digest_data_ptr->digest_flags.flag.opaque = VALID;
													if ( 0U !=
														 strncmp( ( reinterpret_cast<char_t*>( temp_opaque_buf ) ),
																  httpd_digest_authentication::OPAQUE,
																  OPAQUE_LENGTH ) )
													{
														digest_data_ptr->digest_flags.flag.opaque = INVALID;
													}
												}
												function_end = VALID;
											}
											break;

											case QOP_STATE:
											{
												uint8_t temp_qop_buf[AUTH_QOP_LENGTH] = {0U};
												return_val = Digest_Data_Extract( x, temp_qop_buf,
																				  &data_length, &data_received,
																				  buffer_length,
																				  loc_state );
												if ( ( data_received == VALID ) && ( data_length == AUTH_QOP_LENGTH ) )
												{
													digest_data_ptr->digest_flags.flag.qop = VALID;
													if ( 0U !=
														 strncmp( ( reinterpret_cast<char_t*>( temp_qop_buf ) ),
																  AUTH_QOP,
																  AUTH_QOP_LENGTH ) )
													{
														digest_data_ptr->digest_flags.flag.qop = INVALID;
													}
												}
												function_end = VALID;
											}
											break;

											case NC_STATE:
											{
												return_val = Digest_Data_Extract( x,
																				  digest_data_ptr->rx_nc_response,
																				  &data_length,
																				  &data_received,
																				  buffer_length,
																				  loc_state );
												if ( ( data_received == VALID ) && ( data_length == NC_LENGTH ) )
												{
													digest_data_ptr->digest_flags.flag.nc = VALID;
												}
												function_end = VALID;
											}
											break;

											case CNONCE_STATE:
											{
												return_val = Digest_Data_Extract( x,
																				  digest_data_ptr->rx_cnonce_response,
																				  &digest_data_ptr->rx_cnonce_length,
																				  &data_received,
																				  buffer_length,
																				  loc_state );
												if ( data_received == VALID )
												{
													digest_data_ptr->digest_flags.flag.cnonce = VALID;
												}
												function_end = VALID;
											}
											break;

											default:
											{
												if ( ( *x == '\r' ) && ( *( x + 1 ) == '\n' ) )
												{
													/* Just ignore the rest of the line. */
													parser->buffer_expect = S_MAX_HEADER_LENGTH;
													parser->cur_label = ST_HEADER_FIELD;
													parser->eating = EAT_EOL;
													function_end = VALID;
													return_val = 2;
												}
												else
												{
													x += 1;
													return_val = 1;
												}
											}
											break;
										}
									}
									if ( ( digest_data_ptr->digest_flags.digest_all_flags & DIGEST_DATA_MASK ) ==
										 DIGEST_DATA_MASK )
									/* Here you have received complete data */
									{
										digest_data_ptr->digest_flags.digest_all_flags = 0U;
										digest_data_ptr->state = MAX_STATE;

										/* First char should be a ' '
										   Should also have '\r' char.*/
										const uint8_t* xy =
											( uint8_t* ) ( memchr( buffer_data, '\r',
																   buffer_length - 1 ) );
										if ( !xy )
										{
											/* No EOL...not good. */
											hams_respond_error( parser,
																E400_BAD_AUTH_FORMAT );
											function_end = VALID;
											discard = VALID;
										}
										else
										{

											/* Check for authentication */
											http_server_req_state* rs = Http_Server_Get_State(
												parser );
											if ( parser->method != HTTP_METHOD_OPTIONS )
											{
												USER_SESSION_ERR_TD user_auth_response;
												user_auth_response = m_user_session_handler->Authenticate_User(
													digest_data_ptr,
													&rs->user_id, &rs->auth_level,
													parser->http_auth_type );
												switch ( user_auth_response )
												{
													case USER_SESS_NO_ERR:
														rs->flags |= REQ_FLAG_AUTH_VALID;
														break;

													case USER_SESS_AUTH_REQUIRED_ERR:
														hams_respond_error( parser, 401U );
														function_end = VALID;
														discard = VALID;
														break;

													case USER_SESS_USER_LOCKED_ERR:
														hams_respond_error( parser, E423_USER_LOCKED_TEMPORARILY );
														function_end = VALID;
														discard = VALID;
														break;

													case USER_SESS_NO_SESSION_AVAILABLE_ERR:
														hams_respond_error( parser, 503U );
														function_end = VALID;
														discard = VALID;
														break;

													case USER_SESS_INTERNAL_ERR:
														hams_respond_error( parser, 500U );
														function_end = VALID;
														discard = VALID;
														break;

													case USER_SESS_UNKNOWN_ERR:
														hams_respond_error( parser, 401U );	///< Think about this
														function_end = VALID;
														discard = VALID;
														break;

													case USER_SESS_USER_NOT_AVAILABLE:
													case USER_SESS_INVALID_CRED_ERR:
														hams_respond_error( parser, E401_USER_INVALID_CRED );
														function_end = VALID;
														discard = VALID;
														break;

													case USER_SESS_ACCEPTED_ERR:
														hams_respond_error( parser, 202U );
														function_end = VALID;
														discard = VALID;
														break;

													default:
														break;
												}
											}
											else
											{
												rs->flags |= REQ_FLAG_AUTH_VALID;
											}

											if ( INVALID == discard )
											{
												/* Just ignore the rest of the line. */
												parser->buffer_expect = S_MAX_HEADER_LENGTH;
												parser->cur_label = ST_HEADER_FIELD;
												parser->eating = EAT_EOL;
												// return_val =  (x - buffer_data);
												function_end = VALID;
											}
										}
										function_end = INVALID;

									}
								}
									function_end = VALID;
									break;

#endif
								default:
									// if server wants to redirect to https, ignore authentication
									function_end = VALID;
									if ( parser->redir_custom_uri == 0U )
									{
										uint8_t strlength = STRLEN( AUTH_STRING );
										if ( ( buffer_length > strlength ) &&
											 !memcmp( buffer_data, AUTH_STRING,
													  strlength ) )
										{
#ifdef __DIGEST_ACCESS_AUTH__
											parser->http_auth_type = HTTP_AUTH_TYPE_DIGEST;
#endif
											return_val = strlength + 1;
											if ( !( s_char_flags[buffer_data[strlength]] &
													CF_LWS ) )
											{
												discard = VALID;
												hams_respond_error( parser, 401U );
											}
										}
										else
										{
											if ( ( buffer_length > STRLEN( CUSTOM_AUTH_STRING ) ) &&
												 !memcmp( buffer_data, CUSTOM_AUTH_STRING,
														  STRLEN( CUSTOM_AUTH_STRING ) ) )
											{
												parser->http_auth_type = HTTP_AUTH_TYPE_CUSTOM_AUTH;
												return_val = STRLEN( CUSTOM_AUTH_STRING ) + 1;
												if ( !( s_char_flags[buffer_data[STRLEN( CUSTOM_AUTH_STRING )]]
														& CF_LWS ) )
												{
													discard = VALID;
													hams_respond_error( parser, 401U );
												}
											}
											else if ( ( buffer_length > STRLEN( CUSTOM_AUTH ) ) &&
													  !memcmp( buffer_data, CUSTOM_AUTH,
															   STRLEN( CUSTOM_AUTH ) ) )
											{
												parser->http_auth_type = HTTP_AUTH_TYPE_CUSTOM_AUTH;
												return_val = STRLEN( CUSTOM_AUTH ) + 1;
												if ( !( s_char_flags[buffer_data[STRLEN( CUSTOM_AUTH )]]
														& CF_LWS ) )
												{
													discard = VALID;
													hams_respond_error( parser, 401U );
												}
											}
											else
											{
												return_val = 1;
												discard = VALID;
												hams_respond_error( parser, 401U );
											}
										}
									}
									else
									{
										parser->cur_label = ST_HEADER_FIELD;
										parser->eating = EAT_EOL;
										return_val = 1U;
									}
									break;
							}
						}
							if ( ( INVALID == function_end ) && ( INVALID == discard ) )
							{
								return_val = 0;
								function_end = VALID;
							}
							break;

						case HTTP_HDR_CONNECTION:
						{
							uint32_t id = s_util_match_key(
								s_http_connection_types, COUNT_HTTP_CONNECTION_TYPES,
								buffer_data, buffer_length );

							if ( COUNT_HTTP_CONNECTION_TYPES == id )
							{
								hams_respond_error( parser, E400_CONNECTION_TYPE );
								function_end = VALID;
								discard = VALID;
							}
							else
							{
								/* TODO enable this */
#if 0
								if ( id == HTTP_CONNECTION_TYPE_KEEP_ALIVE )
								{
									parser->flags |= HAMS_FLAG_KEEP_ALIVE;
								}
#endif

								parser->buffer_expect = S_MAX_HEADER_LENGTH;
								parser->cur_label = ST_HEADER_FIELD;
								parser->eating = EAT_EOL;
								return_val = strlen(
									s_http_connection_types[COUNT_HTTP_CONNECTION_TYPES] );
								function_end = VALID;
							}
						}
						break;

						case HTTP_HDR_CONTENT_DISPOSITION:
							/* This must begin with form-data;
							   This must begin with form-data;*/
							if ( !( parser->flags & HAMS_FLAG_MULTIPART ) ||
								 ( buffer_length < 10 ) ||
								 memcmp( buffer_data, "form-data;", 10 ) )
							{
								hams_respond_error( parser, E400_BAD_DISPOSITION );
								function_end = VALID;
								discard = VALID;
							}
							else
							{
								parser->cur_label = ST_DISPOSITION_NAME;
								parser->eating = EAT_SPACE;
								return_val = 10;
								function_end = VALID;
							}

							break;

						case HTTP_HDR_CONTENT_LENGTH:
						{
							const uint8_t* n = buffer_data;
							const uint8_t* end = n + buffer_length;
							uint32_t c = 0;
							bool_t break_loop = INVALID;
							while ( ( n != end ) && ( INVALID == break_loop ) &&
									( INVALID == discard ) )
							{
								/* Check for bad content element. */
								if ( ( *n < '0' ) || ( *n > '9' ) )
								{
									break_loop = VALID;
								}
								if ( INVALID == break_loop )
								{

									c *= 10;
									c += *n - '0';

									if ( c > MAX_CONTENT_LENGTH )
									{
										hams_respond_error( parser, 413 );
										function_end = VALID;
										discard = VALID;
									}
									else
									{
										++n;
									}
								}
							}

							if ( discard == INVALID )
							{
								/* Packet must have been cut off or has bad numerical format. */
								if ( ( n == end ) || !( s_char_flags[*n] & ( CF_TERM | CF_LWS ) ) )
								{
									hams_respond_error( parser, E400_BAD_CONTENT_LENGTH );
									function_end = VALID;
									discard = VALID;
								}
								else
								{
									parser->content_length = c;

									parser->buffer_expect = S_MAX_HEADER_LENGTH;
									parser->cur_label = ST_HEADER_FIELD;
									parser->eating = EAT_EOL;
									return_val = ( n - buffer_data );
									function_end = VALID;
								}
							}
						}
						break;

						case HTTP_HDR_CONTENT_TYPE:
						{
							/* First try to find the end of the line. */
							const uint8_t* eol =
								( uint8_t* ) ( memchr( buffer_data, '\r', buffer_length ) );
							if ( !eol )
							{
								hams_respond_error( parser, E400_CONTENT_TYPE_MISSING );
								function_end = VALID;
								discard = VALID;
							}
							else
							{

								/* See if a semi-colon is separating the line. */

								const uint8_t* x = ( uint8_t* ) ( memchr( buffer_data, ';',
																		  eol - buffer_data ) );
								if ( !x )
								{
									x = eol;
								}

								DERR( "CTYPE %u %02hhx\n", x - buffer_data, *x );

								/* Read the MIME type */
								parser->content_type = s_util_match_key(
									s_http_content_types, COUNT_HTTP_CONTENT_TYPES,
									buffer_data, x - buffer_data );

								/* Consume endpt */
								if ( *x == ';' )
								{
									++x;
								}

								if ( HTTP_CONTENT_TYPE_MULTIPART_FORM == parser->content_type )
								{
									DERR( "CTYPE MULTIPART\n" );
									/* NOT allowed when already parsing multibodies! */
									if ( parser->flags & HAMS_FLAG_MULTIPART )
									{
										hams_respond_error( parser, E400_BAD_MULTIPART_TYPE );
										function_end = VALID;
										discard = VALID;
									}
									else
									{
										/* Expecting to parse boundary. */
										parser->cur_label = ST_CONTENT_TYPE_BOUNDARY;
										parser->eating = EAT_SPACE;
										/* Expect maximum boundary length plus boundary= and some padding bytes. */
										parser->buffer_expect = MAX_MULTIPART_BOUNDARY_LENGTH + 9
											+ 8;
									}
								}
								else
								{
									DERR( "CTYPE OTHER\n" );
									parser->buffer_expect = S_MAX_HEADER_LENGTH;
									parser->cur_label = ST_HEADER_FIELD;
									parser->eating = EAT_EOL;
								}
								if ( INVALID == function_end )
								{
									return_val = x - buffer_data;
									function_end = VALID;
								}
							}
						}
						break;

						case HTTP_HDR_ETAG:
						case HTTP_HDR_IF_NONE_MATCH:
							/* Expect quotes. */
						{
							const uint8_t* x = ( uint8_t* ) ( memchr( buffer_data + 1, '"',
																	  buffer_length ) );
							if ( !x || ( buffer_data[0] != '"' ) ||
								 ( ( x - buffer_data + 1 ) > HTTP_ETAG_LEN ) )
							{
								hams_respond_error( parser, E400_IF_NONE_MATCH );
								function_end = VALID;
								discard = VALID;
							}
							else
							{

								/* Send etag value to user. */
								if ( g_hams_demultiplexer->etag_cb )
								{
									g_hams_demultiplexer->etag_cb( parser, buffer_data,
																   x - buffer_data + 1 );
								}

								parser->buffer_expect = S_MAX_HEADER_LENGTH;
								parser->cur_label = ST_HEADER_FIELD;
								parser->eating = EAT_EOL;
								return_val = x - buffer_data + 1;
								function_end = VALID;
							}
						}
						break;

						case HTTP_HDR_ORIGIN:
						{
							if ( parser->cors_auth_common->origin_buffer != NULL )
							{
								delete( parser->cors_auth_common->origin_buffer );
								parser->cors_auth_common->origin_buffer = NULL;
							}
							uint8_t* ptr = ( uint8_t* )memchr( buffer_data, '\n', buffer_length );
							uint8_t origin_type = 0;
							if ( ptr != nullptr )
							{
								parser->cors_auth_common->origin_length = ( uint32_t )( ptr
																						-
																						buffer_data )
									+ 1U;
								if ( parser->cors_auth_common->origin_length > 2U )
								{
									parser->cors_auth_common->origin_buffer = new
										uint8_t[parser->cors_auth_common->origin_length];
									uint8_t* ptr = ( uint8_t* )memchr( buffer_data, '/', buffer_length );
									memcpy( parser->cors_auth_common->origin_buffer, buffer_data,
											parser->cors_auth_common->origin_length );
									parser->origin_header_received = 1U;
									/* handling for CORS access allow control origin */
									origin_type = Specified_Allowed_Origin_Type();
									if ( ( origin_type == DCI_CORS_TYPE_ENUM::ALLOW_ALL_DOMAIN ) ||
										 ( HTTP_METHOD_OPTIONS == parser->method ) )
									{}
									else if ( origin_type == DCI_CORS_TYPE_ENUM::ALLOW_ACTIVE_IP )	/* Active IP  */
									{
										uint32_t ip_address = 0;
										uint32_t rcv_ip_address = 0;
										active_ip_address_owner->Check_Out( &ip_address );
										/* handling the condition to get value Of origin.if its not there need to avoid
										 */
										if ( ( ( *ptr == '/' ) && ( *( ptr + 1 ) == '/' ) ) )
										{
											ETH_DEBUG_PRINT( BF_DBG_MSG_DEBUG, "ptr=%s\n", ptr );
											ptr = ptr + 2;
											uint8_t* cnt_ptr = ptr;
											uint8_t index = 0;
											uint8_t local_ip[MAX_IPV4_STR_BUF_SIZE] =
											{ 0 };

											while ( *cnt_ptr != '\r' && index < MAX_IPV4_STR_LEN )
											{
												local_ip[index] = *cnt_ptr;
												cnt_ptr++;
												index++;
											}

											uint16_t return_val = 0;
											if ( index < MAX_IPV4_STR_LEN )
											{
												return_val = FORMAT_HANDLER_IPV4_BIG_ENDIAN( &rcv_ip_address,
																							 sizeof ( rcv_ip_address ),
																							 local_ip,
																							 MAX_IPV4_STR_BUF_SIZE,
																							 STRING_TO_RAW, nullptr );
											}
											if ( ( return_val != 0U ) && ( rcv_ip_address == ip_address ) )
											{
												// MISRA compatible
											}
											else
											{
												hams_respond_error( parser, E400_BAD_ORIGIN );
												function_end = VALID;
												discard = VALID;
											}

										}
										else
										{
											hams_respond_error( parser, E400_BAD_ORIGIN );
											function_end = VALID;
											discard = VALID;
										}
									}
									else
									{
										hams_respond_error( parser, E400_BAD_ORIGIN );
										function_end = VALID;
										discard = VALID;
									}
								}
								return_val = parser->cors_auth_common->origin_length;
								parser->cur_label = ST_HEADER_FIELD;
							}
							else
							{
								return_val = 0U;
							}
							function_end = VALID;
						}
						break;

						default:
							break;
					}
				}
				break;

			case ST_MPFD_FIND_BOUNDARY:
				DERR( "FINDING MULTIPART BOUND %u\n", buffer_length );
				{
					/* If looking for the end of the boundary, we must always have enough data to find the boundary,
					   that is '--' at begin and '--\r\n' at the end. If not enough data, don't consume any of it. */
					if ( buffer_length < MIN_MPFD_BUFFLEN( parser->ext.multipart_boundary_length ) )
					{
						function_end = VALID;
						return_val = 0;
					}
					else
					{

						/* Look for boundary within the data. Choose an end such that our 4 byte comparison is properly
						   aligned.*/
						const uint8_t* end = buffer_data + ( buffer_length & ~0x3 ) - 5;
						const uint8_t* x = ( uint8_t* ) ( memchr( buffer_data, '\r',
																  end - buffer_data ) );

						/* Must see boundary characters on first go through. */
						if ( !x && ( parser->flags & HAMS_FLAG_FIRST_BOUNDARY ) )
						{
							hams_respond_error( parser, E400_BAD_MULTIPART_BOUNDARY );
							function_end = VALID;
							discard = VALID;
						}
						else
						{

							uint32_t off = 0;
							if ( x )
							{
								/* We expect to get this line if we are doing a boundary match, because the next major
								   code clause will return if it finds a possible match. */
								if ( x == buffer_data )
								{
									switch ( s_check_multipart_boundary( parser, buffer_data,
																		 buffer_length ) )
									{
										case 0:
											/* This MUST match on the first iteration or we have a bad start to the
											   multipart. */
											if ( parser->flags & HAMS_FLAG_FIRST_BOUNDARY )
											{
												hams_respond_error( parser,
																	E400_BAD_MULTIPART_BOUNDARY );
												function_end = VALID;
												discard = VALID;
											}
											else
											{

												/* Negative match, just go past \r\n */
												x += 2;
												if ( !( HAMS_FLAG_FIRST_SKIP & parser->flags ) )
												{
													off = 2;
													parser->flags |= HAMS_FLAG_FIRST_SKIP;
												}
											}
											break;

										case 1:
											/* Reached end of data, do nothing. */
											return_val = parser->ext.multipart_boundary_length + 8;
											function_end = VALID;
											break;

										case 2:
											/* At least got through the first. */
											parser->flags &= ~HAMS_FLAG_FIRST_BOUNDARY;

											/* Announce new field with empty string. */
											parser->user_data_cb( parser, ( const uint8_t* )"", 0 );
											if ( parser->response_code >= 400 )
											{
												function_end = VALID;
												discard = VALID;
											}
											else
											{

												/* Beginning of headers.*/
												parser->buffer_expect = S_MAX_HEADER_LENGTH;
												parser->cur_label = ST_HEADER_FIELD;
												parser->eating = EAT_EOL;
												return_val = parser->ext.multipart_boundary_length
													+ 2;
												function_end = VALID;
											}
											break;

										/* Not possible. */
										default:
											// _ASSERT( 0 );
											break;
									}
								}
								if ( INVALID == function_end )
								{

									/* Search for next possible occurrence of the boundary. */
									uint8_t loop_break = INVALID;
									do
									{
										uint32_t offset = x - buffer_data;

										/* Pass on what we can to the user. */
										// _ASSERT( off < buffer_length );
										// _ASSERT( offset < buffer_length );
										if ( offset - off )
										{
											parser->user_data_cb( parser, buffer_data + off,
																  offset - off );
											if ( parser->response_code >= 400 )
											{
												function_end = VALID;
												discard = VALID;
											}
										}
										if ( INVALID == function_end )
										{

											if ( *( x + 1 ) == '\n' )
											{
												uint32_t byte_offset = offset & 0x3;
												/* Do a quick test for negative matching; that is if this test fails
												   this is a NOT a match; but if it passes it MAY be a match. */
												const uint32_t next = *( const uint32_t* ) ( x + 4
																							 -
																							 byte_offset );
												uint32_t possible = 0;
												if ( byte_offset &&
													 ( parser->ext.boundary_match[3 -
																				  byte_offset] ==
													   next ) )
												{
													possible = 1;
												}
												else if ( *( const uint32_t* ) ( parser->ext
																				 .multipart_boundary )
														  ==
														  next )
												{
													possible = 1;
												}

												/* If there is a possible match, push what data we can to the user. */
												if ( possible )
												{
													DERR( "POSSIBLE MATCH, SENDING TO USER\n" );
													return_val = ( x - buffer_data );
													function_end = VALID;
												}
											}
											if ( INVALID == function_end )
											{
												off = offset;

												/* Consumed these two chars. */
												x += 2;
												if ( x > end )
												{
													loop_break = VALID;
												}
												if ( INVALID == loop_break )
												{

													/* Does not match, just continue. */
													x = ( uint8_t* ) ( memchr( x, '\r', end - x ) );
												}
											}
										}
									} while ( ( x ) && ( INVALID == function_end ) &&
											  ( INVALID == loop_break ) );
								}
							}
							else
							{
								DERR( "DID NOT SEE BOUNDARY\n" );
							}
							if ( INVALID == function_end )
							{
								/* Found no signs, just pass the whole buffer onto the user. */

								// _ASSERT( off < buffer_length );
								// _ASSERT( end < buffer_data + buffer_length );
								parser->user_data_cb( parser, buffer_data + off,
													  end - buffer_data - off );
								if ( parser->response_code >= 400 )
								{
									function_end = VALID;
									discard = VALID;
								}
								else
								{

									return_val = ( end - buffer_data );
									function_end = VALID;
								}
							}
						}
					}
				}
				break;

			case ST_USER_CONTENT:
			{
				parser->user_data_cb( parser, buffer_data, buffer_length );
				if ( parser->response_code >= 400 )
				{
					discard = VALID;
				}
				else
				{

					return_val = buffer_length;
				}
			}
				function_end = VALID;
				break;

			case ST_DISCARD_CASE:
				// ST_DISCARD :
				/* Response code must be nonzero at this point. */
				// _ASSERT( parser->response_code );
				// DERR("ST_DISCARD\n");
				parser->cur_label = ST_DISCARD_CASE;
				return_val = buffer_length;
				break;

			default:
				break;
		}

		if ( INVALID == discard )
		{
			if ( VALID == content_type_boundary )
			{

				DERR( "BOUNDARY SEARCH %u %.*s\n", buffer_length, buffer_length, buffer_data );
				if ( memcmp( buffer_data, "boundary=", 9 ) )
				{
					hams_respond_error( parser, E400_CONTENT_BOUNDARY );
					discard = VALID;
					function_end = VALID;
				}
				else
				{

					/* Find end of the boundary. */
					const uint8_t* begin = buffer_data + 9;
					const uint8_t* x =
						( uint8_t* ) ( memchr( begin, '\n', buffer_length ) );
					if ( !x || ( x == begin ) )
					{
						hams_respond_error( parser, E400_CONTENT_BOUNDARY );
						discard = VALID;
						function_end = VALID;
					}
					else
					{

						/* Reject boundary if it's too long or too short. */
						if ( ( x - begin > MAX_MULTIPART_BOUNDARY_LENGTH ) ||
							 ( x - begin < MIN_MULTIPART_BOUNDARY_LENGTH ) )
						{
							hams_respond_error( parser, E400_CONTENT_BOUNDARY );
							discard = VALID;
							function_end = VALID;
						}
						else
						{

							/* Copy the boundary. Adjust if there is only \n */
							parser->ext.multipart_boundary_length = x - begin;
							if ( *( x - 1 ) == '\r' )
							{
								--parser->ext.multipart_boundary_length;
							}
							if ( BF_Lib::Copy_Data( parser->ext.multipart_boundary, MAX_MULTIPART_BOUNDARY_LENGTH,
													begin, parser->ext.multipart_boundary_length ) == true )
							{
								DERR( "GOT BOUNDARY %.*s\n",
									  parser->ext.multipart_boundary_length,
									  parser->ext.multipart_boundary );

								/* Go directly back to reading headers...got the \r\n... */
								parser->buffer_expect = S_MAX_HEADER_LENGTH;
								parser->cur_label = ST_HEADER_FIELD;
								parser->eating = EAT_EOL;
								DERR( "BOUNDLEN %u CONSUMED %u\n",
									  parser->ext.multipart_boundary_length, x - buffer_data + 1 );
								/* Don't consume \r\n; just an easy way to terminate the line... */
								return_val = ( x - buffer_data + 1 - 2 );
								function_end = VALID;
							}
							else
							{
								hams_respond_error( parser, E400_CONTENT_BOUNDARY );
								discard = VALID;
								function_end = VALID;
							}


						}
					}
				}
			}

			if ( VALID == disposition_type )
			{
				disposition_type = INVALID;
				/* If boundary is not defined or value does not begin with name then bail. */
				if ( !parser->ext.multipart_boundary_length ||
					 ( buffer_length < 7 ) ||
					 memcmp( buffer_data, "name=", 5 ) )
				{
					hams_respond_error( parser, E400_BAD_DISPOSITION );
					discard = VALID;
					function_end = VALID;
				}
				else
				{
					/* Pass name parameter to user. */
					const uint8_t* x =
						( uint8_t* ) ( memchr( buffer_data + 6, '"', buffer_length ) );
					// uint32_t len = x - buffer_data - 6;

					if ( x != NULL )
					{
						DERR( "CONTENT_DISPOSITION NAME: %u %.*s\n", len, len, buffer_data + 6 );
						parser->user_data_cb( parser, buffer_data + 6,
											  x - buffer_data - 6 );
						if ( parser->response_code >= 400 )
						{
							discard = VALID;

						}
						else
						{
							++x;
							parser->buffer_expect = S_MAX_HEADER_LENGTH;
							parser->cur_label = ST_HEADER_FIELD;
							parser->eating = EAT_EOL;
							return_val = ( x - buffer_data + 1 );
							function_end = VALID;
						}
					}
					else
					{
						hams_respond_error( parser, E400_BAD_DISPOSITION );
						discard = VALID;
						function_end = VALID;
					}
				}

			}
		}

		if ( VALID == discard )
		{
			/* Response code must be nonzero at this point. */
			// _ASSERT( parser->response_code );
			// DERR("ST_DISCARD\n");
			parser->cur_label = ST_DISCARD_CASE;
			return_val = buffer_length;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void hams_init( void )
{
	g_hams_demultiplexer->staged_len = 0;
	cors_origin_type = new DCI_Owner( DCI_CORS_TYPE_DCID );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t hams_recv( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	// _ASSERT( parser );

	// _ASSERT( hams_integrity_check(parser) );

	bool_t return_flag = INVALID;
	bool_t loop_break = INVALID;
	uint8_t return_val = 0;

	struct real_hams_parser_s* parserb = ( struct real_hams_parser_s* )parser;

	uint32_t present = parser->present;

	/* TODO optimize for fewer memcpy() calls? */
	while ( ( length ) && ( INVALID == return_flag ) )
	{
		loop_break = INVALID;
		/* Figure out how much to append to current buffer. */
		uint32_t amount = parser->buffer_size - present;
		if ( amount > length )
		{
			amount = length;
		}

		if ( amount )
		{
			memcpy( parserb->buffer + present, data, amount );
			length -= amount;
			data += amount;
			present += amount;
		}
		else
		{
			length = 0;
		}

		const uint8_t* src = ( uint8_t* ) ( parserb->buffer );

		while ( ( present ) && ( INVALID == loop_break ) )
		{
			if ( !parser->eating )
			{
				if ( present < parser->buffer_expect )
				{
					const uint8_t* possible = ( uint8_t* ) ( memchr( src, '\r', present ) );
					if ( !possible || ( possible == src + present ) )
					{
						loop_break = VALID;
					}
					else
					{
						/* Check for */
						++possible;
						if ( *possible != '\n' )
						{
							loop_break = VALID;
						}
					}
				}
			}

			if ( INVALID == loop_break )
			{
				/* If parsing content data, then only pass on at most the rest of the content. */
				uint32_t parse_len = present;
				if ( ( parser->flags & HAMS_FLAG_CONTENT ) && ( parser->content_length < present ) )
				{
					parse_len = parser->content_length;
				}

				/* Parse available data; if error then request to close. */
				// DERR("-----PFEED EXP %u %u-----\n",
				// parser->buffer_expect, parse_len);
				int32_t result = hams_parser_feed( parser, src, parse_len );

				DERR( "PFEED RESULT %u EXPECT %u\n", result, parser->buffer_expect );

				/* If no data parsed then need more data. */
				if ( !result )
				{
					DERR( "-----PARSER %p MORE DATA %u, LEN %u-----\n",
						  parser, parser->buffer_expect, length );
					loop_break = VALID;
				}
				else
				{
					/* adjust remaining data. */
					present -= result;
					src += result;

					/* If parser started echoing, make sure we get whatever content bytes sitting in the current buffer
					   back out.*/
					if ( parser->flags & HAMS_FLAG_ECHO_CONTENT )
					{
						parser->flags |= HAMS_FLAG_SENDING | HAMS_FLAG_STAGE_REQ;

						/* Begin request. */
						Http_Server_Output_Sent( NULL, 0 );
						hams_response_header( parser, HTTP_HDR_CONTENT_LENGTH,
											  ( void* )&parser->content_length, 0 );
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
											  "application/octet-stream",
											  24 );
						hams_response_header( parser, HTTP_HDR_CONTENT_DISPOSITION,
											  parser->ext.multipart_boundary,
											  parser->ext.multipart_boundary_length );
						hams_response_header( parser, COUNT_HTTP_HDRS, NULL, 0 );

						/* Send the content in the buffer. */
						/* TODO make sure this fits in the MSS, but it should because headers are small and MSS should
						   be about 3000. TODO check return code. */
						uint32_t sc = g_hams_demultiplexer->send_cb( parser, src, present, 1 );
						parser->content_length -= present;
						parser->present = 0;

						if ( length )
						{
							uint32_t sc = g_hams_demultiplexer->send_cb( parser, data, length, 1 );
							parser->content_length -= length;
						}

						parser->flags &= ~HAMS_FLAG_CONTENT;

						/* HAMS no longer wants to manage this connection. */
						return_val = 0;
						return_flag = VALID;
						loop_break = VALID;
					}
					else
					{
						if ( ( !parser->user_data_cb ) || ( parser->flags & HAMS_FLAG_ERROR ) )
						{
							parser->user_data_cb = s_hams_end_response;
						}

						/* Check if method is set then we can begin sending response. */
						if ( parser->response_code )
						{
							parser->content_length = 0U;
							DERR( "PARSER SET RESPONSE %u\n",
								  parser->response_code );
							parser->flags |= HAMS_FLAG_SENDING | HAMS_FLAG_STAGE_REQ;
							/* Begin request. */
							Http_Server_Output_Sent( NULL, 0 );
							return_val = HAMS_EXPECT_TX;
							return_flag = VALID;
							loop_break = VALID;
						}
						else
						{

							/* reduce content length if parsing content. */
							if ( parser->flags & HAMS_FLAG_CONTENT )
							{
								parser->content_length -= result;
								/* If reached end of content length, let user know. */
								if ( !parser->content_length )
								{
									parser->flags &= ~HAMS_FLAG_CONTENT;

									DERR( "REACHED END OF REQ BODY\n" );
									parser->user_data_cb( parser, NULL, 0 );

									if ( !parser->user_data_cb )
									{
										parser->user_data_cb = s_hams_end_response;
									}
									/* TODO refactor  */
									if ( parser->response_code )
									{
										DERR( "PARSER SET RESPONSE %u\n",
											  parser->response_code );
										parser->flags |= HAMS_FLAG_SENDING | HAMS_FLAG_STAGE_REQ;
										/* Begin request. */
										Http_Server_Output_Sent( NULL, 0 );
										return_val = HAMS_EXPECT_TX;
										return_flag = VALID;
										loop_break = VALID;
									}
								}
							}
						}
					}
				}
			}
		}

		if ( present )
		{
			/* Move remaining data back to beginning. */
			memmove( parserb->buffer, src, present );
		}
	}

	// _ASSERT( hams_integrity_check(parser) );
	DERR( "-----PARSER %p PRESENT %u-----\n", parser, present );
	parser->present = present;
	if ( INVALID == return_flag )
	{
		return_val = HAMS_EXPECT_RX;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t s_hams_end_response( hams_parser* parser, const uint8_t* data,
									 uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( parser->redir_custom_uri == 1U )
	{
		Http_Server_Free( rs->p.redir.filename );
		parser->redir_custom_uri = 0U;
	}
	if ( !data )
	{
		hams_response_header( parser, COUNT_HTTP_HDRS, NULL, 0 );
		parser->user_data_cb = NULL;
	}
	return ( 0 );
}

/* This is echo, Looking for the following sequence of calls:
 *
 * (parser, "", 0) (parser, DATA, len)
 * ....
 * (parser, DATA, len) (parser, NULL, 0)
 *
 * */
static uint32_t s_hams_echo( hams_parser* parser, const uint8_t* data,
							 uint32_t length )
{
	if ( hams_check_headers_ended( data, length ) )
	{
		/* Make sure mime type is application/octet-stream */
		if ( parser->content_type != HTTP_CONTENT_TYPE_APPLICATION_OCTET_STREAM )
		{
			hams_respond_error( parser, 415 );
		}
		/* Make sure we have a content body. */
		else if ( !parser->content_length )
		{
			hams_respond_error( parser, 411 );
		}
		else
		{
			parser->flags |= HAMS_FLAG_ECHO_CONTENT | HAMS_FLAG_SENDING;
			parser->response_code = 200U;
		}
	}
	else
	{
		// _ASSERT( hams_can_respond(data, length) );
		/* Just responding to transmit callbacks, nothing to do really. */
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t hams_echo_query( hams_parser* parser, const uint8_t* data,
						  uint32_t length )
{
	// _ASSERT( hams_has_uri_part(data, length) );
	bool_t error_set = VALID;

	DERR( "ECHO_QUERY\n" );

	if ( '=' == data[0] )
	{
		/* Only accepting 1 query value. */
		if ( ' ' != data[length] )
		{
			hams_respond_error( parser, 400 );
			error_set = INVALID;
		}
		else
		{

			/* Copy filename to space used for MPFD parsing. Clearly we're not using that mime type here. */
			memcpy( parser->ext.multipart_boundary, data + 1, length - 1 );
			parser->ext.multipart_boundary_length = length - 1;
			parser->user_data_cb = s_hams_echo;
			error_set = INVALID;
		}
	}
	else
	{
		/* Check query field name. */
		if ( ( length == 9 ) && !strncmp( "filename", ( const char_t* )data + 1, 8 ) )
		{
			error_set = INVALID;
		}
	}

	if ( VALID == error_set )
	{
		hams_respond_error( parser, 400 );
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void hams_start_response( hams_parser* parser )
{
	// _ASSERT( parser );
	bool_t loop_break = INVALID;

	/* Response starts with something like
	 *
	 * HTTP/1.0 RES Explanation
	 *
	 * e.g HTTP/1.0 200 OK */

	g_hams_demultiplexer->staged_len = 0;
	uint8_t* o = g_hams_demultiplexer->output_buffer;

	memcpy( o, "HTTP/1.0", 8 );
	o += 8;

	// _ASSERT( parser->response_code );

	/* Lookup corresponding HTTP code. TODO use a binary search here instead.*/
	for ( uint32_t i = 0;
		  ( ( i < sizeof ( s_http_response_codes ) / sizeof( char_t* ) ) &&
			( INVALID == loop_break ) );
		  ++i )
	{
		const uint16_t* code = ( const uint16_t* )s_http_response_codes[i];
		if ( *code == parser->response_code )
		{
			const char_t* s = s_http_response_codes[i] + 2;
			while ( *s )
			{
				*o = *s;
				++o;
				++s;
			}

			loop_break = VALID;
		}
	}

	*o = '\r';
	++o;
	*o = '\n';
	++o;

	g_hams_demultiplexer->staged_len = o - g_hams_demultiplexer->output_buffer;

	/* Output boilerplate. */
	memcpy( o, s_boiler_plate, STRLEN( s_boiler_plate ) );
	g_hams_demultiplexer->staged_len += STRLEN( s_boiler_plate );
	o += STRLEN( s_boiler_plate );
	/* Keep socket alive only for GET method of HTTPS request */
	if ( ( true == parser->https ) && ( HTTP_METHOD_GET == parser->method ) )
	{
#ifdef HTTPS_PERSISTENT_CONNECTION
		/* Output keep alive plate only for HTTPS request*/
		snprintf( ( char* )o, sizeof( s_keep_alive_plate ),
				  "Connection: Keep-Alive\r\nKeep-Alive: timeout=%03u, max=100\r\n",
				  KEEP_ALIVE_TIME );
		g_hams_demultiplexer->staged_len += STRLEN( s_keep_alive_plate );
		o += STRLEN( s_keep_alive_plate );
#else
		memcpy( o, s_connection_close_plate, STRLEN( s_connection_close_plate ) );
		g_hams_demultiplexer->staged_len += STRLEN( s_connection_close_plate );
		o += STRLEN( s_connection_close_plate );
#endif
	}
	else
	{
		memcpy( o, s_connection_close_plate, STRLEN( s_connection_close_plate ) );
		g_hams_demultiplexer->staged_len += STRLEN( s_connection_close_plate );
		o += STRLEN( s_connection_close_plate );
	}
	/* Add CORS header, if origin was provided */
#ifndef CORS_ALLOW_ALL_ORIGIN
	if ( parser->origin_header_received == 1U )
#endif
	{

		if ( ( NULL != parser->cors_auth_common )
#ifndef CORS_ALLOW_ALL_ORIGIN
			 && ( parser->cors_auth_common->origin_buffer != NULL ) &&
			 ( ( parser->response_code == 200U ) || ( parser->response_code == 201U ) ||
			   ( parser->response_code == 202U ) ) )
#else
			 )
#endif
		{
			memcpy( o, VARY_FIELD, STRLEN( VARY_FIELD ) );
			o += STRLEN( VARY_FIELD );

			memcpy( o, ACCESS_CONTROL_ALLOW_ORIGIN_STR, STRLEN( ACCESS_CONTROL_ALLOW_ORIGIN_STR ) );
			o += STRLEN( ACCESS_CONTROL_ALLOW_ORIGIN_STR );
			g_hams_demultiplexer->staged_len += STRLEN( VARY_FIELD )
				+ STRLEN( ACCESS_CONTROL_ALLOW_ORIGIN_STR );
#ifndef CORS_ALLOW_ALL_ORIGIN
			memcpy( o, parser->cors_auth_common->origin_buffer,
					parser->cors_auth_common->origin_length );
			o += parser->cors_auth_common->origin_length;
			g_hams_demultiplexer->staged_len += parser->cors_auth_common->origin_length;
#endif
			memcpy( o, CORS_COMMON_FIELD, STRLEN( CORS_COMMON_FIELD ) );
			o += STRLEN( CORS_COMMON_FIELD );
			g_hams_demultiplexer->staged_len += STRLEN( CORS_COMMON_FIELD );

			if ( HTTP_METHOD_OPTIONS == parser->method )
			{
				memcpy( o, CORS_OPTIONS_FIELD, STRLEN( CORS_OPTIONS_FIELD ) );
				o += STRLEN( CORS_OPTIONS_FIELD );
				g_hams_demultiplexer->staged_len += STRLEN( CORS_OPTIONS_FIELD );
			}
		}
	}

#ifdef __DIGEST_ACCESS_AUTH__

	if ( 401U == parser->response_code )
	{
		memcpy( o, s_http_hdrs[HTTP_HDR_WWW_AUTHENTICATE],
				strlen( s_http_hdrs[HTTP_HDR_WWW_AUTHENTICATE] ) );
		o += strlen( s_http_hdrs[HTTP_HDR_WWW_AUTHENTICATE] );
		*o = ' ';
		++o;
		if ( parser->http_auth_type == HTTP_AUTH_TYPE_CUSTOM_AUTH )
		{
			memcpy( o, CUSTOM_AUTH_STRING, STRLEN( CUSTOM_AUTH_STRING ) );
			/* Default String */
			o += STRLEN( CUSTOM_AUTH_STRING );
		}
		else
		{
			memcpy( o, AUTH_STRING, STRLEN( AUTH_STRING ) );
			/* Default String */
			o += STRLEN( AUTH_STRING );
		}
		*o = ' ';
		++o;

		memcpy( o, REALM_PARAM_STR, strlen( REALM_PARAM_STR ) );
		o += strlen( REALM_PARAM_STR );
		*o = '=';
		++o;

		memcpy( o, WEB_REALM, strlen( WEB_REALM ) );
		o += strlen( WEB_REALM );
		*o = ',';
		++o;
		*o = ' ';
		++o;

		memcpy( o, NONCE_PARAM_STR, strlen( NONCE_PARAM_STR ) );
		o += strlen( NONCE_PARAM_STR );
		*o = '=';
		++o;

		uint8_t nonce_buf[OUTPUT_NONCE_LENGTH] = {0U};
		uint8_t opaque_buf[OPAQUE_LENGTH] = {0U};
		http_server_req_state* rs = Http_Server_Get_State( parser );
		m_user_session_handler->Create_Nonce( reinterpret_cast<char_t*>( nonce_buf ),
											  reinterpret_cast<char_t*>( opaque_buf ), rs->parser.general_use_reg,
											  rs->parser.https );

		memcpy( o, nonce_buf, OUTPUT_NONCE_LENGTH );
		o += OUTPUT_NONCE_LENGTH;
		*o = ',';
		++o;
		*o = ' ';
		++o;

		memcpy( o, OPAQUE_PARAM_STR, strlen( OPAQUE_PARAM_STR ) );
		o += strlen( OPAQUE_PARAM_STR );
		*o = '=';
		++o;

		memcpy( o, opaque_buf, OPAQUE_LENGTH );
		o += OPAQUE_LENGTH;
		*o = ',';
		++o;
		*o = ' ';
		++o;

		memcpy( o, ALGORITHM_STR, strlen( ALGORITHM_STR ) );
		o += strlen( ALGORITHM_STR );
		*o = '=';
		++o;

		memcpy( o, MD5_STR, strlen( MD5_STR ) );
		o += strlen( MD5_STR );
		*o = ',';
		++o;
		*o = ' ';
		++o;

		memcpy( o, QOP_PARAM_STR, strlen( QOP_PARAM_STR ) );
		o += strlen( QOP_PARAM_STR );
		*o = '=';
		++o;

		memcpy( o, AUTH_QOP, strlen( AUTH_QOP ) );
		o += strlen( AUTH_QOP );
		*o = '\r';
		++o;
		*o = '\n';
		++o;

		g_hams_demultiplexer->staged_len = o - g_hams_demultiplexer->output_buffer;
	}
	if ( NULL != parser->cors_auth_common )
	{
		if ( NULL != parser->cors_auth_common->digest_ptr )
		{
			delete parser->cors_auth_common->digest_ptr;
			parser->cors_auth_common->digest_ptr = NULL;
		}
	}
#endif
	if ( NULL != parser->cors_auth_common )
	{
		if ( NULL != parser->cors_auth_common->origin_buffer )
		{
			delete( parser->cors_auth_common->origin_buffer );
			parser->cors_auth_common->origin_buffer = NULL;
		}
		delete( parser->cors_auth_common );
		parser->cors_auth_common = NULL;

	}

	parser->origin_header_received = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void hams_response_header( hams_parser* parser, uint32_t header_id,
						   const void* data, uint32_t data_size )
{
	/* Count \r\n. */
	bool_t return_flag = INVALID;
	uint32_t len = 2;
	uint32_t value_len = 0;
	uint8_t* o;
	bool_t bounce_loop = VALID;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/* User indicates all headers are written. */
	if ( COUNT_HTTP_HDRS > header_id )
	{
		/* Count length of header and space and \r\n line termination */
		len += strlen( s_http_hdrs[header_id] ) + 1;
	}
	else
	{
		header_id = COUNT_HTTP_HDRS;
	}

	/* This holds the address of the writing code.*/
	uint32_t bounce = 0;
	uint32_t use_protol_str = 1U;

	while ( VALID == bounce_loop )
	{
		bounce_loop = INVALID;
		switch ( header_id )
		{

			/* TODO */
			case HTTP_HDR_ALLOW:
				/* Expecting a bitfield listing all allowable methods. */
				if ( !bounce )
				{
					bounce = 1;
					value_len = data_size;
				}
				else
				{
					memcpy( o, data, data_size );
					return_flag = VALID;

				}
				break;

			case HTTP_HDR_CONNECTION:
				/* Expecting integer data */
				if ( !bounce )
				{
					uint32_t type = *( ULBASE* ) ( data );
					value_len = strlen( s_http_connection_types[type] );
					bounce = 1;
				}
				else
				{
					/* Write to memory. */
					memcpy( o, s_http_connection_types[*( ULBASE* ) ( data )], value_len );
					return_flag = VALID;
				}
				break;

			case HTTP_HDR_CONTENT_DISPOSITION:
				/* Will write out:
				 *
				 * attachment; filename=FILENAME
				 *
				 * 21 bytes precede FILENAME */
				if ( !bounce )
				{
					bounce = 1;
					value_len = data_size + 21;
				}
				else
				{
					memcpy( o, "attachment; filename=", 21 );
					o += 21;
					memcpy( o, data, data_size );
					return_flag = VALID;
				}
				break;

			case HTTP_HDR_CONTENT_ENCODING:
				/* Will write out:
				 *
				 * ENCTYPE
				 *
				 * which is most likely gzip.. */
				if ( !bounce )
				{
					bounce = 1;
					value_len = data_size;
				}
				else
				{
					memcpy( o, data, data_size );
					return_flag = VALID;
				}
				break;

			case HTTP_HDR_CONTENT_LENGTH:
				/* Expecting a positive integer for the content length. TODO*/
				if ( !bounce )
				{
					/* Maximum of 9 digits possible. */
					value_len = 9;
					bounce = 1;
				}
				else
				{
					{
						uint32_t content_length = *( ULBASE* ) ( data );
						parser->content_length = content_length;
						uint8_t* x = o + 8;
						while ( content_length )
						{
							*x = ( content_length % 10 ) + '0';
							content_length /= 10;
							--x;
						}
						*x = ' ';
						while ( o != x )
						{
							*o = ' ';
							++o;
						}
						return_flag = VALID;
					}
				}
				break;

			case HTTP_HDR_CONTENT_TYPE:
				/* Expecting string for the Content Type. */
				if ( !bounce )
				{
					bounce = 1;
					value_len = data_size;
				}
				else
				{
					memcpy( o, data, data_size );
					return_flag = VALID;
				}
				break;

			case HTTP_HDR_ETAG:
				/* Expecting string for the ETAG. */
				if ( !bounce )
				{
					/* Will print double quoted ETAG. */
					bounce = 1;
					value_len = data_size + 2;
				}
				else
				{
					*o = '"';
					++o;
					memcpy( o, data, data_size );
					o += data_size;
					*o = '"';
					return_flag = VALID;
				}
				break;

			case HTTP_HDR_LOCATION:

				if ( !bounce )
				{
					/* Expecting URI string for the location. Will print http://HOSTNAME/URI
					 *
					 * so 7 prefix char_tacters, hostname length, and URI string (including leading '/')
					 * */
					bounce = 1;

					if ( true == Is_Conn_Enabled( HTTP_MEDIA ) )
					{
						use_protol_str = parser->https;
					}
					value_len = data_size + protocol_str_length[use_protol_str] +
						strlen( g_hams_demultiplexer->hostname );
				}
				else
				{
					memcpy( o, protocol_str[use_protol_str], protocol_str_length[use_protol_str] );
					o += protocol_str_length[use_protol_str];
					/* Copy hostname. */
					memcpy( o, g_hams_demultiplexer->hostname,
							value_len - data_size - protocol_str_length[use_protol_str] );
					o += value_len - data_size - protocol_str_length[use_protol_str];
					/* Copy uri */
					memcpy( o, data, data_size );
					return_flag = VALID;
				}
				break;


			case HTTP_HDR_WWW_AUTHENTICATE:
				/* Expecting uint32_t integer. */
				if ( !bounce )
				{
					/* TODO */
					bounce = 1;
					value_len = 0;
				}
				else
				{
					{
						// uint32_t access = *(ULBASE*) ( data );
						/* TODO */
						return_flag = VALID;
					}
				}
				break;

			case HTTP_HDR_X_REQUEST_ACCESS_LEVEL:
				/* Expecting uint32_t integer. */
				if ( !bounce )
				{
					/* TODO */
					bounce = 1;
					value_len = 0;
				}
				else
				{
					{
						// uint32_t access = *(ULBASE*) ( data );
						/* TODO */
						return_flag = VALID;
					}
				}
				break;

			default:
				break;
		}

		if ( INVALID == return_flag )
		{
			len += value_len;
			/* Check if these data will fit in the current output buffer. */
			uint32_t extlen = len + g_hams_demultiplexer->staged_len;
			if ( Hamsif_Extend_Output( parser, extlen ) < extlen )
			{
				/* Will not fit, flush buffer and reallocate */
				uint32_t sc = g_hams_demultiplexer->send_cb( parser,
															 g_hams_demultiplexer->output_buffer,
															 g_hams_demultiplexer->staged_len, 1 );
				g_hams_demultiplexer->staged_len = 0;

				/* Try to allocate again. */
				if ( Hamsif_Extend_Output( parser, len ) < len )
				{
					/* Should always have enough RAM in reserve. */
					// _ASSERT( 0 );
					/* TODO handle this error, cannot allocate data. */
					return_flag = VALID;
				}
			}
		}
		if ( INVALID == return_flag )
		{
			/* Initialize output pointer. */
			o = g_hams_demultiplexer->output_buffer
				+ g_hams_demultiplexer->staged_len;

			if ( COUNT_HTTP_HDRS > header_id )
			{
				/* Write out the header (always includes ':'.) */
				{
					uint32_t slen = strlen( s_http_hdrs[header_id] );
					( uint8_t* )memcpy( o, s_http_hdrs[header_id], slen );
					o += slen;
					*o = ' ';
					++o;
				}

				/* Write trailing \r\n */
				o[value_len] = '\r';
				o[value_len + 1] = '\n';

				/* Adjust count of bytes in buffer and bounce back to the write section. */
				g_hams_demultiplexer->staged_len += len;
				bounce_loop = VALID;
			}
			else
			{
				/* Write out header terminator. */
				memcpy( o, "\r\n", 2 );
				g_hams_demultiplexer->staged_len += len;

				/* Send out the header. */
				uint32_t sc = g_hams_demultiplexer->send_cb( parser,
															 g_hams_demultiplexer->output_buffer,
															 g_hams_demultiplexer->staged_len, 1 );
				g_hams_demultiplexer->staged_len = 0;

				uint32_t mask = HAMS_FLAG_STAGE_REQ;

				if ( ( parser->content_length ) && ( 0 == ( parser->flags & HAMS_FLAG_CONTENT ) ) )
				{
					Http_Server_Output_Sent( parser, 1U );
				}
				/* check if what HAMS sent has been fully sent. */
				if ( ( ( !parser->content_length ) || ( HTTP_METHOD_HEAD == parser->method ) ) &&
					 !( parser->flags & HAMS_FLAG_ERROR || parser->flags & HAMS_FLAG_REDIRECT ) )
				{
					mask |= HAMS_FLAG_SENDING;
					/* Close the connection */
					Http_Server_Output_Sent( parser, 0U );
					g_hams_demultiplexer->send_cb( parser, NULL, 0, 3 );
				}
				parser->flags &= ~mask;
				Hamsif_Extend_Output( parser, 0 );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t hams_custom_headers( hams_parser* parser, const char_t* response, uint32_t len )
{

	uint8_t return_val = 0;
	uint32_t extlen = len + g_hams_demultiplexer->staged_len;

	if ( Hamsif_Extend_Output( parser, extlen ) < extlen )
	{
		/* Will not fit, flush buffer and reallocate */
		uint32_t sc = g_hams_demultiplexer->send_cb( parser,
													 g_hams_demultiplexer->output_buffer,
													 g_hams_demultiplexer->staged_len, 1 );
		g_hams_demultiplexer->staged_len = 0;

		/* Try to allocate again. */
		if ( Hamsif_Extend_Output( parser, len ) < len )
		{
			/* Should always have enough to do this. */
			// _ASSERT( 0 );
		}
		return_val = 0;
	}
	else
	{

		uint8_t* o = g_hams_demultiplexer->output_buffer
			+ g_hams_demultiplexer->staged_len;

		memcpy( o, response, len );
		g_hams_demultiplexer->staged_len += len;
		return_val = 1;
	}
	return ( return_val );
}

#ifdef __DIGEST_ACCESS_AUTH__
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint8_t Digest_Data_Extract( const uint8_t* x, uint8_t* rx_buffer, uint8_t* rx_length,
									bool_t* receive_flag, uint32_t buffer_length,
									uint8_t* digest_state )
{
	uint8_t seek_count = 0U;
	uint8_t range_count = 0U;
	uint8_t eat_count = 0U;
	static bool_t quote_present;
	uint8_t max_rx_buf_size = 0U;

	if ( *x == 0x22U )	/* " */
	{
		x++;
		if ( VALID == quote_present )
		{
			*receive_flag = VALID;	/* In case of blank arguments */
			*digest_state = MAX_STATE;
			quote_present = INVALID;
		}
		else
		{
			quote_present = VALID;
		}
		eat_count = 1;
	}
	else if ( *x == 0x20U /*Space*/ )
	{
		x++;
		eat_count = 1;
		quote_present = INVALID;
	}
	else if ( *x == 0x3DU /* = */ )
	{
		x++;
		eat_count = 1;
		quote_present = INVALID;
	}
	else
	{
		seek_count = *rx_length;
		eat_count = 0;

		switch ( *digest_state )
		{
			case USERNAME_STATE:
				max_rx_buf_size = USERNAME_LENGTH;
				range_count = USERNAME_LENGTH;
				break;

			case NONCE_STATE:
				max_rx_buf_size = OUTPUT_NONCE_LENGTH;
				range_count = OUTPUT_NONCE_LENGTH;
				break;

			case OPAQUE_STATE:
				max_rx_buf_size = OPAQUE_LENGTH;
				range_count = OPAQUE_LENGTH;
				break;

			case REALM_STATE:
				max_rx_buf_size = WEB_REALM_LENGTH;
				range_count = WEB_REALM_LENGTH;
				break;

			case RESPONSE_STATE:
				max_rx_buf_size = RESPONSE_LENGTH;
				range_count = RESPONSE_LENGTH;
				break;

			case QOP_STATE:
				max_rx_buf_size = AUTH_QOP_LENGTH;
				range_count = AUTH_QOP_LENGTH;
				break;

			case NC_STATE:
				max_rx_buf_size = NC_LENGTH;
				range_count = NC_LENGTH;
				break;

			case CNONCE_STATE:
				max_rx_buf_size = CNONCE_LENGTH;
				range_count = CNONCE_LENGTH;
				break;

			case URI_STATE:
				max_rx_buf_size = AUTH_BUF_URI_FIELD_LENGTH;
				range_count = AUTH_BUF_URI_FIELD_LENGTH;
				break;

			case ALGO_STATE:
				max_rx_buf_size = ALGORITHM_LENGTH;
				range_count = ALGORITHM_LENGTH;
				break;

			default:
				max_rx_buf_size = AUTH_BUF_FIELD_LENGTH;
				range_count = AUTH_BUF_FIELD_LENGTH;
				break;
		}

		if ( VALID == quote_present )
		{
			while ( ( *x != 0x22U ) && ( seek_count < max_rx_buf_size ) &&
					( eat_count < ( uint8_t )buffer_length ) )		/* " */
			{
				if ( rx_buffer != NULL )
				{
					rx_buffer[seek_count++] = *x++;
					eat_count++;
				}
				else
				{
					seek_count++;
					eat_count++;
					*x++;
				}
			}

			*rx_length = seek_count;

			if ( ( *x == 0x22U ) || ( seek_count <= range_count ) )	/* " */
			{
				*receive_flag = VALID;
				*digest_state = MAX_STATE;
				quote_present = INVALID;
			}
			else
			{
				*receive_flag = INVALID;
			}
		}
		else
		{
			while ( ( *x != 0x2C ) && ( seek_count < max_rx_buf_size ) &&
					( eat_count < ( uint8_t )buffer_length ) )	/* , */
			{
				if ( rx_buffer != NULL )
				{
					rx_buffer[seek_count++] = *x++;
					eat_count++;
				}
				else
				{
					seek_count++;
					eat_count++;
					*x++;
				}
			}
			*rx_length = seek_count;
			if ( ( *x == 0x2CU ) || ( seek_count <= range_count ) )	/* , */
			{
				*receive_flag = VALID;
				*digest_state = MAX_STATE;
				quote_present = INVALID;
			}
			else
			{
				*receive_flag = INVALID;
			}
		}
	}

	return ( eat_count );
}

#endif

/**
 **********************************************************************************************
 * @brief                     returns CORS specified allowed origin type
 * @param[in] void            none
 * @return[out] uint8_t       returns the origin type
 * @n
 **********************************************************************************************
 */
uint8_t Specified_Allowed_Origin_Type( void )
{
	uint8_t origin_type = 0;

	cors_origin_type->Check_Out( &origin_type );
	return ( origin_type );
}

/**
 **********************************************************************************************
 * @brief                     returns auth level of the user
 * @param[in] parser          Structure containing parameters related to a packet
 * @return[out] uint8_t       returns the auth level of the user
 * @n
 **********************************************************************************************
 */
uint8_t Get_Auth_Level( hams_parser* parser )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	return ( rs->auth_level );
}
