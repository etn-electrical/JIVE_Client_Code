/**
 *****************************************************************************************
 *	@file        Server header file for the webservices.
 *
 *	@brief       This file shall wrap the functions required for server functioning while the webservices function using
 * the REST protocol.
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

#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "hams.h"
#include "REST_FUS_Interface.h"
#include "Rest_WebUI_Interface.h"
#include "User_Session.h"

#ifdef JSON_ACTIVE
#include "REST_FUS_Interface_Json.h"
#include "Diag_Json.h"
#include "Yjson.h"
#endif

/* Forward declaration */
struct yxml_t_s;
#ifdef JSON_ACTIVE
struct yjson_t_s;
#endif

#define HTTP_MIN_OUT \
	( ( HAMS_MIN_STAGE_EXT + HTTP_POOL_BLOCK_SIZE - 1 ) / ( HTTP_POOL_BLOCK_SIZE ) )

#define HTTP_POOL_SIZE ( ( HTTP_POOL_BLOCK_SIZE ) *( HTTP_POOL_BLOCK_COUNT ) )

/** @brief Standard files for webservices */
enum
{
	/** @brief Standard files for webservices 404(Not Found) */
	WWW_STD_FILE_404
	/** @brief Default files */
	, WWW_STD_FILE_DEFAULT
	/** @brief Index files */
	, WWW_STD_FILE_INDEX
	/** @brief Standard Count files */
	, COUNT_WWW_STD_FILE
};

static const uint32_t WRITE_FLAG_STATIC_COPY = 0U;	// Flag value used to identify https page static buffer data send
													// request
static const uint32_t WRITE_FLAG_DYNAMIC_COPY = 1U;	// Flag value used to identify https page dynamic buffer data send
													// request
static const uint32_t WRITE_FLAG_NV_CTRL_COPY = 2U;	// Flag value used to identify https page external flash buffer data
													// send request
static const uint32_t WRITE_FLAG_CLOSE_COPY = 3U;	// Flag value to identify https page close buffer send request

static const uint8_t REQ_FLAG_ETAG_MATCH = 0x01;
static const uint8_t REQ_FLAG_FIN = 0x02;
static const uint8_t REQ_FLAG_RETRY = 0x04;
static const uint8_t REQ_FLAG_AUTH_VALID = 0x08;
static const uint8_t REQ_FLAG_MAINTENANCE_PAGES = 0x10;
static const uint8_t REQ_FLAG_NO_DYN_PARSER_SPACE = 0x20;
static const uint8_t REQ_FLAG_NO_START_REQ = 0x40;
static const uint8_t COUNT_REQ_FLAGS = REQ_FLAG_NO_START_REQ + 1;

enum
{
	/** @brief Number of blocks to be used for composing header responses. */
	SERV_MASK_ALLOC = 0x0F

		/** @brief Buffer space used for HTTP connection. */
	, SERV_FLAG_PARSER = 0x80

		/** @brief Buffer space used for composing dynamic responses. */
	, SERV_FLAG_DYNAMIC_BUFFER = 0x40

		/** @brief Dynamic buffer space used for general scratch purposes. */
	, SERV_FLAG_SCRATCH = 0x20 | SERV_FLAG_DYNAMIC_BUFFER
};

/** @brief Server status */
enum
{
	/** @brief Server status New Connections */
	SERV_STAT_NEW_CONNS
	/** @brief Server status Dropped Connections */
	, SERV_STAT_DROPPED_CONNS
	/** @brief Server status Connections Closed */
	, SERV_STAT_REM_CLOSED
	/** @brief Server status Connections Hangs up*/
	, SERV_STAT_REM_HANGUPS
	/** @brief Server status Finish sending data */
	, SERV_STAT_FINISH_SENDING
	/** @brief Server status No output remaining */
	, SERV_STAT_NO_MORE_OUTPUT
	/** @brief Server status End the request */
	, SERV_STAT_END_REQUESTS,
	COUNT_SERV_STATS
};



enum
{
	/** @brief Web Server/ UI is not initialized, send 503 service unavailable */
	HTTP_SERVER_FLAG_NOT_READY = 0x00,
	/** @brief Serve maintenance pages instead of pages in flash. */
	HTTP_SERVER_FLAG_MAINTENANCE_PAGES = 0x01,
	/** @brief Web UI pages */
	HTTP_SERVER_FLAG_WEB_PAGES = 0x02,
};

