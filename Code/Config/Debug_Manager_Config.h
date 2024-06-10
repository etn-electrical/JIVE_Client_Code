/**
 **********************************************************************************************
 * @file            Debug Manager configuration file for
 *
 * @brief           This file contains Debug Manager configuration. These configuration
 * 					settings will be used to configure module level and message type debug
 * 					level logs.
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
#ifndef DEBUG_MANAGER_CONFIG_H
#define DEBUG_MANAGER_CONFIG_H

//#define _DISABLE_DEBUG_MESSAGES_



/*  Macro for enabling Debug Manager functionality for Debug Print
 *  Do not enable Term Print or Modbus UART if enabling Debug Manager
 *  as they all use same UART interface.
 */
#define DEBUG_MANAGER

#ifdef DEBUG
// #define DEBUG_OUTPUTS_ENABLED
#define DEBUG_PRINTF_SUPPORT/* Terminal IO */
// #define UART_DEBUG /* Debug msgs through UART Terminal */
#endif

// The following Code provides a printf substitute.  Even though the IAR printf support is disabled
// mostly when the debug mode is not on - it is a good thing to just pull it out because it does
// still call a function when present.  So we remove it entirely.
#ifdef DEBUG_PRINTF_SUPPORT
#undef UART_DEBUG
#endif
#ifdef DEBUG_PRINTF_SUPPORT
	#include <stdio.h>
	#define Term_Print( ... )         printf( __VA_ARGS__ )
	#define Term_vPrint( ... )        vprintf( __VA_ARGS__ )
	#define BLE_Print( ... )          printf( __VA_ARGS__ )
	#define Term_Print_Idle()
#elif defined UART_DEBUG
extern void UARTDebug( const char* pcString, ... );

	#define Term_Print( ... )         UARTDebug( __VA_ARGS__ )
	#define BLE_Print( ... )          UARTDebug( __VA_ARGS__ )
	#define Term_vPrint( ... )        UARTDebug( __VA_ARGS__ )
#else
	#define Term_Print( ... )
	#define Term_vPrint( ... )
	#define BLE_Print( ... )
	#define Term_Print_Idle()
#endif

// If debug manager is defined then only include other defines
#ifdef DEBUG_MANAGER
/*
 * Babelfish supported COMMUNICATION INTERFACE for debug print
 * Babelfish will support only one interface for debugging purpose.
 * To enable communication interface, uncomment the respective macro. If multiple interfaces are
 * enabled, the first in the list will be used.
 */

/*
 *   Check BF_Shell_Main.cpp for UART initialization and communication setting.
 *	By default, On board UART connectivity with ST link cable is used for modbus RTU communication on STM32F427/9 and
 * STM32F767 based nucleo board.
 *	Therefore connecting debug manager over UART on these boards require external connectors on these boards.
 */

// #define BF_DBG_IF_UART
#define BF_DBG_IF_TERM_IO

/*
 * Debug Message Additional functionality
 * BF_ADD_MSG_TIME: ON if wants to add system time in debug message
 * BF_ADD_FUNC_NAME: ON if wants to add called function name in debug message
 */
#define BF_DBG_PRINT_TIME           DBG_OFF
#define BF_DBG_PRINT_FUNC_NAME      DBG_ON

/*
 * Debug Manager options for enabling / disabling debug message types
 */
#define BF_DBG_MSG_EMERGENCY    DBG_ON
#define BF_DBG_MSG_ERROR        DBG_ON
#define BF_DBG_MSG_CRITICAL     DBG_ON




/*
 * Module level configuration
 */
#ifndef _DISABLE_DEBUG_MESSAGES_
	#define BF_DBG_MSG_INFO         DBG_OFF
	#define BF_DBG_MSG_DEBUG        DBG_OFF

	#define BF_PROD_SPEC_DBG        DBG_OFF
	#define BF_DCI_DBG              DBG_OFF
	#define BF_DSI_DBG              DBG_OFF
	#define BF_USER_SESS_DBG        DBG_OFF
	#define BF_NV_CTRL_DBG          DBG_OFF
	#define BF_TLS_SERVER_DBG       DBG_OFF
	#define BF_SSL_CONN_DBG         DBG_OFF
	#define BF_ETHERNET_DBG         DBG_OFF
	#define BF_BLE_DBG              DBG_OFF
	#define BF_EIP_DBG              DBG_OFF
	#define BF_REST_DBG             DBG_OFF
	#define BF_WEBSERVER_DBG        DBG_OFF
	#define BF_FAULTS_DBG           DBG_OFF
	#define BF_CR_TASKER_DBG        DBG_OFF
	#define BF_FUS_DBG              DBG_ON
	#define BF_PKI_DBG              DBG_OFF
	#define BF_FREERTOS_DBG         DBG_OFF
	#define BF_USER_ACC_DBG         DBG_OFF
	#define BF_IOT_DBG              DBG_ON
	#define BF_ACD_DBG              DBG_OFF
	#define BF_LANGUAGE_DBG         DBG_OFF
	#define BF_MODBUS_TCP_DBG       DBG_OFF
	#define BF_PTP_DBG              DBG_OFF
	#define BF_IEC61850_DBG         DBG_OFF
	#define BF_TEST_CODE_DBG        DBG_OFF
	#define BF_HAL_DBG              DBG_OFF
#else

	#define BF_DBG_MSG_INFO         DBG_OFF
	#define BF_DBG_MSG_DEBUG        DBG_OFF

	#define BF_PROD_SPEC_DBG        DBG_OFF
	#define BF_DCI_DBG              DBG_OFF
	#define BF_DSI_DBG              DBG_OFF
	#define BF_USER_SESS_DBG        DBG_OFF
	#define BF_NV_CTRL_DBG          DBG_OFF
	#define BF_TLS_SERVER_DBG       DBG_OFF
	#define BF_SSL_CONN_DBG         DBG_OFF
	#define BF_ETHERNET_DBG         DBG_OFF
	#define BF_BLE_DBG              DBG_OFF
	#define BF_EIP_DBG              DBG_OFF
	#define BF_REST_DBG             DBG_OFF
	#define BF_WEBSERVER_DBG        DBG_OFF
	#define BF_FAULTS_DBG           DBG_OFF
	#define BF_CR_TASKER_DBG        DBG_OFF
	#define BF_FUS_DBG              DBG_OFF
	#define BF_PKI_DBG              DBG_OFF
	#define BF_FREERTOS_DBG         DBG_OFF
	#define BF_USER_ACC_DBG         DBG_OFF
	#define BF_IOT_DBG              DBG_OFF
	#define BF_ACD_DBG              DBG_OFF
	#define BF_LANGUAGE_DBG         DBG_OFF
	#define BF_MODBUS_TCP_DBG       DBG_OFF
	#define BF_PTP_DBG              DBG_OFF
	#define BF_IEC61850_DBG         DBG_OFF
	#define BF_TEST_CODE_DBG        DBG_OFF
	#define BF_HAL_DBG              DBG_OFF
#endif //#ifndef _DISABLE_DEBUG_MESSAGES_

// <<<< ADD 	NEW MODULE ENTRY HERE >>>>

#endif	// #ifdef DEBUG_MANAGER
#endif	// #ifndef DEBUG_MANAGER_CONFIG_H
