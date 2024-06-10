/**
 **********************************************************************************************
 * @file            Ram debug interface configuration file.
 *
 * @brief           This file contains debug macros used for printing debug messages in
 * 					Ram modules
 *
 * @details			Enabling Debugging messages for BF Ram code will generate extensive
 *                  messages showing every allocation and free operation, which can be
 * 					then compared to spot leaks (allocations without a matching deallocation).
 *
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef RAM_DEBUG_H
#define RAM_DEBUG_H

#include "Babelfish_Debug.h"

/****************************************************************************************************
*  Debug defines for RAM debug logging
****************************************************************************************************/
#if BF_RAM_DBG
#define RAM_DBG_PRINT   BF_DBG_ON
#else
#define RAM_DBG_PRINT   BF_DBG_OFF
#endif	// #if BF_RAM_DBG

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    Here, for the RAM messages, we ensure that the timestamp and function name are not printed out,
    since we will have so many of these messages and don't want to overrun the DebugManager buffer.
 */
#ifndef RAM_ENABLE_MASK
#define RAM_ENABLE_MASK   DEBUG_MASK( RAM_DBG_PRINT )
#endif

/**
 * Macros to print RAM code-specific debug messages
 * We can override these macros if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define RAM_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( RAM_ENABLE_MASK, MessageType, __VA_ARGS__ )
#define RAM_DEBUG_PRINT_FUNC( MessageType, FuncName, ... )  BF_DEBUG_PRINT_FUNC( RAM_ENABLE_MASK, MessageType, FuncName, \
																				 __VA_ARGS__ )

/****************************************************************************************************
*  Add other debug defines for sub modules of Web folder below
****************************************************************************************************/

#endif	// #ifndef RAM_DEBUG_H
