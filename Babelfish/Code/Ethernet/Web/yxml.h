/* Copyright (c) 2013-2014 Yoran Heling

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef YXML_H
#define YXML_H

#include <stdint.h>
#include <stddef.h>
#include "Etn_Types.h"

#if defined(_MSC_VER) && !defined(__cplusplus) && !defined(inline)
#define inline __inline
#endif

/* Full API documentation for this library can be found in the "yxml.pod" file
 * in the yxml git repository, or online at http://dev.yorhel.nl/yxml/man */

//typedef enum {
static const int32_t YXML_EEOF = -5; /* Unexpected EOF                             */
static const int32_t YXML_EREF = -4; /* Invalid character or entity reference (&whatever;) */
static const int32_t YXML_ECLOSE = -3; /* Close tag does not match open tag (<Tag> .. </OtherTag>) */
static const int32_t YXML_ESTACK = -2; /* Stack overflow (too deeply nested tags or too long element/attribute name) */
static const int32_t YXML_ESYN = -1; /* Syntax error (unexpected byte)             */
static const int32_t YXML_OK = 0; /* Character consumed, no new token present   */
static const int32_t YXML_ELEMSTART = 1; /* Start of an element:   '<Tag ..'           */
static const int32_t YXML_CONTENT = 2; /* Element content                            */
static const int32_t YXML_ELEMEND = 3; /* End of an element:     '.. />' or '</Tag>' */
static const int32_t YXML_ATTRSTART = 4; /* Attribute:             'Name=..'           */
static const int32_t YXML_ATTRVAL = 5; /* Attribute value                            */
static const int32_t YXML_ATTREND = 6; /* End of attribute       '.."'               */
static const int32_t YXML_PISTART = 7; /* Start of a processing instruction          */
static const int32_t YXML_PICONTENT = 8; /* Content of a PI                            */
static const int32_t YXML_PIEND = 9; /* End of a processing instruction            */
//} yxml_ret_t;

/* When, exactly, are tokens returned?
 *
 * <TagName
 *   '>' ELEMSTART
 *   '/' ELEMSTART, '>' ELEMEND
 *   ' ' ELEMSTART
 *     '>'
 *     '/', '>' ELEMEND
 *     Attr
 *       '=' ATTRSTART
 *         "X ATTRVAL
 *           'Y'  ATTRVAL
 *             'Z'  ATTRVAL
 *               '"' ATTREND
 *                 '>'
 *                 '/', '>' ELEMEND
 *
 * </TagName
 *   '>' ELEMEND
 */

typedef struct yxml_t_s
{

		/* DDB HACK: Make binary compatible with dynamic_buffer
		 * so server_request_idx appears at 3rd byte. */

		uint16_t nextstate; /* Used for '@' state remembering and for the "string" consuming state */

		/* Added by DDB. Used for identifying block containing server request.*/
		uint8_t server_request_idx;

		uint8_t ignore;

		/* PUBLIC (read-only) */

		/* Name of the current element, zero-length if not in any element. Changed
		 * after YXML_ELEMSTART. The pointer will remain valid up to and including
		 * the next non-YXML_ATTR* token, the pointed-to buffer will remain valid
		 * up to and including the YXML_ELEMCLOSE for the corresponding element. */
		char_t *elem;

		/* The last read character(s) of an attribute value (YXML_ATTRVAL), element
		 * data (YXML_CONTENT), or processing instruction (YXML_PICONTENT). Changed
		 * after one of the respective YXML_ values is returned, and only valid
		 * until the next yxml_parse() call. Usually, this string only consists of
		 * a single byte, but multiple bytes are returned in the following cases:
		 * - "<?SomePI ?x ?>": The two characters "?x"
		 * - "<![CDATA[ ]x ]]>": The two characters "]x"
		 * - "<![CDATA[ ]]x ]]>": The three characters "]]x"
		 * - "&#N;" and "&#xN;", where dec(n) > 127. The referenced Unicode
		 *   character is then encoded in multiple UTF-8 bytes.
		 */
		char_t data[8];

		/* Name of the current attribute. Changed after YXML_ATTRSTART, valid up to
		 * and including the next YXML_ATTREND. */
		char_t *attr;

		/* Name/target of the current processing instruction, zero-length if not in
		 * a PI. Changed after YXML_PISTART, valid up to (but excluding)
		 * the next YXML_PIEND. */
		char_t *pi;

		/* Line number, byte offset within that line, and total bytes read. These
		 * values refer to the position _after_ the last byte given to
		 * yxml_parse(). These are useful for debugging and error reporting. */
		uint16_t byte;
		uint16_t total;

		/* PRIVATE */
		int32_t state;
		uint16_t stacksize, stacklen;
		uint32_t reflen;
		uint32_t quote;
		uint8_t *string;

//#ifndef __cplusplus
		uint8_t stack[1]; /* Stack of element names + attribute/PI name, separated by \0. Also starts with a \0. */
//#endif
} yxml_t;

#define PUT_SINGLE_YXML_STACK_SIZE 32

static const uint8_t YXML_PUT_IS_HEX = 0x01;
static const uint8_t YXML_PUT_IS_NEGATIVE = 0x02;
static const uint8_t YXML_PUT_HAS_LEADING_ZERO = 0x04;
static const uint8_t YXML_PUT_IS_BASE64 = 0x08;
static const uint8_t YXML_PUT_BASE64_PAD = 0x10;
static const uint8_t YXML_PUT_END = 0x20;

struct yxml_put_xml_state
{
		yxml_t yxml;
		uint8_t stack[PUT_SINGLE_YXML_STACK_SIZE];
		uint64_t tmp_number;
		union
		{
				char_t buff[4U];
				uint32_t all;
		} base64;
		uint8_t flags;
		uint8_t put_state;
		uint8_t val_index;
		uint8_t separator;
		uint8_t base64_bits;
		uint8_t num_of_units;
		uint8_t max_num_list;
		uint8_t curr_num_list;
};

#ifdef __cplusplus
extern "C"
{
#endif

void yxml_init( yxml_t *, size_t );

int32_t yxml_parse( yxml_t *, int );

/* May be called after the last character has been given to yxml_parse().
 * Returns YXML_OK if the XML document is valid, YXML_EEOF otherwise.  Using
 * this function isn't really necessary, but can be used to detect documents
 * that don't end correctly. In particular, an error is returned when the XML
 * document did not contain a (complete) root element, or when the document
 * ended while in a comment or processing instruction. */
int32_t yxml_eof( yxml_t * );

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus
/* Returns the length of the element name (x->elem), attribute name (x->attr),
 * or PI name (x->pi). This function should ONLY be used directly after the
 * YXML_ELEMSTART, YXML_ATTRSTART or YXML_PISTART (respectively) tokens have
 * been returned by yxml_parse(), calling this at any other time may not give
 * the correct results. This function should also NOT be used on strings other
 * than x->elem, x->attr or x->pi. */
static inline size_t yxml_symlen(yxml_t *x, const char_t *s)
{
	return (x->stack + x->stacklen) - (const uint8_t*)s;
}
#endif

#endif

/* vim: set noet sw=4 ts=4: */
