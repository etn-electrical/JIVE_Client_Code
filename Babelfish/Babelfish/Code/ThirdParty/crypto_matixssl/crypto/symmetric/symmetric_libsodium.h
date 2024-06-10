/**
 *      @file    symmetric_libsodium.h
 *
 *
 *      Header for libsodium crypto layer.
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

#ifndef _h_LIBSODIUM_SYMMETRIC
# define _h_LIBSODIUM_SYMMETRIC

# include "../cryptoApi.h"
# include "sodium.h"

/******************************************************************************/

# ifdef USE_LIBSODIUM_AES_GCM
typedef struct __attribute__((aligned(16)))
{
    crypto_aead_aes256gcm_state libSodiumCtx;
    unsigned char IV[AES_BLOCKLEN];
    unsigned char Tag[AES_BLOCKLEN];
    unsigned char Aad[crypto_aead_aes256gcm_ABYTES];   /* TODO change this length */
    uint32_t AadLen;
} psAesGcm_t;
# endif

# ifdef USE_LIBSODIUM_CHACHA20_POLY1305_IETF
typedef struct
{
    unsigned char key[crypto_aead_chacha20poly1305_ietf_KEYBYTES];
    unsigned char IV[crypto_aead_chacha20poly1305_IETF_NPUBBYTES];
    unsigned char Tag[crypto_aead_chacha20poly1305_ietf_ABYTES];
    unsigned char Aad[16];
    uint32_t AadLen;
} psChacha20Poly1305Ietf_t;
# define PS_CHACHA20POLY1305IETF_DEFINED 1
# endif

/******************************************************************************/

#endif /* _h_LIBSODIUM_SYMMETRIC */

