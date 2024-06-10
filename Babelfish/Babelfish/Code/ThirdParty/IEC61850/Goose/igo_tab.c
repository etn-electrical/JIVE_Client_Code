/* ------------------------------------------------------
   -- File: GO_TAB.C
   --
   --       Goose Implementation
   --          (c) 1997
   --     Tamarack Consulting
   --
   --     all rights reserved.
   --  THIS WORK MAY NOT BE COPIED FOR ANY REASON
   --  WITHOUT EXPRESSED WRITTEN PERMISSION OF AN
   --  OFFICER OF Tamarack Consulting.
   --
   ------------------------------------------------------

This file contains the following routines:

   --- MMSd_gooseParse

*/
#if (0)
#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93 (Tam6. Reent)";
#endif
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define TMWVERSION 5
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse igoparse
#define yylex igolex
#define yyerror igoerror
#define yychar igochar
#define yyval igoval
#define yylval igolval
#define yydebug igodebug
#define yynerrs igonerrs
#define yyerrflag igoerrflag
#define yyss igoss
#define yyssp igossp
#define yyvs igovs
#define yyvsp igovsp
#define yylhs igolhs
#define yylen igolen
#define yydefred igodefred
#define yydgoto igodgoto
#define yysindex igosindex
#define yyrindex igorindex
#define yygindex igogindex
#define yytable igotable
#define yycheck igocheck
#define yyname igoname
#define yyrule igorule
#define yydeb_file igodeb_file
#define yydeb_out igodeb_out
#define yydeb_name "igodeb.out"
#define yydeb_env "IGODEBUG"
#define YYPREFIX "igo"
/* ************************************************************************ */
/*     Module: IECGOOSE.BAS                                                 */
/*     Comments:                                                            */
/*         IEC Goose decoder grammer for IEC61850-7-2 peer-to-peer          */
/*         communications. (A YACC grammer for Goose messaging)             */
/*                                                                          */
/*           Copyright Tamarack Consulting, Inc. (c) 1994, 1999, 2002       */
/*                     all rights reserved.                                 */
/*             THIS WORK MAY NOT BE COPIED FOR ANY REASON                   */
/*             WITHOUT EXPRESSED WRITTEN PERMISSION OF AN                   */
/*                 OFFICER OF Tamarack Consulting.                          */
/*                                                                          */
/* ************************************************************************ */
/* Routines included:                                                       */
/*                                                                          */
/*  _IEC_gooseParse()                       - Grammer form of parse code    */
/*    IecGoosePdu                           -                               */
/*      GSEMngtPdu                     -                               */
/*        GSEEventRequestPdu                         -                      */
/*          GetReferenceRequest                      -                      */
/*          GetGSEElementNumberRequest                    -                 */
/*        GSEEventResponsePdu                         -                     */
/*          GetReferenceResponse                      -                     */
/*          GetGSEElementNumberResponse                    -                */
/*            resultOpt                     - Defaults to 0 if not present  */
/*            morefollowsOpt                - Defaults to 0 if not present  */
/*      IECGoosePdu                        -                               */
/*        listofDataOpt                     -                               */
/*        timeStamp                         -                               */
/*          TimeQuality                     -                               */
/*            faultClockFailureOpt          - Defaults to 0 if not present  */
/*            clockNotSynchronizedOpt       - Defaults to 0 if not present  */
/*          LeapSecondPendingOpt            - Defaults to 0 if not present  */
/*          LeapSecondOccuredOpt            - Defaults to 0 if not present  */
/*          LeapSecondNegativeOpt           - Defaults to 0 if not present  */
/*          DaylightSavingsPendingOpt       - Defaults to 0 if not present  */
/*          DaylightSavingsTimeOpt          - Defaults to 0 if not present  */
/*        goIDOpt                          - If goID missing set err flag */
/*        testOpt                       - Defaults to 0 if not present  */
/*        ndsComOpt             - Defaults to 0 if not present  */
/*  int         MMSd_MdyToUtc()             -                               */
/*  int         MMSd_UtcToMdy()             -                               */
/*  int         MMSd_MmsToUtc()             -                               */
/*  int         MMSd_UtcToMms()             -                               */
/*  int         MMSd_fetchUTCBTIME()        -                               */
/*  MMSd_length MMSd_emitUTCBTIME()         -                               */
/*                                                                          */
/* ************************************************************************ */

   /* **************************************************************
                             Definitions
   ************************************************************** */
#define moduleGRAMMAR     1
#define moduleName_String "Goose"
#define YYSTACKSIZE       50    /* Depth of Parse and Translation Stacks    */
#define YYDEBUG           0     /* Non-zero value enables parser debugging  */

   /* **************************************************************
        Undefine yyparse, yylex and yyerror if any are defined
   ************************************************************** */
#ifdef yyparse
#undef yyparse
#endif
#ifdef yylex
#undef yylex
#endif
#ifdef yyerror
#undef yyerror
#endif


#include <stdlib.h>
#include <string.h>

#include "sys.h" 

#include "tmwtarg.h"
#if !defined(USE_MMSD_IN_IEC_GOOSE)

#include "ber.h"
#include "mms_erro.h"
#else
#include "ACSI.h"
#include "minicntx.h"
#endif
#include "gram.h"     /* Macros for manipulating Translation Stack Elements */
#include "iecgoose.h"
#include "igotags.h"

/* V8.9 - Decouple IEC_Goose and MMS parsers */
#if !defined(MMSd_getDescOctet)
#define MMSd_getDescOctet( _F_ )    *(MMSd_getDescPtr(_F_))
#define MMSd_fetchDescOctet( _F_ )  *((_F_).bufPtr + (_F_).index++)
#define MMSd_skipToken(_D_)         (_D_).index = (MMSd_length) \
                                              ((_D_).index + (_D_).tokenLength)
