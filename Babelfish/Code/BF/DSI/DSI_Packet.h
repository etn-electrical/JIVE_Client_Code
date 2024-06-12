/**
 **************************************************************************************************
 *  @file
 *  @brief DSI_Packet.h This file provides APIs to access Data Stream APIs
 *
 *  @details The DSI Packet class builds a packet for the larger systems.  We load a pointer to a packet header into the
 *  queue. Then the subscriber receives the pointer to the header and pulls the data out from the packet header data
 *  pointer. We may have a callback associated with this class as well. This callback could be used for errors or more
 *  importantly to tell the owner of this class that it is done.
 *
 *  @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef DSI_PACKET_H
#define DSI_PACKET_H

#include "Includes.h"
#include "DSI_Buffer.h"

class DSI_Packet
{
	public:
		typedef void* meta_data_t;
		typedef uint8_t* data_t;
		typedef uint16_t data_len_t;

		struct dsi_packet_t
		{
			dsi_packet_t* next;
			DSI_Packet* source;	// This is the pointer to the source DSI_Packet.  Essentially the owner of the packet
								// class which owns this packet.
			meta_data_t meta;	// Can be used to store time or an array of meta data.  Only for fixed length data.
								// Otherwise you have to encode the length as the first member or something.
			uint8_t use_count;	// This will be used for counting the number of subscribers and counting down every time
								// it is acked.
			data_len_t data_len;// The length of the data buffer.
			data_t data;// The pointer to data buffer.
		};

		typedef void* cback_param_t;
		typedef void (* cback_t)( dsi_packet_t* dsi_packet, cback_param_t cback_param );

		/**
		 * @brief Set the max number of packets that can be used.
		 * This would likely be a big list of available packets constructed at once.  Could look at statically building
		 * this as well.
		 * @param[in] max_packets_hdrs: The maximum number of packets that will be available.  It is necessary to make
		 * sure that the available
		 * packets are there for the possible pub overflow.
		 * @param[in] cback: This is an optional item to pass.  This callback will be used for the ack callback from the
		 * subscriber after complete.
		 * @param[in] cback_param: The parameter associated with the cback.
		 * @return The DSI_Packet pointer.
		 */
		DSI_Packet( uint16_t max_packets_hdrs, cback_t cback = nullptr, cback_param_t cback_param = nullptr );

		/**
		 * @brief This will pull a packet from the free list.
		 * @param[in] data: It is optional to assign the data on allocation.
		 * @param[in] length: It is optional to send the length on allocation.
		 * @param[in] meta_data: It is optional to send the meta_data.
		 * @param[in] subscriber_count: It is optional to send the subscriber count.
		 * @return The pointer to a packet.  If it is null then you have run out of packets.
		 */
		dsi_packet_t* Alloc( data_t data = nullptr, data_len_t length = 0, meta_data_t meta_data = nullptr,
							 uint8_t subscriber_count = 0U );

		/**
		 * @brief This is a shortcut to assigning the data pointer.
		 * @param[in] dsi_packet: the pack to load.
		 * @param[in] data: The pointer to the data buffer.
		 * @param[in] length: the length of the data buffer.
		 * @return
		 */
		void Assign_Data( dsi_packet_t* dsi_packet, data_t data, data_len_t length )
		{
			dsi_packet->data = data;
			dsi_packet->data_len = length;
		}

		/**
		 * @brief This is a shortcut to assigning meta data pointer or meta-data value.
		 * An example use:  You can load this pointer with the time of the sample.
		 * @param[in] dsi_packet: the pack to load.
		 * @param[in] meta_data: This is a method of sending meta data between pub and sub.
		 * @return
		 */
		void Assign_Meta( dsi_packet_t* dsi_packet, meta_data_t meta_data )
		{
			dsi_packet->meta = meta_data;
		}

		/**
		 * @brief This is a shortcut to assigning meta data pointer or meta-data value.
		 * An example use:  You can load this pointer with the time of the sample.
		 * @param[in] dsi_packet: the pack to load.
		 * @param[in] subscriber_count: number of subscribers of specific DSID
		 * @return
		 */
		void Assign_Subscriber_Count( dsi_packet_t* dsi_packet, uint8_t subscriber_count )
		{
			dsi_packet->use_count = subscriber_count;
		}

		/**
		 * @brief This will put the packet back into the free list.
		 * @param[in] dsi_packet_1: Destination packet.
		 * @param[in] dsi_packet_2: Source packet.
		 * @return
		 */
		void Concat( dsi_packet_t* dsi_packet_1, dsi_packet_t* dsi_packet_2 )
		{
			dsi_packet_1->next = dsi_packet_2;
		}

		/**
		 * @brief This will put the packet back into the free list.
		 * @param[in] dsi_packet: The packet that we want to deallocate.
		 * @return
		 */
		bool Dealloc( dsi_packet_t* dsi_packet );

		/**
		 * @brief This returns number of available/unused packets in the list
		 * @return number of available packets
		 */
		uint16_t Available( void )
		{
			return ( m_free );
		}

		/**
		 * @brief This is a static function which can be used to effectively ack back to the publisher that we are done
		 * with the packet.  The acker should assume that this packet AND the buffer is going to be destroyed during
		 * this process.
		 * Since the packet can be used by multiple subscribers we need to only delete this packet after it has been
		 * released by
		 * all the subscribers.  We will call back the publisher only after all subscribers have acked.
		 * @param[in] dsi_packet: The packet that we want to ack.
		 * @return
		 */
		static void Ack_Pub( dsi_packet_t* dsi_packet );

	private:
		dsi_packet_t* m_free_list;
		uint16_t m_free;
		cback_t m_cback;
		cback_param_t m_cback_param;
};


#endif	// DSI_Packet
