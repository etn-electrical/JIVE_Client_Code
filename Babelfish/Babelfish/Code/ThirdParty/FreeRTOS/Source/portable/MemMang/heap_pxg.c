/**
 *****************************************************************************************
 *	@file
 *	@brief Contains the Ram class Wrappers with functionalities mainly for Memory
 *	allocation and de-allocation.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *****************************************************************************************
 */
#ifndef HEAP_PXG
#define HEAP_PXG

#include <stdlib.h>
#include "Includes.h"
#include "FreeRTOS.h"
#include "Ram_C_Connector.h"

void *pvPortMalloc( size_t xWantedSize )
{
	void *pvReturn = NULL;
	
	pvReturn = Ram_Allocate( xWantedSize );
	
	return ( pvReturn );
}

void vPortFree( void *pv )
{
	Ram_De_Allocate( pv );
}

#endif