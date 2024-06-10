#include "iot_device.h"
#include <stdarg.h>
#include <time.h>

/******************************************************************************/
/*                      Open connection to IoT Hub                            */
/******************************************************************************/

/*
   Open a connection to IoT Hub.
 */
IOT_DEVICE_HANDLE iot_open( const IOT_CONNECTION_OPTIONS *connectionOptions, int *status, void *owner_context )
{
	assert( connectionOptions != NULL );

	#if defined( IOT_ENABLE_ERROR_LOGGING ) || defined( IOT_ENABLE_INFO_LOGGING ) || defined( IOT_ENABLE_DEBUG_LOGGING )
		#ifdef IOT_ENABLE_LOG_SIGHANDLER
	//Install signal handler to change logging at runtime
	installLogSignalHandler();
		#endif
	#endif

	/* initialize global variables */
	initializeGlobalVariables();

	/* validate the connection options */
	if ( !validateConnectionOptions( connectionOptions, status ) )
	{
		Log_Error( "Invalid connection options.  IoT Hub connection not established." );
		return ( NULL );
	}

	/* perform platform initialization */
	if ( !initializePlatform( status ) )
	{
		Log_Error( "Platform initialization failed. Iot Hub connection not established." );
		return ( NULL );
	}

	Log_Info( "IoT Hub Connection: %s", connectionOptions->connectionString );

	/* open a connection to IoT Hub */
	IOTHUB_CLIENT_LL_HANDLE handle = NULL;
	bool validProtocol = false;
#ifdef IOT_INCLUDE_HTTP
	if ( connectionOptions->protocol == HTTP )
	{
		Log_Debug( "Connecting to IoT Hub using HTTP Protocol" );
		validProtocol = true;
		handle = IoTHubClient_LL_CreateFromConnectionString( connectionOptions->connectionString, HTTP_Protocol );
	}
#endif
#ifdef IOT_INCLUDE_AMQP_WEBSOCKETS
	if ( connectionOptions->protocol == AMQP_WEBSOCKETS )
	{
		Log_Debug( "Connecting to IoT Hub using AMQP Websockets" );
		validProtocol = true;
		handle = IoTHubClient_LL_CreateFromConnectionString( connectionOptions->connectionString, AMQP_Protocol_over_WebSocketsTls );
	}
#endif
#ifdef IOT_INCLUDE_MQTT_WEBSOCKETS
	if ( connectionOptions->protocol == MQTT_WEBSOCKETS )
	{
		Log_Debug( "Connecting to IoT Hub using MQTT Websockets" );
		validProtocol = true;
		handle = IoTHubClient_LL_CreateFromConnectionString( connectionOptions->connectionString, MQTT_WebSocket_Protocol );
	}
#endif

	if ( !validProtocol )
	{
		Log_Error( "Invalid protocol" );
		*status = IOT_INVALID_ARGUMENT;
		return ( NULL );
	}

	/* check if connection was successful */
	if ( handle == NULL )
	{
		Log_Error( "IoT Hub connection failed" );
		*status = IOT_CONNECTION_FAILED;
		return ( NULL );
	}

	/* initialize connection */
	IOT_DEVICE_CONNECTION *connection = initializeConnection( handle, connectionOptions, owner_context );
	if ( connection == NULL )
	{
		Log_Error( "Failed to create connection" );
		*status = IOT_CONNECTION_FAILED;
		IoTHubClient_LL_Destroy( handle );		/* Prevent memory leaks; free this data. */
		return ( NULL );
	}
	Log_Info( "Created connection" );

	/* create a message thread */
	if ( ThreadAPI_Create( &connection->messageThreadId, ( THREAD_START_FUNC )messageThread, connection ) != THREADAPI_OK )
	{
		Log_Error( "Error creating message thread" );
		*status = IOT_THREAD_CREATION_FAILED;
		IoTHubClient_LL_Destroy( handle );
		return ( NULL );
	}
	Log_Info( "Created message thread" );

	/* set the new style IoT Hub C2D device command callback */
	if ( connectionOptions->protocol != HTTP )
	{
		if ( IoTHubClient_LL_SetDeviceMethodCallback( handle, receiveC2DDeviceCommandCallback, connection ) != IOTHUB_CLIENT_OK )
		{
			Log_Error( "Error setting device command callback" );
			*status = IOT_SET_DEVICE_COMMAND_CALLBACK_FAILED;
			IoTHubClient_LL_Destroy( handle );
			return ( NULL );
		}
	}

	*status = IOT_OK;
	connection->lastErrorCode = IOT_OK;

	return ( handle );
}

