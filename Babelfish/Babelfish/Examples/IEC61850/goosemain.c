/* ***************************************************************************
   Triangle MicroWorks, Inc.                         Copyright (c) 1997-2011
  ***************************************************************************
  
   This file is the property of:
  
                         Triangle MicroWorks, Inc.
                        Raleigh, North Carolina USA
                         www.TriangleMicroWorks.com
                            (919) 870-6615
  
   This Source Code and the associated Documentation contain proprietary
   information of Triangle MicroWorks, Inc. and may not be copied or
   distributed in any form without the written permission of Triangle
   MicroWorks, Inc.  Copies of the source code may be made only for backup
   purposes.
  
   Your License agreement may limit the installation of this source code to
   specific products.  Before installing this source code on a new
   application, check your license agreement to ensure it allows use on the
   product in question.  Contact Triangle MicroWorks for information about
   extending the number of products that may use this source code library or
   obtaining the newest revision.
  
  ****************************************************************************/

/* file: GooseMain.c
 * description: Simple Example Goose application
 *
 *  This module contains an example of the initializion and
 *  main processing loop to be used with the Tamarack IEC Goose
 *  software.
 *
 *  This code has been written to run under DOS/Windows, although
 *  in general it should run in most environments with minimal
 *  changes for a single thread implementation.
 *
 *  Most of this code is concerned with keyboard and control-C
 *  processing.  The primary GOOSE code has been isolated to the
 *  module GooseApp.c.  Hopefully this will make it simple to follow.
 *
 *  This example code is designed to run as single-threaded. If a multi-threaded
 *  application is appropriate, the call to ServiceTest can safely be called from
 *  a thread which waits on the Ethernet socket.
 */

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys.h"
#include "tmwtarg.h"
#include "Prod_Spec_LTK_STM32F.h"

/// * IEC GOOSE specific includes */
#include "iecgoose.h"				/* Goose programming interface */
#include "gooseapp.h"
#include "goosemain.h"

#ifdef TEST_GOOSE_APP

void* pTestHandle;	// Goose context

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TMW_Goose_Init( void )
{
	char* pInMacAddress = NULL;
	char* pOutMacAddress = NULL;
	TestMode nTestMode = Test_Display;
	int bSwitch = 0;
	int bEtherLoop = 0;

	pTestHandle = StartGooseTest( nTestMode, bSwitch, bEtherLoop, pInMacAddress, pOutMacAddress );

	if ( pTestHandle == NULL )
	{
		printf( "\n Goose context is not created" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TMW_Goose_App_Run( void )
{
	static char flag = 0;
	static u16_t count = 0;

	if ( flag == 0 )
	{
		flag = 1;
		SendGooseFirst( pTestHandle );
	}
	else
	{
		ServiceTest( pTestHandle );
	}
	count++;
	if ( count > 10000 )// Wait for 10 sec to transmit the new packet
	{
		count = 0;
		SendTestData( pTestHandle );
	}
}

#endif	// TEST_GOOSE_APP

