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
#ifndef OS_SEMAPHORE_H
   #define OS_SEMAPHORE_H

#include "OS_Queue.h"

/**
 * @brief Timeout for posting.
 */
#define OS_SEMAPHORE_POST_TIMEOUT       0

/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define OS_SEMAPHORE_NO_TIMEOUT         OS_QUEUE_INDEFINITE_TIMEOUT

/**
 * @brief Class represents a semaphore.
 */
class OS_Semaphore
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 */
		OS_Semaphore( bool pre_post = false );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_Semaphore( void );

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

		/**
		 * @brief Signals the semaphore, which awakens any task which might be pending.
		 * @return True if semaphore signaled successfully, False if errors.
		 */
		inline bool Post( void )
		{
			return ( m_os_queue->Send( NULL, OS_SEMAPHORE_POST_TIMEOUT ) );
		}

		/**
		 * @brief Signals the semaphore, which awakens any task which might be pending. For use inside ISR interrupt
		 *service routines.
		 * @param higher_priority_task_woken[out]: Pointer to flag to indicate whether higher priority task was woken.
		 * @return
		 */
		inline bool Post_From_ISR( bool previously_woken )
		{
			m_os_queue->Send_From_ISR( NULL, &previously_woken );
			return ( previously_woken );
		}

	private:
		/**
		 * @brief Pointer to underlying queue.
		 */
		OS_Queue* m_os_queue;

};


#endif