/******************************************************************************/
/*                      Close connection to IoT Hub                           */
/******************************************************************************/

/*
   Close a connection to IoT Hub.
 */
extern bool iot_close( IOT_DEVICE_HANDLE deviceHandle )
{
	Log_Debug( "Closing IoT Hub connection" );

	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	if ( connection != NULL )
	{
		connection->connectionStatus = IOT_CLOSING;
		connection->close = true;
		int exitStatus;
		if ( connection->messageThreadId != NULL )
		{
			if ( ThreadAPI_Join( connection->messageThreadId, &exitStatus ) != THREADAPI_OK )
			{
				Log_Error( "iot_close: Error joining thread" );
			}
		}

		IoTHubClient_LL_Destroy( connection->handle );

		deleteConnection( connection );

#ifdef WANT_BASE64_ENCODING
		base64_cleanup();
#endif

		return ( true );
	}

	return ( false );
}

/******************************************************************************/
/*                      Logging configuration                     */
/******************************************************************************/
#if defined( IOT_ENABLE_ERROR_LOGGING ) || defined( IOT_ENABLE_INFO_LOGGING ) || defined( IOT_ENABLE_DEBUG_LOGGING )
/**
 * Sets the logging level for the Device SDK at runtime.  Typically used for debugging.
 * @param level Level of messages to be logged.
 */
void iot_setLogLevel( enum IOT_LOG_LEVELS level )
{
	setDeviceSDKLogLevel( level );
}

/**
 * Registers callback function for user to implement custom logging functionality.
 * @param callback - Function pointer.
 */
void iot_registerCustomLoggerCallback( CustomLoggerCallback callback )
{
	setSDKLogCallback( callback );
}

/**
 * Unregisters custom logging function pointers to return to default logging.
 */
void iot_unregisterCustomLoggerCallback()
{
	deleteSDKLogCallback();
}

#endif


/******************************************************************************/
/*                           Send data to IoT Hub                             */
/******************************************************************************/

bool iot_send( IOT_DEVICE_HANDLE deviceHandle, IOT_DATA *data, int *message_size )
{
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection == NULL )
	{
		return ( false );
	}

	if ( data == NULL )
	{
		Log_Error( "Missing IOT_DATA" );
		connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		return ( false );
	}

	if ( connection->connectionStatus != IOT_OPENED )
	{
		Log_Info( "Connection is not open (status = %u), message discarded.", connection->connectionStatus );
		// Don't change the connection->lastErrorCode
		return ( false );
	}

	/*
	   Check if we've lost communication to IoT Hub.
	   Prevent additional sends if the maximum number of unconfirmed messages has been reached.
	 */
#if IOT_MAX_UNCONFIRMED_MESSAGE_COUNT != 0
	if ( connection->unconfirmedMessageCount > IOT_MAX_UNCONFIRMED_MESSAGE_COUNT )
	{
		Log_Error( "Send request rejected - maximum number of unconfirmed messages has been reached" );
		connection->lastErrorCode = IOT_SEND_REQUEST_REJECTED;
		return ( false );
	}
