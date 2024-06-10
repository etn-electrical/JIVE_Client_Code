/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Stream_OS.h"
#include "DSI_Core.h"
#include "DSI_Buffer.h"
#include "DSI_Debug.h"

// Initialization of static class member
DSI_Core::stream_xface_t** DSI_Core::m_stream_xfaces = nullptr;
dsi_id_t DSI_Core::m_total_dsids = 0;
dsi_item_size_t const* DSI_Core::m_dsid_item_sizes = nullptr;
Stream_OS_Source* DSI_Core::m_stream_source = nullptr;
uint8_t* DSI_Core::m_subscriber_counts = 0;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DSI_Core::Init( dsi_id_t num_dsids, dsi_item_size_t const* dsid_item_sizes,
					 Stream_OS_Source* stream_source )
{
	m_stream_xfaces = reinterpret_cast<stream_xface_t**>(
		DSI_Buffer::Allocate( num_dsids * sizeof( stream_xface_t* ) ) );

	m_subscriber_counts = reinterpret_cast<uint8_t*>(
		DSI_Buffer::Allocate( num_dsids ) );

	for ( uint_fast8_t i = 0; i < num_dsids; i++ )
	{
		m_subscriber_counts[i] = 0;
	}

	m_total_dsids = num_dsids;
	m_dsid_item_sizes = dsid_item_sizes;
	m_stream_source = stream_source;

	DSI_DEBUG_PRINT( DBG_MSG_INFO, "DSI module contains total dsid = %d", num_dsids );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DSI_Core::Add_Stream( dsi_id_t dsid, Stream_OS* stream_to_add )
{
	bool status = true;
	stream_xface_t* new_stream_xface;

	new_stream_xface = reinterpret_cast<stream_xface_t*>( DSI_Buffer::Allocate( sizeof( stream_xface_t ) ) );
	if ( new_stream_xface != nullptr )
	{
		new_stream_xface->stream = stream_to_add;

		new_stream_xface->next = m_stream_xfaces[dsid];
		m_stream_xfaces[dsid] = new_stream_xface;

		/// Here is where we keep track of the number of subscribers in the chain.  This speeds up our checking later.
		m_subscriber_counts[dsid]++;

		DSI_DEBUG_PRINT( DBG_MSG_INFO, "Stream %x added for DSID number %d", stream_to_add, dsid );
		DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Allocated Stream interface %x", new_stream_xface );
		DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Stream %x wrapped in stream interface %x", stream_to_add, new_stream_xface );
	}
	else
	{
		status = false;		///< Do not have enough memory
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Stream_OS* DSI_Core::Create_Stream( dsi_id_t dsid, dsi_item_count_t total_items )
{
	Stream_OS* stream_os_handle;

	stream_os_handle = m_stream_source->Create( Item_Size( dsid ), total_items );

	Add_Stream( dsid, stream_os_handle );

	return ( stream_os_handle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DSI_Core::Remove_Dsid_Registration( dsi_id_t dsid, Stream_OS* stream_to_remove )
{
	bool entry_found = false;
	stream_xface_t* iterator = m_stream_xfaces[dsid];
	stream_xface_t* iterator_prev = nullptr;

	// First check for valid request
	while ( ( iterator != nullptr ) && ( entry_found == false ) )
	{
		if ( iterator->stream == stream_to_remove )
		{
			entry_found = true;	/// > Match found. Now we can remove this entry from list.
			m_subscriber_counts[dsid]--;
			// Entry found. Now we can remove it.
			if ( iterator_prev != nullptr )
			{
				iterator_prev->next = iterator->next;			/// > Removing intermediate entry
			}
			else
			{
				m_stream_xfaces[dsid] = iterator->next;		/// > Removing first entry
			}
			DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "De-allocated Stream interface %x", iterator );
			DSI_Buffer::Deallocate( iterator );				/// > Free stream interface
		}
		else
		{
			iterator_prev = iterator;
			iterator = iterator->next;
		}
	}
	return ( entry_found );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DSI_Core::Destroy_Stream( Stream_OS* stream_to_destroy )
{
	return ( m_stream_source->Destroy( stream_to_destroy ) );
}
