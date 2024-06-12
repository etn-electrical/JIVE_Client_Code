#include "iot_device.h"

#define VALUE_FIELD_LENGTH 50
#define ALARM_ID_PREFIX "00000000-0000-0000-0000-"

/*
   Serialize data in Json format to be transmitted from Device-to-Cloud.
 */
bool serializeData( IOT_DEVICE_CONNECTION *connection, IOT_MESSAGE *iotMessage, const IOT_DATA *data )
{
	if ( connection == NULL )
	{
		Log_Error( "Missing IOT_DEVICE_CONNECTION" );
		return ( false );
	}

	if ( iotMessage == NULL )
	{
		Log_Error( "Missing IOT_MESSAGE" );
		connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		return ( false );
	}
	if ( data == NULL )
	{
		Log_Error( "Missing IOT_DATA" );
		connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		return ( false );
	}

	if ( data->dataType == DEVICE_TREE )
	{
		if ( data->devices == NULL )
		{
			Log_Error( "DEVICE_TREE: Missing IOT_DATA.devices" );
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
			return ( false );
		}
		return ( serializeDeviceTreeJson( iotMessage, data ) );
	}
	else if ( data->dataType == DEVICES_REALTIME )
	{
		if ( data->devicesRealtime == NULL )
		{
			Log_Error( "DEVICES_REALTIME: Missing IOT_DATA.devicesRealtime" );
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
			return ( false );
		}
		if ( ( data->deviceUUID == NULL ) || ( strlen( data->deviceUUID ) == 0 ) )
		{
			Log_Error( "DEVICES_REALTIME: Missing IOT_DATA.deviceUUID" );
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
			return ( false );
		}
		return ( serializeDevicesRealtimeJson( iotMessage, data ) );
	}
	else if ( data->dataType == CHANNEL_REALTIMES )
	{
		if ( ( data->deviceUUID == NULL ) || ( strlen( data->deviceUUID ) == 0 ) )
		{
			Log_Error( "CHANNEL_REALTIMES: Missing\\Invalid IOT_DATA.deviceUUID" );
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
			return ( false );
		}
		if ( data->channelRealtimes == NULL )
		{
			Log_Error( "CHANNEL_REALTIMES: Missing IOT_DATA.channelRealtimes" );
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
			return ( false );
		}
		return ( serializeChannelRealtimesJson( iotMessage, data ) );
	}
	else if ( data->dataType == TRENDS )
	{
		if ( ( data->deviceUUID == NULL ) || ( strlen( data->deviceUUID ) == 0 ) )
		{
			Log_Error( "TRENDS: Missing\\Invalid IOT_DATA.deviceUUID" );
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
			return ( false );
		}
		if ( data->trends == NULL )
		{
			Log_Error( "TRENDS: Missing IOT_DATA.trends" );
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
			return ( false );
		}
		return ( serializeTrendsJson( iotMessage, data ) );
	}
#ifndef IOT_NO_DEVICE_ALARMS
	else if ( data->dataType == ALARMS )
	{
		if ( ( data->deviceUUID == NULL ) || ( strlen( data->deviceUUID ) == 0 ) )
		{
			Log_Error( "ALARMS: Missing\\Invalid IOT_DATA.deviceUUID" );
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
			return ( false );
		}
		if ( data->alarms == NULL )
		{
			Log_Error( "ALARMS: Missing IOT_DATA.alarms" );
			connection->lastErrorCode = IOT_INVALID_ARGUMENT;
			return ( false );
		}
		return ( serializeAlarmsJson( iotMessage, data ) );
	}
#endif
	else
	{
		Log_Error( "Invalid IOT_DATA.dataType" );
		connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		return ( false );
	}
}

/*
   Serialize device list

   Sample Json:
               {
                    "d":
                        {
                            "d":"91651651-9BF7-490D-A332-000000000000",
                            "profile":"00000000-0000-0000-00000000000000000",
                            "name":"publisher",
                            "serial":"serial",
                            "asset":"asset",
                            "mac":"mac",
                            "ds":[
                                    {
                                        "d":"91651651-9BF7-490D-A332-111111111111",
                                        "profile":"11111111-1111-1111-11111111111111111",
                                        "name":"name 1",
                                        "serial":"serial 1",
                                        "asset":"asset 1",
                                        "mac":"mac 1",
                                        "ds":[]
                                    },
                                    {
                                        "d":"91651651-9BF7-490D-A332-222222222222",
                                        "profile":"22222222-2222-2222-22222222222222222",
                                        "name":"name 2",
                                        "serial":"serial 2",
                                        "asset":"asset 2",
                                        "mac":"mac 2",
                                        "ds":[]
                                    }
                                ]
                        }
                }
 */
bool serializeDeviceTreeJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data )
{
#ifndef PX_GREEN_IOT_DEVICE
	if ( !allocateSerializedBuffer( iotMessage ) )
	{
		return ( false );
	}
#else
	iotMessage->serializedMessage[0] = '\0';
#endif

	strncat( iotMessage->serializedMessage, "{\"", 2 );
	strncat( iotMessage->serializedMessage, API_DEVICES, strlen( API_DEVICES ) );
	strncat( iotMessage->serializedMessage, "\":", 2 );

	LIST_ITEM_HANDLE handleDeviceList = singlylinkedlist_get_head_item( data->devices );
	if ( handleDeviceList != NULL )
	{
		const IOT_DATA_DEVICE_ITEM *dataDevice = ( const IOT_DATA_DEVICE_ITEM * )singlylinkedlist_item_get_value( handleDeviceList );
		if ( dataDevice != NULL )
		{
			serializeDeviceJson( iotMessage, dataDevice, true );
		}
	}

	strncat( iotMessage->serializedMessage, "}", 1 );

	return ( true );
}

/*
   Serialize a single device.
 */
bool serializeDeviceJson( IOT_MESSAGE *iotMessage, const IOT_DATA_DEVICE_ITEM *dataDevice, bool firstDevice )
{
#ifndef PX_GREEN_IOT_DEVICE
	int dataEntitySize = 70;																	// constants
	if ( dataDevice->deviceUUID != NULL )
	{
		dataEntitySize += strlen( dataDevice->deviceUUID ) + 1;									// device UUID
	}
	if ( dataDevice->profile != NULL )
	{
		dataEntitySize += strlen( dataDevice->profile ) + 1;									// profile
	}
	if ( dataDevice->name != NULL )
	{
		dataEntitySize += strlen( dataDevice->name ) + 1;										// name
	}
	if ( dataDevice->serial != NULL )
	{
		dataEntitySize += strlen( dataDevice->serial ) + 1;										// serial
	}
	if ( dataDevice->assetTag != NULL )
	{
		dataEntitySize += strlen( dataDevice->assetTag ) + 1;									// asset
	}
	if ( dataDevice->mac != NULL )
	{
		dataEntitySize += strlen( dataDevice->mac ) + 1;										// mac
	}
	if ( !checkSerializedBufferSize( iotMessage, dataEntitySize ) )
	{
		return ( false );
	}
#endif

	if ( !firstDevice )
	{
		strncat( iotMessage->serializedMessage, ",", 1 );
	}

	strncat( iotMessage->serializedMessage, "{", 1 );

	if ( ( dataDevice->deviceUUID != NULL ) && ( strlen( dataDevice->deviceUUID ) == 36 ) )
	{
		addStringField( iotMessage->serializedMessage, "d", dataDevice->deviceUUID, true );
	}
	else
	{
		if ( dataDevice->deviceUUID == NULL )
		{
			Log_Error( "UUID not defined for device in device tree messages." );
		}
		else
		{
			Log_Error( "Invalid UUID format for device in device tree messages: %s", dataDevice->deviceUUID );
		}
	}

	if ( ( dataDevice->profile != NULL ) && ( strlen( dataDevice->profile ) == 36 ) )
	{
		addStringField( iotMessage->serializedMessage, "profile", dataDevice->profile, true );
	}
	else
	{
		if ( dataDevice->profile == NULL )
		{
			Log_Error( "Profile UUID not defined for device in device tree messages." );
		}
		else
		{
			Log_Error( "Invalid UUID format for device profile in device tree messages: %s", dataDevice->profile );
		}
	}
	if ( dataDevice->name != NULL )
	{
		addStringField( iotMessage->serializedMessage, "name", dataDevice->name, true );
	}
	if ( dataDevice->serial != NULL )
	{
		addStringField( iotMessage->serializedMessage, "serial", dataDevice->serial, true );
	}
	if ( dataDevice->assetTag != NULL )
	{
		addStringField( iotMessage->serializedMessage, "asset", dataDevice->assetTag, true );
	}
	if ( dataDevice->mac != NULL )
	{
		addStringField( iotMessage->serializedMessage, "mac", dataDevice->mac, true );
	}

	strncat( iotMessage->serializedMessage, "\"ds\":[", 6 );

	if ( dataDevice->subDevices != NULL )
	{
		bool first = true;
		LIST_ITEM_HANDLE handleDeviceList = singlylinkedlist_get_head_item( dataDevice->subDevices );
		while ( handleDeviceList != NULL )
		{
			const IOT_DATA_DEVICE_ITEM *dataSubDevice = ( const IOT_DATA_DEVICE_ITEM * )singlylinkedlist_item_get_value( handleDeviceList );
			if ( dataSubDevice != NULL )
			{
				serializeDeviceJson( iotMessage, dataSubDevice, first );
				first = false;
			}
			handleDeviceList = singlylinkedlist_get_next_item( handleDeviceList );
		}
	}
	strncat( iotMessage->serializedMessage, "]", 1 );

	strncat( iotMessage->serializedMessage, "}", 1 );

	return ( true );
}

