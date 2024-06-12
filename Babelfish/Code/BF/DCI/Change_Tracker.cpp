/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Change_Tracker.h"

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
Change_Tracker* const Change_Tracker::NULL_TRACKER =
	reinterpret_cast<Change_Tracker*>( nullptr );
BF_Lib::Bit_List* const Change_Tracker::NULL_BITLIST =
	reinterpret_cast<BF_Lib::Bit_List*>( nullptr );
Change_Tracker* Change_Tracker::m_tracker_list = NULL_TRACKER;
CR_Queue* Change_Tracker::m_cr_queue = reinterpret_cast<CR_Queue*>( nullptr );
bool Change_Tracker::m_task_woken = false;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule:
 * @n PCLint: Warning 1566: Variable might have been initialized by a separate function but no
 * '-sem(Change_Tracker::Initialize_Trackers,initializer)' was seen.
 * @n Justification: These are handled in the initialize Trackers function.  LINT is missing them.
 */
/*lint -e{1566} */
Change_Tracker::Change_Tracker( cback_func_t change_cb, cback_param_t change_cb_param,
								bool value_change_only, attrib_mask_t attribute_track_bitfield ) :
	m_next( reinterpret_cast<Change_Tracker*>( nullptr ) ),
	m_change_cb( change_cb ),
	m_change_cb_param( change_cb_param ),
	m_value_change_only( value_change_only ),
	m_attrib_scan_start( 0U )
{
	Initialize_Trackers( attribute_track_bitfield );

	if ( m_cr_queue == reinterpret_cast<CR_Queue*>( nullptr ) )
	{
		// coverity[leaked_storage]
		new CR_Tasker( &( Maintenance_Task ), reinterpret_cast<CR_TASKER_PARAM>( nullptr ),
					   CR_TASKER_PRIORITY_3, "Change Tracker" );

		m_cr_queue = new CR_Queue();

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule:
 * @n PCLint: Warning 1566: Variable might have been initialized by a separate function but no
 * '-sem(Change_Tracker::Initialize_Trackers,initializer)' was seen.
 * @n Justification: These are handled in the initialize Trackers function.  LINT is missing them.
 */
/*lint -e{1566} */
Change_Tracker::Change_Tracker( bool value_change_only, attrib_mask_t attribute_track_bitfield ) :
	m_next( reinterpret_cast<Change_Tracker*>( nullptr ) ),
	m_change_cb( reinterpret_cast<cback_func_t>( nullptr ) ),
	m_change_cb_param( reinterpret_cast<cback_param_t>( nullptr ) ),
	m_value_change_only( value_change_only ),
	m_attrib_scan_start( 0U )
{
	Initialize_Trackers( attribute_track_bitfield );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::Initialize_Trackers( attrib_mask_t attribute_track_bitfield )
{
	m_attrib_scan_start = 0U;
	for ( attrib_t i = 0U; i < TOTAL_ATTRIBS; i++ )
	{
		if ( BF_Lib::Bit::Test( attribute_track_bitfield, i ) )
		{
			// Because we start at 0 in this case m_attrib_scan_start will always be the largest
			// number possible when done.  This will reduce the attribute scan that is necessary
			// in most cases because not many people need the full gambit of coverage.
			m_attrib_scan_start = i + 1U;
			m_que[i] = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );

		}
		else
		{
			m_que[i] = NULL_BITLIST;
		}
	}

	for ( DCI_ID_TD i = 0U; i < TOTAL_BIT_WORDS; i++ )
	{
		m_track_array[i] = 0U;
	}

	Add_Tracker( this );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Change_Tracker::~Change_Tracker( void )
{
	Change_Tracker* tracker_walker;

	for ( attrib_t i = 0U; i < TOTAL_ATTRIBS; i++ )
	{
		delete m_que[i];
	}

	tracker_walker = m_tracker_list;
	if ( tracker_walker == this )
	{
		tracker_walker = m_next;
	}
	else
	{
		while ( tracker_walker->m_next != this )
		{
			tracker_walker = tracker_walker->m_next;
		}
		tracker_walker->m_next = m_next;
	}

	m_next = NULL_TRACKER;
	m_change_cb = reinterpret_cast<cback_func_t>( nullptr );
	m_change_cb_param = reinterpret_cast<cback_param_t>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::Maintenance_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	CR_QUEUE_STATUS result;
	Change_Tracker* tracker_walker;
	DCI_ID_TD dci_id;
	attrib_mask_t attribute_bits;

	BF_Lib::Unused<CR_TASKER_PARAM>::Okay( param );

	CR_Tasker_Begin( cr_task )

	for (;;)
	{
		CR_Queue_Receive( cr_task, m_cr_queue, NULL, OS_TASKER_INDEFINITE_TIMEOUT, result )
		if ( result == CR_QUEUE_PASS )
		{
			m_task_woken = false;

			tracker_walker = m_tracker_list;

			while ( tracker_walker != NULL_TRACKER )
			{
				if ( tracker_walker->m_change_cb != reinterpret_cast<cback_func_t>( nullptr ) )
				{
					while ( tracker_walker->Get_Next_Full_DCID( &dci_id, &attribute_bits ) )
					{
						( *tracker_walker->m_change_cb )(
							tracker_walker->m_change_cb_param, dci_id, attribute_bits );
					}
				}
				tracker_walker = tracker_walker->m_next;
			}
		}
	}

	CR_Tasker_End()
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Change_Tracker::Get_Next_Full_DCID( DCI_ID_TD* data_id,
										 attrib_mask_t* attribute_bitfield ) const
{
	DCI_ID_TD changed_id = DCI_ID_UNDEFINED;
	bool change_found = false;

	*attribute_bitfield = 0U;
	attrib_t attrib_selector;

	attrib_selector = m_attrib_scan_start;
	while ( ( attrib_selector > 0U ) && ( change_found == false ) )
	{
		attrib_selector--;
		if ( m_que[attrib_selector] != NULL_BITLIST )
		{
			if ( !m_que[attrib_selector]->Is_Empty() )
			{
				change_found = true;
				changed_id = static_cast<DCI_ID_TD>( m_que[attrib_selector]->Get_Next() );
				BF_Lib::Bit::Set( *attribute_bitfield, attrib_selector );

				while ( attrib_selector > 0U )
				{
					attrib_selector--;
					if ( m_que[attrib_selector] != NULL_BITLIST )
					{
						if ( m_que[attrib_selector]->Test( changed_id ) )
						{
							m_que[attrib_selector]->Dump( changed_id );
							BF_Lib::Bit::Set( *attribute_bitfield, attrib_selector );
						}
					}
				}
			}
		}
	}

	*data_id = changed_id;

	return ( change_found );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::Track_ID( DCI_ID_TD data_id, bool set_id_as_changed )
{
	DCI_DATA_BLOCK_TD const* data_block;
	DCI_OWNER_ATTRIB_TD owner_attrib;

	if ( data_id < DCI_TOTAL_DCIDS )
	{
		Set_Track_Bit( data_id );

		if ( set_id_as_changed == true )
		{
			data_block = DCI::Get_Data_Block( data_id );
			owner_attrib = data_block->owner_attrib_info;

			if ( m_que[RAM_ATTRIB] != NULL_BITLIST )
			{
				m_que[RAM_ATTRIB]->Set( data_id );
			}
			if ( BF_Lib::Bit::Test( owner_attrib, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
			{
				if ( m_que[NV_ATTRIB] != NULL_BITLIST )
				{
					m_que[NV_ATTRIB]->Set( data_id );
				}
			}
			if ( BF_Lib::Bit::Test( owner_attrib, DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL ) )
			{
				if ( m_que[DEFAULT_ATTRIB] != NULL_BITLIST )
				{
					m_que[DEFAULT_ATTRIB]->Set( data_id );
				}
			}
			if ( BF_Lib::Bit::Test( owner_attrib, DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL ) )
			{
				if ( m_que[MIN_ATTRIB] != NULL_BITLIST )
				{
					m_que[MIN_ATTRIB]->Set( data_id );
				}
				if ( m_que[MAX_ATTRIB] != NULL_BITLIST )
				{
					m_que[MAX_ATTRIB]->Set( data_id );
				}
			}
			if ( BF_Lib::Bit::Test( owner_attrib, DCI_OWNER_ATTRIB_INFO_RW_LENGTH ) )
			{
				if ( m_que[LENGTH_ATTRIB] != NULL_BITLIST )
				{
					m_que[LENGTH_ATTRIB]->Set( data_id );
				}
			}
			if ( BF_Lib::Bit::Test( owner_attrib, DCI_OWNER_ATTRIB_INFO_RW_ENUM_VAL ) )
			{
				if ( m_que[ENUM_ATTRIB] != NULL_BITLIST )
				{
					m_que[ENUM_ATTRIB]->Set( data_id );
				}
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::UnTrack_ID( DCI_ID_TD data_id, bool clear_pending_changes )
{
	if ( data_id < DCI_TOTAL_DCIDS )
	{
		Clr_Track_Bit( data_id );

		if ( clear_pending_changes == true )
		{
			for ( attrib_t i = 0U; i < m_attrib_scan_start; i++ )
			{
				if ( m_que[i] != NULL_BITLIST )
				{
					m_que[i]->Dump( data_id );
				}
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::UnTrack_All_IDs( bool clear_pending_changes )
{
	for ( DCI_ID_TD i = 0U; i < TOTAL_BIT_WORDS; i++ )
	{
		m_track_array[i] = 0x00U;

		if ( clear_pending_changes == true )
		{
			for ( attrib_t j = 0U; j < m_attrib_scan_start; j++ )
			{
				if ( m_que[j] != NULL_BITLIST )
				{
					m_que[j]->Dump( i );
				}
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Change_Tracker::ID_Tracked( DCI_ID_TD data_id )
{
	bool tracked = false;

	if ( data_id < DCI_TOTAL_DCIDS )
	{
		if ( Test_Track_Bit( data_id ) )
		{
			tracked = true;
		}
	}

	return ( tracked );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::ID_Change( DCI_ID_TD data_id, bool value_changed, attrib_t attribute,
								Change_Tracker const* req_tracker )
{
	Change_Tracker* tracker_walker;
	bool changed_val = false;

	if ( attribute < TOTAL_ATTRIBS )
	{
		tracker_walker = m_tracker_list;
		while ( tracker_walker != NULL_TRACKER )
		{
			if ( tracker_walker->Test_Track_Bit( data_id ) && ( req_tracker != tracker_walker ) &&
				 ( ( value_changed == true ) || ( tracker_walker->m_value_change_only == false ) ) )
			{
				if ( tracker_walker->m_que[attribute] != NULL_BITLIST )
				{
					tracker_walker->m_que[attribute]->Set( data_id );
					changed_val = true;
				}
			}
			tracker_walker = tracker_walker->m_next;
		}
		if ( changed_val == true )
		{
			if ( m_cr_queue != nullptr )
			{
				Push_TGINT();
				m_task_woken = m_cr_queue->Post_From_ISR( m_task_woken );
				Pop_TGINT();
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::Add_Tracker( Change_Tracker* new_tracker )
{
	if ( m_tracker_list == NULL_TRACKER )
	{
		m_tracker_list = new_tracker;
	}
	else
	{
		Change_Tracker* tracker_walker;

		tracker_walker = m_tracker_list;

		while ( tracker_walker->m_next != NULL_TRACKER )
		{
			tracker_walker = tracker_walker->m_next;
		}

		tracker_walker->m_next = new_tracker;
	}

	new_tracker->m_next = NULL_TRACKER;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::Remove_Tracker( Change_Tracker* old_tracker )
{
	Change_Tracker* walker;

	if ( ( m_tracker_list != NULL_TRACKER ) &&
		 ( old_tracker != NULL_TRACKER ) )
	{
		walker = m_tracker_list;

		if ( walker == old_tracker )
		{
			m_tracker_list = walker->m_next;
		}
		else
		{
			while ( ( walker->m_next != NULL_TRACKER ) &&
					( walker->m_next != old_tracker ) )
			{
				walker = walker->m_next;
			}
			if ( walker->m_next == old_tracker )
			{
				walker->m_next = walker->m_next->m_next;
			}
		}
	}
}

/*
 *****************************************************************************************
 * Get the Next available DCI ID in the Change Tracker, if any is available.
 *****************************************************************************************
 */
bool Change_Tracker::Get_Next( DCI_ID_TD* data_id, attrib_t attribute )
{
	bool change_found = false;

	*data_id = DCI_ID_UNDEFINED;

	if ( attribute < TOTAL_ATTRIBS )
	{
		if ( m_que[attribute] != NULL_BITLIST )
		{
			if ( !m_que[attribute]->Is_Empty() )
			{
				*data_id = m_que[attribute]->Get_Next();
				change_found = true;
			}
		}
	}

	return ( change_found );
}

#if 0	// Defunct this stuff.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::Clear_Change( DCI_ID_TD data_id, uint8_t attribute_bitfield ) const
{
	if ( data_id < DCI_TOTAL_DCIDS )
	{
		for ( attrib_t i = 0U; i < m_attrib_scan_start; i++ )
		{
			if ( BF_Lib::Bit::Test( attribute_bitfield, i ) )
			{
				if ( m_que[i] != NULL_BITLIST )
				{
					m_que[i]->Dump( data_id );
				}
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::Set_Change( DCI_ID_TD data_id, uint8_t attribute_bitfield ) const
{
	if ( data_id < DCI_TOTAL_DCIDS )
	{
		for ( attrib_t i = 0U; i < m_attrib_scan_start; i++ )
		{
			if ( BF_Lib::Bit::Test( attribute_bitfield, i ) )
			{
				if ( m_que[i] != NULL_BITLIST )
				{
					if ( !m_que[i]->Test( data_id ) )
					{
						m_que[i]->Set( data_id );
						Push_TGINT();
						m_task_woken = m_cr_queue->Post_From_ISR( m_task_woken );
						Pop_TGINT();
					}
				}
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::ID_Change_RAM( DCI_ID_TD data_id, bool value_changed,
									Change_Tracker const* req_tracker )
{
	ID_Change( data_id, value_changed, RAM_ATTRIB, req_tracker );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::ID_Change_Init( DCI_ID_TD data_id, bool value_changed,
									 Change_Tracker const* req_tracker )
{
	ID_Change( data_id, value_changed, NV_ATTRIB, req_tracker );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker::ID_Change_All_Other_Attributes( DCI_ID_TD data_id, bool value_changed,
													 Change_Tracker const* req_tracker )
{
	ID_Change( data_id, value_changed, DEFAULT_ATTRIB, req_tracker );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Change_Tracker::Peek_Next_RAM_ID( DCI_ID_TD* data_id )
{
	return ( Peek_Next( data_id, RAM_ATTRIB ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Change_Tracker::Peek_Next_Init_ID( DCI_ID_TD* data_id )
{
	return ( Peek_Next( data_id, NV_ATTRIB ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Change_Tracker::Peek_Next_Anything_Else_ID( DCI_ID_TD* data_id )
{
	return ( Peek_Next( data_id, DEFAULT_ATTRIB ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Change_Tracker::Get_Next_RAM_ID( DCI_ID_TD* data_id )
{
	return ( Get_Next( data_id, RAM_ATTRIB ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Change_Tracker::Get_Next_Init_ID( DCI_ID_TD* data_id )
{
	return ( Get_Next( data_id, NV_ATTRIB ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Change_Tracker::Get_Next_Anything_Else_ID( DCI_ID_TD* data_id )
{
	return ( Get_Next( data_id, DEFAULT_ATTRIB ) );
}

#endif
