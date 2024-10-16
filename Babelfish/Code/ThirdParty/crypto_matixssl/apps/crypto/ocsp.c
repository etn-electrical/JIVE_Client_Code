/**
 *      @file    ocsp.c
 *
 *
 *      Simple example of OCSP client program.
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

#include "crypto/cryptoApi.h"
#include "core/coreApi.h"
/* Currently this example uses _psTrace for tracing, so osdep.h is needed: */
#include "osdep_stdio.h"
#include "core/osdep.h"

#if defined USE_PS_NETWORKING && defined USE_OCSP_RESPONSE && defined USE_OCSP_REQUEST && defined USE_CERT_GEN && defined MATRIX_USE_FILE_SYSTEM

/* The string identified used when fetching OCSP response. */
# define TOOL_USER_AGENT_STR "ocsp/1.0"

# include "osdep_stdio.h"         /* Printf() / Fprintf() for standard output. */
# include "osdep_strings.h"       /* strncasecmp() for case-insensitive comparison. */
# include "osdep_stdlib.h"        /* EXIT_SUCCESS and EXIT_FAILURE. */
# include "osdep_signal.h"        /* Defines SIGTERM, etc. */
# include "osdep_inttypes.h"      /* SCNu8 */

/* Example for creating an OCSP Request and fetching the OCSP Response
    from a server */

# define OCSP_RESPONSE_BUF_SIZE  4096 /* max size of incoming OCSPResponse */
# define DEFAULT_USE_NONCE 1          /* Default for using nonce (on.) */

/* format struct tm representing UTC time for output.
   Warning: This implementation uses a static formatting buffer. */
const char *format_time(psBrokenDownTime_t *tm_time)
{
    static char timestr[80];

    (void) mktime(tm_time); /* Compute day of week. */
    strftime(timestr, 80, "%a, %d %b %Y %I:%M:%S %p UTC", tm_time);
    return timestr;
}

/* Map revocation reason to readable string. */
const char *mapRevocationReason(x509CrlReason_t revocationReason)
{
    const char *reasonstr;

    switch (revocationReason)
    {
    case PS_CRLREASON_KEY_COMPROMISE:
        reasonstr = "key compromise";
        break;
    case PS_CRLREASON_CA_COMPROMISE:
        reasonstr = "CA compromise";
        break;
    case PS_CRLREASON_AFFILIATION_CHANGED:
        reasonstr = "affiliation changed";
        break;
    case PS_CRLREASON_SUPERSEDED:
        reasonstr = "superseded";
        break;
    case PS_CRLREASON_CESSATION_OF_OPERATION:
        reasonstr = "cessation of operation";
        break;
    case PS_CRLREASON_CERTIFICATE_HOLD:
        reasonstr = "certificate hold";
        break;
    case PS_CRLREASON_REMOVE_FROM_CRL:
        reasonstr = "remove from CRL";
        break;
    case PS_CRLREASON_PRIVILEGE_WITHDRAWN:
        reasonstr = "privilege withdrawn";
        break;
    case PS_CRLREASON_AA_COMPROMISE:
        reasonstr = "AA compromise";
        break;
    default:
        reasonstr = "unspecified";
    }
    return reasonstr;
}

/* Get OCSP response using HTTP protocol (via psUrlInteract function.) */
static int32 getOCSPResponse(const char *url,
    unsigned char *request,
    size_t requestLen,
    unsigned char *ocspResponseBuf,
    size_t *ocspResponseBufLen_p)
{
    int32 err;

    /* HTTP headers for OCSP request/response. */
    static const char *ocsp_request_headers[] = {
        "Content-Type", "User-Agent"
    };

    static const char *ocsp_request_header_values[] = {
        "application/ocsp-request", TOOL_USER_AGENT_STR
    };

    static const char *ocsp_response_headers[] = {
        "Content-Type"
    };
    char ocsp_response_content_type[] =
        "(space reserved to be filled in by HTTP request)";
    char *ocsp_response_header_values_p[] = { ocsp_response_content_type };
    size_t ocsp_response_header_values_lengths =
        sizeof ocsp_response_content_type;

    err = psUrlInteract("POST", url,
        ocsp_request_headers,
        ocsp_request_header_values, 2,
        request, requestLen,
        ocsp_response_headers,
        ocsp_response_header_values_p,
        &ocsp_response_header_values_lengths, 1,
        ocspResponseBuf, ocspResponseBufLen_p, NULL);

    /* Check content-type */
    if (err == PS_SUCCESS &&
        strcasecmp(ocsp_response_content_type,
            "application/ocsp-response") != 0)
    {
        /* Unexpected Content-Type. */
        return PS_FAILURE;
    }
    return err;
}

