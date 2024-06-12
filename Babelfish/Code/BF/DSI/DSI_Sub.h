/**
 **************************************************************************************************
 *  @file
 *  @brief DSI_Sub.h This file provides APIs to access Data Stream APIs
 *
 *	@details Responsible for the subscription and the receive essentially.
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef DSI_SUB_H
#define DSI_SUB_H

#include "Includes.h"
#include "DSI_Core.h"

class DSI_Sub
{
	public:
		/**
		 * @brief Constructor of DSI subscriber
		 * @param[in] dsid: DSID number
		 * @param[in] total_items: The maximum number of items that queue can hold at any one time
		 * @return DSI subscriber handle
		 */
		DSI_Sub( dsi_id_t dsid, dsi_item_count_t total_items );

		/**
		 * @brief Constructor of DSI subscriber
		 * @param[in] dsid_list: List of DSID's
		 * @param[in] total_dsid_count: Number of dsid's
		 * @param[in] item_count: List of maximum number of items that queue can hold at any one time
		 * @return DSI multiple subscriber handle
		 */
		DSI_Sub( dsi_id_t* dsid_list, dsi_item_count_t total_dsid_count, dsi_item_count_t item_count );

		/**
		 * @brief Destructor of DSI subscriber
		 * @return void
		 */
		~DSI_Sub();

		/**
		 * @brief Initialization method that gets the OS stream setup.
		 * @param[in] max_item_count: The number of items that the stream should
		 * be able to hold.
		 */
		void Init( dsi_item_count_t max_item_count );

		/**
		 * @brief Receive data from DSI subscriber
		 * @param[in] rx_data: Data will get received in this buffer
		 * @param[in] wait_time_ms: The maximum amount of time the task should remain in the
		 * Blocked state to wait for enough space to become available in the stream buffer.
		 * @param[in] dsid: The DSID to receive for.
		 * @return Returns operation status
		 */
		dsi_sub_resp_t Receive( dsi_data_t rx_data, uint32_t wait_time_ms, dsi_id_t* dsid = nullptr );

	private:
		/**
		 * Since this class is pure static we disallow the Constructor, Destructor,
		 * Copy Constructor and Copy Assignment Operator definitions.
		 */
		DSI_Sub( const DSI_Sub& rhs );
		DSI_Sub & operator =( const DSI_Sub& object );

		Stream_OS* m_shared_stream;
		dsi_id_t m_dsid;
		dsi_id_t* m_dsid_list;
		dsi_item_count_t m_dsid_count;
};


#endif	// DSI_SUB_H
