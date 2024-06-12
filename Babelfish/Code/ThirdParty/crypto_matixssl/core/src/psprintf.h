/* psprintf.h
 *
 * Description: Interface for formatted printing functions.
 */

/*****************************************************************************
* Copyright (c) 2007-2018 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#ifndef INCLUDE_GUARD_PSPRINTF_H
#define INCLUDE_GUARD_PSPRINTF_H 1

#include "coreApi.h"

struct arg
{
    char ch; /* Current character. */
    /* Flags. */
    unsigned char padz;
    unsigned char altform;
    unsigned char upper;
    /* Field formatting. */
    long min;
    long max;
    /* Output count. */
    size_t count;
    /* Formatting control. */
    void (*do_putc)(struct arg *);
    void *context;
    const char *fmt;
};

void psVprintf(struct arg *arg, const char *fmt, va_list va);

#endif /* INCLUDE_GUARD_PSPRINTF_H */

/* end of file psprintf.h */
