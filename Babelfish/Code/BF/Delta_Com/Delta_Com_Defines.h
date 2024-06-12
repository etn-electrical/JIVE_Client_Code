/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DELTA_COM_DEFINES_H
	#define DELTA_COM_DEFINES_H

#include "Includes.h"
#include "DCI.h"

// #define DCOM_PRINTF_SUPPORT
#ifdef DCOM_PRINTF_SUPPORT
	#include <stdio.h>

	#define DCOM_Print_Active( ... )          printf( "DCOM Debug Print Active...\n" )

// #define DCOM_Access_Print(...)			printf( __VA_ARGS__ )
	#define DCOM_Access_Print( ... )

	#define DCOM_Error_Print( ... )           printf( __VA_ARGS__ )
// #define DCOM_Error_Print(...)

// #define DCOM_SubPub_Print(...)			printf( __VA_ARGS__ )
	#define DCOM_SubPub_Print( ... )

	#define DCOM_Print_Idle()
#else
	#define DCOM_Print_Active( ... )
	#define DCOM_Access_Print( ... )
	#define DCOM_Error_Print( ... )
	#define DCOM_SubPub_Print( ... )
	#define DCOM_Print_Idle()
#endif


typedef uint16_t product_code_t;
typedef uint8_t discovery_status_t;
typedef uint16_t param_attrib_info_t;
// typedef uint8_t delta_com_status_t;
typedef uint8_t delta_com_device_address;

static const uint8_t INVALID_DELTA_COM_DEVICE_ADDRESS = 0xFFU;

static const uint8_t DELTA_COM_FUNCTION_CODE = 0x46U;

static const uint16_t PARAMETER_LENGTH = 2U;
static const uint16_t ATTRIBUTE_LENGTH = 1U;
static const uint16_t COUNT_LENGTH = 2U;
static const uint8_t MIN_DATA_LENGTH = 1U;
static const uint16_t SYSTEM_COMMAND_LENGTH = 1U;


enum DELTA_COM_PRODUCT_CODES
{
	VIRTUAL_PRODUCT_CODE_UNDEFINED,
	VIRTUAL_BUI_PRODUCT_CODE = 0x1000,
	VIRTUAL_MEASUREMENT_MODULE_PRODUCT_CODE = 0x1001,
	VIRTUAL_BASE_CONTROL_MODULE_PRODUCT_CODE = 0x1002
};

/**
 * Delta Com Error Enums
 */
typedef uint8_t delta_com_error_t;
static const delta_com_error_t NO_DCOM_ERROR = 0U;
static const delta_com_error_t INVALID_COMMAND = 1U;
static const delta_com_error_t BAD_FRAME_LENGTH = 2U;
static const delta_com_error_t INVALID_SYSTEM_COMMAND = 3U;
static const delta_com_error_t INVALID_PARAMETER = 4U;
static const delta_com_error_t DCOM_ERROR_INVALID_ATTRIBUTE = 5U;
static const delta_com_error_t INVALID_PARAMETER_LENGTH = 6U;
static const delta_com_error_t INVALID_PARAMETER_DATA = 7U;
static const delta_com_error_t ACCESS_VIOLATION = 8U;
static const delta_com_error_t DEVICE_UN_INITIALIZED = 9U;
static const delta_com_error_t SYNCHRONIZATION_ERROR = 10U;
static const delta_com_error_t UNKNOWN_ERROR = 11U;
static const delta_com_error_t RESERVED = 12U;

enum delta_com_status_t
{
	DELTA_COM_OK,
	DELTA_COM_PARAMETER_MISSING,	// REMOVE//Indicates a non-fatal mismatch in expected paramterst to thos that exist.
	DELTA_COM_ERROR,// REMOVE
	DELTA_COM_INVALID_COMMAND,	// REMOVE
	DELTA_COM_INITIALIZATION_ERROR,
	DELTA_COM_TIMEOUT,
	DELTA_COM_PROTOCOL_LAYER_INITIALIZED
};


typedef uint8_t delta_com_attrib_t;
static const delta_com_attrib_t CURRENT_VALUE = 0U;
static const delta_com_attrib_t INITIAL_VALUE = 1U;
static const delta_com_attrib_t DEFAULT_VALUE = 2U;
static const delta_com_attrib_t LENGTH = 3U;
static const delta_com_attrib_t DATATYPE = 4U;
static const delta_com_attrib_t ATTRIBUTE_INFORMATION = 5U;
static const delta_com_attrib_t MIN = 6U;
static const delta_com_attrib_t MAX = 7U;
static const delta_com_attrib_t ENUM_LIST_LENGTH = 8U;
static const delta_com_attrib_t ENUM_LIST = 9U;
static const delta_com_attrib_t SYSTEM_COMMAND_PARAMETER = 10U;
static const delta_com_attrib_t END_OF_ATTRIBUTE_LIST = 11U;

