/** @file iot_device_common.c Implementation of common IoT functions (eg, for message handling). */
#include "iot_device.h"
/* Since Azure version 2018_09_11, now need to specifically include this header: */
#include "iothub_client_ll.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/agenttime.h"


// #include "azure_c_shared_utility/agenttime.h"

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************/
/*                               Global Variables                             */
/******************************************************************************/

/*
    Maximum size of the response message in Device Commands
 */
#define DEVICE_COMMANDS_RESPONSE_SIZE 4096


/*
   Handle to our single connection to the Azure IoT Hub
 */
static IOT_DEVICE_CONNECTION *iot_connection = NULL;

#ifdef PX_GREEN_IOT_DEVICE
/* Small Array of statically allocated message buffers, of fixed size. */
static char greenMessageBuffers[IOT_MAX_CONCURRENT_MESSAGES][IOT_MAX_OUTBOUND_BUFFER_SIZE];
#endif

/*
   Handle to lock structure
 */
static LOCK_HANDLE lock = NULL;

/*
   Unique message sequence number assigned to an outbound (Device-to-Cloud) message
 */
static unsigned long messageId = 0;

/*
   Platform initialization flag
 */
static bool platformInitialized = false;

/*
   Send DeviceToCloud message timeout
 */
static unsigned int timeout = 30000;	// 30 seconds

/*
   Interval between pulling for Cloud-To-Device messages
 */
static unsigned int minimumPollingTime = 4;

/*
   Interval between heartbeat messages - MQTT only
 */
static unsigned int heartbeat = 60;		// seconds


/******************************************************************************/
/*                                Misc Methods                                */
/******************************************************************************/

/*
   Looks up the data type associated with a data entity structure.
 */
const char *getDataType( const IOT_DATA *data )
{
	if ( data == NULL )
	{
		return ( TYPE_UNKNOWN );
	}
	else if ( data->dataType == DEVICE_TREE )
	{
		return ( TYPE_DEVICE_TREE );
	}
	else if ( data->dataType == DEVICES_REALTIME )
	{
		return ( TYPE_DEVICES_REALTIME );
	}
	else if ( data->dataType == CHANNEL_REALTIMES )
	{
		return ( TYPE_REALTIMES );
	}
	else if ( data->dataType == TRENDS )
	{
		return ( TYPE_TRENDS );
	}
#ifndef IOT_NO_DEVICE_ALARMS
	else if ( data->dataType == ALARMS )
	{
		return ( TYPE_ALARMS );
	}
#endif
	else
	{
		return ( TYPE_UNKNOWN );
	}
}

/*
   Looks up the data type associated with a data entity structure.
 */
const char *getCommandType( const IOT_COMMAND *command )
{
	if ( command == NULL )
	{
		return ( TYPE_UNKNOWN );
	}
	else if ( command->commandType == TRANSFER_IMAGE )
	{
		return ( TYPE_TRANSFER_IMAGE );
	}
	else if ( command->commandType == DEVICE_COMMAND_STATUS )
	{
		return ( TYPE_DEVICE_COMMAND_STATUS );
	}
	else
	{
		return ( TYPE_UNKNOWN );
	}
}

/*
   Returns an IOT_DEVICE_CONNECTION associated with a IOT_DEVICE_HANDLE.
 */
IOT_DEVICE_CONNECTION *getConnection( IOT_DEVICE_HANDLE deviceHandle )
{
	if ( deviceHandle == NULL )
	{
		Log_Error( "Invalid Device Handle" );
		return ( NULL );
	}

	if ( iot_connection == NULL )
	{
		Log_Error( "No Connection established, or Closed." );
		return ( NULL );
	}

	if ( iot_connection->handle != deviceHandle )
	{
		Log_Error( "Handle %p not found in Connection (%p).", deviceHandle, iot_connection->handle );
		return ( NULL );
	}

	return ( iot_connection );
}

/*
   Determines if a connection associated with a Device UUID exists.
 */
bool connectionExists( const char *deviceUUID )
{
	Log_Debug( "Checking if connection exists for %s", deviceUUID );

	if ( iot_connection != NULL )
	{
		if ( strcmp( iot_connection->deviceUUID, deviceUUID ) == 0 )
		{
			return ( true );
		}
	}
	return ( false );
}

/*
   Deletes an IOT_DEVICE_CONNECTION and associated memory.
 */
void deleteConnection( IOT_DEVICE_CONNECTION *connection )
{
	if ( ( connection != NULL ) && ( iot_connection != NULL ) && ( iot_connection == connection ) )
	{
		Log_Debug( "Deleting iot_connection" );
		iot_free( iot_connection );
		iot_connection = NULL;
	}
}

/*
   Validates the contents of IOT_CONNECTION_OPTIONS.
 */
bool validateConnectionOptions( const IOT_CONNECTION_OPTIONS *connectionOptions, int *status )
{
	if ( connectionOptions == NULL )
	{
		Log_Error( "Missing argument(IOT_CONNECTION_OPTIONS)" );
		*status = IOT_INVALID_ARGUMENT;
		return ( false );
	}

	if ( ( connectionOptions->deviceUUID == NULL ) || ( strlen( connectionOptions->deviceUUID ) == 0 ) )
	{
		Log_Error( "Invalid argument(IOT_CONNECTION_OPTIONS.deviceUUID)" );
		*status = IOT_INVALID_ARGUMENT;
		return ( false );
	}

	if ( ( connectionOptions->connectionString == NULL ) || ( strlen( connectionOptions->connectionString ) == 0 ) )
	{
		Log_Error( "Invalid argument(IOT_CONNECTION_OPTIONS.connectionString)" );
		*status = IOT_INVALID_ARGUMENT;
		return ( false );
	}

	/* prohibit more than 1 connection from the same deviceUUID */
	if ( connectionExists( connectionOptions->deviceUUID ) )
	{
		Log_Error( "Connection already exists for deviceUUID: %s", connectionOptions->deviceUUID );
		*status = IOT_DUPLICATE_CONNECTION;
		return ( false );
	}

	return ( true );
}

