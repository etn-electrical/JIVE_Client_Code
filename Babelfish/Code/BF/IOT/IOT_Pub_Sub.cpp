/**
 *****************************************************************************************
 * @file IOT_Pub_Sub.cpp Implementation of the IOT_Pub_Sub class.
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram.h"
#include "IOT_Config.h"
#include "IOT_Pub_Sub.h"
#include "DCI_Owner.h"
#include "IOT_Group_Handler.h"
#include "IOT_Debug.h"
#include "Babelfish_Assert.h"
#include "Timestamp_Format.h"
#include "INT_ASCII_Conversion.h"
#include "StdLib_MV.h"
#include <string.h>
#ifdef IOT_ALARM_SUPPORT
#include "IOT_Alarms.h"
#endif

// Provided by the product code:
extern time_t Get_Epoch_Time( void );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Pub_Sub::IOT_Pub_Sub( const iot_target_info_st_t* iot_database, DCI_Patron* patron, DCI_SOURCE_ID_TD source_id,
						  VECTOR_HANDLE cadence_owners, const char* device_uuid ) :
	m_iot_dci_database( iot_database ),
	m_device_handle( nullptr ),
	m_device_uuid( device_uuid ),
	m_iot_dci( new IOT_DCI( iot_database, patron, source_id ) ),
	m_group_handlers( nullptr ),
	m_starting_search_index( 0 ),
	m_trends_done( false )
{
	m_group_handlers = VECTOR_create( sizeof( IOT_Group_Handler* ) );
	if ( m_group_handlers != nullptr )
	{
		for ( uint_fast16_t group_index = 0; group_index < iot_database->total_groups; group_index++ )
		{
			IOT_Group_Handler* the_group = new IOT_Group_Handler( m_iot_dci, group_index, cadence_owners );
			if ( the_group != nullptr )
			{
				// Save these cadence groups
				// Seems wrong to push the pointer to the pointer, and retrieve same, but this works
				if ( VECTOR_push_back( m_group_handlers, &the_group, 1 ) != 0 )
				{
					IOT_DEBUG_PRINT( DBG_MSG_ERROR, "unable to VECTOR_push_back" );
					delete the_group;
					break;
				}
				#ifdef DEBUG_VECTOR_RECALL
				else
				{
					// A little debugging reality check, to prove how this works
					IOT_Group_Handler** test_group =
						reinterpret_cast<IOT_Group_Handler**>( VECTOR_element( m_group_handlers, group_index ) );
					if ( *test_group != the_group )
					{
						IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Mismatch on recall, original = %p, retrieved = %p", the_group,
										 *test_group );
						BF_ASSERT( false );
					}
					else if ( ( *test_group )->Get_Group_Cadence_ms() != the_group->Get_Group_Cadence_ms() )
					{
						BF_ASSERT( false );
					}
				}
				#endif
			}
			else
			{
				// That's a fail, so bail out of here
				break;
			}
			// coverity[leaked_storage]
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Pub_Sub::~IOT_Pub_Sub( void )
{
	VECTOR_destroy( m_group_handlers );
	delete m_iot_dci;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Pub_Sub::Initialize( void )
{
	///< (Re)Initialize class members before starting to publish.
	m_starting_search_index = 0;
	m_trends_done = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Publish_All_Channels( uint16_t group_index )
{
	///< Publish all the channel data for the indicated group.
	bool send_all = true;
	bool is_done = Publish_Channel_Data( group_index, send_all );

	return ( is_done );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Publish_Channel_Data( uint16_t group_index, bool send_all )
{
	///< Publish the StoreMe/trend and/or SeeMe/realtime data for the indicated group.
	bool is_done = true;
	IOT_Group_Handler** the_group = reinterpret_cast<IOT_Group_Handler**>( VECTOR_element( m_group_handlers,
																						   group_index ) );

	if ( ( the_group != nullptr ) && ( *the_group != nullptr ) && ( m_device_handle != nullptr ) )
	{
		// First give the Trends-on-interval a chance
		if ( ( *the_group )->Has_Trends_On_Interval() && !m_trends_done )
		{
			is_done = Publish_StoreMeOnInterval_Trends_Message( *the_group );
			m_trends_done = is_done;
		}
		else
		{
			m_trends_done = true;
		}
		// Then the Realtimes, unless the Trends-on-interval were incomplete
		if ( m_trends_done )
		{
			if ( ( ( *the_group )->Has_Change_Tracker_Attached() ) &&
				 ( send_all || ( *the_group )->Is_Data_Changed() ) )
			{
				if ( ( *the_group )->Has_Realtimes() )
				{
					is_done = Publish_SeeMe_Realtimes_Message( *the_group, send_all );
				}

				///< If all previous messages fit properly, lets check if we have trends
				if ( is_done && ( *the_group )->Has_Trends() )
				{
					is_done = Publish_StoreMe_Trends_Message( *the_group, send_all );
				}

				///< If all the above messages are fit properly and we still have some room then simply ignore and begin
				///< the next cycle
				if ( is_done )
				{
					m_trends_done = false;		// Reset for next group
				}

			}
			else
			{
				m_trends_done = false;
			}
		}
	}
	else
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Bad group information" );
	}
	return ( is_done );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Publish_SeeMe_Realtimes_Message( IOT_Group_Handler* this_group, bool send_all )
{
	///< Publish the SeeMe/realtime data for all, or all changed, channels of the given group.
	bool is_done = true;
	int message_size = 0;
	// Get channel count
	uint16_t channel_count = this_group->Get_Channel_Count();
	// Do all of our allocations up front
	IOT_DATA_CHANNEL_REALTIMES_ITEM* realtime_items =
		reinterpret_cast<IOT_DATA_CHANNEL_REALTIMES_ITEM*>( Ram::Allocate( sizeof( IOT_DATA_CHANNEL_REALTIMES_ITEM ) *
																		   channel_count ) );

	// This one holds all the fetched data bytes end-to-end; assumes all channels
	uint8_t* data_buffer = reinterpret_cast<uint8_t*>( Ram::Allocate( sizeof( uint8_t ) * IOT_DCI_DATA_BUFFER_SIZE ) );

	// This array will hold the list of indices that we must process and hopefully publish
	uint16_t* group_indices = reinterpret_cast<uint16_t*>( Ram::Allocate( sizeof( uint16_t ) * channel_count ) );

	BF_ASSERT( realtime_items );
	BF_ASSERT( data_buffer );
	BF_ASSERT( group_indices );
	for ( uint16_t i = 0; i < channel_count; i++ )
	{
		group_indices[i] = 0U;
	}

	if ( realtime_items && data_buffer && group_indices )
	{
		DCI_ERROR_TD error = DCI_ERR_NO_ERROR;
		uint16_t data_length = 0;
		uint16_t length = 0;
		bool change_flag = false;
		uint16_t idx_count = 0;
		uint16_t estimated_message_length = 0;
		DCI_ID_TD dcid;
		// Now, one of two paths: All Channels in this Group, OR only the Updated Channels in this Group
		if ( send_all )
		{
			is_done = Get_All_Fitting_Channels( this_group, group_indices, idx_count, estimated_message_length );
			// Now clear any channels already marked in this group's Change Tracker, since we are getting All.
			do
			{
				dcid = this_group->Get_Next_Channel();
			} while ( is_done && ( dcid != 0 ) );
		}
		else
		{
			is_done = Get_Updated_Fitting_Channels( this_group, group_indices, idx_count, estimated_message_length );
		}
		IOT_DATA message_data_struct;
		if ( ( idx_count > 0 ) && Initialize_Message_Data_Struct( message_data_struct, CHANNEL_REALTIMES ) )
		{
			long now = get_time( nullptr );
			for ( uint_fast16_t counter = 0; counter < idx_count; counter++ )
			{
				uint16_t channel_index = group_indices[counter];
				const iot_channel_config_struct_t* channel_struct = m_iot_dci->Get_Channel_Config( channel_index );
				if ( channel_struct != nullptr )
				{
					error = m_iot_dci->Get_DCI_Value( channel_struct->dcid, channel_struct->array_offset,
													  data_buffer + length, &data_length );
					if ( error == DCI_ERR_NO_ERROR )
					{
						realtime_items[counter].channelTag = channel_struct->channel_tag_str;
						realtime_items[counter].time = now;
						/* If we change to start providing non-zero milliseconds here,
						 * then we need to add SERIALIZED_MS_TIME_LENGTH to the estimate for each channel. */
						realtime_items[counter].milliseconds = 0;
						realtime_items[counter].value = reinterpret_cast<char*>( data_buffer + length );
						realtime_items[counter].disconnected = 0;
						realtime_items[counter].disabled = 0;
						realtime_items[counter].disarmed = 0;

						singlylinkedlist_add( message_data_struct.channelRealtimes, &realtime_items[counter] );
						change_flag = true;
						length += ( data_length + 1 );
					}
				}
			}

			if ( change_flag == true )
			{
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Publishing %u Realtimes for Group Marked: %u, %s Done", idx_count,
								 this_group->Get_Group_Marker(), ( is_done ? "is" : "NOT" ) );
				if ( iot_send( m_device_handle, &message_data_struct, &message_size ) )
				{
					if ( message_size <= estimated_message_length )
					{
						IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Sent Realtimes message, len = %d vs estimated = %d",
										 message_size, estimated_message_length );
					}
					else
					{
						IOT_DEBUG_PRINT( DBG_MSG_ERROR,
										 "Realtimes message for %d channels, len = %d is more than estimated = %d",
										 idx_count, message_size, estimated_message_length );
					}
				}
				else
				{
					IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to send Realtimes message" );
				}
			}
			singlylinkedlist_destroy( message_data_struct.channelRealtimes );
		}
	}

	Ram::De_Allocate( group_indices );
	Ram::De_Allocate( data_buffer );
	Ram::De_Allocate( realtime_items );

	return ( is_done );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Publish_StoreMeOnInterval_Trends_Message( IOT_Group_Handler* this_group )
{
	///< Publish the StoreMe/trend on-interval type data for all channels of the given group.
	bool is_done = true;
	int message_size = 0;
	// Get channel count
	uint16_t channel_count = this_group->Get_Channel_Count();
	// Do all of our allocations up front
	IOT_DATA_TREND_ITEM* trend_items =
		reinterpret_cast<IOT_DATA_TREND_ITEM*>( Ram::Allocate( sizeof( IOT_DATA_TREND_ITEM ) * channel_count ) );
	// This one holds all the fetched data bytes end-to-end; assumes all channels
	uint8_t* data_buffer = reinterpret_cast<uint8_t*>( Ram::Allocate( sizeof( uint8_t ) * IOT_DCI_DATA_BUFFER_SIZE ) );
	// This array will hold the list of indices that we must process and hopefully publish
	uint16_t* group_indices = reinterpret_cast<uint16_t*>( Ram::Allocate( sizeof( uint16_t ) * channel_count ) );

	BF_ASSERT( trend_items );
	BF_ASSERT( data_buffer );
	BF_ASSERT( group_indices );
	for ( uint16_t i = 0; i < channel_count; i++ )
	{
		group_indices[i] = 0U;
	}

	if ( trend_items && data_buffer && group_indices )
	{
		DCI_ERROR_TD error = DCI_ERR_NO_ERROR;
		uint16_t data_length = 0;
		uint16_t length = 0;
		bool change_flag = false;
		uint16_t idx_count = 0;
		uint16_t estimated_message_length = 0;
		is_done = Get_All_Fitting_Channels( this_group, group_indices, idx_count, estimated_message_length );
		IOT_DATA message_data_struct;
		if ( ( idx_count > 0 ) && Initialize_Message_Data_Struct( message_data_struct, TRENDS ) )
		{
			long now = get_time( nullptr );
			for ( uint_fast16_t counter = 0; counter < idx_count; counter++ )
			{
				uint16_t channel_index = group_indices[counter];
				const iot_channel_config_struct_t* channel_struct = m_iot_dci->Get_Channel_Config( channel_index );
				if ( channel_struct != nullptr )
				{
					error = m_iot_dci->Get_DCI_Value( channel_struct->dcid, channel_struct->array_offset,
													  data_buffer + length, &data_length );
					if ( error == DCI_ERR_NO_ERROR )
					{
						trend_items[counter].channelTag = channel_struct->channel_tag_str;
						trend_items[counter].time = now;
						// Publish as "actual value" in all cases
						trend_items[counter].actValue = reinterpret_cast<char*>( data_buffer + length );

						singlylinkedlist_add( message_data_struct.trends, &trend_items[counter] );
						change_flag = true;
						length += ( data_length + 1 );
					}
				}
			}

			if ( change_flag == true )
			{
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Publishing %u Trends-on-interval for Group Marked: %u, %s Done",
								 idx_count,
								 this_group->Get_Group_Marker(), ( is_done ? "is" : "NOT" ) );
				if ( iot_send( m_device_handle, &message_data_struct, &message_size ) )
				{
					if ( message_size <= estimated_message_length )
					{
						IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Sent Trends-on-interval message, len = %d vs estimated = %d",
										 message_size,
										 estimated_message_length );
					}
					else
					{
						IOT_DEBUG_PRINT( DBG_MSG_ERROR,
										 "Trends-on-interval message for %d channels, len = %d is more than estimated = %d",
										 idx_count, message_size, estimated_message_length );
					}
				}
				else
				{
					IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to send Trends-on-interval message" );
				}
			}
			singlylinkedlist_destroy( message_data_struct.trends );
		}
	}
	Ram::De_Allocate( group_indices );
	Ram::De_Allocate( data_buffer );
	Ram::De_Allocate( trend_items );

	return ( is_done );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Publish_StoreMe_Trends_Message( IOT_Group_Handler* this_group, bool send_all )
{
	///< Publish the StoreMe/trend type data for all or changed channels of the given group.
	bool is_done = true;
	int message_size = 0;
	// Get channel count
	uint16_t channel_count = this_group->Get_Channel_Count();
	// Do all of our allocations up front
	IOT_DATA_TREND_ITEM* trend_items =
		reinterpret_cast<IOT_DATA_TREND_ITEM*>( Ram::Allocate( sizeof( IOT_DATA_TREND_ITEM ) * channel_count ) );
	// This one holds all the fetched data bytes end-to-end; assumes all channels
	uint8_t* data_buffer = reinterpret_cast<uint8_t*>( Ram::Allocate( sizeof( uint8_t ) * IOT_DCI_DATA_BUFFER_SIZE ) );
	// This array will hold the list of indices that we must process and hopefully publish
	uint16_t* group_indices = reinterpret_cast<uint16_t*>( Ram::Allocate( sizeof( uint16_t ) * channel_count ) );

	BF_ASSERT( trend_items );
	BF_ASSERT( data_buffer );
	BF_ASSERT( group_indices );

	for ( uint16_t i = 0; i < channel_count; i++ )
	{
		group_indices[i] = 0U;
	}

	if ( trend_items && data_buffer && group_indices )
	{
		DCI_ERROR_TD error = DCI_ERR_NO_ERROR;
		uint16_t data_length = 0;
		uint16_t length = 0;
		bool change_flag = false;
		uint16_t idx_count = 0;
		uint16_t estimated_message_length = 0;
		DCI_ID_TD dcid;
		// Now, one of two paths: All Channels in this Group, OR only the Updated Channels in this Group
		if ( send_all )
		{
			is_done = Get_All_Fitting_Channels( this_group, group_indices, idx_count, estimated_message_length );
			// Now clear any channels already marked in this group's Change Tracker, since we are getting All.
			do
			{
				dcid = this_group->Get_Next_Channel();
			} while ( is_done && ( dcid != 0 ) );
		}
		else
		{
			is_done = Get_Updated_Fitting_Channels( this_group, group_indices, idx_count, estimated_message_length );
		}
		IOT_DATA message_data_struct;
		if ( ( idx_count > 0 ) && Initialize_Message_Data_Struct( message_data_struct, TRENDS ) )
		{
			long now = get_time( nullptr );
			for ( uint_fast16_t counter = 0; counter < idx_count; counter++ )
			{
				uint16_t channel_index = group_indices[counter];
				const iot_channel_config_struct_t* channel_struct = m_iot_dci->Get_Channel_Config( channel_index );
				if ( channel_struct != nullptr )
				{
					error = m_iot_dci->Get_DCI_Value( channel_struct->dcid, channel_struct->array_offset,
													  data_buffer + length, &data_length );
					if ( error == DCI_ERR_NO_ERROR )
					{
						trend_items[counter].channelTag = channel_struct->channel_tag_str;
						trend_items[counter].time = now;
						// Publish as "actual value" in all cases
						trend_items[counter].actValue = reinterpret_cast<char*>( data_buffer + length );

						singlylinkedlist_add( message_data_struct.trends, &trend_items[counter] );
						change_flag = true;
						length += ( data_length + 1 );
					}
				}
			}

			if ( change_flag == true )
			{
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Publishing %u Trends for Group Marked: %u, %s Done", idx_count,
								 this_group->Get_Group_Marker(), ( is_done ? "is" : "NOT" ) );
				if ( iot_send( m_device_handle, &message_data_struct, &message_size ) )
				{
					if ( message_size <= estimated_message_length )
					{
						IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Sent Trends message, len = %d vs estimated = %d", message_size,
										 estimated_message_length );
					}
					else
					{
						IOT_DEBUG_PRINT( DBG_MSG_ERROR,
										 "Trends message for %d channels, len = %d is more than estimated = %d",
										 idx_count, message_size, estimated_message_length );
					}
				}
				else
				{
					IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to send Trends message" );
				}
			}
			singlylinkedlist_destroy( message_data_struct.trends );
		}
	}

	Ram::De_Allocate( group_indices );
	Ram::De_Allocate( data_buffer );
	Ram::De_Allocate( trend_items );

	return ( is_done );
}