static const uint16_t LENGTH_LEN = 2U;	// it is a u16 for storing length.
static const uint16_t DATATYPE_LEN = 1U;
static const uint16_t PARAM_ATTRIB_INFO_LEN = sizeof( param_attrib_info_t );

/***************************************************************
* 				Parameter Attribute Information Definition
***************************************************************/
static const uint8_t CURRENT_VAL_RW_ACCESS_BIT = 0U;
static const uint8_t INIT_VAL_READ_ACCESS_BIT = 1U;
static const uint8_t INIT_VAL_RW_ACCESS_BIT = 2U;
static const uint8_t DEFAULT_VAL_READ_ACCESS_BIT = 3U;
static const uint8_t DEFAULT_VAL_RW_ACCESS_BIT = 4U;
static const uint8_t RANGE_VAL_READ_ACCESS_BIT = 5U;
static const uint8_t RANGE_VAL_RW_ACCESS_BIT = 6U;
static const uint8_t ENUM_VAL_READ_ACCESS_BIT = 7U;
static const uint8_t ENUM_VAL_RW_ACCESS_BIT = 8U;
static const uint8_t LENGTH_RW_ACCESS_BIT = 9U;

/***************************************************************
* 				General Frame
***************************************************************/
// GENERAL_FRAME
static const uint16_t LENGTH_LOW = 0;
static const uint16_t LENGTH_HIGH = 1;
static const uint16_t COMMAND_BYTE = 2;

static const uint16_t GENERAL_FRAME_LENGTH = COMMAND_BYTE + 1U;

/***************************************************************
* 				System_Command
***************************************************************/
// SYSTEM_FRAME

static const uint16_t SYSTEM_COMMAND_BYTE = GENERAL_FRAME_LENGTH;
static const uint16_t SYSTEM_COMMAND_DATA = SYSTEM_COMMAND_BYTE + 1U;

static const uint16_t SYSTEM_COMMAND_FRAME_LENGTH = SYSTEM_COMMAND_DATA;


/***************************************************************
* 				GET_PARAMETER
***************************************************************/
// GET_FRAME
static const uint16_t GET_ATTRIBUTE = GENERAL_FRAME_LENGTH;
static const uint16_t GET_OFFSET_LOW = GET_ATTRIBUTE + 1U;
static const uint16_t GET_OFFSET_HIGH = GET_OFFSET_LOW + 1U;
static const uint16_t GET_AMOUNT_LOW = GET_OFFSET_HIGH + 1U;
static const uint16_t GET_AMOUNT_HIGH = GET_AMOUNT_LOW + 1U;
static const uint16_t GET_PARAMETER_LOW = GET_AMOUNT_HIGH + 1U;
static const uint16_t GET_PARAMETER_HIGH = GET_PARAMETER_LOW + 1U;
static const uint16_t GET_DATA = GET_PARAMETER_HIGH + 1U;

static const uint16_t GET_FRAME_LENGTH = GET_DATA;


/***************************************************************
* 				SET_PARAMETER
***************************************************************/
// SET_FRAME
static const uint16_t SET_ATTRIBUTE = GENERAL_FRAME_LENGTH;
static const uint16_t SET_OFFSET_LOW = SET_ATTRIBUTE + 1U;
static const uint16_t SET_OFFSET_HIGH = SET_OFFSET_LOW + 1U;
static const uint16_t SET_AMOUNT_LOW = SET_OFFSET_HIGH + 1U;
static const uint16_t SET_AMOUNT_HIGH = SET_AMOUNT_LOW + 1U;
static const uint16_t SET_PARAMETER_LOW = SET_AMOUNT_HIGH + 1U;
static const uint16_t SET_PARAMETER_HIGH = SET_PARAMETER_LOW + 1U;
static const uint16_t SET_DATA = SET_PARAMETER_HIGH + 1U;

static const uint16_t SET_FRAME_LENGTH = SET_DATA;


/***************************************************************
* 				EXCHANGE_MULTIPLE_PARAMETERS
***************************************************************/

// EXCHANGE_MULTIPLE_PARAMETERS_REQUEST_FRAME
static const uint16_t EXCHANGE_MULT_PARAMETERS_FRAME_START = GENERAL_FRAME_LENGTH;

