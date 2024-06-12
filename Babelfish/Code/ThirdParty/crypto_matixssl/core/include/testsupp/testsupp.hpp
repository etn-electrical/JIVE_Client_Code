/* testsupp.hpp
 *
 * SafeZone/MatrixSSL specific wrapper for catch.hpp.
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
*****************************************************************************/

#ifndef TESTSUPP_H
#define TESTSUPP_H 1

#ifdef TESTSUPP_MAIN
#define CATCH_CONFIG_MAIN
#endif

/* Classification of test results (for compatibility with testsupp.h). */
typedef enum { OK, FAILED, WEAK, SKIPPED } TEST_RESULT;

/* Assumes catch.hpp 1.8.2. */
#include "thirdparty/catch.hpp"

/* Helper functionality to aid with catch.hpp we have developed. */

/* Converts memory containing digest into std:string 0xdigestvalue. */
#define HEXDIGEST(output_buffer, size)                  \
    Util::rawMemoryToString((const void *)(output_buffer), (size))

/* Converts memory to hexadecimal (any content). */
#define HEX(output_buffer, size)                        \
    Util::rawMemoryToString((const void *)(output_buffer), (size))

/* Converts any content to string. */
#define STRING(output_buffer, size)                             \
    std::string((output_buffer), (output_buffer) + (size))

/* Note: Also available: opt_WRITE_FILE(filename_string, data, size). */

/* ----------------------------------------------------------------------- */

/* Implementation of utility functions. */
/* Note: This is under libboost license because of sources. */

/* Loaned from catch.hpp with different endianness detail: */
namespace Util {

    const std::string unprintableString = "{?}";

    static inline std::string rawMemoryToString(
            const void *object,
            std::size_t size)
    {
        // Reverse order for little endian architectures
        int i = 0, end = static_cast<int>( size ), inc = 1;

        unsigned char const *bytes = static_cast<unsigned char const *>(object);
        std::ostringstream os;
        os << "0x" << std::setfill('0') << std::hex;
        for( ; i != end; i += inc )
             os << std::setw(2) << static_cast<unsigned>(bytes[i]);
       return os.str();
    }
}

bool testsupp_write_debug_files;
static inline
void opt_WRITE_FILE(
        const char *target,
        const void *data,
        size_t data_length)
{
    FILE *f;

    if (!testsupp_write_debug_files && !getenv("TESTSUPP_WRITE_DEBUG_FILES"))
    {
        return; /* Do not produce debugging files. */
    }

    f = fopen(target, "w");
    if (f)
    {
        if (fwrite(data, data_length, 1, f) != 1)
        {
            fprintf(stderr, "write error\n");
            exit(1);
        }
        fprintf(stderr, "(Written %lu data bytes to %s)\n",
                (long unsigned int) data_length, target);
    }
    fclose(f);
}

#endif /* testsupp.hpp */
