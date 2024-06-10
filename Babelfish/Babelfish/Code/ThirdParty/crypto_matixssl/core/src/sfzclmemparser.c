/* sfzclmemparser.c
 *
 * Implementation of simple parser for parsing number and strings from
 * string or memory area.
 */

/*****************************************************************************
* Copyright (c) 2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#include "sfzclincludes.h"
#include "sfzclmemparser.h"
#include "implementation_defs.h"
#include "c_lib.h"

#define SFZCL_DEBUG_MODULE "SfzclMemParser"

void sfzcl_memparser_init_mem(
    SfzclMemParserStruct * const MemParserUninitialized_p,
    const unsigned char *Memory_p,
    size_t Length)
{
    MemParserUninitialized_p->Memory_p = Memory_p;
    MemParserUninitialized_p->Remaining = Length;
    MemParserUninitialized_p->Error = 0;
}

void sfzcl_memparser_init_string(
    SfzclMemParserStruct * const MemParserUninitialized_p,
    const char *String_p)
{
    sfzcl_memparser_init_mem(MemParserUninitialized_p,
        (const unsigned char *) String_p,
        c_strlen(String_p));
}

size_t sfzcl_memparser_remaining_length(
    SfzclMemParserStruct * const MemParser_p)
{
    return MemParser_p->Error ? 0 : MemParser_p->Remaining;
}

bool sfzcl_memparser_is_error(
    SfzclMemParserStruct * const MemParser_p)
{
    return MemParser_p->Error == 1;
}

/* Make alias, for easier reading of code below. */
#define SFZCL_MEMPARSER_NUM_ACCEPT_ZERO \
    SFZCL_MEMPARSER_NUM_ACCEPT_INITIAL_ZERO

unsigned long sfzcl_memparser_read_uinteger(
    SfzclMemParserStruct * const MemParser_p,
    SfzclMemParserOptionsBitmask Options)
{
    size_t min = Options & SFZCL_MEMPARSER_MIN_MASK;
    size_t max = (Options & SFZCL_MEMPARSER_MAX_MASK) / SFZCL_MEMPARSER_MAX_MULT;
    size_t len = 0;
    unsigned long uinteger = 0;

    while (len < max && MemParser_p->Remaining > 0)
    {
        unsigned char uch = *(MemParser_p->Memory_p++);
        MemParser_p->Remaining--;

        if (uch >= '1' && uch <= '9')
        {
            /* Accept single digit. */
            uinteger *= 10;
            uinteger += uch - '0';

            /* Alter options and update length. */
            Options &= ~SFZCL_MEMPARSER_NUM_ACCEPT_INITIAL_BLANK;
            Options |= SFZCL_MEMPARSER_NUM_ACCEPT_ZERO;
            len++;
        }
        else if (uch == '0' &&
                 (Options & SFZCL_MEMPARSER_NUM_ACCEPT_ZERO) != 0)
        {
            uinteger *= 10;

            /* Alter options and update length. */
            Options &= ~SFZCL_MEMPARSER_NUM_ACCEPT_INITIAL_BLANK;
            len++;
        }
        else if (Isspace(uch) &&
                 (Options & SFZCL_MEMPARSER_NUM_ACCEPT_INITIAL_BLANK) != 0)
        {
            len++;
        }
        else
        {
            /* Encountered illegal character. */
            MemParser_p->Memory_p--;
            MemParser_p->Remaining++;
            break;
        }
    }
    ASSERT(len <= max);
    if (len < min)
    {
        MemParser_p->Error = 1;
    }
    return uinteger;
}

size_t sfzcl_memparser_read_string(
    SfzclMemParserStruct * const MemParser_p,
    SfzclMemParserOptionsBitmask Options,
    char *String,
    size_t StringBytes)
{
    size_t min = Options & SFZCL_MEMPARSER_MIN_MASK;
    size_t max = (Options & SFZCL_MEMPARSER_MAX_MASK) / SFZCL_MEMPARSER_MAX_MULT;
    size_t len = 0;

    ASSERT(StringBytes > 0);
    if (max > StringBytes - 1)
    {
        max = StringBytes - 1;
    }

    if (MemParser_p->Error)
    {
        if (StringBytes > 0)
        {
            *String = 0;
        }
        return 0;
    }

    while (len < max && MemParser_p->Remaining > 0)
    {
        unsigned char uch = *(MemParser_p->Memory_p++);
        MemParser_p->Remaining--;

        if (uch != 0)
        {
            if ((Options & SFZCL_MEMPARSER_STR_ACCEPT_SPACE) ||
                !Isspace(uch))
            {
                *(String++) = uch;
                len++;
            }
            else
            {
                /* Character unacceptable. */
                MemParser_p->Memory_p--;
                MemParser_p->Remaining++;
                break;
            }
        }
        else
        {
            /* Character unacceptable. */
            MemParser_p->Memory_p--;
            MemParser_p->Remaining++;
            break;
        }
    }

    /* Terminating zero (not counted to len). */
    *String = 0;

    ASSERT(len <= max);
    if (len < min)
    {
        MemParser_p->Error = 1;
    }
    return len;
}

size_t sfzcl_memparser_skip_span(
    SfzclMemParserStruct * const MemParser_p,
    SfzclMemParserOptionsBitmask Options,
    const char *Span)
{
    size_t min = Options & SFZCL_MEMPARSER_MIN_MASK;
    size_t max = (Options & SFZCL_MEMPARSER_MAX_MASK) / SFZCL_MEMPARSER_MAX_MULT;
    size_t len = 0;

    if (MemParser_p->Error)
    {
        return 0;
    }

    while (len < max && MemParser_p->Remaining > 0)
    {
        unsigned char uch = *(MemParser_p->Memory_p++);
        MemParser_p->Remaining--;

        if (uch != 0 && c_strchr(Span, uch))
        {
            len++;
        }
        else
        {
            /* Character unacceptable. */
            MemParser_p->Memory_p--;
            MemParser_p->Remaining++;
            break;
        }
    }

    ASSERT(len <= max);
    if (len < min)
    {
        MemParser_p->Error = 1;
    }
    return len;
}

bool sfzcl_memparser_accept_string(
    SfzclMemParserStruct * const MemParser_p,
    const char *String_p)
{
    size_t Length = c_strlen(String_p);

    if (MemParser_p->Remaining >= Length)
    {
        if (c_memcmp(MemParser_p->Memory_p, String_p, Length) == 0)
        {
            MemParser_p->Memory_p += Length;
            MemParser_p->Remaining -= Length;
            return true;
        }
    }

    MemParser_p->Error = 1;
    return false;
}

/* end of file sfzclmemparser.c */
