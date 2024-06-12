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
#include "OS_Counting_Semaphore.h"
#if CONFIG_USE_COUNTING_SEMAPHORES == 1U

/*
 *****************************************************************************
 *		Constants
 *****************************************************************************
 */
#define OS_COUNTING_SEMAPHORE_ITEM_SIZE         0U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Counting_Semaphore::OS_Counting_Semaphore( BF_Lib::MBASE max_count,
											  BF_Lib::MBASE initial_count ) :
	m_os_queue( reinterpret_cast<OS_Queue*>( nullptr ) )
{
	m_os_queue = new OS_Queue( max_count,
							   OS_COUNTING_SEMAPHORE_ITEM_SIZE, false );

	m_os_queue->Set_Num_Messages_Waiting( initial_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Counting_Semaphore::~OS_Counting_Semaphore( void )
{
	delete m_os_queue;
}

#endif	// CONFIG_USE_COUNTING_SEMAPHORES == 1
