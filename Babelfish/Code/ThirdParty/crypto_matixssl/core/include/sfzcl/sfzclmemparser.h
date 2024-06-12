/* sfzclmemparser.h
 *
 * Description: Parse memory area.
 *
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

#ifndef INCLUDE_GUARD_SFZCLMEMPARSER_H
#define INCLUDE_GUARD_SFZCLMEMPARSER_H

typedef struct SfzclMemParserRec
{
    const unsigned char *Memory_p;
    size_t Remaining;
    unsigned int Error : 1;
} SfzclMemParserStruct;

typedef uint32_t SfzclMemParserOptionsBitmask;
#define SFZCL_MEMPARSER_MIN_MULT                 0x00000001
#define SFZCL_MEMPARSER_MAX_MULT                 0x00000100
#define SFZCL_MEMPARSER_MIN_MAX_MULT             0x00000101
#define SFZCL_MEMPARSER_MIN_MASK                 0x000000ff
#define SFZCL_MEMPARSER_MAX_MASK                 0x0000ff00
#define SFZCL_MEMPARSER_NUM_ACCEPT_INITIAL_BLANK 0x00020000
#define SFZCL_MEMPARSER_NUM_ACCEPT_INITIAL_ZERO  0x00040000
#define SFZCL_MEMPARSER_STR_ACCEPT_SPACE         0x00100000

/* Initialize parser with specified memory area. */
void sfzcl_memparser_init_mem(
    SfzclMemParserStruct * const MemParserUninitialized_p,
    const unsigned char *Memory_p,
    size_t Length);

/* Init memory parsing using specified string. */
void sfzcl_memparser_init_string(
    SfzclMemParserStruct * const MemParserUninitialized_p,
    const char *String_p);

/* Request remaining length. (Returns 0 if the parser is in error state.) */
size_t sfzcl_memparser_remaining_length(
    SfzclMemParserStruct * const MemParser_p);

/* Macro for querying end of string condition without errors */
#define SFZCL_MEMPARSER_IS_END(MemParser_p)                \
    ((!sfzcl_memparser_remaining_length(MemParser_p)) &&   \
     (!sfzcl_memparser_is_error(MemParser_p)))

/* Check if sfzcl_memparser is in error state. */
bool sfzcl_memparser_is_error(
    SfzclMemParserStruct * const MemParser_p);

/* Read uinteger value. In case of parse error, zero is returned and error
   state is set. */
unsigned long sfzcl_memparser_read_uinteger(
    SfzclMemParserStruct * const MemParser_p,
    SfzclMemParserOptionsBitmask Options);

/* Read string: returns number of characters read. */
size_t sfzcl_memparser_read_string(
    SfzclMemParserStruct * const MemParser_p,
    SfzclMemParserOptionsBitmask Options,
    char *String,
    size_t StringBytes);

/* Skip characters from specified string.
   Return number of characters skipped. */
size_t sfzcl_memparser_skip_span(
    SfzclMemParserStruct * const MemParser_p,
    SfzclMemParserOptionsBitmask Options,
    const char *Span);

/* Accept specified string.
   Return true/false, sets error state if false. */
bool sfzcl_memparser_accept_string(
    SfzclMemParserStruct * const MemParser_p,
    const char *String_p);

#define SFZCL_MEMPARSER_SKIP_WHITE(MemParser_p) \
    (void) sfzcl_memparser_skip_span( \
    MemParser_p, \
    (SFZCL_MEMPARSER_MIN_MULT * 0) | \
    (SFZCL_MEMPARSER_MAX_MULT * 255), \
    " \f\n\r\t\v")

#endif /* Include Guard */

/* end of file sfzclmemparser.h */