/*
   Serialize device realtime statuses for a list of devices

   Sample Json:
                {
                    "devicerealtimes":
                                        [
                                            {
                                                "d":"91651651-9BF7-490D-A332-95590AD4A541",
                                                "t":0,
                                                "t_ms":0,
                                                "disconnected":false,
                                                "disabled":false,
                                                "disarmed":false
                                          },
                                          {
                                                "d":"91651651-9BF7-490D-A332-222222222222",
                                                "t":0,
                                                "t_ms":0,
                                                "disconnected":false,
                                                "disabled":false,
                                                "disarmed":false
                                            },
                                            {
                                                "d":"91651651-9BF7-490D-A332-333333333333",
                                                "t":0,
                                               "t_ms":0,
                                                "disconnected":false,
                                                "disabled":false,
                                                "disarmed":false
                                            }
                                        ]
                }
 */
bool serializeDevicesRealtimeJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data )
{
#ifndef PX_GREEN_IOT_DEVICE
	if ( !allocateSerializedBuffer( iotMessage ) )
	{
		return ( false );
	}
#else
	iotMessage->serializedMessage[0] = '\0';
#endif

	strncat( iotMessage->serializedMessage, "{\"", 2 );
	strncat( iotMessage->serializedMessage, API_DEVICE_REALTIMES, strlen( API_DEVICE_REALTIMES ) );
	strncat( iotMessage->serializedMessage, "\":[", 3 );

	LIST_ITEM_HANDLE handleList = singlylinkedlist_get_head_item( data->devicesRealtime );
	if ( handleList != NULL )
	{
		bool first = true;
		while ( handleList != NULL )
		{
			const IOT_DATA_DEVICES_REALTIME_ITEM *dataItem = ( const IOT_DATA_DEVICES_REALTIME_ITEM * )singlylinkedlist_item_get_value( handleList );
			if ( dataItem != NULL )
			{
				serializeDeviceRealtimeJson( iotMessage, dataItem, first );
				first = false;
			}
			handleList = singlylinkedlist_get_next_item( handleList );
		}
	}

	strncat( iotMessage->serializedMessage, "]}", 2 );

	return ( true );
}

/*
   Serialize a single device realtime status.
 */
bool serializeDeviceRealtimeJson( IOT_MESSAGE *iotMessage, const IOT_DATA_DEVICES_REALTIME_ITEM *data, bool firstItem )
{
#ifndef PX_GREEN_IOT_DEVICE
	int dataEntitySize = 75;																	// constants
	if ( data->deviceUUID != NULL )
	{
		dataEntitySize += strlen( data->deviceUUID ) + 1;										// device UUID
	}
	dataEntitySize += 30;																		// numeric values
	dataEntitySize += 15;																		// boolean values
	if ( !checkSerializedBufferSize( iotMessage, dataEntitySize ) )
	{
		return ( false );
	}
#endif
	if ( !firstItem )
	{
		strncat( iotMessage->serializedMessage, ",", 1 );
	}

	strncat( iotMessage->serializedMessage, "{", 1 );

	if ( data->deviceUUID != NULL )
	{
		addStringField( iotMessage->serializedMessage, "d", data->deviceUUID, true );
	}
	addLongNumericField( iotMessage->serializedMessage, "t", &data->time, true );
	if ( data->elapsedms > 0 )
	{
		// Optimize: don't publish t_ms if it is 0
		addIntNumericField( iotMessage->serializedMessage, "t_ms", &data->elapsedms, true );
	}
	if ( data->disconnected == true )
	{
		addBooleanField( iotMessage->serializedMessage, "disconnected", &data->disconnected, true );
	}
	if ( data->disabled == true )
	{
		addBooleanField( iotMessage->serializedMessage, "disabled", &data->disabled, true );
	}
	if ( data->disarmed == true )
	{
		addBooleanField( iotMessage->serializedMessage, "disarmed", &data->disarmed, true );
	}

	iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] = 0;		// remove trailing comma

	strncat( iotMessage->serializedMessage, "}", 1 );

	return ( true );
}

/*
   Serialize channel states and values for a list of channels.

   Sample Json:
                {
                    "realtimes":
                                [
                                    {
                                        "c":"Ia",
                                        "t":0,
                                        "t_ms":0,
                                        "v":"10.0",
                                        "disconnected":false,
                                        "disabled":false,
                                        "disarmed":false
                                    },
                                    {
                                        "c":"Ib",
                                        "t":0,
                                        "t_ms":0,
                                        "v":
                                        "20.0",
                                        "disconnected":false,
                                        "disabled":false,
                                        "disarmed":false
                                    },
                                    {
                                        "c":"Ic",
                                        "t":0,
                                        "t_ms":0,
                                        "v":"30.0",
                                        "disconnected":false,
                                        "disabled":false,
                                        "disarmed":false
                                    }
                                ]
                }
 */
bool serializeChannelRealtimesJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data )
{
#ifndef PX_GREEN_IOT_DEVICE
	if ( !allocateSerializedBuffer( iotMessage ) )
	{
		return ( false );
	}
#else
	iotMessage->serializedMessage[0] = '\0';
#endif

	strncat( iotMessage->serializedMessage, "{\"", 2 );
	strncat( iotMessage->serializedMessage, API_REALTIMES, strlen( API_REALTIMES ) );
	strncat( iotMessage->serializedMessage, "\":[", 3 );

	LIST_ITEM_HANDLE handleList = singlylinkedlist_get_head_item( data->channelRealtimes );
	if ( handleList != NULL )
	{
		bool first = true;
		while ( handleList != NULL )
		{
			const IOT_DATA_CHANNEL_REALTIMES_ITEM *dataItem = ( const IOT_DATA_CHANNEL_REALTIMES_ITEM * )singlylinkedlist_item_get_value( handleList );
			if ( dataItem != NULL )
			{
				serializeChannelRealtimeJson( iotMessage, data->deviceUUID, dataItem, first );
				first = false;
			}
			handleList = singlylinkedlist_get_next_item( handleList );
		}
	}

	strncat( iotMessage->serializedMessage, "]}", 2 );

	return ( true );
}

/*
   Serialize channel state and value for a single channel.
 */
bool serializeChannelRealtimeJson( IOT_MESSAGE *iotMessage, const char *deviceUUID, const IOT_DATA_CHANNEL_REALTIMES_ITEM *data, bool firstItem )
{
	if ( deviceUUID )
	{}
	;							// do nothing, but avoid an unused parameter warning
#ifndef PX_GREEN_IOT_DEVICE
	int dataEntitySize = 80;																	// constants
	if ( data->channelTag != NULL )
	{
		dataEntitySize += strlen( data->channelTag ) + 1;										// channel tag
	}
	if ( data->value != NULL )
	{
		dataEntitySize += strlen( data->value ) + 1;											// channel tag
	}
	dataEntitySize += 30;																		// numeric values
	dataEntitySize += 15;																		// boolean values
	if ( !checkSerializedBufferSize( iotMessage, dataEntitySize ) )
	{
		return ( false );
	}
#endif
	if ( !firstItem )
	{
		strncat( iotMessage->serializedMessage, ",", 1 );
	}

	strncat( iotMessage->serializedMessage, "{", 1 );

	if ( data->channelTag != NULL )
	{
		addStringField( iotMessage->serializedMessage, "c", data->channelTag, true );
	}
	addLongNumericField( iotMessage->serializedMessage, "t", &data->time, true );
	if ( data->milliseconds > 0 )
	{
		// Optimize: don't publish t_ms if it is 0
		addIntNumericField( iotMessage->serializedMessage, "t_ms", &data->milliseconds, true );
	}
	if ( data->value != NULL )
	{
		addStringField( iotMessage->serializedMessage, "v", data->value, true );
	}
	// Don't publish these flags unless they are in the non-normal state (true)
	if ( data->disconnected == true )
	{
		addBooleanField( iotMessage->serializedMessage, "disconnected", &data->disconnected, true );
	}
	if ( data->disabled == true )
	{
		addBooleanField( iotMessage->serializedMessage, "disabled", &data->disabled, true );
	}
	if ( data->disarmed == true )
	{
		addBooleanField( iotMessage->serializedMessage, "disarmed", &data->disarmed, true );
	}

	iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] = 0;		// remove trailing comma

	strncat( iotMessage->serializedMessage, "}", 1 );

	return ( true );
}

/*
   Serialize channel time-series data for a list of channels.

   Sample Json:
                {
                    "trends":
                                [
                                    {
                                        "c":"Ia",
                                        "t":0,
                                        "v":"10.0",
                                        "avg":"",
                                        "min":"",
                                        "max":""
                                    },
                                    {
                                        "c":"Ib",
                                        "t":0,
                                        "v":"20.0",
                                        "avg":"",
                                        "min":"",
                                        "max":""
                                    },
                                    {
                                        "c":"Ic",
                                        "t":0,
                                        "v":"30.0",
                                        "avg":"",
                                        "min":"",
                                        "max":""
                                    }
                                ]
                }
 */
