/**
 **********************************************************************************************
 * @file            Babelfish Debug interface configuration file.
 *
 * @brief           This file contains all top level debug macros.
 *
 * @details			This file defines all macros required for enabling debugging feature.
 * 					User can edit only those options which are marked with <<Add here>>
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef BABELFISH_DEBUG_H
#define BABELFISH_DEBUG_H

#include "Includes.h"
#include "Debug_Manager.h"
#include "Debug_Manager_Config.h"

/*
   Enable / Disable debug messages completely
 */
#define BF_DBG_ON   0x8000U
#define BF_DBG_OFF  0x0000U

/*
 * Set debug message type configuration
 * Babelfish Supported debug Message types
 * Emergency: Device stop condition
 * Error    : Device errors conditions
 * Critical : Critical messages
 * INFO     : Informational messages
 * DEBUG    : Debug messages (For developer's only)
 */
#define DBG_MSG_EMERGENCY   ( BF_DBG_MSG_EMERGENCY << 4 )		// 4th Bit
#define DBG_MSG_ERROR       ( BF_DBG_MSG_ERROR << 3 )			// 3rd Bit
#define DBG_MSG_CRITICAL    ( BF_DBG_MSG_CRITICAL << 2 )		// 2nd Bit
#define DBG_MSG_INFO        ( BF_DBG_MSG_INFO << 1 )			// 1st Bit
#define DBG_MSG_DEBUG       ( BF_DBG_MSG_DEBUG << 0 )			// 0th Bit

// Mask for all Babelfish supported message types
#define MSG_TYPE_MASK       ( DBG_MSG_EMERGENCY | DBG_MSG_CRITICAL | DBG_MSG_ERROR | DBG_MSG_INFO | DBG_MSG_DEBUG )

// Debug mask define
#define DEBUG_MASK( module_status )     ( module_status | MSG_TYPE_MASK )

/*
 * DebugPrint Macro
 * This macro will be used to print debug logs on selected interfaces
 */
#ifdef DEBUG_MANAGER
	#define BF_DEBUG_PRINT( enable_mask, msg_type, ... )                                   \
	if ( ( enable_mask & BF_DBG_ON ) != 0U )                                               \
	{                                                                                      \
		if ( ( enable_mask & msg_type ) != 0U )                                             \
		{                                                                                   \
			Debug_Manager::Debug_Msg_Print( enable_mask, msg_type, __func__, __VA_ARGS__ ); \
		}                                                                                   \
	}

/* Add a variant when the logging needs to pass through its choice for the function name.
 * Avoids the problem where a common logging function is used, and you don't want to see its name instead.
 */
	#define BF_DEBUG_PRINT_FUNC( enable_mask, msg_type, func_name, ... )                     \
	if ( ( enable_mask & BF_DBG_ON ) != 0U )                                                 \
	{                                                                                        \
		if ( ( enable_mask & msg_type ) != 0U )                                              \
		{                                                                                    \
			Debug_Manager::Debug_Msg_Print( enable_mask, msg_type, func_name, __VA_ARGS__ ); \
		}                                                                                    \
	}

#else
	#define BF_DEBUG_PRINT( enable_mask, msg_type, ... )
	#define BF_DEBUG_PRINT_FUNC( enable_mask, msg_type, func_name, ... )


#endif	// #ifdef DEBUG_MANAGER

#endif	// #ifndef BABELFISH_DEBUG_H
