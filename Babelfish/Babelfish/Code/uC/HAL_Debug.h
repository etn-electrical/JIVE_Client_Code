/**
 **********************************************************************************************
 * @file            HAL_Debug.h HAL debug interface configuration file.
 *
 * @brief           This file contains debug macros used for printing debug messages in
 * 					HAL modules
 *
 * @details			Enabling Debugging in HAL will enable debug logs for all sub-modules
 * 					of HALs
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef HAL_DEBUG_H
#define HAL_DEBUG_H

#include "Babelfish_Debug.h"

/****************************************************************************************************
*  Debug defines for User Session debug logs
****************************************************************************************************/
#if BF_HAL_DBG
	#define HAL_DBG_PRINT   BF_DBG_ON
#else
	#define HAL_DBG_PRINT   BF_DBG_OFF
#endif	// #if BF_HAL_DBG

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef HAL_ENABLE_MASK
	#define HAL_ENABLE_MASK   DEBUG_MASK( HAL_DBG_PRINT )
#endif

/**
 * Macros to print DCI module specific debug prints
 * We can override these macros if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define HAL_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( HAL_ENABLE_MASK, MessageType, __VA_ARGS__ )
#define HAL_DEBUG_PRINT_FUNC( MessageType, FuncName, ... )  BF_DEBUG_PRINT_FUNC( HAL_ENABLE_MASK, MessageType, \
																				 FuncName, \
																				 __VA_ARGS__ )

/****************************************************************************************************
*  Add other debug defines for sub modules of Web folder below
****************************************************************************************************/

#endif	// #ifndef HAL_DEBUG_H