bool serializeTrendsJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data )
{
#ifndef PX_GREEN_IOT_DEVICE
	if ( !allocateSerializedBuffer( iotMessage ) )
	{
		return ( false );
	}
#else
	iotMessage->serializedMessage[0] = '\0';
#endif

	strncat( iotMessage->serializedMessage, "{\"", 2 );
	strncat( iotMessage->serializedMessage, API_TRENDS, strlen( API_TRENDS ) );
	strncat( iotMessage->serializedMessage, "\":[", 3 );

	LIST_ITEM_HANDLE handleList = singlylinkedlist_get_head_item( data->trends );
	if ( handleList != NULL )
	{
		bool first = true;
		while ( handleList != NULL )
		{
			const IOT_DATA_TREND_ITEM *dataItem = ( const IOT_DATA_TREND_ITEM * )singlylinkedlist_item_get_value( handleList );
			if ( dataItem != NULL )
			{
				serializeTrendJson( iotMessage, data->deviceUUID, dataItem, first );
				first = false;
			}
			handleList = singlylinkedlist_get_next_item( handleList );
		}
	}

	strncat( iotMessage->serializedMessage, "]}", 2 );

	return ( true );
}

/*
   Serialize channel time-series data for a single channel.
 */
bool serializeTrendJson( IOT_MESSAGE *iotMessage, const char *deviceUUID, const IOT_DATA_TREND_ITEM *data, bool firstItem )
{
	if ( deviceUUID )
	{}
	;							// do nothing, but avoid an unused parameter warning
#ifndef PX_GREEN_IOT_DEVICE
	int dataEntitySize = 55;																	// constants
	if ( data->channelTag != NULL )
	{
		dataEntitySize += strlen( data->channelTag ) + 1;										// channel tag
	}
	if ( data->actValue != NULL )
	{
		dataEntitySize += strlen( data->actValue ) + 1;											// act value
	}
	if ( data->avgValue != NULL )
	{
		dataEntitySize += strlen( data->avgValue ) + 1;											// avg value
	}
	if ( data->minValue != NULL )
	{
		dataEntitySize += strlen( data->minValue ) + 1;											// min value
	}
	if ( data->maxValue != NULL )
	{
		dataEntitySize += strlen( data->maxValue ) + 1;											// max value
	}
	dataEntitySize += 15;																		// numeric value
	if ( !checkSerializedBufferSize( iotMessage, dataEntitySize ) )
	{
		return ( false );
	}
#endif

	if ( !firstItem )
	{
		strncat( iotMessage->serializedMessage, ",", 1 );
	}

	strncat( iotMessage->serializedMessage, "{", 1 );

	if ( data->channelTag != NULL )
	{
		addStringField( iotMessage->serializedMessage, "c", data->channelTag, true );
	}
	addLongNumericField( iotMessage->serializedMessage, "t", &data->time, true );
	if ( data->actValue != NULL )
	{
		addStringField( iotMessage->serializedMessage, "v", data->actValue, true );
	}
	if ( data->avgValue != NULL )
	{
		addStringField( iotMessage->serializedMessage, "avg", data->avgValue, true );
	}
	if ( data->minValue != NULL )
	{
		addStringField( iotMessage->serializedMessage, "min", data->minValue, true );
	}
	if ( data->maxValue != NULL )
	{
		addStringField( iotMessage->serializedMessage, "max", data->maxValue, true );
	}

	iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] = 0;		// remove trailing comma

	strncat( iotMessage->serializedMessage, "}", 1 );

	return ( true );
}

#ifndef IOT_NO_DEVICE_ALARMS

/*
   Function get estimate size of serialized alarm message
 */
int get_alarm_data_length( const IOT_DATA *data )
{
	int dataEntitySize = 0;

	dataEntitySize += 12;
	LIST_ITEM_HANDLE handleList = singlylinkedlist_get_head_item( data->alarms );
	if ( handleList != NULL )
	{
		while ( handleList != NULL )
		{
			const IOT_DATA_ALARM_ITEM *dataItem = ( const IOT_DATA_ALARM_ITEM * )singlylinkedlist_item_get_value( handleList );
			dataEntitySize += 123;
			dataEntitySize += IOT_MAX_DEVICE_UUID_SIZE;
			if ( dataItem->timestamp != NULL )
			{
				dataEntitySize += strlen( dataItem->timestamp ) + 1;
			}
			if ( dataItem->device_id != NULL )
			{
				dataEntitySize += strlen( dataItem->device_id ) + 1;
			}
			if ( dataItem->source != NULL )
			{
				dataEntitySize += strlen( dataItem->source ) + 1;
			}
			// Max sizes of version and channel_id
			dataEntitySize += 20;
			if ( dataItem->channel_v != NULL )
			{
				dataEntitySize += strlen( dataItem->channel_v ) + 1;
			}
			dataEntitySize += 18;
			dataEntitySize += 10;
			if ( dataItem->note != NULL )
			{
				dataEntitySize += strlen( dataItem->note ) + 1;
			}
			if ( dataItem->user != NULL )
			{
				dataEntitySize += strlen( dataItem->user ) + 1;
			}
			if ( dataItem->trigger != NULL )
			{
				dataEntitySize += 80;
				if ( dataItem->trigger->trigger_id != NULL )
				{
					dataEntitySize += strlen( dataItem->trigger->trigger_id ) + 1;
				}
				dataEntitySize += 30;
				if ( dataItem->trigger->trigger_id != NULL )
				{
					dataEntitySize += strlen( dataItem->trigger->trigger_id ) + 1;
				}
				if ( dataItem->trigger->threshold != NULL )
				{
					dataEntitySize += strlen( dataItem->trigger->threshold ) + 1;
				}
				if ( dataItem->trigger->between_threshold != NULL )
				{
					dataEntitySize += strlen( dataItem->trigger->between_threshold ) + 1;
				}
				if ( dataItem->trigger->custom_message != NULL )
				{
					dataEntitySize += strlen( dataItem->trigger->custom_message ) + 1;
				}
			}
#ifndef PX_GREEN_IOT_DEVICE
			if ( dataItem->custom != NULL )
			{
				dataEntitySize += 80;
				if ( dataItem->custom->PXW_site_id != NULL )
				{
					dataEntitySize += strlen( dataItem->custom->PXW_site_id ) + 1;
				}
				if ( dataItem->custom->PXW_adopter_id != NULL )
				{
					dataEntitySize += strlen( dataItem->custom->PXW_adopter_id ) + 1;
				}
				if ( dataItem->custom->waveform != NULL )
				{
					dataEntitySize += strlen( dataItem->custom->waveform ) + 1;
				}
				if ( dataItem->custom->channel_1 != NULL )
				{
					dataEntitySize += strlen( dataItem->custom->channel_1 ) + 1;
				}
				if ( dataItem->custom->channel_2 != NULL )
				{
					dataEntitySize += strlen( dataItem->custom->channel_2 ) + 1;
				}
			}
#endif
			handleList = singlylinkedlist_get_next_item( handleList );
		}
	}
	dataEntitySize += 3;

	return ( dataEntitySize );
}

/*
   Serialize alarm data for a list of alarms.
 */
bool serializeAlarmsJson( IOT_MESSAGE *iotMessage, const IOT_DATA *data )
{
#ifndef PX_GREEN_IOT_DEVICE
	if ( !allocateSerializedBuffer( iotMessage ) )
	{
		return ( false );
	}
#else
	iotMessage->serializedMessage[0] = '\0';
#endif

#ifndef PX_GREEN_IOT_DEVICE
	// Default serialize buffer size is 1024. Estimate actuall size/length of serialized buffer for alarm message.
	// Resize serialize buffer size if required.
	//Log_Debug( "serializeAlarmsJson(): Alarm Data  size is: %d", get_alarm_data_length( data ));
	if ( !checkSerializedBufferSize( iotMessage, get_alarm_data_length( data ) ) )
	{
		return ( false );
	}
#endif
	strncat( iotMessage->serializedMessage, "{\"", 2 );
	strncat( iotMessage->serializedMessage, API_ALARMS, strlen( API_ALARMS ) );
	strncat( iotMessage->serializedMessage, "\":[", 3 );

	LIST_ITEM_HANDLE handleList = singlylinkedlist_get_head_item( data->alarms );
	if ( handleList != NULL )
	{
		bool first = true;
		while ( handleList != NULL )
		{
			const IOT_DATA_ALARM_ITEM *dataItem = ( const IOT_DATA_ALARM_ITEM * )singlylinkedlist_item_get_value( handleList );
			serializeAlarmJson( iotMessage, data->deviceUUID, dataItem, first );
			first = false;
			handleList = singlylinkedlist_get_next_item( handleList );
		}
	}

	strncat( iotMessage->serializedMessage, "]}", 2 );
	//Log_Debug( "Allocated Buffer size is:%d, Length of serialized Message is: %d", iotMessage->serializedBufferSize, strlen(iotMessage->serializedMessage));
	return ( true );
}

/*
   Serialize data for single alarm.
 */
