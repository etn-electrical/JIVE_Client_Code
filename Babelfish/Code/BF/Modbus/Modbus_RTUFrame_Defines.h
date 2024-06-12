/**
 *****************************************************************************************
 *	@file
 *
 *	@brief define Modbus RTU frame
 *
 *	@details
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_RTUFRAME_DEFINES_H
   #define MODBUS_RTUFRAME_DEFINES_H


// ***********************************************************
// **** Generic Frame Structure

// Generic RTU Frame Index
static const uint8_t MB_RTU_FRAME_ADDRESS_IND = 0U;
static const uint8_t MB_RTU_FRAME_FUNC_CODE_IND = 1U;
static const uint8_t MB_RTU_FRAME_DATA_IND = 2U;

// Location of exception response value.
static const uint8_t MB_RTU_FRAME_EXCEPTION_IND = MB_RTU_FRAME_DATA_IND;
static const uint8_t MB_RTU_FRAME_EXCEPTION_HEADER_SIZE = MB_RTU_FRAME_DATA_IND + 1U;
// Minimum RTU Frame Size.
static const uint8_t MB_RTU_FRAME_FUNC_CODE_HEADER_SIZE = MB_RTU_FRAME_DATA_IND;


/*
 *****************************************************************************************
 *****************************************************************************************
 *****************************************************************************************
 *		Modbus Get Message Indexes.
 *****************************************************************************************
 */


// ***********************************************************
// **** Get Exception Frames
// ***********************************************************
// RTU Multi-Reg Get Frame Index
static const uint8_t MB_EXCEPTION_GET_ADDRESS_IND = 0U;
static const uint8_t MB_EXCEPTION_GET_FUNC_CODE_IND = 1U;

static const uint8_t MB_EXCEPTION_GET_HEADER_SIZE = 2U;		// Size is sans the CRC.


// RTU Multi-Reg Get Response Frame Index
static const uint8_t MB_EXCEPTION_GET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_EXCEPTION_GET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_EXCEPTION_GET_RESP_DATA_IND = 2U;

static const uint8_t MB_EXCEPTION_GET_RESP_HEADER_SIZE = 3U;	// Size is sans the CRC.



// ***********************************************************
// **** Multi-Bit Get Frames
// ***********************************************************
// RTU Multi-Bit Get Frame Index
static const uint8_t MB_MBIT_GET_ADDRESS_IND = 0U;
static const uint8_t MB_MBIT_GET_FUNC_CODE_IND = 1U;
static const uint8_t MB_MBIT_GET_BIT_NUM_HIGH_IND = 2U;
static const uint8_t MB_MBIT_GET_BIT_NUM_LOW_IND = 3U;
static const uint8_t MB_MBIT_GET_NUM_BITS_HIGH_IND = 4U;
static const uint8_t MB_MBIT_GET_NUM_BITS_LOW_IND = 5U;

static const uint8_t MB_MBIT_GET_HEADER_SIZE = 6U;	// Size is sans the CRC.

// RTU Multi-Bit Get Response Frame Index
static const uint8_t MB_MBIT_GET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_MBIT_GET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_MBIT_GET_RESP_BYTE_NUM_IND = 2U;
static const uint8_t MB_MBIT_GET_RESP_DATA = 3U;

static const uint8_t MB_MBIT_GET_RESP_HEADER_SIZE = 3U;	// Size is sans the CRC.


// ***********************************************************
// **** Multi-Reg Get Frames
// RTU Multi-Reg Get Frame Index
static const uint8_t MB_MREG_GET_ADDRESS_IND = 0U;
static const uint8_t MB_MREG_GET_FUNC_CODE_IND = 1U;
static const uint8_t MB_MREG_GET_REG_NUM_HIGH_IND = 2U;
static const uint8_t MB_MREG_GET_REG_NUM_LOW_IND = 3U;
static const uint8_t MB_MREG_GET_NUM_REGS_HIGH_IND = 4U;
static const uint8_t MB_MREG_GET_NUM_REGS_LOW_IND = 5U;

static const uint8_t MB_MREG_GET_HEADER_SIZE = 6U;	// Size is sans the CRC.

// RTU Multi-Reg Get Response Frame Index
static const uint8_t MB_MREG_GET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_MREG_GET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_MREG_GET_RESP_BYTE_NUM_IND = 2U;
static const uint8_t MB_MREG_GET_RESP_DATA = 3U;

static const uint8_t MB_MREG_GET_RESP_HEADER_SIZE = 3U;	// Size is sans the CRC.


/*
 *****************************************************************************************
 *****************************************************************************************
 *****************************************************************************************
 *		Modbus Set Message Indexes.
 *****************************************************************************************
 */