/*
   Initializes global variables.
 */
void initializeGlobalVariables()
{
	if ( iot_connection != NULL )
	{
		iot_free( iot_connection );
		iot_connection = NULL;
	}

	/* if first time - create a lock object */
	if ( lock == NULL )
	{
		lock = Lock_Init();
	}
}

// Prototype for this external function:
extern int platform_init();

/*
   Initializes platform.
 */
bool initializePlatform( int *status )
{
	if ( !platformInitialized )
	{
		if ( platform_init() != 0 )
		{
			Log_Error( "platform_init() failed" );
			*status = IOT_PLATFORM_INIT_FAILED;
			return ( false );
		}
		platformInitialized = true;
	}

	return ( true );
}

// (Internal or Test) Constructor for IOT_DEVICE_CONNECTION
IOT_DEVICE_CONNECTION *constructConnection( IOTHUB_CLIENT_LL_HANDLE handle, const IOT_CONNECTION_OPTIONS *connectionOptions,
											void *owner_context )
{
	// If a previous connection is still hanging around, remove it
	if ( iot_connection != NULL )
	{
		Log_Error( "Removing previous iot_connection." );
		iot_free( iot_connection );
		iot_connection = NULL;
	}

	iot_connection = ( IOT_DEVICE_CONNECTION * ) iot_malloc( sizeof( IOT_DEVICE_CONNECTION ) );
	if ( iot_connection == NULL )
	{
		Log_Error( "Error allocating memory for IOT_DEVICE_CONNECTION" );
		return ( NULL );
	}
	memset( iot_connection, 0, sizeof( IOT_DEVICE_CONNECTION ) );

	/* initialize the connection structure  */
	strncpy( iot_connection->deviceUUID, connectionOptions->deviceUUID, IOT_MAX_DEVICE_UUID_SIZE );
	iot_connection->deviceUUID[IOT_MAX_DEVICE_UUID_SIZE - 1] = '\0';
	iot_connection->context = owner_context;
	iot_connection->unconfirmedMessageCount = 0;
	iot_connection->connectionStatus = IOT_UNKNOWN;		// Until we know better
	iot_connection->lastErrorCode = IOT_OK;
	iot_connection->handle = handle;
	iot_connection->c2dDeviceCommandCallback = NULL;
	iot_connection->timerCallback = NULL;
	iot_connection->connectionStatusCallback = NULL;

	iot_connection->lastTimerCallbackInvoked = get_time( NULL );
	iot_connection->timerCallbackInterval = 0;
	iot_connection->msgOutbound = 0;
	iot_connection->msgConfirmed = 0;
	iot_connection->msgInbound = 0;
	iot_connection->cmdInbound = 0;
	iot_connection->close = false;
	iot_connection->messageThreadId = NULL;

#ifdef PX_GREEN_IOT_DEVICE
	for ( int i = 0; i < IOT_MAX_CONCURRENT_MESSAGES; i++ )
	{
		// Point to the static message buffers for these static iotMessage instances
		iot_connection->iotMessage[i].serializedMessage = greenMessageBuffers[i];
		iot_connection->iotMessage[i].inUse = false;
		iot_connection->iotMessage[i].isAllocated = false;
	}
#endif

	return ( iot_connection );
}

/*
   Initializes a connection to IoT Hub.
   See https://github.com/Azure/azure-iot-sdk-c/blob/master/doc/Iothub_sdk_options.md
   for the descriptions of the available options and their per-protocol usage.
 */
