/** @file iot_agent.h Main Eaton IoT SDK header */

#ifndef IOT_DEVICE_H
#define IOT_DEVICE_H

/* Include the platform-specific header which enables\disables features,
 * including the IOT_DEVICE_LIMITS build configuration definitions.
 * If building for PX-Red, get the PX Red specific header,
 * Else get the PX Green specific header.
 * Done in this order since it is easier and more certain for PX Red to set the
 * build configuration defines.
 */
#ifdef PX_RED_DEVICE_BUILD
#include "pxred_device_platform.h"
#else
#include "iot_device_platform.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include <assert.h>

/* Eaton IoT SDK headers */
#include "iot_json_parser.h"		/* open source JSON parsing library */

/* Azure IoT SDK iothub_client headers */
#include "iothub_client.h"
#include "iothub_message.h"

#if defined ( IOT_INCLUDE_HTTP ) || !defined ( DONT_USE_UPLOADTOBLOB )
#include "iothubtransporthttp.h"
#endif
#ifdef IOT_INCLUDE_AMQP_WEBSOCKETS
#include "iothubtransportamqp_websockets.h"
#endif
#ifdef IOT_INCLUDE_MQTT_WEBSOCKETS
#include "iothubtransportmqtt_websockets.h"
#endif

/* Azure IoT SDK c-utility headers */
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/threadapi.h"

