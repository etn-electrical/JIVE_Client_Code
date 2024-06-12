#define TESTSUPP_MAIN
#include "testsupp.hpp"
#include "../matrixCmsApi.h"
#include "core/psUtil.h"
#include "cryptoApi.h"
#include <stdio.h>

/* Helper resources. */
#include "data/EC_256_EC.h"
#include "data/EC_256_EC_KEY.h"
#include "data/EC_384_EC.h"
#include "data/EC_384_EC_KEY.h"
#include "data/RSA_2048_RSA.h"
#include "data/RSA_2048_RSA_KEY.h"
#include "data/messageMedium.txt.h"
#include "data/messageSmall.txt.h"
#include "data/messageSmall.txt.encrypted.h"
#include "data/signed.bin.h"
#include "data/pkcs7_crl_basic.h"
#include "data/crls_intermediate.crl.h"
#include "data/authenv_ext.data.bin.h"

#define CUC(x) ((const unsigned char *)x)

TEST_CASE("getAsnTagLenUnsafe")
{
    /* TODO: This test case does not actually belong to this file.
       Find a better place. */
    REQUIRE(getAsnTagLenUnsafe(NULL) == 0);
    REQUIRE(getAsnTagLenUnsafe(CUC("")) == 0);
    REQUIRE(getAsnTagLenUnsafe(CUC("\1\0")) == 2);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\1\1")) == 3);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\x7F\xee...")) == 129);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\x80\xee...")) == 0);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\x81\x80...")) == 131);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\x81\xfd...")) == 256);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\x81\xff...")) == 258);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\x82\x02\x01...")) == 0x0205);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\x83\x03\x02\x01...")) == 0x030206);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\x84\xee...")) == 0);
    REQUIRE(getAsnTagLenUnsafe(CUC("\2\x85\xee...")) == 0);
}

void common_initialize_cms(void)
{
    static int initialized = 0;

    if (!initialized)
    {
        int32 ret;

        ret = psCoreOpen(PSCORE_CONFIG);
        REQUIRE(ret == PS_SUCCESS);
        ret = psCryptoOpen(PSCRYPTO_CONFIG);
        REQUIRE(ret == PS_SUCCESS);
        initialized = 1;
    }
}

TEST_CASE("initialize")
{
    common_initialize_cms();
}

static void kat_test(unsigned char *messageData, int32 messageDataLen,
                     int32 flags)
{
    int32 rc;
    psPool_t *pool = NULL;
    cmsSignedData_t *signedData;

    /* Parse previously signed data. */
    rc = matrixCmsParseSignedData(
            pool, signed_bin, signed_bin_len,
            &signedData,
            flags);

    REQUIRE(rc == PS_SUCCESS);

    /* Ensure output of signing + recovery is the original input. */
    REQUIRE(
            STRING(messageData, messageDataLen) ==
            STRING(signedData->eContent, signedData->eContentLen));

    matrixCmsFreeParsedSignedData(signedData);
}

