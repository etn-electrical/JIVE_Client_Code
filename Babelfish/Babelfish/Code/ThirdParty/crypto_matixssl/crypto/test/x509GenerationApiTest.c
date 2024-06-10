/* x509GenerationApiTest.c
 * Tests for CSR and certificate generation APIs (psX509Set* and psX509Add*).
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

#include "../cryptoImpl.h"
#include "osdep_stdlib.h"
#include "osdep_assert.h"
#include "osdep_stdio.h"

#if defined(USE_CERT_GEN) && defined(MATRIX_USE_FILE_SYSTEM)

# include "coreApi.h" /* For psOpenPool, etc. */

/* Check some values in the generated certs using the openssl x509 tool. */
#ifndef USE_OPENSSL_CMD
/* #define USE_OPENSSL_CMD */
#endif

/* For testing challenge password extension: when
   set to 1, createCASignedCert will process challenge password
   of UTF-8("foot") and . */
static int expect_challengepassword;

#ifdef USE_OPENSSL_CMD
#include "osdep_stdarg.h"
static
int my_asprintf(char **str, char *format, ...);
#endif /* USE_OPENSSL_CMD */

typedef enum
{
    KEY_USAGE_DEFAULT = 0,
    KEY_USAGE_DECIPHER,
    KEY_USAGE_ENCIPHER
} usage_type;

