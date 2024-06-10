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
#include "OS_Semaphore.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// The Queue system requires a length of 1 and an items size of 0 when it is a semaphore.
#define OS_SEMAPHORE_QUEUE_LENGTH       1U
#define OS_SEMAPHORE_QUEUE_ITEM_SIZE    0U


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Semaphore::OS_Semaphore( bool pre_post ) :
	m_os_queue( reinterpret_cast<OS_Queue*>( nullptr ) )
{
	m_os_queue = new OS_Queue( OS_SEMAPHORE_QUEUE_LENGTH, OS_SEMAPHORE_QUEUE_ITEM_SIZE );

	if ( pre_post == true )
	{
		Post();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Semaphore::~OS_Semaphore( void )
{
	delete m_os_queue;
}
