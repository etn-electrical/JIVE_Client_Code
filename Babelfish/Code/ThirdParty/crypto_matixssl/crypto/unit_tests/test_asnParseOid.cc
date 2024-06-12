/*
 * Unit test application for psCrypto APIs.
 */

/*****************************************************************************
* Copyright (c) 2019 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*****************************************************************************/

#include "pscrypto-test-supp.hpp"
#include <vector>

/* Default buffer size used by test.
   Typically maximum is MAX_OID_PRINTED_LEN, but this test should use a
   larger value to detect and handle problems with overflowing
   MAX_OID_PRINTED_LEN. */
#define TEST_MAX_BUF 256

static const char oid0[] =
{
    /* OID 0.1.2 */
    "\x06\x02\x01\x02",
};

static const char oid1[] =
{
    /* OID 1.2.840.113549.1.1.1 - RSA encryption encoded: */
    "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x01\x01",
};

static const char oid2[] =
{
    /* OID 2.25.24197857203266734864793317670504947440
       (UUID 12345678-9abc-def0-1234-56789abcdef0): */
    "\x06\x13\x69\xA4\xB4\xAB\x9E\x93\xAB\xE6\xFB\xE0\x92\x9A\x95\xCF\x89\xD5\xF3\xBD\x70"
};

static const char oid3[] =
{
    /* OID 0.1.2.0 */
    "\x06\x03\x01\x02\x00"
};

static const char oid4[] =
{
    /* OID 2.5.29.15 */
    "\x06\x03\x55\x1D\x0F"
};

static const char oid5[] =
{
    /* OID 2.5.29 */
    "\x06\x02\x55\x1D"
};

/* oid0/oid1/oid2, in string format. */
static const char *oidsStr[] =
{
    "0.1.2",
    "1.2.840.113549.1.1.1",
    "2.25.24197857203266734864793317670504947440"
};

/* Helper: compare C arrays of same type and size. */
#define FAIL_IF_NOT_SAME(type, array1, array2, len)                     \
    do                                                                  \
    {                                                                   \
        std::vector<type> expected((array1), (array1) + (len));         \
        std::vector<type> actual((array2), (array2) + (len));           \
                                                                        \
        FAIL_IF(expected != actual);                                    \
    } while(0)

/* Helper function, like psSprintAsnOid, but returns C++ string and performs no
   database lookup. This function is only for the test cases. */
static std::string printoid(uint8_t oid[MAX_OID_BYTES])
{
    char *mem;
    std::string str;

    mem = asnFormatOid(
            (psPool_t*) MATRIX_NO_POOL,
            (const unsigned char *) oid,
            oid[1] + 2);

    FAIL_IF(mem == NULL);
    str = std::string(mem);
    psFree(mem, (psPool_t*) MATRIX_NO_POOL);
    return str;
}

/* Helper function: count needles in haystack. */
static int chrcount(const char *haystack, const char needle)
{
    int count = 0;

    while(*haystack)
    {
        if (*haystack == needle)
        {
            count++;
        }
        haystack++;
    }
    return count;
}

/* Helper function: count needles in haystack. */
static int bytecount(const unsigned char *haystack, const uint8_t needle, size_t len)
{
    size_t i;
    int count = 0;

    for(i = 0; i < len; i++)
    {
        if (haystack[i] == needle)
        {
            count++;
        }
    }
    return count;
}

PSDEPRECATED_START
TEST_CASE("TEST_asnParseOid")
{
    static const uint32_t expectOid0[MAX_OID_LEN] =
    {
        0, 1, 2
    };
    static const uint32_t expectOid1[MAX_OID_LEN] =
    {
        1, 2, 840, 113549, 1, 1, 1
    };
    uint32_t oid[MAX_OID_LEN] = { 0 };
    uint8_t res;
    int i;
    
    REQUIRE(perform_psCrypto_library_init() == OK);
    res = asnParseOid((const unsigned char *) &oid0[2],
                      sizeof(oid0) - 3,
                      oid);
    REQUIRE(res == 3);
    FAIL_IF_NOT_SAME(uint32_t, expectOid0, oid, MAX_OID_LEN);
    res = asnParseOid((const unsigned char *) &oid1[2],
                      sizeof(oid1) - 3,
                      oid);
    REQUIRE(res == 7);
    FAIL_IF_NOT_SAME(uint32_t, expectOid1, oid, MAX_OID_LEN);
    perform_psCrypto_library_cleanup();
}
PSDEPRECATED_END

