/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <stdarg.h>
#include "server.h"
#include "diag.h"
#include "yxml.h"
#include "REST.h"
#include "Eth_if.h"
#include "httpd_basic_authentication.h"
#include "BIN_ASCII_Conversion.h"
#include "Server_IO.h"
#include "User_Session.h"
#include "StdLib_MV.h"
#include "REST_FUS_Interface.h"
#include "Rest_WebUI_Interface.h"
#include "REST_Common.h"
#include "Web_Debug.h"
#include "Babelfish_Assert.h"
#include "OS_Binary_Semaphore.h"

#define HAMS_DEBUG
#ifdef HAMS_DEBUG
// #define DERR(...) fprintf(stderr, __VA_ARGS__)
#define DERR( ... )
#define _ASSERT( x ) do { if ( !( x ) )while ( 1 ){} \
} while ( 0 )
#else
#define DERR( ... )
#define _ASSERT( x )
#endif

/*
   Below maintenance_pages.c file is auto generated which includes an array of type "unsigned char
      s_maintenance_pages[]". This s_maintenance_pages[] array doesn't need to be in RAM. With "static const" prefix
      array is moved from RAM to ROM.
 */
static const
#include "maintenance_pages.c"
static const
#include "default_page.c"

#define DYNIN_PARSER_LENGTH ( 2 * HTTP_POOL_BLOCK_SIZE - sizeof( http_server_req_state ) + \
							  sizeof( struct hams_extended ) )
#define DYNIN_PARSER_DATA( parser ) ( ( uint8_t* )&( parser )->ext )
#define PROFILE_DYNWASTED
#define Diag_Dynwasted     "GET, HEAD, POST, OPTIONS"
enum
{
	ROOT_ROOT,
	ROOT_DIAG,
	ROOT_INTERFACE,
	ROOT_MODBUS,
	ROOT_REST,
	COUNT_ROOTS
};

enum
{
	DIAG_DYN_WASTED,
	DIAG_LWIP,
	COUNT_DIAG
};

struct real_http_server
{
	http_server s;
	uint8_t pool[4];
};


typedef struct dyn_metadata_t
{
	/* Amount of data in the buffer. */
	uint16_t data_length;

	/* With which request this buffer is associated. */
	uint8_t request_id;

	/* Next buffer containing data; if 0 then this is last one.. */
	uint8_t next;
} dyn_metadata;

#define DYNAMIC_BUFFER_SIZE ( HTTP_POOL_BLOCK_SIZE - sizeof( dyn_metadata ) )

typedef struct dynamic_buffer_s
{
	dyn_metadata dyn_mdata;
	/* Actual data. */
	uint8_t data[DYNAMIC_BUFFER_SIZE];
} dynamic_buffer;

extern uint32_t Http_Total_Pool_Block_Count( void );

#ifdef JSON_ACTIVE
extern bool pre_pbin_in_progress_js;
#else
extern bool pre_pbin_in_progress;
#endif
static PROD_SPEC_REST_CBACK prod_spec_rest_cb = nullptr;

