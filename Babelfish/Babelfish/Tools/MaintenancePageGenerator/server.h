#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "hams.h"
#include "fw.h"

#define HTTP_MIN_OUT \
	((HAMS_MIN_STAGE_EXT + HTTP_POOL_BLOCK_SIZE - 1) / (HTTP_POOL_BLOCK_SIZE))
#define HTTP_POOL_BLOCK_SIZE 128
#define HTTP_POOL_BLOCK_COUNT 16
#define HTTP_POOL_SIZE ((HTTP_POOL_BLOCK_SIZE) * (HTTP_POOL_BLOCK_COUNT))

enum {
	WWW_STD_FILE_404
	,WWW_STD_FILE_DEFAULT
	,WWW_STD_FILE_INDEX
	,COUNT_WWW_STD_FILE
};

enum {
	REQ_FLAG_ETAG_MATCH = 0x01
	,REQ_FLAG_FIN = 0x02
	,COUNT_REQ_FLAGS
};

enum {
	SERV_MASK_ALLOC = 0x07
	,SERV_FLAG_PARSER = 0x80
};

enum {
	SERV_STAT_NEW_CONNS
	,SERV_STAT_DROPPED_CONNS
	,SERV_STAT_REM_CLOSED
	,SERV_STAT_REM_HANGUPS
	,SERV_STAT_FINISH_SENDING
	,SERV_STAT_NO_MORE_OUTPUT
	,COUNT_SERV_STATS
};

enum {
	/* No error. */
	SERV_FW_ERR_OK

	/* Too much data received. */
	,SERV_FW_ERR_OVERFLOW

	/* HW error writing to flash. */
	,SERV_FW_ERR_HW

	/* Cannot write to flash at the moment. */
	,SERV_FW_ERR_LOCKED

	,COUNT_SERV_FW_ERR
};

typedef struct http_server_s{
	const fw_www_header* web_pages;

	uint16_t flags;

	/** @brief Pointer to parser currently using the staging area. */
	hams_parser* stager;

	/** @brief Important files to index. */
	uint16_t file_indices[COUNT_WWW_STD_FILE];

	/** @brief HAMS demultiplexer configuration. */
	struct hams_demultiplexer_s demux;

	unsigned stats[COUNT_SERV_STATS];

	/** @brief Block state flags. */
	uint8_t block_flags[HTTP_POOL_BLOCK_COUNT];

	/** @brief Memory pool from which to allocate hams_parser, etc. In bytes. */
	uint8_t pool[0];

} http_server;

typedef struct http_server_req_state_s {
	union {
		/** @brief  */
		fw_www_search_ctx fs_ctx;

		/** @brief URL parameters parsed from the paths. */
		uint16_t url_param[2];

		struct fw_op{
			uint8_t target;
			uint8_t state;
		} fwop;

		/** @brief Pointer to file data. */
		const uint8_t* file_ptr;
	} p;

	uint16_t flags;

	/** @brief State maintained for communicating with TCP stack. */
	void* stack_data;

	/** @brief Parser state + buffer. */
	hams_parser parser;

} http_server_req_state;

/** @brief Initialize the http server
 *  @return Handle to http server. */
http_server* http_server_init(hams_send_cb send_cb);

/** @brief Set pages */
unsigned http_server_set_pages(const fw_www_header* pages);

/** @brief Allocate parser instance
 *  @return NULL if no space available. */
hams_parser* http_server_alloc_parser(void* id);

/** @brief Get a parser context, given the id.
 *  @return NULL if no more parsers available; otherwise parser context.
 *
 *  NOTE that this call MAY send data
 *  (ie 501 out of memory or 429 too many conns) */
hams_parser* http_server_get_parser(void* id);

/** @brief Inform server more output is sent.
 *  @param code If 0 then send failed; otherwise number of bytes user requested to send via send_cb().
 *  @return 0 if connection should be shutdown. */
unsigned http_server_output_sent(hams_parser* parser, unsigned successful);

/** @brief Inform server of input for parser.
 *  @return 0 if connection should be shutdown. */
unsigned http_server_input(hams_parser* parser, const uint8_t* data, unsigned len);

/** @brief Inform http server that a segment of memory is ready for use.
 *  This is intended for storing read-only nonvolatile data such as
 *  web pages, firmware upload, etc. */
unsigned http_server_memory_ready(unsigned type);

static inline http_server_req_state* http_server_get_state(hams_parser* parser){
	return (http_server_req_state *)parser->user_data;
}

unsigned http_server_count_parsers(void);


/* External user defined functions required for http_server to function. */
extern unsigned http_serverif_clear_memory(void);

/* External user defined functions for receiving FW data.
 * @param data Pointer to data to write; NULL if no more data to recv.
 * @param length number of bytes to write; if nonzero and data is NULL, then
 *  abort the write.
 * @return One of SERV_FW_ERR_* result codes. */
extern unsigned http_serverif_fw_recv(const uint8_t* data, unsigned length);

/** @brief How much of the send window remains for queuing output data. */
extern unsigned http_serverif_send_size(http_server_req_state* rs);

#endif
