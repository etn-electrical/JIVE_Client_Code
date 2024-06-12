/******************************************************************************/
/*   convert a set of files into hex arrays in a C source file                */
/*   compiled and tested in MS Visual Studio 2005, and GCC 3.4.4              */
/******************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

#define CONFIRM_CHANGE 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fw.h"
#include "md5.h"

#define IS_UPALPHA(c)   ( (c) >= 'A' && (c) <= 'Z' )
#define IS_LOALPHA(c)   ( (c) >= 'a' && (c) <= 'z' )
#define TO_UPPER(c)     ( IS_LOALPHA(c) ? (c) - 32 : (c))
#define TO_LOWER(c)     ( IS_UPALPHA(c) ? (c) + 32 : (c))

#define DEFAULT_OUTPUT_FILE_NAME "webFiles.bin"
#define LIST_FILE_NAME "list.txt"
#define MIMES_FILE_NAME "mimes.txt"

#define MUST_EXIST_COUNT 3

#define WWW_DATA_PADDED(x) ((x + 7) & ~0x7)

static const char* s_must_exist[MUST_EXIST_COUNT] =
	{
		"default.html"
		, "index.html"
		, "not_found.html"
	};

#define MAX_NUM_FILES 1000
#define MAX_NUM_FILENAME_CHAR 100

#define DEFAULT_MIME_TYPE "application/octet-stream"

typedef struct
{
	const char *extension;
	const char *type;
	const unsigned extension_len;
	unsigned index;
} mime_lookup_entry;

mime_lookup_entry mime_table[] =
	{
			{ "bmp", "image/bmp", sizeof ( "bmp" ) - 1 },
			{ "css", "text/css", sizeof ( "css" ) - 1 },
			{ "gif", "image/gif", sizeof ( "gif" ) - 1 },
			{ "htm", "text/html", sizeof ( "htm" ) - 1 },
			{ "html", "text/html", sizeof ( "html" ) - 1 },
			{ "jpe", "image/jpeg", sizeof ( "jpe" ) - 1 },
			{ "jpeg", "image/jpeg", sizeof ( "jpeg" ) - 1 },
			{ "jpg", "image/jpeg", sizeof ( "jpg" ) - 1 },
			{ "js", "text/javascript", sizeof ( "js" ) - 1 },
			{ "png", "image/png", sizeof ( "png" ) - 1 },
			{ "txt", "text/plain", sizeof ( "txt" ) - 1 },
			{ "xml", "application/xml", sizeof ( "xml" ) - 1 },
	};

#define MIME_TABLE_SIZE (sizeof(mime_table) / sizeof(mime_lookup_entry))

FILE *fOut;
FILE *fList;
FILE *fIn;

char fileNames[MAX_NUM_FILES][MAX_NUM_FILENAME_CHAR + 1];
uint32_t mimeTypeSizes[MAX_NUM_FILES];
uint32_t fileNameSizes[MAX_NUM_FILES];
uint32_t fileSizes[MAX_NUM_FILES];

/** @brief Can't have more mime types than files! */
char mimeTypes[MAX_NUM_FILES][MAX_NUM_FILENAME_CHAR + 1];

/* Used for qsort. */
int fileCmp( const void* v1, const void* v2 )
{
	return strcmp( (const char*)v1, (const char*)v2 );
}