// ***********************************************************
// **** Single-Bit Set Frames

// RTU Single-Bit Set Frame Index
static const uint8_t MB_SBIT_SET_ADDRESS_IND = 0U;
static const uint8_t MB_SBIT_SET_FUNC_CODE_IND = 1U;
static const uint8_t MB_SBIT_SET_BIT_NUM_HIGH_IND = 2U;
static const uint8_t MB_SBIT_SET_BIT_NUM_LOW_IND = 3U;
static const uint8_t MB_SBIT_SET_BIT_VAL_HIGH_IND = 4U;
static const uint8_t MB_SBIT_SET_BIT_VAL_LOW_IND = 5U;

static const uint8_t MB_SBIT_SET_HEADER_SIZE = 6U;	// Size is sans the CRC.

// RTU Single-Bit Set Response Frame Index
static const uint8_t MB_SBIT_SET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_SBIT_SET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_SBIT_SET_RESP_BIT_NUM_HIGH_IND = 2U;
static const uint8_t MB_SBIT_SET_RESP_BIT_NUM_LOW_IND = 3U;
static const uint8_t MB_SBIT_SET_RESP_BIT_VAL_HIGH_IND = 4U;
static const uint8_t MB_SBIT_SET_RESP_BIT_VAL_LOW_IND = 5U;

static const uint8_t MB_SBIT_SET_RESP_HEADER_SIZE = 6U;	// Size is sans the CRC.

static const uint16_t SET_SINGLE_COIL_TRUE_VAL = 0xFF00U;
static const uint16_t SET_SINGLE_COIL_FALSE_VAL = 0x0000U;


// ***********************************************************
// **** Single-Reg Set Frames

// RTU Single-Reg Set Frame Index
static const uint8_t MB_SREG_SET_ADDRESS_IND = 0U;
static const uint8_t MB_SREG_SET_FUNC_CODE_IND = 1U;
static const uint8_t MB_SREG_SET_REG_NUM_HIGH_IND = 2U;
static const uint8_t MB_SREG_SET_REG_NUM_LOW_IND = 3U;
static const uint8_t MB_SREG_SET_REG_VAL_HIGH_IND = 4U;
static const uint8_t MB_SREG_SET_REG_VAL_LOW_IND = 5U;

static const uint8_t MB_SREG_SET_HEADER_SIZE = 6U;	// Size is sans the CRC.

// RTU Single-Reg Set Response Frame Index
static const uint8_t MB_SREG_SET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_SREG_SET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_SREG_SET_RESP_REG_NUM_HIGH_IND = 2U;
static const uint8_t MB_SREG_SET_RESP_REG_NUM_LOW_IND = 3U;
static const uint8_t MB_SREG_SET_RESP_REG_VAL_HIGH_IND = 4U;
static const uint8_t MB_SREG_SET_RESP_REG_VAL_LOW_IND = 5U;

static const uint8_t MB_SREG_SET_RESP_HEADER_SIZE = 6U;	// Size is sans the CRC.


// ***********************************************************
// **** Multi-Bit Set Frames
// RTU Multi-Bit Set Frame Index
static const uint8_t MB_MBIT_SET_ADDRESS_IND = 0U;
static const uint8_t MB_MBIT_SET_FUNC_CODE_IND = 1U;
static const uint8_t MB_MBIT_SET_BIT_NUM_HIGH_IND = 2U;
static const uint8_t MB_MBIT_SET_BIT_NUM_LOW_IND = 3U;
static const uint8_t MB_MBIT_SET_NUM_BITS_HIGH_IND = 4U;
static const uint8_t MB_MBIT_SET_NUM_BITS_LOW_IND = 5U;
static const uint8_t MB_MBIT_SET_NUM_BYTES_IND = 6U;
static const uint8_t MB_MBIT_SET_BIT_VALS_IND = 7U;

static const uint8_t MB_MBIT_SET_HEADER_SIZE = 7U;	// Size is sans the CRC.

// RTU Multi-Bit Set Response Frame Index
static const uint8_t MB_MBIT_SET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_MBIT_SET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_MBIT_SET_RESP_BIT_NUM_HIGH_IND = 2U;
static const uint8_t MB_MBIT_SET_RESP_BIT_NUM_LOW_IND = 3U;
static const uint8_t MB_MBIT_SET_RESP_NUM_BITS_HIGH_IND = 4U;
static const uint8_t MB_MBIT_SET_RESP_NUM_BITS_LOW_IND = 5U;

static const uint8_t MB_MBIT_SET_RESP_HEADER_SIZE = 6U;	// Size is sans the CRC.

