/** osdep_time_gmtime_r.h
 *
 * Wrapper for system header osdep_time_gmtime_r.h
 */

/*****************************************************************************
* Copyright (c) 2017-2018 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*****************************************************************************/

/* This file is extension of osdep_time.h, intended to be used when
   the caller would like to call function gmtime_r().

   This function is unfortunately not available on all operating systems.
*/

#ifndef OSDEP_TIME_GMTIME_R_H_DEFINED
#define OSDEP_TIME_GMTIME_R_H_DEFINED 1

#include "osdep_time.h"

/* On most unix like Unix-like devices gmtime_r() is available.
   Detect its availability by including osdep_unistd.h. */

#if defined __unix__ || defined __unix || (defined (__APPLE__) && defined (__MACH__))
# include "osdep_unistd.h" /* Possibly provides _POSIX_VERSION. */
#endif /* __unix__ */

#if defined _POSIX_VERSION && !defined NO_GMTIME_R
# define USE_GMTIME_R /* On posix systems, we use gmtime_r() */
#endif /* _POSIX_VERSION && !defined NO_GMTIME_R */

#ifdef USE_GMTIME_R

/* Macro that provides Gmtime_r, which is macro wrapper for gmtime_r. */
#ifndef Gmtime_r
#define Gmtime_r gmtime_r
#endif /* Gmtime_r */

#endif

#endif /* OSDEP_TIME_GMTIME_R_H_DEFINED */