IOT_DEVICE_CONNECTION *initializeConnection( IOTHUB_CLIENT_LL_HANDLE handle, const IOT_CONNECTION_OPTIONS *connectionOptions,
											 void *owner_context )
{
	IOTHUB_CLIENT_RESULT result;

	/* set IOT Hub connection options */
	if ( connectionOptions->protocol == HTTP )
	{
		IoTHubClient_LL_SetOption( handle, "timeout", &timeout );
		IoTHubClient_LL_SetOption( handle, "MinimumPollingTime", &minimumPollingTime );
#ifdef IOT_ENABLE_VERBOSE_LOGGING
		int verbose = 1;
		IoTHubClient_LL_SetOption( handle, "CURLOPT_VERBOSE", &verbose );	/* only applies when using HTTP */
#endif
	}
	else if ( ( connectionOptions->protocol == MQTT ) ||
			  ( connectionOptions->protocol == MQTT_WEBSOCKETS ) )
	{
		/* Length of time to send Keep Alives to service for D2C Messages - not entirely clear... */
		result = IoTHubClient_LL_SetOption( handle, "keepalive", &heartbeat );
		if ( result != IOTHUB_CLIENT_OK )
		{
			Log_Error( "Setting keepalive option failed" );
			return ( NULL );
		}
	}
	else if ( ( connectionOptions->protocol == AMQP ) ||
			  ( connectionOptions->protocol == AMQP_WEBSOCKETS ) )
	{
		/* Possible options to set are OPTION_SAS_TOKEN_REFRESH_TIME, OPTION_EVENT_SEND_TIMEOUT_SECS
		 * and OPTION_C2D_KEEP_ALIVE_FREQ_SECS.
		 */
	}

#ifdef IOT_SAS_TOKEN_REFRESH_SECS
	/* Set the OPTION_SAS_TOKEN_LIFETIME to this value, if given by the build.
	 * The Azure default is 60min => 3600secs. We might set it much shorter to accelerate testing.
	 * For MQTT, it uses 80% of the value we set as the point at which it reconnects.
	 * It is not clear if AMQP uses this at all, though it supports the option.
	 * Azure changes "sas_token_lifetime" into "sas_token_lifetime_secs".
	 */
	{
		int lifetime = IOT_SAS_TOKEN_REFRESH_SECS;
		result = IoTHubClient_LL_SetOption( handle, "sas_token_lifetime", &lifetime );
		if ( result != IOTHUB_CLIENT_OK )
		{
			Log_Error( "Setting OPTION_SAS_TOKEN_LIFETIME failed" );
			return ( NULL );
		}
		else
		{
			int refresh_time = 0;
			if ( connectionOptions->protocol == AMQP_WEBSOCKETS )
			{
				/* We will choose to set a refresh time of 30sec less than the lifetime.
				   For AMQP, we can set this as an option. */
				refresh_time = lifetime - 30;
				result = IoTHubClient_LL_SetOption( handle, "sas_token_refresh_time", &refresh_time );
				if ( result != IOTHUB_CLIENT_OK )
				{
					Log_Error( "Setting OPTION_SAS_TOKEN_LIFETIME failed" );
					return ( NULL );
				}
			}
			else if ( connectionOptions->protocol == MQTT_WEBSOCKETS )
			{
				/* For MQTT, the Azure code forces a "refresh" (disconnect and reconnect) of 80% of lifetime.
				 * Avoid floating point math, multiply by 80% the long way*/
				refresh_time = ( lifetime * 4 ) / 5;
			}
			Log_Info( "Set the OPTION_SAS_TOKEN_LIFETIME to %d seconds, and refresh time = %d", lifetime, refresh_time );
		}
	}
#endif

	/* set the proxy */
	if ( connectionOptions->proxyAddress != NULL )
	{
		HTTP_PROXY_OPTIONS http_proxy_options;
		memset( &http_proxy_options, 0, sizeof( HTTP_PROXY_OPTIONS ) );
		Log_Info( "Setting proxy options for %s", connectionOptions->proxyAddress );
		http_proxy_options.host_address = connectionOptions->proxyAddress;
		if ( connectionOptions->proxyPort > 0 )
		{
			http_proxy_options.port = connectionOptions->proxyPort;
		}
		if ( connectionOptions->proxyUser != NULL )
		{
			http_proxy_options.username = connectionOptions->proxyUser;
		}
		if ( connectionOptions->proxyPassword != NULL )
		{
			http_proxy_options.password = connectionOptions->proxyPassword;
		}
		result = IoTHubClient_LL_SetOption( handle, OPTION_HTTP_PROXY, &http_proxy_options );
		if ( result != IOTHUB_CLIENT_OK )
		{
			Log_Error( "Setting of proxy options for %s failed", http_proxy_options.host_address );
			return ( NULL );
		}
	}

	// For any of the protocols that use TLS:
	if ( ( connectionOptions->protocol == HTTP ) ||
		 ( connectionOptions->protocol == MQTT_WEBSOCKETS ) ||
		 ( connectionOptions->protocol == AMQP_WEBSOCKETS ) )
	{
		result = IoTHubClient_LL_SetOption( handle, "TrustedCerts", certificates );
		if ( result != IOTHUB_CLIENT_OK )
		{
			Log_Error( "Setting of Trusted Certificate options failed" );
			return ( NULL );
		}
	}

	/* Sets the retry policy, but we will just leave it at its very good default. */
	/*
	 * Available options:
	 *  IOTHUB_CLIENT_RETRY_NONE
	 *  IOTHUB_CLIENT_RETRY_IMMEDIATE
	 *  IOTHUB_CLIENT_RETRY_INTERVAL
	 *  IOTHUB_CLIENT_RETRY_LINEAR_BACKOFF
	 *  IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF (default)
	 *  IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER
	 *  IOTHUB_CLIENT_RETRY_RANDOM
	 */
	/* IoTHubClient_LL_SetRetryPolicy(handle,IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER,0); */

	/* Allocate an IOT_DEVICE_CONNECTION structure, initialize it, and save to internal iot_connection pointer.
	 * Do this before trying to open the connection, in case there is a low level failure,
	 * which will be indicated later in iot_connection->connectionStatus */
	constructConnection( handle, connectionOptions, owner_context );
	if ( iot_connection == NULL )
	{
		Log_Error( "Failed to allocate iot_connection." );
		return ( NULL );
	}

	/* Set the connection status callback early, in case we have a Low-Level failure to connect.
	 * Provide our connection structure as the "user context" for when we are called back. */
	if ( IoTHubClient_LL_SetConnectionStatusCallback( handle, handleConnectionStatusCallback, ( void * ) iot_connection ) != IOTHUB_CLIENT_OK )
	{
		Log_Error( "Error setting connection status callback" );
		iot_free( iot_connection );
		iot_connection = NULL;
		return ( NULL );
	}

	iot_connection->connectionStatus = IOT_CONNECTING;
#ifndef PX_GREEN_IOT_DEVICE
	/* allow the connection to be established */
	/* Note: required when using a proxy */
	IoTHubClient_LL_DoWork( handle );
	ThreadAPI_Sleep( 1 );		/* token sleep time of 1ms */

	/* Let's see if we can detect if that open/connect failed in the low level Azure code.
	 * if it did, the handleConnectionStatusCallback() would have been invoked,
	 * and the connectionStatus set to IOT_CLOSED there. */
	if ( iot_connection->connectionStatus == IOT_CLOSED )
	{
		Log_Error( "Failed to establish low-level connection." );
		iot_free( iot_connection );
		iot_connection = NULL;
		return ( NULL );
	}

	// Else
	Log_Info( "Low-level Connection established now." );
#else
	/* For Green, we don't want to initiate DoWork yet, until our message thread has been created
	 * and we can execute the connection attempt from that thread. */
	Log_Info( "Low-level Connection to be established now." );
#endif

	return ( iot_connection );
}

/*
   Creates an IoT Hub message.
 */