static char *getAuthorityInfoOCSP_URI(const psX509Cert_t *subject)
{
    char *url = NULL;
    x509authorityInfoAccess_t *authInfo;

    authInfo = subject->extensions.authorityInfoAccess;

    /* Find the first AuthorityInfoAccess extension with OCSP. */
    while (authInfo != NULL)
    {
        if (authInfo->ocsp && authInfo->ocspLen > 0)
        {
            url = Calloc(1, authInfo->ocspLen + 1);
            if (url)
            {
                Memcpy(url, authInfo->ocsp, authInfo->ocspLen);
            }
            break;
        }
        authInfo = authInfo->next;
    }
    return url;
}

/* Identify certificate */
static void printCert(const char *file, const psX509Cert_t *cert, int ln)
{
    const char *cnStr;
    const char *optLf = ln ? "\n" : "";   /* Optional linefeed. */

    /* Usually commonName identifies subject relatively well for reader of
       software output. Currently this function will just output that. */

    cnStr = cert->subject.commonName;
    if (cnStr)
    {
        Printf("Certificate for %s: %s", cnStr, optLf);
    }
    else
    {
        /* No commonName, use filename. */
        Printf("Certificate from file %s: %s", file, optLf);
    }
}

/* Store 1-3 certificates (from OCSP response). */
static void storeCerts(const char *storeCertFile,
    const char *storeCertFile2,
    const char *storeCertFile3,
    const psX509Cert_t *store_cert)
{
    if (store_cert)
    {
        if (store_cert->unparsedBin)
        {
            if (psBase64EncodeAndWrite(
                    NULL, storeCertFile,
                    store_cert->unparsedBin, store_cert->binLen,
                    CERT_FILE_TYPE, NULL, 0) >= 0)
            {
                Printf("Successfully written certificate to %s\n",
                    storeCertFile);
            }
            else
            {
                Printf("Failure writing file %s\n", storeCertFile);
            }
        }
        else
        {
            Printf("Certificate not stored: no raw certificate data\n");
        }
        store_cert = store_cert->next; /* follow certificate list */
    }
    else
    {
        Printf("Certificate not stored: "
            "the OCSP response does not contain a certificate\n");
    }
    if (storeCertFile2 && store_cert)
    {
        if (store_cert->unparsedBin)
        {
            if (psBase64EncodeAndWrite(
                    NULL, storeCertFile2,
                    store_cert->unparsedBin, store_cert->binLen,
                    CERT_FILE_TYPE, NULL, 0) >= 0)
            {
                Printf("Successfully written certificate#2 to %s\n",
                    storeCertFile2);
            }
            else
            {
                Printf("Failure writing file %s\n", storeCertFile2);
            }
        }
        else
        {
            Printf("Certificate#2 not stored: no raw certificate data\n");
        }
        store_cert = store_cert->next; /* Follow list. */
    }
    if (storeCertFile3 && store_cert)
    {
        if (store_cert->unparsedBin)
        {
            if (psBase64EncodeAndWrite(
                    NULL, storeCertFile3,
                    store_cert->unparsedBin, store_cert->binLen,
                    CERT_FILE_TYPE, NULL, 0) >= 0)
            {
                Printf("Successfully written certificate#3 to %s\n",
                    storeCertFile3);
            }
            else
            {
                Printf("Failure writing file %s\n", storeCertFile3);
            }
        }
        else
        {
            Printf("Certificate#3 not stored: no raw certificate data\n");
        }
        store_cert = store_cert->next;
    }
}

/* This function inspects received OCSP Response against the request and
   prints terse or verbose output interpretation of the response. */
