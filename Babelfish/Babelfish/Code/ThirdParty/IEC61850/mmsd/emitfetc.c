/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1994-2014 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                       www.TriangleMicroWorks.com                          */
/*                          (919) 870-6615                                   */
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
 *  MMS ASN1 Fetch and Emit Routines   .
 *
 *  This file should not require modification.
 */

/* #define USE_MEMCPY_FOR_FLOAT */

#if defined(USE_MEMCPY_FOR_FLOAT)
/* #include <memory.h> */
#endif

#include "sys.h" 

#include "tmwtarg.h"
#if !defined(GOOSE_ONLY_EMITFETCH)
#include "vendor.h"
#else
#endif
#include "ber.h"
#include "mms_erro.h"

#include "mms_tags.h"
#include "interfac.h"

/* These are the defaults per Ed2 8-1 Table A3.  They may be overridden in VENDOR.H */

#if !defined( MMSD_MAX_GOOSE_FIXED_OCTET_STRING_LENGTH )
#define MMSD_MAX_GOOSE_FIXED_OCTET_STRING_LENGTH        20
#endif

#if !defined( MMSD_MAX_GOOSE_FIXED_VISIBLE_STRING_LENGTH )
#define MMSD_MAX_GOOSE_FIXED_VISIBLE_STRING_LENGTH      35
#endif

#define MMSd_fetchTAG( D )                                          \
{                                                                   \
    MMSd_tag _tag_;                                                 \
    _tag_ = (MMSd_tag) MMSd_fetchDescOctet( D );                    \
    if ((_tag_ & 0x1F) == 0x1F)      /* -- Large Tag? */            \
    {                                                               \
        int _temp;                                                  \
        do {} while ((_temp = MMSd_fetchDescOctet( D )) == 0x80);   \
        _tag_ = (MMSd_tag) ((_tag_ << 8) | _temp);                  \
        while ((_tag_ & 0x80) != 0)                                 \
        {                                                           \
            _tag_ = (MMSd_tag) ((_tag_ << 8) | MMSd_fetchDescOctet( D )); \
        }                                                           \
    }                                                               \
    MMSd_DescTag( D ) = _tag_;                                      \
}

#define MMSd_fetchLENGTH( F )                                   \
{                                                               \
    (F).tokenLength = (MMSd_ushort) MMSd_fetchDescOctet( F );   \
    if (((F).tokenLength & 0x80) != 0)  /* length > 127? */     \
    {                                                           \
        if ((F).tokenLength == 0x80)    /* Indefinite Form */   \
        {                                                       \
            (F).tokenLength = INDEFINITE_LENGTH;                \
        }                                                       \
        else    /* tokenLength != 0x80) ** Definite Form */     \
        {                                                       \
            int i, octetCount = (F).tokenLength & 0x7F;         \
                                                                \
            (F).tokenLength = 0;                                \
            for (i = 0; i < octetCount; i++)                    \
            {                                                   \
                (F).tokenLength = (MMSd_ushort)                 \
                    (((F).tokenLength<<8) |                     \
                        (int) MMSd_fetchDescOctet( F ));        \
            }                                                   \
        }                                                       \
    }                                                           \
}

/* Local Prototypes */
MMSd_length MMSd_appendInfoObject(char * n,
                                  char *d,
                                  int extra,
                                  MMSd_descriptor *E);
MMSd_length MMSd_emitOBJECT_SUBIDENTIFIER(int val,
        MMSd_descriptor *E);

#ifdef DEBUG_MMSD
#include <stdio.h>
void MMSd_dump_descr(MMSd_descriptor *D);
#endif

#if defined(MMSD_FLOAT_FETCH) || defined(MMSD_FLOAT64_FETCH)
void MMSd_movebits(unsigned char *src,
                   int src_start,
                   int length,
                   unsigned char *dst,
                   int dst_start);

void MMSd_byteorder(unsigned char *valstr,
                    int length);
#endif

/* --------------------------------------------------------------
 --
 -- --- fetchTAG_and_LENGTH
 --
 --------------------------------------------------------------- */

void MMSd_fetchTAG_and_LENGTH(MMSd_descriptor *D)
{
#ifdef MMSD_FULL_STACK
    if (D->rules == session_encoding_rules) {
        MMSd_fetchSessionTAG( D );
        MMSd_fetchSessionLENGTH( D );
        return;
    }
#endif
    /* Use basic encoding rules */
    MMSd_fetchTAG( *D );
    MMSd_fetchLENGTH( *D );

	if (D->tokenLength > (D->bufLen - D->index))
		MMSd_setDescErrorFlag(MMSd_pdu_error, *D);
}


#if defined(DEBUG_ERROR_HANDLING)
/**
 * In normal operation there is a macro (MMSd_setDescErrorFlag, which is also used by macro MMSd_setErrorFlag)
 * which set error codes in the MMS descriptors.  It is sometimes convenient when debugging to be able to
 * set a breakpoint on the setting of either a particular error condition or on any error condition.  The
 * macro MMSD_DEBUG_ERROR_HANDLING can be used to switch the error setting macros to use this
 * routine instead for that purpose.  Production code should have this turned off for efficiency.
 */

