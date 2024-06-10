/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "OS_Recursive_Mutex.h"

#if CONFIG_USE_RECURSIVE_MUTEXES == 1U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Recursive_Mutex::OS_Recursive_Mutex( void ) :
	m_os_recursive_mutex( xSemaphoreCreateRecursiveMutex() )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Recursive_Mutex::~OS_Recursive_Mutex( void )
{
	vSemaphoreDelete( m_os_recursive_mutex );
}

#endif	// CONFIG_USE_RECURSIVE_MUTEXES == 1