IOT_MESSAGE *createMessage( IOT_DEVICE_CONNECTION *connection, const IOT_DATA *data, const IOT_COMMAND *command )
{
#ifdef PX_GREEN_IOT_DEVICE
	IOT_MESSAGE *iotMessage = getNextMessage( connection );
#else
	IOT_MESSAGE *iotMessage = calloc( 1, sizeof( IOT_MESSAGE ) );
#endif
	if ( iotMessage == NULL )
	{
		Log_Error( "Could not allocate a message buffer." );
		connection->lastErrorCode = IOT_NO_AVAILABLE_MESSAGE_BUFFER;
		return ( NULL );
	}

	iotMessage->connection = connection;

	if ( data != NULL )
	{
		if ( !serializeData( connection, iotMessage, data ) )				/* serialize data to json */
		{
			Log_Error( "Error serializing message" );
			connection->lastErrorCode = IOT_SERIALIZE_ERROR;
			releaseIotMessage( iotMessage );
			return ( NULL );
		}
	}
	else if ( command != NULL )
	{
		if ( !serializeCommand( connection, iotMessage, command ) )			/* serialize command to json */
		{
			Log_Error( "Error serializing message" );
			connection->lastErrorCode = IOT_SERIALIZE_ERROR;
			releaseIotMessage( iotMessage );
			return ( NULL );
		}
	}
	else
	{
		Log_Error( "No Data or Command to send!" );
		connection->lastErrorCode = IOT_SERIALIZE_ERROR;
		releaseIotMessage( iotMessage );
		return ( NULL );
	}

	//Log_Debug( "Serialized Message:\r\n%s", iotMessage->serializedMessage );
#ifdef PX_GREEN_IOT_DEVICE
	// Save the serialized message length
	iotMessage->serializedBufferSize = strlen( iotMessage->serializedMessage );
#endif

	// Allocate a new messageHandle and have it hold an allocated copy of the serializedMessage
	iotMessage->messageHandle = IoTHubMessage_CreateFromString( iotMessage->serializedMessage );

	if ( LOCK_ERROR == Lock( lock ) )
	{
		Log_Error( "Error obtaining lock" );
		connection->lastErrorCode = IOT_SERIALIZE_ERROR;
		releaseIotMessage( iotMessage );
		return ( NULL );
	}

	/* assign a unique Id to the message */
	messageId++;
	iotMessage->messageId = messageId;
	Unlock( lock );

	return ( iotMessage );
}

IOT_MESSAGE *createCustomMessage( IOT_DEVICE_CONNECTION *connection, const char *serialized_message )
{
	if ( ( connection == NULL ) || ( serialized_message == NULL ) )
	{
		Log_Error( "No Connection or no Message to send!" );
		if ( connection != NULL )
		{
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		}
		return ( NULL );
	}

	IOT_MESSAGE *iotMessage = ( IOT_MESSAGE * ) iot_malloc( sizeof( IOT_MESSAGE ) );
	if ( iotMessage == NULL )
	{
		Log_Error( "Could not allocate a message buffer." );
		connection->lastErrorCode = IOT_NO_AVAILABLE_MESSAGE_BUFFER;
		return ( NULL );
	}
	memset( iotMessage, 0, sizeof( IOT_MESSAGE ) );
	iotMessage->connection = connection;
	iotMessage->serializedMessage = ( char * ) serialized_message;
#ifdef PX_GREEN_IOT_DEVICE
	iotMessage->isAllocated = true;
#endif
	// Allocate a new messageHandle and have it hold an allocated copy of the serializedMessage
	iotMessage->messageHandle = IoTHubMessage_CreateFromString( iotMessage->serializedMessage );

	if ( LOCK_ERROR == Lock( lock ) )
	{
		Log_Error( "Error obtaining lock" );
		connection->lastErrorCode = IOT_SERIALIZE_ERROR;
		releaseIotMessage( iotMessage );
		return ( NULL );
	}

	/* assign a unique Id to the message */
	messageId++;
	iotMessage->messageId = messageId;
	Unlock( lock );

	return ( iotMessage );
}

/*
   Deletes an IoT Hub message.
 */
void releaseIotMessage( IOT_MESSAGE *iotMessage )
{
	if ( iotMessage != NULL )
	{
#ifdef PX_GREEN_IOT_DEVICE
		IoTHubMessage_Destroy( iotMessage->messageHandle );
		// If this was an allocated message, free the message and its container
		if ( iotMessage->isAllocated )
		{
			if ( iotMessage->serializedMessage != NULL )
			{
				iot_free( iotMessage->serializedMessage );
			}
			iot_free( iotMessage );
		}
		else
		{
			// Unmark the statically allocated memory
			iotMessage->inUse = false;
		}
#else
		// Free our own serializedMessage buffer, if it was used.
		if ( iotMessage->serializedMessage != NULL )
		{
			iot_free( iotMessage->serializedMessage );
		}
		IoTHubMessage_Destroy( iotMessage->messageHandle );
		// I believe that this next is a double-free, since IoTHubMessage_Destroy() uses pointer math
		// to find the address of the beginning of the iotMessage and free it.
		// But no one has been complaining, so keep it in to double-stitch
		iot_free( iotMessage );
#endif
	}
}

/*
   Set IoT Hub Message properties.
 */
void setMessageProperties( IOT_DEVICE_CONNECTION *connection, IOT_MESSAGE *iotMessage, const IOT_DATA *data, const IOT_COMMAND *command )
{
	// Get a pointer to the structure field for the map of message properties
	// (These end up in the topic path)
	MAP_HANDLE propMap = IoTHubMessage_Properties( iotMessage->messageHandle );

	if ( data != NULL )
	{
		Log_Debug( "Setting data message property 'a' = %s", getDataType( data ) );
		Map_Add( propMap, "a", getDataType( data ) );
	}
	else if ( command != NULL )
	{
		Log_Debug( "Setting command message property 'a' = %s", getCommandType( command ) );
		Map_Add( propMap, "a", getCommandType( command ) );
		// Set the Job UUID as a property
		if ( ( command->commandType == TRANSFER_IMAGE ) || ( command->commandType == DEVICE_COMMAND_STATUS ) )
		{
			Log_Debug( "Setting command message property 'j' = %s", command->arg_1 );
			Map_Add( propMap, "j", command->arg_1 );
		}
	}
	Log_Debug( "Setting message property 'e' = json" );
	Map_Add( propMap, "e", "json" );

	if ( data != NULL )
	{
		// set the "p" property if this is a child device
		if ( data->deviceUUID != NULL )
		{
			if ( data->dataType == ALARMS )
			{
				// For alarm data, it is mandatory to add publisher information else swagger APIs will not work
				// It is recomended for alarms data, "p" (publisher) field should should always store device UUID of the Root device/Gateway
				Log_Debug( "Setting message property 'p' = %s", connection->deviceUUID );
				Map_Add( propMap, "p", connection->deviceUUID );
			}
			else if ( strcmp( data->deviceUUID, connection->deviceUUID ) != 0 )
			{
				Log_Debug( "Setting message property 'p' = %s", data->deviceUUID );
				Map_Add( propMap, "p", data->deviceUUID );
			}
		}

		// set the "r" property for custom routing
		if ( data->routingName != NULL )
		{
			Log_Debug( "Setting message property 'r' = %s", data->routingName );
			Map_Add( propMap, "r", data->routingName );
		}
	}
	else if ( command != NULL )
	{
		// set the "p" property if this is a child device
		if ( command->deviceUUID != NULL )
		{
			if ( strcmp( command->deviceUUID, connection->deviceUUID ) != 0 )
			{
				Log_Debug( "Setting message property 'p' = %s", command->deviceUUID );
				Map_Add( propMap, "p", command->deviceUUID );
			}
		}
	}
}

