/**
 **********************************************************************************************
 * @file            EVENT Module debug message interface file
 * @brief           This file contains macros used for debug messages in EVENT Module
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef _EVENT_DEBUG_H_
#define _EVENT_DEBUG_H_

#include "Babelfish_Debug.h"

#if BF_EVENT_DBG
	#define EVENT_DBG_PRINT  BF_DBG_ON
#else
	#define EVENT_DBG_PRINT  BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef EVENT_ENABLE_MASK
	#define EVENT_ENABLE_MASK     DEBUG_MASK( EVENT_DBG_PRINT )
#endif

/**
 * Macro to print EVENT Module debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define EVENT_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( EVENT_ENABLE_MASK, MessageType, __VA_ARGS__ )


#endif	/* _EVENT_DEBUG_H_ */
