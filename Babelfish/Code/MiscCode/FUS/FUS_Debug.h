/**
 **********************************************************************************************
 * @file            FUS Module debug message interface file
 *
 * @brief           This file contains macros used for debug messages in FUS Module
 *
 * @details
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef FUS_DEBUG_H
#define FUS_DEBUG_H

#include "Babelfish_Debug.h"

#if BF_FUS_DBG
	#define FUS_DBG_PRINT  BF_DBG_ON
#else
	#define FUS_DBG_PRINT  BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef FUS_ENABLE_MASK
	#define FUS_ENABLE_MASK     DEBUG_MASK( FUS_DBG_PRINT )
#endif

/**
 * Macro to print FUS Module debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define FUS_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( FUS_ENABLE_MASK, MessageType, __VA_ARGS__ )

#endif	// #ifndef FUS_DEBUG_H