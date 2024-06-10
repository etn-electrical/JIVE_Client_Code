/*
 **********************************************************************************************
 * @file            Prod_Spec_IOT.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product.
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "IOT_Net.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "IOT_Device_Command.h"
#include "IoT_Config.h"
#include "Prod_Spec_Debug.h"
#include "Babelfish_Assert.h"
#include "Ram.h"

#include <esp_log.h>
#include "Breaker.h"
// Enable the next line to demonstrate publishing of a product's "custom" IoT Message
#define DEMO_CUSTOM_IOT_MESSAGES
extern DRAM_ATTR Device_Info_str DeviceInfo;

#ifdef DEMO_CUSTOM_IOT_MESSAGES
// Declare a local prototype
unsigned long Prod_Custom_IoT_Message_Publisher( uint16_t iot_task_state, IOT_DEVICE_HANDLE iot_device_handle );

size_t state_counts = 0;
#endif

void PROD_SPEC_IOT_Init( void )
{
	IOT_DCI_Config::iot_config_param_t iot_config;

	/*
	 * @brief IOT configuration related DCIDs.
	 * @details These DCIDs are used to configure the IOT connection.
	 * iot_enable: Enable or disable the IOT.
	 * iot_connection_status: Status of the IOT connection.
	 * proxy_enable: It is used to enable the proxy.
	 * proxy_server_address: Address of the proxy server.
	 * proxy_server_port: Port of the proxy server.
	 * proxy_username: User name for the proxy server.
	 * proxy_password: Password for the proxy server.
	 * device_guid: Unique ID of the device.
	 * device_profile: Profile ID of the device family.
	 * iot_connection_string: Connection string for the device ID.
	 * data_update_rate: Rate at which real time data will be updated to the cloud.
	 * cadence_update_rate: Rate at which cadence data will be updated to the cloud.
	 * iot_conn_status_reason: IoT connection status reason.
	 */
	iot_config.iot_enable = new DCI_Owner( DCI_IOT_CONNECT_DCID );
	iot_config.iot_connection_status = new DCI_Owner( DCI_IOT_STATUS_DCID );
	iot_config.proxy_enable = new DCI_Owner( DCI_IOT_PROXY_ENABLE_DCID );
	iot_config.proxy_server_address = new DCI_Owner( DCI_IOT_PROXY_SERVER_DCID );
	iot_config.proxy_server_port = new DCI_Owner( DCI_IOT_PROXY_PORT_DCID );
	iot_config.proxy_username = new DCI_Owner( DCI_IOT_PROXY_USERNAME_DCID );
	iot_config.proxy_password = new DCI_Owner( DCI_IOT_PROXY_PASSWORD_DCID );
	iot_config.device_guid = new DCI_Owner( DCI_IOT_DEVICE_GUID_DCID );
	iot_config.device_profile = new DCI_Owner( DCI_IOT_DEVICE_PROFILE_DCID );
	iot_config.iot_connection_string = new DCI_Owner( DCI_IOT_CONN_STRING_DCID );
	iot_config.data_update_rate = new DCI_Owner( DCI_IOT_UPDATE_RATE_DCID );
	iot_config.cadence_update_rate = new DCI_Owner( DCI_IOT_CADENCE_UPDATE_RATE_DCID );
	iot_config.iot_conn_status_reason = new DCI_Owner( DCI_IOT_CONN_STAT_REASON_DCID );
	iot_config.dps_endpoint = new DCI_Owner( DCI_IOT_DPS_ENDPOINT_DCID );
	iot_config.dps_id_scope = new DCI_Owner( DCI_IOT_DPS_ID_SCOPE_DCID );
	iot_config.dps_reg_id = new DCI_Owner( DCI_IOT_DPS_DEVICE_REG_ID_DCID );
	iot_config.dps_device_sym_key = new DCI_Owner( DCI_IOT_DPS_SYMM_KEY_DCID );

//Ahmed
   iot_config.device_guid->Check_In((DCI_DATA_PASS_TD *)DeviceInfo.DeviceId);
   iot_config.device_guid->Check_In_Init((DCI_DATA_PASS_TD *)DeviceInfo.DeviceId);
   iot_config.iot_connection_string->Check_In((DCI_DATA_PASS_TD *)DeviceInfo.ConnectionString);
   iot_config.iot_connection_string->Check_In_Init((DCI_DATA_PASS_TD *)DeviceInfo.ConnectionString);

   
   