#ifdef IOT_ALARM_SUPPORT
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Publish_Alarms_Message( void )
{
	///< Get IOT_Alarms instance
	IOT_Alarms* iot_alarms_instance = IOT_Alarms::Get_IOT_Alarms_Handle_Static();
	int message_size = 0;

	BF_ASSERT( iot_alarms_instance );

	///< Get alarm count
	uint32_t alarm_count = iot_alarms_instance->Get_Alarm_Update_Count();

	///< Do all of our allocations up front
	IOT_DATA_ALARM_ITEM* alarm_items = new IOT_DATA_ALARM_ITEM[alarm_count];
	IOT_DATA_ALARM_TRIGGER_ITEM* trigger_items = new IOT_DATA_ALARM_TRIGGER_ITEM[alarm_count];

	BF_ASSERT( alarm_items );
	BF_ASSERT( trigger_items );

	bool change_flag = false;
	bool is_done = false;

	///< Create vector to store timestamps, threshold of all alarms
	VECTOR_HANDLE alarm_timestamp = VECTOR_create( sizeof( char* ) );
	VECTOR_HANDLE threshold_value = VECTOR_create( sizeof( STRING_HANDLE ) );

	BF_ASSERT( alarm_timestamp );
	BF_ASSERT( threshold_value );

	for ( uint32_t i = 0; i < alarm_count; i++ )
	{
		char* timestamp_iso_8601 = new char[Timestamp_Format::TIMESTAMP_ISO_8601_SIZE];
		BF_ASSERT( timestamp_iso_8601 );
		if ( VECTOR_push_back( alarm_timestamp, &timestamp_iso_8601, 1 ) != 0 )
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "unable to VECTOR_push_back for alarm timestamp" );
			delete[] timestamp_iso_8601;
			break;
		}
		// coverity[leaked_storage]
	}

	for ( uint32_t i = 0; i < alarm_count; i++ )
	{
		STRING_HANDLE threshold_val = STRING_new();
		BF_ASSERT( threshold_val );
		if ( VECTOR_push_back( threshold_value, &threshold_val, 1 ) != 0 )
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "unable to VECTOR_push_back for threshold_value" );
			STRING_delete( threshold_val );
			break;
		}
		// coverity[leaked_storage]
	}

	if ( alarm_items )
	{
		///< Estimate message buffer size
		uint16_t alarm_fit_count = 0;
		uint16_t estimated_message_length = 0;

		is_done = Get_Fitting_Alarm_Updates( alarm_fit_count, estimated_message_length );

		IOT_DATA message_data_struct;
		if ( ( alarm_fit_count > 0 ) && ( Initialize_Message_Data_Struct( message_data_struct, ALARMS ) ) )
		{
			time_t now = 0;
			///< Read the alarm updates from log
			uint16_t buffer_size = ( sizeof( event_log_t ) );
			uint8_t* log_data = new uint8_t[buffer_size];
			log_events_req_t log_data_req = {0U};
			log_events_resp_t log_data_resp = {0U};
			event_log_t alarm_log_data = {0U};
			log_data_req.entry_count = 1;
			log_data_req.buffer_size = buffer_size;
			log_data_resp.data = log_data;

			for ( uint_fast16_t counter = 0; counter < alarm_fit_count; counter++ )
			{
				///< Get oldest entries first
				log_data_req.start_index = iot_alarms_instance->m_last_pub_head_index + counter;
				iot_alarms_instance->Get_Event_Data( log_data_req, log_data_resp );
				Copy_String( reinterpret_cast<uint8_t*>( &alarm_log_data ), log_data_resp.data, buffer_size );
				alarm_items[counter].trigger = &trigger_items[counter];
				STRING_HANDLE* threshold_val =
					reinterpret_cast<STRING_HANDLE*>( VECTOR_element( threshold_value, counter ) );
				BF_ASSERT( threshold_val );
				BF_ASSERT( *threshold_val );
				now = iot_alarms_instance->Fill_Alarms_Message( alarm_items[counter], *threshold_val, alarm_log_data );
				char** timestamp_iso_8601 = reinterpret_cast<char**>( VECTOR_element( alarm_timestamp, counter ) );
				BF_ASSERT( timestamp_iso_8601 );
				BF_ASSERT( *timestamp_iso_8601 );
				/**This conversion of timestamp to ISO 8601 format is mandatory when we publish alarms message(Otherwise
				 * Alarms will not appear when cloud side alarm's APIs are executed. The published
				 * alarms will show up in cloud database though)
				 * */
				Timestamp_Format::Get_Timestamp_In_ISO_8601_Format( *timestamp_iso_8601, now );

				// alarm_items[counter].channel_v = STRING_c_str( *channel_val );///< Unused
				alarm_items[counter].timestamp = *timestamp_iso_8601;///< The timestamp in ISO-8601 format, eg
																	///< 2021-05-14T11:24:24.000Z
				alarm_items[counter].device_id = Get_Publisher_Device_Uuid();
				alarm_items[counter].source = IOT_Alarms::ALARM_SOURCE;
				alarm_items[counter].version = IOT_Alarms::ALARM_SCHEMA_VERSION;///< Alarms schema version

				singlylinkedlist_add( message_data_struct.alarms, &alarm_items[counter] );
				change_flag = true;
			}

			if ( change_flag == true )
			{
				if ( iot_send( m_device_handle, &message_data_struct, ( int* )( &message_size ) ) )
				{
					if ( message_size <= estimated_message_length )
					{
						IOT_DEBUG_PRINT( DBG_MSG_DEBUG,
										 "Sent Alarms message, len = %d vs estimated = %d, alarm count = %d",
										 message_size,
										 estimated_message_length, alarm_fit_count );
					}
					else
					{
						IOT_DEBUG_PRINT( DBG_MSG_ERROR,
										 "Sent Alarms message, len = %d is more than estimated = %d, alarm count = %d",
										 message_size,
										 estimated_message_length, alarm_fit_count );
					}

					///< Save the indices
					iot_alarms_instance->m_last_pub_head_index += alarm_fit_count;	///< Published
					iot_alarms_instance->m_last_pub_idx_owner->Check_In_Init( reinterpret_cast<uint32_t*>( &
																										   iot_alarms_instance
																										   ->
																										   m_last_pub_head_index ) );
					// IOT_DEBUG_PRINT( DBG_MSG_ERROR, "m_last_pub_head_index = %d",
					// iot_alarms_instance->m_last_pub_head_index );
				}
				else
				{
					IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to send Alarms message" );
				}
			}
			singlylinkedlist_destroy( message_data_struct.alarms );
			delete[] log_data;
		}
	}

	///< Deallocate all the allocations
	if ( alarm_timestamp != nullptr )
	{
		for ( uint32_t i = 0; i < alarm_count; i++ )
		{
			char** timestamp_iso_8601 = reinterpret_cast<char**>( VECTOR_element( alarm_timestamp, i ) );
			BF_ASSERT( timestamp_iso_8601 );
			BF_ASSERT( *timestamp_iso_8601 );
			STRING_HANDLE* threshold_val = reinterpret_cast<STRING_HANDLE*>( VECTOR_element( threshold_value, i ) );
			BF_ASSERT( threshold_val );
			BF_ASSERT( *threshold_val );
			delete *timestamp_iso_8601;
			STRING_delete( *threshold_val );
		}
	}
	VECTOR_destroy( alarm_timestamp );
	VECTOR_destroy( threshold_value );
	delete[] alarm_items;
	delete[] trigger_items;

	return ( is_done );
}