void MMSd_setDebugDescErrorFlag( unsigned long nErrorFlag, MMSd_descriptor *pDescriptor )
{
    switch ( nErrorFlag ) {
    case MMSd_noHandlerErrors:
        break;
    case MMSd_emitOverflow:
        break;
    case MMSd_fetchLengthError:
        break;
    case MMSd_data_object_invalidated:
        break;
    case MMSd_data_hardware_fault:
        break;
    case MMSd_data_temporarily_unavailable:
        break;
    case MMSd_data_object_access_denied:
        break;
    case MMSd_data_object_undefined:
        break;
    case MMSd_data_invalid_address:
        break;
    case MMSd_data_type_unsupported:
        break;
    case MMSd_data_type_inconsistent:
        break;
    case MMSd_data_object_attribute_inconsistent:
        break;
    case MMSd_data_object_access_unsupported:
        break;
    case MMSd_data_object_non_existent:
        break;
    case MMSd_queueOverrun:
        break;
    case MMSd_null_descriptor:
        break;
    case MMSd_IOargs:
        break;
    case MMSd_neg_svc_error:
        break;
    case MMSd_data_object_value_invalid:
        break;
    case MMSd_search_error:
        break;
    case MMSd_pdu_error:
        break;
    case MMSd_file_other:
        break;
    case MMSd_file_file_busy:
        break;
    case MMSd_file_name_syntax_error:
        break;
    case MMSd_file_content_invalid:
        break;
    case MMSd_file_file_access_denied:
        break;
    case MMSd_file_file_non_existent:
        break;
    case MMSd_file_duplicate_filename:
        break;
    case MMSd_file_insufficient_space:
        break;
    case MMSd_file_position_invalid:
        break;
    case MMSd_authentication_failure:
        break;
    case MMSd_authentication_mech_nr:
        break;
    case MMSd_uninitiatedMessage:
        break;
    default:
        break;
    }

    pDescriptor->errorFlag = nErrorFlag;
}
#endif  /* DEBUG_ERROR_HANDLING */

/* --------------------------------------------------------------
 --------------------------------------------------------------
 ------------------------ FETCH Routines ----------------------
 --------------------------------------------------------------
 -------------------------------------------------------------- */

/* --------------------------------------------------------------
 --
 -- --- checkTag
 --
 -- This macro checks the TAG of the data TOKEN it is attempting
 -- to read from.
 --
 -- When used by an "internal" routine, the tag passed is "noTag".
 -- In this case, no check is performed.
 --
 -- For USER routines, if the tag passed in does not match the
 -- tag of the TOKEN, the routine returns a FAIL indication.
 --
 --------------------------------------------------------------- */

#define checkTag( _tag_, _TKN_ )                                     \
{                                                                    \
    if ((_tag_ != noTag) && ((_TKN_).tag != _tag_)) {                \
        MMSd_setDescErrorFlag( MMSd_data_type_inconsistent, _TKN_ ); \
        return( 0 );                                                 \
}   }

/* --------------------------------------------------------------
 --
 -- --- checkErrors
 --
 -- This macro checks errorFlags of the data TOKEN it is
 -- attempting to read from.
 --
 --------------------------------------------------------------- */