// EXCHANGE_MULTIPLE_PARAMETERS_RETURN_FRAME
static const uint16_t EXCHANGE_MULT_PARAMETERS_RETURN_ATTRIBUTE = GENERAL_FRAME_LENGTH;
static const uint16_t EXCHANGE_MULTIPLE_PARAMETERS_RETURN_COUNT_LOW =
	EXCHANGE_MULT_PARAMETERS_RETURN_ATTRIBUTE + 1U;
static const uint16_t EXCHANGE_MULTIPLE_PARAMETERS_RETURN_COUNT_HIGH =
	EXCHANGE_MULTIPLE_PARAMETERS_RETURN_COUNT_LOW + 1U;

static const uint16_t EXCHANGE_MULT_PARAMETERS_RETURN_FRAME_START =
	EXCHANGE_MULTIPLE_PARAMETERS_RETURN_COUNT_HIGH + 1U;

/***************************************************************
* 				GET_MULTIPLE_ATTRIBUTES
***************************************************************/
// GET_MULTIPLE_ATTRIBUTES_REQUEST_FRAME
static const uint16_t MULT_ATTRIBUTES_PARAMETER_LOW = GENERAL_FRAME_LENGTH;
static const uint16_t MULT_ATTRIBUTES_PARAMETER_HIGH = MULT_ATTRIBUTES_PARAMETER_LOW + 1U;

static const uint16_t MULT_ATTRIBUTES_BEGIN = MULT_ATTRIBUTES_PARAMETER_HIGH + 1U;

// GET_MULTIPLE_ATTRIBUTES_RETURN_FRAME
static const uint16_t MULT_ATTRIBUTES_RETURN_LOW = GENERAL_FRAME_LENGTH;
static const uint16_t MULT_ATTRIBUTES_RETURN_HIGH = MULT_ATTRIBUTES_RETURN_LOW + 1U;

static const uint16_t MULT_ATTRIBUTES_OUTPUT_BEGIN = MULT_ATTRIBUTES_RETURN_HIGH + 1U;


/***************************************************************
* 				GET Delta Data
***************************************************************/
// SET_DELTA_DATA_FRAME
static const uint16_t SET_DELTA_NEW_DE_QUE_KEY = GENERAL_FRAME_LENGTH;
static const uint16_t SET_DELTA_PREVIOUS_DE_QUE_KEY = SET_DELTA_NEW_DE_QUE_KEY + 1U;

static const uint16_t SET_DELTA_FRAME_BEGIN = SET_DELTA_PREVIOUS_DE_QUE_KEY + 1U;

// GET_DELTA_DATA_FRAME
static const uint16_t GET_DELTA_NEW_DE_QUE_KEY = GENERAL_FRAME_LENGTH;
static const uint16_t GET_DELTA_PREVIOUS_DE_QUE_KEY = GET_DELTA_NEW_DE_QUE_KEY + 1U;

static const uint16_t GET_DELTA_FRAME_BEGIN = GET_DELTA_PREVIOUS_DE_QUE_KEY + 1U;


/***************************************************************
* 				Manage Subscriptions
***************************************************************/
// MANAGE_SUBSCRIPTIONS_ATTRIBUTES
static const uint8_t STOP_TRACKING_PARAMETER_FOR_CHANGES = 0U;
static const uint8_t CLIENT_PUB_REQ_CLIENT_SOURCE_OF_TRUTH = 1U;
static const uint8_t CLIENT_PUB_REQ_SERVER_SOURCE_OF_TRUTH = 2U;
static const uint8_t SERVER_PUB_REQ = 3U;
static const uint8_t CLIENT_PUB_REQ_CLIENT_SOURCE_OF_TRUTH_GEN_ACCESS = 4U;
static const uint8_t CLIENT_PUB_REQ_SERVER_SOURCE_OF_TRUTH_GEN_ACCESS = 5U;

// MANAGE_SUBSCRIPTIONS_REQUEST_FRAME
static const uint16_t MANAGE_SUBSCRIPTIONS_FRAME_BEGIN = GENERAL_FRAME_LENGTH;

// MANAGE_SUBSCRIPTIONS_RETURN_FRAME
static const uint16_t MANAGE_SUBSCRIPTIONS_RETURN_SUBSCRIPTION_TYPE = GENERAL_FRAME_LENGTH;
static const uint16_t MANAGE_SUBSCRIPTIONS_RETURN_NUMBER_PARAMS_LOW =
	MANAGE_SUBSCRIPTIONS_RETURN_SUBSCRIPTION_TYPE + 1U;
