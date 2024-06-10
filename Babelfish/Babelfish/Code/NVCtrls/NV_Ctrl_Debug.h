/**
 **********************************************************************************************
 * @file            NV Ctrl debug interface configuration file.
 *
 * @brief           This file contains debug macros used for printing debug messages in
 * 					NV Ctrl modules
 *
 * @details			Enabling Debugging in NV Ctrl will enable debug logs for all sub-modules
 * 					of NV Ctrls
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef NV_CTRL_DEBUG_H
#define NV_CTRL_DEBUG_H

#include "Babelfish_Debug.h"

/****************************************************************************************************
*  Debug defines for User Session debug logs
****************************************************************************************************/
#if BF_NV_CTRL_DBG
	#define NV_CTRL_DBG_PRINT   BF_DBG_ON
#else
	#define NV_CTRL_DBG_PRINT   BF_DBG_OFF
#endif	// #if BF_NV_CTRL_DBG

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef NV_CTRL_ENABLE_MASK
	#define NV_CTRL_ENABLE_MASK   DEBUG_MASK( NV_CTRL_DBG_PRINT )
#endif

/**
 * Macro to print DCI module specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define NV_CTRL_PRINT( MessageType, ... )     BF_DEBUG_PRINT( NV_CTRL_ENABLE_MASK, MessageType, __VA_ARGS__ )

/****************************************************************************************************
*  Add other debug defines for sub modules of Web folder below
****************************************************************************************************/

#endif	// #ifndef NV_CTRL_H