void setCustomMessageProperties( IOT_MESSAGE *iotMessage, const char *properties[], size_t num_properties )
{
	if ( ( properties[0] == NULL ) || ( num_properties == 0 ) )
	{
		Log_Info( "No properties provided for custom message." );
		return;
	}

	// Get a pointer to the structure field for the map of message properties
	// (These end up in the topic path)
	MAP_HANDLE propMap = IoTHubMessage_Properties( iotMessage->messageHandle );

	for ( size_t i = 0; i < num_properties; i++ )
	{
		// Get the key and value pair for each entry
		size_t index = i * 2;
		Log_Debug( "Setting message property '%s' = %s", properties[index], properties[index + 1] );
		Map_Add( propMap, properties[index], properties[index + 1] );
	}
}

#ifdef PX_GREEN_IOT_DEVICE
// For PXGreen, gets the next available message buffer.
IOT_MESSAGE *getNextMessage( IOT_DEVICE_CONNECTION *connection )
{
	IOT_MESSAGE *iotMessage = NULL;

	if ( LOCK_ERROR == Lock( lock ) )
	{
		Log_Error( "Error obtaining lock" );
		connection->lastErrorCode = IOT_SERIALIZE_ERROR;
		return ( iotMessage );
	}

	for ( uint8_t i = 0; i < IOT_MAX_CONCURRENT_MESSAGES; i++ )
	{
		if ( !connection->iotMessage[i].inUse )
		{
			connection->iotMessage[i].inUse = true;
			iotMessage = &connection->iotMessage[i];
			break;
		}
	}
	Unlock( lock );
	if ( iotMessage == NULL )
	{
		Log_Error( "No available message buffers" );
		connection->lastErrorCode = IOT_NO_AVAILABLE_MESSAGE_BUFFER;
	}

	return ( iotMessage );
}

// For PXGreen, gets the count of available message buffers.
uint8_t getFreeMessageCount( IOT_DEVICE_HANDLE deviceHandle )
{
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	uint8_t count = 0;

	for ( uint8_t i = 0; i < IOT_MAX_CONCURRENT_MESSAGES; i++ )
	{
		if ( connection && !connection->iotMessage[i].inUse )
		{
			count++;
		}
	}
	return ( count );
}

#endif


/******************************************************************************/
/*                           Message Thread                                   */
/******************************************************************************/

#ifdef PX_GREEN_IOT_DEVICE
/*
   Worker Function to process messages on the work queue for PXGreen.
   Not threaded, just executes once and returns.
 */
void *messageThread( void *arg )
{
	IOT_DEVICE_CONNECTION *connection = ( IOT_DEVICE_CONNECTION * )arg;

	if ( !connection->close )
	{
		/* Always call the timer callback (if registered),
		 * which may queue up messages to be sent when we next call DoWork.
		 * (We ignore the connection->timerCallbackInterval for PXGreen.) */
		if ( ( connection->timerCallback != NULL ) && ( connection->handle != NULL ) )
		{
			connection->timerCallback( connection->handle, connection->context );
			if ( connection->close )
			{
				// If this flag was set on our return, don't go further
				ThreadAPI_Exit( 0 );
				return ( NULL );
			}
		}

		/* Check for new messages in the queue (even if SAS token is in the process of being refreshed)
		* and process (ie, send) them. This is handled in the Low-Level AMQP or MQTT transport code. */
		IoTHubClient_LL_DoWork( connection->handle );
	}
	else
	{
		// Must have just been closed; keep this from being called again until reconnected
		ThreadAPI_Exit( 0 );
	}

	return ( NULL );
}

#else
/*
   Thread to process messages on the work queue for PXRed and others that support pthreads.
 */
void *messageThread( void *arg )
{
	time_t now;

#ifdef IOT_INCLUDE_AMQP_WEBSOCKETS
	// Tracks when we should DoWork and publish the next batch
	time_t next_do_work_time = 0;
#endif

	IOT_DEVICE_CONNECTION *connection = ( IOT_DEVICE_CONNECTION * )arg;

	while ( !connection->close )
	{
		now = get_time( NULL );

		/* Call timer callback (if registered and interval has expired),
		 * which may queue up messages to be sent when we next call DoWork. */
		if ( ( connection->timerCallback != NULL ) && ( connection->handle != NULL ) )
		{
			double seconds = difftime( now, connection->lastTimerCallbackInvoked );
			if ( seconds >= connection->timerCallbackInterval )
			{
				connection->lastTimerCallbackInvoked = now;
				connection->timerCallback( connection->handle, connection->context );
				if ( connection->close )
				{
					// If this flag was set on our return, don't go further
					continue;
				}
			}
		}

#ifdef IOT_INCLUDE_AMQP_WEBSOCKETS
		/* In order to batch messages -- currently only for AMQP -- use iot_send() as many times as you want, but
		 * hold off calling IoTHubClient_LL_DoWork() until you are ready to have the batch processed. */
		if ( !connection->wantBatching || ( connection->doWorkIntervalSeconds == 0 ) )
		{
			IoTHubClient_LL_DoWork( connection->handle );
		}
		else if ( connection->sendImmediately || ( now >= next_do_work_time ) )
		{
			IoTHubClient_LL_DoWork( connection->handle );
			next_do_work_time = now + connection->doWorkIntervalSeconds;
			Log_Debug( "DoWork at %lu %s- next DoWork scheduled for %lu", now,
					   ( connection->sendImmediately ? "(immediate) " : "" ), next_do_work_time );
			connection->sendImmediately = false;
		}
#else
		/* Check for new messages in the queue (even if SAS token is in the process of being refreshed)
		* and process (ie, send) them. This is handled in the Low-Level AMQP or MQTT transport code. */
		IoTHubClient_LL_DoWork( connection->handle );
#endif

		if ( !connection->close )
		{
			/* sleep for a specified interval before checking the queue for more messages to process */
			platform_message_thread_sleep();
		}
	}

	/* Only get here if we're shutting down */
	Log_Info( "Message Thread Exiting" );
	ThreadAPI_Exit( 0 );		// May not actually return
	return ( NULL );
}

