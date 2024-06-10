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

#include "Includes.h"
#include "uC_OS_Device_Defines.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "OS_Tasker.h"

#if CONFIG_USE_MUTEXES == 1

/**
 * @brief Timeout for releasing mutex.
 */
#define OS_MUTEX_RELEASE_TIMEOUT        0

/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define OS_MUTEX_INDEFINITE_TIMEOUT     OS_QUEUE_INDEFINITE_TIMEOUT

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
			bool return_state;

			// Obtain a semaphore.
			if ( xSemaphoreTake( m_os_mutex, ticks_to_wait ) == pdPASS )
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
		 * @brief Releases the mutex.
		 * @return True if the semaphore was released, False if the semaphore had an error.
		 */
		inline bool Release( void )
		{
			bool return_state;

			// Release a semaphore.
			if ( xSemaphoreGive( m_os_mutex ) == pdPASS )
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
		 * @brief Releases the mutex. For use inside ISR interrupt service routines.
		 * @param higher_priority_task_woken[out]: Pointer to flag to indicate whether higher priority task was woken.
		 * @return True if the semaphore was released, False if the semaphore had an error.
		 */
		inline bool Release_From_ISR( BaseType_t* higher_priority_task_woken )
		{
			bool return_state;

			// Release a semaphore.
			if ( xSemaphoreGiveFromISR( m_os_mutex, higher_priority_task_woken ) == pdPASS )
			{
				return_state = true;
			}
			else
			{
				return_state = false;
			}
			OS_Tasker::Yield_From_ISR( *higher_priority_task_woken );
			return ( return_state );
		}

	private:

		/**
		 * Pointer to the QueueHandle_t.
		 */
		QueueHandle_t m_os_mutex;
};

#endif	// CONFIG_USE_MUTEXES == 1
#endif	// OS_MUTEX_H
