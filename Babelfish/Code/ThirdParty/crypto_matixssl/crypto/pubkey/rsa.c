/**
 *      @file    rsa.c
 *
 *
 *      RSA crypto.
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

/******************************************************************************/
/* TODO - the following functions are not implementation layer specific...
    move to a common file?

    Matrix-specific starts at #ifdef USE_MATRIX_RSA
 */

#define ASN_OVERHEAD_LEN_RSA_SHA2   19
#define ASN_OVERHEAD_LEN_RSA_SHA1   15

#ifdef USE_RSA

#ifdef USE_MATRIX_RSA

/******************************************************************************/
/**
    Initialize an allocated RSA key.

    @note that in this case, a psRsaKey_t is a structure type.
    This means that the caller must have statically or dynamically allocated
    the structure before calling this Api.

    TODO, may not be necessary, since crypt apis also take pool.
    @param[in] pool The pool to use to allocate any temporary working memory
    beyond what is provided in the 'key' structure.

    @param[in,out] key A pointer to an allocated (statically or dynamically)
    key structure to be initalized as a blank RSA keypair.
 */
int32_t psRsaInitKey(psPool_t *pool, psRsaKey_t *key)
{
    if (!key)
    {
        return PS_MEM_FAIL;
    }
    Memset(key, 0x0, sizeof(psRsaKey_t));
    key->pool = pool;
    return PS_SUCCESS;
}

/*
    Zero an RSA key. The caller is responsible for freeing 'key' if it is
    allocated (or not if it is static, or stack based).
 */
void psRsaClearKey(psRsaKey_t *key)
{
    pstm_clear(&(key->N));
    pstm_clear(&(key->e));
    pstm_clear(&(key->d));
    pstm_clear(&(key->p));
    pstm_clear(&(key->q));
    pstm_clear(&(key->dP));
    pstm_clear(&(key->dQ));
    pstm_clear(&(key->qP));
    key->size = 0;
    key->optimized = 0;
    key->pool = NULL;
}

/* 'to' key digits are allocated here */
int32_t psRsaCopyKey(psRsaKey_t *to, const psRsaKey_t *from)
{
    int32_t err = 0;

    if ((err = pstm_init_copy(from->pool, &to->N, &from->N, 0)) != PSTM_OKAY)
    {
        goto error;
    }
    if ((err = pstm_init_copy(from->pool, &to->e, &from->e, 0)) != PSTM_OKAY)
    {
        goto error;
    }
    if ((err = pstm_init_copy(from->pool, &to->d, &from->d, 0)) != PSTM_OKAY)
    {
        goto error;
    }
    if ((err = pstm_init_copy(from->pool, &to->p, &from->p, 0)) != PSTM_OKAY)
    {
        goto error;
    }
    if ((err = pstm_init_copy(from->pool, &to->q, &from->q, 0)) != PSTM_OKAY)
    {
        goto error;
    }
    if ((err = pstm_init_copy(from->pool, &to->dP, &from->dP, 0)) != PSTM_OKAY)
    {
        goto error;
    }
    if ((err = pstm_init_copy(from->pool, &to->dQ, &from->dQ, 0)) != PSTM_OKAY)
    {
        goto error;
    }
    if ((err = pstm_init_copy(from->pool, &to->qP, &from->qP, 0)) != PSTM_OKAY)
    {
        goto error;
    }
    to->size = from->size;
    to->optimized = from->optimized;
    to->pool = from->pool;
error:
    if (err < 0)
    {
        psRsaClearKey(to);
    }
    return err;
}
#endif /* USE_MATRIX_RSA */

/******************************************************************************/
/**
    Get the size in bytes of the RSA public exponent.
    Eg. 128 for 1024 bit RSA keys, 256 for 2048 and 512 for 4096 bit keys.
    @param[in] key RSA key
    @return Number of bytes of public exponent.
 */
psSize_t psRsaSize(const psRsaKey_t *key)
{
    return key->size;
}

/******************************************************************************/
/**
    Compare if the public modulus and exponent is the same between two keys.

    @return < 0 on failure, >= 0 on success.
 */
