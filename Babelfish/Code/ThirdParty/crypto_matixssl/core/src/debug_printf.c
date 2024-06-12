/* debug_printf.c
 *
 * Description: Implementation of DEBUG_printf.
 */

/*****************************************************************************
* Copyright (c) 2007-2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#ifndef _POSIX_C_SOURCE
# define _POSIX_C_SOURCE 1 /* Depends on POSIX.1-2001 (file I/O locking) */
#endif /* _POSIX_C_SOURCE */

#ifndef _DEFAULT_SOURCE
# define _DEFAULT_SOURCE /* Needs fputc_unlocked  */
#endif /* _DEFAULT_SOURCE */

#include "implementation_defs.h"

#ifdef IMPLDEFS_NEED_DEBUG_PRINTF

#include "osdep_stdarg.h"
#include "osdep_stddef.h"
#include "osdep_stdio.h"
#include "osdep_stdlib.h" /* For strtol. */
#include "osdep_limits.h" /* For strtol. */
#include "coreApi.h"
#include "psprintf.h"

#ifdef DEBUG_CF_USE_STDOUT
# define DEBUG_fd  stdout
#else
# define DEBUG_fd  stderr
#endif

static
void SZ_putc(struct arg *arg)
{
    fputc_unlocked(arg->ch, DEBUG_fd);
}

int DEBUG_printf(const char *format, ...)
{
    struct arg arg;
    va_list ap;

    va_start(ap, format);

    arg.do_putc = &SZ_putc;
    arg.count = 0;
    (void) flockfile(DEBUG_fd);
    (void) psVprintf(&arg, format, ap);

#ifdef DEBUG_CF_USE_FLUSH
    (void) fflush_unlocked(DEBUG_fd);
#endif
    (void) funlockfile(DEBUG_fd);

    va_end(ap);

    return 0;
}

#else /* IMPLDEFS_NEED_DEBUG_PRINTF */

/* DEBUG_printf() has been deprecated. */
extern int DEBUG_printf_not_available;

#endif /* IMPLDEFS_NEED_DEBUG_PRINTF */

/* end of file debug_printf.c */
