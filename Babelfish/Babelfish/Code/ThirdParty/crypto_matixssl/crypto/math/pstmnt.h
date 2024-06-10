/**
 *      @file    pstmnt.c
 *
 *
 *      Multiprecision number implementation: constant time montgomery.
 */
/*
 *      Copyright (c) 2013-2017 INSIDE Secure Corporation
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

#ifndef INCLUDE_GUARD_PSTMNT_H
# define INCLUDE_GUARD_PSTMNT_H

# include "../cryptoApi.h"
# include "pstm.h"

/* Before defining pstmnt, ensure pstm has been processed.
   In some configurations the pstm is disabled,
   which also disables pstmnt. */
# ifndef PSTM_AVAILABLE
#  undef USE_CONSTANT_TIME_MODEXP
# endif /* PSTM_AVAILABLE */

# ifdef USE_CONSTANT_TIME_MODEXP

/* pstmnt uses data types subtly different from pstm:
   all limbs specified in the API are always multiples of uint32_t. */
#  define pstmnt_bits uint32_t
#  define pstmnt_word uint32_t
#  define pstmnt_words uint32_t
#  define pstmnt_dword uint64_t
#  define PSTMNT_WORD_BITS 32
#  define PSTMNT_WORD_BYTES 4

/* Inline functions for conversion between types. */

static inline
const pstmnt_word *pstmnt_const_ptr(const pstm_int *A_const)
{
    return (const pstmnt_word *) A_const->dp;
}

static inline
pstmnt_word *pstmnt_ptr(pstm_int *A)
{
    return (pstmnt_word *) A->dp;
}

static inline
pstmnt_words pstmnt_size(const pstm_int *A)
{
    return ((A->used) * DIGIT_BIT) / PSTMNT_WORD_BITS;
}

static inline
unsigned int pstmnt_size_bytes(const pstm_int *A)
{
    return ((A->used) * DIGIT_BIT) / 8;
}

/* Function attributes. */

#  define PSTMNT_RESTORED /* Parameter is temporary changed during function call,
                             but restored before the function returns. */

/* Function API. */

/* Compute small inverse constant for montgomery operations. */
pstmnt_word pstmnt_neg_small_inv(const pstmnt_word *a_p);

/* Convert values to montgomery format for processing modular exponentiation. */
void
pstmnt_montgomery_input(
    const pstmnt_word Input[] /* NWords */,
    PSTMNT_RESTORED pstmnt_word Prime[] /* NWords */,
    pstmnt_word TempLarge[] /* NWords * 6 */,    /* Note: differs from
                                                    pstmnt_montgomery_input. */
    pstmnt_word Target[] /* NWords */,
    pstmnt_words NWords,
    pstmnt_word PrimeSmallInv);

/* Convert values back from montgomery format. */
void
pstmnt_montgomery_output(
    const pstmnt_word Input[] /* NWords */,
    pstmnt_word Output[] /* NWords */,
    const pstmnt_word Prime[] /* NWords */,
    pstmnt_word TempLarge[] /* NWords * 2 */,
    pstmnt_words NWords,
    pstmnt_word PrimeSmallInv);

/* Compute modular exponentiation with values in montgomery format.
   The modular exponentiation attempts to use constant time amthematical
   operations. */
void
pstmnt_mod_exp_montgomery_skip(
    const pstmnt_word a[],
    const pstmnt_word x[],
    pstmnt_word r[],
    const pstmnt_word start_bit,
    const pstmnt_word bits,
    const pstmnt_word m[],
    pstmnt_word temp[] /* len * 4 */,
    pstmnt_word mp,
    pstmnt_words len);

# endif /* INCLUDE_GUARD_PSTMNT_H */

#endif  /* USE_CONSTANT_TIME_MODEXP */

/* end of file pstmnt.h */
