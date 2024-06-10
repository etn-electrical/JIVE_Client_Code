/** @file iot_device_platform.h The Babelfish platform-specific header for the IoT Device SDK configuration.
 * Some of the functions defined here are implemented in platform_ltk.cpp.
 */
#ifndef IOT_DEVICE_PLATFORM_H
#define IOT_DEVICE_PLATFORM_H

/* These #defines are now selected by the product-level options in IoT_Config.h  */
#include "IoT_Config.h"
#include <stddef.h>

#ifndef bool
#include "stdbool.h"		// Somehow not covered by stddef.h
#endif

/* #define IOT_INCLUDE_HTTP
 #define IOT_INCLUDE_MQTT
 #define IOT_INCLUDE_MQTT_WEBSOCKETS
 #define IOT_INCLUDE_AMQP
 #define IOT_INCLUDE_AMQP_WEBSOCKETS
 #define DONT_USE_UPLOADTOBLOB*/

/* This enables the build of the Eaton IoT Device SDK for the RTOS-based OS (PX Green, as opposed to PX Red Linux). */
// #define IOT_DEVICE_LO	-- old name
// #define PX_GREEN_IOT_DEVICE	// --Define this earlier

// We always want Base64 decoding, for UpdateFirmware's ImageTransfer messages
#define WANT_BASE64_ENCODING


/* ****************************************************************************
                      Build Configuration Settings
    Your product build headers can pre-empt these definitions.
 *****************************************************************************/

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

/****************************************************************************
 * 		 Note: Any of these may have been set already in IoT_Config.h
 **************************************************************************** */

#ifndef TOKEN_ALLOC_SIZE
/** Declare the fixed allocation size of the tokens[] arrays we will use when deserializing JSON.
 * For PX Green, we fix this at a reasonably large size of 40, sufficient for the UpdateFirmware command. */
	#define TOKEN_ALLOC_SIZE 40
#endif

#ifndef IOT_MAX_COMMAND_METHOD_SIZE
/* @ToDo: Remove this no-longer-need definition: Maximum allowed size of "Device Command" method name */
	#define IOT_MAX_COMMAND_METHOD_SIZE 20
#endif

#ifndef IOT_MAX_COMMAND_PARAMS_SIZE
/* @ToDo: Remove this no-longer-need definition: Maximum allowed size of "Device Command" parameters (received as a JSON
   string) */
	#define IOT_MAX_COMMAND_PARAMS_SIZE 2048
#endif

#ifndef IOT_MAX_ALARM_MESSAGE_SIZE
/** Maximum size (bytes) of "Alarms message" (i.e. note, user, and custom_message strings) */
	#define IOT_MAX_ALARM_MESSAGE_SIZE 512
#endif

