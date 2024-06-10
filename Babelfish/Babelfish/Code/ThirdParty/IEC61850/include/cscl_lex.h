/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2005-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/

/*
 *
 *  This file contains
 *  Declarations for CSCL LEX
 *
 *  This file should not require modification.
 */


#ifndef CSCL_LEX_H
#define CSCL_LEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tmwtargtrace.h"

#define YY_READ_BUF_SIZE 2048
typedef struct yy_buffer_state *CNTX_YY_BUFFER_STATE;

typedef enum {
    UTF_8,
    UTF_16BE,
    UTF_16LE,
    UTF_32BE,
    UTF_32LE,
    UTF_None
} BOM_Type;


#define SCL_YY_CHAR unsigned char

/* This is the maximum length of the URL for name space definitions */
#if defined(MAX_ATTRIBUTE_NAME)
#define MAX_NAMESPACE_PREFIX MAX_ATTRIBUTE_NAME
#else
#define MAX_NAMESPACE_PREFIX 100
#endif

/*
    This is the maximum number of name space prefixes that can be defined for 61850 SCL.
    Note that other name spaces are basically ignored, since it is the only schema we know.
*/
#define MAX_61850_NAMESPACE_DEFINITIONS 5

typedef struct SCL_NameSpaceScopePrefix {
    char                pPrefix[ MAX_NAMESPACE_PREFIX ];
    int                 nPrefixLen;
} SCL_NameSpaceScopePrefix;

typedef struct SCL_NameSpaceScope {
    int                         bIsDefaultNameSpace;
    SCL_NameSpaceScopePrefix    pNameSpacePrefix[ MAX_61850_NAMESPACE_DEFINITIONS ];
} SCL_NameSpaceScope;

typedef struct SCL_parser_context_type {

    CNTX_YY_BUFFER_STATE CNTX_yy_current_buffer;

    SCL_NameSpaceScope *CNTX_nameSpaceScopes;
    int                 CNTX_nMaxNameSpaceScopes;
    int                 CNTX_nNameSpaceScope;

    /* yy_hold_char holds the character lost when yytext is formed */
    SCL_YY_CHAR         CNTX_yy_hold_char;

    /* number of characters read into yy_ch_buf */
    int                 CNTX_yy_n_chars;

    char               *CNTX_yytext;
    int                 CNTX_yyleng;
    FILE               *CNTX_yyin;
    FILE               *CNTX_yyout;

    /* points to current character in buffer */
    SCL_YY_CHAR        *CNTX_yy_c_buf_p; /* = (YY_CHAR *) 0; */
    /* whether we need to initialize */
    int                 CNTX_yy_init; /* = 1;         */
    /* start state number */
    int                 CNTX_yy_start; /* = 0;        */

    /* flag which is used to allow yywrap()'s to do buffer switches
     * instead of setting up a fresh yyin.  A bit of a hack ...
     */
    int                 CNTX_yy_did_buffer_switch_on_eof;

    /* Error message indication function */
    TmwTargTraceMessageFunc    err_message_ind;
    void                *err_message_param;

    /* user data */
    int                 source_lineno;
    int                 CNTX_in_unknown_element;
    char                SCL_BOM_Buffer[ YY_READ_BUF_SIZE ];
    int                 SCL_BOM_Buffer_Len;
    int                 SCL_BOM_Buffer_Start;
    BOM_Type            SCL_BOM_Found;
    int                 relaxed;            /* If non-zero, omit checks for required attributes */
} SCL_parser_context_type;

#define in_unknown_element yy_context->CNTX_in_unknown_element
#define is_scl_nameSpace( prefix )  SCL_isSclNameSpace{ prefix, yy_context )

void SCL_init_lex( SCL_parser_context_type *yy_context, FILE *fd );
void SCL_reset_lex( SCL_parser_context_type *yy_context );

/*
    Called from LEX to check if an element name is from a currently defined SCL nameSpace.
    Defined in template.yacc (CSCL.Y).
*/
int SCL_isSclNameSpace( char *pPrefix, int nPrefixLen, SCL_parser_context_type *pParserContext );


#ifdef __cplusplus
};
#endif

#endif /* CSCL_LEX_H */
