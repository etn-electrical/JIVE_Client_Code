/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Define Modbus frame structures
 *
 *	@details
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_DCI_DEFINES_H
#define MODBUS_DCI_DEFINES_H

#include "Modbus_Defines.h"

/*
 *****************************************************************************************
 *****************************************************************************************
 *****************************************************************************************
 *		Modbus Get Message Indexes.
 *****************************************************************************************
 */


// ***********************************************************
// **** Multi-Bit Get Frames
// ***********************************************************
typedef struct
{
	MB_FRAME_STRUCT* frame;

	BF_Lib::SPLIT_UINT16 start_bit;
	BF_Lib::SPLIT_UINT16 num_bits;
	uint8_t attribute;						// Added for attribute retrival
} MB_BIT_GET_STRUCT;

typedef struct
{
	MB_FRAME_STRUCT* frame;

	uint8_t byte_count;
	uint8_t* data;
} MB_BIT_GET_RESP_STRUCT;


// ***********************************************************
// **** Multi-Reg Get Frames

typedef struct
{
	MB_FRAME_STRUCT* frame;

	BF_Lib::SPLIT_UINT16 start_reg;
	BF_Lib::SPLIT_UINT16 num_regs;
	uint8_t attribute;
} MB_REG_GET_STRUCT;

typedef struct
{
	MB_FRAME_STRUCT* frame;

	uint16_t byte_count;
	uint8_t* data;
} MB_REG_GET_RESP_STRUCT;

/*
 *****************************************************************************************
 *****************************************************************************************
 *****************************************************************************************
 *		Modbus Set Message Indexes.
 *****************************************************************************************
 */

// ***********************************************************
// **** Multi-Bit Set Frames

typedef struct
{
	MB_FRAME_STRUCT* frame;

	BF_Lib::SPLIT_UINT16 start_bit;
	BF_Lib::SPLIT_UINT16 num_bits;
	uint8_t attribute;						// Added for attribute retrival
	uint8_t num_bytes;
	uint8_t* data;
} MB_BIT_SET_STRUCT;

typedef struct
{
	MB_FRAME_STRUCT* frame;

	BF_Lib::SPLIT_UINT16 start_bit;
	BF_Lib::SPLIT_UINT16 num_bits;
} MB_BIT_SET_RESP_STRUCT;


// ***********************************************************
// **** Multi-Reg Set Frames

typedef struct
{
	MB_FRAME_STRUCT* frame;

	BF_Lib::SPLIT_UINT16 start_reg;
	BF_Lib::SPLIT_UINT16 num_regs;
	uint8_t attribute;
	uint8_t num_bytes;
	uint8_t* data;
} MB_REG_SET_STRUCT;

typedef struct
{
	MB_FRAME_STRUCT* frame;

	BF_Lib::SPLIT_UINT16 start_reg;
	BF_Lib::SPLIT_UINT16 num_regs;
} MB_REG_SET_RESP_STRUCT;


// ***********************************************************
/// **** Device Identity Request Frame
typedef struct
{
	MB_FRAME_STRUCT* frame;

	uint8_t mei_req;
	uint8_t read_dev_id_code;
	uint8_t obj_id;
	uint16_t max_obj_data_length;
} MB_DEV_ID_REQ_STRUCT;

typedef struct
{
	MB_FRAME_STRUCT* frame;

	uint8_t more_follows;
	uint8_t next_obj_id;
	uint8_t conformity_level;
	uint8_t num_of_objects;
	uint8_t* obj_list_dest;
	uint16_t obj_list_len;
} MB_DEV_ID_RESP_STRUCT;


static const uint8_t MB_DEV_ID_VENDOR_OBJ_NUM = 0U;
static const uint8_t MB_DEV_ID_PROD_CODE_OBJ_NUM = 1U;
static const uint8_t MB_DEV_ID_FW_REV_OBJ_NUM = 2U;
static const uint8_t MB_DEV_ID_VENDOR_URL_OBJ_NUM = 3U;
static const uint8_t MB_DEV_ID_PROD_NAME_OBJ_NUM = 4U;
static const uint8_t MB_DEV_ID_MODEL_NAME_OBJ_NUM = 5U;
static const uint8_t MB_DEV_ID_USER_APP_NAME_OBJ_NUM = 6U;
static const uint8_t MB_DEV_ID_MAX_OBJECTS = 7U;

static const uint8_t MB_DEV_ID_READ_DEV_ID_UNDEFINED = 0U;
static const uint8_t MB_DEV_ID_READ_BASIC_DEV_ID_CODE = 1U;
static const uint8_t MB_DEV_ID_READ_REGULAR_DEV_ID_CODE = 2U;
static const uint8_t MB_DEV_ID_READ_EXTENDED_DEV_ID_CODE = 3U;
static const uint8_t MB_DEV_ID_READ_SINGLE_DEV_ID_CODE = 4U;
static const uint8_t MB_DEV_ID_MAX_READ_DEV_ID_CODE = 5U;


static const uint8_t MB_DEV_ID_OBJ_ID_INDEX = 0U;
static const uint8_t MB_DEV_ID_OBJ_LENGTH_INDEX = 1U;
static const uint8_t MB_DEV_ID_OBJ_DATA_INDEX = 2U;

#endif
