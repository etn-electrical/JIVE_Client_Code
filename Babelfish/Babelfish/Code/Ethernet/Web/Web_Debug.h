/**
 **********************************************************************************************
 * @file            Web module debug interface configuration file.
 *
 * @brief           This file contains debug macros used for printing debug messages in
 * 					Web module
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
#ifndef WEB_DEBUG_H
#define WEB_DEBUG_H

#include "Babelfish_Debug.h"

/****************************************************************************************************
*  Debug defines for User Session debug logs
****************************************************************************************************/
#if BF_USER_SESS_DBG
	#define USER_SESS_DBG_PRINT BF_DBG_ON
#else
	#define USER_SESS_DBG_PRINT BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef USER_SESS_ENABLE_MASK
	#define USER_SESS_ENABLE_MASK   DEBUG_MASK( USER_SESS_DBG_PRINT )
#endif

/**
 * Macro to print User Session specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define USER_SESS_PRINT( MessageType, ... )     BF_DEBUG_PRINT( USER_SESS_ENABLE_MASK, MessageType, __VA_ARGS__ )

/****************************************************************************************************
*  Debug defines for Rest debug logs
****************************************************************************************************/
#if BF_REST_DBG
	#define REST_DBG_PRINT BF_DBG_ON
#else
	#define REST_DBG_PRINT BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef REST_ENABLE_MASK
	#define REST_ENABLE_MASK   DEBUG_MASK( REST_DBG_PRINT )
#endif

/**
 * Macro to print REST module specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define REST_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( REST_ENABLE_MASK, MessageType, __VA_ARGS__ )

/****************************************************************************************************
*  Add other debug defines for sub modules of Web folder below
****************************************************************************************************/
#if BF_WEBSERVER_DBG
	#define WEBSERVER_DBG_PRINT  BF_DBG_ON
#else
	#define WEBSERVER_DBG_PRINT  BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef WEBSERVER_ENABLE_MASK
	#define WEBSERVER_ENABLE_MASK     DEBUG_MASK( WEBSERVER_DBG_PRINT )
#endif

/**
 * Macro to print External flash specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define WEBSERVER_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( WEBSERVER_ENABLE_MASK, MessageType, __VA_ARGS__ )

#endif	// #ifndef WEB_DEBUG_H
