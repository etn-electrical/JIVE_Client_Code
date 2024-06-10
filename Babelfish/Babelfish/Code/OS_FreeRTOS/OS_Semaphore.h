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

#include "Includes.h"
#include "uC_OS_Device_Defines.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "OS_Queue.h"

/**
 * @brief Timeout for posting.
 */
#define OS_SEMAPHORE_POST_TIMEOUT       0

/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define OS_SEMAPHORE_NO_TIMEOUT     OS_QUEUE_INDEFINITE_TIMEOUT

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
			bool return_state;

			// Obtain a semaphore.
			if ( xSemaphoreTake( m_os_semaphore, ticks_to_wait ) == pdPASS )
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
		 * @brief Signals the semaphore, which awakens any task which might be pending.
		 * @return True if semaphore signaled successfully, False if errors.
		 */
		inline bool Post( void )
		{
			bool return_state;

			// Release a semaphore.
			if ( xSemaphoreGive( m_os_semaphore ) == pdPASS )
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
		 * @brief Signals the semaphore, which awakens any task which might be pending. For use inside ISR interrupt
		 * service routines.
		 * @param higher_priority_task_woken[out]: Pointer to flag to indicate whether higher priority task was woken.
		 * @return
		 */
		inline bool Post_From_ISR( BaseType_t* previously_woken )
		{
			bool return_state;

			// Release a semaphore.
			if ( xSemaphoreGiveFromISR( m_os_semaphore, previously_woken ) == pdPASS )
			{
				return_state = true;
			}
			else
			{
				return_state = false;
			}
			OS_Tasker::Yield_From_ISR( *previously_woken );
			return ( return_state );
		}

	private:
		/**
		 * Pointer to the QueueHandle_t.
		 */
		QueueHandle_t m_os_semaphore;
};


#endif