#endif

	/* create an IoT Hub message */
	IOT_MESSAGE *iotMessage = createMessage( connection, data, NULL );
	if ( iotMessage == NULL )
	{
		Log_Error( "Error creating IoT Message.  Message will not be sent." );
		return ( false );
	}

	/* set message properties */
	setMessageProperties( connection, iotMessage, data, NULL );

	/* send the message to IoT Hub (asynchronously). It will be freed when confirmed. */
	if ( IoTHubClient_LL_SendEventAsync( connection->handle, iotMessage->messageHandle, handleD2CMessageConfirmationCallback, iotMessage ) != IOTHUB_CLIENT_OK )
	{
#ifdef PX_GREEN_IOT_DEVICE
		iotMessage->inUse = false;
#endif
		Log_Error( "Error occurred sending message to IoT Hub" );
		connection->lastErrorCode = IOT_SEND_ERROR;
		return ( false );
	}
	else
	{
		connection->msgOutbound = connection->msgOutbound + 1;
		if ( message_size != NULL )
		{
			*message_size = iotMessage->serializedBufferSize;
		}
	}

	//Log device tree message as info due to their importance, but log all other message types as debug so they don't flood the log
	if ( data->dataType == DEVICE_TREE )
	{
		Log_Info( "Message Sent, Seq: %lu\n     Serialized Message: %s", iotMessage->messageId, iotMessage->serializedMessage );
	}
	else
	{
		Log_Debug( "Message Sent, Seq: %lu\n     Serialized Message: %s", iotMessage->messageId, iotMessage->serializedMessage );
	}
	// Return the messageID in the IOT_DATA structure, for the application's convenience in tracking the message.
	data->messageId = iotMessage->messageId;
	connection->lastErrorCode = IOT_OK;

	return ( true );
}

unsigned long iot_sendCustomMessage( IOT_DEVICE_HANDLE deviceHandle, const char *payload, const char *properties[], size_t num_properties )
{
	unsigned long message_id = 0;
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection == NULL )
	{
		return ( message_id );
	}

	if ( ( payload == NULL ) || ( properties == NULL ) || ( num_properties == 0 ) )
	{
		Log_Error( "Missing necessary custom arguments" );
		connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		return ( message_id );
	}

	if ( connection->connectionStatus != IOT_OPENED )
	{
		Log_Info( "Connection is not open (status = %u), message discarded.", connection->connectionStatus );
		// Don't change the connection->lastErrorCode
		return ( message_id );
	}

	/*
	   Check if we've lost communication to IoT Hub.
	   Prevent additional sends if the maximum number of unconfirmed messages has been reached.
	 */
#if IOT_MAX_UNCONFIRMED_MESSAGE_COUNT != 0
	if ( connection->unconfirmedMessageCount > IOT_MAX_UNCONFIRMED_MESSAGE_COUNT )
	{
		Log_Error( "Send request rejected - maximum number of unconfirmed messages has been reached" );
		connection->lastErrorCode = IOT_SEND_REQUEST_REJECTED;
		return ( message_id );
	}
#endif

	/* Create and allocate an IoT Hub message */
	IOT_MESSAGE *iotMessage = createCustomMessage( connection, payload );
	if ( iotMessage == NULL )
	{
		// Already logged the reason
		return ( message_id );
	}
#ifdef PX_GREEN_IOT_DEVICE
	// We should be freeing our payload on return.
	// Here, we just null out the pointer for it that was copied into iotMessage
	iotMessage->serializedMessage = NULL;
#endif

	/* set message properties */
	setCustomMessageProperties( iotMessage, properties, num_properties );

	/* send the message to IoT Hub (asynchronously). It will be freed when confirmed. */
	if ( IoTHubClient_LL_SendEventAsync( connection->handle, iotMessage->messageHandle, handleD2CMessageConfirmationCallback, iotMessage ) != IOTHUB_CLIENT_OK )
	{
		Log_Error( "Error occurred sending message to IoT Hub" );
		connection->lastErrorCode = IOT_SEND_ERROR;
		return ( message_id );
	}
	else
	{
		message_id = iotMessage->messageId;
		connection->msgOutbound = connection->msgOutbound + 1;
	}

	Log_Info( "Custom Message Sent, Seq: %lu, Message: \n   %.80s ...", iotMessage->messageId, payload );
	connection->lastErrorCode = IOT_OK;

	return ( message_id );
}

