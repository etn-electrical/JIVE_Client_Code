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
#ifndef OS_COUNTING_SEMAPHORE_H
#define OS_COUNTINGSEMAPHORE_H

#if CONFIG_USE_COUNTING_SEMAPHORES == 1

/**
 * @brief Timeout for posting.
 */
#define OS_COUNTING_SEMAPHORE_POST_TIMEOUT      0

/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define OS_COUNTING_SEMAPHORE_INDEFINITE_TIMEOUT            OS_QUEUE_INDEFINITE_TIMEOUT

/**
 * @brief Class represents a counting semaphore.
 */
class OS_Counting_Semaphore
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 */
		OS_Counting_Semaphore( BF_Lib::MBASE max_count, BF_Lib::MBASE initial_count = 0 );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_Counting_Semaphore( void );

		/**
		 * @brief Waits for the semaphore to become signaled.
		 * @param ticks_to_wait: The time in ticks to wait for the semaphore to become available.
		 * A time of zero can be used to poll the semaphore. OS_SEMAPHORE_NO_TIMEOUT can be used to wait indefinitely
		 *(forever).
		 * @return True if the semaphore was obtained, False if timed out prior to semaphore signaled.
		 */
		inline bool Pend( OS_TICK_TD ticks_to_wait )
		{
			return ( m_os_queue->Receive( NULL, ticks_to_wait ) );
		}

#if CONFIG_USE_ALTERNATIVE_API == 1
		/**
		 * @brief Waits for the semaphore to become signaled. Uses alternative API.
		 * @param ticks_to_wait: The time in ticks to wait for the semaphore to become available.
		 * A time of zero can be used to poll the semaphore. OS_SEMAPHORE_NO_TIMEOUT can be used to wait indefinitely
		 *(forever).
		 * @return True if the semaphore was obtained, False if timed out prior to semaphore signaled.
		 */
		inline bool Pend_Alternative( OS_TICK_TD ticks_to_wait )
		{
			return ( m_os_queue->ReceiveAlternative( NULL, ticks_to_wait ) );
		}

#endif	// CONFIG_USE_ALTERNATIVE_API == 1

		/**
		 * @brief Signals the semaphore, which awakens any task which might be pending.
		 * @return True if semaphore signaled successfully, False if errors.
		 */
		inline bool Post( void )
		{
			return ( m_os_queue->Send( NULL, OS_COUNTING_SEMAPHORE_POST_TIMEOUT ) );
		}

#if CONFIG_USE_ALTERNATIVE_API == 1
		/**
		 * @brief Signals the semaphore, which awakens any task which might be pending. Uses alternative API.
		 * @return True if semaphore signaled successfully, False if errors.
		 */
		inline bool Post_Alternative( void )
		{
			return ( m_os_queue->Send_Alternative( NULL, OS_COUNTING_SEMAPHORE_POST_TIMEOUT ) );
		}

#endif	// CONFIG_USE_ALTERNATIVE_API == 1

		/**
		 * @brief Signals the semaphore, which awakens any task which might be pending. For use inside ISR interrupt
		 *service routines.
		 * @param higher_priority_task_woken[out]: Pointer to flag to indicate whether higher priority task was woken.
		 * @return
		 */
		inline bool Post_From_ISR( bool* higher_priority_task_woken )
		{
			return ( m_os_queue->Send_From_ISR( NULL, higher_priority_task_woken ) );
		}

	private:
		/**
		 * @brief Pointer to underlying queue.
		 */
		OS_Queue* m_os_queue;

};

#endif	// CONFIG_USE_COUNTING_SEMAPHORES == 1

#endif
