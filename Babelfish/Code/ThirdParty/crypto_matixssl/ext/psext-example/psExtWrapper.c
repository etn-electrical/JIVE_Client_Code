/* psExtWrapper.c
 * Wrapper for the exampleExtCvSigModule implementation of the psExt API.
 * This helps reduce the amount of code needed in the test programs.
 * apps/ssl/client.c and matrixssl/test/sslTest.c both invoke this.
 *
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
#include "matrixssl/matrixsslImpl.h"

#ifdef USE_EXT_CERTIFICATE_VERIFY_SIGNING
# ifdef USE_EXT_EXAMPLE_MODULE

#  ifdef USE_ECC
static int contains_ecdh_rsa_pubkey(psX509Cert_t *cert);
#  endif /* USE_ECC */

int32 compute_external_cv_signature(ssl_t *ssl)
{
    unsigned char my_hash[64] = { 0 };
    unsigned char *my_sig;
    size_t hash_len = sizeof(my_hash);
    size_t sig_len;
    int32_t rc2;
    int32_t sigAlg;
    unsigned int cert_key_id = CERT_KEY_ID_RSA2048;
    psBool_t ckid_found = PS_FALSE;
    sslIdentity_t *id;

    psTraceInfo("compute_external_cv_signature called\n");

    /*
       Get the handshake_messages hash to sign.
     */
    rc2 = matrixSslGetHSMessagesHash(ssl, my_hash, &hash_len);
    if (rc2 != PS_SUCCESS)
    {
        return MATRIXSSL_ERROR;
    }

    /*
       Now compute the signature outside the MatrixSSL library.
       We call the example external module via the psExt API.
     */
    sigAlg = matrixSslGetCvSignatureAlg(ssl);
    switch (sigAlg)
    {
#  ifdef USE_RSA
    case PS_RSA:
        psTraceInfo("Using RSA for ext Cv sig\n");
        psTraceIntInfo("hash_len == %zu\n", hash_len);
        /*
           Determine which RSA priv key to use.
         */
        rc2 = matrixSslGetPubKeySize(ssl);
        if (rc2 < 0)
        {
            return MATRIXSSL_ERROR;
        }
        switch (rc2)
        {
        case 128:
            cert_key_id = CERT_KEY_ID_RSA1024;
            break;
        case 256:
            cert_key_id = CERT_KEY_ID_RSA2048;
            break;
        case 384:
            cert_key_id = CERT_KEY_ID_RSA3072;
            break;
        case 512:
            cert_key_id = CERT_KEY_ID_RSA4096;
            break;
        default:
            psTraceInfo("Unknown cert key ID\n");
            return MATRIXSSL_ERROR;
        }
        rc2 = psExtRsaSignHash(cert_key_id,
            my_hash, hash_len,
            &my_sig, &sig_len);
        if (rc2 != PS_SUCCESS)
        {
            psTraceIntInfo("psExtRsaSignHash failed: %d\n", rc2);
            return MATRIXSSL_ERROR;
        }
        break;
#  endif /* USE_RSA */
#  ifdef USE_ECC
    case PS_ECC:
        psTraceInfo("Using ECC for ext Cv sig\n");
        psTraceIntInfo("hash_len == %zu\n", hash_len);
        /*
           Determine which ECDSA private key to use.
         */
        rc2 = matrixSslGetPubKeySize(ssl);
        if (rc2 < 0)
        {
            return MATRIXSSL_ERROR;
        }
        switch (rc2)
        {
        case 24:
            cert_key_id = CERT_KEY_ID_ECC192;
            break;
        case 28:
            cert_key_id = CERT_KEY_ID_ECC224;
            break;
        case 32:
            for (id = ssl->keys->identity; id; id = id->next)
            {
                if (contains_ecdh_rsa_pubkey(id->cert))
                {
                    cert_key_id = CERT_KEY_ID_ECC256_ECDH_RSA;
                    ckid_found = PS_TRUE;
                }
            }
            if (!ckid_found)
            {
                cert_key_id = CERT_KEY_ID_ECC256;
            }
            break;
        case 48:
            cert_key_id = CERT_KEY_ID_ECC384;
            break;
        case 66:
            for (id = ssl->keys->identity; id; id = id->next)
            {
                if (contains_ecdh_rsa_pubkey(id->cert))
                {
                    cert_key_id = CERT_KEY_ID_ECC521_ECDH_RSA;
                    ckid_found = PS_TRUE;
                }
            }
            if (!ckid_found)
            {
                cert_key_id = CERT_KEY_ID_ECC521;
            }
            break;
        default:
            psTraceInfo("Unknown cert key ID\n");
            return MATRIXSSL_ERROR;
        }

        rc2 = psExtEcdsaSignHash(cert_key_id,
            my_hash, hash_len,
            &my_sig, &sig_len);
        if (rc2 != PS_SUCCESS)
        {
            psTraceIntInfo("psExtEcdsaSignHash failed: %d\n", rc2);
            return MATRIXSSL_ERROR;
        }
        break;
#  endif /* USE_ECC */
    default:
        psTraceInfo("Unsupported external Cv sig alg\n");
        return MATRIXSSL_ERROR;
    }
    psTraceIntInfo("Computed signature of length %zu\n", sig_len);

    /*
       Set the signature.
     */
    rc2 = matrixSslSetCvSignature(ssl, my_sig, sig_len);
    if (rc2 != PS_SUCCESS)
    {
        return MATRIXSSL_ERROR;
    }

    /*
       It is the responsibility of the caller of matrixSslSetCvSignature
       to free the signature buffer.
     */
    psFree(my_sig, NULL);

    /*
       @note Test scripts may look for the following log print to ensure that
       the external sig operation was performed.
     */
    Printf("External CertificateVerify signature operation successful\n");

    /*
       Ask the module to perform a cleanup operation.
     */
    psExtCleanup();

    return PS_SUCCESS;
}

#  ifdef USE_ECC
static int contains_ecdh_rsa_pubkey(psX509Cert_t *cert)
{
    if (!cert || cert->subject.commonNameLen <= 1)
    {
        return PS_FALSE;
    }
    if (Strstr(cert->subject.commonName, "ECDH-RSA") != NULL)
    {
        return PS_TRUE;
    }
    return PS_FALSE;
}
#  endif /* USE_ECC */

# endif  /* USE_EXT_EXAMPLE_MODULE */
#endif   /* USE_EXT_CERTIFICATE_VERIFY_SIGNING */

/* end of file psExtWrapper.c */