static void pairwise_test(unsigned char *messageData, int32 messageDataLen,
                          psX509Cert_t *cert, psPubKey_t *privkey, int32 flags)
{
    int32 rc;
    char *output;
    psPool_t *pool = NULL;
    unsigned char *createdBuf;
    cmsSignedData_t *signedData;
    int32 createdLen;

    /* Test requires: */
    REQUIRE(messageData != NULL);
    REQUIRE(messageDataLen >= 1);
    REQUIRE(cert != NULL);
    REQUIRE(privkey != NULL);

    rc = matrixCmsCreateSignedData(
            pool,
            messageData,
            messageDataLen,
            CMS_PKCS7_SIGNED_DATA,
            cert,
            privkey,
            MCMS_SHA256_ALG,
            &createdBuf,
            &createdLen,
            flags);
    REQUIRE(rc == PS_SUCCESS);

    /* Set testsupp_write_debug_files or
       TESTSUPP_WRITE_DEBUG_FILES environment variable to debug by storing
       created files. */
    opt_WRITE_FILE("signed.data", createdBuf, createdLen);
    /* Generated file can be viewed with
       cat signed.data | openssl cms -inform DER -cmsout -noout -print

       And data payload extracted with
       cat signed.data | openssl cms -inform DER -verify -purpose any \
       -CAfile ../../../testkeys/EC/256_EC_CA.pem

       Interesting provided:
       cat signed.data | openssl cms -inform DER -cmsout -noout -print | \
       grep -C2 -e d.certificate -e signatureAlgorithm -e signature: \
       -e signerInfos -e CMS_ContentInfo
    */

    rc = matrixCmsParseSignedData(
            pool, createdBuf, createdLen,
            &signedData,
            flags);

    REQUIRE(rc == PS_SUCCESS);

    if ((flags & MCMS_FLAGS_SD_NODETACH) > 0)
    {
        /* Ensure output of signing + recovery is the original input. */
        REQUIRE(
                STRING(messageData, messageDataLen) ==
                STRING(signedData->eContent, signedData->eContentLen));

        /* Check signed data against encapsulated data. */
        rc = matrixCmsConfirmSignature(pool, signedData, NULL, 0, cert);
        REQUIRE(rc == PS_SUCCESS);
    }

    /* Confirm Signature fails with wrong data (note: first byte is different). */
    messageData[0] ^= 0x40;
    rc = matrixCmsConfirmSignature(
            pool,
            signedData,
            messageData,
            messageDataLen,
            cert);
    if ((flags & MCMS_FLAGS_SD_NODETACH) > 0)
    {
        REQUIRE(rc == MCMS_SIG_FAIL_CONTENT_MISMATCH);
    }
    else
    {
        REQUIRE(rc == MCMS_SIG_FAIL_CONTENT_HASH_MISMATCH);
    }
    messageData[0] ^= 0x40;

    rc = matrixCmsConfirmSignature(
            pool,
            signedData,
            messageData,
            messageDataLen,
            cert);
    REQUIRE(rc == PS_SUCCESS);

    /* Check there is exactly one signer info. */
    REQUIRE(signedData->signers != NULL);
    REQUIRE(signedData->signers->next == NULL);

    matrixCmsFreeParsedSignedData(signedData);
    psFree(createdBuf, pool);
}

static void matrixCmsCreateSignedDataExt_test(
        int32 messageType,
        unsigned char *messageData,
        int32 messageDataLen,
        int num_signers,
        psX509Cert_t * const *certs,
        psPubKey_t * const *privkeys,
        matrixCmsCreateSignedDataExtInfo_t *ei_p,
        psBool_t match,
        const unsigned char *kat,
        psSize32_t katlen)
{
    int32 rc;
    char *output;
    psPool_t *pool = NULL;
    unsigned char *createdOldBuf = NULL;
    unsigned char *createdBuf;
    cmsSignedData_t *signedData;
    int32 createdOldLen;
    int32 createdLen;
    const int32 hashAlgs[2] = { MCMS_SHA256_ALG, MCMS_SHA256_ALG };

    /* Test requires: */
    REQUIRE(messageData != NULL);
    REQUIRE(messageDataLen >= 1);

    if (match)
    {
        /* Generate comparison target using old matrixCmsCreateSignedData(). */
        rc = matrixCmsCreateSignedData(
                pool,
                messageData,
                messageDataLen,
                messageType,
                certs[0],
                privkeys[0],
                hashAlgs[0],
                &createdOldBuf,
                &createdOldLen,
                ei_p->flags);
        REQUIRE(rc == PS_SUCCESS);

        opt_WRITE_FILE("signed.data", createdOldBuf, createdOldLen);
    }

    rc = matrixCmsCreateSignedDataExt(
            pool,
            messageData,
            messageDataLen,
            messageType,
            num_signers,
            certs,
            privkeys,
            hashAlgs,
            &createdBuf,
            &createdLen,
            ei_p);
    REQUIRE(rc == PS_SUCCESS);

    opt_WRITE_FILE("signedext.data", createdBuf, createdLen);
    
    if (match)
    {
        REQUIRE(HEX(createdOldBuf, createdOldLen) ==
                HEX(createdBuf, createdLen));
    }

    if (kat)
    {
        REQUIRE(HEX(kat, katlen) ==
                HEX(createdBuf, createdLen));
    }
    
    psFree(createdOldBuf, pool);
    psFree(createdBuf, pool);
}

