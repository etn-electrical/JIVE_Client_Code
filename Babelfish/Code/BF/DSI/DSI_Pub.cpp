/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "DSI_Pub.h"
#include "DSI_Debug.h"
#include "Stream_OS.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DSI_Pub::DSI_Pub( dsi_id_t dsid ) :
	m_dsid( dsid )
{
	DSI_DEBUG_PRINT( DBG_MSG_INFO, "New publisher[%x] created for DSID %d", this, dsid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
dsi_pub_resp_t DSI_Pub::Pub( dsi_data_t data )
{
	DSI_Core::stream_xface_t* iterator;
	dsi_pub_resp_t tx_resp = DSI_PUB_FAILURE;
	bool loop_break = false;

	iterator = DSI_Core::Get_Stream_List( m_dsid );

	/// This is going to step through the different stream handles and pub to them without waiting.  This is up for
	/// debate but I think this is the right way for now.

	/// We are suspending the scheduler here.  The purpose for this is to let all the subscribers get a taste.  Then
	/// when we resume the task with the appropriate priority will get turned on.  This makes sure that a task with
	/// higher priority than the publisher AND lower priority than another subscriber will get run appropriately.
	OS_Tasker::Suspend_Scheduler();
	while ( ( iterator != nullptr ) && ( loop_break == false ) )
	{
		// For now we are just sending with a 0 of time wait so we just ship and forget it.
		// We are cycling through the different handles in the list that we are associated with.
		// Part of me would like to get rid of the length and assume it is a message more than bytes but we will need
		// to look at that a bit more.

		tx_resp = iterator->stream->Send( data, m_dsid );

		if ( tx_resp != NO_DSI_PUB_ERROR )
		{
			// At this point we are basically just saying something went wrong but not really telling what or where.
			// That will be a future issue.
			loop_break = true;

		}

		iterator = iterator->next;
	}
	OS_Tasker::Resume_Scheduler();

	return ( tx_resp );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
dsi_pub_resp_t DSI_Pub::Pub_Int( dsi_data_t data, bool* context_switch_req )
{
	DSI_Core::stream_xface_t* iterator;
	dsi_pub_resp_t tx_resp = DSI_PUB_FAILURE;
	bool loop_break = false;

	iterator = DSI_Core::Get_Stream_List( m_dsid );

	// This is going to step through the different stream handles and pub to them without waiting.  This is up for
	// debate but I think this is the right way for now.
	while ( ( iterator != nullptr ) && ( loop_break == false ) )
	{
		// For now we are just sending with a 0 of time wait so we just ship and forget it.
		// We are cycling through the different handles in the list that we are associated with.
		// Part of me would like to get rid of the length and assume it is a message more than bytes but we will need
		// to look at that a bit more.
		tx_resp = iterator->stream->Send_From_ISR( data, context_switch_req, m_dsid );

		if ( tx_resp == DSI_PUB_FAILURE )
		{
			// At this point we are basically just saying something went wrong but not really telling what or where.
			// That will be a future issue.
			loop_break = true;
		}

		iterator = iterator->next;
	}

	return ( tx_resp );
}