/** @brief Structure for web page and maintenance page. */
typedef struct http_server_s
{
	const fw_www_header* web_pages;

	const fw_www_header* maintenance_pages;

	uint16_t flags;

	/** @brief Pointer to parser currently using the staging area. */
	hams_parser* stager;

	/** @brief Important files to index. */
	uint16_t file_indices[COUNT_WWW_STD_FILE];

	/** @brief HAMS demultiplexer configuration. */
	struct hams_demultiplexer_s demux;

	unsigned stats[COUNT_SERV_STATS];

	/** @brief Block state flags. Note that block 0 is special; its role is to provide the beginning of staging space
	   for HTTP headers. Thus its flag value is simply the number of block required to stage the HTTP header text. */
	uint8_t block_flags[HTTP_POOL_BLOCK_COUNT];

} http_server;
/** @brief Structure for server request state. */
typedef struct http_server_req_state_s
{

	union
	{
		/** @brief  */
		fw_www_search_ctx fs_ctx;

		struct rest_s
		{
			void* ptr;

			/* For PUT requests */
			struct yxml_t_s* yxml;
#ifdef JSON_ACTIVE
			struct yjson_t_s* yjson;
#endif
			void* put_request;

			void* assy_ptr;
			uint8_t assy_type;
			uint16_t rest_param;

		} rest;

		struct fw_op
		{
			void* ptr;
			struct yxml_t_s* yxml;
#ifdef JSON_ACTIVE
			struct yjson_t_s* yjson;
#endif
			uint8_t image_id;
			uint8_t proc_id;
			uint8_t state;
			uint16_t session_timeout;
			uint32_t session_id;
		} fwop;
		struct user_mgmt
		{
			void* ptr;
			struct yxml_t_s* yxml;
#ifdef JSON_ACTIVE
			struct yjson_t_s* yjson;
#endif
			void* put_request;
			uint8_t req_user_id;
			uint8_t req_user_role;
			uint16_t reserved;
		} usermgmt;
		struct redirect
		{
			void* filename;
			uint8_t file_name_len;
		} redir;

		struct dynamic_response
		{
			uint16_t offset;
			uint8_t block_id;
			uint8_t start_block;
			unsigned byte_count;

			struct rest_list_s
			{
				unsigned count;
				unsigned max_auth;
				// uint8_t has_9999;
			} rest_list;

		} dyn;

		struct static_response_s
		{
			/** @brief Pointer to file data. */
			const uint8_t* file_ptr;
			unsigned len;
		} static_response;

		struct log_s
		{
			void* ptr;
			struct yxml_t_s* yxml;
			uint32_t entry_id;
			uint32_t num_of_entries;
			bool tail;
			bool data_after_tail;
			uint8_t log_id;
		} log_s;

		/* Metadata for language URIs */
		struct lang_s
		{
			void* ptr;
			struct yxml_t_s* yxml;
			uint8_t id;				///< Use to store language ID
		} lang;

	} p;

	uint8_t flags;

	uint8_t auth_level;
	uint8_t user_id;
	uint8_t max_auth_exempt_level;

	/** @brief State maintained for communicating with TCP stack. */
	void* stack_data;

	/** @brief Parser state + buffer. */
	hams_parser parser;

} http_server_req_state;

/** @brief Initialize the Web server
 *  @param[in] Send callback
 */
void Web_Server_Init( REST_TARGET_INFO_ST_TD const* prod_rest_def, User_Session* user_session, NV_Ctrl* web_nv_ctrl );

/** @brief Initialize the http server
 *  @param[in] Send callback
 */
void Http_Server_Init( hams_send_cb send_cb );

/** @brief Get handle to http server. */
http_server* Http_Server_Get_Handle( void );

/** @brief Set pages
 * @param[in] Pages  Header pointing to the pages to be set.
 * @param[in] ui_type  ui_type - Maintainance page, webui page or default_page.
 */
uint32_t Http_Server_Set_Pages( const fw_www_header* pages, uint8_t ui_type );

/** @brief Allocate parser instance
 *  @return[out]NULL if no space available.
 **/
hams_parser* Http_Server_Alloc_Parser( void* id, bool_t https_conn );

/** @brief       Allocate a block for general scratch purpose.
 *  @param[in] count Number of contiguous blocks.
 *  @return[out] pointer to HTTP_POOL_BLOCK_SIZE data or 0 if no memory available.
 **/
void* Http_Server_Alloc_Scratch( hams_parser* parser, unsigned count );

/** @brief Allocate a block for XML parsing.
 *  @param[in] stack_size if 0, stack size determined by HTTP_POOL_BLOCK_SIZE
 *  @return[out] pointer to yxml or 0 if no memory available.
 **/
struct yxml_t_s* Http_Server_Alloc_Xml_Parser( hams_parser* parser, unsigned stack_size );

