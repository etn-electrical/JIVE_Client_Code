/**
 *****************************************************************************************
 * @file IOT_Device_Command.cpp Implementation of the IOT_Device_Command class.
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "IOT_Device_Command.h"
#include "IOT_Net.h"
#include "IOT_Pub_Sub.h"
#include "DCI_Owner.h"
#include "IOT_Group_Handler.h"
#include "IOT_Debug.h"
#include "StdLib_MV.h"
#include "IOT_Fus.h"
#include "Babelfish_Assert.h"
#ifdef IOT_ALARM_SUPPORT
#include "IOT_Alarms.h"
#endif
//Ahmed
#include "Breaker.h"
#include "Led.h"
/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
uint32_t IOT_Device_Command::m_c2d_message_count = 0;
C2D_Poduct_Command_Callback IOT_Device_Command::m_c2d_product_command_callback = nullptr;
fus_job_control_t* IOT_Device_Command::m_job_control = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void C2D_Device_Command_Callback( IOT_DEVICE_HANDLE device_handle, void* owner_context,
								  const IOT_DEVICE_COMMAND* command,
								  IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	if ( ( device_handle != nullptr ) && ( owner_context != nullptr ) )
	{
		IOT_Device_Command* dchandler = new IOT_Device_Command( reinterpret_cast<IOT_Net*>( owner_context ) );
		if ( dchandler != nullptr )
		{
			dchandler->Execute_C2D_Command( device_handle, command, command_response );
			delete dchandler;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Device_Command::IOT_Device_Command( IOT_Net* iot_net )
	: m_iot_net( iot_net )
{
	m_pub_sub = m_iot_net->Get_Pub_Sub_Instance();
	BF_ASSERT( m_pub_sub );
	m_iot_dci = m_pub_sub->Get_Iot_Dci_Interface();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Device_Command::Execute_C2D_Command( IOT_DEVICE_HANDLE device_handle, const IOT_DEVICE_COMMAND* command,
											  IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	m_c2d_message_count++;
	IOT_DEBUG_PRINT( DBG_MSG_INFO, "Cloud To Device Message Confirmation Count = %d \n",
					 m_c2d_message_count );

	/* When the SDK code is done sending the command_response, it will free the memory allocated
	* for the msg unless the msg_size is set to 0.
	* Since we do (mostly) use fixed text messages in our responses, we will set msg_size to 0 here;
	* if any command handler wants to malloc a msg, then it must set the msg_size member too. */
	command_response->msg_size = 0;

	/* We are going to carve up command_params_org, inserting nulls as we process its tokens.
	 * But I don't think that's a justification for copying the string before using it;
	 * I think we're safe to carve up the string that we were given by the SDK. */
	char* command_params_org = ( char* ) command->commandParams;

	if ( ( command_params_org == nullptr ) || ( strlen( command_params_org ) < 2 ) )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "No command parameters provided" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "No command parameters provided" );
	}
	else
	{
		// Check if the method received is the QueryProcessorImages request. It doesn't require to unpack params as the
		// params are empty
		/*
		   {
		      "method": "QueryProcessorImages",
		      "params": {}
		   }
		 * */
		if ( !strcmp( command->commandMethod, QUERY_PROCESSOR_IMAGES_COMMAND ) )
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Cloud To Device Command = %s \n", command->commandMethod );

			// Query images - Respond with processor and image info
			IOT_Fus::Query_Processor_Images( command_response );
		}

#ifdef IOT_ALARM_SUPPORT
		else if ( !strcmp( command->commandMethod, CLEAR_ALARM_HISTORY_COMMAND ) )
		{
			///< This command doesn't need to call Unpack_Cmd_Params_And_Store_In_Map() as it doesn't have params
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Cloud To Device Command = %s \n", command->commandMethod );
			IOT_Alarms* iot_alarms_instance = IOT_Alarms::Get_IOT_Alarms_Handle_Static();
			BF_ASSERT( iot_alarms_instance );
			iot_alarms_instance->Clear_Alarm_History( command_response );
		}
