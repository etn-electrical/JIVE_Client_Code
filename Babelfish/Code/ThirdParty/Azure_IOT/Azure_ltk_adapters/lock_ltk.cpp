// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
// #include "includes.h"
// #include "platform_ltk_azure.h"
// #include <pthread.h>
#include <ctype.h>

#include "OS_Tasker.h"
#include "OS_Queue.h"
#include "OS_Mutex.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/lock.h"

MU_DEFINE_ENUM_STRINGS( LOCK_RESULT, LOCK_RESULT_VALUES );

/*SRS_LOCK_99_002:[ This API on success will return a valid lock handle which should be a non NULL value]*/
LOCK_HANDLE Lock_Init( void )
{
	OS_Mutex* lock_mtx = new OS_Mutex();

	if ( NULL == lock_mtx )
	{
		LogError( "Failed to initialize mutex" );
	}
	return ( ( LOCK_HANDLE )lock_mtx );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
LOCK_RESULT Lock( LOCK_HANDLE handle )
{
	LOCK_RESULT result;
	OS_Mutex* mutex = ( OS_Mutex* )handle;

	if ( handle == NULL )
	{
		/*SRS_LOCK_99_007:[ This API on NULL handle passed returns LOCK_ERROR]*/
		result = LOCK_ERROR;
		LogError( "(result = %s)", MU_ENUM_TO_STRING( LOCK_RESULT, result ) );
	}
	else
	{
		if ( mutex->Acquire( OS_MUTEX_INDEFINITE_TIMEOUT ) )
		{
			/*SRS_LOCK_99_005:[ This API on success should return LOCK_OK]*/
			result = LOCK_OK;
		}
		else
		{
			/*SRS_LOCK_99_006:[ This API on error should return LOCK_ERROR]*/
			result = LOCK_ERROR;
			LogError( "(result = %s)", MU_ENUM_TO_STRING( LOCK_RESULT, result ) );
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
LOCK_RESULT Unlock( LOCK_HANDLE handle )
{
	LOCK_RESULT result;
	OS_Mutex* mutex = ( OS_Mutex* )handle;

	if ( handle == NULL )
	{
		/*SRS_LOCK_99_011:[ This API on NULL handle passed returns LOCK_ERROR]*/
		result = LOCK_ERROR;
		LogError( "(result = %s)", MU_ENUM_TO_STRING( LOCK_RESULT, result ) );
	}
	else
	{
		if ( mutex->Release() )
		{
			/*SRS_LOCK_99_009:[ This API on success should return LOCK_OK]*/
			result = LOCK_OK;
		}
		else
		{
			/*SRS_LOCK_99_010:[ This API on error should return LOCK_ERROR]*/
			result = LOCK_ERROR;
			LogError( "(result = %s)", MU_ENUM_TO_STRING( LOCK_RESULT, result ) );
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
LOCK_RESULT Lock_Deinit( LOCK_HANDLE handle )
{
	LOCK_RESULT result = LOCK_OK;

	if ( NULL == handle )
	{
		/*SRS_LOCK_99_013:[ This API on NULL handle passed returns LOCK_ERROR]*/
		result = LOCK_ERROR;
		LogError( "(result = %s)", MU_ENUM_TO_STRING( LOCK_RESULT, result ) );
	}
	else
	{
		/*SRS_LOCK_99_012:[ This API frees the memory pointed by handle]*/
		OS_Mutex* lock_mtx = ( OS_Mutex* )handle;
		delete lock_mtx;
		handle = nullptr;
	}
	return ( result );
}
