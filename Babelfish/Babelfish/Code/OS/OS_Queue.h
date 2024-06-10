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

#include "OS_Tasker.h"
#include "OS_FIFO.hpp"
#include "OS_List.h"


/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define OS_QUEUE_INDEFINITE_TIMEOUT             OS_TASKER_INDEFINITE_TIMEOUT

/**
 * @brief Constant indicating queue is unlocked. See OS_Queue::m_rx_lock_count and OS_Queue::m_tx_lock_count.
 */
#define QUEUE_UNLOCKED                  -1

/**
 * @brief Constant indicating queue is locked. See OS_Queue::m_rx_lock_count and OS_Queue::m_tx_lock_count.
 */
#define QUEUE_LOCKED_UNMODIFIED         0

#if CONFIG_USE_RECURSIVE_MUTEXES == 1
/**
 * @brief Timeout for releasing mutex.
 */
#define RECURSIVE_MUTEX_GIVE_BLOCK_TIME           0
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
#define OS_QUEUE_MIN_LENGTH         1

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
				  BF_Lib::MBASE item_size = sizeof( BF_Lib::MBASE ), bool is_mutex = false );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_Queue( void );

		/**
		 * @brief Sends an item to the queue.
		 * @param pItem: Pointer to item.
		 * @param ticks_to_wait: The time in ticks to wait for the queue to have room.
		 * A time of zero can be used to poll the queue. A value of OS_QUEUE_INDEFINITE_TIMEOUT can be used to wait
		 *forever.
		 * @param copy_position: One of SEND_TO_BACK or SEND_TO_FRONT.
		 * @note Values are passed by reference - not be value.
		 * @return True if successful, false if queue is full.
		 */
		bool Send( const void* const pItem, OS_TICK_TD ticks_to_wait,
				   bool copy_position = SEND_TO_BACK );

#if CONFIG_USE_ALTERNATIVE_API == 1
		/**
		 * @brief Sends an item to the queue using alternate API.
		 * @param pItem: Pointer to item.
		 * @param ticks_to_wait: The time in ticks to wait for the queue to have room.
		 * A time of zero can be used to poll the queue. A value of OS_QUEUE_INDEFINITE_TIMEOUT can be used to wait
		 *forever.
		 * @param copy_position: One of SEND_TO_BACK or SEND_TO_FRONT.
		 * @return True if successful, false if queue is full.
		 */
		bool Send_Alternative( const void* const pItem,
							   OS_TICK_TD ticks_to_wait, bool copy_position = SEND_TO_BACK );

#endif	// CONFIG_USE_ALTERNATIVE_API == 1

		/**
		 * @brief Sends an item to the queue. For use inside ISR interrupt service routines.
		 * @param pItem: Pointer to item.
		 * @param higher_priority_task_woken[out]: Pointer to flag to indicate whether higher priority task was woken.
		 * @param copy_position: One of SEND_TO_BACK or SEND_TO_FRONT.
		 * @return True if successful, false if queue is full.
		 */
		bool Send_From_ISR( const void* const pItem,
							bool* higher_priority_task_woken, bool copy_position = SEND_TO_BACK );

#if CONFIG_USE_RECURSIVE_MUTEXES == 1
		/**
		 * @brief Releases recursive mutex.
		 * @return True if successful, false if errors.
		 */
		bool ReleaseRecursive( void );

#endif	// CONFIG_USE_RECURSIVE_MUTEXES == 1

		/**
		 * @brief Receives an item from the queue.
		 * @param pBuffer. Pointer to buffer to receive item.
		 * @param ticks_to_wait: The time in ticks to wait for an item to arrive.
		 * @param peeking: Flag to select whether to peek or remove item from queue.
		 * @return True if successful, false if errors.
		 */
		bool Receive( void* const pBuffer, OS_TICK_TD ticks_to_wait = 0,
					  bool peeking = false );

#if CONFIG_USE_ALTERNATIVE_API == 1
		/**
		 * @brief Receives an item from the queue using alternative API.
		 * @param pBuffer. Pointer to buffer to receive item.
		 * @param ticks_to_wait: The time in ticks to wait for an item to become available.
		 * A time of zero can be used to poll the queue. A value of OS_QUEUE_INDEFINITE_TIMEOUT can be used to wait
		 *forever.
		 * @param peeking: Flag to select whether to peek or remove item from queue.
		 * @return True if successful, false if errors.
		 */
		bool ReceiveAlternative( void* const pBuffer,
								 OS_TICK_TD ticks_to_wait = 0, bool peeking = false );

