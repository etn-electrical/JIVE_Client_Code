// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// #include <cstdlib>

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/tickcounter.h"
#include "matrixos.h"
#include "mtxssl_if.h"
#include "platform_ltk_azure.h"


class TICK_COUNTER_INSTANCE_TAG
{
	public:
		// clock_t last_clock_value;
		psTime_t current_ms;
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
TICK_COUNTER_HANDLE tickcounter_create( void )
{
	TICK_COUNTER_INSTANCE_TAG* result;

	result = new TICK_COUNTER_INSTANCE_TAG();
	mtxssl_if_psGetTime( &result->current_ms );
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void tickcounter_destroy( TICK_COUNTER_HANDLE tick_counter )
{
	if ( tick_counter != NULL )
	{
		delete tick_counter;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int tickcounter_get_current_ms( TICK_COUNTER_HANDLE tick_counter, uint32_t* current_ms )
{
	int result;

	if ( current_ms == NULL )
	{
		result = __FAILURE__;
	}
	else
	{
		mtxssl_if_psGetTime( current_ms );

		result = 0;
	}
	return ( result );
}