bool serializeAlarmJson( IOT_MESSAGE *iotMessage, const char *deviceUUID, const IOT_DATA_ALARM_ITEM *data, bool firstItem )
{
	if ( deviceUUID )
	{}
	;							// do nothing, but avoid an unused parameter warning
#ifndef PX_GREEN_IOT_DEVICE
	// TBD
#endif

	if ( !firstItem )
	{
		strncat( iotMessage->serializedMessage, ",", 1 );
	}

	strncat( iotMessage->serializedMessage, "{", 1 );

	// Required parameters
	// Convert numerical alarm id to uuid format
	// Example: Alarm id in UUID format: 00000000-0000-0000-0000-000000000000
	char alarm_uuid[IOT_MAX_DEVICE_UUID_SIZE];
	sprintf( alarm_uuid, "%s%012d", ALARM_ID_PREFIX, data->alarm_id );
	if ( strlen( alarm_uuid ) <= IOT_MAX_DEVICE_UUID_SIZE )
	{
		addStringField( iotMessage->serializedMessage, "alarm_id", alarm_uuid, true );
	}
	if ( data->timestamp != NULL )
	{
		addStringField( iotMessage->serializedMessage, "timestamp", data->timestamp, true );
	}
	if ( ( data->device_id != NULL ) && ( strlen( data->device_id ) <= IOT_MAX_DEVICE_UUID_SIZE ) )
	{
		addStringField( iotMessage->serializedMessage, "device_id", data->device_id, true );
	}
	if ( data->source != NULL )
	{
		addStringField( iotMessage->serializedMessage, "source", data->source, true );
	}
	if ( data->version > 0 )
	{
		addULongNumericField( iotMessage->serializedMessage, "version", &data->version, true );
	}
	if ( data->channel_id > 0 )
	{
		addULongNumericField( iotMessage->serializedMessage, "channel_id", &data->channel_id, true );
	}

	// Optional parameters
	if ( data->channel_v != NULL )
	{
		addStringField( iotMessage->serializedMessage, "channel_v", data->channel_v, true );
	}
	if ( data->latching )
	{
		addBooleanField( iotMessage->serializedMessage, "latching", &data->latching, true );
	}
	if ( data->closed )
	{
		addBooleanField( iotMessage->serializedMessage, "closed", &data->closed, true );
	}
	if ( data->condition_cleared )
	{
		addBooleanField( iotMessage->serializedMessage, "condition_cleared", &data->condition_cleared, true );
	}
	if ( ( data->ack_level > ALM_ACK_UNKNOWN ) && ( data->ack_level < ALM_ACK_MAX ) )
	{
		addUIntNumericField( iotMessage->serializedMessage, "ack_level", &data->ack_level, true );
	}
	if ( ( data->note != NULL ) && ( strlen( data->note ) <= IOT_MAX_ALARM_MESSAGE_SIZE ) )
	{
		addStringField( iotMessage->serializedMessage, "note", data->note, true );
	}
	if ( ( data->user != NULL ) && ( strlen( data->user ) <= IOT_MAX_ALARM_MESSAGE_SIZE ) )
	{
		addStringField( iotMessage->serializedMessage, "user", data->user, true );
	}

	// Optional trigger entity
	if ( data->trigger != NULL )
	{
		addOpenBraceStringField( iotMessage->serializedMessage, "trigger" );
		if ( ( data->trigger->trigger_id != NULL ) )
		{
			addStringField( iotMessage->serializedMessage, "trigger_id", data->trigger->trigger_id, true );
		}
#ifndef PX_GREEN_IOT_DEVICE
		if ( ( data->trigger->severity > TRIG_SEV_UNKNOWN ) && ( data->trigger->severity < TRIG_SEV_MAX ) )
#endif
		{
			addUIntNumericField( iotMessage->serializedMessage, "severity", &data->trigger->severity, true );
		}
		if ( ( data->trigger->trigger_type > TRIG_TYPE_UNKNOWN ) && ( data->trigger->trigger_type < TRIG_TYPE_MAX ) )
		{
			addUIntNumericField( iotMessage->serializedMessage, "trigger_type", &data->trigger->trigger_type, true );
		}
#ifndef PX_GREEN_IOT_DEVICE
		if ( data->trigger->priority == TRIG_PRIO_HIGH )
#endif
		{
			addUIntNumericField( iotMessage->serializedMessage, "priority", &data->trigger->priority, true );
		}
		if ( data->trigger->threshold != NULL )
		{
			addStringField( iotMessage->serializedMessage, "threshold", data->trigger->threshold, true );
		}
		if ( ( data->trigger->trigger_type == TRIG_TYPE_IN_BETWEEN ) && ( data->trigger->between_threshold != NULL ) )
		{
			addStringField( iotMessage->serializedMessage, "between_threshold", data->trigger->between_threshold, true );
		}
		if ( ( data->trigger->custom_message != NULL ) && ( strlen( data->trigger->custom_message ) < IOT_MAX_ALARM_MESSAGE_SIZE ) )
		{
			addStringField( iotMessage->serializedMessage, "custom_message", data->trigger->custom_message, true );
		}
		if ( iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] == ',' )
		{
			iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] = 0;		// remove trailing comma
		}
		strncat( iotMessage->serializedMessage, "},", 2 );
	}

#ifndef PX_GREEN_IOT_DEVICE
	// Optional custom entity
	if ( data->custom != NULL )
	{
		addOpenBraceStringField( iotMessage->serializedMessage, "custom" );
		if ( ( data->custom->PXW_site_id != NULL ) && ( strlen( data->custom->PXW_site_id ) <= IOT_MAX_DEVICE_UUID_SIZE ) )
		{
			addStringField( iotMessage->serializedMessage, "PXW.site_id", data->custom->PXW_site_id, true );
		}
		if ( ( data->custom->PXW_adopter_id != NULL ) && ( strlen( data->custom->PXW_adopter_id ) <= IOT_MAX_DEVICE_UUID_SIZE ) )
		{
			addStringField( iotMessage->serializedMessage, "PXW.adopter_id", data->custom->PXW_adopter_id, true );
		}

		if ( ( data->custom->waveform != NULL ) && ( strlen( data->custom->waveform ) <= IOT_MAX_ALARM_CUSTOM_KEY_SIZE ) )
		{
			addStringField( iotMessage->serializedMessage, "waveform", data->custom->waveform, true );
		}
		if ( ( data->custom->channel_1 != NULL ) && ( strlen( data->custom->channel_1 ) <= IOT_MAX_ALARM_CUSTOM_KEY_SIZE ) )
		{
			addStringField( iotMessage->serializedMessage, "channel_1", data->custom->waveform, true );
		}
		if ( ( data->custom->channel_2 != NULL ) && ( strlen( data->custom->channel_2 ) <= IOT_MAX_ALARM_CUSTOM_KEY_SIZE ) )
		{
			addStringField( iotMessage->serializedMessage, "channel_2", data->custom->channel_2, true );
		}
		if ( iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] == ',' )
		{
			iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] = 0;		// remove trailing comma
		}
		strncat( iotMessage->serializedMessage, "},", 2 );
	}
#endif
	iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] = 0;		// remove trailing comma

	strncat( iotMessage->serializedMessage, "}", 1 );

	return ( true );
}

#endif

/*
   Serialize command in Json format to be transmitted from Device-to-Cloud.
 */
bool serializeCommand( IOT_DEVICE_CONNECTION *connection, IOT_MESSAGE *iotMessage, const IOT_COMMAND *command )
{
	if ( connection == NULL )
	{
		Log_Error( "Missing IOT_DEVICE_CONNECTION" );
		return ( false );
	}

	if ( iotMessage == NULL )
	{
		Log_Error( "Missing IOT_MESSAGE" );
		connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		return ( false );
	}
	if ( command == NULL )
	{
		Log_Error( "Missing IOT_COMMAND" );
		connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		return ( false );
	}
	if ( command->commandType == TRANSFER_IMAGE )
	{
		return ( serializeTransferImageJson( iotMessage, command ) );
	}
	if ( command->commandType == DEVICE_COMMAND_STATUS )
	{
		return ( serializeDeviceCommandStatusJson( iotMessage, command ) );
	}
	else
	{
		Log_Error( "Unknown Command Type" );
		connection->lastErrorCode = IOT_INVALID_ARGUMENT;
		return ( false );
	}
}

/*
   Serialize the TransferImage DLM command.

   @ToDo: if we expand and add more commands, there is no reason that
   serializeTransferImageJson() and serializeDeviceCommandStatusJson()
   couldn't be combined into one function, and extended.

   Sample Json:
                {
                    "transferimage":
                        {
                            "id":"91651651-9BF7-490D-A332-95590AD4A541",
                            "chunk_size":5000,
                            "delay":0
                        }
                }
 */
bool serializeTransferImageJson( IOT_MESSAGE *iotMessage, const IOT_COMMAND *command )
{
#ifndef PX_GREEN_IOT_DEVICE
	if ( !allocateSerializedBuffer( iotMessage ) )
	{
		return ( false );
	}
#else
	iotMessage->serializedMessage[0] = '\0';
#endif

	strncat( iotMessage->serializedMessage, "{\"", 2 );
	strncat( iotMessage->serializedMessage, API_TRANSFER_IMAGE, strlen( API_TRANSFER_IMAGE ) );
	strncat( iotMessage->serializedMessage, "\":{", 3 );

	int commandEntitySize = 45;																	// constants
	if ( command->arg_1 != NULL )
	{
		commandEntitySize += strlen( command->arg_1 ) + 1;										// command id
	}
	if ( command->arg_2 != NULL )
	{
		commandEntitySize += strlen( command->arg_2 ) + 1;										// chunk_size
	}
	if ( command->arg_3 != NULL )
	{
		commandEntitySize += strlen( command->arg_3 ) + 1;										// delay
	}
#ifdef PX_GREEN_IOT_DEVICE
	if ( commandEntitySize > IOT_MAX_OUTBOUND_BUFFER_SIZE )
	{
		return ( false );
	}
#else
	if ( !checkSerializedBufferSize( iotMessage, commandEntitySize ) )
	{
		return ( false );
	}
#endif

	int numericArg = 0;
	if ( command->arg_1 != NULL )
	{
		addStringField( iotMessage->serializedMessage, "id", command->arg_1, true );
	}
	if ( command->arg_2 != NULL )
	{
		numericArg = atoi( command->arg_2 );
		addIntNumericField( iotMessage->serializedMessage, "chunk_size", &numericArg, true );
	}
	if ( command->arg_3 != NULL )
	{
		numericArg = atoi( command->arg_3 );
		addIntNumericField( iotMessage->serializedMessage, "delay", &numericArg, true );
	}

	iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] = 0;		// remove trailing comma

	strncat( iotMessage->serializedMessage, "}}", 2 );

	return ( true );
}

