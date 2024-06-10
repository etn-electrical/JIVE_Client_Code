/** osdep_stdio.h
 *
 * Wrapper for system header osdep_stdio.h
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

/* This file just includes system header stdio.h.
   In case your system does not include all functions
   fopen/ferror/fread/fwrite/fclose/fflush/fprintf/printf/puts/snprintf/sprintf/sscanf via that file or
   does not have implementation of stdio.h, please
   customize this place holder header.
*/

#ifndef OSDEP_STDIO_H_DEFINED
#define OSDEP_STDIO_H_DEFINED 1


#include <stdio.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Fopen, which is macro wrapper for fopen. */
#ifndef Fopen
#define Fopen fopen
#endif /* Fopen */

/* Macro that provides Ferror, which is macro wrapper for ferror. */
#ifndef Ferror
#define Ferror ferror
#endif /* Ferror */

/* Macro that provides Fread, which is macro wrapper for fread. */
#ifndef Fread
#define Fread fread
#endif /* Fread */

/* Macro that provides Fwrite, which is macro wrapper for fwrite. */
#ifndef Fwrite
#define Fwrite fwrite
#endif /* Fwrite */

/* Macro that provides Fclose, which is macro wrapper for fclose. */
#ifndef Fclose
#define Fclose fclose
#endif /* Fclose */

/* Macro that provides Fflush, which is macro wrapper for fflush. */
#ifndef Fflush
#define Fflush fflush
#endif /* Fflush */

/* Macro that provides Fprintf, which is macro wrapper for fprintf. */
#ifndef Fprintf
#define Fprintf fprintf
#endif /* Fprintf */

/* Macro that provides Printf, which is macro wrapper for printf. */
#ifndef Printf
#define Printf printf
#endif /* Printf */

/* Macro that provides Puts, which is macro wrapper for puts. */
#ifndef Puts
#define Puts puts
#endif /* Puts */

/* Macro that provides Snprintf, which is macro wrapper for snprintf. */
#ifndef Snprintf
#define Snprintf snprintf
#endif /* Snprintf */

/* Macro that provides Sprintf, which is macro wrapper for sprintf. */
#ifndef Sprintf
#define Sprintf sprintf
#endif /* Sprintf */

/* Macro that provides Sscanf, which is macro wrapper for sscanf. */
#ifndef Sscanf
#define Sscanf sscanf
#endif /* Sscanf */



#endif /* OSDEP_STDIO_H_DEFINED */