PSDEPRECATED_START
TEST_CASE("TEST_asnParseOid_flaws")
{
    /* FLAW in the API: over 32-bit input segments cannot be parsed.
       These are used e.g. by UUIDs converted to OID. */
    
    uint32_t oid[MAX_OID_LEN] = { 0 };
    uint8_t res;
    int i;
    static const uint32_t expectOid0[MAX_OID_LEN] =
    {
        0, 1, 2
    };

    REQUIRE(perform_psCrypto_library_init() == OK);
    res = asnParseOid((const unsigned char *) &oid2[2],
                      sizeof(oid2) - 3,
                      oid);
    REQUIRE(res == 0); /* Parsing problem. */

    /* Issue with typical handling of asnParseOid: return value indicates
       length of output, but in certain cases the value is not stored.
       This means OID X.0 is confused with X.
       if ((oidlen = asnParseOid(p, len, oid)) < 1)
    */
    res = asnParseOid((const unsigned char *) &oid3[2],
                      sizeof(oid3) - 3,
                      oid);
    REQUIRE(res == 4); /* Parsing problem. */
    FAIL_IF_NOT_SAME(uint32_t, expectOid0, oid, MAX_OID_LEN);

    perform_psCrypto_library_cleanup();
}
PSDEPRECATED_END

TEST_CASE("TEST_getAsnOID")
{
    const unsigned char *pp0;
    const unsigned char *pp;
    psSizeL_t size;
    psSize_t paramlen;
    int32_t oi = 0;
    int32_t res;

    REQUIRE(perform_psCrypto_library_init() == OK);

    /* Unknown OID. */
    pp = pp0 = (const unsigned char *) &oid0;
    size = (psSizeL_t) sizeof(oid0) - 1;
    res = getAsnOID(&pp, size, &oi, 0, &paramlen);
    FAIL_IF(res != PS_SUCCESS);
    FAIL_IF(pp != pp0 + 4);
    FAIL_IF((oi & 0xff) != 0x3); /* Masking to ensure same result with and
                                    without OID database. */

    pp = pp0 = (const unsigned char *) &oid1;
    size = (psSizeL_t) sizeof(oid1) - 1;
    res = getAsnOID(&pp, size, &oi, 0, &paramlen);
    FAIL_IF(res != PS_SUCCESS);
    FAIL_IF(pp != pp0 + 11);
    FAIL_IF(oi != OID_RSA_KEY_ALG);

    /* Check under length processing. */
    pp = pp0 = (const unsigned char *) &oid1;
    size = (psSizeL_t) 4;
    res = getAsnOID(&pp, size, &oi, 0, &paramlen);
    FAIL_IF(res != PS_LIMIT_FAIL);
    
    perform_psCrypto_library_cleanup();
}

TEST_CASE("TEST_asnFormatOid")
{
    char *mem;

    REQUIRE(perform_psCrypto_library_init() == OK);

    mem = asnFormatOid(
            (psPool_t*) MATRIX_NO_POOL,
            (const unsigned char *) oid0,
            sizeof(oid0) - 1);

    FAIL_IF(mem == NULL);
    FAIL_IF(mem != std::string(oidsStr[0]));
    psFree(mem, (psPool_t*) MATRIX_NO_POOL);

    mem = asnFormatOid(
            (psPool_t*) MATRIX_NO_POOL,
            (const unsigned char *) oid1,
            sizeof(oid1) - 1);

    FAIL_IF(mem == NULL);
    FAIL_IF(mem != std::string(oidsStr[1]));
    psFree(mem, (psPool_t*) MATRIX_NO_POOL);

    perform_psCrypto_library_cleanup();
};

TEST_CASE("TEST_asnCopyOid")
{
    uint8_t oid[MAX_OID_BYTES] = { 0 };
    uint8_t res;
    int i;
    /* Copy OBJECT IDENTIFIER. */

    /* Detect if user has enlarged MAX_OID_BYTES too much. */
    REQUIRE(MAX_OID_BYTES <= 128);
    REQUIRE(sizeof(psAsnOid_t));
            
    REQUIRE(perform_psCrypto_library_init() == OK);
    res = asnCopyOid((const unsigned char *) &oid0[2],
                     sizeof(oid0) - 3,
                     oid);
    REQUIRE(res == 3);
    REQUIRE(printoid(oid) == oidsStr[0]);

    res = asnCopyOid((const unsigned char *) &oid1[2],
                     sizeof(oid1) - 3,
                     oid);
    REQUIRE(res == 7);
    REQUIRE(printoid(oid) == oidsStr[1]);

    res = asnCopyOid((const unsigned char *) &oid2[2],
                     sizeof(oid2) - 3,
                     oid);
    REQUIRE(res == 3);
    REQUIRE(printoid(oid) == oidsStr[2]);
    perform_psCrypto_library_cleanup();
}

