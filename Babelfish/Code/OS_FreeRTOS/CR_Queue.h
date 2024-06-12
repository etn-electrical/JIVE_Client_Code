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
#ifndef CR_QUEUE_H
	#define CR_QUEUE_H

#include "CR_Tasker.h"
#include "OS_Tasker.h"
#include "OS_FIFO.hpp"
#include "OS_List.h"


/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define CR_QUEUE_INDEFINITE_TIMEOUT             OS_TASKER_INDEFINITE_TIMEOUT

/**
 * List of queue status values.
 */
typedef enum
{
	CR_QUEUE_FAIL,		///< A fail is a timeout or the queue is empty with timeout 0.
	CR_QUEUE_PASS,		///< A pass means the queue is successful.
	CR_QUEUE_YIELD,		///< This is an internal state holder.
	CR_QUEUE_BLOCKED	///< This is an internal state holder.
} CR_QUEUE_STATUS;

/**
 * @brief Type of data item contained in queue.
 */
typedef void* CR_QUEUE_DATA;

/**
 * @brief Type of data item for transmit direction.
 */
typedef const void* const CR_TX_QUEUE_DATA;

/**
 * @brief Type of data item for receive direction.
 */
typedef void* const CR_RX_QUEUE_DATA;

/**
 * @brief Minimum length of queue
 */
#define CR_QUEUE_MIN_LENGTH         1

/**
 * @brief Timeout value for waiting indefinitely (forever).
 */
#define CR_QUEUE_MAX_TIMEOUT        ( ~( static_cast<OS_TICK_TD>( 0 ) ) )

/**
 * @brief Macro to send an item to a queue.
 * @param cr_task: Our coroutine task (pointer to CR_Tasker instance).
 * @param queue: The queue (pointer to CR_Queue instance).
 * @param item_to_queue: Pointer to the item (cast to void*).
 * @param ticks_to_wait: The time in ticks to wait for the queue to have room.
 * @param result: Result status of enum type CR_QUEUE_STATUS.
 * @note For use inside coroutine callback function.
 */
#define CR_Queue_Send( cr_task, queue, item_to_queue, ticks_to_wait, result )           \
	{                                                                                       \
		result = queue->Send( item_to_queue, ticks_to_wait );                               \
		if ( result == CR_QUEUE_BLOCKED )                                                    \
		{                                                                                   \
			CR_Tasker_Set_State0( cr_task );                                                \
			result = queue->Send( item_to_queue, 0U );                                      \
		}                                                                                   \
		if ( result == CR_QUEUE_YIELD )                                                      \
		{                                                                                   \
			CR_Tasker_Set_State1( cr_task );                                                \
			result = CR_QUEUE_PASS;                                                         \
		}                                                                                   \
	}

/**
 * @brief Macro to send an item to queue.
 * @param cr_task: Our coroutine task (pointer to CR_Tasker instance).
 * @param queue: The queue (pointer to CR_Queue instance).
 * @param data: Pointer to buffer to hold received item.
 * @param ticks_to_wait: The time in ticks to wait for item to arrive.
 * @param result: Result status of enum type CR_QUEUE_STATUS.
 * @note For use inside coroutine callback function.
 * @note The buffer pointed to by data must be large enough to contain an item of the
 * size specified in the constructor.
 */
#define CR_Queue_Receive( cr_task, queue, data, ticks_to_wait, result )                 \
	{                                                                                       \
		result = queue->Receive( reinterpret_cast<CR_RX_QUEUE_DATA>( data ), static_cast<OS_TICK_TD>( ticks_to_wait ) );           \
		if ( result == CR_QUEUE_BLOCKED )                                                    \
		{                                                                                   \
			CR_Tasker_Set_State0( cr_task );                                                \
			result = queue->Receive( reinterpret_cast<CR_RX_QUEUE_DATA>( data ), static_cast<OS_TICK_TD>( 0 ) );                   \
		}                                                                                   \
		if ( result == CR_QUEUE_YIELD )                                                      \
		{                                                                                   \
			CR_Tasker_Set_State1( cr_task );                                                \
			result = CR_QUEUE_PASS;                                                         \
		}                                                                                   \
	}

/**
 * @brief Macro to give SemaphoreTake like interface using queue.
 * @param cr_task: Our coroutine task (pointer to CR_Tasker instance).
 * @param queue: The queue (pointer to CR_Queue instance).
 * @param ticks_to_wait: The time in ticks to wait for item to arrive.
 * @param result: Result status of enum type CR_QUEUE_STATUS.
 * @note For use inside coroutine callback function.
 */
#define CR_Queue_Pend( cr_task, queue, ticks_to_wait, result )                 \
	CR_Queue_Receive( cr_task, queue, NULL, ticks_to_wait, result )

/**
 * @brief Macro to give semaphoreTakeFromISR like interface using queue.
 * @param queue: The queue (pointer to CR_Queue instance).
 * @param task_woken: Pointer to flag to indicate whether higher priority task was woken.
 * @param result: The result value is true if the item is received or it will be false.
 */
#define CR_Queue_Pend_From_ISR( queue, task_woken, result )         \
	{                                                               \
		result = queue->Pend_From_ISR( task_woken );                \
	}

/**
 * @brief Macro to give SemaphoreGive like interface using queue.
 * @param queue: The queue (pointer to CR_Queue instance).
 * @param ticks_to_wait: The time in ticks to wait for item to arrive.
 * @param result: Result status of enum type CR_QUEUE_STATUS.
 * @note For use inside coroutine callback function.
 */
