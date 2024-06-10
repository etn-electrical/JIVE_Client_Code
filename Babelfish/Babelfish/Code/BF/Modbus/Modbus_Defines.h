/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Define Modbus command indications
 *
 *	@details
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_DEFINES_H
#define MODBUS_DEFINES_H

#include "Bit.hpp"

// ***********************************************************
/// **** Modbus Typedefs.
typedef uint16_t MODBUS_REG_TD;
typedef uint16_t modbus_reg_t;

// ***********************************************************
// Modbus buffer size
#ifndef MODBUS_NET_BUFFER_SIZE
#define MODBUS_NET_BUFFER_SIZE 256U
#endif


#ifndef MODBUS_UART_BUFFER_SIZE
#define MODBUS_UART_BUFFER_SIZE 256U
#endif
// ***********************************************************
/// **** Modbus TX Modes.
enum tx_mode_t
{
	MODBUS_RTU_TX_MODE,
	MODBUS_ASCII_TX_MODE,
	MODBUS_TCP_TX_MODE
};

// static const uint8_t	MODBUS_RTU_TX_MODE = 0U;
// static const uint8_t	MODBUS_ASCII_TX_MODE = 1U;
// static const uint8_t	MODBUS_TCP_TX_MODE = 2U;

// ***********************************************************
/// **** Modbus Parity Codes (parity and stop bit combos)
enum parity_stop_t
{
	MODBUS_PARITY_EVEN_1_STOP_BIT,	// 0
	MODBUS_PARITY_ODD_1_STOP_BIT,	// 1
	MODBUS_PARITY_NONE_2_STOP_BIT,	// 2
	MODBUS_PARITY_EVEN_2_STOP_BIT,	// 3
	MODBUS_PARITY_ODD_2_STOP_BIT,	// 4
	MODBUS_PARITY_NONE_1_STOP_BIT	// 5
};

// ***********************************************************
/// **** Modbus Parity Modes
enum parity_t
{
	MODBUS_PARITY_EVEN,	// 0
	MODBUS_PARITY_ODD,	// 1
	MODBUS_PARITY_NONE	// 2
};

// ***********************************************************
/// **** Modbus Stop Bits
enum stop_bits_t
{
	MODBUS_1_STOP_BIT,
	MODBUS_2_STOP_BIT
};

// ***********************************************************
/// **** Modbus Baud Rates
static const uint32_t MODBUS_BAUD_19200 = 19200U;
static const uint32_t MODBUS_BAUD_9600 = 9600U;
static const uint32_t MODBUS_BAUD_38400 = 38400U;
static const uint32_t MODBUS_BAUD_57600 = 57600U;
static const uint32_t MODBUS_BAUD_115200 = 115200U;
static const uint32_t MODBUS_BAUD_230400 = 230400U;
static const uint32_t MODBUS_BAUD_460800 = 460800U;
static const uint32_t MODBUS_BAUD_INVALID = MODBUS_BAUD_19200;

// ***********************************************************
/// **** Generic Frame Structure this is used by DCI and MSG.
typedef struct
{
	uint8_t* data;
	uint16_t data_length;
} MB_FRAME_STRUCT;

// ***********************************************************
/// **** Modbus ASCII Frame definitions.
static const uint8_t MODBUS_ASCII_BOF = ':';	// function is for attribute retrieval.
static const uint8_t MODBUS_ASCII_CR = 0x0DU;	// CR function is for attribute retrieval.
static const uint8_t MODBUS_ASCII_LF = 0x0AU;	// LF function is for attribute retrieval.
static const uint8_t MODBUS_ASCII_FRAMING_SIZE = 3U;

// ***********************************************************
/// **** Modbus RTU Frame length
static const uint16_t MODBUS_MAX_RTU_MSG_ADDRESS_LEN = 1U;
static const uint16_t MODBUS_MAX_RTU_MSG_FUNCTION_CODE_LEN = 1U;
static const uint16_t MODBUS_MAX_RTU_MSG_CRC_LEN = 2U;
static const uint16_t MODBUS_MAX_RTU_MSG_LENGTH = 256U;
static const uint16_t MODBUS_MIN_RTU_MSG_LENGTH = MODBUS_MAX_RTU_MSG_ADDRESS_LEN +
	MODBUS_MAX_RTU_MSG_FUNCTION_CODE_LEN + MODBUS_MAX_RTU_MSG_CRC_LEN;
