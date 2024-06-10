/* c_lib.h
 *
 * Description: Wrappers for C Library functions.
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

#ifndef INCLUDE_GUARD_C_LIB_H
#define INCLUDE_GUARD_C_LIB_H

#include "public_defs.h"

#if __STDC_VERSION__ >= 199901L
# define RESTRICT restrict
#else
# define RESTRICT
#endif /* __STDC_VERSION__ >= 199901L */

void *
c_memcpy(
    void *RESTRICT s1,
    const void *RESTRICT s2,
    size_t n);

void *
c_memmove(
    void *s1,
    const void *s2,
    size_t n);


void *
c_memset(
    void *s,
    int c,
    size_t n);


int
c_memcmp(
    const void *s1,
    const void *s2,
    size_t n);


int
c_strcmp(
    const char *s1,
    const char *s2);


char *
c_strcpy(
    char *RESTRICT s1,
    const char *RESTRICT s2);

char *
c_strcat(
    char *dest,
    const char *src);

char *
c_strncpy(
    char *dest,
    const char *src,
    size_t n);

int
c_strncmp(
    const char *s1,
    const char *s2,
    size_t n);

size_t
c_strlen(
    const char *s);

char *
c_strstr(
    const char *str1,
    const char *str2);


long
c_strtol(
    const char *str,
    char **endptr,
    int16_t radix);

char *
c_strchr(
    const char *str,
    int32_t c);

int
c_tolower(
    int c);

int
c_toupper(
    int c);


int *
c_memchr(
    const void *buf,
    int32_t ch,
    size_t num);

#endif /* Include guard */

/* end of file c_lib.h */