TEST_CASE("matrixCms")
{
    /* Get keys. */
    
    int32 rc;
    int32 flags;
    psX509Cert_t *myCert = NULL;
    psPubKey_t privkey;

    common_initialize_cms();

    rc = psX509ParseCert(
            NULL,
            EC_256_EC,
            EC_256_EC_len,
            &myCert,
            CERT_STORE_UNPARSED_BUFFER | CERT_STORE_DN_BUFFER);
    REQUIRE(rc == EC_256_EC_len);

    memset(&privkey, 0, sizeof(privkey));
    rc = psParseUnknownPrivKeyMem(
            NULL, EC_256_EC_KEY,
            EC_256_EC_KEY_len, NULL, &privkey);
    REQUIRE(rc == 2 /* ecc key loaded */);

    SECTION( "cms parsing previous output" )
    {
        flags = MCMS_FLAGS_SD_NODETACH | MCMS_FLAGS_SD_SUBJECT_KEY_ID;
        kat_test(
                messageMedium_txt,
                messageMedium_txt_len,
                flags);
    }

    SECTION( "cms signing pairwise test using subject_key_id" )
    {
        flags = MCMS_FLAGS_SD_NODETACH | MCMS_FLAGS_SD_SUBJECT_KEY_ID;
        pairwise_test(
                messageMedium_txt,
                messageMedium_txt_len,
                myCert,
                &privkey,
                flags);
    }

    psX509Cert_t *myCert2 = NULL;
    psPubKey_t privkey2;

    rc = psX509ParseCert(
            NULL,
            RSA_2048_RSA,
            RSA_2048_RSA_len,
            &myCert2,
            CERT_STORE_UNPARSED_BUFFER | CERT_STORE_DN_BUFFER);
    REQUIRE(rc == RSA_2048_RSA_len);

    memset(&privkey2, 0, sizeof(privkey2));
    rc = psParseUnknownPrivKeyMem(
            NULL, RSA_2048_RSA_KEY,
            RSA_2048_RSA_KEY_len, NULL, &privkey2);
    REQUIRE(rc == 1 /* rsa key loaded */);

    SECTION( "cms signing pairwise test; detached; RSA key" )
    {
        flags = 0;
        pairwise_test(
                messageMedium_txt,
                messageMedium_txt_len,
                myCert2,
                &privkey2,
                flags);
    }

    SECTION( "cms signing pairwise test; RSA key" )
    {
        flags = MCMS_FLAGS_SD_NODETACH;
        pairwise_test(
                messageMedium_txt,
                messageMedium_txt_len,
                myCert2,
                &privkey2,
                flags);
    }

    SECTION( "cms signing test; RSA key; matrixCmsCreateSignedDataExt; no-ci" )
    {
        matrixCmsCreateSignedDataExtInfo_t ei =
        {
            MCMS_FLAGS_NO_CONTENT_INFO,
        };
        psX509Cert_t *certs[1] = { myCert2 };
        psPubKey_t *privkeys[1] = { &privkey2 };

        matrixCmsCreateSignedDataExt_test(
                CMS_PKCS7_SIGNED_DATA,
                messageMedium_txt,
                messageMedium_txt_len,
                1,
                certs,
                privkeys,
                &ei,
                true,
                NULL,
                0);
    }

    SECTION( "cms signing test; RSA key; matrixCmsCreateSignedDataExt" )
    {
        matrixCmsCreateSignedDataExtInfo_t ei =
        {
            0,
        };
        psX509Cert_t *certs[1] = { myCert2 };
        psPubKey_t *privkeys[1] = { &privkey2 };

        matrixCmsCreateSignedDataExt_test(
                CMS_PKCS7_SIGNED_DATA,
                messageMedium_txt,
                messageMedium_txt_len,
                1,
                certs,
                privkeys,
                &ei,
                true,
                NULL,
                0);
    }

    SECTION( "cms signing test; RSA key; matrixCmsCreateSignedDataExt; CC0" )
    {
        matrixCmsCreateSignedDataExtInfo_t ei =
        {
          MCMS_FLAGS_SD_CERT_CHAIN,
        };
        psX509Cert_t *certs[1] = { myCert2 };
        psPubKey_t *privkeys[1] = { &privkey2 };

        matrixCmsCreateSignedDataExt_test(
                CMS_PKCS7_SIGNED_DATA,
                messageMedium_txt,
                messageMedium_txt_len,
                1,
                certs,
                privkeys,
                &ei,
                true,
                NULL,
                0);
    }

    SECTION( "cms signing test; RSA key; matrixCmsCreateSignedDataExt; CC" )
    {
        matrixCmsCreateSignedDataExtInfo_t ei =
        {
            MCMS_FLAGS_SD_CERT_CHAIN,
        };
        psX509Cert_t *certs[1] = { myCert2 };
        psPubKey_t *privkeys[1] = { &privkey2 };

        myCert2->next = myCert;
        matrixCmsCreateSignedDataExt_test(
                CMS_PKCS7_SIGNED_DATA,
                messageMedium_txt,
                messageMedium_txt_len,
                1,
                certs,
                privkeys,
                &ei,
                true,
                NULL,
                0);
        myCert2->next = NULL;
    }

    SECTION( "cms signing test; matrixCmsCreateSignedDataExt; 2signs" )
    {
        matrixCmsCreateSignedDataExtInfo_t ei =
        {
            MCMS_FLAGS_SD_NODETACH,
        };
        psX509Cert_t *certs[2] = { myCert, myCert2 };
        psPubKey_t *privkeys[2] = { &privkey, &privkey2 };

        matrixCmsCreateSignedDataExt_test(
                CMS_PKCS7_SIGNED_DATA,
                messageMedium_txt,
                messageMedium_txt_len,
                2,
                certs,
                privkeys,
                &ei,
                false /* Old function only uses single signer. */,
                NULL,
                0);
    }

    SECTION( "crl2pkcs7 test using matrixCmsCreateSignedDataExt" )
    {
        matrixCmsCreateSignedDataExtInfo_t ei =
        {
            MCMS_FLAGS_SD_HAS_CRL,
            crls_intermediate_crl,
            crls_intermediate_crl_len
        };

        matrixCmsCreateSignedDataExt_test(
                CMS_PKCS7_DATA,
                messageMedium_txt,
                messageMedium_txt_len,
                0,
                NULL,
                NULL,
                &ei,
                false /* The old function cannot add CRL. */,
                pkcs7_crl_basic,
                pkcs7_crl_basic_len);
    }

    SECTION( "crl2pkcs7 test using matrixCmsCreateSignedDataExt; crl and certs" )
    {
        matrixCmsCreateSignedDataExtInfo_t ei =
        {
            MCMS_FLAGS_SD_HAS_CRL,
            crls_intermediate_crl,
            crls_intermediate_crl_len
        };

        psX509Cert_t *certs[2] = { myCert2, myCert };

        matrixCmsCreateSignedDataExt_test(
                CMS_PKCS7_DATA,
                messageMedium_txt,
                messageMedium_txt_len,
                2,
                certs,
                NULL,
                &ei,
                false /* The old function cannot add CRL. */,
                NULL,
                0);

        /* TODO: Add automatic validation of result, the result has
           been validated by hand. */
    }

    psX509Cert_t *myCert3 = NULL;
    psPubKey_t privkey3;
    
    rc = psX509ParseCert(
            NULL,
            EC_384_EC,
            EC_384_EC_len,
            &myCert3,
            CERT_STORE_UNPARSED_BUFFER | CERT_STORE_DN_BUFFER);
    REQUIRE(rc == EC_384_EC_len);

    memset(&privkey3, 0, sizeof(privkey3));
    rc = psParseUnknownPrivKeyMem(
            NULL, EC_384_EC_KEY,
            EC_384_EC_KEY_len, NULL, &privkey3);
    REQUIRE(rc == 2 /* ecc key loaded */);

    SECTION( "matrixCmsCreateAuthEnvData test" )
    {
        int rc;
        psPool_t *pool = NULL;
        unsigned char *outBuf;
        int32 outBufLen;
        unsigned char *recoveredBuf;
        int32 recoveredBufLen;
        cmsEncryptedEnvelope_t *authData;

        rc = matrixCmsCreateAuthEnvData(
                pool,
                myCert,
                &privkey,
                myCert3,
                MCMS_AED_KEY_AGREE_METHOD,
                MCMS_AES128_GCM,
                MCMS_AES256_WRAP,
                MCMS_ECKA_X963KDF_SHA256,
                messageSmall_txt,
                messageSmall_txt_len,
                CMS_PKCS7_DATA,
                &outBuf,
                &outBufLen,
                MCMS_FLAGS_AED_ORIG_DHE_PUBLIC_KEY);
        REQUIRE(rc == PS_SUCCESS);
        opt_WRITE_FILE("authenv.data", outBuf, outBufLen);
        /* When investigating the result, on command line the output buffer can
           be examined with
           openssl cms -in authenv.data -inform der -cmsout -noout -print
           Unfortunately, there are no good tools within openssl cms to work
           with RFC 5083 AuthEnvelopedData. */

        /* Recover authenticated encrypted data. */
        rc = matrixCmsParseAuthEnvData(
                pool,
                outBuf,
                outBufLen,
                myCert,
                &privkey3,
                0,
                &recoveredBuf,
                &recoveredBufLen,
                &authData);
        REQUIRE(rc == PS_SUCCESS);
        REQUIRE(recoveredBufLen == messageSmall_txt_len);
        REQUIRE(memcmp(messageSmall_txt, recoveredBuf, recoveredBufLen) == 0);
        psFree(outBuf, pool);
        psFree(recoveredBuf, pool);
        matrixCmsFreeParsedAuthEnvData(authData);
    }

    SECTION( "matrixCmsCreateAuthEnvData test with RSA private key" )
    {
        int rc;
        psPool_t *pool = NULL;
        unsigned char *outBuf;
        int32 outBufLen;
        unsigned char *recoveredBuf;
        int32 recoveredBufLen;
        cmsEncryptedEnvelope_t *authData;

        rc = matrixCmsCreateAuthEnvData(
                pool,
                myCert2,
                &privkey2,
                myCert3,
                MCMS_AED_KEY_AGREE_METHOD,
                MCMS_AES128_GCM,
                MCMS_AES256_WRAP,
                MCMS_ECKA_X963KDF_SHA256,
                messageSmall_txt,
                messageSmall_txt_len,
                CMS_PKCS7_DATA,
                &outBuf,
                &outBufLen,
                MCMS_FLAGS_AED_ORIG_DHE_PUBLIC_KEY);
        REQUIRE(rc == PS_SUCCESS);
        opt_WRITE_FILE("authenv.data", outBuf, outBufLen);
        /* When investigating the result, on command line the output buffer can
           be examined with
           openssl cms -in authenv.data -inform der -cmsout -noout -print
           Unfortunately, there are no good tools within openssl cms to work
           with RFC 5083 AuthEnvelopedData. */

        /* Recover authenticated encrypted data. */
        rc = matrixCmsParseAuthEnvData(
                pool,
                outBuf,
                outBufLen,
                myCert2,
                &privkey3,
                0,
                &recoveredBuf,
                &recoveredBufLen,
                &authData);
        REQUIRE(rc == PS_SUCCESS);
        REQUIRE(recoveredBufLen == messageSmall_txt_len);
        REQUIRE(memcmp(messageSmall_txt, recoveredBuf, recoveredBufLen) == 0);
        psFree(outBuf, pool);
        psFree(recoveredBuf, pool);
        matrixCmsFreeParsedAuthEnvData(authData);
    }

    SECTION( "matrixCmsCreateAuthEnvDataExt test, compare with known answer" )
    {
        int rc;
        psPool_t *pool = NULL;
        unsigned char *outBuf;
        int32 outBufLen;
        unsigned char *recoveredBuf;
        int32 recoveredBufLen;
        cmsEncryptedEnvelope_t *authData;
        const unsigned char stored_keymat[] = {
          0xfc, 0x08, 0x6f, 0x61, 0x7b, 0x94, 0xb0, 0x29, 0xff, 0x97, 0x61,
          0x5c, 0x94, 0x48, 0xa4, 0xfb, 0x5f, 0x15, 0x0a, 0x48, 0xb2, 0xaa,
          0xa5, 0x3b, 0x0c, 0x1d, 0x50, 0x3f, 0x79, 0xb1, 0x3f, 0x7c, 0x1b,
          0xf1, 0x29, 0x74, 0x7a, 0x9c, 0xee, 0xea, 0xa9, 0x0e, 0x4a, 0x26,
          0x7c, 0xd0, 0x23, 0x9b, 0xef, 0x75, 0x17, 0x80, 0x29, 0x8e, 0xfa,
          0x1f, 0x9b, 0xbe, 0xad, 0x47, 0x48, 0x5e, 0xbd, 0x63, 0xd9, 0x30,
          0x63, 0x0b, 0x47, 0xf7, 0xec, 0x75, 0x37, 0xf2, 0xb9, 0x02, 0xf3,
          0xff, 0x5a, 0xc0
        };
        const psX509Cert_t *certs[1] = { myCert };

        /* use previously computed keymat to enforce test to use
           always the same key material. */
        matrixCmsCreateAuthEnvDataExtInfo_t ei = {
            MCMS_FLAGS_AED_PROVIDE_KEYMAT,
        };
        ei.authEncryptKeyMat = stored_keymat;
        ei.authEncryptKeyMatLen = (psSize_t) sizeof stored_keymat;

        /* Note: recipient and source must use the same curve.
           Currently they use the same key, which should not be the
           case outside test cases. */
        rc = matrixCmsCreateAuthEnvDataExt(
                pool,
                myCert,
                &privkey,
                1,
                certs,
                MCMS_AED_KEY_AGREE_METHOD,
                MCMS_AES128_CBC_CMAC,
                MCMS_AES256_WRAP,
                MCMS_ECKA_X963KDF_SHA256,
                messageSmall_txt,
                messageSmall_txt_len,
                CMS_PKCS7_DATA,
                &outBuf,
                &outBufLen,
                &ei);
        REQUIRE(rc == PS_SUCCESS);
        opt_WRITE_FILE("authenv_ext.data", outBuf, outBufLen);
        /* When investigating the result, on command line the output buffer can
           be examined with
           openssl cms -in authenv.data -inform der -cmsout -noout -print
           Unfortunately, there are no good tools within openssl cms to work
           with RFC 5083 AuthEnvelopedData. */

        /* Ensure output matches expected. */
        REQUIRE(outBufLen == authenv_ext_data_bin_len);
        REQUIRE(memcmp(outBuf,
                       authenv_ext_data_bin,
                       authenv_ext_data_bin_len) == 0);
        
        /* Recover authenticated encrypted data. */
        rc = matrixCmsParseAuthEnvData(
                pool,
                outBuf,
                outBufLen,
                myCert,
                &privkey,
                0,
                &recoveredBuf,
                &recoveredBufLen,
                &authData);
        REQUIRE(rc == PS_SUCCESS);
        REQUIRE(recoveredBufLen == messageSmall_txt_len);
        REQUIRE(memcmp(messageSmall_txt, recoveredBuf, recoveredBufLen) == 0);
        psFree(outBuf, pool);
        psFree(recoveredBuf, pool);
        matrixCmsFreeParsedAuthEnvData(authData);
    }

    SECTION( "matrixCmsCreateAuthEnvData test with RSA private key" )
    {
        int rc;
        psPool_t *pool = NULL;
        unsigned char *outBuf;
        int32 outBufLen;
        unsigned char *recoveredBuf;
        int32 recoveredBufLen;
        cmsEncryptedEnvelope_t *authData;

        rc = matrixCmsCreateAuthEnvData(
                pool,
                myCert2,
                &privkey2,
                myCert3,
                MCMS_AED_KEY_AGREE_METHOD,
                MCMS_AES128_GCM,
                MCMS_AES256_WRAP,
                MCMS_ECKA_X963KDF_SHA256,
                messageSmall_txt,
                messageSmall_txt_len,
                CMS_PKCS7_DATA,
                &outBuf,
                &outBufLen,
                MCMS_FLAGS_AED_ORIG_DHE_PUBLIC_KEY);
        REQUIRE(rc == PS_SUCCESS);
        opt_WRITE_FILE("authenv_rsa.data", outBuf, outBufLen);
        /* When investigating the result, on command line the output buffer can
           be examined with
           openssl cms -in authenv.data -inform der -cmsout -noout -print
           Unfortunately, there are no good tools within openssl cms to work
           with RFC 5083 AuthEnvelopedData. */

        /* Recover authenticated encrypted data. */
        rc = matrixCmsParseAuthEnvData(
                pool,
                outBuf,
                outBufLen,
                myCert2,
                &privkey3,
                0,
                &recoveredBuf,
                &recoveredBufLen,
                &authData);
        REQUIRE(rc == PS_SUCCESS);
        REQUIRE(recoveredBufLen == messageSmall_txt_len);
        REQUIRE(memcmp(messageSmall_txt, recoveredBuf, recoveredBufLen) == 0);
        psFree(outBuf, pool);
        psFree(recoveredBuf, pool);
        matrixCmsFreeParsedAuthEnvData(authData);
    }

    SECTION( "matrixCmsCreateAuthEnvDataExt test with RSA private key and two recipients" )
    {
        int rc;
        psPool_t *pool = NULL;
        unsigned char *outBuf;
        int32 outBufLen;
        unsigned char *recoveredBuf;
        int32 recoveredBufLen;
        cmsEncryptedEnvelope_t *authData;
        const psX509Cert_t *certs[2] = { myCert3, myCert };

        /* use previously computed keymat to enforce test to use
           always the same key material. */
        matrixCmsCreateAuthEnvDataExtInfo_t ei = {
            MCMS_FLAGS_AED_ORIG_DHE_PUBLIC_KEY,
        };

        rc = matrixCmsCreateAuthEnvDataExt(
                pool,
                myCert2,
                &privkey2,
                2,
                certs,
                MCMS_AED_KEY_AGREE_METHOD,
                MCMS_AES128_GCM,
                MCMS_AES256_WRAP,
                MCMS_ECKA_X963KDF_SHA256,
                messageSmall_txt,
                messageSmall_txt_len,
                CMS_PKCS7_DATA,
                &outBuf,
                &outBufLen,
                &ei);
        REQUIRE(rc == PS_SUCCESS);
        opt_WRITE_FILE("authenv_rsa2.data", outBuf, outBufLen);
        /* When investigating the result, on command line the output buffer can
           be examined with
           openssl cms -in authenv_rsa2.data -inform der -cmsout -noout -print
           Unfortunately, there are no good tools within openssl cms to work
           with RFC 5083 AuthEnvelopedData. */

        /* Recover authenticated encrypted data. */
        rc = matrixCmsParseAuthEnvData(
                pool,
                outBuf,
                outBufLen,
                myCert2,
                &privkey3,
                0,
                &recoveredBuf,
                &recoveredBufLen,
                &authData);
        REQUIRE(rc == PS_SUCCESS);
        REQUIRE(recoveredBufLen == messageSmall_txt_len);
        REQUIRE(memcmp(messageSmall_txt, recoveredBuf, recoveredBufLen) == 0);
        psFree(recoveredBuf, pool);
        matrixCmsFreeParsedAuthEnvData(authData);

        /* Recover authenticated encrypted data. */
        rc = matrixCmsParseAuthEnvData(
                pool,
                outBuf,
                outBufLen,
                myCert2,
                &privkey,
                0,
                &recoveredBuf,
                &recoveredBufLen,
                &authData);
        REQUIRE(rc == PS_SUCCESS);
        REQUIRE(recoveredBufLen == messageSmall_txt_len);
        REQUIRE(memcmp(messageSmall_txt, recoveredBuf, recoveredBufLen) == 0);
        psFree(recoveredBuf, pool);
        matrixCmsFreeParsedAuthEnvData(authData);

        psFree(outBuf, pool);
    }

    psX509FreeCert(myCert);
    psClearPubKey(&privkey);
    psX509FreeCert(myCert2);
    psClearPubKey(&privkey2);
    psX509FreeCert(myCert3);
    psClearPubKey(&privkey3);
}
