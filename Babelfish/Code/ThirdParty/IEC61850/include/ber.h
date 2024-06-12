/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1997-2015 */
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


/** @ber.h
 * This file contains definition for Basic Encoding Rules.
 * This file should not require modification.
 */


#ifndef _BER_H
#define _BER_H

#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned short  MMSd_tag;          /* Strictly for tags */
typedef unsigned short  MMSd_ushort;       /* For lengths. */
typedef short           MMSd_short;
typedef unsigned char   MMSd_uchar;
typedef char            MMSd_char;
typedef unsigned long   MMSd_errorFlag;    /* For Handler Errors */
typedef MMSd_ushort     MMSd_length;

typedef int MMSd_dictionaryIndex;

typedef unsigned char   * MMSd_MSGptr;
typedef unsigned long   MMSd_InvokeIdType;

/**
 * Encoding rules
 */
typedef enum {
    basic_encoding_rules,
    packed_encoding_rules,
    session_encoding_rules,
    goose_fixed_encoding_rules,
    goose_fixed_header_encoding_rules
} MMSd_enc_rules ;


/** MMSd_descriptor
 ---
 --- This is structure used to retain information about a sub-message.
 --- The message can either be a REQUEST (and thus being parsed) or it
 --- could be a RESPONSE (and thus the target for EMITs).
 ---
 --- If the descriptor is used as a token, the index is the start and
 --- the tokenLength is the length of the token.
 ---
 --- If the descriptor is an emit descriptor and bufLen is zero, the
 --- message is in the buffer between 'index' and the end of the
 --- buffer.
 ---
 --- If the descriptor is an emit descriptor and bufLen is nonzero,
 --- the message is in the buffer between 'tokenLength' and 'index'.
 ---
 --------------------------------------------------------------
 */
typedef struct MMSd_descriptor {
    MMSd_MSGptr     bufPtr;         /* Buffer pointer */
    MMSd_errorFlag  errorFlag;      /* Cumulative errors */
    MMSd_tag        tag;            /* ASN1 tag value */
    MMSd_ushort     index;          /* Curent position in msg */
    MMSd_ushort     tokenLength;    /* Length of subordinate type */
    MMSd_ushort     bufLen;         /* Current length of msg */
    boolean         primitive;      /* primitive or constructed token? */
    MMSd_enc_rules  rules;          /* Encode/Decode rules in effect */
} MMSd_descriptor;


/** MMSd_descriptor macros
---
--- This macro package hides all of the internal
--- structure of the descriptors from the rest of the
--- code.  This is critical, in that on some platform
--- the internal representation may change.  In particular,
--- some memory constrained platforms may not allow for
--- copying segmented PDUs into a contiguous buffer.  The
--- descriptors may then be required to walk through chained
--- segments.
---
--- Basically, all descriptor access should be through these
--- macros.
--------------------------------------------------------------
*/

#define MMSd_getDescPtr( _F_ )     ((_F_).bufPtr + (_F_).index)
#define MMSd_TokenLen( _F_ )       (_F_).tokenLength
#define MMSd_DescLen( _F_ )        (_F_).bufLen
#define MMSd_DescIndex( _F_ )      (_F_).index
#define MMSd_DescTag( _F_ )        (_F_).tag

#define MMSd_emittedLength( _F_ )                   \
   ((MMSd_length) (((_F_).tokenLength <= 0) ? ((_F_).bufLen - (_F_).index - 1) \
                            : ((_F_).tokenLength - (_F_).index)))

#define MMSd_emittedInfoLength( _F_ )               \
   ((MMSd_length) ((_F_).bufLen - (_F_).index - 1))

/* Macros for manipulating descriptors as a whole
   -------------------------------------------------------------
*/
#define MMSd_makeDescriptor( _D_, _B_, _L_, _I_ )   \
{                                                   \
    (_D_).rules = basic_encoding_rules;             \
    (_D_).tokenLength = 0;                          \
    (_D_).index = _I_;                              \
    (_D_).bufLen = _L_;                             \
    (_D_).bufPtr = _B_;                             \
    (_D_).tag = 0;                                  \
    (_D_).errorFlag = 0;                            \
}
#define MMSd_takeDescriptor( _D_, _S_, _L_ ) \
{                                            \
    _S_ = (MMSd_length) ((_D_).index + 1);   \
    if ((_D_).tokenLength <= 0)              \
        _L_ = (MMSd_length) ( (_D_).bufLen - _S_); \
    else                                     \
        _L_ = (MMSd_length) ( (_D_).tokenLength - _S_ + 1 ); \
}

#define    MMSd_emptyDescriptor( _IN_, _OUT_ )  \
{                                               \
    MMSd_copyDescriptor( _IN_, _OUT_ );         \
    (_OUT_).index = (MMSd_length) ((_OUT_).bufLen - 1); \
    (_OUT_).tokenLength = 0;                     \
}

#define    MMSd_isEmptyDescriptor( _D_ )  \
 ( ((_D_).index == (_D_).bufLen - 1) \
    && ((_D_).tokenLength == 0 ) \
        && ((_D_).errorFlag == 0 ))

#define    MMSd_copyDescriptor( _IN_, _OUT_ )   (_OUT_ = _IN_)


