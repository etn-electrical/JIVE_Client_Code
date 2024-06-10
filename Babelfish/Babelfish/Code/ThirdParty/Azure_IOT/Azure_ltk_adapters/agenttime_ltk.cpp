// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/gballoc.h"

#include <time.h>
#include "azure_c_shared_utility/agenttime.h"

// Provided by the product code:
extern time_t Get_Epoch_Time( void );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
time_t get_time( time_t* currentTime )
{
	time_t epoch_time = 0;

	epoch_time = Get_Epoch_Time();
	if ( currentTime != nullptr )
	{
		*currentTime = epoch_time;
	}

	return ( epoch_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
char* get_ctime( time_t* timeToGet )
{
	return ( ctime( timeToGet ) );	// (time_t *)timeToGet;//ctime(timeToGet);
}

#if 0
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
struct tm* get_gmtime( time_t* currentTime )
{
	return ( gmtime( currentTime ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
time_t get_mktime( struct tm* cal_time )
{
	return ( mktime( cal_time ) );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
double get_difftime( time_t stopTime, time_t startTime )
{
	return ( difftime( stopTime, startTime ) );
}
