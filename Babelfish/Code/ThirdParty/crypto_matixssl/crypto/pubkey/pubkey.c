/**
 *      @file    pubkey.c
 *
 *
 *      Public and Private key operations shared by crypto implementations.
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

#include "../cryptoImpl.h"

# if defined(USE_RSA) || defined(USE_ECC) || defined(USE_DH) || defined(USE_X25519) || defined(USE_ED25519)

/******************************************************************************/

int32_t psInitPubKey(psPool_t *pool, psPubKey_t *key, uint8_t type)
{
    if (!key)
    {
        return PS_ARG_FAIL;
    }
    switch (type)
    {
# ifdef USE_RSA
    case PS_RSA:
        psRsaInitKey(pool, &key->key.rsa);
        break;
# endif
# ifdef USE_ECC
    case PS_ECC:
        psEccInitKey(pool, &key->key.ecc, NULL);
        break;
# endif
    default:
        break;
    }
    key->pool = pool;
    key->type = type;
    key->keysize = 0;
    return PS_SUCCESS;
}

/******************************************************************************/

void psClearPubKey(psPubKey_t *key)
{
    if (!key)
    {
        return;
    }
    switch (key->type)
    {
# ifdef USE_RSA
    case PS_RSA:
        psRsaClearKey(&key->key.rsa);
        break;
# endif
# ifdef USE_ECC
    case PS_ECC:
        psEccClearKey(&key->key.ecc);
        break;
# endif
# ifdef USE_DH
    case PS_DH:
        psDhClearKey(&key->key.dh);
        break;
# endif
    default:
        break;
    }
    key->pool = NULL;
    key->keysize = 0;
    key->type = 0;
}

int32_t psNewPubKey(psPool_t *pool, uint8_t type, psPubKey_t **key)
{
    int32_t rc;

    if ((*key = psMalloc(pool, sizeof(psPubKey_t))) == NULL)
    {
        return PS_MEM_FAIL;
    }

    if ((rc = psInitPubKey(pool, *key, type)) < 0)
    {
        psFree(*key, pool);
    }
    return rc;
}

void psDeletePubKey(psPubKey_t **key)
{
# ifdef USE_MATRIX_MEMORY_MANAGEMENT
    psPool_t *pool;
    pool = (*key)->pool;
    psClearPubKey(*key);
    psFree(*key, pool);
# else
    psClearPubKey(*key);
    psFree(*key, NULL);
# endif
    *key = NULL;
}

/******************************************************************************/

#endif /* USE_RSA || USE_ECC */