int32 OCSPResponseCheckAndPrint(const char *cert,
    unsigned char *ocspResponseBuf,
    size_t ocspResponseBufLen,
    psX509Cert_t *subject,
    psX509Cert_t *issuer,
    const unsigned char *request,
    uint32 requestLen,
    const char *storeCertFile,
    const char *storeCertFile2,
    const char *storeCertFile3,
    int verbose,
    int summary,
    int *persistent_error)
{
    int32 err;

    struct tm timeNow = { 0 };
    struct tm ProducedAt = { 0 };
    struct tm thisUpdate = { 0 };
    struct tm nextUpdate = { 0 };
    struct tm revocationTime = { 0 };

    x509CrlReason_t revocationReason = PS_CRLREASON_UNSPECIFIED;
    psOcspResponse_t response;

    static psValidateOCSPResponseOptions_t opts;
    psBool_t known = PS_FALSE;
    psBool_t revocated = PS_FALSE;
    psBool_t nonceOk = PS_TRUE;
    unsigned char *ocspResponseBufPtr;
    int32 index = -1;

    *persistent_error = 1;

    /* Parse the response received. */
    ocspResponseBufPtr = ocspResponseBuf;
    err = psOcspParseResponse(NULL, ocspResponseBufLen,
        &ocspResponseBufPtr,
        ocspResponseBuf + ocspResponseBufLen,
        &response);
    if (err != PS_SUCCESS)
    {
        int res = psOcspResponseGetStatus(err);
        const char *resStr[7] = {
            "(successful)",
            "malformed request",
            "internal error",
            "try later",
            "(reserved for future use)",
            "signature required",
            "request unauthorized"
        };
        if (err == PS_MESSAGE_UNSUPPORTED)
        {
            char *oid = NULL;
            if (response.responseType != NULL)
            {
                /* Use a generic OID formatter. */
                oid = asnFormatOid(NULL, response.responseType,
                    response.responseType[1] + 2);
            }
            if (oid)
            {
                /* Display formatted version of the OID. */
                Fprintf(stderr, "Unsupported OCSP Response OID: %s\n", oid);
                psFree(oid, NULL);
            }
            else
            {
                /* OID cannot be formatted. */
                Fprintf(stderr, "Unsupported OCSP Response.\n");
            }
        }
        else if (err == PS_VERSION_UNSUPPORTED)
        {
            Fprintf(stderr, "Unsupported OCSP Basic Response version: v%d\n",
                response.version + 1);
        }

        if (res > 0)
        {
            Fprintf(stderr, "OCSP Response Error: %s\n", resStr[res]);
        }
        else
        {
            Fprintf(stderr, "OCSP parsing failed!\n");
        }
        return err;
    }

    *persistent_error = 0;

    /* Format response OID (currently always OCSP Basic Response). */
    if (verbose && response.responseType != NULL)
    {
        /* Use a generic OID formatter. */
        char *oid = asnFormatOid(NULL, response.responseType,
            response.responseType[1] + 2);
        if (oid)
        {
            /* Display formatted version of the OID. */
            Printf("OCSP Response OID: %s%s\n",
                oid,
                !Strcmp(oid, "1.3.6.1.5.5.7.48.1.1") ?
                " (id-pkix-ocsp-basic)" : "");
            psFree(oid, NULL);
        }
    }

    if (verbose || summary)
    {
        printCert(cert, subject, verbose);
    }

    /* Set up options, to get detailed information on revocation. */
    opts.knownFlag = &known;
    opts.revocationFlag = &revocated;
    opts.nonceMatch = &nonceOk;
    opts.revocationTime = &revocationTime;
    opts.revocationReason = &revocationReason;
    opts.index_p = &index;

    /* For matching nonce, it is neccessary to also provide request. */
    opts.request = request;
    opts.requestLen = requestLen;

    err = psOcspResponseValidate(NULL, issuer, subject, &response, &opts);
    /* The psOcspResponseCheckDates function gets the dates specified in
       response. The function usually does not need to be called separately,
       unless caller wishes to get exact validity time information rather than
       to validate against the current time. */

    if (index > -1 && verbose)
    {
        int32 err_times;

        /* If the response was gotten and we're verbose, obtain validity
           time information. */
        err_times = psOcspResponseCheckDates(&response,
            index,
            &timeNow,
            &ProducedAt,
            &thisUpdate,
            &nextUpdate,
            PS_OCSP_TIME_LINGER);
        if (err_times != PS_SUCCESS && err != PS_TIMEOUT_FAIL)
        {
            Fprintf(stderr,
                "Could not parse validity dates in OCSP response.\n");
            psOcspResponseUninit(&response);
            return err_times;
        }

        if (err_times == PS_TIMEOUT_FAIL)
        {
            Fprintf(stderr, "Warning: The OCSP response is unreliable.\n");
        }

        /* Use "human readable" indexing starting from 1. */
        Fprintf(stderr, "Response #%d\n", index + 1);
        Fprintf(stderr, "Validated on %s\n", format_time(&timeNow));
        Fprintf(stderr, "Produced at %s\n", format_time(&ProducedAt));
        Fprintf(stderr, "This Update: %s\n", format_time(&thisUpdate));
        if (Memcmp(&thisUpdate, &nextUpdate, sizeof thisUpdate) != 0)
        {
            Fprintf(stderr, "Next Update: %s\n",
                format_time(&nextUpdate));
        }
    }

    if (storeCertFile && (verbose || summary))
    {
        storeCerts(storeCertFile, storeCertFile2, storeCertFile3,
            response.OCSPResponseCert);
    }

    if (verbose)
    {
        if (err != PS_SUCCESS)
        {
            if (known == PS_TRUE && revocated == PS_TRUE)
            {
                const char *reasonstr = mapRevocationReason(revocationReason);
                Fprintf(stderr, "Revoked on %s\n",
                    format_time(&revocationTime));
                Fprintf(stderr, "Revocation reason: %s\n", reasonstr);
            }
            else if (known == PS_FALSE )
            {
                Fprintf(stderr,
                    "Could not obtain validity information.\n");
            }
            else
            {
                Fprintf(stderr, "OCSP validation failed!\n");
            }
            /* Note: err has been set. */
        }
    }
    else if (summary)
    {
        if (err == PS_SUCCESS)
        {
            Printf("OK.\n");
        }
        else if (err == PS_CERT_AUTH_FAIL_REVOKED &&
                 revocationReason != PS_CRLREASON_UNSPECIFIED)
        {
            Printf("Revoked (%s)!\n", mapRevocationReason(revocationReason));
        }
        else if (err == PS_CERT_AUTH_FAIL_REVOKED)
        {
            Printf("Revoked!\n");
        }
        else
        {
            Printf("FAILED.\n");
        }
    }

    if (!nonceOk)
    {
        Fprintf(stderr, "Warning: Nonce mismatch detected: "
            "result cannot be trusted.\n");
    }

    if (verbose && nonceOk && response.nonce.start != response.nonce.end)
    {
        unsigned char *p;
        Fprintf(stderr, "OCSP Nonce Match: ");
        for (p = response.nonce.start; p < response.nonce.end; p++)
        {
            Fprintf(stderr, "%02x", *p);
        }
        Fprintf(stderr, "\n");
    }

    if (verbose)
    {
        Fprintf(stderr, "\n");
    }

    psOcspResponseUninit(&response);
    return err;
}