/** Macros for in parsing PDUs
 ---
 --- Parsing always proceeds left to right.
 --- The index is of the next field to parse.
 --- The field tokenLength is the length of the parsed token
 --------------------------------------------------------------
 */
#define MMSd_getDescOctet( _F_ )    *(MMSd_getDescPtr(_F_))
#define MMSd_fetchDescOctet( _F_ )  *((_F_).bufPtr + (_F_).index++)
#define MMSd_skipToken(_D_)         (_D_).index = (MMSd_length) \
                                              ((_D_).index + (_D_).tokenLength)
#define MMSd_makeToken(_D_,_E_)     { MMSd_copyDescriptor(_D_, _E_); }

/* -- Error flag manipulations */

#define MMSd_noError(_D_)        ( !(_D_).errorFlag )
#define MMSd_fatalError(_D_)     ( (_D_).errorFlag & MMSd_GLOBAL_ERROR )
#define MMSd_localError(_D_)     ( (_D_).errorFlag & ~MMSd_GLOBAL_ERROR )


#define noTag       0x00        /* This is used for fetch calls when no
                                   checking is necessary */

/** -- Error flag Access
 *-- These macros are used to access the error status of
 * descriptors.  Most error handling is performed in this way.
 *****************************************************************************/


#if !defined(DEBUG_ERROR_HANDLING)
#define MMSd_setDescErrorFlag(_V_,_F_)  (_F_).errorFlag = _V_
#else
void MMSd_setDebugDescErrorFlag( unsigned long nErrorFlag, MMSd_descriptor *pDescriptor );  /* in src\mmsd\emitfetc.c */
#define MMSd_setDescErrorFlag(_V_,_F_)  \
        MMSd_setDebugDescErrorFlag( (_V_), &(_F_) )
#endif

#define MMSd_checkDescErrorFlag( _V_, _E_ )    ((_E_).errorFlag & (_V_))

#define MMSd_initDescErrorFlag(_E_) (_E_).errorFlag = MMSd_noHandlerErrors


/* -- LENGTH_TYPE
 *
 * This type is used for checking the lengths of MMS sub-fields
 *
 *****************************************************************************
 */

#define INDEFINITE_LENGTH   ((MMSd_ushort) ~0)   /* All bits set */
#define MAX_LENGTH_DEPTH    35              /* Maximum message nesting */

typedef struct {
    boolean new_PDU;                        /* YYLEX shouldn't push the first one */
    short   top;
    MMSd_ushort  end_pos[ MAX_LENGTH_DEPTH ];
} LENGTH_TYPE;

/** MMSd_fetchTAG_and_LENGTH ----------------
 ---
 --- Defined in EMITFETCH.C
 --- Used throughout to isolate the encoding rules.
 */

TMW_CLIB_API void MMSd_fetchTAG_and_LENGTH(MMSd_descriptor *T);

/** Macros for emitting octets into the response buffer
 ---
 --- The index and tokenLength components are used differently
 --- when the descriptor is used for emits.  Index is generally
 --- at the front of the data in the buffer, and tokenLength is
 --- at the back of the data. These indices can be used in
 --- reverse order, however, when things need to be emitted in
 --- backward.  This is used, for instance, when working with
 --- Info reports.  To allow for emitting names and values at
 --- the same time, the names go in reverse order at the front
 --- of the buffer, and are later moved to prepend the data.
 ---
 --- This is all handled in EMITOctet, which checks the value
 --- of the tag component of the descriptor to decide which
 --- operation is being performed.
 -------------------------------------------------------------
 */

/* Switches for the tag field on emit descriptors
 -------------------------------------------------------------
 */
#define MMSdEmitDec 0
#define MMSdEmitInc 1
#define MMSdEmitDirectionMASK     MMSdEmitInc|MMSdEmitDec
#define MMSdEmitFront 0
#define MMSdEmitBack 2
#define MMSdEmitEndMASK     MMSdEmitFront|MMSdEmitBack

/* Emit operations on the descriptor
 ------------------------------------------------------------
 */
#define MMSd_appendFront( _value_, _F_ )                 \
    *((_F_)->bufPtr + (_F_)->index++) = (unsigned char) (_value_)

#define MMSd_appendBack( _value_, _F_ )                 \
    *((_F_)->bufPtr + (_F_)->tokenLength++) = (unsigned char) (_value_)

#define MMSd_prependFront( _value_, _F_ )                 \
    *((_F_)->bufPtr + (_F_)->index--) = (unsigned char) (_value_)

#define MMSd_prependBack( _value_, _F_ )                 \
    *((_F_)->bufPtr + (_F_)->tokenLength--) = (unsigned char) (_value_)

/** Used in BER.H when full checking is turned on
 -------------------------------------------------------------
 */
#define MMSd_appCheckDesc( _F_ )     ((_F_).index < (_F_).bufLen)
#define MMSd_preCheckDesc( _F_ )     ((_F_).index > 0)

/* If uca_time.h is included, we do not need this here */
#if !defined(_UCA_TIME_DEF)
/** MMSd_time ------------------------------------------------------
 --- For MMS variables of type Binary-Time.
 --- Used by both FETCH and EMIT routines.
 -----------------------------------------------------------------
 */
typedef struct {
    long  millisecondsSinceMidnight;
    int     daysSinceJan1_1984;
#if defined(MMSD_MICROSECONDS_TIMER)
    int     microseconds;
#endif
} MMSd_time;
#define _UCA_TIME_DEF
#endif

