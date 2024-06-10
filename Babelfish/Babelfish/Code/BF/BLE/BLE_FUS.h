/**
 *****************************************************************************************
 *  @file
 *
 *	@details BLE-FUS interface file help to get the execute BLE operation.
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef _BLE_FUS_H_
#define _BLE_FUS_H_

#include <stdint.h>
#include "Includes.h"
#include "DCI_Defines.h"
#include "BLE_Stack_Interface.h"
#include "FUS_Msg_Structs.h"

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
#define CONST_BLE_FUS_PROTOCOL_REV_MAJOR 0
#define CONST_BLE_FUS_PROTOCOL_REV_MINOR 1

#define  CONST_BLE_FUS_PROTOCOL_VER ( ( CONST_BLE_FUS_PROTOCOL_REV_MINOR << 8 ) & 0XFF00 ) + \
	( ( CONST_BLE_FUS_PROTOCOL_REV_MAJOR ) & 0X00FF )

static const uint8_t MAX_BLE_ERROR_LENGTH = 2U;
static const uint8_t APP_IMAGE_COPY_TIME_MS = 100U;

// As per CFG_BLE_MAX_ATT_MTU of \Babelfish\Code\BF\BLE\STM32_WPAN_BF\app_conf.h
#ifndef MAX_BLE_FUS_FRAME_SIZE
#define MAX_BLE_FUS_FRAME_SIZE 128U
#endif

#ifndef MAX_BLE_FUS_DATA_SIZE
#define MAX_BLE_FUS_DATA_SIZE 112U
#endif
/* BLE firmware upgrade service has two characteristics DCI_BLE_FUS_REQ_FRAME_DCID and DCI_BLE_FUS_RESP_FRAME_DCID.
   DCI_BLE_FUS_REQ_FRAME_DCID is used by client to initiate firmware upgrade requests. DCI_BLE_FUS_RESP_FRAME_DCID is
   used by server to respond back to client. Thus MAX_BLE_RESPONSE_LENGTH should be same as length of
   DCI_BLE_FUS_RESP_FRAME_DCID.
 */
#ifndef MAX_BLE_RESPONSE_LENGTH
#define MAX_BLE_RESPONSE_LENGTH 32U
#endif

#pragma pack(1)	// disable padding

enum ble_status_t
{
	BLE_SUCCESS,
	BLE_BUSY,
	BLE_IN_PROGRESS,
	BLE_REJECT,
	BLE_INVALID,
	BLE_ERROR
};

enum
{
	NO_ENCODE = 0U,
	BASE_64_ENCODE,		// In current implementation we are not supporting Base64.
	MAX_NUMBER_ENCODE
	/* Dont insert any enum after this */
};

//////////// BLE FUS Operations command //////////////////////
enum ble_cmd_t
{
	NO_OPTION,
	PRODUCT_INFO,
	COMPONENT_INFO,
	IMAGE_INFO,
	CREATE_SESSION,
	DELETE_SESSION,
	DATA_WRITE,
	DATA_READ,	// Not implemented
	DATA_VALIDATE,
	DATA_COMMIT,
	FUS_STATUS,
	MAX_CAMMAND
};

enum identifier_type_t
{
	TWO_BYTE,	// 16-bit
	FOUR_BYTE,	// 32-bit
	STRING	// Null terminated string
};

enum validate_status_t
{
	IMAGE_VALIDATION_IN_PROCESS,
	IMAGE_VALID,
	IMAGE_INVALID
};

/* Error Messages */
enum ble_error_t
{
	BAD_COMMAND = 1U,
	ACCESS_DENIED,
	DEVICE_BUSY,
	PREVIOUS_COMMAND_ERROR,
	REQUEST_INVALID,
	INVALID_ENCODING_TYPE,
	INVALID_TIMEOUT,
	INVALID_DATA_LENGTH,
	DATA_WRITE_FAILS,
	SESSION_ID_MISMATCH,
	SESSION_ALREADY_EXISTS,
	SESSION_DOES_NOT_EXISTS,
	SESSION_IN_PROGRESS,
	COMMIT_FAIL,
	DUPLICATE_WRITE_CHUNK
};

struct cmd_resp_t
{
	uint8_t cmd_enum : 5;
	uint8_t wait_bit : 1;
	uint8_t err_bit : 1;
	uint8_t resp_bit : 1;
};

// error frame
struct error_frame_t
{
	cmd_resp_t resp_byte;
	uint8_t err_code;
};

// Product info
struct prod_info_req_t
{
	uint8_t funct_cmd;
};

struct prod_info_resp_t
{
	cmd_resp_t resp_byte;
	uint16_t fus_protcol_ver;
	BF_FUS::img_fw_version_t* firm_ver;
	uint8_t comp_cnt;
	identifier_type_t unique_identifier_type;	// By default its FOUR_BYTE, as unique_identifier does not supported 16
												// bytes and Null Terminated
												// ToDo: Current FUS version only support FOUR_BYTE of unique
												// identifier. Need to add for TWO_BYTE and STRING
	uint32_t unique_identifier;
	uint8_t name[1];
};

union prod_resp_t
{
	prod_info_resp_t prod_info;
	error_frame_t err_frame;
};

// Component Info
struct proc_info_req_t
{
	uint8_t funct_cmd;
	uint8_t proc_id;
};

