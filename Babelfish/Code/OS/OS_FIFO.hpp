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
#ifndef OS_FIFO_HPP
#define OS_FIFO_HPP

#include "Ram.h"
#include "OS_Task.h"
#include "Babelfish_Assert.h"

#define SEND_TO_BACK    false
#define SEND_TO_FRONT   true

#if CONFIG_USE_MUTEXES == 1
#define m_pMutexHolder m_buffer_end
#define m_recursive_call_count  m_deque
#endif	// CONFIG_USE_MUTEXES == 1

/**
 * @brief This class represents a generic FIFO queue.
 * @details Is used for OS_Queue, OS_Binary_Semaphore, OS_Mutex,
 * OS_Recursive_Mutex and OS_Counting_Semaphore.
 * @note This class must only be used with interrupts disabled!
 * For efficiency, no protection against multitasking contentions is provided internally.
 * Use OS_Enter_Critical_Section and OS_Exit_Critical_Section macros!
 */
class OS_FIFO
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 * @param queue_length: Maximum number of items queue may contain.
		 * @param item_size: Size in bytes of each item.
		 * @param is_mutex: Flag to indicate whether fifo is used as mutex.
		 */
		OS_FIFO( BF_Lib::MBASE queue_length, BF_Lib::MBASE item_size, bool is_mutex = false )
		{
#if CONFIG_USE_MUTEXES == 1
			m_is_mutex = is_mutex;
#endif	// CONFIG_USE_MUTEXES == 1
			if ( ( queue_length > 0 ) && ( item_size > 0 ) )
			{
				m_buffer_start = ( int8_t* ) Ram::Allocate( item_size * queue_length,
															false, 0 );
			}
			else
			{
				m_buffer_start = ( int8_t* ) NULL;
			}
			m_buffer_end = m_buffer_start + item_size * queue_length;
			m_item_size = item_size;
			m_queue_length = queue_length;
			m_num_items_in_queue = 0;
			m_enque = m_buffer_start;
			m_deque = m_buffer_start;

			// implied: for mutexes, item_size == 0, queue_length == 1,
			// m_pMutexHolder and m_recursive_call_count are assigned 0
		}

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_FIFO()
		{
			if ( m_buffer_start != NULL )
			{
				Ram::De_Allocate( m_buffer_start );
			}
		}

		/**
		 * @brief Gets an item from the queue.
		 * @param pData: Pointer to buffer to receive the item.
		 * @note The pointer must indicate a buffer large enough to contain
		 * an item of the size specified at construction.
		 * @note A runtime halt exception is thrown if called when queue is empty!
		 */
		inline void Get( int8_t* pData )
		{
			BF_ASSERT( m_num_items_in_queue > 0 );
			m_num_items_in_queue--;

#if CONFIG_USE_MUTEXES == 1
			if ( !m_is_mutex )
			{
#endif	// CONFIG_USE_MUTEXES == 1
			BF_Lib::MBASE i;
			for ( i = 0; i < m_item_size; i++ )
			{
				*pData++ = *m_deque++;
			}

			if ( m_deque >= m_buffer_end )
			{
				m_deque = m_buffer_start;
			}
#if CONFIG_USE_MUTEXES == 1
		}

#endif	// CONFIG_USE_MUTEXES == 1
		}

		/**
		 * @brief Peeks into the queue.
		 * @param pData: Pointer to buffer to receive the item.
		 * @note The pointer must indicate a buffer large enough to contain
		 * an item of the size specified at construction.
		 */
		inline void Peek( int8_t* pData )
		{
#if CONFIG_USE_MUTEXES == 1
			if ( !m_is_mutex )
			{
#endif	// CONFIG_USE_MUTEXES == 1
			BF_ASSERT( m_num_items_in_queue > 0 );
			if ( m_item_size != 0 )
			{
				BF_Lib::MBASE i;
				int8_t* pTemp = m_deque;

				for ( i = 0; i < m_item_size; i++ )
				{
					*pData++ = *pTemp++;
				}
			}
#if CONFIG_USE_MUTEXES == 1
		}

#endif	// CONFIG_USE_MUTEXES == 1
		}

		/**
		 * @brief Sets now data into queue.
		 * @param pData: Pointer to data.
		 * @param copy_position: Either SEND_TO_BACK or SEND_TO_FRONT.
		 */
		inline void Set( const int8_t* pData, bool copy_position = SEND_TO_BACK )
		{
			BF_ASSERT( m_num_items_in_queue < m_queue_length );

			m_num_items_in_queue++;
#if CONFIG_USE_MUTEXES == 1
			if ( !m_is_mutex )
			{
#endif	// CONFIG_USE_MUTEXES == 1
			if ( m_item_size != 0 )
			{
				BF_Lib::MBASE i;
				if ( copy_position == SEND_TO_BACK )
				{
					for ( i = 0; i < m_item_size; i++ )
					{
						*m_enque++ = *pData++;
					}
					if ( m_enque >= m_buffer_end )
					{
						m_enque = m_buffer_start;
					}
				}
				else
				{
					pData += m_item_size - 1;
					if ( m_deque <= m_buffer_start )
					{
						m_deque = m_buffer_end;
					}

					for ( i = 0; i < m_item_size; i++ )
					{
						*( --m_deque ) = *pData--;
					}

				}
			}
#if CONFIG_USE_MUTEXES == 1
		}

#endif	// CONFIG_USE_MUTEXES == 1
		}

		/**
		 * Tests whether the queue is empty.
		 * @return True if empty.
		 */
		inline bool Is_Empty( void )
		{
			return ( m_num_items_in_queue == 0 );
		}

		/**
		 * Tests whether the queue is full.
		 * @return True if full.
		 */
		inline bool Is_Full( void )
		{
			return ( m_num_items_in_queue >= m_queue_length );
		}

		/**
		 * Gets the number of items in queue.
		 * @return
		 */
		inline BF_Lib::MBASE Items_In_Queue( void )
		{
			return ( m_num_items_in_queue );
		}

		/**
		 * Sets the number of items.
		 * @param num_items_in_queue
		 */
		inline void Set_Items_In_Queue( BF_Lib::MBASE num_items_in_queue )
		{
			m_num_items_in_queue = num_items_in_queue;
		}

