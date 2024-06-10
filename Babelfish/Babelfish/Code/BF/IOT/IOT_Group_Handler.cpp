/**
 *****************************************************************************************
 * @file IOT_Group_Handler.cpp
 * @details Implements the IOT_Group_Handler class.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "IOT_Group_Handler.h"
#include "IOT_Net.h"
#include "DCI_Owner.h"
#include "Change_Tracker_Plus.h"
#include "IOT_Debug.h"
#include "Babelfish_Assert.h"


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Group_Handler::IOT_Group_Handler( const IOT_DCI* iot_dci_handler, uint16_t group_index,
									  VECTOR_HANDLE cadence_owners ) :
	m_channel_count( 0 ),
	m_cadence_ms( 0 ),
	m_has_trends_on_interval( false ),
	m_has_trends( false ),
	m_has_realtimes( false ),
	m_group_tracker( nullptr ),
	m_cadence_check_timer( nullptr ),
	m_publish_all_on_connect( false ),
	m_cadence_rate_owner( cadence_owners ),
	m_group_index( group_index ),
	m_is_group_publishing_enabled( false )
{
	// Get our group configuration
	const iot_cadence_group_struct_t* our_group = iot_dci_handler->Get_Group_Config( group_index );

	BF_ASSERT( our_group != nullptr );
	m_group_marker = our_group->group_marker;
	m_has_trends_on_interval = !( our_group->publication_types ^ IOT_DCI::PUB_TYPE_STOREMEONINTERVAL );
	m_has_realtimes = !( our_group->publication_types ^ IOT_DCI::PUB_TYPE_SEEME );
	m_has_trends = !( our_group->publication_types ^ IOT_DCI::PUB_TYPE_STOREME );
	m_publish_all_on_connect = !( our_group->pub_all_on_connect ^ IOT_DCI::PUBLISH_ALL_ON_CONNECT );

	if ( !Initialize_Cadence( group_index, cadence_owners ) )
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to initialize group %u", group_index );
	}
	else
	{
		if ( m_has_realtimes || m_has_trends )
		{
			Attach_Change_Tracker( iot_dci_handler );
		}

		// Now count the number of channels that are actually members of this group
		for ( uint16_t counter = 0; counter < iot_dci_handler->Get_Total_Channels(); counter++ )
		{
			const iot_channel_config_struct_t* channel_struct = iot_dci_handler->Get_Channel_Config( counter );
			if ( ( channel_struct != nullptr ) && ( channel_struct->group_memberships & m_group_marker ) )
			{
				m_channel_count++;
			}
		}

		IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Group %u count = %u cadence = %u ms for %s, %s", group_index, m_channel_count,
						 m_cadence_ms,
						 ( m_has_realtimes ? "SeeMe" : "" ), ( m_has_trends_on_interval ? "StoreMeOnInterval" : "" ),
						 ( m_has_trends ? "StoreMe" : "" ) );

		IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Group %u will %s publish all channels on connect", group_index,
						 ( m_publish_all_on_connect ? "" : "not" ) );


		// Create a new timer to handle the cadence timing for this group.
		m_cadence_check_timer = new BF_Lib::Timers( Cadence_Check_Timer_Static,
													reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
													CR_TASKER_PRIORITY_0 );
		if ( ( m_cadence_check_timer != nullptr ) && ( m_is_group_publishing_enabled ) )
		{
			m_cadence_check_timer->Start( m_cadence_ms, true );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Group_Handler::~IOT_Group_Handler( void )
{
	///< Default XTOR for this class.
	if ( m_cadence_check_timer != nullptr )
	{
		delete m_cadence_check_timer;
	}
	if ( m_group_tracker != nullptr )
	{
		delete m_group_tracker;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Group_Handler::Initialize_Cadence( uint16_t group_index, VECTOR_HANDLE cadence_owners )
{
	///< Read the DCI value for this group's cadence, and convert to milliseconds.
	bool success = false;
	DCI_Owner* the_owner = reinterpret_cast<DCI_Owner*>( VECTOR_element( cadence_owners, group_index ) );

	if ( the_owner != nullptr )
	{
		the_owner->Attach_Callback( Reinitialize_Cadence_static,
									reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
									DCI_ACCESS_POST_SET_RAM_CMD_MSK );
		int32_t cadence_msec = 0;
		success = the_owner->Check_Out( cadence_msec );
		if ( success )			// We do allow a 0 value
		{
			Validate_Set_Cadence_Rate( cadence_msec );
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t IOT_Group_Handler::Get_Group_Cadence_ms( void )
{
	///< Getter for the current cadence, in milliseconds, for this group.
	return ( m_cadence_ms );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Group_Handler::Has_Trends_On_Interval( void )
{
	///< Tells if this group has any StoreMe/trended on-interval channel members.
	return ( m_has_trends_on_interval );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Group_Handler::Has_Realtimes( void )
{
	///< Tells if this group has any SeeMe/realtime update channel members.
	return ( m_has_realtimes );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Group_Handler::Has_Trends( void )
{
	///< Tells if this group has any StoreMe/Trends update channel members.
	return ( m_has_trends );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t IOT_Group_Handler::Get_Group_Marker( void )
{
	///< Simple getter for the Group Marker for this group.
	return ( m_group_marker );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t IOT_Group_Handler::Get_Channel_Count( void )
{
	///< Simple getter for the number of channels in this group.
	return ( m_channel_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Group_Handler::Is_Data_Changed( void )
{
	///< Checks if any of the SeeMe/realtime channel values subscribed to for this group are changed.
	DCI_ID_TD next_data_id = Peek_Next_Channel();

	// If that returned a non-zero ID, then there is some data changed
	return ( next_data_id != 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Group_Handler::Attach_Change_Tracker( const IOT_DCI* iot_dci_handler )
{
	// We only want to be pay attention to changes:
	bool value_change_only = true;
	/* COMPATIBLE_TRACKING_MASK means changes in either RAM or NV storage for the channels.
	 * There is a cost to having both: one extra Bit_List in the Tracker.
	 * Determined that we don't care about the NV case, so using just RAM_ATTRIB_MASK.
	 * Note that this is RAM_ATTRIB_MASK here, not RAM_ATTRIB as is used in the other calls. */
	Change_Tracker::attrib_mask_t attribute_track_bitfield = Change_Tracker::RAM_ATTRIB_MASK;

	m_group_tracker = new Change_Tracker_Plus( value_change_only, attribute_track_bitfield );

	if ( m_group_tracker == nullptr )
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to allocate change tracker." );
	}
	else
	{
		uint16_t counter;
		for ( counter = 0; counter < iot_dci_handler->Get_Total_Channels(); counter++ )
		{
			const iot_channel_config_struct_t* channel_struct = iot_dci_handler->Get_Channel_Config( counter );
			if ( ( channel_struct != nullptr ) && ( channel_struct->group_memberships & m_group_marker ) )
			{
				m_group_tracker->Track_ID( channel_struct->dcid );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ID_TD IOT_Group_Handler::Peek_Next_Channel( void )
{
	///< Peek into the Change Tracker and learn what the next changed channel's DCI ID would be.
	DCI_ID_TD data_id = 0;
	// Here we use the RAM_ATTRIB, not the RAM_ATTRIB_MASK type.
	Change_Tracker::attrib_t attribute = Change_Tracker::RAM_ATTRIB;

	if ( !m_group_tracker->Peek_Next( &data_id, attribute ) )
	{
		// If Peek_Next() returns False, data_id is returned as DCI_ID_UNDEFINED, so override that
		data_id = 0;
	}
	return ( data_id );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ID_TD IOT_Group_Handler::Get_Next_Channel( void )
{
	///< Gets the next changed channel's DCI ID from the Change Tracker.
	DCI_ID_TD data_id = 0;
	Change_Tracker::attrib_t attribute = Change_Tracker::RAM_ATTRIB;

	if ( !m_group_tracker->Get_Next( &data_id, attribute ) )
	{
		// If Get_Next() returns False, data_id is returned as DCI_ID_UNDEFINED, so override that
		data_id = 0;
	}
	return ( data_id );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Group_Handler::Cadence_Check_Timer_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	( reinterpret_cast<IOT_Group_Handler*>( param ) )->Cadence_Check_Timer_Task();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Group_Handler::Cadence_Check_Timer_Task( void )
{
	///< Steady timer task that checks to see if this group needs anything to be published.
	static long last_time = 0;

	// The easy case is if this group does StoreMe/trends on-interval type data;
	// if so, we always publish when triggered.
	if ( m_has_trends_on_interval )
	{
		IOT_Net::Mark_Group_Ready( m_group_marker );
		long now = get_time( nullptr );
		long delta = now - last_time;
		last_time = now;
		IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "In trend-on-interval timer; delta = %lds.", delta );
	}
	else if ( m_group_tracker != nullptr )
	{
		// See if any of this group's DCI items are set
		DCI_ID_TD data_id = 0;
		Change_Tracker::attrib_t attribute = Change_Tracker::RAM_ATTRIB;
		if ( m_group_tracker->Peek_Next( &data_id, attribute ) )
		{
			IOT_Net::Mark_Group_Ready( m_group_marker );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Group_Handler::Allow_Publishing_All_channels_On_Connect( void )
{
	return ( m_publish_all_on_connect );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Group_Handler::Has_Change_Tracker_Attached( void )
{
	// We only have a Change Tracker if we have SeeMe or StorMeOnChange types
	return ( m_group_tracker != nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD IOT_Group_Handler::Reinitialize_Cadence_static( DCI_CBACK_PARAM_TD handle,
															  DCI_ACCESS_ST_TD* access_struct )
{
	IOT_Group_Handler* object_ptr = static_cast<IOT_Group_Handler*>( handle );

	return ( object_ptr->Reinitialize_Cadence( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD IOT_Group_Handler::Reinitialize_Cadence( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	bool success = false;

	if ( access_struct->command == DCI_ACCESS_POST_SET_RAM_CMD )
	{
		DCI_Owner* the_owner = reinterpret_cast<DCI_Owner*>( VECTOR_element( m_cadence_rate_owner, m_group_index ) );
		int32_t cadence_msec;
		success = the_owner->Check_Out( cadence_msec );
		///< Read the DCI value for this group's cadence, stored in milliseconds.
		if ( success )
		{
			Validate_Set_Cadence_Rate( cadence_msec );
			if ( m_is_group_publishing_enabled )
			{
				///< Restart the timer with new cadence rate
				m_cadence_check_timer->Stop();
				m_cadence_check_timer->Start( m_cadence_ms, true );
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Setting new cadence rate %u ms for group %d", m_cadence_ms,
								 m_group_index );
			}
			else
			{
				///< Stop publishing - Stop timer and clear the group ready bit
				m_cadence_check_timer->Stop();
				IOT_Net::Clear_Group_Ready( m_group_marker );
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Publishing stopped for group %d", m_group_index );
			}
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Group_Handler::Validate_Set_Cadence_Rate( int32_t cadence_rate_msec )
{
	if ( cadence_rate_msec == m_CADENCE_RATE_0_SEC )
	{
		/* 0s indicates immediate publishing for all types except StoreMe-on-interval
		 * As IOT task sleep time is 100ms, we set the same for cadence check timer task
		 */
		m_cadence_ms = static_cast<uint32_t>( IOT_MESSAGE_SLEEP_MS );
		m_is_group_publishing_enabled = true;
	}
	else if ( cadence_rate_msec == m_CADENCE_STOP_PUBLISHING )
	{
		/* Stop publishing if cadence rate is -1. Just stop the timer and
		 * clear the group ready bit( Bit that indicates group is ready for
		 * publishing )
		 */
		m_is_group_publishing_enabled = false;
	}
	else
	{
		m_cadence_ms = static_cast<uint32_t>( cadence_rate_msec );
		m_is_group_publishing_enabled = true;
	}

	if ( m_has_trends_on_interval )
	{
		/* For StoreMe-on-interval types, we don't support cadence rate from 0 to 9 seconds
		 * If adopter wants to set cadence rate for StoreMe-on-interval types in the above range
		 * then please contact PX Green team
		 */
		if ( ( cadence_rate_msec >= m_CADENCE_RATE_0_SEC ) && ( cadence_rate_msec < m_CADENCE_RATE_10_SEC ) )
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR,
							 "Cadence rate 0 to 9 seconds isn't supported for store me on interval. Setting its value to 10, contact PX Green if you still want to allow" );
			m_cadence_ms = static_cast<uint32_t>( m_CADENCE_RATE_10_SEC );
			m_is_group_publishing_enabled = true;
		}
	}
}
