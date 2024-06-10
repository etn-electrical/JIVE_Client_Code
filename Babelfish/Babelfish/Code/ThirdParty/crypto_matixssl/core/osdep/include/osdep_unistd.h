/** osdep_unistd.h
 *
 * Wrapper for system header osdep_unistd.h
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

/* This file just includes system header unistd.h.
   In case your system does not include all functions
   select/sysconf/sleep via that file or
   does not have implementation of unistd.h, please
   customize this place holder header. 
*/

#ifndef OSDEP_UNISTD_H_DEFINED
#define OSDEP_UNISTD_H_DEFINED 1

#if defined _POSIX_C_SOURCE || defined __unix__ || defined __linux__ || defined __android_ || defined __APPLE__
#include <unistd.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Select, which is macro wrapper for select. */
#ifndef Select
#define Select select
#endif /* Select */

/* Macro that provides Sysconf, which is macro wrapper for sysconf. */
#ifndef Sysconf
#define Sysconf sysconf
#endif /* Sysconf */

/* Macro that provides Sleep, which is macro wrapper for sleep. */
#ifndef Sleep
#define Sleep sleep
#endif /* Sleep */

/* Macro that provides Close, which is macro wrapper for close. */
#ifndef Close
#define Close close
#endif /* Close */

#endif /* system that usually has unistd.h. */

#endif /* OSDEP_UNISTD_H_DEFINED */
