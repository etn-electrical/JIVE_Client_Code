/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */


#include "DSI_Sub.h"
#include "Stream_OS.h"
#include "DSI_Debug.h"


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DSI_Sub::DSI_Sub( dsi_id_t dsid, dsi_item_count_t total_items ) :
	m_shared_stream( nullptr ),
	m_dsid( dsid ),
	m_dsid_list( &m_dsid ),
	m_dsid_count( 1 )
{
	Init( total_items );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DSI_Sub::DSI_Sub( dsi_id_t* dsid_list, dsi_item_count_t total_dsid_count, dsi_item_count_t max_item_count ) :
	m_shared_stream( nullptr ),
	m_dsid_list( dsid_list ),
	m_dsid_count( total_dsid_count )
{
	Init( max_item_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DSI_Sub::~DSI_Sub()
{
	for ( dsi_item_count_t dsi_index = 0; dsi_index < m_dsid_count; dsi_index++ )
	{
		DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Deleting subscriber[%x] and stream[%x] which was created for DSID %d", \
						 this, m_shared_stream, m_dsid_list[dsi_index] );
		DSI_Core::Remove_Dsid_Registration( m_dsid_list[dsi_index], m_shared_stream );
	}
	DSI_Core::Destroy_Stream( m_shared_stream );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DSI_Sub::Init( dsi_item_count_t max_item_count )
{
	dsi_id_t max_size_dsid = m_dsid_list[0];
	dsi_item_size_t max_item_size = 0;

	/// Find maximum item size and maximum number of items for a stream
	for ( dsi_item_count_t dsi_index = 0; dsi_index < m_dsid_count; dsi_index++ )
	{
		if ( DSI_Core::Item_Size( m_dsid_list[dsi_index] ) > max_item_size )
		{
			max_item_size = DSI_Core::Item_Size( m_dsid_list[dsi_index] );
			max_size_dsid = m_dsid_list[dsi_index];
		}
	}

	/// Each item of queue should have space for DSI Identifier + DSI data
	m_shared_stream = DSI_Core::Create_Stream( max_size_dsid, max_item_count );

	for ( dsi_item_count_t dsi_index = 0; dsi_index < m_dsid_count; dsi_index++ )
	{
		/* Have to not add the DSID with the largest data because it already was in
		 * the call to DSI_Core::Create_Stream.
		 */
		if ( m_dsid_list[dsi_index] != max_size_dsid )
		{
			DSI_Core::Add_Stream( m_dsid_list[dsi_index], m_shared_stream );

			DSI_DEBUG_PRINT( DBG_MSG_INFO, "New subscriber[%x] created for DSID %d", this, m_dsid_list[dsi_index] );
			DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Subscriber[%x] created stream[%x] for DSID %d", this, m_shared_stream,
							 m_dsid_list[dsi_index] );
			DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Stream[%x] created with size = %d, trigger size = %d", \
							 m_shared_stream, ( m_dsid_count * ( sizeof( dsi_id_t ) + max_item_size ) ),
							 1 );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
dsi_sub_resp_t DSI_Sub::Receive( dsi_data_t rx_data, uint32_t wait_time_ms, dsi_id_t* dsid )
{
	dsi_sub_resp_t rx_resp = NO_DSI_SUB_ERROR;

	rx_resp = m_shared_stream->Receive( rx_data, wait_time_ms, dsid );

	return ( rx_resp );
}
