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
#include "OS_Binary_Semaphore.h"

/*
 *****************************************************************************
 *		Constants
 *****************************************************************************
 */
#define OS_BINARY_SEMAPHORE_QUEUE_LENGTH        1U
#define OS_BINARY_SEMAPHORE_ITEM_SIZE          0U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Binary_Semaphore::OS_Binary_Semaphore( bool pre_post )  :
	m_os_queue( reinterpret_cast<OS_Queue*>( nullptr ) )
{
	m_os_queue = new OS_Queue( OS_BINARY_SEMAPHORE_QUEUE_LENGTH,
							   OS_BINARY_SEMAPHORE_ITEM_SIZE, false );

	if ( pre_post )
	{
		Post();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Binary_Semaphore::~OS_Binary_Semaphore( void )
{
	delete m_os_queue;
}
