/**
 **************************************************************************************************
 *  @file  Stream_Buffer.h
 *  @brief Stream_Buffer.h This file provides APIs to access Data Stream APIs
 *
 *	@details This class is interface layer between DSI module and third party os.
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */

#ifndef STREAM_OS_BUFFER_H
#define STREAM_OS_BUFFER_H

#include "FreeRTOS.h"
#include "Stream_OS.h"
#include "stream_buffer.h"

/* This is used for Multi-Subscriber implementation since as data from multiple publishers
 * comes in the subscriber must know how to cast the data to the correct type.
 */
#ifndef SEND_RECEIVE_DSID
	#define SEND_RECEIVE_DSID true
#endif

/**
 ****************************************************************************************
 * @brief The Stream_OS_Buffer class provides an API to the FreeRTOS stream class.
 *
 * @details The FreeRTOS stream module works on bytes and not on items.  Because of this
 * we have to essentially fake it out with the number of bytes it would take.  The
 * stream is designed for byte or word transfer.  There are certain efficiencies in the
 * way it handles the data to allow pulling from the queue as it fills.  We are using
 * some of these features and for purposes of generic solutions we are not using others.
 * For example we are overriding the trigger threshold to just set at 1.
 *
 ****************************************************************************************
 */
class Stream_OS_Buffer : public Stream_OS
{
	public:

		/**
		 * @brief Construction of stream buffer object
		 * @param[in] Buff_Size: Size of stream buffer
		 * @param[in] Trigger_Level: Trigger at which blocked task will get unblocked
		 * @return None
		 */
		Stream_OS_Buffer( size_t item_size, uint32_t item_count );

		/**
		 * @brief Destructor
		 * @details It will get called when object of stream buffer goes out of scope or deleted.
		 * @return None
		 */
		~Stream_OS_Buffer( void );

		/**
		 * @brief A pure virtual function to Send data over stream
		 * @param[in] tx_data: Data which needs to be sent
		 * @param[in] dsid: DSID of the data being sent.
		 * @return Status of stream operation
		 */
		dsi_pub_resp_t Send( const dsi_data_t tx_data, dsi_id_t dsid = DSI_UNDEFINED_ID );

		/**
		 * @brief Receive data over stream
		 * @param[in] rx_data: Space for data which is going to be received
		 * @param[in] ticks_to_wait: The maximum amount of time the task should remain in the
		 * Blocked state to wait for enough space to become available in the stream buffer
		 * @param[in] dsid: DSID of the data being received.
		 * @return Status of stream operation
		 */
		dsi_sub_resp_t Receive( dsi_data_t rx_data, uint32_t ticks_to_wait, dsi_id_t* dsid = nullptr );

		/**
		 * @brief Send data over stream
		 * @param[in] tx_data: Data which needs to be sent
		 * @param[in] context_switch_req: If context_switch_req was set to TRUE then task that has a priority above the
		 * priority of the currently executing task was unblocked and a context switch should be performed to ensure
		 * the ISR returns to the unblocked task.
		 * @param[in] dsid: DSID of the data being sent.
		 * @return Status of stream operation
		 */
		dsi_pub_resp_t Send_From_ISR( const dsi_data_t tx_data, bool* context_switch_req,
									  dsi_id_t dsid = DSI_UNDEFINED_ID );

	/**
	 * @brief MAY NOT EVER BE NEEDED:
	 * ---- Receive data over stream in an ISR.  This could cause issues if actually used and is currently not.
	 * @param[in] rx_buff: Data which needs to be sent
	 * @param[in] rx_len: Length of rx buffer. This defines maximum number of bytes which can be read out at a
	 * time.
	 * @param[in] context_switch_req: If context_switch_req was set to TRUE then task that has a priority above the
	 * priority of the currently executing task was unblocked and a context switch should be performed to ensure
	 * the ISR returns to the unblocked task.
	 * @param[in] dsid: DSID of the data being received.
	 * @return Status of stream operation
	   dsi_sub_resp_t Receive_From_ISR( void* rx_data, size_t rx_len, bool* context_switch_req, dsi_id_t* dsid = nullptr
	 *);
	 */

	private:
		/// Because this is the stream buffer from FreeRTOS it has a trigger level.
		/// If we set the trigger level to something other than 1 then the subscribers will only wake after
		/// this number of items are in the queue.  Fun but not very helpful the way we are trying to
		/// remain portable across different queue providers.
		const uint32_t DEFAULT_TRIGGER_LEVEL = 1U;
		/// We will not wait for the pub to finish since that could hold up others.
		const uint32_t DEFAULT_PUB_WAIT_TICKS = 0;
		const bool SEND_RECEIVE_DSID_IN_STREAM = SEND_RECEIVE_DSID;
		StreamBufferHandle_t m_os_stream;
		size_t m_item_size;
};


/**
 ****************************************************************************************
 * @brief The interface for creating and destroying Stream interfaces.  A central
 * distribution point for the OS based queues.
 *
 * @details It was necessary to provide a central point to get OS dependent queue
 * functionality.  This file is intended to be replaced with a different implementation
 * when a different type of queue is needed.
 *
 ****************************************************************************************
 */
class Stream_OS_Buffer_Source : public Stream_OS_Source
{
	public:

		/**
		 * @brief This constructor will create a Stream OS Source interface.
		 * The primary purpose this is to hide the OS specific creation tasks behind the queue
		 * functionality.  This specific implementation is for using the FreeRTOS streams.
		 */
		Stream_OS_Buffer_Source( void ) {}

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Stream_OS goes out of scope or deleted.
		 *  @return None
		 */
		~Stream_OS_Buffer_Source( void ) {}

		/**
		 * @brief Create an OS specific queue interface.
		 * @param[in] item_size: The size of the item which will be queued.
		 * @param[in] item_count: Number of items which can be loaded into the queue before it
		 * overflows.
		 * @return The stream control which allows send receive.
		 */
		Stream_OS* Create( size_t item_size, uint32_t item_count )
		{
			return ( new Stream_OS_Buffer( item_size, item_count ) );
		}

		/**
		 * @brief This will destroy and clear the OS specific queue
		 * @param[in] stream: Data which needs to be sent
		 * @return success of the destruction
		 */
		bool Destroy( Stream_OS* stream )
		{
			delete stream;

			return ( true );
		}

};

#endif	/* STREAM_OS_BUFFER_H */