/* Load resources for signing and add to info request for signing. */
static void loadKeyAndRequestSign(psPool_t *pool,
    psOcspRequestWriteInfo_t *info_p,
    const char *signFile,
    char *signCertFile,
    psPubKey_t **signKey_pp,
    psX509Cert_t **signCert_pp)
{
    int32 rc;
    int32 alg_oid = 0;

    /* Note: on errors this function provides error message and exists. */

# ifdef USE_RSA
    rc = psNewPubKey(pool, PS_RSA, signKey_pp);
    if (rc == PS_SUCCESS)
    {
        rc = psPkcs1ParsePrivFile(pool, signFile, NULL, &(*signKey_pp)->key.rsa);
        if (rc != PS_SUCCESS)
        {
            psDeletePubKey(signKey_pp);
        }
        else
        {
            (*signKey_pp)->keysize =
                (uint16_t) psRsaSize(&(*signKey_pp)->key.rsa);
            alg_oid = OID_SHA256_RSA_SIG;
        }
    }
# else
    *signKey_pp = NULL;
# endif
# ifdef USE_ECC
    if (*signKey_pp == NULL)
    {
        rc = psNewPubKey(pool, PS_ECC, signKey_pp);
        if (rc == PS_SUCCESS)
        {
            rc = psEccParsePrivFile(pool, signFile, NULL,
                &(*signKey_pp)->key.ecc);
            if (rc != PS_SUCCESS)
            {
                psDeletePubKey(signKey_pp);
            }
            else
            {
                (*signKey_pp)->keysize =
                    (uint16_t) psEccSize(&(*signKey_pp)->key.ecc);
                alg_oid = OID_SHA256_ECDSA_SIG;
            }
        }
    }
# endif /* USE_ECC */
    if (*signKey_pp == NULL)
    {
        Fprintf(stderr, "Unable to read signing key: %s\n", signFile);
        exit(1);
    }

    /* The option CERT_STORE_UNPARSED_BUFFER is mandatory for this purpose. */
    if (psX509ParseCertFile(NULL, signCertFile, signCert_pp,
            CERT_STORE_UNPARSED_BUFFER) < 0)
    {
        Fprintf(stderr, "Certificate Loading Failed: Unable to load %s\n",
            signCertFile);
        exit(1);
    }

    if (psOcspRequestWriteInfoSetSigning(info_p, alg_oid,
            *signKey_pp, *signCert_pp,
            0) != PS_SUCCESS)
    {
        Fprintf(stderr, "Unable to setup request info for signing.\n");
        exit(1);
    }

    /* Note: signKey_pp and signCert_pp shall be freed after use. */
}