#if 0
//HostName=EatonCRDSWDeviceIothub1.azure-devices.net;DeviceId=28d4e9fe-4cfb-447c-83d4-c234d3b0b755;SharedAccessKey=vBTkva1Z1f6kEBrgX4Sgm+aHE4utqFe9sxbYq4GUiFI=
        memset ((char *)&DeviceInfo.ConnectionString,0, sizeof (DeviceInfo.ConnectionString));
        memcpy ((char *)&DeviceInfo.ConnectionString, 
            "HostName=EatonCRDSWDeviceIothub1.azure-devices.net;DeviceId=28d4e9fe-4cfb-447c-83d4-c234d3b0b755;SharedAccessKey=vBTkva1Z1f6kEBrgX4Sgm+aHE4utqFe9sxbYq4GUiFI=",
            sizeof ("HostName=EatonCRDSWDeviceIothub1.azure-devices.net;DeviceId=28d4e9fe-4cfb-447c-83d4-c234d3b0b755;SharedAccessKey=vBTkva1Z1f6kEBrgX4Sgm+aHE4utqFe9sxbYq4GUiFI="));
        ESP_LOGI("ProductSpec", "ConnectionString: %.*s", sizeof(DeviceInfo.ConnectionString), DeviceInfo.ConnectionString);
        SetConnectStringInfo();
       iot_config.device_guid->Check_In((DCI_DATA_PASS_TD *)DeviceInfo.DeviceId);
       iot_config.iot_connection_string->Check_In((DCI_DATA_PASS_TD *)DeviceInfo.ConnectionString);
#endif

        

	/* Create the owners of the IOT's channel parameters here, but don't bother to
	 * retain the handles to them.
	 * In a real product, these will be created in the process that updates them.
	 * Our IoT code will access them as a Patron. */
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {
		printf("Total Channels %d \n",iot_dci_data_target_info.total_channels);
    }
	for ( uint_fast16_t i = 0; i < iot_dci_data_target_info.total_channels; i++ )
	{
		const iot_channel_config_struct_t* channel_list = iot_dci_data_target_info.channel_list;
		const iot_channel_config_struct_t* channel_config = &channel_list[i];
		// coverity[leaked_storage]
		new DCI_Owner( channel_config->dcid );
		//printf("channel_config->dcid  %d \n",channel_config->dcid);

	}
	// Create the owners for the cadence channels. For now, don't do anything with them.
#ifdef PX_GREEN_IOT_SUPPORT

	//VECTOR_ functions need to be addressed
	
	/* Note: first tried this with the std::vector; it added 252 bytes to codespace.
	 * But then saw in the .map file that VECTOR is available and already used in the Azure code. */
	VECTOR_HANDLE cadence_owners = VECTOR_create( sizeof( DCI_Owner* ) );
	if ( cadence_owners != nullptr )
	{
		for ( uint_fast16_t i = 0; i < iot_dci_data_target_info.total_groups; i++ )
		{
			const iot_cadence_group_struct_t* group_list = iot_dci_data_target_info.group_list;
			const iot_cadence_group_struct_t* group_config = &group_list[i];

			DCI_Owner* the_owner = new DCI_Owner( group_config->cadence_dcid );
			BF_ASSERT( the_owner );

			// Save these owner instances
			if ( VECTOR_push_back( cadence_owners, the_owner, 1 ) != 0 )
			{
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "unable to VECTOR_push_back" );
				break;
			}
		}
	}
	/* @ToDo: Does it give the REST interface fits if channels don't have any owner?
	 * Ie, why not include all the above under the the #ifdef PX_GREEN_IOT_SUPPORT ? */

	new IOT_Net( &iot_dci_data_target_info, &iot_config, cadence_owners );	///< Initializing IOT

#ifdef DEMO_CUSTOM_IOT_MESSAGES
	// As a proof of concept, register a callback that will send a custom message later
	IOT_Net::Register_Custom_IoT_Message_Callback( &Prod_Custom_IoT_Message_Publisher );
#endif

#else
	// To supress "not used" warning
	BF_Lib::Unused<DCI_Owner*>::Okay( iot_config.iot_enable );
	// in case IOT is disable, create owner instance for the cadence DCIDs so that (Automated) Test Harness can execute
	// the DCI tests successfully.
	for ( uint_fast16_t i = 0; i < iot_dci_data_target_info.total_groups; i++ )
	{
		const iot_cadence_group_struct_t* group_list = iot_dci_data_target_info.group_list;
		const iot_cadence_group_struct_t* group_config = &group_list[i];
		// coverity[leaked_storage]
		new DCI_Owner( group_config->cadence_dcid );
	}
