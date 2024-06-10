/** osdep_setjmp.h
 *
 * Wrapper for system header osdep_setjmp.h
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

/* This file just includes system header setjmp.h.
   In case your system does not include all functions
   setjmp/longjmp via that file or
   does not have implementation of setjmp.h, please
   customize this place holder header. 
*/

#ifndef OSDEP_SETJMP_H_DEFINED
#define OSDEP_SETJMP_H_DEFINED 1


#include <setjmp.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Setjmp, which is macro wrapper for setjmp. */
#ifndef Setjmp
#define Setjmp setjmp
#endif /* Setjmp */

/* Macro that provides Longjmp, which is macro wrapper for longjmp. */
#ifndef Longjmp
#define Longjmp longjmp
#endif /* Longjmp */



#endif /* OSDEP_SETJMP_H_DEFINED */
