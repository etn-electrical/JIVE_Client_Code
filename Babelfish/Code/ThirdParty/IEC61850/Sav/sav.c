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
 *  Sampled Analog Values Routines.
 *
 *  This file should not require modification.
 */

#include "sys.h" //!<IEC61850>

#include "tmwtarg.h"

#define CRCTESTING
#define TMW_SAV_ALLOCATE_SENDS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "eth1.h"

#define MMSD_STRING_TYPE
#define MMSD_UTCTIME_TYPE

#include "ber.h"
#include "mms_erro.h"
#include "HndlData.h"

#include "sav.h"

typedef struct TMW_SAV_LanHeader {
    int etherType;
    int lsap;
    int VLAN_Priority;
    unsigned int VLAN_VID;
    int ApplID;
    unsigned char   simulateBit;
    int lengthOfExtension;
} TMW_SAV_LanHeader;

/* ASN.1 Tags for the Sampled Values Grammar */
#define savPDU0      0x60
#define savASDU0     0x30        /* SEQUENCE */
#define noAsdu0      0x80        /* IMPLICIT INTEGER */
#define security1    0x81        /* ANY */
#define asdu2        0xa2        /* IMPLICIT SEQUENCE */

#define svID0        0x80        /* IMPLICIT VisibleString */
#define datSet1      0x81        /* IMPLICIT VisibleString */
#define smpCnt2      0x82        /* IMPLICIT OctetString[2] (INT16U) */
#define confRev3     0x83        /* IMPLICIT OctetString[4] (INT32U) */
#define refrTm4      0x84        /* IMPLICIT UtcTime (EntryTime) */
#define smpSynch5    0x85        /* IMPLICIT OctetString[1] (INT8U) */
#define smpRate6     0x86        /* IMPLICIT OctetString[2] (INT16U) */
#define sample7      0x87        /* IMPLICIT OctetString */
#define smpMod8      0x88        /* IMPLICIT OctetString[2] (INT16U) */

/* --------------------------------------------------------------
 Local Definitions
 -------------------------------------------------------------- */
#define MMSd_noError(_D_)        ( !(_D_).errorFlag )

/* Retrieves unsigned integer encoded as fixed-length octet string */
#define TMW_FETCH_UINT( v, D ) \
    { \
        int i; \
        v = 0; \
        for ( i = 0; i < MMSd_TokenLen( *(D) ); i++ ) \
        {  \
            v = ( v << 8 ) | (unsigned char) *( MMSd_getDescPtr( *(D) ) + i ); \
        } \
    }

#if defined(TMW_SAV_STATIC_ALLOCATION)
/* --------------------------------------------------------------
 Pool of Sampled Values stream entries
 -------------------------------------------------------------- */
static TMW_SAV_APDU TMW_SAV_apdu_pool[ TMW_SAV_MAX_APDUS ];
static TMW_SAV_ASDU TMW_SAV_asdu_pool[ TMW_SAV_MAX_ASDUS ];
#endif

MMSd_length TMW_SAV_EmitIntegerOctetString(int value,
        MMSd_length len,
        unsigned char tag,
        MMSd_descriptor *E);
MMSd_length TMW_SAV_EmitOctetString(unsigned char *pOctets,
                                    MMSd_length length,
                                    unsigned char tag,
                                    MMSd_descriptor *E);
MMSd_length TMW_SAV_EmitUtcBTime(MMSd_UtcBTime *timeVal,
                                 unsigned char tag,
                                 MMSd_descriptor *E);

TMW_SAV_Error TMW_SAV_Encode(TMW_Packet *packet,
                             TMW_SAV_APDU *pApdu);

int TMW_SAV_Parse(unsigned char *bufferPtr,
                  int length,
                  TMW_SAV_APDU *pApdu);

/* User callable */
void TMW_SAV_byteorder(unsigned char *valstr,
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

/* --------------------------------------------------------------
 NOTE: These Fast_Emit routines have been optimized for speed to
 facilitate high-speed Sampled Values processing. The normal fetch/emit
 routines handle a number of other options which are not relevant
 for Sampled Values operations.
 -------------------------------------------------------------- */

/* --------------------------------------------------------------
 --- Sampled Values BER encoding and decoding macros
 ---
 --- The Sampled Vaues messages contain ASN.1 BER encoded fields
 --- for the 'wrapper' elements, and fixed length (non-ASN.1)
 --- encoded data elements.  These macros are used in the ASN.1
 --- decoding.
 -------------------------------------------------------------- */
#define TMW_SAV_fetchTAG( D )                                       \
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

#define TMW_SAV_fetchLENGTH( F )                                \
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

/* This is the same as MMSd_fetchCOMPLETE, but included here for stand-alone 9-2 implementations */
#define TMW_SAV_fetchBerCOMPLETE( des1, des2 )        \
    ( MMSd_noError( *(des1) )                         \
        ? ( (MMSd_getDescPtr( *(des1) ) + MMSd_TokenLen( *(des1) )) != MMSd_getDescPtr( *(des2) ) ? 0 : 1 ) \
        : 0 )

/* This is the same as MMSd_stringLength, but included here for stand-alone 9-2 implementations */
#define TMW_SAV_stringLength( s )                     \
    ( (s) == NULL ? 0 : strlen( (s) ) )

/* --------------------------------------------------------------
 --
 -- --- TMW_SAV_TAG_and_LENGTH
 --
 --------------------------------------------------------------- */

void TMW_SAV_fetchTAG_and_LENGTH(MMSd_descriptor *D)
{
    /* Use basic encoding rules */
    TMW_SAV_fetchTAG( *D );
    TMW_SAV_fetchLENGTH( *D );
}
/* --------------------------------------------------------------
 --
 -- --- TMW_SAV_fetchOCTETS
 --
 --------------------------------------------------------------- */

int TMW_SAV_fetchOCTETS(MMSd_descriptor *TKN,
                        unsigned char *octets,
                        int outLen,
                        MMSd_tag tag)
{
    MMSd_length length;

    /* Same as checkTag in emitfect.c */
    if ((tag != noTag) && (TKN->tag != tag)) {
        MMSd_setDescErrorFlag( MMSd_data_type_inconsistent, *TKN );
        return (0);
    }

    /* Same as checkError in emitfect.c */
    if (!MMSd_noError( *TKN ))
        return (0);

    length = MMSd_TokenLen( *TKN );
    if (length > outLen)
        length = (MMSd_length) outLen;

    memcpy( octets, MMSd_getDescPtr( *TKN ), length );
    MMSd_skipToken( *TKN );
    return (1);
}

/* --------------------------------------------------------------
 --
 -- --- TMW_SAV_fetchSTRING
 --
 --------------------------------------------------------------- */

int TMW_SAV_fetchSTRING(MMSd_descriptor *TKN,
                        char *str,
                        int outLen,
                        MMSd_tag tag)
{
    if (TMW_SAV_fetchOCTETS( TKN, (unsigned char *) str, outLen, tag )) {
        if (outLen > MMSd_TokenLen( *TKN ))
            outLen = MMSd_TokenLen( *TKN );
        str[outLen] = '\0';
        return (1);
    } else {
        str[0] = '\0';
        return (0);
    }
}

/* --------------------------------------------------------------
 --
 -- --- TMW_SAV_fetchINTEGER
 --
 --------------------------------------------------------------- */

int TMW_SAV_fetchINTEGER(MMSd_descriptor *TKN,
                         int *ival,
                         MMSd_tag tag)
{
    int length;
    int value;

    /* Same as checkTag in emitfect.c */
    if ((tag != noTag) && (TKN->tag != tag)) {
        MMSd_setDescErrorFlag( MMSd_data_type_inconsistent, *TKN );
        return (0);
    }

    /* Same as checkError in emitfect.c */
    if (!MMSd_noError( *TKN ))
        return (0);

    length = MMSd_TokenLen( *TKN );
    value = ( MMSd_getDescOctet( *TKN ) & 0x80) ? -1 : 0;
    while (length-- > 0) {
        value = ((value << 8) & ~0xFF) | MMSd_fetchDescOctet( *TKN );
    }
    *ival = value;
    return (1);
}

/* --------------------------------------------------------------
 --
 -- --- TMW_SAV_fetchUTCTIME
 --
 --------------------------------------------------------------- */

int TMW_SAV_fetchUTCTIME(MMSd_descriptor *TKN,
                         MMSd_UtcBTime *tval,
                         MMSd_tag tag)
{
    int i;
    unsigned long temp;
    unsigned char buffer[8];

    if ( tag )
    {}; /* suppress warning */

    TMW_SAV_fetchOCTETS( TKN, buffer, sizeof(buffer), noTag );
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

/* --------------------------------------------------------------
 ---   TMW_SAV_EmitTagAndLength()
 ---       This function quickly encodes the specified tag and
 --- length to the Sampled Values buffer by simply copying the
 --- length and tag to the buffer.
 ---   Calling sequence:
 ---       tag     - Tag to encode into the Sampled Values buffer
 ---       length  - length to encode into the buffer
 ---       E       - MMSd_descriptor pointer used to store
 ---   Return:
 ---       MMSd_length - New length (length + encoded tag+length)
 -------------------------------------------------------------- */
MMSd_length TMW_SAV_EmitTagAndLength(unsigned char tag,
                                     int length,
                                     MMSd_descriptor *E)
{
    int value = length;
    int size = 0;
    if (E->index < 3) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return (0);
    }
    do {
        if (E->index < 1) {
            MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
            return (0);
        }
        E->bufPtr[E->index] = (unsigned char) (value & 0xFF);
        E->index--;
        size++;
        value = value >> 8;
    } while (value > 0);
    if (length > 127) {
        if (E->index < 1) {
            MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
            return (0);
        }
        E->bufPtr[E->index] = (unsigned char) (0x80 | size);
        E->index--;
        size++;
    }
    if (E->index < 1) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return (0);
    }
    E->bufPtr[E->index] = tag;
    E->index--;
    return ((MMSd_length) (length + size + 1));
}

/* --------------------------------------------------------------
 ---   TMW_SAV_EmitIntegerValue()
 ---       This function quickly encodes an integer value to
 --- the Sampled Values buffer by simply copying the value,
 --- computed length, and specified tag to the buffer.
 ---   Calling sequence:
 ---       value   - value to encode into the Sampled Values buffer
 ---       tag     - Tag to encode into the Sampled Values buffer
 ---       E       - MMSd_descriptor pointer used to store
 ---   Return:
 ---       MMSd_length - Number of bytes encoded (tag+length+value)
 -------------------------------------------------------------- */
MMSd_length TMW_SAV_EmitIntegerValue(long value,
                                     unsigned char tag,
                                     MMSd_descriptor *E)
{
    boolean flag;
    int length = 0;
    boolean isPositive = (boolean)( value >= 0L );
    long comparand = -(!isPositive); /* domain { 0, -1 } */

    do {
        flag = (boolean)( !(value & 0x80L) ^ isPositive );
        if (E->index < 1) {
            MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
            return (0);
        }
        E->bufPtr[E->index--] = (unsigned char) (value & 0xFF);
        value = value >> 8;
        length += 1;
    } while ((value != comparand) || (flag));
    if (E->index < 2) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return (0);
    }
    return (TMW_SAV_EmitTagAndLength( tag, length, E ));
}