#endif


// Tell the messageThread to close itself and exit.
void closeMessageThread( IOT_DEVICE_HANDLE deviceHandle )
{
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection != NULL )
	{
		connection->close = true;
	}
}

// Getter for the messageThreadId, given the deviceHandle.
THREAD_HANDLE getConnectionThreadHandle( IOT_DEVICE_HANDLE deviceHandle )
{
	THREAD_HANDLE handle = NULL;
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection != NULL )
	{
		handle = connection->messageThreadId;
	}
	return ( handle );
}

#ifdef IOT_INCLUDE_AMQP_WEBSOCKETS
// Setter for the connection's wantBatching flag and doWorkIntervalSeconds value.
bool iot_set_enable_batching( IOT_DEVICE_HANDLE deviceHandle, bool enable, uint32_t doWorkIntervalSeconds )
{
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection == NULL )
	{
		return ( false );
	}
	connection->wantBatching = enable;
	// Also set this to ensure we fire off the first batch
	connection->sendImmediately = enable;
	connection->doWorkIntervalSeconds = doWorkIntervalSeconds;
	if ( enable )
	{
		Log_Info( "Enable Batching, using DoWork interval of %u seconds.", doWorkIntervalSeconds );
	}
	return ( true );
}

// Sets the connection's sendImmediately flag to trigger publishing of all batched messages.
void iot_do_send_immediate( IOT_DEVICE_HANDLE deviceHandle )
{
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection != NULL )
	{
		connection->sendImmediately = true;
	}
}

#else
void iot_do_send_immediate( IOT_DEVICE_HANDLE deviceHandle )
{}

// Batching is not implemented for MQTT
bool iot_set_enable_batching( IOT_DEVICE_HANDLE deviceHandle, bool enable, uint32_t doWorkIntervalSeconds )
{
	return ( false );
}

#endif



/******************************************************************************/
/*                      Receive Device Command Callback                       */
/******************************************************************************/

