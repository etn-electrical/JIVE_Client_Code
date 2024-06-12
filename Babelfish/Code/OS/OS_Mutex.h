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
#ifndef OS_MUTEX_H
#define OS_MUTEX_H

#include "OS_Queue.h"

#if CONFIG_USE_MUTEXES == 1

/**
 * @brief Timeout for releasing mutex.
 */
#define OS_MUTEX_RELEASE_TIMEOUT        0

/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define OS_MUTEX_INDEFINITE_TIMEOUT         OS_QUEUE_INDEFINITE_TIMEOUT

/**
 * @brief Class represents a mutex mutual exclusion flag.
 */
class OS_Mutex
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 */
		OS_Mutex( void );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_Mutex( void );

		/**
		 * @brief Acquires the mutex.
		 * @param ticks_to_wait: The time in ticks to wait for the semaphore to become available.
		 * A time of zero can be used to poll the semaphore.
		 * @return True if the semaphore was obtained, False if the semaphore timed out.
		 */
		inline bool Acquire( OS_TICK_TD ticks_to_wait )
		{
			return ( m_os_queue->Receive( NULL, ticks_to_wait ) );
		}

#if CONFIG_USE_ALTERNATIVE_API == 1
		/**
		 * @brief Acquires the mutex.  Uses alternative API.
		 * @param ticks_to_wait: The time in ticks to wait for the semaphore to become available.
		 * A time of zero can be used to poll the semaphore.
		 * @return True if the semaphore was obtained, False if the semaphore timed out.
		 */
		inline bool Acquire_Alternative( OS_TICK_TD ticks_to_wait )
		{
			return ( m_os_queue->ReceiveAlternative( NULL, ticks_to_wait ) );
		}

#endif	// CONFIG_USE_ALTERNATIVE_API == 1

		/**
		 * @brief Releases the mutex.
		 * @return True if the semaphore was released, False if the semaphore had an error.
		 */
		inline bool Release( void )
		{
			return ( m_os_queue->Send( NULL, OS_MUTEX_RELEASE_TIMEOUT ) );
		}

#if CONFIG_USE_ALTERNATIVE_API == 1
		/**
		 * @brief Releases the mutex. Uses alternative API.
		 * @return True if the semaphore was released, False if the semaphore had an error.
		 */
		inline bool Release_Alternative( void )
		{
			return ( m_os_queue->Send_Alternative( NULL, OS_MUTEX_RELEASE_TIMEOUT ) );
		}

#endif	// CONFIG_USE_ALTERNATIVE_API == 1

		/**
		 * @brief Releases the mutex. For use inside ISR interrupt service routines.
		 * @param higher_priority_task_woken[out]: Pointer to flag to indicate whether higher priority task was woken.
		 * @return True if the semaphore was released, False if the semaphore had an error.
		 */
		inline bool Release_From_ISR( bool* higher_priority_task_woken )
		{
			return ( m_os_queue->Send_From_ISR( NULL, higher_priority_task_woken ) );
		}

	private:
		/**
		 * @brief Pointer to queue.
		 */
		OS_Queue* m_os_queue;

};

#endif	// CONFIG_USE_MUTEXES == 1
#endif	// OS_MUTEX_H