#endif
		else
		{
			/* Create map */
			MAP_HANDLE iot_map_handle = Map_Create( nullptr );
			if ( Unpack_Cmd_Params_And_Store_In_Map( iot_map_handle, command, command_params_org ) )
			{
				// Check if the method received is the SetChannelValues request
				if ( !strcmp( command->commandMethod, SET_CHANNEL_VALUES_COMMAND ) )
				{
					IOT_DEBUG_PRINT( DBG_MSG_INFO, "Cloud To Device Command = %s \n", command->commandMethod );

					Set_Params( iot_map_handle, command_response );
				}
				// Check if the method received is the GetChannelValues request
				else if ( !strcmp( command->commandMethod, GET_CHANNEL_VALUES_COMMAND ) )
				{
					IOT_DEBUG_PRINT( DBG_MSG_INFO, "Cloud To Device Command = %s \n", command->commandMethod );

					Get_Params( iot_map_handle, command_response );
				}
				else if ( IOT_Fus::Command_Handler( command, iot_map_handle, reinterpret_cast<void*>( this ),
													command_response ) )
				{
					IOT_DEBUG_PRINT( DBG_MSG_INFO, "Cloud To Device Command = %s \n", command->commandMethod );
				}
#ifdef IOT_ALARM_SUPPORT
				else if ( !strcmp( command->commandMethod, ACK_ALARMS_COMMAND ) )
				{
					IOT_DEBUG_PRINT( DBG_MSG_INFO, "Cloud To Device Command = %s \n", command->commandMethod );
					IOT_Alarms* iot_alarms_instance = IOT_Alarms::Get_IOT_Alarms_Handle_Static();
					BF_ASSERT( iot_alarms_instance );
					iot_alarms_instance->Ack_Alarms( iot_map_handle, command, command_response );
				}
				else if ( !strcmp( command->commandMethod, CLOSE_ALARMS_COMMAND ) )
				{
					IOT_DEBUG_PRINT( DBG_MSG_INFO, "Cloud To Device Command = %s \n", command->commandMethod );
					IOT_Alarms* iot_alarms_instance = IOT_Alarms::Get_IOT_Alarms_Handle_Static();
					BF_ASSERT( iot_alarms_instance );
					iot_alarms_instance->Close_Alarms( iot_map_handle, command, command_response );
				}
#endif
				// Optional product callback
				else if ( m_c2d_product_command_callback != nullptr )
				{
					// Set the default response if the product doesn't handle this command
					command_response->status = HTTP_STATUS_BAD_REQUEST;
					command_response->msg = const_cast<char*>( "This C2D command is not supported" );
					m_c2d_product_command_callback( command->commandMethod, iot_map_handle, command_response );
				}
				else
				{
					command_response->status = HTTP_STATUS_BAD_REQUEST;
					command_response->msg = const_cast<char*>( "This C2D command is not supported" );
					IOT_DEBUG_PRINT( DBG_MSG_ERROR, "C2D command %s is not supported", command->commandMethod );
				}

			}
			else
			{
				command_response->status = HTTP_STATUS_BAD_REQUEST;
				command_response->msg = const_cast<char*>( "Failed to unpack Cloud-to-device command" );
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to unpack Cloud-to-device command" );
			}
			/* Delete map */
			Map_Destroy( iot_map_handle );
		}
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Device_Command::Set_Params( MAP_HANDLE iot_map_handle, IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	uint16_t valueLen = 0U;
	uint16_t index_from_tag = 0U;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	const char* const* keys = nullptr;
	const char* const* values = nullptr;
	char* tag_str, * value_str;
	size_t mcount = 0;

	MAP_RESULT result = Map_GetInternals( iot_map_handle, &keys, &values, &mcount );

	/* Check if the tag is present in device database */
	if ( ( Check_Tag_Validity( iot_map_handle, &mcount ) ) && ( result == MAP_OK ) )
	{
		// Assume success
		command_response->status = HTTP_STATUS_OK;
		command_response->msg = const_cast<char*>( "OK" );
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "SetChannelValues for %zu channels", mcount );

		for ( uint16_t i = 0; ( i < mcount ) && ( dci_error == DCI_ERR_NO_ERROR ); i++ )
		{
			tag_str = const_cast<char*>( keys[i] );
			value_str = const_cast<char*>( values[i] );
			index_from_tag = m_iot_dci->Find_Index_From_Tag_Str( tag_str, strlen( tag_str ) );
			valueLen = strlen( value_str );
			dci_error = m_iot_dci->Set_Tag_Value( index_from_tag,
												  reinterpret_cast<uint8_t*>( value_str ),
												  &valueLen );
			if ( dci_error != DCI_ERR_NO_ERROR )
			{
				command_response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
				command_response->msg =
					const_cast<char*>( "One or more tags failed to set the values due to DCI error" );
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "One or more tags failed to set the values due to DCI error" );
			}
		}
	}
	else
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "One or more tags not found in device database" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "One or more tags not found in device database" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Device_Command::Get_Params( MAP_HANDLE iot_map_handle, IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	char* tag_data = nullptr;
	char* value_data = nullptr;
	// Extra bytes are added for string conversion of double precision values.
	const uint16_t EXTRA_BYTES_FOR_DFLOAT = 4U;
	uint8_t valueBuff[IOT_DCI_DATA_MAX_VAL_LENGTH + EXTRA_BYTES_FOR_DFLOAT] = {0U};
	uint16_t valueLen = 0U, tagLen = 0U;
	uint16_t index_from_tag = 0U;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	const char* const* keys = nullptr;
	const char* const* values = nullptr;
	char* tag_str = nullptr;
	size_t mcount = 0;

	MAP_RESULT result = Map_GetInternals( iot_map_handle, &keys, &values, &mcount );

	/* Check if the tag is present in device database */
	if ( ( Check_Tag_Validity( iot_map_handle, &mcount ) ) && ( result == MAP_OK ) )
	{
		// Assume success
		command_response->status = HTTP_STATUS_OK;
		command_response->msg = const_cast<char*>( "OK" );
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "GetChannelValues for %zu channels", mcount );

		for ( uint16_t i = 0; ( i < mcount ) && ( dci_error == DCI_ERR_NO_ERROR ); i++ )
		{
			tag_str = const_cast<char*>( keys[i] );
			tagLen = strlen( tag_str );

			// We will now allocate the length+1 bytes to tag_data to store it as a null terminated
			// string
			tag_data = ( char* ) Ram::Allocate_Basic( ( tagLen + 1 ) * sizeof( char ) );
			/************** Coverity is rightly complaining that tag_data isn't checked for nullptr.
			 * Adding check for nullptr. ************************/

			if ( tag_data )
			{
				strncpy( tag_data, tag_str, tagLen );
				tag_data[tagLen] = '\0';
			}
			else
			{
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to allocate ram for tag" );
			}

			// Now fetch the value from DCIDs
			index_from_tag = m_iot_dci->Find_Index_From_Tag_Str( tag_str, tagLen );
			dci_error = m_iot_dci->Get_Tag_Value( index_from_tag,
												  reinterpret_cast<uint8_t*>( valueBuff ),
												  &valueLen );
			if ( dci_error != DCI_ERR_NO_ERROR )
			{
				command_response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
				command_response->msg =
					const_cast<char*>( "One or more tags failed to get the values due to DCI error" );
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "One or more tags failed to get the values due to DCI error" );
			}
			else
			{
				// We will now allocate the length+1 bytes to value_data to store it as a null
				// terminated string
				value_data = ( char* ) Ram::Allocate_Basic( ( valueLen + 1 ) * sizeof( char ) );
				/************** Coverity is rightly complaining that value_data isn't checked for nullptr.
				 * I don't see a quick workaround, so hoping this whole parsing is replaced with something better
				 * and more robust. ************************/
				if ( value_data != nullptr )
				{
					strncpy( value_data, reinterpret_cast<char*>( valueBuff ), valueLen );
					value_data[valueLen] = '\0';

					// Add the tag and value to the response list
					singlylinkedlist_add( command_response->list_1, tag_data );
					singlylinkedlist_add( command_response->list_2, value_data );
					// Assume success now
					command_response->status = HTTP_STATUS_OK;
					command_response->msg = const_cast<char*>( "OK" );
				}
				else
				{
					command_response->status = HTTP_STATUS_BAD_REQUEST;
					command_response->msg = const_cast<char*>( "Failed to allocate memory for response" );
					IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to allocate memory for response" );
				}
			}
		}
		if ( value_data != nullptr )
		{
			// Set the list_cleanup flags for tag and data
			command_response->list_1_cleanup = true;
			command_response->list_2_cleanup = true;
		}
	}
	else
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "One or more tags not found in device database" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "One or more tags not found in device database" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Device_Command::Unpack_Cmd_Params_And_Store_In_Map( MAP_HANDLE handle, const IOT_DEVICE_COMMAND* command,
															 char* command_params_org )
{
	bool result;
	char* extracted_str = nullptr;

	/* Extract string between braces {} */
	if ( !( Extract_String_Between_Delimiter( extracted_str, command_params_org, '{', '}' ) ) )
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Braces {} not found" );
		result = false;
	}
	else
	{
		/* Check whether param string is a list or key:value pairs  */
		if ( !strcmp( command->commandMethod, GET_CHANNEL_VALUES_COMMAND ) )
		{
			// Param string contains list, for e.g list of tags for GetChannelValues command
			result = Store_Tag_List_In_Map( handle, extracted_str );
		}
		// Add checking for any other list types here
		// For all others, assume key:value pairs
		else
		{
			// Param string contains key-value pairs, for e.g tag-value pairs for SetChannelValues command
			result = Store_Tag_Value_Pair_In_Map( handle, extracted_str );
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Device_Command::Store_Tag_List_In_Map( MAP_HANDLE handle, char* extracted_str )
{
	const char* dummy_val = "0";
	char* extracted_str_dest = nullptr;
	char* start = nullptr;
	char* end = nullptr;

	/* For e.g GetChannelValues list: "tags":"102962,102970,102969" */

	start = strchr( extracted_str, ':' ) + 1;
	bool result = Extract_String_Between_Delimiter( extracted_str_dest, start, '"', '"' );

	if ( result )
	{
		start = extracted_str_dest;
		while ( start )
		{
			end = strchr( start, ',' );
			MAP_RESULT map_result;
			if ( end )
			{
				*end = '\0';// Extracted a tag
				/* Add channel tag and dummy value as key:value pair in map */
				map_result = Map_Add( handle, start, dummy_val );
				start = end + 1;// Next tag
			}
			else
			{
				/* This is the last tag or there is only one tag */
				map_result = Map_Add( handle, start, dummy_val );
				start = end;
			}
			if ( map_result != MAP_OK )
			{
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to key=%s and value=%s to map", start, dummy_val );
				result = false;
			}
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Device_Command::Store_Tag_Value_Pair_In_Map( MAP_HANDLE handle, char* extracted_str )
{
	char* start = nullptr, * colon_pos = nullptr, * more_pos = nullptr;
	const char* dummy_val = "0";
	char* map_key = nullptr, * map_value = nullptr;
	bool result = true;

	/* For e.g SetChannelValues pairs are "102962":"1","102963":"1","102982":"1","102969":"1"
	 * Tags to be stored as key and dummy value i.e "0" to be stored as value in map */
	start = extracted_str;
	while ( start )
	{
		// printf( "Starting string: %s \n", start );
		// Find the key, in quotes
		if ( !Extract_String_Between_Delimiter( map_key, start, '"', '"' ) )
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to find a key" );
			result = false;
			start = nullptr;
			continue;
		}
		start = start + strlen( map_key ) + 2;
		/* Now we need to parse the next part. We consider the following cases:
		 *  : "some text",
		 *  : XXXX, (some number)
		 *  : [ <stuff> ], (an array of things)
		 *  (no ':' - just a key, needs dummy_val)
		 *  (no ',' - this is the final value)
		 */
		more_pos = strchr( start, ',' );
		colon_pos = strchr( start, ':' );
		// Do we have a value?
		if ( colon_pos && ( ( more_pos > colon_pos ) || ( more_pos == nullptr ) ) )
		{
			char next_char = colon_pos[1];
			if ( next_char == ' ' )		// space char, the normal case
			{
				next_char = colon_pos[2];
			}
			switch ( next_char )
			{
				case '"':
					result = Extract_String_Between_Delimiter( map_value, start, '"', '"' );	// Extract value
					break;

				case '[':
					result = Extract_String_Between_Delimiter( map_value, start, '[', ']' );	// Extract array
					break;

				default:
					// Assume numeric. Might get a leading space. Do equivalent of Extract_String_Between_Delimiter
					map_value = ( colon_pos + 1 );
					if ( map_value[0] == ' ' )
					{
						map_value++;
					}
					if ( more_pos )
					{
						*more_pos = '\0';	/*  Terminate with '\0' */
					}
					break;
			}
		}
		else
		{
			// Provide a dummy_val
			map_value = const_cast<char*>( dummy_val );
		}
		if ( result == true )
		{
			/* Add channel tag and value as key:value pairs in map.
			 * This will copy the map_key and map_value text we provide into new storage. */
			// printf( "Adding k:v %s: %s \n", map_key, map_value );
			MAP_RESULT map_result = Map_Add( handle, map_key, map_value );
			if ( map_result != MAP_OK )
			{
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to add key=%s and value=%s to map", map_key, map_value );
				result = false;
				start = nullptr;
			}
			else
			{
				start = more_pos;
				if ( start )
				{
					start++;	// advance by one, as long as not nullptr
				}
			}
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Device_Command::Extract_String_Between_Delimiter( char*& dest, char* src, char delimiter_start,
														   char delimiter_end )
{
	char* start = strchr( src, delimiter_start );
	char* end = nullptr;

	if ( start )
	{
		end = strchr( start + 1, delimiter_end );
	}

	if ( ( start == nullptr ) || ( end == nullptr ) )	// either start or end is null
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to extract string between delimiters %c and %c", delimiter_start,
						 delimiter_end );
		return ( false );	// One or more delimiter not found
	}
	else
	{
		dest = ( start + 1 );
		*end = '\0';/*  Terminate with '\0' */
	}
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Device_Command::Check_Tag_Validity( MAP_HANDLE handle, size_t* count )
{
	uint16_t index_from_tag_error = 0U;
	bool tagFound = true;
	const char* const* m_values = nullptr;
	const char* const* m_keys = nullptr;
	char* tag_str;
	size_t mcount = 0;
	MAP_RESULT result = Map_GetInternals( handle, &m_keys, &m_values, &mcount );

	if ( result == MAP_OK )
	{
		*count = mcount;// Get count of key:value pairs

		/* Check tag validity */
		for ( size_t i = 0; ( i < mcount ) && ( index_from_tag_error != IOT_DCI::IOT_INPUT_ERROR ); i++ )
		{
			tag_str = const_cast<char*>( m_keys[i] );
			index_from_tag_error = m_iot_dci->Find_Index_From_Tag_Str( tag_str, strlen( tag_str ) );
			if ( index_from_tag_error == IOT_DCI::IOT_INPUT_ERROR )
			{
				tagFound = false;
			}
		}
	}
	else
	{
		tagFound = false;
	}
	return ( tagFound );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Device_Command::Reset()
{
	// Report if canceling a valid job control structure
	if ( m_job_control )
	{
		/* Delete FUS session */
		if ( IOT_Fus::Firmware_Upgrade_Inprogress() )
		{
			const char* final_message = "";
			bool success = false;
			success = IOT_Fus::Exit_Firmware_Session( &final_message );
			if ( success )
			{
				IOT_DEBUG_PRINT( DBG_MSG_CRITICAL, "Deleted FUS session successfully" );
			}
			else
			{
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "%s", final_message );
			}
            StopBlinkingLEDs();
		}
		IOT_DEBUG_PRINT( DBG_MSG_CRITICAL, "Canceling IOT_Device_Command job: %s", m_job_control->job_uuid );
		m_iot_net->Unregister_State_Machine_Callback( &IOT_Device_Command::State_Machine_Handler );
		Ram::De_Allocate_Basic( m_job_control );
		m_job_control = nullptr;
	}
	// Else, nothing to do here
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Device_Command::Send_D2C_Command( IOT_DEVICE_HANDLE device_handle, IOT_COMMAND_TYPE command_type )
{
	bool success = false;
	IOT_COMMAND command_struct;

	memset( &command_struct, 0, sizeof( IOT_COMMAND ) );
	command_struct.commandType = command_type;

	if ( ( device_handle == nullptr ) || ( m_job_control == nullptr ) )
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Cannot proceed to send D2C command" );
	}
	else if ( command_type == TRANSFER_IMAGE )
	{
		success = Send_Transfer_Image_Command( device_handle, &command_struct );
	}
	else if ( command_type == DEVICE_COMMAND_STATUS )
	{
		success = Send_Device_Command_Status( device_handle, &command_struct );
	}
	else
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Unknown D2C command" );
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Device_Command::Send_Transfer_Image_Command( IOT_DEVICE_HANDLE device_handle, IOT_COMMAND* command )
{
	bool success = false;
	int message_size;		// We'll ignore the message size returned to us
	char chunk_size[8];
	char delay[8];

	command->arg_1 = m_job_control->job_uuid;
	snprintf( chunk_size, 8, "%u", m_TRANSFER_IMAGE_CHUNK_SIZE );
	command->arg_2 = chunk_size;
	snprintf( delay, 8, "%u", m_TRANSFER_DELAY_SECS );
	command->arg_3 = delay;

	// Now send the command
	success = iot_sendCommand( device_handle, command, &message_size );

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Device_Command::Send_Device_Command_Status( IOT_DEVICE_HANDLE device_handle, IOT_COMMAND* command )
{
	bool success = false;

	if ( m_job_control )
	{
		int message_size;
		success = iot_updateDeviceCommandStatus( device_handle, m_job_control->job_uuid, m_job_control->device_uuid,
												 m_job_control->task_text, m_job_control->step_status,
												 m_job_control->step_description, &message_size );
		if ( success )
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Sent DeviceCommandStatus for %s - %s", m_job_control->task_text,
							 m_job_control->step_description );
		}
		else
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed sending DeviceCommandStatus for %s - %s", m_job_control->task_text,
							 m_job_control->step_description );
		}
		if ( success && ( m_job_control->step_status == COMMAND_SUCCEEDED ) )
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Done with success; ending Firmware Update job." );
			if ( IOT_Fus::Reboot_Required() )
			{
				/* Schedule system reset after 10seconds */
				m_iot_net->Schedule_Device_Command( 10 * MULTIPLIER_1000MS,
													IOT_Device_Command::SCHEDULED_SYSTEM_RESET_FUS );
			}
			else
			{
				/* If it is any other firmware i.e web or lang that has been upgraded over IoT,
				 * then we need to unregister the callback. For App image, no need to unregister the callback
				 * as we have "System reset" scheduled that will be executed in the state machine */
				m_iot_net->Unregister_State_Machine_Callback( &IOT_Device_Command::State_Machine_Handler );
			}
			Ram::De_Allocate_Basic( m_job_control );
			m_job_control = nullptr;
		}
		else if ( m_job_control->step_status == COMMAND_FAILED )
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed in update; ending Firmware Update job." );
			Reset();
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Device_Command::Update_Device_Command_Status( const char* task_text, IOT_COMMAND_STATUS step_status,
													   const char* step_description )
{
	// Copy the arguments
	if ( m_job_control )
	{
		m_job_control->task_text = task_text;
		m_job_control->step_status = step_status;
		m_job_control->step_description = step_description;
		// Now ask for this DeviceCommandStatus command to be sent
		m_iot_net->Request_Timer_Task_State( IOT_Net::SEND_DEVICE_COMMAND_STATUS );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
time_t IOT_Device_Command::Get_Command_Timeout_Seconds( void )
{
	return ( m_COMMAND_TIMEOUT_SECS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Device_Command::State_Machine_Handler( void )
{
	/* See if we need to schedule a Device Command.
	 * Note m_reset_device_command_time_ms is in milliseconds
	 * */
	if ( m_iot_net->m_reset_device_command_time_ms > 0 )
	{
#ifdef ESP32_SETUP
		uint64_t tnow = get_time( nullptr );
		tnow = tnow * MULTIPLIER_1000MS;
#else
		time_t tnow = get_time( nullptr ) * MULTIPLIER_1000MS;
#endif
		if ( tnow > m_iot_net->m_reset_device_command_time_ms )
		{
			m_iot_net->m_reset_device_command_time_ms = 0;
			IOT_Fus::Scheduled_State_Machine_Handler( reinterpret_cast<void*>( this ) );
		}
	}
	IOT_Fus::D2C_State_Machine_Handler( reinterpret_cast<void*>( this ) );
}