// ***********************************************************
// **** Multi-Reg Set Frames
// RTU Multi-Reg Set Frame Index
static const uint8_t MB_MREG_SET_ADDRESS_IND = 0U;
static const uint8_t MB_MREG_SET_FUNC_CODE_IND = 1U;
static const uint8_t MB_MREG_SET_REG_NUM_HIGH_IND = 2U;
static const uint8_t MB_MREG_SET_REG_NUM_LOW_IND = 3U;
static const uint8_t MB_MREG_SET_NUM_REGS_HIGH_IND = 4U;
static const uint8_t MB_MREG_SET_NUM_REGS_LOW_IND = 5U;
static const uint8_t MB_MREG_SET_NUM_BYTES_IND = 6U;
static const uint8_t MB_MREG_SET_REG_VALS_IND = 7U;

static const uint8_t MB_MREG_SET_HEADER_SIZE = 7U;	// Size is sans the CRC.

// RTU Multi-Reg Set Response Frame Index
static const uint8_t MB_MREG_SET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_MREG_SET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_MREG_SET_RESP_REG_NUM_HIGH_IND = 2U;
static const uint8_t MB_MREG_SET_RESP_REG_NUM_LOW_IND = 3U;
static const uint8_t MB_MREG_SET_RESP_NUM_REGS_HIGH_IND = 4U;
static const uint8_t MB_MREG_SET_RESP_NUM_REGS_LOW_IND = 5U;

static const uint8_t MB_MREG_SET_RESP_HEADER_SIZE = 6U;	// Size is sans the CRC.


// ***********************************************************
// **** Single-Reg Write Mask Frames
// RTU Single-Reg Mask Set Frame Index
static const uint8_t MB_MSKSREG_SET_ADDRESS_IND = 0U;
static const uint8_t MB_MSKSREG_SET_FUNC_CODE_IND = 1U;
static const uint8_t MB_MSKSREG_SET_REG_NUM_HIGH_IND = 2U;
static const uint8_t MB_MSKREG_SET_REG_NUM_LOW_IND = 3U;
static const uint8_t MB_MSKREG_SET_AND_MASK_HIGH_IND = 4U;
static const uint8_t MB_MSKREG_SET_AND_MASK_LOW_IND = 5U;
static const uint8_t MB_MSKREG_SET_OR_MASK_HIGH_IND = 6U;
static const uint8_t MB_MSKREG_SET_OR_MASK_LOW_IND = 7U;

static const uint8_t MB_MSKREG_SET_HEADER_SIZE = 8U;	// Size is sans the CRC.

// RTU Single-Reg Set Mask Response Frame Index
static const uint8_t MB_MSKREG_SET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_MSKREG_SET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_MSKREG_SET_RESP_REG_NUM_HIGH_IND = 2U;
static const uint8_t MB_MSKREG_SET_RESP_REG_NUM_LOW_IND = 3U;
static const uint8_t MB_MSKREG_SET_RESP_AND_MASK_HIGH_IND = 4U;
static const uint8_t MB_MSKREG_SET_RESP_AND_MASK_LOW_IND = 5U;
static const uint8_t MB_MSKREG_SET_RESP_OR_MASK_HIGH_IND = 6U;
static const uint8_t MB_MSKREG_SET_RESP_OR_MASK_LOW_IND = 7U;

static const uint8_t MB_MSKREG_SET_RESP_HEADER_SIZE = 8U;	// Size is sans the CRC.


/*
 *****************************************************************************************
 *		Modbus Set/Get Message Indexes.
 */


// ***********************************************************
// **** Multi-Reg Set/Get Frames
// RTU Multi-Bit Get Frame Index
static const uint8_t MB_MREG_GETSET_ADDRESS_IND = 0U;
static const uint8_t MB_MREG_GETSET_FUNC_CODE_IND = 1U;
static const uint8_t MB_MREG_GETSET_GET_REG_NUM_HIGH_IND = 2U;
static const uint8_t MB_MREG_GETSET_GET_REG_NUM_LOW_IND = 3U;
static const uint8_t MB_MREG_GETSET_NUM_REGS_GET_HIGH_IND = 4U;
static const uint8_t MB_MREG_GETSET_NUM_REGS_GET_LOW_IND = 5U;
static const uint8_t MB_MREG_GETSET_SET_REG_NUM_HIGH_IND = 6U;
static const uint8_t MB_MREG_GETSET_SET_REG_NUM_LOW_IND = 7U;
static const uint8_t MB_MREG_GETSET_NUM_REGS_SET_HIGH_IND = 8U;
static const uint8_t MB_MREG_GETSET_NUM_REGS_SET_LOW_IND = 9U;
static const uint8_t MB_MREG_GETSET_NUM_BYTES_SET_LOW_IND = 10U;
static const uint8_t MB_MREG_GETSET_DATA_IND = 11U;