/*
   Serialize the DeviceCommandStatus message (DLM command).

   Sample Json:
                {
                    "devicecommandstatus":
                        {
                            "d":"12345678-9BF7-490D-A332-95590AD4A541",
                            "id":"91651651-9BF7-490D-A332-95590AD4A541",
                            "step":"step #1",
                            "status":"step #1 status",
                            "description":"step #1 notes"
                        }
                }
 */
bool serializeDeviceCommandStatusJson( IOT_MESSAGE *iotMessage, const IOT_COMMAND *command )
{
#ifndef PX_GREEN_IOT_DEVICE
	if ( !allocateSerializedBuffer( iotMessage ) )
	{
		return ( false );
	}
#else
	iotMessage->serializedMessage[0] = '\0';
#endif

	strncat( iotMessage->serializedMessage, "{\"", 2 );
	strncat( iotMessage->serializedMessage, API_DEVICE_COMMAND_STATUS, strlen( API_DEVICE_COMMAND_STATUS ) );
	strncat( iotMessage->serializedMessage, "\":{", 3 );

	int commandEntitySize = 50;																		// constants
	if ( command->deviceUUID != NULL )
	{
		commandEntitySize += strlen( command->deviceUUID ) + 1;										// device UUID
	}
	if ( command->arg_1 != NULL )
	{
		commandEntitySize += strlen( command->arg_1 ) + 1;											// command id
	}
	if ( command->arg_2 != NULL )
	{
		commandEntitySize += strlen( command->arg_2 ) + 1;											// step
	}
	if ( command->arg_3 != NULL )
	{
		commandEntitySize += strlen( command->arg_3 ) + 1;											// status
	}
	if ( command->arg_4 != NULL )
	{
		commandEntitySize += strlen( command->arg_4 ) + 1;											// description
	}
#ifdef PX_GREEN_IOT_DEVICE
	if ( commandEntitySize > IOT_MAX_OUTBOUND_BUFFER_SIZE )
	{
		Log_Error( "Command size would exceed message buffer: %d vs %u", commandEntitySize, IOT_MAX_OUTBOUND_BUFFER_SIZE );
		return ( false );
	}
#else
	if ( !checkSerializedBufferSize( iotMessage, commandEntitySize ) )
	{
		return ( false );
	}
#endif

	if ( command->deviceUUID != NULL )
	{
		addStringField( iotMessage->serializedMessage, "d", command->deviceUUID, true );
	}
	if ( command->arg_1 != NULL )
	{
		addStringField( iotMessage->serializedMessage, "id", command->arg_1, true );
	}
	if ( command->arg_2 != NULL )
	{
		addStringField( iotMessage->serializedMessage, "task", command->arg_2, true );
	}
	if ( command->arg_3 != NULL )
	{
		addStringField( iotMessage->serializedMessage, "status", command->arg_3, true );
	}
	if ( command->arg_4 != NULL )
	{
		addStringField( iotMessage->serializedMessage, "description", command->arg_4, true );
	}

	iotMessage->serializedMessage[strlen( iotMessage->serializedMessage ) - 1] = 0;		// remove trailing comma

	strncat( iotMessage->serializedMessage, "}}", 2 );

	return ( true );
}

/*
   Serialize a device command response (Direct method).
 */
