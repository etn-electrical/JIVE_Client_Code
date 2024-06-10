/** osdep_pthread.h
 *
 * Wrapper for system header osdep_pthread.h
 */

/*****************************************************************************
* Copyright (c) 2017 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*****************************************************************************/

/* This file just includes system header pthread.h.
   In case your system does not include all functions
   pthread_create/pthread_join/pthread_mutex_init/pthread_mutex_lock/pthread_mutex_unlock via that file or
   does not have implementation of pthread.h, please
   customize this place holder header.
*/

#ifndef OSDEP_PTHREAD_H_DEFINED
#define OSDEP_PTHREAD_H_DEFINED 1
#ifdef __ARMCC5
/* pthread.h may use __leaf__ attribute which ARMCC does not recognize. */
#pragma push
#pragma diag_suppress 1207
#endif /* __ARMCC5 */

#include <pthread.h>

#ifdef __ARMCC5
#pragma pop /* restore diagnostics messages settings. */
#endif /* __ARMCC5 */


/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Pthread_create, which is macro wrapper for pthread_create. */
#ifndef Pthread_create
#define Pthread_create pthread_create
#endif /* Pthread_create */

/* Macro that provides Pthread_join, which is macro wrapper for pthread_join. */
#ifndef Pthread_join
#define Pthread_join pthread_join
#endif /* Pthread_join */

/* Macro that provides Pthread_mutex_init, which is macro wrapper for pthread_mutex_init. */
#ifndef Pthread_mutex_init
#define Pthread_mutex_init pthread_mutex_init
#endif /* Pthread_mutex_init */

/* Macro that provides Pthread_mutex_lock, which is macro wrapper for pthread_mutex_lock. */
#ifndef Pthread_mutex_lock
#define Pthread_mutex_lock pthread_mutex_lock
#endif /* Pthread_mutex_lock */

/* Macro that provides Pthread_mutex_unlock, which is macro wrapper for pthread_mutex_unlock. */
#ifndef Pthread_mutex_unlock
#define Pthread_mutex_unlock pthread_mutex_unlock
#endif /* Pthread_mutex_unlock */

#endif /* OSDEP_PTHREAD_H_DEFINED */