int main( int argc, const char *argv[] )
{
	MD5_CTX mctx;
	unsigned i = 0;
	unsigned k = 0;
	unsigned numFiles = 0;
	unsigned numMimes = 0;

	unsigned default_mime_type = MAX_NUM_FILES;

	unsigned total_fw_size = sizeof(fw_header) + sizeof(fw_www_header);

	const char *pFOutName = DEFAULT_OUTPUT_FILE_NAME;
	if ( 1 < argc )
	{
		pFOutName = argv[1];
	}

	// open mime files
	fprintf( stdout, "reading mimes file: %s ...\n", MIMES_FILE_NAME );
	if ( NULL == ( fList = fopen( MIMES_FILE_NAME, "r" ) ) )
	{
		fprintf( stderr, "failed to read mimes file: %s\n", MIMES_FILE_NAME );
		return EXIT_FAILURE;
	}

	/* Initialize indices in mime table. */
	for ( k = 0; k < MIME_TABLE_SIZE; ++k )
		mime_table[k].index = MAX_NUM_FILES;

	for ( i = 0; i < MAX_NUM_FILES; i++ )
	{
		if ( EOF != fscanf( fList, "%s", &mimeTypes[i][0] ) )
		{
			fprintf( stdout, "... %s %u\n", mimeTypes[i], mimeTypeSizes[i] );
			if ( !strcmp( DEFAULT_MIME_TYPE, mimeTypes[i] ) )
				default_mime_type = i;

			/* Update indices in mime_table */
			for ( k = 0; k < MIME_TABLE_SIZE; ++k )
			{
				if ( !strcmp( mime_table[k].type, mimeTypes[i] ) )
				{
					fprintf( stderr, "MATCHING %s to %u\n", mimeTypes[i], k );
					mime_table[k].index = i;
				}
			}

			mimeTypeSizes[i] = strlen( mimeTypes[i] );
			total_fw_size += WWW_DATA_PADDED(mimeTypeSizes[i]);
			total_fw_size += sizeof(fw_data_entry);
			numMimes++;
		}
		else
		{
			break;
		}
	}

	/* Make sure all indices are initialized. */
	for ( k = 0; k < MIME_TABLE_SIZE; ++k )
	{
		if ( MAX_NUM_FILES == mime_table[k].index )
		{
			fprintf( stderr, "mimes.txt is missing %s\n", mime_table[k].type );
			return EXIT_FAILURE;
		}
	}

	fclose( fList );
	fprintf( stdout, "mime file %s read successful\n\n\n", MIMES_FILE_NAME );

	if ( MAX_NUM_FILES == i )
	{
		fprintf( stderr, "mimes.txt is missing application/octet-stream\n" );
		return EXIT_FAILURE;
	}

	// open list file
	fprintf( stdout, "reading list file: %s ...\n", LIST_FILE_NAME );
	if ( NULL == ( fList = fopen( LIST_FILE_NAME, "r" ) ) )
	{
		fprintf( stderr, "failed to read list file: %s\n", LIST_FILE_NAME );
		return EXIT_FAILURE;
	}

	for ( i = 0; i < MAX_NUM_FILES; i++ )
	{
		if ( EOF != fscanf( fList, "%s", &fileNames[i][0] ) )
		{
			fprintf( stdout, "... %s %lu\n", &fileNames[i][0], strlen( fileNames[i] ) );
			numFiles++;
		}
		else
		{
			break;
		}
	}

	fclose( fList );
	fprintf( stdout, "list file %s read successful\n\n\n", LIST_FILE_NAME );

	qsort( fileNames, numFiles, MAX_NUM_FILENAME_CHAR + 1, fileCmp );

	/* Verify that the following files are listed. */
	for ( i = 0; i < MUST_EXIST_COUNT; ++i )
	{
		for ( k = 0; k < numFiles; ++k )
		{
			if ( !strcmp( fileNames[k], s_must_exist[i] ) )
				break;
		}

		if ( k == numFiles )
		{
			fprintf( stderr, "File %s missing from the list!\n", s_must_exist[i] );
			return EXIT_FAILURE;
		}
	}

	// test and see if the files all exist
	for ( i = 0; i < numFiles; i++ )
	{

		fprintf( stdout, "trying to open file: %s ...\n", &fileNames[i][0] );
		if ( NULL == ( fIn = fopen( &fileNames[i][0], "rb" ) ) )
		{
			fprintf( stderr, "failed to open file: %s, exiting ...\n", &fileNames[i][0] );
			return EXIT_FAILURE;
		}

		/* Get the file size, the lazy way. */
		if ( fseek( fIn, 0, SEEK_END ) == 0 ) /* seek was successful */
			fileSizes[i] = ftell( fIn );
		else
		{
			fprintf( stderr, "failed to seek end of file: %s, exiting ...\n", &fileNames[i][0] );
			return EXIT_FAILURE;
		}
		fileNameSizes[i] = strlen( fileNames[i] );
		total_fw_size += WWW_DATA_PADDED(fileNameSizes[i] + 1);
		total_fw_size += sizeof(fw_data_entry);

		/* Calculate padded file_size. */
		total_fw_size += WWW_DATA_PADDED(sizeof(fw_www_file) + fileSizes[i]);
		total_fw_size += sizeof(fw_data_entry);

		fclose( fIn );
		fprintf( stdout, "successfully opened file: %s\n", &fileNames[i][0] );
	}

	fprintf( stdout, "\n\n" );

#if (0 != CONFIRM_CHANGE)
	fprintf(stdout, "open %s for writing\n", pFOutName);
	if (NULL != (fOut = fopen(pFOutName, "r")))
	{
		int c=0;
		fprintf(stdout, "file %s exists, overwrite?(Y/N)", pFOutName);
		c=fgetc(stdin);
		if ('y' != c && 'Y' != c)
		{
			fprintf(stdout, "exiting WITHOUT modifying file %s ...\n", pFOutName);
			fclose(fOut);
			return EXIT_SUCCESS;
		}
		fclose(fOut);
		fprintf(stdout, "overwriting %s ...\n", pFOutName);
	}
#endif

	if ( NULL == ( fOut = fopen( pFOutName, "wb" ) ) )
	{
		fprintf( stderr, "failed to open file %s for write, exiting ...\n", pFOutName );
		return EXIT_FAILURE;
	}

	if ( total_fw_size > 640 * 1024 )
	{
		fprintf( stderr, "FW data size %u exceeds maximum...\n", total_fw_size );
		return EXIT_FAILURE;
	}

	uint8_t* pool = malloc( 1024 * 1024 );
	if ( !pool )
	{
		fprintf( stderr, "Failed to allocate FW memory...\n" );
		return EXIT_FAILURE;
	}

	fw_header* header = (fw_header*) ( pool );
	header->magic_endian = MAGIC_ENDIAN;

	fw_www_header* www_header = (fw_www_header*) ( header->data );
	www_header->mime_type_count = numMimes;
	www_header->file_count = numFiles;
	strcpy( (char*)www_header->language_code, "en" );

	const unsigned total_entries =
		www_header->mime_type_count + 2 * www_header->file_count;

	fw_data_entry* entry = www_header->entries;
	uint8_t* wwwdata = header->data
		+ sizeof(fw_www_header) + sizeof(fw_data_entry) * total_entries;
	const uint8_t* const begin = wwwdata;

	/* Populate mime entries. */
	for ( i = 0; i < www_header->mime_type_count; ++i )
	{
		entry->offset = wwwdata - begin;
		entry->length = mimeTypeSizes[i];
		++entry;

		memcpy( wwwdata, mimeTypes[i], mimeTypeSizes[i] );
		wwwdata += WWW_DATA_PADDED(mimeTypeSizes[i]);
	}

	/* Populate file names. Add a '/' to the start. */
	for ( i = 0; i < www_header->file_count; ++i )
	{
		entry->offset = wwwdata - begin;
		entry->length = fileNameSizes[i] + 1;
		++entry;

		memcpy( wwwdata + 1, fileNames[i], fileNameSizes[i] );
		*wwwdata = '/';
		wwwdata += WWW_DATA_PADDED(fileNameSizes[i] + 1);
	}

	/* Populate file data. */
	for ( i = 0; i < www_header->file_count; ++i )
	{
		entry->offset = wwwdata - begin;
		entry->length = fileSizes[i];
		fprintf( stderr, "OFFSET %u LEN %u\n", entry->offset, entry->length );
		++entry;

		fw_www_file* header = (fw_www_file*)wwwdata;

		/* Find matching mime type index. */
		header->mime_type = default_mime_type;
		const char* ext = strrchr( fileNames[i], '.' );
		if ( ext )
		{
			++ext;
			fprintf( stderr, "EXT %s ", ext );
			if ( strlen( ext ) <= 4 )
			{
				/* Convert extension to lower case. */
				char lower[5];

				for ( k = 0; ext[k]; ++k )
					lower[k] = ext[k] | 0x20;
				lower[k] = '\0';

				fprintf( stderr, "LOWER %s ", lower );
				/* Find in table. */
				for ( k = 0; k < MIME_TABLE_SIZE; ++k )
				{
					const mime_lookup_entry* e = mime_table + k;
					if ( !strcmp( e->extension, lower ) )
					{
						header->mime_type = e->index;
						break;
					}
				}
				fprintf( stderr, "MATCH %u ", header->mime_type );
			}
			else
			{
				fprintf( stderr, "\n" );
			}
		}

		MD5_Init( &mctx );
		int ch;
		uint8_t* out = wwwdata + sizeof(fw_www_file);
		const uint8_t* data_begin = out;
		if ( NULL == ( fIn = fopen( &fileNames[i][0], "rb" ) ) )
		{
			fprintf( stderr, "failed to open file: %s, exiting ...\n", &fileNames[i][0] );
			return EXIT_FAILURE;
		}

		/* Determine encoding. If file begins with 0x1f 0x8 it is probably gzipped. */
		header->encoding = WWW_ENC_NONE;
		if ( fileSizes[i] > 12 )
		{
			int x1, x2;
			x1 = fgetc( fIn );
			x2 = fgetc( fIn );
			if ( x1 == 0x1f && x2 == 0x8b )
				header->encoding = WWW_ENC_GZIP;
			*out = x1;
			++out;
			*out = x2;
			++out;
		}

		while ( EOF != ( ch = fgetc( fIn ) ) )
		{
			*out = ch;
			++out;
		}
		fclose( fIn );

		MD5_Update( &mctx, data_begin, out - data_begin );
		uint8_t digest[16];
		MD5_Final( digest, &mctx );

		/* Convert to hexadecimal. */
		uint8_t* e = header->etag;
		for ( k = 0; k < 16; ++k )
		{
			unsigned char half = digest[k] & 0xF;
			e[0] = half;
			e[0] += ( half <= 9 ) ? '0' : '7';
			half = digest[k] >> 4;
			e[1] = half;
			e[1] += ( half <= 9 ) ? '0' : '7';
			e += 2;
		}

		wwwdata += WWW_DATA_PADDED(sizeof(fw_www_file) + fileSizes[i]);
	}

	header->data_length = (wwwdata - pool) - sizeof(fw_header);

	/* Write out the data. */
	fprintf(stderr, "WRITING %lu bytes\n", wwwdata - pool);
	fwrite((pool + sizeof(fw_header)), 1, ((wwwdata - pool)- sizeof(fw_header)), fOut);

	fclose( fOut );

	return EXIT_SUCCESS;
}
