/**
 *      @file    pubkey_parse_mem.c
 *
 *
 *      Generic public and private key parsing from memory.
 */
/*
 *      Copyright (c) 2013-2018 INSIDE Secure Corporation
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

#if defined(USE_RSA) || defined(USE_ECC)
#ifdef USE_PRIVATE_KEY_PARSING

#ifdef USE_RSA
static psBool_t possiblyRSAKey(const unsigned char *keyBuf,
        int32 keyBufLen)
{
    /* Guess if this can be RSA key based on length of encoding and content.
       Even the smallest (obsolete 512-bit modulus) RSA private keys are >
       256 bytes.
    */
    return (keyBufLen > 256 && keyBuf[0] == 0x30 && keyBuf[1] >= 0x82);
}
#endif /* USE_RSA */

/*
  Helpers for trial and error private key parse
  Return codes:
  1 RSA key
  2 ECC key
  3 ED25519 key
  < 0 error
 */
static int32_t psTryParsePrivKeyMem(psPool_t *pool,
        const unsigned char *keyBuf,
        int32 keyBufLen,
        const char *password,
        psPubKey_t *privKey)
{
    int32_t rc;

#ifdef USE_RSA
    /* Examine data to ensure parses which could not succeed are not tried. */
    if (possiblyRSAKey(keyBuf, keyBufLen)) {
#  ifdef USE_ROT_RSA
        if (privKey->rotSigAlg == 0)
        {
            privKey->key.rsa.rotSigAlg = OID_SHA256_RSA_SIG;
        }
        else
        {
            privKey->key.rsa.rotSigAlg = privKey->rotSigAlg;
        }
#  endif
        rc = psRsaParsePkcs1PrivKey(pool,
                keyBuf,
                keyBufLen,
                &privKey->key.rsa);
        if (rc >= PS_SUCCESS)
        {
            privKey->type = PS_RSA;
            privKey->keysize = psRsaSize(&privKey->key.rsa);
            privKey->pool = pool;
            return PS_RSA;
        }
    }
#endif /* USE_RSA */

#ifdef USE_ECC
    rc = psEccParsePrivKey(pool,
            keyBuf,
            keyBufLen,
            &privKey->key.ecc,
            NULL);
    if (rc >= PS_SUCCESS)
    {
        privKey->type = PS_ECC;
        privKey->keysize = psEccSize(&privKey->key.ecc);
        privKey->pool = pool;
        return PS_ECC;
    }
# ifdef USE_ED25519
    rc = psEd25519ParsePrivKey(pool, keyBuf, keyBufLen,
            &privKey->key.ed25519);
    if (rc >= PS_SUCCESS)
    {
        privKey->type = PS_ED25519;
        privKey->keysize = 32;
        privKey->pool = pool;
        return PS_ED25519;
    }
# endif /* USE_ED25519 */
#endif /* USE_ECC */

#ifdef USE_PKCS8
     if (psPkcs8ParsePrivBin(pool, keyBuf, keyBufLen,
         (char*)password, privKey) >= PS_SUCCESS)
     {
# ifdef USE_RSA
         if (privKey->type == PS_RSA)
         {
             return PS_RSA;
         }
# endif /* USE_RSA */

# ifdef USE_ECC
         if (privKey->type == PS_ECC)
         {
             return PS_ECC;
         }
# endif /* USE_ECC */

         return -1; /* Success, but keytype unknown? */
     }
#endif /* USE_PKCS8 */

    return -1; /* Error */
}

/*
  Trial and error private key parse for when ECC or RSA is unknown.
  keyBuf must point to a buffer of length keyBufLen, containing
  a DER-encoded key.

  Return codes:
  1 RSA key
  2 ECC key
  3 ED25519 key
  < 0 error
 */
int32_t psParseUnknownPrivKeyMem(psPool_t *pool,
        const unsigned char *keyBuf,
        int32 keyBufLen,
        const char *password,
        psPubKey_t *privKey)
{
    int32_t keytype;

    if (keyBuf == NULL || keyBufLen <= 0)
        return PS_ARG_FAIL;

     privKey->keysize = 0;

     keytype = psTryParsePrivKeyMem(pool,
             keyBuf,
             keyBufLen,
             password,
             privKey);

     if (keytype < 0)
     {
         psTraceCrypto("Unable to parse private key. " \
                 "Supported formats are RSAPrivateKey, " \
                 "ECPrivateKey and PKCS #8.\n");
         return PS_FAILURE;
     }

     return keytype;
}

int32_t
psParseSubjectPublicKeyInfo(psPool_t *pool,
        const unsigned char *in,
        psSizeL_t inLen,
        int32_t *algId,
        unsigned char **algIdParams,
        psSizeL_t *algIdParamsLen,
        const unsigned char **pubKeyBitString)
{
    const unsigned char *p = in;
    const unsigned char *end = in + inLen;
    psSize_t seqLen, paramLen;
    int32_t rc, oid;
    unsigned char *pubKeyStart = NULL;
    unsigned char *paramsStart = NULL;

    /*
      PublicKeyInfo ::= SEQUENCE {
      algorithm       AlgorithmIdentifier,
      PublicKey       BIT STRING
      }

      AlgorithmIdentifier ::= SEQUENCE {
      algorithm       OBJECT IDENTIFIER,
      parameters      ANY DEFINED BY algorithm OPTIONAL
      }
    */

    if (in == NULL || algId == NULL || pubKeyBitString == NULL)
    {
        return PS_ARG_FAIL;
    }

    rc = getAsnSequence(&p, (int32) (end - p), &seqLen);
    if (rc < 0)
    {
        psTraceCrypto("psParseSubjectPublicKeyInfo: initial SEQ parse fail\n");
        goto out_fail;
    }
    rc = getAsnAlgorithmIdentifier(&p, (int32) (end - p), &oid, &paramLen);
    if (rc < 0)
    {
        psTraceCrypto("psParseSubjectPublicKeyInfo: alg OID parse fail\n");
        goto out_fail;
    }
    if (paramLen > 0)
    {
        paramsStart = (unsigned char*)p;
        p += paramLen;
    }
    if (p >= end)
    {
        psTraceCrypto("psParseSubjectPublicKeyInfo: invalid length\n");
        goto out_fail;
    }

    pubKeyStart = (unsigned char*)p;

    *algId = oid;
    *pubKeyBitString = pubKeyStart;

    /* Only give out params if caller is interested. */
    if (algIdParams != NULL)
    {
        *algIdParams = paramsStart;
    }
    if (algIdParamsLen != NULL)
    {
        *algIdParamsLen = paramLen;
    }

    return PS_SUCCESS;

out_fail:
    return PS_PARSE_FAIL;
}

