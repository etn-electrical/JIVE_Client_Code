/*
 * A test case for core API testing.
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

#define _FILE_OFFSET_BITS 64 /* Allow opening large file. */
#include "coreApi.h"
#include "psUtil.h"
#include "testsupp/testsupp.h"
#include "testsupp/info.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

psPool_t *pool = NULL;

static int can_read(const char *Filename_p)
{
    FILE *f = fopen(Filename_p, "r");
    int success = 0;

    if (f != NULL)
    {
        fclose(f);
        success = 1;
    }
    return success;
}

static size_t sum(const unsigned char *bytes, size_t sz)
{
    size_t sum = 0;
    size_t i;

    for(i = 0; i < sz; i++)
    {
        sum += bytes[i];
    }

    return sum;
}

AUTO_TEST(TEST_psGetFileBuf_empty_file)
{
    const char *file = "/dev/null";
    int32 res;
    unsigned char array[2] = { '?', 0 };
    unsigned char *buf = array;
    psSizeL_t len = 3;

    if (!can_read(file))
    {
        return SKIPPED;
    }

    res = psGetFileBuf(pool, file, &buf, &len);
    FAIL_IF(res != PS_SUCCESS);
    FAIL_IF(buf == NULL);
    FAIL_IF(buf == array);
    FAIL_IF(len != 0);
    FAIL_IF(strlen((const char *) buf) != 0);
    psFree(buf, pool);
    return OK;
}

AUTO_TEST(TEST_psGetFileBuf_endless_file)
{
    const char *file = "/dev/zero";
    int32 res;
    unsigned char array[2] = { '?', 0 };
    unsigned char *buf = array;
    psSizeL_t len = 3;

    if (!can_read(file))
    {
        return SKIPPED;
    }

    res = psGetFileBuf(pool, file, &buf, &len);
    FAIL_IF(res != PS_SUCCESS);
    FAIL_IF(buf == NULL);
    FAIL_IF(buf == array);
    FAIL_IF(len != 0);
    FAIL_IF(strlen((const char *) buf) != 0);
    psFree(buf, pool);
    return OK;
}

AUTO_TEST(TEST_psGetFileBuf_4294967295_bytes_file)
{
#define FILENAME "long_file"
    int32 res;
    unsigned char array[2] = { '?', 0 };
    unsigned char *buf = array;
    psSizeL_t len = 3;

    if (TargetIs64Bit() && !TargetEnableSlowTests())
    {
        /* Reading 4GB file will succeed on most 64-bit platforms but
           take a long time. */
        return SKIPPED;
    }

    FAIL_IF(system("rm " FILENAME) < -1);
    FAIL_IF(system("fallocate -l 4294967295 " FILENAME) < -1);
    if (!can_read(FILENAME))
    {
        FAIL_IF(system("rm " FILENAME) < -1);
        return SKIPPED;
    }

    /* File is too large to fit in memory (on some platforms). */
    /* This test may fail on some operating systems. */
    res = psGetFileBuf(pool, FILENAME, &buf, &len);
    if (res == PS_SUCCESS)
    {
        FAIL_IF(res != PS_SUCCESS);
        FAIL_IF(buf == NULL);
        FAIL_IF(buf == array);
        FAIL_IF(len != 4294967295U);
        FAIL_IF(sum(buf, len) != 0);
        return OK;
    }
    FAIL_IF(res == PS_SUCCESS);
    FAIL_IF(system("rm " FILENAME) < 0);
    return OK;
#undef FILENAME
}

AUTO_TEST(TEST_psGetFileBuf_4294967297_bytes_file)
{
#define FILENAME "long_file"
    int32 res;
    unsigned char array[2] = { '?', 0 };
    unsigned char *buf = array;
    psSizeL_t len = 3;

    if (TargetIs64Bit() && !TargetEnableSlowTests())
    {
        /* Reading 4GB file will succeed on most 64-bit platforms but
           take a long time. */
        return SKIPPED;
    }

    FAIL_IF(system("rm " FILENAME) < -1);
    FAIL_IF(system("fallocate -l 4294967297 " FILENAME) < -1);
    if (!can_read(FILENAME))
    {
        FAIL_IF(system("rm " FILENAME) < -1);
        return SKIPPED;
    }

    /* File is too large to fit in memory. */
    /* This test may fail on some operating systems. */
    res = psGetFileBuf(pool, FILENAME, &buf, &len);
    if (res == PS_SUCCESS)
    {
        FAIL_IF(res != PS_SUCCESS);
        FAIL_IF(buf == NULL);
        FAIL_IF(buf == array);
        FAIL_IF(((uint64_t) len) != 4294967297ULL);
        FAIL_IF(sum(buf, len) != 0);
        return OK;
    }
    FAIL_IF(res == PS_SUCCESS);
    FAIL_IF(system("rm " FILENAME) < 0);
    return OK;
#undef FILENAME
}

TESTSUPP_MAIN();
