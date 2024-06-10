#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <poll.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/mman.h>

#include "server.h"

static uint8_t s_web_page_space[1024*1024];
static unsigned s_web_page_offset = 0;
static unsigned s_web_locked = 0;

enum {
	MAX_CONNS = 8
	,RX_BUFF_SIZE = 1024
	/* We can make buffers big on a desktop system... */
	,TX_BUFF_SIZE = 1492
};


typedef struct conn_state_s {
	/* Internally buffered output data;*/
	uint8_t buffer[TX_BUFF_SIZE];

	/* How much data left to send. */
	unsigned remaining;

	/* Operation flags. */
	uint16_t flags;

} conn_state;

static unsigned long conn_count = 0;
static conn_state conns[MAX_CONNS];
static struct pollfd pfds[MAX_CONNS + 1];

/* Clear this just like flash memory (set to all 0xFF) */
unsigned http_serverif_clear_memory(void){
	fprintf(stderr, "CLEARING FW MEMORY\n");
	memset(s_web_page_space, 0xFF, sizeof(s_web_page_space));
	s_web_page_offset = 0;
	s_web_locked = 0;

	/* Ready to use. */
	return 0;
}

unsigned http_serverif_fw_recv(const uint8_t* data, unsigned length){
	if(s_web_locked){
		fprintf(stderr, "FW IS STILL LOCKED\n");
		return SERV_FW_ERR_LOCKED;
	}

	if(!data){
		fprintf(stderr, "FW XFER OVER, LOCKING\n");
		/* Not really any flash to worry about.... */
		s_web_locked = 1;
		return SERV_FW_ERR_OK;
	}

	if(s_web_page_offset + length > sizeof(s_web_page_space)){
		fprintf(stderr, "FW XFER OVERFLOW %u\n", s_web_page_offset + length);
		return SERV_FW_ERR_OVERFLOW;
	}

	memcpy(s_web_page_space + s_web_page_offset, data, length);
	s_web_page_offset += length;

	return SERV_FW_ERR_OK;
}

unsigned http_serverif_send_size(http_server_req_state* rs){
	unsigned long idx = (unsigned long)rs->stack_data;
	assert(idx < conn_count);
	conn_state* cs = conns + idx;
	return TX_BUFF_SIZE - cs->remaining;
}

unsigned s_send_cb(hams_parser* parser, const uint8_t* data, unsigned length,
	unsigned copy)
{
	http_server_req_state *rs = http_server_get_state(parser);
	unsigned long idx = (unsigned long)rs->stack_data;

	assert(idx < conn_count);
	conn_state* cs = conns + idx;

	/* In this case, we do not distingush on the copy parameter. */
	/* Check if there is enough room to buffer. */
	assert(length + cs->remaining <= TX_BUFF_SIZE);
	memcpy(cs->buffer + cs->remaining, data, length);
	cs->remaining += length;

	fprintf(stderr, "-->SEND_CB %p %lu %u\n", parser, idx, length);

	return 1;
}