#define CR_Queue_Post( queue, ticks_to_wait, result )                 \
	{                                                                 \
		result = queue->Send( NULL, ticks_to_wait );                  \
	}

/**
 * @brief Macro to give semaphoreGiveFromISR like interface using queue.
 * @param queue: The queue (pointer to CR_Queue instance).
 * @param task_previously_woken: Flag to indicate whether higher priority task was woken.
 * @param result: The result value is true if the item is sent or it will be false.
 */
#define CR_Queue_Post_From_ISR( queue, task_previously_woken, result )         \
	{                                                                          \
		result = queue->Post_From_ISR( task_previously_woken );                \
	}

/*
 *****************************************************************************
 *  Class Declaration
   -----------------------------------------------------------------------------
 */
class CR_Queue
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 * @param queue_length: Maximum number of items to allocate.
		 * @param item_size: Size in bytes of each item.
		 * @note Use an item size of 0 to indicate a semaphore functionality.
		 */
		CR_Queue( uint16_t queue_length = CR_QUEUE_MIN_LENGTH, uint8_t item_size = 0 );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~CR_Queue( void );

		/**
		 * @brief Sends an item to the queue.
		 * @param data: Pointer to data to be sent.
		 * @param ticks_to_wait: The time in ticks to wait for the queue to have room.
		 * @return Status of queue.
		 * @note The use the macro CR_Queue_Send() inside coroutine callback functions is strongly recommended.
		 */
		CR_QUEUE_STATUS Send( CR_TX_QUEUE_DATA data, OS_TICK_TD ticks_to_wait );

		/**
		 * @brief Sends an item to the queue. For use inside ISR interrupt service routines.
		 * @param data: Pointer to data to be sent.
		 * @param task_previously_woken[out]: Pointer to flag to indicate whether higher priority task was woken.
		 * @return Value of flag to indicate task woken status.
		 */
		bool Send_From_ISR( CR_TX_QUEUE_DATA data, bool task_previously_woken );

		/**
		 * @brief Receives an item from the queue.
		 * @param data: Pointer to data to be received.
		 * @param ticks_to_wait: The time in ticks to wait for an item to arrive.
		 * @return
		 * @note The use of macro CR_Queue_Receive() inside coroutine callback functions is strongly recommended.
		 * @note The buffer pointed to by data must be large enough to contain an item of the
		 * size specified in the constructor.
		 */
		CR_QUEUE_STATUS Receive( CR_RX_QUEUE_DATA data, OS_TICK_TD ticks_to_wait = 0 );

		/**
		 * @brief Receives an item from the queue.  For use inside ISR interrupt service routines.
		 * @param data: Pointer to data to be received.
		 * @param task_woken:  Pointer to flag to indicate whether higher priority task was woken.
		 * @return True if successful, false otherwise.
		 * @note The buffer pointed to by data must be large enough to contain an item of the
		 * size specified in the constructor.
		 */
		bool Receive_From_ISR( CR_RX_QUEUE_DATA data, bool* task_woken );

		// TODO clean up crufty comments -> refactor to doxygen comments or omit.
		// Semaphore behavior follows.  Please NOTE:  The ticks to wait should be set to zero
		// if there is a potential to overfill the queue AND you are not using the macros above
		// to call this.  Ideally you would not access these functions ever but due to the need
		// to "send" something outside a Coroutine sometimes you may need to use this.

		/**
		 * @brief Post to queue as semaphore.
		 * @param ticks_to_wait
		 * @return True if successful, false otherwise.
		 * @note Post API should be called within the CR_Task scope.
		 */
		CR_QUEUE_STATUS Post( OS_TICK_TD ticks_to_wait = CR_QUEUE_MAX_TIMEOUT )
		{
			return ( Send( NULL, ticks_to_wait ) );
		}

		/**
		 * @brief Post to queue as semaphore. For use inside ISR interrupt service routines.
		 * @param task_previously_woken[out]: Pointer to flag to indicate whether higher priority task was woken.
		 * @return True if successful, false otherwise.
		 */
		bool Post_From_ISR( bool task_previously_woken )
		{
			return ( Send_From_ISR( NULL, task_previously_woken ) );
		}

		/**
		 * @brief Pend on queue as semaphore.
		 * @param ticks_to_wait
		 * @return True if successful, false otherwise.
		 * @note Pend API should be called within the CR_Task scope.
		 */
		CR_QUEUE_STATUS Pend( OS_TICK_TD ticks_to_wait = CR_QUEUE_MAX_TIMEOUT )
		{
			return ( Receive( NULL, ticks_to_wait ) );
		}

		/**
		 * @brief Pend on queue as semaphore. For use inside ISR interrupt service routines.
		 * @param task_woken:  Pointer to flag to indicate whether higher priority task was woken.
		 * @return True if successful, false otherwise.
		 */
		bool Pend_From_ISR( bool* task_woken )
		{
			return ( Receive_From_ISR( NULL, task_woken ) );
		}

	private:
		/**
		 * @brief Pointer to underlying FIFO.
		 */
		OS_FIFO* m_fifo;

		/**
		 * @brief Pointer to list of tasks pending on sending.
		 */
		OS_List* m_send_wait_list;

		/**
		 * @brief Pointer to list of tasks pending on receiving.
		 */
		OS_List* m_receive_wait_list;
};

#endif
