/**
 *      @file    ps_x25519.h
 *
 *
 *      Header for Matrix X25519 interface.
 */

/*****************************************************************************
* Copyright (c) 2018 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*****************************************************************************/

/******************************************************************************/

#ifndef _h_PS_X25519
# define _h_PS_X25519

# include "coreApi.h" /* Must be included first */
# ifdef MATRIX_CONFIGURATION_INCDIR_FIRST
#  include <cryptoConfig.h>   /* Must be included second */
# else
#  include "../cryptoConfig.h"   /* Must be included second */
# endif
# include "osdep-types.h"

/******************************************************************************/

/** Keylength of X25519 private key. */
#define PS_DH_X25519_PRIVATE_KEY_BYTES 32U

/** Keylength of X25519 public key. */
#define PS_DH_X25519_PUBLIC_KEY_BYTES 32U

/** Keylength of X25519 shared secret. */
#define PS_DH_X25519_SHARED_SECRET_BYTES 32U

PSPUBLIC psRes_t psDhX25519GenKey(
        unsigned char priv_key[PS_DH_X25519_PRIVATE_KEY_BYTES],
        unsigned char pub_key[PS_DH_X25519_PUBLIC_KEY_BYTES]);

PSPUBLIC psRes_t psDhX25519GenSharedSecret(
        const unsigned char peer_pub_key[PS_DH_X25519_PUBLIC_KEY_BYTES],
        const unsigned char my_priv_key[PS_DH_X25519_PRIVATE_KEY_BYTES],
        unsigned char secret[PS_DH_X25519_PUBLIC_KEY_BYTES]);

/******************************************************************************/

#endif /* _h_PS_X25519 */


