/*
 *****************************************************************************************
 *		$Workfile$
 *
 *		$Author$
 *		$Date$
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator:
 *
 *		Description:
 *
 *
 *
 *		$Header$
 *****************************************************************************************
 */
#ifndef APP_IO_CONFIG_ESP32_H
#define APP_IO_CONFIG_ESP32_H

#ifdef DEBUG
	#define DEBUG_OUTPUTS_ENABLED
// #define DEBUG_PRINTF_SUPPORT /* IAR Terminal IO */
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
	#define Term_Print( ... )			// printf( __VA_ARGS__ )
	#define Term_vPrint( ... )			// vprintf( __VA_ARGS__ )
	#define Term_Print_Idle()
#elif defined UART_DEBUG
extern void UARTDebug( const char* pcString, ... );

	#define Term_Print( ... )			// UARTDebug( __VA_ARGS__ )
	#define Term_vPrint( ... )			// UARTDebug( __VA_ARGS__ )
#else
	#include <stdio.h>
	#define Term_Print( ... )
	#define Term_vPrint( ... )
	#define Term_Print_Idle()
#endif

/*
 *****************************************************************************************
 *		RTC Periph
 *****************************************************************************************
 */
#define RTC_CLOCK_SOURCE        CLOCK_SOURCE_LSE
#define RTC_FORMAT              RTC_BCD_FORMAT
#define USE_ONBOARD_RTC

/*****************************************************************************************
 * Controls Certificate Generation and storage in the product
 *****************************************************************************************
 */
/* Preprocessor define DISABLE_PKI_CERT_GEN_SUPPORT can be uncommented to disable
 * communication certificate and key generation and storage in the device(used for TLS/DTLS communication)
 * If a product doesn't want the certificate generation functionality but code sign,
 * this define can make only Code sign functionality and interfaces enabled.
 */
#define DISABLE_PKI_CERT_GEN_SUPPORT

/*
 *****************************************************************************************
 *		OSCR Configuration
 *****************************************************************************************
 */
#define OSCR_DOG_TIMER              uC_BASE_TIMER_CTRL_1
// OSCR Configuration
// Set watchdog monitor timeout to 500ms mostly useful in TLS_Server Task.
#define OSCR_DOG_FREQUENCY          ( ( TIMER_BASE_CLK / TIMER_DIVIDER ) )	// convert counter value to seconds
#define OSCR_DOG_HW_INT_PRIORITY    uC_INT_HW_PRIORITY_8
#define CR_TASK_NORMAL_PRIORITY     OS_TASK_PRIORITY_LOW_IDLE_TASK
#define CR_TASK_ELEVATED_PRIORITY   OS_TASK_PRIORITY_7



#endif	// #ifndef APP_IO_CONFIG_ESP32_H
