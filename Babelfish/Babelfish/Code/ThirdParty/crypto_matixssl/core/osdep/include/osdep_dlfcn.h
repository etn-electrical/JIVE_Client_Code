/** osdep_dlfcn.h
 *
 * Wrapper for system header osdep_dlfcn.h
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

/* This file just includes system header dlfcn.h.
   In case your system does not include all functions
   dlopen/dlclose/dlsym via that file or
   does not have implementation of dlfcn.h, please
   customize this place holder header.
*/

#ifndef OSDEP_DLFCN_H_DEFINED
#define OSDEP_DLFCN_H_DEFINED 1


#include <dlfcn.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Dlopen, which is macro wrapper for dlopen. */
#ifndef Dlopen
#define Dlopen dlopen
#endif /* Dlopen */

/* Macro that provides Dlclose, which is macro wrapper for dlclose. */
#ifndef Dlclose
#define Dlclose dlclose
#endif /* Dlclose */

/* Macro that provides Dlsym, which is macro wrapper for dlsym. */
#ifndef Dlsym
#define Dlsym dlsym
#endif /* Dlsym */



#endif /* OSDEP_DLFCN_H_DEFINED */
