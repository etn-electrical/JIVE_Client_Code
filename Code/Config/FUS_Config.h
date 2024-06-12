/**
 *****************************************************************************************
 *  @file

 *	@brief Web image control.
 *
 *	@details This class will provide interfaces of web image.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef     FUS_CONFIG_H
#define     FUS_CONFIG_H

#include "Device_Config.h"
#include "INT_ASCII_Conversion.h"
#include "string.h"
#include "Proc_Mem_ID_Codes.h"
#include "Rest_Config.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define PROD_SPEC_FW_UPGRADE_MIN_ACCESS_LEVEL   99
#define NAME_LEN( x ) strlen( reinterpret_cast<char*>( const_cast<uint8_t*>( x ) ) );

#define FUS_BIG_ENDIAN 0xFFFE0000U
#define FUS_LITTLE_ENDIAN 0x0000FEFFU
#define MAGIC_ENDIAN FUS_LITTLE_ENDIAN

#define MAX_CHUNK_SIZE MAX_FW_UPGRADE_DECODED_PACKET_SIZE

/* Uses this information inside FUS for ram optimization */
#define MAX_IMAGES_PER_COMPONENT    8

/* Define the length of the space required for operations */
static const uint8_t ASCII_STRING_SIZE = 14U;
static const uint8_t DCID_STRING_SIZE = 32U;
static const uint8_t PROD_NAME_MAX_LENGTH = 32U;
static const uint8_t COMP_NAME_MAX_LENGTH = 32U;
static const uint8_t IMAGE_NAME_MAX_LENGTH = 32U;

/* FUS - Common defines */
static const uint8_t* PRODUCT_NAME = ( const uint8_t* )"RTOS TOOL KIT";
static const uint8_t PRODUCT_NAME_LEN = NAME_LEN( PRODUCT_NAME );
static const uint16_t PRODUCT_CODE = PRODUCT_GUID;
static const uint16_t FUS_VERSION = 0;
static const uint16_t FUS_INIT_OP_POLLING_TIME_MS = 100U;

/* Component related defines */
static const uint8_t* LOCAL_COMP_NAME = ( const uint8_t* )"RTK_MAIN_PROCESSOR";
static const uint8_t LOCAL_COMP_NAME_LEN = NAME_LEN( LOCAL_COMP_NAME );
static const uint32_t LOCAL_COMP_HW_VERSION = 0x00030201;

static const uint8_t LOCAL_COMP_INDEX_0 = 0;
static const uint8_t TOTAL_COMPONENTS = 1;

/* Image related defines */
const uint32_t DEFAULT_WAIT_TIME_MS = 2000U;
const uint32_t RETRY_TIME_MS = 100U;
const uint32_t MAX_RETRY_COUNT = 3U;
const uint8_t DEFAULT_IDLE_TIME_MS = 0U;
extern uint8_t* g_fus_op_buf;

/* Image index for component 0 */
static const uint8_t LOCAL_COMP_APP_IMG_INDEX_0 = 0;
static const uint8_t LOCAL_COMP_WEB_IMG_INDEX_1 = 1U;

#if defined ( WEB_SERVER_SUPPORT )
#if defined ( REST_I18N_LANGUAGE )
/* Keep the Language image index greater than Web and Application image index */
static const uint8_t LOCAL_COMP_LANG_IMG_INDEX_2 = 2U;
static const uint8_t LOCAL_COMP_TOTAL_IMAGES = 3U;
#else
static const uint8_t LOCAL_COMP_TOTAL_IMAGES = 2U;
#endif	// REST_I18N_LANGUAGE
#else
static const uint8_t LOCAL_COMP_TOTAL_IMAGES = 1U;	///< For only APP IMAGE
#endif	// WEB_SERVER_SUPPORT

/* Session related defines */
static const uint32_t MAX_NUMBER_5_DIGIT = 99999U;
static const uint32_t MIN_NUMBER_4_DIGIT = 9999U;
static const uint16_t MAX_SESSION_TIMEOUT_SEC = 600U;	/* 600 sec 10 minutes */
static const uint8_t SESSION_ZERO_WAIT_TIME_MS = 0U;

#if defined ( REST_I18N_LANGUAGE )
/* Language image related defines */
static const uint8_t* IMAGE_NAME = "RTK LANGUAGE IMAGE";
static const uint8_t IMAGE_NAME_LEN = NAME_LEN( IMAGE_NAME );
#endif	// REST_I18N_LANGUAGE

/**
 * @brief Enum of interfaces that can be selected for doing
 * firmware upgrade
 */
enum fus_if_t
{
	NO_FUS,		///< Interface should not use 0
	REST_FUS,	///< REST interface.
	IOT_FUS		///< IOT interface.
	/* New interfaces will be added here */
};

#endif	// FUS_CONFIG_H