static const uint8_t MB_MREG_GETSET_HEADER_SIZE = 11U;	// Size is sans the CRC.

// RTU Multi-Bit Get Response Frame Index
static const uint8_t MB_MREG_GETSET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_MREG_GETSET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_MREG_GETSET_RESP_BYTE_NUM_IND = 2U;
static const uint8_t MB_MREG_GETSET_RESP_DATA = 3U;

static const uint8_t MB_MREG_GETSET_RESP_HEADER_SIZE = 3U;	// Size is sans the CRC.


// ***********************************************************
// **** Diagnostic Frames
// RTU Diagnostic Get Frame Index
static const uint8_t MB_DIAG_GET_FUNC_ADDRESS_IND = 0U;
static const uint8_t MB_DIAG_GET_FUNC_CODE_IND = 1U;
static const uint8_t MB_DIAG_GET_SUBCMD_HIGH_IND = 2U;
static const uint8_t MB_DIAG_GET_SUBCMD_LOW_IND = 3U;
static const uint8_t MB_DIAG_GET_DATA_IND = 4U;

static const uint8_t MB_DIAG_GET_HEADER_SIZE = 4U;	// Size is sans the CRC.

// RTU Diagnostic Get Response Frame Index
static const uint8_t MB_DIAG_GET_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_DIAG_GET_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_DIAG_GET_RESP_SUBCMD_HIGH_IND = 2U;
static const uint8_t MB_DIAG_GET_RESP_SUBCMD_LOW_IND = 3U;
static const uint8_t MB_DIAG_GET_RESP_DATA_IND = 4U;

static const uint8_t MB_DIAG_GET_RESP_HEADER_SIZE = 4U;	// Size is sans the CRC.


// ***********************************************************
// **** Modbus System Command Frame
// ***********************************************************
// RTU User Function Code Frame Index
static const uint8_t MB_SYS_CMD_ADDRESS_IND = 0U;
static const uint8_t MB_SYS_CMD_FUNC_CODE_IND = 1U;
static const uint8_t MB_SYS_CMD_SUB_FUNC_CODE_IND = 2U;
static const uint8_t MB_SYS_CMD_DATA_IND = 3U;

static const uint8_t MB_SYS_CMD_HEADER_SIZE = 3U;		// Size is sans the CRC.


// RTU User Function Code Frame Index
static const uint8_t MB_SYS_CMD_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_SYS_CMD_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_SYS_CMD_RESP_SUB_FUNC_CODE_IND = 2U;
static const uint8_t MB_SYS_CMD_RESP_DATA_IND = 3U;

static const uint8_t MB_SYS_CMD_RESP_HEADER_SIZE = 3U;	// Size is sans the CRC.


// ***********************************************************
// **** Get Device Object Identity
// ***********************************************************
// Device Identity Object Frame Index
static const uint8_t MB_DEV_ID_ADDRESS_IND = 0U;
static const uint8_t MB_DEV_ID_FUNC_CODE_IND = 1U;
static const uint8_t MB_DEV_ID_MEI_TYPE_IND = 2U;
static const uint8_t MB_DEV_ID_READ_DEV_ID_IND = 3U;
static const uint8_t MB_DEV_ID_OBJ_ID_IND = 4U;

static const uint8_t MB_DEV_ID_HEADER_SIZE = 5U;		// Size is sans the CRC.


// Device Identity Object Response Frame Index
static const uint8_t MB_DEV_ID_RESP_ADDRESS_IND = 0U;
static const uint8_t MB_DEV_ID_RESP_FUNC_CODE_IND = 1U;
static const uint8_t MB_DEV_ID_RESP_MEI_TYPE_IND = 2U;
static const uint8_t MB_DEV_ID_RESP_READ_DEV_ID_IND = 3U;
static const uint8_t MB_DEV_ID_RESP_CONFORMITY_LEVEL_IND = 4U;
static const uint8_t MB_DEV_ID_RESP_MORE_FOLLOWS_IND = 5U;
static const uint8_t MB_DEV_ID_RESP_NEXT_OBJ_ID_IND = 6U;
static const uint8_t MB_DEV_ID_RESP_NUM_OBJ_IND = 7U;
static const uint8_t MB_DEV_ID_RESP_OBJ_LIST_IND = 8U;

static const uint8_t MB_DEV_ID_RESP_HEADER_SIZE = 7U;	// Size is sans the CRC.


// ***********************************************************
// **** Attribute Access Identity
// ***********************************************************
// The attribute access function is a hijack of the
// normal read multiple reg, write multiple reg and read write multiple.



#endif