static const uint16_t MODBUS_MAX_RTU_MSG_DATA_LEN = MODBUS_MAX_RTU_MSG_LENGTH - ( MODBUS_MAX_RTU_MSG_ADDRESS_LEN +
																				  MODBUS_MAX_RTU_MSG_FUNCTION_CODE_LEN +
																				  MODBUS_MAX_RTU_MSG_CRC_LEN );



// ***********************************************************
/// **** Modbus ASCII Frame length with the CRC.
static const uint16_t MODBUS_MAX_ASCII_MSG_LENGTH = 513U;
static const uint16_t MODBUS_MIN_ASCII_MSG_LENGTH = 6U;


// ***********************************************************
/// **** Special address for broadcast.
static const uint8_t MODBUS_BROADCAST_ADDRESS = 0U;


// ***********************************************************
/// **** User Function Codes - User Reset
static const uint8_t MB_READ_COILS_FUNC_CODE = 1U;
static const uint8_t MB_READ_DISCRETE_INPUTS_FUNC_CODE = 2U;
static const uint8_t MB_READ_HOLDING_REG_FUNC_CODE = 3U;
static const uint8_t MB_READ_INPUT_REG = 4U;
static const uint8_t MB_WRITE_SINGLE_COIL_FUNC_CODE = 5U;
static const uint8_t MB_WRITE_SINGLE_REGISTER_FUNC_CODE = 6U;
static const uint8_t MB_READ_EXCEPTION_STATUS_FUNC_CODE = 7U;
static const uint8_t MB_DIAGNOSTICS_FUNC_CODE = 8U;
static const uint8_t MB_WRITE_MULTIPLE_COILS_FUNC_CODE = 15U;
static const uint8_t MB_WRITE_MULTIPLE_REG_FUNC_CODE = 16U;
static const uint8_t MB_WRITE_MASK_WRITE_REG_FUNC_CODE = 22U;	///< This function is presently not supported.
static const uint8_t MB_READ_WRITE_MULTIPLE_REG_FUNC_CODE = 23U;


// ***********************************************************
/// **** System Command Function Codes - User Reset
static const uint8_t MB_SYS_CMD_USER_FUNC_CODE = 0x42U;
static const uint8_t MB_SYS_CMD_USER_FUNC_CODE_LENGTH = 1U;
enum	// System Command User Function Codes.
{
	MB_SYS_CMD_FACTORY_RESET,
	MB_SYS_CMD_SOFT_RESET,
	MB_SYS_CMD_APP_PARAM_RESET,
	MB_SYS_CMD_WATCHDOG_TEST,
	MB_SYS_CMD_TRIG_COMM_LOSS_ACTION,
	MB_SYS_CMD_MFG_UNLOCK
	// There are service commands above this level.  They must start at 128 and go up.  See Services_List.h to see what
	// is available.
};

// ***********************************************************
/// **** User Function Codes - Attribute Access Function Code
/// Attribute access is accomplished by stuffing the high byte
/// of the "num bits" with the attribute you want access to.
static const uint8_t MB_READ_ATTRIB_BIT_ACCESS_FUNC_CODE = 0x46U;	///< This function code is for attribute retrieval.
static const uint8_t MB_WRITE_ATTRIB_BIT_ACCESS_FUNC_CODE = 0x47U;	///< This function code is for attribute retrieval.
/// Attribute access is accomplished by stuffing the high byte
/// of the "num registers" with the attribute you want access to.
static const uint8_t MB_READ_ATTRIB_ACCESS_FUNC_CODE = 0x43U;	///< This function is for attribute retrieval.
static const uint8_t MB_WRITE_ATTRIB_ACCESS_FUNC_CODE = 0x44U;	///< This function is for attribute retrieval.
static const uint8_t MB_READ_WRITE_ATTRIB_ACCESS_FUNC_CODE = 0x45U;	///< This function is for attribute retrieval.
/// These are the attribute selections.
static const uint8_t MB_ATTRIB_NORMAL_ACCESS = 0U;
static const uint8_t MB_ATTRIB_RAM_ACCESS = 1U;
static const uint8_t MB_ATTRIB_LENGTH_ACCESS = 2U;
static const uint8_t MB_ATTRIB_INIT_ACCESS = 3U;
static const uint8_t MB_ATTRIB_DEFAULT_ACCESS = 4U;
static const uint8_t MB_ATTRIB_RANGE_MIN_ACCESS = 5U;
static const uint8_t MB_ATTRIB_RANGE_MAX_ACCESS = 6U;