int receiveC2DDeviceCommandCallback( const char *methodName,
									 const unsigned char *payload,
									 size_t size,
									 unsigned char * *response,
									 size_t *responseSize,
									 void *userContextCallback )
{
	IOT_DEVICE_CONNECTION *connection = ( IOT_DEVICE_CONNECTION * )userContextCallback;

	Log_Info( "Processing C2D command: %.*s", ( int ) size, payload );

	if ( connection == NULL )
	{
		return ( -1 );
	}

	connection->cmdInbound = connection->cmdInbound + 1;

	/* check for a valid command */
	if ( strcmp( methodName, REQUEST_DEVICE_COMMAND ) != 0 )
	{
		const char *invalidCommandResponse = "{\"Response\":\"Invalid device command.\"}";
		*responseSize = strlen( invalidCommandResponse ) + 1;
		*response = ( unsigned char * ) iot_malloc( *responseSize );
		if ( *response != NULL )
		{
			strcpy_s( ( char * )*response, *responseSize, invalidCommandResponse );
		}
		else
		{
			Log_Error( "Allocation Failed: Insufficient memory for response" );
			*responseSize = 0;
		}
		return ( 400 );
	}

	/* check if a callback was registered */
	if ( connection->c2dDeviceCommandCallback == NULL )
	{
		const char *notImplementedResponse = "{\"Response\":\"Device Commands not implemented.\"}";
		*responseSize = strlen( notImplementedResponse ) + 1;
		*response = ( unsigned char * ) iot_malloc( *responseSize );
		if ( *response != NULL )
		{
			strcpy_s( ( char * )*response, *responseSize, notImplementedResponse );
		}
		else
		{
			Log_Error( "Allocation Failed: Insufficient memory for response" );
			*responseSize = 0;
		}
		return ( 501 );
	}

	IOT_DEVICE_COMMAND *deviceCommand = NULL;

	IOT_DEVICE_COMMAND_RESPONSE commandResponse;
	commandResponse.msg_size = 0;
	commandResponse.status = 0;
	commandResponse.msg = NULL;
	commandResponse.arg_1 = NULL;
	commandResponse.arg_2 = NULL;
	commandResponse.arg_3 = NULL;
	commandResponse.arg_4 = NULL;
	commandResponse.list_1 = singlylinkedlist_create();
	commandResponse.list_1_cleanup = false;
	commandResponse.list_2 = singlylinkedlist_create();
	commandResponse.list_2_cleanup = false;

#ifdef PX_GREEN_IOT_DEVICE
	// The command handler will provide fixed text messages for PXGreen
	commandResponse.msg = NULL;
	deviceCommand = ( IOT_DEVICE_COMMAND * ) iot_malloc( sizeof( IOT_DEVICE_COMMAND ) );
#else
	// Allocate a good-sized (4KB) area for the response message
	commandResponse.msg_size = DEVICE_COMMANDS_RESPONSE_SIZE;
	commandResponse.msg = ( const char * ) iot_malloc( commandResponse.msg_size );
	if ( commandResponse.msg != NULL )
	{
		deviceCommand = ( IOT_DEVICE_COMMAND * ) iot_malloc( sizeof( IOT_DEVICE_COMMAND ) );
	}
#endif

	if ( deviceCommand == NULL )
	{
		iot_free( ( void * ) commandResponse.msg );			// if malloc was done and had succeeded
		commandResponse.msg_size = 0;				// So we know below not to free commandResponse.msg
		commandResponse.msg = "Insufficient memory for command";
		commandResponse.status = 500;				// HTTP_STATUS_INTERNAL_SERVER_ERROR 500
		Log_Error( "Allocation Failed: %s", commandResponse.msg );
		serializeDeviceCommandResponse( "C2D_Command", &commandResponse, response, responseSize );
	}
	else
	{
		memset( deviceCommand, 0, sizeof( IOT_DEVICE_COMMAND ) );
		deserializeCommand( connection, ( char * )payload, ( int )size, deviceCommand );

		if ( deviceCommand->commandMethod != NULL )
		{
			// invoke the registered "DeviceCommand" callback
			connection->c2dDeviceCommandCallback( connection->handle, connection->context, deviceCommand, &commandResponse );

			// serialize the response
			serializeDeviceCommandResponse( deviceCommand->commandMethod, &commandResponse, response, responseSize );
		}
		else
		{
			// It's very unlikely that we won't have found a commandMethod when deserializing, but just in case:
			// The iot_free() function has no effect if the pointer is NULL
			iot_free( ( void * ) commandResponse.msg );			// if its malloc had succeeded
			commandResponse.msg_size = 0;				// So we know below not to free commandResponse.msg
			commandResponse.msg = "No recognized Command Method";
			commandResponse.status = 400;		// HTTP_STATUS_BAD_REQUEST 400
			serializeDeviceCommandResponse( "NoCommandMethod", &commandResponse, response, responseSize );
		}
	}

	// Get the return status:
	int status = commandResponse.status;

	// Now cleanup our allocated resources
	// free memory from the allocated Command request structure, then free it too - if malloc succeeded
	if ( deviceCommand != NULL )
	{
		// The iot_free() function has no effect if the pointer is NULL
		if ( deviceCommand->commandDevices != NULL )
		{
			iot_list_destroy_with_items( deviceCommand->commandDevices );
			deviceCommand->commandDevices = NULL;
		}
		iot_free( deviceCommand->commandMethod );
		iot_free( deviceCommand->commandParams );
		iot_free( deviceCommand );
	}
	// free allocated items from the response structure, casting away their const trait
	if ( commandResponse.msg_size != 0 )
	{
		iot_free( ( void * ) commandResponse.msg );
	}
	iot_free( ( void * ) commandResponse.arg_1 );
	iot_free( ( void * ) commandResponse.arg_2 );
	iot_free( ( void * ) commandResponse.arg_3 );
	iot_free( ( void * ) commandResponse.arg_4 );

	/* If any strings were allocated, they must be freed.
	 * list_1 and list_2 were created by singlylinkedlist_create() above;
	 * if their item values were allocated, then they must also be destroyed here.
	 */

	// free list memory
	if ( commandResponse.list_1_cleanup == true )
	{
		iot_list_destroy_with_items( commandResponse.list_1 );
	}
	else
	{
		singlylinkedlist_destroy( commandResponse.list_1 );
	}

	if ( commandResponse.list_2_cleanup == true )
	{
		iot_list_destroy_with_items( commandResponse.list_2 );
	}
	else
	{
		singlylinkedlist_destroy( commandResponse.list_2 );
	}

	return ( status );
}

#ifndef PX_GREEN_IOT_DEVICE
/*
   Test function to print the serialized Message buffer.
 */
void iot_print_message( const IOT_DATA *data )
{
	if ( data == NULL )
	{
		Log_Error( "iot_print_message: Error IOT_DATA pointer is null" );
		return;
	}

	IOT_DEVICE_CONNECTION *connection = ( IOT_DEVICE_CONNECTION * ) iot_malloc( sizeof( IOT_DEVICE_CONNECTION ) );
	if ( connection == NULL )
	{
		Log_Error( "iot_print_message: Error allocating memory for IOT_DEVICE_CONNECTION" );
		return;
	}
	memset( connection, 0, sizeof( IOT_DEVICE_CONNECTION ) );

	IOT_MESSAGE *iotMessage = iot_malloc( sizeof( IOT_MESSAGE ) );
	if ( iotMessage == NULL )
	{
		Log_Error( "iot_print_message: Error allocating memory for IOT_MESSAGE" );
		iot_free( connection );
		return;
	}
	iotMessage->connection = connection;
	iotMessage->serializedMessage = NULL;

	if ( !serializeData( connection, iotMessage, data ) )	/* serialize data to json */
	{
		Log_Error( "iot_print_message: Error serializing message" );
	}
	else
	{
		if ( iotMessage->serializedMessage )
		{
			Log_Info( "Serialized Message: \n%s\n", iotMessage->serializedMessage );
		}
	}

	iot_free( connection );
	iot_free( iotMessage );
}

#endif

/******************************************************************************/
/*               Handle Sent Message Confirmation Callback                    */
/******************************************************************************/

/*
   Callback routine to process outbound message (Device-to-Cloud) confirmations.
 */
