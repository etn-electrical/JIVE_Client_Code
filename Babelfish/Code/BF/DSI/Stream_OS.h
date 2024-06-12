/**
 **************************************************************************************************
 *  @file  Stream_OS.h
 *  @brief Stream_OS.h This file provides APIs to access Data Stream APIs
 *
 *	@details This class is interface layer between DSI module and third party os.
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */

#ifndef STREAM_OS_H
#define STREAM_OS_H

#include "DSI_Defines.h"

class Stream_OS
{
	public:
		/**
		 *  @brief Default constructor.
		 *  @details Nothing really needed here to be constructed.
		 *  @return None
		 */
		Stream_OS( void ) {}

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Stream_OS goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Stream_OS( void ) {}

		/**
		 * @brief A pure virtual function to Send data over stream
		 * @param[in] tx_data: Data which needs to be sent
		 * @param[in] dsid: DSID of the data being sent.
		 * @return Status of stream operation
		 */
		virtual dsi_pub_resp_t Send( const dsi_data_t tx_data, dsi_id_t dsid = DSI_UNDEFINED_ID ) = 0;

		/**
		 * @brief Receive data over stream
		 * @param[in] rx_data: Space for data which is going to be received
		 * @param[in] ticks_to_wait: The maximum amount of time the task should remain in the
		 * Blocked state to wait for enough space to become available in the stream buffer
		 * @param[in] dsid: DSID of the data being received.
		 * @return Status of stream operation
		 */
		virtual dsi_sub_resp_t Receive( dsi_data_t rx_data, uint32_t ticks_to_wait, dsi_id_t* dsid = nullptr ) = 0;

		/**
		 * @brief Send data over stream
		 * @param[in] tx_data: Data which needs to be sent
		 * @param[in] context_switch_req: If context_switch_req was set to TRUE then task that has a priority above the
		 * priority of the currently executing task was unblocked and a context switch should be performed to ensure
		 * the ISR returns to the unblocked task.
		 * @param[in] dsid: DSID of the data being sent.
		 * @return Status of stream operation
		 */
		virtual dsi_pub_resp_t Send_From_ISR( const dsi_data_t tx_data, bool* context_switch_req,
											  dsi_id_t dsid = DSI_UNDEFINED_ID ) = 0;

		/**
		 * @brief NOT CURRENTLY USED:  May not have any application in this area at all.
		 * ---- Receive data over stream in an ISR.
		 * @param[in] rx_data: Data which needs to be sent
		 * @param[in] context_switch_req: If context_switch_req was set to TRUE then task that has a priority above the
		 * priority of the currently executing task was unblocked and a context switch should be performed to ensure
		 * the ISR returns to the unblocked task.
		 * @param[in] dsid: DSID of the data being received.
		 * @return Status of stream operation

		   virtual dsi_sub_resp_t Receive_From_ISR( void* rx_data, bool* context_switch_req, dsi_id_t* dsid = nullptr )
		 *= 0;
		 */
};

class Stream_OS_Source
{
	public:
		/**
		 *  @brief Destructor
		 *  @details It will get called when object Stream_OS goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Stream_OS_Source( void )
		{}

		/**
		 * @brief Create an OS specific queue interface.
		 * @param[in] item_size: The size of the item which will be queued.
		 * @param[in] item_count: Number of items which can be loaded into the queue before it
		 * overflows.
		 * @return The stream control which allows send receive.
		 */
		virtual Stream_OS* Create( size_t item_size, uint32_t item_count ) = 0;

		/**
		 * @brief This will destroy and clear the OS specific queue
		 * @param[in] stream_2_del: Data which needs to be sent
		 * @return success of the destruction
		 */
		virtual bool Destroy( Stream_OS* stream ) = 0;

};



#endif	/* STREAM_OS_H */
