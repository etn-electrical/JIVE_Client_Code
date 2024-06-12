/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2001-2015 */
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

/** @interfac.h
 *
 *  This file contains
 *  Declarations for Interfaces for MMS Routines.
 *
 *  This file should not require modification.
 */

#ifndef _INTERFAC_H
#define _INTERFAC_H

#ifdef __cplusplus
extern "C" {
#endif

/** These functions are Defined in the MMS grammar file (mms.bas, mmsfull.bas,
 * or mmscli.bas)
 * Used in both the MMS grammar files and the Presentation grammar files
 */
int MMSd_lex(const int *token_index, LENGTH_TYPE *L, MMSd_descriptor *Y, void *yylval_par, unsigned char *mask);

int MMSd_yylex(MMSd_context *_CNTX, const int *token_index, void *_yylval_par);
#define yylex() MMSd_yylex( _CNTX,  local_index, (void *) &yylval )

void MMSd_yyskip(MMSd_context *_CNTX, MMSd_descriptor *desc);
#define yyskip  MMSd_yyskip

void MMSd_yyerror(char *message);
#define yyerror( m )            MMSd_yyerror( m)

boolean MMSd_MSGerror(MMSd_context *_CNTX, MMSd_descriptor *T, MMSd_descriptor *E);
boolean MMSd_DLSerror(MMSd_context *_CNTX, MMSd_errorFlag flag, MMSd_descriptor *E);

int MMSd_prparse(MMSd_context *_CNTX);
int MMSd_yyparse(MMSd_context *_CNTX);

/** Read and Write Services
 * defined in ioroutine.c, used in the grammar.
 */
int MMSd_READ_serviceRoutine(MMSd_context *_CNTX, boolean b, MMSd_descriptor *t);

int MMSd_WRITE_serviceRoutine(MMSd_context *_CNTX);

/* Information Reports
 * defined in EMITFETCH.C, used in user data handlers
 *
 */
typedef enum {
    mmsdInfoRpt_NameList,
    mmsdInfoRpt_Variables
} mmsdInfoRpt_t;

MMSd_length MMSd_appendInfoRptName(char *s, char *d, MMSd_descriptor *E);
MMSd_length MMSd_appendInfoRptListName(char *s, char *d, MMSd_descriptor *E);

#define MMSd_InfoRptDeleteNames(E)              \
   (E)->tokenLength = 0;

#define MMSd_appendInfoRoom(E, length)             \
   (((*E).tokenLength + length) < (*E).index)

MMSd_length MMSd_emitInformationReport(MMSd_context *Context, mmsdInfoRpt_t fmt, MMSd_descriptor * E);

MMSd_length MMSd_emitContents(MMSd_descriptor *T, MMSd_descriptor *E);

/** Macros for internal ASN.1 Emits
 * Users have a distinct interface layer of macros to do emits.
 *
 * These macros are used internal to the MMSd Server, because
 * there is a wider range of ASN1 tags, etc. which must be
 * utilized.  These all rely on the basic primitives in
 * emitfetch.c.
 */
#define  MMSd_emitNULL( _E_ )                                           \
    MMSd_emitTAG_and_LENGTH(  Null, 0, _E_ )

#define MMSd_emitTAG_and_VALUE( _t_, _v_, _E_ )                         \
    MMSd_emitTAGGEDVALUE( _t_, (TMW_INT64) _v_, _E_ )

#define MMSd_emitBOOLEAN( _b_, _E_ )                                    \
    MMSd_emitTAGGEDVALUE( BOOLEAN, (TMW_INT64) _b_, _E_ )

#define MMSd_emitIMPLICITBOOLEAN( _t_, _v_, _E_ )                       \
    MMSd_emitTAGGEDVALUE( _t_, (TMW_INT64) _v_, _E_ )

#define MMSd_emitINTEGER( _v_, _E_ )                                    \
    MMSd_emitTAGGEDVALUE( INTEGER, (TMW_INT64) _v_, _E_ )

#define MMSd_emitIMPLICITINTEGER( _t_, _v_, _E_ )                       \
    MMSd_emitTAGGEDVALUE( _t_, (TMW_INT64) _v_, _E_ )

#define MMSd_emitLONG( _v_, _E_ )                                       \
    MMSd_emitTAGGEDVALUE( INTEGER, (TMW_INT64) _v_, _E_ )

#define MMSd_emitIMPLICITLONG( _t_, _v_, _E_ )                          \
    MMSd_emitTAGGEDVALUE( _t_, (TMW_INT64) _v_, _E_ )

#define MMSd_emitUNSIGNEDINTEGER( _v_, _E_ )                            \
    MMSd_emitTAGGEDUNSIGNEDVALUE( INTEGER, (unsigned long) _v_, _E_ )

#define MMSd_emitIMPLICITUNSIGNEDINTEGER( _t_, _v_, _E_ )               \
    MMSd_emitTAGGEDUNSIGNEDVALUE( _t_, (unsigned long) _v_, _E_ )

#define MMSd_emitUNSIGNEDLONG( _v_, _E_ )                               \
    MMSd_emitTAGGEDUNSIGNEDVALUE( INTEGER, (unsigned long) _v_, _E_ )

#define MMSd_emitIMPLICITUNSIGNEDLONG( _t_, _v_, _E_ )                  \
    MMSd_emitTAGGEDUNSIGNEDVALUE( _t_, (unsigned long) _v_, _E_ )

#define    MMSd_emitOCTETSTRING( _t_, _b_, _l_, _E_ )                   \
    MMSd_emitTAG_and_LENGTH( _t_,                                       \
                 MMSd_emitOCTETS( (unsigned char *) _b_, _l_, _E_ ),    \
                 _E_ )

#define  MMSd_emitVISIBLESTRING( _t_, _b_, _E_ )                        \
    MMSd_emitTAG_and_LENGTH(  _t_, MMSd_emitSTRING( _b_, _E_ ), _E_ )

/* Macros for internal ASN.1 Fetches -----------------------------
 * Users have a distinct interface layer of macros to do fetches.
 *
 * These macros are used internal to the MMSd Server, because
 * there is a wider range of ASN1 tags, etc. which must be
 * utilized.  These all rely on the basic primitives in
 * emitfetch.c.
 */

#define MMSd_fetchBOOLEAN( _tok_, _val_ )                               \
    {                                                                   \
        int _fetch_;                                                    \
        MMSd_fetchINTEGER( _tok_, &_fetch_, noTag);                     \
        *(_val_) = _fetch_;                                             \
    }

/** NameList processing ---------------------------------------
 * These functions are used when working with services like
 * GetNameList, which processes lists of names sequentially,
 * and which have to anticipate running out of PDU space
 * (moreFollows).  These are handled front to back in the PDU
 * buffer, rather than back to front, as in handlers.
 * The first set are strictly local to the internal MMS services.
 * The last one (appendName) is used in the user Name Handlers.
 */

#ifdef MMSD_FULL_STACK
#define MMSdNameListStart 30  /* Start position for generating name list */
#else
#define MMSdNameListStart 20  /* Start position for generating name list */
#endif

#define initializeNameListDescriptor( _E_ )                       \
   _E_.tokenLength = _E_.index = MMSdNameListStart;

/** CAUTION: Opens curly brace! (closed in doneAppending, below */
#define MMSd_prepareToAppend(_E_, _L_)          \
{                                               \
   MMSd_length saveIndex ,savePos;              \
   saveIndex = (_E_).index;                     \
   (_E_).index = (MMSd_length) ((_E_).tokenLength + _L_); \
   savePos = (_E_).index;

/** CAUTION: Closes a curly brace! (opened in MMSd_prepareToAppend, above */
#define MMSd_doneAppending(_E_)                 \
   (_E_).tokenLength = savePos;                 \
   (_E_).index = saveIndex;                     \
}

#ifdef  __IAR_SYSTEMS_ICC
#pragma warnings=off
#endif
#define MMSd_resetNameListDescriptor( E )
#ifdef  __IAR_SYSTEMS_ICC
#pragma warnings=default
#endif

#define MMSd_appendRoom(_E_, _L_)               \
   ( ( (_E_)->tokenLength + _L_ ) < (_E_)->bufLen )

/** Named Variable List (dataSet) default handlers */
int MMSd_attrNVL(MMSd_context *_CNTX, int size, void *parameter, MMSd_descriptor **pNvlMemberAltAccess, void * E);

#if defined(MMSD_ATTR_HANDLER_NAME)
int MMSd_attrSTRUCT( MMSd_context *_CNTX, void *N, int size, void *parameter, void * E );
#else
int MMSd_attrSTRUCT(MMSd_context *_CNTX, int size, void *parameter, void * E);
#endif

#if defined(MMSD_ATTR_HANDLER_NAME)
int MMSd_attrARRAY( MMSd_context *_CNTX,
                    void *N,
                    int array_size,
                    void *parameter,
                    void *E );
#else
int MMSd_attrARRAY(MMSd_context *_CNTX, int array_size, void *parameter, void *E);
#endif

#if defined(DEBUG_MMSD)
void MMSd_dump_descr(MMSd_descriptor *D);
#endif

#ifdef __cplusplus
}
;
#endif

#endif /* _INTERFAC_H */