#if !defined(_UTC_TIME_DEF)
typedef unsigned char MMSd_UtcQuality;
#define MMSd_Utc_LeapSecondOcurred      0x01
#define MMSd_Utc_LeapSecondNegative     0x02
#define MMSd_Utc_AccuracyMask           0x1C
#define MMSd_Utc_FaultNotSynchronzed    0x20
#define MMSd_Utc_FaultClockFailure      0x40
#define MMSd_UtcAccuracy( q )   ( (q & MMSd_Utc_AccuracyMask ) >> 2 )

typedef struct MMSd_UtcBTime {
    unsigned long   secondOfCentury;
    unsigned long   fractionOfSecond;
    MMSd_UtcQuality quality;
} MMSd_UtcBTime;
#define _UTC_TIME_DEF
#endif

/** EMIT functions  -----------------------------------------------
 --- These are used internally for emit processing.  Note that
 --- there is a distinct set of fetch and emit macros for use
 --- in data handlers which make assumptions regarding the ASN.1
 --- tags, since they are intended for a particular scope.
 --------------------------------------------------------------
 */

/** Prototypes for INTERNAL emit functions. -----------------------------
 --- Users have a distinct interface layer of macros to do emits.
 ---
 --- The first three routines (capital EMIT) allow for emitting
 --- in either direction and at the front or back of a message.
 --- All of the others simply PREPEND to the front.
 ----------------------------------------------------------------
 */

TMW_CLIB_API void MMSd_EMITOctet(int octet, MMSd_descriptor *E);
MMSd_length MMSd_EMITValue(TMW_INT64 value, MMSd_descriptor *E);
TMW_CLIB_API MMSd_length MMSd_EMITUnsignedValue(unsigned long value, MMSd_descriptor *E);
TMW_CLIB_API MMSd_length MMSd_emitTAGGEDVALUE(MMSd_tag tag, TMW_INT64, MMSd_descriptor *E);
TMW_CLIB_API MMSd_length MMSd_emitTAGGEDUNSIGNEDVALUE(MMSd_tag tag, unsigned long value, MMSd_descriptor *E);

/* Emit routines for fixed-length Goose encodings */
MMSd_length MMSd_EMITFIXEDValue( TMW_INT64 value, int size, MMSd_descriptor *E );
TMW_CLIB_API MMSd_length MMSd_emitFIXEDTAGGEDVALUE(MMSd_tag tag, TMW_INT64 value, int size, MMSd_descriptor *E);
TMW_CLIB_API MMSd_length MMSd_EMITFIXEDUnsignedValue(unsigned long value, int size, MMSd_descriptor *E);
TMW_CLIB_API MMSd_length MMSd_emitFIXEDSTRING(char *s, int size, MMSd_descriptor *E);
TMW_CLIB_API MMSd_length MMSd_emitFIXEDOCTETS(unsigned char *o, MMSd_length len, MMSd_descriptor *E);

MMSd_length MMSd_emitAuthenticationValue(unsigned char *pCertificate, int nCertificateLength, unsigned char *pSignedTimeValue, int nSignedTimeValueLength, char *pGeneralizedTimeStamp,
        int nStaseMmsPresentationContextId, MMSd_descriptor *E);

TMW_CLIB_API MMSd_length MMSd_emitTAG_and_LENGTH(MMSd_tag TOKEN, MMSd_ushort len, MMSd_descriptor *E);
TMW_CLIB_API MMSd_length MMSd_emitOCTETS(unsigned char *o, MMSd_ushort l, MMSd_descriptor *E);
TMW_CLIB_API MMSd_length MMSd_emitBITSTRING(unsigned char *s, MMSd_ushort l, MMSd_descriptor *E);

#ifdef MMSD_STRING_TYPE
TMW_CLIB_API MMSd_length MMSd_emitSTRING(char *s, MMSd_descriptor *E);
#endif

#ifdef MMSD_FLOAT_EMIT
TMW_CLIB_API MMSd_length MMSd_emitFLOAT(MMSD_FLOAT_DATA_TYPE v, MMSd_descriptor *E);
#endif

#ifdef MMSD_FLOAT64_EMIT
TMW_CLIB_API MMSd_length MMSd_emitFLOAT64(MMSD_FLOAT64_DATA_TYPE v, MMSd_descriptor *E);
#endif

#ifdef MMSD_REAL_TYPE
MMSd_length MMSd_emitREAL( double v,  MMSd_descriptor *E );
#endif

#ifdef MMSD_BCD_TYPE
TMW_CLIB_API MMSd_length MMSd_emitBCD(TMW_INT64 val, MMSd_descriptor *E);
#endif

#ifdef MMSD_BINARYTIME_TYPE
TMW_CLIB_API MMSd_length MMSd_emitBINARYTIME(MMSd_time *t, int size, MMSd_descriptor *E);
#endif

#ifdef MMSD_UTCTIME_TYPE
TMW_CLIB_API MMSd_length MMSd_emitUTCTIME(MMSd_UtcBTime *t, int size, MMSd_descriptor *E);
#endif

#ifdef MMSD_OBJECT_IDENTIFIER_TYPE
TMW_CLIB_API MMSd_length MMSd_emitOBJECT_IDENTIFIER(int *o, int len, MMSd_descriptor *E);
#endif