int32 writeMaximalDNToConfig(psPool_t *pool,
    psCertConfig_t *conf)
{
    int32 rc;

    rc = psX509SetDNAttribute(pool,
        conf,
        "country",
        Strlen("country"),
        "FI",
        Strlen("FI"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "stateOrProvince",
        Strlen("stateOrProvince"),
        "Uusimaa",
        Strlen("Uusimaa"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "locality",
        Strlen("locality"),
        "Helsinki;Kamppi",
        Strlen("Helsinki;Kamppi"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "organizationalUnit",
        Strlen("organizationalUnit"),
        "INSIDE Secure R&D",
        Strlen("INSIDE Secure R&D"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "organization",
        Strlen("organization"),
        "INSIDE Secure",
        Strlen("INSIDE Secure"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "givenName",
        Strlen("givenName"),
        "John",
        Strlen("John"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "surname",
        Strlen("surname"),
        "Smith",
        Strlen("Smith"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "name",
        Strlen("name"),
        "John W. Smith II",
        Strlen("John W. Smith II"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "generationQualifier",
        Strlen("generationQualifier"),
        "II",
        Strlen("II"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "initials",
        Strlen("initials"),
        "J. W. S.",
        Strlen("J. W. S."),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "title",
        Strlen("title"),
        "Dr.",
        Strlen("Dr."),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "streetAddress",
        Strlen("streetAdress"),
        "3rd Avenue; Apartment 6; Postal code = \"12345\"",
        Strlen("3rd Avenue; Apartment 6; Postal code = \"12345\""),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "telephoneNumber",
        Strlen("telephoneNumber"),
        "+3581234565789",
        Strlen("+3581234565789"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "commonName",
        Strlen("commonName"),
        "insidesecure.com",
        Strlen("insidesecure.com"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "domainComponent",
        Strlen("domainComponent"),
        "com",
        Strlen("com"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "domainComponent",
        Strlen("domainComponent"),
        "insidesecure",
        Strlen("insidesecure"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "domainComponent",
        Strlen("domainComponent"),
        "test",
        Strlen("test"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "uid",
        Strlen("uid"),
        "666",
        Strlen("666"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "dnQualifier",
        Strlen("dnQualifier"),
        "\"myDnQualifier\"",
        Strlen("\"myDnQualifier\""),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "email",
        Strlen("email"),
        "test.email@insidesecure.com",
        Strlen("test.email@insidesecure.com"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    return PS_SUCCESS;
}

/* Demonstrage usage of older supported ASN.1 string types
   in DN attribute encoding. */
int32 writeOldAsn1StringTypeDNToConfig(psPool_t *pool,
    psCertConfig_t *conf)
{
    int32 rc;

    rc = psX509SetDNAttribute(pool,
        conf,
        "country",
        Strlen("country"),
        "FI",
        Strlen("FI"),
        ASN_PRINTABLESTRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "dnQualifier",
        Strlen("dnQualifier"),
        "myDNQualifier",
        Strlen("myDNQualifier"),
        ASN_PRINTABLESTRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "serialNumber",
        Strlen("serialNumber"),
        "235713192129",
        Strlen("235713192129"),
        ASN_PRINTABLESTRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "commonName",
        Strlen("commonName"),
        "test.insidesecure.com",
        Strlen("test.insidesecure.com"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "domainComponent",
        Strlen("domainComponent"),
        "test",
        Strlen("test"),
        ASN_IA5STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "domainComponent",
        Strlen("domainComponent"),
        "insidesecure",
        Strlen("insidesecure"),
        ASN_IA5STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "domainComponent",
        Strlen("domainComponent"),
        "com",
        Strlen("com"),
        ASN_IA5STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        conf,
        "email",
        Strlen("email"),
        "test@insidesecure.com",
        Strlen("test@insidesecure.com"),
        ASN_IA5STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    return PS_SUCCESS;
}

int32 writeSubjectAltNameToConfig(psPool_t *pool,
    psCertConfig_t *conf)
{
    subjectAltNameEntry_t sanEntry;
    int32 rc;

    Memset(&sanEntry, 0, sizeof(subjectAltNameEntry_t));
    sanEntry.rfc822Name = "email@address.com";
    sanEntry.rfc822NameLen = Strlen(sanEntry.rfc822Name);
    sanEntry.dNSName = "insidesecure.com";
    sanEntry.dNSNameLen = Strlen(sanEntry.dNSName);
    sanEntry.uniformResourceIdentifier = "http://www.insidesecure.com";
    sanEntry.uniformResourceIdentifierLen = Strlen(sanEntry.uniformResourceIdentifier);
    sanEntry.iPAddress = "46.105.106.144";
    sanEntry.iPAddressLen = Strlen(sanEntry.iPAddress);
    /* Old approach: give the otherName OID as a hex string containing the DER
       encoded octets without the tag and the length octets. Still works,
       but the new approach below is easier to use. */
    /*
    sanEntry.otherName = "2A864886F70D:some other identifier";
    sanEntry.otherNameLen = Strlen(sanEntry.otherName);
    */
    /* New approach: give the otherName OID as a dot-notation string. */
    sanEntry.otherNameOIDDotNotation = "1.2.840.113549:some other identifier";
    sanEntry.otherNameOIDDotNotationLen = Strlen(sanEntry.otherNameOIDDotNotation);
    rc = psX509AddSubjectAltName(pool,
        conf,
        &sanEntry);
    if (rc < 0)
    {
        Printf("psX509AddSubjectAltName failed\n");
        return PS_FAILURE;
    }

    Memset(&sanEntry, 0, sizeof(subjectAltNameEntry_t));
    sanEntry.rfc822Name = "another.email@address.com";
    sanEntry.rfc822NameLen = Strlen("another.email@address.com");
    sanEntry.otherName = NULL;
    sanEntry.iPAddress = NULL;
    sanEntry.dNSName = "insidesecure.fr";
    sanEntry.dNSNameLen = Strlen(sanEntry.dNSName);
    sanEntry.uniformResourceIdentifier = NULL;
    rc = psX509AddSubjectAltName(pool,
        conf,
        &sanEntry);
    if (rc < 0)
    {
        Printf("psX509AddSubjectAltName failed\n");
        return PS_FAILURE;
    }

    return PS_SUCCESS;
}

/* Test case 1: create maximal CSR. */
int32 createMaximalCSR(psPool_t *pool,
    psPubKey_t *subjectKeyPair,
    psPubKey_t *subjectKeyPair2,
    const char *out_filename)
{
    psCertConfig_t conf;
    unsigned char *request;
    unsigned char subjectKeyId[] = { 0xff, 0xab, 0xcd, 0xef };
    int32 request_len, rc;

    Memset(&conf, 0, sizeof(psCertConfig_t));
    conf.pool = pool;

    rc = psX509SetValidDays(pool,
        &conf,
        30);
    if (rc < 0)
    {
        Printf("psX509SetValidDays failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetCertHashAlg(pool,
        &conf,
        ALG_SHA256);
    if (rc < 0)
    {
        Printf("psX509SetCertHashAlg failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetSubjectKeyId(pool,
        &conf,
        subjectKeyId,
        sizeof(subjectKeyId));
    if (rc < 0)
    {
        Printf("psX509SetSubjectKeyId failed\n");
        return PS_FAILURE;
    }

    if (subjectKeyPair2)
    {
        rc = psX509SetPublicKey(pool,
            &conf,
            subjectKeyPair2);
        if (rc < 0)
        {
            Printf("psX509SetPublicKey failed\n");
            return PS_FAILURE;
        }
    }

    rc = writeMaximalDNToConfig(pool, &conf);
    if (rc < 0)
    {
        return rc;
    }

    rc = writeSubjectAltNameToConfig(pool, &conf);
    if (rc < 0)
    {
        return rc;
    }

    /* Add attribute. */
    if (psX509SetChallengePassword(pool, &conf, "foot", 4,
                                   PS_STRING_UTF8_STRING))
    {
        Printf("psX509SetChallengePassword failed.\n");
        return PS_FAILURE;
    }

    rc = psWriteCertReqMem(pool,
        subjectKeyPair,
        &conf,
        &request,
        &request_len);
    if (rc < 0)
    {
        Printf("psWriteCertReqMem failed\n");
        return PS_FAILURE;
    }

    Printf("Produced a CSR of length %d\n",
        request_len);

    rc = psBase64EncodeAndWrite(pool, out_filename,
        request, request_len,
        REQ_FILE_TYPE, NULL, 0);
    if (rc < 0)
    {
        Printf("psBase64EncodeAndWrite failed\n");
        return PS_FAILURE;
    }

    Printf("Wrote: %s\n", out_filename);

    psX509FreeCertConfig(&conf);
    psFree(request, pool);

    return PS_SUCCESS;
}

static int32 make_csr_with_sig_alg(psPool_t *pool,
        psPubKey_t *keyPair,
        int32_t hashAlg,
        const char *out_filename)
{
    psCertConfig_t conf;
    unsigned char serialNum[] = { 1, 2, 3, 4, 5, 6, 7 };
    int32 rc;
    unsigned char *request;
    int32 request_len;

    Memset(&conf, 0, sizeof(psCertConfig_t));
    conf.pool = pool;

    rc = psX509SetValidDays(pool,
        &conf,
        365);
    if (rc < 0)
    {
        Printf("psX509SetValidDays failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetCertHashAlg(pool,
        &conf,
        hashAlg);
    if (rc < 0)
    {
        Printf("psX509SetCertHashAlg failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetSerialNum(pool,
        &conf,
        serialNum,
        sizeof(serialNum));
    if (rc < 0)
    {
        Printf("psX509SetSerialNum failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        &conf,
        "commonName",
        Strlen("commonName"),
        "insidesecure.com",
        Strlen("insidesecure.com"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    rc = psWriteCertReqMem(pool,
        keyPair,
        &conf,
        &request,
        &request_len);
    if (rc < 0)
    {
        Printf("psWriteCertReqMem failed\n");
        return PS_FAILURE;
    }

    Printf("Produced a CSR of length %d\n",
        request_len);

    rc = psBase64EncodeAndWrite(pool, out_filename,
        request, request_len,
        REQ_FILE_TYPE, NULL, 0);
    if (rc < 0)
    {
        Printf("psBase64EncodeAndWrite failed\n");
        return PS_FAILURE;
    }

    Printf("Wrote: %s\n", out_filename);

    psX509FreeCertConfig(&conf);
    psFree(request, pool);

    return PS_SUCCESS;
}

/* Test case 2: create self-signed certificate. */
int32 createSelfSignedCert(psPool_t *pool,
        psPubKey_t *keyPair,
        const char *out_filename,
        int use_old_asn1_string_types,
        usage_type usage,
        x509_cert_version version)
{
    psCertConfig_t conf;
    unsigned char serialNum[] = { 1, 2, 3, 4, 5, 6, 7 };
    int32 rc;
    authorityInfoAccessEntry_t authInfo;
    certificatePoliciesEntry_t pols;
    const char *notBeforeUtc = "17" "12" "18" "17" "00" "00" "00" "Z";
    const char *notAfterGeneralized = PS_GENERALIZEDTIME_INDEFINITE;

    Memset(&conf, 0, sizeof(psCertConfig_t));
    conf.pool = pool;

    rc = psX509SetValidNotBefore(pool,
            &conf,
            notBeforeUtc,
            strlen(notBeforeUtc),
            ASN_UTCTIME);
    if (rc < 0)
    {
        Printf("psX509SetValidNotBefore failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetValidNotAfter(pool,
            &conf,
            notAfterGeneralized,
            strlen(notAfterGeneralized),
            ASN_GENERALIZEDTIME);
    if (rc < 0)
    {
        printf("psX509SetValidNotAfter failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetCertHashAlg(pool,
        &conf,
        ALG_SHA256);
    if (rc < 0)
    {
        Printf("psX509SetCertHashAlg failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetSerialNum(pool,
        &conf,
        serialNum,
        sizeof(serialNum));
    if (rc < 0)
    {
        Printf("psX509SetSerialNum failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetVersion(pool,
            &conf,
            version);
    if (rc < 0)
    {
        Printf("psX509SetVersion failed\n");
        return PS_FAILURE;
    }

    if (use_old_asn1_string_types)
    {
        rc = writeOldAsn1StringTypeDNToConfig(pool, &conf);
    }
    else
    {
        rc = writeMaximalDNToConfig(pool, &conf);
    }

    if (rc < 0)
    {
        return rc;
    }

    if (version != x509_cert_v3)
    {
        goto skip_extensions;
    }

    rc = psX509SetBasicConstraintsCA(pool,
        &conf,
        1);
    if (rc < 0)
    {
        Printf("psX509SetBasicConstraintsCA\n");
        return PS_FAILURE;
    }

    rc = psX509SetBasicConstraintsPathLen(pool,
        &conf,
        5);
    if (rc < 0)
    {
        Printf("psX509SetBasicConstraintsPathLen\n");
        return PS_FAILURE;
    }

    rc = writeSubjectAltNameToConfig(pool, &conf);
    if (rc < 0)
    {
        return rc;
    }

    if (usage == KEY_USAGE_DEFAULT)
    {
        rc = psX509AddKeyUsageBit(pool,
            &conf,
            "keyEncipherment");
        if (rc < 0)
        {
            Printf("addKeyUsageBit failed\n");
            return PS_FAILURE;
        }

        rc = psX509AddKeyUsageBit(pool,
            &conf,
            "keyCertSign");
        if (rc < 0)
        {
            Printf("addKeyUsageBit failed\n");
            return PS_FAILURE;
        }

        rc = psX509AddExtendedKeyUsage(pool,
            &conf,
            "serverAuth");
        if (rc < 0)
        {
            Printf("addKeyUsageBit failed\n");
            return PS_FAILURE;
        }
    }
    else
    {
        rc = psX509AddKeyUsageBit(pool,
            &conf,
            "keyAgreement");
        if (rc < 0)
        {
            Printf("addKeyUsageBit failed\n");
            return PS_FAILURE;
        }

        if (usage == KEY_USAGE_ENCIPHER)
        {
            rc = psX509AddKeyUsageBit(pool,
                &conf,
                "encipherOnly");
        }
        else if (usage == KEY_USAGE_DECIPHER)
        {
            rc = psX509AddKeyUsageBit(pool,
                &conf,
                "decipherOnly");
        }
        else
        {
            Printf("Unknown key usage\n");
            return PS_FAILURE;
        }

        if (rc < 0)
        {
            Printf("addKeyUsageBit failed\n");
            return PS_FAILURE;
        }
    }

    Memset(&authInfo, 0, sizeof(authorityInfoAccessEntry_t));
    authInfo.ocsp = "ocsp.insidesecure.com";
    authInfo.ocspLen = Strlen("ocsp.insidesecure.com");
    authInfo.caIssuers = "ca.insidesecure.com";
    authInfo.caIssuersLen = Strlen("ca.insidesecure.com");

    rc = psX509AddAuthorityInfoAccess(pool,
        &conf,
        &authInfo);
    if (rc < 0)
    {
        Printf("psX509AddAuthorityInfoAccess failed\n");
        return PS_FAILURE;
    }

    Memset(&authInfo, 0, sizeof(authorityInfoAccessEntry_t));
    authInfo.ocsp = "ocsp2.insidesecure.com";
    authInfo.ocspLen = Strlen("ocsp2.insidesecure.com");
    authInfo.caIssuers = "ca2.insidesecure.com";
    authInfo.caIssuersLen = Strlen("ca2.insidesecure.com");

    rc = psX509AddAuthorityInfoAccess(pool,
        &conf,
        &authInfo);
    if (rc < 0)
    {
        Printf("psX509AddAuthorityInfoAccess failed\n");
        return PS_FAILURE;
    }

    /* Add a policy with OID. */
    Memset(&pols, 0, sizeof(certificatePoliciesEntry_t));
    pols.policyIndex = 1;
    pols.policyOid = "67810C010201";
    pols.policyOidLen = Strlen(pols.policyOid);

    /* Add CPS to policy #1 */
    pols.cps = "http://www.insidesecure.com/cps1";
    pols.cpsLen = Strlen(pols.cps);

    /* Add UserNotice to policy #1 */
    pols.unoticeIndex = 1;
    pols.unoticeOrganization = "INSIDE Secure Oyj 1";
    pols.unoticeOrganizationLen = Strlen(pols.unoticeOrganization);
    pols.unoticeExplicitText = "Explicit Text 1";
    pols.unoticeExplicitTextLen = Strlen(pols.unoticeExplicitText);
    pols.unoticeNumbers[0] = 1;
    pols.unoticeNumbers[1] = 2;
    pols.unoticeNumbersLen = 2;

    rc = psX509AddPolicy(pool,
        &conf,
        &pols);
    if (rc < 0)
    {
        Printf("psX509AddPolicy failed\n");
        return PS_FAILURE;
    }

    /* Add another UserNotice to policy #1. */
    Memset(&pols, 0, sizeof(certificatePoliciesEntry_t));
    pols.policyIndex = 1;
    pols.unoticeIndex = 2;
    pols.unoticeOrganization = "INSIDE Secure Oyj 2";
    pols.unoticeOrganizationLen = Strlen(pols.unoticeOrganization);
    pols.unoticeExplicitText = "Explicit Text 2";
    pols.unoticeExplicitTextLen = Strlen(pols.unoticeExplicitText);
    pols.unoticeNumbers[0] = 3;
    pols.unoticeNumbers[1] = 4;
    pols.unoticeNumbersLen = 2;

    rc = psX509AddPolicy(pool,
        &conf,
        &pols);
    if (rc < 0)
    {
        Printf("psX509AddPolicy failed\n");
        return PS_FAILURE;
    }

    /* Now add policy #2 with a different OID. */
    Memset(&pols, 0, sizeof(certificatePoliciesEntry_t));
    pols.policyIndex = 2;
    pols.policyOid = "67810C010202";
    pols.policyOidLen = Strlen(pols.policyOid);

    /* Add CPS to policy #2 */
    pols.cps = "http://www.insidesecure.com/cps2";
    pols.cpsLen = Strlen(pols.cps);

    rc = psX509AddPolicy(pool,
        &conf,
        &pols);
    if (rc < 0)
    {
        Printf("psX509AddPolicy failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetConstraintRequireExplicitPolicy(pool,
        &conf,
        9);
    if (rc < 0)
    {
        Printf("psX509SetConstraintsRequireExplicitPolicy failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetConstraintInhibitPolicyMappings(pool,
        &conf,
        5);
    if (rc < 0)
    {
        Printf("psX509SetConstraintsInhibitPolicyMappings failed\n");
        return PS_FAILURE;
    }

    rc = psX509AddPolicyMapping(pool,
        &conf,
        "67810C010201",
        Strlen("67810C010201"),
        "67810C010202",
        Strlen("67810C010202"));
    if (rc < 0)
    {
        Printf("psX509AddPolicyMapping failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetNetscapeComment(pool,
        &conf,
        "My Netscape Comment",
        Strlen("My Netscape Comment"));
    if (rc < 0)
    {
        Printf("psX509SetNetscapeComment failed\n");
        return PS_FAILURE;
    }

skip_extensions:

    rc = psX509WriteSelfSignedCert(pool,
        &conf,
        keyPair,
        (char *) out_filename);
    if (rc < 0)
    {
        Printf("psX509WriteSelfSignedCertMem\n");
        return PS_FAILURE;
    }

    Printf("Wrote: %s\n", out_filename);

    psX509FreeCertConfig(&conf);

#  ifdef USE_OPENSSL_CMD
    {
        char *cmd;
        int system_rc;
        const char *lookForVersion;

        /* Check version. */
        if (version == x509_cert_v1)
        {
            lookForVersion = "1";
        }
        else if (version == x509_cert_v2)
        {
            lookForVersion = "2";
        }
        else if (version == x509_cert_v3)
        {
            lookForVersion = "3";
        }
        my_asprintf(&cmd, "openssl x509 -text -noout -in %s"   \
                " |grep -q 'Version: %s'",
                out_filename,
                lookForVersion);
        system_rc = system(cmd);
        if (system_rc != 0)
        {
            printf("version check with openssl x509 failed\n");
            free(cmd);
            return PS_FAILURE;
        }
        free(cmd);

# ifdef USE_EXTRA_DN_ATTRIBUTES_RFC5280_SHOULD
        /* Note: writeMaximalDN writes these known answers.
           Remember to update the checks if that function changes! */
        if (!use_old_asn1_string_types)
        {
            /* Check locality. Semicolons should work in DN attribs. */
            my_asprintf(&cmd, "openssl x509 -text -noout -in %s"   \
                    " |grep -qE '%s'",
                    out_filename,
                    /* Remember to update when changing the known answer: */
                    /* Note: OpenSSL 1.1.1 uses L = str, while earlier
                       versions used L=str. Use regex to allow both. */
                    "L.*=.*Helsinki;Kamppi");
            system_rc = system(cmd);
            if (system_rc != 0)
            {
                printf("Locality DN attrib check with openssl x509 failed\n");
                free(cmd);
                return PS_FAILURE;
            }
            free(cmd);
        }
# endif

        /* Check notBefore date. */
        my_asprintf(&cmd, "openssl x509 -text -noout -in %s"   \
                " |grep -q '%s'",
                out_filename,
                /* Remember to update when changing the known answer: */
                "Not Before: Dec 18 17:00:00 2017 GMT");
        system_rc = system(cmd);
        if (system_rc != 0)
        {
            printf("notBefore date check with openssl x509 failed\n");
            free(cmd);
            return PS_FAILURE;
        }
        free(cmd);

        /* Check notAfter date. */
        my_asprintf(&cmd, "openssl x509 -text -noout -in %s"   \
                " |grep -q '%s'",
                out_filename,
                /* Remember to update when changing the known answer: */
                "Not After : Dec 31 23:59:59 9999 GMT");
        system_rc = system(cmd);
        if (system_rc != 0)
        {
            printf("notAfter date check with openssl x509 failed\n");
            free(cmd);
            return PS_FAILURE;
        }
        free(cmd);

        if (version != x509_cert_v3)
        {
            goto skip_extension_checks;
        }

        /* Check keyUsage. */
        if (usage == KEY_USAGE_DECIPHER)
        {
            my_asprintf(&cmd, "openssl x509 -text -noout -in %s"    \
                    " |grep -A1 'X509v3 Key Usage'" \
                    " |grep -q '%s'",
                    out_filename,
                    "Decipher Only");
        }
        else if (usage == KEY_USAGE_ENCIPHER)
        {
            my_asprintf(&cmd, "openssl x509 -text -noout -in %s"    \
                    " |grep -A1 'X509v3 Key Usage'" \
                    " |grep -q '%s'",
                    out_filename,
                    "Encipher Only");
        }
        else
        {
            my_asprintf(&cmd, "openssl x509 -text -noout -in %s"    \
                    " |grep -A1 'X509v3 Key Usage'" \
                    " |grep -q '%s'",
                    out_filename,
                    "Key Encipherment");
        }
        system_rc = system(cmd);
        if (system_rc != 0)
        {
            printf("keyUsage check with openssl x509 failed\n");
            free(cmd);
            return PS_FAILURE;
        }
        free(cmd);
    }
#  endif /* USE_OPENSSL_CMD */

# ifdef USE_OPENSSL_CMD
skip_extension_checks:
# endif

    return PS_SUCCESS;
}

int32_t check_dn_attrib(x509DNattributes_t *dn,
        x509DNAttributeType_t type,
        int32_t nth_occurrence,
        const char *expectedValue)
{
    x509DNAttributeType_t attrType;
    short valueType;
    psSize_t valueLen;
    char *value;
    int32_t rc;

    rc = psX509GetDNAttributeIndex(dn,
            type,
            nth_occurrence);
    if (rc < 0)
    {
        return PS_FAILURE;
    }
    else
    {
        rc = psX509GetDNAttributeTypeAndValue(dn,
                rc,
                &attrType,
                &valueType,
                &valueLen,
                &value);
        if (rc < 0)
        {
            return PS_FAILURE;
        }
        if (attrType != type)
        {
            return PS_FAILURE;
        }
        if ((valueLen - DN_NUM_TERMINATING_NULLS) != Strlen(expectedValue))
        {
            return PS_FAILURE;
        }
        if (Strncmp(value, expectedValue, valueLen) != 0)
        {
            return PS_FAILURE;
        }
    }

    return PS_SUCCESS;
}

/* Test case 3: create CA-signed certificate. */
int32 createCASignedCert(psPool_t *pool,
        psPubKey_t *caKeyPair,
        const char *csr_filename,
        const char *ca_cert_filename,
        const char *out_filename,
        int32 certHashAlg,
        int32 do_csr_kat,
        int32 no_extensions_from_request,
        int32 no_extensions_to_config)
{
    int32 rc;
    unsigned char *csr;
    int32 csr_len;
    x509DNattributes_t *subjectDN = NULL;
    psPubKey_t *reqPubKey;
    x509v3extensions_t *reqExt;
    psX509Cert_t *ca_cert;
    psCertConfig_t conf;
    certificatePoliciesEntry_t pols;
    authorityInfoAccessEntry_t authInfo;
    subjectAltNameEntry_t ianEntry;
    unsigned char serialNum[] = { 0x01, 0x02, 0x03 };
    psCertReq_t *parsedReq;
    char *ianEmailStr = "ca@insidesecure.com"; /* For openssl KAT. */
    psX509Cert_t *certOut;

    Memset(&conf, 0, sizeof(psCertConfig_t));
    conf.pool = pool;

    rc = psParseCertReqFile(pool,
        csr_filename,
        &csr,
        &csr_len);
    if (rc < 0)
    {
        Printf("psParseCertReqFile failed\n");
        return PS_FAILURE;
    }

    rc = psParseCertReqBufExt(pool,
        csr,
        csr_len,
        &subjectDN,
        &reqPubKey,
        &reqExt,
        &parsedReq);
    if (rc < 0)
    {
        Printf("psParseCertReqBufExt failed\n");
        return PS_FAILURE;
    }

    if (expect_challengepassword)
    {
        /* Challenge password is expected. */
        if (psCertReqNumChallengePassword(parsedReq) == 1)
        {
            psStringType_t type;
            psSize_t len;
            const unsigned char *pwd =
                psCertReqGetChallengePassword(parsedReq, 0);
            len = psCertReqGetChallengePasswordLen(parsedReq, 0);
            type = psCertReqGetChallengePasswordType(parsedReq, 0);

            if (type != PS_STRING_UTF8_STRING ||
                len != 4 ||
                Strncmp((const char *)pwd, "foot", 4) == 0)
            {
                Printf("challenge password mismatch.\n");
                Abort();
            }

            /* handled challenge password. */
            expect_challengepassword --;
        }
    }

    /*
       Now check some properties of the parsed CSR.
     */
    if (do_csr_kat)
    {
        if (psCertReqGetSignatureAlgorithm(parsedReq) != OID_SHA256_RSA_SIG)
        {
            Printf("parsedReq has incorrect sigAlg: %d\n",
                    psCertReqGetSignatureAlgorithm(parsedReq));
            return PS_FAILURE;
        }
        if (psCertReqGetPubKeyAlgorithm(parsedReq) != OID_RSA_KEY_ALG)
        {
            Printf("parsedReq has incorrect pubKeyAlg: %d\n",
                    psCertReqGetPubKeyAlgorithm(parsedReq));
            return PS_FAILURE;
        }
        if (psCertReqGetVersion(parsedReq) != 0)
        {
            Printf("parsedReq has incorrect version: %d\n",
                    psCertReqGetVersion(parsedReq));
            return PS_FAILURE;
        }
    }

    /*
       parsedReq was allocated in psParseCertReqBufExt. Caller must free.
     */
    psFree(parsedReq, pool);

    rc = psX509ParseCertFile(pool,
        (char *) ca_cert_filename,
        &ca_cert,
        CERT_STORE_DN_BUFFER);
    if (rc < 0)
    {
        Printf("psX509ParseCertFile failed\n");
        return PS_FAILURE;
    }

    /*
       Now we have the data in the CSR, the subject DN, the CA cert
       and the CA privKey. Still need some fields the CA wants to set,
       such as validDays, hashAlg, etc. Also we need some extensions
       that the CA wants to set. These are provided via a
       psCertConfig_t, which we will create now.
     */

    rc = psX509SetValidDays(pool,
        &conf,
        365);
    if (rc < 0)
    {
        Printf("psX509SetValidDays failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetCertHashAlg(pool,
        &conf,
        certHashAlg);
    if (rc < 0)
    {
        Printf("psX509SetCertHashAlg failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetSerialNum(pool,
        &conf,
        serialNum,
        sizeof(serialNum));
    if (rc < 0)
    {
        Printf("psX509SetSerialNum failed\n");
        return PS_FAILURE;
    }

    if (no_extensions_to_config)
    {
        /* Don't add any extensions to the CA config. */
        goto skip_add_extensions;
    }

    Memset(&ianEntry, 0, sizeof(subjectAltNameEntry_t));
    ianEntry.rfc822Name = ianEmailStr;
    ianEntry.rfc822NameLen = Strlen(ianEntry.rfc822Name);
    ianEntry.uniformResourceIdentifier = "http://www.insidesecure.com";
    ianEntry.uniformResourceIdentifierLen
        = Strlen(ianEntry.uniformResourceIdentifier);
    ianEntry.iPAddress = "1.2.3.4";
    ianEntry.iPAddressLen = Strlen(ianEntry.iPAddress);

    rc = psX509AddIssuerAltName(pool,
        &conf,
        &ianEntry);
    if (rc < 0)
    {
        Printf("psX509AddIssuerAltName failed\n");
        return PS_FAILURE;
    }

    Memset(&authInfo, 0, sizeof(authorityInfoAccessEntry_t));
    authInfo.ocsp = "ocsp.insidesecure.com";
    authInfo.ocspLen = Strlen("ocsp.insidesecure.com");
    authInfo.caIssuers = "ca.insidesecure.com";
    authInfo.caIssuersLen = Strlen("ca.insidesecure.com");

    rc = psX509AddAuthorityInfoAccess(pool,
        &conf,
        &authInfo);
    if (rc < 0)
    {
        Printf("psX509AddAuthorityInfoAccess failed\n");
        return PS_FAILURE;
    }

    Memset(&authInfo, 0, sizeof(authorityInfoAccessEntry_t));
    authInfo.ocsp = "ocsp2.insidesecure.com";
    authInfo.ocspLen = Strlen("ocsp2.insidesecure.com");
    authInfo.caIssuers = "ca2.insidesecure.com";
    authInfo.caIssuersLen = Strlen("ca2.insidesecure.com");

    rc = psX509AddAuthorityInfoAccess(pool,
        &conf,
        &authInfo);
    if (rc < 0)
    {
        Printf("psX509AddAuthorityInfoAccess failed\n");
        return PS_FAILURE;
    }

    /* Add a policy, a CPS and a UserNotice. */
    Memset(&pols, 0, sizeof(certificatePoliciesEntry_t));
    pols.policyIndex = 1;
    pols.policyOid = "67810C010201";
    pols.policyOidLen = Strlen(pols.policyOid);
    pols.cps = "http://www.insidesecure.com/cps1";
    pols.cpsLen = Strlen(pols.cps);
    pols.unoticeIndex = 1;
    pols.unoticeOrganization = "INSIDE Secure Oyj 1";
    pols.unoticeOrganizationLen = Strlen(pols.unoticeOrganization);
    pols.unoticeExplicitText = "Explicit Text 1";
    pols.unoticeExplicitTextLen = Strlen(pols.unoticeExplicitText);
    pols.unoticeNumbers[0] = 1;
    pols.unoticeNumbers[1] = 2;
    pols.unoticeNumbersLen = 2;

    rc = psX509AddPolicy(pool,
        &conf,
        &pols);
    if (rc < 0)
    {
        Printf("psX509AddPolicy failed\n");
        return PS_FAILURE;
    }

    /* Add another UserNotice to the same extension. */
    Memset(&pols, 0, sizeof(certificatePoliciesEntry_t));
    pols.policyIndex = 1;
    pols.unoticeIndex = 2;
    pols.unoticeOrganization = "INSIDE Secure Oyj 2";
    pols.unoticeOrganizationLen = Strlen(pols.unoticeOrganization);
    pols.unoticeExplicitText = "Explicit Text 2";
    pols.unoticeExplicitTextLen = Strlen(pols.unoticeExplicitText);
    pols.unoticeNumbers[0] = 7;
    pols.unoticeNumbers[1] = 8;
    pols.unoticeNumbers[2] = 9;
    pols.unoticeNumbersLen = 3;

    pols.unoticeNumbersLen = 2;

    rc = psX509AddPolicy(pool,
        &conf,
        &pols);
    if (rc < 0)
    {
        Printf("psX509AddPolicy failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetConstraintRequireExplicitPolicy(pool,
        &conf,
        9);
    if (rc < 0)
    {
        Printf("psX509SetConstraintsRequireExplicitPolicy failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetConstraintInhibitPolicyMappings(pool,
        &conf,
        5);
    if (rc < 0)
    {
        Printf("psX509SetConstraintsInhibitPolicyMappings failed\n");
        return PS_FAILURE;
    }

    rc = psX509AddPolicyMapping(pool,
        &conf,
        "67810C010201",
        Strlen("67810C010201"),
        "67810C010202",
        Strlen("67810C010202"));
    if (rc < 0)
    {
        Printf("psX509AddPolicyMapping failed\n");
        return PS_FAILURE;
    }

skip_add_extensions:
    if (no_extensions_from_request)
    {
        /* Ignore the extensions from the CSR. AuthorityKeyIdentifier
           extension is something we will always add even if the
           extension struct arg to psX509SetCAIssuedCertExtensions
           is NULL. */
        x509FreeExtensions(reqExt);
        psFree(reqExt, NULL);
        reqExt = NULL;
    }

    rc = psX509SetCAIssuedCertExtensions(pool,
        &conf,
        reqExt,
        ca_cert);
    if (rc < 0)
    {
        Printf("psX509SetCAIssuedCertExtensions failed\n");
        return PS_FAILURE;
    }

    /* Write to file. */
    rc = psX509WriteCAIssuedCert(pool,
        &conf,
        reqPubKey,
        subjectDN->dnenc,
        subjectDN->dnencLen,
        ca_cert,
        caKeyPair,
        (char *) out_filename);
    if (rc < 0)
    {
        Printf("psX509WriteCAIssuedCert failed\n");
        return PS_FAILURE;
    }
    Printf("Wrote: %s\n", out_filename);

    /* KAT with MatrixSSL. */
    rc = psX509ParseCertFile(NULL,
            out_filename,
            &certOut,
            CERT_STORE_UNPARSED_BUFFER);
    if (rc < 0)
    {
        Printf("psX509ParseCert failed\n");
        return PS_FAILURE;
    }
    /* Check that the DN looks as expected. */
    rc = check_dn_attrib(&certOut->subject,
            ATTRIB_COMMON_NAME,
            1,
            "insidesecure.com");
    /* Following if statement is needed because one of the DNs tested
       here does not have any DCs. */
    if (psX509GetNumDomainComponents(&certOut->subject) > 0)
    {
        rc |= check_dn_attrib(&certOut->subject,
                ATTRIB_DOMAIN_COMPONENT,
                1,
                "com");
        rc |= check_dn_attrib(&certOut->subject,
                ATTRIB_DOMAIN_COMPONENT,
                2,
                "insidesecure");
        rc |= check_dn_attrib(&certOut->subject,
                ATTRIB_DOMAIN_COMPONENT,
                3,
                "test");
    }
    if (rc != PS_SUCCESS)
    {
        Printf("DN attrib KAT failed\n");
        return PS_FAILURE;
    }
    psX509FreeCert(certOut);

#ifdef USE_OPENSSL_CMD
    /* KAT with OpenSSL command line tool. */
    /* If the CA config had an issuerAltName, check that it gets
       encoded correctly. */
    if (!no_extensions_to_config)
    {
        char *cmd;
        int system_rc;

        my_asprintf(&cmd, "openssl x509 -text -noout -in %s"   \
                " |grep -A2 'X509v3 Issuer Alternative Name:'" \
                " |grep -q '%s'",
                out_filename,
                ianEmailStr);
        system_rc = system(cmd);
        if (system_rc != 0)
        {
            Printf("issuerAltName check with openssl x509 failed\n");
            Free(cmd);
            return PS_FAILURE;
        }
        Free(cmd);
    }
    /* If the CA config or the CSR had basicConstraints, check that
       it gets encoded with the correct value for the CA boolean.
       If neither had basicConstraints, check that we don't encode
       it at all. */
    {
        char *cmd;
        int system_rc;
        int expectBasicConstraints = 1;
        const char *look_str;

        if ((reqExt && reqExt->bc.cA == CA_TRUE) ||
                (conf.ext && conf.ext->bc.cA == CA_TRUE))
        {
            look_str = "CA:TRUE";
        }
        else if ((reqExt && reqExt->bc.cA == CA_FALSE) ||
                (conf.ext && conf.ext->bc.cA == CA_FALSE))
        {
            look_str = "CA:FALSE";
        }
        else
        {
            /* Either extensions are missing completely or basicConstraints
               is CA_UNDEFINED. Grepping for Basic Constraints should fail. */
            expectBasicConstraints = 0;
        }
        if (expectBasicConstraints)
        {
            my_asprintf(&cmd, "openssl x509 -text -noout -in %s"      \
                    " |grep -A2 'X509v3 Basic Constraints: critical'" \
                    " |grep -q '%s'",
                    out_filename,
                    look_str);
        }
        else
        {
            my_asprintf(&cmd, "openssl x509 -text -noout -in %s"        \
                    " |grep -q -A2 'X509v3 Basic Constraints: critical'",
                    out_filename);
        }
        system_rc = system(cmd);
        if ((expectBasicConstraints && (system_rc != 0)) ||
                (!expectBasicConstraints && (system_rc == 0)))
        {
            Printf("basicConstraints CA bit check with openssl x509 failed\n");
            Free(cmd);
            return PS_FAILURE;
        }
        Free(cmd);
    }
#endif /* USE_OPENSSL_CMD */

    psX509FreeCert(ca_cert);
    psX509FreeCertConfig(&conf);
    x509FreeExtensions(reqExt);
    psFree(reqExt, pool);
    psX509FreeDNStruct(subjectDN, pool);
    psFree(subjectDN, pool);
    psDeletePubKey(&reqPubKey);
    psFree(csr, pool);

    return PS_SUCCESS;
}

/* Test case 4: Generate CSRs using various signature algorithms. */
int32 combinatorialGenerate(psPool_t *pool,
        psPubKey_t *rsa_keys,
        psPubKey_t *ec_keys,
        psPubKey_t *ca_keys,
        const char *ca_cert_filename)
{
    int32 rc;

    /**/

#ifdef ENABLE_SHA1_SIGNED_CERTS
    rc = make_csr_with_sig_alg(pool, rsa_keys, ALG_SHA1, "rsa_sha1.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg RSA-SHA1 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "rsa_sha1.req",
            ca_cert_filename,
            "rsa_sha1.req.cer",
            ALG_SHA1, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

#ifdef USE_ECC
    rc = make_csr_with_sig_alg(pool, ec_keys, ALG_SHA1, "ec_sha1.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg EC-SHA1 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "ec_sha1.req",
            ca_cert_filename,
            "ec_sha1.req.cer",
            ALG_SHA1, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#endif /* USE_ECC */
#endif /* ENABLE_SHA1_SIGNED_CERTS */

#ifdef USE_SHA224
    rc = make_csr_with_sig_alg(pool, rsa_keys, ALG_SHA224, "rsa_sha224.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg RSA-SHA224 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "rsa_sha224.req",
            ca_cert_filename,
            "rsa_sha224.req.cer",
            ALG_SHA224, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#ifdef USE_ECC
    rc = make_csr_with_sig_alg(pool, ec_keys, ALG_SHA224, "ec_sha224.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg EC-SHA224 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "ec_sha224.req",
            ca_cert_filename,
            "ec_sha224.req.cer",
            ALG_SHA224, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#endif /* USE_ECC */
#endif /* USE_SHA224 */

    rc = make_csr_with_sig_alg(pool, rsa_keys, ALG_SHA256, "rsa_sha256.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg RSA-SHA256 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "rsa_sha256.req",
            ca_cert_filename,
            "rsa_sha256.req.cer",
            ALG_SHA256, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#ifdef USE_ECC
    rc = make_csr_with_sig_alg(pool, ec_keys, ALG_SHA256, "ec_sha256.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg EC-SHA256 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "ec_sha256.req",
            ca_cert_filename,
            "ec_sha256.req.cer",
            ALG_SHA256, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#endif /* USE_ECC */

    rc = make_csr_with_sig_alg(pool, rsa_keys, ALG_SHA384, "rsa_sha384.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg RSA-SHA384 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "rsa_sha384.req",
            ca_cert_filename,
            "rsa_sha384.req.cer",
            ALG_SHA384, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#ifdef USE_ECC
    rc = make_csr_with_sig_alg(pool, ec_keys, ALG_SHA384, "ec_sha384.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg EC-SHA384 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "ec_sha384.req",
            ca_cert_filename,
            "ec_sha384.req.cer",
            ALG_SHA384, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#endif /* USE_ECC */

    rc = make_csr_with_sig_alg(pool, rsa_keys, ALG_SHA512, "rsa_sha512.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg RSA-SHA512 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "rsa_sha512.req",
            ca_cert_filename,
            "rsa_sha512.req.cer",
            ALG_SHA512, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

#ifdef USE_ECC
    rc = make_csr_with_sig_alg(pool, ec_keys, ALG_SHA512, "ec_sha512.req");
    if (rc < 0)
    {
        Printf("make_csr_with_sig_alg EC-SHA512 failed\n");
        return PS_FAILURE;
    }
    rc = createCASignedCert(pool,
            ca_keys,
            "ec_sha512.req",
            ca_cert_filename,
            "ec_sha512.req.cer",
            ALG_SHA512, 0, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#endif /* USE_ECC */

    return PS_SUCCESS;
}

/* Test case 5: write minimal CSR. The only attribute is the
   challengePassword. */
int32 createMinimalCSR(psPool_t *pool,
    psPubKey_t *subjectKeyPair,
    psPubKey_t *subjectKeyPair2,
    const char *out_filename)
{
    psCertConfig_t conf;
    unsigned char *request;
    int32 request_len, rc;

    Memset(&conf, 0, sizeof(psCertConfig_t));
    conf.pool = pool;

    rc = psX509SetValidDays(pool,
        &conf,
        30);
    if (rc < 0)
    {
        Printf("psX509SetValidDays failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetCertHashAlg(pool,
        &conf,
        ALG_SHA256);
    if (rc < 0)
    {
        Printf("psX509SetCertHashAlg failed\n");
        return PS_FAILURE;
    }

    rc = psX509SetDNAttribute(pool,
        &conf,
        "commonName",
        Strlen("commonName"),
        "Minimal CSR",
        Strlen("Minimal CSR"),
        ASN_UTF8STRING);
    if (rc < 0)
    {
        Printf("psX509SetDNAttribute failed\n");
        return PS_FAILURE;
    }

    /* Add attribute. */
    if (psX509SetChallengePassword(pool, &conf, "foot", 4,
                                   PS_STRING_UTF8_STRING))
    {
        Printf("psX509SetChallengePassword failed.\n");
        return PS_FAILURE;
    }

    rc = psWriteCertReqMem(pool,
        subjectKeyPair,
        &conf,
        &request,
        &request_len);
    if (rc < 0)
    {
        Printf("psWriteCertReqMem failed\n");
        return PS_FAILURE;
    }

    Printf("Produced a CSR of length %d\n",
        request_len);

    rc = psBase64EncodeAndWrite(pool, out_filename,
        request, request_len,
        REQ_FILE_TYPE, NULL, 0);
    if (rc < 0)
    {
        Printf("psBase64EncodeAndWrite failed\n");
        return PS_FAILURE;
    }

    Printf("Wrote: %s\n", out_filename);

    psX509FreeCertConfig(&conf);
    psFree(request, pool);

    return PS_SUCCESS;
}

/* Check whether we are in the root directory of MatrixSSL.
   Used to determine paths of default certificates and keys. */
static psBool_t cwdIsMatrixRoot(void)
{
    psRes_t rc;
    unsigned char *buf;
    psSizeL_t len;

    rc = psGetFileBuf(NULL, "testkeys/RSA/2048_RSA_KEY.pem", &buf, &len);
    if (rc != PS_SUCCESS)
    {
        return PS_FALSE;
    }

    psFree(buf, NULL);
    return PS_TRUE;
}

static char* getFilepath(const char* filepath)
{
    static char filepathBuf[256];
    static const char *test_path1 = "crypto/test";

    /* Assume CWD is either matrix root or crypto/test. */
    if (cwdIsMatrixRoot())
    {
        snprintf(filepathBuf,
                sizeof(filepathBuf),
                "%s/%s",
                test_path1,
                filepath);
    }
    else
    {
        snprintf(filepathBuf,
                sizeof(filepathBuf),
                "%s",
                filepath);
    }

    return filepathBuf;
}

int32 testWriteCAIssuedCertMemAndFileIO(psPool_t *pool)
{
    int32_t rc = -1;
    int32_t keytype = 0;
    unsigned char *csr = NULL;
    int32 csr_len = 0;
    x509DNattributes_t *DN = NULL;
    psPubKey_t *reqPubKey = NULL;
    x509v3extensions_t *reqExt = NULL;
    psCertReq_t *parsedReq = NULL;
    psCertConfig_t certConfig;
    psX509Cert_t *caCert = NULL;
    psPubKey_t caPrivKey;
    unsigned char serialNum[] = { 0x12, 0x34 };
    unsigned char *certMemOut = NULL;
    int32_t certMemLenOut = 0;
    psX509Cert_t* cert_out = NULL;
    const char* testCertOutName = "testpsX509WriteCAIssuedCert.pem";

    Memset(&certConfig, 0, sizeof(certConfig));
    Memset(&caPrivKey, 0, sizeof(caPrivKey));

    /* Parse certificate signing request */
    rc = psParseCertReqFile(pool,
            getFilepath("test_x509enc_data/csr.pem"),
            &csr,
            &csr_len);
    if (rc != PS_SUCCESS)
    {
        return rc;
    }

    rc = psParseCertReqBufExt(pool,
            csr,
            csr_len,
            &DN,
            &reqPubKey,
            &reqExt,
            &parsedReq);
    if (rc != PS_SUCCESS)
    {
        return rc;
    }

    /* Set X509 signing options */
    rc = psX509SetValidDays(pool,
            &certConfig,
            365);
    if (rc != PS_SUCCESS)
    {
        return rc;
    }

    rc = psX509SetCertHashAlg(pool,
            &certConfig,
            ALG_SHA256);
    if (rc != PS_SUCCESS)
    {
        return rc;
    }

    rc = psX509SetSerialNum(pool,
            &certConfig,
            serialNum,
            sizeof(serialNum));
    if (rc != PS_SUCCESS)
    {
        return rc;
    }

    keytype = psParseUnknownPrivKey(pool,
            1,
            getFilepath("test_x509enc_data/ca_key.pem"),
            NULL,
            &caPrivKey);
    if (keytype < 0)
    {
        return PS_FAILURE;
    }

    rc = psX509ParseCertFile(pool,
            getFilepath("test_x509enc_data/ca_cert.pem"),
            &caCert,
            CERT_STORE_DN_BUFFER);
    if (rc < 1)
    {
        return PS_FAILURE;
    }

    /* Test writing to memory */
    rc = psX509WriteCAIssuedCertMem(pool,
            &certConfig,
            reqPubKey,
            DN->dnenc, /* subjectDN */
            DN->dnencLen, /* subjectDNLen */
            caCert,
            &caPrivKey,
            &certMemOut,
            &certMemLenOut);
    if (rc != PS_SUCCESS)
    {
        return rc;
    }

    rc = psX509ParseCert(pool,
            certMemOut,
            certMemLenOut,
            &cert_out,
            CERT_STORE_DN_BUFFER);
    if (rc < 1)
    {
        psX509FreeCert(cert_out);
        return PS_FAILURE;
    }
    psX509FreeCert(cert_out);
    cert_out = NULL;

    /* Test writing to file */
    rc = psX509WriteCAIssuedCert(pool,
            &certConfig,
            reqPubKey,
            DN->dnenc, /* subjectDN */
            DN->dnencLen, /* subjectDNLen */
            caCert,
            &caPrivKey,
            getFilepath(testCertOutName));
    if (rc != PS_SUCCESS)
    {
        return rc;
    }

    rc = psX509ParseCertFile(pool,
            getFilepath(testCertOutName),
            &cert_out,
            CERT_STORE_DN_BUFFER);
    if (rc < 1)
    {
        return PS_FAILURE;
    }

    psX509FreeCert(cert_out);
    cert_out = NULL;

    psFree(certMemOut, pool);
    psX509FreeCert(caCert);
    psClearPubKey(&caPrivKey);
    psX509FreeCertConfig(&certConfig);
    psFree(parsedReq, pool);
    x509FreeExtensions(reqExt);
    psFree(reqExt, pool);
    psDeletePubKey(&reqPubKey);
    psX509FreeDNStruct(DN, pool);
    psFree(DN, pool);
    psFree(csr, pool);

    return PS_SUCCESS;
}

int32 loadKeyPub(psPool_t *pool,
    psPubKey_t *keyPub,
    const char *key_filename)
{
    int32 keytype;

    keytype = psParseUnknownPubKey(pool,
        1,
        (char *) key_filename,
        NULL,
        keyPub);
    if (keytype < 0)
    {
        Printf("psParseUnknownPubKey failed\n");
        return PS_FAILURE;
    }

    return PS_SUCCESS;
}

int32 loadKeyPair(psPool_t *pool,
    psPubKey_t *keyPair,
    const char *key_filename)
{
    int32 keytype;

    keytype = psParseUnknownPrivKey(pool,
        1,
        (char *) key_filename,
        NULL,
        keyPair);
    if (keytype < 0)
    {
        Printf("psParseUnknownPrivKey failed\n");
        return PS_FAILURE;
    }

    return PS_SUCCESS;
}


#ifdef USE_OPENSSL_CMD
#include "osdep_stdarg.h"
static
int my_asprintf(char **str, char *format, ...)
{
    va_list argp;
    va_start(argp, format);
    char c[1];
    int len;

    /* Compute required length. */
    len = vsnprintf(c, 1, format, argp);
    if (len < 1)
    {
        *str = NULL;
        return len;
    }
    va_end(argp);

    /* Alloc and print. */
    *str = psMalloc(NULL, len + 1);
    if (*str == NULL)
    {
        return -1;
    }
    va_start(argp, format);
    vsnprintf(*str, len + 1, format, argp);
    va_end(argp);

    return len;
}
#endif /* USE_OPENSSL_CMD */

int32 x509GenerationApiTest(psPool_t *pool)
{
    int32 rc;
    psPubKey_t keys, rsa_keys, ec_keys, pkeys, ca_keys;
    const char *key_filename = "testkeys/RSA/2048_RSA_KEY.pem";
    const char *pkey_filename = "testkeys/RSA/2048_RSA_PUB.pem";
    const char *rsa_key_filename = "testkeys/RSA/2048_RSA_KEY.pem";
#ifdef USE_ECC
    const char *ec_key_filename = "testkeys/EC/256_EC_KEY.pem";
#endif /* USE_ECC */
    const char *ca_key_filename = "testkeys/EC/256_EC_CA_KEY.pem";
    const char *ca_cert_filename = "testkeys/EC/256_EC_CA.pem";

    /* Allow override of file names via environment variables. */
    if (Getenv("X509_GENERATION_KEY_FILENAME"))
    {
        key_filename = Getenv("X509_GENERATION_KEY_FILENAME");
    }

    if (Getenv("X509_GENERATION_PKEY_FILENAME"))
    {
        pkey_filename = Getenv("X509_GENERATION_PKEY_FILENAME");
    }

    if (Getenv("X509_GENERATION_CA_KEY_FILENAME"))
    {
        ca_key_filename = Getenv("X509_GENERATION_CA_KEY_FILENAME");
    }

    if (Getenv("X509_GENERATION_CA_CERT_FILENAME"))
    {
        ca_cert_filename = Getenv("X509_GENERATION_CA_CERT_FILENAME");
    }

    rc = loadKeyPair(pool, &keys, key_filename);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
    rc = loadKeyPair(pool, &rsa_keys, rsa_key_filename);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#ifdef USE_ECC
    rc = loadKeyPair(pool, &ec_keys, ec_key_filename);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
#endif /* USE_ECC */
    Memset(&pkeys, 0, sizeof pkeys);
    rc = loadKeyPub(pool, &pkeys, pkey_filename);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
    rc = loadKeyPair(pool, &ca_keys, ca_key_filename);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = createMaximalCSR(pool, &keys, &pkeys, "x509gen_max_csr.pem");
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = createMinimalCSR(pool, &keys, &pkeys, "x509gen_min_csr.pem");
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = createSelfSignedCert(pool, &keys, "x509gen_self_signed_v1.cer",
            0, KEY_USAGE_DEFAULT, x509_cert_v1);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = createSelfSignedCert(pool, &keys, "x509gen_self_signed_v2.cer",
            0, KEY_USAGE_DEFAULT, x509_cert_v2);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = createSelfSignedCert(pool, &keys, "x509gen_self_signed.cer", 0,
            KEY_USAGE_DEFAULT, x509_cert_v3);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = createSelfSignedCert(pool, &keys,
            "x509gen_self_signed_old_asn1_string_types.cer", 1,
            KEY_USAGE_DEFAULT, x509_cert_v3);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = createSelfSignedCert(pool, &keys,
            "x509gen_self_signed_encipher_only.cer", 1,
            KEY_USAGE_ENCIPHER, x509_cert_v3);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = createSelfSignedCert(pool, &keys,
            "x509gen_self_signed_decipher_only.cer", 1,
            KEY_USAGE_DECIPHER, x509_cert_v3);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    //expect_challengepassword = 1;
    rc = createCASignedCert(pool,
            &ca_keys,
            "x509gen_max_csr.pem",
            ca_cert_filename,
            "x509gen_ca_signed_from_max_csr.pem",
            ALG_SHA256,
            1, 0, 0);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }
    if (expect_challengepassword)
    {
        /* Challenge password not processed during reading. */
        return PS_FAILURE;
    }

    /* Omit all extensions except authorityKeyIdentifier. */
    rc = createCASignedCert(pool,
            &ca_keys,
            "x509gen_max_csr.pem",
            ca_cert_filename,
            "x509gen_ca_signed_from_max_csr_no_ext.pem",
            ALG_SHA256, 0, 1, 1);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    /* Add no extensions to the CA config. Use request extensions only */
    rc = createCASignedCert(pool,
            &ca_keys,
            "x509gen_max_csr.pem",
            ca_cert_filename,
            "x509gen_ca_signed_from_max_cs_only_req_ext.pem",
            ALG_SHA256, 0, 0, 1);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = testWriteCAIssuedCertMemAndFileIO(pool);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    rc = combinatorialGenerate(pool, &rsa_keys, &ec_keys, &ca_keys,
            ca_cert_filename);
    if (rc != PS_SUCCESS)
    {
        return PS_FAILURE;
    }

    psClearPubKey(&keys);
    psClearPubKey(&rsa_keys);
    psClearPubKey(&ec_keys);
    psClearPubKey(&pkeys);
    psClearPubKey(&ca_keys);

    return PS_SUCCESS;
}

#define EXIT_IF(cond, msg)                             \
    do                                                 \
    {                                                  \
        if (cond)                                      \
        {                                              \
            Printf("FAILED");                          \
            exit(EXIT_FAILURE);                        \
        }                                              \
    } while (0);

/* Test asnEncodeDotNotationOID. */
int TEST_asnEncodeDotNotationOID(void)
{
    unsigned char buf[2048] = {0};
    int i, rc;
    psSizeL_t enc_len, real_enc_len;
    char *oid_string;
    psPool_t *pool = NULL;
    const char *oid_short_1 = "1.0.8571.2";
    const char *oid_rsa_encryption = "1.2.840.113549";
    const char *oid_long_1
     = "1.2.840.113549.100000000000000000000000000000000000000000000000";
    const char *oid_long_2
     = "2.23456.840.113549.100000000005000000060000000000007000000000000009";
    const char *oid_long_3
     = "2.987654321987654321.1234567891234567890.1.2.840.113549.3.4.5.6.7.8.0";
    const char *wrong_oid_1 = "4.2.4.815.8";
    const char *wrong_oid_2 = "1.2..3.4";

    Printf("TEST_asnEncodeDotNotationOID... ");

    rc = asnEncodeDotNotationOID(pool, buf, sizeof(buf),
            &enc_len, oid_short_1, Strlen(oid_short_1));
    EXIT_IF(rc < 0, "asnEncodeDotNotationOID");
    if (Memcmp(buf, "\x6\x4\x28\xc2\x7b\x02", enc_len))
    {
        Printf("OID dot notation KAT failed\n");
        Abort();
    }
    Memset(buf, 0, sizeof(buf));

    rc = asnEncodeDotNotationOID(pool, buf, sizeof(buf),
            &enc_len, oid_rsa_encryption, Strlen(oid_rsa_encryption));
    EXIT_IF(rc < 0, "asnEncodeDotNotationOID");
    if (Memcmp(buf, "\x6\x6\x2a\x86\x48\x86\xf7\xd", enc_len))
    {
        Printf("OID dot notation KAT failed\n");
        Abort();
    }

    /* In the following KATs, we use asnFormatOid to convert
       from the encoded OID back to dot-notation string,
       and see if we get the original back. */

    rc = asnEncodeDotNotationOID(pool, buf, sizeof(buf),
            &enc_len, oid_long_1, Strlen(oid_long_1));
    EXIT_IF(rc < 0, "asnEncodeDotNotationOID");
    oid_string = asnFormatOid(pool, buf, enc_len);
    if (Memcmp(oid_string, oid_long_1, Strlen(oid_string)))
    {
        Printf("OID dot notation KAT failed\n");
        Abort();
    }
    Free(oid_string);

    rc = asnEncodeDotNotationOID(pool, buf, sizeof(buf),
            &enc_len, oid_long_2, Strlen(oid_long_2));
    EXIT_IF(rc < 0, "asnEncodeDotNotationOID");
    oid_string = asnFormatOid(pool, buf, enc_len);
    if (Memcmp(oid_string, oid_long_2, Strlen(oid_string)))
    {
        Printf("OID dot notation KAT failed\n");
        Abort();
    }
    Free(oid_string);

    rc = asnEncodeDotNotationOID(pool, buf, sizeof(buf),
            &enc_len, oid_long_3, Strlen(oid_long_3));
    EXIT_IF(rc < 0, "asnEncodeDotNotationOID");
    oid_string = asnFormatOid(pool, buf, enc_len);
    if (Memcmp(oid_string, oid_long_3, Strlen(oid_string)))
    {
        Printf("OID dot notation KAT failed\n");
        Abort();
    }
    Free(oid_string);

    /* Same OID as above, but now use a buffer that is too small.
       We know the length of the encoding, so try all too small
       buffer sizes. Should get PS_OUTPUT_LENGTH always. */
    Memset(&buf, 0, sizeof(buf));
    real_enc_len = enc_len;
    for (i = 0; i < enc_len; i++)
    {
        rc = asnEncodeDotNotationOID(pool, buf, i,
                &enc_len, oid_long_3, Strlen(oid_long_3));
        EXIT_IF(rc != PS_OUTPUT_LENGTH, "asnEncodeDotNotationOID");
        EXIT_IF(enc_len != 0, "asnEncodeDotNotationOID");
    }

    /* Now try with the minimal sufficient size. KAT. */
    rc = asnEncodeDotNotationOID(pool, buf, real_enc_len,
            &enc_len, oid_long_3, Strlen(oid_long_3));
    EXIT_IF(rc < 0, "asnEncodeDotNotationOID");
    oid_string = asnFormatOid(pool, buf, enc_len);
    if (Memcmp(oid_string, oid_long_3, Strlen(oid_string)))
    {
        Printf("OID dot notation KAT failed\n");
        Abort();
    }
    Free(oid_string);

    /* Now try some invalid OIDs. Should get PS_PARSE_FAIL. */
    rc = asnEncodeDotNotationOID(pool, buf, sizeof(buf),
            &enc_len, wrong_oid_1, Strlen(wrong_oid_1));
    EXIT_IF(rc != PS_PARSE_FAIL, "asnEncodeDotNotationOID");
    rc = asnEncodeDotNotationOID(pool, buf, sizeof(buf),
            &enc_len, wrong_oid_2, Strlen(wrong_oid_2));
    EXIT_IF(rc != PS_PARSE_FAIL, "asnEncodeDotNotationOID");

    Printf("SUCCESS\n");

    return PS_SUCCESS;
}

/* A simple test case for special cases of
   psCertReqGetChallengePasswordWorker() function and functions derived from
   it. Note: psCertReqGetChallengePasswordWorker() is a good start for a
   generic ASN.1 DER Item Count/Select implementation. */
static void TEST_psCertReqGetChallengePasswordWorkerTest(void)
{
    psCertReq_t req;
    const unsigned char testdata[] =
    {
        /* Two der encoded items:
           PrintableString 'pass',
           IA5String '0user@example.com' */
        0x13, 0x04, 0x70, 0x61, 0x73, 0x73, 0x16, 0x10, 0x75, 0x73, 0x65, 0x72,
        0x40, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d
    };
    psSize_t len;

#define ENSURE(x) Assert(x)
    Memset(&req, 0, sizeof req);
    ENSURE(psCertReqNumChallengePassword(&req) == 0);
    req.challengeAttribute = testdata;
    ENSURE(psCertReqNumChallengePassword(&req) == 0);
    ENSURE(psCertReqGetChallengePassword(&req, 0) == NULL);
    ENSURE(psCertReqGetChallengePassword(&req, 1) == NULL);
    ENSURE(psCertReqGetChallengePassword(&req, 2) == NULL);
    ENSURE(psCertReqGetChallengePasswordLen(&req, 0) == 0);
    ENSURE(psCertReqGetChallengePasswordLen(&req, 1) == 0);
    ENSURE(psCertReqGetChallengePasswordLen(&req, 2) == 0);
    ENSURE(psCertReqGetChallengePasswordType(&req, 0) == 0);
    ENSURE(psCertReqGetChallengePasswordType(&req, 1) == 0);
    ENSURE(psCertReqGetChallengePasswordType(&req, 2) == 0);

    for(len = 1; len <= 5; len++)
    {
        req.challengeAttributeLen = len;

        /* Single item, that is incomplete. */
        ENSURE(psCertReqNumChallengePassword(&req) == 1);
        ENSURE(psCertReqGetChallengePassword(&req, 0) == NULL);
        ENSURE(psCertReqGetChallengePassword(&req, 1) == NULL);
        ENSURE(psCertReqGetChallengePassword(&req, 2) == NULL);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 0) == 0);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 1) == 0);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 2) == 0);
        ENSURE(psCertReqGetChallengePasswordType(&req, 0) == 0);
        ENSURE(psCertReqGetChallengePasswordType(&req, 1) == 0);
        ENSURE(psCertReqGetChallengePasswordType(&req, 2) == 0);
    }
    for(; len <= 6; len++)
    {
        Printf("Testing len: %d\n", (int) len);
        req.challengeAttributeLen = len;

        /* Single item. */
        ENSURE(psCertReqNumChallengePassword(&req) == 1);
        ENSURE(psCertReqGetChallengePassword(&req, 0) == &testdata[2]);
        ENSURE(psCertReqGetChallengePassword(&req, 1) == NULL);
        ENSURE(psCertReqGetChallengePassword(&req, 2) == NULL);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 0) == 4);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 1) == 0);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 2) == 0);
        ENSURE(psCertReqGetChallengePasswordType(&req, 0) == 0x13);
        ENSURE(psCertReqGetChallengePasswordType(&req, 1) == 0);
        ENSURE(psCertReqGetChallengePasswordType(&req, 2) == 0);
    }
    for(; len < (psSize_t) sizeof testdata; len++)
    {
        req.challengeAttributeLen = len;

        /* Single item + second incomplete item. */
        ENSURE(psCertReqNumChallengePassword(&req) == 2);
        ENSURE(psCertReqGetChallengePassword(&req, 0) == &testdata[2]);
        ENSURE(psCertReqGetChallengePassword(&req, 1) == NULL);
        ENSURE(psCertReqGetChallengePassword(&req, 2) == NULL);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 0) == 4);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 1) == 0);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 2) == 0);
        ENSURE(psCertReqGetChallengePasswordType(&req, 0) == 0x13);
        ENSURE(psCertReqGetChallengePasswordType(&req, 1) == 0);
        ENSURE(psCertReqGetChallengePasswordType(&req, 2) == 0);
    }
    for(; len <= (psSize_t) sizeof testdata; len++)
    {
        req.challengeAttributeLen = len;

        /* Two complete items. */
        ENSURE(psCertReqNumChallengePassword(&req) == 2);
        ENSURE(psCertReqGetChallengePassword(&req, 0) == &testdata[2]);
        ENSURE(psCertReqGetChallengePassword(&req, 1) == &testdata[8]);
        ENSURE(psCertReqGetChallengePassword(&req, 2) == NULL);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 0) == 4);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 1) == 16);
        ENSURE(psCertReqGetChallengePasswordLen(&req, 2) == 0);
        ENSURE(psCertReqGetChallengePasswordType(&req, 0) == 0x13);
        ENSURE(psCertReqGetChallengePasswordType(&req, 1) == 0x16);
        ENSURE(psCertReqGetChallengePasswordType(&req, 2) == 0);
    }
}

int main(int argc, char **argv)
{
    int rc;
    psPool_t *pool = NULL;

    if (psCryptoOpen(PSCRYPTO_CONFIG) < PS_SUCCESS)
    {
        _psTrace("Failed to initialize library:  psCryptoOpen failed\n");
        return PS_FAILURE;
    }

# ifdef USE_MATRIX_MEMORY_MANAGEMENT
    pool = psOpenPool("myPool", 4 * 1024, 0, NULL, NULL);
# endif /* USE_MATRIX_MEMORY_MANAGEMENT */

# ifndef USE_EXTRA_DN_ATTRIBUTES_RFC5280_SHOULD
    Printf("Note: for maximal test coverage, please enable ");
    Printf("USE_EXTRA_DN_ATTRIBUTES_RFC5280_SHOULD\n");
# endif
# ifndef USE_EXTRA_DN_ATTRIBUTES
    Printf("Note: for maximal test coverage, please enable ");
    Printf("USE_EXTRA_DN_ATTRIBUTES\n");
# endif

    TEST_asnEncodeDotNotationOID();
    TEST_psCertReqGetChallengePasswordWorkerTest();

    rc = x509GenerationApiTest(pool);
    if (rc == PS_SUCCESS)
    {
        Printf("All OK\n");
        rc = 0;
    }
    else
    {
        Printf("FAIL\n");
        rc = 1;
    }

# ifdef USE_MATRIX_MEMORY_MANAGEMENT
    psClosePool(pool);
# endif /* USE_MATRIX_MEMORY_MANAGEMENT */
    psCryptoClose();

    return rc;
}

#else
# include "osdep_stdio.h"
int main(int argc, char **argv)
{
    Printf("You need to #define USE_CERT_GEN for this test.\n");
# ifndef MATRIX_USE_FILE_SYSTEM
    Printf("You need to #define MATRIX_USE_FILE_SYSTEM for this test\n");
# endif /* MATRIX_USE_FILE_SYSTEM */
    return 1;
}
#endif /* USE_CERT_GEN && MATRIX_USE_FILE_SYSTEM */
/* end of file x509GenerationApiTest.c */
