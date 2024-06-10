/** osdep_strict.h
 *
 * Helper for avoiding extra system dependencies.
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

/* This file tries to prevent accidental use of operating system
   dependent functions. Once this header is included, the references
   to operating system dependent functions must go through osdep_*.h wrappers.
   In case direct reference is made, when this header is included the compiler
   detects the reference and prevent it on compile time.

   The header is only effective when using GNU C compiler, and compiling with
   -DOSDEP_POISON_FUNCTIONS. The header typically is no-op on most other
   compilers.
 */

#ifndef OSDEP_STRICT_H_DEFINED
#define OSDEP_STRICT_H_DEFINED 1

#if defined __GNUC__ && defined OSDEP_POISON_FUNCTIONS
#ifndef __ARMCC5 /* ARMCC is not compatible with #pragma GCC poison. */
#pragma GCC poison malloc
#pragma GCC poison free
#pragma GCC poison calloc
#pragma GCC poison realloc
#pragma GCC poison abort
#pragma GCC poison fopen
#pragma GCC poison fwread
#pragma GCC poison fwrite
#pragma GCC poison fclose
#pragma GCC poison fflush
#pragma GCC poison fprintf
#pragma GCC poison printf
#pragma GCC poison snprintf
#pragma GCC poison sprintf
#pragma GCC poison memcmp
#pragma GCC poison memmove
#pragma GCC poison memcpy
#pragma GCC poison memset
#ifndef strcmp /* Cannot use GCC poison if strcmp is a macro. */
#pragma GCC poison strcmp
#endif
#ifndef strncmp /* Cannot use GCC poison if strncmp is a macro. */
#pragma GCC poison strncmp
#endif
#pragma GCC poison strcpy
#ifndef strncpy /* Cannot use GCC poison if strncmp is a macro. */
#pragma GCC poison strncpy
#endif
#pragma GCC poison strcat
#pragma GCC poison strlen
#pragma GCC poison strstr
#pragma GCC poison strtol
#ifndef strchr /* Cannot use GCC poison if strchr is a macro. */
#pragma GCC poison strchr
#endif
#pragma GCC poison memchr
#pragma GCC poison tolower
#pragma GCC poison toupper
#pragma GCC poison isdigit
#pragma GCC poison isalpha
#pragma GCC poison isxalpha
#pragma GCC poison isxdigit
#pragma GCC poison isspace
#pragma GCC poison select
#endif
#endif /* defined __GNUC__ && defined OSDEP_POISON_FUNCTIONS */

#endif /* OSDEP_STRICT_H_DEFINED */