int32_t psRsaCmpPubKey(const psRsaKey_t *k1, const psRsaKey_t *k2)
{
    if ((pstm_cmp(&k1->N, &k2->N) == PSTM_EQ) &&
        (pstm_cmp(&k1->e, &k2->e) == PSTM_EQ))
    {
        return PS_SUCCESS;
    }
    return PS_FAIL;
}

# ifdef OLD
/******************************************************************************/
/*
 */
static int32_t getBig(psPool_t *pool, const unsigned char **pp, psSize_t len,
    pstm_int *big)
{
    const unsigned char *p = *pp;
    psSize_t vlen;

    if (len < 1 || *(p++) != ASN_INTEGER ||
        getAsnLength(&p, len - 1, &vlen) < 0 || (len - 1) < vlen)
    {
        return PS_PARSE_FAIL;
    }
    /* Make a smart size since we know the length */
    if (pstm_init_for_read_unsigned_bin(pool, big, vlen) != PSTM_OKAY)
    {
        return PS_MEM_FAIL;
    }
    if (pstm_read_unsigned_bin(big, p, vlen) != 0)
    {
        pstm_clear(big);
        psTraceCrypto("ASN getBig failed\n");
        return PS_PARSE_FAIL;
    }
    *pp = p + vlen;
    return PS_SUCCESS;
}
# endif

#ifdef USE_MATRIX_RSA
/******************************************************************************/
/**
    Primary RSA crypto routine, with either public or private key.

    @param[in] pool Pool to use for temporary memory allocation for this op.
    @param[in] key RSA key to use for this operation.
    @param[in] in Pointer to allocated buffer to encrypt.
    @param[in] inlen Number of bytes pointed to by 'in' to encrypt.
    @param[out] out Pointer to allocated buffer to store encrypted data.
    @param[out] outlen Number of bytes written to 'out' buffer.
    @param[in] type PS_PRIVKEY or PS_PUBKEY.
    @param[in] data TODO Hardware context.

    @return 0 on success, < 0 on failure.

    @note 'out' and 'in' can be equal for in-situ operation.
 */
