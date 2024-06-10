#include "server.h"

#define HAMS_DEBUG

#ifdef HAMS_DEBUG
#include <stdio.h>
#include <assert.h>
#define DERR(...) fprintf(stderr, __VA_ARGS__)
#define _ASSERT(x) assert(x)
#else
#define DERR(...)
#define _ASSERT(x)
#endif

enum {
	ROOT_ROOT
	,ROOT_DIAG
	,ROOT_FW
	,ROOT_INTERFACE
	,ROOT_REST
	,COUNT_ROOTS
};

enum {
	FW_APPLICATION
	,FW_METER
	,FW_WWW
	,COUNT_FW
};

enum {
	FW_OP_CLEAR
	,FW_OP_UPLOAD
	,COUNT_FW_OPS
};

#define HTTP_REQ_BUFFER_BYTES 94
#define HTTP_MIN_REQ_COUNT \
	((sizeof(http_server_req_state) + HTTP_REQ_BUFFER_BYTES + HTTP_POOL_BLOCK_SIZE - 1) / HTTP_POOL_BLOCK_SIZE)

_Static_assert((sizeof(http_server_req_state) + HTTP_REQ_BUFFER_BYTES) < 256,
		"HTTP_REQ_BUFFER_BYTES too big!");

_Static_assert(HTTP_MIN_OUT < HTTP_POOL_BLOCK_COUNT,
	"HTTP_POOL cannot fit min output buffer requirements.");

_Static_assert(HTTP_MIN_OUT < SERV_MASK_ALLOC,
	"SERV_MASK_ALLOC is not big enough to store minimum output buffer.");

_Static_assert(HTTP_MIN_OUT < SERV_MASK_ALLOC,
	"SERV_MASK_ALLOC is not big enough to store request state.");

_Static_assert(HTTP_MIN_REQ_COUNT < SERV_MASK_ALLOC,
	"SERV_MASK_ALLOC is not big enough to store request state.");

static uint8_t s_server_memory[sizeof(http_server) + HTTP_POOL_SIZE];

struct hams_demultiplexer_s *g_hams_demultiplexer;

static http_server* s_server = (http_server*)&s_server_memory;

static const char* s_root_paths[COUNT_ROOTS] = {
	"/"
	,"/diag"
	,"/fw"
	,"/if"
	,"/rs"
};

static const char* s_fw_paths[COUNT_FW] = {
	"/app"
	,"/meter"
	,"/www"
};

static const char* s_default_files[COUNT_WWW_STD_FILE] = {
	"/not_found.html"
	,"/default.html"
	,"/index.html"
};


/*****************************************************************************/
/* /fw Firmware section. */
/*****************************************************************************/

#if 0
static unsigned s_fw_send(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	/* TODO */
	http_server_req_state *rs = http_server_get_state(parser);

	unsigned len = http_serverif_send_size(rs);
	unsigned remaining = METER_BUFFER_SIZE - rs->p.url_param[1];
	if(remaining  < len)
		len = remaining;

	g_hams_demultiplexer->send_cb(parser,
		s_meter_fw_buffer + rs->p.url_param[1], len, 0);
	rs->p.url_param[1] += len;

	return 0;
}

static unsigned s_fw_prepare_send(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	/* TODO */
	unsigned long l = METER_BUFFER_SIZE;
	hams_response_header(parser, HTTP_HDR_CONTENT_LENGTH, (void*)&l, 0);
	hams_response_header(parser, HTTP_HDR_CONTENT_TYPE, "application/octet-stream", 24);
	hams_response_header(parser, COUNT_HTTP_HDRS, NULL, 0);

	parser->user_data_cb = s_fw_send;
	return 0;
}
#endif

static unsigned s_fw_op_clear(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	DERR("FW_OP_CLEAR\n");
	/* Wait for headers to process. */
	if(data)
		return 0;

	/* There should not be any request body.. */
	if(parser->content_length){
		parser->response_code = 413;
		return 0;
	}

	/* Return 202 Accepted if erase has initialized
	 * (ie flash erasing will block for a long time)
	 * or 200 if ready to use. */
	parser->response_code =
		http_serverif_clear_memory() ? 202 : 200;
	return 0;
}

