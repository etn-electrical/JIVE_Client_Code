/**
 *      @file    digest_matrix.h
 *
 *
 *      Header for internal digest support.
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

#ifndef _h_MATRIX_DIGEST
# define _h_MATRIX_DIGEST

/******************************************************************************/

# ifdef USE_MATRIX_SHA1
typedef struct
{
#  ifdef HAVE_NATIVE_INT64
    uint64 length;
#  else
    uint32 lengthHi;
    uint32 lengthLo;
#  endif /* HAVE_NATIVE_INT64 */
    uint32 state[5], curlen;
    unsigned char buf[64];
} psSha1_t;
# endif

# ifdef USE_MATRIX_SHA256
typedef struct
{
#  ifdef HAVE_NATIVE_INT64
    uint64 length;
#  else
    uint32 lengthHi;
    uint32 lengthLo;
#  endif /* HAVE_NATIVE_INT64 */
    uint32 state[8], curlen;
    unsigned char buf[64];
} psSha256_t;
# endif

# ifdef USE_MATRIX_SHA512
typedef struct
{
    uint64 length, state[8];
    unsigned long curlen;
    unsigned char buf[128];
} psSha512_t;
# endif

# ifdef USE_MATRIX_SHA384
#  ifndef USE_MATRIX_SHA512
#   error "USE_MATRIX_SHA512 must be enabled if USE_MATRIX_SHA384 is enabled"
#  endif
typedef psSha512_t psSha384_t;
# endif

# ifdef USE_MATRIX_MD5
typedef struct
{
#  ifdef HAVE_NATIVE_INT64
    uint64 length;
#  else
    uint32 lengthHi;
    uint32 lengthLo;
#  endif /* HAVE_NATIVE_INT64 */
    uint32 state[4], curlen;
    unsigned char buf[64];
} psMd5_t;
# endif

# ifdef USE_MATRIX_MD5SHA1
typedef struct
{
    psMd5_t md5;
    psSha1_t sha1;
} psMd5Sha1_t;
# endif

# ifdef USE_MATRIX_MD4
typedef struct
{
#  ifdef HAVE_NATIVE_INT64
    uint64 length;
#  else
    uint32 lengthHi;
    uint32 lengthLo;
#  endif /* HAVE_NATIVE_INT64 */
    uint32 state[4], curlen;
    unsigned char buf[64];
} psMd4_t;
# endif

# ifdef USE_MATRIX_MD2
typedef struct
{
    unsigned char chksum[16], X[48], buf[16];
    uint32 curlen;
} psMd2_t;
# endif

#  ifdef USE_MATRIX_AES_CMAC
typedef struct
{
    unsigned char key[32];
    int32 keyLen;
    unsigned char subKey[16];
    uint32 IV[AES_BLOCKLEN / sizeof(uint32)];
    uint32 inputBufferCount;
    union
    {
        unsigned char Buffer[128];
        uint32 BufferAlignment;
    } Input;
} psAesCmac_t;
#  endif

/******************************************************************************/

# ifdef USE_MATRIX_HMAC_MD5
typedef struct
{
    unsigned char pad[64];
    psMd5_t md5;
} psHmacMd5_t;
# endif

# ifdef USE_MATRIX_HMAC_SHA1
typedef struct
{
    unsigned char pad[64];
    psSha1_t sha1;
} psHmacSha1_t;
# endif

# ifdef USE_MATRIX_HMAC_SHA256
typedef struct
{
    unsigned char pad[64];
    psSha256_t sha256;
} psHmacSha256_t;
# endif

# ifdef USE_MATRIX_HMAC_SHA384
typedef struct
{
    unsigned char pad[128];
    psSha384_t sha384;
} psHmacSha384_t;
# endif

#endif /* _h_MATRIX_DIGEST */

/******************************************************************************/

