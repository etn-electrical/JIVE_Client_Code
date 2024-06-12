/** osdep_malloc.h
 *
 * Wrapper for system header osdep_malloc.h
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

/* This file just includes system header malloc.h.
   In case your system does not include all functions
   malloc/free/calloc/realloc via that file or
   does not have implementation of malloc.h, please
   customize this place holder header.
*/

#ifndef OSDEP_MALLOC_H_DEFINED
#define OSDEP_MALLOC_H_DEFINED 1


#include <malloc.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Malloc, which is macro wrapper for malloc. */
#ifndef Malloc
#define Malloc malloc
#endif /* Malloc */

/* Macro that provides Free, which is macro wrapper for free. */
#ifndef Free
#define Free free
#endif /* Free */

/* Macro that provides Calloc, which is macro wrapper for calloc. */
#ifndef Calloc
#define Calloc calloc
#endif /* Calloc */

/* Macro that provides Realloc, which is macro wrapper for realloc. */
#ifndef Realloc
#define Realloc realloc
#endif /* Realloc */



#endif /* OSDEP_MALLOC_H_DEFINED */