/* Looking for the following sequence of calls:
 *
 * (parser, "", 0)
 * (parser, "", 0)
 * (parser, "fw", 2)
 * (parser, FWDATA, len)
 * ....
 * (parser, FWDATA, len)
 * (parser, NULL, 0)
 *
 * */
static unsigned s_fw_op_upload(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	http_server_req_state *rs = http_server_get_state(parser);

	DERR("FW_UPLOAD %p %u LEN %u\n", data, rs->p.fwop.state, length);
	switch(rs->p.fwop.state){
		case 0:
		case 1:
			/* Expect empty string. */
			if(!data || length){
				parser->response_code = 403;
				return 0;
			}
			++rs->p.fwop.state;
			break;

		case 2:
			if(!data || 2 != length || strncmp("fw", (const char*)data, 2)){
				parser->response_code = 403;
				return 0;
			}
			++rs->p.fwop.state;
			break;

		default:
			/* TODO: handle aborted FW upload. */
			if(length){
				unsigned code = http_serverif_fw_recv(data, length);
				switch(code){
					case SERV_FW_ERR_OK:
						break;

					case SERV_FW_ERR_OVERFLOW:
						parser->response_code = 413;
						break;

					case SERV_FW_ERR_HW:
						parser->response_code = 500;
						break;

					case SERV_FW_ERR_LOCKED:
						parser->response_code = 503;
						break;

					default:
						break;
				}
			}
			else{
				/* Finished receiving data. */
				parser->response_code = 200;
			}
			break;
	}

	return 0;
}

static unsigned s_fw_query(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	http_server_req_state *rs = http_server_get_state(parser);

	DERR("FW_QUERY\n");

	if('=' == data[0]){
		/* Only accepting 1 query value. */
		if(' ' != data[length]){
			parser->response_code = 400;
			return 0;
		}

		rs->p.fwop.state = 0;
		if(6 == length && !strncmp("clear", (const char*)data + 1, 5)){
			DERR("FW_QUERY GOT CLEAR\n");
			parser->user_data_cb = s_fw_op_clear;
			return 0;
		}
		else if(7 == length && !strncmp("upload", (const char*)data + 1, 6)){
			DERR("FW_QUERY GOT UPLOAD\n");
			parser->user_data_cb = s_fw_op_upload;
			return 0;
		}
	}
	else{
		/* Check query field name. */
		if(length == 3 && !strncmp("op", (const char*)data + 1, 2)){
			return 0;
		}
	}

	/* Goes to the reject pile. */
	parser->response_code = 403;
	return 0;
}

static unsigned s_fw_node(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	http_server_req_state *rs = http_server_get_state(parser);

	rs->p.fwop.target = COUNT_FW;
	for(unsigned i = 0; i < COUNT_FW; ++i){
		unsigned plen = strlen(s_fw_paths[i]);
		if(plen == length && !strncmp((const char*)data, s_fw_paths[i], length)){
			rs->p.fwop.target = i;
			break;
		}
	}

	DERR("S_FW_NODE \"%.*s\" len %u match %u\n", length, data, length, rs->p.fwop.target);

	switch(rs->p.fwop.target){
		case FW_WWW:
			{
				/* This must be a POST */
				if(parser->method != HTTP_METHOD_POST){
					parser->response_code = 405;
					return 0;
				}

				/* TODO If area is not clear then reject with 409 */
			}
			break;

		case FW_APPLICATION:
			/* TODO. */

		case FW_METER:
			break;

		/* If not found then 404. */
		default:
			parser->response_code = 404;
			return 0;
	}

	if(HTTP_METHOD_POST == parser->method){
		/* Make sure at least one query variable supplied with the POST. */
		if(data[length] != '?'){
			parser->response_code = 400;
			return 0;
		}
		parser->user_data_cb = s_fw_query;
	}
	else{
		/* TODO */
#if 0
		if(data[length] != ' '){
			parser->response_code = 404;
			return 0;
		}
		parser->user_data_cb = s_fw_prepare_send;
		rs->p.url_param[1] = 0;
#endif
	}

	return 0;
}

/*****************************************************************************/


/*****************************************************************************/
/* /if Web page handling. */
/*****************************************************************************/

static unsigned s_xmit_file(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	DERR("URI XMIT %p\n", parser);
	http_server_req_state *rs = http_server_get_state(parser);

	unsigned len = http_serverif_send_size(rs);
	if(parser->content_length < len)
		len = parser->content_length;

	g_hams_demultiplexer->send_cb(parser, rs->p.file_ptr, len, 0);
	parser->content_length -= len;
	rs->p.file_ptr += len;

	if(0 == parser->content_length)
		rs->flags |= REQ_FLAG_FIN;

	return 0;
}