#ifdef __cplusplus
extern "C" {
#endif



/******************************************************************************/
/*                              Data Types                                    */
/******************************************************************************/

/*
   List of data types passed in the "a" meta property of the IoT Hub Message.
 */
static const char TYPE_UNKNOWN[] = "Unknown";
static const char TYPE_DEVICE_TREE[] = "DeviceTree";
static const char TYPE_DEVICES_REALTIME[] = "DevicesRealtimes";
static const char TYPE_REALTIMES[] = "Realtimes";
static const char TYPE_TRENDS[] = "Trends";
static const char TYPE_ALARMS[] = "Alarms";
static const char TYPE_TRANSFER_IMAGE[] = "dlm_job_message";
static const char TYPE_DEVICE_COMMAND_STATUS[] = "dlm_job_status";

/******************************************************************************/
/*                              API TYPES                                     */
/******************************************************************************/

/*
   List of API data types in the serialized JSON message (the "a" message property).
 */
#define API_DEVICES          "d"
#define API_DEVICE_REALTIMES "devicerealtimes"
#define API_REALTIMES        "realtimes"
#define API_TRENDS           "trends"
#define API_ALARMS           "alarms"
#define API_TRANSFER_IMAGE   "transferimage"
#define API_DEVICE_COMMAND_STATUS "devicecommandstatus"

/******************************************************************************/
/*                     CloudToDevice Request Types                            */
/******************************************************************************/

/*
   List of CloudToDevice requests.
 */
static const char REQUEST_DEVICE_COMMAND[] = "DeviceCommand";
static const char REQUEST_UPGRADE_FIRMWARE[] = "UpgradeFirmware";


/*
   List of C2D commands
 */
static const char SET_CHANNEL_VALUE_COMMAND[] = "SetChannelValue";
static const char SET_CHANNEL_VALUES_COMMAND[] = "SetChannelValues";
static const char GET_CHANNEL_VALUES_COMMAND[] = "GetChannelValues";
static const char REBOOT_COMMAND[] = "Reboot";
static const char UPDATE_FIRMWARE_COMMAND[] = "UpdateFirmware";
static const char UPDATE_CONFIGURATION_COMMAND[] = "UpdateConfiguration";
static const char UPLOAD_CONFIGURATION_COMMAND[] = "UploadConfiguration";
static const char IMAGE_TRANSFER_COMMAND[] = "ImageTransfer";
static const char QUERY_IMAGES_COMMAND[] = "QueryImages";
static const char QUERY_PROCESSOR_IMAGES_COMMAND[] = "QueryProcessorImages";//PX green
static const char ACK_ALARMS_COMMAND[] = "AckAlarms";
static const char CLOSE_ALARMS_COMMAND[] = "CloseAlarms";//PX green
static const char CLEAR_ALARM_HISTORY_COMMAND[] = "ClearAlarmHistory";//PX green

/*
   Sizes of the C2D command strings
 */
static const uint8_t SIZE_UPDATE_FIRMWARE_STR = ( sizeof( UPDATE_FIRMWARE_COMMAND ) - 1 );
static const uint8_t SIZE_IMAGE_TRANSFER_STR = ( sizeof( IMAGE_TRANSFER_COMMAND ) - 1 );
static const uint8_t SIZE_QUERY_IMAGES_STR = ( sizeof( QUERY_IMAGES_COMMAND ) - 1 );
static const uint8_t SIZE_GET_CHANNEL_VALUES_STR = ( sizeof( GET_CHANNEL_VALUES_COMMAND ) - 1 );
static const uint8_t SIZE_SET_CHANNEL_VALUES_STR = ( sizeof( SET_CHANNEL_VALUES_COMMAND ) - 1 );
static const uint8_t SIZE_ACK_ALARMS_STR = ( sizeof( ACK_ALARMS_COMMAND ) - 1 );
#ifdef PX_GREEN_IOT_DEVICE
static const uint8_t SIZE_QUERY_PROCESSOR_IMAGES_STR = ( sizeof( QUERY_PROCESSOR_IMAGES_COMMAND ) - 1 );
static const uint8_t SIZE_CLOSE_ALARMS_STR = ( sizeof( CLOSE_ALARMS_COMMAND ) - 1 );
static const uint8_t SIZE_CLEAR_ALARM_HISTORY_STR = ( sizeof( CLEAR_ALARM_HISTORY_COMMAND ) - 1 );
#endif

/******************************************************************************/
/*                              Type Defs                                     */
/******************************************************************************/

/**
 * Enumeration of device error codes.
 */
typedef enum
{
	IOT_OK,									/*!< Operation was successful */
	IOT_INVALID_HANDLE,						/*!< Invalid device handle argument passed to function */
	IOT_INVALID_ARGUMENT,					/*!< Invalid\missing argument passed to function */
	IOT_SEND_ERROR,							/*!< Error occurred sending message to IoT Hub */
	IOT_NO_AVAILABLE_MESSAGE_BUFFER,		/*!< No available message buffer availavble for sending message to IoT Hub */
	IOT_SERIALIZE_ERROR,					/*!< Error occurred serializing message */
	IOT_PLATFORM_INIT_FAILED,				/*!< Error occurred initializing the platform */
	IOT_CONNECTION_FAILED,					/*!< Error occurred connecting to IoT Hub */
	IOT_THREAD_CREATION_FAILED,				/*!< Error creating message processing thread */
	IOT_SET_MESSAGE_CALLBACK_FAILED,		/*!< Error registering message callback function */
	IOT_SET_DEVICE_COMMAND_CALLBACK_FAILED,	/*!< Error registering device command callback function */
	IOT_DUPLICATE_CONNECTION,				/*!< Connection failed due to duplicate active connection */
	IOT_SEND_REQUEST_REJECTED,				/*!< Maximum number of unconfirmed messages has been reached */
	IOT_UPLOAD_TO_BLOB_FAILED				/*!< Error occurred uploading to blob storage */
} IOT_DEVICE_ERROR_CODES;

/**
 * Enumeration of device connection statuses.
 * These are mapped from the protocol-specific client statuses or connection states,
 * and the handleConnectionStatusCallback statusCode and reasonCode, according to this:
 *
 * | SDK Status     | MQTT_CLIENT_STATUS_ | AMQP_STATE_           | statusCode      | reasonCode |
 * | -------------- | --------------------| --------------------- | ----------------| ---------- |
 * | IOT_UNKNOWN    |                     |                       |                 |            |
 * | IOT_OPENED     | CONNECTED           | CONNECTED             | AUTHENTICATED   | OK         |
 * | IOT_CLOSING    | PENDING_CLOSE       |                       | UNAUTHENTICATED | ?          |
 * | IOT_CLOSED     | NOT_CONNECTED       | DISCONNECTED or ERROR | UNAUTHENTICATED | (the rest) |
 * | IOT_CONNECTING | CONNECTING          | CONNECTING            | UNAUTHENTICATED | NO_NETWORK |
 * | IOT_OTHER      |                     | IDLE                  |                 |            |
 *
 */
typedef enum
{
	IOT_UNKNOWN,						/*!< Unknown connection status */
	IOT_OPENED,							/*!< Connection is active */
	IOT_CLOSING,						/*!< Connection is in the process of closing */
	IOT_CLOSED,							/*!< Connection is closed */
	IOT_CONNECTING,						/*!< Connection attempting to be established. */
	IOT_OTHER							/*!< Other connection status (i.e. possible intermittent communication errors */
} IOT_CONNECTION_STATUS_CODES;

/**
 * Enumeration of data types.  Identifies the type of data in an IOT_DATA structure.
 */
typedef enum
{
	UNKNOWN_DATA_TYPE,					/*!< Unknown data type */
	DEVICE_TREE,						/*!< List of devices and child devices */
	DEVICES_REALTIME,					/*!< List of device states */
	CHANNEL_REALTIMES,					/*!< List of channel values and status */
	TRENDS,								/*!< List of channel time series values */
	ALARMS								/*!< List of alarms */
} IOT_DATA_TYPE;

/**
 * Enumeration of command types.  Identifies the type of command in an IOT_COMMAND structure.
 */
typedef enum
{
	UNKNOWN_COMMAND_TYPE,				/*!< Unknown data type */
	TRANSFER_IMAGE,						/*!< Start the image transfer (cloud-to-device) */
	DEVICE_COMMAND_STATUS				/*!< Update device command status */
} IOT_COMMAND_TYPE;

/**
 * Enumeration of command status.
 */
typedef enum
{
	UNKNOWN_COMMAND_STATUS,				/*!< Unknown device command status */
	COMMAND_INITIATED,					/*!< Device command initiated */
	COMMAND_PROCESSING,					/*!< Device command processing */
	COMMAND_SUCCEEDED,					/*!< Device command was successful */
	COMMAND_FAILED,						/*!< Device command Failed */
} IOT_COMMAND_STATUS;

/**
 * DEPRECATED: Enumeration of Cloud-to-Device request types.  Identifies the type of request in an IOT_REQUEST structure.
 * Only DEVICE_COMMAND was used.
 */
typedef enum
{
	UNKNOWN_REQUEST_TYPE,				/*!< Unused: Unknown request type */
	GET_DEVICE_TREE,					/*!< Unused: Request to publish device list */
	GET_DEVICES_REALTIME,				/*!< Unused: Request to publish device states */
	GET_CHANNEL_REALTIMES,				/*!< Unused: Request to publish channel values and states */
	GET_TRENDS,							/*!< Unused: Request to publish channel time series values */
	DEVICE_COMMAND						/*!< Request to process device command */
} IOT_REQUEST_TYPE;

/**
 * Enumeration of supported IoT Hub transport protocols.
 */
typedef enum
{
	HTTP,								/*!< HTTP protocol */
	AMQP,								/*!< AMQP protocol */
	AMQP_WEBSOCKETS,					/*!< AMQP over Websockets protocol */
	MQTT,								/*!< MQTT protocol */
	MQTT_WEBSOCKETS						/*!< MQTT over Websockets protocol */
} IOT_PROTOCOL;


/*
   Handle to an IoT Hub connection.
 */
typedef IOTHUB_CLIENT_LL_HANDLE IOT_DEVICE_HANDLE;

/*
   Structure containing session information for an IoT Hub connection.
 */
typedef struct iot_device_connection IOT_DEVICE_CONNECTION;

/**
 * Structure containing the status of a device connection with IoT Hub.
 * This structure is populated with a call to function "iot_getStatus".
 */
typedef struct iot_connection_status IOT_CONNECTION_STATUS;

/**
 * Structure containing device connection options used to initialize a
 * connection with IoT Hub.
 * This structure is passed as a parameter to function "iot_open".
 */
typedef struct iot_connection_options IOT_CONNECTION_OPTIONS;

/**
 * Structure containing one or more data entities (i.e. DeviceTree, DeviceRealtime,
 * ChannelRealtime, Trend etc).
 * This structure is passed as a parameter to the function "iot_send".
 */
typedef struct iot_data IOT_DATA;

/**
 * Structure containing a command (i.e. InitiateBlobTransfer, etc.)
 * This structure is passed as a parameter to the function "iot_send_command".
 */
typedef struct iot_command IOT_COMMAND;

/**
 * Data structure representing a channel trend value(s).
 */
typedef struct iot_data_trend_item IOT_DATA_TREND_ITEM;

/**
 * Data structure representing a device and its optional list of child devices.
 */
typedef struct iot_data_device_item IOT_DATA_DEVICE_ITEM;

/**
 * Data structure representing the status and latest value of a channel.
 */
typedef struct iot_data_channel_realtimes_item IOT_DATA_CHANNEL_REALTIMES_ITEM;

/**
 * Data structure representing the status of a device.
 */
typedef struct iot_data_devices_realtime_item IOT_DATA_DEVICES_REALTIME_ITEM;

#ifndef IOT_NO_DEVICE_ALARMS

/**
 * Data structure representing an alarm.
 */
typedef struct iot_data_alarm_item IOT_DATA_ALARM_ITEM;

/**
 * Data structure representing an alarm trigger.
 */
typedef struct iot_data_alarm_trigger_item IOT_DATA_ALARM_TRIGGER_ITEM;

/**
 * Data structure representing an alarm trigger.
 */
typedef struct iot_data_alarm_custom_item IOT_DATA_ALARM_CUSTOM_ITEM;

#endif

/**
 * Structure representing a Device-to-Cloud message.
 * This is an internal data structure used to send messages to IoT Hub.
 */
typedef struct iot_message IOT_MESSAGE;

/**
 * DEPRECATED: Structure representing a Cloud-to-Device request.
 * This structure is passed as a parameter to the "CloudToDevice" message
 * callback.
 */
typedef struct iot_request IOT_REQUEST;

/**
 * Structure representing a Cloud-to-Device device command.
 * This structure is passed as a parameter to the "CloudToDevice" device
 * command callback.
 */
typedef struct iot_device_command IOT_DEVICE_COMMAND;

/**
 * Structure representing a Cloud-to-Device device command response.
 * This structure is returned from the "CloudToDevice" device command callback.
 */
typedef struct iot_device_command_response IOT_DEVICE_COMMAND_RESPONSE;


/******************************************************************************/
/*                         Callback Signatures                                */
/******************************************************************************/

/**
 * Typedef for the Implementor process' Callback function to receive Cloud-to-Device device commands.
 * This "C" function pointer is passed as a parameter to the function "iot_registerC2DDeviceCommandCallback".
 * This is the preferred C2D Command type, which includes a response.
 * @ingroup iot_sdk_callbacks
 *
 * @param devicehandle              A handle to the Low-Level Azure device connection.
 * @param ownerContext				An opaque pointer to the owning device's context.
 * @param command                   A pointer to the command structure from Azure (refer to iot_device_command).
 * @param commandResponse           A pointer to the response structure (to be filled in by the implementor's code).
 *                                  Note: The allocated fields for the IOT_DEVICE_COMMAND_RESPONSE structure will be freed
 *                                        by the SDK after serializing the response for Azure.
 *                                        The implementor's callback must allocate any const char* members used.
 */
typedef void (*C2DDeviceCommandCallback)( IOT_DEVICE_HANDLE deviceHandle, void *ownerContext,
										  const IOT_DEVICE_COMMAND *command, IOT_DEVICE_COMMAND_RESPONSE *commandResponse );


/**
 * Callback function invoked at specified intervals by our messageThread.
 * This "C" function pointer is passed as a parameter to the function "iot_registerTimerCallback".
 * @ingroup iot_sdk_callbacks
 *
 * @param devicehandle              A handle to the Low-Level Azure device connection.
 * @param ownerContext				An opaque pointer to the owning device's context.
 */
typedef void (*TimerCallback)( IOT_DEVICE_HANDLE deviceHandle, void *ownerContext );

/**
 * Callback function invoked when the status of the connection to IoT Hub changes.
 * This "C" function pointer is passed as a parameter to the function "iot_registerConnectionStatusCallback".
 * @ingroup iot_sdk_callbacks
 *
 * Enumeration of status codes (defined in Azure's iothub_client_core_common.h)
 *
 *  #define IOTHUB_CLIENT_CONNECTION_STATUS_VALUES
 *      IOTHUB_CLIENT_CONNECTION_AUTHENTICATED,
 *      IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED
 *
 * Enumeration of reason codes:
 *
 *  #define IOTHUB_CLIENT_CONNECTION_STATUS_REASON_VALUES
 *      IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN,
 *      IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED,
 *      IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL,
 *      IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED,
 *      IOTHUB_CLIENT_CONNECTION_NO_NETWORK,
 *      IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR,
 *      IOTHUB_CLIENT_CONNECTION_OK
 *
 * @param devicehandle              A handle to the device connection.
 * @param ownerContext				An opaque pointer to the owning device's context.
 * @param statusCode                Code representing the status of the connection.
 * @param reasonCode                Code representing the reason code of the connection.
 */
typedef void (*ConnectionStatusCallback)( IOT_DEVICE_HANDLE deviceHandle, void *ownerContext,
										  IOTHUB_CLIENT_CONNECTION_STATUS statusCode,
										  IOTHUB_CLIENT_CONNECTION_STATUS_REASON reasonCode );

/**
 * @brief Callback template for a handler to be notified when a message is Done, either in success or failure.
 *  This optional handler can be registered by the platform so the application can know when a message has
 *  been confirmed (on success) or dropped (on failure).
 *  This "C" function pointer is passed as a parameter to the function "iot_registerMessageDoneCallback".
 * @ingroup iot_sdk_callbacks
 *
 * @param ownerContext	An opaque pointer to the owning device's context.
 * @param result The status of the message; one of IOTHUB_CLIENT_CONFIRMATION_OK, _MESSAGE_TIMEOUT, _BECAUSE_DESTROY,
 *                  or _ERROR (the latter usually meaning that the daily limit has been reached).
 * @param message The message which has been finished.
 */
typedef void (*MessageDoneCallback)( void *ownerContext, IOTHUB_CLIENT_CONFIRMATION_RESULT result, IOT_MESSAGE *message );


/******************************************************************************/
/*                    Cloud-to-Device Device Command                          */
/******************************************************************************/

/**
 * Structure representing a Cloud-to-Device device command.
 * This structure is passed as a parameter to the "CloudToDevice" device command callback.
 */
struct iot_device_command
{
	/** String to identify the command instance (i.e. the "Job" UUID). This generated ID is passed in from the cloud. */
	char commandID[IOT_MAX_DEVICE_UUID_SIZE];
	/** List of Device UUIDs that the command is targeting. (Normally  just a single Device UUID.) */
	SINGLYLINKEDLIST_HANDLE commandDevices;
	/** Command method name (i.e. SetChannelValue). */
	char *commandMethod;
	/** Command parameters in JSON format (i.e. {"tag":"<channel-tag>","v":"<channel-value>"} ). */
	char *commandParams;
	/** User who issued the command. Usually given as a UUID that identifies that User. */
	char commandWho[IOT_MAX_DEVICE_UUID_SIZE];
	/** Command version. Usually given as a integer. */
	uint32_t commandVersion;
};


/**
 * Structure representing a Cloud-to-Device device command response.
 * This structure will be filled in by the implementor's "CloudToDevice" device command callback.
 * The const char* members here will be freed by the SDK (if they are non-null),
 * so their contents should all be allocated by the implementor's callback.
 */
struct iot_device_command_response
{
	int status;						/*!< Device command response status code (ie. 200, 400, etc.) */
	const char *msg;				/*!< Device command response message */
	const char *arg_1;				/*!< Char argument #1 (specific to each command) (optional) */
	const char *arg_2;				/*!< Char argument #2 (specific to each command) (optional) */
	const char *arg_3;				/*!< Char argument #3 (specific to each command) (optional) */
	const char *arg_4;				/*!< Char argument #4 (specific to each command) (optional) */
	/** Optional List argument #1 (specific to each command).
	 *  The SDK will create/initialize this list before invoking the implementor's callback. */
	SINGLYLINKEDLIST_HANDLE list_1;
	/** In addition to freeing list_1 itself, free the contents ("items") of list_1 after response is processed.
	 *  Default is false */
	bool list_1_cleanup;
	SINGLYLINKEDLIST_HANDLE list_2;	/*!< List argument #2 (specific to each command) (optional) */
	bool list_2_cleanup;			/*!< Free contents of list_2 after response is processed. Default is false */
	/** Optional field to show the size of the buffer allocated for the msg member.
	 *  Added for the convenience of implementor code; not serialized by the SDK.
	 *  If set to 0, then any contents pointed to by msg are static, not allocated, text, and should not be freed. */
	size_t msg_size;
};


/******************************************************************************/
/*                Device-to-Cloud Message (Internal)                          */
/******************************************************************************/

/**
 * Structure representing a Device-to-Cloud message.
 * This is an internal data structure used to send messages to IoT Hub.
 *
 * When given to Azure code, the contents pointed to by serializedMessage
 * are duplicated by the Azure code.
 * PXGreen normally uses pre-allocated storage for serializedMessage buffers,
 * but for custom messages serializedMessage can point to malloc'd memory
 * which must be free when done. This struct should be freed as well in that case.
 */
struct iot_message
{
	IOTHUB_MESSAGE_HANDLE messageHandle;	/*!< Handle to IoT Hub message */
	void *connection;				/*!< Pointer to message context */
	unsigned long messageId;		/*!< Unique Id assigned to the message */
	int serializedBufferSize;		/*!< Length of message after serializing */
	char *serializedMessage;		/*!< Serialized message payload */
#ifdef PX_GREEN_IOT_DEVICE
	bool inUse;						/*!< Flag indicating if message buffer is in use (i.e. message sent but not confirmed). */
	bool isAllocated;				/*!< Flag indicating if serializedMessage points to allocated memory which must be freed. */
#endif
};


/******************************************************************************/
/*                     IOT Device Connection (Internal)                       */
/******************************************************************************/
struct iot_device_connection
{
	char deviceUUID[IOT_MAX_DEVICE_UUID_SIZE];			/**< Publisher Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */
	void *context;										/**< Opaque pointer to the owner's context (eg, class instance). */
	IOTHUB_CLIENT_LL_HANDLE handle;						/**< A handle to the IoT Hub device connection. */
	C2DDeviceCommandCallback c2dDeviceCommandCallback;	/**< Address of "C" CloudToDevice device command callback */
	TimerCallback timerCallback;						/**< Address of "C" TIMER function callback */
	int timerCallbackInterval;							/**< Timer callback interval (in seconds) */
	ConnectionStatusCallback connectionStatusCallback;	/**< Address of "C" connection status callback */
	THREAD_HANDLE messageThreadId;						/**< Thread Id of message thread */
	bool close;											/**< Function "iot_close" has been invoked */
	IOT_CONNECTION_STATUS_CODES connectionStatus;		/**< Status of IoT Hub connection */
	long msgOutbound;									/**< Number of Device-to-Cloud messages sent to IoT Hub */
	long msgConfirmed;									/**< Number of confirmed Device-to-Cloud messages sent to IoT Hub */
	long msgInbound;									/**< Number of Cloud-to-Device messages received */
	long cmdInbound;									/**< Number of Cloud-to-Device device commands received */
	time_t lastTimerCallbackInvoked;					/**< Last time the "TIMER" callback was invoked */
	IOT_DEVICE_ERROR_CODES lastErrorCode;				/**< last SDK error code */
	int unconfirmedMessageCount;						/**< number of unconfirmed sent messages (consecutive) */
	IOTHUB_CLIENT_CONNECTION_STATUS connectionStatusCode;			/* IoT Hub connection status code */
	IOTHUB_CLIENT_CONNECTION_STATUS_REASON connectionReasonCode;	/* IoT Hub connection reason code */
	MessageDoneCallback messageDoneCallback;			/**< Optional Callback to be notified when a Message is Done. */
#ifdef PX_GREEN_IOT_DEVICE
	IOT_MESSAGE iotMessage[IOT_MAX_CONCURRENT_MESSAGES];	/* List of message buffers */
#else
	bool wantBatching;									/**< Do batching of messages when sending, if AMQP. */
	bool sendImmediately;								/**< Override batching and publish all batched messages now. */
	uint32_t doWorkIntervalSeconds;						/**< DoWork cycle time. */
#endif
};



/******************************************************************************/
/*              Device SDK Serialization Methods (Internal)                   */
/******************************************************************************/

/*
   Serialize data entities to Json to be transmitted from Device-to-Cloud.

   @param connection                A pointer to the device connection.
   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeData( IOT_DEVICE_CONNECTION *connection, IOT_MESSAGE *iotMessage, const IOT_DATA *data );

/*
   Serialize list of devices to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeDeviceTreeJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data );

/*
   Serialize a single device to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param dataDevice                The device data entity to serialize.
   @param firstDevice               Flag indicating whether this is the first device in the list.

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeDeviceJson( IOT_MESSAGE *iotMessage, const IOT_DATA_DEVICE_ITEM *dataDevice, bool firstDevice );

/*
   Serialize a list of device states to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeDevicesRealtimeJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data );

/*
   Serialize device state for a single device to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The device realtime data entity to serialize.
   @param firstItem                 Flag indicating whether this is the first data item in the list.

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeDeviceRealtimeJson( IOT_MESSAGE *iotMessage, const IOT_DATA_DEVICES_REALTIME_ITEM *data, bool firstItem );

/*
   Serialize a list of channel states and values to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeChannelRealtimesJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data );

/*
   Serialize a single channel state and value to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param deviceUUID                The Device Id in GUID format: 00000000-0000-0000-0000-000000000000.
   @param data                      The Channel Realtime data entity to serialize.
   @param firstItem                 Flag indicating whether this is the first data item in the list.

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeChannelRealtimeJson( IOT_MESSAGE *iotMessage, const char *deviceUUID,
										  const IOT_DATA_CHANNEL_REALTIMES_ITEM *data, bool firstItem );

/*
   Serialize timeseries data for a list of channels to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeTrendsJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data );

/*
   Serialize timeseries data for a single channel to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param device UUID               The Device Id in GUID format: 00000000-0000-0000-0000-000000000000.
   @param data                      The trend data entity to serialize.
   @param firstItem                 Flag indicating whether this is the first data item in the list.

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeTrendJson( IOT_MESSAGE *iotMessage, const char *deviceUUID, const IOT_DATA_TREND_ITEM *data, bool firstItem );

#ifndef IOT_NO_DEVICE_ALARMS

/*
   Serialize alarm data for a list of alarms to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param data                      The data entities to be serialized (refer to iot_data).

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeAlarmsJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data );

/*
   Serialize alarm data for a single alarm to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param device UUID               The Device Id in GUID format: 00000000-0000-0000-0000-000000000000.
   @param data                      The alarm data entity to serialize.
   @param firstItem                 Flag indicating whether this is the first data item in the list.

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeAlarmJson( IOT_MESSAGE *iotMessage, const char *deviceUUID, const IOT_DATA_ALARM_ITEM *data, bool firstItem );

#endif

/*
   Serialize command to Json to be transmitted from Device-to-Cloud.

   @param connection                A pointer to the device connection.
   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param command                   The command to be serialized (refer to iot_command).

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeCommand( IOT_DEVICE_CONNECTION *connection, IOT_MESSAGE *iotMessage, const IOT_COMMAND *command );

/*
   Serialize TransferImage command to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param command                   The command entity to serialize.

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeTransferImageJson( IOT_MESSAGE *iotMessage, const IOT_COMMAND *command );

/*
   Serialize a D2C DeviceCommandStatus command to Json.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param command                   The command entity to serialize.

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeDeviceCommandStatusJson( IOT_MESSAGE *iotMessage, const IOT_COMMAND *command );


/*
   Serialize a C2D device command response to Json.

   @param method                    The device command method.
   @param commandResponse           The device command response returned from the device command callback.
   @param response                  The json formatted response, allocated and returned to IoT Hub.
   @param responseSize              The size of the response returned to IoT Hub.

   @return bool                     True if serialization was successful else False.
 */
extern bool serializeDeviceCommandResponse( const char *method, IOT_DEVICE_COMMAND_RESPONSE *commandResponse,
											unsigned char * *response, size_t *responseSize );

/*
   Append a open brace string field to a Json buffer

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the open brace field.
 */
extern void addOpenBraceStringField( char *buffer, const char *name );

/*
   Append a string value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
 */
extern void addStringField( char *buffer, const char *name, const char *value, bool includeTrailingComma );

/*
   Append a numeric (int) value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
 */
extern void addIntNumericField( char *buffer, const char *name, const int *value, bool includeTrailingComma );

/*
   Append a numeric (unsigned int) value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
 */
extern void addUIntNumericField( char *buffer, const char *name, const unsigned int *value, bool includeTrailingComma );

/*
   Append a numeric (long) value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
 */
extern void addLongNumericField( char *buffer, const char *name, const long *value, bool includeTrailingComma );

/*
   Append a numeric (unsigned long) value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
 */
extern void addULongNumericField( char *buffer, const char *name, const unsigned long *value, bool includeTrailingComma );

/*
   Append a boolean value to a Json buffer.

   @param buffer                A pointer to a character string to append the value to.
   @param name                  The name of the field.
   @param value                 The value of the field.
   @param includeTrailingComma  Flag indicating whether a trailing comma should be added to the buffer.
 */
extern void addBooleanField( char *buffer, const char *name, const bool *value, bool includeTrailingComma );

/*
   Deserialize a Cloud-to-Device Json device command request.

   @param connection                A pointer to the device connection.
   @param msg                       Pointer to character string containing the command.
   @param len                       Length of command message.
   @param command                   Pointer to IOT_DEVICE_COMMAND structure to be populated.
                                    If deserialization fails, the command->method will be null.
 */
extern void deserializeCommand( IOT_DEVICE_CONNECTION *connection, const char *msg, int len, IOT_DEVICE_COMMAND *command );


#ifndef PX_GREEN_IOT_DEVICE
/*
   Allocates memory for a serialized message buffer.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.

   @return bool                     True if memory was successfully allocated else False.
 */
extern bool allocateSerializedBuffer( IOT_MESSAGE *iotMessage );

/*
   Checks if memory for a serialized message buffer needs to be reallocated.

   @param iotMessage                A pointer to the message to be transmitted to IoT Hub.
   @param dataEntitySize            Size of data entity to be serialized and added to the message buffer.

   @return bool                     True if buffer size was sufficient or memory was successfully reallocated else False.
 */
extern bool checkSerializedBufferSize( IOT_MESSAGE *iotMessage, int dataEntitySize );

#ifndef PX_GREEN_IOT_DEVICE
/**
 *  Test function to print the serialized Message buffer.
 *  Requires debug level IOT_LOG_INFO to be set in order to see output on stdout
 * @param data The IOT_DATA structure containing the data to be serialized for the message. Is NULL if this is a Command message.
 */
extern void iot_print_message( const IOT_DATA *data );

#endif
#endif

/******************************************************************************/
/*                  Device SDK Misc Methods (Internal)                        */
/******************************************************************************/

/**
   Allocates and initializes the members of the IoT Hub connection structure, given its options and defaults - a constructor.
   Saves the resulting connection internally for SDK use; will free any previous connection first.
   While this is intended to be an internal-only function, it is exported for use by test code.
   @param handle Low-Level handle to Azure
   @param connectionOptions The options to use for this connection, given by the owning device.
   @param owner_context An opaque pointer to the owner device's choice of context (eg, class instance)
   @return The Connection structure, filled out with defaults and the options (but not the callbacks) on success, else NULL on failure.
            This is the same as that stored internally for SDK use.
 */
extern IOT_DEVICE_CONNECTION *constructConnection( IOTHUB_CLIENT_LL_HANDLE handle, const IOT_CONNECTION_OPTIONS *connectionOptions,
												   void *owner_context );

/**
 *  Returns the IOT_DEVICE_CONNECTION associated with an IOT_DEVICE_HANDLE.
 *  Will log an error if the given deviceHandle is NULL
 *  or not found in the existing internal connection structure.
 *
 *  Originally, there could be more than one connection maintained by this SDK,
 *  but the reality was that this never happened in practice, so the concept
 *  of multiple connections was removed to simplify this function.
 *
 *  @param deviceHandle A handle to the publisher device for this connection.
 *  @return The existing connection for the publisher, if valid;
 *             returns NULL if deviceHandle or the connection are invalid.
 */
extern IOT_DEVICE_CONNECTION *getConnection( IOT_DEVICE_HANDLE deviceHandle );


/*
   Determines if a connection associated with a Device UUID exists.
 */
extern bool connectionExists( const char *deviceUUID );

/*
   Deletes our IOT_DEVICE_CONNECTION and associated memory.
 */
extern void deleteConnection( IOT_DEVICE_CONNECTION *connection );

/*
   Looks up the data type associated with a data entity structure.
 */
extern const char *getDataType( const IOT_DATA *data );

/*
   Looks up the command type associated with a command entity structure.
 */
extern const char *getCommandType( const IOT_COMMAND *command );

/*
   Validates the contents of IOT_CONNECTION_OPTIONS.
 */
extern bool validateConnectionOptions( const IOT_CONNECTION_OPTIONS *connectionOptions, int *status );

/*
   Initializes global variables.
 */
extern void initializeGlobalVariables(void);

/*
   Performs platform initialization.
 */
extern bool initializePlatform( int *status );

/**
   Initializes an IoT Hub connection structure, given its options.
   @param handle Low-Level handle to Azure
   @param connectionOptions The options to use for this connection, given by the owning device.
   @param owner_context An opaque pointer to the owner device's choice of context (eg, class instance)
   @return The Connection structure, partially filled out (not the callbacks) on success, else NULL on failure.
 */
extern IOT_DEVICE_CONNECTION *initializeConnection( IOTHUB_CLIENT_LL_HANDLE handle, const IOT_CONNECTION_OPTIONS *connectionOptions,
													void *owner_context );

/** Creates an IoT Hub message, either by allocating memory for it (PX Red) or choosing the next available message in the pool (PX Green).
 *  Serializes the Data or Command information (only one of these must be provided).
 *
 * @param connection The connection structure.
 * @param data The IOT_DATA structure containing the data to be serialized for the message. Is NULL if this is a Command message.
 * @param command The IOT_COMMAND structure containing the command to be serialized for the message. Is NULL if this is a Data message.
 * @return Pointer to the message created and serialized on success; is NULL on failure.
 *          If PX Red, is allocated and must be freed when done; if PX Green, points to the message in the pool.
 */
IOT_MESSAGE *createMessage( IOT_DEVICE_CONNECTION *connection, const IOT_DATA *data, const IOT_COMMAND *command );

/** Creates an IoT Hub message by allocating memory for it, and copying in the serialized message contents.
 *
 * @param connection The connection structure.
 * @param serialized_message The serialized JSON Object to be sent as the message.
 * @return Pointer to the message created and serialized on success; is NULL on failure.
 *         This must be freed when done, normally in releaseIotMessage().
 *         On return, iotMessage->serializedMessage points to the given serialized_message,
 *         and iotMessage->messageHandle holds an allocated copy of the same contents to be sent to Azure.
 *         For PXGreen, if iotMessage->serializedMessage (serialized_message) points to allocated memory,
 *         it should be freed and nulled on return.
 */
IOT_MESSAGE *createCustomMessage( IOT_DEVICE_CONNECTION *connection, const char *serialized_message );

/** Deletes or marks unused an IoT Hub message, freeing any allocated resources.
 *  @param iotMessage The message to be released.
 */
void releaseIotMessage( IOT_MESSAGE *iotMessage );

/**
 * @brief Set Iot Hub message properties, such as message type and (optionally) parent device.
 * The properties are determined by fields of the given data or command;
 * only one of these should be provided, and the other should be null.
 * The properties are set in the properties map of the iotMessage.
 *
 * @param connection The IoT Connection structure
 * @param iotMessage The payload (the message) whose properties are being set.
 * @param data The data input that will be used for this message, if a data message.
 * @param command The data that will be used for this message, if a command message.
 */
void setMessageProperties( IOT_DEVICE_CONNECTION *connection, IOT_MESSAGE *iotMessage,
						   const IOT_DATA *data, const IOT_COMMAND *command );

/**
 * @brief Set Custom Iot Hub message properties from the given array of properties.
 * The properties are determined by fields of the given data or command;
 * only one of these should be provided, and the other should be null.
 * The properties are set in the properties map of the iotMessage.
 *
 * @param iotMessage The payload (the message) whose properties are being set.
 * @param properties An array of key-values pairs for properties of the message, to be used in the topic path when sending.
 *                   Each pair takes two places in this array; the first is the key, the second the associated value.
 * @param num_properties How many key-value property pairs are included in the properties array
 *                       (So the number of array entries is twice this number)
 */
void setCustomMessageProperties( IOT_MESSAGE *iotMessage, const char *properties[], size_t num_properties );

/** For PXGreen, gets the next available message buffer.
 * @param connection The IoT Connection structure
 * @return Pointer to the message from the PX Green message pool.
 *          If no message buffer is available, returns nullptr.
 */
IOT_MESSAGE *getNextMessage( IOT_DEVICE_CONNECTION *connection );

/** For PXGreen, gets the count of available message buffers.
 * @param deviceHandle A handle to the device connection.
 * @return Count of available message buffers, up to IOT_MAX_CONCURRENT_MESSAGES.
 *          If no message buffer is available, returns 0.
 */
extern uint8_t getFreeMessageCount( IOT_DEVICE_HANDLE deviceHandle );



/******************************************************************************/
/*                  Base64 Encoding and Decoding functions                    */
/******************************************************************************/
#ifdef WANT_BASE64_ENCODING
/**
 *  Encodes binary data to a base64 string.
 *
 *  @param data                  Pointer to binary input.
 *  @param input_length          Length of input data.
 *  @param output_length         Length of output data.
 *
 *  @return char*                The encoded string.
 */
extern char *iot_base64_encode( const unsigned char *data, size_t input_length, size_t *output_length );

/**
 *  Decodes a base64 string to binary.
 *
 *  @param data                  Pointer to base64 input.
 *  @param input_length          Length of input data.
 *  @param output_length         Length of output data.
 *
 *  @return char*                The decoded binary data.
 */
extern unsigned char *iot_base64_decode( const char *data, size_t input_length, size_t *output_length );

/*
   Build base64 decoding table.
 */
extern void build_decoding_table(void);

/*
   Free memory for base64 decoding table.
 */
extern void base64_cleanup(void);

#endif


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*                       Device SDK Interface                                 */
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/*                            Structures                                      */
/******************************************************************************/

/******************************************************************************/
/*                  IOT Device Connection Options                             */
/******************************************************************************/

/**
 * Structure containing device connection options used to initialize a
 * connection with IoT Hub.
 * This structure is passed as a parameter to function "iot_open".
 * @ingroup connection_status_apis
 */
struct iot_connection_options
{
	IOT_PROTOCOL protocol;			/*!< Communication protocol (HTTP,MQTT,MQTT_WEBSOCKETS) */
	const char *deviceUUID;			/*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */
	const char *connectionString;	/*!< Connection string to IoT Hub instance (format: HostName=<name>;DeviceId=<00000000-0000-0000-0000-000000000000>;SharedAccessKey=<key> ) */
	const char *proxyAddress;		/*!< Proxy IP address, or NULL if no proxy. (Empty string is invalid.) */
	const char *proxyUser;			/*!< User name to pass to proxy, or NULL. */
	const char *proxyPassword;		/*!< Password to pass to proxy, or NULL. */
	int proxyPort;					/*!< Proxy port (zero to use default */
};

/******************************************************************************/
/*                  IOT Device Connection Status                              */
/******************************************************************************/

/**
 * Structure containing the status of a device connection with IoT Hub.
 * This structure is populated with a call to function "iot_getStatus".
 * @ingroup connection_status_apis
 */
struct iot_connection_status
{
	IOT_CONNECTION_STATUS_CODES connectionStatus;	/*!< IoT Hub connection status (refer to iot_connection_status_codes) */
	long msgOutbound;								/*!< Number of messages sent to IoT Hub */
	long msgConfirmed;								/*!< Number of message confirmations received from IoT Hub */
	long msgInbound;								/*!< Number of messages received from IoT Hub */
	long cmdInbound;								/*!< Number of device commands received from IoT Hub */
	IOT_DEVICE_ERROR_CODES lastErrorCode;			/*!< Last SDK error code (refer to iot_device_error_codes) */
};

/******************************************************************************/
/*                        IOT Device Data Entities                            */
/******************************************************************************/

/**
 * Structure containing the data to be published for one message type (i.e. DeviceTree, DeviceRealtime,
 * ChannelRealtime, Trend etc) from one device.
 * The values or data are stored in an Azure list of items.
 * This structure is passed as a parameter to the function "iot_send".
 * @ingroup message_handling_apis
 */
struct iot_data
{
	/** Type of data entity (refer to iot_data_type), which indicates which one of the list types is provided below. */
	IOT_DATA_TYPE dataType;

	/** Device Id in GUID format: 00000000-0000-0000-0000-000000000000 or NULL.
	 *  This points to some storage for the UUID text, and should not be freed. */
	const char *deviceUUID;

	/** (optional) The name of a special routing path in the cloud for this message; if NULL, then no special routing is to be done. */
	const char *routingName;

	/** Handle to list of device entities (refer to iot_data_device_item) or NULL if dataType isn't DEVICE_TREE */
	SINGLYLINKEDLIST_HANDLE devices;

	/** Handle to list of device realtime entities (refer to iot_data_devices_realtime_item) or NULL if dataType isn't DEVICES_REALTIME */
	SINGLYLINKEDLIST_HANDLE devicesRealtime;

	/** Handle to list of channel realtime entities (refer to iot_data_channel_realtimes_item) or NULL if dataType isn't CHANNEL_REALTIMES */
	SINGLYLINKEDLIST_HANDLE channelRealtimes;

	/** Handle to list of trend entities (refer to iot_data_trend_item) or NULL if dataType isn't TRENDS */
	SINGLYLINKEDLIST_HANDLE trends;

	/** Handle to list of alarm_item entities (refer to iot_data_alarm_item) or NULL if dataType isn't TRENDS */
	SINGLYLINKEDLIST_HANDLE alarms;

	/** Unique Id assigned to the message by iot_send().
	 * This field is set during the iot_send() operation and is available upon return.
	 * It can be used by the application code to track the success (acknowledgement) or failure of the message. */
	unsigned long messageId;
};

/**
 * Data structure representing a device and its optional list of child devices.
 */
struct iot_data_device_item
{
	const char *deviceUUID;			/*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */
	const char *profile;			/*!< Device Profile id in GUID format: 00000000-0000-0000-0000-000000000000 */
	const char *name;				/*!< (optional) User friendly name for the device or NULL */
	const char *serial;				/*!< (optional) Device serial number or NULL */
	const char *assetTag;			/*!< (optional) Asset name associated with the device or NULL */
	const char *mac;				/*!< (optional) Device mac address or NULL */
	SINGLYLINKEDLIST_HANDLE subDevices;	/*!< (optional) List of child devices or NULL */
};

/**
 * Data structure representing the status of a device.
 */
struct iot_data_devices_realtime_item
{
	const char *deviceUUID;			/*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 */
	long time;						/*!< Number of seconds since the Epoch (January 1st, 1970) when this update is captured or zero if not available */
	int elapsedms;					/*!< Number of milliseconds elapsed since the last second boundary or zero if not available */
	bool disconnected;				/*!< False if the device is not disconnected */
	bool disabled;					/*!< False if the device is not disabled */
	bool disarmed;					/*!< False if the device is not disarmed */
};

/**
 * Data structure representing the status and latest value of a channel.
 */
struct iot_data_channel_realtimes_item
{
	const char *channelTag;			/*!< Assigned channel Tag */
	long time;						/*!< Number of seconds since the Epoch (January 1st, 1970) when this update is captured or zero if not available */
	int milliseconds;				/*!< Number of milliseconds elapsed since the last second boundary or zero if not available */
	const char *value;				/*!< Represents the channel's measurement value in string format */
	bool disconnected;				/*!< False if the channel is not disconnected */
	bool disabled;					/*!< False if the channel is not disabled */
	bool disarmed;					/*!< False if the channel is not disarmed */
};

/**
 * Data structure representing a channel trend value(s).
 */
struct iot_data_trend_item
{
	const char *channelTag;			/*!< Assigned channel Tag */
	long time;						/*!< Number of seconds since the Epoch (January 1st, 1970) when this update is captured or zero if not available */
	const char *actValue;			/*!< (optional) The channel's actual value (on that time interval) in string format or NULL if it dosen't exist */
	const char *avgValue;			/*!< (optional) The channel's avg value during that time interval in string format or NULL if it doesn't exist */
	const char *minValue;			/*!< (optional) The channel's min value during that time interval in string format or NULL if it dosen't exist */
	const char *maxValue;			/*!< (optional) The channel's max value during that time interval in string format or NULL it it doesn't exist */
};

#ifndef IOT_NO_DEVICE_ALARMS
/**
 * Enumeration of Alarm ack level.
   Indicates that this alarm has been acknowledged by a user at the given level.
 */
typedef enum
{
	ALM_ACK_UNKNOWN = 0,
	ALM_ACK_CAUTIONARY = 1,
	ALM_ACK_CRITICAL = 2,
	ALM_ACK_MAX = 3
} IOT_ALARM_ACK_LEVEL;

/**
 * Enumeration of Alarm trigger severity (level).
 */
typedef enum
{
	TRIG_SEV_UNKNOWN = 0,
	TRIG_SEV_CAUTIONARY = 1,
	TRIG_SEV_CRITICAL = 2,
	TRIG_SEV_MAX = 3
} IOT_ALARM_TRIGGER_SEVERITY;

/**
 * Enumeration of Alarm trigger type.
 */
typedef enum
{
	TRIG_TYPE_UNKNOWN = 0,
	TRIG_TYPE_UPPER_BOUND = 1,
	TRIG_TYPE_LOWER_BOUND = 2,
	TRIG_TYPE_ON_VALUE = 3,
	TRIG_TYPE_IN_BETWEEN = 4,
	TRIG_TYPE_MAX = 5
} IOT_ALARM_TRIGGER_TYPE;

/**
 * Enumeration of Alarm trigger priority.
 */
typedef enum
{
	TRIG_PRIO_LOW = 0,
	TRIG_PRIO_HIGH = 1,
	TRIG_PRIO_MAX = 2
} IOT_ALARM_TRIGGER_PRIORITY;

/**
 * Data structure representing an alarm.
 */
struct iot_data_alarm_item
{
	uint32_t alarm_id;				/*!< Alarm id in numerical format. */
	const char *timestamp;			/*!< UTC time when this update occurred or was captured (in ISO-8601 format). */
	const char *device_id;			/*!< Source device id of the alarm update in GUID format: 00000000-0000-0000-0000-000000000000 */
	const char *source;				/*!< The The source for the alarm update. Possible values are: "Edge" (Device) or "Cloud" */
	unsigned long version;			/*!< Version of the alarm update message schema. 32 bit integer */
	unsigned long channel_id;		/*!< Id of the channel on the device which triggered the alarm. 32 bit integer. (optional) */
	const char *channel_v;			/*!< Current value of the channel on the device which triggered the alarm. (optional) */
	bool latching;					/*!< Indicates whether this alarm is latching. (optional) */
	bool closed;					/*!< Indicates that this is the final update in an alarm sequence. (optional) */
	bool condition_cleared;			/*!< Indicates that the trigger condition is no longer met, as the current value becomes normal, compared to any threshold. (optional) */
	unsigned int ack_level;			/*!< Indicates that this alarm has been acknowledged by a user at the given level. (optional) */
	const char *note;				/*!< The note entered by the user as part of an ACK which triggered this update. String Max Len: 8192 bytes. (optional) */
	const char *user;				/*!< Represents the name (account name, real name, or otherwise) of the user who just performed an ACK which triggered this update. (optional) */
	int status;					/*!< Gives the overall status of this Alarm/Event Update, of type px::proto::AlarmStatusEnums */
	IOT_DATA_ALARM_TRIGGER_ITEM *trigger;
#ifndef PX_GREEN_IOT_DEVICE			/*!< PXGreen is not using custom field as we have limited message buffers */
	IOT_DATA_ALARM_CUSTOM_ITEM *custom;
#endif
};

/**
 * Data structure representing an optional alarm trigger entity.
 */
struct iot_data_alarm_trigger_item
{
	const char *trigger_id;			/*!< Represents the trigger id in string. Use Alarm Rule Name as a trigger_id */
	unsigned int severity;			/*!< Represents the trigger severity (level). Only positive levels are supported. 1: Cautionary, 2: Critical */
	unsigned int trigger_type;		/*!< Represents the trigger type */
	unsigned int priority;			/*!< Whether this trigger is high priority. 0 not high priority and 1 is high priority. */
	const char *threshold;			/*!< The actual value to be used in the trigger calculation. */
	const char *between_threshold;	/*!< The actual This is only used when the trigger is IN_BETWEEN (trigger type). */
	const char *custom_message;		/*!<  Represents a custom message to display. By default, it should be empty. String Max Len: 8192 bytes (optional) */
};

/**
 * Data structure representing an option alarm custom entity.
 */
struct iot_data_alarm_custom_item
{
	const char *PXW_site_id;		/*!< UUID of the PX White Site ID: 00000000-0000-0000-0000-000000000000. (optional) */
	const char *PXW_adopter_id;		/*!< UUID of the PX White Adopter ID: 00000000-0000-0000-0000-000000000000. (optional) */
	const char *waveform;			/*!< uri/id string. (optional) */
	const char *channel_1;			/*!< Capture the channel 1 and its value. (optional) */
	const char *channel_2;			/*!< Capture the channel 2 and its value. (optional) */
};

#endif

#ifndef DONT_USE_UPLOADTOBLOB

/**
 * Enumeration of Blob upload status.
 */
typedef enum
{
	IOT_BLOB_UPLOAD_INITIALIZED = 0,
	IOT_BLOB_UPLOAD_DONE_SUCCESS = 1,
	IOT_BLOB_UPLOAD_INTERRUPTED = 2,
	IOT_BLOB_UPLOAD_DONE_ERROR = 3,
	IOT_BLOB_UPLOAD_TIMEOUT = 4
} IotBlobUploadErrorEnums;

/**
 * Data structure representing a blob file to be uploaded.
 */
struct blob_file
{
	const unsigned char *source;	/*!< Source buffer containing blob data */
	size_t size;					/*!< Size of the buffer in number of bytes */
	size_t current_block;			/*!< Current block which needs to be uploaded */
	time_t start_time;				/*!< Epoch time of blob upload start operation */
	time_t timeout_duration;		/*!< Timeout duration for the blob file */
	bool is_timed_out;				/*!< True if blob upload operation aborted due to timeout. False otherwise */
};

#endif

/******************************************************************************/
/*                     IOT Device Command Entities                            */
/******************************************************************************/

/**
 * Structure containing a command to be sent to the cloud (i.e. DownloadFirmware, etc.).
 * This structure is passed as a parameter to the function "iot_sendCommand".
 * @ingroup message_handling_apis
 */
struct iot_command
{
	IOT_COMMAND_TYPE commandType;	/*!< Type of command (refer to iot_command_type) */
	const char *deviceUUID;			/*!< Device Id in GUID format: 00000000-0000-0000-0000-000000000000 (optional) */
	const char *arg_1;				/*!< Command specific argument #1 (specific to each command) (optional) */
	const char *arg_2;				/*!< Command specific argument #2 (specific to each command) (optional) */
	const char *arg_3;				/*!< Command specific argument #3 (specific to each command) (optional) */
	const char *arg_4;				/*!< Command specific argument #4 (specific to each command) (optional) */
	const char *arg_5;				/*!< Command specific argument #5 (specific to each command) (optional) */
};

/******************************************************************************/
/*                               Functions                                    */
/******************************************************************************/

/**
 * Opens a connection with IoT Hub.
 * Each device connecting to IoT Hub must first be registered with Azure via PX White,
 * and assigned a unique device UUID in the form: 00000000-0000-0000-0000-000000000000.
 * Once registered, a device will be assigned a unique connection string by PX White.
 * @ingroup connection_status_apis
 *
 * @ingroup connection_status_apis
 * @param connectionOptions             Device connection options\settings (refer to iot_connection_options).
 * @param status                        The status of the open request (refer to iot_device_error_codes).
 * @param owner_context					Optional Opaque pointer to the owning device's context (eg, class instance).
 *
 * @return IOT_DEVICE_HANDLE            A handle to the device connection or NULL if the
 *                                      connection failed.
 */
extern IOT_DEVICE_HANDLE iot_open( const IOT_CONNECTION_OPTIONS *connectionOptions, int *status, void *owner_context );

/**
 * Closes a connection with IoT Hub.
 * After a connection has been closed, no further messages will be sent or received
 * from IoT Hub.
 * @ingroup connection_status_apis
 *
 * @ingroup connection_status_apis
 * @param deviceHandle              A handle to the device connection.
 *
 * @return bool                     True if the connection successfully closed else False.
 */
extern bool iot_close( IOT_DEVICE_HANDLE deviceHandle );


#if defined( IOT_ENABLE_ERROR_LOGGING ) || defined( IOT_ENABLE_INFO_LOGGING ) || defined( IOT_ENABLE_DEBUG_LOGGING )

/**
 * Sets the verbosity of the logging of the Device SDK messages at runtime.
 * Typically used for debugging.
 * @ingroup iot_sdk_logging
 * @param level Highest level (maximum verbosity) of messages to be logged.
 */
extern void iot_setLogLevel( enum IOT_LOG_LEVELS level );

/**
 * Registers callback function for user to implement custom logging functionality.
 * @ingroup iot_sdk_logging
 * @param callback - Function pointer.
 */
extern void iot_registerCustomLoggerCallback( CustomLoggerCallback callback );

/**
 * Unregisters custom logging function pointers to return to default logging.
 * @ingroup iot_sdk_logging
 */
extern void iot_unregisterCustomLoggerCallback(void);

#endif

/**
 * Registers a CloudToDevice device command callback function for processing Cloud-To-Device device commands.
 * Only one "CloudToDevice" device command callback can be registered per device connection.
 * @ingroup iot_sdk_callbacks
 *
 * @param deviceHandle              A handle to the device connection.
 * @param callback                  A pointer to the "C" callback function.
 *
 * @return bool                     True if callback was successfully registered else False.
 */
extern bool iot_registerC2DDeviceCommandCallback( IOT_DEVICE_HANDLE deviceHandle, C2DDeviceCommandCallback callback );

/**
 * Registers a TIMER callback function to be invoked at set intervals.
 * Only one "Timer" callback can be registered per device connection.
 * @ingroup iot_sdk_callbacks
 *
 * @param deviceHandle              A handle to the device connection.
 * @param interval                  The interval at which the callback will be invoked (seconds).
 *                                  A value of 0 means "without additional delay".
 * @param callback                  A pointer to the "C" callback function.
 *
 * @return bool                     True if callback was successfully registered else False.
 */
extern bool iot_registerTimerCallback( IOT_DEVICE_HANDLE deviceHandle, int interval, TimerCallback callback );

/**
 * Registers a connection status callback function to be invoked when the status of the connection to IoT Hub changes.
 * Only one "Connection Status" callback can be registered per device connection.
 * @ingroup iot_sdk_callbacks
 *
 * @param deviceHandle              A handle to the device connection.
 * @param callback                  A pointer to the "C" callback function.
 *
 * @return bool                     True if callback was successfully registered else False.
 */
extern bool iot_registerConnectionStatusCallback( IOT_DEVICE_HANDLE deviceHandle, ConnectionStatusCallback callback );

/**
 * Registers an optional callback function to be invoked when a sent Message (D2C) is done, either on Success or Failure.
 * This can be useful for the application to track for itself when a message has (or has not) reached Azure.
 * @ingroup iot_sdk_callbacks
 *
 * @param deviceHandle              A handle to the device connection.
 * @param callback                  A pointer to the "C" callback function.
 *
 * @return bool                     True if callback was successfully registered else False.
 */
extern bool iot_registerMessageDoneCallback( IOT_DEVICE_HANDLE deviceHandle, MessageDoneCallback callback );

/**
 * UnRegisters the CloudToDevice device command callback.
 * @ingroup iot_sdk_callbacks
 *
 * @param deviceHandle              A handle to the device connection.
 *
 * @return bool                     True if callback was successfully unregistered else False.
 */
extern bool iot_unRegisterC2DDeviceCommandCallback( IOT_DEVICE_HANDLE deviceHandle );

/**
 * UnRegisters the TIMER callback.
 * @ingroup iot_sdk_callbacks
 *
 * @param deviceHandle              A handle to the device connection.
 *
 * @return bool                     True if callback was successfully unregistered else False.
 */
extern bool iot_unRegisterTimerCallback( IOT_DEVICE_HANDLE deviceHandle );

/**
 * UnRegisters the Connection Status callback.
 * @ingroup iot_sdk_callbacks
 *
 * @param deviceHandle              A handle to the device connection.
 *
 * @return bool                     True if callback was successfully unregistered else False.
 */
extern bool iot_unRegisterConnectionStatusCallback( IOT_DEVICE_HANDLE deviceHandle );

/**
 * UnRegisters the Message Done callback.
 * @ingroup iot_sdk_callbacks
 *
 * @param deviceHandle              A handle to the device connection.
 *
 * @return bool                     True if callback was successfully unregistered else False.
 */
extern bool iot_unRegisterMessageDoneCallback( IOT_DEVICE_HANDLE deviceHandle );

/**
 * Sends data to IoT Hub (i.e. DeviceTree, DeviceRealtime, ChannelRealtime, Trends etc).
 * This is a non-blocking call. On success, sets the messageId in the IOT_DATA structure.
 * @ingroup message_handling_apis
 *
 * @param deviceHandle              A handle to the device connection.
 * @param data                      A list of one or more data entity values (refer to iot_data).
 *                                  Note: the caller is responsible for freeing the IOT_DATA structure.
 * @param[out] message_size         If a valid pointer, returns the size of the message that was sent, for I/O statistics
 *                                  or checking the size of the message that was sent.
 *									This is the serialized data size, not including the message "parameters" like "a" or "d".
 * @return bool                     True if data was successfully sent, else False.
 */
extern bool iot_send( IOT_DEVICE_HANDLE deviceHandle, IOT_DATA *data, int *message_size );

/**
 * Sends a command to IoT Hub (i.e. InitiateBlobTransfer, etc).
 * This is a non-blocking, non-batched call.
 * @ingroup message_handling_apis
 *
 * @param deviceHandle              A handle to the device connection.
 * @param command                   A command entity (refer to iot_command).
 *                                  Note: the caller is responsible for freeing the IOT_COMMAND structure.
 * @param[out] message_size         If a valid pointer, returns the size of the message that was sent, for I/O statistics.
 *
 * @return bool                     True if command was successfully sent else False.
 */
extern bool iot_sendCommand( IOT_DEVICE_HANDLE deviceHandle, const IOT_COMMAND *command, int *message_size );

/**
 * Sends a custom message to the IoT Hub (i.e., one created by the product).
 * This is a non-blocking call.
 * @ingroup message_handling_apis
 *
 * @param deviceHandle A handle to the device connection.
 * @param payload    The message payload as a JSON object (ie, already serialized).
 *                   For PXGreen, the caller should free the payload upon the return, if it was allocated,
 *                   since it has been duplicated in the internal IotMessage created for Azure code to handle.
 *                   For PXRed, it will always be freed internally.
 * @param properties An array of key-values pairs for properties of the message, to be used in the topic path when sending.
 *                   Each pair takes two places in this array; the first is the key, the second the associated value.
 *                   Well known PX White properties include:
 *                      'a' - the message name, eg 'trends'
 *                      'e' - the encoding, eg 'json'
 *                      'p' - the UUID for a child device (not the publisher)
 *                      'r' - the routing path for this message
 *                      'j' - a Job UUID, for commands
 * @param num_properties How many key-value property pairs are included in the properties array
 *                       (So the number of array entries is twice this number)
 * @return               On success, returns the messageId; on failure, returns 0.
 */
extern unsigned long iot_sendCustomMessage( IOT_DEVICE_HANDLE deviceHandle, const char *payload, const char *properties[], size_t num_properties );


/**
 * Sends a device command status update to IoT Hub.
 * This is a non-blocking, non-batched call.
 * @ingroup message_handling_apis
 *
 * @param deviceHandle     A handle to the device connection.
 * @param commandID        A command instance id.
 * @param deviceUUID       A device UUID in the form: 00000000-0000-0000-0000-000000000000.
 * @param task             The name of the task either the a main task (i.e. UpdateFirmware, Reboot) or a subtask (i.e. ImageTransfer).
 * @param status           The status of the step refer to iot_command_status).
 * @param description      A free form text description (optional).
 * @param[out] message_size  If a valid pointer, returns the size of the message that was sent, for I/O statistics.
 *
 * @return bool            True if status was successfully sent else False.
 */
extern bool iot_updateDeviceCommandStatus( IOT_DEVICE_HANDLE deviceHandle, const char *commandID, const char *deviceUUID,
										   const char *task, const IOT_COMMAND_STATUS status, const char *description, int *message_size );


/**
 * Gets the status of the connection.
 * If possible, will get the low-level status as well and incorporate that in the connectionStatus.
 * @ingroup connection_status_apis
 *
 * @param deviceHandle        A handle to the device connection.
 * @param connectionStatus    A structure containing the connection status (refer to iot_connection_status).
 *                            Note: The passed in address of the connection status is
 *                            populated with the latest connection information.
 *
 * @return bool               True if connection status was successfully retrieved else False.
 */
extern bool iot_getStatus( IOT_DEVICE_HANDLE deviceHandle, IOT_CONNECTION_STATUS *connectionStatus );


/************************************************************************************
*		Batching control
*
* Batching is when we allow a bunch of messages published via iot_send() to be enqueued
* without sending them yet to Azure, and then sent all at once.
* Important note: this only applies to AMQP; MQTT does not have the code in Azure to support this.
* The triggers for sending them are
*   1) The DoWork timer expires and we call Azure's DoWork function to drive its state machine
*   2) iot_do_send_immediate() is called
* You must call iot_set_enable_batching() to enable (or disable) this batching operation.
* Initially we don't enable batching, in order to get the fastest acknowledgement of the first messages,
* to confirm that the connection is valid.
*
************************************************************************************/

/**
 * @brief  Setter for the connection's wantBatching flag and doWorkIntervalSeconds value.
 * @ingroup message_handling_apis
 *
 * @param deviceHandle  A handle to the device connection.
 * @param enable Set to true to enable batching, false to disable it.
 * @param doWorkIntervalSeconds Value to use for the DoWork (batching) interval, in seconds. Ignored when disabling batching.
 * @return true If succeed in setting this.
 * @return false If fail to set, including the MQTT case where batching is not enabled.
 */
extern bool iot_set_enable_batching( IOT_DEVICE_HANDLE deviceHandle, bool enable, uint32_t doWorkIntervalSeconds );

/**
 * @brief  Sets the connection's sendImmediately flag to trigger publishing of all batched messages.
 * @ingroup message_handling_apis
 *
 * Calling this will trigger a DoWork cycle for an immediate send of all batched messages,
 * and resets the DoWork interval timer.
 * The sendImmediately flag will be cleared automatically when the publishing happens, so this
 * method can be called again later as needed.
 *
 * This might be useful for events which need to be communicated immediately, or when it is time
 * to send all trend data.
 *
 * @param deviceHandle        A handle to the device connection.
 */
extern void iot_do_send_immediate( IOT_DEVICE_HANDLE deviceHandle );


/************************************************************************************
*    List Methods
*
* Each LIST_ITEM_INSTANCE has this simple structure, where the ->item is the "value" stored in the list.
*                      struct LIST_ITEM_INSTANCE
*                      {
*                        const void* item;
*                        void* next;
*                      }
*
* For most list methods, use the native Azure functions:
* singlylinkedlist_create()
* singlylinkedlist_destroy()
* singlylinkedlist_add()
* singlylinkedlist_remove()
* singlylinkedlist_get_head_item()
* singlylinkedlist_get_next_item()
* singlylinkedlist_item_get_value()
*
* But if the item values were allocated, use iot_list_destroy_with_items()
* instead of singlylinkedlist_destroy().
*
************************************************************************************/

/**
 * Destroys a list of items while also freeing the memory allocated for each item.
 * Memory for the list of LIST_ITEM_INSTANCE structures, along with their item members, is freed.
 * The caller is responsible for previously freeing any allocated memory for the fields
 * within the item (eg, a message structure).
 *
 * @param listHandle    A handle to the list.
 *
 * @return void
 */
extern void iot_list_destroy_with_items( SINGLYLINKEDLIST_HANDLE listHandle );


 #ifndef DONT_USE_UPLOADTOBLOB
/**
 * Uploads to Azure Storage the content pointed to by @p source having the size @p size.
 * The blob path will have the format:  {device-uuid}/{blob-type}/{yyyy-mm-dd}/{hh-mm-ss.SSS}
 * Uses HTTP transport.
 *
 * @param deviceHandle     A handle to the device connection.
 * @param blobPath         A blob path is combination of device uuid ,blob type & timestamp for eg"{device-uuid}/{blob-type}/{yyyy-mm-dd}/{hh-mm-ss.SSS}.
 * @param source           A pointer to the source of the blob content.
 * @param size             The size of the blob content.
 * @return bool            True if upload to blob was successful else False.
 */
extern bool iot_uploadToBlob( IOT_DEVICE_HANDLE deviceHandle, const char *blobPath, const unsigned char *source, size_t size );

/**
 * Uploads to Azure Storage the content pointed to by @p source having the size @p size.
 * The blob path will have the format:  {device-uuid}/{blob-type}/{yyyy-mm-dd}/{hh-mm-ss.SSS}
 * Uses a callback function to send the content in smaller blocks. This allows better control in case there is need to abort upload in midway.
 * Uses @p timeout_duration to abort blob upload operation. Duration in seconds.
 * Uses HTTP transport.
 *
 * @param deviceHandle     A handle to the device connection.
 * @param blobPath         A blob path is combination of device uuid ,blob type & timestamp for eg"{device-uuid}/{blob-type}/{yyyy-mm-dd}/{hh-mm-ss.SSS}.
 * @param source           A pointer to the source of the blob content.
 * @param size             The size of the blob content.
 * @param timeout_duration Timeout duration for blob upload operation in seconds.
 * @return IotBlobUploadErrorEnums	Status of blob upload operation.
 */
extern IotBlobUploadErrorEnums iot_uploadToBlob_async( IOT_DEVICE_HANDLE deviceHandle, const char *blobPath, const unsigned char *source, size_t size, time_t timeout_duration );

#endif

/**
 *  Retrieve location and size of a device command parameter by name.
 *
 *  Example: "params":{"tag1":"value1", "tag2":"value2"}
 *  @ingroup message_handling_apis
 *
 *  @param params       Device command parameters in JSON format.
 *  @param tag          The tag to extract the location and size for.
 *  @param[out] offset  The offset of the parameter.
 *  @param[out] length  The length of the parameter.
 *  @return				True on parsing success, False if failed and offset and length are invalid.
 */
extern bool iot_getCommandParameter( const char *params, const char *tag, int *offset, int *length );

/**
 *  Retrieve location and size of a device command parameter by token index.
 *
 *  Example: "params":{"tag1":"value1", "tag2":"value2"}
 *  @ingroup message_handling_apis
 *
 *  @param params       Device command parameters in JSON format.
 *  @param index        The index of the token to extract the location and size for.
 *  @param[out] offset  The offset of the parameter.
 *  @param[out] length  The length of the parameter.
 *  @return				True on parsing success, False if failed and offset and length are invalid.
 */
extern bool iot_getCommandParameterToken( const char *params, int index, int *offset, int *length );


/**
 *  Retrieve the number of tokens in a device command parameter.
 *
 *  Example: "params":{"tag1":"value1", "tag2":"value2"}
 *  @ingroup message_handling_apis
 *
 *  @param params       Device command parameters in JSON format.
 *  @param[out] count   The number of tokens.
 *  @return				True on parsing success, False if failed and count is invalid.
 */
extern bool iot_getCommandParameterTokenCount( const char *params, int *count );

/**
 *  Retrieve the number of tokens in a json string.
 *  @ingroup message_handling_apis
 *
 *  @param json         Json string to search.
 *  @param len          The length of the json string.
 *  @return             The number of JSON tokens counted.
 */
extern unsigned int iot_getJsonTokenCount( const char *json, int len );


/******************************************************************************/
/*                        Azure Callback Routines                             */
/******************************************************************************/

/**
 * @brief Callback routine to process confirmation (or failure) of an outbound message (sent Device-to-Cloud).
 *  This is invoked either when Azure acknowledges the message as received, or the effort to send it times out,
 *  or when Azure declined the message because the connection is closing or you hit the daily limit.
 *
 *  The timeout or error condition here will cause the connection to be lost, and reconnect will be necessary.
 *
 *  This routine can call the optional platform MessageDoneCallback before releasing the message.
 *
 * @param result The status of the message; one of IOTHUB_CLIENT_CONFIRMATION_OK, _MESSAGE_TIMEOUT, _BECAUSE_DESTROY,
 *                  or _ERROR (the latter usually meaning that the daily limit has been reached).
 * @param userContextCallback An opaque "contex", which will be cast to the IOT_MESSAGE* type for the message itself.

 */
void handleD2CMessageConfirmationCallback( IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback );

/**
 * @brief Callback routine to process connection status changes.
 *
 * @param statusCode The new status
 * @param reasonCode The reason for the status (further detail)
 * @param userContextCallback An opaque "contex", which maps to our connection structure.
 */

void handleConnectionStatusCallback( IOTHUB_CLIENT_CONNECTION_STATUS statusCode, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reasonCode,
									 void *userContextCallback );

/**
 * @brief Callback routine to process inbound (Azure Cloud-to-Device) device commands, internal to the SDK.
 *    This is the preferred C2D Command type, which includes a response.
 *    This function will deserialize the payload, invoke the implementor's C2DDeviceCommandCallback function,
 *    allocate and serialize the response, and return it to Azure.
 *    This method conforms to the IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC function type.
 * @param methodName
 * @param payload
 * @param response
 * @param responseSize
 * @param userContextCallback An opaque "contex", which maps to our connection structure.
 * @return int
 */
int receiveC2DDeviceCommandCallback( const char *methodName, const unsigned char *payload, size_t size,
									 unsigned char * *response, size_t *responseSize, void *userContextCallback );


/******************************************************************************/
/*    Thread connectors                                                       */
/******************************************************************************/

/**
 * @brief Thread to process messages on the work queue.
 * @details For PXRed, this is a pthread-based thread function that normally runs in a loop
 *      until the connection is closed.
 *      For PXGreen, however, it is a worker function that runs through once and returns,
 *      so it must called repetitively.
 *
 * @param arg Pointer to the connection structure, of type IOT_DEVICE_CONNECTION*
 * @return Just returns NULL.
 */
void *messageThread( void *arg );

/** Tell the messageThread to close itself and exit.
 *  Effectively a setter for the connection->close flag.
 * @param deviceHandle  A handle to the Low-Level Azure device connection.
 */
void closeMessageThread( IOT_DEVICE_HANDLE deviceHandle );

/**
 * @brief Thread to process connection status changes
 *
 * @param arg Pointer to the connection structure, of type IOT_DEVICE_CONNECTION*
 * @return Just returns NULL.
 */
void *connectionStatusChangedThread( void *arg );

/** Getter for the messageThreadId, given the deviceHandle.
 * This lets us access the task structure for the messageThread.
 * @param deviceHandle  A handle to the Low-Level Azure device connection.
 * @return The messageThreadId from the connection structure.
 */
THREAD_HANDLE getConnectionThreadHandle( IOT_DEVICE_HANDLE deviceHandle );


#ifdef __cplusplus
}

