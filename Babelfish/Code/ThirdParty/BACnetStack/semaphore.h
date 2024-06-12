//Contents of SEMAPHORE.H
//	©2003-2010, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------

#ifndef __SEMAPHORE_H_INCLUDED
#define __SEMAPHORE_H_INCLUDED

/*
#ifndef _WINDOWS
#include <pthread.h>
#else
#include <windows.h>
#endif
*/
#include "core.h"

/*SRP Added*/
#include "Includes.h"
#include "Device_Defines.h"

#ifndef bpublic
#define bpublic 	extern
#endif

#ifdef _WINDOWS
#ifndef word
typedef unsigned short word;
#endif
#endif

typedef struct	_semaphore {
	GINT_TDEF mtx;
/*
#ifndef _WINDOWS
	pthread_mutex_t mtx; // *The* mutex	// ***252
#else
	CRITICAL_SECTION mtx;
#endif
	word	w;
*/
} semaphore;

//------------------------------------------------------------------------
//Functions provided by SEMAPHORE.C:

void bpublic		semaCapture(semaphore *);
void bpublic		semaRelease(semaphore *);
//void bpublic		semaClear(semaphore *);

#endif //__SEMAPHORE_H_INCLUDED