bool serializeDeviceCommandResponse( const char *method, IOT_DEVICE_COMMAND_RESPONSE *commandResponse, unsigned char * *response, size_t *responseSize )
{
	if ( strncmp( method, UPDATE_FIRMWARE_COMMAND, SIZE_UPDATE_FIRMWARE_STR ) == 0 )
	{
		/*
		    {
		        "status":200,
		        "msg":"text"
		    }
		 */

		int size = 25;																						// constants
		size = size + 5;																					// status
		if ( commandResponse->msg != NULL )
		{
			size = size + strlen( commandResponse->msg ) + 1;												// error
		}
		*response = ( unsigned char * )iot_malloc( size );
		if ( *response == NULL )
		{
			return ( false );
		}
		*responseSize = size;
		memset( *response, 0, size );

		strncat( ( char * )*response, "{", 1 );
		addIntNumericField( ( char * )*response, "status", &commandResponse->status, true );
		if ( commandResponse->msg != NULL )
		{
			addStringField( ( char * )*response, "msg", commandResponse->msg, true );
		}
		memset( *response + strlen( ( char * )*response ) - 1, 0, 1 );	// remove trailing comma
		strncat( ( char * )*response, "}", 1 );
	}
	else if ( strncmp( method, IMAGE_TRANSFER_COMMAND, SIZE_IMAGE_TRANSFER_STR ) == 0 )
	{
		/*
		    {
		        "status":200,
		        "msg":"text",
		        "params":
		        {
		          "delay":0
		        }
		    }
		 */

		int size = 70;																						// constants
		size = size + 5;																					// status
		if ( commandResponse->msg != NULL )
		{
			size = size + strlen( commandResponse->msg ) + 1;												// error
		}
		if ( commandResponse->arg_1 != NULL )
		{
			size = size + strlen( commandResponse->arg_1 ) + 1;												// delay
		}
		*response = ( unsigned char * )iot_malloc( size );
		if ( *response == NULL )
		{
			return ( false );
		}
		*responseSize = size;
		memset( *response, 0, size );

		strncat( ( char * )*response, "{", 1 );

		addIntNumericField( ( char * )*response, "status", &commandResponse->status, true );
		if ( commandResponse->msg != NULL )
		{
			addStringField( ( char * )*response, "msg", commandResponse->msg, true );
		}
		if ( commandResponse->arg_1 != NULL )
		{
			strncat( ( char * )*response, "\"params\": {", 11 );
			int numericArg = atoi( commandResponse->arg_1 );
			addIntNumericField( ( char * )*response, "delay", &numericArg, false );
			strncat( ( char * )*response, " },", 3 );
		}
		memset( *response + strlen( ( char * )*response ) - 1, 0, 1 );	// remove trailing comma
		strncat( ( char * )*response, "}", 1 );
	}
	else if ( strncmp( method, QUERY_IMAGES_COMMAND, SIZE_QUERY_IMAGES_STR ) == 0 )
	{
		/*
		    {
		        "status":200,
		        "msg":"text",
		        "params":
		                 {
		                   "images": [ {
		                               "d": [ "<uuid>" ],
		                               "name": "<string>",
		                               "version": "<string>"
		                             } ]
		                 }
		    }
		 * For the simple response, and if the request was for "detailed":true,
		 * the following fields (all optional) can be added to the image:
		 *      "board_name":"Architecture: x86_64  Board: kvm",
		 *      "board_version":"Cpu: QEMU Virtual CPU version 2.0.0  ",
		 *      "component_details":["PX Red Toolkit: 3.1.0","Linux Kernel: 4.10.17",
		 *          "GCC Compiler: x86_64-pcm-linux-gnu-gcc (GCC) 4.9.4","Build Date: 09/11/17 20:57"],
		 *      "platform":{"update_constraints":8,"additional_capabilities":1}
		 */

		if ( commandResponse->list_1 == NULL )
		{
			return ( false );
		}

		int imageCount = 0;
		int imageLength = 0;
		LIST_ITEM_HANDLE imageListItem = singlylinkedlist_get_head_item( commandResponse->list_1 );
		while ( imageListItem != NULL )
		{
			imageCount = imageCount + 1;
			char *image_text = ( char * ) singlylinkedlist_item_get_value( imageListItem );
			if ( image_text != NULL )
			{
				imageLength = imageLength + strlen( image_text ) + 10;
			}
			imageListItem = singlylinkedlist_get_next_item( imageListItem );
		}
		//Log_Info( "Received %d Images, total text length = %d", imageCount, imageLength );

		int size = 85;																						// constants
		size = size + 5;																					// status
		if ( commandResponse->msg != NULL )
		{
			size = size + strlen( commandResponse->msg ) + 1;												// error
		}
		size = size + imageLength;																			// imagges

		*response = ( unsigned char * )iot_malloc( size );
		if ( *response == NULL )
		{
			return ( false );
		}
		*responseSize = size;
		memset( *response, 0, size );

		strncat( ( char * )*response, "{", 1 );

		addIntNumericField( ( char * )*response, "status", &commandResponse->status, true );

		if ( commandResponse->msg != NULL )
		{
			addStringField( ( char * )*response, "msg", commandResponse->msg, true );
		}

		strncat( ( char * )*response, "\"params\":{", 10 );

		strncat( ( char * )*response, "\"images\":[", 10 );
		imageListItem = singlylinkedlist_get_head_item( commandResponse->list_1 );
		while ( imageListItem != NULL )
		{
			char *img_text = ( char * ) singlylinkedlist_item_get_value( imageListItem );
			if ( img_text == NULL )
			{
				continue;		// oops! - no one home here; go to next
			}
			strncat( ( char * )*response, img_text, strlen( img_text ) );
			imageListItem = singlylinkedlist_get_next_item( imageListItem );
			if ( imageListItem != NULL )
			{
				strncat( ( char * )*response, ",", 1 );
			}
		}

		strncat( ( char * )*response, "]}}", 3 );
	}
	else if ( strncmp( method, GET_CHANNEL_VALUES_COMMAND, SIZE_GET_CHANNEL_VALUES_STR ) == 0 )
	{
		/*
		   {
		        "status":200,
		        "msg":"text",
		        "params":
		                 {
		                    "<tag1>": "<value1>",
		                    "<tag2>": "<value2>",
		                    "<tagn>": "<valuen>"
		                 }
		 */

		if ( commandResponse->list_1 == NULL )
		{
			return ( false );
		}

		if ( commandResponse->list_2 == NULL )
		{
			return ( false );
		}

		int tagCount = 0;
		int tagLength = 0;
		LIST_ITEM_HANDLE tagListItem = singlylinkedlist_get_head_item( commandResponse->list_1 );
		while ( tagListItem != NULL )
		{
			tagCount = tagCount + 1;
			char *tag = ( char * ) singlylinkedlist_item_get_value( tagListItem );
			if ( tag != NULL )
			{
				tagLength = tagLength + strlen( tag ) + 3;
			}
			tagListItem = singlylinkedlist_get_next_item( tagListItem );
		}

		int valueCount = 0;
		int valueLength = 0;
		LIST_ITEM_HANDLE valueListItem = singlylinkedlist_get_head_item( commandResponse->list_2 );
		while ( valueListItem != NULL )
		{
			valueCount = valueCount + 1;
			char *value = ( char * ) singlylinkedlist_item_get_value( valueListItem );
			if ( value != NULL )
			{
				valueLength = valueLength + strlen( value ) + 3;
			}
			valueListItem = singlylinkedlist_get_next_item( valueListItem );
		}

		int size = 40;																						// constants
		size = size + 5;																					// status
		if ( commandResponse->msg != NULL )
		{
			size = size + strlen( commandResponse->msg ) + 1;												// msg
		}
		size = size + tagLength;																			// channel tags
		size = size + valueLength;																			// channel values

		*response = ( unsigned char * )iot_malloc( size );
		if ( *response == NULL )
		{
			return ( false );
		}
		*responseSize = size;
		memset( *response, 0, size );

		strncat( ( char * )*response, "{", 1 );

		addIntNumericField( ( char * )*response, "status", &commandResponse->status, true );

		if ( commandResponse->msg != NULL )
		{
			addStringField( ( char * )*response, "msg", commandResponse->msg, true );
		}

		strncat( ( char * )*response, "\"params\":{", 10 );

		if ( tagCount > 0 )
		{
			tagListItem = singlylinkedlist_get_head_item( commandResponse->list_1 );
			valueListItem = singlylinkedlist_get_head_item( commandResponse->list_2 );
			while ( tagListItem != NULL )
			{
				char *tag = ( char * ) singlylinkedlist_item_get_value( tagListItem );
				char *value = ( char * ) singlylinkedlist_item_get_value( valueListItem );
				if ( ( tag == NULL ) || ( value == NULL ) )
				{
					continue;									// oops! - no one home here; go to next
				}
				addStringField( ( char * )*response, tag, value, true );

				tagListItem = singlylinkedlist_get_next_item( tagListItem );
				valueListItem = singlylinkedlist_get_next_item( valueListItem );
			}
			memset( *response + strlen( ( char * )*response ) - 1, 0, 1 );	// remove trailing comma
		}

		strncat( ( char * )*response, "}}", 2 );
	}
#ifdef PX_GREEN_IOT_DEVICE
	else if ( strncmp( method, QUERY_PROCESSOR_IMAGES_COMMAND, SIZE_QUERY_PROCESSOR_IMAGES_STR ) == 0 )
	{
		/*
		   {
		   "status":  <int>,                       // HTTP status (eg, 200)
		   "msg":     "<string>",              // Optional message
		   "params": {
		                "product_id": "<string>",         // The "Product GUID" (identifier) for this product
		                "boot_spec_ver": "<string>",      // The BootSpecVer for this product's main processor, if
		                   available - what is this?
		                "boot_version": "<string>",           // The BootVer for this product's bootloader, if available
		                "serial_num": "<string>",         // The Product's Serial Number, if available
		                "processors": [
		                           {
		                            "index":        <int>,              // Processor index, eg 0
		                            "name":         "<string>",         // The name of this Processor (optional)
		                            "serial_num":   "<string>",         // The Processor's Serial Number, if available
		                            "cpu_version":   "<string>",      // The Processor's version, if available
		                            "hw_version":   "<string>",         // The hardware version for this Processor
		                               (optional)
		                            "cpu_id":       "<string>"          // The "GUID" (identifier) for this processor
		                           }
		                        ],
		                "images": [
		                           {
		                            "path":         "<string>",         // Processor/Image indices, eg "0/1"
		                            "image_type":   "<string>",       // Image type, the key as used in UpdateFirmware
		                            "image_version": "<string>",      // The current image version
		                            "cpu_id":       "<string>",         // The "GUID" (identifier) for this image's
		                               processor
		                            "img_id":       "<string>"          // The "GUID" (identifier) for this image
		                           }
		                        ]
		            }
		   }
		 */

		if ( commandResponse->list_1 == NULL )
		{
			return ( false );
		}

		if ( commandResponse->list_2 == NULL )
		{
			return ( false );
		}

		int tagCount = 0;
		int tagLength = 0;
		LIST_ITEM_HANDLE tagListItem = singlylinkedlist_get_head_item( commandResponse->list_1 );
		while ( tagListItem != NULL )
		{
			tagCount = tagCount + 1;
			char *tag = ( char * ) singlylinkedlist_item_get_value( tagListItem );
			if ( tag != NULL )
			{
				tagLength = tagLength + strlen( tag ) + 3;
			}
			tagListItem = singlylinkedlist_get_next_item( tagListItem );
		}

		int valueCount = 0;
		int valueLength = 0;
		LIST_ITEM_HANDLE valueListItem = singlylinkedlist_get_head_item( commandResponse->list_2 );
		while ( valueListItem != NULL )
		{
			valueCount = valueCount + 1;
			char *value = ( char * ) singlylinkedlist_item_get_value( valueListItem );
			if ( value != NULL )
			{
				valueLength = valueLength + strlen( value ) + 3;
			}
			valueListItem = singlylinkedlist_get_next_item( valueListItem );
		}

		int size = 200;																						// constants
		size = size + 5;																					// status
		if ( commandResponse->msg != NULL )
		{
			size = size + strlen( commandResponse->msg ) + 1;												// error
		}
		if ( commandResponse->arg_1 != NULL )
		{
			size = size + strlen( commandResponse->arg_1 ) + 1;												// product_id
		}
		if ( commandResponse->arg_2 != NULL )
		{
			size = size + strlen( commandResponse->arg_2 ) + 1;												// boot_spec_ver
		}
		if ( commandResponse->arg_3 != NULL )
		{
			size = size + strlen( commandResponse->arg_3 ) + 1;												// boot_version
		}
		if ( commandResponse->arg_4 != NULL )
		{
			size = size + strlen( commandResponse->arg_4 ) + 1;												// serial_num
		}
		size = size + tagLength;																			// tags like
																											// "processors",
																											// "images"
		size = size + valueLength;
		*response = ( unsigned char * )iot_malloc( size );
		if ( *response == NULL )
		{
			return ( false );
		}
		*responseSize = size;
		memset( *response, 0, size );

		strncat( ( char * )*response, "{", 1 );
		addIntNumericField( ( char * )*response, "status", &commandResponse->status, true );
		if ( commandResponse->msg != NULL )
		{
			addStringField( ( char * )*response, "msg", commandResponse->msg, true );
		}
		strncat( ( char * )*response, "\"params\": {", 11 );
		if ( commandResponse->arg_1 != NULL )
		{
			addStringField( ( char * )*response, "product_id", commandResponse->arg_1, true );
		}
		if ( commandResponse->arg_2 != NULL )
		{
			addStringField( ( char * )*response, "boot_spec_ver", commandResponse->arg_2, true );
		}
		if ( commandResponse->arg_3 != NULL )
		{
			addStringField( ( char * )*response, "boot_version", commandResponse->arg_3, true );
		}
		if ( commandResponse->arg_4 != NULL )
		{
			addStringField( ( char * )*response, "serial_num", commandResponse->arg_4, true );
		}
		if ( tagCount > 0 )
		{
			tagListItem = singlylinkedlist_get_head_item( commandResponse->list_1 );
			valueListItem = singlylinkedlist_get_head_item( commandResponse->list_2 );
			while ( tagListItem != NULL )
			{
				char *tag = ( char * ) singlylinkedlist_item_get_value( tagListItem );
				char *value = ( char * ) singlylinkedlist_item_get_value( valueListItem );
				if ( ( tag == NULL ) || ( value == NULL ) )
				{
					continue;									// oops! - no one home here; go to next
				}
				strncat( ( char * )*response, "\"", 1 );
				strncat( ( char * )*response, tag, strlen( tag ) );
				strncat( ( char * )*response, "\":", 2 );
				strncat( ( char * )*response, value, strlen( value ) );
				strncat( ( char * )*response, ",", 1 );
				tagListItem = singlylinkedlist_get_next_item( tagListItem );
				valueListItem = singlylinkedlist_get_next_item( valueListItem );
			}
			memset( *response + strlen( ( char * )*response ) - 1, 0, 1 );	// remove trailing comma
		}
		strncat( ( char * )*response, "}}", 2 );
	}
#endif
	else
	{
		/*
		    {
		        "status":200,
		        "msg":"text"
		    }
		 */

		int size = 25;																						// constants
		size = size + 5;																					// status
		if ( commandResponse->msg != NULL )
		{
			size = size + strlen( commandResponse->msg ) + 1;												// error
		}
		*response = ( unsigned char * )iot_malloc( size );
		if ( *response == NULL )
		{
			return ( false );
		}
		*responseSize = size;
		memset( *response, 0, size );

		strncat( ( char * )*response, "{", 1 );
		addIntNumericField( ( char * )*response, "status", &commandResponse->status, true );
		if ( commandResponse->msg != NULL )
		{
			addStringField( ( char * )*response, "msg", commandResponse->msg, true );
		}
		memset( *response + strlen( ( char * )*response ) - 1, 0, 1 );	// remove trailing comma
		strncat( ( char * )*response, "}", 1 );
	}

	Log_Info( "Device Command Response: %s", *response );

	return ( true );
}

