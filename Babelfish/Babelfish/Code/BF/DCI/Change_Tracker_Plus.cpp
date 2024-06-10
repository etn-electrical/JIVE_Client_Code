/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Change_Tracker_Plus.h"

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
Change_Tracker_Plus::Change_Tracker_Plus( cback_func_t change_cb, cback_param_t change_cb_param,
										  bool value_change_only, attrib_mask_t attribute_track_bitfield ) :
	Change_Tracker( change_cb, change_cb_param, value_change_only, attribute_track_bitfield )
{}

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
Change_Tracker_Plus::Change_Tracker_Plus( bool value_change_only, attrib_mask_t attribute_track_bitfield ) :
	Change_Tracker( value_change_only, attribute_track_bitfield )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Change_Tracker_Plus::~Change_Tracker_Plus( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Change_Tracker_Plus::Clear_Change( DCI_ID_TD data_id, attrib_mask_t attribute_bitfield ) const
{
	if ( data_id < DCI_TOTAL_DCIDS )
	{
		for ( attrib_t i = 0U; i < m_attrib_scan_start; i++ )
		{
			if ( BF_Lib::Bit::Test( attribute_bitfield, i ) )
			{
				if ( Change_Tracker::m_que[i] != NULL_BITLIST )
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
void Change_Tracker_Plus::Set_Change( DCI_ID_TD data_id, attrib_mask_t attribute_bitfield ) const
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
						if ( m_cr_queue != nullptr )
						{
							Push_TGINT();
							m_task_woken = m_cr_queue->Post_From_ISR( m_task_woken );
							Pop_TGINT();
						}
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
bool Change_Tracker_Plus::Peek_Next( DCI_ID_TD* data_id, attrib_t attribute ) const
{
	bool change_found = false;

	*data_id = DCI_ID_UNDEFINED;

	if ( attribute < TOTAL_ATTRIBS )
	{
		if ( m_que[attribute] != NULL_BITLIST )
		{
			if ( !m_que[attribute]->Is_Empty() )
			{
				*data_id = m_que[attribute]->Peek_Next();
				change_found = true;
			}
		}
	}

	return ( change_found );
}
