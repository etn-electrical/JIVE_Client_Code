/* exampleExtCvSigModule.c
 *
 * Example implementation of the psExt* API for external signature
 * generation for the CertificateVerify message. This implementation
 * uses Matrix Crypto to compute the signature.
 */

/*****************************************************************************
* Copyright (c) 2017 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://essoemsupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail to
* ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#include "matrixssl/matrixsslApi.h"
#include "psUtil.h" /* For psAssert. */

#ifdef USE_EXT_CERTIFICATE_VERIFY_SIGNING
/*
   @note psExt.h is included in matrixsslApi.h
 */
# ifdef USE_EXT_EXAMPLE_MODULE
/*
   This is just an example, so include all possible keys;
   don't worry about code size.
 */
#  include "testkeys/RSA/1024_RSA.h"
#  include "testkeys/RSA/1024_RSA_KEY.h"
#  include "testkeys/RSA/2048_RSA.h"
#  include "testkeys/RSA/2048_RSA_KEY.h"
#  include "testkeys/RSA/3072_RSA.h"
#  include "testkeys/RSA/3072_RSA_KEY.h"
#  include "testkeys/RSA/4096_RSA.h"
#  include "testkeys/RSA/4096_RSA_KEY.h"
#  include "testkeys/EC/192_EC.h"
#  include "testkeys/EC/192_EC_KEY.h"
#  include "testkeys/EC/224_EC.h"
#  include "testkeys/EC/224_EC_KEY.h"
#  include "testkeys/EC/256_EC.h"
#  include "testkeys/EC/256_EC_KEY.h"
#  include "testkeys/EC/384_EC.h"
#  include "testkeys/EC/384_EC_KEY.h"
#  include "testkeys/EC/521_EC.h"
#  include "testkeys/EC/521_EC_KEY.h"
#  include "testkeys/ECDH_RSA/256_ECDH-RSA_KEY.h"
#  include "testkeys/ECDH_RSA/521_ECDH-RSA_KEY.h"
#  ifdef USE_RSA
static psRsaKey_t rsa_key;
static int load_rsa_key(unsigned int id);
#  endif
#  ifdef USE_ECC
static psEccKey_t ecc_key;
static int load_ecc_key(unsigned int id);
#  endif

static psBool_t key_loaded = PS_FALSE;
static unsigned int loaded_cert_key_id;
static int32_t key_type;

int psExtLoadCert(unsigned int cert_key_id,
    unsigned char **cert_buf,
    size_t *cert_len)
{
    return PS_FAILURE;
}

#  ifdef USE_RSA
int psExtRsaSignHash(unsigned int cert_key_id,
    unsigned char *hash_to_sign,
    size_t hash_len,
    unsigned char **signature,
    size_t *signature_len)
{
    int32_t rc;
    unsigned char sig[512] = { 0 };
    size_t sig_len = sizeof(sig);

    rc = load_rsa_key(cert_key_id);
    if (rc < 0)
    {
        return rc;
    }

    psAssert(rsa_key.size >= 128 && rsa_key.size <= 512);
    sig_len = rsa_key.size;

    *signature = psMalloc(NULL, sig_len);
    if (*signature == NULL)
    {
        return PS_MEM_FAIL;
    }

    if (hash_len == 36)
    {
        /* TLS <1.2 */
        rc = psRsaEncryptPriv(NULL,
            &rsa_key,
            hash_to_sign,
            hash_len,
            sig,
            sig_len,
            NULL);
    }
    else
    {
        /* TLS 1.2 */
        rc = privRsaEncryptSignedElement(NULL,
            &rsa_key,
            hash_to_sign,
            hash_len,
            sig,
            sig_len,
            NULL);
    }
    if (rc < 0)
    {
        Printf("privRsaEncryptSignedElement failed: %d\n", rc);
        psFree(*signature, NULL);
        return PS_FAILURE;
    }

    Memcpy(*signature, sig, sig_len);
    *signature_len = sig_len;

    return PS_SUCCESS;
}
#  endif /* USE_RSA */

#  ifdef USE_ECC
int psExtEcdsaSignHash(unsigned int cert_key_id,
    unsigned char *hash_to_sign,
    size_t hash_len,
    unsigned char **signature,
    size_t *signature_len)
{
    int32_t rc;
    unsigned char sig[512] = { 0 };
    size_t sig_len = sizeof(sig);

    rc = load_ecc_key(cert_key_id);
    if (rc < 0)
    {
        return rc;
    }

    psAssert(ecc_key.curve->size >= 24 &&
        ecc_key.curve->size <= 66);

/*
   @note: This will alloc a lot more than needed.
 */
    *signature = psMalloc(NULL, sig_len);
    if (*signature == NULL)
    {
        return PS_MEM_FAIL;
    }

    rc = psEccDsaSign(NULL,
        &ecc_key,
        hash_to_sign,
        (uint16_t) hash_len,
        sig,
        (uint16_t *) &sig_len,
        0,
        NULL);

    if (rc < 0)
    {
        Printf("privRsaEncryptSignedElement failed: %d\n", rc);
        psFree(*signature, NULL);
        return PS_FAILURE;
    }

    Memcpy(*signature, sig, sig_len);
    *signature_len = sig_len;

    return PS_SUCCESS;
}
#  endif /* USE_ECC */

