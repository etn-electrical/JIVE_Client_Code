/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "OS_Binary_Semaphore.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Binary_Semaphore::OS_Binary_Semaphore( bool pre_post ) :
	m_os_binary_semaphore( xSemaphoreCreateBinary() )
{
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
	vSemaphoreDelete( m_os_binary_semaphore );
}
