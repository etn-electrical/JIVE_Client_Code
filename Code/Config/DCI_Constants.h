/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *
 *****************************************************************************************
 */
#ifndef DCI_CONSTANTS_H
   #define DCI_CONSTANTS_H

#include "Toolkit_Revision.h"
#include "Breaker.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// Firmware Rev is broken up as 0xMMmmbbbb Where MM is major, mm is minor and bbbb is build.
// 0xMMmmbbbb

// #define CONST_FIRMWARE_REV			0x0001
// #define CONST_FIRMWARE_REV_BUILD	0x0007
// #define CONST_FIRMWARE_REV_FULL	( ( CONST_FIRMWARE_REV<<16 ) | CONST_FIRMWARE_REV_BUILD )
// #define CONST_FIRMWARE_REV_ASCII	"0.01.0007"

/**
 * @brief Required for NV_Refresh functionality to set default value to newly added DCIs into the new firmware.
 * CONST_DCI_NV_MAP_VERSION_NUM : This version number should be increased if the new dci_nv_init_ver_XX increased and
   parameter DCIs added.
 * this version number will be compared with previous version number and if version number found mismatch, NV Refresh
   will be applied to added new DCIs.
 * If there is no change in version number, NV Refresh function will not be performed.
 * Refer files: NV_Refresh, NV_Refresh_List, Prod_Spec_NV_Refresh
 */
#define CONST_DCI_NV_MAP_VERSION_NUM 1


typedef struct file_version_s
{
	uint8_t major;
	uint8_t minor;
	uint16_t build;
} fw_file_version;

/* Don't add any suffix(like U) to numeric values, as It will also get added in ASCII version string
   Max numeric value of version can be decided based on the each element's datatype defined in above structure*/

//Ahmed
#define CONST_FW_REV_MAJOR      (MY_FIRMWARE_VERSION & 0xFF0000)>>16
#define CONST_FW_REV_MINOR      (MY_FIRMWARE_VERSION &0xFF00)>>8
#define CONST_FW_REV_BUILD       MY_FIRMWARE_VERSION &0xFF

#define CONST_WEB_FW_REV_MAJOR 0
#define CONST_WEB_FW_REV_MINOR 1
#define CONST_WEB_FW_REV_BUILD 1

#define CONST_PROD_HW_MAJOR 1
#define CONST_PROD_HW_MINOR 2
#define CONST_PROD_HW_BUILD 3

// PRODUCT_CODE defined in FUS_Config.h. - CONST_PROD_CODE should be removed, once DCI workbook is updated with
// PRODUCT_CODE
#define CONST_PROD_CODE 0x1234	// PRODUCT_CODE should be used instead of CONST_PROD_CODE

#define CONST_PROD_SERIAL_NUM 12345678

// CONST_PROD_CODE should be removed, once DCI workbook is updated with PRODUCT_CODE
#define CONST_PROD_CODE_ASCII   QUOTES( CONST_PROD_CODE )
#define CONST_FIRMWARE_VER_NUM ( ( CONST_FW_REV_BUILD << 16 ) & 0XFFFF0000 ) + \
	( ( CONST_FW_REV_MINOR << 8 ) & 0X0000FF00 ) + ( ( CONST_FW_REV_MAJOR ) & 0X000000FF )
#define CONST_PROD_HW_REV QUOTES( CONST_PROD_HW_MAJOR.CONST_PROD_HW_MINOR.CONST_PROD_HW_BUILD )

// Compatibility code for APP, WEB and HOST should be same.
#define CONST_APP_FW_COMPATIBILITY_CODE 4

// use this to remove PARAM Name content from code
// #define REMOVE_REST_PARAM_NAME_TEXT
// use this to remove PARAM Description content from code
// #define REMOVE_REST_PARAM_DESCRIPTION_TEXT

/* This is the role based logging access level, Adopter can define the value based on the level they
   want for Read and delete access to logs.Any role based user having access level less then the one
   defined here for read and delete will not be able to read and delete logs respectively
 */
#define LOG_READ_ACCESS_ROLE_LEVEL 31
#define LOG_DELETE_ACCESS_ROLE_LEVEL 99

#endif