#endif	// IOT_ALARM_SUPPORT

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Initialize_Message_Data_Struct( IOT_DATA& message_data_struct, IOT_DATA_TYPE message_type )
{
	///< Initialize the given data structure for one message, of the given message type.
	bool is_success;

	memset( &message_data_struct, 0, sizeof( IOT_DATA ) );
	message_data_struct.dataType = message_type;
	message_data_struct.deviceUUID = m_device_uuid;
	switch ( message_type )
	{
		case CHANNEL_REALTIMES:
			message_data_struct.channelRealtimes = singlylinkedlist_create();
			is_success = ( message_data_struct.channelRealtimes != nullptr );
			break;

		case TRENDS:
			message_data_struct.trends = singlylinkedlist_create();
			is_success = ( message_data_struct.trends != nullptr );
			break;

		case ALARMS:
			message_data_struct.alarms = singlylinkedlist_create();
			is_success = ( message_data_struct.alarms != nullptr );
			break;

		default:
			is_success = false;
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Unsupported message type." );
			break;
	}
	return ( is_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Get_All_Fitting_Channels( IOT_Group_Handler* this_group, uint16_t* group_indices,
											uint16_t& idx_count, uint16_t& estimated_message_length )
{
	bool all_fit = true;
	uint16_t this_group_marker = this_group->Get_Group_Marker();

	idx_count = 0;
	if ( group_indices != nullptr )
	{
		estimated_message_length = MESSAGE_HEADER_LENGTH;
		for ( uint16_t search_index = m_starting_search_index;
			  search_index < m_iot_dci_database->total_channels;
			  search_index++ )
		{
			const iot_channel_config_struct_t* channel_struct = m_iot_dci->Get_Channel_Config( search_index );
			if ( channel_struct != nullptr )
			{
				if ( ( channel_struct->group_memberships & this_group_marker ) == 0 )
				{
					continue;		// Not a member of this group
				}
				uint16_t est_channel_length = Estimate_Serialized_Size( channel_struct->dcid );
				if ( ( estimated_message_length + est_channel_length ) < IOT_MAX_OUTBOUND_BUFFER_SIZE )
				{
					if ( idx_count >= this_group->Get_Channel_Count() )
					{
						// This might be normal, if we've gotten all and still have more channels
						// in the overall list that are not in this group.
						break;
					}
					// Save this index/offset
					group_indices[idx_count++] = search_index;
					estimated_message_length += est_channel_length;
				}
				else
				{
					// We reached the end of the message buffer before we looked at all channels
					m_starting_search_index = search_index;
					IOT_DEBUG_PRINT( DBG_MSG_INFO, "Only fit %u channels in array, estimated message size = %u.",
									 idx_count, estimated_message_length );
					all_fit = false;
					break;
				}
			}
		}
		if ( all_fit )
		{
			m_starting_search_index = 0;
			IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Done fitting %u channels in array, estimated message size = %u.",
							 idx_count, estimated_message_length );
		}
	}
	return ( all_fit );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Get_Updated_Fitting_Channels( IOT_Group_Handler* this_group, uint16_t* group_indices,
												uint16_t& idx_count, uint16_t& estimated_message_length )
{
	bool all_fit = true;

	idx_count = 0;
	if ( group_indices != nullptr )
	{
		estimated_message_length = MESSAGE_HEADER_LENGTH;
		DCI_ID_TD dcid = this_group->Peek_Next_Channel();
		while ( dcid != 0 )
		{
			uint16_t search_index = m_iot_dci->Find_Index_From_DCID( dcid );
			BF_ASSERT( search_index != IOT_DCI::IOT_INPUT_ERROR );

			uint16_t est_channel_length = Estimate_Serialized_Size( dcid );
			if ( ( estimated_message_length + est_channel_length ) < IOT_MAX_OUTBOUND_BUFFER_SIZE )
			{
				if ( idx_count >= this_group->Get_Channel_Count() )
				{
					IOT_DEBUG_PRINT( DBG_MSG_INFO, "Done updating channels in array." );
					break;
				}
				// Save this index/offset
				group_indices[idx_count++] = search_index;
				estimated_message_length += est_channel_length;
				// And "Get" the (same) ID to clear it from the Change Tracker
				static_cast<void>( this_group->Get_Next_Channel() );
			}
			else
			{
				// We reached the end of the message buffer before we looked at all channels
				all_fit = false;
				break;
			}
			dcid = this_group->Peek_Next_Channel();
		}
	}
	return ( all_fit );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t IOT_Pub_Sub::Estimate_Serialized_Size( DCI_ID_TD dcid )
{
	///< Given the DCI ID, get its type and estimate the length of its value when serialized.
	// Starting estimate for all but the actual value
	uint16_t estimated_length = SERIALIZED_TAG_LENGTH + SERIALIZED_TIMESTAMP_LENGTH + SERIALIZED_VALUE_ITEM_LENGTH;
	DCI_DATATYPE_TD data_type = m_iot_dci->Get_Datatype( dcid );
	DCI_LENGTH_TD data_len = m_iot_dci->Get_Length( dcid );

	switch ( data_type )
	{
		case DCI_DTYPE_FLOAT:
		case DCI_DTYPE_LFLOAT:
		case DCI_DTYPE_DFLOAT:
			estimated_length += data_len * 3;		// So 12 bytes for an IEEE 32bit float
			break;

		case DCI_DTYPE_STRING8:
			estimated_length += data_len;
			break;

		default:
			// For the rest, estimate it at 2 chars per byte plus 2 bytes padding
			estimated_length += ( data_len * 2 ) + 2;
			break;
	}

	return ( estimated_length );
}

#ifdef IOT_ALARM_SUPPORT
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Get_Fitting_Alarm_Updates( uint16_t& alarm_fit_count,
											 uint16_t& estimated_message_length )
{
	bool all_fit = true;
	IOT_Alarms* iot_alarms_instance = IOT_Alarms::Get_IOT_Alarms_Handle_Static();

	BF_ASSERT( iot_alarms_instance );
	uint8_t alarm_update_count = iot_alarms_instance->Get_Alarm_Update_Count();

	///< Count how many alarms will fit in
	for ( alarm_fit_count = 0; alarm_fit_count < alarm_update_count; alarm_fit_count++ )
	{
		///< Estimate serialized size of 1 alarm
		uint16_t est_alarm_length = Estimate_Serialized_Alarms_Size();
		if ( ( estimated_message_length + est_alarm_length ) < IOT_MAX_OUTBOUND_BUFFER_SIZE )
		{
			estimated_message_length += est_alarm_length;
		}
		else
		{
			// We reached the end of the message buffer before we looked at all alarms
			all_fit = false;
			break;
		}
	}
	IOT_DEBUG_PRINT( DBG_MSG_INFO, "%u alarms out of %u fit into the message buffer, estimated message size = %u.",
					 alarm_fit_count, alarm_update_count, estimated_message_length );
	return ( all_fit );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t IOT_Pub_Sub::Estimate_Serialized_Alarms_Size( void )
{
	IOT_Alarms* iot_alarms_instance = IOT_Alarms::Get_IOT_Alarms_Handle_Static();

	BF_ASSERT( iot_alarms_instance );

	IOT_Event_Interface* iot_interface_instance = IOT_Event_Interface::Get_IOT_Event_Interface_Handle_Static();

	///< Total length of all fixed length literals
	uint16_t estimated_length = strlen( IOT_Alarms::ALARM_MESSAGE_TEMPLATE );

	///< TODO SERIALIZED_ALARM_CHANNEL_ID_LENGTH + SERIALIZED_ALARM_CHANNEL_VAL_LENGTH

	///< Add the lengths of all variable length literals/variables
	estimated_length +=
		INT_ASCII_Conversion::UINT16_Number_Digits( static_cast<uint16_t>( IOT_Alarms::ALARM_SCHEMA_VERSION ) );
	uint16_t max_severity_level = iot_interface_instance->Get_Severity_Levels();

	estimated_length += INT_ASCII_Conversion::UINT16_Number_Digits( max_severity_level );///< Ack lvl
	estimated_length +=
		BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>( const_cast<char*>( IOT_Alarms::ALARM_SOURCE ) ), 10 );

	///< Note ptr and user ptr considering max length
	estimated_length += IOT_Alarms::MAX_NOTE_LENGTH;
	estimated_length += IOT_Alarms::MAX_USER_LENGTH;

	///< Estimate Trigger info
	estimated_length += iot_alarms_instance->Get_Max_Rulename_Len();	///< Trigger id
	estimated_length += INT_ASCII_Conversion::UINT16_Number_Digits( max_severity_level );
	estimated_length += INT_ASCII_Conversion::UINT16_Number_Digits( iot_alarms_instance->Get_Max_Trigger_Type() );
	estimated_length += INT_ASCII_Conversion::UINT16_Number_Digits( iot_alarms_instance->Get_Max_Priority() );
	estimated_length += MAX_TRIGGER_VALUE_SIZE;	///< Threshold
	///< threshold2 => Unused
	///< Channel value length(TODO Currently unused as we don't have channel based alarms in place)
	/*
	    DCI_DATATYPE_TD data_type = m_iot_dci->Get_Datatype( DCI_AE_RULE_NAME0_DCID );
	    DCI_LENGTH_TD data_len = m_iot_dci->Get_Length( DCI_AE_RULE_NAME0_DCID );

	    switch ( data_type )
	    {
	        case DCI_DTYPE_FLOAT:
	        case DCI_DTYPE_LFLOAT:
	        case DCI_DTYPE_DFLOAT:
	            estimated_length += data_len * 3;		// So 12 bytes for an IEEE 32bit float
	            break;

	        case DCI_DTYPE_STRING8:
	            estimated_length += data_len;
	            break;

	        default:
	            // For the rest, estimate it at 2 chars per byte plus 2 bytes padding
	            estimated_length += ( data_len * 2 ) + 2;
	            break;
	    }
	 */
	return ( estimated_length );
}

#endif	// IOT_ALARM_SUPPORT

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Pub_Sub::Is_Publish_All_Channels_On_Connect( uint16_t group_index )
{
	bool is_allowed = false;
	IOT_Group_Handler** the_group = reinterpret_cast<IOT_Group_Handler**>( VECTOR_element( m_group_handlers,
																						   group_index ) );

	if ( ( the_group != nullptr ) && ( *the_group != nullptr ) && ( m_device_handle != nullptr ) )
	{
		is_allowed = ( *the_group )->Allow_Publishing_All_channels_On_Connect();
	}
	return ( is_allowed );
}