PSPUBLIC int32_t
psParseUnknownPubKeyMem(psPool_t *pool,
                        const unsigned char *keyBuf, int32 keyBufLen,
                        const char *password, psPubKey_t *pubkey)
{
    int32_t rc;
    unsigned char *data;
    psSizeL_t data_len;
    psBool_t mustFreeData = PS_TRUE;
# if defined USE_RSA || defined USE_ECC
    unsigned char hashBuf[SHA1_HASH_SIZE];
# endif

    rc = psPemTryDecode(pool,
            keyBuf,
            keyBufLen,
            PEM_TYPE_PUBLIC_KEY,
            password,
            &data,
            &data_len);
    if (rc != PS_SUCCESS)
    {
        /* Not PEM or PEM decoding not supported. Try DER. */
        data = (unsigned char *)keyBuf;
        data_len = keyBufLen;
        mustFreeData = PS_FALSE;
    }

# ifdef USE_RSA
    rc = psRsaParseAsnPubKey(pool,
                             (const unsigned char **)&data, data_len,
                             &pubkey->key.rsa,
                             hashBuf);
    if (rc == PS_SUCCESS)
    {
        pubkey->type = PS_RSA;
    }
# endif
# ifdef USE_ECC
    if (rc < PS_SUCCESS)
    {
        const unsigned char *datac = data;
        const unsigned char *end;

        /* Typical ECC public key follows this structure (subset of X.509 certificate). */
        /*
          SubjectPublicKeyInfo  ::=  SEQUENCE  {
          algorithm                       AlgorithmIdentifier,
          subjectPublicKey        BIT STRING      }
        */

        /* The sequence and algorithm identifier are considered optional. */
        if (data_len > 0 && *datac == (ASN_CONSTRUCTED | ASN_SEQUENCE))
        {
            int32 pubKeyAlgorithm;
            psSize_t plen;

            if ((rc = getAsnSequence(&datac, (uint32) data_len, &plen)) < 0)
            {
                psTraceCrypto("Couldn't get ASN sequence for pubKeyAlgorithm\n");
                goto exit_free_data;
            }
            data_len = plen;
            /* We will just parse one SEQUENCE.
               This will allow compatibility with inputs containing multiple
               keys or other inputs like certificate after the first key. */
            end = datac + data_len;
            if ((rc = getAsnAlgorithmIdentifier(&datac, (uint32) data_len,
                                                &pubKeyAlgorithm, &plen)) < 0)
            {
                psTraceCrypto("Couldn't parse algorithm id for pubKeyAlgorithm\n");
                goto exit_free_data;
            }
            data_len = (psSizeL_t) (end - datac);
        }
        rc = getEcPubKey(pool, &datac, data_len,
                         &pubkey->key.ecc, hashBuf);
        if (rc == PS_SUCCESS)
        {
            /* keysize will be the size of the public ecc key (2 * privateLen) */
            pubkey->keysize = psEccSize(&pubkey->key.ecc);
            if (pubkey->keysize < (MIN_ECC_BITS / 8))
            {
                /* Ensure correct key size. */
                psTraceIntCrypto("ECC key size < %d\n", MIN_ECC_BITS);
                psClearPubKey(pubkey);
                rc = PS_PARSE_FAIL;
                goto exit_free_data;
            }
        }
        /* Fallback: parse Ecc private key structure if
           algorithm identifier was not found and getEcPubKey failed. */
        if (rc < PS_SUCCESS && datac == keyBuf)
        {
            rc = psEccParsePrivKey(pool, data, data_len,
                                   &pubkey->key.ecc, NULL);
        }
        if (rc == PS_SUCCESS)
        {
            pubkey->type = PS_ECC;
        }
    }
exit_free_data:
# endif


    if (mustFreeData)
    {
        psAssert(data != keyBuf);
        psFree(data, pool);
    }

    return rc;
}

# else /* USE_PRIVATE_KEY_PARSING */
PSPUBLIC int32_t psParseUnknownPrivKeyMem(psPool_t *pool,
        const unsigned char *keyBuf, int32 keyBufLen,
        const char *password, psPubKey_t *privKey)
{
    PS_VARIABLE_SET_BUT_UNUSED(pool);
    PS_VARIABLE_SET_BUT_UNUSED(keyBuf);
    PS_VARIABLE_SET_BUT_UNUSED(keyBufLen);
    PS_VARIABLE_SET_BUT_UNUSED(password);
    PS_VARIABLE_SET_BUT_UNUSED(privKey);
    return -1; /* Not implemented */
}
# endif   /* USE_PRIVATE_KEY_PARSING */
#endif /* USE_RSA || USE_ECC */