/* Note: table copied from x509.c, with extensions. */
#   define OID_LIST(A, B, C, D) { { A, B }, #A, #B, oid_ ## B, #C, D}
static const struct
{
    uint16_t oid[MAX_OID_LEN];
    char base[33];
    char name[33];
    int id;
    const char *oidstr;
    const char *oid_encoded;
} oid_list[] = {
    /* X.509 certificate extensions */
    OID_LIST(id_ce, id_ce_authorityKeyIdentifier, 2.5.29.35, "\x06\x03\x55\x1D\x23"),
    OID_LIST(id_ce, id_ce_subjectKeyIdentifier, 2.5.29.14, "\x06\x03\x55\x1D\x0E"),
    OID_LIST(id_ce, id_ce_keyUsage, 2.5.29.15, "\x06\x03\x55\x1D\x0F"),
    OID_LIST(id_ce, id_ce_certificatePolicies, 2.5.29.32, "\x06\x03\x55\x1D\x20"),
    OID_LIST(id_ce, id_ce_policyMappings, 2.5.29.33, "\x06\x03\x55\x1D\x21"),
    OID_LIST(id_ce, id_ce_subjectAltName, 2.5.29.17, "\x06\x03\x55\x1D\x11"),
    OID_LIST(id_ce, id_ce_issuerAltName, 2.5.29.18, "\x06\x03\x55\x1D\x12"),
    OID_LIST(id_ce, id_ce_subjectDirectoryAttributes, 2.5.29.9, "\x06\x03\x55\x1D\x09"),
    OID_LIST(id_ce, id_ce_basicConstraints, 2.5.29.19, "\x06\x03\x55\x1D\x13"),
    OID_LIST(id_ce, id_ce_nameConstraints, 2.5.29.30, "\x06\x03\x55\x1D\x1E"),
    OID_LIST(id_ce, id_ce_policyConstraints, 2.5.29.36, "\x06\x03\x55\x1D\x24"),
    OID_LIST(id_ce, id_ce_extKeyUsage, 2.5.29.37, "\x06\x03\x55\x1D\x25"),
    OID_LIST(id_ce, id_ce_cRLDistributionPoints, 2.5.29.31, "\x06\x03\x55\x1D\x1F"),
    OID_LIST(id_ce, id_ce_cRLNumber, 2.5.29.20, "\x06\x03\x55\x1D\x14"),
    OID_LIST(id_ce, id_ce_issuingDistributionPoint, 2.5.29.28, "\x06\x03\x55\x1D\x1C"),
    OID_LIST(id_ce, id_ce_inhibitAnyPolicy, 2.5.29.54, "\x06\x03\x55\x1D\x36"),
    OID_LIST(id_ce, id_ce_freshestCRL, 2.5.29.46, "\x06\x03\x55\x1D\x2E"),
    OID_LIST(id_pe, id_pe_authorityInfoAccess, 1.3.6.1.5.5.7.1.1, "\x06\x08\x2B\x06\x01\x05\x05\x07\x01\x01"),
    OID_LIST(id_pe, id_pe_subjectInfoAccess, 1.3.6.1.5.5.7.1.11, "\x06\x08\x2B\x06\x01\x05\x05\x07\x01\x0B"),
    OID_LIST(id_ce_eku, id_ce_eku_anyExtendedKeyUsage, 2.5.29.37.0, "\x06\x04\x55\x1D\x25\x00"),
    OID_LIST(id_kp, id_kp_serverAuth, 1.3.6.1.5.5.7.3.1, "\x06\x08\x2B\x06\x01\x05\x05\x07\x03\x01"),
    OID_LIST(id_kp, id_kp_clientAuth, 1.3.6.1.5.5.7.3.2, "\x06\x08\x2B\x06\x01\x05\x05\x07\x03\x02"),
    OID_LIST(id_kp, id_kp_codeSigning, 1.3.6.1.5.5.7.3.3, "\x06\x08\x2B\x06\x01\x05\x05\x07\x03\x03"),
    OID_LIST(id_kp, id_kp_emailProtection, 1.3.6.1.5.5.7.3.4, "\x06\x08\x2B\x06\x01\x05\x05\x07\x03\x04"),
    OID_LIST(id_kp, id_kp_timeStamping, 1.3.6.1.5.5.7.3.8, "\x06\x08\x2B\x06\x01\x05\x05\x07\x03\x08"),
    OID_LIST(id_kp, id_kp_OCSPSigning, 1.3.6.1.5.5.7.3.9, "\x06\x08\x2B\x06\x01\x05\x05\x07\x03\x09"),
    OID_LIST(id_qt, id_qt_cps, 1.3.6.1.5.5.7.2.1, "\x06\x08\x2B\x06\x01\x05\x05\x07\x02\x01"),
    OID_LIST(id_qt, id_qt_unotice, 1.3.6.1.5.5.7.2.2, "\x06\x08\x2B\x06\x01\x05\x05\x07\x02\x02"),
    OID_LIST(id_ad, id_ad_caIssuers, 1.3.6.1.5.5.7.48.2, "\x06\x08\x2B\x06\x01\x05\x05\x07\x30\x02"),
    OID_LIST(id_ad, id_ad_ocsp, 1.3.6.1.5.5.7.48.1, "\x06\x08\x2B\x06\x01\x05\x05\x07\x30\x01"),
    /* List terminator */
    OID_LIST(0, 0, 0, NULL),
};

