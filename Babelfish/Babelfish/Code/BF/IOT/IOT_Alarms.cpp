/**
 *****************************************************************************************
 * @file
 * @brief This file defines all the functions required to handle alarms over IOT
 * @details See header file for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "IOT_Alarms.h"
#include "IOT_Debug.h"
#include "Babelfish_Assert.h"
#include <stdlib.h>
#include <string.h>
#include <StdLib_MV.h>
#include "INT_ASCII_Conversion.h"
#include "Event_Defines.h"
#include "IoT_Config.h"

#ifdef IOT_ALARM_SUPPORT
IOT_Alarms* IOT_Alarms::m_iot_alarm_handle = nullptr;
const char* IOT_Alarms::ALARM_SOURCE = "Edge";
const char IOT_Alarms::ALARM_ID_PREFIX[] = "00000000-0000-0000-0000-";
const char* IOT_Alarms::ALARM_MESSAGE_TEMPLATE =
	"\"{\"alarms\":[{\
\"alarm_id\": \"00000000-0000-0000-0000-000000000001\",\
\"timestamp\": \"2021-05-20T10:54:16.000Z\",\
\"device_id\": \"1265741f-3958-43fe-80fa-3f250599e0cb\",\
\"version\": 1,\
\"latching\": false,\
\"closed\": false,\
\"ack_level\": ,\
\"note\": \"\",\
\"user\": \"\",\
\"source\": \"\",\
\"condition_cleared\": false,\
\"trigger\": {\
\"trigger_id\": \"\",\
\"severity\": ,\
\"trigger_type\": ,\
\"priority\": ,\
\"threshold\": \"\"\
}\
}]}\"";

// "channel_id\": 2,
// "channel_v\": \"channel value\",

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Alarms::IOT_Alarms( IOT_Event_Interface* iot_event_interface, DCI_Owner** pub_cadence_owners ) :
	m_iot_event_interface( iot_event_interface ),
	m_last_pub_head_index( 0U ),
	m_last_pub_idx_owner( nullptr ),
	m_head_tail_init_done( false ),
	m_cadence_handlers( nullptr ),
	m_pub_cadence_owners( pub_cadence_owners )
{
	m_iot_alarm_handle = this;
	m_last_pub_idx_owner = new DCI_Owner( DCI_IOT_ALARMS_LAST_PUB_INDEX_DCID );
	m_last_pub_idx_owner->Check_Out_Init( reinterpret_cast<uint32_t*>( &m_last_pub_head_index ) );
	// IOT_DEBUG_PRINT( DBG_MSG_INFO, "last_pub_head = %d", m_last_pub_head_index )
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Alarms::~IOT_Alarms( void )
{
	delete m_last_pub_idx_owner;
	VECTOR_destroy( m_cadence_handlers );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t IOT_Alarms::Get_Alarm_Update_Count( void )
{
	uint32_t new_entries_count = 0;

	///< Get head index to calculate the new entries
	uint32_t current_head_index = m_iot_event_interface->Latest_Event_Counts();

	///< Check if new event updates available
	if ( m_last_pub_head_index < current_head_index )
	{
		new_entries_count = current_head_index - m_last_pub_head_index;
	}
	return ( new_entries_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
time_t IOT_Alarms::Fill_Alarms_Message( IOT_DATA_ALARM_ITEM& alarm_item, STRING_HANDLE& threshold_value,
										event_log_t log_data )
{
	/** Fill the alarms message structure with event config and event status update
	 * information. Note that we're not checking event priority here as events manager
	 * handles the priority and generates events based on their priority
	 */

	rule_config_t rule_config = {0};

	alarm_item.alarm_id = log_data.event_id;	///< Sequence number
	m_iot_event_interface->Rule_Configuration( static_cast<rule_id_t>( log_data.rule_id ), &rule_config );
	// alarm_items.channel_id = rule_id;	///< Rule index
	alarm_item.latching = rule_config.is_latched;
	if ( rule_config.is_latched )///< Latching type of alarm
	{
		alarm_item.closed = !( static_cast<bool>( log_data.event_status & Event_Manager::LATCHED_BIT ) );
	}
	else///< Non-latching type of event
	{
		alarm_item.closed = !( static_cast<bool>( log_data.event_status & Event_Manager::ACTIVE_BIT ) );
	}
	alarm_item.condition_cleared = !( static_cast<bool>( log_data.event_status & Event_Manager::ACTIVE_BIT ) );
	/**
	 * Severity levels( Ack level ):
	 *  0 - Event
	 *  1 - Notice
	 *  2 - Cautionary
	 *  3 - Critical
	 */
	BF_ASSERT( rule_config.dcid_data );
	alarm_item.ack_level = ( rule_config.dcid_data->level ) + 1U;	// Support cloud side Ack level
	alarm_item.note = reinterpret_cast<const char*>( log_data.note );
	alarm_item.user = reinterpret_cast<const char*>( log_data.user );
	Fill_Trigger_Info( alarm_item.trigger, threshold_value, rule_config );

	return ( log_data.unix_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms::Fill_Trigger_Info( IOT_DATA_ALARM_TRIGGER_ITEM*& trigger_item, STRING_HANDLE& threshold_value,
									rule_config_t rule_config )
{
	///< Trigger info
	trigger_item->trigger_id = reinterpret_cast<const char*>( rule_config.dcid_data->name );///< Rule name
	trigger_item->severity = rule_config.dcid_data->level;
	trigger_item->trigger_type = rule_config.type;
	trigger_item->priority = rule_config.dcid_data->priority;
	STRING_copy( threshold_value, reinterpret_cast<const char*>( rule_config.threshold1 ) );
	trigger_item->threshold = STRING_c_str( threshold_value );
	// alarm_items.trigger->between_threshold = rule_config.threshold2;///< Unused
	trigger_item->custom_message = reinterpret_cast<const char*>( rule_config.dcid_data->desc );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Alarms* IOT_Alarms::Get_IOT_Alarms_Handle_Static( void )
{
	return ( IOT_Alarms::m_iot_alarm_handle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms::Ack_Alarms( MAP_HANDLE iot_map_handle, const IOT_DEVICE_COMMAND* command,
							 IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	/**
	 * * Power Xpert Swagger command (api/v1/alarms/acknowledge) payload - iot_map handle stores the content of the
	 * 						"params" in the following payload as key-value pair
	   {
	       "who": "<user email id>",
	       "command_instance_id": "< The command instance uuid >",
	       "device_id": "<The target device uuid>",
	       "version": "1",
	       "timeout": 100,
	       "alarm_ack": [
	       {
	           "alarm_id": "00000000-0000-0000-0000-000000000022",
	           "ack_level": 1,
	           "note": "demo_note"
	       }
	       ]
	   }

	 * * Generic Command payload - iot_map_handle stores the contents of the "params" in the following payload
	 * 						as key-value pair
	    {
	      "method": "alarm_ack",
	      "params":
	      {
	        "alarm_id":"00000000-0000-0000-0000-000000000001",
	        "ack_level": 1,
	        "note":"demo note"
	      }
	    }
	 */

	bool error = false;
	user_info_req_t event_req = {nullptr, nullptr, 0U};
	const char* const* keys;
	const char* const* values;
	uint16_t ack_level = 0U;
	size_t mcount = 0;

	MAP_RESULT result = Map_GetInternals( iot_map_handle, &keys, &values, &mcount );

	BF_ASSERT( result == MAP_OK );


	event_req.user = reinterpret_cast<uint8_t*>( const_cast<char*>( command->commandWho ) );
	IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Rule user = %s", command->commandWho );
	///< Check whether user has provided "note" in the request
	char* param_val = ( char* )Map_GetValueFromKey( iot_map_handle, "note" );

	if ( param_val != nullptr )
	{
		event_req.note = reinterpret_cast<uint8_t*>( param_val );
		IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Rule note = %s", param_val );
	}

	param_val = ( char* )Map_GetValueFromKey( iot_map_handle, "ack_level" );
	if ( param_val != nullptr )
	{
		char* ack_level_ptr = const_cast<char*>( param_val );
		ack_level = strtoul( ack_level_ptr, NULL, 10 );
		ack_level -= 1U;// Support PX Green ack level starting from 0 index so -1 from the Ack level coming from cloud
		IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "ack_level = %s", param_val );
	}

	param_val = ( char* )Map_GetValueFromKey( iot_map_handle, "alarm_id" );
	if ( param_val != nullptr )
	{
		char* alarm_id = const_cast<char*>( param_val );
		IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "alarm_id = %s", param_val );
		if ( !Extract_Alarm_Id( alarm_id, event_req.event_id ) )
		{
			error = true;
			command_response->msg =
				const_cast<char*>( "Failed to extract one or more alarm id" );

		}
		else
		{
			event_op_status_t op_status = m_iot_event_interface->Ack( event_req, ack_level );
			if ( op_status != EVENT_NO_ERROR )
			{
				error = true;
				Alarm_Error_Response( op_status, command_response );
				IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Failed ack Alarm with error = %d", op_status );
			}
		}
	}
	if ( error )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "%s", command_response->msg );
	}
	else
	{
		command_response->status = HTTP_STATUS_OK;
		command_response->msg = const_cast<char*>( "OK" );
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "Acked alarms successfully" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms::Clear_Alarm_History( IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	event_log_state_t return_status = m_iot_event_interface->Clear_All_Events();

	if ( return_status == LOG_CLEAR )
	{
		command_response->status = HTTP_STATUS_OK;
		command_response->msg = const_cast<char*>( "OK" );
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "Cleared alarm history successfully" );
		m_head_tail_init_done = false;
		m_last_pub_head_index = 0U;
		OS_Tasker::Delay( LOG_ERASE_TIME_MS );	///< Erase time
		Init_Head_Tail_Indices();
	}
	else if ( return_status == CLEAR_INPROGRESS )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg =
			const_cast<char*>( "In progress" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Clearing alarm history in progress" );
	}
	else if ( return_status == CLEAR_ERROR )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg =
			const_cast<char*>( "Failed to clear alarms history" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to clear alarm history" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms::Get_Event_Data( log_events_req_t log_events_req, log_events_resp_t log_events_resp )
{
	m_iot_event_interface->Get_Event_Data( log_events_req, log_events_resp );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms::Close_Alarms( MAP_HANDLE iot_map_handle, const IOT_DEVICE_COMMAND* command,
							   IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	/*** Command payload - iot_map_handle stores the contents of the "params" in the following payload
	 * 						as key-value pair
	    {
	      "method": "CloseAlarms",
	      "params":
	      {
	        "0":"00000000-0000-0000-0000-000000000001",
	        "1":"00000000-0000-0000-0000-000000000002",
	        "2":"00000000-0000-0000-0000-000000000003",
	        "3":"00000000-0000-0000-0000-000000000004",
	        "note":"demo note",
	      }
	    }

	 *** Command payload for close all alarms
	    {
	      "method": "CloseAlarms",
	      "params":
	      {
	        "close_all":"true",
	        "note":"demo note",
	      }
	    }
	 */

	bool error = false;
	const char* const* keys;
	const char* const* values;
	size_t mcount = 0, other_param_cnt = 0;
	bool close_all = false;
	uint16_t alarm_idx = 0;
	user_info_req_t req_info = {nullptr, nullptr, 0U};

	MAP_RESULT result = Map_GetInternals( iot_map_handle, &keys, &values, &mcount );

	BF_ASSERT( result == MAP_OK );
	uint32_t* alarm_id = new uint32_t[mcount];

	BF_ASSERT( alarm_id );
	/** Cloud sends "who" for every C2D command which represents logged in user or service account.
	 * We set it as user
	 */
	req_info.user = reinterpret_cast<uint8_t*>( const_cast<char*>( command->commandWho ) );
	IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Rule user = %s", command->commandWho );
	char* param_val = ( char* )Map_GetValueFromKey( iot_map_handle, "note" );

	///< Check whether user has provided "note" in the request(optional)
	if ( param_val != nullptr )
	{
		other_param_cnt++;
		req_info.note = reinterpret_cast<uint8_t*>( param_val );
		IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Rule note = %s", param_val );
	}
	param_val = ( char* )Map_GetValueFromKey( iot_map_handle, "close_all" );
	if ( param_val != nullptr )
	{
		other_param_cnt++;
		if ( strncmp( param_val, "true", 5 ) == 0 )
		{
			close_all = true;
		}
		// IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "close_all = %s", param_val );
	}

	if ( close_all )
	{
		error = !Close_All_Alarms( req_info, command_response );
	}
	else
	{
		///< Validate alarm id and populate alarm_id[alarm_idx]
		for ( uint16_t i = 0; ( ( i < mcount - other_param_cnt ) && ( !error ) ); i++ )
		{
			char* param_key = const_cast<char*>( keys[i] );
			char* param_value = const_cast<char*>( values[i] );
			if ( Validate_Alarm_Sr_No( param_key, param_value, command_response ) )
			{
				if ( !Extract_Alarm_Id( param_value, alarm_id[alarm_idx] ) )
				{
					error = true;
					command_response->msg =
						const_cast<char*>( "Failed to extract one or more alarm id" );
				}
				else
				{
					IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Parsed alarm id = %d", alarm_id[alarm_idx] );
					alarm_idx++;
				}
			}
		}
		if ( ( alarm_idx == 0 ) && ( !error ) )
		{
			error = true;
			command_response->msg =
				const_cast<char*>( "Alarm ids missing in the payload" );
			IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Alarm ids missing in the payload" );
		}
		if ( !error )
		{
			error = !Close_Given_Alarms( alarm_id, alarm_idx, req_info, command_response );
		}
	}

	if ( error )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "%s", command_response->msg );
	}
	else
	{
		command_response->status = HTTP_STATUS_OK;
		if ( close_all )
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Closed all alarms successfully" );
		}
		else
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Closed alarms successfully" );
		}
	}
	delete[] alarm_id;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Alarms::Close_Given_Alarms( uint32_t* alarm_id_arr, size_t arr_size, user_info_req_t event_req,
									 IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	bool return_status = false;


	for ( size_t i = 0; i < arr_size; i++ )
	{
		event_req.event_id = alarm_id_arr[i];
		event_op_status_t close_status = m_iot_event_interface->Reset( event_req );
		if ( close_status == EVENT_NO_ERROR )
		{
			command_response->msg = const_cast<char*>( "OK" );
			return_status = true;
		}
		else
		{
			Alarm_Error_Response( close_status, command_response );
			// IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Failed Alarm id = %d", alarm_id_arr[i] );
			break;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms::Alarm_Error_Response( event_op_status_t status,
									   IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	switch ( status )
	{
		case EVENT_ACK_ERROR:
			command_response->msg =
				const_cast<char*>( "Error while acknowledging an alarm" );
			break;

		case EVENT_RESET_ERROR:
			command_response->msg =
				const_cast<char*>( "Error while closing an alarm" );
			break;

		case NO_EVENT_TO_RESET:
			command_response->msg =
				const_cast<char*>( "No alarms to close" );
			break;

		case EVENT_ID_NOT_FOUND:
			command_response->msg =
				const_cast<char*>( "Alarm id not found" );
			break;

		case EVENT_ALREADY_ACK:
			command_response->msg =
				const_cast<char*>( "Alarm already acked" );
			break;

		case EVENT_ALREADY_RESET:
			command_response->msg =
				const_cast<char*>( "Alarm already closed" );
			break;

		case EVENT_RULE_ID_NOT_FOUND:
			command_response->msg =
				const_cast<char*>( "Alarm rule id not found" );
			break;

		case EVENT_RULE_OUT_OF_RANGE:
			command_response->msg =
				const_cast<char*>( "Alarm rule out of range" );
			break;

		case EVENT_RULE_DISABLE:
			command_response->msg =
				const_cast<char*>( "Alarm rule disabled" );
			break;

		case EVENT_NOT_CLEARED:
			command_response->msg =
				const_cast<char*>( "Alarm latched and need to clear first" );
			break;

		case EVENT_ACK_LEVEL_MISMATCH:
			command_response->msg =
				const_cast<char*>( "Alarm ack level mismatched" );
			break;

		default:
			command_response->msg =
				const_cast<char*>( "Failed to close" );
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Alarms::Close_All_Alarms( user_info_req_t event_req, IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	bool return_status = false;

	event_req.event_id = 1;
	event_op_status_t close_all_status = m_iot_event_interface->Reset( event_req, true );

	if ( close_all_status == EVENT_NO_ERROR )
	{
		command_response->msg = const_cast<char*>( "OK" );
		return_status = true;
	}
	else
	{
		Alarm_Error_Response( close_all_status, command_response );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to close all alarms" );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms::Init_Head_Tail_Indices( void )
{
	if ( !m_head_tail_init_done )
	{
		if ( m_last_pub_head_index == 0U )
		{
			///< Indicates factory reset or alarm history cleared
			m_last_pub_head_index = m_iot_event_interface->Latest_Event_Counts();
			m_last_pub_idx_owner->Check_In_Init( reinterpret_cast<uint32_t*>( &m_last_pub_head_index ) );
		}
		else
		{
			m_last_pub_idx_owner->Check_Out_Init( reinterpret_cast<uint32_t*>( &m_last_pub_head_index ) );
		}
		// IOT_DEBUG_PRINT( DBG_MSG_INFO, "Last pub Head = %d", m_last_pub_head_index );
		m_head_tail_init_done = true;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms::Init_Cadence( void )
{
	uint16_t total_severity_levels = m_iot_event_interface->Get_Severity_Levels();

	IOT_Alarms_Cadence::m_total_severity_levels = total_severity_levels;

	m_cadence_handlers = VECTOR_create( sizeof( IOT_Alarms_Cadence* ) );
	for ( uint16_t i = 0; i < total_severity_levels; i++ )
	{
		IOT_Alarms_Cadence* alarm_cadence = new IOT_Alarms_Cadence( m_pub_cadence_owners[i], i, m_cadence_handlers );
		BF_ASSERT( alarm_cadence );

		// Save these owner instances
		if ( VECTOR_push_back( m_cadence_handlers, m_pub_cadence_owners[i], 1 ) != 0 )
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "unable to VECTOR_push_back" );
			delete alarm_cadence;
			break;
		}
		// coverity[leaked_storage]
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t IOT_Alarms::Get_Max_Rulename_Len( void )
{
	rule_dcid_config_t rule_config = {0U};
	uint8_t name_len_arr[MAX_RULES] = {0};
	uint8_t max_len = 0U;

	for ( uint_fast16_t i = 0U; i < MAX_RULES; i++ )
	{
		m_iot_event_interface->Get_Rule_DCID_Config( static_cast<rule_id_t>( i ), &rule_config );
		name_len_arr[i] = rule_config.name_len;
		if ( name_len_arr[i] > max_len )
		{
			max_len = name_len_arr[i];
		}
	}
	return ( max_len );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Alarms::Extract_Alarm_Id( const char* alarm_id_str, uint32_t& alarm_id )
{
	bool status = false;
	char* alarm_id_pos = const_cast<char*>( alarm_id_str );

	///< String compare just to identify the presence of alarm id in uuid format
	if ( strnlen( alarm_id_str, IOT_MAX_DEVICE_UUID_SIZE ) == ( IOT_MAX_DEVICE_UUID_SIZE - 1 ) )
	{
		///< Discard alarm prefix
		alarm_id_pos = alarm_id_pos + sizeof( ALARM_ID_PREFIX ) - 1;
		alarm_id = strtoul( alarm_id_pos, NULL, 10 );
		status = true;
	}
	else
	{
		///< non-uuid format
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t IOT_Alarms::Get_Max_Trigger_Type( void )
{
	return ( static_cast<trigger_type_t>( MAX_TRIGGER_TYPE ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Alarms::Validate_Alarm_Sr_No( const char* param_key, const char* param_value,
									   IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	bool status = false;

	///< Validate that key(i.e Sr.no or index) is numeric
	if ( INT_ASCII_Conversion::Is_String_Numeric( reinterpret_cast<const uint8_t*>( const_cast<char*>( param_key ) ),
												  STRLEN( UINT8_MAX_STRING ) ) )
	{
		status = true;
	}
	else
	{
		///< Non-numeric may mean that it contains "note" or "user"
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t IOT_Alarms::Get_Max_Priority( void )
{
	rule_dcid_config_t rule_config = {0U};
	uint8_t priority_arr[MAX_RULES] = {0};
	uint8_t max_len = 0U;

	for ( uint_fast16_t i = 0U; i < MAX_RULES; i++ )
	{
		m_iot_event_interface->Get_Rule_DCID_Config( static_cast<rule_id_t>( i ), &rule_config );
		priority_arr[i] = rule_config.priority;
		if ( priority_arr[i] > max_len )
		{
			max_len = priority_arr[i];
		}
	}
	return ( max_len );
}

#endif	// IOT_ALARM_SUPPORT
