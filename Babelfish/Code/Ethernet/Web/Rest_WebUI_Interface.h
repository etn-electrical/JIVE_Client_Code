/**
 **********************************************************************************************
 * @file            Rest_WebUI_Interface.h  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product Fw Upgrade.
 *
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef _REST_WEBUI_INTERFACE_H_
#define _REST_WEBUI_INTERFACE_H_

#include "includes.h"
#include "Fw_Store.h"
#include "hams.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define BAD_FILE 0xFFFFU

/** @brief Entry for describing variable length data.
 * All strings, file data, etc are described by an entry. */
typedef struct fw_data_entry_s
{
	uint32_t length;
	uint32_t offset;
} fw_data_entry;

typedef struct fw_www_header_s
{
	/** @brief Web page language code. */
	uint8_t language_code[8];

	/** @brief All possible mime types. */
	uint16_t mime_type_count;

	/** @brief Number of files that can be served. */
	uint16_t file_count;

	/** @brief Total number entries =  mime_type_count + 2 * file_count */
	// fw_data_entry entries[];
} fw_www_header;

typedef struct fw_www_file_s
{
	/** @brief File ETAG, the MD5SUM of the file. */
	uint8_t etag[32];

	/** @brief The file type */
	uint16_t mime_type;

	/** @brief How the file is encoded, ie raw or gzipped or other. */
	uint16_t encoding;

	/** @brief File data. */
	// uint8_t data[];
} fw_www_file;

/** @brief Structure to maintain state between path segment lookups. */
typedef struct fw_www_search_ctx_s
{
	uint16_t low;
	uint16_t high;
	uint32_t str_idx;
} fw_www_search_ctx;

struct real_fw_www_header
{
	struct fw_www_header_s f;
	fw_data_entry entries[4U];	// mimetype_count + ( 2 * fileCount )
};

struct real_fw_www_file
{
	struct fw_www_file_s f;
	uint8_t data[4U];
};

static const uint32_t MIMETYPE_BUFFER_SIZE = 48U;	// current max is 30
static const uint32_t FILENAME_BUFFER_SIZE = 90U;	// current max is 54
static const uint8_t MAX_NUMBER_OF_READS = 10U;		// maximum number of NV read retry count

#ifdef WEBUI_ON_EXT_FLASH
#ifndef WEB_PAGE_READ_BUFF_SIZE
static const uint32_t WEB_PAGE_READ_BUFF_SIZE = 512U;
#endif
#endif

/*
 *****************************************************************************************
 * @ Get Firmware header data
 * @return Header data
 *****************************************************************************************
 */
const fw_www_header* Get_Fw_WWW_Header( NV_Ctrl* web_ui_nv_ctrl );

/*
 *****************************************************************************************
 * @ Calculate number of entries required to load the web pages
 * @ return total number of entries
 *****************************************************************************************
 */

uint32_t Fw_WWW_Total_Entries( const fw_www_header* www );

/*
 *****************************************************************************************
 * @ Assign the starting pointer for the webpages.
 * @ return the address pointer where the webpage start
 *****************************************************************************************
 */

const uint8_t* Fw_WWW_Data_Begin( const fw_www_header* www );

/*
 *****************************************************************************************
 * @ Calculate the total file size .
 * @ Return file size count
 *****************************************************************************************
 */

uint32_t Fw_WWW_Get_Filesize( const fw_www_header* www, uint32_t idx );

/*
 *****************************************************************************************
 * @ Get the file from the memory .
 * @ return file details
 *****************************************************************************************
 */

const fw_www_file* Fw_WWW_Get_File( const fw_www_header* www, uint32_t idx, const uint8_t* www_file_ptr );

/*
 *****************************************************************************************
 * @ Collect the data from the files.
 * @return the data of the files
 *****************************************************************************************
 */
#ifdef WEBUI_ON_EXT_FLASH
const uint8_t* Fw_WWW_Get_File_Data_Chunk( hams_parser* parser, uint32_t address, uint32_t length );

#endif
/*
 *****************************************************************************************
 * @ Get the file data
 * @return the offset of the files
 *****************************************************************************************
 */
const uint8_t* Fw_WWW_Get_File_Data( hams_parser* parser, const fw_www_header* www, uint32_t idx );

/*
 *****************************************************************************************
 * @ Get the name of the file
 * @return size of the file
 *****************************************************************************************
 */

void Fw_WWW_Get_Filename( const fw_www_header* www, uint32_t idx, const uint8_t* filename_ptr );

/*
 *****************************************************************************************
 * @ Get the mimetype count .
 * @return number of possible matches remaining
 *****************************************************************************************
 */

void Fw_WWW_Get_Mimetype( const fw_www_header* www, uint32_t idx, const uint8_t* mime_ptr );

/*
 *****************************************************************************************
 * @ get the total length of mimetype .
 *  @return number of possible matches remaining
 *****************************************************************************************
 */

uint32_t Fw_WWW_Get_Mimetype_Length( const fw_www_header* www, uint32_t idx );

/*
 *****************************************************************************************
 * @brief Get a search context ready for finding a file.
 * @return number of possible matches remaining
 *****************************************************************************************
 */
void Fw_WWW_Init_Search_Ctx( const fw_www_header* www, fw_www_search_ctx* ctx );

/*
 *****************************************************************************************
 * @brief Search for a file, given piece a of the path.
 *  @param www Head of the filesystem
 *  @param path Part of the path
 *  @param pathlen Length of the path
 *  @param ctx Search context; Should be initialized by fw_www_init_search_ctx
 *  before first call
 * @return number of possible matches remaining
 *****************************************************************************************
 */

uint32_t Fw_WWW_Search( const fw_www_header* www, const uint8_t* path, uint32_t pathlen,
						fw_www_search_ctx* ctx );

/*
 *****************************************************************************************
 * @ Get firmware webpage match count.
 * @return number of possible matches remaining
 *****************************************************************************************
 */
uint32_t Fw_WWW_Match_Count( const fw_www_search_ctx* ctx );


#endif	/* _REST_WEBUI_INTERFACE_H_ */
