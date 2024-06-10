#ifndef __HTTP_STREAM_PARSER__
#define __HTTP_STREAM_PARSER__

#include <stdint.h>
#include <string.h>

/* HAMS: HTTP Authenticating and Multipart/form-data Support
 *
 * Foundation for server processing and authenticating HTTP requests,
 * as well as parsing multipart/form-data uploads.
 *
 * Features:
 * -Stack agnostic and cross platform
 * -Modular server construction
 * -Minimal code size
 * -Low RAM usage (both heap and stack)
 * -Minimal API
 * */

#define MAX_URI_DEPTH 8
#define MAX_URI_SEG_LENGTH 64
/* 640K should be more than enough for all BCM related stuff */
#define MAX_CONTENT_LENGTH 655360
#define HTTP_ETAG_LEN 32
#define MAX_HTTP_BASIC_AUTH_FIELD 128

/* Reference http://stackoverflow.com/questions/8250154/html-multipart-form-maximum-length-of-boundary-string
 *
 * RFC2616 (HTTP/1.1) section "3.7 Media Types" says that the allowed types in the Content-Type header is defined by RFC1590 (Media Type Registration Procedure).
 * RFC1590 updates RFC-1521(MIME).
 * RFC1521 says that a boundary "must be no longer than 70 characters, not counting the two leading hyphens".
 * The same text also appears in RFC2046 which supposedly obsoletes RFC1521.
 *
 * */
#define MAX_MULTIPART_BOUNDARY_LENGTH 70

/** @brief Define minimum amount of data user is required to supply for
 * shared output buffer */
#define HAMS_MIN_STAGE_EXT 256

enum {
	HTTP_METHOD_DELETE
	,HTTP_METHOD_GET
	,HTTP_METHOD_HEAD
	,HTTP_METHOD_OPTIONS
	,HTTP_METHOD_POST
	,HTTP_METHOD_PUT
	,COUNT_HTTP_METHODS
};

enum {
	HTTP_HDR_ALLOW
	,HTTP_HDR_CONTENT_ENCODING
	,HTTP_HDR_LOCATION
	,HTTP_HDR_WWW_AUTHENTICATE

	,HTTP_HDR_X_REQUEST_ACCESS_LEVEL
	,HTTP_HDR_AUTHORIZATION
	,HTTP_HDR_CONNECTION
	,HTTP_HDR_CONTENT_DISPOSITION

	,HTTP_HDR_CONTENT_LENGTH
	,HTTP_HDR_CONTENT_TYPE
	,HTTP_HDR_ETAG
	,HTTP_HDR_IF_NONE_MATCH
	,COUNT_HTTP_HDRS
};

enum {
	HTTP_AUTH_TYPE_BASIC
	,HTTP_AUTH_TYPE_DIGEST
	,COUNT_HTTP_AUTH_TYPES
};

enum {
	HTTP_CONTENT_TYPE_APPLICATION_OCTET_STREAM
	,HTTP_CONTENT_TYPE_MULTIPART_FORM
	,COUNT_HTTP_CONTENT_TYPES
};

/* 4XX Error reasons */
enum {
	HTTP_ERR_4XX_URI_SLASH
	,HTTP_ERR_4XX_INTR_QUERY
	,HTTP_ERR_4XX_BAD_QUERY_EQ
	,HTTP_ERR_4XX_BAD_QUERY_QUEST
	,HTTP_ERR_4XX_BAD_QUERY_AMP
	,HTTP_ERR_4XX_BAD_HTTP_VER_MAJOR
	,HTTP_ERR_4XX_BAD_HTTP_VER_MINOR
	,HTTP_ERR_4XX_CONTENT_LENGTH_OVERLONG
	,HTTP_ERR_4XX_CONTENT_LENGTH
	,HTTP_ERR_4XX_URI_OVERLONG
	,HTTP_ERR_4XX_BAD_LINE_TERM
	,HTTP_ERR_4XX_CONNECTION_TYPE
	,HTTP_ERR_4XX_BAD_AUTH_TYPE
	,HTTP_ERR_4XX_IF_NONE_MATCH
	,HTTP_ERR_4XX_BAD_HTTP_HEADER
	,HTTP_ERR_4XX_BAD_MULTIPART_BOUNDARY
	,HTTP_ERR_4XX_BAD_DISPOSITION
	,HTTP_ERR_4XX_CONTENT_BOUNDARY
	,HTTP_ERR_4XX_CONTENT_UNDERRUN
	,HTTP_ERR_4XX_CONTENT_TYPE_MISSING
	,HTTP_ERR_4XX_BAD_MULTIPART_TYPE
	,HTTP_ERR_4XX_USER_FUNCTION
	,COUNT_HTTP_ERR_4XX
};

