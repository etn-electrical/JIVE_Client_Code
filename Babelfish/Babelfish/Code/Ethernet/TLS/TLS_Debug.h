/**
 **********************************************************************************************
 * @file            TLS Server and SSL Connection debug message interface file
 *
 * @brief           This file contains macros used for debug messages in the modules included in this file
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
#ifndef TLS_DEBUG_H
#define TLS_DEBUG_H

#include "Babelfish_Debug.h"

/**
 **********************************************************************************************
 *
 * TLS Serevr Module
 *
 **********************************************************************************************
 */

#if BF_TLS_SERVER_DBG
	#define TLS_SERVER_DBG_PRINT  BF_DBG_ON
#else
	#define TLS_SERVER_DBG_PRINT  BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef TLS_SERVER_ENABLE_MASK
	#define TLS_SERVER_ENABLE_MASK     DEBUG_MASK( TLS_SERVER_DBG_PRINT )
#endif

/**
 * Macro to print TLS Server module specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define TLS_SERVER_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( TLS_SERVER_ENABLE_MASK, MessageType, \
																	   __VA_ARGS__ )

/**
 **********************************************************************************************
 *
 * SSL Connection Module
 *
 **********************************************************************************************
 */

#if BF_SSL_CONN_DBG
	#define SSL_CONN_DBG_PRINT  BF_DBG_ON
#else
	#define SSL_CONN_DBG_PRINT  BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef SSL_CONN_ENABLE_MASK
	#define SSL_CONN_ENABLE_MASK     DEBUG_MASK( SSL_CONN_DBG_PRINT )
#endif

/**
 * Macro to print SSL Connection module specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define SSL_CONN_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( SSL_CONN_ENABLE_MASK, MessageType, __VA_ARGS__ )

#endif	// #ifndef TLS_DEBUG_H