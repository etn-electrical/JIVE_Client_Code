#ifndef __BCM_FILESYSTEM_UPLOAD_H__
#define __BCM_FILESYSTEM_UPLOAD_H__

#include <stdint.h>

enum {
	FW_TYPE_APPLICATION
	,FW_TYPE_WWW
	,FW_TYPE_METER
};

enum {
	WWW_ENC_NONE
	,WWW_ENC_GZIP
	,COUNT_WWW_ENC
};

/* Take from UTF-32 Byte Order Mark (BOM) character. */
#define MAGIC_ENDIAN 0x0FEFF
#define DATA_START 0x6e65 /* en */

/*********************** BCM headers/Structures Start *****************************/
typedef struct file_version_s{
	uint8_t major;
	uint8_t minor;
	uint8_t rev;
	uint8_t fw_type;
} fw_file_version;

typedef struct fw_header_s {
	/** @brief Magic number header; also helps define endianness. */
	uint32_t magic_endian;

	/** @brief Version of this FW file. */
	fw_file_version version;

	/** @brief If WWW or METER, the version with which is intended to pair
	 * If APPLICATION_ZIP, could this be the minimum FW version to replace? */
	fw_file_version target_fw_version;

	/** @brief How much data is in the FW. */
	uint32_t data_length;

	/** @brief Product for which FW is intended. Note-aligned to 8 bytes. */
	uint8_t product_code[32];

	/** @brief Payload data. */
	uint8_t *data;
} fw_header;

/** @brief Entry for describing variable length data.
 * All strings, file data, etc are described by an entry. */
typedef struct fw_data_entry_s {
	uint32_t length;
	uint32_t offset;
} fw_data_entry;

typedef struct fw_www_header_s {
	/** @brief Web page language code. */
	uint8_t language_code[8];

	/** @brief All possible mime types. */
	uint16_t mime_type_count;

	/** @brief Number of files that can be served. */
	uint16_t file_count;

	/** @brief Total number entries =  mime_type_count + 2 * file_count */
	fw_data_entry entries[];

} fw_www_header;

typedef struct fw_www_file_s {
	/** @brief File ETAG, the MD5SUM of the file. */
	uint8_t etag[32];

	/** @brief The file type */
	uint16_t mime_type;

	/** @brief How the file is encoded, ie raw or gzipped or other. */
	uint16_t encoding;

	/** @brief File data. */
	uint8_t data[];
} fw_www_file;

/*********************** BCM headers/Structures End *****************************/

/*********************** LO Toolkit headers/Structures Start *****************************/
/* Note the below structure should not exceed size limit of 128bytes */
typedef struct prodinfo_for_boot_s_ltk
{
	uint32_t serial_number_nvadd;
	uint32_t hardware_rev_nvadd;
	uint32_t rtu_slave_nvadd;
	uint32_t rtu_baud_rate_nvadd;
	uint32_t rtu_parity_type_nvadd;
	uint8_t reserved[108];
} prodinfo_for_boot_s_ltk;

typedef struct fw_header_s_ltk
{
	/** @brief Magic number header; also helps define endianness. */
	uint32_t magic_endian; //@Done

	/** @brief How much data is in the FW. */
	uint32_t data_length; //@Done

	/** @brief nv address for boot loader to receive message from application. */
	uint32_t msg_for_boot_nvadd; //@Done

	/** @brief Version of this header format; right now only one is defined.
	 * Any field past this one is defined by this format.*/
	uint16_t header_format_version;

	/** @brief fw image id. */
	uint16_t image_id; //@Done

	/** @brief Version of this FW file. */
	uint8_t version_info[16];

	/** @brief Product for which FW is intended. Note-aligned to 8 bytes. */
	uint8_t product_code[32];
	
	/** @brief fw compatibility number */
	uint16_t compatibility_num;
	/** @brief Reserved header space */
	// total header size(including crc at start) becomes 128 bytes.
	uint8_t reserved[58];
} fw_header_ltk;