/* Note: psFindOid() is a function from x509.c.
   It is replicated here to faciliate testing. */

/**
    Look up an OID in our known oid table.
    @param[in] oid Array of OID segments to look up in table.
    @param[in] oidlen Number of segments in 'oid'
    @return A valid OID enum on success, 0 on failure.
 */
static oid_e psFindOid(const uint32_t oid[MAX_OID_LEN], uint8_t oidlen)
{
    int i, j;

    psAssert(oidlen <= MAX_OID_LEN);
    for (j = 0; oid_list[j].id != 0; j++)
    {
        for (i = 0; i < oidlen; i++)
        {
            if ((uint16_t) (oid[i] & 0xFFFF) != oid_list[j].oid[i])
            {
                break;
            }
            if ((i + 1) == oidlen)
            {
                return (oid_e) oid_list[j].id;
            }
        }
    }
    return (oid_e) 0;
}

/* Note: psSprintOid() is function based on psTraceOid in x509.c.
   It is replicated here to faciliate testing. */

/**
    Print an OID in dot notation, with it's symbolic name, if found.
    @param[in] oid Array of OID segments print.
    @param[in] oidlen Number of segments in 'oid'
    @param[out] out formatting target buffer (must be sufficient: 256 will suffice.)
    @return void
 */
static const char *psSprintOid(uint32_t oid[MAX_OID_LEN],
                               uint8_t oidlen,
                               char out[256])
{
    int i, j, found;
    const char *orig_out = out;

    for (i = 0; i < oidlen; i++)
    {
        if ((i + 1) < oidlen)
        {
            sprintf(out, "%u.", oid[i]);
            out += strlen(out);
        }
        else
        {
            sprintf(out, "%u", oid[i]);
            out += strlen(out);
        }
    }
    found = 0;
    for (j = 0; oid_list[j].oid[0] != 0 && !found; j++)
    {
        for (i = 0; i < oidlen; i++)
        {
            if ((uint8_t) (oid[i] & 0xFF) != oid_list[j].oid[i])
            {
                break;
            }
            if ((i + 1) == oidlen)
            {
                sprintf(out, " (%s)", oid_list[j].name);
                found++;
            }
        }
    }
    return orig_out; /* Return buffer for convenience. */
}

/* Note: psFindOid() is a function from x509.c.
   It is replicated here to faciliate testing. */

/**
    Look up an OID in our known oid table.
    @param[in] oid Copy (or reference) to oid
    @return A valid OID enum on success, 0 on failure.
 */
