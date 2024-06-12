/*
 * Usual test skeleton for psCrypto API testing.
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

/* Common set of includes: */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "osdep-types.h"
#include "psUtil.h"
#include "testsupp/testsupp.hpp"
#include "cryptoApi.h"

/* Helper definitions.
   Could be moved to main testsupp.hpp. */
#define FAIL_IF(xxx) REQUIRE_FALSE(xxx)
#define STRING_AS_CUCPTRSZ(str) \
  ((const unsigned char*)(std::string(str).c_str())), \
    (std::string(str).length())

/* Helper functions for common tasks. */
static inline
TEST_RESULT perform_psCrypto_library_init(void)
{
    int32_t initializeResult;

    initializeResult = psCryptoOpen(PSCRYPTO_CONFIG);
    FAIL_IF(initializeResult != PS_SUCCESS);
    return OK;
}

static inline
TEST_RESULT perform_psCrypto_library_cleanup(void)
{
    psCryptoClose();
    return OK;
}