static unsigned s_uri_response_begin(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	/* Some expected conditions. */
	_ASSERT(!parser->content_length);

	DERR("URI RESP BEGIN %p %u\n", parser, parser->response_code);
	http_server_req_state *rs = http_server_get_state(parser);

	/* Send back 404 file if 404; otherwise actual file. */
	unsigned idx = (parser->response_code != 404) ?
		rs->p.fs_ctx.low : s_server->file_indices[WWW_STD_FILE_DEFAULT];
	unsigned filesize = fw_www_get_filesize(s_server->web_pages, idx);
	const fw_www_file* www_file = fw_www_get_file(s_server->web_pages, idx);

	/* Send back etag if not in error. */
	if(200 == parser->response_code || 304 == parser->response_code)
		hams_response_header(parser, HTTP_HDR_ETAG, www_file->etag, 32);

	/* 304 and HEAD requests are headers only. */
	unsigned send_body = parser->response_code != 304
		 && HTTP_METHOD_GET == parser->method;

	if(send_body){
		DERR("BEGIN FILE SENDBACK IDX %u SIZE %u\n", idx, filesize);
		unsigned long l = filesize;
		const char* mime =
			fw_www_get_mimetype(s_server->web_pages, www_file->mime_type);
		unsigned mime_len =
			fw_www_get_mimetype_length(s_server->web_pages, www_file->mime_type);

		hams_response_header(parser, HTTP_HDR_CONTENT_LENGTH, (void*)&l, 0);
		hams_response_header(parser, HTTP_HDR_CONTENT_TYPE, mime, mime_len);

		_ASSERT(parser->content_length == l);

		/* If gzipped encoded, add header */
		if(www_file->encoding)
			hams_response_header(parser, HTTP_HDR_CONTENT_ENCODING, "gzip", 4);

		/* Prepare to send content body after header is finished sending. */
		rs->p.file_ptr = www_file->data;
		parser->user_data_cb = s_xmit_file;
	}
	else{
		DERR("NOT SENDING BODY\n");
		/* Nothing more to send.
		 * Not strictly necessary to NULL, but good for stomping bugs.*/
		parser->user_data_cb = NULL;
	}

	/* No more headers to send back. */
	hams_response_header(parser, COUNT_HTTP_HDRS, NULL, 0);

	return 0;

}

static unsigned s_uri_hdrs_end(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	DERR("URI ENDED %p\n", parser);
	http_server_req_state *rs = http_server_get_state(parser);

	/* There should not be any request body.. */
	if(parser->content_length){
		parser->response_code = 413;
		return 0;
	}

	parser->user_data_cb = s_uri_response_begin;

	unsigned ret = fw_www_match_count(&rs->p.fs_ctx);
	if(ret != 1){
		DERR("FILE NOT FOUND\n");
		parser->response_code = 404;
		return 0;
	}

	if(rs->flags & REQ_FLAG_ETAG_MATCH){
		parser->response_code = 304;
		return 0;
	}

	parser->response_code = 200;
	return 0;
}

static void s_uri_etag(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	http_server_req_state *rs = http_server_get_state(parser);

	/* ETAG for files must be 32 characters */
	if(32 != length)
		return;

	/* Must be a matching file. */
	if(1 != fw_www_match_count(&rs->p.fs_ctx))
		return;

	const fw_www_file* www_file = fw_www_get_file(s_server->web_pages, rs->p.fs_ctx.low);
	if(memcmp(www_file->etag, data, 32))
		return;

	/* Etags match, we will be doing a 304 instead of a 200. */
	rs->flags |= REQ_FLAG_ETAG_MATCH;
}

static unsigned s_uri_part(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	http_server_req_state *rs = http_server_get_state(parser);

	/* TODO: reject query variables? Postel would say no... */

	/* If FS segment then match. */
	if(data[0] == '/')
		fw_www_search(s_server->web_pages, (const char*)data, length, &rs->p.fs_ctx);

	/* Check if this is the final segment. */
	if(data[length] == ' ')
		parser->user_data_cb = s_uri_hdrs_end;

	/* Wait for next piece. */
	return 0;
}

