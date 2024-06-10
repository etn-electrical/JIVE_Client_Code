#include "hams.h"

#include <stdio.h>

#define HAMS_DEBUG

#ifdef HAMS_DEBUG
#include <assert.h>
#include <stdio.h>
#define DERR(...) fprintf(stderr, __VA_ARGS__)
#define _ASSERT(x) assert(x)
#else
#define DERR(...)
#define _ASSERT(x)
#endif

enum {
	EAT_NONE
	/* Dropping all space characters */
	,EAT_SPACE
	/* Like EAT_EOL, but checking for LWS to see if this is a continued line. */
	,EAT_EOL_LWS
	/* Dropping all characters until the end of the line. */
	,EAT_EOL
};

/* Internal enums user does not care about. */
enum {
	HTTP_CONNECTION_TYPE_CLOSE
	,HTTP_CONNECTION_TYPE_KEEP_ALIVE
	,COUNT_HTTP_CONNECTION_TYPES
};

/* String tables. */
const char* s_http_methods[COUNT_HTTP_METHODS + 1] = {
	"DELETE "
	,"GET "
	,"HEAD "
	,"OPTIONS "
	,"POST "
	,"PUT "
	,""
};
#define S_MAX_METHOD_LENGTH sizeof("OPTIONS ")

enum {
	CF_LWS = 0x01
	,CF_SP = 0x02
	,CF_TERM = 0x04
	,CF_CONT = 0x08
	,CF_URI_SEP = 0x10
	,CF_TOK_SEP = 0x10
};

/* NOTE: Response-only headers are properly CAPITALIZED because
 * comparisons for request headers occur only in lower case. */
static const char* s_http_hdrs[COUNT_HTTP_HDRS + 1] = {
	"Allow:"
	,"Content-Encoding:"
	,"Location:"
	,"WWW-Authenticate:"

	,"X-Request-Access-Level:"
	,"authorization:"
	,"connection:"
	,"content-disposition:"

	,"content-length:"
	,"content-type:"
	,"etag:"
	,"if-none-match:"

	,"l"
};
#define S_MAX_HEADER_LENGTH 40
#define S_MAX_HEADER_VALUE_LENGTH  64

#define MIN_MULTIPART_BOUNDARY_LENGTH 4
#define MIN_MPFD_BUFFLEN(x) ((x) + 8)

static const uint8_t s_expected_lengths[COUNT_HTTP_HDRS] = {
	0
	,0
	,0
	,0

	,0
	,36
	,16
	,32

	,8
	,70 + 17
	,HTTP_ETAG_LEN + 4
	,HTTP_ETAG_LEN + 4
};

static const char* s_http_connection_types[COUNT_HTTP_CONNECTION_TYPES + 1] = {
	"close"
	,"keep-alive"
	,"l"
};

static const char* s_http_content_types[COUNT_HTTP_CONTENT_TYPES + 1] = {
	"application/octet-stream"
	,"multipart/form-data"
	,"l"
};

/* These strings are prefixed by the binary value of the http code,
 * for easy lookup. */
static const char* s_http_response_codes[] = {
	/* Special flavor HTTP code to "gently" show the developer
	 * that he/she forgot to set the http response code. */
	"\x00\x00 555 Programmer Did Not Set Response"

	,"\x64\x00 100 Continue"
	,"\x65\x00 101 Switching Protocols"
	,"\xC8\x00 200 OK"
	,"\xC9\x00 201 Created"
	,"\xCA\x00 202 Accepted"
	,"\xCB\x00 203 Non-Authoritative Information"
	,"\xCC\x00 204 No Content"
	,"\xCD\x00 205 Reset Content"
	,"\xCE\x00 206 Partial Content"

	/* 300 */
	,"\x2C\x01 300 Redirection"
	,"\x2D\x01 301 Moved Permanently"
	,"\x2E\x01 302 Found"
	,"\x2F\x01 303 See Other"
	,"\x30\x01 304 Not Modified"
	,"\x31\x01 305 Use Proxy"
	/* 306 is reserved. */
	,"\x33\x01 307 Temporary Redirect"

	/* 400 */
	,"\x90\x01 400 Bad Request"
	,"\x91\x01 401 Unauthorized"
	,"\x92\x01 402 Payment Required"
	,"\x93\x01 403 Forbidden"
	,"\x94\x01 404 Not Found"
	,"\x95\x01 405 Method Not Allowed"
	,"\x96\x01 406 Not Acceptable"
	,"\x97\x01 407 Proxy Authentication Required"
	,"\x98\x01 408 Request Timeout"
	,"\x99\x01 409 Conflict"
	,"\x9A\x01 410 Gone"
	,"\x9B\x01 411 Length Required"
	,"\x9C\x01 412 Precondition Failed"
	,"\x9D\x01 413 Request Entity Too Large"
	,"\x9E\x01 414 Request-URI Too Long"
	,"\x9F\x01 415 Unsupported Media Type"
	,"\xA0\x01 416 Requested Range Not Satisfiable"
	,"\xA1\x01 417 Expectation Failed"
	,"\xA2\x01 418 I'm a teapot" //Why not...
	,"\xAD\x01 429 Too Many Requests"
	,"\xAF\x01 431 Request Header Fields Too Large"

	/* 500 */
	,"\xF4\x01 500 Internal Server Error"
	,"\xF5\x01 501 Not Implemented"
	,"\xF6\x01 502 Bad Gateway"
	,"\xF7\x01 503 Service Unavailable"
	,"\xF8\x01 504 Gateway Timeout"
	,"\xF9\x01 505 HTTP Version Not Supported"
};

#define ACCESS_CONTROL_ALLOW_ORIGIN "Access-Control-Allow-Origin: *\r\n"

static const char s_boiler_plate[] =
"Server: HAMS_BF/0.1\r\n"
"Cache-Control: max-age=0,no-cache,no-store,post-check=0,pre-check=0\r\n"
"Expires: Mon, 26 Jul 1997 05:00:00 GMT\r\n"
"Pragma: no-cache\r\n"
ACCESS_CONTROL_ALLOW_ORIGIN;

