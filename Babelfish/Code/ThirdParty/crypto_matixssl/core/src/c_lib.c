/* c_lib.c
 *
 * Description: Wrappers for C Library functions
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

#include "implementation_defs.h"
#include "c_lib.h"

#include "osdep_string.h"
#include "osdep_stdlib.h"
#include "osdep_ctype.h"

void *
c_memcpy(
    void *RESTRICT s1,
    const void *RESTRICT s2,
    size_t n)
{
    return Memcpy(s1, s2, n);
}


void *
c_memmove(
    void *s1,
    const void *s2,
    size_t n)
{
    return Memmove(s1, s2, n);
}

void *
c_memset(
    void *s,
    int c,
    size_t n)
{
    return Memset(s, c, n);
}


int
c_memcmp(
    const void *s1,
    const void *s2,
    size_t n)
{
    return Memcmp(s1, s2, n);
}

int
c_strcmp(
    const char *s1,
    const char *s2)
{
    return Strcmp(s1, s2);
}

int
c_strncmp(
    const char *s1,
    const char *s2,
    size_t n)
{
    return Strncmp(s1, s2, n);
}

char *
c_strcpy(
    char *RESTRICT s1,
    const char *RESTRICT s2)
{
    return Strcpy(s1, s2);
}

char *
c_strcat(
    char *dest,
    const char *src)
{
    return Strcat(dest, src);
}

char *
c_strncpy(
    char *dest,
    const char *src,
    size_t n)
{
    return Strncpy(dest, src, n);
}

size_t
c_strlen(
    const char *s)
{
    return Strlen(s);
}

char *
c_strstr(
    const char *str1,
    const char *str2)
{
    return Strstr(str1, str2);
}

long
c_strtol(
    const char *str,
    char **endptr,
    int16_t radix)
{
    return Strtol(str, endptr, radix);
}

char *
c_strchr(const char *str, int32_t c)
{
    return Strchr(str, c);
}

int
c_tolower(int c)
{
    return Tolower(c);
}

int
c_toupper(int c)
{
    return Toupper(c);
}

int *
c_memchr(
    const void *buf,
    int32_t ch,
    size_t num)
{
    return Memchr(buf, ch, num);
}

/* end of file c_lib.c */
