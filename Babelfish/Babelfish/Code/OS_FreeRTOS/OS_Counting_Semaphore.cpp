/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "OS_Counting_Semaphore.h"
#if CONFIG_USE_COUNTING_SEMAPHORES == 1U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Counting_Semaphore::OS_Counting_Semaphore( BF_Lib::MBASE max_count,
											  BF_Lib::MBASE initial_count ) :
	m_os_counting_semaphore( xSemaphoreCreateCounting( max_count, initial_count ) )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Counting_Semaphore::~OS_Counting_Semaphore( void )
{
	vSemaphoreDelete( m_os_counting_semaphore );
}

#endif	// CONFIG_USE_COUNTING_SEMAPHORES == 1