#endif	// CONFIG_USE_ALTERNATIVE_API == 1

		/**
		 * @brief Receives an item from the queue. For use inside ISR interrupt service routines.
		 * @param pBuffer. Pointer to buffer to receive item.
		 * @param task_woken[out]: Pointer to flag indicating higher priority task was woken.
		 * @return True if successful, false if errors.
		 */
		bool Receive_From_ISR( void* const pBuffer, bool* task_woken );

#if CONFIG_USE_RECURSIVE_MUTEXES == 1
		/**
		 * @brief Acquires the lock to the queue.
		 * @return True if successful, false if errors.
		 */
		bool AcquireRecursive( OS_TICK_TD ticks_to_wait = 0 );

#endif	// CONFIG_USE_RECURSIVE_MUTEXES == 1

		/**
		 * @brief Gets number of items waiting in queue.
		 * @return Number of items waiting.
		 */
		inline BF_Lib::MBASE Get_Num_Messages_Waiting( void )
		{
			BF_Lib::MBASE messages_waiting;

			OS_Tasker::Enter_Critical();
			messages_waiting = m_fifo->Items_In_Queue();
			OS_Tasker::Exit_Critical();

			return ( messages_waiting );
		}

		/**
		 * @brief Gets number of items waiting in queue. For use within ISR interrupt service routine.
		 * @return Number of items waiting.
		 */
		inline BF_Lib::MBASE Get_Num_Messages_Waiting_From_ISR( void )
		{
			return ( m_fifo->Items_In_Queue() );
		}

		/**
		 * @brief Sets number of items in queue.
		 * @param message_waiting: Number of items.
		 */
		inline void Set_Num_Messages_Waiting( BF_Lib::MBASE message_waiting )
		{
			OS_Tasker::Enter_Critical();
			m_fifo->Set_Items_In_Queue( message_waiting );
			OS_Tasker::Exit_Critical();

		}

		/**
		 * @brief Locks queue.
		 * @note Does nothing if already locked.
		 */
		inline void Lock_Queue( void )
		{
			OS_Tasker::Enter_Critical();

			if ( m_rx_lock_count == QUEUE_UNLOCKED )
			{
				m_rx_lock_count = QUEUE_LOCKED_UNMODIFIED;
			}
			if ( m_tx_lock_count == QUEUE_UNLOCKED )
			{
				m_tx_lock_count = QUEUE_LOCKED_UNMODIFIED;
			}

			OS_Tasker::Exit_Critical();
		}

		/**
		 * @brief Unlocks queue.
		 */
		void Unlock_Queue( void );

		/**
		 * @brief Tests whether queue is full.
		 * @return True if full, false otherwise.
		 */
		inline bool Is_Full( void )
		{
			bool is_full;

			OS_Tasker::Enter_Critical();
			is_full = m_fifo->Is_Full();
			OS_Tasker::Exit_Critical();

			return ( is_full );
		}

		/**
		 * @brief Tests whether queue is empty.
		 * @return True if empty, false otherwise.
		 */
		inline bool Is_Empty( void )
		{
			bool is_empty;

			OS_Tasker::Enter_Critical();
			is_empty = m_fifo->Is_Empty();
			OS_Tasker::Exit_Critical();

			return ( is_empty );
		}

		/**
		 * @brief Tests whether queue is full. For use inside ISR interrupt service routines.
		 * @return True if full, false otherwise.
		 */
		inline bool Is_Full_From_ISR( void )
		{
			return ( m_fifo->Is_Full() );
		}

		/**
		 * @brief Tests whether queue is empty. For use inside ISR interrupt service routines.
		 * @return True if empty, false otherwise.
		 */
		inline bool Is_Empty_From_ISR( void )
		{
			return ( m_fifo->Is_Empty() );
		}

	private:

		/**
		 * @brief Pointer to underlying FIFO.
		 */
		OS_FIFO* m_fifo;

		/**
		 * @brief List of tasks pending on send.
		 */
		OS_List* m_send_wait_list;

		/**
		 * @brief List of tasks pending on receive.
		 */
		OS_List* m_receive_wait_list;

		/**
		 * @brief Receive lock count. See constants QUEUE_UNLOCKED and QUEUE_LOCKED_MODIFIED.
		 */
		BF_Lib::MSBASE m_rx_lock_count;

		/**
		 * @brief Transmit lock count. See constants QUEUE_UNLOCKED and QUEUE_LOCKED_MODIFIED.
		 */
		BF_Lib::MSBASE m_tx_lock_count;

};

#endif
