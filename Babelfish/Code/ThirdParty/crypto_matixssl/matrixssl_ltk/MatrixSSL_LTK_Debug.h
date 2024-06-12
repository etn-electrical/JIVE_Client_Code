/**
 **********************************************************************************************
 * @file            BG Lib debug message interface file
 *
 * @brief           This file contains macros used for debug messages in BG Lib module
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
#ifndef MATRIXSSL_LTK_DEBUG_H
#define MATRIXSSL_LTK_DEBUG_H

#include "Babelfish_Debug.h"

#if BF_MATRIXSSL_LTK_DBG
#define MATRIXSSL_LTK_DBG_PRINT  BF_DBG_ON
#else
#define MATRIXSSL_LTK_DBG_PRINT  BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef MATRIXSSL_LTK_DBG_ENABLE_MASK
#define MATRIXSSL_LTK_DBG_ENABLE_MASK      DEBUG_MASK( MATRIXSSL_LTK_DBG_PRINT )
#endif

/**
 * Macro to print BG Lib specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define MATRIXSSL_LTK_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( MATRIXSSL_LTK_DBG_ENABLE_MASK, MessageType, \
																		  __VA_ARGS__ )

#endif	// #ifndef MATRIXSSL_LTK_DEBUG_H
