/** @file sample_iot_device_platform.h Example of a product-specific header for the IoT Device SDK configuration. */
#ifndef IOT_DEVICE_PLATFORM_H
#define IOT_DEVICE_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/* For your product, modify a copy of this file in your product area,
 * and there specify the configuration options necessary for your product
 * when using this Eaton IoT Device SDK.
 *
 * This file only serves as an example, to list the configurable definitions that you
 * might want to specify for your product.
 */

/* This enables the build for a low-cost RTOS-based OS (PX Green, as opposed to PX Red Linux) */
#define PX_GREEN_IOT_DEVICE

/******************************************************************************/
/*                    Build Configuration Settings                            */
/*  Your product build headers can pre-empt these definitions.
   /******************************************************************************/

#ifndef IOT_MAX_OUTBOUND_BUFFER_SIZE
/** Maximum size allocated for published messages.
 * While this size may be kept low for constrained resource devices,
 * note that Azure charges for messages at a 4KB minimum,
 * so more shorter messages are more expensive than one large one, if the RAM resources support it.
 */
	#define IOT_MAX_OUTBOUND_BUFFER_SIZE 512
#endif

#ifndef IOT_MAX_CONCURRENT_MESSAGES
/** Number of allocated ("concurrent") messages in pool for published messages. */
	#define IOT_MAX_CONCURRENT_MESSAGES 4
#endif

#ifndef IOT_MAX_UNCONFIRMED_MESSAGE_COUNT
/** Maximum number of unconfirmed messages before additiional device-to-cloud send requests are rejected.
 * Should not exceed IOT_MAX_CONCURRENT_MESSAGES. (Note: zero means no limit)  */
	#define IOT_MAX_UNCONFIRMED_MESSAGE_COUNT 4
#endif

#ifndef IOT_MAX_MESSAGE_RETRY_COUNT
/** Maximum number of retries for failed (unacknowledged) messages */
	#define IOT_MAX_MESSAGE_RETRY_COUNT 3
#endif

/** The size of all UUIDs in the standard text form 8-4-4-4-12 = 36, plus null */
#define IOT_MAX_DEVICE_UUID_SIZE 37

#ifndef TOKEN_ALLOC_SIZE
/** Declare the fixed allocation size of the tokens[] arrays we will use when deserializing JSON.
 * For PX Green, we fix this at a reasonably large size of 40, to handle the UpdateFirmware command. */
	#define TOKEN_ALLOC_SIZE 40
#endif

#ifndef IOT_MAX_ALARM_MESSAGE_SIZE
/** Maximum size (bytes) of "Alarms message" (i.e. note, user, and custom_message strings) */
	#define IOT_MAX_ALARM_MESSAGE_SIZE 512
#endif

#ifndef IOT_MAX_ALARM_CUSTOM_KEY_SIZE
/** maximum size of "Alarms custom key string" */
	#define IOT_MAX_ALARM_CUSTOM_KEY_SIZE 256
#endif




/* These #defines may be selected by the build instructions (CFLAGS)
 * in Makefiles and NetBeans Project Properties->Build->C Compiler->Additional Options,
 * or you may include a product header to specify them.
 */
/* #define IOT_INCLUDE_HTTP */
/* #define IOT_INCLUDE_MQTT_WEBSOCKETS */
/* #define IOT_INCLUDE_AMQP_WEBSOCKETS */
/* #define DONT_USE_UPLOADTOBLOB */

/**
 * @brief Do product-specific initialization as part of iot_open(
 *
 * @return 0 on success, else non-zero on failure.
 */
int platform_init();

/** Do a platform-specific sleep to pace the message thread.
 * When the thread resumes, it will check for new messages received.
 * Commonly sleeps for 100ms or 1s.
 * The Azure specs say that the platform must implement ThreadAPI_Sleep(), whether or not they do threading,
 * so that function is to be used here as well.
 */
void platform_message_thread_sleep();


/* Defines to compile in logging features.  Set in your product configuration. */
// #define IOT_ENABLE_DEBUG_LOGGING
// #define IOT_ENABLE_INFO_LOGGING
// #define IOT_ENABLE_ERROR_LOGGING
// #define IOT_ENABLE_LOG_SIGHANDLER

/* Can #define NO_LOGGING for Azure */
#if defined( IOT_ENABLE_ERROR_LOGGING ) || defined( IOT_ENABLE_INFO_LOGGING ) || defined( IOT_ENABLE_DEBUG_LOGGING )
	#include <stdarg.h>
	#include "azure_c_shared_utility/xlogging.h"	// Azure logging functions

/* Supported log levels */
enum IOT_LOG_LEVELS
{
	IOT_LOG_DISABLE = 0,
	IOT_LOG_ERROR,
	IOT_LOG_INFO,
	IOT_LOG_DEBUG,
};

/**
 * Custom logging callback function definition.
 * @param level     - Message log level.
 * @param funcName  - Name of function originating log message.
 * @param format    - Log message.
 * @param vargs     - Variable list of log message arguments for the format string.
 */
typedef void (*CustomLoggerCallback)( enum IOT_LOG_LEVELS level, const char *funcName, const char *format, va_list vargs );

/**
 * Registers a callback function for custom log functionality.
 * @param callback Function pointer.
 */
void setSDKLogCallback( CustomLoggerCallback callback );

/**
 * Removes the callback function for custom logging and will revert back to
 * default log functionality.
 */
void deleteSDKLogCallback();

/**
 * Sets the logging level for the Device SDK.
 * @param level Logging level.
 */
void setDeviceSDKLogLevel( enum IOT_LOG_LEVELS level );

#endif

#ifdef IOT_ENABLE_LOG_SIGHANDLER
/**
 * Installs signal handler to change logging level at runtime.
 */
void installLogSignalHandler();

#endif

//Define macro for each log function to auto populate the calling function's name.
#ifdef IOT_ENABLE_DEBUG_LOGGING
	#define Log_Debug( s, data ... ) LOG( AZ_LOG_INFO, LOG_LINE, s, ## data );
void Log_DEBUG( const char *funcName, const char *format,  ... );

#else
	#define Log_Debug( FORMAT, ... )
#endif

#ifdef IOT_ENABLE_INFO_LOGGING
	#define Log_Info( s, data ... ) LOG( AZ_LOG_INFO, LOG_LINE, s, ## data );
void Log_INFO( const char *funcName, const char *format,  ... );

#else
	#define Log_Info( FORMAT, ... )
#endif

#ifdef IOT_ENABLE_ERROR_LOGGING
	#define Log_Error( s, data ... ) LOG( AZ_LOG_ERROR, LOG_LINE, s, ## data );
void Log_ERROR( const char *funcName, const char *format,  ... );

#else
	#define Log_Error( FORMAT, ... )
#endif

/* Note: comment out the following line to disable verbose logging (only applies when using HTTP) */
/* #define IOT_ENABLE_VERBOSE_LOGGING */

#ifdef __cplusplus
}
#endif


/**********************************************************************************************
 ************** Heap Allocation and Deallocation Function Redirection *************************
 *
 * For this sample, this is a simple story: we substitute the common malloc, free, etc 
 * for the iot_malloc, iot_free, etc.
 * But if memory allocation is more carefully and locally managed in your application,
 * substitute your local functions here.
 */

#define iot_malloc( X ) 	malloc( X )
#define iot_free( X )		free( X )

/**********************************************************************************************/

#endif	/* IOT_DEVICE_PLATFORM_H */