/******************************************************************************/
/*                           Send a command to IoT Hub                        */
/******************************************************************************/

bool iot_sendCommand( IOT_DEVICE_HANDLE deviceHandle, const IOT_COMMAND *command, int *message_size )
{
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection == NULL )
	{
		return ( false );
	}

	if ( command == NULL )
	{
		Log_Error( "Missing IOT_COMMAND" );
		connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		return ( false );
	}

	if ( connection->handle == NULL )
	{
		Log_Error( "Connection is closed" );
		return ( false );
	}

	/*
	   Check if we've lost communication to IoT Hub.
	   Prevent additional sends if the maximum number of unconfirmed messages has been reached.
	 */
#if IOT_MAX_UNCONFIRMED_MESSAGE_COUNT != 0
	if ( connection->unconfirmedMessageCount > IOT_MAX_UNCONFIRMED_MESSAGE_COUNT )
	{
		Log_Error( "Send request rejected - maximum number of unconfirmed messages has been reached" );
		connection->lastErrorCode = IOT_SEND_REQUEST_REJECTED;
		return ( false );
	}
#endif

	/* create an IoT Hub message */
	IOT_MESSAGE *iotMessage = createMessage( connection, NULL, command );
	if ( iotMessage == NULL )
	{
		Log_Error( "Error creating IoT Message. Command will not be sent." );
		return ( false );
	}

	/* set message properties */
	setMessageProperties( connection, iotMessage, NULL, command );

	/* send the message to IoT Hub (asynchronously). It will be freed when confirmed. */
	if ( IoTHubClient_LL_SendEventAsync( connection->handle, iotMessage->messageHandle, handleD2CMessageConfirmationCallback, iotMessage ) != IOTHUB_CLIENT_OK )
	{
#ifdef PX_GREEN_IOT_DEVICE
		iotMessage->inUse = false;
#endif
		Log_Error( "Error occurred sending message to IoT Hub" );
		connection->lastErrorCode = IOT_SEND_ERROR;
		return ( false );
	}
	else
	{
		connection->msgOutbound = connection->msgOutbound + 1;
		if ( message_size != NULL )
		{
			*message_size = iotMessage->serializedBufferSize;
		}
	}

	Log_Info( "Message Sent, Seq: %lu\n     Serialized Command Message: %s", iotMessage->messageId, iotMessage->serializedMessage );
	connection->lastErrorCode = IOT_OK;

	return ( true );
}

/******************************************************************************/
/*                  Send Device Command Status Update                         */
/******************************************************************************/

/*
   Send a status update for a device command.
 */
bool iot_updateDeviceCommandStatus( IOT_DEVICE_HANDLE deviceHandle,
									const char *commandID,
									const char *deviceUUID,
									const char *task,
									const IOT_COMMAND_STATUS status,
									const char *description,
									int *message_size )
{
	assert( commandID != NULL );
	assert( deviceUUID != NULL );
	assert( task != NULL );

	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	if ( connection == NULL )
	{
		return ( false );
	}

	char commandStatus[12];
	strncpy( commandStatus, "unknown", 12 );
	if ( status == COMMAND_INITIATED )
	{
		strncpy( commandStatus, "initiated", 12 );
	}
	else if ( status == COMMAND_PROCESSING )
	{
		strncpy( commandStatus, "processing", 12 );
	}
	else if ( status == COMMAND_SUCCEEDED )
	{
		strncpy( commandStatus, "succeeded", 12 );
	}
	else if ( status == COMMAND_FAILED )
	{
		strncpy( commandStatus, "failed", 12 );
	}


	IOT_COMMAND cmd;
	cmd.commandType = DEVICE_COMMAND_STATUS;
	cmd.deviceUUID = deviceUUID;
	cmd.arg_1 = commandID;
	cmd.arg_2 = task;
	cmd.arg_3 = commandStatus;
	cmd.arg_4 = description;
	cmd.arg_5 = NULL;

	return ( iot_sendCommand( deviceHandle, &cmd, message_size ) );
}