static oid_e psOidToEnum(psAsnOid_t oid)
{
    int i;
    uint8_t id;
    uint8_t len_encoded;
    unsigned int len;

    id = oid[0];
    len_encoded = oid[1];
    len = len_encoded + 2;

    if (id != ASN_OID || len < 2 || len > MAX_OID_BYTES)
    {
        return (oid_e) 0;
    }

    for (i = 0; oid_list[i].id != 0; i++)
    {
        if (oid_list[i].oid_encoded[1] == len_encoded &&
            memcmp(oid, oid_list[i].oid_encoded, len) == 0)
        {
            return (oid_e) oid_list[i].id;
        }
    }
    return (oid_e) 0;
}

/* Note: psSprintAsnOid() is function based on psTraceOid in x509.c.
   It is replicated here to faciliate testing. */

PSDEPRECATED_START
TEST_CASE("TEST_asnParseOid_find_and_print")
{
    uint32_t oid[MAX_OID_LEN] = { 0 };
    uint8_t res;
    int i;
    char out[TEST_MAX_BUF];
    oid_e oe;
    
    REQUIRE(perform_psCrypto_library_init() == OK);
    res = asnParseOid((const unsigned char *) &oid0[2],
                      sizeof(oid0) - 3,
                      oid);
    REQUIRE(res == 3);
    FAIL_IF(std::string("0.1.2") != psSprintOid(oid, res, out));
    oe = psFindOid(oid, res);
    FAIL_IF(oe != oid_0);
    res = asnParseOid((const unsigned char *) &oid1[2],
                      sizeof(oid1) - 3,
                      oid);
    REQUIRE(res == 7);
    FAIL_IF(std::string("1.2.840.113549.1.1.1") !=
            psSprintOid(oid, res, out));
    oe = psFindOid(oid, res);
    FAIL_IF(oe != oid_0);
    res = asnParseOid((const unsigned char *) &oid4[2],
                      sizeof(oid4) - 3,
                      oid);
    REQUIRE(res == 4);
    oe = psFindOid(oid, res);
    FAIL_IF(std::string("2.5.29.15 (id_ce_keyUsage)") !=
            psSprintOid(oid, res, out));
    oe = psFindOid(oid, res);
    FAIL_IF(oe != oid_id_ce_keyUsage);
    perform_psCrypto_library_cleanup();
}
PSDEPRECATED_END

#if REBUILD_OID_LIST
/* This "test" reconstructs oid list. The code is provided here for convenience, because
   producing oid encodings is bit complex.
   The reconstruction only requires first two fields. */
TEST_CASE("TEST_makeoid_list")
{
    int i, j;
    char out[TEST_MAX_BUF];
    char cmd[TEST_MAX_BUF];
    char str[TEST_MAX_BUF];
    FILE *f;
    char *lf;

    for (j = 0; oid_list[j].id != 0; j++)
    {
        char *s = out;
        printf("    OID_LIST(%s, %s, ", oid_list[j].base, oid_list[j].name);
        for (i = 0; i < MAX_OID_LEN; i++)
        {
            if (oid_list[j].oid[i] != 0)
            {
                sprintf(s, "%s%d", i > 0 ? "." : "", oid_list[j].oid[i]);
                s += strlen(s);
            }
        }
        sprintf(cmd, "../../../bin/oid -C %s", out);
        f = popen(cmd, "r");
        if (f)
        {
            char *str_out = fgets(str, TEST_MAX_BUF, f);
            pclose(f);

            if (str_out == NULL)
            {
                str[0] = 0; /* Omit result if too short or error. */
            }
        }
        else
        {
            str[0] = 0; /* Omit oid. */
        }

        /* Remove linefeed. */
        lf = strchr(str, '\n');
        if (lf)
        {
            *lf = 0;
        }
        
        printf("%s, %s),\n", out, str);
    }
}
#endif /* REBUILD_OID_LIST */

PSDEPRECATED_START
TEST_CASE("TEST_asnParseOid_find_and_print_flaws")
{
    /* asnFindOid() has the problem that it is able to match prefix.
       Therefore, 2.5.29 (id_ce) matches id_ce_authorityKeyIdentifier. */
    
    uint32_t oid[MAX_OID_LEN] = { 0 };
    uint8_t res;
    int i;
    char out[TEST_MAX_BUF];
    oid_e oe;
    
    REQUIRE(perform_psCrypto_library_init() == OK);
    res = asnParseOid((const unsigned char *) &oid5[2],
                      sizeof(oid5) - 3,
                      oid);
    REQUIRE(res == 3);
    oe = psFindOid(oid, res);
    FAIL_IF(std::string("2.5.29 (id_ce_authorityKeyIdentifier)") !=
            psSprintOid(oid, res, out));
    oe = psFindOid(oid, res);
    FAIL_IF(oe != oid_id_ce_authorityKeyIdentifier);
    perform_psCrypto_library_cleanup();
}
PSDEPRECATED_END

