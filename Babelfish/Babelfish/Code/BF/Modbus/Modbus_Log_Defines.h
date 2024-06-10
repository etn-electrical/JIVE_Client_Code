/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_LOG_DEFINES_H
   #define MODBUS_LOG_DEFINES_H
/********************************LogSupport Start*******************/
// RTU Multi-Reg Get Frame Index for Log Query

static const uint8_t MB_MREG_LOG_CMD = 2U;
static const uint8_t MB_MREG_LOG_ID = 3U;
static const uint8_t MB_MREG_LOG_ERRORCODE = 3U;

static const uint8_t MB_MREG_REG_STARTINDEX3 = 4U;
static const uint8_t MB_MREG_REG_STARTINDEX2 = 5U;
static const uint8_t MB_MREG_REG_STARTINDEX1 = 6U;
static const uint8_t MB_MREG_REG_STARTINDEX0 = 7U;

static const uint8_t MB_MREG_REG_DATALENTH3 = 8U;
static const uint8_t MB_MREG_REG_DATALENTH2 = 9U;
static const uint8_t MB_MREG_REG_DATALENTH1 = 10U;
static const uint8_t MB_MREG_REG_DATALENTH0 = 11U;

// RTU Multi-Reg Get Response Frame Index for LogData Response

static const uint8_t MB_MREG_LOGRESP_NXT_IND3 = 4U;
static const uint8_t MB_MREG_LOGRESP_NXT_IND2 = 5U;
static const uint8_t MB_MREG_LOGRESP_NXT_IND1 = 6U;
static const uint8_t MB_MREG_LOGRESP_NXT_IND0 = 7U;

static const uint8_t MB_MREG_LOGRESP_REMAINING_IND3 = 12U;
static const uint8_t MB_MREG_LOGRESP_REMAINING_IND2 = 13U;
static const uint8_t MB_MREG_LOGRESP_REMAINING_IND1 = 14U;
static const uint8_t MB_MREG_LOGRESP_REMAINING_IND0 = 15U;

static const uint8_t MB_MREG_LOGRESP_DELETED_IND3 = 4U;
static const uint8_t MB_MREG_LOGRESP_DELETED_IND2 = 5U;
static const uint8_t MB_MREG_LOGRESP_DELETED_IND1 = 6U;
static const uint8_t MB_MREG_LOGRESP_DELETED_IND0 = 7U;


static const uint8_t MB_MREG_LOGRESP_DATA = 16U;
static const uint8_t MB_MREG_CLEAR_LOGRESP_DATASIZE = 6U;

static const uint8_t MB_MREG_CLEAR_LOGRESP_ERR_DATASIZE = 3U;
static const uint8_t MB_MREG_RESP_HEADER_SIZE = 2U;	// Device Id and Fun_Code
static const uint8_t MB_MREG_LOGRESP_CMD_IDSIZE = 2U;// cmd & log id size
static const uint8_t MB_MREG_LOGRESP_NXT_INDSIZE = 4U;
static const uint8_t MB_MREG_LOGRESP_DATA_INDSIZE = 4U;
static const uint8_t MB_MREG_LOGRESP_REMAINING_INDSIZE = 4U;
static const uint8_t MB_MREG_RESP_CRC_SIZE = 2U;
static const uint8_t MB_MREG_DELETE_RESP_SIZE = 4U;
static const uint8_t MB_MREG_ERROR_LOG_CMD = 1U;
static const uint8_t MB_MREG_ERROR_CODE_SIZE = 1U;
static const uint8_t MB_MREG_ERROR_DATA_SIZE = 4U;

static const uint8_t MB_MREG_ERROR_DATA_TIME1 = 4U;
static const uint8_t MB_MREG_ERROR_DATA_TIME2 = 5U;
static const uint8_t MB_MREG_ERROR_DATA_TIME3 = 6U;
static const uint8_t MB_MREG_ERROR_DATA_TIME4 = 7U;

/*	(1)Device_Addr + (1)Fun_Code + (1)Log_Cmd + (1)Log_Id +
    (4)Next_Start + (4)Resp_Length + (4)Remaining + (2)CRC = 18U*/
static const uint16_t MODBUS_LOG_HEADER_SIZE = MB_MREG_RESP_HEADER_SIZE + MB_MREG_LOGRESP_CMD_IDSIZE +
	MB_MREG_LOGRESP_NXT_INDSIZE + MB_MREG_LOGRESP_DATA_INDSIZE + MB_MREG_LOGRESP_REMAINING_INDSIZE +
	MB_MREG_RESP_CRC_SIZE;

/* 2 byte CRC	is already substracted from m_buffer_size*/

/********************************LogSupportEnd*******************/

// **** Multi-Reg Get Frames
typedef struct
{
	MB_FRAME_STRUCT* frame;
	BF_Lib::SPLIT_UINT32 start_index;
	BF_Lib::SPLIT_UINT32 num_regs;
} MBLOG_REG_GET_STRUCT;

typedef struct
{
	MB_FRAME_STRUCT* frame;
	BF_Lib::SPLIT_UINT32 next_index;
	BF_Lib::SPLIT_UINT32 byte_count;
	BF_Lib::SPLIT_UINT32 remaining_entries;
	uint8_t* data;
} MBLOG_REG_GET_RESP_STRUCT;


// TO Clear Log
typedef struct
{
	MB_FRAME_STRUCT* frame;
	BF_Lib::SPLIT_UINT32 entry_count;
} MBLOG_CLEAR_GET_STRUCT;

typedef struct
{
	MB_FRAME_STRUCT* frame;
	BF_Lib::SPLIT_UINT32 num_deleted;
} MBLOG_CLEAR_RESP_STRUCT;

/********************************LogSupport END*******************/
#endif