static unsigned s_uri_404(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	fprintf(stderr, "URI_404 %.*s\n", length, data);
	if(!data || !length)
		return s_uri_hdrs_end(parser, data, length);
	else if(data[length] == ' ')
		parser->user_data_cb = s_uri_hdrs_end;
	return 0;
}

/*****************************************************************************/

static unsigned s_on_redirect_headers(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	const char* fname = s_default_files[WWW_STD_FILE_INDEX];
	hams_response_header(parser, HTTP_HDR_LOCATION, fname, strlen(fname));
	hams_response_header(parser, COUNT_HTTP_HDRS, NULL, 0);
	parser->user_data_cb = NULL;
	return 0;
}

static unsigned s_uri_redirect(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	/* Just ignore all. */
	if(data)
		return 0;

	/* There should not be any request body.. */
	if(parser->content_length){
		parser->response_code = 413;
		return 0;
	}

	/* Redirect to default. */
	parser->response_code = 301;
	parser->user_data_cb = s_on_redirect_headers;
	return 0;

}

static unsigned s_uri_root(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	if(!length){
		return 0;
	}

	http_server_req_state *rs = http_server_get_state(parser);

	unsigned choice = COUNT_ROOTS;
	for(unsigned i = 0; i < COUNT_ROOTS; ++i){
		unsigned len = strlen((const char*)s_root_paths[i]);
		if(length == len && !strncmp((const char*)data, s_root_paths[i], length)){
			choice = i;
			break;
		}
	}

	DERR("S_URI_ROOT \"%.*s\" len %u match %u\n", length, data, length, choice);

	switch(choice){

		case ROOT_ROOT:
			/* Otherwise redirect to index page. */
			parser->user_data_cb = s_uri_redirect;
			return 0;

		case ROOT_FW:
			if(' ' == data[length]){
				/* Not a valid endpoint. */
				parser->response_code = 404;
			}
			else{
				parser->user_data_cb = s_fw_node;
			}
			break;

		case ROOT_INTERFACE:
			/* Only GET and HEAD methods are acceptable. */
			if(parser->method != HTTP_METHOD_GET
					&& parser->method != HTTP_METHOD_HEAD)
			{
				parser->response_code = 405;
				/* TODO populate hams_parser bitfield with Allow mask. */
				return 0;
			}

			/* If no web pages are loaded then redirect to
			 * download page. */
			if(NULL == s_server->web_pages){
				/* TODO */
			}

			/* /if should redirect to /if/index.html. */
			if(' ' == data[length]){
				parser->user_data_cb = s_uri_redirect;
				return 0;
			}

			fw_www_init_search_ctx(s_server->web_pages, &rs->p.fs_ctx);
			parser->user_data_cb = s_uri_part;
			break;

		case ROOT_DIAG:
			parser->response_code = 501;
			return 0;

			/* Only GET and HEAD methods are acceptable. */
			if(parser->method != HTTP_METHOD_GET && parser->method != HTTP_METHOD_HEAD){
				parser->response_code = 405;
				return 0;
			}

			/* Currently only /diag is valid. */
			if(' ' != data[length]){
				parser->response_code = 404;
				return 0;
			}

			/* Note diagnostics are dynamic data; content-length CANNOT be guaranteed! */

			/* TODO */;

		case ROOT_REST:
			/* TODO */
			parser->response_code = 501;
			return 0;

		/* If root not found the 404. */
		default:
			DERR("404 on %.*s\n", length, data);
			parser->user_data_cb = s_uri_404;
			fw_www_init_search_ctx(s_server->web_pages, &rs->p.fs_ctx);
			return 0;
	}

	return 0;
}

static unsigned s_init_cb(hams_parser* parser, const uint8_t* data
	,unsigned length)
{
	parser->user_data_cb = s_uri_root;

	return 0;
}

/*****************************************************************************/

static void s_end_request(http_server_req_state *rs){
	_ASSERT(rs);
	uint8_t* x = (uint8_t*)rs;
	unsigned idx = (x - s_server->pool) / HTTP_POOL_BLOCK_SIZE;
	s_server->block_flags[idx] = 0;

	/* If this is the stager then discard it. */
	if(&rs->parser == s_server->stager){
		hamsif_extend_output(&rs->parser, 0);
	}

	DERR("ENDED REQUEST %p\n", &rs->parser);
}

