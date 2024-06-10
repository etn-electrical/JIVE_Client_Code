/**
 *****************************************************************************************
 *  @file IEC61850 stack integration debug message interface file
 *	@brief This file contains macros used for debug messages in IEC61850 stack module
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef IEC61850_DEBUG_H
   #define IEC61850_DEBUG_H

#include "Babelfish_Debug.h"

#if BF_IEC61850_DBG
	#define IEC61850_DBG_PRINT  BF_DBG_ON		///< Enables the IEC61850 Debug print feature
#else
	#define IEC61850_DBG_PRINT  BF_DBG_OFF		///< Disbales the IEC61850 Debug print feature
#endif

/*
    Macro to handle what all different severity and message types are allowed on debug interface
    In later stage, we should be able to receive setting for this mask from user
 */
#ifndef IEC61850_ENABLE_MASK
	#define IEC61850_ENABLE_MASK    DEBUG_MASK( IEC61850_DBG_PRINT )
#endif

/*
   Macro to print Ethernet module specific debug prints
   We can override this macro if we want to transmit debug print messages
   of this module to a specific interface whereas all other modules on another interface
 */
#define IEC61850_DEBUG_PRINT( MessageType, ... )     BF_DEBUG_PRINT( IEC61850_ENABLE_MASK, MessageType, __VA_ARGS__ )

#endif	// #ifndef IEC61850_DEBUG_H
