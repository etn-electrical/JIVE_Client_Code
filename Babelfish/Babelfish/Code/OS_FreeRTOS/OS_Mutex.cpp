/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "OS_Mutex.h"

#if CONFIG_USE_MUTEXES == 1U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Mutex::OS_Mutex( void ) :
	m_os_mutex( xSemaphoreCreateMutex() )
{
	// Release a semaphore.
	xSemaphoreGive( m_os_mutex );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Mutex::~OS_Mutex( void )
{
	vSemaphoreDelete( m_os_mutex );
}

#endif	// CONFIG_USE_MUTEXES == 1
