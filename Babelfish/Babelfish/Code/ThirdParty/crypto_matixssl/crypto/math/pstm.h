/**
 *      @file    pstm.h
 *
 *
 *      multiple-precision integer library.
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

#ifndef _h_PSTMATH
# define _h_PSTMATH

# include "../cryptoApi.h"

# if defined(USE_MATRIX_RSA) || defined(USE_MATRIX_ECC) || defined(USE_MATRIX_DH) || defined(USE_CL_RSA) || defined(USE_CL_DH) || defined(USE_QUICK_ASSIST_RSA) || defined(USE_QUICK_ASSIST_ECC)

#  if defined(PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED) && defined(PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM)
#   error "May only enable either PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED or PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM"
#  endif

#  if !defined(PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED) && !defined(PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM)
#   define PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM
#  endif

#  ifdef PS_PUBKEY_OPTIMIZE_FOR_SMALLER_RAM
#   define PS_EXPTMOD_WINSIZE      3
#  endif

#  ifdef PS_PUBKEY_OPTIMIZE_FOR_FASTER_SPEED
#   define PS_EXPTMOD_WINSIZE      5
#  endif

/******************************************************************************/
/* Define this here to avoid including circular limits.h on some platforms */
#  ifndef CHAR_BIT
#   define CHAR_BIT   8
#  endif

/******************************************************************************/
/*
    If native 64 bit integers are not supported, we do not support 32x32->64
    in hardware, so we must set the 16 bit flag to produce 16x16->32 products.
 */
#  ifndef HAVE_NATIVE_INT64
#   define PSTM_16BIT
#  endif /* ! HAVE_NATIVE_INT64 */

/******************************************************************************/
/*
    Some default configurations.

    pstm_word should be the largest value the processor can hold as the product
        of a multiplication. Most platforms support a 32x32->64 MAC instruction,
        so 64bits is the default pstm_word size.
    pstm_digit should be half the size of pstm_word
 */
#  ifdef PSTM_8BIT
/*      8-bit digits, 16-bit word products */
typedef unsigned char pstm_digit;
typedef unsigned short pstm_word;
#   define DIGIT_BIT           8

#  elif defined(PSTM_16BIT)
/*      16-bit digits, 32-bit word products */
typedef unsigned short pstm_digit;
typedef unsigned long pstm_word;
#   define DIGIT_BIT           16

#  elif defined(PSTM_64BIT)
/*      64-bit digits, 128-bit word products */
#   ifndef __GNUC__
#    error "64bit digits requires GCC"
#   endif
typedef unsigned long pstm_digit;
typedef unsigned long pstm_word __attribute__ ((mode(TI)));
#   define DIGIT_BIT           64

#  else
/*      This is the default case, 32-bit digits, 64-bit word products */
typedef uint32 pstm_digit;
typedef uint64 pstm_word;
#   define DIGIT_BIT       32
#   define PSTM_32BIT
#  endif /* digit and word size */

#  define PSTM_MASK           (pstm_digit) (-1)
#  define PSTM_DIGIT_MAX      PSTM_MASK

/******************************************************************************/
/*
    equalities
 */
#  define PSTM_LT         -1    /* less than */
#  define PSTM_EQ         0     /* equal to */
#  define PSTM_GT         1     /* greater than */

#  define PSTM_ZPOS       0     /* pstm_int.sign positive */
#  define PSTM_NEG        1     /* pstm_int.sign negative */

#  define PSTM_OKAY       PS_SUCCESS
#  define PSTM_MEM        PS_MEM_FAIL
#  define PSTM_AVAILABLE  1

/* replies */
#  define PSTM_YES        1 /* yes response */
#  define PSTM_NO         0 /* no response */


/******************************************************************************/
/*      This is the maximum size that pstm_int.alloc can be for crypto operations.
    Effectively, it is three times the size of the largest private key. */
#  ifdef USE_LARGE_DH_GROUPS
#   define PSTM_MAX_SIZE   ((8192 / DIGIT_BIT) * 3)
#  else
#   define PSTM_MAX_SIZE   ((4096 / DIGIT_BIT) * 3)
#endif

typedef struct
{
    pstm_digit *dp;
    psPool_t *pool;
#  if defined USE_COMPACT_PSTM_INT
    /* Save a little space if requested. */
    uint32_t used : 12,
             alloc : 12,
             sign : 1;
#  else
    uint16_t used;
    uint16_t alloc;
    uint8_t sign;
#  endif
} pstm_int;

/******************************************************************************/
/*
    Operations on large integers
 */
#  define pstm_iszero(a) (((a)->used == 0) ? PS_TRUE : PS_FALSE)
#  define pstm_iseven(a) (((a)->used > 0 && (((a)->dp[0] & 1) == 0)) ? PS_TRUE : PS_FALSE)
#  define pstm_isodd(a)  (((a)->used > 0 && (((a)->dp[0] & 1) == 1)) ? PS_TRUE : PS_FALSE)

extern void pstm_set(pstm_int *a, pstm_digit b);
extern void pstm_zero(pstm_int *a);

extern int32_t pstm_init(psPool_t *pool, pstm_int *a);
extern int32_t pstm_init_size(psPool_t *pool, pstm_int *a, psSize_t size);
extern int32_t pstm_init_copy(psPool_t *pool, pstm_int *a, const pstm_int *b,
                              uint8_t toSqr);
extern int32_t pstm_init_for_read_unsigned_bin(psPool_t *pool, pstm_int *a,
                                               psSize_t len);

extern int32_t pstm_grow(pstm_int *a, psSize_t size);
extern void pstm_clamp(pstm_int *a);

