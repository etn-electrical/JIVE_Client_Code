#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fw.h"
#include "md5.h"

int main(int argc, const char *argv[]){
	/* Read wwwbin from file. */
	int fd = open(argv[1], O_RDONLY | O_CLOEXEC);
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

	const fw_header* top = (fw_header*)mdata;

	fprintf(stderr, "MAGIC %u, DATA %u, CRC %hu\n"
			,top->magic_endian, top->data_length, top->crc_checksum);

	if(top->magic_endian != MAGIC_ENDIAN){
		fprintf(stderr, "BAD MAGIC\n");
		return 1;
	}

	const fw_www_header* www = (const fw_www_header*)(top->data);
	fprintf(stderr, "WWW MIME CNT %u, FILE CNT %u\n"
		,www->mime_type_count, www->file_count);

	const unsigned total_entries = fw_www_total_entries(www);
	const uint8_t* begin = fw_www_data_begin(www);

	const fw_data_entry* e = www->entries;
	for(unsigned i = 0; i < total_entries; ++i)
		fprintf(stderr, "ENTRY LENGTH %u, OFFSET %u\n", e[i].length, e[i].offset);

	fprintf(stderr, "MIME TYPES\n");
	for(unsigned i = 0; i < www->mime_type_count; ++i)
		fprintf(stderr, "%.*s\n", e[i].length, begin + e[i].offset);
	e += www->mime_type_count;

	fprintf(stderr, "FILE NAMES\n");
	for(unsigned i = 0; i < www->file_count; ++i)
		fprintf(stderr, "%03u %.*s\n", i, e[i].length, begin + e[i].offset);
	e += www->file_count;

	fprintf(stderr, "FILE METADATA\n");
	for(unsigned i = 0; i < www->file_count; ++i){
		const fw_www_file* hdr = (const fw_www_file*)(begin + e[i].offset);
		fprintf(stderr, "%03u Size %8u   Mime %2u  Enc %2u    ETAG %.*s\n"
			,i ,e[i].length, hdr->mime_type, hdr->encoding, 32, hdr->etag);

		/* Sanity checks. */
		if(hdr->mime_type > www->mime_type_count){
			fprintf(stderr, "INVALID MIME TYPE!\n");
			return 1;
		}

		if(hdr->encoding > COUNT_WWW_ENC){
			fprintf(stderr, "INVALID ENCODING!\n");
			return 1;
		}
	}

	return 0;
}
