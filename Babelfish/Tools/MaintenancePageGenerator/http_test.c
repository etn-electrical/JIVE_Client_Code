#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include "hams.h"

typedef struct example_user_data_s{
	unsigned uri_depth;
	unsigned header_count;
	uint8_t query_field;
} example_user_data;

#define POOL_BLOCK_COUNT 16
static uint8_t s_demux_memory[sizeof(struct hams_demultiplexer_s)
	+ HAMS_POOL_BLOCK_SIZE * POOL_BLOCK_COUNT];

struct hams_demultiplexer_s *g_hams_demultiplexer =
	(struct hams_demultiplexer_s *)s_demux_memory;


static void print_parser(hams_parser* parser){
	printf("\t,\"parser\":{\n");

	printf("\t\t\"content-length\":%u\n", parser->content_length);

	if(parser->ext.multipart_boundary_length){
		printf("\t\t,\"multipart_boundary\":\"%.*s\"\n",
			parser->ext.multipart_boundary_length, parser->ext.multipart_boundary);
	}

	printf("\t}\n");
}

static unsigned s_form_data(hams_parser* parser, const uint8_t* data, unsigned length){
	if(data)
		fprintf(stderr, "s_form_data ||%.*s||\n", length, data);
	else
		fprintf(stderr, "s_form_data NULL\n");

	return 0;
}

static unsigned s_headers_complete(hams_parser* parser, const uint8_t* data, unsigned length){
	assert(!data);

	fprintf(stderr, "s_headers_complete\n");

	print_parser(parser);

	if(!parser->ext.multipart_boundary_length){
		printf("}\n");
		parser->response_code = 200;
		return 200;
	}

	parser->user_data_cb = s_form_data;
	return 0;
}

static unsigned s_uri_query(hams_parser* parser, const uint8_t* data, unsigned length){
	assert(data);
	assert(length);

	fprintf(stderr, "s_uri_query %u\n", length);
	example_user_data* u = (example_user_data*)(parser->user_data);

	if(u->query_field){
		/* Process field name. TODO should be JSON escaped...*/
		printf("[\"%.*s\"", length, data);
		/* ] */

		if(data[length] == '='){
			u->query_field = 0;
			return 0;
		}
	}
	else{
		/* Process value name. TODO should be JSON escaped... [*/
		printf(",\"%.*s\"]", length, data);

		if(data[length] == '&'){
			printf(",");
			u->query_field = 1;
			return 0;
		}
	}

	if(data[length] == ' '){
		/* [ End query part */
		printf("]\n");
		parser->user_data_cb = s_headers_complete;
		/* Reached end of URI */
		return 0;
	}

	return 0;
}

static unsigned s_uri_part(hams_parser* parser, const uint8_t* data, unsigned length){
	assert(data);
	assert(length);

	fprintf(stderr, "s_uri_part %u\n", length);
	example_user_data* u = (example_user_data*)(parser->user_data);

	/* Must start with '/', since we start with hostname... */
	assert(data[0] == '/');

	if(0 == u->uri_depth)
		printf("{\n\t\"method\":%u\n\t,\"url\":[", parser->method);

	++u->uri_depth;
	/* Print URL segment */
	printf("\"%.*s\"", length, data);

	/* Check tail character to see what's next. */
	if(data[length] == '?'){
		printf("]\n\t,\"query\":[");
		/* ] */

		parser->user_data_cb = s_uri_query;
		u->query_field = 1;
		return 0;
	}

	if(data[length] == ' '){
		/* [ */
		printf("]\n");
		parser->user_data_cb = s_headers_complete;
		/* Reached end of URI */
		return 0;
	}

	/* Otherwise must be another '/' following. */
	assert(data[length] == '/');

	/* Get ready to print another URI segment */
	printf(",");

	return 0;
}

static unsigned s_init(hams_parser* parser, const uint8_t* data, unsigned length){
	example_user_data* u = (example_user_data*)(parser->user_data);
	fprintf(stderr, "s_init %u\n", length);
	u->uri_depth = 0;
	u->header_count= 0;
	parser->user_data_cb = s_uri_part;

	return 0;
}

int main(int argc, const char* argv[]){
	/* Initialize the demux. */
	g_hams_demultiplexer->pool_size = sizeof(s_demux_memory);
	hams_init();
	g_hams_demultiplexer->init_cb = s_init;

	example_user_data u;

	printf("[\n");
	/* ] */

	hams_parser *hsp = hams_open(&u);
	if(!hsp){
		fprintf(stderr, "Failed to open HAMS session\n");
		return 1;
	}

#define BUFFSIZE 256
	uint8_t buffer[BUFFSIZE];
	int cur_read;
	while((cur_read = read(0, buffer, BUFFSIZE))){
		/* Check for read error. */
		if(cur_read < 0){
			fprintf(stderr, "Read error %s\n", strerror(errno));
			hams_recv(hsp, NULL, 1);
			return 1;
		}

		if(hams_recv(hsp, buffer, cur_read))
			break;
	}

	printf("]");
	return 0;
}
