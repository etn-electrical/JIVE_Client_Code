/** osdep_ctype.h
 *
 * Wrapper for system header osdep_ctype.h
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

/* This file just includes system header ctype.h.
   In case your system does not include all functions
   tolower/toupper/isdigit/isalpha/isxalpha/isxdigit/isspace via that file or
   does not have implementation of ctype.h, please
   customize this place holder header.
*/

#ifndef OSDEP_CTYPE_H_DEFINED
#define OSDEP_CTYPE_H_DEFINED 1


#include <ctype.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Tolower, which is macro wrapper for tolower. */
#ifndef Tolower
#define Tolower tolower
#endif /* Tolower */

/* Macro that provides Toupper, which is macro wrapper for toupper. */
#ifndef Toupper
#define Toupper toupper
#endif /* Toupper */

/* Macro that provides Isdigit, which is macro wrapper for isdigit. */
#ifndef Isdigit
#define Isdigit isdigit
#endif /* Isdigit */

/* Macro that provides Isalpha, which is macro wrapper for isalpha. */
#ifndef Isalpha
#define Isalpha isalpha
#endif /* Isalpha */

/* Macro that provides Isxalpha, which is macro wrapper for isxalpha. */
#ifndef Isxalpha
#define Isxalpha isxalpha
#endif /* Isxalpha */

/* Macro that provides Isxdigit, which is macro wrapper for isxdigit. */
#ifndef Isxdigit
#define Isxdigit isxdigit
#endif /* Isxdigit */

/* Macro that provides Isspace, which is macro wrapper for isspace. */
#ifndef Isspace
#define Isspace isspace
#endif /* Isspace */



#endif /* OSDEP_CTYPE_H_DEFINED */
