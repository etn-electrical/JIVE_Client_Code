/**
 **************************************************************************************************
 *  @file
 *  @brief DSI_Core.h This file provides APIs to access Data Stream APIs
 *
 *	@details This is the central DSI list reference.  It is merely a catalog of DSID specific lists of streams.
 * The subscriber adds its stream handle to the appropriate list. Then when the publisher comes along they
 * just walk through the list calling each of the stream handles.
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */

#ifndef DSI_H
#define DSI_H

#include "Includes.h"
#include "DSI_Defines.h"
#include "Stream_OS.h"

class DSI_Core
{
	public:
		struct stream_xface_t
		{
			stream_xface_t* next;	///< This is going to link the streams attached to a specific stream interface.
			Stream_OS* stream;		///< This is the actual stream control.  It is going to be RTOS specific.
									///< Whether we use a FreeRTOS Stream or a queue or a something else.
		};

		/**
		 * @brief Constructor of centralize DSI module wants to know how big the DSID list is
		 * @param[in] num_dsids: Total number of DSID's
		 * @param[in] dsid_item_sizes: Item size of each DSID passed by application.
		 * @param[in] stream_source: A source for OS specific queue functionality.
		 * @return Centralize DSI module
		 */
		static void Init( dsi_id_t num_dsids, dsi_item_size_t const* dsid_item_sizes, Stream_OS_Source* stream_source );

		/**
		 * @brief Static function to add stream into DSI database
		 * @param[in] dsid: DSID number
		 * @param[in] stream_to_add: stream handle
		 * @return Returns TRUE if stream got added into DSI database
		 */
		static bool Add_Stream( dsi_id_t dsid, Stream_OS* stream_to_add );

		/**
		 * @brief Static function to create new stream
		 * @param[in] dsid: DSID number
		 * @param[in] total_items: Total items into one stream
		 * @return Returns stream handle
		 */
		static Stream_OS* Create_Stream( dsi_id_t dsid, dsi_item_count_t total_items );

		/**
		 * @brief Static function to remove stream from DSI database
		 * @param[in] dsid: DSID number
		 * @param[in] stream_to_remove: stream handle
		 * @return Returns TRUE if stream got removed from DSI database
		 */
		static bool Remove_Dsid_Registration( dsi_id_t dsid, Stream_OS* stream_to_remove );

		/**
		 * @brief Static function to release resources of the stream. Should only be
		 * called after all DSID registrations for the stream are removed.
		 * @param[in] stream_to_destroy: stream handle to be destroyed.
		 * @return Returns TRUE if the stream was found and destroyed
		 * */
		static bool Destroy_Stream( Stream_OS* stream_to_destroy );

		/**
		 * @brief Static function to get stream interface handle
		 * @param[in] dsid: DSID number
		 * @return Returns base address of stream interface from DSI database
		 * It still returns valid base address when specific DSID is not subscribed.
		 */
		static stream_xface_t* Get_Stream_List( dsi_id_t dsid )
		{
			return ( m_stream_xfaces[dsid] );
		}

		/**
		 * @brief Get item size of specified dsid
		 * @param[in] dsid: DSID number
		 * @return Returns item size of specified dsid
		 */
		static dsi_item_size_t Item_Size( dsi_id_t dsid )
		{
			return ( m_dsid_item_sizes[dsid] );
		}

		/**
		 * @brief Get count of total dsid's
		 * @return Returns count of total dsid's
		 */
		static dsi_id_t Total_Dsids( void )
		{
			return ( m_total_dsids );
		}

		/**
		 * @brief How many subscribers are currently attached to a DSID.
		 * @param dsid: DSID number
		 * @return Returns total number of subscribers associated with the list.
		 */
		static uint8_t Sub_Count( dsi_id_t dsid )
		{
			return ( m_subscriber_counts[dsid] );
		}

	private:
		/**
		 * Since this class is pure static we disallow the Constructor, Destructor,
		 * Copy Constructor and Copy Assignment Operator definitions.
		 */
		DSI_Core( void );
		~DSI_Core( void );
		DSI_Core( const DSI_Core& rhs );
		DSI_Core & operator =( const DSI_Core& object );

		static stream_xface_t** m_stream_xfaces;
		static dsi_id_t m_total_dsids;
		static dsi_item_size_t const* m_dsid_item_sizes;
		static Stream_OS_Source* m_stream_source;
		static uint8_t* m_subscriber_counts;
};


#endif	// CERT_STORE_NV_H