static const uint8_t s_char_flags[256] = {
	/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
	/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
	,0 ,CF_LWS | CF_CONT ,CF_TERM ,0 ,0 ,CF_TERM ,0 ,0
	/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
	,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
	/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
	,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
	/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
	,CF_LWS | CF_CONT | CF_SP | CF_URI_SEP,0 ,0 ,0 ,0 ,0 ,CF_URI_SEP ,0

/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
	,CF_TOK_SEP ,CF_TOK_SEP ,0 ,0 ,CF_TOK_SEP ,0 ,0 ,CF_URI_SEP
/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
	,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
	,0 ,0 ,CF_TOK_SEP ,CF_TOK_SEP ,CF_TOK_SEP ,CF_URI_SEP | CF_TOK_SEP ,CF_TOK_SEP ,CF_URI_SEP | CF_TOK_SEP
/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
	,CF_TOK_SEP ,0 ,0 ,0 ,0 ,0 ,0 ,0
/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
	,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
/*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
	,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
	,0 ,0 ,0 ,0 ,CF_TOK_SEP ,0 ,0 ,0
/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
	,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
	,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
	,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
	,0 ,0 ,0 ,CF_TOK_SEP ,0 ,CF_TOK_SEP ,0 ,0
};

static unsigned s_hams_end_response(hams_parser* parser, const uint8_t* data
	,unsigned length);

/* Do a binary search over a range of alphabetically sorted keys. */
static unsigned s_util_match_key(const char* const* key_set, unsigned key_count
	,const uint8_t* datum, unsigned datum_len)
{
	/* Supremum is the least element GREATER than the datum. */
	unsigned maximum = key_count - 1;
	/* Minimum is the greatest element LESS THAN OR EQUAL to the datum. */
	unsigned minimum = 0;

	const uint8_t* x = datum;
	const uint8_t* end = datum + datum_len;
	while(x != end){

		/* Check for an exact match. */
		if('\0' == key_set[minimum][x - datum])
			return minimum;

		/* Set alphabetic character to lower case if appropriate. */
		uint8_t test = *x;
		if(key_set[key_count][0] == 'l'){
			if(test >= 'A' && test <= 'Z')
				test |= 0x20;
		}

		while(minimum <= maximum && test > key_set[minimum][x - datum])
			++minimum;

		if(test != key_set[minimum][x - datum])
			return key_count;

		while(minimum <= maximum && test < key_set[maximum][x - datum])
			--maximum;

		/* Go onto next character. */
		++x;
	}

	/* At this point, we reached the end of the datum.
	 * However if there are more characters in the key then this is NOT a match. */
	return key_set[minimum][x - datum] ? key_count : minimum;
}

/* @param buffer_length Must be at least parser->ext.multipart_boundary_length + 6
 * @return 0 on error, 1 if terminating boundary; 2 if boundary indicates
 * content follows. */
static unsigned s_check_multipart_boundary(hams_parser* parser
	,const uint8_t* buffer_data, unsigned buffer_length)
{
	/* Check for premature termination. */
	if(parser->ext.multipart_boundary_length > buffer_length + 4 + 4
		|| *(const uint32_t*)(buffer_data) != BOUND_MATCH_PREFIX
		|| memcmp(parser->ext.multipart_boundary,
			buffer_data + 4, parser->ext.multipart_boundary_length))
	{
		return 0;
	}

	/* Assume this is not a final boundary. */
	unsigned ret = 2;

	/* Next two chars must be '--' xor '\r\n' */
	const uint8_t* x = buffer_data + parser->ext.multipart_boundary_length + 4;
	if(*x == '-'){
		++x;
		if(*x != '-')
			return 0;
		++x;
		ret = 1;
	}

	/* Look for ending \r\n */
	if(*x == '\r')
		return (*(x + 1) == '\n') ? ret : 0;

	return 0;
}

void hams_parser_init(hams_parser* parser){
	parser->flags = 0;
	parser->method = COUNT_HTTP_METHODS;
	parser->content_length = 0;
	parser->content_type = COUNT_HTTP_CONTENT_TYPES;
	parser->http_auth_type = COUNT_HTTP_AUTH_TYPES;
	parser->response_code = 0;
	parser->err_reason = 0;
	parser->eating = EAT_NONE;
	parser->present = 0;

	parser->ext.multipart_boundary_length = 0;

	parser->cur_label = NULL;
	parser->cur_header = COUNT_HTTP_HDRS;

	/* Expecting METHOD as first field. */
	parser->buffer_expect = S_MAX_METHOD_LENGTH;
}

/* Adding these attributes for "belts and braces"...
 * since we are using label variables we don't want inlining
 * or another copy of the function somewhere. */
__attribute__((__noinline__,__noclone__))
static int hams_parser_feed(hams_parser* parser, const uint8_t* buffer_data
	,unsigned buffer_length)
{

	/* If eating spaces, then seek until non space encountered. */
	switch(parser->eating){
		case EAT_SPACE:
			{
				const uint8_t* x = buffer_data;
				const uint8_t* end = x + buffer_length;

				/* Look for first non-space character. */
				while(x != end){
					if(!(s_char_flags[*x] & CF_LWS)){
						parser->eating = EAT_NONE;
						//DERR("EAT_SPACE %lu\n", x - buffer_data);
						return x - buffer_data;
					}
					++x;
				}

				//DERR("EAT_SPACE %u\n", buffer_length);
				return buffer_length;
			}
			break;

		case EAT_EOL_LWS:
			/* Need more bytes to properly eat... */
			if(buffer_length < 2){
				DERR("EAT_EOL_LWS: NEED MORE %u\n", buffer_length);
				return 0;
			}

			//DERR("EAT_LWS\n");
			/* If this is not an LWS continuation, then check if last line terminator.. */
			if(!(s_char_flags[buffer_data[0]] & CF_LWS)){
				parser->eating = EAT_NONE;

				DERR("b[0] %u = %02hx %c\n", buffer_length, buffer_data[0], buffer_data[0]);
				/* Check for header end. */
				if(buffer_data[0] == '\n' ||
					('\r' == buffer_data[0] && buffer_data[1] == '\n'))
				{
					DERR("HEADER COMPLETE\n");
					if(!(parser->flags & HAMS_FLAG_MULTIPART)){
						/* If there is content, mark it here. */
						const uint8_t* data = NULL;
						if(parser->content_length){
							parser->flags |= HAMS_FLAG_CONTENT;
							data = (const uint8_t*)"";
						}

						/* Inform user that headers have ended. */
						parser->user_data_cb(parser, data, 0);
						if(parser->response_code >= 400)
							goto ST_DISCARD;

						/* If not parsing multibody, let the user do the parsing. */
						/* drop the line terminators. */
						if(!parser->ext.multipart_boundary_length){
							parser->cur_label = &&ST_USER_CONTENT;
							parser->buffer_expect = 1;

							return (buffer_data[0] == '\n') ? 1 : 2;
						}
						DERR("SET UP FOR MULTIPART\n");

						/* Multipart data to parse, initialize parsing state here. */
						const uint32_t u = *(const uint32_t*)(parser->ext.multipart_boundary);
						parser->ext.boundary_match[0] = u;
						parser->ext.boundary_match[0] <<= 24;
						parser->ext.boundary_match[0] |= (BOUND_MATCH_PREFIX >> 8);

						parser->ext.boundary_match[1] = u;
						parser->ext.boundary_match[1] <<= 16;
						parser->ext.boundary_match[1] |= (BOUND_MATCH_PREFIX >> 16);

						parser->ext.boundary_match[2] = u;
						parser->ext.boundary_match[2] <<= 8;
						parser->ext.boundary_match[2] |= (BOUND_MATCH_PREFIX >> 24);

						parser->flags |= HAMS_FLAG_MULTIPART | HAMS_FLAG_FIRST_BOUNDARY;

						/* Add two the content length, since we are counting final
						 * \r\n to the headers as part of the content when we parse.*/
						parser->content_length += 2;
					}

					parser->cur_label = &&ST_MPFD_FIND_BOUNDARY;
					parser->buffer_expect =
						MIN_MPFD_BUFFLEN(parser->ext.multipart_boundary_length);

					/* If we got here, then consuming multipart data. Since
					 * we need leading \r\n for generic case parsing to work,
					 * leave them in.*/
					return 0;
				}

				if(buffer_length < parser->buffer_expect)
					if(!memchr(buffer_data, '\r', buffer_length)){
						//DERR("NO terminator found\n");
						return 0;
					}
				break;
			}

			/* Otherwise this is a continuation line. */
			parser->eating = EAT_EOL;

		case EAT_EOL:
			{
				const uint8_t* x = memchr(buffer_data, '\n', buffer_length);

				if(x){
					parser->eating = EAT_EOL_LWS;
					//DERR("EAT_EOL TERM %lu, REMAIN %.*s\n", x - buffer_data + 1
					//	, buffer_length - (x - buffer_data), x + 1);
					return x - buffer_data + 1;
				}

				//DERR("EAT_EOL %u\n", buffer_length);
				return buffer_length;
			}
			break;

		default:
		case EAT_NONE:
			break;
	}

	/* Optimizing for code size, so no unwieldy switch tables here.
	 * Instead we go back to the same rights computing pioneers had:
	 * Setting the PC directly with a value.
	 * This is just the GCC way to do it.
	 *
	 * Known as label variables in GCC
	 * This may not work with other compilers though.... */
	if(parser->cur_label)
		goto *parser->cur_label;

	/* Label is not set, so we are in the initial state. */
	parser->method = s_util_match_key(s_http_methods, COUNT_HTTP_METHODS
			,buffer_data, buffer_length);

	/* If method is unrecognized return 501 */
	if(COUNT_HTTP_METHODS == parser->method){
		parser->response_code = 501;
		goto ST_DISCARD;
	}

	/* Method is recognized, parse URI protocol and IP next.
	 * only expect http://IP0.IP1.IP2.IP3:PORT */
	parser->cur_label = &&ST_URI_PROTOCOL;
	parser->buffer_expect = MAX_URI_SEG_LENGTH;
	return strlen((const char*)s_http_methods[parser->method]);

ST_URI_PROTOCOL:
	DERR("ST_URI_PROTOCOL\n");
	if('*' == buffer_data[0]){
		/* next character must be space. */
		if(' ' == buffer_data[1]){
			/* TODO */
		}
		parser->response_code = 404;
		goto ST_DISCARD;
	}
	else{

		if('/' == buffer_data[0]){
			parser->cur_label = &&ST_URI_COMPONENT;
		}
		else{
			if(memcmp(buffer_data, "http:/", 6) || '/' != buffer_data[6]){
				parser->response_code = 404;
				goto ST_DISCARD;
			}
			else{
				/* Strip off http:/ */
				parser->cur_label = &&ST_URI_COMPONENT;
				return 6;
			}
		}
	}

ST_URI_COMPONENT:
	{
		_ASSERT(s_char_flags[buffer_data[0]] & CF_URI_SEP);

		/* Find end of URI segment */
		const uint8_t* x = buffer_data + 1;
		while(x != buffer_data + buffer_length &&
			!(s_char_flags[*x] & CF_URI_SEP))
		{
			++x;
		}
		DERR("ST_URI_COMPONENT \"%.*s\"\n", x - buffer_data, buffer_data);

		/* URI component is too long. */
		if(x == buffer_data + buffer_length){
			parser->response_code = 414;
			parser->err_reason = HTTP_ERR_4XX_URI_OVERLONG;
			goto ST_DISCARD;
		}

		if('/' == *x){
			/* Only a '/' may precede this token. */
			if(buffer_data[0] != '/'){
				parser->response_code = 400;
				parser->err_reason = HTTP_ERR_4XX_URI_SLASH;
				goto ST_DISCARD;
			}
		}
		else if(' ' == *x){
			/* Cannot interrupt a query variable. */
			if(buffer_data[0] == '?' || buffer_data[0] == '&'){
				parser->response_code = 400;
				parser->err_reason = HTTP_ERR_4XX_INTR_QUERY;
				goto ST_DISCARD;
			}

			/* HTTP version will be upcoming. */
			parser->cur_label = &&ST_HTTP_VERSION;
		}
		else if('=' == *x){
			/* Query variable value, only ? or & can precede. */
			if(buffer_data[0] != '?' && buffer_data[0] != '&'){
				parser->response_code = 400;
				parser->err_reason = HTTP_ERR_4XX_BAD_QUERY_EQ;
				goto ST_DISCARD;
			}

			/* TODO If processing _method, make sure this is a valid HTTP method. */

		}
		else{
			if('?' == *x){
				/* Only a '/' may precede this token. */
				if(buffer_data[0] != '/'){
					parser->response_code = 400;
					parser->err_reason = HTTP_ERR_4XX_BAD_QUERY_QUEST;
					goto ST_DISCARD;
				}
			}
			else{
				/* This is the '&' character, must be preceded by '=' */
				if(buffer_data[0] != '='){
					parser->response_code = 400;
					parser->err_reason = HTTP_ERR_4XX_BAD_QUERY_AMP;
					goto ST_DISCARD;
				}
			}

			/* TODO Check for _method. */
		}

		parser->user_data_cb(parser, buffer_data, x - buffer_data);
		if(parser->response_code >= 400){
			DERR("URI ERR %u\n", parser->response_code);
			goto ST_DISCARD;
		}

		return x - buffer_data;
	}

ST_HTTP_VERSION:
	//DERR("ST_HTTP_VERSION\n");
	{
		if(memcmp(buffer_data, " HTTP/1.", 8)){
			parser->response_code = 400;
			parser->err_reason = HTTP_ERR_4XX_BAD_HTTP_VER_MAJOR;
			goto ST_DISCARD;
		}

		if('0' != buffer_data[8] && '1' != buffer_data[8]){
			parser->response_code = 400;
			parser->err_reason = HTTP_ERR_4XX_BAD_HTTP_VER_MINOR;
			goto ST_DISCARD;
		}

		parser->buffer_expect = S_MAX_HEADER_LENGTH;
		parser->cur_label = &&ST_HEADER_FIELD;

		/* Line must terminate. */
		if('\r' != buffer_data[9] || '\n' != buffer_data[10]){
			parser->response_code = 400;
			parser->err_reason = HTTP_ERR_4XX_BAD_LINE_TERM;
			goto ST_DISCARD;
		}

		/* Don't consume terminators here; we want to go into
		 * header eating mode.*/
		parser->eating = EAT_EOL;

		return 9;
	}

ST_HEADER_FIELD:
	DERR("ST_HEADER_FIELD %u\n", buffer_length);
	{
		/* Every header should end in a ':' */
		const uint8_t* p = memchr(buffer_data, ':', buffer_length);
		if(!p){
			DERR("BAD FIELD '%.*s'\n", buffer_length, buffer_data);
			parser->response_code = 400;
			parser->err_reason = HTTP_ERR_4XX_BAD_HTTP_HEADER;
			goto ST_DISCARD;
		}
		unsigned len = (p - buffer_data);
		unsigned id = s_util_match_key(s_http_hdrs, COUNT_HTTP_HDRS
			,buffer_data, len + 1);

		/* Don't process etags unless supPorted. */
		if(HTTP_HDR_ETAG == id || HTTP_HDR_IF_NONE_MATCH == id){
			if(!g_hams_demultiplexer->etag_cb)
				id = COUNT_HTTP_HDRS;
		}

		if(COUNT_HTTP_HDRS == id){
			/* Unrecognized header, Just drop the rest of the line. */
			parser->eating = EAT_EOL;
		}
		else{
			parser->cur_header = id;
			parser->cur_label = &&ST_HEADER_VALUE;
			parser->eating = EAT_SPACE;
			parser->buffer_expect = s_expected_lengths[id];
		}

		DERR("HEADER_NAME %u %.*s\n", id, p - buffer_data, buffer_data);


		return len + 1;
	}

ST_HEADER_VALUE:
	DERR("ST_HEADER_VALUE %u %.*s\n", buffer_length, buffer_length, buffer_data);
	{
		switch(parser->cur_header){
			case HTTP_HDR_AUTHORIZATION:
				{
					switch(parser->http_auth_type){
						case HTTP_AUTH_TYPE_BASIC:
							{
								/* TODO */
								/* Parse the Base64 encoded user/pass */

								parser->buffer_expect = S_MAX_HEADER_LENGTH;
								parser->cur_label = &&ST_HEADER_FIELD;
								parser->eating = EAT_EOL;
							}
							/* TODO return bytes consumed by Base64 encoding. */
							return 0;

						case HTTP_AUTH_TYPE_DIGEST:
							{
								/* TODO */
								parser->buffer_expect = S_MAX_HEADER_LENGTH;
								parser->cur_label = &&ST_HEADER_FIELD;
								parser->eating = EAT_EOL;
							}
							break;

						default:
							if(buffer_length >= 6
								&& (s_char_flags[buffer_data[5]] & CF_LWS)
								&& !memcmp(buffer_data, "Basic", 5))
							{
								parser->http_auth_type = HTTP_AUTH_TYPE_BASIC;
								parser->eating = EAT_EOL;

								/* Allow for long username/passes. */
								parser->buffer_expect = MAX_HTTP_BASIC_AUTH_FIELD;
								return 6;
							}
							else if(buffer_length >= 7
								&& (s_char_flags[buffer_data[6]] & CF_LWS)
								&& !memcmp(buffer_data, "Digest", 6))
							{
								parser->http_auth_type = HTTP_AUTH_TYPE_DIGEST;
								parser->eating = EAT_EOL;
								return 7;
							}
							else{
								parser->err_reason = HTTP_ERR_4XX_BAD_AUTH_TYPE;
								parser->response_code = 400;
								goto ST_DISCARD;
							}
					}
				}
				return 0;

			case HTTP_HDR_CONNECTION:
				{
					unsigned id = s_util_match_key(
						s_http_connection_types, COUNT_HTTP_CONNECTION_TYPES
						,buffer_data, buffer_length);

					if(COUNT_HTTP_CONNECTION_TYPES == id){
						parser->err_reason = HTTP_ERR_4XX_CONNECTION_TYPE;
						parser->response_code = 400;
						goto ST_DISCARD;
					}
					/* TODO enable this */
#if 0
					if(id == HTTP_CONNECTION_TYPE_KEEP_ALIVE)
						parser->flags |= HAMS_FLAG_KEEP_ALIVE;
#endif

					parser->buffer_expect = S_MAX_HEADER_LENGTH;
					parser->cur_label = &&ST_HEADER_FIELD;
					parser->eating = EAT_EOL;
					return strlen(s_http_connection_types[COUNT_HTTP_CONNECTION_TYPES]);
				}

			case HTTP_HDR_CONTENT_DISPOSITION:
				/* This must begin with form-data; */
				if(!(parser->flags & HAMS_FLAG_MULTIPART)
						|| buffer_length < 10
						|| memcmp(buffer_data, "form-data;", 10))
				{
					parser->err_reason = HTTP_ERR_4XX_BAD_DISPOSITION;
					parser->response_code = 400;
					goto ST_DISCARD;
				}

				parser->cur_label = &&ST_DISPOSITION_NAME;
				parser->eating = EAT_SPACE;
				return 10;

ST_DISPOSITION_NAME:
				/* If boundary is not defined or value does not begin with name
				 * then bail. */
				if(!parser->ext.multipart_boundary_length
						|| buffer_length < 7
						|| memcmp(buffer_data, "name=", 5))
				{
					parser->err_reason = HTTP_ERR_4XX_BAD_DISPOSITION;
					parser->response_code = 400;
					goto ST_DISCARD;
				}

				{
					/* Pass name parameter to user. */
					const uint8_t* x =
						memchr(buffer_data + 6, '"', buffer_length);
					unsigned len = x - buffer_data - 6;

					if(!x){
					}

					DERR("CONTENT_DISPOSITION NAME: %u %.*s\n", len, len, buffer_data + 6);
					parser->user_data_cb(parser, buffer_data + 6, x - buffer_data - 6);
					if(parser->response_code >= 400)
						goto ST_DISCARD;

					++x;
					parser->buffer_expect = S_MAX_HEADER_LENGTH;
					parser->cur_label = &&ST_HEADER_FIELD;
					parser->eating = EAT_EOL;
					return x - buffer_data + 1;
				}

				break;

			case HTTP_HDR_CONTENT_LENGTH:
				{
					const uint8_t* n = buffer_data;
					const uint8_t* end = n + buffer_length;
					unsigned c = 0;
					while(n != end){
						/* Check for bad content element. */
						if(*n < '0' || *n > '9')
							break;

						c *= 10;
						c += *n - '0';

						if(c > MAX_CONTENT_LENGTH){
							parser->response_code = 413;
							goto ST_DISCARD;
						}
						++n;
					}

					/* Packet must have been cut off or has bad numerical format. */
					if(n == end || !(s_char_flags[*n] & (CF_TERM | CF_LWS))){
						parser->response_code = 400;
						parser->err_reason = HTTP_ERR_4XX_CONTENT_LENGTH;
						goto ST_DISCARD;
					}

					parser->content_length = c;

					parser->buffer_expect = S_MAX_HEADER_LENGTH;
					parser->cur_label = &&ST_HEADER_FIELD;
					parser->eating = EAT_EOL;
					return n - buffer_data;
				}
				break;

			case HTTP_HDR_CONTENT_TYPE:
				{
					const uint8_t* x = memchr(buffer_data, ';', buffer_length);
					if(!x){
						x = memchr(buffer_data, '\r', buffer_length);
					}

					if(!x){
						parser->response_code = 400;
						parser->err_reason = HTTP_ERR_4XX_CONTENT_TYPE_MISSING;
						goto ST_DISCARD;
					}

					DERR("CTYPE %u %02hhx\n", x - buffer_data, *x);

					/* Read the MIME type */
					parser->content_type = s_util_match_key(
							s_http_content_types, COUNT_HTTP_CONTENT_TYPES
							,buffer_data, x - buffer_data);

					/* Consume endpt */
					if(*x == ';')
						++x;

					if(HTTP_CONTENT_TYPE_MULTIPART_FORM == parser->content_type){
						DERR("CTYPE MULTIPART\n");
						/* NOT allowed when already parsing multibodies! */
						if(parser->flags & HAMS_FLAG_MULTIPART){
							parser->response_code = 400;
							parser->err_reason = HTTP_ERR_4XX_BAD_MULTIPART_TYPE;
							goto ST_DISCARD;
						}

						/* Expecting to parse boundary. */
						parser->cur_label = &&ST_CONTENT_TYPE_BOUNDARY;
						parser->eating = EAT_SPACE;
						/* Expect maximum boundary length plus
						 *  boundary= and some padding bytes. */
						parser->buffer_expect = MAX_MULTIPART_BOUNDARY_LENGTH + 9 + 8;
					}
					else{
						DERR("CTYPE OTHER\n");
						parser->buffer_expect = S_MAX_HEADER_LENGTH;
						parser->cur_label = &&ST_HEADER_FIELD;
						parser->eating = EAT_EOL;
					}

					return x - buffer_data;
				}

				if(0){
ST_CONTENT_TYPE_BOUNDARY:

					DERR("BOUNDARY SEARCH %u %.*s\n", buffer_length, buffer_length, buffer_data);
					if(memcmp(buffer_data, "boundary=", 9)){
						parser->response_code = 400;
						parser->err_reason = HTTP_ERR_4XX_CONTENT_BOUNDARY;
						goto ST_DISCARD;
					}

					/* Find end of the boundary. */
					const uint8_t* begin = buffer_data + 9;
					const uint8_t* x = memchr(begin, '\n', buffer_length);
					if(!x || x == begin){
						parser->response_code = 400;
						parser->err_reason = HTTP_ERR_4XX_CONTENT_BOUNDARY;
						goto ST_DISCARD;
					}

					/* Reject boundary if it's too long or too short. */
					if(x - begin > MAX_MULTIPART_BOUNDARY_LENGTH
							|| x - begin < MIN_MULTIPART_BOUNDARY_LENGTH)
					{
						parser->response_code = 400;
						parser->err_reason = HTTP_ERR_4XX_CONTENT_BOUNDARY;
						goto ST_DISCARD;
					}

					/* Copy the boundary. Adjust if there is only \n */
					parser->ext.multipart_boundary_length = x - begin;
					if(*(x - 1) == '\r')
						--parser->ext.multipart_boundary_length;
					memcpy(parser->ext.multipart_boundary, begin,
						parser->ext.multipart_boundary_length);

					DERR("GOT BOUNDARY %.*s\n"
							,parser->ext.multipart_boundary_length
							,parser->ext.multipart_boundary);

					/* Go directly back to reading headers...got the \r\n... */
					parser->buffer_expect = S_MAX_HEADER_LENGTH;
					parser->cur_label = &&ST_HEADER_FIELD;
					parser->eating = EAT_EOL;
					DERR("BOUNDLEN %u CONSUMED %u\n"
							,parser->ext.multipart_boundary_length, x - buffer_data + 1);
					/* Don't consume \r\n; just an easy way to terminate the line... */
					return x - buffer_data + 1 - 2;
				}

			case HTTP_HDR_ETAG:
			case HTTP_HDR_IF_NONE_MATCH:
				/* Expect quotes. */
				{
					const uint8_t* x = memchr(buffer_data + 1, '"', buffer_length);
					if(!x || buffer_data[0] != '"'
							|| (x - buffer_data + 1) > HTTP_ETAG_LEN)
					{
						parser->response_code = 400;
						parser->err_reason = HTTP_ERR_4XX_IF_NONE_MATCH;
						goto ST_DISCARD;
					}

					/* Send etag value to user. */
					g_hams_demultiplexer->etag_cb(parser, buffer_data, x - buffer_data + 1);

					parser->buffer_expect = S_MAX_HEADER_LENGTH;
					parser->cur_label = &&ST_HEADER_FIELD;
					parser->eating = EAT_EOL;
					return x - buffer_data + 1;
				}

			default:
				break;
		}
	}

ST_MPFD_FIND_BOUNDARY:
	DERR("FINDING MULTIPART BOUND %u\n", buffer_length);
	{
		/* If looking for the end of the boundary, we must always have enough
		 * data to find the boundary, that is '--' at begin
		 * and '--\r\n' at the end.
		 * If not enough data, this is an error. */
		if(buffer_length < MIN_MPFD_BUFFLEN(parser->ext.multipart_boundary_length)){
			parser->response_code = 400;
			parser->err_reason = HTTP_ERR_4XX_CONTENT_UNDERRUN;
			goto ST_DISCARD;
		}

		/* Look for boundary within the data.
		 * Choose an end such that our 4 byte comparison is properly aligned.*/
		const uint8_t* end = buffer_data + (buffer_length & ~0x3) - 5;
		const uint8_t* x = memchr(buffer_data, '\r', end - buffer_data);

		/* Must see boundary characters on first go through. */
		if(!x && (parser->flags & HAMS_FLAG_FIRST_BOUNDARY)){
			parser->response_code = 400;
			parser->err_reason = HTTP_ERR_4XX_BAD_MULTIPART_BOUNDARY;
			goto ST_DISCARD;
		}

		unsigned off = 0;
		if(x){
			/* We expect to get this line if we are doing a boundary match, because
			 * the next major code clause will return if it finds a possible match. */
			if(x == buffer_data){
				switch(s_check_multipart_boundary(parser, buffer_data, buffer_length)){
					case 0:
						/* This MUST match on the first iteration or we have a bad
						 * start to the multipart. */
						if(parser->flags & HAMS_FLAG_FIRST_BOUNDARY){
							parser->response_code = 400;
							parser->err_reason = HTTP_ERR_4XX_BAD_MULTIPART_BOUNDARY;
							goto ST_DISCARD;
						}
						else{

							/* Negative match, just go past \r\n */
							x += 2;
							off = 2;
							break;
						}

					case 1:
						/* Reached end of data, do nothing. */
						return parser->ext.multipart_boundary_length + 8;

					case 2:
						/* At least got through the first. */
						parser->flags &= ~HAMS_FLAG_FIRST_BOUNDARY;

						/* Announce new field with empty string. */
						parser->user_data_cb(parser, (const uint8_t*)"", 0);
						if(parser->response_code >= 400)
							goto ST_DISCARD;

						/* Beginning of headers.*/
						parser->buffer_expect = S_MAX_HEADER_LENGTH;
						parser->cur_label = &&ST_HEADER_FIELD;
						parser->eating = EAT_EOL;
						return parser->ext.multipart_boundary_length + 2;

						/* Not possible. */
					default:
						break;
				}
			}

			/* Search for next possible occurrence of the boundary. */
			do{
				unsigned offset = x - buffer_data;
				unsigned byte_offset = offset & 0x3;

				if(*(x + 1) == '\n'){
					/* Do a quick test for negative matching; that is if this test fails
					 * this is a NOT a match; but if it passes it MAY be a match. */
					const uint32_t next = *(const uint32_t*)(x + 4 - byte_offset);
					unsigned possible = 0;
					if(byte_offset && parser->ext.boundary_match[3 - byte_offset] == next)
						possible = 1;
					else if(*(const uint32_t*)(parser->ext.multipart_boundary) == next)
						possible = 1;

					/* If there is a possible match, push what data we can to the user. */
					if(possible){
						DERR("POSSIBLE MATCH, SENDING TO USER\n");
						parser->user_data_cb(parser, buffer_data + off,
							x - buffer_data - off);
						if(parser->response_code >= 400)
							goto ST_DISCARD;
						return x - buffer_data;
					}
				}

				/* Consumed these two chars. */
				x += 2;

				/* Does not match, just continue. */
				x = memchr(x, '\r', end - x);
			} while(x);
		}
		else{
			DERR("DID NOT SEE BOUNDARY\n");
		}

		/* Found no signs, just pass the whole buffer onto the user. */
		parser->user_data_cb(parser, buffer_data + off, end - buffer_data - off);
		if(parser->response_code >= 400)
			goto ST_DISCARD;

		return end - buffer_data;
	}

ST_USER_CONTENT:
	{
		parser->user_data_cb(parser, buffer_data, buffer_length);
		if(parser->response_code >= 400)
			goto ST_DISCARD;

		return buffer_length;
	}

ST_DISCARD:
	/* Response code must be nonzero at this point. */
	_ASSERT(parser->response_code);

	//DERR("ST_DISCARD\n");
	parser->cur_label = &&ST_DISCARD;
	return buffer_length;
}

void hams_init(void){
	g_hams_demultiplexer->staged_len = 0;
}

unsigned hams_recv(hams_parser* parser, const uint8_t* data, unsigned length){
	_ASSERT(parser);

	/* Check for close. */
	if(!data){
		return 0;
	}

	unsigned present = parser->present;

	/* TODO optimize for fewer memcpy() calls? */
	while(length){

		/* Figure out how much to append to current buffer. */
		unsigned amount = parser->buffer_size - present;
		if(amount > length)
			amount = length;

		memcpy(parser->buffer + present, data, amount);
		length -= amount;
		data += amount;
		present += amount;

		const uint8_t* src = parser->buffer;

		while(present){
			if(!parser->eating){
				if(present < parser->buffer_expect){
					if(!memchr(src, '\r', present))
						break;
				}
			}
		
			/* If parsing content data, then only pass on at most the rest of the content. */
			unsigned parse_len = present;
			if(parser->flags & HAMS_FLAG_CONTENT && parser->content_length < present)
				parse_len = parser->content_length;

			/* Parse available data; if error then request to close. */
			//DERR("-----PFEED EXP %u %u-----\n",
			//parser->buffer_expect, parse_len);
			int result = hams_parser_feed(parser, src, parse_len);

			DERR("PFEED RESULT %u EXPECT %u\n", result, parser->buffer_expect);

			/* If no data parsed then need more data. */
			if(!result){
				DERR("-----PARSER %p MORE DATA %u, LEN %u-----\n"
					, parser, parser->buffer_expect, length);
				break;
			}

			/* adjust remaining data. */
			present -= result;
			src += result;

			/* reduce content length if parsing content. */
			if(parser->flags & HAMS_FLAG_CONTENT){
				parser->content_length -= result;
				/* If reached end of content length, let user know. */
				if(!parser->content_length){
					parser->flags &= ~HAMS_FLAG_CONTENT;

					DERR("REACHED END OF REQ BODY\n");
					parser->user_data_cb(parser, NULL, 0);
				}
			}

			/* Check if method is set then we can begin sending response. */
			if(parser->response_code){
				DERR("PARSER SET RESPONSE %u REASON %u\n"
						, parser->response_code, parser->err_reason);
				parser->flags |= HAMS_FLAG_SENDING | HAMS_FLAG_STAGE_REQ;
				parser->user_data_cb = s_hams_end_response;
				/* Begin request if we can. */
				http_server_output_sent(NULL, 0);
				return HAMS_EXPECT_TX;
			}
		}

		if(present){
			/* Move remaining data back to beginning. */
			memmove(parser->buffer, src, present);
		}
	}

	DERR("-----PARSER %p PRESENT %u-----\n", parser, present);
	parser->present = present;

	return HAMS_EXPECT_RX;
}

static unsigned s_hams_end_response(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	if(!data)
		hams_response_header(parser, COUNT_HTTP_HDRS, NULL, 0);
	return 0;
}

void hams_start_response(hams_parser* parser){
	_ASSERT(parser);

	/* Response starts with something like
	 *
	 * HTTP/1.0 RES Explanation
	 *
	 * e.g HTTP/1.0 200 OK */

	g_hams_demultiplexer->staged_len = 0;
	uint8_t* o = g_hams_demultiplexer->output_buffer;
	memcpy(o, "HTTP/1.0", 8);
	o += 8;

	_ASSERT(parser->response_code);

	/* Lookup corresponding HTTP code. */
	/* TODO use a binary search here instead. */
	for(unsigned i = 0; i < sizeof(s_http_response_codes) / sizeof(char*); ++i){
		const uint16_t* code = (const uint16_t*)s_http_response_codes[i];
		if(*code == parser->response_code){
			const char* s = s_http_response_codes[i] + 2;
			while(*s){
				*o = *s;
				++o;
				++s;
			}
			break;
		}
	}

	*o = '\r';
	++o;
	*o = '\n';
	++o;

	g_hams_demultiplexer->staged_len = o - g_hams_demultiplexer->output_buffer;

	/* Output boilerplate. */
	memcpy(o, s_boiler_plate, sizeof(s_boiler_plate) - 1);
	g_hams_demultiplexer->staged_len += sizeof(s_boiler_plate) - 1;

	/* Write out Connection header. */
	unsigned long conntype = (HAMS_FLAG_KEEP_ALIVE & parser->flags)
		? HTTP_CONNECTION_TYPE_KEEP_ALIVE : HTTP_CONNECTION_TYPE_CLOSE;
	hams_response_header(parser, HTTP_HDR_CONNECTION, (void*)&conntype, 0);

	/* Give user a chance to transmit headers. */
	parser->user_data_cb(parser, NULL, 0);
}

void hams_response_header(hams_parser* parser, unsigned header_id
	,const void* data, unsigned data_size)
{
	/* Count \r\n. */
	unsigned len = 2;
	unsigned value_len = 0;
	uint8_t* o;

	/* User indicates all headers are written. */
	if(COUNT_HTTP_HDRS > header_id){
		/* Count length of header and space and \r\n line termination */
		len += strlen(s_http_hdrs[header_id]) + 1;
	}
	else{
		header_id = COUNT_HTTP_HDRS;
	}

	/* This holds the address of the writing code.*/
	const void* bounce = NULL;

	switch(header_id){

		/* TODO */
		case HTTP_HDR_ALLOW:
			/* Expecting a bitfield listing all allowable methods. */
			if(!bounce){
				bounce = &&WRITE_HTTP_HDR_ALLOW;
				value_len = 0;
			}
			else{
				WRITE_HTTP_HDR_ALLOW:
				return;
			}
			break;

		case HTTP_HDR_CONNECTION:
			/* Expecting integer data */
			if(!bounce){
				unsigned type = *(unsigned long*)(data);
				value_len = strlen(s_http_connection_types[type]);
				bounce = &&WRITE_HTTP_HDR_CONNECTION;
			}
			else{
				WRITE_HTTP_HDR_CONNECTION:
				/* Write to memory. */
				memcpy(o, s_http_connection_types[*(unsigned long*)(data)], value_len);
				return;
			}
			break;

		case HTTP_HDR_CONTENT_DISPOSITION:
			/* Will write out:
			 *
			 * attachment; filename=FILENAME
			 *
			 * 21 bytes precede FILENAME */
			if(!bounce){
				bounce = &&WRITE_HTTP_HDR_DISPOSITION;
				value_len = data_size + 21;
			}
			else{
				WRITE_HTTP_HDR_DISPOSITION:
				memcpy(o, "attachment; filename=", 21);
				o += 21;
				memcpy(o, data, data_size);
				return;
			}
			break;

		case HTTP_HDR_CONTENT_ENCODING:
			/* Will write out:
			 *
			 * ENCTYPE
			 *
			 * which is most likely gzip.. */
			if(!bounce){
				bounce = &&WRITE_HTTP_HDR_ENCODING;
				value_len = data_size;
			}
			else{
				WRITE_HTTP_HDR_ENCODING:
				memcpy(o, data, data_size);
				return;
			}
			break;

		case HTTP_HDR_CONTENT_LENGTH:
			/* Expecting a positive integer for the content length. */
			/* TODO */
			if(!bounce){
				/* Maximum of 9 digits possible. */
				value_len = 9;
				bounce = &&WRITE_HTTP_HDR_CONTENT_LENGTH;
			}
			else{
				WRITE_HTTP_HDR_CONTENT_LENGTH:
				{
					unsigned content_length = *(unsigned long*)(data);
					parser->content_length = content_length;
					uint8_t* x = o + 8;
					while(content_length){
						*x = (content_length % 10) + '0';
						content_length /= 10;
						--x;
					}
					*x = ' ';
					while(o != x){
						*o = ' ';
						++o;
					}
					return;
				}
			}
			break;

		case HTTP_HDR_CONTENT_TYPE:
			/* Expecting string for the Content Type. */
			if(!bounce){
				bounce = &&WRITE_HTTP_HDR_CONTENT_TYPE;
				value_len = data_size;
			}
			else{
				WRITE_HTTP_HDR_CONTENT_TYPE:
				memcpy(o, data, data_size);
				return;
			}
			break;

		case HTTP_HDR_ETAG:
			/* Expecting string for the ETAG. */
			if(!bounce){
				/* Will print double quoted ETAG. */
				bounce = &&WRITE_HTTP_HDR_ETAG;
				value_len = data_size + 2;
			}
			else{
				WRITE_HTTP_HDR_ETAG:
				*o = '"';
				++o;
				memcpy(o, data, data_size);
				o += data_size;
				*o = '"';
				return;
			}
			break;

		case HTTP_HDR_LOCATION:
			if(!bounce){
				/* Expecting URI string for the location.
				 * Will print
				 * http://HOSTNAME/URI
				 *
				 * so 7 prefix characters, hostname length, and URI string
				 * (including leading '/')
				 * */
				bounce = &&WRITE_HTTP_HDR_LOCATION;
				value_len = data_size + 7 +
					strlen(g_hams_demultiplexer->hostname);
			}
			else{
				WRITE_HTTP_HDR_LOCATION:
				memcpy(o, "http://", 7);
				o += 7;
				/* Copy hostname. */
				memcpy(o, g_hams_demultiplexer->hostname, value_len - data_size - 7);
				o += value_len - data_size - 7;
				/* Copy uri */
				memcpy(o, data, data_size);
				return;
			}
			break;

		case HTTP_HDR_WWW_AUTHENTICATE:
			/* Expecting unsigned integer. */
			if(!bounce){
				/* TODO */
				bounce = &&WRITE_HTTP_HDR_WWW_AUTHENTICATE;
				value_len = 0;
			}
			else{
				WRITE_HTTP_HDR_WWW_AUTHENTICATE:
				{
					unsigned access = *(unsigned long*)(data);
					/* TODO */
					return;
				}
			}
			break;

		case HTTP_HDR_X_REQUEST_ACCESS_LEVEL:
			/* Expecting unsigned integer. */
			if(!bounce){
				/* TODO */
				bounce = &&WRITE_HTTP_HDR_ACCESS_LEVEL;
				value_len = 0;
			}
			else{
				WRITE_HTTP_HDR_ACCESS_LEVEL:
				{
					unsigned access = *(unsigned long*)(data);
					/* TODO */
					return;
				}
			}
			break;

		default:
			break;
	}

	len += value_len;
	/* Check if these data will fit in the current output buffer. */
	unsigned extlen = len + g_hams_demultiplexer->staged_len;
	if(hamsif_extend_output(parser, extlen) < extlen){
		/* Will not fit, flush buffer and reallocate */
		g_hams_demultiplexer->send_cb(parser
			,g_hams_demultiplexer->output_buffer, g_hams_demultiplexer->staged_len, 1);
		g_hams_demultiplexer->staged_len = 0;

		/* Try to allocate again. */
		if(hamsif_extend_output(parser, len)){
			_ASSERT(0);
			/* TODO handle this error, cannot allocate data. */
			return;
		}
	}

	/* Initialize output pointer. */
	o = g_hams_demultiplexer->output_buffer
		+ g_hams_demultiplexer->staged_len;

	if(COUNT_HTTP_HDRS > header_id){
		/* Write out the header (always includes ':'.) */
		o = (uint8_t*)stpcpy((char*)o, s_http_hdrs[header_id]);
		*o = ' ';
		++o;

		/* Write trailing \r\n */
		o[value_len] = '\r';
		o[value_len + 1] = '\n';

		/* Adjust count of bytes in buffer and bounce back
		 * to the write section. */
		g_hams_demultiplexer->staged_len += len;
		goto *bounce;
	}
	else{
		/* Write out header terminator. */
		memcpy(o, "\r\n", 2);
		g_hams_demultiplexer->staged_len += len;

		/* Send out the header. */
		g_hams_demultiplexer->send_cb(parser
			,g_hams_demultiplexer->output_buffer, g_hams_demultiplexer->staged_len, 1);
		g_hams_demultiplexer->staged_len = 0;

		parser->flags &= ~HAMS_FLAG_STAGE_REQ;
		hamsif_extend_output(parser, 0);
	}
}