#define checkErrors( _TKN_ )        \
{                                   \
    if (!MMSd_noError( (_TKN_) ))   \
        return( 0 );                \
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_fetch LONG
 --
 --------------------------------------------------------------- */

int MMSd_fetchLONG(MMSd_descriptor *TKN,
                   TMW_INT64 *lval,
                   MMSd_tag tag)
{
    int length;
    TMW_INT64 value;

    checkTag( tag, (*TKN) );
    checkErrors( (*TKN) );

    length = MMSd_TokenLen( *TKN );
    value = ( MMSd_getDescOctet( *TKN ) & 0x80) ? -1 : 0;
    while (length-- > 0) {
        value = ((value << 8) & ~0xFF) | MMSd_fetchDescOctet( *TKN );
    }
    *lval = value;
    return (1);
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_fetch UNSIGNEDLONG    (internal version)
 --
 --------------------------------------------------------------- */

int MMSd_fetchUNSIGNEDLONG(MMSd_descriptor *TKN,
                           unsigned long *ulval,
                           MMSd_tag tag)
{
    MMSd_length length;
    unsigned long value;

    checkTag( tag, (*TKN) );
    checkErrors( (*TKN) );

    length = MMSd_TokenLen( *TKN );
    value = 0;
    while (length-- > 0) {
        value = ((value << 8) & ~0xFF) | MMSd_fetchDescOctet( *TKN );
    }
    *ulval = value;
    return (1);
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchINTEGER  (internal version)
 --
 --------------------------------------------------------------- */

int MMSd_fetchINTEGER(MMSd_descriptor *TKN,
                      int *ival,
                      MMSd_tag tag)
{
    TMW_INT64 lval;
    int retvalue;

    retvalue = MMSd_fetchLONG( TKN, &lval, tag );
    if (retvalue)
        *ival = (int) lval;
    return (retvalue);
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_fetch UNSIGNEDINTEGER (internal version)
 --
 --------------------------------------------------------------- */

int MMSd_fetchUNSIGNEDINTEGER(MMSd_descriptor *TKN,
                              unsigned int *uival,
                              MMSd_tag tag)
{
    unsigned long ulval;
    int retvalue;

    retvalue = MMSd_fetchUNSIGNEDLONG( TKN, &ulval, tag );
    if (retvalue)
        *uival = (unsigned int) ulval;
    return (retvalue);
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchBITSTRING
 --
 --------------------------------------------------------------- */
/* BUG Fix - Changed to be bit oriented for both fetch and emit */
int MMSd_fetchBITSTRING(MMSd_descriptor *TKN,
                        unsigned char *outFlags,
                        int outBitLen,
                        MMSd_tag tag)
{
    int i, inByteLen, outByteLen, maxByteLen, unusedBits;
    unsigned char c;

    checkTag( tag, (*TKN) );
    checkErrors( (*TKN) );

    /* -- First byte of ASN1 has # of unused bits at the end */

    outByteLen = outBitLen >> 3;
    if (outBitLen & 0x7)
        outByteLen++;

    inByteLen = MMSd_TokenLen( *TKN ) - 1;
    unusedBits = MMSd_fetchDescOctet( *TKN );
    maxByteLen = (inByteLen > outByteLen) ? inByteLen : outByteLen;

    for (i = 0; i < maxByteLen; i++) {
        if (i < inByteLen) {
            c = MMSd_fetchDescOctet( *TKN );
            if (i == (inByteLen - 1))
                c &= 0xFF << unusedBits;
        } else
            c = 0;
        if (i < outByteLen) {
            *outFlags++ = c;
        }
    }

    return (1);
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchOCTETS
 --
 --------------------------------------------------------------- */

int MMSd_fetchOCTETS(MMSd_descriptor *TKN,
                     unsigned char *octets,
                     int outLen,
                     MMSd_tag tag)
{
    MMSd_length i, length;

    checkTag( tag, (*TKN) );
    checkErrors( (*TKN) );

    /* We used to silently accept (but truncate) writes of Octet and Visible
     strings that were too long.  Now we flag the error. */
    length = MMSd_TokenLen( *TKN );
    if (length > outLen) {
        MMSd_setDescErrorFlag( MMSd_data_type_inconsistent, *TKN );
        return (0);
    }

    for (i = 0; i < length; i++) {

            *(octets + i) = MMSd_fetchDescOctet( *TKN );

    }

    return (i);
}

#ifdef MMSD_STRING_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchSTRING
 --
 --------------------------------------------------------------- */

int MMSd_fetchSTRING(MMSd_descriptor *TKN,
                     char *str,
                     int outLen,
                     MMSd_tag tag)
{

    if (MMSd_fetchOCTETS( TKN, (unsigned char *) str, outLen, tag )) {
        if (outLen > MMSd_TokenLen( *TKN ))
            outLen = MMSd_TokenLen( *TKN );
        str[outLen] = '\0';
        return (1);
    } else {
        str[0] = '\0';
        return (0);
    }
}
#endif

#if defined(MMSD_FLOAT_FETCH) || defined(MMSD_FLOAT64_FETCH)
/* --------------------------------------------------------------
 --
 -- --- MMSd_movebits
 --
 -- This is used in fetchFLOAT (below) for manipulating bits
 -- in the floating point representation
 --
 --------------------------------------------------------------- */
void MMSd_movebits(unsigned char *src,
                   int src_start,
                   int length,
                   unsigned char *dst,
                   int dst_start)
{
    int srcByteNum, srcOffset;
    int srcHiMask, srcLoMask;
    int dstByteNum, dstOffset, dstRemain;
    int dstHiMask, dstLoMask;
    unsigned char srcVal, dstVal1, dstVal2;
    int bitNum;

    srcByteNum = src_start / 8;
    srcOffset = src_start % 8;
    srcHiMask = 0xff << (8 - srcOffset);
    srcLoMask = 0xff >> srcOffset;

    dstByteNum = dst_start / 8;
    dstOffset = dst_start % 8;
    dstRemain = (dst_start + length) % 8;
    dstHiMask = 0xff << (8 - dstOffset);
    dstLoMask = 0xff >> dstOffset;

    bitNum = 0;
    while (bitNum < length) {
        /* Fetch one full byte (although not aligned!) */
        if (srcOffset != 0)
            srcVal = (unsigned char) (((src[srcByteNum] & srcLoMask) << srcOffset)
                                      | ((src[srcByteNum + 1] & srcHiMask) >> (8 - srcOffset)));
        else
            srcVal = src[srcByteNum];

        /* Store one full byte (although not aligned!) */
        if (dstOffset != 0) {
            dstVal1 = (unsigned char) ((dst[dstByteNum] & dstHiMask) | ((srcVal >> dstOffset) & dstLoMask));
            dstVal2 = (unsigned char) (((srcVal << (8 - dstOffset)) & dstHiMask) | (dst[dstByteNum + 1] & dstLoMask));
        } else {
            dstVal1 = srcVal;
            dstVal2 = dst[dstByteNum + 1];
        }

        if ((bitNum + 8) > length) { /* We are at the last one */
            if (dstRemain > 0)
                dstVal1 = (unsigned char) ((dstVal1 & (0xff << (8 - dstRemain)))
                                           | (dst[dstByteNum] & (0xff >> dstRemain)));
            dstVal2 = dst[dstByteNum + 1];
        }

        dst[dstByteNum] = dstVal1;
        dst[dstByteNum + 1] = dstVal2;

        srcByteNum++;
        dstByteNum++;
        bitNum = bitNum + 8;
    }
}
void MMSd_byteorder(unsigned char *valstr,
                    int length)
{
    int i;
    unsigned char temp;

    for (i = 0; i < (length / 2); i++) {
        temp = valstr[i];
        valstr[i] = valstr[length - i - 1];
        valstr[length - i - 1] = temp;
    }
}
#endif

#if defined(MMSD_FLOAT_FETCH)
/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchFLOAT
 --
 --------------------------------------------------------------- */
#define MMSD_FLOAT_SIGNIFICAND_WIDTH \
    (((int) sizeof(MMSD_FLOAT_DATA_TYPE) * 8) - MMSD_FLOAT_EXPONENT_WIDTH - 1)
#define LONG_BYTE_SIZE ( (int) sizeof(TMW_INT64) )
#define LONG_BIT_SIZE (LONG_BYTE_SIZE * 8)
#define FLOAT_BYTE_SIZE ( (int) sizeof(MMSD_FLOAT_DATA_TYPE))

int MMSd_convertFloatTypeToInternal(unsigned char *buffer, /* (after 1st byte) */
                                    int sig_bit_len,
                                    int exp_bit_len,
                                    int out_floatBitSize,
                                    int out_floatExponentWidth)
{
    int i, use_significand;
    TMW_INT64 exp_val, exp_bias;
    int floatByteSize = (out_floatBitSize + 7) / 8;
    unsigned char new1[12];

    if (exp_bit_len >= LONG_BIT_SIZE)
        return (0);

    /* -- We must convert to the local representation.
     -- This code assumes that the local machine uses some
     -- form of IEEE floating point, with an arbitrary exponent
     -- and significand width.
     */
    for (i = 0; i < floatByteSize; i++)
        new1[i] = 0;

    exp_val = 0L;
    use_significand = 1;
    MMSd_movebits( buffer, 1, exp_bit_len, (unsigned char *) &exp_val,
                   LONG_BIT_SIZE - exp_bit_len );

#ifdef MMSD_FLOAT_INTEL_BYTE_ORDERING
    MMSd_byteorder( (unsigned char *) &exp_val, LONG_BYTE_SIZE );
#endif

    if (exp_val != 0) {
        if (exp_val == ((1L << exp_bit_len) - 1)) {
            exp_val = -1;
        } else {
            /* remove transmitted bias */
            exp_val = exp_val - ((1L << (exp_bit_len - 1)) - 1);

            /* Compute local bias */
            exp_bias = (1L << (out_floatExponentWidth - 1)) - 1;
            if (exp_val >= exp_bias) {
                exp_val = -1;
                use_significand = 0;
            } else {
                if (exp_val <= -exp_bias) {
                    exp_val = 0;
                    use_significand = 0;
                } else {
                    exp_val = exp_val + exp_bias;
#ifdef MMSD_FLOAT_INTEL_BYTE_ORDERING
                    MMSd_byteorder( (unsigned char *) &exp_val, sizeof(TMW_INT64) );
#endif
                }
            }
        }
    }
    /* Copy sign bit (rest is overwritten later */
    new1[0] = buffer[0];

    /* Move exponent into place */
    MMSd_movebits( (unsigned char *) &exp_val, (LONG_BIT_SIZE - out_floatExponentWidth), out_floatExponentWidth, new1,
                   1 );

    /* Move significand into place */
    if (use_significand) {
        if (sig_bit_len > (out_floatBitSize - out_floatExponentWidth - 1))
            sig_bit_len = out_floatBitSize - out_floatExponentWidth - 1;

        MMSd_movebits( buffer, exp_bit_len + 1, sig_bit_len, new1, out_floatExponentWidth + 1 );
    }

    for (i = 0; i < floatByteSize; i++)
        buffer[i] = new1[i];

    return (1);
}

int MMSd_fetchFLOAT(MMSd_descriptor *TKN,
                    MMSD_FLOAT_DATA_TYPE *fval,
                    MMSd_tag tag)
{
    int float_byte_len, exp_bit_len, sig_bit_len;
    unsigned char buffer[12]; /* Its larger than we should ever need! */

    checkTag( tag, (*TKN) );
    checkErrors( (*TKN) );

    float_byte_len = MMSd_TokenLen( *TKN ) - 1;
    if (MMSd_fetchOCTETS( TKN, buffer, sizeof(buffer), tag )) {
        exp_bit_len = buffer[0];
        sig_bit_len = (float_byte_len * 8) - exp_bit_len - 1;
        if ((exp_bit_len != MMSD_FLOAT_EXPONENT_WIDTH) || (float_byte_len != ((int) sizeof(MMSD_FLOAT_DATA_TYPE)))) {
            if (!MMSd_convertFloatTypeToInternal( &buffer[1], /* (after 1st byte) */
                                                  sig_bit_len, exp_bit_len, ((int) sizeof(MMSD_FLOAT_DATA_TYPE)) * 8,
                                                  MMSD_FLOAT_EXPONENT_WIDTH )) {
                MMSd_setDescErrorFlag( MMSd_data_type_inconsistent, *TKN );
                return (0);
            }

        };

#ifdef MMSD_FLOAT_INTEL_BYTE_ORDERING
        MMSd_byteorder( buffer + 1, ((int) sizeof(MMSD_FLOAT_DATA_TYPE)) );
#endif
#if defined(USE_MEMCPY_FOR_FLOAT)
        memcpy( fval, (buffer + 1), sizeof(MMSD_FLOAT_DATA_TYPE) );
#else
        *fval = *( (MMSD_FLOAT_DATA_TYPE *) (buffer + 1));
#endif
        return (1);
    }
    return (0);
}
#endif

#if defined(MMSD_FLOAT64_FETCH)
int MMSd_fetchFLOAT64(MMSd_descriptor *TKN,
                      MMSD_FLOAT64_DATA_TYPE *fval,
                      MMSd_tag tag)
{
    int float_byte_len, exp_bit_len, sig_bit_len;
    unsigned char buffer[12]; /* Its larger than we should ever need! */

    checkTag( tag, (*TKN) );
    checkErrors( (*TKN) );

    float_byte_len = MMSd_TokenLen( *TKN ) - 1;
    if (MMSd_fetchOCTETS( TKN, buffer, sizeof(buffer), tag )) {
        exp_bit_len = buffer[0];
        sig_bit_len = (float_byte_len * 8) - exp_bit_len - 1;
        if ((exp_bit_len != MMSD_FLOAT_EXPONENT_WIDTH) || (float_byte_len != ((int) sizeof(MMSD_FLOAT64_DATA_TYPE)))) {
            if (!MMSd_convertFloatTypeToInternal( &buffer[1], /* (after 1st byte) */
                                                  sig_bit_len, exp_bit_len, ((int) sizeof(MMSD_FLOAT64_DATA_TYPE)) * 8,
                                                  MMSD_FLOAT64_EXPONENT_WIDTH )) {
                MMSd_setDescErrorFlag( MMSd_data_type_inconsistent, *TKN );
                return (0);
            }
        };

#ifdef MMSD_FLOAT_INTEL_BYTE_ORDERING
        MMSd_byteorder( buffer + 1, ((int) sizeof(MMSD_FLOAT64_DATA_TYPE)) );
#endif
#if defined(USE_MEMCPY_FOR_FLOAT)
        memcpy( fval, (buffer + 1), sizeof(MMSD_FLOAT64_DATA_TYPE) );
#else
        *fval = *( (MMSD_FLOAT64_DATA_TYPE *) (buffer + 1));
#endif
        return (1);
    }
    return (0);
}
#endif

#ifdef MMSD_BCD_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchBCD    -----UNIMPLEMENTED----
 --
 -- BCD is defined in MMS as transmitting an INTEGER value.
 -- The mapping to the local representation of BCD is missing here.
 -- The value will be returned as a normal binary number.
 --
 --------------------------------------------------------------- */

int MMSd_fetchBCD(MMSd_descriptor *TKN,
                  TMW_INT64 *lval,
                  MMSd_tag tag)
{
    int returnCode;
    TMW_INT64 value;

    returnCode = MMSd_fetchLONG( TKN, &value, tag );

    if (returnCode)
        *lval = value;

    return (returnCode);
}
#endif

#ifdef MMSD_REAL_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchREAL   -----UNIMPLEMENTED----
 --
 -- But it is set up to consume the proper amount from the input PDU
 --
 --------------------------------------------------------------- */

int MMSd_fetchREAL(
    MMSd_descriptor *TKN,
    double *val,
    MMSd_tag tag )
{
    unsigned char buffer[ 8 ];
    float fval;

    MMSd_fetchOCTETS( TKN, buffer, sizeof(buffer), tag);
    fval = *( (float *) buffer );
    *val = (double) fval;
    return(1);
}
#endif

#ifdef MMSD_BINARYTIME_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchBINARYTIME
 --
 --------------------------------------------------------------- */

int MMSd_fetchBINARYTIME(MMSd_descriptor *TKN,
                         MMSd_time *tval,
                         MMSd_tag tag)
{
    int i;
    unsigned long temp;
    unsigned char buffer[6];

    checkTag( tag, (*TKN) );
    checkErrors( (*TKN) );

    temp = 0L;
    buffer[4] = 0;
    buffer[5] = 0;
    MMSd_fetchOCTETS( TKN, buffer, sizeof(buffer), noTag );
    for (i = 0; i < 4; i++) {
        temp = (temp << 8);
        temp = temp | buffer[i];
    }
    tval->millisecondsSinceMidnight = temp;
    tval->daysSinceJan1_1984 = (buffer[4] << 8) | buffer[5];
    return (1);
}
#endif

#ifdef MMSD_UTCTIME_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchUTCTIME
 --
 --------------------------------------------------------------- */

int MMSd_fetchUTCTIME(MMSd_descriptor *TKN,
                      MMSd_UtcBTime *tval,
                      MMSd_tag tag)
{
    int i;
    unsigned long temp;
    unsigned char buffer[8];

    checkTag( tag, (*TKN) );
    checkErrors( (*TKN) );

    if ( sizeof(buffer) != MMSd_fetchOCTETS( TKN, buffer, sizeof(buffer), noTag ) )
        return 0;

    temp = 0L;
    for (i = 0; i < 4; i++) {
        temp = (temp << 8);
        temp = temp | buffer[i];
    }
    tval->secondOfCentury = temp;

    temp = 0L;
    for (i = 0; i < 3; i++) {
        temp = (temp << 8);
        temp = temp | buffer[i + 4];
    }
    tval->fractionOfSecond = temp << 8;
    tval->quality = (MMSd_UtcQuality) buffer[7];
    return (1);
}
#endif

#ifdef MMSD_OBJECT_IDENTIFIER_TYPE
int MMSd_fetchOBJECT_IDENTIFIER(MMSd_descriptor *TKN,
                                int limit,
                                int *oval,
                                int *num_o,
                                MMSd_tag tag)
{
    int length, val, i, byt, num_items, hi_bit;

    checkTag( tag, (*TKN) );
    checkErrors( (*TKN) );
    limit = limit / sizeof(int);

    length = MMSd_TokenLen( *TKN );
    val = 0;
    num_items = 0;
    for (i = 0; i < length; i++) {
        byt = MMSd_fetchDescOctet( *TKN );
        hi_bit = byt & 0x80;
        val = (val << 7) | (byt & 0x7f);
        if (!hi_bit) { /* We have a sub-item */
            if (num_items < limit ) {
                oval[num_items] = val;
                num_items++;
                val = 0;
            } else {
                MMSd_setDescErrorFlag( MMSd_pdu_error, *TKN );
                return (0);
            }
        }
    }
    if (val) {
        MMSd_setDescErrorFlag( MMSd_pdu_error, *TKN );
        return (0);
    }
    *num_o = num_items;
    return (1);
}
#endif

/* --------------------------------------------------------------
 --
 -- --- MMSd_fetchCOMPLETE
 --
 -- Tests an initial and final fetch descriptor to assure that
 -- exactly the right amount has been fetched. This is used
 -- to assure that structures are processed properly in handlers.
 --------------------------------------------------------------- */
boolean MMSd_fetchCOMPLETE(MMSd_descriptor *des1,
                           MMSd_descriptor *des2)
{
    if ((MMSd_getDescPtr( *des1 ) + MMSd_TokenLen( *des1 )) != MMSd_getDescPtr( *des2 ))
        return ( FALSE);
    else
        return ( TRUE);
}

/* --------------------------------------------------------------
 --------------------------------------------------------------
 ------------------------ EMIT Routines -----------------------
 --------------------------------------------------------------
 --------------------------------------------------------------- */

void MMSd_EMITOctet(int octet,
                    MMSd_descriptor *E)
{
    int key;

    key = (MMSdEmitDirectionMASK | MMSdEmitEndMASK) & E->tag;
    switch (key) {
    case (MMSdEmitFront | MMSdEmitInc):
        if ((E->index >= E->bufLen) || (E->index >= E->tokenLength))
            break;
        MMSd_appendFront( octet, E );
        return;
    case (MMSdEmitBack | MMSdEmitInc):
        if (E->tokenLength >= E->bufLen)
            break;
        MMSd_appendBack( octet, E );
        return;
    case (MMSdEmitFront | MMSdEmitDec):
        if (E->index <= 0)
            break;
        MMSd_prependFront( octet, E );
        return;
    case (MMSdEmitBack | MMSdEmitDec):
        if ((E->tokenLength <= 0) || (E->tokenLength <= E->index))
            break;
        MMSd_prependBack( octet, E );
        return;
    default:
        return;
    }
    MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_EMITValue
 --
 --  There is a problem when emitting integers which is
 --  complementary for POSITIVE and NEGATIVE values.
 --
 --  For a positive number, if the high order bit of the
 --  leftmost octet is '1', a leading octet of 0x00 must
 --  be added to enable the value to be decoded as a
 --  a positive number.
 --
 --  For a negative number, if the high order bit of the
 --  leftmost octet is '0', a leading octet of 0xFF must
 --  be added to permit decoding the value as negative.
 --
 --  The expression  "(!(value & 0x80) ^ isPositive)"
 --  will determine if either of the above cases occurs.
 --  If TRUE, it will cause the loop to iterate 1 additional
 --  time causing a "Sign Octet" to be written to the output buffer.
 --
 --  The C "!" operator maps an expression into the boolean
 --  domain (i.e. {0,1}).  In the case of "comparand" below, this
 --  means that the loop terminating value will be either "0" or "-1"
 --  (which will be the end result for ARITHMETIC shift operations which
 --  start with positive or negative values).
 --
 --  For the FLAG expression, the following truth table tells it all:
 --
 --           isPositive !(value & 0x80)   XOR
 --                 0            0          0
 --                 0            1          1
 --                 1            0          1
 --                 1            1          0
 --
 --  In the second case (a NEGATIVE number whose octet bit is
 --  NOT set) and in the third case (a POSTIVE number whose
 --  octet bit IS set), the result of the XOR will cause the loop
 --  to iterate one extra time to EMIT the required sign octet.
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_EMITValue(TMW_INT64 value,
                           MMSd_descriptor *E)
{
    boolean flag;
    boolean isPositive = (boolean) (value >= 0LL);
    MMSd_length length = 0;
    TMW_INT64 comparand = -(!isPositive); /* domain { 0, -1 } */

    do {
        /* bitSet == isPositive */
        flag = (boolean) (!(value & 0x80L) ^ isPositive);
        MMSd_EMITOctet( (int) (value & 0xFFL), E );
        value = value >> 8;
        length += 1;
    } while ((value != comparand) || flag);

    return (length);
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_EMITUnsignedValue
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_EMITUnsignedValue(unsigned long value,
                                   MMSd_descriptor *E)
{
    boolean flag;
    MMSd_length length = 0;

    do {
        flag = (boolean) ((value & 0x80L) != 0);
        MMSd_EMITOctet( (int) (value & 0xFF), E );
        value = value >> 8;
        length += 1;
    } while ((value != 0) || (flag));

    return (length);
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_emitTAG_and_LENGTH        Space saving version
 --
 --------------------------------------------------------------- */
static int doEMIT(MMSd_ushort value,
                  MMSd_descriptor *E);
static int doEMIT(MMSd_ushort value,
                  MMSd_descriptor *E)
{
    MMSd_length length = 0;

    do {
        MMSd_EMITOctet( value, E );
        length += 1;
        value = (MMSd_length) (value >> 8);
    } while (value > 0);
    return (length);
}

MMSd_length MMSd_emitTAG_and_LENGTH(MMSd_tag TOKEN,
                                    MMSd_length length, /* Sub-message len */
                                    MMSd_descriptor *E)
{
    MMSd_length size;

#ifdef MMSD_FULL_STACK
    if (E->rules == session_encoding_rules) {
        size = (MMSd_length) (MMSd_emitSessionLENGTH( length, E ));
        size = (MMSd_length) (size + MMSd_emitSessionTAG( TOKEN, E ));
        return ((MMSd_length) (length + size));
    }
#endif
    size = (MMSd_length) (doEMIT( length, E ));
    if (length > 127)
        MMSd_EMITOctet( 0x80 | size++, E );

    size = (MMSd_length) (size + doEMIT( TOKEN, E ));

    return ((MMSd_length) (length + size));
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_emitOCTETS
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitOCTETS(unsigned char *o,
                            MMSd_length len,
                            MMSd_descriptor *E)
{
    MMSd_length length = len;

    while (length > 0) {
        MMSd_EMITOctet( o[length - 1], E );
        length--;
    }
    return (len);
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_emitBITSTRING
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitBITSTRING(unsigned char *s,
                               MMSd_length len,
                               MMSd_descriptor *E)
{
    MMSd_length length, emitLength;
    int unused_bits;

    if (len <= 0)
        return (0);

    length = (MMSd_length) (len >> 3); /* length = len / 8; */
    unused_bits = len & 0x7; /* unused_bits = (len % 8); */
    emitLength = (MMSd_length) (length + 1);

    if (unused_bits != 0) {
        emitLength++;
        MMSd_EMITOctet( s[length] & (0xff << (8 - unused_bits)), E );
    }
#if 1
    while (length > 0) {
        MMSd_EMITOctet( s[length - 1], E );
        length--;
    }
    if (unused_bits != 0) {
        MMSd_EMITOctet( (8 - unused_bits), E );
    } else {
        MMSd_EMITOctet( 0, E );
    }
#else
    while ( length-- > 0 ) {
        MMSd_EMITOctet( s[ length ], E);
    }
    MMSd_EMITOctet( ((unused_bits != 0) ? (8 - unused_bits) : 0), E );
#endif

    return (emitLength);
}

/* ---------------------------------------------------------------
 ---
 --- --- MMSd_stringLength
 ---
 --------------------------------------------------------------- */

int MMSd_stringLength(char *strPtr)
{
    int length = 0;

    if (strPtr == NULL)
        return (0);

    while ((*strPtr) != 0) {
        strPtr++;
        length++;
    }
    return (length);
}

#ifdef MMSD_STRING_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_emitSTRING
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitSTRING(char *s,
                            MMSd_descriptor *E)
{
    int length;

    length = MMSd_stringLength( s );
    return (MMSd_emitOCTETS( (unsigned char *) s, (MMSd_length) length, E ));
}
#endif

#ifdef MMSD_FLOAT_EMIT
/* --------------------------------------------------------------
 --
 -- --- MMSd_emitFLOAT
 --
 -- Insert OCTETS into the outgoing message
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitFLOAT( MMSD_FLOAT_DATA_TYPE val,
                            MMSd_descriptor *E)
{
    unsigned char buffer[sizeof(MMSD_FLOAT_DATA_TYPE) + 1];

#ifdef MMSD_FLOAT_INTEL_BYTE_ORDERING
    int i;
    unsigned char *valPtr = (unsigned char *) &val;
    for (i = 0; i < (int) sizeof(MMSD_FLOAT_DATA_TYPE); i++) {
        buffer[sizeof(MMSD_FLOAT_DATA_TYPE) - i] = valPtr[i];
    }
#else

#if defined(USE_MEMCPY_FOR_FLOAT)
    memcpy( (buffer + 1), &val, sizeof(MMSD_FLOAT_DATA_TYPE));
#else
    MMSD_FLOAT_DATA_TYPE *buffPtr = ((MMSD_FLOAT_DATA_TYPE *) (buffer + 1));
    *buffPtr = val;
#endif

#endif
    buffer[0] = MMSD_FLOAT_EXPONENT_WIDTH;
    return (MMSd_emitOCTETS( buffer, sizeof(MMSD_FLOAT_DATA_TYPE) + 1, E ));
}
#endif

#ifdef MMSD_FLOAT64_EMIT
MMSd_length MMSd_emitFLOAT64( MMSD_FLOAT64_DATA_TYPE val,
                              MMSd_descriptor *E)
{
    unsigned char buffer[sizeof(MMSD_FLOAT64_DATA_TYPE) + 1];

#ifdef MMSD_FLOAT_INTEL_BYTE_ORDERING
    int i;
    unsigned char *valPtr = (unsigned char *) &val;
    for (i = 0; i < (int) sizeof(MMSD_FLOAT64_DATA_TYPE); i++) {
        buffer[sizeof(MMSD_FLOAT64_DATA_TYPE) - i] = valPtr[i];
    }
#else

#if defined(USE_MEMCPY_FOR_FLOAT)
    memcpy( (buffer + 1), &val, sizeof(MMSD_FLOAT64_DATA_TYPE));
#else
    MMSD_FLOAT64_DATA_TYPE *buffPtr = ((MMSD_FLOAT64_DATA_TYPE *) (buffer + 1));
    *buffPtr = val;
#endif

#endif
    buffer[0] = MMSD_FLOAT64_EXPONENT_WIDTH;
    return (MMSd_emitOCTETS( buffer, sizeof(MMSD_FLOAT64_DATA_TYPE) + 1, E ));
}
#endif

#ifdef MMSD_REAL_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_emitREAL    -----UNIMPLEMENTED----
 --
 -- Insert OCTETS into the outgoing message
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitREAL( double val, MMSd_descriptor *E )
{
    float fval;

    fval = (float) val;
    return( MMSd_emitOCTETS( (unsigned char *) &fval, sizeof(fval), E ) );
}
#endif

#ifdef MMSD_BCD_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_emitBCD
 --
 -- BCD is defined in MMS as transmitting an INTEGER value.
 -- The mapping from the local representation of BCD is missing here.
 -- The value must be supplied as a normal binary number.
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitBCD(TMW_INT64 val,
                         MMSd_descriptor *E)
{

    return (MMSd_EMITValue( val, E ));
}
#endif

#ifdef MMSD_BINARYTIME_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_emitBINARYTIME
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitBINARYTIME(MMSd_time *t,
                                int size,
                                MMSd_descriptor *E)
{
    int i, length;
    TMW_INT64 ival;
    unsigned char buffer[6];

    length = 6; /* Default to BTIME6 if size is illegal */
    if ((size == 4) || (size == 6))
        length = size;

    ival = t->millisecondsSinceMidnight;
    for (i = 0; i < 4; i++) {
        buffer[3 - i] = (unsigned char) (ival & 0xff);
        ival = ival >> 8;
    }
    if (length == 6) {
        buffer[5] = (unsigned char) (t->daysSinceJan1_1984 & 0xff);
        buffer[4] = (unsigned char) ((t->daysSinceJan1_1984 >> 8) & 0xff);
    }
    return (MMSd_emitOCTETS( buffer, (MMSd_length) length, E ));
}
#endif

#ifdef MMSD_UTCTIME_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_emitUTCTIME
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitUTCTIME(MMSd_UtcBTime *t,
                             int size,
                             MMSd_descriptor *E)
{
    int i;
    TMW_INT64 ival;
    unsigned char buffer[8];

    if (size != 8)
        return (0);

    ival = t->secondOfCentury;
    for (i = 0; i < 4; i++) {
        buffer[3 - i] = (unsigned char) (ival & 0xff);
        ival = ival >> 8;
    }
    ival = t->fractionOfSecond >> 8;
    for (i = 0; i < 3; i++) {
        buffer[6 - i] = (unsigned char) (ival & 0xff);
        ival = ival >> 8;
    }

    buffer[7] = (unsigned char) (t->quality & 0xff);
    return (MMSd_emitOCTETS( buffer, (MMSd_length) size, E ));
}
#endif

#ifdef MMSD_OBJECT_IDENTIFIER_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_emitOBJECT_IDENTIFIER
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitOBJECT_SUBIDENTIFIER(int val,
        MMSd_descriptor *E)
{
    int byt, hi_bit;
    MMSd_length emit_len;

    emit_len = 0;
    hi_bit = 0;
    do {
        byt = val & 0x7F;
        MMSd_EMITOctet( byt | hi_bit, E );
        val = val >> 7;
        if (val)
            hi_bit = 0x80;
        else
            hi_bit = 0;
        emit_len++;
    } while (val);

    return (emit_len);
}


/* --------------------------------------------------------------------------
--- pObject is a pointer to the object name to be emitted
--- E is the descriptor of the output buffer.
--- returns length of emitted field
--------------------------------------------------------------------------- */
MMSd_length  MMSd_emitOName( MMSd_ObjectName *pObject, MMSd_descriptor *E )
{
    MMSd_length     len;

    switch (pObject->scope) {
    case MMSdScope_vmdSpecific:
        len = MMSd_emitVISIBLESTRING(vmdSpecific0, (char *) pObject->name, E );
        break;
    case MMSdScope_domainSpecific:
        len = MMSd_emitVISIBLESTRING( itemID, (char *) pObject->name, E ),
              len = len + MMSd_emitVISIBLESTRING( domainID, (char *) pObject->domain, E ),
                    len = MMSd_emitTAG_and_LENGTH( domainSpecific1_1, len, E );
        break;
    case MMSdScope_aaSpecific:
        len = MMSd_emitVISIBLESTRING( aaSpecific2, (char *) pObject->name, E );
        break;
    default:
        return( 0 );
    }
    return( len );
}


MMSd_length MMSd_emitOBJECT_IDENTIFIER(int *o,
                                       int len,
                                       MMSd_descriptor *E)
{

    int i;
    MMSd_length emit_len;

    emit_len = 0;
    if (len <= 1) {
        MMSd_setDescErrorFlag( MMSd_pdu_error, *E );
        return (0);
    }
    for (i = len - 1; i >= 0; i--) {
        emit_len = (MMSd_length) (emit_len + MMSd_emitOBJECT_SUBIDENTIFIER( o[i], E ));

    }

    return (emit_len);
}
#endif

/* --------------------------------------------------------------
 --
 -- --- MMSd_emitTAGGEDUNSIGNEDVALUE
 --
 --------------------------------------------------------------- */
MMSd_length MMSd_emitTAGGEDUNSIGNEDVALUE(MMSd_tag tag,
        unsigned long value,
        MMSd_descriptor *E)
{
    return (MMSd_emitTAG_and_LENGTH( tag, (MMSd_length) MMSd_EMITUnsignedValue( value, E ), E ));
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_emitTAGGEDVALUE
 --
 --------------------------------------------------------------- */
MMSd_length MMSd_emitTAGGEDVALUE(MMSd_tag tag,
                                 TMW_INT64 value,
                                 MMSd_descriptor *E)
{
    return (MMSd_emitTAG_and_LENGTH( tag, (MMSd_length) MMSd_EMITValue( value, E ), E ));
}

/* V10.0 - emit routines for fixed-length Goose encodings */

/* --------------------------------------------------------------
 --
 -- --- MMSd_EMITFIXEDValue
 --
 --------------------------------------------------------------- */
MMSd_length MMSd_EMITFIXEDValue(TMW_INT64 value,
                                int size,
                                MMSd_descriptor *E)
{
    boolean isPositive = (boolean) (value >= 0L);
    MMSd_length length = 0;

    do {
        MMSd_EMITOctet( (int) (value & 0xFFL), E );
        value = value >> 8;
        length += 1;
        size -= 1;
    } while (size > 0);

    if (isPositive)
        MMSd_EMITOctet( (int) 0, E );
    else
        MMSd_EMITOctet( (int) 0xFF, E );

    return (length + 1);
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_emitFIXEDTAGGEDVALUE
 --
 --------------------------------------------------------------- */
MMSd_length MMSd_emitFIXEDTAGGEDVALUE(MMSd_tag tag,
                                      TMW_INT64 value,
                                      int size,
                                      MMSd_descriptor *E)
{
    return (MMSd_emitTAG_and_LENGTH( tag, (MMSd_length) MMSd_EMITFIXEDValue( value, size, E ), E ));
}

/* --------------------------------------------------------------
 --
 -- --- MMSd_EMITFIXEDUnsignedValue
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_EMITFIXEDUnsignedValue(unsigned long value,
                                        int size,
                                        MMSd_descriptor *E)
{
    MMSd_length length = 0;

    do {
        MMSd_EMITOctet( (int) (value & 0xFF), E );
        value = value >> 8;
        length += 1;
        size -= 1;
    } while (size > 0);

    MMSd_EMITOctet( (int) 0, E );

    return (length + 1);
}

MMSd_length MMSd_emitFIXEDOCTETS(unsigned char *o,
                                 MMSd_length len,
                                 MMSd_descriptor *E)
{
    MMSd_length total_length = 0;

    if (len > MMSD_MAX_GOOSE_FIXED_OCTET_STRING_LENGTH)
        len = MMSD_MAX_GOOSE_FIXED_OCTET_STRING_LENGTH;

    if (len < MMSD_MAX_GOOSE_FIXED_OCTET_STRING_LENGTH) {
        int i;

        for (i = 0; i < MMSD_MAX_GOOSE_FIXED_OCTET_STRING_LENGTH - len; i++) {
            MMSd_EMITOctet( (int) 0, E );
            total_length = total_length + 1;
        }
    }
    total_length = total_length + MMSd_emitOCTETS( o, len, E );
    return (total_length);
}

#ifdef MMSD_STRING_TYPE
/* --------------------------------------------------------------
 --
 -- --- MMSd_emitFIXEDSTRING
 --
 --------------------------------------------------------------- */

MMSd_length MMSd_emitFIXEDSTRING(char *s,
                                 int size,
                                 MMSd_descriptor *E)
{
    int length, saveSize;

    if (size > MMSD_MAX_GOOSE_FIXED_VISIBLE_STRING_LENGTH)
        size = MMSD_MAX_GOOSE_FIXED_VISIBLE_STRING_LENGTH;

    saveSize = size;
    length = MMSd_stringLength( s );
    if (length >= size)
        return (MMSd_emitOCTETS( (unsigned char *) s, (MMSd_length) size, E ));

    while (size > length) {
        MMSd_EMITOctet( (int) 0, E );
        size -= 1;
    }

    MMSd_emitOCTETS( (unsigned char *) s, (MMSd_length) length, E );
    return ((MMSd_length) saveSize);
}
#endif

#ifdef DEBUG_MMSD
void MMSd_dump_descr(MMSd_descriptor *D)
{
    printf("# ..Descriptor[%p]:", D);
    printf("# ..  bufPtr = %p\n", D->bufPtr);
    printf("# ..  errorFlag = %lx\n", D->errorFlag);
    printf("# ..  tag = %d\n", D->tag);
    printf("# ..  index = %d\n", D->index);
    printf("# ..  tokenLength = %d\n", D->tokenLength);
    printf("# ..  bufLen = %d\n", D->bufLen);
    printf("# ..  primitive = %d\n", D->primitive);
#ifdef MMSD_FULL_STACK
    printf("# ..  rules = %d\n", D->rules);
#endif
}
#endif

/* End of emitfetc.c */