static const uint8_t MB_ATTRIB_LENGTH_ACCESS_NUM_REG = 1U;


// ***********************************************************
/// **** Modbus Object Function Codes - Identity information
static const uint8_t MB_GET_DEVICE_IDENTITY_FUNC_CODE = 43U;
static const uint8_t MB_GET_DEVICE_IDENTITY_MEI_CODE = 0x0EU;


/// ***********************************************************
/// **** Modbus Error Codes
static const uint8_t MB_NO_ERROR_CODE = 0U;
static const uint8_t MB_ILLEGAL_FUNCTION_ERROR_CODE = 1U;
static const uint8_t MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE = 2U;
static const uint8_t MB_ILLEGAL_DATA_VALUE_ERROR_CODE = 3U;
static const uint8_t MB_SLAVE_DEVICE_FAILURE_ERROR_CODE = 4U;
static const uint8_t MB_SLAVE_DEVICE_ACK_ERROR_CODE = 5U;	///< Processing takes time. Come back.
static const uint8_t MB_SLAVE_DEVICE_BUSY_ERROR_CODE = 6U;
static const uint8_t MB_GENERAL_RECEIVE_ERROR_CODE = 0xFCU;
static const uint8_t MB_DEST_STRING_SIZE_WRONG_ERROR_CODE = 0xFDU;
static const uint8_t MB_INVALID_INTERCOM_BLOCK_ERROR_CODE = 0xFEU;
static const uint8_t MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE = 0xFFU;

static const uint8_t MB_DIAG_CODE_RETURN_QUERY_DATA = 0U;


// ***********************************************************
/// **** Frame size definitions.
static const uint16_t MB_READ_COILS_MAX_BITS = 0x7D0U;		// 2000
static const uint16_t MB_READ_DISCRETE_INPUTS_MAX_BITS = 0x7B0U;	// 1968
static const uint16_t MB_READ_HOLDING_REG_MAX_REGS = 125U;
static const uint16_t MB_READ_INPUT_REG_MAX_REGS = 125U;
static const uint16_t MB_WRITE_SINGLE_COIL_MAX_BYTES = 2U;
static const uint16_t MB_WRITE_SINGLE_REGISTER_MAX_BYTES = 2U;
// static const uint16_t MB_READ_EXCEPTION_STATUS_MAX_BYTES = 7U;
// static const uint16_t MB_DIAGNOSTICS_MAX_BYTES = 8U;
static const uint16_t MB_WRITE_MULTIPLE_COILS_MAX_BITS = 0x7B0U;
static const uint16_t MB_WRITE_MULTIPLE_REG_MAX_REGS = 0x7BU;
// static const uint16_t MB_WRITE_MASK_WRITE_REG_MAX_BYTES = 22U;
static const uint16_t MB_READ_WRITE_MULTIPLE_READ_REG_MAX_REGS = 0x7DU;
static const uint16_t MB_READ_WRITE_MULTIPLE_WRITE_REG_MAX_REGS = 0x79U;


// ***********************************************************
/// **** Error code bit in the command response.
static const uint8_t MB_FUNC_ERROR_RESP_BIT = 7U;
static const uint8_t MB_FUNC_ERROR_RESP_BITMASK = static_cast<uint8_t>( 1 << MB_FUNC_ERROR_RESP_BIT );
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline bool Test_MB_Error_Resp( uint8_t func_code )
{ return ( ( func_code & MB_FUNC_ERROR_RESP_BITMASK ) != 0 ); }

/**
 *	Modbus Message User Function Code defines
 */
typedef void* modbus_message_callback_param_t;
typedef uint8_t (* modbus_message_callback_t)( modbus_message_callback_param_t param,  MB_FRAME_STRUCT* rx_struct,
											   MB_FRAME_STRUCT* tx_struct );



#endif
