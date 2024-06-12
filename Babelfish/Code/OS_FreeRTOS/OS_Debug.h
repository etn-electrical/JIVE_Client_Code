/**
 **********************************************************************************************
 * @file            OSCR Mon debug message interface file
 *
 * @brief           This file contains macros used for debug messages in OSCR Mon module
 *
 * @details
 *
 * @copyright       2022 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef OS_DEBUG_H
#define OS_DEBUG_H

#include "Babelfish_Debug.h"

#if BF_OS_DBG
#define OS_DBG_PRINT  BF_DBG_ON
#else
#define OS_DBG_PRINT  BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef OS_ENABLE_MASK
#define OS_ENABLE_MASK      DEBUG_MASK( OS_DBG_PRINT )
#endif

/**
 * Macro to print RAM Management specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define OS_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( OS_ENABLE_MASK, MessageType, __VA_ARGS__ )

#endif	// #ifndef OS_DEBUG_H
