/**
 *****************************************************************************************
 *	@file	Stream_OS_Buffer.cpp
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Stream_OS_Buffer.h"
#include "DSI_Debug.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Stream_OS_Buffer::Stream_OS_Buffer( size_t item_size, uint32_t item_count ) :
	m_os_stream( xStreamBufferCreate( item_size * item_count, DEFAULT_TRIGGER_LEVEL ) ),
	m_item_size( item_size )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Stream_OS_Buffer::~Stream_OS_Buffer()
{
	vStreamBufferDelete( m_os_stream );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
dsi_pub_resp_t Stream_OS_Buffer::Send( const dsi_data_t tx_data, dsi_id_t dsid )
{
	dsi_pub_resp_t result = DSI_PUB_FAILURE;

	size_t bytes_sent = 0;
	size_t bytes_to_send = m_item_size;

	if ( SEND_RECEIVE_DSID_IN_STREAM )
	{
		bytes_to_send += sizeof( dsi_id_t );
	}
	if ( bytes_to_send <= xStreamBufferSpacesAvailable( m_os_stream ) )
	{
		Push_TGINT();
		if ( SEND_RECEIVE_DSID_IN_STREAM )
		{
			bytes_sent = xStreamBufferSend( m_os_stream, static_cast<const void*>( &dsid ), sizeof( dsi_id_t ),
											DEFAULT_PUB_WAIT_TICKS );
		}
		bytes_sent += xStreamBufferSend( m_os_stream, tx_data, m_item_size, DEFAULT_PUB_WAIT_TICKS );
		Pop_TGINT();

		if ( bytes_sent != bytes_to_send )
		{
			DSI_DEBUG_PRINT( DBG_MSG_ERROR, "Publisher could not write %d bytes out of %d over stream[%x]", \
							 ( m_item_size - bytes_sent ), m_item_size, m_os_stream );
		}
		else
		{
			result = NO_DSI_PUB_ERROR;
			DSI_DEBUG_PRINT( DBG_MSG_INFO, "Publisher sent %d bytes ( %d %%)", bytes_sent, \
							 ( bytes_sent * 100U / m_item_size ) );
		}
	}
	else	/* What should be done when not enough room to send? */
	{}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
dsi_pub_resp_t Stream_OS_Buffer::Send_From_ISR( const dsi_data_t tx_data, bool* context_switch_req, dsi_id_t dsid )
{
	dsi_pub_resp_t result = DSI_PUB_FAILURE;
	BaseType_t higher_prio_task_woken = pdFALSE;// Initialized to pdFALSE.

	size_t bytes_sent = 0;
	size_t bytes_to_send = m_item_size;

	if ( SEND_RECEIVE_DSID_IN_STREAM )
	{
		bytes_to_send += sizeof( dsi_id_t );
	}

	if ( bytes_to_send <= xStreamBufferSpacesAvailable( m_os_stream ) )
	{
		Push_TGINT();
		if ( SEND_RECEIVE_DSID_IN_STREAM )
		{
			bytes_sent = xStreamBufferSendFromISR( m_os_stream, static_cast<const void*>( &dsid ), sizeof( dsi_id_t ),
												   &higher_prio_task_woken );
		}
		bytes_sent += xStreamBufferSendFromISR( m_os_stream, tx_data, m_item_size, &higher_prio_task_woken );
		Pop_TGINT();

		if ( bytes_sent != bytes_to_send )
		{
			DSI_DEBUG_PRINT( DBG_MSG_ERROR, "Publisher could not write %d bytes out of %d over stream[%x]", \
							 ( m_item_size - bytes_sent ), m_item_size, m_os_stream );
		}
		else
		{
			// Update context switching flag.
			if ( ( context_switch_req != nullptr ) && ( higher_prio_task_woken == pdTRUE ) )
			{
				*context_switch_req = true;
			}
			result = NO_DSI_PUB_ERROR;
			DSI_DEBUG_PRINT( DBG_MSG_INFO, "Publisher sent %d bytes ( %d %%)", bytes_sent, \
							 ( bytes_sent * 100U / m_item_size ) );
		}
	}
	else	/* What should be done when not enough room to send? */
	{}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
dsi_sub_resp_t Stream_OS_Buffer::Receive( dsi_data_t rx_data, uint32_t ticks_to_wait, dsi_id_t* dsid )
{
	dsi_sub_resp_t result = DSI_SUB_FAILURE;
	size_t bytes_recvd = 0;
	dsi_id_t rx_dsid = DSI_UNDEFINED_ID;

	if ( SEND_RECEIVE_DSID_IN_STREAM )
	{
		bytes_recvd = xStreamBufferReceive( m_os_stream, static_cast<void*>( &rx_dsid ), sizeof( dsi_id_t ),
											ticks_to_wait );
	}
	if ( !SEND_RECEIVE_DSID_IN_STREAM || ( bytes_recvd != 0 ) )
	{
		if ( ( dsid != nullptr ) && ( rx_dsid != DSI_UNDEFINED_ID ) )
		{
			*dsid = rx_dsid;
		}
		bytes_recvd = xStreamBufferReceive( m_os_stream, rx_data, m_item_size, ticks_to_wait );

		if ( bytes_recvd == m_item_size )
		{
			result = NO_DSI_SUB_ERROR;
			DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Subscriber copied %d bytes into data[%x] over stream[%x]", \
							 bytes_recvd, rx_data, m_os_stream );
		}
		else
		{
			DSI_DEBUG_PRINT( DBG_MSG_ERROR, "Subscriber could not read one item size data over stream[%x]", \
							 m_os_stream );
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
   Stream_OS_Buffer::resp_t Stream_OS_Buffer::Receive_From_ISR( void* rx_data, bool* context_switch_req, dsi_id_t* dsid
 *)
   {
    resp_t result = true;
    BaseType_t higher_prio_task_woken = pdFALSE;// Initialized to pdFALSE.

    size_t bytes_recvd = xStreamBufferReceiveFromISR( m_stream_handle, rx_data, m_item_size, &higher_prio_task_woken );

    if( bytes_recvd != m_item_size )
    {
        result = false;
        DSI_DEBUG_PRINT( DBG_MSG_ERROR, "Subscriber could not read one item size data over stream[%x]", \
                            m_stream_handle );
    }
    else
    {
        DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Subscriber copied %d bytes into data[%x] over stream[%x]", \
                            bytes_recvd, rx_data, m_stream_handle );
        // Update context switching flag.
        if ( ( context_switch_req != nullptr ) && ( higher_prio_task_woken == pdTRUE ) )
        {
            context_switch_req = true;
        }
    }

    return ( result );
   }
 */