extern uint32_t s_modbus_rx_request( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t rest_root_handler( hams_parser* parser, const uint8_t* data, uint32_t length );

#ifdef JSON_ACTIVE
extern uint32_t Rest_Other_Handler_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t S_Lwip_Diag_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

#else
extern uint32_t rest_other_handler( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t s_lwip_diag_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

#endif

extern uint32_t fw_node_information( hams_parser* parser, const uint8_t* data,
									 uint32_t length );

extern uint32_t s_send_cb( hams_parser* parser, const uint8_t* data, uint32_t length,
						   uint32_t copy );

static void S_Update_Host_From_Ip( uint32_t ip );

#define HTTP_REQ_BUFFER_BYTES 94

/*
   Services like rs/param and assembly requires more number of data block while sending response. We need to limit the
      number of blocks used by large data service so that remaining blocks can be used by other services if any of new
      serice is received concurrently. So out of total 48 (HTTP_POOL_BLOCK_COUNT) blocks only 10 are used in each xmit
      function.
 */
static const uint8_t MAX_BLOCKS_FOR_LARGE_DATA_SERVICE = 6U;

#define HTTP_MIN_REQ_COUNT \
	( ( sizeof( http_server_req_state ) + HTTP_REQ_BUFFER_BYTES + HTTP_POOL_BLOCK_SIZE - 1 ) / HTTP_POOL_BLOCK_SIZE )

// static_assert((sizeof(http_server_req_state) + HTTP_REQ_BUFFER_BYTES) < 256,
// "HTTP_REQ_BUFFER_BYTES too big!");

// static_assert(HTTP_MIN_OUT < HTTP_POOL_BLOCK_COUNT,
// "HTTP_POOL cannot fit min output buffer requirements.");

// static_assert(HTTP_MIN_OUT < SERV_MASK_ALLOC,
// "SERV_MASK_ALLOC is not big enough to store minimum output buffer.");

// static_assert(HTTP_MIN_OUT < SERV_MASK_ALLOC,
// "SERV_MASK_ALLOC is not big enough to store request state.");

// static_assert(HTTP_MIN_REQ_COUNT < SERV_MASK_ALLOC,
// "SERV_MASK_ALLOC is not big enough to store request state.");

#ifdef PROFILE_DYNWASTED
#define DYNWASTED_BUFFER_SIZE ( DYNAMIC_BUFFER_SIZE + 1 )
static uint32_t s_wasted_bytes[DYNWASTED_BUFFER_SIZE];
#endif

// As per LTK-2833: Below change is IAR specific and thus not required for ESP32
#ifndef ESP32_SETUP
#pragma location = ".bss"		/* Strictly place the variable in .bss segment to reduce the memory usage (LTK-2883) */
#endif
static uint8_t s_server_memory[sizeof( http_server ) + HTTP_POOL_SIZE];
static http_server* s_server = reinterpret_cast<http_server*>( &s_server_memory[0] );

struct hams_demultiplexer_s* g_hams_demultiplexer;

static const char_t* const s_root_paths[COUNT_ROOTS] =
{
	"/",
	"/diag",
	"/if",
	"/modbus",
	"/rs"
};

static const char_t* const s_default_files[COUNT_WWW_STD_FILE] =
{
	"/not_found.html",
	"/default.html",
	"/index.html"
};

static const char_t* const s_uri_redirects[COUNT_URI_REDIRECTS] =
{
	"/if/index.html"
};

static const char_t* const s_diag_paths[COUNT_DIAG] =
{
	"/dynwasted",
	"/lwip"
};

extern uint32_t s_rest_param_all_remaining( hams_parser* parser, const uint8_t* data,
											uint32_t length );

static uint32_t S_Uri_Redirect( hams_parser* parser, const uint8_t* data,
								uint32_t length );

static uint32_t S_Uri_Part( hams_parser* parser, const uint8_t* data,
							uint32_t length );

static void Free_Dynamic_Memory( hams_parser* parser );

#ifdef WEBUI_ON_EXT_FLASH
// Protect HTTP and HTTPS from simultaneous read from NV and transmit the data
extern OS_Binary_Semaphore* webui_semaphore;
#endif  // WEBUI_ON_EXT_FLASH

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t S_Xmit_Dynamic_Data( hams_parser* parser, const uint8_t* data,
							  uint32_t length )
{
	// _ASSERT( hams_can_respond(data, length) );
	uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	uint32_t len = Serverif_Send_Size( rs );
	bool function_end = false;
	bool loop_break = false;

	if ( HTTP_METHOD_HEAD == parser->method )
	{
		rs->flags |= REQ_FLAG_FIN;
	}
	else
	{
		if ( parser->content_length < len )
		{
			len = parser->content_length;
		}
		while ( ( len ) && ( false == function_end ) && ( false == loop_break ) )
		{
			if ( 0 != rs->p.dyn.byte_count )
			{
				/*  */
				uint32_t remainder = rs->p.dyn.byte_count;
				// if ( remainder > len )
				// {
				// commenting this as part of LTK - 2822:
				// remainder = len;
				// }

				const uint8_t* src = DYNIN_PARSER_DATA( parser );
				uint32_t sc =
					g_hams_demultiplexer->send_cb( parser, src + rs->p.dyn.offset, remainder, 1 );
				if ( 0 != sc )
				{
					if ( 1 == sc )
					{
						rs->flags |= REQ_FLAG_RETRY;
					}

					function_end = true;
				}
				if ( false == function_end )
				{
					len -= remainder;

					rs->p.dyn.byte_count -= remainder;
					if ( 0 == rs->p.dyn.byte_count )
					{
						rs->p.dyn.offset = 0;
					}
					else
					{
						rs->p.dyn.offset += remainder;
					}
				}
			}
			else
			{

				const dynamic_buffer* db =
					( dynamic_buffer* ) ( pool + rs->p.dyn.block_id * HTTP_POOL_BLOCK_SIZE );

				/* Send */
				uint32_t remainder = db->dyn_mdata.data_length - rs->p.dyn.offset;
				// if ( remainder > len )
				// {
				// commenting this as part of LTK - 2822:
				// remainder = len;
				// }

				uint32_t sc =
					g_hams_demultiplexer->send_cb( parser, db->data + rs->p.dyn.offset, remainder,
												   1 );
				if ( 0 != sc )
				{
					if ( 1 == sc )
					{
						rs->flags |= REQ_FLAG_RETRY;
					}
					else if ( sc == 2 )
					{
						parser->content_length = 0U;
					}
					function_end = true;
				}
				else
				{

					rs->p.dyn.offset += remainder;
					len -= remainder;

					/* If transmitted entire block then free it and go onto the next. */
					if ( rs->p.dyn.offset == db->dyn_mdata.data_length )
					{
						parser->content_length -= db->dyn_mdata.data_length;
						s_server->block_flags[rs->p.dyn.block_id] = 0;
						rs->p.dyn.block_id = db->dyn_mdata.next;
						rs->p.dyn.offset = 0;
					}
					if ( db->dyn_mdata.next == 0 )
					{
						loop_break = true;
					}
				}
			}
		}
		if ( false == function_end )
		{
			if ( 0 != parser->transmission_continue )
			{
				parser->transmission_continue = false;
				parser->user_data_cb_remaining( parser, data, length );
			}
			else
			{
				rs->flags |= REQ_FLAG_FIN;
			}
		}
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t S_Xmit_Flash_Data( hams_parser* parser, const uint8_t* data,
							uint32_t length )
{
	// _ASSERT( hams_can_respond(data, length) );

	DERR( "URI XMIT %p\n", parser );
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( HTTP_METHOD_HEAD == parser->method )
	{
		rs->flags |= REQ_FLAG_FIN;
	}
	else
	{
		uint32_t len = Serverif_Send_Size( rs );
		if ( parser->content_length < len )
		{
			len = parser->content_length;
		}

		uint32_t sc =
			g_hams_demultiplexer->send_cb( parser, rs->p.static_response.file_ptr, len, 0 );
		if ( 0 != sc )
		{
			if ( 1 == sc )
			{
				rs->flags |= REQ_FLAG_RETRY;
			}
			else if ( sc == 2 )
			{
				parser->content_length = 0U;
			}
		}
		else
		{
			parser->content_length -= len;
			rs->p.static_response.file_ptr += len;

			if ( 0 == parser->content_length )
			{
				rs->flags |= REQ_FLAG_FIN;
			}
		}
	}
	return ( 0 );
}

/*****************************************************************************/

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t S_Xmit_Webui_Data( hams_parser* parser, const uint8_t* data,
							uint32_t length )
{
	uint8_t copy_flag = WRITE_FLAG_STATIC_COPY;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	uint32_t sc = 0;

	DERR( "URI XMIT %p\n", parser );

	if ( HTTP_METHOD_HEAD == parser->method )
	{
		rs->flags |= REQ_FLAG_FIN;
	}
	else
	{
		uint32_t len = Serverif_Send_Size( rs );
		if ( parser->content_length < len )
		{
			len = parser->content_length;
		}

#ifdef WEBUI_ON_EXT_FLASH
		/* Taking the backup of the external flash address */
		uint32_t address = reinterpret_cast<uint32_t>( rs->p.static_response.file_ptr );
		uint16_t server_flag = Get_Server_Flags();
		/* To synchronize between HTTP and HTTPS response semaphore is used to avoid the simulateous access of NV data
		   and transmittion to web browser */
		if ( webui_semaphore->Pend( OS_BINARY_SEMAPHORE_INDEFINITE_TIMEOUT ) )	// acquire semaphore
		{
			if ( server_flag == HTTP_SERVER_FLAG_WEB_PAGES )
			{
				if ( parser->https == HTTP_MEDIA )
				{
					if ( WEB_PAGE_READ_BUFF_SIZE < len )
					{
						len = WEB_PAGE_READ_BUFF_SIZE;
					}
					// read data chunk from file
					rs->p.static_response.file_ptr = Fw_WWW_Get_File_Data_Chunk( parser, address, len );

					/*
					   LTK-8544 :-
					   HTTP page failed to load above data chunk buffer size 3833 bytes, if copy flag is 0 (
					      WRITE_FLAG_STATIC_COPY ). Making copy flag 1 ( WRITE_FLAG_DYNAMIC_COPY ), allow HTTP page to
					         load even if buffer size is >= 3834 bytes. But this approach increases the RAM memory
					         consumption as there is another buffer of similar size gets allocated in the LWIP library (
					         tcp_write() ) and web image data gets copied into that buffer before sending it to web
					            browser.

					   To avoid more consumtion of RAM memory we are setting this flag to 1 only when
					      WEB_PAGE_READ_BUFF_SIZE is >= 3834.
					 */
					if ( WEB_PAGE_READ_BUFF_SIZE >= 3834 )
					{
						copy_flag = WRITE_FLAG_DYNAMIC_COPY;
					}
				}
				else	// if ( parser->https == HTTPS_MEDIA )
				{
					copy_flag = WRITE_FLAG_NV_CTRL_COPY;
				}
			}
			sc =
				g_hams_demultiplexer->send_cb( parser, rs->p.static_response.file_ptr, len, copy_flag );
			webui_semaphore->Post();	// release semaphore
		}
		/* Storing back the backup address to the static response file pointer */
		rs->p.static_response.file_ptr = reinterpret_cast<uint8_t*>( address );
#else
		sc =
			g_hams_demultiplexer->send_cb( parser, rs->p.static_response.file_ptr, len, copy_flag );
#endif
		WEBSERVER_DEBUG_PRINT( DBG_MSG_INFO, "Send Chunk Length =%ld", len );

		if ( 0 != sc )
		{
			if ( 1 == sc )
			{
				rs->flags |= REQ_FLAG_RETRY;
			}
			else if ( sc == 2 )
			{
				parser->content_length = 0U;
			}
		}
		else
		{
			parser->content_length -= len;
			rs->p.static_response.file_ptr += len;

			if ( 0 == parser->content_length )
			{
				rs->flags |= REQ_FLAG_FIN;
			}
		}
	}
	return ( 0 );
}

/* ***************************************************************************
   /if Web page handling.
 ****************************************************************************/

static uint32_t S_Uri_Response_Begin( hams_parser* parser, const uint8_t* data,
									  uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );
	const uint8_t mime[MIMETYPE_BUFFER_SIZE] = {0};
	const uint8_t www_file_buff[sizeof( fw_www_file )] = {0};
	const fw_www_file* www_file = ( reinterpret_cast<const fw_www_file*>( &www_file_buff ) );

	/* Some expected conditions. */
	// _ASSERT( !parser->content_length );
	// _ASSERT( hams_can_respond(data, length) );
	/* If no web pages then request must be using maintenance pages. */
	// _ASSERT( s_server->web_pages || (rs->flags & REQ_FLAG_MAINTENANCE_PAGES) );
	const fw_www_header* pages =
		( rs->flags & REQ_FLAG_MAINTENANCE_PAGES )
		? s_server->maintenance_pages : s_server->web_pages;

	DERR( "URI RESP BEGIN %p %u\n", parser, parser->response_code );

	/* Send back 404 file if 404; otherwise actual file. */
	uint32_t idx = ( parser->response_code != 288 ) ?
		rs->p.fs_ctx.low : s_server->file_indices[WWW_STD_FILE_DEFAULT];

	uint32_t filesize = Fw_WWW_Get_Filesize( pages, idx );

	Fw_WWW_Get_File( pages, idx, ( reinterpret_cast<const uint8_t*>( www_file ) ) );

	/* Send back etag if not in error. */
	if ( ( 200 == parser->response_code ) || ( 304 == parser->response_code ) )
	{
		if ( www_file != nullptr )
		{
			hams_response_header( parser, HTTP_HDR_ETAG, www_file->etag, 32 );
		}
	}

	/* 304 and HEAD requests are headers only. */
	uint32_t send_body = ( ( parser->response_code != 304 ) &&
						   ( HTTP_METHOD_GET == parser->method ) );

	if ( 0 != send_body )
	{
		ULBASE l = filesize;
		if ( ( www_file != nullptr ) && ( www_file->mime_type <= pages->mime_type_count ) )
		{
			Fw_WWW_Get_Mimetype( pages, www_file->mime_type, ( reinterpret_cast<const uint8_t*>( &mime ) ) );
			uint32_t mime_len = Fw_WWW_Get_Mimetype_Length( pages, www_file->mime_type );

			hams_response_header( parser, HTTP_HDR_CONTENT_LENGTH, ( void* )&l, 0 );
			hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, mime, mime_len );

			// _ASSERT( parser->content_length == l );

			/* If gzipped encoded, add header */
			if ( 0 != www_file->encoding )
			{
				hams_response_header( parser, HTTP_HDR_CONTENT_ENCODING, "gzip", 4 );
			}

			/* Prepare to send content body after header is finished sending. */
			WEBSERVER_DEBUG_PRINT( DBG_MSG_INFO, "Prepare To Send File Data Chunk By Chunk" );
			rs->p.static_response.file_ptr = Fw_WWW_Get_File_Data( parser, pages, idx );
			parser->user_data_cb = S_Xmit_Webui_Data;

		}
		else
		{
			DERR( "NOT SENDING BODY\n" );
			/* Nothing more to send. Not strictly necessary to NULL, but good for stomping bugs.*/
			parser->user_data_cb = NULL;
		}
	}
	else
	{
		DERR( "NOT SENDING BODY\n" );
		/* Nothing more to send. Not strictly necessary to NULL, but good for stomping bugs.*/
		parser->user_data_cb = NULL;
	}

	/* No more headers to send back. */
	hams_response_header( parser, COUNT_HTTP_HDRS, NULL, 0 );

	return ( 0 );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Uri_Hdrs_End( hams_parser* parser, const uint8_t* data,
								uint32_t length )
{
	/* Only do something when headers end (and thus URL) ended. */
	if ( 0 != hams_check_headers_ended( data, length ) )
	{
		DERR( "URI ENDED %p\n", parser );
		http_server_req_state* rs = Http_Server_Get_State( parser );

		/* There should not be any request body.. */
		if ( 0 != parser->content_length )
		{
			hams_respond_error( parser, 413 );
		}
		else
		{
			parser->user_data_cb = S_Uri_Response_Begin;

			uint32_t ret = Fw_WWW_Match_Count( &rs->p.fs_ctx );
			if ( ret != 1 )
			{
				DERR( "FILE NOT FOUND\n" );
				parser->response_code = 288;
			}
			else if ( 0 != ( rs->flags & REQ_FLAG_ETAG_MATCH ) )
			{
				parser->response_code = 304;
			}
			else
			{
				parser->response_code = 200;
			}
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void S_Uri_Etag( hams_parser* parser, const uint8_t* data,
						uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );
	const uint8_t www_file_buff[sizeof( fw_www_file )] = {0};
	const fw_www_file* www_file = ( reinterpret_cast<const fw_www_file*>( &www_file_buff ) );

	/* ETAG for files must be 32 characters */
	if ( 32 == length )
	{

		/* Must be a matching file. */
		if ( 1 == Fw_WWW_Match_Count( &rs->p.fs_ctx ) )
		{

			/* If no web pages then request must be using maintenance pages. */
			// _ASSERT( s_server->web_pages || (rs->flags & REQ_FLAG_MAINTENANCE_PAGES) );
			const fw_www_header* pages =
				( rs->flags & REQ_FLAG_MAINTENANCE_PAGES )
				? s_server->maintenance_pages : s_server->web_pages;

			Fw_WWW_Get_File( pages, rs->p.fs_ctx.low, ( reinterpret_cast<const uint8_t*>( www_file ) ) );
			if ( www_file != nullptr )
			{
				if ( !memcmp( www_file->etag, data, 32 ) )
				{

					/* Etags match, we will be doing a 304 instead of a 200. */
					rs->flags |= REQ_FLAG_ETAG_MATCH;
				}
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Uri_Part( hams_parser* parser, const uint8_t* data,
							uint32_t length )
{

	bool exit_flag = false;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/* If no web pages then request must be using maintenance pages. */
	// _ASSERT( s_server->web_pages || (rs->flags & REQ_FLAG_MAINTENANCE_PAGES) );
	const fw_www_header* pages =
		( rs->flags & REQ_FLAG_MAINTENANCE_PAGES )
		? s_server->maintenance_pages : s_server->web_pages;

	/* If FS segment then match. */
	if ( data[0] == '/' )
	{
		if ( 0 == Fw_WWW_Search( pages, ( const uint8_t* )data, length, &rs->p.fs_ctx ) )
		{
			/* No match, just bleed out the rest of the URL. */
			WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "No Match, Just Bleed Out Rest Of The URL" );
			parser->user_data_cb = S_Uri_Hdrs_End;
			exit_flag = true;
		}
	}
	if ( false == exit_flag )
	{
		/* Check if this is the final segment. */
		if ( data[length] == ' ' )
		{
			parser->user_data_cb = S_Uri_Hdrs_End;
		}

		/* Wait for next piece. */
	}
	return ( 0 );
}

/*****************************************************************************/

#ifdef PROFILE_DYNWASTED
static uint32_t S_Dynwasted_End_Cb( hams_parser* parser, const uint8_t* data,
									uint32_t length )
{
	// _ASSERT( hams_can_respond(data, length) );
	if ( HTTP_METHOD_OPTIONS == parser->method )
	{
		hams_response_header( parser, HTTP_HDR_ALLOW, Diag_Dynwasted,
							  STRLEN( Diag_Dynwasted ) );
		hams_response_header( parser, COUNT_HTTP_HDRS,
							  ( const void* )nullptr, 0U );
	}
	else if ( 0 != Http_Server_End_Dynamic( parser ) )
	{
		hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/json", 16 );
		hams_response_header( parser, COUNT_HTTP_HDRS, NULL, 0 );
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Dynwasted_Diag_Cb( hams_parser* parser, const uint8_t* data,
									 uint32_t length )
{
	if ( 0 != hams_check_headers_ended( data, length ) )
	{

		if ( ( HTTP_METHOD_GET == parser->method ) || ( HTTP_METHOD_HEAD == parser->method ) )
		{
			/* Print out dynamic usage in JSON. */
			parser->response_code = 200;
			http_server_req_state* rs = Http_Server_Get_State( parser );
			rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
			Http_Server_Begin_Dynamic( parser );
			Http_Server_Print_Dynamic( parser, "[\n\bu\n", s_wasted_bytes[0] );
			for ( uint32_t i = 1; i < DYNWASTED_BUFFER_SIZE; ++i )
			{
				Http_Server_Print_Dynamic( parser, ",\bu\n", s_wasted_bytes[i] );
			}
			Http_Server_Print_Dynamic( parser, "]" );

			parser->user_data_cb = S_Dynwasted_End_Cb;
		}
		else if ( HTTP_METHOD_POST == parser->method )
		{
			/* Reset to zero. */
			memset( s_wasted_bytes, 0, sizeof ( s_wasted_bytes ) );
			parser->response_code = 200;
			parser->user_data_cb = NULL;
		}
		else if ( HTTP_METHOD_OPTIONS == parser->method )
		{

			if ( ( 0U == parser->cors_auth_common->origin_length ) ||
				 ( NULL == parser->cors_auth_common->origin_buffer ) )
			{
				hams_respond_error( parser, 400U );
			}
			else
			{
				parser->response_code = 200U;
				http_server_req_state* rs = Http_Server_Get_State( parser );
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				parser->user_data_cb = S_Dynwasted_End_Cb;
			}
		}
		else
		{
			hams_respond_error( parser, 405 );
		}
	}
	return ( 0 );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Diag_Cb( hams_parser* parser, const uint8_t* data,
						   uint32_t length )
{
	bool choice_found = false;
	uint32_t choice = COUNT_DIAG;

	for ( uint32_t i = 0; ( ( i < COUNT_DIAG ) && ( false == choice_found ) ); ++i )
	{
		uint32_t len = strlen( ( const char_t* )s_diag_paths[i] );
		if ( ( length == len ) && ( !strncmp( ( const char_t* )data, s_diag_paths[i], length ) ) )
		{
			choice = i;
			choice_found = true;
		}
	}

	switch ( choice )
	{
		case DIAG_LWIP:
			/* Only GET and HEAD methods are acceptable. */
			if ( ( parser->method != HTTP_METHOD_GET ) && ( parser->method != HTTP_METHOD_HEAD ) &&
				 ( parser->method != HTTP_METHOD_OPTIONS ) )
			{
				hams_respond_error( parser, 405 );
			}
			else
			{
#ifdef JSON_ACTIVE
				parser->user_data_cb = S_Lwip_Diag_Cb_Json;
#else
				parser->user_data_cb = s_lwip_diag_cb;
#endif
			}
			break;

#ifdef PROFILE_DYNWASTED
		case DIAG_DYN_WASTED:
			parser->user_data_cb = S_Dynwasted_Diag_Cb;
			break;
#endif

		default:
			hams_respond_error( parser, 400U );
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_On_Redirect_Headers( hams_parser* parser, const uint8_t* data,
									   uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( parser->redir_custom_uri == 1U )
	{
		hams_response_header( parser, HTTP_HDR_LOCATION,
							  rs->p.redir.filename, rs->p.redir.file_name_len );
		Http_Server_Free( rs->p.redir.filename );
		parser->redir_custom_uri = 0U;
	}
	else
	{
		hams_response_header( parser, HTTP_HDR_LOCATION,
							  s_uri_redirects[0], strlen( s_uri_redirects[0] ) );
	}

	hams_response_header( parser, COUNT_HTTP_HDRS, NULL, 0U );
	parser->user_data_cb = NULL;
	parser->flags |= HAMS_FLAG_REDIRECT;
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Uri_Redirect( hams_parser* parser, const uint8_t* data,
								uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/* Just wait until we get our header space. */
	if ( 0 != hams_check_headers_ended( data, length ) )
	{
		parser->user_data_cb = S_On_Redirect_Headers;
		/* There should not be any request body.. */
		if ( 0 != parser->content_length )
		{
			hams_respond_error( parser, 413U );
		}
		else
		{
			parser->response_code = 301U;
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Static_Response( hams_parser* parser, const uint8_t* data,
								   uint32_t length )
{
	if ( 0 != hams_can_respond( data, length ) )
	{
		hams_response_header( parser, COUNT_HTTP_HDRS, NULL, 0 );
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t S_Options_Handler( hams_parser* parser, const uint8_t* data,
							uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( ( 0U == parser->cors_auth_common->origin_length ) ||
		 ( NULL == parser->cors_auth_common->origin_buffer ) )
	{
		hams_respond_error( parser, 400U );
	}
	else
	{
		parser->response_code = 200U;
		parser->user_data_cb = S_Static_Response;
		/* s_static_response is required to send the response header */
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Redirect_To_Https_Server( hams_parser* parser, const uint8_t* data,
								   uint32_t length )
{
	uint32_t return_val = 0U;

	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( parser->redir_custom_uri == 0U )
	{
		// Allocate 1 block for redirect url; 1 block should be suffcient enough
		rs->p.redir.filename = Http_Server_Alloc_Scratch( parser, 1 );
		if ( rs->p.redir.filename != nullptr )
		{
			parser->redir_custom_uri = 1U;
			rs->p.redir.file_name_len = 0U;
			parser->user_data_cb = Redirect_To_Https_Server;
		}
		else
		{
			hams_respond_error( parser, E503_SCRATCH_SPACE );
			return_val = 1U;
		}
	}
	/* Just wait until we get our header space. */
	if ( 0 != hams_check_headers_ended( data, length ) )
	{
		parser->user_data_cb = S_On_Redirect_Headers;
		parser->response_code = 307U;
	}
	else if ( rs->p.redir.filename != nullptr )
	{
		if ( BF_Lib::Copy_Data( ( ( uint8_t* )rs->p.redir.filename + rs->p.redir.file_name_len ),
								( HTTP_POOL_BLOCK_SIZE - rs->p.redir.file_name_len ), data, length ) == true )
		{
			rs->p.redir.file_name_len += length;
		}
		else
		{
			// one block of 128 bytes allocated for saving URI (to be sent in location header)
			/* URI component is too long. */
			hams_respond_error( parser,  414U );
			return_val = 1U;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Uri_Root( hams_parser* parser, const uint8_t* data,
							uint32_t length )
{
	bool choice_found = false;
	uint32_t choice = 0U;

	if ( !IS_EXTRA_SLASH( data ) )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );

		choice = COUNT_ROOTS;
		for ( uint32_t i = 0; ( ( i < COUNT_ROOTS ) && ( false == choice_found ) ); ++i )
		{
			uint32_t len = strlen( ( const char_t* )s_root_paths[i] );
			if ( ( length == len ) && !strncmp( ( const char_t* )data, s_root_paths[i], length ) )
			{
				choice = i;
				choice_found = true;
			}
		}

		switch ( choice )
		{
			case ROOT_ROOT:
				/* Otherwise redirect to index page. */
				parser->user_data_cb = S_Uri_Redirect;
				break;

			case ROOT_INTERFACE:
				/* Only GET and HEAD methods are acceptable. */
				if ( ( parser->method != HTTP_METHOD_GET ) &&
					 ( parser->method != HTTP_METHOD_HEAD ) )
				{
					hams_respond_error( parser, 405 );
					/* TODO populate hams_parser bitfield with Allow mask. */
				}
				else
				{

					/* /if should redirect to /if/index.html. */
					if ( ' ' == data[length] )
					{
						parser->user_data_cb = S_Uri_Redirect;
					}
					else
					{
						if ( s_server->flags != HTTP_SERVER_FLAG_NOT_READY )
						{
							const fw_www_header* pages = nullptr;
							if ( s_server->flags & HTTP_SERVER_FLAG_MAINTENANCE_PAGES )
							{
								rs->flags |= REQ_FLAG_MAINTENANCE_PAGES;
								pages = s_server->maintenance_pages;
							}
							else if ( s_server->flags & HTTP_SERVER_FLAG_WEB_PAGES )
							{
								pages = s_server->web_pages;
							}
							else
							{
								// Misra
							}
							Fw_WWW_Init_Search_Ctx( pages, &rs->p.fs_ctx );
							parser->user_data_cb = S_Uri_Part;
						}
						else
						{
							/* Service Unavailable */
							hams_respond_error( parser, 503U );
						}
					}
				}
				break;

			case ROOT_DIAG:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					/* /diag is not valid. */
					if ( '/' != data[length] )
					{
						hams_respond_error( parser, 404 );
					}
					else
					{
						/* Note diagnostics are dynamic data; content-length CANNOT be guaranteed!
						 */
						parser->user_data_cb = S_Diag_Cb;
					}
				}
				break;

			case ROOT_REST:
				if ( ' ' == data[length] )
				{
					// Just the root.
					if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
						 ( HTTP_METHOD_OPTIONS != parser->method
						 ) )
					{
						hams_respond_error( parser, 405 );
					}
					else
					{
						parser->user_data_cb = rest_root_handler;
					}
				}
				else if ( '/' == data[length] )
				{
					bool rs_choice_found = false;
					uint8_t const* data_ptr = data + length;
					/* check whether the URI is supported by Webserver */
					for ( uint32_t i = 1U; ( ( i < COUNT_REST ) && ( rs_choice_found == false ) ); ++i )
					{
						uint32_t len = BF_Lib::Get_String_Length( const_cast<uint8_t*>( s_rest_paths[i] ),
																  REST_PATH_MAX_LEN );
						if ( BF_Lib::Compare_Data( data_ptr, s_rest_paths[i], len ) == true )
						{
							rs_choice_found = true;
						}
					}
					if ( rs_choice_found == true )
					{
#ifdef JSON_ACTIVE
						parser->user_data_cb = Rest_Other_Handler_Json;
#else
						parser->user_data_cb = rest_other_handler;
#endif
					}
					else
					{
						/* URI is not supported by the weserver, call the product spec callback */
						parser->user_data_cb = prod_spec_rest_cb;
					}
				}
				else
				{
					hams_respond_error( parser, 400U );
				}
				break;

			case ROOT_MODBUS:
				if ( true == Firmware_Upgrade_Inprogress() )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					if ( '?' == data[length] )
					{
						hams_respond_error( parser, 400 );
					}
					else if ( ( parser->method != HTTP_METHOD_POST ) && ( parser->method != HTTP_METHOD_OPTIONS ) )
					{
						hams_respond_error( parser, 405 );
					}
					else
					{
						parser->user_data_cb = s_modbus_rx_request;
					}
				}
				break;

			default:
				hams_respond_error( parser, 400U );
				break;
		}
	}
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Init_Cb( hams_parser* parser, const uint8_t* data,
						   uint32_t length )
{
	parser->user_data_cb = S_Uri_Root;

	return ( 0 );
}

/*****************************************************************************/

static void S_End_Request( http_server_req_state* rs )
{
	uint32_t pool_block_count = Http_Total_Pool_Block_Count();
	uint16_t error_code = rs->parser.response_code;
	uint8_t state = rs->p.fwop.state;

	++s_server->stats[SERV_STAT_END_REQUESTS];

	/* If cb is defined and the cb wants to handle an error, let it know the client closed. */
	if ( rs->parser.user_data_cb )
	{
		if ( 0 != ( rs->parser.flags & HAMS_FLAG_HANDLE_ERROR ) )
		{
			rs->parser.user_data_cb( &rs->parser, NULL, 1 );
		}
	}
	Free_Dynamic_Memory( &rs->parser );
	// _ASSERT( rs );
	uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
	uint8_t* x = ( uint8_t* )rs;
	uint32_t idx = ( x - pool ) / HTTP_POOL_BLOCK_SIZE;

	for ( uint32_t i = 0; i < HTTP_MIN_REQ_COUNT; ++i )
	{
		s_server->block_flags[idx + i] = 0;
	}

	/* Clear out any dynamic memory associated with this block. */
	for ( uint32_t i = s_server->block_flags[0]; i < pool_block_count; ++i )
	{
		if ( ( s_server->block_flags[i] & SERV_FLAG_DYNAMIC_BUFFER ) &&
			 ( s_server->block_flags[i] & SERV_MASK_ALLOC ) )
		{
			dynamic_buffer* db =
				( dynamic_buffer* ) ( pool + i * HTTP_POOL_BLOCK_SIZE );
			if ( db->dyn_mdata.request_id == idx )
			{
				uint32_t count = s_server->block_flags[i] & SERV_MASK_ALLOC;
				for ( uint32_t k = 0; k < count; ++k )
				{
					s_server->block_flags[i + k] = 0;
				}
			}
		}
	}

	/* If this is the stager then discard it. */
	if ( &rs->parser == s_server->stager )
	{
		Hamsif_Extend_Output( &rs->parser, 0 );
	}

	bool reset_blocks = true;

	if ( error_code == E503_SCRATCH_SPACE )	// scratch_alloc_error
	{
		// don't try to optimize it by using variable "pool_block_count"
		for ( uint8_t idx = 1; idx < HTTP_POOL_BLOCK_COUNT; idx++ )
		{
			if ( s_server->block_flags[idx] == ( SERV_FLAG_PARSER | HTTP_MIN_REQ_COUNT ) )
			{
				reset_blocks = false;
				idx = HTTP_POOL_BLOCK_COUNT;
			}
		}
		if ( reset_blocks == true )
		{
			for ( uint8_t idx = 1; idx < HTTP_POOL_BLOCK_COUNT; idx++ )
			{
				s_server->block_flags[idx] = 0x0U;
			}
		}
	}
	DERR( "ENDED REQUEST %p\n", &rs->parser );
	if ( Firmware_Upgrade_Inprogress() == true )
	{
		if ( state == 0x55 )
		{
#ifdef JSON_ACTIVE
			pre_pbin_in_progress_js = false;
#else
			pre_pbin_in_progress = false;
#endif
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Web_Server_Init( REST_TARGET_INFO_ST_TD const* prod_rest_def, User_Session* user_session, NV_Ctrl* web_nv_ctrl )
{
#ifdef DEBUG
	/* Size of every HTTP request will use more than 2 blocks which is not desired */
	BF_ASSERT( HTTP_MIN_REQ_COUNT <= 2 );
#endif

	uint32_t pool_block_count = Http_Total_Pool_Block_Count();

	uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
	memset( pool, 0, HTTP_POOL_SIZE );
	g_hams_demultiplexer = &( s_server->demux );
	hams_init();
	g_hams_demultiplexer->init_cb = S_Init_Cb;
	g_hams_demultiplexer->etag_cb = S_Uri_Etag;
	g_hams_demultiplexer->send_cb = SERVER_IO_Send_cb;

	g_hams_demultiplexer->output_buffer = pool;

	s_server->maintenance_pages = ( const fw_www_header* )s_default_page;

	/* Initialize flags block. */
	for ( uint32_t i = 0; i < pool_block_count; ++i )
	{
		s_server->block_flags[i] = 0;
	}

	/* Indicate the initial blocks are added already. */
	s_server->block_flags[0] = HTTP_MIN_OUT;

	s_server->web_pages = NULL;

	Eth_if::Update_Hostname_Handle( S_Update_Host_From_Ip );

#ifdef JSON_ACTIVE
	User_Session_Init_Handler_Json( user_session );
#else
	User_Session_Init_Hanlder( user_session );
#endif
	User_Session_Init( user_session );
#ifdef JSON_ACTIVE
	Fw_Upgrade_Session_Init_Handler_Json( user_session );
#else
	Fw_Upgrade_Session_Init_Handler( user_session );
#endif
	/* Expected httpd_update_hostname to be called to set a real hostname. */
	s_server->demux.hostname = "BADHOST";

	Web_Page_Init( web_nv_ctrl );

	// find all the REST data structures specifically for the married device
	httpd_rest_dci::init_httpd_rest_dci();
	httpd_rest_dci::setRestStruct( prod_rest_def );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Http_Server_Set_Pages( const fw_www_header* pages, uint8_t ui_type )
{
	uint32_t return_val = 0;

	if ( pages == nullptr )
	{
		Push_TGINT();
		s_server->flags = HTTP_SERVER_FLAG_MAINTENANCE_PAGES;
		s_server->web_pages = NULL;
		if ( ui_type == RUN_MAINTENANCE_UI )
		{
			s_server->maintenance_pages = ( const fw_www_header* )s_maintenance_pages;
		}
		else
		{
			s_server->maintenance_pages = ( const fw_www_header* )s_default_page;
		}
		Pop_TGINT();
	}
	else
	{
		/* Check if web server initialized yet */
		if ( s_server->flags != HTTP_SERVER_FLAG_NOT_READY )
		{
			/* Assign the web pages. */
			s_server->web_pages = pages;
			s_server->flags = HTTP_SERVER_FLAG_WEB_PAGES;

			/* Find the default files. */
			fw_www_search_ctx ctx = { };
			bool loop_break = false;
			for ( uint32_t i = 0; ( ( i < COUNT_WWW_STD_FILE ) && ( false == loop_break ) ); ++i )
			{
				Fw_WWW_Init_Search_Ctx( s_server->web_pages, &ctx );
				uint32_t len = strlen( s_default_files[i] );
				uint32_t ret =
					Fw_WWW_Search( s_server->web_pages, ( const uint8_t* )s_default_files[i], len,
								   &ctx );

				if ( 1 == ret )
				{
					s_server->file_indices[i] = ctx.low;
					WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "Search File Indices = %d", s_server->file_indices[i] );
				}
				else
				{
					/* Error, pages must be present. */
					WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "WebUI Files Not found, Loading Maintainance Page" );
					s_server->flags = HTTP_SERVER_FLAG_MAINTENANCE_PAGES;
					s_server->web_pages = nullptr;
					return_val = 1;
					loop_break = true;
				}
			}
		}
		else
		{
			/* Web server has not initialized yet thus just updating flags */
			s_server->flags = HTTP_SERVER_FLAG_WEB_PAGES;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
hams_parser* Http_Server_Alloc_Parser( void* id, bool https_conn )
{
	hams_parser* return_val = NULL;
	bool function_end = false;

	uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;

	/* Not found, look for empty space to allocate it. */
	uint32_t idx = Http_Total_Pool_Block_Count() - 1;
	uint32_t free_count = 0;

	while ( ( idx > s_server->block_flags[0] ) && ( false == function_end ) )
	{
		if ( 0 == s_server->block_flags[idx] )
		{
			++free_count;
			if ( free_count == HTTP_MIN_REQ_COUNT )
			{
				s_server->block_flags[idx] = SERV_FLAG_PARSER | HTTP_MIN_REQ_COUNT;
				for ( uint32_t k = 1; k < HTTP_MIN_REQ_COUNT; ++k )
				{
					s_server->block_flags[idx + k] = SERV_FLAG_PARSER;
				}

				http_server_req_state* req = ( http_server_req_state* )
					( pool + idx * HTTP_POOL_BLOCK_SIZE );
				req->stack_data = id;

				hams_parser_init( &req->parser, https_conn );
				/* TODO use pointer arithmetic instead, save some bytes */
				req->parser.buffer_size = HTTP_REQ_BUFFER_BYTES;
				req->flags = 0;
				req->auth_level = 0;
				req->user_id = 0;
				req->max_auth_exempt_level = 0;
				g_hams_demultiplexer->init_cb( &req->parser, NULL, 0 );
				++s_server->stats[SERV_STAT_NEW_CONNS];

				function_end = true;
				return_val = &req->parser;
			}
		}
		else
		{
			free_count = 0;
		}
		--idx;
	}
	if ( false == function_end )
	{
		/* Otherwise not found and no space for it. */
		idx = s_server->block_flags[0] + 1;
		s_server->block_flags[idx] = SERV_FLAG_PARSER | HTTP_MIN_REQ_COUNT;
		for ( unsigned k = 1; k < HTTP_MIN_REQ_COUNT; ++k )
		{
			s_server->block_flags[idx + k] = SERV_FLAG_PARSER;
		}
		http_server_req_state* req = ( http_server_req_state* )
			( pool + idx * HTTP_POOL_BLOCK_SIZE );
		req->stack_data = id;
		hams_parser_init( &req->parser, https_conn );
		/* TODO use pointer arithmetic instead, save some bytes */
		req->flags = 0;
		req->auth_level = 0;
		req->user_id = 0;
		req->max_auth_exempt_level = 0;
		g_hams_demultiplexer->init_cb( &req->parser, NULL, 0 );
		req->parser.buffer_size = HTTP_REQ_BUFFER_BYTES;
		hams_respond_error( &req->parser, 429U );	/* Server Busy Error */
		++s_server->stats[SERV_STAT_DROPPED_CONNS];
		return_val = &req->parser;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
hams_parser* Http_Server_Get_Parser( void* id )
{
	hams_parser* return_val = NULL;
	bool function_end = false;

	uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
	ULBASE target = ( ULBASE )( id );

	/* Search for corresponding id. */
	uint32_t idx = Http_Total_Pool_Block_Count() - 1;

	while ( ( idx > s_server->block_flags[0] ) && ( false == function_end ) )
	{
		if ( ( s_server->block_flags[idx] & SERV_FLAG_PARSER ) &&
			 ( s_server->block_flags[idx] & SERV_MASK_ALLOC ) )
		{
			/* Found allocated block, check id. */
			http_server_req_state* req = ( http_server_req_state* )
				( pool + idx * HTTP_POOL_BLOCK_SIZE );
			if ( ( ( ULBASE )req->stack_data ) == target )
			{
				return_val = &req->parser;
				function_end = true;
			}
		}
		--idx;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Http_Server_Output_Sent( hams_parser* parser, uint32_t successful )
{

	uint8_t return_val = 0;
	uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;

	if ( NULL == parser )
	{
		uint32_t idx = Http_Total_Pool_Block_Count() - 1;
		while ( ( 0 == s_server->stager ) && ( idx > s_server->block_flags[0] ) )
		{
			// _ASSERT( 0 == g_hams_demultiplexer->staged_len );

			if ( ( s_server->block_flags[idx] & SERV_FLAG_PARSER ) &&
				 ( s_server->block_flags[idx] & SERV_MASK_ALLOC ) )
			{
				/* Found allocated block, check id. */
				http_server_req_state* req = ( http_server_req_state* )
					( pool + idx * HTTP_POOL_BLOCK_SIZE );

				if ( 0 != ( req->parser.flags & HAMS_FLAG_STAGE_REQ ) )
				{
					s_server->stager = &req->parser;
					if ( !( req->flags & REQ_FLAG_NO_START_REQ ) )
					{
						hams_start_response( &req->parser );
					}
					else
					{
						req->flags &= ~REQ_FLAG_NO_START_REQ;
					}
					/* Give user a chance to transmit headers. */
					req->parser.user_data_cb( &req->parser, NULL, 0 );
				}
			}
			--idx;
		}
		return_val = 0;
	}
	else
	{

		http_server_req_state* rs = Http_Server_Get_State( parser );
		DERR( "OUTPUT SENT %p\n", parser );

		/* If parser has any content remaining, give it a chance to output. */
		if ( ( parser->content_length ) && ( 0 == ( parser->flags & HAMS_FLAG_CONTENT ) ) )
		{
			if ( 0 != successful )
			{
				DERR( "ASKING USER CB %p\n", parser );
				parser->user_data_cb( parser, NULL, 0 );
				return_val = 1;
			}
			else
			{
				++s_server->stats[SERV_STAT_REM_HANGUPS];
				S_End_Request( rs );
				return_val = 0;
			}
		}
		else
		{
			/* TODO check keep alive. */

			if ( 0 != successful )
			{
				++s_server->stats[SERV_STAT_NO_MORE_OUTPUT];
				S_End_Request( rs );
			}
			else
			{
				/* TODO */
				S_End_Request( rs );
			}
			return_val = 0;
		}
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Http_Server_Input( hams_parser* parser, const uint8_t* data,
							uint16_t len )
{
	// _ASSERT( parser );
	uint16_t return_val = 0;

	if ( 0 != data )
	{
		uint32_t ret = hams_recv( parser, data, len );
		http_server_req_state* rs = Http_Server_Get_State( parser );
		if ( ( 0 != ( parser->flags & HAMS_FLAG_ERROR ) ) ||
			 ( 0 != ( parser->flags & HAMS_FLAG_REDIRECT ) ) )
		{
			S_End_Request( rs );
			return_val = 0;
		}
		else
		{

			if ( ( 0 == ret ) && ( parser->flags & HAMS_FLAG_ECHO_CONTENT ) )
			{
				S_End_Request( rs );
			}
			return_val = ret;
		}
	}
	else
	{
		DERR( "REMOTE CLOSED INPUT %p\n", parser );
		++s_server->stats[SERV_STAT_REM_CLOSED];
		/* Otherwise the remote closed the connection. */
		if ( 0 == ( parser->flags & HAMS_FLAG_SENDING ) )
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			S_End_Request( rs );
			return_val = 0;
		}
		else
		{
			DERR( "STILL SENDING\n" );
			http_server_req_state* rs = Http_Server_Get_State( parser );
			++s_server->stats[SERV_STAT_FINISH_SENDING];
			// _ASSERT( parser->content_length
			// | (rs->flags & REQ_FLAG_FIN)
			// | (parser->flags & HAMS_FLAG_STAGE_REQ) );
			/* Still sending data, keep open. */
			return_val = 1;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Http_Server_Count_Parsers( void )
{
	uint32_t count = 0;
	uint32_t idx = Http_Total_Pool_Block_Count() - HTTP_MIN_REQ_COUNT;

	while ( idx > s_server->block_flags[0] )
	{
		if ( ( s_server->block_flags[idx] & SERV_FLAG_PARSER ) &&
			 ( s_server->block_flags[idx] & SERV_MASK_ALLOC ) )
		{
			++count;
		}
		idx -= HTTP_MIN_REQ_COUNT;
	}

	return ( count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Http_Server_Alloc_Scratch( hams_parser* parser, uint32_t count )
{
	/* Make sure count is not astronomically high... */
	// _ASSERT( count < HTTP_POOL_BLOCK_COUNT - 1 - 2 );
	// _ASSERT( count > 0 );
	void* return_val = NULL;
	bool loop_break = false;
	uint32_t idx = Http_Total_Pool_Block_Count() - 1;
	uint32_t remaining = count;

	while ( ( idx > s_server->block_flags[0] ) && ( false == loop_break ) )
	{
		if ( 0 == s_server->block_flags[idx] )
		{
			--remaining;
			if ( 0 == remaining )
			{
				/* Get the block index of the request... */
				uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
				http_server_req_state* rs = Http_Server_Get_State( parser );

				for ( uint32_t k = 0; k < count; ++k )
				{
					s_server->block_flags[idx + k] = SERV_FLAG_SCRATCH;
				}
				s_server->block_flags[idx] |= count;

				uint8_t* ret = ( uint8_t* ) ( pool + idx * HTTP_POOL_BLOCK_SIZE );
				memset( ret, 0, HTTP_POOL_BLOCK_SIZE * count );

				return_val = ret;
				loop_break = true;
			}
		}
		else
		{
			remaining = count;
		}
		--idx;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
struct yxml_t_s* Http_Server_Alloc_Xml_Parser( hams_parser* parser, uint32_t stack_size )
{
	struct yxml_t_s* return_val = NULL;
	yxml_t* xml = ( yxml_t* ) ( Http_Server_Alloc_Scratch( parser, 1 ) );

	if ( 0 != xml )
	{

		if ( 0 == stack_size )
		{
			stack_size = HTTP_POOL_BLOCK_SIZE - sizeof ( *xml );
		}
		yxml_init( xml, stack_size );

		uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
		http_server_req_state* rs = Http_Server_Get_State( parser );
		const uint8_t* x = ( uint8_t* )rs;
		const uint32_t request_idx = ( x - pool ) / HTTP_POOL_BLOCK_SIZE;
		xml->server_request_idx = request_idx;

		return_val = xml;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Http_Server_Free( void* ptr )
{
	uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;

	// coverity[no_escape]
	_ASSERT( ptr >= pool && ptr < pool + HTTP_POOL_SIZE );
	// _ASSERT( ((size_t)pool % HTTP_POOL_BLOCK_SIZE) == ((size_t)ptr % HTTP_POOL_BLOCK_SIZE) );
	const uint32_t block_id = ( ( uint8_t* )ptr - pool ) / HTTP_POOL_BLOCK_SIZE;
	// _ASSERT( s_server->block_flags[block_id] );

	const uint32_t count = s_server->block_flags[block_id] - SERV_FLAG_SCRATCH;		// & 0xF;

	// _ASSERT( count );
	for ( uint32_t i = 0; i < count; ++i )
	{
		s_server->block_flags[block_id + i] = 0;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t S_Alloc_First_Dynamic( hams_parser* parser )
{
	bool loop_break = false;
	uint8_t return_val = 0;
	uint32_t idx = Http_Total_Pool_Block_Count() - 1;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	while ( ( idx > s_server->block_flags[0] ) && ( false == loop_break ) )
	{
		if ( 0 == s_server->block_flags[idx] )
		{
			/* Get the block index of the request... */
			uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
			const uint8_t* x = ( uint8_t* )rs;
			const uint32_t request_idx = ( x - pool ) / HTTP_POOL_BLOCK_SIZE;

			s_server->block_flags[idx] = SERV_FLAG_DYNAMIC_BUFFER | 1;
			dynamic_buffer* next = ( dynamic_buffer* ) ( pool + idx * HTTP_POOL_BLOCK_SIZE );
			parser->blocks_for_data++;

			next->dyn_mdata.request_id = request_idx;
			next->dyn_mdata.data_length = 0;
			next->dyn_mdata.next = 0;
			rs->p.dyn.start_block = idx;
			rs->p.dyn.block_id = idx;
			return_val = idx;
			loop_break = true;
		}
		if ( false == loop_break )
		{
			--idx;
		}
	}
	if ( false == loop_break )
	{
		rs->p.dyn.start_block = 0;
		rs->p.dyn.block_id = 0;
		return_val = 0;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Http_Server_Begin_Dynamic( hams_parser* parser )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	rs->p.dyn.byte_count = 0;
	parser->flags &= ~HAMS_FLAG_CONTENT;
	if ( 0 == ( rs->flags & REQ_FLAG_NO_DYN_PARSER_SPACE ) )
	{
		/* Note should NEVER be called if actually using the multipart */
		// _ASSERT( 0 == parser->ext.multipart_boundary_length );
		rs->p.dyn.start_block = Http_Total_Pool_Block_Count();
		rs->p.dyn.block_id = Http_Total_Pool_Block_Count();
	}
	else
	{
		S_Alloc_First_Dynamic( parser );
	}
}

/* Returns next block id to write to. 0 if could not be allocated. */
static uint32_t Http_Server_Write_Buffer( hams_parser* parser,
										  uint32_t block_id, const char_t* src, uint32_t len )
{
	uint32_t pool_block_count = Http_Total_Pool_Block_Count();
	uint32_t return_val = 0;
	bool function_end = false;

	/* Check if we are still writing to parser space. */
	if ( pool_block_count == block_id )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		if ( ( len + rs->p.dyn.byte_count ) < DYNIN_PARSER_LENGTH )
		{
			if ( HTTP_METHOD_HEAD != parser->method )
			{
				memcpy( DYNIN_PARSER_DATA( parser ) + rs->p.dyn.byte_count, src, len );
			}
			rs->p.dyn.byte_count += len;
			return_val = pool_block_count;
			function_end = true;
		}
		else
		{

			/* Otherwise need to allocate some dynamic data. */
			const uint32_t remain = DYNIN_PARSER_LENGTH - rs->p.dyn.byte_count;
			if ( HTTP_METHOD_HEAD != parser->method )
			{
				memcpy( DYNIN_PARSER_DATA( parser ) + rs->p.dyn.byte_count, src, remain );
			}
			rs->p.dyn.byte_count += remain;
			len -= remain;
			src += remain;
			block_id = S_Alloc_First_Dynamic( parser );
		}
	}
	if ( false == function_end )
	{
		uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
		dynamic_buffer* db = ( dynamic_buffer* ) ( pool + block_id * HTTP_POOL_BLOCK_SIZE );

		uint32_t cur_block = block_id;

		while ( ( ( len + db->dyn_mdata.data_length ) > DYNAMIC_BUFFER_SIZE ) &&
				( false == function_end ) )
		{
			const uint32_t remainder = DYNAMIC_BUFFER_SIZE - db->dyn_mdata.data_length;
			if ( HTTP_METHOD_HEAD != parser->method )
			{
				memcpy( db->data + db->dyn_mdata.data_length, src, remainder );
			}
			len -= remainder;
			src += remainder;
			db->dyn_mdata.data_length += remainder;

			/* Find free block. */
			uint32_t idx = pool_block_count - 1;
			bool loop_break = false;
			while ( ( idx > s_server->block_flags[0] ) && ( false == loop_break ) )
			{
				if ( 0 == s_server->block_flags[idx] )
				{
					s_server->block_flags[idx] = SERV_FLAG_DYNAMIC_BUFFER | 1;
					dynamic_buffer* next =
						( dynamic_buffer* ) ( pool + idx * HTTP_POOL_BLOCK_SIZE );
					parser->blocks_for_data++;

					next->dyn_mdata.request_id = db->dyn_mdata.request_id;
					next->dyn_mdata.data_length = 0;
					next->dyn_mdata.next = 0;

					db->dyn_mdata.next = idx;
					db = next;
					cur_block = idx;
					loop_break = true;
				}
				if ( false == loop_break )
				{
					--idx;
				}
			}

			if ( idx == s_server->block_flags[0] )
			{
				return_val = 0;
				function_end = true;
			}
		}
		if ( false == function_end )
		{
			if ( 0 != len )
			{
				memcpy( db->data + db->dyn_mdata.data_length, src, len );
				db->dyn_mdata.data_length += len;
			}

			return_val = cur_block;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Http_Server_Print_Dynamic( hams_parser* parser, const char_t* fmt, ... )
{

	uint8_t return_val = 0;
	bool response_update = false;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/* block_id of 0 indicates output overflow. */
	if ( rs->p.dyn.block_id <= s_server->block_flags[0] )
	{
		return_val = 0;
	}
	else
	{

		va_list ap;
		va_start( ap, fmt );

		const char_t* p = fmt;
		const char_t* start = fmt;
		uint32_t cur_block = rs->p.dyn.block_id;

		/* Fast forward to latest block if necessary.*/

		/* Variables used for string copying. */
		uint32_t copylen = 0U;
		const char_t* src = nullptr;

		char_t integers[9] = { 0U };

		while ( 0 != *p )
		{
			if ( *p != '\b' )
			{
				++p;
			}
			else
			{

				/* Copy data between start and p. */
				if ( 0 ==
					 ( cur_block = Http_Server_Write_Buffer( parser, cur_block, start,
															 p - start ) ) )
				{
					response_update = true;
				}
				if ( false == response_update )
				{

					/* Take action based on type. */
					++p;
					uint8_t loop_break = false;
					switch ( *p )
					{
						/* Unescape the '\b' character. */
						case '\b':
							copylen = 1;
							src = "\b";
							break;

						/* Decimal unsigned 32 bit integer. */
						case 'u':
						{
							/* Taken from bit twiddling hacks:
							   https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10 */
							uint32_t v = va_arg( ap, uint32_t );// non-zero 32-bit integer value to
																// compute the log base 10 of
							uint32_t r;			// result goes here

							r = ( v >= 1000000000 ) ? 10 : ( v >= 100000000 ) ? 9 :
								( v >= 10000000 ) ? 8 :
								( v >= 1000000 ) ? 7 :
								( v >= 100000 ) ? 6 :
								( v >= 10000 ) ? 5 :
								( v >= 1000 ) ? 4 :
								( v >= 100 ) ? 3 : ( v >= 10 ) ? 2 : 1;
							char_t* out = integers + r - 1;
							copylen = r;
							src = integers;
							do
							{
								*out = '0' + ( v % 10 );
								v /= 10;
								--out;
							} while ( v );
						}
						break;

						/* XML formatted string; expecting length and pointer. */
						case 'X':
							copylen = va_arg( ap, uint32_t );
							src = va_arg( ap, const char_t* );
							{
								const char_t* ps = src;
								const char_t* const end = src + copylen;
								while ( ( copylen ) && ( false == loop_break ) )
								{
									/* Printed only a prefix of the string, so just output and break */
									if ( ps == end )
									{
										if ( !( cur_block =
													Http_Server_Write_Buffer( parser, cur_block,
																			  src,
																			  ps - src ) ) )
										{
											response_update = true;
										}
										loop_break = true;
									}
									else
									{

										const char_t* entity;
										bool match_found = true;
										switch ( *ps )
										{
											/* Just advance pointer on non-entity chars. */
											default:
												++ps;
												match_found = false;
												break;

											case '\0':
												entity = NULL;
												break;

											case '&':
												entity = "&amp;";
												break;

											case '<':
												entity = "&lt;";
												break;

											case '>':
												entity = "&gt;";
												break;

												// case '\'':
												// entity = "&apos;";
												// break;
												//
												// case '"':
												// entity = "&quot;";
												// break;

										}
										if ( match_found == true )
										{
											/* Whether we found an entity or the null terminator, output whatever we
											   can. */
											if ( !( cur_block =
														Http_Server_Write_Buffer( parser,
																				  cur_block,
																				  src, ps - src ) ) )
											{
												response_update = true;
											}
											else
											{

												if ( entity )
												{
													++ps;
													if ( !( cur_block =
																Http_Server_Write_Buffer(
																	parser,
																	cur_block,
																	entity, strlen( entity ) ) ) )
													{
														response_update = true;
													}
													else
													{
														copylen -= ps - src;
														src = ps;
													}
												}
												else
												{
													copylen = 0;
												}
											}
										}
									}
								}
								if ( false == response_update )
								{
									copylen = 0;
								}
							}
							break;

						/* Two digit hex byte. */
						case 'b':
						{
							copylen = 2;
							uint32_t v = va_arg( ap, uint32_t );

							BIN_ASCII_Conversion::byte_to_str_no_prefix( v,
																		 reinterpret_cast<uint8_t*>( integers ) );
							src = integers;
						}
						break;

						/* String; expecting length and pointer. */
						case 's':
							copylen = va_arg( ap, uint32_t );
							src = va_arg( ap, const char_t* );
							break;

						/* Otherwise invalid char. */
						default:
							// _ASSERT( 0 );
							break;
					}
					if ( false == response_update )
					{
						if ( 0 != copylen )
						{
							if ( 0 ==
								 ( cur_block = Http_Server_Write_Buffer( parser, cur_block,
																		 src,
																		 copylen ) ) )
							{
								response_update = true;
							}
							else
							{}
						}
						if ( false == response_update )
						{
							++p;
							start = p;
						}
					}
				}
			}
		}

		/* TODO Copy remaining string data. */
		if ( ( false == response_update ) && ( p - start ) )
		{
			cur_block = Http_Server_Write_Buffer( parser, cur_block, start, p - start );
		}

		va_end( ap );
		rs->p.dyn.block_id = cur_block;
		/* Update the response code for the user. */
		if ( rs->p.dyn.block_id <= s_server->block_flags[0] )
		{
			parser->response_code = E503_DYNAMIC_SPACE;
			parser->user_data_cb = NULL;
			parser->flags |= HAMS_FLAG_ERROR;

			uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
			const uint8_t* x = ( uint8_t* )rs;
			const uint32_t request_idx = ( x - pool ) / HTTP_POOL_BLOCK_SIZE;
			uint32_t pool_block_count = Http_Total_Pool_Block_Count();
			for ( uint32_t i = s_server->block_flags[0]; i < pool_block_count;
				  ++i )
			{
				if ( s_server->block_flags[i] & SERV_FLAG_DYNAMIC_BUFFER )
				{
					dynamic_buffer* db =
						( dynamic_buffer* ) ( pool + i * HTTP_POOL_BLOCK_SIZE );
					if ( db->dyn_mdata.request_id == request_idx )
					{
						s_server->block_flags[i] = 0;
					}
				}
			}
			return_val = 0;
		}
		else
		{
			return_val = 1;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Http_Server_End_Dynamic( hams_parser* parser )
{
	uint8_t return_val = 0;
	uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	const uint8_t* x = ( uint8_t* )rs;
	const uint32_t request_idx = ( x - pool ) / HTTP_POOL_BLOCK_SIZE;
	uint32_t pool_block_count = Http_Total_Pool_Block_Count();

	/* If no overflow then begin outputting response. */
	if ( rs->p.dyn.block_id > s_server->block_flags[0] )
	{

		/* Count up dynamic data. */
		uint32_t content_length = rs->p.dyn.byte_count;
		for ( uint32_t i = s_server->block_flags[0]; i < pool_block_count; ++i )
		{
			if ( s_server->block_flags[i] & SERV_FLAG_DYNAMIC_BUFFER )
			{
				dynamic_buffer* db =
					( dynamic_buffer* ) ( pool + i * HTTP_POOL_BLOCK_SIZE );
				if ( db->dyn_mdata.request_id == request_idx )
				{
#ifdef PROFILE_DYNWASTED
					if ( db->dyn_mdata.data_length <= DYNAMIC_BUFFER_SIZE )
					{
						++s_wasted_bytes[DYNAMIC_BUFFER_SIZE - db->dyn_mdata.data_length];
					}
#endif
					content_length += db->dyn_mdata.data_length;
				}
			}
		}

		if ( parser->transmission_continue == true )
		{
			content_length = parser->total_content_length;
		}
		hams_response_header( parser, HTTP_HDR_CONTENT_LENGTH, ( void* )&content_length, 0 );
		/* Set parser content length. */

		parser->user_data_cb = S_Xmit_Dynamic_Data;

		/* Update dynamic output state. */
		rs->p.dyn.offset = 0;
		rs->p.dyn.block_id = rs->p.dyn.start_block;

		return_val = 1;
	}
	else
	{
		/* Release dynamic data. */
		for ( uint32_t i = s_server->block_flags[0]; i < pool_block_count; ++i )
		{
			if ( 0 != ( s_server->block_flags[i] & SERV_FLAG_DYNAMIC_BUFFER ) )
			{
				dynamic_buffer* db =
					( dynamic_buffer* ) ( pool + i * HTTP_POOL_BLOCK_SIZE );
				if ( db->dyn_mdata.request_id == request_idx )
				{
					s_server->block_flags[i] = 0;
				}
			}
		}

		/* Otherwise change the response to 503 */
		parser->response_code = E503_DYNAMIC_SPACE;
		hams_start_response( parser );
		hams_response_header( parser, COUNT_HTTP_HDRS, NULL, 0 );

		parser->user_data_cb = NULL;

		return_val = 0;
	}
	return ( return_val );
}

/* **********************************************************
   hamsif_* functions.
 ***********************************************************/

uint32_t Hamsif_Extend_Output( hams_parser* parser, uint32_t extended_len )
{
	// _ASSERT( parser );
	uint8_t loop_break = false;

	/* If asking for less than the minimum, just allocate what we have. */
	if ( extended_len < HAMS_MIN_STAGE_EXT )
	{
		/* Clear the stager pointer if this is zero. */
		if ( 0 == extended_len )
		{
			// _ASSERT( parser == s_server->stager );
			s_server->stager = ( hams_parser_s* ) ( NULL );
		}
		/* Indicate the initial blocks are added already. */
		s_server->block_flags[0] = HTTP_MIN_OUT;
	}
	else if ( extended_len > ( s_server->block_flags[0] * HTTP_POOL_BLOCK_SIZE ) )
	{
		/* Make sure user is asking for less than the maximum amount we can allocate. If greater than just return what
		   we have. */
		if ( extended_len <= ( SERV_MASK_ALLOC * HTTP_POOL_BLOCK_SIZE ) )
		{
			/* Calculate blocks needed. */
			const uint32_t needed =
				( ( extended_len + HTTP_POOL_BLOCK_SIZE - 1 ) / HTTP_POOL_BLOCK_SIZE );

			/* Find first allocated block. */
			uint32_t i;
			for ( i = HTTP_MIN_OUT; ( ( i <= needed ) && ( false == loop_break ) ); ++i )
			{
				if ( s_server->block_flags[i] )
				{
					/* This block is not free, cannot allocate the memory. TODO implement block rearranging to see if
					   this can be moved to a block farther up the pool.*/
					loop_break = true;
				}
			}

			/* Have the needed blocks, allocate. */
			if ( i > needed )
			{
				s_server->block_flags[0] = needed;
			}
		}
	}

	return ( s_server->block_flags[0] * HTTP_POOL_BLOCK_SIZE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Check_Block_Overflow_And_Datablock_Limit( hams_parser* parser )
{
	bool return_val = false;
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/*
	   We have 48 data block for dynamic buffer as defined by HTTP_POOL_BLOCK_COUNT. Out of which only 30 are used for
	      large data service (like rs/param, assembly) and 20 are used by concurrent queries.
	 */

	if ( ( rs->p.dyn.block_id - 15 ) <= s_server->block_flags[0] )
	{
		return_val = true;
	}
	else
	{
		if ( parser->blocks_for_data < MAX_BLOCKS_FOR_LARGE_DATA_SERVICE )
		{
			return_val = false;
		}
		else
		{
			return_val = true;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Free_Dynamic_Memory( hams_parser* parser )
{
	if ( NULL != parser->cors_auth_common )
	{
#ifdef __DIGEST_ACCESS_AUTH__
		if ( NULL != parser->cors_auth_common->digest_ptr )
		{
			delete( parser->cors_auth_common->digest_ptr );
		}
#endif
		if ( NULL != parser->cors_auth_common->origin_buffer )
		{
			delete( parser->cors_auth_common->origin_buffer );
			parser->cors_auth_common->origin_buffer = NULL;
		}

		delete( parser->cors_auth_common );
		parser->cors_auth_common = NULL;

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Get_Server_Flags( void )
{
	return ( s_server->flags );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Http_Total_Pool_Block_Count( void )
{
	uint32_t ret_val = HTTP_POOL_BLOCK_COUNT;

	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void S_Update_Host_From_Ip( uint32_t ip )
{
	static char ipstr[4 * 3 + 3 + 1];
	char* out = ipstr;

	const uint8_t* ipbytes = ( const uint8_t* )&ip;

	for ( unsigned i = 4; i > 0; --i )
	{
		if ( i != 4 )
		{
			*out = '.';
			++out;
		}

		uint8_t v = ipbytes[i - 1];
		if ( v >= 100 )
		{
			if ( v >= 200 )
			{
				*out = '2';
				v -= 200;
			}
			else
			{
				*out = '1';
				v -= 100;
			}
			++out;

			/* Print the tens place 0 if necessary. */
			if ( v < 10 )
			{
				*out = '0';
				++out;
			}
		}

		/* Print nonzero tens place digit. */
		if ( v >= 10 )
		{
			*out = v / 10 + '0';
			++out;
			v %= 10;
		}

		/* Print ones place digit. */
		*out = v + '0';
		++out;
	}
	*out = '\0';

	s_server->demux.hostname = ipstr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Attach_Prod_Spec_Rest_Callback( PROD_SPEC_REST_CBACK rest_cb )
{
	if ( rest_cb != nullptr )
	{
		prod_spec_rest_cb = rest_cb;
	}
}

#ifdef JSON_ACTIVE
struct yjson_t_s* Http_Server_Alloc_Json_Parser( hams_parser* parser, uint32_t stack_size )
{
	struct yjson_t_s* return_val = NULL;
	yjson_t* json = ( yjson_t* ) ( Http_Server_Alloc_Scratch( parser, 1 ) );

	if ( 0 != json )
	{

		if ( 0 == stack_size )
		{
			stack_size = HTTP_POOL_BLOCK_SIZE - sizeof ( *json );
		}
		yjson_init( json, stack_size );

		uint8_t* pool = ( ( struct real_http_server* )s_server )->pool;
		http_server_req_state* rs = Http_Server_Get_State( parser );
		const uint8_t* x = ( uint8_t* )rs;
		const uint32_t request_idx = ( x - pool ) / HTTP_POOL_BLOCK_SIZE;
		json->server_request_idx = request_idx;

		return_val = json;
	}
	return ( return_val );
}

#endif