/* Compose OCSP request and retrieve OCSP response.
   The OCSP response will be inspected by OCSPResponseCheckAndPrint(). */
int32 OCSPRequestAndResponseTest(const char *url,
    char *issuerFile,
    char *cert,
    char *cert2, char *cert3,
    const char *requestor,
    const char *requestFile,
    const char *replyFile,
    const char *storeCertFile,
    const char *storeCertFile2,
    const char *storeCertFile3,
    const char *signFile,
    char *signCertFile,
    int verbose,
    int use_nonce,
    int req_version)
{
    psX509Cert_t *subject, *issuer;
    unsigned char ocspResponseBuf[OCSP_RESPONSE_BUF_SIZE];
    int32 err, err2, err3;
    uint32 requestLen;
    unsigned char *request = NULL;
    size_t ocspResponseBufLen = sizeof(ocspResponseBuf);
    char *url_alloc = NULL;
    /* Identify certificates if verbose or if checking multiple certs. */
    int id_cert = verbose || cert2 != NULL;
    psOcspRequestWriteInfo_t info = { 0 };
    int no_more;
    psX509Cert_t *signCert = NULL;
    psPubKey_t *signKey = NULL;

    /* An OCSP Request requires that the subject cert and the parent of the
       subject be parsed in psX509Cert_t structures. This example requests
       OCSP response based on subject(s) and parent (issuer) certificates.
       The psOcspRequestWrite accepts list, but this function
       uses static maximum 3 (which is the same than default maximum
       for replies). */

    /* Parse subject certificate(s). */
    if (psX509ParseCertFile(NULL, cert, &subject, 0) < 0)
    {
        _psTraceStr("OCSP Test Failed: Unable to load %s\n", cert);
        return PS_FAILURE;
    }
    if (cert2 && (psX509ParseCertFile(NULL, cert2,
                      &(subject->next), 0) < 0))
    {
        _psTraceStr("OCSP Test Failed: Unable to load %s\n", cert);
        psX509FreeCert(subject);
        return PS_FAILURE;
    }
    if (cert3 && (psX509ParseCertFile(NULL, cert3,
                      &(subject->next->next), 0) < 0))
    {
        _psTraceStr("OCSP Test Failed: Unable to load %s\n", cert);
        psX509FreeCert(subject);
        return PS_FAILURE;
    }

    /* Parse issuer/CA certificate. */
    if (psX509ParseCertFile(NULL, issuerFile,
            &issuer, 0) < 0)
    {
        psX509FreeCert(subject);
        _psTraceStr("OCSP Test Failed: Unable to load %s\n",
            issuerFile);
        return PS_FAILURE;
    }

    if (req_version)
    {
        psOcspRequestWriteVersion(req_version, &info);
    }

    if (requestor)
    {
        uint8_t ip[4];
        int32 rv;

        /* The set of requestor identifiers supported is currently limited
           to these options: */
        if (!strncasecmp(requestor, "email:", 6))
        {
            rv = psOcspRequestWriteInfoSetRequestorId(
                NULL, &info, requestor + 6, Strlen(requestor + 6), GN_EMAIL);
        }
        else if (!strncasecmp(requestor, "uri:", 4))
        {
            rv = psOcspRequestWriteInfoSetRequestorId(
                NULL, &info, requestor + 4, Strlen(requestor + 4), GN_URI);
        }
        else if (!strncasecmp(requestor, "dns:", 4))
        {
            rv = psOcspRequestWriteInfoSetRequestorId(
                NULL, &info, requestor + 4, Strlen(requestor + 4), GN_DNS);
        }
        else if (Sscanf(requestor, "ip:%" SCNu8 ".%" SCNu8 ".%" SCNu8 ".%" SCNu8,
                     ip, ip + 1, ip + 2, ip + 3) == 4)
        {
            rv = psOcspRequestWriteInfoSetRequestorId(
                NULL, &info, (char *) ip, 4, GN_IP);
        }
        else if (!strncasecmp(requestor, "dn:", 3))
        {
            int32 dn_len;
            psBuf_t dn_buf = { 0 };
            /* For X.509 Name objects we use psWriteCertDNAttributes()
               function to encode string representation to ASN.1 DER byte
               sequence. */
            rv = psWriteCertDNAttributes(NULL, &dn_buf, (char *) requestor + 3,
                    Strlen(requestor + 3), &dn_len, NULL);
            if (rv == PS_SUCCESS)
            {
                rv = psOcspRequestWriteInfoSetRequestorId(
                    NULL, &info, (char *) dn_buf.start, dn_len,
                    GN_DIR);
                psFree(dn_buf.buf, NULL);
            }
        }
        else
        {
            rv = PS_FAILURE; /* Unknown requestor GeneralName type. */

        }
        if (rv != PS_SUCCESS)
        {
            psX509FreeCert(subject);
            psX509FreeCert(issuer);
            _psTrace("OCSP Test Failed: Unable to set requester Id\n");
            return PS_FAILURE;
        }
    }
    if (cert2)
    {
        /* Multiple certificates requires a flag. */
        info.flags |= MATRIXSSL_WRITE_OCSP_REQUEST_FLAG_CERT_LIST;
    }

    /* Request nonce extension if enabled. */
    info.flags |= use_nonce * MATRIXSSL_WRITE_OCSP_REQUEST_FLAG_NONCE;

    if (signFile)
    {
        loadKeyAndRequestSign(NULL, &info, signFile, signCertFile,
            &signKey, &signCert);
    }
    if (psOcspRequestWrite(NULL, subject, issuer,
            &request, &requestLen, &info) < 0)
    {
        _psTrace("OCSP Test Failed: Unable to generate OCSP Request\n");
        psX509FreeCert(subject);
        psX509FreeCert(issuer);
        if (signKey)
        {
            psDeletePubKey(&signKey);
        }
        if (signCert)
        {
            psX509FreeCert(signCert);
        }
        psOcspRequestWriteInfoFreeRequestorId(NULL, &info);
        return PS_FAILURE;
    }

    /* Free all parts of info and releated resources no longer used. */
    psOcspRequestWriteInfoFreeRequestorId(NULL, &info);

    if (signKey)
    {
        psDeletePubKey(&signKey);
    }
    if (signCert)
    {
        psX509FreeCert(signCert);
    }

    /* Store request if user wanted to. */
    if (requestFile)
    {
        int res = 0;
        FILE *tmp = Fopen(requestFile, "w");
        if (tmp)
        {
            res = Fwrite(request, requestLen, 1, tmp);
        }
        if (tmp)
        {
            Fclose(tmp);
        }
        if (res != 1)
        {
            /* Write diagnostic, but continue after write error, as this
               option is just a debugging option. */
            Fprintf(stderr, "writing to file %s failed.\n",
                requestFile);
        }
    }

    /* Get URL from subject certificate, if specific url has not been provided.
       Note: This is taken from the primary certificate with assumption that
       the others can be queried from the same OCSP responder. */
    if (url == NULL)
    {
        url = url_alloc = getAuthorityInfoOCSP_URI(subject);
        if (url == NULL)
        {
            Fprintf(stderr, "Unable to obtain OCSP Authority info.\n");
            Fprintf(stderr, "Please, provide -url manually.\n");
            exit(1);
        }
        if (verbose)
        {
            Fprintf(stderr, "Requesting OCSP from URI: %s\n", url);
        }
    }

    /* Interact with OCSP server to get OCSP response. */
    err = getOCSPResponse(url, request, requestLen,
        ocspResponseBuf, &ocspResponseBufLen);
    Free(url_alloc); /* Free possible temporary memory for url. */
    if (err != PS_SUCCESS)
    {
        /* The HTTP response was something else than 200 (OK), or some
           other error was detected fetching the response. */
        if (err >= 100)
        {
            _psTraceInt("HTTP POST request unexpectedly returned: %d\n",
                (int) err);
        }
        Fprintf(stderr, "Unable to obtain OCSP response\n");
        psX509FreeCert(subject);
        psX509FreeCert(issuer);
        psFree(request, MATRIX_NO_POOL);
        exit(1);
    }

    /* Write reply to file system. */
    if (replyFile)
    {
        int res = 0;
        FILE *tmp = Fopen(replyFile, "w");
        if (tmp)
        {
            res = Fwrite(ocspResponseBuf, ocspResponseBufLen,
                1, tmp);
        }
        if (tmp)
        {
            Fclose(tmp);
        }
        if (res != 1)
        {
            /* Write diagnostic, but continue after write error, as this
               option is just a debugging option. */
            Fprintf(stderr, "writing to file %s failed.\n",
                replyFile);
        }
    }

    /* Check the response with respect to the first certificate. */
    err = OCSPResponseCheckAndPrint(cert, ocspResponseBuf, ocspResponseBufLen,
        subject, issuer, request, requestLen,
        storeCertFile, storeCertFile2,
        storeCertFile3, verbose, id_cert, &no_more);

    if (no_more)
    {
        /* Persistent error, there is no use trying remaining certificates. */
        cert2 = cert3 = NULL;
    }

    /* Equivalently, check the response against the remaining certificates. */
    err3 = err2 = err;
    if (cert2)
    {
        err2 = OCSPResponseCheckAndPrint(cert2,
            ocspResponseBuf, ocspResponseBufLen,
            subject->next,
            issuer, request, requestLen, NULL,
            NULL, NULL,
            verbose, id_cert, &no_more);

        if (cert3)
        {
            err3 = OCSPResponseCheckAndPrint(cert3,
                ocspResponseBuf,
                ocspResponseBufLen,
                subject->next->next, issuer,
                request, requestLen, NULL,
                NULL, NULL, verbose,
                id_cert, &no_more);
        }
    }

    /* Free certificates. */
    psX509FreeCert(subject);
    psX509FreeCert(issuer);

    /* Free request. */
    psFree(request, MATRIX_NO_POOL);

    /* Summarize situation that some of certificates were found revocated
       into response. */
    if ((err != err2 || err2 != err3) &&
        (err == PS_SUCCESS || err2 == PS_SUCCESS || err3 == PS_SUCCESS))
    {
        if (verbose)
        {
            Fprintf(stderr,
                "Only some of the certificates were considered ok.\n");
        }
        /* Return error rather than success if just some part of checking
           failed. */
        if (err == PS_SUCCESS)
        {
            err = err2;
        }
        if (err == PS_SUCCESS)
        {
            err = err3;
        }
    }
    return err;
}