#ifndef IOT_MAX_ALARM_CUSTOM_KEY_SIZE
/** maximum size of "Alarms custom key string" */
	#define IOT_MAX_ALARM_CUSTOM_KEY_SIZE 256
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Do product-specific initialization as part of iot_open(
 *
 * @return 0 on success, else non-zero on failure.
 */
extern int platform_init();


extern bool Is_System_Clock_Set( void );

extern void Wait_Till_Ip_Configured( void );

/** Do a platform-specific sleep to pace the message thread.
 * When the thread resumes, it will check for new messages received.
 * Commonly sleeps for 100ms or 1s.
 * This Green implementation can be woken early by calling platform_wake_message_thread().
 */
extern void platform_message_thread_sleep( void );

/** Green-specific function to wake the sleeping messageThread.
 * It does this by signaling the semaphore that platform_message_thread_sleep()
 * is sleeping on.
 */
extern void platform_wake_message_thread( void );


/* ****************************************************************************
                                Logging Routines
 *****************************************************************************/

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
typedef void (* CustomLoggerCallback)( enum IOT_LOG_LEVELS level, const char* funcName, const char* format,
									   va_list vargs );

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

// Define macro for each log function to auto populate the calling function's name.
#ifdef IOT_ENABLE_DEBUG_LOGGING
	#define Log_Debug( s, data ... ) LOG( AZ_LOG_INFO, LOG_LINE, s, ## data );
void Log_DEBUG( const char* funcName, const char* format,  ... );

#else
	#define Log_Debug( FORMAT, ... )
#endif

#ifdef IOT_ENABLE_INFO_LOGGING
	#define Log_Info( s, data ... ) LOG( AZ_LOG_INFO, LOG_LINE, s, ## data );
void Log_INFO( const char* funcName, const char* format,  ... );

#else
	#define Log_Info( FORMAT, ... )
#endif

#ifdef IOT_ENABLE_ERROR_LOGGING
	#define Log_Error( s, data ... ) LOG( AZ_LOG_ERROR, LOG_LINE, s, ## data );
void Log_ERROR( const char* funcName, const char* format,  ... );

#else
	#define Log_Error( FORMAT, ... )
#endif

/**********************************************************************************************
 ************** Heap Allocation and Deallocation Function Redirection *************************
 *
 * In PXGreen, where the memory allocation is carefully and locally managed, we declare
 * our own special functions here that will redirect to the Ram class for handling.
 */

/** Interceptor for the malloc() function for use in the IoT SDK.
 *  Will call Ram::Allocate_Basic()
 * @param size The size in bytes to be allocated.
 * @return Pointer to the allocated memory, else nullptr on failure.
 */
void* iot_malloc( size_t size );

/** Interceptor for the free() function for use in the IoT SDK.
 *  Will call Ram::De_Allocate()
 * @param ptr A pointer to the memory block to be freed.
 */
void iot_free( void* ptr );


/**********************************************************************************************/

/* Note: comment out the following line to disable verbose logging (only applies when using HTTP)
 #define IOT_ENABLE_VERBOSE_LOGGING*/

#ifdef __cplusplus
}

/* ****************************************************************************
                               SSL Functionality
                       Only included for C++ compiling!
 *****************************************************************************/
// #include "Ssl_Context.h"
// Declare this class, but don't include here.
class Ssl_Context;

/**
 *  Provide the Babelfish SSL context to the TLS layer code.
 *  This is primarily so it can get the trusted Azure Certificate Authority.
 *
 *  @param bf_ssl_context  Pointer to the Babelfish SSL (TLS) context.
 *  @return void			Null.
 */
extern void Attach_IOT_SSL_Context( Ssl_Context* bf_ssl_context );

/**
 *  Provide ssl_context to the other IoT components.
 *
 *  @param void         Null.
 *  @return Ssl_Context*	Pointer to ssl context.
 */
extern "C" Ssl_Context* Get_IOT_SSL_Context( void );




#endif	// __cplusplus


/*************************************************************************
* DOXYGEN modules defined here, and connected to their markdown groups,
* where full descriptions are found.
*************************************************************************/

/** @defgroup iot_init_tasks IoT Initialization and Tasks
 *  @details See [IoT Code startup details markdown](../../IOT/documents/iot_startup.md)
 * 				and [Tasks and Timers design markdown](../../IOT/documents/iot_tasks_timers.md)
 */

/** @defgroup iot_dci_config IoT DCI usage and Configuration
 *  @details See [How-to and explanation of IoT configuration markdown](../../IOT/documents/iot_config.md)
 */

/** @defgroup iot_publish_cadence IoT Publishing and Group Cadence
 *  @details See [IoT Publishing and Group Cadence markdown](../../IOT/documents/iot_publishing_cadence.md)
 */

/** @defgroup iot_c2d_commands IoT Cloud-to-Device (C2D) Device Commands
 *  @details Details of C2D Commands
 */

/** @defgroup fus_over_iot Firmware Update Service (FUS) over IoT
 *  @details Details of Firmware Update through IoT.
 */

/** @defgroup alarms_over_iot Alarms over IoT
 *  @details Details of Alarms through IoT.
 */


#endif	/* IOT_DEVICE_PLATFORM_H */
