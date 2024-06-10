/**
 *****************************************************************************************
 *  @file

 *	@brief Firmware upgrade service message structures.
 *
 *	@details This file will provide data structures required for firmware upgrade service.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef FUS_MSG_STRUCTS_H
#define FUS_MSG_STRUCTS_H

#include "FUS_Defines.h"
#include "FUS_Config.h"

namespace BF_FUS
{

/*
 *****************************************************************************************
 *		Message Passing Structures
 *****************************************************************************************
 */
/**
   @brief Structure used to get the Specific events from the images
 */
typedef struct
{
	uint8_t comp_id;
	uint8_t image_id;
	fus_event_t event;
} event_t;

typedef op_status_t (* event_cb_t)( event_t* status );


/**
   @brief Structure used to get the Product information
 */
typedef struct
{
	uint32_t boot_version;
	uint32_t boot_spec_version;
	uint32_t serial_num;
	uint16_t fus_version;
	uint16_t code;
	uint16_t guid;
	uint8_t component_count;
	const uint8_t* name;
	uint8_t name_len;

} prod_info_t;

/**
   @brief Structure used pass the boot version and serial no. to the product control
 */

typedef struct
{
	uint32_t boot_version;
	uint32_t boot_spec_version;
	uint32_t serial_num;
} prod_config_t;

/**
   @brief Structure used to pass session id and timeout to the product control
 */
typedef struct
{
	uint32_t timeout_sec;
	uint32_t session_id;
	fus_if_t interface;
} session_info_req_t;

/**
   @brief Structure used to pass wait time for session creation
 */
typedef struct
{
	uint32_t wait_time;
} session_info_resp_t;

/**
   @brief Structure used to get information of passed comp_id
 */
typedef struct
{
	uint8_t comp_id;
} comp_info_req_t;

/**
   @brief Structure used to get component specific information
 */
typedef struct
{
	uint32_t firmware_version;
	uint32_t hardware_version;
	uint32_t serial_num;
	uint16_t code;
	uint16_t guid;
	uint8_t image_count;
	const uint8_t* name;
	uint8_t name_len;
} comp_info_resp_t;

/**
   @brief Structure used to get information of passed comp_id and image id
 */
typedef struct
{
	uint8_t proc_id;
	uint8_t image_id;
} image_info_req_t;

/**
   @brief Structure used to get image specific information
 */
typedef struct
{
	uint32_t version;
	uint16_t guid;
	uint16_t compatibility_num;
	uint8_t name_len;
	uint8_t* name;
} image_info_resp_t;

/**
   @brief Structure for the required thing to write data into memory
 */
typedef struct
{
	uint32_t address;
	uint32_t session_id;
	uint16_t data_len;
	uint8_t proc_index;
	uint8_t image_index;
	uint8_t* data;
} image_write_req_t;

/**
   @brief Structure of write with wait time
 */
typedef struct
{
	uint32_t op_time;
	uint8_t response_code;
} image_write_resp_t;
/**
   @brief Structure for the required thing to commit data to the memory
 */
typedef struct
{
	uint32_t session_id;
	uint8_t comp_id;
	uint8_t image_id;
} image_commit_req_t;
/**
   @brief Structure of commit wait time
 */
typedef struct
{
	uint32_t op_time;
} image_commit_resp_t;

/**
   @brief Structure for the required thing to Read data to the memory
 */
typedef struct
{
	uint32_t address;
	uint16_t data_len;
	uint8_t proc_index;
	uint8_t image_index;
} image_read_req_t;
/**
   @brief Structure of read wait time
 */
typedef struct
{
	uint32_t op_time;
	uint16_t data_len;
	uint8_t* data;
} image_read_resp_t;

/**
   @brief Structure for the required thing to status request
 */
typedef struct
{
	uint8_t proc_index;
	uint8_t image_index;
} image_status_req_t;
/**
   @brief Structure of status request with wait time and status
 */
typedef struct
{
	uint32_t remaining_op_time;
	uint8_t status;
} image_status_resp_t;

/**
   @brief Structure for the required thing to Validate data of the memory
 */
typedef struct
{
	uint8_t comp_index;
	uint8_t image_index;
} image_validate_req_t;
/**
   @brief Structure of validate wait time and status
 */
typedef struct
{
	uint32_t op_time;
	fus_op_status_t status;
} image_validate_resp_t;

/**
   @brief Structure to separate firmware version
 */
typedef struct
{
	uint8_t major;
	uint8_t minor;
	uint16_t build;
} img_fw_version_t;
}
#endif
