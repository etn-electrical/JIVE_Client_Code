/**
 *****************************************************************************************
 * @file	   	Test_Fault_Catch.cpp

 * @details    	This file contains test application for internal RTC.
 * @copyright  	2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Test_Fault_Catch.h"
#include <stdio.h>

void Hardfault_Test( int en )
{
	int x = 5, y = 2, z = 0;

	if ( en )
	{
		y = 0;
	}
	printf( "x=%d, y=%d, z=%d\r\n", x, y, z );
	// Avoiding device by 0 condition
	if ( y != 0U )
	{
		z = x / y;
		printf( "x=%d, y=%d, z=%d\r\n", x, y, z );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Test_Fault_Catch_App_Main( void )
{
#ifdef HARDFAULT_FAULTCATCH_TEST

	Hardfault_Test( 1 );

#elif WATCHDOG_FAULTCATCH_TEST

#elif EXCEPTION_FAULTCATCH_TEST

#endif
}
