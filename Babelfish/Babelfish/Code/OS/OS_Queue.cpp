/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "OS_Tasker.h"
#include "OS_Queue.h"
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
OS_Queue::OS_Queue( BF_Lib::MBASE queue_length, BF_Lib::MBASE item_size, bool is_mutex ) :
	m_fifo( reinterpret_cast<OS_FIFO*>( nullptr ) ),
	m_send_wait_list( reinterpret_cast<OS_List*>( nullptr ) ),
	m_receive_wait_list( reinterpret_cast<OS_List*>( nullptr ) ),
	m_rx_lock_count( 0 ),
	m_tx_lock_count( 0 )
{
	// this constructor can be used to construct a generic queue,
	// a semaphone (binary or counting), a mutex, and a recursive mutex
	// for a non-semaphore, non-mutex queue, is_mutex is false,
	// queue_length and item_size are both non-zeros
	// for a semaphore (binary or counting), is_mutex is false,
	// queue_length is 1 for binary, a positive number for counting,
	// item_size is always 0
	// for a mutex (recursive or not), is_mutex is true,
	// queue length is 1, and item_size is 0
	// as a matter of fact, both types of mutexes use the same constructor

	m_fifo = new OS_FIFO( queue_length, item_size, is_mutex );

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 0-1-9, Operator '!=' always evaluates to True
	 *@n Justification: This if check is needed to check for null pointer.
	 */
	/* lint -e{948} # MISRA Deviation
	   lint -e{774} # MISRA Deviation*/
	BF_ASSERT( m_fifo != reinterpret_cast<OS_FIFO*>( nullptr ) );
	m_rx_lock_count = QUEUE_UNLOCKED;
	m_tx_lock_count = QUEUE_UNLOCKED;

	m_send_wait_list = new OS_List();

	m_receive_wait_list = new OS_List();

#if CONFIG_USE_MUTEXES == 1U
	if ( is_mutex )
	{
		Send( reinterpret_cast<void const*>( nullptr ), 0U, SEND_TO_BACK );
	}
#endif	// CONFIG_USE_MUTEXES == 1
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Queue::~OS_Queue( void )
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
bool OS_Queue::Send( const void* const pItem, OS_TICK_TD ticks_to_wait, bool copy_position )
{
	bool entry_time_set = false;
	OS_TASKER_TIME_STRUCT time_out;
	bool stay_in_while = true;
	bool result;

	while ( stay_in_while )
	{
		OS_Tasker::Enter_Critical();

		if ( !m_fifo->Is_Full() )
		{
			// if the queue is not full we can safely copy the data to it

#if CONFIG_USE_MUTEXES == 1U
			if ( m_fifo->Is_Mutex() )
			{
				OS_Tasker::PriorityDisinherit( m_fifo->Get_Mutex_Holder() );
				m_fifo->Set_Mutex_Holder( reinterpret_cast<OS_Task*>( nullptr ) );
			}
#endif	// CONFIG_USE_MUTEXES == 1
			m_fifo->Set( reinterpret_cast<const int8_t*>( pItem ), copy_position );

			// if there is a task waiting, unblock it, and yield if necessary
			if ( !m_receive_wait_list->Is_Empty() )
			{
				if ( OS_Tasker::Remove_From_Event_List( m_receive_wait_list ) )
				{
					// priority change, yield
					OS_Tasker::Yield();
				}
			}

			OS_Tasker::Exit_Critical();

			stay_in_while = false;
			result = OS_QUEUE_SUCCESS;
		}
		else
		{
			if ( ticks_to_wait == 0U )
			{
				OS_Tasker::Exit_Critical();

				stay_in_while = false;
				result = OS_QUEUE_FULL_ERROR;
			}
			else if ( entry_time_set == false )
			{
				OS_Tasker::Get_Current_Time( &time_out );
				entry_time_set = true;
			}
			else
			{
				// do nothing.
			}

		}

		if ( stay_in_while )
		{
			OS_Tasker::Exit_Critical();

			OS_Tasker::Suspend_Scheduler();

			Lock_Queue();

			if ( false == OS_Tasker::Check_For_Timeout( &time_out, &ticks_to_wait ) )
			{
				// still waiting for time out to expire
				if ( m_fifo->Is_Full() )
				{
					// blocked
					OS_Tasker::Add_To_Event_List( m_send_wait_list, ticks_to_wait );
					// locking is only used to prevent "add to event list"

					Unlock_Queue();

					if ( !OS_Tasker::Resume_Scheduler() )
					{
						OS_Tasker::Yield();
					}
				}
				else
				{
					// try again, queue is not full
					Unlock_Queue();

					OS_Tasker::Resume_Scheduler();
				}
			}
			else
			{
				// time out expired
				Unlock_Queue();
				OS_Tasker::Resume_Scheduler();

				stay_in_while = false;
				result = OS_QUEUE_FULL_ERROR;
			}
		}
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
bool OS_Queue::Send_From_ISR( const void* const pItem, bool* higher_priority_task_woken,
							  bool copy_position )
{
	bool return_value;

	asmSetInterruptMask();

	if ( !m_fifo->Is_Full() )
	{

#if CONFIG_USE_MUTEXES == 1U
		if ( m_fifo->Is_Mutex() )
		{
			OS_Tasker::PriorityDisinherit( m_fifo->Get_Mutex_Holder() );
			m_fifo->Set_Mutex_Holder( reinterpret_cast<OS_Task*>( nullptr ) );
		}
#endif	// CONFIG_USE_MUTEXES == 1
		m_fifo->Set( reinterpret_cast<const int8_t*>( pItem ), copy_position );

		if ( m_tx_lock_count == QUEUE_UNLOCKED )
		{
			if ( !m_receive_wait_list->Is_Empty() )
			{
				if ( OS_Tasker::Remove_From_Event_List( m_receive_wait_list ) )
				{
					*higher_priority_task_woken = true;
				}
			}
		}
		else
		{
			m_tx_lock_count++;
		}

		return_value = OS_QUEUE_SUCCESS;
	}
	else
	{
		return_value = OS_QUEUE_FULL_ERROR;
	}

	asmClearInterruptMask();

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Queue::Receive( void* const pBuffer, OS_TICK_TD ticks_to_wait, bool peeking )
{
	bool entry_time_set = false;
	OS_TASKER_TIME_STRUCT time_out;
	bool stay_in_while = true;
	bool result;

	while ( stay_in_while )
	{
		OS_Tasker::Enter_Critical();
		{
			if ( !m_fifo->Is_Empty() )
			{
				if ( peeking )
				{
					m_fifo->Peek( reinterpret_cast<int8_t*>( pBuffer ) );
					// we are not removing data, so give other tasks a chance
					if ( !m_receive_wait_list->Is_Empty() )
					{
						if ( OS_Tasker::Remove_From_Event_List( m_receive_wait_list ) )
						{
							OS_Tasker::Yield();
						}
					}
				}
				else// actually remove data
				{
					m_fifo->Get( reinterpret_cast<int8_t*>( pBuffer ) );

#if CONFIG_USE_MUTEXES == 1U
					if ( m_fifo->Is_Mutex() )
					{
						m_fifo->Set_Mutex_Holder( OS_Tasker::Get_Current_Task() );
					}
#endif	// CONFIG_USE_MUTEXES == 1
					// for any tasks waiting to send
					if ( !m_send_wait_list->Is_Empty() )
					{
						if ( OS_Tasker::Remove_From_Event_List( m_send_wait_list ) )
						{
							OS_Tasker::Yield();
						}
					}

				}

				OS_Tasker::Exit_Critical();
				stay_in_while = false;
				result = OS_QUEUE_SUCCESS;
			}
			else// queue empty
			{
				if ( ticks_to_wait == 0U )
				{
					OS_Tasker::Exit_Critical();

					stay_in_while = false;
					result = OS_QUEUE_EMPTY_ERROR;
				}
				else if ( entry_time_set == false )
				{
					OS_Tasker::Get_Current_Time( &time_out );
					entry_time_set = true;
				}
				else
				{
					// do nothing.
				}
			}

		}
		if ( stay_in_while )
		{
			OS_Tasker::Exit_Critical();

			OS_Tasker::Suspend_Scheduler();
			Lock_Queue();

			if ( false == OS_Tasker::Check_For_Timeout( &time_out, &ticks_to_wait ) )
			{
				// still waiting for time out to expire
				if ( m_fifo->Is_Empty() )
				{
					// blocked

#if CONFIG_USE_MUTEXES == 1U
					if ( m_fifo->Is_Mutex() )
					{
						OS_Tasker::Enter_Critical();
						OS_Tasker::PriorityInherit( m_fifo->Get_Mutex_Holder() );
						OS_Tasker::Exit_Critical();
					}
#endif	// CONFIG_USE_MUTEXES == 1
					OS_Tasker::Add_To_Event_List( m_receive_wait_list, ticks_to_wait );
					Unlock_Queue();

					if ( !OS_Tasker::Resume_Scheduler() )
					{
						OS_Tasker::Yield();
					}
				}
				else
				{
					// try again, queue is not full
					Unlock_Queue();

					OS_Tasker::Resume_Scheduler();
				}
			}
			else
			{
				// time out expired
				Unlock_Queue();
				OS_Tasker::Resume_Scheduler();

				stay_in_while = false;
				result = OS_QUEUE_EMPTY_ERROR;
			}
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Queue::Receive_From_ISR( void* const pBuffer, bool* task_woken )
{
	bool return_value;

	asmSetInterruptMask();

	if ( !m_fifo->Is_Empty() )
	{
		m_fifo->Get( reinterpret_cast<int8_t*>( pBuffer ) );

		if ( m_rx_lock_count == QUEUE_UNLOCKED )
		{
			if ( !m_send_wait_list->Is_Empty() )
			{
				if ( OS_Tasker::Remove_From_Event_List( m_send_wait_list ) )
				{
					*task_woken = true;
				}
			}

		}
		else
		{
			m_rx_lock_count++;
		}

		return_value = OS_QUEUE_SUCCESS;
	}
	else
	{
		return_value = OS_QUEUE_EMPTY_ERROR;
	}

	asmClearInterruptMask();

	return ( return_value );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/* Locking a queue does not
 * prevent an ISR from adding or removing items to the queue, but does prevent
 * an ISR from removing tasks from the queue event lists.  If an ISR finds a
 * queue is locked it will instead increment the appropriate queue lock count
 * to indicate that a task may require unblocking.  When the queue in unlocked
 * these lock counts are inspected, and the appropriate action taken.
 */
void OS_Queue::Unlock_Queue( void )
{
	OS_Tasker::Enter_Critical();

	while ( m_tx_lock_count > QUEUE_LOCKED_UNMODIFIED )
	{
		if ( !m_receive_wait_list->Is_Empty() )
		{
			if ( OS_Tasker::Remove_From_Event_List( m_receive_wait_list ) )
			{
				OS_Tasker::Yield_Missed();
			}
		}

		--m_tx_lock_count;
	}

	m_tx_lock_count = QUEUE_UNLOCKED;

	OS_Tasker::Exit_Critical();

	OS_Tasker::Enter_Critical();

	while ( m_rx_lock_count > QUEUE_LOCKED_UNMODIFIED )
	{
		if ( !m_send_wait_list->Is_Empty() )
		{
			if ( OS_Tasker::Remove_From_Event_List( m_send_wait_list ) )
			{
				OS_Tasker::Yield_Missed();
			}
		}

		--m_rx_lock_count;
	}

	m_rx_lock_count = QUEUE_UNLOCKED;

	OS_Tasker::Exit_Critical();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_USE_ALTERNATIVE_API == 1
bool OS_Queue::Send_Alternative( const void* const pItem, OS_TICK_TD ticks_to_wait,
								 bool copy_position )
{
	bool entry_time_set = false;
	bool stay_in_while = true;
	bool result;
	OS_TASKER_TIME_STRUCT time_out;

	while ( stay_in_while )
	{
		OS_Tasker::Enter_Critical();

		if ( !m_fifo->Is_Full() )
		{
			// if the queue is not full we can safely copy the data to it
#if CONFIG_USE_MUTEXES == 1U
			if ( m_fifo->Is_Mutex() )
			{
				OS_Tasker::PriorityDisinherit( m_fifo->Get_Mutex_Holder() );
				m_fifo->Set_Mutex_Holder( reinterpret_cast<OS_Task*>( nullptr ) );
			}
#endif	// CONFIG_USE_MUTEXES == 1
			m_fifo->Set( reinterpret_cast<const int8_t*>( pItem ), copy_position );

			// if there is a task waiting, unblock it, and yield if necessary
			if ( !m_receive_wait_list->Is_Empty() )
			{
				if ( OS_Tasker::Remove_From_Event_List( m_receive_wait_list ) )
				{
					OS_Tasker::Yield();
				}
			}

			OS_Tasker::Exit_Critical();

			stay_in_while = false;
			result = OS_QUEUE_SUCCESS;
		}
		else
		{
			if ( ticks_to_wait == 0U )
			{
				OS_Tasker::Exit_Critical();

				stay_in_while = false;
				result = OS_QUEUE_FULL_ERROR;
			}
			else if ( entry_time_set == false )
			{
				OS_Tasker::Get_Current_Time( &time_out );
				entry_time_set = true;
			}
			else
			{
				// do nothing.
			}
		}
		if ( stay_in_while )
		{

			OS_Tasker::Exit_Critical();

			OS_Tasker::Enter_Critical();

			if ( false == OS_Tasker::Check_For_Timeout( &time_out, &ticks_to_wait ) )
			{
				// still waiting for time out to expire
				if ( m_fifo->Is_Full() )
				{
					// blocked
					OS_Tasker::Add_To_Event_List( m_send_wait_list, ticks_to_wait );
					OS_Tasker::Yield();
				}
				OS_Tasker::Exit_Critical();
			}
			else
			{
				// time out expired
				OS_Tasker::Exit_Critical();

				stay_in_while = false;
				result = OS_QUEUE_FULL_ERROR;
			}

		}
	}
	return ( result );
}

#endif	// CONFIG_USE_ALTERNATIVE_API == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_USE_ALTERNATIVE_API == 1
bool OS_Queue::ReceiveAlternative( void* const pBuffer, OS_TICK_TD ticks_to_wait, bool peeking )
{
	bool entry_time_set = false;
	OS_TASKER_TIME_STRUCT time_out;
	bool result;
	bool stay_in_while = true;

	while ( stay_in_while )
	{
		OS_Tasker::Enter_Critical();
		{
			if ( !m_fifo->Is_Empty() )
			{
				if ( peeking )
				{
					m_fifo->Peek( reinterpret_cast<int8_t*>( pBuffer ) );
					// we are not removing data, so give other tasks a chance
					if ( !m_receive_wait_list->Is_Empty() )
					{
						if ( OS_Tasker::Remove_From_Event_List( m_receive_wait_list ) )
						{
							OS_Tasker::Yield();
						}
					}
				}
				else// actually remove data
				{
					m_fifo->Get( reinterpret_cast<int8_t*>( pBuffer ) );

#if CONFIG_USE_MUTEXES == 1U
					if ( m_fifo->Is_Mutex() )
					{
						m_fifo->Set_Mutex_Holder( OS_Tasker::Get_Current_Task() );
					}
#endif	// CONFIG_USE_MUTEXES == 1

					// for any tasks waiting to send
					if ( !m_send_wait_list->Is_Empty() )
					{
						if ( OS_Tasker::Remove_From_Event_List( m_send_wait_list ) )
						{
							OS_Tasker::Yield();
						}
					}

				}

				OS_Tasker::Exit_Critical();
				stay_in_while = false;
				result = OS_QUEUE_SUCCESS;
			}
			else// queue empty
			{
				if ( ticks_to_wait == 0U )
				{
					OS_Tasker::Exit_Critical();
					stay_in_while = false;
					result = OS_QUEUE_EMPTY_ERROR;
				}
				else if ( entry_time_set == false )
				{
					OS_Tasker::Get_Current_Time( &time_out );
					entry_time_set = true;
				}
				else
				{
					// do nothing.
				}

			}

		}
		if ( stay_in_while )
		{
			OS_Tasker::Exit_Critical();

			OS_Tasker::Enter_Critical();

			if ( false == OS_Tasker::Check_For_Timeout( &time_out, &ticks_to_wait ) )
			{
				// still waiting for time out to expire
				if ( m_fifo->Is_Empty() )
				{
					// blocked
#if CONFIG_USE_MUTEXES == 1U
					if ( m_fifo->Is_Mutex() )
					{
						OS_Tasker::Enter_Critical();
						OS_Tasker::PriorityInherit( m_fifo->Get_Mutex_Holder() );
						OS_Tasker::Exit_Critical();
					}
#endif

					OS_Tasker::Add_To_Event_List( m_receive_wait_list, ticks_to_wait );
					OS_Tasker::Yield();
				}
			}
			else
			{
				// time out expired
				OS_Tasker::Exit_Critical();
				stay_in_while = false;
				result = OS_QUEUE_EMPTY_ERROR;
			}
		}
	}
	return ( result );
}

#endif	// CONFIG_USE_ALTERNATIVE_API == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_USE_RECURSIVE_MUTEXES == 1U
bool OS_Queue::ReleaseRecursive( void )
{
	bool return_value;

	if ( m_fifo->Get_Mutex_Holder() == OS_Tasker::Get_Current_Task() )
	{
		m_fifo->Decrement_Recursive_Count();

		if ( m_fifo->Is_Recursive_Count_Zero() )
		{
			Send( reinterpret_cast<void const*>( nullptr ),
				  static_cast<OS_TICK_TD>( RECURSIVE_MUTEX_GIVE_BLOCK_TIME ), SEND_TO_BACK );
		}

		return_value = PASS;
	}
	else
	{
		// if not the holder, return false
		return_value = FAIL;
	}

	return ( return_value );
}

#endif	// CONFIG_USE_RECURSIVE_MUTEXES == 1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#if CONFIG_USE_RECURSIVE_MUTEXES == 1U

bool OS_Queue::AcquireRecursive( OS_TICK_TD ticks_to_wait )
{
	bool return_value;

	if ( m_fifo->Get_Mutex_Holder() == OS_Tasker::Get_Current_Task() )
	{
		m_fifo->Increment_Recursive_Count();

		return_value = true;
	}
	else
	{
		// may block here
		return_value = Receive( reinterpret_cast<void*>( nullptr ), ticks_to_wait, false );

		if ( return_value != false )
		{
			m_fifo->Increment_Recursive_Count();
		}
	}

	return ( return_value );
}

#endif	// CONFIG_USE_RECURSIVE_MUTEXES == 1