/******************************************************************************/
/*                             Get Methods                                    */
/******************************************************************************/

/*
   Get the status of the IoT Hub connection.
 */
bool iot_getStatus( IOT_DEVICE_HANDLE deviceHandle, IOT_CONNECTION_STATUS *connectionStatus )
{
	assert( connectionStatus != NULL );

	// Get our Connection structure from the Client Device's "handle"
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	// Keep in mind that connection->handle == deviceHandle in that structure.

	if ( connection == NULL )
	{
		return ( false );
	}

	/* Let's see if we can detect if the connection has failed at a low-level.
	 *
	 * For AMQP, we have patched the IoTHubTransport_AMQP_Common_GetSendStatus() function to
	 * check the connection status and return IOTHUB_CLIENT_ERROR if the connection isn't
	 * Open (so send would fail).
	 * Also check that connection->messageThreadId has been initialized,
	 * else we are probably in the test code without a real connection, and should skip this call. */
	if ( ( connection->connectionStatus != IOT_CLOSED ) && ( connection->messageThreadId != 0 ) )
	{
		IOTHUB_CLIENT_STATUS iotHubClientStatus;
		IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_GetSendStatus( deviceHandle, &iotHubClientStatus );
		if ( result != IOTHUB_CLIENT_OK )
		{
			Log_Info( "Low-level connection unexpectedly closed." );
			connection->connectionStatus = IOT_CLOSED;
		}
		// Else we ignore the iotHubClientStatus == IDLE or BUSY
	}
	//coverity[var_deref_op:FALSE]
	connectionStatus->connectionStatus = connection->connectionStatus;
	connectionStatus->msgOutbound = connection->msgOutbound;
	connectionStatus->msgConfirmed = connection->msgConfirmed;
	connectionStatus->msgInbound = connection->msgInbound;
	connectionStatus->cmdInbound = connection->cmdInbound;
	connectionStatus->lastErrorCode = connection->lastErrorCode;

	connection->lastErrorCode = IOT_OK;

	return ( true );
}

/******************************************************************************/
/*                      Callback Register\UnRegister                          */
/******************************************************************************/

/*
   Register the "CloudToDevice" device command callback.
 */
bool iot_registerC2DDeviceCommandCallback( IOT_DEVICE_HANDLE deviceHandle, C2DDeviceCommandCallback callback )
{
	bool result = false;

	assert( callback != NULL );
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	if ( connection != NULL )
	{
		connection->c2dDeviceCommandCallback = callback;
		connection->lastErrorCode = IOT_OK;
		result = true;
	}
	return ( result );
}

/*
   Register the "TIMER" callback.
 */
bool iot_registerTimerCallback( IOT_DEVICE_HANDLE deviceHandle, int interval, TimerCallback callback )
{
	bool result = false;

	assert( callback != NULL );
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	if ( connection != NULL )
	{
		connection->timerCallback = callback;
		connection->timerCallbackInterval = interval;
		if ( interval < 0 )
		{
			connection->timerCallbackInterval = 10;	/* default to 10 seconds if given an invalid interval */
		}
		connection->lastErrorCode = IOT_OK;
		result = true;
	}
	return ( result );
}

/*
   Register the "Connection Status" callback.
 */
bool iot_registerConnectionStatusCallback( IOT_DEVICE_HANDLE deviceHandle, ConnectionStatusCallback callback )
{
	bool result = false;

	assert( callback != NULL );
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	if ( connection != NULL )
	{
		connection->connectionStatusCallback = callback;
		connection->lastErrorCode = IOT_OK;
		result = true;
	}
	return ( result );
}

