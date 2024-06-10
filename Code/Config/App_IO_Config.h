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
#ifndef APP_IO_CONFIG_H
#define APP_IO_CONFIG_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#ifndef BOOTLOADER_BUILD
#include "uC_OS_Device_Defines.h"
#include "Debug_Manager_Config.h"
#endif
/*
 * General macros for Enabling and Disabling Babelfish Debug feature
 */
#define DBG_ON      0x01
#define DBG_OFF     0x00
/*
 *****************************************************************************************
 *		The following includes the micro specific Pin IO definitions.
 *****************************************************************************************
 */
#include "App_IO_Config_ESP32.h"

#endif	// #ifndef APP_IO_CONFIG_H

