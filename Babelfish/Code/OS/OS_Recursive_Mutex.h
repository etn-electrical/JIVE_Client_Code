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
#ifndef OS_RECURSIVE_MUTEX_H
#define OS_RECURSIVE_MUTEX_H

#if CONFIG_USE_RECURSIVE_MUTEXES == 1

/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define OS_RECURSIVE_MUTEX_INDEFINITE_TIMEOUT           OS_QUEUE_INDEFINITE_TIMEOUT

/**
 * @brief Class represents a recursive mutex mutual exclusion flag.
 */
class OS_Recursive_Mutex
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 */
		OS_Recursive_Mutex( void );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_Recursive_Mutex( void );

		/**
		 * @brief Acquires the mutex.
		 * @param ticks_to_wait: The time in ticks to wait for the semaphore to become available.
		 * A time of zero can be used to poll the semaphore.
		 * @return True if the semaphore was obtained, False if the semaphore timed out.
		 */
		inline bool Acquire( OS_TICK_TD ticks_to_wait )
		{
			return ( m_os_queue->AcquireRecursive( ticks_to_wait ) );
		}

		/**
		 * @brief Releases the mutex.
		 * @return True if the semaphore was released, False if the semaphore had an error.
		 */
		inline bool Release( void )
		{
			return ( m_os_queue->ReleaseRecursive() );
		}

	private:
		/**
		 * @brief Pointer to queue.
		 */
		OS_Queue* m_os_queue;

};

#endif	// CONFIG_USE_RECURSIVE_MUTEXES == 1

#endif	// OS_RECURSIVE_MUTEX_H
