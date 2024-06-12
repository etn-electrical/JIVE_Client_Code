/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OS_QUEUE_H
#define OS_QUEUE_H

#include "Includes.h"
#include "uC_OS_Device_Defines.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "OS_Tasker.h"
#include "OS_List_Item.h"

/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define OS_QUEUE_INDEFINITE_TIMEOUT     OS_TASKER_INDEFINITE_TIMEOUT

/**
 * @brief Constant indicating queue is unlocked. See OS_Queue::m_rx_lock_count and OS_Queue::m_tx_lock_count.
 */
#define QUEUE_UNLOCKED      -1

/**
 * @brief Constant indicating queue is locked. See OS_Queue::m_rx_lock_count and OS_Queue::m_tx_lock_count.
 */
#define QUEUE_LOCKED_UNMODIFIED     0

#if CONFIG_USE_RECURSIVE_MUTEXES == 1
/**
 * @brief Timeout for releasing mutex.
 */
#define RECURSIVE_MUTEX_GIVE_BLOCK_TIME     0
#endif	// CONFIG_USE_RECURSIVE_MUTEX == 1

/**
 * @brief List of status values.
 */
enum
{
	OS_QUEUE_FULL_ERROR,
	OS_QUEUE_SUCCESS = true
};

/**
 * @brief Constant indicating queue empty error.
 */
#define OS_QUEUE_EMPTY_ERROR        OS_QUEUE_FULL_ERROR

/**
 * @brief Minimum length of queue
 */
#define OS_QUEUE_MIN_LENGTH     1

/**
 * @brief Class represents a queue
 */
class OS_Queue
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 * @param queue_length: Maximum number of items queue may contain.
		 * @param item_size: Size in bytes of each item.
		 * @param is_mutex: Flag to indicate whether fifo is used as mutex.
		 */
		OS_Queue( BF_Lib::MBASE queue_length = OS_QUEUE_MIN_LENGTH,
				  BF_Lib::MBASE item_size = sizeof( BF_Lib::MBASE ) );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_Queue( void );

		/**
		 * @brief Sends an item to the queue.
		 * @param p_item: Pointer to item.
		 * @param ticks_to_wait: The time in ticks to wait for the queue to have room.
		 * A time of zero can be used to poll the queue. A value of OS_QUEUE_INDEFINITE_TIMEOUT can be used to wait
		 * forever.
		 * @param copy_position: One of SEND_TO_BACK or SEND_TO_FRONT.
		 * @note Values are passed by reference - not be value.
		 * @return True if successful, false if queue is full.
		 */
		bool Send( const void* const p_item, OS_TICK_TD ticks_to_wait,
				   bool copy_position = queueSEND_TO_BACK );

		/**
		 * @brief Sends an item to the queue. For use inside ISR interrupt service routines.
		 * @param p_item: Pointer to item.
		 * @param higher_priority_task_woken[out]: Pointer to flag to indicate whether higher priority task was woken.
		 * @param copy_position: One of SEND_TO_BACK or SEND_TO_FRONT.
		 * @return True if successful, false if queue is full.
		 */
		bool Send_From_ISR( const void* const p_item,
							BaseType_t* higher_priority_task_woken, bool copy_position = queueSEND_TO_BACK );

		/**
		 * @brief Receives an item from the queue.
		 * @param p_buffer. Pointer to buffer to receive item.
		 * @param ticks_to_wait: The time in ticks to wait for an item to arrive.
		 * @return True if successful, false if errors.
		 */
		bool Receive( void* const p_buffer, OS_TICK_TD ticks_to_wait = 0 );

		/**
		 * @brief Receives an item from the queue. For use inside ISR interrupt service routines.
		 * @param p_buffer. Pointer to buffer to receive item.
		 * @param task_woken[out]: Pointer to flag indicating higher priority task was woken.
		 * @return True if successful, false if errors.
		 */
		bool Receive_From_ISR( void* const p_buffer, BaseType_t* task_woken );

		/**
		 * @brief Gets number of items waiting in queue.
		 * @return Number of items waiting.
		 */
		inline BF_Lib::MBASE Get_Num_Messages_Waiting( void )
		{
			BF_Lib::MBASE messages_waiting;

			messages_waiting = uxQueueMessagesWaiting( m_os_queue );

			return ( messages_waiting );
		}

		/**
		 * @brief Tests whether queue is full.
		 * @return True if full, false otherwise.
		 */
		inline bool Is_Full( void )
		{
			return ( ( uxQueueMessagesWaiting( m_os_queue ) ) >= m_queue_length );
		}

		/**
		 * @brief Tests whether queue is empty.
		 * @return True if empty, false otherwise.
		 */
		inline bool Is_Empty( void )
		{
			return ( ( uxQueueMessagesWaiting( m_os_queue ) ) == 0 );
		}

		/**
		 * @brief Tests whether queue is full. For use inside ISR interrupt service routines.
		 * @return True if full, false otherwise.
		 */
		inline bool Is_Full_From_ISR( void )
		{
			bool return_state;

			if ( xQueueIsQueueFullFromISR( m_os_queue ) == pdTRUE )
			{
				return_state = true;
			}
			else
			{
				return_state = false;
			}
			return ( return_state );
		}

		/**
		 * @brief Tests whether queue is empty. For use inside ISR interrupt service routines.
		 * @return True if empty, false otherwise.
		 */
		inline bool Is_Empty_From_ISR( void )
		{
			bool return_state;

			if ( xQueueIsQueueEmptyFromISR( m_os_queue ) == pdTRUE )
			{
				return_state = true;
			}
			else
			{
				return_state = false;
			}
			return ( return_state );
		}

	private:

		/**
		 * Maximum number of items queue may contain.
		 */
		BF_Lib::MBASE m_queue_length;

		/**
		 * Pointer to the QueueHandle_t.
		 */
		QueueHandle_t m_os_queue;
};

#endif