MMSd_length MMSd_emitSessionLENGTH(int length, MMSd_descriptor *E);
MMSd_length MMSd_emitSessionTAG(MMSd_tag tag, MMSd_descriptor *E);
MMSd_length MMSd_appendName( int tag, char * S, int len, MMSd_descriptor *E );

TMW_CLIB_API int MMSd_stringLength(char *strPtr);

/** FETCH functions  -----------------------------------------------
 --- These are used internally for fetch processing.  Note that
 --- there is a distinct set of fetch and emit macros for use
 --- in data handlers which make assumptions regarding the ASN.1
 --- tags, since they are intended for a particular scope.
 --------------------------------------------------------------
 */

/** Prototypes for INTERNAL fetch functions. -------------------------
 --- Users have a distinct interface layer of macros to do fetch.
 ---
 --- All fetches operate left to right in the buffer.
 ----------------------------------------------------------------
 */

TMW_CLIB_API int MMSd_fetchLONG(MMSd_descriptor *TOKEN, TMW_INT64 *lval, MMSd_tag tag);

TMW_CLIB_API int MMSd_fetchUNSIGNEDLONG(MMSd_descriptor *TOKEN, unsigned long *ulval, MMSd_tag tag);

TMW_CLIB_API int MMSd_fetchINTEGER(MMSd_descriptor *TOKEN, int *ival, MMSd_tag tag);

TMW_CLIB_API int MMSd_fetchUNSIGNEDINTEGER(MMSd_descriptor *TOKEN, unsigned int *uival, MMSd_tag tag);

TMW_CLIB_API int MMSd_fetchBITSTRING(MMSd_descriptor *TKN, unsigned char *outflags, int outByteLen, MMSd_tag tag);

TMW_CLIB_API int MMSd_fetchOCTETS(MMSd_descriptor *TKN, unsigned char *outoctets, int outLen, MMSd_tag tag);

#ifdef MMSD_STRING_TYPE
TMW_CLIB_API int MMSd_fetchSTRING(MMSd_descriptor *TKN, char *outStr, int outLen, MMSd_tag tag);
#endif

#ifdef MMSD_FLOAT_FETCH
TMW_CLIB_API int MMSd_fetchFLOAT(MMSd_descriptor *TKN, MMSD_FLOAT_DATA_TYPE *fval, MMSd_tag tag);
#endif

#ifdef MMSD_FLOAT64_FETCH
TMW_CLIB_API int MMSd_fetchFLOAT64(MMSd_descriptor *TKN, MMSD_FLOAT64_DATA_TYPE *fval, MMSd_tag tag);
#endif

#ifdef MMSD_REAL_TYPE
int MMSd_fetchREAL( MMSd_descriptor *TKN,
                    double *fval,
                    MMSd_tag tag);
#endif

#ifdef MMSD_BCD_TYPE
TMW_CLIB_API int MMSd_fetchBCD(MMSd_descriptor *TKN, TMW_INT64 *ival, MMSd_tag tag);
#endif

#ifdef MMSD_OBJECT_IDENTIFIER_TYPE
int MMSd_fetchOBJECT_IDENTIFIER( MMSd_descriptor *TKN, int limit, int *oval, int *num_o, MMSd_tag tag );
#endif

#ifdef MMSD_BINARYTIME_TYPE
TMW_CLIB_API int MMSd_fetchBINARYTIME(MMSd_descriptor *TKN, MMSd_time *tval, MMSd_tag tag);
#endif

#ifdef MMSD_UTCTIME_TYPE
TMW_CLIB_API int MMSd_fetchUTCTIME(MMSd_descriptor *TKN, MMSd_UtcBTime *tval, MMSd_tag tag);
#endif

TMW_CLIB_API boolean MMSd_fetchCOMPLETE(MMSd_descriptor *des1, MMSd_descriptor *des2);

void MMSd_fetchSessionTAG( MMSd_descriptor *D );
void MMSd_fetchSessionLENGTH( MMSd_descriptor *D );

/** ASN.1 Tags
 --- These are the ASN.1 tag values for each of the data
 --- primitives that data handlers need to use.  The user
 --- doesn't need to be concerned about them, their use is
 --- embedded in the macros below.
 ------------------------------------------------------------
 */

#define array1                  0xa1
#define structure2              0xa2
#define boolean3                0x83
#define bitString4              0x84
#define integer5                0x85
#define unsigned6               0x86
#define floatingPointData7      0x87
#define real8                   0xa8
#define octetString9            0x89
#define visibleString10         0x8a
#define generalizedTime11       0x8b
#define binaryTime12            0x8c
#define bcd13                   0x8d
#define objId15                 0x8f
#define mmsString16             0x90
#define utcTime17               0x91

/** --
 -- The following are used for output of TYPE information
 --
 */

#define mmsDeletable0           0x80
#define components1             0xa1
#define componentName0          0x80
#define componentType1          0xa1
#define typeSpecification2      0xa2
#define typeName0           0xa0
#define packed0             0x80
#define base0               0x80
#define exponent1           0x81
#define mantissa2           0x82
#define exponentWidth1          0x02
#define floatingPoint7          0xa7
#define formatWidth0            0x02