/** @brief Free a block used for scratch or xml.
 *  @return[out] pointer to HTTP_POOL_BLOCK_SIZE data or 0 if no memory available.
 **/
void Http_Server_Free( void* ptr );

/** @brief Start a dynamic response body.
 *  @return[out] block id or 0 if no memory available.
 **/
void Http_Server_Begin_Dynamic( hams_parser* parser );

/** @brief Write out data for dynamically formed response body.
 *  @param[in] block block id returned in Http_Server_Begin_Dynamic
 *  @return[out] 0 if none written or error.
 **/
unsigned Http_Server_Print_Dynamic( hams_parser* parser, const char* fmt, ... );

/** @brief End dynamic reponse body.
 *  @return[out] 0 if error (ie not enough memory for response), nonzero on success.
 **/
unsigned Http_Server_End_Dynamic( hams_parser* parser );

/** @brief Get a parser context, given the id.
 *  @return[out] NULL if no more parsers available; otherwise parser context. NOTE that this call MAY send data (ie 501
 * out of memory or 429 too many conns)
 */
hams_parser* Http_Server_Get_Parser( void* id );

/** @brief Inform server more output is sent.
 *  @param[in] code If 0 then send failed; otherwise number of bytes user requested to send via send_cb().
 *  @return[out] 0 if connection should be shutdown.
 */
unsigned Http_Server_Output_Sent( hams_parser* parser, unsigned successful );

/** @brief Inform server of input for parser.
 *  @return[out] 0 if connection should be shutdown.
 */
uint16_t Http_Server_Input( hams_parser* parser, const uint8_t* data, uint16_t len );

/** @brief  Get server state.
 *  @return[out] Sever state.
 */
static inline http_server_req_state* Http_Server_Get_State( hams_parser* parser )
{
	/* The hams parser is the last element in Http_Server_Get_State */
	return ( ( http_server_req_state* ) ( ( uint8_t* )parser - sizeof( http_server_req_state )
										  + sizeof( hams_parser ) ) );
}

/** @brief  Count the parsers*/
unsigned Http_Server_Count_Parsers( void );

uint32_t Redirect_To_Https_Server( hams_parser* parser, const uint8_t* data,
								   uint32_t length );

/** @brief  call to init User Session for User management file
 *  @return[in] user_account pointer to User Account.
 */
void User_Session_Init_Hanlder( User_Session* user_session );

/** @brief  call to init User Session for hams file
 *  @return[in] user_session pointer to User Account.
 */
void User_Session_Init( User_Session* user_session );

/** @brief Initiate a system reboot.
 *  @return[out] 0 on success; < 0 on error. */
/** @brief Initiate a system reboot. */

bool_t Check_Block_Overflow_And_Datablock_Limit( hams_parser* parser );


/** @brief  call to initiate dynamic buffer data transmission
 *  @param[in] parser state
 *  @param[in] data buffer to be transmitted
 *  @param[in] data length
 */
unsigned S_Xmit_Dynamic_Data( hams_parser* parser, const uint8_t* data,
							  unsigned length );

/** @brief  call to initiate static buffer data transmission
 *  @param[in] parser state
 *  @param[in] data buffer to be transmitted
 *  @param[in] data length
 */
uint32_t S_Xmit_Flash_Data( hams_parser* parser, const uint8_t* data,
							uint32_t length );

/** @brief  call to initiate webui static buffer data transmission
 *  @param[in] parser state
 *  @param[in] data buffer to be transmitted
 *  @param[in] data length
 */
uint32_t S_Xmit_Webui_Data( hams_parser* parser, const uint8_t* data,
							uint32_t length );

uint16_t Get_Server_Flags( void );

/**@brief URIs to redirect to root and maintence page */
enum
{
	URI_REDIRECT_ROOT,
	COUNT_URI_REDIRECTS

};

typedef uint32_t ( * PROD_SPEC_REST_CBACK )( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief Attach call back for prod spec rest handler
 * @param[in] rest_cb : prod spec rest handler.
 * @return None.
 */
void Attach_Prod_Spec_Rest_Callback( PROD_SPEC_REST_CBACK rest_cb );

#ifdef JSON_ACTIVE
/** @brief Allocate a block for JSON parsing.
 *  @param[in] stack_size if 0, stack size determined by HTTP_POOL_BLOCK_SIZE
 *  @return[out] pointer to yjson or 0 if no memory available.
 **/
struct yjson_t_s* Http_Server_Alloc_Json_Parser( hams_parser* parser, unsigned stack_size );

/** @brief  call to init User Session for User management file
 *  @return[in] user_account pointer to User Account.
 */
void User_Session_Init_Handler_Json( User_Session* user_session );

#endif //JSON_ACTIVE

#endif