http_server* http_server_init(hams_send_cb send_cb){
	memset(s_server->pool, 0, HTTP_POOL_SIZE);
	g_hams_demultiplexer = &(s_server->demux);
	hams_init();
	g_hams_demultiplexer->init_cb = s_init_cb;
	g_hams_demultiplexer->etag_cb = s_uri_etag;
	g_hams_demultiplexer->send_cb = send_cb;

	g_hams_demultiplexer->output_buffer = s_server->pool;

	/* Initialize flags block. */
	for(unsigned i = 0; i < HTTP_POOL_BLOCK_COUNT; ++i)
		s_server->block_flags[i] = 0;

	/* Indicate the initial blocks are added already. */
	s_server->block_flags[0] = HTTP_MIN_OUT;

	s_server->web_pages = NULL;

	return s_server;
}

unsigned http_server_set_pages(const fw_www_header* pages){
	if(!pages && !s_server->web_pages){
		/* Already cleared, so error. */
		return 1;
	}
	else if(!pages && s_server->web_pages){
		/* Clearing the web current web pages. */
		http_serverif_clear_memory();
		s_server->web_pages = NULL;
		return 0;
	}
	else if(s_server->web_pages){
		/* Can't set web pages if already set... */
		return 1;
	}
	else{
		/* Assign the web pages. */
		s_server->web_pages = pages;

		/* Find the default files. */
		fw_www_search_ctx ctx;
		for(unsigned i = 0; i < COUNT_WWW_STD_FILE; ++i){
			fw_www_init_search_ctx(s_server->web_pages, &ctx);
			unsigned len = strlen(s_default_files[i]);
			unsigned ret =
				fw_www_search(s_server->web_pages, s_default_files[i], len, &ctx);

			if(1 == ret){
				s_server->file_indices[i] = ctx.low;
			}
			else{
				/* Error, pages must be present, clear the pages. */
				http_server_set_pages(NULL);
				return 1;
			}
		}

		return 0;
	}
}

hams_parser* http_server_alloc_parser(void* id){
	/* Not found, look for empty space to allocate it. */
	unsigned idx = HTTP_POOL_BLOCK_COUNT - HTTP_MIN_REQ_COUNT;
	while(idx > s_server->block_flags[0]){
		if(!s_server->block_flags[idx]){
			s_server->block_flags[idx] = SERV_FLAG_PARSER | HTTP_MIN_REQ_COUNT;

			http_server_req_state *req = (http_server_req_state *)
				(s_server->pool + idx * HTTP_POOL_BLOCK_SIZE);
			req->stack_data = id;

			hams_parser_init(&req->parser);
			/* TODO use pointer arithmetic instead, save some bytes */
			req->parser.user_data = req;
			req->parser.buffer_size = HTTP_REQ_BUFFER_BYTES;
			req->flags = 0;

			g_hams_demultiplexer->init_cb(&req->parser, NULL, 0);
			++s_server->stats[SERV_STAT_NEW_CONNS];

			return &req->parser;
		}
		idx -= HTTP_MIN_REQ_COUNT;
	}

	/* Otherwise not found and no space for it. */
	++s_server->stats[SERV_STAT_DROPPED_CONNS];
	return NULL;
}

hams_parser* http_server_get_parser(void* id){
	unsigned long target = (unsigned long)(id);

	/* Search for corresponding id. */
	unsigned idx = HTTP_POOL_BLOCK_COUNT - HTTP_MIN_REQ_COUNT;
	while(idx > s_server->block_flags[0]){
		if(s_server->block_flags[idx] & SERV_FLAG_PARSER){
			/* Found allocated block, check id. */
			http_server_req_state *req = (http_server_req_state *)
				(s_server->pool + idx * HTTP_POOL_BLOCK_SIZE);
			if((unsigned long)req->stack_data == target)
				return &req->parser;
		}
		idx -= HTTP_MIN_REQ_COUNT;
	}
	return NULL;
}