#define aaSpecific2             0x82
#define domainSpecific1         0x81
#define domainSpecific1_1       0xa1
#define failure0                0x80
#define informationReport0      0xa0
#define listOfAccessResult0     0xa0
#define listOfVariable0         0xa0
#define name0                   0xa0
#define success1                0x81
#define unconfirmed_RequestPDU3 0xa3
#define variableSpec            0x30
#define vmdSpecific0            0x80
#define variableListName1       0xa1
#define VisibleString26         0x1a
#define elementType2            0xa2

/** --
 -- The following are used for performing alternate access
 --
 */

#define  altComponent0           0x80
#define  altIndex1               0x81
#define  altIndexRange2          0xa2
#define  altAllElements3         0x83
#define  namedAltAccess4         0x84
#define  altComponent1           0x81
#define  altIndex2               0x82
#define  altIndexRange3          0xa3
#define  altAllElements4         0x84
#define  namedAltAccess5         0x85
#define  altComponentName0       0x80
#define  selectAltAccess0        0xa0
#define  lowIndex0               0x80
#define  numberOfElements1       0x81
#define	 alternateAccess5	     0xa5   /* Added V7.34 */

/** --
 -- The following are used for the file access services
 --
 */

#define fileAttributes1          0xa1
#define fileClose74              0x9f4a
#define fileDelete76             0xbf4c
#define fileDirContinue1         0xa1
#define fileDirectory77          0xbf4d
#define fileName0                0xa0
#define fileOpen72               0xbf48
#define fileRead73               0xbf49
#define GraphicString25          0x19
#define sequenceof16             0x30
#define listOfDirectoryEntry0    0xa0
#define sizeOfFile0              0x80
#define lastModified1            0x81
#define moreFollows1             0x81

/** ASN1 User Macros ---------------------------------------------
 --- The following macros define the programming interface
 --- for data handlers and other server application user
 --- functions.  They are defined as macros to keep the
 --- code size as small as possible.
 --------------------------------------------------------------
 */

#define castTKN( _T_ )                                                  \
    ((MMSd_descriptor *) _T_)

/** ---- For advancing to the next field in a WRITE handler --- */
/** ----------------------------------------------------------- */

#define MMSd_nextToken( _TKN_ )                                         \
    MMSd_fetchTAG_and_LENGTH( castTKN(_TKN_) )

/** WARNING: The following macros two macros open and close a 'C'
 ------- scope, hence the curly braces ("{") are NOT matched in
 ------- their declarations.
 -------------------------------------------------------------- */

#define MMSd_beginHandlerStruct(_TKN_)                                  \
    {                                                                   \
       MMSd_descriptor save;                        \
       MMSd_copyDescriptor( *castTKN(_TKN_), save );            \
       MMSd_nextToken(_TKN_);

#define MMSd_endHandlerStruct(_TKN_)                                    \
    if (!MMSd_fetchCOMPLETE( &save, castTKN(_TKN_) ))           \
        MMSd_setDescErrorFlag(MMSd_data_type_inconsistent, *castTKN(_TKN_));  \
        /* WARNING: This is the closing brace                           \
            for MMSd_beginHandlerStruct */                              \
    }

/* ---- For emitting a structure in a READ handler ----------- */
/* ----------------------------------------------------------- */

#define MMSd_emitStructure( _L_, _D_ )                                  \
    MMSd_emitTAG_and_LENGTH( structure2, _L_, _D_)

/** ASN1 Data Primitives ----------------------------------------
 --- The following macros are used to manipulate the MMS
 --- primitive data types:
 ---
 --- The MMSd_checkXXX macros test the type of incoming data for
 --- WRITE handlers.  These allow the user to perform some
 --- error handling, and also allow for limited type coersion
 --- in WRITE handlers.
 ---
 --- The MMSd_fetchUSER_XXX macros retrieve the value pointed at
 --- by the MMSd_descriptor argument, for use in WRITE handlers.
 ---
 --- The MMSd_emitUSER_XXX macros place the user data valueinto
 --- into the output MMS response message at the position
 --- pointed at by the MMSd_descriptor argument, for use in READ
 --- handlers.  Users should note that for handlers performing
 --- complex operations (such as structures), the components
 --- must be emitted in REVERSE order.
 -------------------------------------------------------------- */
#define MMSd_checkError( _V_, _E_ )                 \
    MMSd_checkDescErrorFlag(_V_, *castTKN(_E_))

#define MMSd_setError( _V_, _E_ )                   \
    MMSd_setDescErrorFlag(_V_, *castTKN(_E_))

#define MMSd_checkTAG( __tag, __D )    (castTKN(__D)->tag == __tag)

/* NOTE:
   --- The following two macros look like the external emit macros,
   --- but they are strictly local to this module.  They are
   --- differentiated by not following the normal convention of
   --- capitalization.
   ----------------------------------------------------------------
*/
#define MMSd_emitInt( __tag,  __value, __E )                                \
    MMSd_emitTAG_and_LENGTH( __tag,                                     \
                            MMSd_EMITValue( (TMW_INT64) __value, __E ),      \
                            __E )

#define MMSd_emitUnsigned( __tag,  __value, __E )                           \
    MMSd_emitTAG_and_LENGTH( __tag,                                     \
                 MMSd_EMITUnsignedValue( (unsigned TMW_INT64) __value, __E ), \
                             __E )

/* ---- Name Fields ----------------------------------------------- */
/* ---------------------------------------------------------------- */

