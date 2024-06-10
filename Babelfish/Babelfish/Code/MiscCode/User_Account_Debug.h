/**
 **********************************************************************************************
 * @file            User Account module debug interface configuration file.
 *
 * @brief           This file contains debug macros used for printing debug messages in
 * 					User Account module
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
#ifndef USER_ACCOUNT_DEBUG_H
#define USER_ACCOUNT_DEBUG_H

#include "Babelfish_Debug.h"

/****************************************************************************************************
*  Debug defines for User Session debug logs
****************************************************************************************************/
#if BF_USER_ACC_DBG
	#define USER_ACC_DBG_PRINT BF_DBG_ON
#else
	#define USER_ACC_DBG_PRINT BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef USER_ACC_ENABLE_MASK
	#define USER_ACC_ENABLE_MASK   DEBUG_MASK( USER_ACC_DBG_PRINT )
#endif

/**
 * Macro to print User Session specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define USER_ACC_PRINT( MessageType, ... )     BF_DEBUG_PRINT( USER_ACC_ENABLE_MASK, MessageType, __VA_ARGS__ )

#endif	// #ifndef USER_ACCOUNT_DEBUG_H