#define MMSd_makeToken(_D_,_E_)     { MMSd_copyDescriptor(_D_, _E_); }
#endif


const IEC_PrimitiveType IEC_Goose_ResponsePrimitive[] = {
    Unknown_Mgmt_Req,                      /* Unknown_Mgmt_Req       */
    GSE_Elements_Res,       /* GSE_Elements_Req       */
    GOOSE_Elements_Res,     /* GOOSE_Elements_Req     */
    Unknown_Mgmt_Req,                      /* GSE_Elements_Res       */
    Unknown_Mgmt_Req,                      /* GOOSE_Elements_Res     */
    GSE_References_Res,     /* GSE_References_Req     */
    GOOSE_References_Res,   /* GOOSE_References_Req   */
    Unknown_Mgmt_Req,                      /* GSE_References_Res     */
    Unknown_Mgmt_Req,                      /* GOOSE_References_Res   */
    Unknown_Mgmt_Req,                      /* GOOSE_Unsupported_Res  */
};

const unsigned char IEC_Goose_ResponseTag[] = {
    0,                      /* Unknown_Mgmt_Req       */
    getGSSEsDataOffset4,    /* GSE_Elements_Req       */
    getGOOSEElementNumber2, /* GOOSE_Elements_Req     */
    getGSSEsDataOffset4,    /* GSE_Elements_Res       */
    getGOOSEElementNumber2, /* GOOSE_Elements_Res     */
    getGsReference3,        /* GSE_References_Req     */
    getGoReference1,        /* GOOSE_References_Req   */
    getGsReference3,        /* GSE_References_Res     */
    getGoReference1,        /* GOOSE_References_Res   */
    0,                      /* GOOSE_Unsupported_Res  */
};

#define MMSd_fetchBOOLEAN( _tok_, _val_ )                               \
    {                                                                   \
        int _fetch_;                                                    \
        MMSd_fetchINTEGER( _tok_, &_fetch_, noTag);                     \
        *(_val_) = _fetch_;                                             \
    }


#if defined(yylex)
#undef yylex
#endif


int  IEC_Goose_yylex( const int *token_index, LENGTH_TYPE *L, MMSd_descriptor *D, void *yylval_par );
void IEC_Goose_yyskip( MMSd_descriptor *tok, MMSd_descriptor *desc, LENGTH_TYPE *L );
#define yyerror( m )            
#define yyskip                  IEC_Goose_yyskip
#define yylex()                 IEC_Goose_yylex( local_index, &length_nesting, &descriptor, &yylval )


/* Taken from MMSd/context.h */
#define MMSd_noError(_D_)        ( !(_D_).errorFlag )



#define NO_REPLY { MMSd_emptyDescriptor( descriptor, descriptor );} 


#ifdef yyparse
#undef yyparse
#endif
#define yyparse()                                       \
    _IEC_gooseParse( IEC_Goose_Context  *cntxt,         \
                     TMW_Mac_Address    *srcAddr,       \
                     TMW_LanHeader      *lanAddress,    \
                     unsigned char      *bufferPtr,     \
                     int                length,         \
                     IEC_GooseHeader    *passedHeader,  \
                     unsigned char      **dataPtr,      \
                     MMSd_length        *dataLen )


#define YYUSERDECLS             \
    IEC_GooseHeader     header; \
    TMW_Packet          packet; \
    unsigned long       stateId = 0; \
    MMSd_descriptor     securityToken = { NULL, 0, 0, 0, 0, 0, 0 }; \
    MMSd_descriptor     requestDataToken = { NULL, 0, 0, 0, 0, 0, 0 }; \
    IEC_PrimitiveType   primitive; \
    char                goID[ MMSD_OBJECT_NAME_LEN ]; \
    char                dataSetName[ MMSD_OBJECT_NAME_LEN ]; \
    long                confRev; \
    int                 responseErrorCode = 0; \
    LENGTH_TYPE         length_nesting; \
    MMSd_descriptor     descriptor;


#define YY_SKIP_ERROR_COUNT


#if YYDEBUG
#if defined(TMWVERSION)
#define YYDEB_OUT( msg ) yydeb_out( yydeb_file, msg )
#else
#define YYDEB_OUT( msg ) yydeb_out( msg )
#endif
#define YYUSERINIT                                                       \
    if (yydebug)                                                         \
    {                                                                    \
        int i;                                                           \
        char buffer[120];                                                \
        YYDEB_OUT("Begining parse of pdu:\n");                           \
        for (i = 0; i < length; i++)                                     \
        {                                                                \
            if ( ((i % 16) == 0) && (i != 0) )                           \
            {                                                            \
                YYDEB_OUT( buffer );                                     \
                YYDEB_OUT("\n") ;                                        \
                buffer[0] = 0;                                           \
            }                                                            \
            sprintf(&buffer[(i % 16) * 3], " %02x", bufferPtr[i]);       \
        }                                                                \
        if (strlen( buffer ) > 0)                                        \
            YYDEB_OUT( buffer );                                         \
        YYDEB_OUT("\n");                                                 \
    }                                                                    \
    primitive = Unknown_Mgmt_Req;                                        \
    MMSd_setDescErrorFlag( MMSd_null_descriptor, securityToken );        \
    goID[ 0 ] = 0;                                                       \
    confRev = 0;                                                         \
    MMSd_makeDescriptor(descriptor, bufferPtr, (MMSd_length) length, 0); \
    length_nesting.new_PDU = TRUE;                                       \
    length_nesting.top = 0;                                              \
    length_nesting.end_pos[ 0 ] = (MMSd_length) length;