#define MMSd_checkNAME(N)                                               \
    (MMSd_checkTAG( vmdSpecific0, N ) ||                                       \
    MMSd_checkTAG( domainSpecific1, N ) ||                                     \
    MMSd_checkTAG( aaSpecific2, N ))

#define MMSd_fetchUSER_NAME(N, buf, len)                                \
    MMSd_fetchSTRING(N, buf, len, noTag)

#define MMSd_emitUSER_NAME( _n_, _E_ )                                  \
    MMSd_emitTAG_and_LENGTH(                                            \
        variableSpec,                                                   \
        MMSd_emitTAG_and_LENGTH(                                        \
            name0,                                                      \
            MMSd_emitTAG_and_LENGTH(                                    \
                vmdSpecific0,                                           \
                MMSd_emitSTRING( _n_, _E_ ),                            \
                _E_),                                                   \
            _E_),                                                       \
        _E_)

#define MMSd_appendUSER_NAME( S, L, E )                                 \
    MMSd_appendName(VisibleString26, S, L, E)


/* ---- Boolean Fields -------------------------------------------- */
/* ---------------------------------------------------------------- */

#define MMSd_checkBOOLEAN( D )                                          \
    MMSd_checkTAG( boolean3, D )

#define MMSd_fetchUSER_BOOLEAN( D, V )                                  \
    MMSd_fetchINTEGER( D, V, (MMSd_tag) boolean3 )

#define MMSd_emitUSER_BOOLEAN(I, E)                                     \
    (I ? MMSd_emitTAGGEDVALUE( boolean3, 1L, E )                        \
       : MMSd_emitTAGGEDVALUE( boolean3, 0L, E ))

#define MMSd_emitUSER_IMPLICITBOOLEAN( _t_, _v_, _E_ )                  \
    MMSd_emitTAGGEDVALUE( _t_, (TMW_INT64) _v_, _E_ )

/* ---- Bitstring Fields ------------------------------------------ */
/* ---------------------------------------------------------------- */

#define MMSd_checkBITSTRING( D )                                        \
    MMSd_checkTAG( bitString4, D )

#define MMSd_fetchUSER_BITSTRING( D, V, L )                             \
    MMSd_fetchBITSTRING( D, V, L, (MMSd_tag) bitString4 )

#define MMSd_emitUSER_BITSTRING(V, L, E)                                \
    MMSd_emitTAG_and_LENGTH( (MMSd_tag) bitString4,                     \
            MMSd_emitBITSTRING( (unsigned char *) V, (MMSd_ushort) L, E ), \
            E )

#define MMSd_getUSER_BITSTRING_LEN( D )                                 \
    ( MMSd_checkTAG( bitString4, D ) ? ( ( MMSd_TokenLen( *D ) - 1 ) * 8 ) - (int) MMSd_getDescOctet( *D ) : 0 )

/* ---- Integer Fields ------------------------------------------- */
/* --------------------------------------------------------------- */

#define MMSd_checkINTEGER( D )                                          \
    MMSd_checkTAG( integer5, D )

#define MMSd_fetchUSER_INTEGER( D, V )                                  \
    MMSd_fetchINTEGER( D, V, (MMSd_tag) integer5 )

#define MMSd_emitUSER_INTEGER(I, E)                                     \
    MMSd_emitTAGGEDVALUE( integer5, (TMW_INT64) (I), E )

#define MMSd_emitUSER_FIXEDINTEGER(I, s, E)                             \
    MMSd_emitFIXEDTAGGEDVALUE( integer5, (TMW_INT64) (I), s, E )

/* ---- Unsigned Integer Fields ---------------------------------- */
/* --------------------------------------------------------------- */

#define MMSd_checkUNSIGNEDINTEGER( D )                                  \
    MMSd_checkTAG( unsigned6, D )

#define MMSd_fetchUSER_UNSIGNEDINTEGER( D, V )                          \
    MMSd_fetchUNSIGNEDINTEGER( D, V, (MMSd_tag) unsigned6 )

#define MMSd_emitUSER_UNSIGNEDINTEGER(I, E)                             \
    MMSd_emitUnsigned( unsigned6, I, E )

#define MMSd_emitUSER_FIXEDUNSIGNEDINTEGER(I, s, E)                     \
    MMSd_emitTAG_and_LENGTH( unsigned6,                                 \
                 MMSd_EMITFIXEDUnsignedValue( (unsigned TMW_INT64) I, s, E ), E )

/* ---- Long Fields ---------------------------------------------- */
/* --------------------------------------------------------------- */

#define MMSd_checkLONG( D )                                             \
    MMSd_checkTAG( integer5, D )

#define MMSd_fetchUSER_LONG( D, V )                                     \
    MMSd_fetchLONG( D, V, (MMSd_tag) integer5 )

#define MMSd_emitUSER_LONG(I, E)                                        \
    MMSd_emitTAGGEDVALUE( integer5, (TMW_INT64) (I), E )

#define MMSd_emitUSER_FIXEDLONG(I, s, E)                                \
    MMSd_emitFIXEDTAGGEDVALUE( integer5, (TMW_INT64) (I), s, E )

/* ---- Unsigned Long Fields ------------------------------------- */
/* --------------------------------------------------------------- */

#define MMSd_checkUNSIGNEDLONG( D )                                     \
    MMSd_checkTAG( unsigned6, D )