/* --------------------------------------------------------------
 ---   TMW_SAV_EmitUnsignedValue()
 ---       This function quickly encodes an unsigned value to
 --- the Sampled Values buffer by simply copying the value,
 --- computed length, and specified tag to the buffer.
 ---   Calling sequence:
 ---       value   - value to encode into the Sampled Values buffer
 ---       tag     - Tag to encode into the Sampled Values buffer
 ---       E       - MMSd_descriptor pointer used to store
 ---   Return:
 ---       MMSd_length - Number of bytes encoded (tag+length+value)
 -------------------------------------------------------------- */
MMSd_length TMW_SAV_EmitUnsignedValue(unsigned long value,
                                      unsigned char tag,
                                      MMSd_descriptor *E)
{
    boolean flag;
    int length = 0;

    do {
        flag = (boolean)( (value & 0x80L) != 0 );
        if (E->index < 1) {
            MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
            return (0);
        }
        E->bufPtr[E->index--] = (unsigned char) (value & 0xFF);
        value = value >> 8;
        length += 1;
    } while ((value != 0) || (flag));
    if (E->index < 2) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return (0);
    }
    return (TMW_SAV_EmitTagAndLength( tag, length, E ));
}

/* --------------------------------------------------------------
 ---   TMW_SAV_EmitBinaryTime6()
 ---       This function quickly encodes a BinaryTime timestamp
 --- to the Sampled Values buffer by simply copying the BinaryTime
 --- tag, length (6 bytes), and 6-byte day and millisecond
 --- time to the Sampled Values Buffer.
 ---   Calling sequence:
 ---       t       - pointer to the time to encode
 ---       tag     - Time tag to encode (i.e. binaryTime12)
 ---       E       - MMSd_descriptor pointer used to store
 ---   Return:
 ---       MMSd_length - Number of bytes encoded (tag+length+6bytevalue)
 -------------------------------------------------------------- */
MMSd_length TMW_SAV_EmitBinaryTime6(MMSd_time *t,
                                    unsigned char tag,
                                    MMSd_descriptor *E)
{
    int i;
    long ival;

    if (E->index < 8) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return (0);
    }
    E->index -= 8;
    i = E->index + 1;
    ival = t->millisecondsSinceMidnight;

    E->bufPtr[i++] = tag;
    E->bufPtr[i++] = 6;
    E->bufPtr[i + 3] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[i + 2] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[i + 1] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[i + 0] = (unsigned char) (ival & 0xff);
    E->bufPtr[i + 5] = (unsigned char) (t->daysSinceJan1_1984 & 0xff);
    E->bufPtr[i + 4] = (unsigned char) ((t->daysSinceJan1_1984 >> 8) & 0xff);
    return (8);
}

/* --------------------------------------------------------------
 ---   MMSd_Fast_EmitUtcBTime()
 ---       This function quickly encodes a UtcBTime to the
 --- Sampled Values buffer by simply copying the UtcBTime tag,
 --- and converting the MMSd_time to UtcBTime and storing
 --- it into the Sampled Values Buffer.
 ---   Calling sequence:
 ---       t       - pointer to the MMSd_time encoded
 ---       tag     - Time tag to encode
 ---       E       - MMSd_descriptor pointer used to store
 ---   Return:
 ---       MMSd_length     - TMW_SAV_EmitTagAndLength( tag, i, E )
 -------------------------------------------------------------- */
MMSd_length TMW_SAV_EmitUtcBTime(MMSd_UtcBTime *t,
                                 unsigned char tag,
                                 MMSd_descriptor *E)
{
    int i;
    long ival;

#if !defined(ASN1_UTC_TIME)
    if (E->index < 10) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return (0);
    }
    E->index -= 10;
    i = E->index + 1;
    ival = t->secondOfCentury;

    E->bufPtr[i++] = tag;
    E->bufPtr[i++] = 8;
    E->bufPtr[i + 3] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[i + 2] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[i + 1] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[i + 0] = (unsigned char) (ival & 0xff);

    ival = t->fractionOfSecond >> 8; /* V9.1 - bug fix */
    E->bufPtr[i + 6] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[i + 5] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[i + 4] = (unsigned char) (ival & 0xff);
    E->bufPtr[i + 7] = (unsigned char) t->quality;
    return (10);
#else
    i = TMW_SAV_EmitUnsignedValue( (unsigned long) t->quality,
                                   timeAttributes2, E );
    i += TMW_SAV_EmitUnsignedValue( t->fractionOfSecond,
                                    fractionOfSecond1, E );
    i += TMW_SAV_EmitUnsignedValue( t->secondOfCentury,
                                    secondsSinceReference0, E );
    return( TMW_SAV_EmitTagAndLength( tag, i, E ) );
#endif
}

/* --------------------------------------------------------------
 ---   TMW_SAV_EmitVisibleString()
 ---       This function quickly encodes a VisibleString to the
 --- Sampled Values buffer by simply copying the VisibleString tag,
 --- the string length, and the string value one byte at a
 --- time into the Sampled Values Buffer.
 ---   Calling sequence:
 ---       s       - pointer to the string to encode
 ---       tag     - Time tag to encode (i.e. visibleString10)
 ---       E       - MMSd_descriptor pointer used to store
 ---   Return:
 ---       MMSd_length     - TMW_SAV_EmitTagAndLength( tag, i, E )
 -------------------------------------------------------------- */
MMSd_length TMW_SAV_EmitVisibleString(char *s,
                                      unsigned char tag,
                                      MMSd_descriptor *E)
{
    int strLen, index;

    strLen = (int) TMW_SAV_stringLength( s );
    E->index = (MMSd_length) (E->index - strLen);
    index = E->index + 1;
    if (index < 3) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return (0);
    }
    if (strLen > 0)
        memcpy( &E->bufPtr[index], s, strLen );
    return (TMW_SAV_EmitTagAndLength( tag, strLen, E ));
}

MMSd_length TMW_SAV_EmitOctets(unsigned char *s,
                               MMSd_length len,
                               MMSd_descriptor *E)
{
    int index;

    E->index = (MMSd_length) (E->index - len);
    index = E->index + 1;
    if (index < 0) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return (0);
    }
    if (len > 0)
        memcpy( &E->bufPtr[index], s, len );
    return (len);
}

