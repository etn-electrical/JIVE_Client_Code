/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "BitMan.h"

// Babelfish includes
#include "Ram.h"
#include "Bit.hpp"
#include "Babelfish_Assert.h"

namespace Logic
{


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BitMan::BitMan( BitMan_Base_Structure const* base ) :
	m_dci_access( reinterpret_cast<DCI_Workers*>( NULL ) ),
	m_sink_bitlist_len( 0U ),
	m_sink_dcid_list( base->m_sink_dcid_list ),
	m_source_bitlist_len( 0U ),
	m_source_dcid_list( base->m_source_dcid_list ),
	m_sink_bitlist_map( reinterpret_cast<BITMAN_TD*>( NULL ) ),
	m_sink_dci_buffer( reinterpret_cast<uint8_t*>( NULL ) ),
	m_sink_change_mask( reinterpret_cast<uint8_t*>( NULL ) ),
	m_source_bitlist_map( reinterpret_cast<BITMAN_TD*>( NULL ) ),
	m_source_shadow( reinterpret_cast<uint8_t*>( NULL ) ),
	m_receive_buffer( reinterpret_cast<uint8_t*>( NULL ) ),
	m_source_byte_array_len( base->m_source_dcid_list_len ),
	m_sink_byte_array_len( base->m_sink_dcid_list_len )
{
	if ( ( reinterpret_cast<DCI_ID_TD*>( NULL ) == m_source_dcid_list ) ||
		 ( reinterpret_cast<DCI_ID_TD*>( NULL ) == m_sink_dcid_list ) ||
		 ( 0U == m_source_byte_array_len ) || ( 0U == m_sink_byte_array_len ) )
	{
		BF_ASSERT( false );
	}

	m_source_bitlist_len = static_cast<BITMAN_TD>( m_source_byte_array_len * BITS_IN_BYTE );
	m_sink_bitlist_len = static_cast<BITMAN_TD>( m_sink_byte_array_len * BITS_IN_BYTE );

	Initialize();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BitMan::~BitMan( void )
{
	delete m_dci_access;

	m_sink_dcid_list = reinterpret_cast<DCI_ID_TD const*>( nullptr );
	m_source_dcid_list = reinterpret_cast<DCI_ID_TD const*>( nullptr );
	Ram::De_Allocate( m_sink_bitlist_map );
	Ram::De_Allocate( m_sink_dci_buffer );
	Ram::De_Allocate( m_sink_change_mask );
	Ram::De_Allocate( m_source_bitlist_map );
	Ram::De_Allocate( m_source_shadow );
	Ram::De_Allocate( m_receive_buffer );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule:
 * @n PCLint: Warning 429: Custodial pointer change_tracker has not been freed or returned
 * @n Justification: None of the dynamically created objects are intended to be destroyed.
 */
/*lint -e{429} */
void BitMan::Initialize()
{
	Change_Tracker* change_tracker;
	uint16_t current_dcid = 0U;
	DCI_LENGTH_TD largest_dcid_size = 0U;
	DCI_LENGTH_TD temp_dcid_size;

	// Create all the buffers
	m_sink_bitlist_map = reinterpret_cast<BITMAN_TD*>( Ram::Allocate(
														   m_sink_bitlist_len *
														   sizeof( BITMAN_TD ),
														   true,
														   static_cast<uint8_t>
														   ( INVALID_BITLIST_ENTRY ) ) );
	m_source_bitlist_map = reinterpret_cast<BITMAN_TD*>( Ram::Allocate(
															 m_source_bitlist_len *
															 sizeof( BITMAN_TD ),
															 true,
															 static_cast<uint8_t>
															 ( INVALID_BITLIST_ENTRY ) ) );
	m_source_shadow = reinterpret_cast<uint8_t*>( Ram::Allocate( m_source_byte_array_len ) );

	m_sink_dci_buffer = reinterpret_cast<uint8_t*>( Ram::Allocate( m_sink_byte_array_len ) );
	m_sink_change_mask = reinterpret_cast<uint8_t*>( Ram::Allocate( m_sink_byte_array_len ) );

	if ( ( reinterpret_cast<BITMAN_TD*>( NULL ) == m_sink_bitlist_map ) ||
		 ( reinterpret_cast<BITMAN_TD*>( NULL ) == m_source_bitlist_map ) ||
		 ( reinterpret_cast<uint8_t*>( NULL ) == m_source_shadow ) ||
		 ( reinterpret_cast<uint8_t*>( NULL ) == m_sink_dci_buffer ) ||
		 ( reinterpret_cast<uint8_t*>( NULL ) == m_sink_change_mask ) )
	{
		BF_ASSERT( false );
	}

	change_tracker = new Change_Tracker( &Control_Bits_Changed_Static,
										 reinterpret_cast<Change_Tracker::cback_param_t>( this ),
										 true );

	m_dci_access = new DCI_Workers( true, true );

	// Attach change tracker for all the source DCIDs
	for ( BITMAN_TD Index = 0U; Index < m_source_byte_array_len; Index++ )
	{
		if ( current_dcid != m_source_dcid_list[Index] )
		{
			current_dcid = m_source_dcid_list[Index];
			change_tracker->Track_ID( current_dcid );
			m_dci_access->Get_Length( current_dcid, &temp_dcid_size );
			if ( temp_dcid_size > largest_dcid_size )
			{
				largest_dcid_size = temp_dcid_size;
			}
		}
	}

	current_dcid = 0U;
	// Examine the sinks to determine the largest sized DCID for sizing the receive buffer
	// Also attach change tracker to detect any writebacks
	for ( BITMAN_TD Index = 0U; Index < m_sink_byte_array_len; Index++ )
	{
		if ( current_dcid != m_sink_dcid_list[Index] )
		{
			current_dcid = m_sink_dcid_list[Index];
			change_tracker->Track_ID( current_dcid );
			m_dci_access->Get_Length( current_dcid, &temp_dcid_size );
			if ( temp_dcid_size > largest_dcid_size )
			{
				largest_dcid_size = temp_dcid_size;
			}
		}
	}

	m_receive_buffer = reinterpret_cast<uint8_t*>( Ram::Allocate( largest_dcid_size ) );

	BF_ASSERT( reinterpret_cast<uint8_t*>( NULL ) != m_receive_buffer );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Bitman_Error_t BitMan::Load_BitMap( BitMan_Profile const* profile, MAP_LOAD_MODE load_mode )
{
	bool remap;
	DCI_ACCESS_ST_TD access_struct;
	DCI_ID_TD cur_dcid;
	Bitman_Error_t error_status = NO_ERRORS;

	if ( CLEAR_EXISTING_MAP == load_mode )
	{
		ClearAllMapping();
	}

	for ( BITMAN_TD ProfMapIndex = 0U; ProfMapIndex < profile->m_map_length; ProfMapIndex++ )
	{
		if ( KEEP_EXISTING_LINKS == load_mode )
		{
			// If it's already mapped, leave it as it is, else map
			remap =
				Sink_Bit_Is_Linked( profile->m_profile_map[ProfMapIndex].m_sink_dcid_bitfield )
					?
				false : true;
		}
		else if ( OVERRIDE_EXISTING_LINKS == load_mode )
		{
			if ( Sink_Bit_Is_Linked( profile->m_profile_map[ProfMapIndex].m_sink_dcid_bitfield ) )
			{
				if ( Mapped( profile->m_profile_map[ProfMapIndex].m_source_dcid_bitfield,
							 profile->m_profile_map[ProfMapIndex].m_sink_dcid_bitfield ) )
				{
					// Already mapped, don't do anything
					remap = false;
				}
				else
				{
					// Remove the link
					UnLink( profile->m_profile_map[ProfMapIndex].m_sink_dcid_bitfield );
					remap = true;
				}
			}
			else
			{
				remap = true;
			}
		}
		else if ( MAP_NORMAL == load_mode )
		{
			// The sink bit should not be mapped to anything in this mode
			if ( Sink_Bit_Is_Linked( profile->m_profile_map[ProfMapIndex].m_sink_dcid_bitfield ) )
			{
				error_status = SINK_REMAP_ERROR;
			}
			else
			{
				remap = true;
			}
		}
		else
		{
			remap = true;
		}

		if ( remap && ( NO_ERRORS == error_status ) )
		{
			Map( profile->m_profile_map[ProfMapIndex].m_source_dcid_bitfield,
				 profile->m_profile_map[ProfMapIndex].m_sink_dcid_bitfield );
		}
	}

	if ( NO_ERRORS == error_status )
	{
		// Read all the source DCIDs and push down the sink Bitlist chains
		// Read all the source DCIDs
		for ( BITMAN_TD source_byte_index = 0U; source_byte_index < m_source_byte_array_len;
			  source_byte_index++ )
		{
			cur_dcid = m_source_dcid_list[source_byte_index];
			access_struct.data_id = cur_dcid;
			access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
			access_struct.data_access.offset = ACCESS_OFFSET_NONE;
			access_struct.command = DCI_ACCESS_GET_RAM_CMD;
			access_struct.data_access.data = &m_source_shadow[source_byte_index];
			m_dci_access->Data_Access( &access_struct );

			while ( ( m_source_dcid_list[source_byte_index + 1U] == cur_dcid ) &&
					( source_byte_index < m_source_byte_array_len ) )
			{
				source_byte_index++;
			}
		}

		// Push down all the source bytes. We are pushing everything because this could the first
		// time flush for some or all of the source DCIDs
		for ( BITMAN_TD source_byte_index = 0U; source_byte_index < m_source_byte_array_len;
			  source_byte_index++ )
		{
			Update_Sink_Bitlist( source_byte_index, m_source_shadow[source_byte_index], 0xFFU );	// Mask
																									// is
																									// set
																									// to
																									// 0xFF
																									// because
																									// we
																									// want
																									// to
																									// push
																									// down
																									// all
																									// bits
																									// in
																									// the
																									// source
		}
	}
	return ( error_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool BitMan::Sink_Bit_Is_Linked( BITMAN_TD sink_dcid_bitfield ) const
{
	return ( ( m_sink_bitlist_map[sink_dcid_bitfield] !=
			   INVALID_BITLIST_ENTRY ) ? true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::Map( BITMAN_TD source_dcid_bitfield, BITMAN_TD sink_dcid_bitfield )
{
	BITMAN_TD sink_iterator;

	if ( INVALID_BITLIST_ENTRY == m_source_bitlist_map[source_dcid_bitfield] )
	{
		BF_ASSERT( INVALID_BITLIST_ENTRY == m_sink_bitlist_map[sink_dcid_bitfield] );
		// There can be only one source for a sink dcid bitfield
		// Looks like some source dcid bitfield mapped to this
		m_source_bitlist_map[source_dcid_bitfield] = sink_dcid_bitfield;
		m_sink_bitlist_map[sink_dcid_bitfield] = sink_dcid_bitfield;
	}
	else
	{
		sink_iterator = m_source_bitlist_map[source_dcid_bitfield];
		// Navigate till the end of the list and add the entry there
		for (;;)
		{
			if ( sink_iterator > m_sink_bitlist_len )
			{
				// Sanity check to ensure that we are not wandering off into oblivion
				BF_ASSERT( false );
			}
			else if ( sink_dcid_bitfield == sink_iterator )
			{
				// Already mapped? something is wrong
				BF_ASSERT( false );
			}
			else if ( INVALID_BITLIST_ENTRY == m_sink_bitlist_map[sink_iterator] )
			{
				// The chain appears broken
				BF_ASSERT( false );
			}
			else if ( m_sink_bitlist_map[sink_iterator] == sink_iterator )
			{
				// Reached the end of the list
				// Add the new bit to the end of the list
				m_sink_bitlist_map[sink_iterator] = sink_dcid_bitfield;
				m_sink_bitlist_map[sink_dcid_bitfield] = sink_dcid_bitfield;
				break;
			}
			else
			{
				// Keep following the list
				sink_iterator = m_sink_bitlist_map[sink_iterator];
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::UnLink( BITMAN_TD sink_dcid_bitfield )
{
	bool search_source_map = true;

	// Find out if the sink Bitfield is a part of some update chain
	// Search the sink Bitlist
	for ( BITMAN_TD sink_iterator = 0U; sink_iterator < m_sink_bitlist_len; sink_iterator++ )
	{
		if ( ( sink_dcid_bitfield == m_sink_bitlist_map[sink_iterator] ) &&
			 ( sink_dcid_bitfield != sink_iterator ) )
		{
			// Sanity check
			BF_ASSERT( INVALID_BITLIST_ENTRY != m_sink_bitlist_map[sink_dcid_bitfield] );
			if ( sink_dcid_bitfield == m_sink_bitlist_map[sink_dcid_bitfield] )
			{
				m_sink_bitlist_map[sink_iterator] = sink_iterator;
			}
			else
			{
				// Bridge the chain
				m_sink_bitlist_map[sink_iterator] = m_sink_bitlist_map[sink_dcid_bitfield];
			}
			// Unlink the target node
			m_sink_bitlist_map[sink_dcid_bitfield] = INVALID_BITLIST_ENTRY;
			search_source_map = false;
			break;		// Consider removing these.
		}
	}

	if ( search_source_map )
	{
		for ( BITMAN_TD source_iterator = 0U; source_iterator < m_source_bitlist_len;
			  source_iterator++ )
		{
			if ( sink_dcid_bitfield == m_source_bitlist_map[source_iterator] )
			{
				if ( sink_dcid_bitfield != m_sink_bitlist_map[sink_dcid_bitfield] )
				{
					m_source_bitlist_map[source_iterator] =
						m_sink_bitlist_map[sink_dcid_bitfield];
				}
				else
				{
					m_source_bitlist_map[source_iterator] = INVALID_BITLIST_ENTRY;
				}
				m_sink_bitlist_map[sink_dcid_bitfield] = INVALID_BITLIST_ENTRY;
				break;		// Consider removing these.
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool BitMan::Mapped( BITMAN_TD source_dcid_bitfield, BITMAN_TD sink_dcid_bitfield ) const
{
	BITMAN_TD sink_iterator = 0U;
	bool link_found = false;
	bool continue_search = true;

	if ( sink_dcid_bitfield == m_source_bitlist_map[source_dcid_bitfield] )
	{
		link_found = true;
	}
	else if ( INVALID_BITLIST_ENTRY == m_source_bitlist_map[source_dcid_bitfield] )
	{
		link_found = false;
	}
	else
	{
		sink_iterator = m_source_bitlist_map[source_dcid_bitfield];
		// Navigate till the end of the list and add the entry there
		while ( continue_search == true )
		{
			if ( sink_iterator > m_sink_bitlist_len )
			{
				// Sanity check to ensure that we are not wandering off into oblivion
				BF_ASSERT( false );
			}
			else if ( INVALID_BITLIST_ENTRY == m_sink_bitlist_map[sink_iterator] )
			{
				// The chain appears broken
				BF_ASSERT( false );
			}
			else if ( sink_dcid_bitfield == m_sink_bitlist_map[sink_iterator] )
			{
				link_found = true;
				continue_search = false;
			}
			else if ( sink_iterator == m_sink_bitlist_map[sink_iterator] )
			{
				link_found = false;
				continue_search = false;
			}
			else
			{
				// Keep following the list
				sink_iterator = m_sink_bitlist_map[sink_iterator];
			}
		}
	}

	return ( link_found );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::Clear_Sink_Change_Mask()
{
	for ( BITMAN_TD Index = 0U; Index < m_sink_byte_array_len; Index++ )
	{
		m_sink_change_mask[Index] = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::Update_Sink_Writeback( BITMAN_TD sink_byte_offset, uint8_t change_mask )
{
	const uint8_t bit_mask = 0x01U;

	for ( uint8_t bit_offset = 0U; bit_offset < BITS_IN_BYTE; bit_offset++ )
	{
		if ( ( ( ( change_mask >> bit_offset ) & bit_mask ) != 0U ) &&
			 ( m_sink_bitlist_map[( sink_byte_offset * BITS_IN_BYTE ) + bit_offset] !=
			   INVALID_BITLIST_ENTRY ) )
		{
			if ( ( m_sink_change_mask[sink_byte_offset]
				   & static_cast<uint8_t>( bit_mask << bit_offset ) ) != 0U )
			{
				// This was just updated, need not update again. So clear the change mask bit
				m_sink_change_mask[sink_byte_offset] &=
					static_cast<uint8_t>
					( ~( static_cast<uint8_t>( bit_mask << bit_offset ) ) );
			}
			else
			{
				// Its a sink writeback. It needs to be updated
				m_sink_change_mask[sink_byte_offset] |=
					static_cast<uint8_t>( bit_mask << bit_offset );
			}
		}
		else
		{
			m_sink_change_mask[sink_byte_offset] &=
				static_cast<uint8_t>
				( ~( static_cast<uint8_t>( bit_mask << bit_offset ) ) );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::Update_Sink_Bitlist( BITMAN_TD source_byte_offset, uint8_t source_data,
								  uint8_t change_mask )
{
	const uint8_t bit_mask = 0x01U;

	for ( uint8_t bit_offset = 0U; bit_offset < BITS_IN_BYTE; bit_offset++ )
	{
		if ( ( ( ( change_mask >> bit_offset ) & bit_mask ) != 0U ) &&
			 ( m_source_bitlist_map[( source_byte_offset * BITS_IN_BYTE ) + bit_offset] !=
			   INVALID_BITLIST_ENTRY ) )
		{
			Update_Sink_Bitlist_Chain(
				static_cast<BITMAN_TD>( ( source_byte_offset * BITS_IN_BYTE ) + bit_offset ),
				( ( ( source_data >> bit_offset ) & bit_mask ) != 0U ) ? true : false );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::Update_Sink_Bit( BITMAN_TD sink_bitlist_index, bool bit_set )
{
	BITMAN_TD byte_index = sink_bitlist_index / BITS_IN_BYTE;
	uint8_t bit_index = static_cast<uint8_t>( sink_bitlist_index % BITS_IN_BYTE );
	uint8_t bit_mask = static_cast<uint8_t>
		( static_cast<uint8_t>( 0x01U ) << bit_index );

	if ( bit_set )
	{
		// Set the bit
		m_sink_dci_buffer[byte_index] |= bit_mask;
	}
	else
	{
		// Clear the bit
		m_sink_dci_buffer[byte_index] &= ~bit_mask;
	}
	// Update the sink change mask
	m_sink_change_mask[byte_index] |= bit_mask;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::Update_Sink_Bitlist_Chain( BITMAN_TD source_bitlist_index, bool bit_set )
{
	BITMAN_TD sink_iterator = m_source_bitlist_map[source_bitlist_index];

	while ( sink_iterator < m_sink_bitlist_len )
	{
		Update_Sink_Bit( sink_iterator, bit_set );
		if ( sink_iterator == m_sink_bitlist_map[sink_iterator] )
		{
			// Reached the end of the list
			sink_iterator = m_sink_bitlist_len;		// We can exit out now.  Could be a do while.
		}
		else
		{
			// Moving on to the next node in the list.
			sink_iterator = m_sink_bitlist_map[sink_iterator];
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BITMAN_TD BitMan::Flush_Sink_Dcid( BITMAN_TD sink_byte_offset )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ID_TD cur_dcid = m_sink_dcid_list[sink_byte_offset];
	BITMAN_TD byte_offset;

	// First locate the beginning of the dcid byte offset
	for (; 0U != sink_byte_offset; sink_byte_offset-- )
	{
		if ( m_sink_dcid_list[sink_byte_offset - 1U] != cur_dcid )
		{
			break;
		}
	}

	// Get the current sink DCI parameter
	access_struct.data_id = cur_dcid;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_access.data = &m_receive_buffer[0U];
	m_dci_access->Data_Access( &access_struct );

	// Walk through the change mask one byte at a time, figure out what changed and make the
	// updates in the data buffer
	byte_offset = 0U;
	for (; cur_dcid == m_sink_dcid_list[sink_byte_offset]; sink_byte_offset++ )
	{
		if ( m_sink_change_mask[sink_byte_offset] != 0U )
		{
			m_receive_buffer[byte_offset] &= ~m_sink_change_mask[sink_byte_offset];
			m_receive_buffer[byte_offset] |= ( m_sink_change_mask[sink_byte_offset]
											   & m_sink_dci_buffer[sink_byte_offset] );
		}
		byte_offset++;
	}
	// Write the parameter back to the DCI
	access_struct.data_access.length = ACCESS_LENGTH_SET_ALL;
	access_struct.command = DCI_ACCESS_SET_RAM_CMD;
	m_dci_access->Data_Access( &access_struct );

	return ( sink_byte_offset );	// point to the next dcid
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::Flush_Sink_Bitlist()
{
	BITMAN_TD Index = 0U;

	while ( Index < m_sink_byte_array_len )
	{
		if ( m_sink_change_mask[Index] != 0U )
		{
			Index = Flush_Sink_Dcid( Index );
		}
		else
		{
			Index++;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::Control_Bits_Changed( DCI_ID_TD dci_id,
								   Change_Tracker::attrib_mask_t attribute_mask )
{
	BITMAN_TD source_byte_index;
	BITMAN_TD sink_byte_index;
	BITMAN_TD data_index;
	DCI_ID_TD cur_dcid;
	uint8_t byte_change_mask;
	DCI_ACCESS_ST_TD access_struct;

	if ( BF_Lib::Bit::Test( attribute_mask, Change_Tracker::RAM_ATTRIB ) )
	{
		access_struct.data_id = dci_id;
		access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
		access_struct.data_access.offset = ACCESS_OFFSET_NONE;
		access_struct.command = DCI_ACCESS_GET_RAM_CMD;
		access_struct.data_access.data = &m_receive_buffer[0U];
		m_dci_access->Data_Access( &access_struct );

		// Clear the sink change mask before we start pushing changes to the sink bitlist
		Clear_Sink_Change_Mask();

		// Locate the dcid in the source dcid array
		for ( source_byte_index = 0U; source_byte_index < m_source_byte_array_len;
			  source_byte_index++ )
		{
			if ( dci_id == m_source_dcid_list[source_byte_index] )
			{
				break;
			}
		}
		if ( source_byte_index < m_source_byte_array_len )
		{
			cur_dcid = m_source_dcid_list[source_byte_index];

			data_index = 0U;
			while ( ( cur_dcid == m_source_dcid_list[source_byte_index] ) &&
					( source_byte_index < m_source_byte_array_len ) )
			{
				// XOR a bye of received data with what is stored in the shadow
				byte_change_mask = m_receive_buffer[data_index]
					^ m_source_shadow[source_byte_index];
				// Check if anything changed in the current byte
				if ( byte_change_mask != 0U )
				{
					// Something changed in the current byte, so update the sinks
					Update_Sink_Bitlist( source_byte_index, m_receive_buffer[data_index],
										 byte_change_mask );

					// Update the shadow with the current.
					m_source_shadow[source_byte_index] = m_receive_buffer[data_index];
				}
				data_index++;
				source_byte_index++;
			}
			// done updating the sink bitlist, now push the sink changes to the DCI
			Flush_Sink_Bitlist();
		}

		// To detect writebacks
		// Search in the Sink Bitlist
		for ( sink_byte_index = 0U; sink_byte_index < m_sink_byte_array_len; sink_byte_index++ )
		{
			if ( dci_id == m_sink_dcid_list[sink_byte_index] )
			{
				break;
			}
		}

		if ( sink_byte_index < m_sink_byte_array_len )
		{
			cur_dcid = m_sink_dcid_list[sink_byte_index];

			for ( data_index = 0U; ( cur_dcid == m_sink_dcid_list[sink_byte_index] ) &&
				  ( sink_byte_index < m_sink_byte_array_len );)
			{
				// XOR a bye of received data with what is stored in the sink dci buffer
				byte_change_mask = m_receive_buffer[data_index]
					^ m_sink_dci_buffer[sink_byte_index];
				// Check if anything changed in the current byte
				if ( byte_change_mask != 0U )
				{
					// Something changed in the current byte, so update the sinks
					Update_Sink_Writeback( sink_byte_index, byte_change_mask );
				}
				data_index++;
				sink_byte_index++;
			}
			// done updating the sink bitlist, now push the sink changes to the DCI
			Flush_Sink_Bitlist();
		}

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BitMan::ClearAllMapping()
{
	// Clear the Source bitlist
	for ( BITMAN_TD Index = 0U; Index < m_source_bitlist_len; Index++ )
	{
		m_source_bitlist_map[Index] = INVALID_BITLIST_ENTRY;
	}

	// Clear the Sink bitlist
	for ( BITMAN_TD Index = 0U; Index < m_sink_bitlist_len; Index++ )
	{
		m_sink_bitlist_map[Index] = INVALID_BITLIST_ENTRY;
	}
}

}