#  ifdef USE_RSA
static int load_rsa_key(unsigned int id)
{
    int32 rc;
    const static unsigned char *key_buf;
    int32 key_buf_len;

    switch (id)
    {
    case CERT_KEY_ID_RSA1024:
        Printf("Example ext module using 1024 bit RSA private key\n");
        key_buf = RSA1024KEY;
        key_buf_len = sizeof(RSA1024KEY);
        break;
    case CERT_KEY_ID_RSA2048:
        Printf("Example ext module using 2048 bit RSA private key\n");
        key_buf = RSA2048KEY;
        key_buf_len = sizeof(RSA2048KEY);
        break;
    case CERT_KEY_ID_RSA3072:
        Printf("Example ext module using 3072 bit RSA private key\n");
        key_buf = RSA3072KEY;
        key_buf_len = sizeof(RSA3072KEY);
        break;
    case CERT_KEY_ID_RSA4096:
        Printf("Example ext module using 4096 bit RSA private key\n");
        key_buf = RSA4096KEY;
        key_buf_len = sizeof(RSA4096KEY);
        break;
    default:
        Printf("Unsupported key_cert ID\n");
        return PS_UNSUPPORTED_FAIL;
    }

    if (key_loaded && loaded_cert_key_id != id)
    {
        /* We are changing the key, so clear the old one. */
        Printf("Example ext module loading new key\n");
        psRsaClearKey(&rsa_key);
        key_loaded = 0;
    }

    rc = psRsaParsePkcs1PrivKey(NULL, key_buf, key_buf_len, &rsa_key);
    if (rc < 0)
    {
        Printf("psRsaParsePkcs1PrivKey failed : %d\n", rc);
        return rc;
    }

    key_loaded = 1;
    loaded_cert_key_id = id;
    key_type = PS_RSA;

    return PS_SUCCESS;
}
#  endif /* USE_RSA */

#  ifdef USE_ECC
static int load_ecc_key(unsigned int id)
{
    int32 rc;
    const unsigned char *key_buf;
    int32 key_buf_len;

    switch (id)
    {
    case CERT_KEY_ID_ECC192:
        Printf("Example ext module using P-192 ECC private key\n");
        key_buf = EC192KEY;
        key_buf_len = sizeof(EC192KEY);
        break;
    case CERT_KEY_ID_ECC224:
        Printf("Example ext module using P-224 ECC private key\n");
        key_buf = EC224KEY;
        key_buf_len = sizeof(EC224KEY);
        break;
    case CERT_KEY_ID_ECC256:
        Printf("Example ext module using P-256 ECC private key\n");
        key_buf = EC256KEY;
        key_buf_len = sizeof(EC256KEY);
        break;
    case CERT_KEY_ID_ECC256_ECDH_RSA:
        Printf("Example ext module using P-256 ECC private key");
        Printf(" for ECDH_RSA\n");
        key_buf = ECDHRSA256KEY;
        key_buf_len = sizeof(ECDHRSA256KEY);
        break;
    case CERT_KEY_ID_ECC384:
        Printf("Example ext module using P-384 ECC private key\n");
        key_buf = EC384KEY;
        key_buf_len = sizeof(EC384KEY);
        break;
    case CERT_KEY_ID_ECC521:
        Printf("Example ext module using P-521 ECC private key\n");
        key_buf = EC521KEY;
        key_buf_len = sizeof(EC521KEY);
        break;
    case CERT_KEY_ID_ECC521_ECDH_RSA:
        Printf("Example ext module using P-521 ECC private key");
        Printf(" for ECDH_RSA\n");
        key_buf = ECDHRSA521KEY;
        key_buf_len = sizeof(ECDHRSA521KEY);
        break;

    default:
        Printf("Unsupported key_cert ID\n");
        return PS_UNSUPPORTED_FAIL;
    }

    if (key_loaded && loaded_cert_key_id != id)
    {
        Printf("Example ext module loading new key\n");
        psEccClearKey(&ecc_key);
        key_loaded = 0;
    }

    rc = psEccParsePrivKey(NULL, key_buf, key_buf_len, &ecc_key, NULL);
    if (rc < 0)
    {
        Printf("psEccParsePrivKey failed : %d\n", rc);
        return rc;
    }

    key_loaded = 1;
    loaded_cert_key_id = id;
    key_type = PS_ECC;

    return PS_SUCCESS;
}
#  endif /* USE_ECC */

void psExtCleanup(void)
{
    if (key_loaded)
    {
        switch (key_type)
        {
#  ifdef USE_RSA
        case PS_RSA:
            psRsaClearKey(&rsa_key);
            break;
#  endif
#  ifdef USE_ECC
        case PS_ECC:
            psEccClearKey(&ecc_key);
            break;
#  endif
        default:
            psAssert(0);
        }
    }
}
# endif /* USE_EXT_EXAMPLE_MODULE */
#endif  /* USE_EXT_CERTIFICATE_VERIFY_SIGNING */
/* end of file exampleExtCvSigModule.c */