/******************************************************************************/
/*
    Main function for argument parsing.
 */
int32 main(int32 argc, char **argv)
{
    int32 rc;
    char *issuer = NULL;
    char *cert = NULL;
    char *cert2 = NULL;
    char *cert3 = NULL;
    char *url = NULL;
    char *request = NULL;
    char *reply = NULL;
    char *requestor = NULL;
    char *store_cert = NULL;
    char *store_cert2 = NULL;
    char *store_cert3 = NULL;
    char *sign = NULL;
    char *sign_cert = NULL;
    int32 res;
    int verbose = 0;
    int use_nonce = DEFAULT_USE_NONCE;
    int req_version = MATRIXSSL_OCSP_VERSION_V1;

    rc = psCryptoOpen(PSCRYPTO_CONFIG);
    if (rc < 0)
    {
        _psTrace("MatrixSSL crypto library init failure.  Exiting\n");
        return EXIT_FAILURE;
    }

    while (argc >= 2)
    {
        /* Single options. */
        if (!Strcmp(argv[1], "-verbose"))
        {
            verbose++;
            argv[1] = argv[0];
            argv += 1;
            argc -= 1;
        }
        else if (!Strcmp(argv[1], "-nonce"))
        {
            use_nonce = 1;
            argv[1] = argv[0];
            argv += 1;
            argc -= 1;
        }
        else if (!Strcmp(argv[1], "-no_nonce"))
        {
            use_nonce = 0;
            argv[1] = argv[0];
            argv += 1;
            argc -= 1;
        }
        else if (argc >= 3)
        {
            /* Options with parameter. */
            if (!Strcmp(argv[1], "-url"))
            {
                url = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-issuer"))
            {
                issuer = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-requestor"))
            {
                requestor = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-cert"))
            {
                cert = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-cert2"))
            {
                cert2 = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-cert3"))
            {
                cert3 = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-request"))
            {
                request = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-version"))
            {
                req_version = atoi(argv[2]);
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-reply"))
            {
                reply = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-store-cert"))
            {
                store_cert = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-store-cert2"))
            {
                store_cert2 = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-store-cert3"))
            {
                store_cert3 = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-sign"))
            {
                sign = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else if (!Strcmp(argv[1], "-sign-cert"))
            {
                sign_cert = argv[2];
                argv[2] = argv[0];
                argv += 2;
                argc -= 2;
            }
            else
            {
                break; /* Option not recognized. */
            }
        }
        else
        {
            break; /* Option not recognized. */
        }
    }

    if (argc != 1 || issuer == NULL || cert == NULL ||
        (cert3 != NULL && cert2 == NULL))
    {
        Fprintf(stderr, "Usage: %s " /* -CAfile ca-cert.pem */
            "-issuer ca-cert.pem "
            "-cert server-cert.pem "
            "[-cert2 server-cert2.pem "
            "[-cert3 server-cert3.pem]] "
            "[-requestor [email:|ip:|dns:|uri:|dn:]address_or_name] "
            "[-url http://target.com/] "
            "[-request request.der] "
            "[-reply reply.der] "
            "[-store-cert signer-cert-gotten.pem "
            "[-store-cert2 signer-cert-gotten.pem "
            "[-store-cert3 signer-cert-gotten.pem]]] "
            "[-sign signer-private-key.pem] "
            "[-sign-cert signer-private-key-certificate.pem] "
            "[-version 0] "
            "[-verbose] "
            "[-nonce] "
            "[-no_nonce]"
            "\n", argv[0]);
        exit(1);
    }

# ifdef POSIX
    /* Ignore SIGPIPE on POSIX-like systems. This prevents fatal failure
       from this program in certain kind of communication problems. */
    Signal(SIGPIPE, SIG_IGN);
# endif /* POSIX */

    res = OCSPRequestAndResponseTest(url, issuer, cert, cert2, cert3,
        requestor, request, reply, store_cert,
        store_cert2, store_cert3, sign,
        sign_cert, verbose,
        use_nonce, req_version);
    if (cert2 == NULL || verbose)
    {
        if (res == 0)
        {
            Printf("OCSP validation successful\n");
        }
        else
        {
            Fprintf(stderr, "OCSP validation error: %d\n", res);
        }
    }

    psCryptoClose();

    return res == PS_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}

#else

/******************************************************************************/
/*
    Stub main for compiling without server enabled
 */
int32 main(int32 argc, char **argv)
{
    Printf("USE_PS_NETWORKING, USE_OCSP_RESPONSE, USE_OCSP_REQUEST, USE_CERT_GEN "
        "and MATRIX_USE_FILE_SYSTEM must be enabled at build" \
        " time to run this application\n");
    return EXIT_FAILURE;
}
#endif /* defined USE_PS_NETWORKING && defined USE_OCSP &&
          defined USE_CERT_GEN && defined MATRIX_USE_FILE_SYSTEM */

/******************************************************************************/