/*
   Append a open brace string field to a Json buffer
 */
void addOpenBraceStringField( char *buffer, const char *name )
{
	if ( ( buffer == NULL ) || ( name == NULL ) )
	{
		return;
	}

	strncat( buffer, "\"", 1 );
	strncat( buffer, name, strlen( name ) );
	strncat( buffer, "\": {", 4 );
}

/*
   Append a string value to a Json buffer
 */
void addStringField( char *buffer, const char *name, const char *value, bool includeTrailingComma )
{
	if ( ( buffer == NULL ) || ( name == NULL ) )
	{
		return;
	}

	strncat( buffer, "\"", 1 );
	strncat( buffer, name, strlen( name ) );
	strncat( buffer, "\":", 2 );
	strncat( buffer, "\"", 1 );
	if ( value != NULL )
	{
		strncat( buffer, value, strlen( value ) );
	}
	strncat( buffer, "\"", 1 );
	if ( includeTrailingComma )
	{
		strncat( buffer, ",", 1 );
	}
}

/*
   Append a numeric value (int) to a Json buffer
 */
void addIntNumericField( char *buffer, const char *name, const int *value, bool includeTrailingComma )
{
	if ( ( buffer == NULL ) || ( name == NULL ) )
	{
		return;
	}

	char numberChar[VALUE_FIELD_LENGTH];
	snprintf( numberChar, VALUE_FIELD_LENGTH, "%d", *value );

	strncat( buffer, "\"", 1 );
	strncat( buffer, name, strlen( name ) );
	strncat( buffer, "\":", 2 );
	strncat( buffer, numberChar, strlen( numberChar ) );
	if ( includeTrailingComma )
	{
		strncat( buffer, ",", 1 );
	}
}

/*
   Append a numeric value (usigned int) to a Json buffer
 */
void addUIntNumericField( char *buffer, const char *name, const unsigned int *value, bool includeTrailingComma )
{
	if ( ( buffer == NULL ) || ( name == NULL ) )
	{
		return;
	}

	char numberChar[VALUE_FIELD_LENGTH];
	snprintf( numberChar, VALUE_FIELD_LENGTH, "%u", *value );

	strncat( buffer, "\"", 1 );
	strncat( buffer, name, strlen( name ) );
	strncat( buffer, "\":", 2 );
	strncat( buffer, numberChar, strlen( numberChar ) );
	if ( includeTrailingComma )
	{
		strncat( buffer, ",", 1 );
	}
}

/*
   Append a numeric value (long) to a Json buffer
 */
void addLongNumericField( char *buffer, const char *name, const long *value, bool includeTrailingComma )
{
	if ( ( buffer == NULL ) || ( name == NULL ) )
	{
		return;
	}

	char numberChar[VALUE_FIELD_LENGTH];
	snprintf( numberChar, VALUE_FIELD_LENGTH, "%ld", *value );

	strncat( buffer, "\"", 1 );
	strncat( buffer, name, strlen( name ) );
	strncat( buffer, "\":", 2 );
	strncat( buffer, numberChar, strlen( numberChar ) );
	if ( includeTrailingComma )
	{
		strncat( buffer, ",", 1 );
	}
}

/*
   Append a numeric value (unsigned long) to a Json buffer
 */
void addULongNumericField( char *buffer, const char *name, const unsigned long *value, bool includeTrailingComma )
{
	if ( ( buffer == NULL ) || ( name == NULL ) )
	{
		return;
	}

	char numberChar[VALUE_FIELD_LENGTH];
	snprintf( numberChar, VALUE_FIELD_LENGTH, "%lu", *value );

	strncat( buffer, "\"", 1 );
	strncat( buffer, name, strlen( name ) );
	strncat( buffer, "\":", 2 );
	strncat( buffer, numberChar, strlen( numberChar ) );
	if ( includeTrailingComma )
	{
		strncat( buffer, ",", 1 );
	}
}

/*
   Append a boolean value to a Json buffer
 */
void addBooleanField( char *buffer, const char *name, const bool *value, bool includeTrailingComma )
{
	if ( ( buffer == NULL ) || ( name == NULL ) )
	{
		return;
	}

	strncat( buffer, "\"", 1 );
	strncat( buffer, name, strlen( name ) );
	strncat( buffer, "\":", 2 );
	if ( *value == true )
	{
		strncat( buffer, "true", 4 );
	}
	else
	{
		strncat( buffer, "false", 5 );
	}
	if ( includeTrailingComma )
	{
		strncat( buffer, ",", 1 );
	}
}

/*
   Deserialize Cloud-to-Device json device command request (Direct Method).

       Sample Json:
                {
                    "id":"00000000-0000-0000-0000-000000000000",
                    "d":["66666666-9BF7-490D-A332-95590AD4A541"],
                    "who":"johndoe@xyz.com",
                    "method":"SetChannelValue",
                    "version":"1",
                    "params":{"rev":"version_1.1","size":"28343452"}
                }
 */
void deserializeCommand( IOT_DEVICE_CONNECTION *connection, const char *msg, int len, IOT_DEVICE_COMMAND *command )
{
	bool isReturn = false;

	if ( ( connection == NULL ) || ( command == NULL ) )
	{
		return;
	}

	jsmn_parser parser;
	unsigned int token_array_size = iot_getJsonTokenCount( msg, len );
	jsmntok_t tokens[TOKEN_ALLOC_SIZE];
	int result;
	int index;

	//Log_Info("Device Command received: %.*s",(int)len,msg);  /* DEBUG */
	jsmn_init( &parser );
	jsmntok_t_init( tokens, TOKEN_ALLOC_SIZE );
	result = jsmn_parse( &parser, msg, len, tokens, TOKEN_ALLOC_SIZE );
	if ( result < 0 )
	{
		Log_Error( "Failed to parse JSON: %d, JSON: %.*s", result, ( int )len, msg );
		return;
	}

	/* Allocate one extra token location, to satisfy valgrind's checking for
	 * use of uninitialized variables. */
	/* jsmntok_t* last_tok = */ jsmn_alloc_token( &parser, tokens, TOKEN_ALLOC_SIZE );
	command->commandMethod = NULL;
	command->commandParams = NULL;

	command->commandDevices = singlylinkedlist_create();

	for ( index = 1; ( index < result ) && !isReturn; index++ )
	{
		size_t token_size = tokens[index + 1].end - tokens[index + 1].start;
		//Log_Info("D2C - Parsing token %d: \"%.*s\"", index, tokens[index].end-tokens[index].start,
		//				msg + tokens[index].start);  /* DEBUG */

		if ( jsoneq( msg, &tokens[index], "d" ) == 0 )
		{
			if ( tokens[index + 1].type != JSMN_ARRAY )
			{
				// single device, check that size is right for 8-4-4-4-12 string
				if ( token_size != IOT_MAX_DEVICE_UUID_SIZE - 1 )
				{
					isReturn = true;		// Not OK
				}
				else
				{
					char *device = ( char * )iot_malloc( sizeof( char ) * ( token_size + 1 ) );
					memset( device, 0, token_size + 1 );
					strncpy( device, msg + tokens[index + 1].start, token_size );
					singlylinkedlist_add( command->commandDevices, device );

					index++;
				}
			}
			else
			{
				// array of devices
				for ( int j = 0; j < tokens[index + 1].size; j++ )
				{
					jsmntok_t *g = &tokens[index + j + 2];
					size_t uuid_size = g->end - g->start;

					if ( uuid_size != IOT_MAX_DEVICE_UUID_SIZE - 1 )
					{
						isReturn = true;		// Not OK
					}
					else
					{
						char *device = ( char * )iot_malloc( sizeof( char ) * ( uuid_size + 1 ) );
						memset( device, 0, uuid_size + 1 );
						strncpy( device, msg + g->start, uuid_size );
						singlylinkedlist_add( command->commandDevices, device );
					}
				}
				index += tokens[index + 1].size + 1;
			}

			//Log_Info("Device Command - parsing tag: \"d\": %s",command->commandDeviceUUID);  /* DEBUG */
		}
		else if ( jsoneq( msg, &tokens[index], "params" ) == 0 )
		{
			/* This copy works because the next "token" is for the opening to closing brace, '{' to '}' */
			int closing_brace_idx = tokens[index + 1].end;
			command->commandParams = ( char * ) iot_malloc( sizeof( char ) * ( token_size + 1 ) );
			memset( command->commandParams, 0, token_size + 1 );
			strncpy( command->commandParams, msg + tokens[index + 1].start, token_size );
			index++;

			// Limit the length of this output, in case it is very long (like 5500 chars)
			//Log_Info("Device Command - parsing tag: \"params\": %.120s",command->commandParams);  /* DEBUG */
			/* Now skip the internal tokens so they don't get processed. */
			while ( ( index < result ) && ( tokens[index + 1].end < closing_brace_idx ) )
			{
				index++;
			}
		}
		else if ( jsoneq( msg, &tokens[index], "id" ) == 0 )
		{
			if ( ( token_size ) <= ( IOT_MAX_DEVICE_UUID_SIZE - 1 ) )
			{
				strncpy( command->commandID, msg + tokens[index + 1].start, ( IOT_MAX_DEVICE_UUID_SIZE - 1 ) );
				command->commandID[IOT_MAX_DEVICE_UUID_SIZE - 1] = '\0';
			}
			else
			{
				isReturn = true;		// Not OK
			}
			index++;

			//Log_Info("Device Command - parsing tag: \"id\": %s",command->commandID);  /* DEBUG */
		}
		else if ( jsoneq( msg, &tokens[index], "who" ) == 0 )
		{
			if ( ( token_size ) <= ( IOT_MAX_DEVICE_UUID_SIZE - 1 ) )
			{
				strncpy( command->commandWho, msg + tokens[index + 1].start, token_size );
				command->commandWho[token_size] = '\0';
			}
			else
			{
				strncpy( command->commandWho, msg + tokens[index + 1].start, ( IOT_MAX_DEVICE_UUID_SIZE - 1 ) );
				command->commandWho[IOT_MAX_DEVICE_UUID_SIZE - 1] = '\0';
			}
			index++;

			//Log_Info("Device Command - parsing tag: \"who\": %s",command->commandWho);  /* DEBUG */
		}
		else if ( jsoneq( msg, &tokens[index], "method" ) == 0 )
		{
			command->commandMethod = ( char * ) iot_malloc( sizeof( char ) * ( token_size + 1 ) );
			memset( command->commandMethod, 0, token_size + 1 );
			strncpy( command->commandMethod, msg + tokens[index + 1].start, token_size );
			index++;

			//Log_Info("Device Command - parsing tag: \"method\": %s",command->commandMethod);  /* DEBUG */
		}
		else if ( jsoneq( msg, &tokens[index], "version" ) == 0 )
		{
			char tmpBuf[15];
			strncpy( tmpBuf, msg + tokens[index + 1].start, token_size );
			command->commandVersion = 1;
			index++;

			//Log_Info("Device Command - parsing tag: \"version\": %s",command->commandVersion);  /* DEBUG */
		}
		else
		{
			Log_Error( "UnExpected JSON tag: %.*s", tokens[index].end - tokens[index].start, msg + tokens[index].start );
		}
	}
}

