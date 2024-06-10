/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "CR_Queue.h"
#include "OS_Tasker.h"
#include "Babelfish_Assert.h"

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
CR_Queue::CR_Queue( uint16_t queue_length, uint8_t item_size ) :
	m_fifo( reinterpret_cast<OS_FIFO*>( nullptr ) ),
	m_send_wait_list( reinterpret_cast<OS_List*>( nullptr ) ),
	m_receive_wait_list( reinterpret_cast<OS_List*>( nullptr ) )
{
	m_fifo = new OS_FIFO( queue_length, item_size );

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 0-1-9, Operator '!=' always evaluates to True
	 *@n Justification: This if check is needed to check for null pointer.
	 */
	/* lint -e{948} # MISRA Deviation
	   lint -e{774} # MISRA Deviation*/
	BF_ASSERT( m_fifo != NULL );

	m_send_wait_list = new OS_List();

	m_receive_wait_list = new OS_List();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CR_Queue::~CR_Queue( void )
{
	delete m_fifo;
	delete m_send_wait_list;
	delete m_receive_wait_list;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CR_QUEUE_STATUS CR_Queue::Send( CR_TX_QUEUE_DATA data, OS_TICK_TD ticks_to_wait )
{
	CR_QUEUE_STATUS return_value;

	/* If the queue is already full we may have to block.  A critical section
	   is required to prevent an interrupt removing something from the queue
	   between the check to see if the queue is full and blocking on the queue. */
	Push_TGINT();

	if ( m_fifo->Is_Full() )
	{
		/* The queue is full - do we want to block or just leave without
		   posting? */
		if ( ticks_to_wait > 0U )
		{
			/* As this is called from a coroutine we cannot block directly, but
			   return indicating that we need to block. */
			CR_Tasker::Add_To_Delayed_List( ticks_to_wait, m_send_wait_list );

			return_value = CR_QUEUE_BLOCKED;
		}
		else
		{
			return_value = CR_QUEUE_FAIL;
		}
	}
	else
	{
		/* There is room in the queue, copy the data into the queue. */
		m_fifo->Set( reinterpret_cast<const int8_t*>( data ) );

		return_value = CR_QUEUE_PASS;

		/* Were any co-routines waiting for data to become available? */
		if ( !m_receive_wait_list->Is_Empty() )
		{
			/* In this instance the co-routine could be placed directly
			   into the ready list as we are within a critical section.
			   Instead the same pending ready list mechanism is used as if
			   the event were caused from within an interrupt. */
			if ( CR_Tasker::Remove_From_Event_List( m_receive_wait_list ) )
			{
				/* The co-routine waiting has a higher priority so record
				   that a yield might be appropriate. */
				return_value = CR_QUEUE_YIELD;
			}
		}
	}
	Pop_TGINT();

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CR_QUEUE_STATUS CR_Queue::Receive( CR_RX_QUEUE_DATA data, OS_TICK_TD ticks_to_wait )
{
	CR_QUEUE_STATUS return_value;

	/* If the queue is already empty we may have to block.  A critical section
	   is required to prevent an interrupt adding something to the queue
	   between the check to see if the queue is empty and blocking on the queue. */
	Push_TGINT();

	if ( m_fifo->Is_Empty() )
	{
		/* There are no messages in the queue, do we want to block or just
		   leave with nothing? */
		if ( ticks_to_wait > 0U )
		{
			/* As this is a co-routine we cannot block directly, but return
			   indicating that we need to block. */
			CR_Tasker::Add_To_Delayed_List( ticks_to_wait, m_receive_wait_list );

			return_value = CR_QUEUE_BLOCKED;
		}
		else
		{
			return_value = CR_QUEUE_FAIL;
		}
	}
	else
	{
		/* Data is available from the queue. */
		m_fifo->Get( reinterpret_cast<int8_t*>( data ) );

		return_value = CR_QUEUE_PASS;

		/* Were any co-routines waiting for space to become available? */
		if ( !m_send_wait_list->Is_Empty() )
		{
			/* In this instance the co-routine could be placed directly
			   into the ready list as we are within a critical section.
			   Instead the same pending ready list mechanism is used as if
			   the event were caused from within an interrupt. */
			if ( CR_Tasker::Remove_From_Event_List( m_send_wait_list ) != false )
			{
				return_value = CR_QUEUE_YIELD;
			}
		}
	}
	Pop_TGINT();

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CR_Queue::Send_From_ISR( CR_TX_QUEUE_DATA data, bool task_previously_woken )
{
	bool task_woken = false;

	/* Cannot block within an ISR so if there is no space on the queue then
	   exit without doing anything. */


	if ( !m_fifo->Is_Full() )
	{
		m_fifo->Set( reinterpret_cast<const int8_t*>( data ) );

		// We only want to wake one co-routine per ISR, so check that a
		// co-routine has not already been woken.
		if ( task_previously_woken == false )
		{
			if ( !m_receive_wait_list->Is_Empty() )
			{
				if ( CR_Tasker::Remove_From_Event_List( m_receive_wait_list ) != false )
				{
					task_woken = true;
				}
			}
		}
	}

	return ( task_woken );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CR_Queue::Receive_From_ISR( CR_RX_QUEUE_DATA data, bool* task_woken )
{
	bool return_value;

	// We cannot block from an ISR, so check there is data available. If
	// not then just leave without doing anything. */
	if ( !m_fifo->Is_Empty() )
	{
		// Copy the data from the queue.
		m_fifo->Get( reinterpret_cast<int8_t*>( data ) );

		if ( *task_woken == false )
		{
			if ( !m_send_wait_list->Is_Empty() )
			{
				if ( CR_Tasker::Remove_From_Event_List( m_send_wait_list ) )
				{
					*task_woken = true;
				}
			}
		}

		return_value = true;
	}
	else
	{
		return_value = false;
	}

	return ( return_value );

}