#else
#define YYUSERINIT                                                       \
    primitive = Unknown_Mgmt_Req;                                        \
    MMSd_setDescErrorFlag( MMSd_null_descriptor, securityToken );        \
    goID[ 0 ] = 0;                                                       \
    confRev = 0;                                                         \
    MMSd_makeDescriptor(descriptor, bufferPtr, (MMSd_length) length, 0); \
    length_nesting.new_PDU = TRUE;                                       \
    length_nesting.top = 0;                                              \
    length_nesting.end_pos[ 0 ] = (MMSd_length) length;


#endif

#define local_index igo_index
const int igo_index[] = {
2,
20,
38,
38,
39,
55,
67,
67,
10000
};
typedef union{                          /* The attribute stack declaration         */
    int     _length;             /* Sub-Message Lengths                     */
    int     _value;              /* Passing values up the translation stack */
    MMSd_InvokeIdType _ID;       /* For passing Invoke IDs up the stack     */
    MMSd_descriptor  TKN;        /* Non-decoded Terminal Symbols            */
} YYSTYPE;
#define YYERRCODE 68
const static signed char igolhs[] = { -1,
    0,    0,    1,    1,    2,    3,    3,    4,    4,    4,
    4,    5,    5,    5,    5,    5,    6,   18,   18,    7,
   19,   19,    8,    8,    9,    9,   10,   10,   11,   11,
   12,   12,   12,   20,   17,   13,   13,   14,   14,   15,
   15,   16,   16,
};
const static unsigned char igolen[] = { 2,
    3,    3,    4,    4,    2,    1,    0,    3,    3,    3,
    3,    1,    3,    3,    3,    3,    4,    2,    1,    4,
    2,    1,    7,    3,    1,    0,    1,    0,    2,    1,
    1,    1,    1,    0,   15,    1,    0,    1,    0,    1,
    0,    1,    0,
};
const static unsigned char igodefred[] = { 0,
    0,    0,    0,    0,    0,    0,    0,    0,    6,    5,
    1,    0,    0,    0,    2,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   12,    0,    0,    0,    0,    0,
    0,    0,    0,    3,    0,    0,    0,    0,    0,    4,
   36,    0,    0,    9,    0,    8,   10,   11,   25,    0,
   14,   13,   15,   16,    0,   22,    0,   19,    0,   24,
    0,    0,   20,   21,   17,   18,   27,    0,    0,    0,
   38,    0,   33,   31,   32,    0,   30,    0,   23,   29,
   40,    0,    0,   34,    0,   42,    0,   35,
};
const static unsigned char igodgoto[] = { 3,
    5,    6,   10,   20,   26,   31,   29,   36,   50,   68,
   76,   77,   42,   72,   82,   87,    8,   59,   57,   85,
};
const static signed char igosindex[] = { -29,
  -32,   -4,    0,  -42,   24,  -52,   -3,   26,    0,    0,
    0,  -44,  -38,   -2,    0,    1,    2,    2,    1,   30,
    4,    4,    4,    4,    0,   32,    3,  -10,   35,   -8,
   37,   38,   39,    0,   12,   41,   42,   43,   44,    0,
    0,   14,   31,    0,   54,    0,    0,    0,    0,  -40,
    0,    0,    0,    0,   16,    0,   -6,    0,   -1,    0,
   22,   17,    0,    0,    0,    0,    0,    7,   18,  -15,
    0,   19,    0,    0,    0,  -12,    0,   20,    0,    0,
    0,   21,    5,    0,    6,    0,   53,    0,
};
const static signed char igorindex[] = { 0,
    0,    0,    0,  -23,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   29,    0,    0,    0,
    0,    0,    0,    0,  -34,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   11,    0,    0,    0,    0,    0,    0,    0,   27,    0,
    0,    0,    0,    0,    0,    0,    0,   28,    0,    0,
    0,    0,    0,    0,   57,    0,    0,    0,
};
const static unsigned char igogindex[] = { 0,
    0,    0,    0,    0,    0,   58,   56,    8,    0,    0,
    0,    9,    0,    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 85
const static unsigned char igotable[] = { 79,
   25,   60,   66,   12,   13,   63,    4,   26,    1,    2,
   65,   17,   16,   18,   19,    9,   61,   22,   21,   23,
   24,   64,   26,   74,   75,   73,   74,   75,   73,   37,
   38,   39,    7,    7,    7,   11,   14,   15,   27,   28,
   30,   34,   35,   40,   41,   43,   44,   45,   46,   47,
   48,   49,   51,   52,   53,   54,   55,   58,   56,   62,
   67,   69,   70,   71,   88,   78,   28,   81,   43,   83,
   84,   37,   86,   39,   33,   32,   41,    0,    0,    0,
    0,    0,    0,    0,   80,
};
const static signed char igocheck[] = { 12,
   39,   42,    4,   56,   57,   12,   39,   42,   38,   39,
   12,   56,   57,   58,   59,   58,   57,   56,   57,   58,
   59,   28,   57,   39,   40,   41,   39,   40,   41,   22,
   23,   24,   56,   57,   39,   12,   40,   12,   41,   39,
   39,   12,   39,   12,   42,   56,   12,   56,   12,   12,
   12,   40,   12,   12,   12,   12,   43,    4,   28,   44,
   39,   45,   56,   46,   12,   47,   56,   48,   12,   49,
   66,   43,   67,   47,   19,   18,   49,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   76,
};
#define YYFINAL 3
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 67
#if YYDEBUG
char *igoname[] = {
"end-of-file",0,"END_OF_CONTENTS",0,"INTEGER","BITSTRING","OCTETSTRING","Null",
"OBJECT_IDENTIFIER","OBJECT_DESCRIPTOR",0,0,"CLOSE",0,0,0,0,0,0,0,
"NumericString18","PrintableString19","TeletexString20","VideotexString21",
"IA5String22","UTCTime23","GeneralizedTime24","GraphicString25","EXTERNAL",0,0,
0,0,0,0,0,"sequenceof16","setof17","extra18","dataSet0","confRev1","datSet2",
"goID3","t4","stNum5","sqNum6","test7","confRev8","ndsCom9",
"numDatSetEntries10",0,0,0,0,0,0,"getGoReference1","getGOOSEElementNumber2",
"getGsReference3","getGSSEsDataOffset4",0,0,0,0,0,0,"allData11","security12",
};
char *igorule[] = {
"$accept : GSEPDU",
"GSEPDU : GSEMngtPDU0 GSEMngtPdu CLOSE",
"GSEPDU : IECGoosePDU1 IECGoosePdu CLOSE",
"GSEMngtPdu : GSEMngtPduHeader requests1 GSEMngtRequests CLOSE",
"GSEMngtPdu : GSEMngtPduHeader responses2 GSEMngtResponses CLOSE",
"GSEMngtPduHeader : stateID0 securityMgmtOpt",
"securityMgmtOpt : security3",
"securityMgmtOpt :",
"GSEMngtRequests : getGoReference1 GetReferenceRequestPdu CLOSE",
"GSEMngtRequests : getGOOSEElementNumber2 GetElementRequestPdu CLOSE",
"GSEMngtRequests : getGsReference3 GetReferenceRequestPdu CLOSE",
"GSEMngtRequests : getGSSEsDataOffset4 GetElementRequestPdu CLOSE",
"GSEMngtResponses : gseMngtNotSupported0",
"GSEMngtResponses : getGoReference1 GSEMngtResponsePdu CLOSE",
"GSEMngtResponses : getGOOSEElementNumber2 GSEMngtResponsePdu CLOSE",
"GSEMngtResponses : getGsReference3 GSEMngtResponsePdu CLOSE",
"GSEMngtResponses : getGSSEsDataOffset4 GSEMngtResponsePdu CLOSE",
"GetReferenceRequestPdu : ident0 offset1 SeqOfInteger CLOSE",
"SeqOfInteger : SeqOfInteger INTEGER",
"SeqOfInteger : INTEGER",
"GetElementRequestPdu : ident0 references1 SeqOfVisibleString CLOSE",
"SeqOfVisibleString : SeqOfVisibleString VisibleString26",
"SeqOfVisibleString : VisibleString26",
"GSEMngtResponsePdu : ident0 confRevOpt responsePositive2 dataSetOpt result1 seqOfResults CLOSE",
"GSEMngtResponsePdu : ident0 confRevOpt responseNegative3",
"confRevOpt : confRev1",
"confRevOpt :",
"dataSetOpt : dataSet0",
"dataSetOpt :",
"seqOfResults : seqOfResults RequestResults",
"seqOfResults : RequestResults",
"RequestResults : offset0",
"RequestResults : reference1",
"RequestResults : error2",
"$$1 :",
"IECGoosePdu : gocbRef0 timeAllowedtoLive1 datSet2 goIDOpt t4 stNum5 sqNum6 testOpt confRev8 ndsComOpt numDatSetEntries10 allData11 $$1 securityOpt CLOSE",
"goIDOpt : goID3",
"goIDOpt :",
"testOpt : test7",
"testOpt :",
"ndsComOpt : ndsCom9",
"ndsComOpt :",
"securityOpt : security12",
"securityOpt :",
};
#endif
#ifdef YYSTACKSIZE
#ifdef YYMAXDEPTH
#undef YYMAXDEPTH
#endif
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
#if defined(YY_USE_LEX)
YYSTYPE yylval;
#endif
#define yystacksize YYSTACKSIZE
#ifndef YYUSERDECLS
#define YYUSERDECLS
#endif
#ifndef YYUSERINIT
#define YYUSERINIT
#endif
#ifndef YYUSERABORT
#define YYUSERABORT return(1);
#endif
#ifndef YYUSERACCEPT
#define YYUSERACCEPT return(0);
#endif
#ifndef YY_SKIP_BREAK
#define YY_SKIP_BREAK
#endif

int IEC_Goose_yylex( const int        *token_index,
                   LENGTH_TYPE      *L,
                   MMSd_descriptor  *Y,
                   void             *yylval_par )
{
    YYSTYPE *yylval = (YYSTYPE *) yylval_par;
        /* -- Do the length check.
        */

    if (L->top < 0)
    {
        return( 0 );
    }

    if (MMSd_DescIndex( *Y ) >= L->end_pos[ L->top ])
    {
        if (MMSd_DescIndex( *Y ) == L->end_pos[ L->top ])
        {
            L->top -= 1;
            return( CLOSE + token_index[ 0 ] );
        }
        MMSd_setDescErrorFlag( MMSd_fetchLengthError, *Y );
        return( 0 );
    }
    else
    {
        MMSd_tag    T;
        MMSd_ushort G;

        MMSd_fetchTAG_and_LENGTH( Y );
        MMSd_makeToken(*Y, yylval->IncomingTOKEN);

            /* -- Have we found the end of an Indefinite Length
            */

        if ((MMSd_DescTag( *Y ) == 0) && (MMSd_TokenLen( *Y ) == 0))
        {
            if (L->end_pos[ L->top ] == INDEFINITE_LENGTH)
            {
                L->top -= 1;

                return( CLOSE + token_index[ 0 ] );
            }
        }
            /* -- Process Tag */

        if (yylval->IncomingTOKEN.tag < 0xFF)
        {
                /* -- We have a flag which is less than 255.  This
                */
            G = (MMSd_ushort) (yylval->IncomingTOKEN.tag >> 5);
            T = (MMSd_tag) (yylval->IncomingTOKEN.tag & 0x1F);
        }
        else    /* -- This does not handle TAGS > 127!!! */
        {
                /* -- The FLAG/TAG bits are organized as follows:
                */
            G = (MMSd_ushort) (yylval->IncomingTOKEN.tag >> 13);
            T = (MMSd_tag) (yylval->IncomingTOKEN.tag & 0x7F);
        }



        if ( !( G & 0x01 ) ) /* Primitive */
        {
            yylval->IncomingTOKEN.primitive = TRUE;   /* V7.991 - avoid true/false conflict with C++ */
            MMSd_skipToken(*Y);
        }
        else if (!L->new_PDU)
        {
            yylval->IncomingTOKEN.primitive = FALSE;   /* V7.991 - avoid true/false conflict with C++ */

                /* -- We have a constructor
                */

            L->top += 1;

            if ( L->top > MAX_LENGTH_DEPTH )
            {
                MMSd_setDescErrorFlag( MMSd_fetchLengthError, *Y );
                return(0);
            }

            if (MMSd_TokenLen( *Y ) == INDEFINITE_LENGTH)
            {
                L->end_pos[ L->top ] = INDEFINITE_LENGTH;
            }
            else
            {
                L->end_pos[ L->top ]
                    =   (MMSd_length) (MMSd_TokenLen( *Y ) + MMSd_DescIndex( *Y ));
            }
        }
        else
        {
            yylval->IncomingTOKEN.primitive = FALSE;   /* V7.991 - avoid true/false conflict with C++ */
            L->new_PDU = FALSE;   /* V7.991 - avoid true/false conflict with C++ */
            /* Added by GS - 5/31 */
            if (MMSd_TokenLen( *Y ) == INDEFINITE_LENGTH)
            {
                L->end_pos[ L->top ] = INDEFINITE_LENGTH;
            }
            /* End GS addition */
        }

        return( token_index[ G ] + T );
    }
}

void IEC_Goose_yyskip( MMSd_descriptor *tok, MMSd_descriptor *desc, LENGTH_TYPE *L )
{
    if (!(tok->primitive))
    {
        MMSd_DescIndex( *desc ) = L->end_pos[ L->top ];

        L->top -= 1;
    }
}
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#if YYDEBUG
#include <stdio.h>
#include <stdlib.h>
void yydeb_out( FILE *fd, char *deb_buff );
#endif

int
yyparse()
{
    register int yym, yyn, yystate;
#if !defined(YY_SKIP_ERROR_COUNT)
    int yynerrs;
#endif
    int yyerrflag;
    int yychar;
    short *yyssp;
    YYSTYPE *yyvsp;
    YYSTYPE yyval;
    YYUSERDECLS
    short yyss[YYSTACKSIZE];
    YYSTYPE yyvs[YYSTACKSIZE];
#if !defined(YY_USE_LEX)
    YYSTYPE yylval = { 0 };
#endif
#if YYDEBUG
    int yydebug = 0;
    FILE *yydeb_file = NULL;
    register char *yys;
    extern char *getenv();
    char deb_buff[300];

    if ((yys = getenv(yydeb_env)) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
        if (yydebug)
           yydeb_file = fopen(yydeb_name, "wt");
    }
#endif

    YYUSERINIT

#if !defined(YY_SKIP_ERROR_COUNT)
    yynerrs = 0;
#endif
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = 0;
    yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            sprintf(deb_buff, "%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
            yydeb_out( yydeb_file, deb_buff );
        }
#endif
    }
    if (   ((yyn = yysindex[yystate]) != 0)
           && ((yyn += yychar) >= 0)
           && (yyn <= YYTABLESIZE)
           && (yycheck[yyn] == yychar) )
    {
#if YYDEBUG
        if (yydebug)
        {
            sprintf(deb_buff, "%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
            yydeb_out( yydeb_file, deb_buff );
        }
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yyssp = (short) yystate;
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (   ((yyn = yyrindex[yystate]) != 0)
           && ((yyn += yychar) >= 0)
           && (yyn <= YYTABLESIZE)
           && (yycheck[yyn] == yychar))
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
yynewerror:
#endif
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
yyerrlab:
#endif
#if !defined(YY_SKIP_ERROR_COUNT)
    ++yynerrs;
#endif
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (   ((yyn = yysindex[*yyssp]) != 0)
                   && ((yyn += YYERRCODE) >= 0)
                   && (yyn <= YYTABLESIZE)
                   && (yycheck[yyn] == YYERRCODE))
            {
#if YYDEBUG
                if (yydebug)
                {
                    sprintf(deb_buff, "%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
                    yydeb_out( yydeb_file, deb_buff );
                }
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yyssp = (short) yystate;
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                {
                    sprintf(deb_buff, "%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
                    yydeb_out( yydeb_file, deb_buff );
                }
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            sprintf(deb_buff, "%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
            yydeb_out( yydeb_file, deb_buff );
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
    {
        sprintf(deb_buff, "%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
        yydeb_out( yydeb_file, deb_buff );
    }
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
{ 
            if (yyvsp[-1].OutgoingLENGTH > 0)
            {
                if (!TMW_DL_send( &packet, srcAddr, cntxt->link_context))
                {
#if defined(DEBUG_IEC_GOOSE)
                    printf("Send of response failed!\n");
#endif
                }
            }
            NO_REPLY;
        }
break;
case 2:
{ NO_REPLY; }
break;
case 3:
{
            unsigned char   *pDataPtr;
            MMSd_length     nDataLen;
            unsigned char   *pSecurityPtr;
            MMSd_length     nSecurityLen;
            long            confRev = 0;
            int             errCode = -1;
            MMSd_length     ret = 0;

            if (!TMW_alloc_packet( &packet,
                                   cntxt->link_context->buffers,
                                   TMW_POOL_SEND ))
                YYABORT;

            if (MMSd_noError( requestDataToken ))
            {
                pDataPtr = MMSd_getDescPtr( requestDataToken );
                nDataLen = MMSd_TokenLen( requestDataToken );
            }
            else
            {
                pDataPtr = NULL;
                nDataLen = 0;
            }

            if (MMSd_noError( securityToken ))
            {
                pSecurityPtr = MMSd_getDescPtr( securityToken );
                nSecurityLen = MMSd_TokenLen( securityToken );
            }
            else
            {
                pSecurityPtr = NULL;
                nSecurityLen = 0;
            }

            dataSetName[0] = 0;
            switch ( primitive )
            {
            case GSE_References_Req:
            case GOOSE_References_Req:
                if (cntxt->getReferenceReqFcn!= NULL)
                {
#if defined(USE_MMSD_IN_IEC_GOOSE)
                    MMSd_mini_context   mmsContext;
                    mmsContext.directory = cntxt->directory;
                    mmsContext.directorySize = cntxt->directorySize;
                    ret = (MMSd_length) cntxt->getReferenceReqFcn( cntxt,
                                                    (void *) &mmsContext,
                                                    cntxt->getReferenceReqParam,
                                                    goID,
                                                    pDataPtr,
                                                    nDataLen,
                                                    pSecurityPtr,
                                                    nSecurityLen,
                                                    dataSetName,
                                                    &packet,
                                                    &confRev,
                                                    &errCode );
#else
                    ret = (MMSd_length) cntxt->getReferenceReqFcn( cntxt,
                                                    (void *) NULL,
                                                    cntxt->getReferenceReqParam,
                                                    goID,
                                                    pDataPtr,
                                                    nDataLen,
                                                    pSecurityPtr,
                                                    nSecurityLen,
                                                    dataSetName,
                                                    &packet,
                                                    &confRev,
                                                    &errCode );
#endif
                }
                else
                {
                }
                break;
            case GSE_Elements_Req:
            case GOOSE_Elements_Req:
                if (cntxt->getElementNumReqFcn!= NULL)
                {
#if defined(USE_MMSD_IN_IEC_GOOSE)
                    MMSd_mini_context   mmsContext;
                    mmsContext.directory = cntxt->directory;
                    mmsContext.directorySize = cntxt->directorySize;

                    ret = (MMSd_length) cntxt->getElementNumReqFcn( cntxt,
                                                    (void *) &mmsContext,
                                                    cntxt->getElementNumReqParam,
                                                    goID,
                                                    pDataPtr,
                                                    nDataLen,
                                                    pSecurityPtr,
                                                    nSecurityLen,
                                                    dataSetName,
                                                    &packet,
                                                    &confRev,
                                                    &errCode );
#else
                    ret = (MMSd_length) cntxt->getElementNumReqFcn( cntxt,
                                                    (void *) NULL,
                                                    cntxt->getElementNumReqParam,
                                                    goID,
                                                    pDataPtr,
                                                    nDataLen,
                                                    pSecurityPtr,
                                                    nSecurityLen,
                                                    dataSetName,
                                                    &packet,
                                                    &confRev,
                                                    &errCode );
#endif
                }
                else
                {
                }
                break;
            case Unknown_Mgmt_Req:
            default:
                ret = 0;
                break;
            }

            if ( (!MMSd_noError( descriptor )) || descriptor.index < 12)
                YYABORT;


            if ( IEC_Goose_ResponsePrimitive[ primitive ] != 0 )
            {
                ret = GSE_Encode_Request(IEC_Goose_ResponsePrimitive[primitive],
                                         goID,
                                         dataSetName,
                                         confRev,
                                         stateId,
                                         errCode,
                                         (MMSd_length) packet.data_length,
                                         &packet );
            }

            if ( !TMW_encode_VLAN_header( &packet, lanAddress ) )
                YYABORT;

            packet.media = TMW_VLAN;   /* v7.26 VLAN support */
            yyval.OutgoingLENGTH = ret;
        }
break;
case 4:
{
            unsigned char   *pDataPtr;
            MMSd_length     nDataLen;
            unsigned char   *pSecurityPtr;
            MMSd_length     nSecurityLen;

            if (MMSd_noError( requestDataToken ))
            {
                pDataPtr = MMSd_getDescPtr( requestDataToken );
                nDataLen = MMSd_TokenLen( requestDataToken );
            }
            else
            {
                pDataPtr = NULL;
                nDataLen = 0;
            }

            if (MMSd_noError( securityToken ))
            {
                pSecurityPtr = MMSd_getDescPtr( securityToken );
                nSecurityLen = MMSd_TokenLen( securityToken );
            }
            else
            {
                pSecurityPtr = NULL;
                nSecurityLen = 0;
            }

            switch ( primitive )
            {
            case GSE_Elements_Res:
            case GOOSE_Elements_Res:
                if ( cntxt->getElementNumResFcn != NULL )
                    cntxt->getElementNumResFcn( cntxt->getElementNumResParam,
                                                pDataPtr,
                                                nDataLen,
                                                pSecurityPtr,
                                                nSecurityLen,
                                                stateId,
                                                goID,
                                                dataSetName,
                                                confRev,
                                                responseErrorCode );
                NO_REPLY;
                break;
            case GSE_References_Res:
            case GOOSE_References_Res:
                /* V7.992 - bug fix - was getElementNumResParam */
                if ( cntxt->getReferenceResFcn != NULL )
                    cntxt->getReferenceResFcn( cntxt->getReferenceResParam,
                                               pDataPtr,
                                               nDataLen,
                                               pSecurityPtr,
                                               nSecurityLen,
                                               stateId,
                                               goID,
                                               dataSetName,
                                               confRev,
                                               responseErrorCode );
                NO_REPLY;
                break;
            case GOOSE_Unsupported_Res:
                if ( cntxt->notSupportedResFcn != NULL )
                    cntxt->notSupportedResFcn( cntxt->notSupportedResParam, 
                                               stateId, 
                                               pSecurityPtr,
                                               nSecurityLen );
                NO_REPLY;
                break;
            case Unknown_Mgmt_Req:
            default:
                break;
            }
            yyval.OutgoingLENGTH = 0;
        }
break;
case 5:
{
            if ( !MMSd_fetchUNSIGNEDLONG( &(yyvsp[-1].IncomingTOKEN), &stateId, stateID0 ) )
                YYABORT;
            securityToken = yyvsp[0].IncomingTOKEN;
        }
break;
case 6:
{ yyval.IncomingTOKEN = yyvsp[0].IncomingTOKEN; }
break;
case 7:
{ MMSd_setDescErrorFlag(MMSd_null_descriptor, yyval.IncomingTOKEN); }
break;
case 8:
{ primitive = GOOSE_References_Req; }
break;
case 9:
{ primitive = GOOSE_Elements_Req; }
break;
case 10:
{ primitive = GSE_References_Req; }
break;
case 11:
{ primitive = GSE_Elements_Req; }
break;
case 12:
{ primitive = GOOSE_Unsupported_Res; }
break;
case 13:
{ primitive = GOOSE_References_Res; }
break;
case 14:
{ primitive = GOOSE_Elements_Res; }
break;
case 15:
{ primitive = GSE_References_Res; }
break;
case 16:
{ primitive = GSE_Elements_Res; }
break;
case 17:
{
            if (!MMSd_fetchSTRING( &(yyvsp[-3].IncomingTOKEN), goID, sizeof( goID ), (yyvsp[-3].IncomingTOKEN).tag ))
                YYABORT;
            requestDataToken = yyvsp[-2].IncomingTOKEN;
            yyval.OutgoingLENGTH = 0;
        }
break;
case 20:
{
            if (!MMSd_fetchSTRING( &(yyvsp[-3].IncomingTOKEN), goID, sizeof( goID ), (yyvsp[-3].IncomingTOKEN).tag ))
                YYABORT;
            requestDataToken = yyvsp[-2].IncomingTOKEN;
            yyval.OutgoingLENGTH = 0;
        }
break;
case 23:
{ 
        	TMW_INT64 temp;
            if (!MMSd_fetchSTRING( &(yyvsp[-6].IncomingTOKEN), goID, sizeof( goID ), (yyvsp[-6].IncomingTOKEN).tag ))
                YYABORT;
            confRev = 0;
            if (MMSd_noError(yyvsp[-5].IncomingTOKEN))
                if (!MMSd_fetchLONG( &(yyvsp[-5].IncomingTOKEN), &temp, (yyvsp[-5].IncomingTOKEN).tag ))
                    YYABORT;
            dataSetName[0] = 0;
            if (MMSd_noError(yyvsp[-3].IncomingTOKEN))
                if (!MMSd_fetchSTRING( &(yyvsp[-3].IncomingTOKEN), 
                                       dataSetName, 
                                       sizeof( dataSetName ), 
                                       (yyvsp[-3].IncomingTOKEN).tag ))
                    YYABORT;
            requestDataToken = yyvsp[-2].IncomingTOKEN;
            responseErrorCode = -1;
            yyval.OutgoingLENGTH = 0; 
            confRev = (long) temp;
        }
break;
case 24:
{ 
            TMW_INT64 temp;
            if (!MMSd_fetchSTRING( &(yyvsp[-2].IncomingTOKEN), goID, sizeof( goID ), (yyvsp[-2].IncomingTOKEN).tag ))
                YYABORT;
            confRev = 0;
            if (MMSd_noError(yyvsp[-1].IncomingTOKEN)) {
                if (!MMSd_fetchLONG( &(yyvsp[-1].IncomingTOKEN), &temp, (yyvsp[-1].IncomingTOKEN).tag ))
                    YYABORT;
                confRev = (long)temp;
            }
            if (!MMSd_fetchINTEGER( &(yyvsp[0].IncomingTOKEN), &responseErrorCode, (yyvsp[0].IncomingTOKEN).tag ))
                    YYABORT;
            yyval.OutgoingLENGTH = 0; 
        }
break;
case 25:
{ yyval.IncomingTOKEN = yyvsp[0].IncomingTOKEN; }
break;
case 26:
{ MMSd_setDescErrorFlag(MMSd_null_descriptor, yyval.IncomingTOKEN); }
break;
case 27:
{ yyval.IncomingTOKEN = yyvsp[0].IncomingTOKEN; }
break;
case 28:
{ MMSd_setDescErrorFlag(MMSd_null_descriptor, yyval.IncomingTOKEN); }
break;
case 29:
{ yyval.OutgoingLENGTH = 0; }
break;
case 30:
{ yyval.OutgoingLENGTH = 0; }
break;
case 31:
{ yyval.OutgoingLENGTH = 0; }
break;
case 32:
{ yyval.OutgoingLENGTH = 0; }
break;
case 33:
{ yyval.OutgoingLENGTH = 0; }
break;
case 34:
{ yyskip( &(yyvsp[0].IncomingTOKEN), &descriptor, &length_nesting /* V8.9 yyskip( _CNTX, &($12) */ ); }
break;
case 35:
{ 
            TMW_INT64 temp;
            if (!MMSd_fetchSTRING( &(yyvsp[-14].IncomingTOKEN), header.gocbRef, 
                                sizeof( header.gocbRef ), gocbRef0 ))
                YYABORT;
            if (!MMSd_fetchUNSIGNEDLONG( &(yyvsp[-13].IncomingTOKEN), &header.HoldTim,
                                         timeAllowedtoLive1 ))
                YYABORT;
            if (!MMSd_fetchSTRING( &(yyvsp[-12].IncomingTOKEN), header.datSet, 
                                sizeof( header.datSet ), datSet2 ))
                YYABORT;
            if (MMSd_noError(yyvsp[-11].IncomingTOKEN))
            {
                if (!MMSd_fetchSTRING( &(yyvsp[-11].IncomingTOKEN), header.goID, 
                                sizeof( header.goID ), goID3 ))
                    YYABORT;
            }
            else
                header.goID[ 0 ] = 0;
            if (!MMSd_fetchUTCTIME( &(yyvsp[-10].IncomingTOKEN), &header.t, t4 ))
                YYABORT;
            if (!MMSd_fetchUNSIGNEDLONG( &(yyvsp[-9].IncomingTOKEN), &header.StNum, stNum5 ))
                YYABORT;
            if (!MMSd_fetchUNSIGNEDLONG( &(yyvsp[-8].IncomingTOKEN), &header.SqNum, sqNum6 ))
                YYABORT;
            header.test = (unsigned char) (yyvsp[-7].OutgoingLENGTH != 0);
            if (!MMSd_fetchLONG( &(yyvsp[-6].IncomingTOKEN), &temp, confRev8 ))
                YYABORT;
            header.confRev = (long)temp;
            header.ndsCom = (unsigned char) (yyvsp[-5].OutgoingLENGTH != 0);
            if (!MMSd_fetchUNSIGNEDLONG( &(yyvsp[-4].IncomingTOKEN), &header.numDataSetEntries, 
                                                        numDatSetEntries10 ))
                YYABORT;
            if (MMSd_noError(yyvsp[-1].IncomingTOKEN))
            {
                header.pSecurity = MMSd_getDescPtr( yyvsp[-1].IncomingTOKEN );
                header.nSecurityLen = MMSd_TokenLen( yyvsp[-1].IncomingTOKEN );
            }
            else
            {
                header.pSecurity = NULL;
                header.nSecurityLen = 0;
            }
            *passedHeader = header;
            *dataPtr = MMSd_getDescPtr( yyvsp[-3].IncomingTOKEN );
            *dataLen = MMSd_TokenLen( yyvsp[-3].IncomingTOKEN );
            YYACCEPT; YY_SKIP_BREAK;
        }
break;
case 36:
{ yyval.IncomingTOKEN= yyvsp[0].IncomingTOKEN; }
break;
case 37:
{ MMSd_setDescErrorFlag(MMSd_null_descriptor, yyval.IncomingTOKEN); }
break;
case 38:
{ MMSd_fetchBOOLEAN( &(yyvsp[0].IncomingTOKEN), &(yyval.OutgoingLENGTH) ); }
break;
case 39:
{ yyval.OutgoingLENGTH = 0; }
break;
case 40:
{ MMSd_fetchBOOLEAN( &(yyvsp[0].IncomingTOKEN), &(yyval.OutgoingLENGTH) ); }
break;
case 41:
{ yyval.OutgoingLENGTH = 0; }
break;
case 42:
{ yyval.IncomingTOKEN = yyvsp[0].IncomingTOKEN; }
break;
case 43:
{ MMSd_setDescErrorFlag(MMSd_null_descriptor, yyval.IncomingTOKEN); }
break;
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
        {
            sprintf(deb_buff, "%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
            yydeb_out( yydeb_file, deb_buff );
        }
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                sprintf(deb_buff, "%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
                yydeb_out( yydeb_file, deb_buff );
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if (   ((yyn = yygindex[yym]) != 0)
           && ((yyn += yystate) >= 0)
           && (yyn <= YYTABLESIZE)
           && (yycheck[yyn] == yystate))
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
    {
        sprintf(deb_buff, "%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
        yydeb_out( yydeb_file, deb_buff );
    }
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = (short) yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
#if YYDEBUG
    if ( yydeb_file != NULL )
       fclose( yydeb_file );
#endif
    YYUSERABORT
yyaccept:
#if YYDEBUG
    if ( yydeb_file != NULL )
       fclose( yydeb_file );
#endif
    YYUSERACCEPT
}

#if YYDEBUG
void yydeb_out( FILE *fd, char *deb_buff )
{
    if ( fd != NULL )
       fprintf(fd, "%s", deb_buff);
}
#endif

/* end go_tab.c */