/*
   Retrieve the number of tokens in a Json string.
 */
unsigned int iot_getJsonTokenCount( const char *json, int len )
{
	if ( json == NULL )
	{
		return ( 0 );
	}

	int count = 0;
	bool array = false;

	for ( int i = 0; i < len; i++ )
	{
		if ( json[i] == '{' )
		{
			count++;
		}
		if ( json[i] == '[' )
		{
			count++;
			array = true;
		}
		if ( json[i] == ']' )
		{
			array = false;
		}
		if ( ( json[i] == ',' ) && ( array == true ) )
		{
			count++;
		}
		if ( json[i] == ':' )
		{
			count = count + 2;
		}
	}

	return ( count );
}

#ifndef PX_GREEN_IOT_DEVICE

/*
   Allocate memory for serialized message.
 */
bool allocateSerializedBuffer( IOT_MESSAGE *iotMessage )
{
	char *buf = iot_malloc( sizeof( char ) * IOT_INITIAL_OUTBOUND_BUFFER_SIZE );

	if ( buf == NULL )
	{
		Log_Error( "Unable to allocate memory (malloc) for serialized message" );
		return ( false );
	}

	iotMessage->serializedMessage = buf;
	iotMessage->serializedBufferSize = sizeof( char ) * IOT_INITIAL_OUTBOUND_BUFFER_SIZE;
	iotMessage->serializedMessage[0] = '\0';

	//Log_Debug( "allocateSerializedBuffer() - Buffer size: %d", iotMessage->serializedBufferSize );

	return ( true );
}

/*
   Determine if serialized buffer needs increased.
 */
bool checkSerializedBufferSize( IOT_MESSAGE *iotMessage, int dataEntitySize )
{
	// * debug **
	//Log_Debug( "CHECK BUFFER SIZE - current size: %d, message size: %zu", iotMessage->serializedBufferSize, strlen(iotMessage->serializedMessage) );
	// ** debug **/
	int rem_buf_sz = ( iotMessage->serializedBufferSize - strlen( iotMessage->serializedMessage ) - dataEntitySize );

	if ( rem_buf_sz <= ( ( int )IOT_LOW_OUTBOUND_BUFFER_SIZE ) )
	{
		int sizeIncrement = ( IOT_INCREMENTAL_OUTBOUND_BUFFER_SIZE ) *sizeof( char );
		char *buf = realloc( iotMessage->serializedMessage, iotMessage->serializedBufferSize + sizeIncrement );
		if ( buf == NULL )
		{
			Log_Error( "Unable to allocate memory (realloc) for serialized message" );
			return ( false );
		}
		if ( ( int ) ( strlen( iotMessage->serializedMessage ) + dataEntitySize ) <= ( iotMessage->serializedBufferSize + ( int )IOT_INCREMENTAL_OUTBOUND_BUFFER_SIZE ) )
		{
			iotMessage->serializedMessage = buf;
			iotMessage->serializedBufferSize = iotMessage->serializedBufferSize + sizeIncrement;
		}
		else
		{
			free( buf );
			buf = NULL;
			Log_Error( "Total Alarms size to be published %zd is greater than the reallocated buffer size %d", ( strlen( iotMessage->serializedMessage ) + dataEntitySize ), iotMessage->serializedBufferSize );
			return ( false );
		}
	}
	return ( true );
}

/*
   Retrieve location and size of a device command parameter.
 */
bool iot_getCommandParameter( const char *params, const char *tag, int *offset, int *length )
{
	if ( ( params == NULL ) || ( tag == NULL ) )
	{
		return ( false );
	}

	*offset = 0;
	*length = 0;

	jsmn_parser parser;
	unsigned int token_array_size = iot_getJsonTokenCount( params, strlen( params ) );
	jsmntok_t tokens[TOKEN_ALLOC_SIZE];
	int result;
	int index;

	jsmn_init( &parser );
	jsmntok_t_init( tokens, TOKEN_ALLOC_SIZE );
	result = jsmn_parse( &parser, params, strlen( params ), tokens, TOKEN_ALLOC_SIZE );
	if ( result < 0 )
	{
		Log_Error( "Failed to parse Device Command parameter: %s", tag );
		return ( false );
	}

	for ( index = 1; index < result; index++ )
	{
		if ( jsoneq( params, &tokens[index], tag ) == 0 )
		{
			*offset = tokens[index + 1].start;
			*length = tokens[index + 1].end - tokens[index + 1].start;

			return ( true );
		}
	}

	Log_Error( "Tag not found: tag: %s", tag );

	return ( false );
}

/*
   Retrieve location and size of a device command parameter by token index.
 */
bool iot_getCommandParameterToken( const char *params, int index, int *offset, int *length )
{
	if ( params == NULL )
	{
		return ( false );
	}

	*offset = 0;
	*length = 0;

	jsmn_parser parser;
	unsigned int token_array_size = iot_getJsonTokenCount( params, strlen( params ) );
	jsmntok_t tokens[TOKEN_ALLOC_SIZE];
	int result;

	jsmn_init( &parser );
	jsmntok_t_init( tokens, TOKEN_ALLOC_SIZE );
	result = jsmn_parse( &parser, params, strlen( params ), tokens, TOKEN_ALLOC_SIZE );
	if ( result < 0 )
	{
		Log_Error( "Failed to parse Device Command parameter: %s", params );
		return ( false );
	}

	if ( index >= result - 1 )
	{
		Log_Error( "Failed to parse Device Command parameter: %s, index: %i", params, index );
		return ( false );
	}

	/* For getting length, verifying index range and start and end value of tokens(index+1). */
	if ( ( ( index + 1 ) < ( int ) TOKEN_ALLOC_SIZE ) && ( tokens[index + 1].start != -1 ) && ( tokens[index + 1].end != -1 ) )
	{
		*offset = tokens[index + 1].start;
		*length = tokens[index + 1].end - tokens[index + 1].start;
		return ( true );
	}
	return ( false );
}

/*
   Retrieve the number of tokens in a device command parameter.
 */
bool iot_getCommandParameterTokenCount( const char *params, int *count )
{
	if ( params == NULL )
	{
		return ( false );
	}

	*count = 0;

	jsmn_parser parser;
	unsigned int token_array_size = iot_getJsonTokenCount( params, strlen( params ) );
	jsmntok_t tokens[TOKEN_ALLOC_SIZE];
	int result;

	jsmn_init( &parser );
	jsmntok_t_init( tokens, TOKEN_ALLOC_SIZE );
	result = jsmn_parse( &parser, params, strlen( params ), tokens, TOKEN_ALLOC_SIZE );
	if ( result < 0 )
	{
		Log_Error( "Failed to parse Device Command parameter: %s", params );
		return ( false );
	}

	*count = result - 1;

	return ( true );
}

#endif