TEST_CASE("TEST_asnCopyOid_find_and_print")
{
    psAsnOid_t oid = { 0 };
    uint8_t res;
    int i;
    char out[TEST_MAX_BUF];
    oid_e oe;
    
    REQUIRE(perform_psCrypto_library_init() == OK);
    res = asnCopyOid((const unsigned char *) &oid0[2],
                      sizeof(oid0) - 3,
                      oid);
    REQUIRE(res == 3);
    FAIL_IF(std::string("0.1.2") != psSprintAsnOid(oid, out));
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);
    res = asnCopyOid((const unsigned char *) &oid1[2],
                     sizeof(oid1) - 3,
                     oid);
    REQUIRE(res == 7);
    FAIL_IF(std::string("1.2.840.113549.1.1.1") !=
            psSprintAsnOid(oid, out));
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);
    res = asnCopyOid((const unsigned char *) &oid4[2],
                     sizeof(oid4) - 3,
                     oid);
    REQUIRE(res == 4);
    FAIL_IF(std::string("2.5.29.15 (id_ce_keyUsage)") !=
            psSprintAsnOid(oid, out));
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_id_ce_keyUsage);
    perform_psCrypto_library_cleanup();
}

TEST_CASE("TEST_asnCopyOid_find_and_print_ext")
{
    psAsnOid_t oid;
    uint8_t res;
    int i;
    char out[TEST_MAX_BUF];
    oid_e oe;
    
    REQUIRE(perform_psCrypto_library_init() == OK);

    /* Old asnFindOid() has the problem that it is able to match prefix.
       Therefore, 2.5.29 (id_ce) matched id_ce_authorityKeyIdentifier.
       Check this issue no longer exists with asnCopyOid(). */
    res = asnCopyOid((const unsigned char *) &oid5[2],
                     sizeof(oid5) - 3,
                     oid);
    REQUIRE(res == 3);
    FAIL_IF(std::string("2.5.29") != psSprintAsnOid(oid, out));
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

    /* asnParseOid() had issue with long inputs.
       Check there is no longer problem with . */
    res = asnCopyOid((const unsigned char *) &oid2[2],
                      sizeof(oid2) - 3,
                      oid);
    REQUIRE(res == 3);
    FAIL_IF(std::string(oidsStr[2]) != psSprintAsnOid(oid, out));
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);
    perform_psCrypto_library_cleanup();
}

TEST_CASE("TEST_asnCopyOid_oidlist")
{
    /* walk through entire oidlist. */
    int i;
    int res;
    char out[TEST_MAX_BUF];
    char expect[TEST_MAX_BUF];
    char *str;
    psAsnOid_t oid;

    REQUIRE(perform_psCrypto_library_init() == OK);

    FAIL_IF(MAX_OID_PRINTED_LEN > sizeof(out)); /* Ensure correct buffer size. */
    
    for (i = 0; oid_list[i].id != 0; i++)
    {
        res = asnCopyOid(
                (const unsigned char *)oid_list[i].oid_encoded + 2,
                oid_list[i].oid_encoded[1],
                oid);
        REQUIRE(res == chrcount(oid_list[i].oidstr, '.') + 1); /* OID is separated by dots. */
        psSprintAsnOid(oid, out);
        sprintf(expect, "%s (%s)", oid_list[i].oidstr, oid_list[i].name);
        FAIL_IF(strlen(out) >= MAX_OID_PRINTED_LEN_NAMED); /* Too short buffer requested. */
        FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
        FAIL_IF(std::string(expect) != out);
    }
    perform_psCrypto_library_cleanup();
}

