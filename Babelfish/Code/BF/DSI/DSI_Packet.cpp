/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "DSI_Packet.h"
#include "DSI_Debug.h"


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DSI_Packet::DSI_Packet( uint16_t max_packets_hdrs, cback_t cback, cback_param_t cback_param )
	: m_free( 0 ),
	m_cback_param( nullptr ),
	m_cback( nullptr ),
	m_free_list( nullptr )
{
	dsi_packet_t* iterator;

	m_free_list = reinterpret_cast<dsi_packet_t*>( DSI_Buffer::Allocate( max_packets_hdrs * sizeof( dsi_packet_t ) ) );

	if ( m_free_list != nullptr )
	{
		m_free = max_packets_hdrs;
		iterator = m_free_list;

		// Let's just go through and create a linked list from here. We know where everyone is.
		for ( uint_fast16_t i = 0; i < ( max_packets_hdrs - 1 ); i++ )
		{
			iterator[i].source = this;
			iterator[i].next = &iterator[i + 1];
		}
		// nullptr out the last one in the list.  Could circular but why?
		iterator[max_packets_hdrs - 1].next = nullptr;
		iterator[max_packets_hdrs - 1].source = this;

		m_cback = cback;
		m_cback_param = cback_param;

		DSI_DEBUG_PRINT( DBG_MSG_INFO, "DSI_Packet object created for %d packets", max_packets_hdrs );
	}
	else
	{
		// BF_Assert();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DSI_Packet::dsi_packet_t* DSI_Packet::Alloc( data_t data, data_len_t length, meta_data_t meta_data,
											 uint8_t subscriber_count )
{
	dsi_packet_t* return_packet = nullptr;

	Push_TGINT();
	if ( m_free > 0 )
	{
		return_packet = m_free_list;			// Grab the first available.
		m_free_list = return_packet->next;			// Make sure to move the free list to the next free one.
		return_packet->next = nullptr;
		return_packet->meta = meta_data;
		return_packet->data = data;
		return_packet->data_len = length;
		return_packet->use_count = subscriber_count;
		m_free--;
	}
	Pop_TGINT();

	DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Allocate one packet[%x] from DSI_Packet object[%x]", return_packet, this );
	DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Initialize packet[%x] with Data = %x, Length = %x", \
					 return_packet, data, length );
	DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Initialize packet[%x] with meta data = %x, subscriber count = %d", \
					 return_packet, meta_data, subscriber_count );

	return ( return_packet );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DSI_Packet::Dealloc( dsi_packet_t* dsi_packet )
{
	Push_TGINT();

	dsi_packet->next = m_free_list;
	m_free_list = dsi_packet;
	m_free++;

	Pop_TGINT();

	DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "De-allocate one packet[%x] from DSI_Packet object[%x]", dsi_packet, this );
	DSI_DEBUG_PRINT( DBG_MSG_DEBUG, "Total free packets = %d in DSI_Packet object[%x]", m_free, this );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DSI_Packet::Ack_Pub( dsi_packet_t* dsi_packet )
{
	Push_TGINT();
	if ( dsi_packet->use_count > 0 )
	{
		dsi_packet->use_count--;
		if ( dsi_packet->use_count == 0 )
		{
			( *( dsi_packet->source )->m_cback )( dsi_packet, dsi_packet->source->m_cback_param );
		}
	}
	Pop_TGINT();
}
