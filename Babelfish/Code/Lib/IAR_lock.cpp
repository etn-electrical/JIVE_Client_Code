/**
 *****************************************************************************************
 * @file	IAR_lock.cpp IAR multi-thread support
 * @details This file implements the functions that are required to make library objects (eg. malloc,
 *          free, calloc, local static variables, etc) thread-safe. By default, all these functions are declared in
 *<DLib_Threads.h>
 *			by IAR but IAR doesn't provide functions definitions for these functions(we implement it as
 *			per FreeRTOS thread-safety handling)
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <DLib_Threads.h>
#include "FreeRTOS.h"
#include "task.h"
#include "OS_Tasker.h"


/* Intended to return the "thread pointer".
 * Assume it means "this" thread. */
void* __aeabi_read_tp( void )
{
	return ( ( void* ) OS_Tasker::Get_Current_Task() );
}

/*
 *****************************************************************************************
 * TBD
 *****************************************************************************************
 */
void __iar_system_Mtxinit( __iar_Rmtx* m )
{
	// Need to implement
}

/*
 *****************************************************************************************
 * TBD
 *****************************************************************************************
 */
void __iar_system_Mtxdst( __iar_Rmtx* m )
{
	// Need to implement
}

/*
 *****************************************************************************************
 * This function is called to lock the shared resource(For eg, this function
 * is automatically called by IAR before calling malloc() function)
 *****************************************************************************************
 */
void __iar_system_Mtxlock( __iar_Rmtx* m )
{
	vTaskSuspendAll();
}

/*
 *****************************************************************************************
 * This function is called to unlock the shared resource(For eg, this function
 * is automatically called by IAR after calling malloc() function)
 *****************************************************************************************
 */
void __iar_system_Mtxunlock( __iar_Rmtx* m )
{
	xTaskResumeAll();
}