int iterate(void){

	unsigned parser_count = http_server_count_parsers();
	fprintf(stderr, "-->POLLING %lu %u\n", conn_count + 1, parser_count);
	assert(parser_count == conn_count);

	int ret = poll(pfds, 1 + conn_count, -1);
	if(ret < 0){
		fprintf(stderr, "POLL ERROR %s\n", strerror(errno));
		return 1;
	}

	/* Output loop; output first to free up any RAM */
	for(unsigned long i = 0; i < conn_count; ++i){
		if(!(pfds[i + 1].revents & POLLOUT))
			continue;

		/* Output any data waiting. */
		int fd = pfds[i + 1].fd;
		conn_state *cs = conns + i;
		int ret = send(fd, cs->buffer, cs->remaining, MSG_NOSIGNAL);
		fprintf(stderr, "-->SEND() %lu %u RET %i\n", i, cs->remaining, ret);
		unsigned keep = 1;
		if(ret <= 0){
			/* Assume client closed his send side as well. */
			hams_parser* hsp = http_server_get_parser((void*)i);
			keep = http_server_output_sent(hsp, 0);
		}
		else{
			cs->remaining -= ret;
			if(0 == cs->remaining){
				/* Sent the data, inform the server */
				hams_parser* hsp = http_server_get_parser((void*)i);
				assert(hsp);
				keep = http_server_output_sent(hsp, 1);
			}
			else{
				/* memmove to make things easy, not efficient. */
				memmove(cs->buffer, cs->buffer + ret, cs->remaining);
			}
		}

		if(!keep){
			/* Prevent next loop from processing fd. */
			fprintf(stderr, "-->STOPPING %lu\n", i);
			pfds[i + 1].revents = 0;
			pfds[i + 1].events = 0;
			conns[i].remaining = 0;
		}
	}

	/* Input loop; */
	for(unsigned long i = 0; i < conn_count; ++i){
		if(!(pfds[i + 1].revents & POLLIN))
			continue;
		int fd = pfds[i + 1].fd;
		uint8_t buffer[RX_BUFF_SIZE];
		int ret = read(fd, buffer, RX_BUFF_SIZE);

		fprintf(stderr, "-->RECV() %lu %i\n", i, ret);

		hams_parser* hsp = http_server_get_parser((void*)i);
		assert(hsp);

		if(ret > 0){
			http_server_input(hsp, buffer, ret);
		}
		else{
			http_server_input(hsp, NULL, 0);
			pfds[i + 1].events &= ~POLLIN;
		}
	}

	/* Allow parsers to use stage buffer. */
	http_server_output_sent(NULL, 0);

	/* Determine which want to transmit. */
	for(unsigned i = 0; i < conn_count; ++i){
		if(conns[i].remaining){
			fprintf(stderr, "-->CONTINUE %u %u\n", i, conns[i].remaining);
			pfds[i + 1].events |= POLLOUT;
		}
		else
			pfds[i + 1].events &= ~POLLOUT;
	}

	unsigned cleanup_count = 0;
	for(unsigned i = 0; i < conn_count; ++i){
		if(!pfds[i + 1].events)
			++cleanup_count;
	}

	/* Now cleanup empyt connections. */
	{
		unsigned long cur = conn_count;
		while(cleanup_count){
			/* Look for cleared connection. */
			if(!pfds[cur].events){
				fprintf(stderr, "__>CLOSING %lu\n", cur);
				close(pfds[cur].fd);
				pfds[cur].fd = -1;
				conns[cur - 1].remaining = 0;
			}
			else{
				/* Find closed connection. */
				unsigned long k = cur - 1;
				while(k && pfds[k].events)
					--k;

				assert(k);
				close(pfds[k].fd);

				/* k must change to conn space. */

				unsigned long dest = k - 1;
				unsigned long src = cur - 1;

				fprintf(stderr, "__>CLOSING %lu, KEEP %lu\n", dest, src);

				/* Reassociate cur index with the k index. */
				fprintf(stderr, "__>REMAPPING %lu -> %lu\n", src, dest);
				memcpy(pfds + k, pfds + cur, sizeof(struct pollfd));
				memcpy(conns + dest, conns + src, sizeof(conns[src]));

				hams_parser* hsp = http_server_get_parser((void*)src);
				http_server_req_state *rs = http_server_get_state(hsp);
				rs->stack_data = (void*)dest;
			}
			--cur;
			--conn_count;
			--cleanup_count;
		}
	}

	/* Now check new connections. */
	if(pfds[0].revents & POLLIN){
		struct sockaddr_storage their_addr;
		unsigned addr_size = sizeof(their_addr);
		int fd = accept(pfds[0].fd, (struct sockaddr*)&their_addr, &addr_size);
		if(fd >= 0){
			if(conn_count < MAX_CONNS){
				/* Associate this fd with a HAM */
				assert(NULL == http_server_get_parser((void*)conn_count));
				if(http_server_alloc_parser((void*)conn_count)){
					/* Initialize connection. */
					pfds[conn_count + 1].fd = fd;
					pfds[conn_count + 1].events = POLLIN;
					conns[conn_count].remaining = 0;
					conns[conn_count].flags = 0;

					fprintf(stderr, "ACCEPTED NEW FD %i, COUNT %lu\n", fd, conn_count);
					++conn_count;
					return 0;
				}
			}

			/* Not enough resources to handle fd, just close it. */
			close(fd);
		}
	}

	return 0;
}

int s_load_pages(const char* filename){
	/* Initialize web pages. */
	http_serverif_clear_memory();

	/* Read wwwbin from file. */
	int fd = open(filename, O_RDONLY | O_CLOEXEC);
	if(-1 == fd)
		return 1;

	/* Get filesize */
	struct stat filestat;
	if(fstat(fd, &filestat)){
		return 1;
	}

	/* Mmap file. */
	void* mdata = mmap(NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if(MAP_FAILED == mdata){
		return 1;
	}
	memcpy(s_web_page_space, mdata, filestat.st_size);
	munmap(mdata, filestat.st_size);
	close(fd);

	return 0;
}

int main(int argc, const char* argv[]){
	struct addrinfo hints, *res;

	/* Initialize web server and load up the pages. */
	http_server* serv = http_server_init(s_send_cb);
	if(s_load_pages(argv[2])){
		fprintf(stderr, "Failed to load pages %s\n", strerror(errno));
		return 1;
	}
	const fw_header* hdr = (const fw_header*)s_web_page_space;
	const fw_www_header* www = (const fw_www_header*)hdr->data;
	http_server_set_pages(www);

	/* Compose hostname */
	char hostname[192] = "localhost:";
	strcat(hostname, argv[1]);
	serv->demux.hostname = hostname;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, argv[1], &hints, &res);

	pfds[0].fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	pfds[0].events = POLLIN;

	if(pfds[0].fd < 0){
		fprintf(stderr, "FD ERROR %s\n", strerror(errno));
		return 1;
	}
		
	if(bind(pfds[0].fd, res->ai_addr, res->ai_addrlen)){
		fprintf(stderr, "BIND ERROR %s\n", strerror(errno));
		return 1;
	}
	 
	if(listen(pfds[0].fd, 20)){
		fprintf(stderr, "LISTEN ERROR %s\n", strerror(errno));
		return 1;
	}

	while(1){
		if(iterate())
			return 1;
	}

	return 0;
}