// Registers an optional callback function to be invoked when a sent Message (D2C) is done, either on Success or Failure.
bool iot_registerMessageDoneCallback( IOT_DEVICE_HANDLE deviceHandle, MessageDoneCallback callback )
{
	bool result = false;

	assert( callback != NULL );
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	if ( connection != NULL )
	{
		connection->messageDoneCallback = callback;
		result = true;
	}
	return ( result );
}

/* Unregister the "CloudToDevice" device command callback. */
bool iot_unRegisterC2DDeviceCommandCallback( IOT_DEVICE_HANDLE deviceHandle )
{
	bool result = false;
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection != NULL )
	{
		connection->c2dDeviceCommandCallback = NULL;
		connection->lastErrorCode = IOT_OK;
		result = true;
	}
	return ( result );
}

/* Unregister the "TIMER" callback. */
bool iot_unRegisterTimerCallback( IOT_DEVICE_HANDLE deviceHandle )
{
	bool result = false;
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection != NULL )
	{
		connection->timerCallback = NULL;
		connection->lastErrorCode = IOT_OK;
		result = true;
	}
	return ( result );
}

/* Unregister the "Connection Status" callback.  */
bool iot_unRegisterConnectionStatusCallback( IOT_DEVICE_HANDLE deviceHandle )
{
	bool result = false;
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection != NULL )
	{
		connection->connectionStatusCallback = NULL;
		connection->lastErrorCode = IOT_OK;
		result = true;
	}
	return ( result );
}

/* Unregister the "Message Done" callback.  */
bool iot_unRegisterMessageDoneCallback( IOT_DEVICE_HANDLE deviceHandle )
{
	bool result = false;
	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );

	if ( connection != NULL )
	{
		connection->messageDoneCallback = NULL;
		result = true;
	}
	return ( result );
}

/******************************************************************************/
/*                                List Methods                                */
/******************************************************************************/

/*
 * Destroys a list of items while also freeing the memory allocated for each item.
 */
void iot_list_destroy_with_items( SINGLYLINKEDLIST_HANDLE listHandle )
{
	assert( listHandle != NULL );

	/* Walk through the list items and free each one's "value" (->item member) */
	LIST_ITEM_HANDLE item = singlylinkedlist_get_head_item( listHandle );
	while ( item != NULL )
	{
		char *item_value = ( char * ) singlylinkedlist_item_get_value( item );
		iot_free( item_value );
		item = singlylinkedlist_get_next_item( item );
	}

	singlylinkedlist_destroy( listHandle );
}

/******************************************************************************/
/*                            Blob Methods                                    */
/******************************************************************************/

 #ifndef DONT_USE_UPLOADTOBLOB
/*
   Upload to blob storage.
   Uses HTTP transport.
 */
bool iot_uploadToBlob( IOT_DEVICE_HANDLE deviceHandle, const char *blobPath, const unsigned char *source, size_t size )
{
	assert( blobPath != NULL );

	assert( source != NULL );

	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	if ( connection == NULL )
	{
		return ( false );
	}

	if ( IoTHubClient_LL_UploadToBlob( deviceHandle, blobPath, source, size ) != IOTHUB_CLIENT_OK )
	{
		Log_Error( "Error uploading to blob storage" );
		connection->lastErrorCode = IOT_UPLOAD_TO_BLOB_FAILED;
		return ( false );
	}

	connection->lastErrorCode = IOT_OK;

	return ( true );
}

static IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_RESULT blob_getDataCallback( IOTHUB_CLIENT_FILE_UPLOAD_RESULT result, unsigned char const * *data, size_t *size, void *context )
{
	const size_t block_size = sizeof( char ) * 1024 * 100;		//100 KB Block size. For 20 MB file, there will be approx 200 blocks
	struct blob_file *b_file = ( struct blob_file * )context;
	size_t total_full_blocks = ( b_file->size ) / block_size;
	time_t current_time = time( NULL );

	Log_Debug( "blob_getDataCallback" );

	if ( ( b_file->timeout_duration != 0 ) && ( current_time - b_file->start_time > b_file->timeout_duration ) )
	{
		Log_Error( "blob_getDataCallback : Timeout occurred. Aborting blob upload." );
		b_file->is_timed_out = true;
		// IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_ABORT to indicate that the upload shall not continue
		return ( IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_ABORT );
	}

	if ( result == FILE_UPLOAD_OK )
	{
		if ( ( data != NULL ) && ( size != NULL ) )
		{
			if ( b_file->current_block < total_full_blocks )
			{
				// This is the full block with block_size bytes of data
				*data = ( const unsigned char * )( ( b_file->source ) + ( b_file->current_block * block_size ) );
				*size = block_size;
				Log_Debug( "blob_getDataCallback : sending full block %d", b_file->current_block );
				b_file->current_block++;
			}
			else if ( ( b_file->current_block == total_full_blocks ) && ( ( b_file->size % block_size ) != 0 ) )
			{
				// This is the last block in file which is less than block_size
				*data = ( const unsigned char * )( ( b_file->source ) + ( b_file->current_block * block_size ) );
				*size = ( b_file->size % block_size );
				Log_Debug( "blob_getDataCallback : sending partial block %d", b_file->current_block );
				b_file->current_block++;
			}
			else
			{
				// Setting data to NULL and/or passing size as zero indicates the upload is completed.
				*data = NULL;
				*size = 0;
				Log_Debug( "blob_getDataCallback : Total blocks sent = %d", b_file->current_block );
			}
		}
		else
		{
			// The last call to this callback is to indicate the result of uploading the previous data block provided.
			// Note: In this last call, data and size pointers are NULL.
			Log_Info( "blob_getDataCallback : Last call to getDataCallback. Total blocks sent = %d", b_file->current_block );
		}
	}
	else
	{
		Log_Error( "blob_getDataCallback : Received unexpected result. Total blocks sent = %zu\n", b_file->current_block );
		return ( IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_ABORT );
	}

	// IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_OK to indicate that the upload shall continue.
	return ( IOTHUB_CLIENT_FILE_UPLOAD_GET_DATA_OK );
}

IotBlobUploadErrorEnums iot_uploadToBlob_async( IOT_DEVICE_HANDLE deviceHandle, const char *blobPath, const unsigned char *source, size_t size, time_t timeout_duration )
{
	IotBlobUploadErrorEnums ret;
	struct blob_file *b_file;

	Log_Debug( "iot_uploadToBlob_async : starting blob upload\n" );

	IOT_DEVICE_CONNECTION *connection = getConnection( deviceHandle );
	if ( connection == NULL )
	{
		Log_Error( "iot_uploadToBlob_async : getConnection failed\n" );
		return ( IOT_BLOB_UPLOAD_DONE_ERROR );
	}

	// Initialize blob file details before launching async upload
	b_file = malloc( sizeof( struct blob_file ) );
	b_file->source = source;
	b_file->size = size;
	b_file->current_block = 0;
	b_file->is_timed_out = false;
	b_file->timeout_duration = timeout_duration;
	b_file->start_time = time( NULL );

	if ( IoTHubClient_LL_UploadMultipleBlocksToBlobEx( deviceHandle, blobPath, blob_getDataCallback, ( void * )b_file ) != IOTHUB_CLIENT_OK )
	{
		Log_Error( "iot_uploadToBlob_async : Error uploading to blob storage\n" );
		ret = IOT_BLOB_UPLOAD_DONE_ERROR;
	}
	else
	{
		connection->lastErrorCode = IOT_OK;
		if ( b_file->is_timed_out == true )
		{
			Log_Error( "iot_uploadToBlob_async : Error uploading to blob storage - timeout\n" );
			ret = IOT_BLOB_UPLOAD_TIMEOUT;
		}
		else
		{
			Log_Info( "iot_uploadToBlob_async : Success uploading to blob storage\n" );
			ret = IOT_BLOB_UPLOAD_DONE_SUCCESS;
		}
	}

	free( b_file );
	return ( ret );
}

 #endif