/******************************************************************************/
/*    Azure Trusted Certificates                                                     */
/******************************************************************************/

/** Text array of certificate(s) provided by MS for TLS encrypting to Azure. */
extern "C" const char certificates[];

#else

// Declare as extern without the "C" qualifier for C code
/** Text array of certificate(s) provided by MS for TLS encrypting to Azure. */
extern const char certificates[];

#endif	// __cplusplus



/*************************************************************************
* DOXYGEN modules defined here, and connected to their markdown groups,
* where full descriptions are found.
*************************************************************************/

/** @defgroup eaton_iot_sdk_apis Eaton IoT SDK APIs
 *  @ingroup md_iot_sdk
 *  @details See [Eaton IoT SDK APIs markdown](@ref md_iot_sdk)
 */

/** @defgroup connection_status_apis Connection and Status APIs
 *  @ingroup eaton_iot_sdk_apis
 *  @details See [Connection and Status overview](@ref md_connection_status_apis)
 */

/** @defgroup message_handling_apis Message handling APIs
 *  @ingroup eaton_iot_sdk_apis
 *  @details See [Message handling overview](@ref md_message_handling_apis)
 */

/** @defgroup iot_sdk_callbacks Callbacks from the Eaton IoT SDK
 *  @ingroup eaton_iot_sdk_apis
 *  @details See [Callbacks from the Eaton IoT SDK overview](@ref md_iot_sdk_callbacks)
 */

/** @defgroup iot_sdk_logging Debug logging for the Eaton IoT SDK
 *  @ingroup eaton_iot_sdk_apis
 *  @details See [Debug logging for the Eaton IoT SDK overview](@ref md_iot_sdk_logging)
 */



#endif	/* IOT_DEVICE_H */