/* User callable */
MMSd_length TMW_SAV_EmitSamples(unsigned char *s,
                                MMSd_length len,
                                MMSd_descriptor *E)
{
    MMSd_length length;

    length = TMW_SAV_EmitOctets( s, len, E );
    if (length > 0)
        return (TMW_SAV_EmitTagAndLength( sample7, length, E ));
    return (length);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_EmitOctetString()
 ---       This function quickly encodes an Octet String to the
 --- Sampled Values buffer by simply copying the String value one
 --- byte at a time into the Sampled Values Buffer.
 ---   Calling sequence:
 ---       s       - pointer to the string to encode
 ---       len     - Length of the string
 ---       tag     - OctetString tag to encode
 ---       E       - MMSd_descriptor pointer used to store
 ---   Return:
 ---       MMSd_length     - TMW_SAV_EmitTagAndLength( tag, i, E )
 -------------------------------------------------------------- */
MMSd_length TMW_SAV_EmitOctetString(unsigned char *s,
                                    MMSd_length len,
                                    unsigned char tag,
                                    MMSd_descriptor *E)
{
    MMSd_length length;

    length = TMW_SAV_EmitOctets( s, len, E );
    if (length > 0)
        return (TMW_SAV_EmitTagAndLength( tag, length, E ));
    return (len);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_EmitIntegerOctetString()
 ---       This function quickly encodes an an integer as an Octet
 --- String (as defined in the 9-2) to the Sampled Values buffer by
 --- shifting the integer value one byte at a time into the
 --- Sampled Values Buffer, thereby puting it in big-endian order.
 ---   Calling sequence:
 ---       value   - value to encode
 ---       len     - Length of the resulting octet string
 ---       tag     - ASN.1 tag to encode
 ---       E       - MMSd_descriptor pointer used to store
 ---   Return:
 ---       MMSd_length     - TMW_SAV_EmitTagAndLength( tag, i, E )
 -------------------------------------------------------------- */
MMSd_length TMW_SAV_EmitIntegerOctetString(int value,
        MMSd_length len,
        unsigned char tag,
        MMSd_descriptor *E)
{
    int i;

    if (len > 8) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return (0);
    }
    for (i = 0; i < len; i++) {
        if (E->index < 2) {
            MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
            return (0);
        }
        E->bufPtr[E->index] = (value & 0xFF);
        value = value >> 8;
        E->index--;
    }
    E->bufPtr[E->index] = (unsigned char) len;
    E->index--;
    E->bufPtr[E->index] = (unsigned char) tag;
    E->index--;
    return (len + 2);
}

int TMW_SAV_decode_VLAN_header(unsigned char *buffer,
                               int bufferLength,
                               int *offset,
                               int *length,
                               TMW_SAV_LanHeader *header)
{

    header->etherType = -1;
    header->lsap = -1;
    header->VLAN_Priority = -1;
    header->VLAN_VID = UINT_MAX;
    header->ApplID = -1;
    header->lengthOfExtension = 0;

    *length = (buffer[(*offset)] << 8) | buffer[(*offset) + 1];
    if (*length <= 1520) {
        return ( FALSE);
    }

    /* Check for VLAN etherType */
    if (*length == 0x8100) {
        if (bufferLength < (*offset) + 5)
            return ( FALSE);

        /* Decode and strip VLAN tag header */
        if ((buffer[(*offset) + 2] & 0x10) != 0)
            return ( FALSE);

        header->VLAN_Priority = buffer[(*offset) + 2] >> 5;
        header->VLAN_VID = ((buffer[(*offset) + 2] & 0x0F) << 8) | buffer[(*offset) + 3];
        *offset = *offset + 4;
    }

    if (bufferLength < (*offset) + 10)
        return ( FALSE);

    /* DIX framing, check etherTypes */
    header->etherType = (buffer[(*offset)] << 8) | buffer[(*offset) + 1];
    switch (header->etherType) {
    case 0x88B8:
    case 0x88B9:
    case 0x88BA:
        header->ApplID = (buffer[(*offset) + 2] << 8) | buffer[(*offset) + 3];
        *length = (buffer[(*offset) + 4] << 8) | buffer[(*offset) + 5];
        *length = *length - 8; /* Header overhead */

        /* Security parameters */
        header->lengthOfExtension = (int) buffer[(*offset) + 7];

        /* *offset + 7 is still reserved */
        header->simulateBit = (buffer[(*offset) + 6] & 0x80)?1:0;

        if (header->lengthOfExtension != 0) {
            unsigned int crc16, foundCrc;

            TMW_61850_DL_crc16_compute( &buffer[*offset], 8, &crc16 );
            foundCrc = (unsigned int) (buffer[(*offset) + 9] << 8) | buffer[(*offset) + 8];
            if (crc16 != foundCrc)
                return (0);
        } else {
            if ((buffer[(*offset) + 8] != 0) || (buffer[(*offset) + 9] != 0))
                return (0); /* Illegal frame */
        }

        *offset = *offset + 10;
        if (bufferLength < (*offset) + (*length))
            return ( FALSE);
        return ( TRUE);
    default:
        break;
    }
    return ( FALSE);
}

/* *************************************************************** */
/* Routines for managing ASDU data structures                      */
/* *************************************************************** */
/* --------------------------------------------------------------
 ---   TMW_SAV_ASDU_insert()
 ---       This function inserts a TMW SAV ASDU entry into
 --- a list, either the active_in list, the active_out
 --- list or the avail list.
 ---
 ---   Calling sequence:
 ---       sub     - pointer to entry to insert into list
 ---       list    - pointer to the pointer to the list
 ---   Return:
 ---       VOID
 -------------------------------------------------------------- */
void TMW_SAV_ASDU_insert(TMW_SAV_ASDU *sub,
                         TMW_SAV_ASDU **ppList)
{
    if (*ppList == NULL)
        sub->pNext = NULL;
    else
        sub->pNext = *ppList;
    *ppList = sub;
}


void TMW_SAV_init_ASDU_pool(TMW_SAV_Context *pContext)
{
#if defined(TMW_SAV_STATIC_ALLOCATION) || defined(TMW_SAV_DYNAMIC_ALLOCATION)
    int i;
#endif

#if defined(TMW_SAV_STATIC_ALLOCATION)
    for (i = 0; i < TMW_SAV_MAX_ASDUS; i++) {
        if (i < (TMW_SAV_MAX_ASDUS - 1))
            TMW_SAV_asdu_pool[i].pNext = &TMW_SAV_asdu_pool[i + 1];
        else
            TMW_SAV_asdu_pool[i].pNext = NULL;
    }
    pContext->avail_asdus = TMW_SAV_asdu_pool;
#endif
#if defined(TMW_SAV_DYNAMIC_ALLOCATION)
    pContext->avail_asdus = NULL;
    for (i = 0; i < TMW_SAV_MAX_ASDUS; i++) {
        TMW_SAV_ASDU *pAsdu;

        pAsdu = (TMW_SAV_ASDU *) TMW_TARG_MALLOC( sizeof( TMW_SAV_ASDU ) );
        if ( pAsdu == NULL )
            break;
        memset( pAsdu, 0, sizeof( TMW_SAV_ASDU ) );
        pAsdu->pNext = pContext->avail_asdus;
        pContext->avail_asdus = pAsdu;
    }
#endif
#if defined(TMW_SAV_UNBOUND_ALLOCATION)
    pContext->avail_apdus = NULL;
#endif
}

void TMW_SAV_drain_ASDU_pool(TMW_SAV_Context *pContext)
{
#if defined(TMW_SAV_DYNAMIC_ALLOCATION)
    while ( pContext->avail_asdus != NULL ) {
        TMW_SAV_ASDU *pAsdu;
        pAsdu = pContext->avail_asdus;
        pContext->avail_asdus = pAsdu->pNext;
        TMW_TARG_FREE( pAsdu );
    }
#else
    if ( pContext )
    {}; /* suppress warning */

#endif
}

/* Call only while holding semaphore */
TMW_SAV_ASDU *TMW_SAV_alloc_ASDU(TMW_SAV_Context *pContext)
{
    TMW_SAV_ASDU *pAsdu;

#if defined(TMW_SAV_UNBOUND_ALLOCATION)
    if (pContext->avail_asdus == NULL) {
        pAsdu = (TMW_SAV_ASDU *) TMW_TARG_MALLOC( sizeof(TMW_SAV_ASDU) );
        if (pAsdu != NULL) {
            memset( pAsdu, 0, sizeof(TMW_SAV_ASDU) );
            pAsdu->pNext = pContext->avail_asdus;
            pContext->avail_asdus = pAsdu;
        }
    }
#endif
    if (pContext->avail_asdus == NULL)
        return (NULL);

    pAsdu = pContext->avail_asdus;
    pContext->avail_asdus = pAsdu->pNext;

    return (pAsdu);
}

/* Call only while holding semaphore */
void TMW_SAV_free_ASDU(TMW_SAV_ASDU *pAsdu,
                       TMW_SAV_Context *pContext)
{
#if defined(TMW_SAV_UNBOUND_ALLOCATION)
    TMW_TARG_FREE( pAsdu );
    if ( pContext )
    {}; /* suppress warning */

#else
    TMW_SAV_ASDU_insert( pAsdu, &(pContext->avail_asdus) );
#endif
}

/* *************************************************************** */
/* Routines for managing APDU data structures                      */
/* *************************************************************** */
/* --------------------------------------------------------------
 ---   TMW_SAV_insert()
 ---       This function inserts a TMW SAV APDU entry into
 --- a list, either the active_in list, the active_out
 --- list or the avail list.
 ---
 ---   Calling sequence:
 ---       sub     - pointer to entry to insert into list
 ---       list    - pointer to the pointer to the list
 ---   Return:
 ---       VOID
 -------------------------------------------------------------- */
void TMW_SAV_insert(TMW_SAV_APDU *sub,
                    TMW_SAV_APDU **ppList)
{
    if (*ppList == NULL)
        sub->pNext = NULL;
    else
        sub->pNext = *ppList;
    *ppList = sub;
}

/* --------------------------------------------------------------
 ---   TMW_SAV_remove()
 ---       This function removes a TMW SAV APDU entry from
 --- a list, either the active_in list or the active_out
 --- list.
 ---
 ---   Calling sequence:
 ---       sub     - pointer to entry to remove from list
 ---       ppList   - pointer to list pointer
 ---   Return:
 ---       VOID
 -------------------------------------------------------------- */
void TMW_SAV_remove(TMW_SAV_APDU *sub,
                    TMW_SAV_APDU **ppList)
{
    while (*ppList != NULL) {
        if (*ppList == sub) {
            *ppList = sub->pNext;
            sub->pNext = NULL;
        } else
            ppList = &((*ppList)->pNext);
    }
}

void TMW_SAV_init_APDU_pool(TMW_SAV_Context *pContext)
{
#if defined(TMW_SAV_STATIC_ALLOCATION) || defined(TMW_SAV_DYNAMIC_ALLOCATION)
    int i;
#endif

#if defined(TMW_SAV_STATIC_ALLOCATION)
    for (i = 0; i < TMW_SAV_MAX_APDUS; i++) {
        if (i < (TMW_SAV_MAX_APDUS - 1))
            TMW_SAV_apdu_pool[i].pNext = &TMW_SAV_apdu_pool[i + 1];
        else
            TMW_SAV_apdu_pool[i].pNext = NULL;
    }
    pContext->avail_apdus = TMW_SAV_apdu_pool;
#endif
#if defined(TMW_SAV_DYNAMIC_ALLOCATION)
    pContext->avail_apdus = NULL;
    for (i = 0; i < TMW_SAV_MAX_APDUS; i++) {
        TMW_SAV_APDU *entry;

        entry = (TMW_SAV_APDU *) TMW_TARG_MALLOC( sizeof( TMW_SAV_APDU ) );
        if ( entry == NULL )
            break;
        memset( entry, 0, sizeof( TMW_SAV_APDU ) );
        TMW_SAV_insert( entry, &(pContext->avail_apdus) );
    }
#endif
#if defined(TMW_SAV_UNBOUND_ALLOCATION)
    pContext->avail_apdus = NULL;
#endif
}

void TMW_SAV_drain_APDU_pool(TMW_SAV_Context *pContext)
{
#if defined(TMW_SAV_DYNAMIC_ALLOCATION)
    while ( pContext->avail_apdus != NULL ) {
        TMW_SAV_APDU *pApdu;
        pApdu = pContext->avail_apdus;
        pContext->avail_apdus = pApdu->pNext;
        TMW_TARG_FREE( pApdu );
    }
#endif
    if ( pContext )
    {}; /* suppress warning */
}

/* Call only while holding semaphore */
TMW_SAV_APDU *TMW_SAV_alloc_APDU(TMW_SAV_Context *pContext)
{
    TMW_SAV_APDU *pApdu;

#if defined(TMW_SAV_UNBOUND_ALLOCATION)
    if (pContext->avail_apdus == NULL) {
        TMW_SAV_APDU *entry;
        entry = (TMW_SAV_APDU *) TMW_TARG_MALLOC( sizeof(TMW_SAV_APDU) );
        if (entry != NULL) {
            memset( entry, 0, sizeof(TMW_SAV_APDU) );
            TMW_SAV_insert( entry, &(pContext->avail_apdus) );
        }
    }
#endif
    if (pContext->avail_apdus == NULL)
        return (NULL);

    pApdu = pContext->avail_apdus;
    pContext->avail_apdus = pApdu->pNext;

    return (pApdu);
}

/* Call only while holding semaphore */
void TMW_SAV_free_APDU(TMW_SAV_APDU *pApdu,
                       TMW_SAV_Context *pContext)
{
    TMW_SAV_ASDU *pAsdu;

    while (pApdu->pHead != NULL) {
        pAsdu = pApdu->pHead;
        pApdu->pHead = pAsdu->pNext;
        TMW_SAV_free_ASDU( pAsdu, pContext );
    }

#if defined(TMW_SAV_UNBOUND_ALLOCATION)
    TMW_TARG_FREE( pApdu );
#else
    TMW_SAV_insert( pApdu, &(pContext->avail_apdus) );
#endif
}

/* --------------------------------------------------------------
 ---   TMW_SAV_find_handle()
 ---       This function scans throught a list for
 --- an entry whose handle matches the one passed and
 --- returns a pointer to that APDU entry.
 ---
 ---   Calling sequence:
 ---       handle  - Used defined handle pointer
 ---       ppList  - pointer to a pointer to the list
 ---   Return:
 ---       Pointer to the pool entry matching handle
 -------------------------------------------------------------- */
TMW_SAV_APDU *TMW_SAV_find_handle(void *handle,
                                  TMW_SAV_APDU **ppList)
{
    while (*ppList != NULL) {
        if ((*ppList)->handle == handle) {
            return (*ppList);
        }
        ppList = &((*ppList)->pNext);
    }
    return (NULL);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_find_entry()
 ---       This function scans throught a APDU list for
 --- an entry whose address and goID match the ones
 --- passed and returns a pointer to that APDU entry.
 ---
 ---   Calling sequence:
 ---       mac     - Pointer to the MAC address of the GOOSE
 ---       pList   - pointer to the pointer to the list
 ---   Return:
 ---       Pointer to the pool entry matching both mac and name
 -------------------------------------------------------------- */
TMW_SAV_APDU *TMW_SAV_find_entry(TMW_Mac_Address *mac, int appID,
                                 TMW_SAV_APDU **ppList)
{
    while (*ppList != NULL) {
        if  ((memcmp( mac->addr, (*ppList)->address.addr, (*ppList)->address.len ) == 0)
            && ((unsigned)appID == (*ppList)->VLAN_AppID))
                return (*ppList);
        ppList = &((*ppList)->pNext);
    }

    return (NULL);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_GetApdu()
 ---       This function scans throught the list of active
 --- outputs for an entry whose handle matches the one passed
 --- and returns a pointer to that APDU entry.
 ---
 ---   Calling sequence:
 ---       pContext  - Sampled Values Context
 ---       handle    - User-supllied handle from TMW_SAV_Create
 ---   Return:
 ---       Pointer to the APDU output matching handle
 -------------------------------------------------------------- */
/* User callable */
TMW_SAV_APDU *TMW_SAV_GetApdu(TMW_SAV_Context *pContext,
                              void *handle)
{
    if (pContext == NULL)
        return (NULL);
    if (pContext->active_out == NULL)
        return (NULL);
    return (TMW_SAV_find_handle( handle, &pContext->active_out ));
}

/* *************************************************************** */
/* Routines for decoding/parsing data                              */
/* *************************************************************** */
int TMW_SAV_ParseASDU(MMSd_descriptor *D,
                      TMW_SAV_ASDU *pAsdu)
{
    MMSd_descriptor save;

    pAsdu->validMembers = 0;

    if (D->tag != savASDU0)
        return ( FALSE);

    MMSd_copyDescriptor( *D, save );

    while ((!TMW_SAV_fetchBerCOMPLETE( &save, D )) && (MMSd_noError( *D ))) {
        TMW_SAV_fetchTAG_and_LENGTH( D );

        switch (D->tag) {
        case svID0: /* IMPLICIT VisibleString */
            if (!TMW_SAV_fetchSTRING( D, pAsdu->svID, sizeof(pAsdu->svID), svID0 ))
                return ( FALSE);
            pAsdu->validMembers |= TMW_SAV_svID_VALID;
            break;
        case datSet1: /* IMPLICIT VisibleString */
            if (!TMW_SAV_fetchSTRING( D, pAsdu->datSet, sizeof(pAsdu->datSet), datSet1 ))
                return ( FALSE);
            pAsdu->validMembers |= TMW_SAV_datSet_VALID;
            break;
        case smpCnt2: /* IMPLICIT OctetString[2] (INT16U) */
            TMW_FETCH_UINT( pAsdu->smpCnt, D )
            ;
            MMSd_skipToken( *D );
            pAsdu->validMembers |= TMW_SAV_smpCnt_VALID;
            break;
        case confRev3: /* IMPLICIT OctetString[4] (INT32U) */
            TMW_FETCH_UINT( pAsdu->confRev, D )
            ;
            MMSd_skipToken( *D );
            pAsdu->validMembers |= TMW_SAV_confRev_VALID;
            break;
        case refrTm4: /* IMPLICIT UtcTime (EntryTime) */
            if (!TMW_SAV_fetchUTCTIME( D, &pAsdu->refrTm, refrTm4 ))
                return ( FALSE);
            pAsdu->validMembers |= TMW_SAV_refrTm_VALID;
            break;
        case smpSynch5: /* IMPLICIT OctetString[1] (INT8U) */
            TMW_FETCH_UINT( pAsdu->smpSynch, D )
            ;
            MMSd_skipToken( *D );
            pAsdu->validMembers |= TMW_SAV_smpSynch_VALID;
            break;
        case smpRate6: /* IMPLICIT OctetString[2] (INT16U) */
            TMW_FETCH_UINT( pAsdu->smpRate, D )
            ;
            MMSd_skipToken( *D );
            pAsdu->validMembers |= TMW_SAV_smpRate_VALID;
            break;
        case sample7: /* IMPLICIT OctetString */
            pAsdu->dataPtr = MMSd_getDescPtr( *D );
            pAsdu->dataLen = MMSd_TokenLen( *D );
            MMSd_skipToken( *D );
            break;
        case smpMod8: /* IMPLICIT OctetString[2] (INT16U) */
            TMW_FETCH_UINT( pAsdu->smpMod, D )
            ;
            MMSd_skipToken( *D );
            pAsdu->validMembers |= TMW_SAV_smpMod_VALID;
            break;
        default:
            return ( FALSE);
        }
    }
    return ((pAsdu->validMembers & TMW_SAV_APDU_MINIMUM) == TMW_SAV_APDU_MINIMUM);
}

int TMW_SAV_ParseAPDU( MMSd_descriptor *pDesc, TMW_SAV_APDU *pApdu )
{
        MMSd_descriptor save;
        int             noASDUs;
        int             i;
        TMW_SAV_ASDU    *pAsdu;

        TMW_SAV_fetchTAG_and_LENGTH( pDesc );

        if ( pDesc->tag != savPDU0 )
            return( FALSE );

        MMSd_copyDescriptor( *pDesc, save );

        while ( (!TMW_SAV_fetchBerCOMPLETE( &save, pDesc )) && (MMSd_noError( (*pDesc) )) )
        {
            TMW_SAV_fetchTAG_and_LENGTH( pDesc );

            switch ( pDesc->tag )
            {
            case noAsdu0: /* IMPLICIT INTEGER */
                TMW_SAV_fetchINTEGER( pDesc, &noASDUs, noAsdu0 );
                if ( noASDUs != pApdu->noASDUs )
                    return( FALSE );
                break;
            case security1: /* ANY */
                MMSd_skipToken( *pDesc );
                break;
            case asdu2:
                pAsdu = pApdu->pHead;
                for ( i = 0; i < pApdu->noASDUs; i++ )
                {
                    if ( pAsdu == NULL )
                        return( FALSE );
                    TMW_SAV_fetchTAG_and_LENGTH( pDesc );
                    if ( !TMW_SAV_ParseASDU( pDesc, pAsdu ) )
                        return( FALSE );
                    pAsdu = pAsdu->pNext;
                }

                break;
            default:
                return( FALSE );
            }
        }
        return( TRUE );
}


int TMW_SAV_ParseAndIndicateAPDU( MMSd_descriptor *pDesc, void *pApdu )
{
    if ( TMW_SAV_ParseAPDU( pDesc, (TMW_SAV_APDU*)pApdu ) ) // ltts
    {
        if ( ((TMW_SAV_APDU *)pApdu)->indication_call_back != NULL)
        	((TMW_SAV_APDU *)pApdu)->indication_call_back( ((TMW_SAV_APDU *)pApdu)->indication_call_back_data, (TMW_SAV_APDU*)pApdu );
    }
    return( TRUE );
}

int TMW_SAV_Parse(unsigned char *bufferPtr,
                  int length,
                  TMW_SAV_APDU *pApdu)
{
    MMSd_descriptor D, save;
    int noASDUs;
    int i;
    TMW_SAV_ASDU *pAsdu;

    MMSd_makeDescriptor( D, bufferPtr, (MMSd_length ) length, 0 );

    TMW_SAV_fetchTAG_and_LENGTH( &D );

    if (D.tag != savPDU0)
        return ( FALSE);

    MMSd_copyDescriptor( D, save );

    while ((!TMW_SAV_fetchBerCOMPLETE( &save, &D )) && (MMSd_noError( D ))) {
        TMW_SAV_fetchTAG_and_LENGTH( &D );

        switch (D.tag) {
        case noAsdu0: /* IMPLICIT INTEGER */
            TMW_SAV_fetchINTEGER( &D, &noASDUs, noAsdu0 );
            if (noASDUs != pApdu->noASDUs)
                return ( FALSE);
            break;
        case security1: /* ANY */
            MMSd_skipToken( D );
            break;
        case asdu2:
            pAsdu = pApdu->pHead;
            for (i = 0; i < pApdu->noASDUs; i++) {
                if (pAsdu == NULL)
                    return ( FALSE);
                TMW_SAV_fetchTAG_and_LENGTH( &D );
                if (!TMW_SAV_ParseASDU( &D, pAsdu ))
                    return ( FALSE);
                pAsdu = pAsdu->pNext;
            }

            break;
        default:
            return ( FALSE);
        }
    }
    return ( TRUE);
}

TMW_SAV_APDU *TMW_SAV_Test(TMW_SAV_Context *cntxt,
                           TMW_Mac_Address *dstAddr,
                           unsigned char *passedBuffer,
                           int passedLength)
{
    TMW_SAV_LanHeader lanAddress;
    int ret;
    int offset, lclLen;
    int length = passedLength + 2;
    unsigned char *buffer = passedBuffer - 2;
    TMW_SAV_APDU *pApdu;


    /* -------------------------------------------------------------------- *
     * Check for valid SAV LSAP/EtherType                                   *
     * -------------------------------------------------------------------- */
    offset = 0;
    lclLen = length;
    if (!TMW_SAV_decode_VLAN_header( buffer, length, &offset, &lclLen, &lanAddress ))
        return (NULL);


    pApdu = TMW_SAV_find_entry( dstAddr, lanAddress.ApplID, &(cntxt->active_in) );
    if (pApdu == NULL)
        return (NULL);



    ret = TMW_SAV_Parse( buffer + offset, lclLen, pApdu );
    if (ret) {
        pApdu->lengthOfExtension = lanAddress.lengthOfExtension;
        pApdu->VLAN_Priority = (unsigned char) lanAddress.VLAN_Priority;
        pApdu->VLAN_VID = lanAddress.VLAN_VID;
        pApdu->VLAN_AppID = lanAddress.ApplID;
        pApdu->simulateBit = lanAddress.simulateBit;
        pApdu->nDecodedPduLength = lclLen + 10;
        pApdu->pDecodedPduStart = buffer + offset - 10;
        return (pApdu);
    }
    return (NULL);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Evaluate()
 ---       This function evaluates all incoming SAV to verify
 --- proper encoding and to determine if the SAV is
 --- subscribed to, and to pass the SAV to the user if
 --- needed.
 ---   Calling sequence:
 ---       dl_cntxt- TMW_DL_Context structure pointer
 ---       buffer  - pointer to incoming message buffer
 ---       length  - length of incoming buffer
 ---       pVoidContext  - void pointer to SAV_Context
 ---   Return:
 ---       int -  TRUE : Valid message evaluated and passed if needed
 ---      FALSE: Invalid message or  not subscribed to
 -------------------------------------------------------------- */
int TMW_SAV_Evaluate(TMW_DL_Context *dl_cntxt,
                     unsigned char *buffer,
                     int length,
                     void *pVoidContext)
{
    TMW_Mac_Address dstAddr;
    TMW_SAV_Context *pContext = (TMW_SAV_Context *) pVoidContext;
    TMW_SAV_APDU *pApdu;

    if (pContext == NULL)
        return ( FALSE);

    if ( dl_cntxt )
    {}; /* suppress warning */

    /* -------------------------------------------------------------------- *
     * Test and decode the SAV. If valid SAV continue, else fail            *
     * -------------------------------------------------------------------- */
    if (length < 9) {
        return ( FALSE);
    }

    dstAddr.len = 6;
    memcpy( dstAddr.addr, buffer - 14, 6 );
    pApdu = TMW_SAV_Test( pContext, &dstAddr, buffer, length );
    if (pApdu != NULL) {
        if ((pApdu->lengthOfExtension != 0) && (pApdu->checkVlanSignature != NULL)) {
            unsigned char *pSignature = NULL;
            int nSignatureLen = 0;
            unsigned char *pToBeSigned;
            int nToBeSignedLength;
            MMSd_descriptor D, save;

            pApdu->pPrivateSecurityData = NULL;
            pApdu->nPrivateSecurityDataLen = 0;

            MMSd_makeDescriptor( D, pApdu->pDecodedPduStart + pApdu->nDecodedPduLength,
                                 (MMSd_length ) pApdu->lengthOfExtension, 0 );
            TMW_SAV_fetchTAG_and_LENGTH( &D );

            if (D.tag == VLAN_Security_SecurityExtension0) {
                save = D;
                nToBeSignedLength = length - pApdu->lengthOfExtension + MMSd_DescIndex( D ) - 2;
                TMW_SAV_fetchTAG_and_LENGTH( &D );
                if (D.tag == VLAN_Security_Reserved1) {
                    MMSd_skipToken( D );
                    nToBeSignedLength = length - pApdu->lengthOfExtension + MMSd_DescIndex( D ) - 2;
                    TMW_SAV_fetchTAG_and_LENGTH( &D );
                }
                if (D.tag == VLAN_Security_Private2) {
                    pApdu->pPrivateSecurityData = MMSd_getDescPtr( D );
                    pApdu->nPrivateSecurityDataLen = MMSd_TokenLen( D );
                    MMSd_skipToken( D );
                    nToBeSignedLength = length - pApdu->lengthOfExtension + MMSd_DescIndex( D ) - 2;
                    TMW_SAV_fetchTAG_and_LENGTH( &D );
                }

                pToBeSigned = pApdu->pDecodedPduStart;
                if (D.tag == VLAN_Security_AuthenticationValue3) {
                    pSignature = MMSd_getDescPtr( D );
                    nSignatureLen = MMSd_TokenLen( D );
                    MMSd_skipToken( D );
                }
                if ((!TMW_SAV_fetchBerCOMPLETE( &save, &D )) || (pSignature == NULL) || (nSignatureLen == 0)) {
                    /* ERROR */
                    return ( TRUE ); /* So processing will not proceed with this frame */
                }
                if (!pApdu->checkVlanSignature( pToBeSigned, nToBeSignedLength, pSignature, nSignatureLen,
                                                pApdu->pCryptoContext, pApdu->pVlanSignatureData )) {
                    /* ERROR */
                    /* SECURITY - log signature check failure */
                    return ( TRUE ); /* So processing will not proceed with this frame */
                }
            } else {
                /* ERROR */
                /* SECURITY - log signature extension format failure */
                return ( TRUE ); /* So processing will not proceed with this frame */
            }
        } else {
            if (pApdu->checkVlanSignature != NULL) {
                /* ERROR */
                /* SECURITY - log security failure - expecting secure PDU */
                return ( TRUE ); /* So processing will not proceed with this frame */
            }
            /* else (if no extension or no signature checking callback, handle as if no security */
        }
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_TAKE( pApdu->pContext->sem );
#endif
        if (pApdu->indication_call_back != NULL)
            pApdu->indication_call_back( pApdu->indication_call_back_data, pApdu );
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( pApdu->pContext->sem );
#endif
        return ( TRUE );
    }
    return ( FALSE );
}

MMSd_length TMW_SAV_EncodeASDU(MMSd_descriptor *E,
                               int smpCnt,
                               TMW_SAV_APDU *pApdu,
                               TMW_SAV_ASDU *pAsdu)
{
    MMSd_length length, subLength;

    length = 0;

    if (pAsdu->validMembers & TMW_SAV_smpMod_VALID)
        length = length + TMW_SAV_EmitIntegerOctetString( pAsdu->smpMod, 2, smpMod8, E );

    subLength = 0;
    if (pAsdu->encode_callback != NULL)
        subLength = pAsdu->encode_callback( pApdu, pAsdu, (smpCnt & 0xFFFF), pAsdu->encode_callback_data, E );

    length = length + subLength;

    if (pAsdu->validMembers & TMW_SAV_smpRate_VALID)
        length = length + TMW_SAV_EmitIntegerOctetString( pApdu->smpRate, 2, smpRate6, E );
    if (pAsdu->validMembers & TMW_SAV_smpSynch_VALID)
        length = length + TMW_SAV_EmitIntegerOctetString( pAsdu->smpSynch, 1, smpSynch5, E );
    if (pAsdu->validMembers & TMW_SAV_refrTm_VALID)
        length = length + TMW_SAV_EmitUtcBTime( &(pAsdu->refrTm), refrTm4, E );
    if (pAsdu->validMembers & TMW_SAV_confRev_VALID)
        length = length + TMW_SAV_EmitIntegerOctetString( pAsdu->confRev, 4, confRev3, E );
    if (pAsdu->validMembers & TMW_SAV_smpCnt_VALID)
        length = length + TMW_SAV_EmitIntegerOctetString( smpCnt, 2, smpCnt2, E );
    if (pAsdu->validMembers & TMW_SAV_datSet_VALID)
        length = length + TMW_SAV_EmitVisibleString( pAsdu->datSet, datSet1, E );
    if (pAsdu->validMembers & TMW_SAV_svID_VALID)
        length = length + TMW_SAV_EmitVisibleString( pAsdu->svID, svID0, E );

    length = TMW_SAV_EmitTagAndLength( savASDU0, length, E );

    if (MMSd_noError( *E ))
        return (length);
    return (0);
}

int TMW_SAV_EncodeSavData( MMSd_descriptor *pDescriptor, void *vApdu )
{
    int             i;
    int             length, subLength;
    TMW_SAV_ASDU    *pAsdu;
    int             smpCnt;
    TMW_SAV_APDU *pApdu = (TMW_SAV_APDU*)vApdu;

    subLength = 0;
    smpCnt = pApdu->smpCnt + pApdu->noASDUs - 1;
    pAsdu = pApdu->pHead;
    for ( i = 0; i < pApdu->noASDUs; i++ )
    {
        if ( pAsdu == NULL )
            return( -1 );
        subLength += (int) TMW_SAV_EncodeASDU( pDescriptor, smpCnt, pApdu, pAsdu );
        smpCnt--;
        pAsdu = pAsdu->pNext;
    }
    pApdu->smpCnt += pApdu->noASDUs;
    length = (int) TMW_SAV_EmitTagAndLength( asdu2, subLength, pDescriptor );

    if ( pApdu->useSecurity )
        length = length + (int) TMW_SAV_EmitOctetString( pApdu->security, sizeof( pApdu->security ), security1, pDescriptor );

    length = length + (int) TMW_SAV_EmitIntegerValue( (long) pApdu->noASDUs, noAsdu0, pDescriptor );
    length = (int) TMW_SAV_EmitTagAndLength( savPDU0, length, pDescriptor );
    if ( !MMSd_noError( *pDescriptor ) )
        return( -1 );
    return( length );
}

TMW_SAV_Error TMW_SAV_Encode(TMW_Packet *packet, TMW_SAV_APDU *pApdu)
{
    int i;
    MMSd_length length, subLength;
    MMSd_descriptor E;
    TMW_SAV_ASDU *pAsdu;
    int smpCnt;
    int nCrcStoreIndex, nCrcComputeIndex;
    unsigned char *pSignature;
    MMSd_length nSignatureTlv = 0;
    MMSd_length lengthOfExtension = 0;
    MMSd_length datalen;

    /* -------------------------------------------------------------------- *
     * Make a descriptor for the SAV and fill in reverse order!             *
     * -------------------------------------------------------------------- */
    MMSd_makeDescriptor( E, packet->buffer, (MMSd_ushort ) packet->buffer_length,
                         (MMSd_ushort ) (packet->buffer_length - 1) );
    if (pApdu->genVlanSignature != NULL) {
        MMSd_makeDescriptor( E, packet->buffer, (MMSd_ushort ) packet->buffer_length,
                             (MMSd_ushort) ( packet->buffer_length - 1 - VLAN_SIGNATURE_SIZE ) );

        pSignature = ((unsigned char *) packet->buffer) + packet->buffer_length - VLAN_SIGNATURE_SIZE;

        nSignatureTlv = TMW_SAV_EmitTagAndLength( VLAN_Security_AuthenticationValue3, (MMSd_ushort) VLAN_SIGNATURE_SIZE, &E );
        datalen = 0;
        if (pApdu->nPrivateSecurityDataLen > 0)
            datalen = nSignatureTlv
                      + TMW_SAV_EmitOctetString( pApdu->pPrivateSecurityData,
                                                 (MMSd_length) pApdu->nPrivateSecurityDataLen,
                                                 VLAN_Security_Private2,
                                                 &E );
        lengthOfExtension = TMW_SAV_EmitTagAndLength( VLAN_Security_SecurityExtension0, (MMSd_ushort) datalen, &E );
    } else {
        MMSd_makeDescriptor( E, packet->buffer, (MMSd_ushort ) packet->buffer_length,
                             (MMSd_ushort ) (packet->buffer_length - 1) );
        pSignature = NULL;
    }

    subLength = 0;
    smpCnt = pApdu->smpCnt + pApdu->noASDUs - 1;
    pAsdu = pApdu->pHead;
    for (i = 0; i < pApdu->noASDUs; i++) {
        if (pAsdu == NULL)
            return (TMW_SAV_ERR_PARAMS);
        subLength += TMW_SAV_EncodeASDU( &E, smpCnt, pApdu, pAsdu );
        smpCnt--;
        pAsdu = pAsdu->pNext;
    }
    pApdu->smpCnt += pApdu->noASDUs;
    length = TMW_SAV_EmitTagAndLength( asdu2, subLength, &E );

    if (pApdu->useSecurity)
        length = length + TMW_SAV_EmitOctetString( pApdu->security, sizeof(pApdu->security), security1, &E );

    length = length + TMW_SAV_EmitIntegerValue( (long) pApdu->noASDUs, noAsdu0, &E );
    length = TMW_SAV_EmitTagAndLength( savPDU0, length, &E );

    if (!MMSd_noError( E ) || E.index < 10)
        return (TMW_SAV_ERR_SIZE);

    E.bufPtr[E.index--] = 0x00; /* (CRC) Reserved 2 */
    E.bufPtr[E.index--] = 0x00; /* (CRC) Reserved 2 */
    nCrcStoreIndex = E.index + 1;

    E.bufPtr[E.index--] = 0x00; /* Reserved 1 */


    if (pApdu->simulateBit)
        E.bufPtr[E.index--] = 0x80; /* Reserved 1 */
    else
        E.bufPtr[E.index--] = 0x00; /* Reserved 1 */

  //  E.bufPtr[E.index--] = (unsigned char) lengthOfExtension; /* lengthOfExtension */

    length += 8;
    E.bufPtr[E.index--] = (unsigned char) (length & 0xff); /* Length */
    E.bufPtr[E.index--] = (unsigned char) ((length >> 8) & 0xff);

    E.bufPtr[E.index--] = (unsigned char) (pApdu->VLAN_AppID & 0xff); /* APPID */
    E.bufPtr[E.index--] = (unsigned char) ((pApdu->VLAN_AppID >> 8) & 0xff);

    E.bufPtr[E.index--] = 0xBA; /* Add sub-ethertype 0x88BA (SAV) */
    E.bufPtr[E.index--] = 0x88;
    nCrcComputeIndex = E.index + 1;

    if (((int) pApdu->VLAN_VID) >= 0) {
        if ((!MMSd_noError( E )) || E.index < 4) {
            return (TMW_SAV_ERR_SIZE);
        }
        E.bufPtr[E.index--] = (unsigned char) (pApdu->VLAN_VID & 0xff); /* VID */
        E.bufPtr[E.index--] = (unsigned char) (((pApdu->VLAN_VID >> 8) & 0x0f) | /* CFI bit is always 0 */
                                               (pApdu->VLAN_Priority << 5)); /* Priority */
        E.bufPtr[E.index--] = 0x00;
        E.bufPtr[E.index--] = 0x81;
    }

    if (lengthOfExtension != 0) {
        /* Fix up CRC if security extension is used */
        unsigned int crc16;

        // or over VLAN data? TMW_61850_DL_crc16_compute( &E.bufPtr[ E.index + 1 ], 8, &crc16 );
        TMW_61850_DL_crc16_compute( &E.bufPtr[nCrcComputeIndex], 8, &crc16 );
        packet->buffer[nCrcStoreIndex] = (unsigned char) crc16 & 0xFF;
        packet->buffer[nCrcStoreIndex + 1] = (unsigned char) (crc16 >> 8) & 0xFF;
    }

    /* -------------------------------------------------------------------- *
     * Save the SAV message length and the offset into the buffer           *
     * -------------------------------------------------------------------- */
    MMSd_takeDescriptor( E, packet->data_offset, packet->data_length );

    if (pSignature != NULL) {
        unsigned char *pToBeSigned;
        int nToBeSignedLength;

        pToBeSigned = &E.bufPtr[nCrcComputeIndex];
        nToBeSignedLength = packet->buffer_length - nSignatureTlv - nCrcComputeIndex;

        if (!pApdu->genVlanSignature( pToBeSigned, nToBeSignedLength, pSignature,
                                      VLAN_SIGNATURE_SIZE,
                                      pApdu->pCryptoContext, pApdu->pVlanSignatureData ))
            return (TMW_SAV_ERR_SIGNATURE);
    }

    return (TMW_SAV_SUCCESS);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Reset_Output_smpCnt()
 ---       This function resets the smpCnts for the active output
 ---       APDUs for a given SAV context.
 ---
 ---       pContext               - Sampled Values Context
 ---
 ---   Return:
 ---       TMW_SAV_PARAMS         - Failed due bad parameters
 ---       TMW_SAV_SUCCESS        - smpCnt successfully reset
 -------------------------------------------------------------- */
TMW_SAV_Error TMW_SAV_Reset_Output_smpCnt(TMW_SAV_Context *pContext)
{
    TMW_SAV_APDU *pApdu;

    if (pContext == NULL) {
        return (TMW_SAV_ERR_PARAMS);
    }

    pApdu = pContext->active_out;
    while (pApdu != NULL) {
      pApdu->smpCnt = 0;
      pApdu = pApdu->pNext;
    }
    return (TMW_SAV_SUCCESS);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Initialize()
 ---       This function initializes the pool of TMW_SAV_APDU records,
 --- verifies the link adapter name and opens the link layer adapter
 --- (calls ETH_open_port()).
 ---
 --- NOTE: adapter_name must be initialized to valid setting before calling.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---
 ---   Returns:
 ---      TMW_SAV_SUCCESS     - Initialization succeeded
 ---      TMW_SAV_ERR_PARAMS  - Bad parameters passed
 ---      TMW_SAV_ERR_LINK    - Link layer failed to open
 -------------------------------------------------------------- */
/* User callable */
TMW_SAV_Error TMW_SAV_Initialize(TMW_SAV_Context *cntxt)
{
    if (cntxt == NULL)
        return (TMW_SAV_ERR_PARAMS);

    /* Note: don't use memset here, must reserve adapter_name */
    cntxt->active_in = NULL;
    cntxt->active_out = NULL;
    cntxt->link_context = NULL;

    TMW_SAV_init_APDU_pool( cntxt );
    TMW_SAV_init_ASDU_pool( cntxt );

    /* -------------------------------------------------------------------- *
     * Verify adapter name                                          *
     * -------------------------------------------------------------------- */
    if (cntxt->adapter_name[0] == 0) {
#if defined(DEBUG_SAV)
        printf("No adapter name specified for SAV\n");
#endif
        return (TMW_SAV_ERR_PARAMS);
    }

    /* -------------------------------------------------------------------- *
     * Open transport link                                          *
     * -------------------------------------------------------------------- */
    cntxt->link_context = ETH_open_port( cntxt->adapter_name );
    if (cntxt->link_context == NULL) {
#if defined(DEBUG_SAV)
        printf("Adapter %s failed to open for SAV\n", cntxt->adapter_name);
#endif
        return (TMW_SAV_ERR_LINK);
    }

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_CREATE( cntxt->sem );
#endif
#if defined(TMW_USE_THREADS)
    TMW_TARG_EVENT_CREATE( cntxt->eve );
#endif

    return (TMW_SAV_SUCCESS);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Close()
 ---       This function shuts down all SAV activity for the given context.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---
 ---   Return:
 ---      none
 -------------------------------------------------------------- */
/* User callable */
void TMW_SAV_Close(TMW_SAV_Context *cntxt)
{
    while (cntxt->active_in != NULL) {
        TMW_SAV_Unsubscribe( cntxt, (void *) cntxt->active_in->handle );
    }
    while (cntxt->active_out != NULL) {
        TMW_SAV_Delete( cntxt, cntxt->active_out->handle );
    }

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    TMW_DL_close( cntxt->link_context );

    TMW_SAV_drain_ASDU_pool( cntxt );

#if defined(TMW_USE_THREADS)
	if ( cntxt->eve != NULL ) {
		TMW_TARG_EVENT_DELETE( cntxt->eve );
	}
	if ( cntxt->sem != NULL ) {
		TMW_TARG_SEM_GIVE( cntxt->sem );
		TMW_TARG_SEM_DELETE( cntxt->sem );
	}
#endif
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Subscribe()
 ---       This function initializes an TMW_SAV_APDU
 --- entry from the available pool with information about a
 --- Sampled Values input stream to receive, and inserts it into the
 --- active_in pool and binds the entry to the data link.
 ---
 ---   Calling sequence:
 ---       cntxt           - TMW_SAV_Contect structure pointer
 ---       mac             - Pointer to the broadcast SAV address
 ---       AppID           - VLAN Application ID
 ---       handle          - Used defined handle pointer
 ---       call_back       - User defined indication callback function
 ---       call_back_data  - parameter to pass into callback function
 ---
 ---   Return:
 ---      TMW_SAV_SUCCESS         - Subscription succeeded
 ---      TMW_SAV_ERR_PARAMS      - Bad parameters
 ---      TMW_SAV_ERR_LINK        - Link layer failed
 ---      TMW_SAV_ERR_NO_BUFFERS  - Insuficient resources
 -------------------------------------------------------------- */
/* User callable */
TMW_SAV_Error TMW_SAV_Subscribe(TMW_SAV_Context *cntxt,
                                TMW_Mac_Address *mac,
                                unsigned int AppID,
                                void *handle,
                                int noASDUs,
                                TMW_SAV_Indication call_back,
                                void *call_back_data)
{
    return (TMW_SAV_SubscribeSecure( cntxt, mac, AppID, handle, noASDUs, call_back, call_back_data, NULL, NULL, NULL ));
}

TMW_SAV_Error TMW_SAV_SubscribeSecure(TMW_SAV_Context *cntxt,
                                      TMW_Mac_Address *mac,
                                      unsigned int AppID,
                                      void *handle,
                                      int noASDUs,
                                      TMW_SAV_Indication call_back,
                                      void *call_back_data,
                                      TMW_61850_VLAN_CheckSignatureFcn checkVlanSignature,
                                      void *pCryptoContext,
                                      void *pVlanSignatureData)
{
    int i;
    TMW_SAV_APDU *pApdu;

    if (cntxt == NULL)
        return (TMW_SAV_ERR_PARAMS);

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    if (cntxt->link_context == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return (TMW_SAV_ERR_LINK);
    }

    /* -------------------------------------------------------------------- *
     * Take an Apdu from the avail_apdus                                     *
     * -------------------------------------------------------------------- */
    pApdu = TMW_SAV_alloc_APDU( cntxt );
    if (pApdu == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return (TMW_SAV_ERR_NO_BUFFERS);
    }

    /* -------------------------------------------------------------------- *
     * Initialize the pApdu with the parameters passed by the user          *
     * -------------------------------------------------------------------- */
    pApdu->address = *mac;
    pApdu->handle = handle;
    pApdu->pContext = cntxt;
    pApdu->indication_call_back = call_back;
    pApdu->indication_call_back_data = call_back_data;
    pApdu->checkVlanSignature = checkVlanSignature;
    pApdu->genVlanSignature = NULL;
    pApdu->pVlanSignatureData = pVlanSignatureData;
    pApdu->pCryptoContext = pCryptoContext;
    pApdu->VLAN_AppID = AppID;

    for (i = 0; i < noASDUs; i++) {
        TMW_SAV_ASDU *pAsdu;

        pAsdu = TMW_SAV_alloc_ASDU( cntxt );
        if (pAsdu == NULL) {
            TMW_SAV_free_APDU( pApdu, cntxt );
#if defined(TMW_USE_THREADS)
            TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
            return (TMW_SAV_ERR_NO_BUFFERS);
        }

        memset( pAsdu, 0, sizeof(TMW_SAV_ASDU) );

        if (pApdu->pHead == NULL) {
            pApdu->pHead = pAsdu;
            pApdu->pTail = pAsdu;
        } else {
            pApdu->pTail->pNext = pAsdu;
            pApdu->pTail = pAsdu;
        }
    }
    pApdu->noASDUs = noASDUs;

    if (!TMW_DL_bind( cntxt->link_context, TMW_BIND_802_3 | TMW_BIND_GOOSE | TMW_BIND_MULTICAST | TMW_BIND_VL_SAMPV,
                      mac, AppID, (TMW_DL_Ind) TMW_SAV_Evaluate, (void *) cntxt )) {
        TMW_SAV_free_APDU( pApdu, cntxt );
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return (TMW_SAV_ERR_LINK);
    }
    TMW_SAV_insert( pApdu, &(cntxt->active_in) );
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return (TMW_SAV_SUCCESS);
}

#if defined(TMW_USE_90_5)
TMW_SAV_Error TMW_SAV_SubscribeUDP( TMW_SAV_Context      *cntxt, 
                                    TMW_Mac_Address      *address, 
                                    int                  port,
                                    unsigned int         nAppID, 
                                    void                 *handle,
                                    int                  noASDUs,
                                    TMW_SAV_Indication   call_back, 
                                    void                 *call_back_data )
{
    int i;
    TMW_SAV_APDU *pApdu;

    if ( cntxt == NULL )
        return( TMW_SAV_ERR_PARAMS );

#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif

    /* -------------------------------------------------------------------- *
     * Take an Apdu from the avail_apdus                                     *
     * -------------------------------------------------------------------- */
    pApdu = TMW_SAV_alloc_APDU( cntxt );
    if ( pApdu == NULL )
    {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( TMW_SAV_ERR_NO_BUFFERS );
    }

    memset( pApdu, 0, sizeof( TMW_SAV_APDU ) );
    /* -------------------------------------------------------------------- *
     * Initialize the pApdu with the parameters passed by the user          *
     * -------------------------------------------------------------------- */
    pApdu->protocol = TMW_SAV_UDP;
    pApdu->address = *address;
    pApdu->handle = handle;
    pApdu->pContext = cntxt;
    pApdu->indication_call_back = call_back;
    pApdu->indication_call_back_data = call_back_data;

    for ( i = 0; i < noASDUs; i++ )
    {
        TMW_SAV_ASDU *pAsdu;

        pAsdu = TMW_SAV_alloc_ASDU( cntxt );
        if ( pAsdu == NULL )
        {
            TMW_SAV_free_APDU( pApdu, cntxt );
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
            return( TMW_SAV_ERR_NO_BUFFERS );
        }

        memset( pAsdu, 0, sizeof( TMW_SAV_ASDU ) );

        if ( pApdu->pHead == NULL )
        {
            pApdu->pHead = pAsdu;
            pApdu->pTail = pAsdu;
        }
        else
        {
            pApdu->pTail->pNext = pAsdu;
            pApdu->pTail = pAsdu;
        }
    }
    pApdu->noASDUs = noASDUs;
    pApdu->sppData.data.sav.decode_func = &TMW_SAV_ParseAndIndicateAPDU;
    pApdu->sppData.data.sav.pApdu = pApdu;

    if ( TMW_SPP_UDP_Subscribe( &pApdu->sppData,
                                TMW_SPP_SPDU_SAV,
                                address, 102,
                                nAppID ) != TMW_SPP_ERR_Success )
    {
        TMW_SAV_free_APDU( pApdu, cntxt );
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( TMW_SAV_ERR_LINK );
    }

    TMW_SAV_insert( pApdu, &(cntxt->active_in) );
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TMW_SAV_SUCCESS );
}
#endif


TMW_SAV_Error TMW_SAV_UnsubscribeNormal( TMW_SAV_Context *cntxt, TMW_SAV_APDU *pApdu )
{
    if (cntxt->link_context == NULL)
        return( TMW_SAV_ERR_PARAMS );

    /* -------------------------------------------------------------------- *
     * Unbind entry from the data link                                      *
     * -------------------------------------------------------------------- */
     TMW_DL_unbind( cntxt->link_context, TMW_BIND_802_3
                    | TMW_BIND_GOOSE | TMW_BIND_MULTICAST | TMW_BIND_VL_SAMPV,
                    &pApdu->address, 0,
                    (TMW_DL_Ind) TMW_SAV_Evaluate, (void *) cntxt );

    /* -------------------------------------------------------------------- *
     * Remove entry from active_in pool and insert it back into avail_apdus  *
     * -------------------------------------------------------------------- */
    TMW_SAV_remove( pApdu, &(cntxt->active_in) );
    TMW_SAV_free_APDU( pApdu, cntxt );
    return( TMW_SAV_SUCCESS );
}


/* --------------------------------------------------------------
 ---   TMW_SAV_Unsubscribe()
 ---       This function finds the TMW_SAV_APDU entry
 --- in the active_in pool that matches the handle, unbinds
 --- it and removes it from the active_in pool.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---       handle  - Used defined handle pointer
 ---
 ---   Return:
 ---      TMW_SAV_SUCCESS: Unsubscribe succeeded
 ---      TMW_SAV_ERR_PARAMS: Unsubscribe failed (handle not found)
 -------------------------------------------------------------- */
/* User callable */
TMW_SAV_Error TMW_SAV_Unsubscribe(TMW_SAV_Context *cntxt,
                                  void *handle)
{
    TMW_SAV_APDU *entry;
    TMW_SAV_Error nError;

    if (cntxt == NULL)
        return (TMW_SAV_ERR_PARAMS);

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_in pool that matches handle*
     * -------------------------------------------------------------------- */
    entry = TMW_SAV_find_handle( handle, &(cntxt->active_in) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return (TMW_SAV_ERR_PARAMS);
    }

	switch ( entry->protocol )
    {
    case TMW_SAV_Normal:
        nError = TMW_SAV_UnsubscribeNormal( cntxt, entry );
        break;
#if defined(TMW_USE_90_5)
    case TMW_SAV_UDP:
        if ( TMW_SPP_UDP_Unsubscribe( &entry->sppData ) == TMW_SPP_ERR_Success )
            nError = TMW_SAV_SUCCESS;
        else
            nError = TMW_SAV_ERR_LINK; 
        TMW_SAV_remove( entry, &(cntxt->active_in) );
        TMW_SAV_free_APDU( entry, cntxt );
        break;
#endif
    default:
        nError = TMW_SAV_ERR_PARAMS;
        break;
    }

    if (cntxt->link_context == NULL)
        return (TMW_SAV_ERR_PARAMS);

    /* -------------------------------------------------------------------- *
     * Unbind entry from the data link                                      *
     * -------------------------------------------------------------------- */
    TMW_DL_unbind( cntxt->link_context, TMW_BIND_802_3 | TMW_BIND_GOOSE | TMW_BIND_MULTICAST | TMW_BIND_VL_SAMPV,
                   &entry->address, 0, (TMW_DL_Ind) TMW_SAV_Evaluate, (void *) cntxt );

    /* -------------------------------------------------------------------- *
     * Remove entry from active_in pool and insert it back into avail_apdus  *
     * -------------------------------------------------------------------- */
    TMW_SAV_remove( entry, &(cntxt->active_in) );
    TMW_SAV_free_APDU( entry, cntxt );

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return (TMW_SAV_SUCCESS);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Create()
 ---       This function sets up a Sampled Values output stream.
 --- Data can be sent on the output stream by passing the same
 --- handle to TMW_SAV_Publish. Note that the initialized stream
 --- contains no ASDUs (sets of sampled values).  These must be
 --- added to the stream by calling TMW_SAV_Add_ASDU for each
 --- set of samples to be contained in each message.  For example
 --- in 9-2LE the MSVCB01 contains one set of samples per message,
 --- so TMW_SAV_Add_ASDU must be called once to initialize the stream
 --- to a single set of samples per message.  For MSVCB02, call
 --- TMW_SAV_Add_ASDU eight times before the first call to
 --- TMW_SAV_Publish to establish eight sample sets per message.
 ---
 ---   Calling sequence:
 ---       cntxt          - TMW_SAV_Context structure pointer
 ---       handle         - Used defined handle pointer
 ---       security       - Unsigned char security data
 ---       nSecurityLen   - Length of security
 ---       address        - Destination address
 ---       VLAN_AppID     - VLAN AppID
 ---       VLAN_VID       - VLAN ID
 ---       VLAN_Priority  - VLAN Priority
 ---
 ---   Return:
 ---       TMW_SAV_ERR_PARAMS     - Bad parameters passed in
 ---       TMW_SAV_ERR_LINK       - Link layer failed to open
 ---       TMW_SAV_ERR_NO_BUFFERS - Allocation failed
 ---       TMW_SAV_SUCCESS        - Output stream created
 -------------------------------------------------------------- */
/* User callable */
TMW_SAV_Error TMW_SAV_Create(TMW_SAV_Context *cntxt,
                             void *handle,
                             unsigned char *security,
                             int nSecurityLen,
                             TMW_Mac_Address *address,
                             unsigned int VLAN_AppID,
                             unsigned int VLAN_VID,
                             unsigned char VLAN_Priority)
{
    return (TMW_SAV_CreateSecure( cntxt, handle, security, nSecurityLen, address, VLAN_AppID, VLAN_VID, VLAN_Priority,
                                  NULL, 0, NULL, NULL, NULL ));
}

TMW_SAV_Error TMW_SAV_CreateSecure(TMW_SAV_Context *cntxt,
                                   void *handle,
                                   unsigned char *security,
                                   int nSecurityLen,
                                   TMW_Mac_Address *address,
                                   unsigned int VLAN_AppID,
                                   unsigned int VLAN_VID,
                                   unsigned char VLAN_Priority,
                                   unsigned char *pPrivateSecurityData,
                                   int nPrivateSecurityDataLen,
                                   TMW_61850_VLAN_MakeSignatureFcn makeVlanSignature,
                                   void *pCryptoContext,
                                   void *pVlanSignatureData)
{
    TMW_SAV_APDU *pApdu;
    int i;

    if (cntxt == NULL)
        return (TMW_SAV_ERR_PARAMS);

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    /* -------------------------------------------------------------------- *
     * Verify link_context                                                  *
     * -------------------------------------------------------------------- */

    if (cntxt->link_context == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return (TMW_SAV_ERR_LINK);
    }

    /* -------------------------------------------------------------------- *
     * Take a Subscription entry from the avail_apdus                *
     * -------------------------------------------------------------------- */
    pApdu = TMW_SAV_alloc_APDU( cntxt );
    if (pApdu == NULL)
        return (TMW_SAV_ERR_NO_BUFFERS);

    memset( pApdu, 0, sizeof(TMW_SAV_APDU) );

    pApdu->pContext = cntxt;

#if defined(TMW_SAV_ALLOCATE_SENDS)
#else
    /* -------------------------------------------------------------------- *
     * Allocate a packet buffer to send GOOSE in                    *
     * -------------------------------------------------------------------- */
    if (!TMW_alloc_packet( &pApdu->packet, cntxt->link_context->buffers, TMW_POOL_SEND )) {
        TMW_SAV_free_APDU( pApdu, cntxt );
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( TMW_SAV_ERR_NO_BUFFERS );
    }

    /* Lock the buffer, so that the link service doesn't free it.  We will re-use
     it for each publish operation on this stream */
    TMW_lock_packet( &pApdu->packet );

    pApdu->packet.media = TMW_VLAN;
#endif

    pApdu->noASDUs = 0;
    if (nSecurityLen > 0) {
        for (i = 0; i < nSecurityLen; i++)
            pApdu->security[i] = security[i];
    }

    pApdu->nSecurityLen = nSecurityLen;
    pApdu->genVlanSignature = makeVlanSignature;
    pApdu->checkVlanSignature = NULL;
    pApdu->pVlanSignatureData = pVlanSignatureData;
    pApdu->pPrivateSecurityData = pPrivateSecurityData;
    pApdu->nPrivateSecurityDataLen = nPrivateSecurityDataLen;
    pApdu->pCryptoContext = pCryptoContext;
    pApdu->address = *address;
    pApdu->VLAN_AppID = VLAN_AppID;
    pApdu->VLAN_VID = VLAN_VID;
    pApdu->VLAN_Priority = VLAN_Priority;
    pApdu->handle = handle;
    pApdu->simulateBit = cntxt->simulateBit;
    TMW_SAV_insert( pApdu, &(cntxt->active_out) );

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return (TMW_SAV_SUCCESS);
}

#if defined(TMW_USE_90_5)
TMW_SAV_Error TMW_SAV_CreateUDP( TMW_SAV_Context     *cntxt, 
                                 void                *handle,
                                 unsigned char       simulateBit,
                                 TMW_Mac_Address     *address,
                                 int                 port,
                                 unsigned int        VLAN_AppID )
{
    TMW_SAV_APDU    *pApdu;

    if ( cntxt == NULL )
        return( TMW_SAV_ERR_PARAMS ); 

#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif

    /* -------------------------------------------------------------------- *
     * Take a Subscription entry from the avail_apdus                *
     * -------------------------------------------------------------------- */
    pApdu = TMW_SAV_alloc_APDU( cntxt );
    if ( pApdu == NULL )
        return( TMW_SAV_ERR_NO_BUFFERS );

    memset( pApdu, 0, sizeof( TMW_SAV_APDU ) );
    if ( TMW_SPP_UDP_Create( &pApdu->sppData,
                             TMW_SPP_SPDU_SAV,
                             address,
                             port,
                             simulateBit,
                             VLAN_AppID ) != TMW_SPP_ERR_Success )
    {
        TMW_SAV_free_APDU( pApdu, cntxt );
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( TMW_SAV_ERR_LINK );
    }
    pApdu->pContext = cntxt;
    pApdu->protocol = TMW_SAV_UDP;
    pApdu->noASDUs = 0;

    pApdu->simulateBit = simulateBit;
    pApdu->address = *address;
    pApdu->VLAN_AppID = VLAN_AppID;
    pApdu->handle = handle;

    TMW_SAV_insert( pApdu, &(cntxt->active_out) );

#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TMW_SAV_SUCCESS );    
}
#endif

/* --------------------------------------------------------------
 ---   TMW_SAV_Add_ASDU()
 ---       This function adds a sample set to a Sampled Values
 ---  output stream previously created by TMW_SAV_Create. This must
 ---  be called one for each sample set to be contained in the
 ---  messages for the stream.  See TMW_SAV_Create for details.
 ---
 ---       pApdu                  - Pointer to output stream APDU
 ---       encode_call_back       - Callback function to encode data
 ---       encode_call_back_data  - User data for callback function
 ---
 ---   Return:
 ---       NULL       - Failed due to insufficient resources
 ---       otherwise  - Pointer to new ASDU
 -------------------------------------------------------------- */
/* User callable */
TMW_SAV_ASDU *TMW_SAV_Add_ASDU(TMW_SAV_APDU *pApdu,
                               TMW_SAV_EncodeAsduData encode_callback,
                               void *encode_callback_data)
{
    TMW_SAV_ASDU *pAsdu;

    if (pApdu == NULL)
        return (NULL);

    if (pApdu->pContext == NULL)
        return (NULL);

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( pApdu->pContext->sem );
#endif

    pAsdu = TMW_SAV_alloc_ASDU( pApdu->pContext );
    if (pAsdu == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( pApdu->pContext->sem );
#endif
        return (NULL);
    }

    memset( pAsdu, 0, sizeof(TMW_SAV_ASDU) );

    pAsdu->encode_callback = encode_callback;
    pAsdu->encode_callback_data = encode_callback_data;
    pAsdu->nAsduNum = pApdu->noASDUs;

    if (pApdu->pHead == NULL) {
        pApdu->pHead = pAsdu;
        pApdu->pTail = pAsdu;
    } else {
        pApdu->pTail->pNext = pAsdu;
        pApdu->pTail = pAsdu;
    }

    pApdu->noASDUs++;
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( pApdu->pContext->sem );
#endif
    return (pAsdu);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Set_ASDU_Encoding()
 ---       This function sets the encoding callback function and
 ---  parameter for an ASDU within an output stream previously
 ---  created by TMW_SAV_Create.
 ---
 ---       pAsdu                  - Pointer to ASDU
 ---       encode_call_back       - Callback function to encode data
 ---       encode_call_back_data  - User data for callback function
 ---
 ---   Return:
 ---       TMW_SAV_ERR_PARAMS     - Failed due bad parameters
 ---       TMW_SAV_SUCCESS        - Successfully set
 -------------------------------------------------------------- */
/* User callable */
TMW_SAV_Error TMW_SAV_Set_ASDU_Encoding(TMW_SAV_ASDU *pAsdu,
                                        TMW_SAV_EncodeAsduData encode_callback,
                                        void *encode_callback_data)
{
    if (pAsdu == NULL)
        return (TMW_SAV_ERR_PARAMS);

    pAsdu->encode_callback = encode_callback;
    pAsdu->encode_callback_data = encode_callback_data;

    return (TMW_SAV_SUCCESS);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Set_All_ASDU_Encoding()
 ---       This function sets the encoding callback function and
 ---  parameter for all ASDUs within the APDU for an output stream
 ---  previously created by TMW_SAV_Create.  See TMW_SAV_Create for
 ---  details.
 ---
 ---       pApdu                  - Pointer to output stream APDU
 ---       encode_call_back       - Callback function to encode data
 ---       encode_call_back_data  - User data for callback function
 ---
 ---   Return:
 ---       TMW_SAV_ERR_PARAMS     - Failed due to bad parameters or
 ---                                  no ASDUs in APDU.
 ---       TMW_SAV_SUCCESS        - Succeeded
 -------------------------------------------------------------- */
/* User callable */
TMW_SAV_Error TMW_SAV_Set_All_ASDU_Encoding(TMW_SAV_APDU *pApdu,
        TMW_SAV_EncodeAsduData encode_callback,
        void *encode_callback_data)
{
    TMW_SAV_ASDU *pAsdu;

    if (pApdu == NULL)
        return (TMW_SAV_ERR_PARAMS);

    if (pApdu->pContext == NULL)
        return (TMW_SAV_ERR_PARAMS);

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( pApdu->pContext->sem );
#endif

    pAsdu = pApdu->pHead;
    if (pAsdu == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( pApdu->pContext->sem );
#endif
        return (TMW_SAV_ERR_PARAMS);
    }

    while (pAsdu != NULL) {
        pAsdu->encode_callback = encode_callback;
        pAsdu->encode_callback_data = encode_callback_data;
        pAsdu = pAsdu->pNext;
    }

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( pApdu->pContext->sem );
#endif
    return (TMW_SAV_SUCCESS);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_DeleteApduOutput()
 ---       Removes teh APDU from the list of active output streams
 --- and frees all resources associated with it.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---       pApdu   - Pointer to output stream APDU
 ---
 ---   Return:
 ---      TMW_SAV_SUCCESS     - Output stream deleted
 -------------------------------------------------------------- */
/* Call only while holding semaphore */
TMW_SAV_Error TMW_SAV_DeleteApduOutput(TMW_SAV_Context *cntxt,
                                       TMW_SAV_APDU *pApdu)
{
    TMW_SAV_remove( pApdu, &(cntxt->active_out) );
#if defined(TMW_SAV_ALLOCATE_SENDS)
#else
    TMW_unlock_packet( &pApdu->packet );
    TMW_free_packet( &pApdu->packet, cntxt->link_context->buffers );
#endif
    TMW_SAV_free_APDU( pApdu, cntxt );

    return (TMW_SAV_SUCCESS);
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Delete()
 ---       This function deletes a Sampled Values output stream.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---       handle  - Used defined handle pointer
 ---
 ---   Return:
 ---      TMW_SAV_SUCCESS     - Output stream deleted
 ---      TMW_SAV_ERR_PARAMS  - Handle not found
 -------------------------------------------------------------- */
/* User callable */
TMW_SAV_Error TMW_SAV_Delete(TMW_SAV_Context *cntxt,
                             void *handle)
{
    TMW_SAV_Error nError;
    TMW_SAV_APDU *pApdu;

    if (cntxt == NULL)
        return (TMW_SAV_ERR_PARAMS);

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    pApdu = TMW_SAV_find_handle( handle, &(cntxt->active_out) );
    if (pApdu == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return (TMW_SAV_ERR_PARAMS);
    }
    nError = TMW_SAV_DeleteApduOutput( cntxt, pApdu );

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif

    return (nError);
}

   /* --------------------------------------------------------------
   ---   TMW_SAV_Publish()
   ---       This function intiates the sending of data on an 
   --- active output Sampled Values stream.
   ---
   ---   Calling sequence:
   ---       cntxt   - TMW_SAV_Contect structure pointer
   ---       handle  - Used defined handle pointer
   ---
   ---   Return:
   ---      TMW_SAV_SUCCESS     - Samples published
   ---      TMW_SAV_ERR_PARAMS  - Bad parameters
   ---      TMW_SAV_ERR_SIZE    - Encoding failed
   ---      TMW_SAV_ERR_LINK    - Link layer error
   -------------------------------------------------------------- */
    /* User callable */
TMW_SAV_Error TMW_SAV_Publish( TMW_SAV_Context *cntxt, void *handle ) 
{
    TMW_SAV_APDU *pApdu;

    if (cntxt == NULL)
        return (TMW_SAV_ERR_PARAMS);

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    if (cntxt->link_context == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return (TMW_SAV_ERR_LINK);
    }

    pApdu = TMW_SAV_find_handle( handle, &(cntxt->active_out) );
    if ( pApdu == NULL )
    {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( TMW_SAV_ERR_PARAMS );
    }

    switch ( pApdu->protocol )
    {
    case TMW_SAV_Normal:
        if ( cntxt->link_context == NULL )
        {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
            return( TMW_SAV_ERR_LINK );
        }

#if defined(TMW_SAV_ALLOCATE_SENDS)
        /* -------------------------------------------------------------------- *
         * Allocate a packet buffer to send SAV in                    *
         * -------------------------------------------------------------------- */
        if (!TMW_alloc_packet( &pApdu->packet, cntxt->link_context->buffers, TMW_POOL_SEND ))
        {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
            return( TMW_SAV_ERR_NO_BUFFERS );
        }

        pApdu->packet.media = TMW_VLAN;
#endif
        /* -------------------------------------------------------------------- *
         * Update the header data, and Startup the SAV                          *
         * -------------------------------------------------------------------- */
        if ( TMW_SAV_Encode( &pApdu->packet, pApdu ) != TMW_SAV_SUCCESS )
        {
            TMW_free_packet( &pApdu->packet, cntxt->link_context->buffers );
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
            return( TMW_SAV_ERR_SIZE );
        }

        if (!TMW_DL_send( &pApdu->packet, &pApdu->address, pApdu->pContext->link_context ))
        {
#if defined(DEBUG_GOOSE)
            printf("Discarding SAV - unable to send link\n");
#endif
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
            return( TMW_SAV_ERR_LINK );
        }
        break;
#if defined(TMW_USE_90_5)
    case TMW_SAV_UDP:
        {
            unsigned char buffer[ TMW_SPP_MAX_UDP_FRAME ];
            TMW_SPP_ERROR status;
            int data_offset, data_length;
            pApdu->sppData.data.sav.pApdu = (void *) pApdu;
            pApdu->sppData.data.sav.func = TMW_SAV_EncodeSavData;
            status = TMW_SPP_Encode( buffer, sizeof( buffer ), 
                                     &data_offset, 
                                     &data_length, 
                                     &pApdu->sppData );
            if ( status != TMW_SPP_ERR_Success )
            {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
                return( TMW_SAV_ERR_SIZE );
            }
            status = TMW_SPP_UDP_Send( (unsigned char *) buffer + data_offset, data_length, &pApdu->sppData );
            if ( status != TMW_SPP_ERR_Success )
            {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
                return( TMW_SAV_ERR_LINK );
            }
        }
        break;
#endif
    default:
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( TMW_SAV_ERR_PARAMS );
    }

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TMW_SAV_SUCCESS );
}

/* --------------------------------------------------------------
 ---   TMW_SAV_Service()
 ---       This function simply services the data link.
 --- The Sampled Values protocol has no internal timers, since
 --- it is strictly driven by the sampling process.  There are
 --- no timeouts or retransmissions, so there is nothing else
 --- required for perioid service. This is only required for
 --- systems not using the full threaded 
 --- part of the API - such as single threaded systems doing polled
 --- I/O - which likely won't be fast enough to do Sampled Values
 --- anyway.
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---   Return:
 ---       VOID
 -------------------------------------------------------------- */
void TMW_SAV_Service(TMW_SAV_Context *cntxt)
{
    if (cntxt->link_context == NULL)
        return;
    TMW_DL_service( cntxt->link_context );
}