#if CONFIG_USE_MUTEXES == 1
		/**
		 * Tests whether this queue represents a mutex.
		 */
		inline bool Is_Mutex( void )
		{
			return ( m_is_mutex );
		}

		/**
		 * Sets pointer to task which holds mutex.
		 */
		inline void Set_Mutex_Holder( OS_Task* task )
		{
			m_pMutexHolder = ( int8_t* ) task;
		}

		/**
		 * Gets pointer to task which holds the mutex.
		 */
		inline OS_Task* Get_Mutex_Holder( void )
		{
			return ( ( OS_Task* ) m_pMutexHolder );
		}

#endif	// CONFIG_USE_MUTEXES == 1
#if CONFIG_USE_RECURSIVE_MUTEXES == 1
		/**
		 * Increments recursion count.
		 */
		inline void Increment_Recursive_Count( void )
		{
			m_recursive_call_count++;
		}

		/**
		 * Decrements recursion count.
		 */
		inline void Decrement_Recursive_Count( void )
		{
			m_recursive_call_count--;
		}

		/**
		 * Tests recursion count.
		 */
		inline bool Is_Recursive_Count_Zero( void )
		{
			return ( m_recursive_call_count == 0 );
		}

#endif	// CONFIG_USE_RECURSIVE_MUTEXES == 1

	private:
		/**
		 * Pointer to the start of the queue buffer.
		 */
		int8_t* m_buffer_start;

		/**
		 * Pointer to the end of the queue buffer area.
		 */
		int8_t* m_buffer_end;

		/**
		 * Pointer to the enqueue location within the buffer (aka the "tail" pointer).
		 */
		int8_t* m_enque;

		/**
		 * Pointer to the dequeue location within the buffer (aka the "head" pointer).
		 */
		int8_t* m_deque;

		/**
		 * Count of items currently in the queue.
		 */
		volatile BF_Lib::MBASE m_num_items_in_queue;

		/**
		 * Maximum number of items queue may contain.
		 */
		BF_Lib::MBASE m_queue_length;

		/**
		 * Size in bytes of each item.
		 */
		BF_Lib::MBASE m_item_size;

#if CONFIG_USE_MUTEXES == 1
		/**
		 * Flag to indicate whether this queue is used as a mutex
		 */
		bool m_is_mutex;
#endif	// CONFIG_USE_MUTEXES == 1

	private:

#if CONFIG_USE_MUTEXES == 1
		/**
		 * @brief Gets the recursive call counter.
		 * @note For unit-test only!
		 */
		int32_t Get_Recursive_Call_Count( void )
		{
			return ( reinterpret_cast<int32_t>( m_recursive_call_count ) );
		}

#endif	// CONFIG_USE_MUTEXES == 1
};

#endif