TEST_CASE("TEST_asnCopyOid_long")
{
    /* Test boundary of OID printing. */
    const char *short_oid = "\x06\x08\x05\x81\xA3\x98\xA8\x7A\x92\x27";
    const char *long_oid1 = "\x06\x1B\x2D\x87\xED\xB3\xDD\xC4\xAF\x4A\x85\xEF\xBE\x01\x85\xF0\xD8\x5B\x8E\xA7\x37\x21\x81\xA3\x98\xA8\x7A\x92\x27";
    const char *long_oid2 = "\x06\x1C\x55\x87\xED\xB3\xDD\xC4\xAF\x4A\x85\xEF\xBE\x01\x85\xF0\xD8\x5B\x8E\xA7\x37\x21\x81\xA3\x98\xA8\x7A\x81\xB7\x08";
    const char *long_oid3 = "\x06\x1D\x55\x87\xED\xB3\xDD\xC4\xAF\x4A\x85\xEF\xBE\x01\x85\xF0\xD8\x5B\x8E\xA7\x37\x21\x81\xA3\x98\xA8\x7A\x81\xB7\x08\x21";
    const char *long_oid4 = "\x06\x1E\x55\x87\xED\xB3\xDD\xC4\xAF\x4A\x85\xEF\xBE\x01\x85\xF0\xD8\x5B\x8E\xA7\x37\x21\x81\xA3\x98\xA8\x7A\x81\xB7\x08\x21\x01";
    const char *long_oid4_2 = "\x06\x1E\x60\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F";
    const char *long_oid4_3 = "\x06\x1E\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F";
    const char *long_oid5 =  "\x06\x1F\x55\x87\xED\xB3\xDD\xC4\xAF\x4A\x85\xEF\xBE\x01\x85\xF0\xD8\x5B\x8E\xA7\x37\x21\x81\xA3\x98\xA8\x7A\x81\xB7\x08\x21\x01\x01";
    psAsnOid_t oid;
    char out[TEST_MAX_BUF];
    int res;
    const char *oidstr;
    oid_e oe;

    REQUIRE(perform_psCrypto_library_init() == OK);

    FAIL_IF(MAX_OID_PRINTED_LEN > sizeof(out)); /* Ensure correct buffer size. */

    if (MAX_OID_BYTES != 32)
    {
        WARN("SKIPPED: This test has been made for default setting of MAX_OID_BYTES == 32.");
        goto skip;
    }

    /* Start with short OID. */
    oidstr = "0.5.342234234.2343";
    res = asnCopyOid((const unsigned char *) short_oid + 2, short_oid[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == chrcount(oidstr, '.') + 1);
    CHECK(res == asnOidLenSegments(oid));
    CHECK(strlen(short_oid) == asnOidLenBytes(oid)); /* Note: short_oid has no zero bytes */
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

    oidstr = "1.5.34545423423434.12312321.12332123.234423.33.342234234.2343";
    res = asnCopyOid((const unsigned char *) long_oid1 + 2, long_oid1[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == chrcount(oidstr, '.') + 1);
    CHECK(res == asnOidLenSegments(oid));
    CHECK(res == asnOidLenSegments(oid));
    CHECK(strlen(long_oid1) == asnOidLenBytes(oid)); /* Note: long_oid1 has no zero bytes */
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

    oidstr = "2.5.34545423423434.12312321.12332123.234423.33.342234234.23432";
    res = asnCopyOid((const unsigned char *) long_oid2 + 2, long_oid2[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == chrcount(oidstr, '.') + 1);
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

    oidstr = "2.5.34545423423434.12312321.12332123.234423.33.342234234.23432.33";
    res = asnCopyOid((const unsigned char *) long_oid3 + 2, long_oid3[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == chrcount(oidstr, '.') + 1);
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

    oidstr = "2.5.34545423423434.12312321.12332123.234423.33.342234234.23432.33.1";
    res = asnCopyOid((const unsigned char *) long_oid4 + 2, long_oid4[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == chrcount(oidstr, '.') + 1);
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

    oidstr = "2.16.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127";
    res = asnCopyOid((const unsigned char *) long_oid4_2 + 2, long_oid4_2[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == chrcount(oidstr, '.') + 1);
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

    oidstr = "2.47.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127.127";
    res = asnCopyOid((const unsigned char *) long_oid4_3 + 2, long_oid4_3[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == chrcount(oidstr, '.') + 1);
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

    oidstr = "(Illegal OID)";
    res = asnCopyOid((const unsigned char *) long_oid5 + 2, long_oid5[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == 0); /* Too long oid binary presentation. */
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

skip:
    perform_psCrypto_library_cleanup();
}

TEST_CASE("TEST_psCopyOid_psSprintAsnOid_previous_bugs")
{
    /* This test case tries to prevent reintroduction of previous bugs. */

    psAsnOid_t oid;
    char out[TEST_MAX_BUF];
    int res;
    const char *oidstr;
    oid_e oe;

    /* Test cases where printing was unable to format OID correctly. */
    /* The printer could not format OID(2.x...), where x >= 40. */
    const char *oid1 = "\x06\x02\x7A\x01";
    const char *oid2 = "\x06\x03\x81\x4F\x01";

    /* Oid that has too larger second segment. */
    oidstr = "2.42.1"; /* Note: This OID is ITU-T X.1081. */
    res = asnCopyOid((const unsigned char *) oid1 + 2, oid1[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == chrcount(oidstr, '.') + 1);
    CHECK(res == asnOidLenSegments(oid));
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);

    /* Another oid: larger value that requires multibyte representation for
       root and first segment. */
    oidstr = "2.127.1"; /* Note: This OID has not been allocated, but
                           could be allocated in future. */
    res = asnCopyOid((const unsigned char *) oid2 + 2, oid2[1], oid);
    psSprintAsnOid(oid, out);
    CHECK(res == chrcount(oidstr, '.') + 1); /* check asnCopyOid counts multibyte correctly. */
    CHECK(res == asnOidLenSegments(oid));
    FAIL_IF(strlen(out) > MAX_OID_PRINTED_LEN); /* Too short buffer requested. */
    FAIL_IF(std::string(oidstr) != out);
    oe = psOidToEnum(oid);
    FAIL_IF(oe != oid_0);
}

TEST_CASE("TEST_psAsnWriteOid")
{
    const unsigned char enc[] =
    {
        0x06, 0x08, 0x05, 0x81, 0xA3, 0x98, 0xA8, 0x7A, 0x92, 0x27
    };
    psAsnOid_t oid;
    unsigned char out_bytes0[9];
    unsigned char out_bytes1[10];
    unsigned char out_bytes2[11];
    unsigned char out_bytes3[20];
    unsigned char out_bytes4[100];
    psSizeL_t len;
    int res;

    /* Note: test assumes input with no 0x3a bytes. */
    res = asnCopyOid(enc + 2, enc[1], oid);
    CHECK(res > 0);

    memset(out_bytes0, 0x3A, sizeof out_bytes0);
    len = psAsnWriteOid(oid, out_bytes0, (psSizeL_t) sizeof out_bytes0);
    FAIL_IF(len != 0);
    FAIL_IF(bytecount(out_bytes0, 0x3A, sizeof out_bytes0) != 9);

    memset(out_bytes1, 0x3A, sizeof out_bytes1);
    len = psAsnWriteOid(oid, out_bytes1, (psSizeL_t) sizeof out_bytes1);
    FAIL_IF(len != sizeof(enc));
    FAIL_IF(bytecount(out_bytes1, 0x3A, sizeof out_bytes1) != sizeof out_bytes1 - sizeof(enc));
    FAIL_IF_NOT_SAME(uint8_t, enc, out_bytes1, sizeof(enc));

    memset(out_bytes2, 0x3A, sizeof out_bytes2);
    len = psAsnWriteOid(oid, out_bytes2, (psSizeL_t) sizeof out_bytes2);
    FAIL_IF(len != sizeof(enc));
    FAIL_IF(bytecount(out_bytes2, 0x3A, sizeof out_bytes2) != sizeof out_bytes2 - sizeof(enc));
    FAIL_IF_NOT_SAME(uint8_t, enc, out_bytes2, sizeof(enc));

    memset(out_bytes3, 0x3A, sizeof out_bytes3);
    len = psAsnWriteOid(oid, out_bytes3, (psSizeL_t) sizeof out_bytes3);
    FAIL_IF(len != sizeof(enc));
    FAIL_IF(bytecount(out_bytes3, 0x3A, sizeof out_bytes3) != sizeof out_bytes3 - sizeof(enc));
    FAIL_IF_NOT_SAME(uint8_t, enc, out_bytes3, sizeof(enc));

    memset(out_bytes4, 0x3A, sizeof out_bytes4);
    len = psAsnWriteOid(oid, out_bytes4, (psSizeL_t) sizeof out_bytes4);
    FAIL_IF(len != sizeof(enc));
    FAIL_IF(bytecount(out_bytes4, 0x3A, sizeof out_bytes4) != sizeof out_bytes4 - sizeof(enc));
    FAIL_IF_NOT_SAME(uint8_t, enc, out_bytes4, sizeof(enc));
}

