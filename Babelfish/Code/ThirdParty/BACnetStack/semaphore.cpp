/*---------------------------------------------------------------------------
	©2003-2010, PolarSoft® Inc., All Rights Reserved
	
module:		SEMAPHORE.C
desc:		Semaphore Manager
authors:	David M. Fisher, Jack R. Neyer
last edit:	23-Oct-2007 [255] CLB Windows CRITICAL_SECTION implementation
			12-Jun-2006 [252] CLB Linux pthread specific implementaiton
			06-Dec-2003 [001] DMF convert from eth.c version to generalized

Note:		You must customize this module to implement semaphored
			lockout for the target system. This may also require changing
			semaphore.h struct for semaphore storage.
----------------------------------------------------------------------------*/ 
#include "semaphore.h"
//Linux-------------------------------------------------------------Linux
/*
#ifndef _WINDOWS
#include "core.h"
#include <pthread.h>	//	***252
//pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; *The* mutex
#else
#include <windows.h>
//CRITICAL_SECTION mtx;
#endif
*/
//Linux-------------------------------------------------------------Linux



///////////////////////////////////////////////////////////////////////
//	Clear a semaphore
//
//in:	q		the semaphore

/*void bpublic semaClear(semaphore *q)
{
#ifndef _WINDOWS 							//	***252 Begin
	pthread_mutex_init(&q->mtx,NULL);	
#else										//	***255
	InitializeCriticalSection(&q->mtx);		//	***255
#endif										//	***252 End
}*/

///////////////////////////////////////////////////////////////////////
//	Capture a semaphore
//
//in:	q		the semaphore

void bpublic semaCapture(semaphore *q)
{
    Push_GINT(q->mtx);
/*
#ifndef _WINDOWS					//  ***252 Begin
	pthread_mutex_lock(&q->mtx);
#else								//	***255
	EnterCriticalSection(&q->mtx);	//	***255
#endif								//  ***252 End
*/
}

///////////////////////////////////////////////////////////////////////
//	Release a semaphore
//
//in:	q		the semaphore

void bpublic semaRelease(semaphore *q)
{
    Pop_GINT(q->mtx);
/*
#ifndef _WINDOWS						//	***252 Begin
	pthread_mutex_unlock(&q->mtx);
#else									//	***255
	LeaveCriticalSection(&q->mtx);		//	***255
#endif									//	***252 End
*/
}