typedef struct crc_info_s_ltk
{
	uint16_t crc;
	uint16_t reserved;
} crc_info_ltk;

typedef struct real_fw_header_s_ltk
{
	crc_info_ltk fw_crc;
	fw_header_ltk f;
	prodinfo_for_boot_s_ltk prod;
/** @brief Payload data. */
//uint8_t data[4];
} real_fw_header_ltk;

typedef struct fw_www_header_s_ltk
{
	/** @brief Web page language code. */
	uint8_t language_code[8];

	/** @brief All possible mime types. */
	uint16_t mime_type_count;

	/** @brief Number of files that can be served. */
	uint16_t file_count;

/** @brief Total number entries =  mime_type_count + 2 * file_count */
//fw_data_entry entries[];
} fw_www_header_ltk;

typedef struct fw_www_file_s_ltk
{
	/** @brief File ETAG, the MD5SUM of the file. */
	uint8_t etag[32];

	/** @brief The file type */
	uint16_t mime_type;

	/** @brief How the file is encoded, ie raw or gzipped or other. */
	uint16_t encoding;

/** @brief File data. */
//uint8_t data[];
} fw_www_file_ltk;

/** @brief Structure to maintain state between path segment lookups. */
typedef struct fw_www_search_ctx_s_ltk
{
	uint16_t low;
	uint16_t high;
	uint32_t str_idx;
} fw_www_search_ctx_ltk;

/*********************** LO Toolkit headers/Structures End *****************************/









/** @brief Structure to maintain state between path segment lookups. */
typedef struct fw_www_search_ctx_s {
	uint16_t low;
	uint16_t high;
	unsigned str_idx;
} fw_www_search_ctx;

static inline unsigned fw_www_total_entries(const fw_www_header* www){
	return www->mime_type_count + 2 * www->file_count;
}

static inline const uint8_t* fw_www_data_begin(const fw_www_header* www){
	const uint8_t* x = (const uint8_t*)www;
	x -= sizeof(fw_header);
	const fw_header* fw = (const fw_header*)x;

	return fw->data + sizeof(fw_www_header)
		+ sizeof(fw_data_entry) * fw_www_total_entries(www);
}

static inline unsigned fw_www_get_filesize(const fw_www_header* www
	,unsigned idx)
{
	return www->entries[www->mime_type_count+ www->file_count + idx].length;
}

static inline const fw_www_file* fw_www_get_file(const fw_www_header* www
	,unsigned idx)
{
	return (const fw_www_file*)(fw_www_data_begin(www)
		+ www->entries[www->mime_type_count + www->file_count + idx].offset);
}

static inline const char* fw_www_get_filename(const fw_www_header* www
	,unsigned idx)
{
	return (const char*)(fw_www_data_begin(www)
			+ www->entries[www->mime_type_count + idx].offset);
}

static inline const char* fw_www_get_mimetype(const fw_www_header* www
	,unsigned idx)
{
	return (const char*)(fw_www_data_begin(www) + www->entries[idx].offset);
}

static inline unsigned fw_www_get_mimetype_length(const fw_www_header* www
	,unsigned idx)
{
	return www->entries[idx].length;
}

/** @brief Get a search context ready for finding a file. */
void fw_www_init_search_ctx(const fw_www_header* www, fw_www_search_ctx* ctx);

/** @brief Search for a file, given piece a of the path.
 *  @param www Head of the filesystem
 *  @param path Part of the path
 *  @param pathlen Length of the path
 *  @param ctx Search context; Should be initialized by fw_www_init_search_ctx
 *  before first call
 * @return number of possible matches remaining */
unsigned fw_www_search(const fw_www_header* www, const char* path, unsigned pathlen,
	fw_www_search_ctx* ctx);

/** @brief  */
unsigned fw_www_match_count(const fw_www_search_ctx* ctx);

#endif