#define MMSd_fetchUSER_UNSIGNEDLONG( D, V )                             \
    MMSd_fetchUNSIGNEDLONG( D, V, (MMSd_tag) unsigned6 )

#define MMSd_emitUSER_UNSIGNEDLONG(I, E)                                \
    MMSd_emitUnsigned( unsigned6, I, E )

#define MMSd_emitUSER_FIXEDUNSIGNEDLONG(I, s, E)                        \
    MMSd_emitTAG_and_LENGTH( unsigned6,                                 \
                 MMSd_EMITFIXEDUnsignedValue( (unsigned TMW_INT64) I, s, E ), E )

/* ---- FloatingPoint Fields ------------------------------------ */
/* -------------------------------------------------------------- */

#define MMSd_checkFLOAT( D )                                            \
    MMSd_checkTAG( floatingPointData7, D )

#ifdef MMSD_FLOAT_FETCH
#define MMSd_fetchUSER_FLOAT( D, V )                                    \
    MMSd_fetchFLOAT( D, V, (MMSd_tag) floatingPointData7 )
#endif

#ifdef MMSD_FLOAT_EMIT
#define MMSd_emitUSER_FLOAT(I, E)                                       \
    MMSd_emitTAG_and_LENGTH( floatingPointData7, MMSd_emitFLOAT( I, E ), E )
#endif

#define MMSd_checkFLOAT64( D )                                            \
    MMSd_checkTAG( floatingPointData7, D )

#ifdef MMSD_FLOAT64_FETCH
#define MMSd_fetchUSER_FLOAT64( D, V )                                    \
    MMSd_fetchFLOAT64( D, V, (MMSd_tag) floatingPointData7 )
#endif

#ifdef MMSD_FLOAT64_EMIT
#define MMSd_emitUSER_FLOAT64(I, E)                                       \
    MMSd_emitTAG_and_LENGTH( floatingPointData7, MMSd_emitFLOAT64( I, E ), E )
#endif

/* ---- Real Fields --------------------------------------------- */
/* -------------------------------------------------------------- */

#ifdef MMSD_REAL_TYPE
#define MMSd_checkREAL( D )                                             \
    MMSd_checkTAG( realData8, D )

#define MMSd_fetchUSER_REAL( D, V )                                     \
    MMSd_fetchREAL( D, V, (MMSd_tag) realData8 )

#define MMSd_emitUSER_REAL(I, E)                                        \
    MMSd_emitTAG_and_LENGTH( realData8, MMSd_emitREAL( I, E ), E )
#endif

/* ---- Octet String Fields ------------------------------------- */
/* -------------------------------------------------------------- */

#define MMSd_checkOCTETSTRING( D )                                      \
    MMSd_checkTAG( octetString9, D )

#define MMSd_fetchUSER_OCTETSTRING( D, V, L )                           \
    MMSd_fetchOCTETS( D, V, L, (MMSd_tag) octetString9 )

#define MMSd_emitUSER_OCTETSTRING( V, L, D )                            \
    MMSd_emitTAG_and_LENGTH( octetString9,                              \
                             MMSd_emitOCTETS( V, (MMSd_ushort) L, D ),  \
                             D )

#define MMSd_emitUSER_FIXEDOCTETSTRING( V, L, D )                       \
    MMSd_emitTAG_and_LENGTH( octetString9,                              \
                             MMSd_emitFIXEDOCTETS( V, (MMSd_ushort) L, D ), D )

/* ---- Visible String Fields ----------------------------------- */
/* -------------------------------------------------------------- */

#ifdef MMSD_STRING_TYPE
#define MMSd_checkVISIBLESTRING( D )                                    \
    MMSd_checkTAG( visibleString10, D )

#define MMSd_fetchUSER_VISIBLESTRING( D, V, L )                         \
    MMSd_fetchSTRING( D, V, L, (MMSd_tag) visibleString10 )

#define MMSd_emitUSER_VISIBLESTRING( V, D )                             \
    MMSd_emitTAG_and_LENGTH( visibleString10, MMSd_emitSTRING( V, D ), D )

#define MMSd_emitUSER_FIXEDVISIBLESTRING( V, s, D )                     \
    MMSd_emitTAG_and_LENGTH( visibleString10, MMSd_emitFIXEDSTRING( V, s, D ), D )
#endif

/* ---- Generalized Time Fields --------------------------------- */
/* -------------------------------------------------------------- */

#ifdef MMSD_GENERALIZEDTIME_TYPE
#define MMSd_checkGENERALIZEDTIME( D )                                  \
     MMSd_checkTAG( generalizedTime11, D )

#define MMSd_fetchUSER_GENERALIZEDTIME( D, V, L )                       \
    MMSd_fetchSTRING( D, V, L, (MMSd_tag) generalizedTime11 )

#define MMSd_emitUSER_GENERALIZEDTIME( V, D )                           \
    MMSd_emitTAG_and_LENGTH( generalizedTime11, MMSd_emitSTRING( V, D ), D )
#endif

/* ---- Binary Time Fields -------------------------------------- */
/* -------------------------------------------------------------- */

#ifdef MMSD_BINARYTIME_TYPE
#define MMSd_checkBINARYTIME( D )                                       \
    MMSd_checkTAG( binaryTime12, D )