static const uint16_t MANAGE_SUBSCRIPTIONS_RETURN_NUMBER_PARAMS_HIGH =
	MANAGE_SUBSCRIPTIONS_RETURN_NUMBER_PARAMS_LOW + 1U;

static const uint16_t MANAGE_SUBSCRIPTIONS_RETURN_PARAMS_BEGIN =
	MANAGE_SUBSCRIPTIONS_RETURN_NUMBER_PARAMS_HIGH + 1U;


/***************************************************************
* 				Manage Subscriptions
***************************************************************/
// IDENTITY_REQUEST_FRAME
static const uint16_t IDENTITY_REQUEST_FRAME_BEGIN = GENERAL_FRAME_LENGTH;
static const uint16_t IDENTITY_REQUEST_FRAME_ID_BLOCK_COMPATIBILITY = IDENTITY_REQUEST_FRAME_BEGIN;
static const uint16_t IDENTITY_REQUEST_FRAME_PROD_COMPATIBILITY = IDENTITY_REQUEST_FRAME_BEGIN + 1;
static const uint16_t IDENTITY_REQUEST_FRAME_PROD_CODE_0 = IDENTITY_REQUEST_FRAME_BEGIN + 2;
static const uint16_t IDENTITY_REQUEST_FRAME_PROD_CODE_1 = IDENTITY_REQUEST_FRAME_BEGIN + 3;
static const uint16_t IDENTITY_REQUEST_FRAME_SUB_PROD_CODE_0 = IDENTITY_REQUEST_FRAME_BEGIN + 4;
static const uint16_t IDENTITY_REQUEST_FRAME_SUB_PROD_CODE_1 = IDENTITY_REQUEST_FRAME_BEGIN + 5;
static const uint16_t IDENTITY_REQUEST_FRAME_FW_REV_0 = IDENTITY_REQUEST_FRAME_BEGIN + 6;
static const uint16_t IDENTITY_REQUEST_FRAME_FW_REV_1 = IDENTITY_REQUEST_FRAME_BEGIN + 7;
static const uint16_t IDENTITY_REQUEST_FRAME_FW_REV_2 = IDENTITY_REQUEST_FRAME_BEGIN + 8;
static const uint16_t IDENTITY_REQUEST_FRAME_FW_REV_3 = IDENTITY_REQUEST_FRAME_BEGIN + 9;
static const uint16_t IDENTITY_REQUEST_FRAME_HW_REV_0 = IDENTITY_REQUEST_FRAME_BEGIN + 10;
static const uint16_t IDENTITY_REQUEST_FRAME_HW_REV_1 = IDENTITY_REQUEST_FRAME_BEGIN + 11;
static const uint16_t IDENTITY_REQUEST_FRAME_SERIAL_NUM_0 = IDENTITY_REQUEST_FRAME_BEGIN + 12;
static const uint16_t IDENTITY_REQUEST_FRAME_SERIAL_NUM_1 = IDENTITY_REQUEST_FRAME_BEGIN + 13;
static const uint16_t IDENTITY_REQUEST_FRAME_SERIAL_NUM_2 = IDENTITY_REQUEST_FRAME_BEGIN + 14;
static const uint16_t IDENTITY_REQUEST_FRAME_SERIAL_NUM_3 = IDENTITY_REQUEST_FRAME_BEGIN + 15;
static const uint16_t IDENTITY_REQUEST_FRAME_BUFFER_SIZE_0 = IDENTITY_REQUEST_FRAME_BEGIN + 16;
static const uint16_t IDENTITY_REQUEST_FRAME_BUFFER_SIZE_1 = IDENTITY_REQUEST_FRAME_BEGIN + 17;
static const uint16_t IDENTITY_REQUEST_FRAME_UPDATE_INTERVAL_TIME_0 = IDENTITY_REQUEST_FRAME_BEGIN + 18;
static const uint16_t IDENTITY_REQUEST_FRAME_UPDATE_INTERVAL_TIME_1 = IDENTITY_REQUEST_FRAME_BEGIN + 19;

static const uint16_t IDENTITY_REQUEST_FRAME_LENGTH = IDENTITY_REQUEST_FRAME_UPDATE_INTERVAL_TIME_1 + 1;

/*********************************************************************
*				Error Frame
*********************************************************************/
static const uint16_t ERROR_CODE = GENERAL_FRAME_LENGTH;
static const uint16_t ERROR_CODE_FRAME_LENGTH = ERROR_CODE + 1U;


#endif
