/**
 **********************************************************************************************
 * @file            Ethernet module debug message interface file
 *
 * @brief           This file contains macros used for debug messages in Ethernet module
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
#ifndef ETHERNET_DEBUG_H
#define ETHERNET_DEBUG_H

#include "Babelfish_Debug.h"

#if BF_ETHERNET_DBG
	#define ETHERNET_DBG_PRINT  BF_DBG_ON
#else
	#define ETHERNET_DBG_PRINT  BF_DBG_OFF
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef ETHERNET_ENABLE_MASK
	#define ETHERNET_ENABLE_MASK    DEBUG_MASK( ETHERNET_DBG_PRINT )
#endif

/**
 * Macro to print Ethernet module specific debug prints
 * We can override this macro if we want to transmit debug print messages
 * of this module to a specific interface whereas all other modules on another interface
 */
#define ETH_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( ETHERNET_ENABLE_MASK, MessageType, __VA_ARGS__ )

#endif	// #ifndef ETHERNET_DEBUG_H
