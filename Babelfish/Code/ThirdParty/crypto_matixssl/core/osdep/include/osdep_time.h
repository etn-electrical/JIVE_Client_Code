/** osdep_time.h
 *
 * Wrapper for system header osdep_time.h
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

/* This file just includes system header time.h.
   In case your system does not include all functions
   time/strftime via that file or
   does not have implementation of time.h, please
   customize this place holder header.
*/

#ifndef OSDEP_TIME_H_DEFINED
#define OSDEP_TIME_H_DEFINED 1


#include <time.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Time, which is macro wrapper for time. */
#ifndef Time
#define Time time
#endif /* Time */

/* Macro that provides Strftime, which is macro wrapper for strftime. */
#ifndef Strftime
#define Strftime strftime
#endif /* Strftime */

/* Macro that provides Gmtime, which is macro wrapper for gmtime. */
#ifndef Gmtime
#define Gmtime gmtime
#endif /* Gmtime */

#endif /* OSDEP_TIME_H_DEFINED */