extern int32_t pstm_copy(const pstm_int *a, pstm_int *b);
extern void pstm_exch(pstm_int *a, pstm_int *b);
extern int32_t pstm_abs(const pstm_int *a, pstm_int *b);

extern void pstm_clear(pstm_int *a);
extern void pstm_clear_multi(
    pstm_int *mp0, pstm_int *mp1, pstm_int *mp2, pstm_int *mp3,
    pstm_int *mp4, pstm_int *mp5, pstm_int *mp6, pstm_int *mp7);

extern uint16_t pstm_unsigned_bin_size(const pstm_int *a);
extern uint16_t pstm_unsigned_bin_size_nullsafe(const pstm_int *a);
extern uint16_t pstm_count_bits(const pstm_int *a);

extern int32_t pstm_read_unsigned_bin(pstm_int *a,
                                      const unsigned char *buf, psSize_t len);
extern int32_t pstm_read_asn(psPool_t *pool, const unsigned char **pp, psSize_t len,
                             pstm_int *a);
extern int32_t pstm_to_unsigned_bin(psPool_t *pool, const pstm_int *a,
                                    unsigned char *b);
extern int32_t pstm_to_unsigned_bin_nr(psPool_t *pool, const pstm_int *a,
                                       unsigned char *b);
extern unsigned char *pstm_to_unsigned_bin_alloc(psPool_t *pool,
                                                 const pstm_int *a);
# ifdef USE_CERT_GEN
extern int32_t pstm_to_base128_be_unsigned_bin(psPool_t *pool, const pstm_int *in,
        unsigned char *out, size_t *out_len);
# endif
#  if defined(USE_ECC) || defined(USE_CERT_GEN)
extern int32_t pstm_read_radix(psPool_t *pool, pstm_int *a,
                               const char *buf, psSize_t len, uint8_t radix);
#  endif

extern int32_t pstm_cmp(const pstm_int *a, const pstm_int *b);
extern int32_t pstm_cmp_mag(const pstm_int *a, const pstm_int *b);
extern int32_t pstm_cmp_d(const pstm_int *a, pstm_digit b);

extern void pstm_rshd(pstm_int *a, uint16_t b);
extern int32_t pstm_lshd(pstm_int *a, uint16_t b);

extern int32_t pstm_add(const pstm_int *a, const pstm_int *b, pstm_int *c);
extern int32_t pstm_add_d(psPool_t *pool, const pstm_int *a, pstm_digit b, pstm_int *c);

extern int32_t pstm_sub_s(const pstm_int *a, const pstm_int *b, pstm_int *c);
extern int32_t pstm_sub(const pstm_int *a, const pstm_int *b, pstm_int *c);
extern int32_t pstm_sub_d(psPool_t *pool, const pstm_int *a, pstm_digit b, pstm_int *c);

extern int32_t pstm_div(psPool_t *pool, const pstm_int *a, const pstm_int *b,
                        pstm_int *c, pstm_int *d);
extern int32_t pstm_div_2d(psPool_t *pool, const pstm_int *a, int16_t b,
                           pstm_int *c, pstm_int *d);
extern int32_t pstm_div_2(const pstm_int *a, pstm_int *b);

extern int32_t pstm_mod(psPool_t *pool, const pstm_int *a, const pstm_int *b,
                        pstm_int *c);
extern int32_t pstm_invmod(psPool_t *pool, const pstm_int *a, const pstm_int *b,
                           pstm_int *c);

extern int32_t pstm_mul_2(const pstm_int *a, pstm_int *b);
extern int32_t pstm_mulmod(psPool_t *pool, const pstm_int *a, const pstm_int *b,
                           const pstm_int *c, pstm_int *d);
extern int32_t pstm_mul_comba(psPool_t *pool, const pstm_int *A, const pstm_int *B,
                              pstm_int *C, pstm_digit *paD, psSize_t paDlen);
extern int32_t pstm_mul_d(const pstm_int *a, const pstm_digit b, pstm_int *c);

extern int32_t pstm_sqr_comba(psPool_t *pool, const pstm_int *A, pstm_int *B,
                              pstm_digit *paD, psSize_t paDlen);

extern int32_t pstm_exptmod(psPool_t *pool, const pstm_int *G, const pstm_int *X,
                            const pstm_int *P, pstm_int *Y);
extern int32_t pstm_2expt(pstm_int *a, int16_t b);

extern int32_t pstm_montgomery_setup(const pstm_int *a, pstm_digit *rho);
extern int32_t pstm_montgomery_reduce(psPool_t *pool, pstm_int *a, const pstm_int *m,
                                      pstm_digit mp, pstm_digit *paD, psSize_t paDlen);
extern int32_t pstm_montgomery_calc_normalization(pstm_int *a, const pstm_int *b);

#  if defined(USE_CERT_GEN) && defined(USE_MATRIX_RSA)
extern int32_t pstm_cnt_lsb(const pstm_int *a);
extern int32_t pstm_gcd(psPool_t *pool, const pstm_int *a, const pstm_int *b,
                        pstm_int *c);
extern int32_t pstm_lcm(psPool_t *pool, const pstm_int *a, const pstm_int *b,
                        pstm_int *c);
extern int32_t pstm_isprime(psPool_t *pool, const pstm_int *a, int32_t *result);
#  endif

# endif /* USE_MATRIX_RSA || USE_MATRIX_ECC || USE_MATRIX_DH || USE_CL_RSA || USE_CL_DH || USE_QUICK_ASSIST_RSA || USE_QUICK_ASSIST_ECC */

#endif  /* _h_PSTMATH */