#endif
	// coverity[leaked_storage]
}

#ifdef DEMO_CUSTOM_IOT_MESSAGES

#define SAMPLES_COUNT 736
/* Template for the custom message that we will send here
 * Provide your own message and field names, that agree with the Azure-based code that will receive your message.
 * Example using this template:
 * {"wvd":{"t":1607370643,"ch":41,"sd":"AQACAAMABAAFAAYABwAIAAkACgALAAwADQAOAA8AEAA..."}}
 */
#define CUSTOM_IOT_MSG_TEMPLATE "{\"wvd\":{\"t\":%lu,\"ch\":%u,\"sd\":\"%s\"}}"

/**
 * Example Product implementation for the Callback function used to send Custom IoT Messages.
 * This conforms to IOT_Net's Custom_IoT_Message_Callback() type.
 *
 * When the Task State is SEND_UPDATED_CHANNELS for the 20th time (spaced so it is not on top of the
 * initial burst of regular messages), we will send one example of a custom message,
 * similar in concept to the LV Cloud Gateway's Waveform Channel record of 1.44KB or 736 uint16_t values.
 * Then we will unregister ourself.
 *
 * @param iot_task_state The present state of the IOT_Net Timer Task. One of iot_net_states_t enums.
 * @param iot_device_handle The handle to the IoT connection, needed to send a message.
 * @return The Message ID if we sent a custom message, else 0 if not.
 */
unsigned long Prod_Custom_IoT_Message_Publisher( uint16_t iot_task_state, IOT_DEVICE_HANDLE iot_device_handle )
{
	unsigned long message_id = 0;

	if ( ( iot_task_state == IOT_Net::SEND_UPDATED_CHANNELS ) && ( ++state_counts == 20 ) )
	{
		// Compose and send the custom message

		// Define the structure of the message data
		// We want an arry of 1.44 KB, or 736 uint16_t values
		struct waveform_data_s
		{
			time_t capture_time;
			uint32_t channel_tag_id;
			union buff_u
			{
				uint16_t samples[SAMPLES_COUNT];
				uint8_t buffer[SAMPLES_COUNT * sizeof( uint16_t )];
			} buff;
		} waveform_data;
		waveform_data.capture_time = get_time( nullptr );		// ie, "now"
		waveform_data.channel_tag_id = 41;						// ie, mACIA, though the choice is not important here
		for ( size_t i = 0; i < SAMPLES_COUNT; i++ )
		{
			waveform_data.buff.samples[i] = ( uint16_t ) i + 1;			// This will roll over to 0; that's OK.
		}

		// Send the message as a JSON message that we will build.
		// Now encode the buffer as a Base64 string
		size_t encoded_length;
		char* buff_as_base64 = iot_base64_encode( waveform_data.buff.buffer, ( SAMPLES_COUNT * sizeof( uint16_t ) ),
												  &encoded_length );
		BF_ASSERT( buff_as_base64 );
		BF_ASSERT( encoded_length > SAMPLES_COUNT * sizeof( uint16_t ) );

		// Estimate the length of the JSON string at size of template + 10 for time + 6 for channel tag id +
		// the length of the Base64 encoded data plus a little extra buffer (4)
		size_t est_length = strlen( CUSTOM_IOT_MSG_TEMPLATE ) + 10 + 6 + encoded_length + 4;
		char* message_buff = ( char* ) iot_malloc( est_length );
		snprintf( message_buff, est_length, CUSTOM_IOT_MSG_TEMPLATE, waveform_data.capture_time,
				  waveform_data.channel_tag_id,
				  buff_as_base64 );
		// OK to free the encoded buffer now
		iot_free( buff_as_base64 );

		// Build the list of properties. Made up Message Type and Routing Path names.
		size_t num_properties = 3;
		const char* properties[6];
		properties[0] = "a";		// Message type
		properties[1] = "Waveform";
		properties[2] = "e";		// Encoding
		properties[3] = "json";
		properties[4] = "r";		// Routing path
		properties[5] = "LVC_Archive";

		PROD_SPEC_DEBUG_PRINT( DBG_MSG_INFO, "Sending a custom message of %d bytes", strlen( message_buff ) );
		message_id = iot_sendCustomMessage( iot_device_handle, message_buff, properties, num_properties );

		/* Perform cleanup */
		// iot_free( message_buff ); -- this if freed in releaseIotMessage()

		// Now, for the PoC, unregister this callback, though this is not normally where you would do that.
		IOT_Net::Unregister_Custom_IoT_Message_Callback();
	}

	return ( message_id );
}

#endif
