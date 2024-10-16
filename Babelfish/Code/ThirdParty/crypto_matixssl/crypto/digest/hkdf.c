/**
 *      @file    hkdf.c
 *
 *
 *      HKDF (RFC 5869) implementation.
 */
/*
 *      Copyright (c) 2018 INSIDE Secure Corporation
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

#include "../cryptoImpl.h"

#ifdef USE_HKDF

#define HKDF_MAX_INFO_LEN 80
//#define DEBUG_HKDF

int32_t psHkdfExpand(psCipherType_e hmacAlg,
        const unsigned char *prk,
        psSize_t prkLen,
        const unsigned char *info,
        psSize_t infoLen,
        unsigned char *okm,
        psSize_t okmLen)
{
    int32_t hashLen;
    unsigned char buf[MAX_HASH_SIZE + HKDF_MAX_INFO_LEN + 1] = {0};
    unsigned char T[MAX_HASH_SIZE] = {0};
    psSizeL_t i, L, stillNeeded;
    int32_t rc;
    unsigned char *out = okm;
    unsigned char *outEnd = okm + okmLen;
    unsigned char *p;

#ifdef DEBUG_HKDF
    psTraceBytes("prk", prk, prkLen);
    psTraceBytes("info", info, infoLen);
#endif

    if (hmacAlg != HMAC_MD5 && hmacAlg != HMAC_SHA1 &&
            hmacAlg != HMAC_SHA256 && hmacAlg != HMAC_SHA384)
    {
        return PS_ARG_FAIL;
    }
    hashLen = psGetOutputBlockLength(hmacAlg);
    if (hashLen < 0)
    {
        return PS_ARG_FAIL;
    }
    L = okmLen;

    if (infoLen > HKDF_MAX_INFO_LEN)
    {
        return PS_LIMIT_FAIL;
    }
    if (prkLen < hashLen || L > hashLen*255)
    {
        return PS_ARG_FAIL;
    }
    if (info == NULL && infoLen != 0)
    {
        return PS_ARG_FAIL;
    }

    i = 1;
    while(1)
    {
        /*
          Setup the HMAC input.
          T(1) = HMAC-Hash(PRK, info || 0x01)
          T(k) = HMAC-Hash(PRK, T(k-1) || info || k) for k > 1
        */
        p = buf;
        if (i != 1)
        {
            Memcpy(p, out - hashLen, hashLen); /* Prev. HMAC res. */
            p += hashLen;
        }
        Memcpy(p, info, infoLen);
        p += infoLen;
        *p = i;
        p++;

        /* Compute HMAC. */
        rc = psHmac(hmacAlg, prk, prkLen, buf, p - buf, T);
        if (rc < 0)
        {
            return rc;
        }

        /*
          T = T(1) | T(2) | T(3) | ... | T(N)

          Concatenate the bytes of T(k) to previous output.
          Do we need the full HMAC result or only a prefix of it?
        */
        stillNeeded = outEnd - out;
        if (stillNeeded < hashLen)
        {
            Memcpy(out, T, stillNeeded);
            break;
        }
        else
        {
            Memcpy(out, T, hashLen);
            out += hashLen;
        }
        i++;
    }

#ifdef DEBUG_HKDF
    psTraceBytes("okm", okm, okmLen);
#endif

    return PS_SUCCESS;
}

int32_t psHkdfExtract(psCipherType_e hmacAlg,
                      const unsigned char *salt,
                      psSize_t saltLen,
                      const unsigned char *ikm,
                      psSize_t ikmLen,
                      unsigned char prk[MAX_HASHLEN],
                      psSize_t *prkLen)
{
    int32_t rc;

    if (hmacAlg != HMAC_MD5 && hmacAlg != HMAC_SHA1 &&
            hmacAlg != HMAC_SHA256 && hmacAlg != HMAC_SHA384)
    {
        return PS_ARG_FAIL;
    }

    rc = psHmac(hmacAlg, salt, saltLen, ikm, ikmLen, prk);
    if (rc < 0)
    {
        return rc;
    }

    rc = psGetOutputBlockLength(hmacAlg);
    if (rc < 0)
    {
        return rc;
    }
    *prkLen = rc;
    return PS_SUCCESS;
}

/*
  TLS 1.3 wrapper around HKDF-Expand.
  See section 7.1 of TLS 1.3 draft 23.
*/
int32_t psHkdfExpandLabel(psPool_t *pool,
        psCipherType_e hmacAlg,
        const unsigned char *secret,
        psSize_t secretLen,
        const char *label,
        psSize_t labelLen,
        const unsigned char *context,
        psSize_t contextLen,
        psSize_t length,
        unsigned char *out)
{
    int32_t rc;
    psDynBuf_t hkdfLabelBuf, labelBuf, contextBuf;
    psSize_t hkdfLabelLen;
    unsigned char *vector_data, *hkdf_label;
    psSize_t vector_data_len, hkdf_label_len;

    hkdfLabelLen = 1 + 1 + 1 + 7 + labelLen + contextLen;
    psDynBufInit(pool, &hkdfLabelBuf, hkdfLabelLen);

    /*
       struct {
           uint16 length = Length;
           opaque label<7..255> = "tls13 " + Label;
           opaque context<0..255> = Context;
       } HkdfLabel;
    */

    /* uint16 length = Length; */
    psDynBufAppendByte(&hkdfLabelBuf, ((length & 0xff00) >> 8));
    psDynBufAppendByte(&hkdfLabelBuf, (length & 0xff));

    /* opaque label<7..255> = "tls13 " + Label; */
    psDynBufInit(pool, &labelBuf, labelLen + 7 + 1);
    psDynBufAppendStr(&labelBuf, "tls13 ");
    psDynBufAppendOctets(&labelBuf, label, labelLen);
    vector_data = psDynBufDetachPsSize(&labelBuf, &vector_data_len);
    if (vector_data == NULL)
    {
        return PS_MEM_FAIL;
    }
    rc = psDynBufAppendTlsVector(&hkdfLabelBuf,
            7, 255,
            vector_data, vector_data_len);
    psFree(vector_data, pool);
    if (rc < 0)
    {
        return rc;
    }
    psDynBufUninit(&labelBuf);

    /* opaque context<0..255> = Context; */
    psDynBufInit(pool, &contextBuf, contextLen + 1);
    psDynBufAppendOctets(&contextBuf, context, contextLen);
    vector_data = psDynBufDetachPsSize(&contextBuf, &vector_data_len);
    if (vector_data == NULL)
    {
        return PS_MEM_FAIL;
    }
    rc = psDynBufAppendTlsVector(&hkdfLabelBuf,
            0, 255,
            vector_data, vector_data_len);
    psFree(vector_data, pool);
    if (rc < 0)
    {
        return rc;
    }
    psDynBufUninit(&contextBuf);

    /* Fetch the final HdkfLabel */
    hkdf_label = psDynBufDetachPsSize(&hkdfLabelBuf, &hkdf_label_len);
    if (hkdf_label == NULL)
    {
        return PS_MEM_FAIL;
    }
#ifdef DEBUG_HKDF
    psTraceBytes("hkdf_label", hkdf_label, hkdf_label_len);
#endif

    rc = psHkdfExpand(hmacAlg,
            secret,
            secretLen,
            hkdf_label,
            hkdf_label_len,
            out,
            length);
    if (rc < 0)
    {
        psFree(hkdf_label, pool);
        return rc;
    }

    psFree(hkdf_label, pool);
    return PS_SUCCESS;
}
#endif /* USE_HKDF */
