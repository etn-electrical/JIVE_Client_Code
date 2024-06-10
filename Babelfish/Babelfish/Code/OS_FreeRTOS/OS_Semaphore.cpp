/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

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
	m_os_semaphore( xQueueCreate( OS_SEMAPHORE_QUEUE_LENGTH, OS_SEMAPHORE_QUEUE_ITEM_SIZE ) )
{
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
	vSemaphoreDelete( m_os_semaphore );
}
