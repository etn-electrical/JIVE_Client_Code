/**
 *      @file    ps_x25519.c
 *
 *
 *      Implementation for Matrix X25519 interface.
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

#include "ps_x25519.h"
#include "include/sodium/crypto_scalarmult.h"
#include "../cryptoApi.h"

#ifdef USE_MATRIX_X25519

psRes_t psDhX25519GenSharedSecret(
        const unsigned char peer_pub_key[PS_DH_X25519_PUBLIC_KEY_BYTES],
        const unsigned char my_priv_key[PS_DH_X25519_PRIVATE_KEY_BYTES],
        unsigned char secret[PS_DH_X25519_PUBLIC_KEY_BYTES])
{
    int res = psSodium_crypto_scalarmult(secret, my_priv_key, peer_pub_key);
    if (res == 0)
    {
        return PS_SUCCESS;
    }
    return PS_FAILURE;
}

# ifdef USE_PS_CRYPTO_SCALARMULT_BASE
/* Use special function for multiplication with the known base.
   This path enables some optimizations. */
psRes_t psDhX25519GenKey(
        unsigned char priv_key[PS_DH_X25519_PRIVATE_KEY_BYTES],
        unsigned char pub_key[PS_DH_X25519_PUBLIC_KEY_BYTES])
{
    psResSize_t rvs;

    rvs = psGetPrngLocked(priv_key, PS_DH_X25519_PRIVATE_KEY_BYTES, NULL);
    if (rvs == PS_DH_X25519_PRIVATE_KEY_BYTES)
    {
        int res = psSodium_crypto_scalarmult_base(pub_key, priv_key);
        if (res == 0)
        {
            return PS_SUCCESS;
        }
        return PS_FAILURE;
    }
    return rvs < 0 ? (psRes_t) rvs : PS_FAILURE;
}
# else
/* Use the shared secret generation function also for key generation. */
psRes_t psDhX25519GenKey(
        unsigned char priv_key[PS_DH_X25519_PRIVATE_KEY_BYTES],
        unsigned char pub_key[PS_DH_X25519_PUBLIC_KEY_BYTES])
{
    psResSize_t rvs;
    const unsigned char base_point[PS_DH_X25519_PRIVATE_KEY_BYTES] = {
        9, /* 0, ... */
    };

    rvs = psGetPrngLocked(priv_key, PS_DH_X25519_PRIVATE_KEY_BYTES, NULL);
    if (rvs == PS_DH_X25519_PRIVATE_KEY_BYTES)
    {
        return psDhX25519GenSharedSecret(base_point, priv_key, pub_key);
    }
    return rvs < 0 ? (psRes_t) rvs : PS_FAILURE;
}
# endif

#endif /* USE_MATRIX_X25519 */

/******************************************************************************/