unsigned http_server_output_sent(hams_parser* parser, unsigned successful){
	if(NULL == parser){
		unsigned idx = HTTP_POOL_BLOCK_COUNT - HTTP_MIN_REQ_COUNT;
		while(!s_server->stager && idx > s_server->block_flags[0]){
			_ASSERT(0 == g_hams_demultiplexer->staged_len);
			if(s_server->block_flags[idx] & SERV_FLAG_PARSER){
				/* Found allocated block, check id. */
				http_server_req_state *req = (http_server_req_state *)
					(s_server->pool + idx * HTTP_POOL_BLOCK_SIZE);

				if(req->parser.flags & HAMS_FLAG_STAGE_REQ){
					s_server->stager = &req->parser;
					hams_start_response(&req->parser);
				}
			}
			idx -= HTTP_MIN_REQ_COUNT;
		}
		return 0;
	}

	http_server_req_state *rs = http_server_get_state(parser);
	DERR("OUTPUT SENT %p\n", parser);

	/* If parser has any content remaining, give it a chance to output. */
	if(parser->content_length && !(parser->flags & HAMS_FLAG_CONTENT)){
		if(successful){
			DERR("ASKING USER CB %p\n", parser);
			parser->user_data_cb(parser, NULL, 0);
			return 1;
		}
		else{
			++s_server->stats[SERV_STAT_REM_HANGUPS];
			s_end_request(rs);
			return 0;
		}
	}
	else{
		/* TODO check keep alive. */

		++s_server->stats[SERV_STAT_NO_MORE_OUTPUT];
		s_end_request(rs);
		return 0;
	}
}

unsigned http_server_input(hams_parser* parser, const uint8_t* data,
	unsigned len)
{
	_ASSERT(parser);
	if(data)
		return hams_recv(parser, data, len);

	DERR("REMOTE CLOSED INPUT %p\n", parser);
	++s_server->stats[SERV_STAT_REM_CLOSED];
	/* Otherwise the remote closed the connection. */
	if(!(parser->flags & HAMS_FLAG_SENDING)){
		http_server_req_state *rs = http_server_get_state(parser);
		s_end_request(rs);
		return 0;
	}
	else{
		DERR("STILL SENDING\n");
		http_server_req_state *rs = http_server_get_state(parser);
		++s_server->stats[SERV_STAT_FINISH_SENDING];
		_ASSERT(parser->content_length
			| (rs->flags & REQ_FLAG_FIN)
			| (parser->flags & HAMS_FLAG_STAGE_REQ));
		/* Still sending data, keep open. */
		return 1;
	}
}

unsigned http_server_count_parsers(void){
	unsigned count = 0;
	unsigned idx = HTTP_POOL_BLOCK_COUNT - HTTP_MIN_REQ_COUNT;
	while(idx > s_server->block_flags[0]){
		if(s_server->block_flags[idx] & SERV_FLAG_PARSER){
			++count;
		}
		idx -= HTTP_MIN_REQ_COUNT;
	}

	return count;
}

/************************************************************/
/* hamsif_* functions. */
/************************************************************/

unsigned hamsif_extend_output(hams_parser* parser, unsigned extended_len){
	_ASSERT(parser);

	/* If asking for less than the minimum, just allocate
	 * what we have. */
	if(extended_len < HAMS_MIN_STAGE_EXT){
		/* Clear the stager pointer if this is zero. */
		if(0 == extended_len){
			_ASSERT(parser == s_server->stager);
			s_server->stager = NULL;
		}
		/* Indicate the initial blocks are added already. */
		s_server->block_flags[0] = HTTP_MIN_OUT;
	}
	else if(extended_len > s_server->block_flags[0] * HTTP_POOL_BLOCK_SIZE){
		/* Make sure user is asking for less than the maximum amount we can allocate.
		 * If greater than just return what we have. */
		if(extended_len <= SERV_MASK_ALLOC * HTTP_POOL_BLOCK_SIZE){
			/* Calculate blocks needed. */
			const unsigned needed =
				((extended_len + HTTP_POOL_BLOCK_SIZE - 1) / HTTP_POOL_BLOCK_SIZE);

			/* Find first allocated block. */
			unsigned i;
			for(i = HTTP_MIN_OUT; i <= needed; ++i){
				if(s_server->block_flags[i]){
					/* This block is not free, cannot allocate the memory. */
					/* TODO implement block rearranging to see if this can be moved
					 * to a block farther up the pool. */
					break;
				}
			}

			/* Have the needed blocks, allocate. */
			if(i > needed)
				s_server->block_flags[0] = needed;
		}
	}

	return s_server->block_flags[0] * HTTP_POOL_BLOCK_SIZE;
}
