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
#include "OS_Mutex.h"

#if CONFIG_USE_MUTEXES == 1U

/*
 *****************************************************************************
 *		Constants
 *****************************************************************************
 */
#define OS_MUTEX_QUEUE_LENGTH       1U
#define OS_MUTEX_ITEM_SIZE          0U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Mutex::OS_Mutex( void ) :
	m_os_queue( reinterpret_cast<OS_Queue*>( nullptr ) )
{
	m_os_queue = new OS_Queue( OS_MUTEX_QUEUE_LENGTH,
							   OS_MUTEX_ITEM_SIZE, true );

	m_os_queue->Send( reinterpret_cast<void const*>( nullptr ), 0U, SEND_TO_BACK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Mutex::~OS_Mutex( void )
{
	delete m_os_queue;
}

#endif	// CONFIG_USE_MUTEXES == 1
