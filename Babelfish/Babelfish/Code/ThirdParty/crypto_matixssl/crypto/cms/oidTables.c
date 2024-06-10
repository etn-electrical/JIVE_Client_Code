/**
 *      @file    oidTables.c
 *
 *
 *      CMS OID management.
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
#include "matrixCms.h"
#include "cryptoImpl.h"

#ifdef USE_CMS

/* NOTE: These OID tables use a mechanism of simple byte summing for the
    ID (third struct member) when being parsed.  It is essential that no two
    OIDs share the same ID in the same table */

static cmsOidTable_t cmsOidPkcsSet[] = {

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x01 },
      9, CMS_PKCS7_DATA },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x02 },
      9, CMS_PKCS7_SIGNED_DATA },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x10, 0x01, 0x17},
      11, CMS_PKCS9_AUTH_ENVELOPED_DATA },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x10, 0x01, 0x09},
      11, CMS_PKCS9_COMPRESSED_DATA },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x03 },
      9, CMS_PKCS7_ENVELOPED_DATA },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x03 },
      9, CMS_PKCS9_CONTENT_TYPE },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x04 },
      9, CMS_PKCS9_MESSAGE_DIGEST },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x05 },
      9, CMS_PKCS9_SIGNING_TIME },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x0F },
      9, CMS_PKCS9_SMIME_CAPABILITIES },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x34 },
      9, CMS_RFC6211_ALGORITHM_PROTECTION },

    { { 0x0 },0,    0 }
};

static cmsOidTable_t cmsOidAlgSet[] = {

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01 },
      9, OID_RSA_KEY_ALG },

    { { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01 },
      7, OID_ECDSA_KEY_ALG },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x05 },
      9, OID_SHA1_RSA_SIG },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x0B },
      9, OID_SHA256_RSA_SIG },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x0C },
      9, OID_SHA384_RSA_SIG },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x0D },
      9, OID_SHA512_RSA_SIG },

    { { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x01 },
      7, OID_SHA1_ECDSA_SIG },

    { { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02 },
      8, OID_SHA256_ECDSA_SIG },

    { { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x03 },
      8, OID_SHA384_ECDSA_SIG },

    { { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x04 },
      8, OID_SHA512_ECDSA_SIG },

    { { 0x2B, 0x0E, 0x03, 0x02, 0x1A },
      5, OID_SHA1_ALG },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01 },
      9, OID_SHA256_ALG },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02 },
      9, OID_SHA384_ALG },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03 },
      9, OID_SHA512_ALG },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x10, 0x03, 0x08},
      11, OID_COMPRESSION_ZLIB },

    { { 0x0 },0,    0 }
};

static cmsOidTable_t cmsOidSchemeSet[] = {
    { { 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x05, 0x01, 0x01, 0x03 },
      11, OID_ECKA_EG_X963KDF_SHA256 },

    { { 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x05, 0x01, 0x01, 0x04 },
      11, OID_ECKA_EG_X963KDF_SHA384 },

    { { 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x05, 0x01, 0x01, 0x05 },
      11, OID_ECKA_EG_X963KDF_SHA512 },

    { { 0x2B, 0x81, 0x04, 0x01, 0x0B, 0x01 },
      6, OID_DHSINGLEPASS_STDDH_SHA256KDF_SCHEME },

    { { 0x2B, 0x81, 0x04, 0x01, 0x0B, 0x02 },
      6, OID_DHSINGLEPASS_STDDH_SHA384KDF_SCHEME },

    { { 0x2B, 0x81, 0x04, 0x01, 0x0B, 0x03 },
      6, OID_DHSINGLEPASS_STDDH_SHA512KDF_SCHEME },

    { { 0x2B, 0x81, 0x05, 0x10, 0x86, 0x48, 0x3F, 0x00, 0x02 },
      9, OID_DHSINGLEPASS_STDDH_SHA1KDF_SCHEME },

    { { 0x2B, 0x81, 0x05, 0x10, 0x86, 0x48, 0x3F, 0x00, 0x03 },
      9, OID_DHSINGLEPASS_COFACTORDH_SHA1KDF_SCHEME },

    { { 0x2B, 0x81, 0x05, 0x10, 0x86, 0x48, 0x3F, 0x00, 0x10 },
      9, OID_MQVSINGLEPASS_SHA1KDF_SCHEME },

    { { 0x0 },0,    0 }
};

static cmsOidTable_t cmsOidSymSet[] = {
    /* These CBC_CMAC OIDs are not defined yet */
    { { 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x01, 0x02 },
      9, OID_AES_CBC_CMAC_128 },

    { { 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x01, 0x03 },
      9, OID_AES_CBC_CMAC_192 },

    { { 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x01, 0x04 },
      9, OID_AES_CBC_CMAC_256 },
    /* NOTE: Can't find OID for AES-CMAC.  Made this up for now (in same
        general area as nistAlgorithm.aes.  Once this is fixed, don't
        forget to change the OID_AES_CMAC sum define in cryptolib.h */
    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0xC8 },
      9, OID_AES_CMAC },

    { { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x03, 0x07 },
      8, OID_DES_EDE3_CBC },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x06 },
      9, OID_AES_128_GCM },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x1A },
      9, OID_AES_192_GCM },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x2E },
      9, OID_AES_256_GCM },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x02 },
      9, OID_AES_128_CBC },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x16 },
      9, OID_AES_192_CBC },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x2A },
      9, OID_AES_256_CBC },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x05 },
      9, OID_AES_128_WRAP },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x19 },
      9, OID_AES_192_WRAP },

    { { 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x2D },
      9, OID_AES_256_WRAP },

    { { 0x0 },0,    0 }
};


