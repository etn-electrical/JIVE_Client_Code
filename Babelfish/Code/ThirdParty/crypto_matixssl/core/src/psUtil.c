/**
 *      @file    psUtil.c
 *
 *
 *      Useful utility macros and functions. These macros and functions
 *  are intended to allow easier use of common idioms and to provide
 *  simple extensions to functions provided by C language standard.
 *
 *  These macros and functions can be used in programs using SafeZone
 *  and MatrixSSL software or related software components.
 */
/*
 *      Copyright (c) 2017 INSIDE Secure Corporation
 *      All Rights Reserved
 *
 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF INSIDE.
 *
 *      Please do not edit this file without first consulting INSIDE support.
 *      Unauthorized changes to this file are not supported by INSIDE.
 *
 *      The copyright notice above does not evidence any actual or intended
 *      publication of such source code.
 *
 *      This Module contains Proprietary Information of INSIDE and should be
 *      treated as Confidential.
 *
 *      The information in this file is provided for the exclusive use of the
 *      licensees of INSIDE. Such users have the right to use, modify,
 *      and incorporate this code into products for purposes authorized by the
 *      license agreement provided they include this notice and the associated
 *      copyright notice with any such product.
 *
 *      The information in this file is provided "AS IS" without warranty.
 */
/******************************************************************************/

#include "osdep.h"
#include "psUtil.h"

/* Initialize memory with specified value.
   This call is never optimized out by the compiler. */
void *psMemsetSR(void *s, int c, psSizeL_t n)
{
    if (s)
    {
        memset_s(s, n, c, n);
    }
    return s;
}

/* Initialize memory with zero value.
   This call is never optimized out by the compiler. */
void *psMemzeroSR(void *s, psSizeL_t n)
{
    if (s)
    {
        memset_s(s, n, 0x00, n);
    }
    return s;
}

/* Initialize memory with specified value.
   This call is never optimized out by the compiler. */
void psMemsetS(void *s, int c, psSizeL_t n)
{
    if (s)
    {
        memset_s(s, n, c, n);
    }
}

/* Initialize memory with zero value.
   This call is never optimized out by the compiler. */
void psMemzeroS(void *s, psSizeL_t n)
{
    if (s)
    {
        memset_s(s, n, 0x00, n);
    }
}

/* Initialize memory with specified value.
   This call is never optimized out by the compiler. */
void *psMemsetSRR(void *s, int c, psSizeL_t n, void *ret)
{
    if (s)
    {
        memset_s(s, n, c, n);
    }
    return ret;
}

/* Initialize memory with zero value.
   This call is never optimized out by the compiler. */
void *psMemzeroSRR(void *s, psSizeL_t n, void *ret)
{
    if (s)
    {
        memset_s(s, n, 0x00, n);
    }
    return ret;
}

char *psStrdupN(const char *string)
{
    size_t len;
    char *new_str;

    if (string == NULL)
    {
        return NULL;
    }
    len = psStrlen(string);
    new_str = psMallocN(len + 1);
    if (new_str)
    {
        psMemcpy(new_str, string, len);
        new_str[len] = 0;
    }
    return new_str;
}

void psFreeN(void *ptr)
{
    psFreeNoPool(ptr);
}

/* Call free function and return specified return value. */
void *psFreeFRR(void (*free_func)(void *ptr), void *ptr, void *ret)
{
    free_func(ptr);
    return ret;
}

/* end of file psUtil.c */