enum {
	HAMS_FLAG_KEEP_ALIVE = 0x01
	,HAMS_FLAG_SENDING   = 0x02
	,HAMS_FLAG_STAGE_REQ = 0x04
	,HAMS_FLAG_ERROR     = 0x08
	,HAMS_FLAG_CONTENT   = 0x20
	,HAMS_FLAG_MULTIPART = 0x40
	,HAMS_FLAG_FIRST_BOUNDARY = 0x80
};

enum {
	HAMS_EXPECT_TX = 0x1
	,HAMS_EXPECT_RX = 0x2
};

/* Match multipart boundary prefix of \r\n-- */
#define BOUND_MATCH_PREFIX 0x2D2D0A0D

/* Forward declaration. */
struct hams_parser_s;

/** @brief This callback type handles all*/
typedef unsigned(*hams_data_cb)(struct hams_parser_s* parser
	,const uint8_t* data, unsigned length);

struct hams_extended {
	uint32_t boundary_match[3];
	uint8_t multipart_boundary[MAX_MULTIPART_BOUNDARY_LENGTH];
	uint8_t multipart_boundary_length;
};

typedef struct hams_parser_s {
	/* User settable. */
	void* user_data;
	hams_data_cb user_data_cb;
	uint16_t response_code;

	/* Read only by user. */
	uint16_t flags;
	unsigned content_length;
	struct hams_extended ext;
	unsigned int method : 4;
	unsigned int content_type : 2;
	unsigned int http_auth_type : 2;

	/* Only read by the multiplexer. */
	uint8_t buffer_expect;

	/* Private. */
	uint8_t err_reason;
	uint8_t buffer_size;

	unsigned int eating : 2;
	unsigned int cur_header : 6;
	uint8_t present;
	const void* cur_label;

	uint8_t buffer[0];
} hams_parser;

/** @brief Callback into user code when HAMS wants to transmit.
 *  @param parser Parser instance
 *  @param data Pointer to data to send; If NULL, hang up the sending connection
 *  @param length How much data to send
 *  @param copy 1 to make a local copy of the data, 0 to not copy (eg readonly)
 *  @return 0 on success; nonzero on error and imply connection hangup.*/
typedef unsigned (*hams_send_cb)
	(hams_parser* parser, const uint8_t* data, unsigned length, unsigned copy);

/** @brief Called when comparing a request's etag to the URL's etag. */
typedef void (*hams_etag_cb)(hams_parser* parser
	,const uint8_t* etag, unsigned length);

/** @brief Callback into user code when HAMS authenticates
 *  @param user_data User data associated with stream
 *  @param username AKA REMOTE_USER (CGI equivalent)
 *  @param password AKA REMOTE_PASSWORD (CGI equivalent)
 *  @return 0 if acceptable, 1 if not. */
typedef int (*hams_auth_cb)(hams_parser* parser
		,const uint8_t* username, const uint8_t* password);


/** @brief Structure for managing multiple HTTP streams. */
extern struct hams_demultiplexer_s {

	/** @brief Initial callback for handler.
	 * Expect the user_data parameter to be NULL.
	 * Should populate the user parameter. */
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

} *g_hams_demultiplexer;

/* Called by the server layer. */

/** @brief Initialize HAMS.
 *  Space must be allocated for g_hams_demultiplexer */
void hams_init(void);

void hams_parser_init(hams_parser* parser);

/** @brief Receive data from remote.
 *  @param stream Opaque stream object associated with connection.
 *  @param data Data from remote; if NULL then no more data.
 *  @param length How much data; if data == NULL then irrelevant
 *  @return Bitmask of HAMS_EXPECT_* values. */
unsigned hams_recv(hams_parser* stream, const uint8_t* data, unsigned length);

void hams_start_response(hams_parser* parser);

/* Called by the user callbacks */

/** @brief Write header line */
void hams_response_header(hams_parser* parser, unsigned header_id
	,const void* data, unsigned data_size);

/** @brief Write response body. */
void hams_write_response_body(hams_parser* parser
	,const void* data, unsigned data_size);

/* String constants useful to the user. */
extern const char* s_http_methods[COUNT_HTTP_METHODS + 1];

/* Interface functions supplied by user. */

/** @brief Extend the length of the shared output buffer.
 *  @param extended_len New length of the output buffer.
 *  If 0 then frees the memory.
 *  @return Actual amount of memory available.
 *  If return value < extended_len then allocation failure (unable to extend)
 *  If return value >= extended_len then success. */
extern unsigned hamsif_extend_output(hams_parser* parser, unsigned extended_len);

#endif