#define MMSd_fetchUSER_BINARYTIME( D, V )                               \
    MMSd_fetchBINARYTIME( D, V, (MMSd_tag) binaryTime12 )

#define MMSd_emitUSER_BINARYTIME( V, S, D )                                \
    MMSd_emitTAG_and_LENGTH( binaryTime12, MMSd_emitBINARYTIME( V, S, D ), D )
#endif

/* ---- MmsString (UNICODE) Fields ------------------------------ */
/* -------------------------------------------------------------- */
#define MMSd_checkMMSSTRING( D )                                    \
    MMSd_checkTAG( mmsString16, D )

#define MMSd_fetchUSER_MMSSTRING( D, V, L )                         \
    MMSd_fetchSTRING( D, V, L, (MMSd_tag) mmsString16 )

#define MMSd_emitUSER_MMSSTRING( V, D )                             \
    MMSd_emitTAG_and_LENGTH( mmsString16, MMSd_emitSTRING( V, D ), D )

/* ---- Utc Time Fields ----------------------------------------- */
/* -------------------------------------------------------------- */

#ifdef MMSD_UTCTIME_TYPE
#define MMSd_checkUTCTIME( D )                                       \
    MMSd_checkTAG( utcTime17, D )

#define MMSd_fetchUSER_UTCTIME( D, V )                               \
    MMSd_fetchUTCTIME( D, V, (MMSd_tag) utcTime17 )

#define MMSd_emitUSER_UTCTIME( V, S, D )                                \
    MMSd_emitTAG_and_LENGTH( utcTime17, MMSd_emitUTCTIME( V, S, D ), D )
#endif

/* ---- BCD Fields ---------------------------------------------- */
/* -------------------------------------------------------------- */

#ifdef MMSD_BCD_TYPE
#define MMSd_checkBCD( D )                                              \
    MMSd_checkTAG( bcd13, D )

#define MMSd_fetchUSER_BCD( D, V )                                      \
    MMSd_fetchBCD( D, (TMW_INT64 *) V, (MMSd_tag) bcd13 )

#define MMSd_emitUSER_BCD( V, D )                                       \
    MMSd_emitTAG_and_LENGTH( bcd13, MMSd_emitBCD( (TMW_INT64) V, D ), D )
#endif


/* ---- Object Name              -------------------------------- */
/* -------------------------------------------------------------- */

MMSd_length  MMSd_emitOName( MMSd_ObjectName *pObject, MMSd_descriptor *E );



/* ---- Object Identifier Fields -------------------------------- */
/* -------------------------------------------------------------- */

#ifdef MMSD_OBJECT_IDENTIFIER_TYPE
#define MMSd_checkOBJECTID( D )                                         \
    MMSd_checkTAG( objId15, D )

#define MMSd_fetchUSER_OBJECTID( D, V, MAX, L )                              \
    MMSd_fetchOBJECT_IDENTIFIER( D, MAX, V, L, (MMSd_tag) objId15 )

#define MMSd_emitUSER_OBJECTID( V, L, D )                                  \
    MMSd_emitTAG_and_LENGTH( objId15, MMSd_emitOBJECT_IDENTIFIER( V, L, D ), D )
#endif


#define    MMSd_initializeInformationReport( _E_, _Buf_, _size_ )    \
    {                                                           \
        MMSd_makeDescriptor(_E_,                                \
                            _Buf_,                              \
                            (MMSd_length) _size_,               \
                            (MMSd_length) (_size_ - 1) );       \
        (_E_).tokenLength = 0;                                  \
    }




/** Object Identifiers used throughout the code:
    First integer is length.  Second integer contains item1 * 40 + item 2, all others follow consecutively.
    For example, { 2, 81, 1 } is really ( 2, 1, 1 ) - ASN.1 BER.
    The 'folding' of the first two items is the way object Ids get encoded in ASN.1 */

/* Presentation contexts */
#define MMSD_BER_OBJECT_ID          { 2, 81, 1 }                  /* ASN.1 BER  (2,1,1) */
#define MMSD_ACSE_OBJECT_ID         { 4, 82, 1, 0, 1 }            /* ACSE   (2,2,1,0,1) */
#define MMSD_MMS_ED1_OBJECT_ID      { 4, 40, 9506, 2, 1 }         /* MMS Edition 1 (1,0,9506,2,1) */
#define MMSD_MMS_ED2_OBJECT_ID      { 4, 40, 9506, 2, 3 }         /* MMS Edition 2 (1,0,9506,2,3) */
#define MMSD_MMS_ASO_OBJECT_ID      { 4, 40, 9506, 1, 1 }         /* MMS ASO (1,0,9506,1,1) */
#define MMSD_STASE_MMS_OBJECT_ID    { 7, 40, 840, 0, 1, 1, 4, 1 } /* STASE MMS authentication value presentation context (1,0,840,0,1,1,4,1)*/

/* ACSE Authentication mechanisms */
#define MMSD_ACSE_MECHANISM_WEAK    { 3, 82, 3, 1 }               /* ACSE authentication mechanism - weak (2,2,3,1) */
#define MMSD_ACSE_MECHANISM_STRONG  { 7, 40, 840, 0, 1, 0, 1, 1 } /* ACSE authentication mechanism - strong (1,0,840,0,1,0,1,1) */

/* End ber.h */

#ifdef __cplusplus
};
#endif

#endif /* _BER_H */
