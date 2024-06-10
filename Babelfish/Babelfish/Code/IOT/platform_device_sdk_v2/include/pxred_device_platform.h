/** @file pxred_device_platform.h PX-Red Toolkit-specific header for the IoT Device SDK configuration. */
#ifndef PXRED_DEVICE_PLATFORM_H
#define PXRED_DEVICE_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/* For your product, you might override some of the settings below. */

/**
 * Enumeration of PX Red build configuration.
 * Mostly to match PX Green configuration; @ToDo mostly unused, maybe of no/low value
 */
enum IOT_DEVICE_LIMITS
{
#ifdef PX_GREEN_IOT_DEVICE
	IOT_INITIAL_OUTBOUND_BUFFER_SIZE = 1024,	/*!< initial message payload size */
	IOT_LOW_OUTBOUND_BUFFER_SIZE = 768,			/*!< remaining low buffer size threshold */
	IOT_INCREMENTAL_OUTBOUND_BUFFER_SIZE = 512,	/*!< size to incrementally increase message payload buffer */
#else
	IOT_INITIAL_OUTBOUND_BUFFER_SIZE = 4096,		/*!< initial message payload size */
	IOT_LOW_OUTBOUND_BUFFER_SIZE = 512,				/*!< remaining low buffer size threshold */
	IOT_INCREMENTAL_OUTBOUND_BUFFER_SIZE = 8192,	/*!< size to incrementally increase message payload buffer */
#endif
	IOT_MAX_DEVICE_UUID_SIZE = 37,				/*!< Size of any UUID */
	IOT_MAX_MESSAGE_RETRY_COUNT = 3,			/*!< maximum number of retries for failed messages */
	IOT_MAX_UNCONFIRMED_MESSAGE_COUNT = 0,		/*!< maximum number of unconfirmed messages before additiional device-to-cloud send requests are rejected (Note: zero means no limit)  */
	IOT_MAX_AZURE_MESSAGE_SIZE = ( 128 * 1024 ),/*!< maximum size (bytes) of a message allowed by Azure for D2C or C2D. */
	IOT_MAX_ALARM_MESSAGE_SIZE = 8192,			/*!< maximum size (bytes) of "Alarms message string" (i.e. note, user, and custom_message) */
	IOT_MAX_ALARM_CUSTOM_KEY_SIZE = 256			/*!< maximum size (bytes) of "Alarms custom key string" */
};


/* These #defines are set in the Makefile. */
/* #define IOT_INCLUDE_HTTP */
/* #define IOT_INCLUDE_MQTT_WEBSOCKETS */
/* #define IOT_INCLUDE_AMQP_WEBSOCKETS */
/* #define DONT_USE_UPLOADTOBLOB */

/**
 * @brief Do product-specific initialization as part of iot_open(
 *
 * @return 0 on success, else non-zero on failure.
 */
extern int platform_init();

/** Define the cycle interval for the messageThread, in milliseconds.
 * The messageThread will pause this long before checking again for messages.
 */
#define IOT_MESSAGE_THREAD_SLEEP 100

/** Do a platform-specific sleep to pace the message thread.
 * When the thread resumes, it will check for new messages received.
 * For px-toolkit, sleeps for 100ms.
 * The Azure specs say that the platform must implement ThreadAPI_Sleep(), whether or not they do threading,
 * so that function is to be used here as well.
 */
extern void platform_message_thread_sleep();


/* Don't enable the build for a low-cost RTOS-based OS (PX Green, as opposed to PX Red Linux) */
// #define IOT_DEVICE_LO

/* Defines to compile in logging features.  Set in your Makefile. */
// #define IOT_ENABLE_DEBUG_LOGGING
// #define IOT_ENABLE_INFO_LOGGING
// #define IOT_ENABLE_ERROR_LOGGING
// #define IOT_ENABLE_LOG_SIGHANDLER

/* Can #define NO_LOGGING for Azure */
#if defined( IOT_ENABLE_ERROR_LOGGING ) || defined( IOT_ENABLE_INFO_LOGGING ) || defined( IOT_ENABLE_DEBUG_LOGGING )
	#include <stdarg.h>
	#include "azure_c_shared_utility/xlogging.h"		// Azure logging functions

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
 * @param funcName - Name of function originating log message.
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
// Don't use the Azure LOG() function, as we lose the Debug level and only get Info level
// #define Log_Debug( s, data ... ) LOG( AZ_LOG_INFO, LOG_LINE, s, ## data );
void Log_DEBUG( const char *funcName, const char *format,  ... );

#define Log_Debug( s, data ... ) Log_DEBUG( FUNC_NAME, s, ## data )

#else
	#define Log_Debug( FORMAT, ... )
#endif

#ifdef IOT_ENABLE_INFO_LOGGING
// Likewise for INFO level, so we can better control the length of the output
// #define Log_Info( s, data ... ) LOG( AZ_LOG_INFO, LOG_LINE, s, ## data );
void Log_INFO( const char *funcName, const char *format,  ... );

#define Log_Info( s, data ... ) Log_INFO( FUNC_NAME, s, ## data )

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
 * For PXRed, this is a simple story: we substitute the common malloc, free, etc 
 * for the iot_malloc, iot_free, etc.
 * While not important for PXRed, it IS for PXGreen, where the memory allocation is more
 * carefully and locally managed.
 * The only uses of calloc() and realloc() are in PXRed code, so they do not need to be redirected.
 */

#define iot_malloc( X ) 	malloc( X )
#define iot_free( X )		free( X )

/**********************************************************************************************/


#endif	/* IOT_DEVICE_PLATFORM_H */
