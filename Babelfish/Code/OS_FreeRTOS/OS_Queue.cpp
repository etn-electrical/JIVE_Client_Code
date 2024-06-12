/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "OS_Queue.h"

/*
 *****************************************************************************
 *		Constants
 *****************************************************************************
 */

/*
 *****************************************************************************
 *		Variables
 *****************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Queue::OS_Queue( BF_Lib::MBASE queue_length, BF_Lib::MBASE item_size ) :
	m_queue_length( queue_length ),
	m_os_queue( xQueueCreate( queue_length, item_size ) )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Queue::~OS_Queue( void )
{
	vQueueDelete( m_os_queue );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Queue::Send( const void* const p_item, OS_TICK_TD ticks_to_wait, bool copy_position )
{
	bool result;

	if ( queueSEND_TO_BACK == copy_position )
	{
		// This is a macro that calls xQueueGenericSend(). Post an item to
		// the back of a queue.
		if ( xQueueSendToBack( m_os_queue, p_item, ticks_to_wait ) == pdPASS )
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	else if ( queueSEND_TO_FRONT == copy_position )
	{
		// This is a macro that calls xQueueGenericSend(). Post an item to
		// the front of a queue.
		if ( xQueueSendToFront( m_os_queue, p_item, ticks_to_wait ) == pdPASS )
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/* Similar to Send, except we don't block if there is no room
   in the queue.  Also we don't directly wake a task that was blocked on a
   queue read, instead we return a flag to say whether a context switch is
   required or not (i.e. has a task with a higher priority than us been woken
   by this	post). */
bool OS_Queue::Send_From_ISR( const void* const p_item, BaseType_t* higher_priority_task_woken,
							  bool copy_position )
{
	bool return_value;

	if ( queueSEND_TO_BACK == copy_position )
	{
		// This is a macro that calls xQueueGenericSendFromISR(). Post an
		// item to the back of a queue.
		if ( xQueueSendToBackFromISR( m_os_queue, p_item, higher_priority_task_woken ) == pdPASS )
		{
			return_value = true;
		}
		else
		{
			return_value = false;
		}
		OS_Tasker::Yield_From_ISR( *higher_priority_task_woken );
	}
	else if ( queueSEND_TO_FRONT == copy_position )
	{
		// This is a macro that calls xQueueGenericSendFromISR(). Post an
		// item to the front of a queue.
		if ( xQueueSendToFrontFromISR( m_os_queue, p_item, higher_priority_task_woken ) == pdPASS )
		{
			return_value = true;
		}
		else
		{
			return_value = false;
		}
		OS_Tasker::Yield_From_ISR( *higher_priority_task_woken );
	}
	else
	{
		return_value = false;
	}

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Queue::Receive( void* const p_buffer, OS_TICK_TD ticks_to_wait )
{
	bool return_state;

	// Receive an item from a queue.
	if ( xQueueReceive( m_os_queue, p_buffer, ticks_to_wait ) == pdPASS )
	{
		return_state = true;
	}
	else
	{
		return_state = false;
	}
	return ( return_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Queue::Receive_From_ISR( void* const p_buffer, BaseType_t* task_woken )
{
	bool return_state;

	// Receive an item from a queue.
	if ( xQueueReceiveFromISR( m_os_queue, p_buffer, task_woken ) == pdPASS )
	{
		return_state = true;
	}
	else
	{
		return_state = false;
	}
	OS_Tasker::Yield_From_ISR( *task_woken );
	return ( return_state );
}