int32_t psRsaCrypt(psPool_t *pool, psRsaKey_t *key,
    const unsigned char *in, psSize_t inlen,
    unsigned char *out, psSize_t *outlen,
    uint8_t type, void *data)
{
    pstm_int tmp, tmpa, tmpb;
    int32_t res;
    uint32_t x;

    if (in == NULL || out == NULL || outlen == NULL || key == NULL)
    {
        psTraceCrypto("NULL parameter error in psRsaCrypt\n");
        return PS_ARG_FAIL;
    }

    tmp.dp = tmpa.dp = tmpb.dp = NULL;

# ifdef USE_MATRIX_MEMORY_MANAGEMENT
/*
    From experience we know that these size values will be used quite a bit
    when performing RSA operations.  Register with the pool handler
    for fast caching.

    Start with caches that are always used.

    This first one is based on the initial pstm_init_for_read_unsigned_bin that
    is fundamental to the smart-sizes in pstm.c
 */
    x = sizeof(pstm_digit);
    res = (((((inlen + x) / x) * (x * CHAR_BIT)) / DIGIT_BIT) + 2) * x;
    psAddPoolCache(pool, res);
    psAddPoolCache(pool, key->size);
    if (type == PS_PRIVKEY)
    {
        psAddPoolCache(pool, ((key->p.used + 2) * 2) * x);
        psAddPoolCache(pool,  key->p.alloc * x);
        psAddPoolCache(pool, ((key->p.used * 2) + 1) * x);
        psAddPoolCache(pool, (key->p.used * 3 * x));
    }
# endif /* USE_MATRIX_MEMORY_MANAGEMENT */
        /* Init and copy into tmp */
    if (pstm_init_for_read_unsigned_bin(pool, &tmp, inlen + sizeof(pstm_digit))
        != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
    if (pstm_read_unsigned_bin(&tmp, (unsigned char *) in, inlen) != PS_SUCCESS)
    {
        pstm_clear(&tmp);
        return PS_FAILURE;
    }
    /* Sanity check on the input */
    if (pstm_cmp(&key->N, &tmp) == PSTM_LT)
    {
        res = PS_LIMIT_FAIL;
        goto done;
    }
    if (type == PS_PRIVKEY)
    {
        if (key->optimized)
        {
            if (pstm_init_size(pool, &tmpa, key->p.alloc) != PS_SUCCESS)
            {
                res = PS_FAILURE;
                goto done;
            }
            if (pstm_init_size(pool, &tmpb, key->q.alloc) != PS_SUCCESS)
            {
                pstm_clear(&tmpa);
                res = PS_FAILURE;
                goto done;
            }
            if (pstm_exptmod(pool, &tmp, &key->dP, &key->p, &tmpa) !=
                PS_SUCCESS)
            {
                psTraceCrypto("decrypt error: pstm_exptmod dP, p\n");
                goto error;
            }
            if (pstm_exptmod(pool, &tmp, &key->dQ, &key->q, &tmpb) !=
                PS_SUCCESS)
            {
                psTraceCrypto("decrypt error: pstm_exptmod dQ, q\n");
                goto error;
            }
            if (pstm_sub(&tmpa, &tmpb, &tmp) != PS_SUCCESS)
            {
                psTraceCrypto("decrypt error: sub tmpb, tmp\n");
                goto error;
            }
            if (pstm_mulmod(pool, &tmp, &key->qP, &key->p, &tmp) != PS_SUCCESS)
            {
                psTraceCrypto("decrypt error: pstm_mulmod qP, p\n");
                goto error;
            }
            if (pstm_mul_comba(pool, &tmp, &key->q, &tmp, NULL, 0)
                != PS_SUCCESS)
            {
                psTraceCrypto("decrypt error: pstm_mul q \n");
                goto error;
            }
            if (pstm_add(&tmp, &tmpb, &tmp) != PS_SUCCESS)
            {
                psTraceCrypto("decrypt error: pstm_add tmp \n");
                goto error;
            }
        }
        else
        {
            if (pstm_exptmod(pool, &tmp, &key->d, &key->N, &tmp) !=
                PS_SUCCESS)
            {
                psTraceCrypto("psRsaCrypt error: pstm_exptmod\n");
                goto error;
            }
        }
    }
    else if (type == PS_PUBKEY)
    {
        if (pstm_exptmod(pool, &tmp, &key->e, &key->N, &tmp) != PS_SUCCESS)
        {
            psTraceCrypto("psRsaCrypt error: pstm_exptmod\n");
            goto error;
        }
    }
    else
    {
        psTraceCrypto("psRsaCrypt error: invalid type param\n");
        goto error;
    }
    /* Read it back */
    x = pstm_unsigned_bin_size(&key->N);

    if ((uint32) x > *outlen)
    {
        res = -1;
        psTraceCrypto("psRsaCrypt error: pstm_unsigned_bin_size\n");
        goto done;
    }
    /* We want the encrypted value to always be the key size.  Pad with 0x0 */
    while ((uint32) x < (unsigned long) key->size)
    {
        *out++ = 0x0;
        x++;
    }

    *outlen = x;
    /* Convert it */
    Memset(out, 0x0, x);

    if (pstm_to_unsigned_bin(pool, &tmp, out + (x - pstm_unsigned_bin_size(&tmp)))
        != PS_SUCCESS)
    {
        psTraceCrypto("psRsaCrypt error: pstm_to_unsigned_bin\n");
        goto error;
    }
    /* Clean up and return */
    res = PS_SUCCESS;
    goto done;
error:
    res = PS_FAILURE;
done:
    if (type == PS_PRIVKEY && key->optimized)
    {
        pstm_clear_multi(&tmpa, &tmpb, NULL, NULL, NULL, NULL, NULL, NULL);
    }
    pstm_clear(&tmp);
    return res;
}

#endif  /* USE_MATRIX_RSA */
#endif /* USE_RSA */

/******************************************************************************/

