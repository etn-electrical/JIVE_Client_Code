/** osdep_strings.h
 *
 * Wrapper for system header osdep_strings.h
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

/* This file just includes system header strings.h.
   In case your system does not include all functions
   ffsll via that file or
   does not have implementation of strings.h, please
   customize this place holder header.
*/

#ifndef OSDEP_STRINGS_H_DEFINED
#define OSDEP_STRINGS_H_DEFINED 1


#include <strings.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Ffsll, which is macro wrapper for ffsll. */
#ifndef Ffsll
#define Ffsll ffsll
#endif /* Ffsll */

/* Macro that provides Strcasecmp, which is macro wrapper for strcasecmp. */
#ifndef Strcasecmp
#define Strcasecmp strcasecmp
#endif /* Strcasecmp */

#endif /* OSDEP_STRINGS_H_DEFINED */
