#include "../matrixCmsApi.h"
#include "psUtil.h"
#include "osdep_stdio.h"
#include "RTK_Example_bin_p7b.c"
#include "RTK_Example_bin.c"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

/* This tests parsing and verification of a SignedData where the
   EncapsulatedContentInfo has indefinite length encoding and
   contains no eContent, and where the SignedData contains the
   RFC 6211 algorithmProtection signed attribute. Parsing and
   verification should succeed, although MatrixSSL only parses
   and does not handle the algorithmProtection signed attribute. */

extern unsigned char RTK_Example_bin_p7b[];
extern unsigned int RTK_Example_bin_p7b_len;

TEST_CASE("matrixcms-signeddata-indefencap-algorithmprotection", "matrixcms")
{
    cmsSignedData_t *signedData, *signedData2;
    int rc = 0;

    uint8_t hashed_data[] =
        {
            0x5D, 0x01, 0x5C, 0xD4, 0xD6, 0xDC, 0x1B, 0xBC, 0x76,
            0x6A, 0x1A, 0xB4, 0x1F, 0x17, 0xD0, 0xA7, 0xD1, 0x54,
            0xCD, 0x26, 0x39, 0xC0, 0x6F, 0x10, 0x23, 0x82, 0xD1,
            0xFF, 0x36, 0xBA, 0x82, 0x5F
        };

    /* We should be able to parse this now, even though we still
       don't support the algorithmProtection signed attr. */
    rc = matrixCmsParseSignedData(
            NULL,
            RTK_Example_bin_p7b,
            RTK_Example_bin_p7b_len,
            &signedData,
            0);
    REQUIRE(rc == PS_SUCCESS);

    /* Corrupt the SignedData. Should no longer parse, assuming
       we hit in the right spot. */
    RTK_Example_bin_p7b[66] = 0x66;
    rc = matrixCmsParseSignedData(
            NULL,
            RTK_Example_bin_p7b,
            RTK_Example_bin_p7b_len,
            &signedData2,
            0);
    REQUIRE(rc != PS_SUCCESS);

    /* Sig should verify. */
    rc = matrixCmsConfirmSignature(
            NULL,
            signedData,
            RTK_Example_bin,
            RTK_Example_bin_len,
            NULL);
    REQUIRE(rc == PS_SUCCESS);

    /* Corrupt the reference (tbs) data. Sig should no longer verify. */
    RTK_Example_bin[666] = 0x66;
    rc = matrixCmsConfirmSignature(
            NULL,
            signedData,
            RTK_Example_bin,
            RTK_Example_bin_len,
            NULL);
    REQUIRE(rc != PS_SUCCESS);

    printf("All OK\n");
}