/*      id-smime OBJECT IDENTIFIER ::= { iso(1) member-body(2)
        us(840) rsadsi(113549) pkcs(1) pkcs9(9) 16 }

    id-alg  OBJECT IDENTIFIER ::= { id-smime 3 }

    id-alg-authEnc-256 OBJECT IDENTIFIER ::= { id-alg 16 }
 */
static unsigned char OID_AUTH_ENC_256[11] =
{ 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x10, 0x3, 0x10 };


static unsigned char OID_PBKDF2[9] =
{ 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x05, 0x0C };

/* Empty PBKDF2 parameters of empty salt and iteration count 1.  Includes SEQ */
static unsigned char MCMS_PBKDF2_PARAMS[7] =
{ 0x30, 0x05, 0x04, 0x00, 0x02, 0x01, 0x01 };

/* rename function */
int32 mcmsParsePkcsOid(const unsigned char **pp, uint32 len, uint16_t *oi)
{
    const unsigned char *p = *pp, *end;
    uint32 i;
    uint32_t arcLen;
    int32 rc;

    end = p + len;

    if (len < 1)
    {
        return PS_LIMIT_FAIL;
    }
    if (*(p++) != ASN_OID)
    {
        return PS_PARSE_FAIL;
    }
    if ((rc = getAsnLength32(&p, (uint32_t) (end - p), &arcLen, 0)) < 0)
    {
        psTraceCrypto("ERROR: Couldn't get OID for mcmsParsePkcsOid\n");
        return rc;
    }
    if (end - p < arcLen)
    {
        psTraceCrypto("Malformed algorithmId for getCMAlgorithmIdentifier\n");
        return PS_LIMIT_FAIL;
    }
    *oi = 0;

    rc = PS_UNSUPPORTED_FAIL;
    i = 0;
    while (cmsOidPkcsSet[i].cmsId != 0)
    {
        if (cmsOidPkcsSet[i].oidLen == arcLen)
        {
            if (Memcmp(p, cmsOidPkcsSet[i].oid, arcLen) == 0)
            {
                *oi = cmsOidPkcsSet[i].cmsId;
                rc = PS_SUCCESS;
                break;
            }
        }
        i++;
    }
    if (rc == PS_UNSUPPORTED_FAIL)
    {
        psTraceCrypto("Unsupported PKCS OID in CMS\n");
        /* psTraceBytes("OID", p, arcLen); */
    }
    p += arcLen;
    *pp = (unsigned char *) p;

    return rc;
}

/* Compare params vs. expected empty params */
int32 mcmsCmpPbkdf2Params(unsigned char *oid, psSize_t oidLen)
{
    if (oidLen != 7)
    {
        return -1;
    }
    if (Memcmp(oid, MCMS_PBKDF2_PARAMS, 7) != 0)
    {
        return -1;
    }
    return 0;
}

void mcmsGetOidAuthEnc(unsigned char *oid, psSize_t *oidLen)
{
    *oidLen = 11;
    Memcpy(oid, OID_AUTH_ENC_256, *oidLen);
}

void mcmsGetOidPBKDF2(unsigned char *oid, psSize_t *oidLen)
{
    *oidLen = 9;
    Memcpy(oid, OID_PBKDF2, *oidLen);
}

int32 mcmsGetAlgOidFromId(int32 id, unsigned char *oid, psSize_t *oidLen)
{
    int32 i = 0;

    while (cmsOidAlgSet[i].cmsId != 0)
    {
        if (cmsOidAlgSet[i].cmsId == id)
        {
            *oidLen = cmsOidAlgSet[i].oidLen;
            Memcpy(oid, cmsOidAlgSet[i].oid, *oidLen);
            return PS_SUCCESS;
        }
        i++;
    }
    return PS_UNSUPPORTED_FAIL;
}

int32 mcmsGetSchemeOidFromId(int32 id, unsigned char *oid, psSize_t *oidLen)
{
    int32 i = 0;

    while (cmsOidSchemeSet[i].cmsId != 0)
    {
        if (cmsOidSchemeSet[i].cmsId == id)
        {
            *oidLen = cmsOidSchemeSet[i].oidLen;
            Memcpy(oid, cmsOidSchemeSet[i].oid, *oidLen);
            return PS_SUCCESS;
        }
        i++;
    }
    return PS_UNSUPPORTED_FAIL;
}

int32 mcmsGetSymOidFromId(int32 id, unsigned char *oid, psSize_t *oidLen)
{
    int32 i = 0;

    while (cmsOidSymSet[i].cmsId != 0)
    {
        if (cmsOidSymSet[i].cmsId == id)
        {
            *oidLen = cmsOidSymSet[i].oidLen;
            Memcpy(oid, cmsOidSymSet[i].oid, *oidLen);
            return PS_SUCCESS;
        }
        i++;
    }
    return PS_UNSUPPORTED_FAIL;
}

int32 mcmsGetPkcsOidFromId(int32 id, unsigned char *oid, psSize_t *oidLen)
{
    int32 i = 0;

    while (cmsOidPkcsSet[i].cmsId != 0)
    {
        if (cmsOidPkcsSet[i].cmsId == id)
        {
            *oidLen = cmsOidPkcsSet[i].oidLen;
            Memcpy(oid, cmsOidPkcsSet[i].oid, *oidLen);
            return PS_SUCCESS;
        }
        i++;
    }
    return PS_UNSUPPORTED_FAIL;
}

#endif /* USE_CMS */
