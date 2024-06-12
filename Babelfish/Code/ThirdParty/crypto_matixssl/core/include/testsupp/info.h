/* info.h
 *
 * Helper for tests: Get information on platform.
 */

/*****************************************************************************
* Copyright (c) 2018 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*****************************************************************************/

#ifndef TESTSUPP_INFO_H
#define TESTSUPP_INFO_H 1

#include "osdep_stdlib.h"

static inline int TargetIs64Bit(void)
{
    /* Works for most environments. */
    return sizeof(void *) > 4 || sizeof(long) > 4;
}

static inline int TargetEnableSlowTests(void)
{
    /* Allow tests which can take a long time. */
    return !!Getenv("ENABLE_SLOW_TESTS") ||
    !!Getenv("ENABLE_VERY_SLOW_TESTS");
}

static inline int TargetEnableVerySlowTests(void)
{
    /* Allow tests which can take a very long time. */
    return !!Getenv("ENABLE_VERY_SLOW_TESTS");
}

#endif /* TESTSUPP_INFO_H */
