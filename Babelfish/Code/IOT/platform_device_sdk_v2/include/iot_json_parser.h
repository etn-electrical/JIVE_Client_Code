#ifndef IOT_JSON_PARSER_H
#define IOT_JSON_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>

/******************************************************************************/
/*          JSON Parser (JSMN - https://github.com/zserge/jsmn)               */
/******************************************************************************/

/*
   Copyright (c) 2010 Serge A. Zaitsev

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
 */

/**
 * JSON type identifier. Basic types are:
 *  o Object
 *  o Array
 *  o String
 *  o Other primitive: number, boolean (true/false) or null
 */
typedef enum
{
	JSMN_UNDEFINED = 0,
	JSMN_OBJECT = 1,
	JSMN_ARRAY = 2,
	JSMN_STRING = 3,
	JSMN_PRIMITIVE = 4
} jsmntype_t;

enum jsmnerr
{
	/* Not enough tokens were provided */
	JSMN_ERROR_NOMEM = -1,
	/* Invalid character inside JSON string */
	JSMN_ERROR_INVAL = -2,
	/* The string is not a full JSON packet, more bytes expected */
	JSMN_ERROR_PART = -3
};

/**
 * JSON token description.
 * @param		type	type (object, array, string etc.)
 * @param		start	start position in JSON data string
 * @param		end		end position in JSON data string
 */
typedef struct
{
	jsmntype_t type;
	int start;
	int end;
	int size;
#ifdef JSMN_PARENT_LINKS
	int parent;
#endif
} jsmntok_t;

#ifndef PX_GREEN_IOT_DEVICE
/** For PX Red, we will use a dynamically sized array (as long as the compiler permits),
 * where the variable named 'token_array_size' must be given the desired size, eg with
 * 'unsigned int token_array_size = iot_getJsonTokenCount( msg, len );' */
	#define TOKEN_ALLOC_SIZE token_array_size
#endif


/**
 * JSON parser structure.
 * Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct
{
	unsigned int pos;		/**< offset in the JSON string */
	unsigned int toknext;	/**< next token to allocate */
	int toksuper;			/**< superior token node, e.g parent object or array */
} jsmn_parser;

/**
 * Initializes the given JSON parser structure.
 * @param parser Pointer to the JSON parser data structure, which will be initialized.
 */
extern void jsmn_init( jsmn_parser *parser );

/**
 * @brief Initializes an array of tokens structures.
 *
 * @param tokens The array of tokens to be initialized.
 * @param token_count The count of tokens (size) of the tokens[] array.
 */
extern void jsmntok_t_init( jsmntok_t tokens[], unsigned int token_count );

/**
 * Run JSON parser.
 * It parses a JSON data string into an array of tokens, each describing
 * a single JSON object.
 *
 * @param parser The JSON parser instance.
 * @param js The JSON string to be parsed.
 * @param len The length of that JSON string.
 * @param tokens The array of allocated tokens.
 * @param num_tokens The number of token allocated in tokens[]
 * @return The count of JSON tokens parsed from the JSON string.
 */
extern int jsmn_parse( jsmn_parser *parser, const char *js, size_t len,
					   jsmntok_t *tokens, unsigned int num_tokens );

/**
 * Tests if a JSON tag is equal to a search string.
 * @param json The full JSON string being parsed.
 * @param tok The token instance within the JSON string being checked.
 * @param s The search string (the text that must be matched)
 * @return 0 if the search string exactly matches the token's tag, else non-zero.
 */
extern int jsoneq( const char *json, jsmntok_t *tok, const char *s );

/**
 * Allocates and initializes a fresh unused token from the token pull (array),
 * one beyond the last initialized by the parser.
 *
 * @param parser The JSON parser instance.
 * @param tokens The array of allocated tokens.
 * @param num_tokens The number of token allocated in tokens[]
 * @return Pointer to the newly initialized token,
 *          or NULL if there is no more space in tokens[]
 */
extern jsmntok_t *jsmn_alloc_token( jsmn_parser *parser,
									jsmntok_t *tokens, size_t num_tokens );

#ifdef __cplusplus
}
#endif

#endif	/* IOT_JSON_PARSER_H */