struct proc_info_resp_t
{
	cmd_resp_t resp_byte;
	uint8_t proc_id;
	uint8_t image_cnt;
	BF_FUS::img_fw_version_t* firm_ver;
	BF_FUS::img_fw_version_t* hardware_ver;
	identifier_type_t unique_identifier_type;	// By default its FOUR_BYTE, as unique_identifier does not supported 16
												// bytes and Null Terminated
												// ToDo: Current FUS version only support FOUR_BYTE of unique
												// identifier. Need to add for TWO_BYTE and STRING
	uint32_t unique_identifier;
	uint8_t name[1];
};

union proc_resp_t
{
	proc_info_resp_t proc_info;
	error_frame_t err_frame;
};

// Firmware Info
struct firm_info_req_t
{
	uint8_t funct_cmd;
	uint8_t comp_id;
	uint8_t image_id;
};

struct firm_info_resp_t
{
	cmd_resp_t resp_byte;
	uint8_t comp_id;
	uint8_t image_id;
	BF_FUS::img_fw_version_t* firm_ver;
	uint16_t compatibility_num;
	uint16_t buffer_size;
	identifier_type_t unique_identifier_type;	// By default its FOUR_BYTE, as unique_identifier does not supported 16
												// bytes and Null Terminated
												// ToDo: Current FUS version only support FOUR_BYTE of unique
												// identifier. Need to add for TWO_BYTE and STRING
	uint32_t unique_identifier;
	uint8_t name[1];
};

union firm_resp_t
{
	firm_info_resp_t firm_info;
	error_frame_t err_frame;
};

// Create Session
struct session_info_req_t
{
	uint8_t funct_cmd;
	uint8_t timeout_time;
	uint8_t encoding_type;
};

struct session_info_t
{
	cmd_resp_t resp_byte;
	uint8_t session_id;
	uint16_t wait_time;
};

union session_resp_t
{
	session_info_t session_info_resp;
	error_frame_t err_frame;
};

// Session Delete
struct session_exit_req_t
{
	uint8_t funct_cmd;
	uint8_t session_id;
};

struct session_exit_resp_t
{
	cmd_resp_t resp_byte;
	uint16_t wait_time;
};

typedef union
{
	session_exit_resp_t session_exit;
	error_frame_t err_frame;
} session_exit_t;

// Write
struct write_req_t
{
	uint8_t funct_cmd;
	uint8_t session_id;
	uint8_t comp_id;
	uint8_t image_id;
	uint32_t address;
	uint16_t data_len;
	uint8_t data[1];
};

struct ble_write_resp_t
{
	cmd_resp_t resp_byte;
	uint8_t session_id;
	uint8_t comp_id;
	uint8_t image_id;
	uint32_t address;
	uint16_t data_len;
	uint16_t wait_time;
};

union write_resp_t
{
	ble_write_resp_t ble_write_resp;
	error_frame_t err_frame;
};

// Validate
struct image_validate_req_t
{
	uint8_t funct_cmd;
	uint8_t session_id;
	uint8_t comp_index;
	uint8_t image_index;
};

struct validate_resp_t
{
	cmd_resp_t resp_byte;
	uint8_t comp_index;
	uint8_t image_index;
	uint8_t status;
	uint16_t op_time;
};

union image_validate_t
{
	validate_resp_t image_validate;
	error_frame_t err_frame;
};


// Commit Image Command

enum  commit_status_t
{
	COMMIT_IN_PROCESS,
	COMMIT_SUCCESSFUL,
	COMMIT_FAILED
};

struct commit_req_t
{
	uint8_t funct_cmd;
	uint8_t session_id;
	uint8_t comp_index;
	uint8_t image_index;
};

struct commit_resp_t
{
	cmd_resp_t resp_byte;
	uint8_t comp_index;
	uint8_t image_index;
	commit_status_t status;
	uint16_t op_time;
};

union image_commit_t
{
	commit_resp_t image_commit;
	error_frame_t err_frame;
};


// Status Check Command

struct image_status_check_req_t
{
	uint8_t funct_cmd;
	uint8_t session_id;
	uint8_t comp_index;
	uint8_t image_index;
};

struct image_status_check_resp_t
{
	cmd_resp_t resp_byte;
	uint8_t comp_index;
	uint8_t image_index;
	uint8_t status;
	uint16_t op_time;
};

union image_status_check_t
{
	image_status_check_resp_t image_status_check;
	error_frame_t err_frame;
};

/**
 * @brief	This function will clear data created for the bootup validation
 */
void Clear_Bootup_validation_Data( void );

/**
 * @brief	BLE FUS request dci callback
 * @param[in] handle   			DCID handle
 * @param[in] access_struct		Access structure of DCID
 */
DCI_CB_RET_TD BLE_FUS_Event( DCI_CBACK_PARAM_TD handle, DCI_ACCESS_ST_TD* access_struct );

/**
 * @brief	This function updates BLE stack interface handle
 * @param[in] handle	BLE stack interface handle
 * @param[in] ble_fus_resp_frame BLE FUS response frame DCI owner
 */
void Update_BLE_Stack_Interface_Handle( BLE_Stack_Interface* handle, const DCI_Owner* ble_fus_resp_frame );

/**
 * @brief	This function will parse request packet and create a response frame
 * @param[in] req_data			BLE FUS request frame
 * @param[in] req_data_len		Length
 */
void BLE_FUS_Interface_Request_Handler( uint8_t* req_data, uint8_t req_data_len );

/**
 * @brief	This function will send response frame to FUS client through notification
 * @param[in] rsp_data			BLE FUS response handler
 * @param[in] rsp_data_len		Length of the response
 */
void BLE_FUS_Interface_Send_Response( uint8_t* rsp_data, uint8_t rsp_data_len );


#endif  /*BLE_FUS_H */