void handleD2CMessageConfirmationCallback( IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback )
{
	IOT_MESSAGE *message = ( IOT_MESSAGE * )userContextCallback;

	if ( message != NULL )
	{
		Log_Info( "Message Confirmation Received, Seq: %lu", message->messageId );

		IOT_DEVICE_CONNECTION *connection = ( IOT_DEVICE_CONNECTION * )message->connection;
		if ( ( connection != NULL ) && !connection->close )
		{
			if ( result != IOTHUB_CLIENT_CONFIRMATION_OK )
			{
				connection->unconfirmedMessageCount++;
				connection->connectionStatus = IOT_UNKNOWN;
				switch ( result )
				{
					case IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT:
						// Note: we used to retry in this case, but that was a useless effort, as the
						// connection is always lost at this point.
						connection->connectionStatus = IOT_CLOSING;
						Log_Error( "Message send failed (message timeout), Seq: %lu", message->messageId );
						break;
					case IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY:
						// iothub_client is being destroyed.
						// Signal that the connection is being closed.
						connection->connectionStatus = IOT_CLOSING;
						Log_Error( "Message send failed (because destroy), Seq: %lu", message->messageId );
						break;
					case IOTHUB_CLIENT_CONFIRMATION_ERROR:
						// Make the assumption that this error will occur when the
						// iot_hub maximum daily message limit has been reached, though there are other causes as well.
						// Signal the iot_agent to go into idle mode.
						connection->connectionStatus = IOT_OTHER;
						Log_Error( "Message send failed (error), Seq: %lu", message->messageId );
						break;
					default:
						Log_Error( "Message send failed (unknown), Seq: %lu", message->messageId );
				}
			}
			else
			{
				connection->msgConfirmed = connection->msgConfirmed + 1;
				connection->unconfirmedMessageCount = 0;// reset count - communication not lost
				connection->connectionStatus = IOT_OPENED;
			}

			if ( connection->messageDoneCallback != NULL )
			{
				connection->messageDoneCallback( connection->context, result, message );
			}
		}

		// Delete the message and free its resources
		releaseIotMessage( message );
	}
}

/******************************************************************************/
/*                     Handle Connection Status Callback                      */
/******************************************************************************/

/*
   Callback routine to process connection status changes.
   For AMQP, see on_device_state_changed_callback() in iothubtransport_amqp_common.c for how the statusCode and
   reasonCode are translated from the AMQP states before calling this function.
 */
void handleConnectionStatusCallback( IOTHUB_CLIENT_CONNECTION_STATUS statusCode, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reasonCode, void *userContextCallback )
{
	Log_Info( "Connection Status Received, Result: %d, Reason: %d", statusCode, reasonCode );

	IOT_DEVICE_CONNECTION *connection = ( IOT_DEVICE_CONNECTION * ) userContextCallback;

	if ( connection != NULL )
	{
		bool do_callback = true;
		if ( ( statusCode == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED ) && ( reasonCode == IOTHUB_CLIENT_CONNECTION_OK ) )
		{
			connection->connectionStatus = IOT_OPENED;
		}
		else if ( reasonCode == IOTHUB_CLIENT_CONNECTION_NO_NETWORK )
		{
			/* We may get this when first opening the connection if the low-level
			 * Azure code realizes that the attempt to open failed. */
			connection->connectionStatus = IOT_CLOSED;
			Log_Info( "Got low-level network loss indication." );
		}
		else if ( reasonCode == IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN )
		{
			// Let the Azure code manage the reconnection, so no callback here
			// (Don't frighten the natives)
			/* The desire here was to let Azure manage the reconnection for us.
			 * do_callback = false;
			 * connection->connectionStatus = IOT_CONNECTING; -- will have to experiment and see whether to do this or do nothing
			 * Log_Debug( "Letting Azure reconnect, after SAS Token epxiration" );
			 * However, it turns out that this doesn't happen (yet - maybe someday).
			 *
			 * So I looked into arranging an under-the-hood reconnect here, without
			 * alerting the IoT Agent process, but could not pull it off:
			 * - The disconnect and connect function allocate and free structures,
			 *    which would make it impossible to let the IoT Agent get the updated "handle".
			 * - The destroy function frees enqued messages, which we had wanted to avoid.
			 * - All of these structures involve opaque pointers that we can't decode here.
			 * - No suitable "just reconnect" Azure method exists that we could use.
			 * - I did try to just call the DoWork function, but it did not work (reconnect on its own).
			 * So we have to do the other alternative: declare the connection Closed, and start over.
			 * Though that will still mean that we will lose any messages that were queued up. */
			connection->connectionStatus = IOT_CLOSED;
			Log_Info( "Got SAS Token epxiration notice; must close and reconnect." );
		}
		else
		{
			connection->connectionStatus = IOT_CLOSED;
		}
		connection->connectionStatusCode = statusCode;
		connection->connectionReasonCode = reasonCode;

		/* Invoke connection status callback (if registered)
		 * Invoking the callback needs to be run on a separate thread */
		if ( do_callback && ( connection->connectionStatusCallback != NULL ) )
		{
#ifndef PX_GREEN_IOT_DEVICE
			THREAD_HANDLE handle = NULL;
			if ( ThreadAPI_Create( &handle, ( THREAD_START_FUNC )connectionStatusChangedThread, connection ) != THREADAPI_OK )
			{
				Log_Error( "Error calling connection status changed method" );
			}

			Log_Info( "Created new thread for connectionStatusChanged" );
			/* Now wait for that very temporary thread to complete;
			 * use join so we free the handle and avoid a leak here. */
			int exitStatus;
			ThreadAPI_Join( handle, &exitStatus );
#else
			connection->connectionStatusCallback( connection->handle, connection->context, connection->connectionStatusCode, connection->connectionReasonCode );
#endif
		}
		else if ( do_callback )
		{
			Log_Debug( "No Connection status callback defined!" );
		}
	}
	else
	{
		Log_Debug( "No Connection structure defined!" );
	}
}

/*
   Invokes the registered connection status callback.
   Very temporary thread run just to invoke the status callback, then exited.
 */
void *connectionStatusChangedThread( void *arg )
{
	IOT_DEVICE_CONNECTION *connection = ( IOT_DEVICE_CONNECTION * )arg;

	if ( connection != NULL )
	{
		connection->connectionStatusCallback( connection->handle, connection->context, connection->connectionStatusCode, connection->connectionReasonCode );
	}

	/* Log and exit connection status thread */
	Log_Info( "Exiting Connection Status Thread" );
	/* Do a clean exit */
	ThreadAPI_Exit( 0 );

	return ( NULL );
}

#ifdef __cplusplus
}
#endif

