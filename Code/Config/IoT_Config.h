/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *
 *****************************************************************************************
 */
#ifndef IOT_CONFIG_H
   #define IOT_CONFIG_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
/****************** IoT Build Variables **********************************
* Defaults are declared here, but can be overridden a the product level.
*************************************************************************/
/* Sets the size of the stack for our main task, the messageThread. */
#ifndef OS_EIOT_SDK_TASK_STACK_SIZE
#define OS_EIOT_SDK_TASK_STACK_SIZE 5120
#endif

/* Define the time that the messageThread should sleep between checking for messages
 * This sets the cycle time for running the Azure DoWork functions, so it effectively
 * sets the floor for worst-case response time by adding 0 to X ms delay before the
 * DoWork function is called.
 * Setting the default here to 100ms (0.1s) for a tradeoff between responsiveness
 * and making the system overly busy. */
#ifndef IOT_MESSAGE_SLEEP_MS
#define IOT_MESSAGE_SLEEP_MS 100
#endif


/* Declare some space savers for the Azure build (even if IOT is not enabled) */
#define DONT_USE_UPLOADTOBLOB 1		// Requires HTTPS support, for pushing files

/* We need to def out Device Twin code to save space, so modify and apply this definition to the Azure code.
 * This is an Eaton modification to Azure, not an Azure #define. */
// #define DONT_USE_DEVICE_TWIN

/* We don't currently support alarms. */
#define IOT_NO_DEVICE_ALARMS

// Use this to disable logging in Azure code (saves considerable flash space)
// You can comment this out to enable the logging, during development only
// #define NO_LOGGING

#ifdef PX_GREEN_IOT_SUPPORT
	#define REFCOUNT_ATOMIC_DONTCARE	// What is this? Not used anywhere!

/* Declare Azure configuration choices */
// #define USE_EDGE_MODULES			-- This looks interesting; not sure what it is used for.

/* Defines to compile in Eaton IoT SDK logging features.
 * If you have NO_LOGGING above, don't define these either. */
	#ifndef NO_LOGGING
	#define IOT_ENABLE_DEBUG_LOGGING
	#define IOT_ENABLE_INFO_LOGGING
	#define IOT_ENABLE_ERROR_LOGGING
	#endif

/* Select only one of these protocol choices: */
// #define IOT_INCLUDE_HTTP
// #define IOT_INCLUDE_MQTT
	#define IOT_INCLUDE_MQTT_WEBSOCKETS
// #define IOT_INCLUDE_AMQP
// #define IOT_INCLUDE_AMQP_WEBSOCKETS

	#define __STDC_FORMAT_MACROS 1

/* Enable these GB_ definitions if you want Azure's debug version of malloc and its instrumentation.
 * Of course, to save space, don't enable these. */
// #define GB_DEBUG_ALLOC 1
// #define GB_MEASURE_MEMORY_FOR_THIS 1

/* IoT Cloud-to-Device command contains "who", "id", "d", "method","params" as follows:
 * {"who":"f0daa7e9-85ee-4f2c-8cce-d9eeb53db07a","id":"1702bba8-d87d-4875-82cf-0dd7f91cc8bc","d":"95cf9b98-7439-4321-9b21-e5f984350050",
 * "method":"GetChannelValues","params":{"tags":"102919"}}
 * So the length of the text up to the parameters is about 170 characters. */

/* Maximum size of "Device Command" parameters  (received as a JSON string)
 * The number of channels that can be simultaneously get/set is affected by this. If this size is increased,
 * more channels can be simultaneously set/get from the cloud.
 * By default, this value is 2048 which is too high for some PX Green instances.
 * The minimum to support the UpdateFirmware command is about 300 characters,
 * and the ImageTransfer messages will use whatever we can allow.
 * So we will use 448 by default, and 2048 for processors with more available memory.  */
#if defined ( uC_STM32F767_USAGE )
	#define IOT_MAX_COMMAND_PARAMS_SIZE     2048
#else
	#define IOT_MAX_COMMAND_PARAMS_SIZE     448
#endif

/* Maximum size of JSON token. Along with IOT_MAX_COMMAND_PARAMS_SIZE, the TOKEN_ALLOC_SIZE also affects the number of
   simultaneous channels
 * that can be set/get using Cloud-to-Device commands
 * By default, this value is 20 which is not enough to accomodate maximum channels in SetChannelsValues command.
 * Each additional channel in SetChannelsValues reqires 2 tokens. We set it to 28. With this value, the total channels
 * supported
 * by SetChannelValues command increased from 4 to 8 and the length of command is within IOT_MAX_COMMAND_PARAMS_SIZE
 * The UpdateFirmware request needs 37 tokens, so that sets the new limit to 40 */
/* Note: TOKEN_ALLOC_SIZE is used to define the size of local array tokens[TOKEN_ALLOC_SIZE].
 * Increasing this will consume more stack space, Please consult with the PX Green Team if you wish to increase token
 * alloc size */
	#define TOKEN_ALLOC_SIZE    40
/* Note for adopter: The above configuration for IOT_MAX_COMMAND_PARAMS_SIZE and TOKEN_ALLOC_SIZE may support different
   number of channels if different data types are used
 * For e.g, with the above configuration, setchannelvalues command supports 8 channels(data type UINT8) simultaneously.
 * But if STRING data type is used, then it could be possible
 * that the number of simultaneous channels supported by setchannelvalues may reduce to 7 or 6 */
#endif


#endif


