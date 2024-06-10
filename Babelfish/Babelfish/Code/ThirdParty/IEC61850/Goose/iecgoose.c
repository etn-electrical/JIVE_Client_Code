/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1994-2013 */
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
 *
 *
 *  This file should not require modification.
 */

#define IECGOOSE_C
#ifdef IECGOOSE_C

#include "sys.h" 

#include "tmwtarg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "eth1.h"
#include "timefunc.h"

#include "ber.h"
#include "gram.h"
#include "mms_erro.h"
#if defined(USE_MMSD_IN_IEC_GOOSE)
#include "dictiona.h"
#include "connect.h"
#include "mmstypes.h"
#include "ACSI.h"
#endif
#include "iecgoose.h"
#include "igotags.h"
#if defined(CHECK_TIME)
#include <wtypes.h>
#include <winnt.h>
#endif
extern unsigned char IEC_Goose_ResponseTag[];


#if defined(IEC_GOOSE_STATIC_ALLOCATION)
/* --------------------------------------------------------------
  Pool of Goose subscription entries
-------------------------------------------------------------- */
static IEC_Goose_Subscription IEC_goose_pool[ MAX_IEC_GOOSE_ENTRIES ];
#endif


/* --------------------------------------------------------------
                       Local Prototypes
    These routines work like the ones in the MMSd library, but
    are faster and don't handle alternate encoding rules, wtc.
-------------------------------------------------------------- */
MMSd_length IEC_Fast_EmitTagAndLength( unsigned char tag, int length,
                                       MMSd_descriptor *E );
MMSd_length IEC_Fast_EmitIntegerValue( long value, unsigned char tag,
                                       MMSd_descriptor *E );
MMSd_length IEC_Fast_EmitUnsignedValue( unsigned long value, unsigned char tag,
                                        MMSd_descriptor *E );
MMSd_length IEC_Fast_EmitBinaryTime6( MMSd_time *t, unsigned char tag,
                                      MMSd_descriptor *E );
MMSd_length IEC_Fast_EmitVisibleString( char *s, unsigned char tag,
                                        MMSd_descriptor *E );
MMSd_length IEC_Fast_EmitOctetString( unsigned char *s, int len,
                                      unsigned char tag, MMSd_descriptor *E );

void IEC_Goose_insert( IEC_Goose_Subscription *sub,
                       IEC_Goose_Subscription **pool );
void IEC_Goose_remove( IEC_Goose_Subscription *sub,
                       IEC_Goose_Subscription **pool );
void IEC_Goose_InitStatus( IEC_GooseStatus *status );
IEC_Goose_Error IEC_Goose_Send( IEC_GooseStatus *status,
                                unsigned char *buff, int len );
void IEC_Goose_Compute_Interval( IEC_GooseStreamStatus *status, IEC_Goose_Strategy  *strategy );
IEC_Goose_Error IEC_Goose_Startup( IEC_GooseStatus *status,
                                   unsigned char *buff, int len );
void IEC_Goose_Resend( IEC_GooseStatus *status, IEC_GooseStreamStatus *pStreamStatus );
IEC_Goose_Error IEC_Goose_Encode( IEC_GooseHeader *header, TMW_Packet *packet,
                                  unsigned char *buffer, int buffLen,
                                  IEC_GooseSendType sendtype );
int IEC_Goose_Test( IEC_Goose_Context *cntxt, TMW_Mac_Address *srcAddr,
                    unsigned char *buffer, int length, IEC_GooseHeader *header,
                    unsigned char **dataPtr, MMSd_length *dataLen );
MMSd_length IEC_Fast_EmitUtcBTime(MMSd_UtcBTime *t,
                                  unsigned char tag, MMSd_descriptor *E);
int         IEC_Goose_CompareSqNum(unsigned long sq1, unsigned long sq2);


IEC_GooseHeader init_goose_header = {
    "",                     /* gocbRef */
    0L,                     /* HoldTim */
    "",                     /* datSet */
    "",                     /* goID */
    { 0 },               /* t */
    0L,                     /* StNum */
    0L,                     /* SqNum */
    0,                      /* test */
    0,                      /* confRev */
    0,                      /* ndsCom */
    0L,                     /* numDataSetEntries */
    0,                      /* fixedOffs */
    0,                      /* VLAN_AppID */
    0,                      /* VLAN_VID */
    0,                      /* VLAN_Priority */
    0,                       /* simulateBit */
	NULL, 0, 0, NULL, 0, NULL, NULL, NULL, NULL
};



#if defined(CHECK_TIME)  /* These are just used for testing under Windows */
/* --------------------------------------------------------------
---   quaddividelong()
---       This function divides number by divisor and stores the
---         result into res. It is used only when debugging using
---         CHECK_TIME.
---   Calling sequence:
---       number          - Pointer to number to divide
---       divisor         - Divisor to divide number by
---       res             - Result of division
---   Return:
---       VOID
-------------------------------------------------------------- */
static void quaddividelong(LARGE_INTEGER *numer, long divisor,
                           LARGE_INTEGER *res )
{
    long w[4], r[4];
    int    i;
    ldiv_t lres;

    w[0] = numer->u.HighPart >> 16;
    w[1] = numer->u.HighPart & 0xFFFF;
    w[2] = numer->u.LowPart >> 16;
    w[3] = numer->u.LowPart & 0xFFFF;
    lres.rem = 0;
    for (i = 0; i < 4; i++) {
        lres = ldiv( w[i] | (lres.rem << 16), divisor );
        r[i] = lres.quot;
    }
    res->u.HighPart = r[0] << 16 | r[1];
    res->u.LowPart  = r[2] << 16 | r[3];
}



/* --------------------------------------------------------------
---   quadadd()
---       This function adds add1 to add2 and stores the result
---         into res. It is used only when debugging using
---         CHECK_TIME.
---   Calling sequence:
---       add1            - Pointer to number to add to add2
---       add2            - Pointer to number to add to add1
---       res             - Result of addition
---   Return:
---       VOID
-------------------------------------------------------------- */
static void quadadd(LARGE_INTEGER *add1, LARGE_INTEGER *add2,
                    LARGE_INTEGER *res )
{
    long w[4], r[4];
    int    i;

    w[0] = add1->u.HighPart >> 16;
    w[1] = add1->u.HighPart & 0xFFFF;
    w[2] = add1->u.LowPart >> 16;
    w[3] = add1->u.LowPart & 0xFFFF;
    r[0] = add2->u.HighPart >> 16;
    r[1] = add2->u.HighPart & 0xFFFF;
    r[2] = add2->u.LowPart >> 16;
    r[3] = add2->u.LowPart & 0xFFFF;

    for (i = 3; i >= 0; i--) {
        r[i] += w[i];
        if (r[i] & 0xFFFF0000) {
            long temp = r[i] >> 16;
            r[i] = (r[i] & 0xFFFF);
            if (i > 0)
                r[i - 1] += temp;
        }
    }
    res->u.HighPart = r[0] << 16 | r[1];
    res->u.LowPart  = r[2] << 16 | r[3];

}



/* --------------------------------------------------------------
---   quadsubtract()
---       This function subtracts sub2 from sub1 and stores the
---         result into res. It is used only when debugging using
---         CHECK_TIME.
---   Calling sequence:
---       sub1            - Pointer to number to subtract sub2 from
---       sub2            - Pointer to number to subrtact from sub1
---       res             - Result of subtraction
---   Return:
---       VOID
-------------------------------------------------------------- */
static void quadsubtract(LARGE_INTEGER *sub1, LARGE_INTEGER *sub2,
                         LARGE_INTEGER *res )
{
    long w[4], r[4];
    int    i;

    w[0] = sub1->u.HighPart >> 16;
    w[1] = sub1->u.HighPart & 0xFFFF;
    w[2] = sub1->u.LowPart >> 16;
    w[3] = sub1->u.LowPart & 0xFFFF;
    r[0] = sub2->u.HighPart >> 16;
    r[1] = sub2->u.HighPart & 0xFFFF;
    r[2] = sub2->u.LowPart >> 16;
    r[3] = sub2->u.LowPart & 0xFFFF;

    for (i = 3; i >= 0; i--) {
        w[i] -= r[i];
        if (w[i] & 0xFFFF0000) {
            w[i] = (w[i] & 0xFFFF);
            if (i > 0)
                w[i - 1] -= 1;
        }
    }
    res->u.HighPart = w[0] << 16 | w[1];
    res->u.LowPart  = w[2] << 16 | w[3];
}



/* --------------------------------------------------------------
---   quadcompare()
---       This function compares l1 to l2 and returns the
--- difference between them. It is used only when
--- debugging using CHECK_TIME.
---   Calling sequence:
---       l1      - Pointer to first number in comparison
---       l2      - Pointer to second number in comparison
---   Return:
---       int - if diff > 0: l1 > l2
---     if diff < 0: l1 < l2
---     if diff = 0: l1 = l2
-------------------------------------------------------------- */
long quadcompare( LARGE_INTEGER *l1, LARGE_INTEGER *l2 )
{
    long diff;

    diff = l1->u.HighPart - l2->u.HighPart;
    if (diff != 0)
        return( diff );
    diff = l1->u.LowPart - l2->u.LowPart;
    return( diff );
}
#endif

int TMW_decode_VLAN_header( unsigned char   *buffer,
                            int             bufferLength,
                            int             *offset,
                            int             *length,
                            TMW_LanHeader   *header )
{
    header->etherType = -1;
    header->lsap = -1;
    header->VLAN_Priority = -1;
    header->VLAN_VID = UINT_MAX;
    header->ApplID = -1;
    header->lengthOfExtension = 0;

    *length = (buffer[(*offset)] << 8) | buffer[(*offset) + 1];
    if (*length <= 1520) {
        if ( bufferLength < (*offset) + 5 )
            return( FALSE );

        /* 802.3 header */
        if ( buffer[(*offset) + 2] != buffer[(*offset) + 3] )
            return( FALSE );

        if ( buffer[(*offset) + 4] != 0x03 /* UI */ )
            return( FALSE );

        header->lsap = buffer[(*offset) + 2];
        *offset = *offset + 5;
        return( TRUE );
    }

    /* Check for VLAN etherType */
    if (*length == 0x8100) {
        if ( bufferLength < (*offset) + 5 )
            return( FALSE );

        /* Decode and strip VLAN tag header */
        if ( ( buffer[(*offset) + 2] & 0x10 ) != 0 )
            return( FALSE );

        header->VLAN_Priority = buffer[(*offset) + 2] >> 5;
        header->VLAN_VID = ( (buffer[(*offset) + 2] & 0x0F) << 8 )
                           | buffer[(*offset) + 3];
        *offset = *offset + 4;
    }

    if ( bufferLength < (*offset) + 10 )
        return( FALSE );

    /* DIX framing, check etherTypes */
    header->etherType = (buffer[(*offset)] << 8) | buffer[(*offset) + 1];
    switch (header->etherType) {
    case 0x88B8:
    case 0x88B9:
    case 0x88BA:
        header->ApplID = (buffer[(*offset) + 2] << 8) | buffer[(*offset) + 3];
        *length = (buffer[(*offset) + 4] << 8) | buffer[(*offset) + 5];
        *length = *length - 8;                  /* Header overhead */

        /* *offset + 6 is still Reserved 1 */
        header->simulateBit = (buffer[(*offset) + 6] & 0x80) != 0;

        /* Security parameters */
        header->lengthOfExtension = ((int) (buffer[ (*offset) + 6] & 0x7) << 8) | (int) buffer[(*offset) + 7];

        if ( header->lengthOfExtension != 0 ) {
            unsigned int crc16, foundCrc;

            TMW_61850_DL_crc16_compute( &buffer[ *offset ], 8, &crc16 );
            foundCrc = (unsigned int) (buffer[ (*offset) + 9 ] << 8) | buffer[ (*offset) + 8 ];
            if ( crc16 != foundCrc )
                return( 0 );
        } else {
            if ( ( buffer[ (*offset) + 8 ] != 0 ) || (buffer[ (*offset) + 9 ] != 0 ) )
                return( 0 );        /* Illegal frame */
        }

        *offset = *offset + 10;
        if ( bufferLength < (*offset) + (*length) )
            return( FALSE );
        return( TRUE );
    default:
        break;
    }
    return( FALSE );
}

int TMW_encode_VLAN_header( TMW_Packet *packet, TMW_LanHeader *header )
{
    int asdu_length;

    if (header->lsap >= 0) {
        if ( packet->data_offset < 5 )
            return( FALSE );

        packet->buffer[ (packet->data_offset) - 1 ] = (unsigned char) 0x03;
        packet->buffer[ (packet->data_offset) - 2 ] = (unsigned char) header->lsap;
        packet->buffer[ (packet->data_offset) - 3 ] = (unsigned char) header->lsap;
        packet->buffer[ (packet->data_offset) - 4 ] = (unsigned char) (packet->data_length >> 8);
        packet->buffer[ (packet->data_offset) - 5 ] = (unsigned char) (packet->data_length & 0xFF);
        packet->data_offset -= 5;
        packet->data_length += 5;
        return( TRUE );
    }

    if ( header->ApplID < 0 )
        return( FALSE );

    if ( packet->data_offset < 10 )
        return( FALSE );

    asdu_length = packet->data_length + 8;
    packet->buffer[ (packet->data_offset) - 1 ] = (unsigned char) 0;                        /* (CRC16) Reserved 2 */
    packet->buffer[ (packet->data_offset) - 2 ] = (unsigned char) 0;                        /* (CRC16) Reserved 2 */
    packet->buffer[ (packet->data_offset) - 3 ] = 0;                                                /* Reserved 1 */
    packet->buffer[ (packet->data_offset) - 4 ] = 0;                                                /* Reserved 1 */

    packet->buffer[ (packet->data_offset) - 5 ] = (unsigned char) (asdu_length & 0xFF);
    packet->buffer[ (packet->data_offset) - 6 ] = (unsigned char) (asdu_length >> 8);
    packet->buffer[ (packet->data_offset) - 7 ] = (unsigned char) (header->ApplID & 0xFF);
    packet->buffer[ (packet->data_offset) - 8 ] = (unsigned char) (header->ApplID >> 8);
    packet->buffer[ (packet->data_offset) - 9 ] = (unsigned char) (header->etherType & 0xFF);
    packet->buffer[ (packet->data_offset) - 10] = (unsigned char) (header->etherType >> 8);
    packet->data_offset = packet->data_offset - 10;
    packet->data_length = packet->data_length + 10;

    if ( ((int) header->VLAN_VID) < 0 )
        return( TRUE );     /* Bare etherType, VLAN stripped */

    if ( packet->data_offset < 4 )
        return( FALSE );

    packet->buffer[ (packet->data_offset) - 1 ] = (unsigned char) header->VLAN_VID & 0xFF;
    packet->buffer[ (packet->data_offset) - 2 ] = (unsigned char) ((header->VLAN_VID >> 8) & 0xFF)
            | (unsigned char) (header->VLAN_Priority << 5);

    /* V7.99X - Added handling for NULL VLAN transmits */
    packet->buffer[ (packet->data_offset) - 3 ] = 0x00;
    packet->buffer[ (packet->data_offset) - 4 ] = 0x81;

    /* VLAN EtherType is put on by ETH_send processing */
    packet->data_offset = packet->data_offset - 4;
    packet->data_length = packet->data_length + 4;

    return( TRUE );
}

/* --------------------------------------------------------------
NOTE: These Fast_Emit routines have been optimized for speed to
facilitate high-speed GOOSE processing. The normal fetch/emit
routines handle a number of other options which are not relevant
for GOOSE operations.
-------------------------------------------------------------- */


/* --------------------------------------------------------------
---   IEC_Fast_EmitTagAndLength()
---       This function quickly encodes the specified tag and
--- length to the GOOSE buffer by simply copying the
--- length and tag to the buffer.
---   Calling sequence:
---       tag     - Tag to encode into the GOOSE buffer
---       length  - length to encode into the buffer
---       E       - MMSd_descriptor pointer used to store
---   Return:
---       MMSd_length - New length (length + encoded tag+length)
-------------------------------------------------------------- */
MMSd_length IEC_Fast_EmitTagAndLength( unsigned char tag,
                                       int length,
                                       MMSd_descriptor *E )
{
    int value = length;
    int size = 0;
    if (E->index < 3) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return( 0 );
    }
    do {
        if (E->index < 1) {
            MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
            return( 0 );
        }
        E->bufPtr[ E->index ] = (unsigned char) (value & 0xFF);
        E->index--;
        size++;
        value = value >> 8;
    } while (value > 0);
    if (length > 127) {
        if (E->index < 1) {
            MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
            return( 0 );
        }
        E->bufPtr[ E->index ] = (unsigned char) (0x80 | size);
        E->index--;
        size++;
    }
    if (E->index < 1) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return( 0 );
    }
    E->bufPtr[ E->index ] = tag;
    E->index--;
    return( (MMSd_length) (length + size + 1) );
}


/* --------------------------------------------------------------
---   IEC_Fast_EmitIntegerValue()
---       This function quickly encodes an integer value to
--- the GOOSE buffer by simply copying the value,
--- computed length, and specified tag to the buffer.
---   Calling sequence:
---       value   - value to encode into the GOOSE buffer
---       tag     - Tag to encode into the GOOSE buffer
---       E       - MMSd_descriptor pointer used to store
---   Return:
---       MMSd_length - Number of bytes encoded (tag+length+value)
-------------------------------------------------------------- */
MMSd_length IEC_Fast_EmitIntegerValue( long value,
                                       unsigned char tag,
                                       MMSd_descriptor *E )
{
    boolean flag;
    int length = 0;
    int size = 4;
    boolean isPositive = (boolean) (value >= 0L);
    long comparand = -(!isPositive);     /* domain { 0, -1 } */

    switch ( ((MMSd_descriptor *) E )->rules ) {
    case goose_fixed_encoding_rules:
    case goose_fixed_header_encoding_rules:
        do {
            MMSd_EMITOctet( (int) (value & 0xFF), E );
            value = value >> 8;
            length += 1;
            size -= 1;
        } while ( size > 0 );
        if ( isPositive )
            MMSd_EMITOctet( (int) 0, E );
        else
            MMSd_EMITOctet( (int) 0xFF, E );
        return( IEC_Fast_EmitTagAndLength( tag, length + 1, E ) );

    case basic_encoding_rules:
    case packed_encoding_rules:
    case session_encoding_rules:
    default:
        break;
    }

    do {
        flag = (boolean) (!(value & 0x80L) ^ isPositive);
        if (E->index < 1) {
            MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
            return( 0 );
        }
        E->bufPtr[E->index--] = (unsigned char) (value & 0xFF);
        value = value >> 8;
        length += 1;
    } while ((value != comparand) || (flag));
    if (E->index < 2) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return( 0 );
    }
    return( IEC_Fast_EmitTagAndLength( tag, length, E ) );
}

/* --------------------------------------------------------------
---   IEC_Fast_EmitUnsignedValue()
---       This function quickly encodes an unsigned value to
--- the GOOSE buffer by simply copying the value,
--- computed length, and specified tag to the buffer.
---   Calling sequence:
---       value   - value to encode into the GOOSE buffer
---       tag     - Tag to encode into the GOOSE buffer
---       E       - MMSd_descriptor pointer used to store
---   Return:
---       MMSd_length - Number of bytes encoded (tag+length+value)
-------------------------------------------------------------- */
MMSd_length IEC_Fast_EmitUnsignedValue( unsigned long value,
                                        unsigned char tag,
                                        MMSd_descriptor *E )
{
    boolean flag;
    int length = 0;
    int size = 4;

    switch ( ((MMSd_descriptor *) E )->rules ) {
    case goose_fixed_encoding_rules:
    case goose_fixed_header_encoding_rules:
        do {
            MMSd_EMITOctet( (int) (value & 0xFF), E );
            value = value >> 8;
            length += 1;
            size -= 1;
        } while ( size > 0 );
        MMSd_EMITOctet( (int) 0, E );
        return( IEC_Fast_EmitTagAndLength( tag, length + 1, E ) );

    case basic_encoding_rules:
    case packed_encoding_rules:
    case session_encoding_rules:
    default:
        break;
    }

    do {
        flag = (boolean) ((value & 0x80L) != 0);
        if (E->index < 1) {
            MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
            return( 0 );
        }
        E->bufPtr[E->index--] = (unsigned char) (value & 0xFF);
        value = value >> 8;
        length += 1;
    } while ((value != 0) || (flag));
    if (E->index < 2) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return( 0 );
    }
    return( IEC_Fast_EmitTagAndLength( tag, length, E ) );
}

/* --------------------------------------------------------------
---   IEC_Fast_EmitBoolean()
---       This function quickly encodes a boolean value to
--- the GOOSE buffer by putting the value, length, and specified
--- tag to the buffer.
---   Calling sequence:
---       value   - value to encode into the GOOSE buffer
---       tag     - Tag to encode into the GOOSE buffer
---       E       - MMSd_descriptor pointer used to store
---   Return:
---       MMSd_length - Number of bytes encoded (tag+length+value)
-------------------------------------------------------------- */
MMSd_length IEC_Fast_EmitBoolean( unsigned char value,
                                  unsigned char tag,
                                  MMSd_descriptor *E )
{

    if (E->index < 3) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return( 0 );
    }
    if ( value == 0 )
        MMSd_EMITOctet( (int) 0, E );
    else
        MMSd_EMITOctet( (int) 1, E );
    return( IEC_Fast_EmitTagAndLength( tag, 1, E ) );
}


/* --------------------------------------------------------------
---   IEC_Fast_EmitBinaryTime6()
---       This function quickly encodes a BinaryTime timestamp
--- to the GOOSE buffer by simply copying the BinaryTime
--- tag, length (6 bytes), and 6-byte day and millisecond
--- time to the GOOSE Buffer.
---   Calling sequence:
---       t       - pointer to the time to encode
---       tag     - Time tag to encode (i.e. binaryTime12)
---       E       - MMSd_descriptor pointer used to store
---   Return:
---       MMSd_length - Number of bytes encoded (tag+length+6bytevalue)
-------------------------------------------------------------- */
MMSd_length IEC_Fast_EmitBinaryTime6( MMSd_time *t,
                                      unsigned char tag,
                                      MMSd_descriptor *E )
{
    int i;
    long ival;

    if (E->index < 8) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return( 0 );
    }
    E->index -= 8;
    i = E->index + 1;
    ival = t->millisecondsSinceMidnight;

    E->bufPtr[ i++ ] = tag;
    E->bufPtr[ i++ ] = 6;
    E->bufPtr[ i + 3 ] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[ i + 2 ] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[ i + 1 ] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[ i + 0 ] = (unsigned char) (ival & 0xff);
    E->bufPtr[ i + 5 ] = (unsigned char) (t->daysSinceJan1_1984 & 0xff);
    E->bufPtr[ i + 4 ] = (unsigned char) ((t->daysSinceJan1_1984 >> 8) & 0xff);
    return( 8 );
}



/* --------------------------------------------------------------
---   MMSd_Fast_EmitUtcBTime()
---       This function quickly encodes a UtcBTime to the
--- GOOSE buffer by simply copying the UtcBTime tag,
--- and converting the MMSd_time to UtcBTime and storing
--- it into the GOOSE Buffer.
---   Calling sequence:
---       t       - pointer to the MMSd_time encoded
---       tag     - Time tag to encode
---       E       - MMSd_descriptor pointer used to store
---   Return:
---       MMSd_length     - IEC_Fast_EmitTagAndLength( tag, i, E )
-------------------------------------------------------------- */
MMSd_length IEC_Fast_EmitUtcBTime( MMSd_UtcBTime *t,
                                   unsigned char tag,
                                   MMSd_descriptor *E )
{
    int i;
    long ival;

#if !defined(ASN1_UTC_TIME)
    if (E->index < 10) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return( 0 );
    }
    E->index -= 10;
    i = E->index + 1;
    ival = t->secondOfCentury;

    E->bufPtr[ i++ ] = tag;
    E->bufPtr[ i++ ] = 8;
    E->bufPtr[ i + 3 ] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[ i + 2 ] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[ i + 1 ] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[ i + 0 ] = (unsigned char) (ival & 0xff);

    ival = t->fractionOfSecond >> 8;        /* V9.1 - bug fix */
    E->bufPtr[ i + 6 ] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[ i + 5 ] = (unsigned char) (ival & 0xff);
    ival = ival >> 8;
    E->bufPtr[ i + 4 ] = (unsigned char) (ival & 0xff);
    E->bufPtr[ i + 7 ] = (unsigned char) t->quality;
    return( 10 );
#else
    i = IEC_Fast_EmitUnsignedValue( (unsigned long) t->quality,
                                    timeAttributes2, E );
    i += IEC_Fast_EmitUnsignedValue( t->fractionOfSecond,
                                     fractionOfSecond1, E );
    i += IEC_Fast_EmitUnsignedValue( t->secondOfCentury,
                                     secondsSinceReference0, E );
    return( IEC_Fast_EmitTagAndLength( tag, i, E ) );
#endif
}



/* --------------------------------------------------------------
---   IEC_Fast_EmitVisibleString()
---       This function quickly encodes a VisibleString to the
--- GOOSE buffer by simply copying the VisibleString tag,
--- the string length, and the string value one byte at a
--- time into the GOOSE Buffer.
---   Calling sequence:
---       s       - pointer to the string to encode
---       tag     - Time tag to encode (i.e. visibleString10)
---       E       - MMSd_descriptor pointer used to store
---   Return:
---       MMSd_length     - IEC_Fast_EmitTagAndLength( tag, i, E )
-------------------------------------------------------------- */
MMSd_length IEC_Fast_EmitVisibleString( char *s,
                                        unsigned char tag,
                                        MMSd_descriptor *E )
{
    int strLen, index;

    strLen = MMSd_stringLength( s );
    E->index = (MMSd_length) (E->index - strLen);
    index = E->index + 1;
    if (index < 3) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return( 0 );
    }
    if (strLen > 0)
        memcpy( &E->bufPtr[ index ], s, strLen );
    return( IEC_Fast_EmitTagAndLength( tag, strLen, E ) );
}



/* --------------------------------------------------------------
---   IEC_Fast_EmitOctetString()
---       This function quickly encodes an Octet String to the
--- GOOSE buffer by simply copying the String value one
--- byte at a time into the GOOSE Buffer.
---   Calling sequence:
---       s       - pointer to the string to encode
---       len     - Length of the string
---       tag     - OctetString tag to encode
---       E       - MMSd_descriptor pointer used to store
---   Return:
---       MMSd_length     - IEC_Fast_EmitTagAndLength( tag, i, E )
-------------------------------------------------------------- */
MMSd_length IEC_Fast_EmitOctetString( unsigned char *s, int len,
                                      unsigned char tag,
                                      MMSd_descriptor *E )
{
    int index;

    E->index = (MMSd_length) (E->index - len);
    index = E->index + 1;
    if (index < 0) {
        MMSd_setDescErrorFlag( MMSd_emitOverflow, *E );
        return( 0 );
    }
    if (len > 0)
        memcpy( &E->bufPtr[ index ], s, len );
    return( IEC_Fast_EmitTagAndLength( tag, len, E ) );
}

int TMW_EncodeGooseData( MMSd_descriptor    *pEmit,
                         IEC_GooseHeader    *header,
                         unsigned char      *buffer,
                         int                buffLen )
{
    MMSd_length datalen;

    /* -------------------------------------------------------------------- *
     * Emit GOOSE data and tag (DatRefIncluded, entryIncluded or allData)   *
     * -------------------------------------------------------------------- */
    datalen = IEC_Fast_EmitOctetString( buffer, buffLen, allData11, pEmit );

    if ( header->fixedOffs )
        pEmit->rules = goose_fixed_header_encoding_rules;

    /* -------------------------------------------------------------------- *
     * Emit gocbRef, timeAllowedtoLive, datSet, goID, t, StNum, SqNum,      *
     * test, confRev, ndsCom and numDatSetEntries as ANS.1          *
     * encoded data, in reverse order as we back up from the bottom of the  *
     * buffer. Add each encoded segment to the total length of the GOOSE    *
     * message (datalen).                                           *
     * -------------------------------------------------------------------- */
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitUnsignedValue( header->numDataSetEntries,
                                      numDatSetEntries10, pEmit ) );
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitBoolean( header->ndsCom,
                                      ndsCom9, pEmit ) );
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitUnsignedValue( header->confRev,
                                      confRev8, pEmit ) );
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitBoolean( header->test,
                                      test7, pEmit ) );
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitUnsignedValue( header->SqNum,
                                      sqNum6, pEmit ) );
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitUnsignedValue( header->StNum,
                                      stNum5, pEmit ) );
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitUtcBTime( &(header->t),
                                      t4, pEmit ) );
    if (header->goID[ 0 ] != 0)
        datalen = (MMSd_length) ( datalen +
                                  IEC_Fast_EmitVisibleString( header->goID,
                                          goID3, pEmit ) );
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitVisibleString( header->datSet,
                                      datSet2, pEmit ) );
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitUnsignedValue( header->HoldTim,
                                      timeAllowedtoLive1, pEmit ) );
    datalen = (MMSd_length) ( datalen +
                              IEC_Fast_EmitVisibleString( header->gocbRef,
                                      gocbRef0, pEmit ) );

    datalen = IEC_Fast_EmitTagAndLength( IECGoosePDU1, datalen, pEmit );

    return( datalen );
}


/* --------------------------------------------------------------
---   IEC_Goose_Encode()
---       This function encodes an outbound GOOSE message. It is
--- called from IEC_Goose_Send.
---   Calling sequence:
---       header  - Pointer to the IEC_GooseHeader
---       packet  - Pointer to the TMW_Packet data
---       buffer  - Pointer to the buffer to encode into
---       buffLen - Length of the buffer
---   Return:
---       IEC_Goose_Error - IEC_GOOSE_SUCCESS  if successful
---                 IEC_GOOSE_ERR_SIZE if not
-------------------------------------------------------------- */
IEC_Goose_Error IEC_Goose_Encode( IEC_GooseHeader       *header,
                                  TMW_Packet            *packet,
                                  unsigned char         *buffer,
                                  int                   buffLen,
                                  IEC_GooseSendType     sendtype )
{
    MMSd_length     datalen, nSignatureTlv = 0;
    MMSd_descriptor E;
    int             nCrcStoreIndex, nCrcComputeIndex;
    unsigned char   *pSignature;
    MMSd_length     lengthOfExtension = 0;

#if defined(CHECK_TIME)
    LARGE_INTEGER perftime, perffreq, encodetime, decodetime;
    LARGE_INTEGER encodedur, decodedur;
    long clockrate;

    QueryPerformanceFrequency( &perffreq );
    quaddividelong( &perffreq, 1000L, &perffreq );
    clockrate = perffreq.u.LowPart;
    QueryPerformanceCounter( &perftime );
#endif

    /* -------------------------------------------------------------------- *
     * Make a descriptor for the iecgoose and fill in reverse order!        *
     * -------------------------------------------------------------------- */
    if ( header->genVlanSignature != NULL ) {
        MMSd_makeDescriptor( E, packet->buffer, (MMSd_ushort) packet->buffer_length,
                             (MMSd_ushort) ( packet->buffer_length - 1 - VLAN_SIGNATURE_SIZE ) );

        pSignature = ((unsigned char *) packet->buffer) + packet->buffer_length - VLAN_SIGNATURE_SIZE;

        nSignatureTlv = IEC_Fast_EmitTagAndLength( VLAN_Security_AuthenticationValue3,
                        (MMSd_ushort) VLAN_SIGNATURE_SIZE,
                        &E );
        datalen = 0;
        if ( header->nPrivateSecurityDataLen > 0  )
            datalen = nSignatureTlv + IEC_Fast_EmitOctetString( header->pPrivateSecurityData,
                      header->nPrivateSecurityDataLen,
                      VLAN_Security_Private2,
                      &E );
        lengthOfExtension = IEC_Fast_EmitTagAndLength( VLAN_Security_SecurityExtension0, (MMSd_ushort) datalen, &E );
    } else {
        MMSd_makeDescriptor( E, packet->buffer, (MMSd_ushort) packet->buffer_length,
                             (MMSd_ushort) ( packet->buffer_length - 1 ) );
        pSignature = NULL;
    }

    datalen = (MMSd_length) TMW_EncodeGooseData( &E, header, buffer, buffLen );

    /* -------------------------------------------------------------------- *
     * Generate Network Header:                                              *
     * -------------------------------------------------------------------- */
    /* datalen = (MMSd_length)  */
    /* VLAN - Get total length of APDU */
    if ( (!MMSd_noError( E )) || E.index < 10) {
        return( IEC_GOOSE_ERR_SIZE );
    }
    E.bufPtr[ E.index-- ] = 0x00;                       /* (CRC) Reserved 2 */
    E.bufPtr[ E.index-- ] = 0x00;                       /* (CRC) Reserved 2 */
    nCrcStoreIndex = E.index + 1;

    E.bufPtr[ E.index-- ] = (unsigned char) (lengthOfExtension & 0xff);

    {
        unsigned char octet;

        octet = (unsigned char) ( (lengthOfExtension >> 8 ) & 0xff);
        if ( header->simulateBit != 0 )
            octet = octet | 0x80;

        E.bufPtr[ E.index-- ] = octet;                       /* Reserved 1 */
    }

    datalen += 8;
    E.bufPtr[ E.index-- ] = (unsigned char) ( datalen & 0xff );   /* Length */
    E.bufPtr[ E.index-- ] = (unsigned char) ( (datalen >> 8) & 0xff );

    E.bufPtr[ E.index-- ] = (unsigned char) ( header->VLAN_AppID & 0xff );          /* APPID */
    E.bufPtr[ E.index-- ] = (unsigned char) ( (header->VLAN_AppID >> 8) & 0xff );   /* V10.0 changed (was 0x3f) */

    E.bufPtr[ E.index-- ] = 0xB8;   /* Add sub-ethertype 0x88B8 (GOOSE) */
    E.bufPtr[ E.index-- ] = 0x88;
    nCrcComputeIndex = E.index + 1;

    if ( ((int) header->VLAN_VID) >= 0 ) {
        if ( (!MMSd_noError( E )) || E.index < 4) {
            return( IEC_GOOSE_ERR_SIZE );
        }
        E.bufPtr[ E.index-- ] = (unsigned char) ( header->VLAN_VID & 0xff );          /* VID */
        E.bufPtr[ E.index-- ] = (unsigned char) ( ((header->VLAN_VID >> 8) & 0x0f) |  /* CFI is always 0 */
                                (header->VLAN_Priority << 5) );   /* Priority */
        E.bufPtr[ E.index-- ] = 0x00;
        E.bufPtr[ E.index-- ] = 0x81;
    }

    if ( lengthOfExtension != 0 ) {
        /* Fix up CRC if security extension is used */
        unsigned int crc16;

        // or over VLAN data? TMW_61850_DL_crc16_compute( &E.bufPtr[ E.index + 1 ], 8, &crc16 );
        TMW_61850_DL_crc16_compute( &E.bufPtr[ nCrcComputeIndex ], 8, &crc16 );
        packet->buffer[ nCrcStoreIndex ] = (unsigned char) crc16 & 0xFF;
        packet->buffer[ nCrcStoreIndex + 1 ] = (unsigned char) (crc16 >> 8) & 0xFF;
    }

    /* -------------------------------------------------------------------- *
     * Save the GOOSE message length and the offset into the buffer *
     * -------------------------------------------------------------------- */
    MMSd_takeDescriptor( E, packet->data_offset, packet->data_length );

    if ( pSignature != NULL ) {
        unsigned char *pToBeSigned;
        int            nToBeSignedLength;

        pToBeSigned = &E.bufPtr[ nCrcComputeIndex ];
        nToBeSignedLength = packet->buffer_length - nSignatureTlv - nCrcComputeIndex;

        if ( !header->genVlanSignature( pToBeSigned,
                                        nToBeSignedLength,
                                        pSignature,
                                        VLAN_SIGNATURE_SIZE,
                                        header->pCryptoContext,
                                        header->pVlanSignatureData ) )
            return( IEC_GOOSE_ERR_SIGNATURE );
    }

    /* -------------------------------------------------------------------- *
     * If CHECK_TIME is defined for debugging, print encode and decode time *
     * -------------------------------------------------------------------- */
#if defined(CHECK_TIME)
    QueryPerformanceCounter( &encodetime );
    QueryPerformanceCounter( &decodetime );
    quadsubtract( &encodetime, &perftime, &encodedur );
    quadsubtract( &decodetime, &encodetime, &decodedur );
    printf("encode: %ld, %ld ticks\n",
           encodedur.u.HighPart, encodedur.u.LowPart);
    printf("decode: %ld, %ld ticks\n",
           decodedur.u.HighPart, decodedur.u.LowPart);
    printf("tick rate is %ld\n", clockrate);
#endif

    return( IEC_GOOSE_SUCCESS );
}



/* --------------------------------------------------------------
---   IEC_Goose_Test()
---       This function tests and decodes all incoming GOOSE
--- messages. It stores all important GOOSE information
--- into the goose_In structure.
---   Calling sequence:
---       cntxt   - Pointer IEC Goose context record
---       srcAddr - Pointer to source MAC address
---       buffer  - Pointer to the buffer to decode
---       length  - Length of the buffer
---       header  - Pointer to the IEC_GooseHeader
---       dataPtr - Pointer to GOOSE data
---       dataLen - Pointer to GOOSE data length
---   Return:
---       int -  TRUE : valid GOOSE message
---      FALSE: not valid GOOSE message
-------------------------------------------------------------- */
int IEC_Goose_Test( IEC_Goose_Context   *cntxt,
                    TMW_Mac_Address     *srcAddr,
                    unsigned char       *passedBuffer,
                    int                 passedLength,
                    IEC_GooseHeader     *header,
                    unsigned char       **dataPtr,
                    MMSd_length         *dataLen )
{
    TMW_LanHeader   lanAddress;
    int             ret;
    int             offset, lclLen;
    int             length = passedLength + 2;
    unsigned char   *buffer = passedBuffer - 2;

    /* -------------------------------------------------------------------- *
     * Check for valid GOOSE LSAP                                   *
     * -------------------------------------------------------------------- */
    offset = 0;
    lclLen = length;    
    
    if ( !TMW_decode_VLAN_header( buffer, length, &offset,
                                  &lclLen, &lanAddress ) )
        return( FALSE );

    ret = _IEC_gooseParse( cntxt, srcAddr, &lanAddress,
                           buffer + offset, lclLen,
                           header, dataPtr, dataLen );

    header->simulateBit = lanAddress.simulateBit;
    header->lengthOfExtension = lanAddress.lengthOfExtension;


    if (!ret) {
        header->VLAN_Priority = (unsigned char) lanAddress.VLAN_Priority;
        header->VLAN_VID = lanAddress.VLAN_VID;
        header->VLAN_AppID = lanAddress.ApplID;
    }
    return( !ret );
}


int TMW_Goose_ParseAndIndicate( MMSd_descriptor *pDescriptor,
                                void            *pPassedStatus )
{
    unsigned char           *dataPtr;
    MMSd_length             dataLen;
    unsigned char           *pBuffer;
    int                     nBufferLen;
    IEC_Goose_Subscription  *entry;
    TMW_ms_time             msCount;
    IEC_GooseHeader         header;
    IEC_GooseStatus         *pStatus;
    TMW_Mac_Address         destAddress;
#if defined(TMW_USE_90_5)
    TMW_SPP_Data            *pSppData;
#endif
    IEC_Goose_Context       *cntxt;

    if ( pDescriptor == NULL )
        return( FALSE );

    pStatus = (IEC_GooseStatus *) pPassedStatus;
    if ( pStatus == NULL )
        return( FALSE );

    cntxt = (IEC_Goose_Context *) pStatus->goose_context;
    if ( cntxt == NULL )
        return( FALSE );

#if defined(TMW_USE_90_5)
    pSppData = &pStatus->sppData;
#endif
    pBuffer = (unsigned char *) MMSd_getDescPtr( *pDescriptor );
    nBufferLen = (int) MMSd_DescLen( *pDescriptor ) - (int) MMSd_DescIndex( *pDescriptor );

    /* Check for GSEDU tag, so that GSE MGMT PDUs can be avoided for now */
    if ( *pBuffer != IECGoosePDU1 )
        return( FALSE );

    memset( &header, 0, sizeof( IEC_GooseHeader ) );
#if defined(TMW_USE_90_5)
    memcpy( destAddress.addr, &pSppData->destAddress.addr, pSppData->destAddress.len );
    destAddress.len = pSppData->destAddress.len;
#endif
    if ( !_IEC_gooseParse( NULL, //cntxt,
                           NULL, NULL, //srcAddr, &lanAddress,
                           pBuffer, nBufferLen,
                           &header,
                           &dataPtr, &dataLen ) ) {
        IEC_GooseStreamStatus  *pStreamStatus;
        int                     had_received = 0;
#if defined(TMW_USE_90_5)
        header.simulateBit = pSppData->simulation;
		header.VLAN_AppID = pSppData->appID;
#endif
        entry = IEC_Goose_find_entry( &destAddress,
                                      header.goID,
                                      header.gocbRef,
                                      header.VLAN_AppID,
                                      &(cntxt->active_in) );
        if (entry != NULL) {
            /* -------------------------------------------------------------------- *
             * Save source address to make GSE requests                             *
             * -------------------------------------------------------------------- */
            //entry->status.srcAddress = srcAddr;

            /* -------------------------------------------------------------------- *
             * Save VLAN address to make GSE requests                               *
             * -------------------------------------------------------------------- */
            /* Must be done before callback indication */
            entry->status.header.VLAN_VID = header.VLAN_VID;
            entry->status.header.VLAN_AppID = header.VLAN_AppID;
            entry->status.header.VLAN_Priority = header.VLAN_Priority;
            entry->status.header.simulateBit = header.simulateBit;
            if ( header.simulateBit )
                pStreamStatus = &entry->status.simulated;
            else
                pStreamStatus = &entry->status.normal;

            /* -------------------------------------------------------------------- *
             * Check for gaps in SqNum that don't cause timeout to be reported      *
             * to user for statistical purposes.                                    *
             * -------------------------------------------------------------------- */
            if (!IEC_Goose_CompareSqNum(pStreamStatus->SqNum, header.SqNum) ) {
                /* At least one goose transmission was missed. If the
                   Statistical call_back is not NULL, call it here    */
                if (cntxt->SqStats_indication != NULL)
                    cntxt->SqStats_indication( entry->handle, entry,
                                               &(header)      );
            }

            pStreamStatus->initial_wait = FALSE;

            /* -------------------------------------------------------------------- *
             * If the StNum has changed or this GOOSE has never been received,      *
             * call the user call back function with the timeout FALSE              *
             * -------------------------------------------------------------------- */
            had_received = pStreamStatus->have_received;
            msCount = tmwTargGetMsCount();
            pStreamStatus->have_received = TRUE;
            if ((pStreamStatus->StNum != header.StNum)
                    || !had_received) {
                entry->status.header = header;
                if (entry->call_back != NULL)
                    entry->call_back( entry->handle, entry->call_back_data,
                                      &(header), dataPtr, dataLen, FALSE );
            } else {
                entry->status.header = header;
            }

            /* -------------------------------------------------------------------- *
             * Update the HoldTim and the BackTim in the header using timeNow       *
             * -------------------------------------------------------------------- */

            pStreamStatus->StNum = header.StNum;
            pStreamStatus->SqNum = header.SqNum;
            pStreamStatus->HoldTim = header.HoldTim;
            pStreamStatus->timeNext = msCount + pStreamStatus->HoldTim;
        }
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( TRUE );
    }
    return( FALSE );
}




/* --------------------------------------------------------------
---   IEC_Goose_InitStatus()
---       This function initializes the IEC_GooseStatus structure
---   Calling sequence:
---       status  - IEC_GooseStatus structure pointer
---   Return:
---       VOID
-------------------------------------------------------------- */
void IEC_Goose_InitStatus( IEC_GooseStatus *status )
{
    TMW_ms_time msCount = tmwTargGetMsCount();

    status->normal.timeStarted = msCount;
    status->normal.timeLastSent = msCount;
    status->normal.timeNext = msCount;
    status->normal.nextInterval = 0;
    status->normal.currentInterval = 0;
    status->normal.StNum = 0;
    status->normal.SqNum = 0;
    status->simulated.timeStarted = msCount;
    status->simulated.timeLastSent = msCount;
    status->simulated.timeNext = msCount;
    status->simulated.nextInterval = 0;
    status->simulated.currentInterval = 0;
    status->simulated.StNum = 0;
    status->simulated.SqNum = 0;
}



/* --------------------------------------------------------------
---   IEC_Goose_Send()
---       This function prepares and sends a GOOSE message
---   Calling sequence:
---       status  - IEC_GooseStatus structure pointer
---   Return:
---       IEC_Goose_Error - IEC_GOOSE_ERR_NO_BUFFERS
---                 IEC_GOOSE_ERR_PARAMS
---                 IEC_GOOSE_ERR_SIZE
---                 IEC_GOOSE_ERR_LINK
---                 IEC_GOOSE_SUCCESS
-------------------------------------------------------------- */
IEC_Goose_Error IEC_Goose_Send( IEC_GooseStatus *status,
                                unsigned char *buff, int len )
{
    TMW_Packet  packet;

    if ( status == NULL )
        return( IEC_GOOSE_ERR_PARAMS );
    if ( status->goose_context == NULL )
        return( IEC_GOOSE_ERR_PARAMS );
    if (status->goose_context->link_context == NULL)
        return( IEC_GOOSE_ERR_PARAMS );

    if (status->goose_context->presend_indication != NULL) {
      status->goose_context->presend_indication(status);
    }
    

    switch ( status->protocol ) {
    case tmwGooseProtocol_Normal:
        /* -------------------------------------------------------------------- *
         * Allocate a packet buffer to send GOOSE in                    *
         * -------------------------------------------------------------------- */
        if (!TMW_alloc_packet( &packet,
                               status->goose_context->link_context->buffers,
                               TMW_POOL_SEND )) {
#if defined(DEBUG_GOOSE)
            printf("Discarding goose - no buffers\n");
#endif
            return( IEC_GOOSE_ERR_NO_BUFFERS );
        }

        /* -------------------------------------------------------------------- *
         * Encode GOOSE message into packet buffer.                     *
         * -------------------------------------------------------------------- */
        if (IEC_Goose_Encode( &(status->header), &packet, buff, len,
                              status->goose_context->sendtype     )
                != IEC_GOOSE_SUCCESS) {
#if defined(DEBUG_GOOSE)
            printf("Discarding goose - too large\n");
#endif
            TMW_free_packet( &packet,
                             status->goose_context->link_context->buffers );
            return( IEC_GOOSE_ERR_SIZE );
        }

        /* -------------------------------------------------------------------- *
         * Send the GOOSE                                               *
         * -------------------------------------------------------------------- */
        packet.media = TMW_VLAN;
        if (!TMW_DL_send( &packet, &status->address,
                          status->goose_context->link_context)) {
#if defined(DEBUG_GOOSE)
            printf("Discarding goose - unable to send link\n");
#endif
            return( IEC_GOOSE_ERR_LINK );
        }
        return( IEC_GOOSE_SUCCESS );
#if defined(TMW_USE_90_5)
    case tmwGooseProtocol_UDP: {
        unsigned char buffer[ TMW_SPP_MAX_UDP_FRAME ];
        TMW_SPP_ERROR sendStatus;
        int data_offset, data_length;

        status->sppData.data.goose.pHeader = (void *) &status->header;
        status->sppData.data.goose.pData = buff;
        status->sppData.data.goose.nDataLen = len;
        status->sppData.data.goose.func = (TMW_SPP_Goose_EncodeFunc)TMW_EncodeGooseData;
        sendStatus = TMW_SPP_Encode( buffer, sizeof( buffer ),
                                     &data_offset,
                                     &data_length,
                                     &status->sppData );
        if ( sendStatus != TMW_SPP_ERR_Success ) {
            return( IEC_GOOSE_ERR_SIZE );
        }
        sendStatus = TMW_SPP_UDP_Send( (unsigned char *) buffer + data_offset, data_length, &status->sppData );
        if ( sendStatus != TMW_SPP_ERR_Success ) {
            return( IEC_GOOSE_ERR_LINK );
        }
    }
    return( IEC_GOOSE_SUCCESS );
#endif
    default:
        break;
    }
    return( IEC_GOOSE_ERR_PARAMS );
}



/* --------------------------------------------------------------
---   IEC_Goose_Compute_Interval()
---       This function computes the next time to resend a GOOSE
---   Calling sequence:
---       status  - IEC_GooseStatus structure pointer
---       strategy - IEC_Goose_Strategy structure pointer
---   Return:
---       VOID
-------------------------------------------------------------- */
void IEC_Goose_Compute_Interval( IEC_GooseStreamStatus *status, IEC_Goose_Strategy  *strategy )
{
    status->currentInterval = status->nextInterval;

    if (strategy->strategyType == IEC_GOOSE_Profile) {
        if ( strategy->pro.currentIndex < strategy->pro.numProfiles    ) {
            status->nextInterval = strategy->pro.profiles[ strategy->pro.currentIndex++ ];
            return;
        }
        /* Else fall through and use arithmetic */
    }

    if ( status->SqNum )
        status->nextInterval = strategy->ari.multiplier * status->currentInterval;

    if (status->nextInterval > strategy->ari.maxInterval )
        status->nextInterval = strategy->ari.maxInterval ;
}



/* --------------------------------------------------------------
---   IEC_Goose_Startup()
---       This function initializes a new GOOSE message to be sent,
--- either newly created (called from IEC_Goose_Create)
--- or a new state of the GOOSE (called from IEC_Goose_Publish)
---   Calling sequence:
---       status  - IEC_GooseStatus structure pointer
---   Return:
---       IEC_Goose_Error - IEC_Goose_Send()
-------------------------------------------------------------- */
IEC_Goose_Error IEC_Goose_Startup( IEC_GooseStatus *status, unsigned char *buff, int len )
{
    MMSd_time               timeNow;
    TMW_ms_time             msCount;
    IEC_GooseStreamStatus   *pStreamStatus;

    /* -------------------------------------------------------------------- *
     * Set all time to now, increment the StNum and set the SqNum to one    *
     * -------------------------------------------------------------------- */
    MMSd_adjusted_time_stamp( &timeNow );
    MMSd_MmsToUtc( &timeNow, &status->header.t );

    pStreamStatus = &status->normal;

    msCount = tmwTargGetMsCount();
    pStreamStatus->timeStarted = msCount;
    pStreamStatus->timeLastSent = msCount;
    pStreamStatus->timeNext = msCount + status->strategy.ari.firstInterval;
    pStreamStatus->StNum++;
    if (pStreamStatus->StNum == 0)
        pStreamStatus->StNum = 1;

    pStreamStatus->SqNum = 0;
    memcpy( status->goose_data, buff, len );
    status->goose_data_len = len;

    /* -------------------------------------------------------------------- *
     * Set the retransmission strategy                              *
     * -------------------------------------------------------------------- */
    if (status->strategy.strategyType == IEC_GOOSE_Profile) {
        status->strategy.pro.currentIndex = 0;
        IEC_Goose_Compute_Interval( pStreamStatus, &status->strategy );   /* Loads 'next' interval */
    } else {
        pStreamStatus->nextInterval = status->strategy.ari.firstInterval;
    }

    /* -------------------------------------------------------------------- *
     * Compute the next interval to send                            *
     * -------------------------------------------------------------------- */
    IEC_Goose_Compute_Interval( pStreamStatus, &status->strategy );   /* Loads 'next' interval */

    /* -------------------------------------------------------------------- *
     * Set the HoldTim and set the timeNext to resend               *
     * -------------------------------------------------------------------- */
    pStreamStatus->HoldTim = pStreamStatus->nextInterval * 2;
    pStreamStatus->timeNext = msCount + pStreamStatus->currentInterval ;
    /* -------------------------------------------------------------------- *
     * Send the GOOSE (Call Goose_Send)                             *
     * -------------------------------------------------------------------- */
    status->header.StNum = pStreamStatus->StNum;
    status->header.SqNum = pStreamStatus->SqNum;
    status->header.HoldTim = pStreamStatus->HoldTim;
    status->header.test = status->testNextPublished;
    status->header.simulateBit = status->simulateNextPublished;
    status->header.HoldTim = pStreamStatus->HoldTim;

    return( IEC_Goose_Send( status, buff, len ) );
}



/* --------------------------------------------------------------
---   IEC_Goose_Resend()
---       This function prepare a GOOSE message to be retransmitted
---   Calling sequence:
---       status  - IEC_GooseStatus structure pointer
---   Return:
---       VOID
-------------------------------------------------------------- */
void IEC_Goose_Resend( IEC_GooseStatus *status, IEC_GooseStreamStatus *pStreamStatus )
{
    /* -------------------------------------------------------------------- *
     * Compute the next interval, set the HoldTim and increment the SqNum   *
     * -------------------------------------------------------------------- */
    pStreamStatus->SqNum++;
    IEC_Goose_Compute_Interval( pStreamStatus, &status->strategy );   /* Loads 'next' interval */
    pStreamStatus->HoldTim = pStreamStatus->currentInterval * 2;
     if (pStreamStatus->SqNum == 0)
        pStreamStatus->SqNum = 1;

    /* -------------------------------------------------------------------- *
     * Set the timeLastSent to now (last timeNext) and reset the timeNext   *
     * -------------------------------------------------------------------- */
    pStreamStatus->timeLastSent = pStreamStatus->timeNext;
    pStreamStatus->timeNext = pStreamStatus->timeLastSent + pStreamStatus->currentInterval;

    /* -------------------------------------------------------------------- *
     * Send the GOOSE (Call Goose_Send)                             *
     * -------------------------------------------------------------------- */
    status->header.StNum = pStreamStatus->StNum;
    status->header.SqNum = pStreamStatus->SqNum;
    status->header.HoldTim = pStreamStatus->HoldTim;
    IEC_Goose_Send( status, status->goose_data, status->goose_data_len );
}



/* --------------------------------------------------------------
---   IEC_Goose_Service_Short()
---       This function scans for any outbound GOOSE that need to
--- be retransmitted, and for any inbound GOOSE that are
--- overdue to be received (retransmission errors)
---   Calling sequence:
---       cntxt   - IEC_Goose_Contect structure pointer
---   Return:
---       soonest - The soonest time to service again
-------------------------------------------------------------- */
#define MAXLONG 0x7fffffff
unsigned long IEC_Goose_Service_Short( IEC_Goose_Context *cntxt )
{
    TMW_ms_time             msCount;
    long                    msDiff;
    IEC_Goose_Subscription  *entry;
    unsigned long           soonest = (unsigned long) MAXLONG;
    IEC_GooseStreamStatus   *pStreamStatus;

     msCount = tmwTargGetMsCount();

    /* Check for inputs from link */

    /* -------------------------------------------------------------------- *
     * Check for Goose outputs pending and resend if pending        *
     * -------------------------------------------------------------------- */
    entry = cntxt->active_out;
    while (entry != NULL) {
        pStreamStatus = &entry->status.normal;


        msDiff = pStreamStatus->timeNext - msCount;

        if  (msDiff <= 0)  {
            IEC_Goose_Resend( &(entry->status), pStreamStatus );
            if (soonest > pStreamStatus->currentInterval)
                soonest = pStreamStatus->currentInterval;
            if (cntxt->send_indication != NULL)
                cntxt->send_indication( entry->handle, entry, TRUE );
        } else {
            if (((unsigned long) msDiff) < soonest)
                soonest = (unsigned long) msDiff;
        }
        entry = entry->next;
    }

    /* -------------------------------------------------------------------- *
     * Check for Goose inputs pending and notify device if pending  *
     * -------------------------------------------------------------------- */
    entry = cntxt->active_in;
    while (entry != NULL) {
        pStreamStatus = &entry->status.normal;
        if ( ( pStreamStatus->have_received) || ( pStreamStatus->initial_wait) ) {
            msDiff = pStreamStatus->timeNext - msCount;
            if ( msDiff <= 0 ) {
                entry->status.header.StNum = pStreamStatus->StNum;
                entry->status.header.SqNum = pStreamStatus->SqNum;
                entry->status.header.HoldTim = pStreamStatus->HoldTim;
                entry->status.header.simulateBit = 0;
                if (entry->call_back != NULL)
                    entry->call_back( entry->handle, entry->call_back_data, &(entry->status.header), NULL, 0, TRUE );
                pStreamStatus->have_received = FALSE;
                pStreamStatus->initial_wait = FALSE;
#if defined(USE_MMSD_IN_IEC_GOOSE)
                if ( entry->lgos != NULL ) {
                    *( entry->lgos->LGOS_Status ) = FALSE;
                }
#endif
            } else {
                if (((unsigned long) msDiff) < soonest)
                    soonest = (unsigned long) msDiff;
            }
        }

        pStreamStatus = &entry->status.simulated;
        if ( ( pStreamStatus->have_received) || ( pStreamStatus->initial_wait) ) {
           msDiff = pStreamStatus->timeNext - msCount;
            if ( msDiff <= 0 ) {
                entry->status.header.StNum = pStreamStatus->StNum;
                entry->status.header.SqNum = pStreamStatus->SqNum;
                entry->status.header.HoldTim = pStreamStatus->HoldTim;
                entry->status.header.simulateBit = 1;
                if (entry->call_back != NULL)
                    entry->call_back( entry->handle, entry->call_back_data, &(entry->status.header), NULL, 0, TRUE );
                pStreamStatus->have_received = FALSE;
                pStreamStatus->initial_wait = FALSE;
            } else {
                if (((unsigned long) msDiff) < soonest)
                    soonest = (unsigned long) msDiff;
            }
        }
        entry = entry->next;
    }
    return( soonest );
}



/* --------------------------------------------------------------
---   IEC_Goose_Service()
---       This function services the data link and then calls
--- IEC_Goose_Service_Short() to scan for any outbound
--- GOOSE that need to be retransmitted, and for any
--- inbound GOOSE that are overdue to be received
--- (retransmission errors)
---   Calling sequence:
---       cntxt   - IEC_Goose_Contect structure pointer
---   Return:
---       VOID
-------------------------------------------------------------- */
void IEC_Goose_Service( IEC_Goose_Context *cntxt )
{
    if (cntxt->link_context == NULL)
        return;
    TMW_DL_service( cntxt->link_context );
    IEC_Goose_Service_Short( cntxt );
}



/* --------------------------------------------------------------
---   IEC_Goose_insert()
---       This function inserts a GOOSE Subsctiption entry into
--- the pool, either the active_in pool, the active_out
--- pool or the avail pool. Each pool is actually a list
--- of entries pulling from the same pool of Subscription
--- entries.
---   Calling sequence:
---       sub     - pointer to entry to insert into pool
---       pool    - pointer to the pointer to the pool
---   Return:
---       VOID
-------------------------------------------------------------- */
void IEC_Goose_insert( IEC_Goose_Subscription *sub,
                       IEC_Goose_Subscription **pool )
{
    if (*pool == NULL)
        sub->next = NULL;
    else
        sub->next = *pool;
    *pool = sub;
}



/* --------------------------------------------------------------
---   IEC_Goose_remove()
---       This function removes a GOOSE Subsctiption entry from
--- a pool, either the active_in pool or the active_out
--- pool. Each pool is actually a list of entries pulling
--- from the same pool of Subscription entries.
---   Calling sequence:
---       sub     - pointer to entry to remove from pool
---       pool    - pointer to the pointer to the pool
---   Return:
---       VOID
-------------------------------------------------------------- */
void IEC_Goose_remove( IEC_Goose_Subscription *sub,
                       IEC_Goose_Subscription **pool )
{
    while (*pool != NULL) {
        if (*pool == sub) {
            *pool = sub->next;
            sub->next = NULL;
        } else
            pool = &((*pool)->next);
    }
}



/* --------------------------------------------------------------
---   IEC_Goose_find_handle()
---       This function scans throught a Subscription pool for
--- an entry whose handle matches the one passed and
--- returns a pointer to that Subscription entry.
---   Calling sequence:
---       handle  - Used defined handle pointer
---       pool    - pointer to the pointer to the pool
---   Return:
---       Pointer to the pool entry matching handle
-------------------------------------------------------------- */
IEC_Goose_Subscription *IEC_Goose_find_handle( void *handle,
        IEC_Goose_Subscription **pool )
{
    while (*pool != NULL) {
        if ((*pool)->handle == handle) {
            return( *pool );
        }
        pool = &((*pool)->next);
    }
    return( NULL );
}



/* --------------------------------------------------------------
---   IEC_Goose_find_entry()
---       This function scans throught a Subscription pool for
--- an entry whose address and goID match the ones
--- passed and returns a pointer to that Subscription entry.
---   Calling sequence:
---       mac     - Pointer to the MAC address of the GOOSE
---       goID    - Pointer to the goID name of the GOOSE
---       gocbRef - Pointer to the gocbRef name of the GOOSE
---       pool    - pointer to the pointer to the pool
---   Return:
---       Pointer to the pool entry matching both mac and name
-------------------------------------------------------------- */
IEC_Goose_Subscription *IEC_Goose_find_entry(TMW_Mac_Address *mac, char *goID,
                                             char *gocbRef, int appID,
                                             IEC_Goose_Subscription **pool)
{
    int goID_sum;
    char *goIDPtr;

    goID_sum = 0;
    goIDPtr = goID;
    while ( *goIDPtr != 0 ) {
        goID_sum += *goIDPtr;
        goIDPtr++;
    }
    while ( *pool != NULL ) {
        if ( ( *pool )->VLAN_AppID == (unsigned)appID ) {
            if ( ( *pool )->goID_sum == goID_sum ) {
                if ( ( strcmp( goID, ( *pool )->goID ) == 0 )
                        && ( strcmp( gocbRef, ( *pool )->gocbRef ) == 0 )
                        && ( memcmp( mac->addr, ( *pool )->status.address.addr,
                                     ( *pool )->status.address.len ) == 0 ) )
                    return ( *pool );
            }
        }
        pool = &( ( *pool )->next );
    }
    return ( NULL );
}



/* --------------------------------------------------------------
---   IEC_Goose_Initialize()
---       This function initializes the pool of Goose_Subscription
--- records, verifies the link adapter name and opens the
--- transport link (call to ETH_open_port())
---   Calling sequence:
---       cntxt   - IEC_Goose_Contect structure pointer
---   Return:
---       int -  TRUE : Initialization succeeded
---      FALSE: Initialization failed
-------------------------------------------------------------- */
int IEC_Goose_Initialize( IEC_Goose_Context *cntxt )
{
#if defined(IEC_GOOSE_STATIC_ALLOCATION) || defined(IEC_GOOSE_DYNAMIC_ALLOCATION)
    int i;
#endif

    /* -------------------------------------------------------------------- *
     * Initialize pool for goose inputs and outputs                 *
     * -------------------------------------------------------------------- */
    cntxt->active_in = NULL;
    cntxt->active_out = NULL;
    cntxt->send_indication = NULL;
    cntxt->SqStats_indication = NULL;
    cntxt->getReferenceReqFcn = NULL;
    cntxt->getReferenceReqParam = NULL;
    cntxt->getElementNumReqFcn = NULL;
    cntxt->getElementNumReqParam = NULL;
    cntxt->getReferenceResFcn = NULL;
    cntxt->getReferenceResParam = NULL;
    cntxt->getElementNumResFcn = NULL;
    cntxt->getElementNumResParam = NULL;
    cntxt->directory = NULL;
    cntxt->directorySize = 0;
    cntxt->sendtype = IEC_GOOSE_SEND_allData11;
    
#if defined(IEC_GOOSE_STATIC_ALLOCATION)
    for (i = 0; i < MAX_IEC_GOOSE_ENTRIES; i++) {
        if (i < (MAX_IEC_GOOSE_ENTRIES - 1))
            IEC_goose_pool[i].next = &IEC_goose_pool[i + 1];
        else
            IEC_goose_pool[i].next = NULL;
    }
    cntxt->avail_pool = IEC_goose_pool;
#endif
#if defined(IEC_GOOSE_DYNAMIC_ALLOCATION)
    cntxt->avail_pool = NULL;
    for (i = 0; i < MAX_IEC_GOOSE_ENTRIES; i++) {
        IEC_Goose_Subscription *entry;

        entry = (IEC_Goose_Subscription *) TMW_TARG_MALLOC( sizeof( IEC_Goose_Subscription ) );
        if ( entry == NULL )
            break;
        memset( entry, 0, sizeof( IEC_Goose_Subscription ) );
        IEC_Goose_insert( entry, &(cntxt->avail_pool) );
    }
#endif
#if defined(IEC_GOOSE_UNBOUND_ALLOCATION)
    cntxt->avail_pool = NULL;
#endif

    /* -------------------------------------------------------------------- *
     * Verify adapter name                                          *
     * -------------------------------------------------------------------- */
    if (cntxt->adapter_name[0] == 0) {
#if defined(DEBUG_GOOSE)
        printf("No adapter name specified for GOOSE\n");
#endif
        return( FALSE );
    }

    /* -------------------------------------------------------------------- *
     * Open transport link                                          *
     * -------------------------------------------------------------------- */
    cntxt->link_context = ETH_open_port( cntxt->adapter_name );
    if ( cntxt->link_context == NULL ) {
#if defined(DEBUG_GOOSE)
        printf("Adapter %s failed to open for GOOSE\n", cntxt->adapter_name);
#endif
        return( FALSE );
    }

    TMW_TARG_SEM_CREATE( cntxt->sem );

    TMW_TARG_EVENT_CREATE( cntxt->eve );


    /* Bind for the management services */
    TMW_DL_bind( cntxt->link_context, TMW_BIND_802_3
                 | TMW_BIND_GOOSE | TMW_BIND_DIRECTED
                 | TMW_BIND_VL_GSE,
                 NULL, 0,
                 (TMW_DL_Ind) IEC_Goose_Evaluate, (void *) cntxt );
    /* No error recovery for this! */
    return( TRUE );
}


/* --------------------------------------------------------------
---   IEC_Goose_Close()
---       This function shuts down all goose activity for the given context.
---   Calling sequence:
---       cntxt   - IEC_Goose_Contect structure pointer
---   Return:
---      none
-------------------------------------------------------------- */
void IEC_Goose_Close( IEC_Goose_Context *cntxt )
{
    while ( cntxt->active_in != NULL ) {
        IEC_Goose_Unsubscribe( cntxt, (void *) cntxt->active_in->handle );
    }
    while ( cntxt->active_out != NULL ) {
        IEC_Goose_Delete( cntxt, (void *) cntxt->active_out->handle );
    }

#if defined(TMW_USE_THREADS)
    if ( cntxt->sem != NULL ) {
        TMW_TARG_SEM_TAKE( cntxt->sem );
    }
#endif
    if (cntxt->link_context != NULL) {
        /* Also remove the additional bind for GSE directed services */
        TMW_DL_unbind( cntxt->link_context, TMW_BIND_802_3
                       | TMW_BIND_GOOSE | TMW_BIND_DIRECTED | TMW_BIND_VL_GSE,
                       &cntxt->link_context->mac_address, 0,
                       (TMW_DL_Ind) IEC_Goose_Evaluate, (void *) cntxt );
        TMW_DL_close( cntxt->link_context );
    }

#if defined(IEC_GOOSE_DYNAMIC_ALLOCATION)
    while ( cntxt->avail_pool != NULL ) {
        IEC_Goose_Subscription *entry;
        entry = cntxt->avail_pool;
        cntxt->avail_pool = entry->next;
        TMW_TARG_FREE( entry );
    }
#endif
#if defined(TMW_USE_THREADS)
    if (cntxt->eve != NULL) {
        TMW_TARG_EVENT_DELETE(cntxt->eve);
    }
    if (cntxt->sem != NULL) {
        TMW_TARG_SEM_GIVE(cntxt->sem);
        TMW_TARG_SEM_DELETE(cntxt->sem);
    }
#endif
}


/* --------------------------------------------------------------
---   IEC_Goose_Subscribe()
---       This function initializes an IEC_Goose_Subscription
--- entry from the avail_pool with information about a
--- GOOSE it wants to receive, and inserts it into the
--- active_in pool and binds the entry to the data link.
---   Calling sequence:
---       cntxt   - IEC_Goose_Contect structure pointer
---       goID    - Pointer to the goID name of the GOOSE
---       gocbRef - Pointer to the gocbRef name of the GOOSE
---       mac     - Pointer to the broadcast GOOSE address
---          - VLAN Application ID
---       handle  - Used defined handle pointer
---       call_back       - User defined call back function
---       call_back_data  - parameter to pass into call back function
---   Return:
---       int -  TRUE : Subscription succeeded
---      FALSE: Subscription failed
-------------------------------------------------------------- */
int IEC_Goose_Subscribe( IEC_Goose_Context      *cntxt,
                         char                   *goID,
                         char                   *gocbRef,
                         TMW_Mac_Address        *mac,
                         unsigned int           appID,
                         void                   *handle,
                         IEC_Goose_Indication   callBack,
                         void                   *callBackData )
{
    return( IEC_Goose_SubscribeSecure( cntxt,
                                       goID,
                                       gocbRef,
                                       mac,
                                       appID,
                                       handle,
                                       callBack,
                                       callBackData,
                                       NULL,
                                       NULL,
                                       NULL ) );
}


int IEC_Goose_SubscribeSecure( IEC_Goose_Context                *cntxt,                 /* context              */
                               char                             *goID,                  /* goID                 */
                               char                             *gocbRef,               /* gocbRef              */
                               TMW_Mac_Address                  *mac,                   /* multicast address    */
                               unsigned int                     AppID,                  /* VLAN AppId           */
                               void                             *handle,                /* user handle          */
                               IEC_Goose_Indication             callBack,               /* user callback fcn    */
                               void                             *callBackData,          /* user callback data   */
                               TMW_61850_VLAN_CheckSignatureFcn checkVlanSignature,     /* Signature Check fcn  */
                               void                             *pCryptoContext,        /* Signature Check data */
                               void                             *pVlanSignatureData )   /* Signature Check data */
{
    int i, sum;
    IEC_Goose_Subscription *entry;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    /* -------------------------------------------------------------------- *
     * Verify available Subscription entry in avail_pool and link_context   *
     * -------------------------------------------------------------------- */
#if defined(IEC_GOOSE_UNBOUND_ALLOCATION)
    if (cntxt->avail_pool == NULL) {
        IEC_Goose_Subscription *entry;

        entry = (IEC_Goose_Subscription *) TMW_TARG_MALLOC( sizeof( IEC_Goose_Subscription ) );
        if ( entry != NULL ) {
            memset( entry, 0, sizeof( IEC_Goose_Subscription ) );
            IEC_Goose_insert( entry, &(cntxt->avail_pool) );
        }
    }
#endif
    if (cntxt->avail_pool == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    if (cntxt->link_context == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    /* -------------------------------------------------------------------- *
     * Take a Subscription entry from the avail_pool                *
     * -------------------------------------------------------------------- */
    entry = cntxt->avail_pool;
    cntxt->avail_pool = entry->next;

    /* -------------------------------------------------------------------- *
     * Initialize the entry with the parameters passed by the user  *
     * -------------------------------------------------------------------- */
    entry->status.address = *mac;
    entry->handle = handle;
    entry->status.goose_context = cntxt;
    entry->call_back = callBack;
    entry->call_back_data = callBackData;
    entry->status.normal.have_received = FALSE;
    entry->status.normal.initial_wait = FALSE;
    entry->status.simulated.have_received = FALSE;
    entry->status.simulated.initial_wait = FALSE;
    entry->status.header.pPrivateSecurityData = NULL;
    entry->status.header.nPrivateSecurityDataLen = 0;
    entry->checkVlanSignature = checkVlanSignature;
    entry->genVlanSignature = NULL;
    entry->pVlanSignatureData = pVlanSignatureData;
    entry->pCryptoContext = pCryptoContext;
    entry->lgos = NULL;

    /* -------------------------------------------------------------------- *
     * Calculate goID sum for fast lookup (not currently used)     *
     * -------------------------------------------------------------------- */
    sum = 0;
    for (i = 0; i < IEC_GOOSE_IDENT_SIZE - 1; i++) {
        if (*goID == 0) {
            break;
        }
        entry->goID[ i ] = *goID;
        sum += *goID;
        goID++;
    }
    entry->goID_sum = sum;
    entry->goID[i] = 0;
    entry->VLAN_AppID = AppID;

    /* -------------------------------------------------------------------- *
     * Insert entry into active_in pool and bind the entry to the data link *
     * -------------------------------------------------------------------- */
    for (i = 0; i < IEC_GOOSE_IDENT_SIZE - 1; i++) {
        entry->gocbRef[ i ] = *gocbRef;
        entry->status.header.gocbRef[i] = *gocbRef;
        if (*gocbRef == 0)
            break;
        gocbRef++;
    }
    entry->gocbRef[ i ] = 0;
    entry->status.header.gocbRef[i] = 0;
    IEC_Goose_insert( entry, &(cntxt->active_in) );
    if (!TMW_DL_bind( cntxt->link_context, TMW_BIND_802_3
                      | TMW_BIND_GOOSE | TMW_BIND_MULTICAST
                      | TMW_BIND_VL_GOOSE,
                      mac, AppID,
                      (TMW_DL_Ind) IEC_Goose_Evaluate, (void *) cntxt )) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TRUE );
}

#if defined(TMW_USE_90_5)
int IEC_Goose_SubscribeUDP( IEC_Goose_Context                *cntxt,                 /* context              */
                            char                             *goID,                  /* goID                 */
                            char                             *gocbRef,               /* gocbRef              */
                            TMW_Mac_Address                  *address,               /* multicast UDP address */
                            int                              port,                   /* multicast UDP port   */
                            unsigned int                     nAppID,                 /* AppId                */
                            void                             *handle,                /* user handle          */
                            IEC_Goose_Indication             callBack,               /* user callback fcn    */
                            void                             *callBackData )         /* user callback data   */
{
    int i, sum;
    IEC_Goose_Subscription *entry;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    /* -------------------------------------------------------------------- *
     * Verify available Subscription entry in avail_pool and link_context   *
     * -------------------------------------------------------------------- */
#if defined(IEC_GOOSE_UNBOUND_ALLOCATION)
    if (cntxt->avail_pool == NULL) {
        IEC_Goose_Subscription *entry;

        entry = (IEC_Goose_Subscription *) TMW_TARG_MALLOC( sizeof( IEC_Goose_Subscription ) );
        if ( entry != NULL ) {
            memset( entry, 0, sizeof( IEC_Goose_Subscription ) );
            IEC_Goose_insert( entry, &(cntxt->avail_pool) );
        }
    }
#endif
    if (cntxt->avail_pool == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    /* -------------------------------------------------------------------- *
     * Take a Subscription entry from the avail_pool                *
     * -------------------------------------------------------------------- */
    entry = cntxt->avail_pool;
    cntxt->avail_pool = entry->next;

    /* -------------------------------------------------------------------- *
     * Initialize the entry with the parameters passed by the user  *
     * -------------------------------------------------------------------- */
    entry->status.address = *address;
    entry->handle = handle;
    entry->status.goose_context = cntxt;
    entry->call_back = callBack;
    entry->call_back_data = callBackData;
    entry->status.normal.have_received = FALSE;
    entry->status.normal.initial_wait = FALSE;
    entry->status.simulated.have_received = FALSE;
    entry->status.simulated.initial_wait = FALSE;
    entry->status.protocol = tmwGooseProtocol_UDP;        /* 90-5 Extension */

    /* -------------------------------------------------------------------- *
     * Calculate goID sum for fast lookup (not currently used)     *
     * -------------------------------------------------------------------- */
    sum = 0;
    for (i = 0; i < IEC_GOOSE_IDENT_SIZE - 1; i++) {
        if (*goID == 0) {
             break;
        }
        entry->goID[ i ] = *goID;
        sum += *goID;
        goID++;
    }
    entry->goID_sum = sum;
    entry->goID[i] = 0;
    entry->VLAN_AppID = nAppID;

    /* -------------------------------------------------------------------- *
     * Insert entry into active_in pool and bind the entry to the data link *
     * -------------------------------------------------------------------- */
    for (i = 0; i < IEC_GOOSE_IDENT_SIZE - 1; i++) {
        entry->gocbRef[ i ] = *gocbRef;
        if (*gocbRef == 0)
            break;
        gocbRef++;
    }
    entry->gocbRef[ i ] = 0;

    memset( &entry->status.sppData, 0, sizeof( entry->status.sppData ) );
    entry->status.sppData.data.goose.decode_func = TMW_Goose_ParseAndIndicate;
    entry->status.sppData.data.goose.pStatus = &entry->status;
    if ( TMW_SPP_UDP_Subscribe( &entry->status.sppData,
                                TMW_SPP_SPDU_Goose,
                                address,
                                port,
                                nAppID ) != TMW_SPP_ERR_Success ) {
#if defined(IEC_GOOSE_UNBOUND_ALLOCATION)
        TMW_TARG_FREE( entry );
#else
        IEC_Goose_insert( entry, &(cntxt->avail_pool) );
#endif
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    IEC_Goose_insert( entry, &(cntxt->active_in) );
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TRUE );
}
#endif

int IEC_Goose_Subscribe_Common( IEC_Goose_Context      *cntxt,
								char                   *goID,
								char                   *gocbRef,
								TMW_Mac_Address        *mac,
								unsigned int           appID,
								void                   *handle,
								IEC_Goose_Indication   callBack,
								void                   *callBackData,
								tmwGooseProtocol_t     protocol		)
{
	if ( protocol == tmwGooseProtocol_UDP )
#if defined(TMW_USE_90_5)
		return IEC_Goose_SubscribeUDP( cntxt, goID, gocbRef, mac,102, appID, handle, callBack, callBackData );
#else
		return 0;
#endif
	else
		return( IEC_Goose_SubscribeSecure( cntxt, goID, gocbRef, mac, appID, handle, callBack, callBackData, NULL, NULL, NULL ) );
}

/* --------------------------------------------------------------
---   IEC_Goose_Unsubscribe()
---       This function finds the IEC_Goose_Subscription entry
--- in the active_in pool that matches the handle, unbinds
--- it and removes it from the active_in pool.
---   Calling sequence:
---       cntxt   - IEC_Goose_Contect structure pointer
---       handle  - Used defined handle pointer
---   Return:
---       int -  TRUE : Unsubscribe succeeded
---      FALSE: Unsubscribe failed
-------------------------------------------------------------- */
int IEC_Goose_Unsubscribe( IEC_Goose_Context *cntxt, void *handle )
{
    IEC_Goose_Subscription *entry;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_in pool that matches handle        *
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_in) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    switch ( entry->status.protocol ) {
    case tmwGooseProtocol_Normal:
        /* -------------------------------------------------------------------- *
         * Unbind entry from the data link                                      *
         * -------------------------------------------------------------------- */
        if (cntxt->link_context != NULL)
            TMW_DL_unbind( cntxt->link_context, TMW_BIND_802_3
                           | TMW_BIND_GOOSE | TMW_BIND_MULTICAST | TMW_BIND_VL_GOOSE,
                           &entry->status.address, entry->VLAN_AppID,
                           (TMW_DL_Ind) IEC_Goose_Evaluate, (void *) cntxt );
        break;
#if defined(TMW_USE_90_5)
    case tmwGooseProtocol_UDP:
        TMW_SPP_UDP_Unsubscribe( &entry->status.sppData );
        break;
#endif
    default:
        return( FALSE );
    }


    /* -------------------------------------------------------------------- *
     * Remove entry from active_in pool and insert it back into avail_pool  *
     * -------------------------------------------------------------------- */
    IEC_Goose_remove( entry, &(cntxt->active_in) );
#if defined(IEC_GOOSE_UNBOUND_ALLOCATION)
    TMW_TARG_FREE( entry );
#else
    IEC_Goose_insert( entry, &(cntxt->avail_pool) );
#endif
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TRUE );
}

/* handle initial timeout */
int IEC_Goose_SetInitialWait( IEC_Goose_Context *cntxt,
                              void *handle,
                              unsigned long timeToWait )
{
    IEC_Goose_Subscription *entry;
    TMW_ms_time msCount;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_in pool that matches handle*
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_in) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    entry->maxInterval = timeToWait;
    msCount = tmwTargGetMsCount();
    if ( !entry->status.normal.have_received ) {
        entry->status.normal.timeNext = timeToWait + msCount;
        entry->status.normal.initial_wait = TRUE;
    }

    if ( !entry->status.simulated.have_received ) {
        entry->status.simulated.timeNext = timeToWait + msCount;
        entry->status.simulated.initial_wait = TRUE;
    }

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TRUE );
}


/* Support for changing to test/simulate mode */
int IEC_Goose_GetOutputTestMode( IEC_Goose_Context *cntxt,
                                 void *handle,
                                 unsigned char *testMode )
{
    IEC_Goose_Subscription *entry;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_in pool that matches handle        *
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_out) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    *testMode = entry->status.testNextPublished;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TRUE );
}

/* Support for changing to test/simulate mode */
int IEC_Goose_SetOutputTestMode( IEC_Goose_Context *cntxt,
                                 void *handle,
                                 unsigned char testMode )
{
    IEC_Goose_Subscription *entry;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_in pool that matches handle        *
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_out) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    entry->status.testNextPublished = testMode;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TRUE );
}

/* Support for changing to test/simulate mode */
int IEC_Goose_GetOutputSimulateMode( IEC_Goose_Context *cntxt,
                                     void *handle,
                                     unsigned char *simulateMode )
{
    IEC_Goose_Subscription *entry;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_in pool that matches handle*
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_out) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    *simulateMode = entry->status.simulateNextPublished;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TRUE );
}

/* Support for changing to test/simulate mode */
int IEC_Goose_SetOutputSimulateMode( IEC_Goose_Context *cntxt,
                                     void *handle,
                                     unsigned char simulateMode )
{
    IEC_Goose_Subscription *entry;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif

    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_in pool that matches handle*
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_out) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    entry->status.simulateNextPublished = simulateMode;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( TRUE );
}



IEC_Goose_Error IEC_Goose_CreateSecure( IEC_Goose_Context               *cntxt,
                                        char                            *goID,
                                        char                            *gocbRef,
                                        char                            *dsRef,
                                        TMW_Mac_Address                 *mac,
                                        IEC_Goose_Strategy              *strategy,
                                        long                            confRev,
                                        unsigned char                   ndsCom,
                                        unsigned char                   test,
                                        unsigned char                   simulateBit,
                                        int                             numDataSetEntries,
                                        unsigned char                   bFixedOffs,
                                        unsigned char                   *buffer,
                                        MMSd_length                     length,
                                        void                            *handle,
                                        unsigned int                    VLAN_AppID,
                                        unsigned int                    VLAN_VID,
                                        unsigned char                   VLAN_Priority,
                                        unsigned char                   *pPrivateSecurityData,
                                        int                             nPrivateSecurityDataLen,
                                        TMW_61850_VLAN_MakeSignatureFcn makeVlanSignature,
                                        void                            *pCryptoContext,
                                        void                            *pVlanSignatureData )
{
    int i, sum;
    IEC_Goose_Subscription *entry;
    IEC_Goose_Error err;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    /* -------------------------------------------------------------------- *
     * Verify available Subscription entry in avail_pool                    *
     * -------------------------------------------------------------------- */
#if defined(IEC_GOOSE_UNBOUND_ALLOCATION)
    if (cntxt->avail_pool == NULL) {
        IEC_Goose_Subscription *entry;

        entry = (IEC_Goose_Subscription *) TMW_TARG_MALLOC( sizeof( IEC_Goose_Subscription ) );
        if ( entry != NULL ) {
            memset( entry, 0, sizeof( IEC_Goose_Subscription ) );
            IEC_Goose_insert( entry, &(cntxt->avail_pool) );
        }
    }
#endif
    if (cntxt->avail_pool == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        //  return( FALSE ); 
        return(IEC_GOOSE_SUCCESS); // return type is IEC_Goose_Error
    }

    /* -------------------------------------------------------------------- *
     * Take a Subscription entry from the avail_pool                        *
     * -------------------------------------------------------------------- */
    entry = cntxt->avail_pool;
    cntxt->avail_pool = entry->next;

    /* -------------------------------------------------------------------- *
     * Initialize the entry with the parameters passed by the user          *
     * -------------------------------------------------------------------- */
    entry->handle = handle;
    entry->status.handle = handle;
    entry->status.protocol = tmwGooseProtocol_Normal;             /* 90-5 Extension */
    entry->genVlanSignature = makeVlanSignature;
    entry->checkVlanSignature = NULL;
    entry->pVlanSignatureData = pVlanSignatureData;
    entry->pCryptoContext = pCryptoContext;
    memset( &entry->status.header, 0, sizeof( entry->status.header ) );
    entry->status.simulateNextPublished = simulateBit;
    entry->status.testNextPublished = test;
    entry->status.header = init_goose_header;
    entry->status.header.fixedOffs = bFixedOffs;
    entry->status.header.ndsCom = ndsCom;
    entry->status.header.confRev = confRev;
    entry->status.header.numDataSetEntries = numDataSetEntries;
    entry->status.header.pPrivateSecurityData = pPrivateSecurityData;
    entry->status.header.nPrivateSecurityDataLen = nPrivateSecurityDataLen;
    entry->status.header.VLAN_AppID = VLAN_AppID;
    entry->status.header.VLAN_VID = VLAN_VID;
    entry->status.header.VLAN_Priority = VLAN_Priority;

    if (goID != NULL) {
        i = (int) strlen( goID ) + 1;
        if (i > IEC_GOOSE_IDENT_SIZE)
            i = IEC_GOOSE_IDENT_SIZE;
        memcpy( entry->status.header.goID, goID, i );
    } else
        entry->status.header.goID[ 0 ] = 0;

    if (gocbRef != NULL) {
        i = (int) strlen( gocbRef ) + 1;
        if (i > IEC_GOOSE_IDENT_SIZE)
            i = IEC_GOOSE_IDENT_SIZE;
        memcpy( entry->status.header.gocbRef, gocbRef, i );
    } else
        entry->status.header.gocbRef[ 0 ] = 0;

    if (dsRef != NULL) {
        i = (int) strlen( dsRef ) + 1;
        if (i > IEC_GOOSE_IDENT_SIZE)
            i = IEC_GOOSE_IDENT_SIZE;
        memcpy( entry->status.header.datSet, dsRef, i );
    } else
        entry->status.header.datSet[ 0 ] = 0;
    entry->status.address = *mac;
    entry->status.strategy = *strategy;
    IEC_Goose_InitStatus( &entry->status );
    entry->status.goose_context = cntxt;

    /* -------------------------------------------------------------------- *
     * Calculate goID sum for fast lookup (not currently used)              *
     * -------------------------------------------------------------------- */
    sum = 0;
    for (i = 0; i < IEC_GOOSE_IDENT_SIZE - 1; i++) {
        if (*goID == 0) {
            break;
        }
        entry->goID[ i ] = *goID;
        sum += *goID;
        goID++;
    }
    entry->goID_sum = sum;
    entry->goID[i] = 0;

    /* -------------------------------------------------------------------- *
     * Insert entry into active_out pool and startup the GOOSE              *
     * -------------------------------------------------------------------- */
    IEC_Goose_insert( entry, &(cntxt->active_out) );
    err = IEC_Goose_Startup( &entry->status, buffer, length );
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
#if defined(TMW_USE_THREADS)
    TMW_TARG_EVENT_SIGNAL( cntxt->eve );
#endif
    if (cntxt->send_indication != NULL)
        cntxt->send_indication( entry->handle, entry, FALSE );
    return( err );
}


#if defined(TMW_USE_90_5)
IEC_Goose_Error IEC_Goose_CreateUDP( IEC_Goose_Context               *cntxt,
                                     char                            *goID,
                                     char                            *gocbRef,
                                     char                            *dsRef,
                                     TMW_Mac_Address                 *mac,
                                     int                             port,
                                     IEC_Goose_Strategy              *strategy,
                                     long                            confRev,
                                     unsigned char                   ndsCom,
                                     unsigned char                   test,
                                     unsigned char                   simulateBit,
                                     int                             numDataSetEntries,
                                     unsigned char                   bFixedOffs,
                                     unsigned char                   *buffer,
                                     MMSd_length                     length,
                                     void                            *handle,
                                     unsigned int                    VLAN_AppID )
{
    int i, sum;
    IEC_Goose_Subscription *entry;
    IEC_Goose_Error err;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    /* -------------------------------------------------------------------- *
     * Verify available Subscription entry in avail_pool                    *
     * -------------------------------------------------------------------- */
#if defined(IEC_GOOSE_UNBOUND_ALLOCATION)
    if (cntxt->avail_pool == NULL) {
        IEC_Goose_Subscription *entry;

        entry = (IEC_Goose_Subscription *) TMW_TARG_MALLOC( sizeof( IEC_Goose_Subscription ) );
        if ( entry != NULL ) {
            memset( entry, 0, sizeof( IEC_Goose_Subscription ) );
            IEC_Goose_insert( entry, &(cntxt->avail_pool) );
        }
    }
#endif
    if (cntxt->avail_pool == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    /* -------------------------------------------------------------------- *
     * Take a Subscription entry from the avail_pool                        *
     * -------------------------------------------------------------------- */
    entry = cntxt->avail_pool;
    cntxt->avail_pool = entry->next;

    /* -------------------------------------------------------------------- *
     * Initialize the entry with the parameters passed by the user          *
     * -------------------------------------------------------------------- */
    entry->handle = handle;
    entry->status.protocol = tmwGooseProtocol_UDP;             /* 90-5 Extension */
    memset( &entry->status.header, 0, sizeof( entry->status.header ) );
    entry->status.simulateNextPublished = simulateBit;
    entry->status.testNextPublished = test;
    entry->status.header = init_goose_header;
    entry->status.header.fixedOffs = bFixedOffs;
    entry->status.header.ndsCom = ndsCom;
    entry->status.header.confRev = confRev;
    entry->status.header.numDataSetEntries = numDataSetEntries;

    /* Add VLAN pars */
    entry->status.header.VLAN_AppID = VLAN_AppID;

    if (goID != NULL) {
        i = (int) strlen( goID ) + 1;
        if (i > IEC_GOOSE_IDENT_SIZE)
            i = IEC_GOOSE_IDENT_SIZE;
        memcpy( entry->status.header.goID, goID, i );
    } else
        entry->status.header.goID[ 0 ] = 0;

    if (gocbRef != NULL) {
        i = (int) strlen( gocbRef ) + 1;
        if (i > IEC_GOOSE_IDENT_SIZE)
            i = IEC_GOOSE_IDENT_SIZE;
        memcpy( entry->status.header.gocbRef, gocbRef, i );
    } else
        entry->status.header.gocbRef[ 0 ] = 0;

    if (dsRef != NULL) {
        i = (int) strlen( dsRef ) + 1;
        if (i > IEC_GOOSE_IDENT_SIZE)
            i = IEC_GOOSE_IDENT_SIZE;
        memcpy( entry->status.header.datSet, dsRef, i );
    } else
        entry->status.header.datSet[ 0 ] = 0;
    entry->status.address = *mac;
    entry->status.strategy = *strategy;
    IEC_Goose_InitStatus( &entry->status );
    entry->status.goose_context = cntxt;

    /* -------------------------------------------------------------------- *
     * Calculate goID sum for fast lookup (not currently used)              *
     * -------------------------------------------------------------------- */
    sum = 0;
    for (i = 0; i < IEC_GOOSE_IDENT_SIZE - 1; i++) {
        if (*goID == 0) {
            break;
        }
        entry->goID[ i ] = *goID;
        sum += *goID;
        goID++;
    }
    entry->goID_sum = sum;
    entry->goID[i] = 0;

    if ( TMW_SPP_UDP_Create( &entry->status.sppData,
                             TMW_SPP_SPDU_Goose,
                             mac,
                             port,
                             simulateBit,
                             VLAN_AppID ) != TMW_SPP_ERR_Success ) {
        IEC_Goose_insert( entry, &(cntxt->avail_pool) );
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( IEC_GOOSE_ERR_LINK );
    }

    /* -------------------------------------------------------------------- *
     * Insert entry into active_out pool and startup the GOOSE              *
     * -------------------------------------------------------------------- */
    IEC_Goose_insert( entry, &(cntxt->active_out) );
    err = IEC_Goose_Startup( &entry->status, buffer, length );
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
#if defined(TMW_USE_THREADS)
    TMW_TARG_EVENT_SIGNAL( cntxt->eve );
#endif
    if (cntxt->send_indication != NULL)
        cntxt->send_indication( entry->handle, entry, FALSE );
    return( err );
}
#endif

/* --------------------------------------------------------------
---   IEC_Goose_Create()
---       This function initializes an IEC_Goose_Subscription
--- entry from the avail_pool with information about a
--- GOOSE it wants to create, and inserts it into the
--- active_out pool and starts the GOOSE transmitting.
---   Calling sequence:
---       cntxt   - IEC_Goose_Contect structure pointer
---       goID    - Pointer to the goID name of the GOOSE
---       gocbRef - Pointer to the gocbRef name of the GOOSE
---       dsRef   - Pointer to the data reference the GOOSE
---       mac     - Pointer to the broadcast GOOSE address
---       strategy- Pointer to GOOSE strategy
---       confRev - Config reference
---       buffer  - Buffer to use
---       length  - Length of the buffer
---       handle  - Used defined handle pointer
---       VLAN_AppID      - Application ID for VLAN frame
---       VLAN_VID- Virtual LAN ID for VLAN frame
---       VLAN_Priority   - Priority for VLAN frame
---   Return:
---       IEC_Goose_Error - IEC_Goose_Startup() if succeeded
---                 FALSE: No available pool entry
-------------------------------------------------------------- */
IEC_Goose_Error IEC_Goose_Create( IEC_Goose_Context *cntxt,
                                  char              *goID,
                                  char              *gocbRef,
                                  char              *dsRef,
                                  TMW_Mac_Address   *mac,
                                  IEC_Goose_Strategy *strategy,
                                  long              confRev,
                                  int               numDataSetEntries,
                                  unsigned char     bFixedOffs,
                                  unsigned char     *buffer,
                                  MMSd_length       length,
                                  void              *handle,
                                  unsigned int      VLAN_AppID,
                                  unsigned int      VLAN_VID,
                                  unsigned char     VLAN_Priority )
{
    return( IEC_Goose_CreateSecure( cntxt,
                                    goID,
                                    gocbRef,
                                    dsRef,
                                    mac,
                                    strategy,
                                    confRev,
                                    0,      /* ndsCon      */
                                    0,      /* test        */
                                    0,      /* simulateBit */
                                    numDataSetEntries,
                                    bFixedOffs,
                                    buffer,
                                    length,
                                    handle,
                                    VLAN_AppID,
                                    VLAN_VID,
                                    VLAN_Priority,
                                    NULL,
                                    0,
                                    NULL,
                                    NULL,
                                    NULL ) );
}


/* --------------------------------------------------------------
---   IEC_Goose_CreateOut()
---      Same as IEC_Goose_Create (above), but adds ndsCom and test
---      parameters (used by IEDScout).
---   Return:
---       IEC_Goose_Error - IEC_Goose_Startup() if succeeded
---                 FALSE: No available pool entry
-------------------------------------------------------------- */
IEC_Goose_Error IEC_Goose_CreateOut( IEC_Goose_Context *cntxt,
                                     char              *goID,
                                     char              *gocbRef,
                                     char              *dsRef,
                                     TMW_Mac_Address   *mac,
                                     IEC_Goose_Strategy *strategy,
                                     long              confRev,
                                     unsigned char     ndsCom,
                                     unsigned char     test,
                                     int               numDataSetEntries,
                                     unsigned char     bFixedOffs,
                                     unsigned char     *buffer,
                                     MMSd_length       length,
                                     void              *handle,
                                     unsigned int      VLAN_AppID,
                                     unsigned int      VLAN_VID,
                                     unsigned char     VLAN_Priority )
{
    return( IEC_Goose_CreateSecure( cntxt,
                                    goID,
                                    gocbRef,
                                    dsRef,
                                    mac,
                                    strategy,
                                    confRev,
                                    ndsCom,
                                    test,
                                    0,      /* simulateBit */
                                    numDataSetEntries,
                                    bFixedOffs,
                                    buffer,
                                    length,
                                    handle,
                                    VLAN_AppID,
                                    VLAN_VID,
                                    VLAN_Priority,
                                    NULL,
                                    0,
                                    NULL,
                                    NULL,
                                    NULL ) );
}


/* --------------------------------------------------------------
---   IEC_Goose_Delete()
---       This function finds the IEC_Goose_Subscription entry
--- in the active_out pool that matches the handle, and
--- removes it from the active_out pool.
---   Calling sequence:
---       cntxt   - IEC_Goose_Contect structure pointer
---       handle  - Used defined handle pointer
---   Return:
---       int -  TRUE : Goose_Delete succeeded
---      FALSE: Goose_Delete failed
-------------------------------------------------------------- */
int IEC_Goose_Delete( IEC_Goose_Context *cntxt, void *handle )
{
    IEC_Goose_Subscription *entry;
    IEC_Goose_Error        nError;

    if (cntxt == NULL)
        return( FALSE );

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_out pool that matches handle       *
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_out) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    nError = IEC_GOOSE_SUCCESS;
    switch ( entry->status.protocol ) {
    case tmwGooseProtocol_Normal:
        break;
#if defined(TMW_USE_90_5)
    case tmwGooseProtocol_UDP:
        if ( TMW_SPP_UDP_Delete( &entry->status.sppData ) == TMW_SPP_ERR_Success )
            nError = IEC_GOOSE_ERR_LINK;
        break;
#endif
    default:
        break;
    }

    /* -------------------------------------------------------------------- *
     * Remove entry from active_out pool and insert it back into avail_pool *
     * -------------------------------------------------------------------- */
    IEC_Goose_remove( entry, &(cntxt->active_out) );
#if defined(IEC_GOOSE_UNBOUND_ALLOCATION)
    TMW_TARG_FREE( entry );
#else
    IEC_Goose_insert( entry, &(cntxt->avail_pool) );
#endif
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    if ( nError == IEC_GOOSE_SUCCESS )
        return( TRUE );
    else
        return( FALSE );
}



/* --------------------------------------------------------------
---   IEC_Goose_Publish()
---       This function updates the header data in a currently
--- active outbound GOOSE, and sends it.
---   Calling sequence:
---       cntxt   - IEC_Goose_Contect structure pointer
---       handle  - Used defined handle pointer
---       header  - Pointer to IEC_GooseHeader
---       buffer  - Buffer to use
---       length  - Length of buffer
---   Return:
---       int -  TRUE : Goose Published
---      FALSE: Goose Not Published
-------------------------------------------------------------- */
int IEC_Goose_Publish( IEC_Goose_Context *cntxt,
                       void              *handle,
                       IEC_GooseHeader   *header,
                       int               numDataSetEntries,
                       unsigned char     *buffer,
                       int               length )
{
    IEC_Goose_Subscription *entry;

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_out pool that matches handle       *
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_out) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( FALSE );
    }

    /* -------------------------------------------------------------------- *
     * Update the header data, and Startup the GOOSE                        *
     * -------------------------------------------------------------------- */
    entry->status.header.numDataSetEntries = numDataSetEntries;
    entry->status.header.genVlanSignature = entry->genVlanSignature;
    entry->status.header.pVlanSignatureData = entry->pVlanSignatureData;
    entry->status.header.pCryptoContext = entry->pCryptoContext;
    entry->status.header.checkVlanSignature = NULL;
    IEC_Goose_Startup( &entry->status, buffer, length );
    if (header != NULL)
        *header = entry->status.header;
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
#if defined(TMW_USE_THREADS)
    TMW_TARG_EVENT_SIGNAL( cntxt->eve );
#endif
    if (cntxt->send_indication != NULL)
        cntxt->send_indication( entry->handle, entry, FALSE );
    return( TRUE );
}


/* --------------------------------------------------------------
--    IEC_Goose_CompareSqNum()
--       This function compares two SqNum fields.  It returns
--    TRUE if the second number is the successor of the first.
-------------------------------------------------------------- */
int IEC_Goose_CompareSqNum( unsigned long sq1, unsigned long sq2 )
{
	if ( sq2 == 0 )  /* state change */
		return ( TRUE );

    sq1++;

    if (sq1 == 0) /* roll over */
        sq1 = 1;

    return( sq1 == sq2 );
}


/* --------------------------------------------------------------
---   IEC_Goose_Evaluate()
---       This function evaluates all incoming GOOSE to verify
--- proper encoding and to determine if the GOOSE is
--- subscribed to, and to pass the GOOSE to the user if
--- needed.
---   Calling sequence:
---       dl_cntxt- TMW_DL_Contect structure pointer
---       buffer  - pointer to incomming GOOSE buffer
---       length  - length of incomming buffer
---       vcntxt  - void pointer to IEC_Goose_Context
---   Return:
---       int -  TRUE : Valid GOOSE evaluated and passed if needed
---      FALSE: Invalid GOOSE or GOOSE not subscribed to
-------------------------------------------------------------- */
int IEC_Goose_Evaluate( TMW_DL_Context  *dl_cntxt,
                        unsigned char   *buffer,
                        int             length,
                        void            *vcntxt )
{
    IEC_Goose_Subscription  *entry;
    IEC_GooseHeader         header;
    IEC_GooseStreamStatus   *pStreamStatus;
    unsigned char           *dataPtr;
    MMSd_length             dataLen;
    TMW_ms_time             msCount;
    TMW_Mac_Address         macAddr, srcAddr;
    IEC_Goose_Context       *cntxt = (IEC_Goose_Context *) vcntxt;
    int                     had_received;

    if (length < 9) {
        return( FALSE );
    }

    /* -------------------------------------------------------------------- *
     * Save source address to make GSE requests                             *
     * -------------------------------------------------------------------- */
    srcAddr.len = 6;
    memcpy( srcAddr.addr, buffer - 8, 6);
    if ( IEC_Goose_Test( cntxt, &srcAddr, buffer, length, &header, &dataPtr, &dataLen ) ) {

#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
        /* -------------------------------------------------------------------- *
         * Find the subscription entry from the active_in list that matches     *
         * the goID, gocbRef and the mac address.  If match continue, else      *
         * fail.                                                                *
         * -------------------------------------------------------------------- */

        /* -------------------------------------------------------------------- *
         * Capture the destination mac address for the incoming GOOSE           *
         * -------------------------------------------------------------------- */
        macAddr.len = 6;            /* buffer is now set at the mac address */
        memcpy( macAddr.addr, buffer - 14, 6);

        entry = IEC_Goose_find_entry( &macAddr, header.goID, header.gocbRef,
                                      header.VLAN_AppID,
                                      &(cntxt->active_in) );
        if (entry != NULL) {
            /* -------------------------------------------------------------------- *
             * Security check                                                       *
             * -------------------------------------------------------------------- */
            if ( ( header.lengthOfExtension != 0 ) && ( entry->checkVlanSignature != NULL ) ) {
                unsigned char   *pSignature = NULL;
                int             nSignatureLen = 0;
                unsigned char   *pToBeSigned;
                int             nToBeSignedLength;
                MMSd_descriptor D, save;

                header.pPrivateSecurityData = NULL;
                header.nPrivateSecurityDataLen = 0;

                MMSd_makeDescriptor(D, dataPtr + dataLen, (MMSd_length ) header.lengthOfExtension, 0);
                MMSd_fetchTAG_and_LENGTH( &D );

                if ( D.tag == VLAN_Security_SecurityExtension0 ) {
                    save = D;
                    nToBeSignedLength = length - header.lengthOfExtension + MMSd_DescIndex( D ) - 2;
                    MMSd_fetchTAG_and_LENGTH( &D );
                    if ( D.tag == VLAN_Security_Reserved1 ) {
                        MMSd_skipToken( D );
                        nToBeSignedLength = length - header.lengthOfExtension + MMSd_DescIndex( D ) - 2;
                        MMSd_fetchTAG_and_LENGTH( &D );
                    }
                    if ( D.tag == VLAN_Security_Private2 ) {
                        header.pPrivateSecurityData = MMSd_getDescPtr( D );
                        header.nPrivateSecurityDataLen = MMSd_TokenLen( D );
                        MMSd_skipToken( D );
                        nToBeSignedLength = length - header.lengthOfExtension + MMSd_DescIndex( D ) - 2;
                        MMSd_fetchTAG_and_LENGTH( &D );
                    }

                    pToBeSigned = buffer + 2;   //??
                    if ( D.tag == VLAN_Security_AuthenticationValue3 ) {
                        pSignature = MMSd_getDescPtr( D );
                        nSignatureLen = MMSd_TokenLen( D );
                        MMSd_skipToken( D );
                    }
                    if ( ( !MMSd_fetchCOMPLETE( &save, &D ) ) || ( pSignature == NULL ) || ( nSignatureLen == 0 ) ) {
                        /* ERROR */
                    }
                    if ( !entry->checkVlanSignature( pToBeSigned,
                                                     nToBeSignedLength,
                                                     pSignature,
                                                     nSignatureLen,
                                                     entry->pCryptoContext,
                                                     entry->pVlanSignatureData ) ) {
                        /* ERROR */
                        /* SECURITY - log signature check failure */
                        return( TRUE );         /* So processing will not proceed with this frame */
                    }
                } else {
                    /* ERROR */
                    /* SECURITY - log signature extension format failure */
                    return( TRUE );         /* So processing will not proceed with this frame */
                }
            } else {
                if ( entry->checkVlanSignature != NULL ) {
                    /* ERROR */
                    /* SECURITY - log security failure - expecting secure PDU */
                    return( TRUE );         /* So processing will not proceed with this frame */
                }
                /* else (if no extension or no signature checking callback, handle as if no security */
            }
            /* -------------------------------------------------------------------- *
             * Save source address to make GSE requests                             *
             * -------------------------------------------------------------------- */
            entry->status.srcAddress = srcAddr;

            /* -------------------------------------------------------------------- *
             * Save VLAN address to make GSE requests                               *
             * -------------------------------------------------------------------- */
            /* Must be done before callback indication */
            entry->status.header.VLAN_VID = header.VLAN_VID;
            entry->status.header.VLAN_AppID = header.VLAN_AppID;
            entry->status.header.VLAN_Priority = header.VLAN_Priority;
            entry->status.header.simulateBit = header.simulateBit;
            if ( header.simulateBit )
                pStreamStatus = &entry->status.simulated;
            else
                pStreamStatus = &entry->status.normal;

            /* -------------------------------------------------------------------- *
             * added check for gaps in SqNum that don't cause timeout               *
             * to be reported to user for statistical purposes.                     *
             * -------------------------------------------------------------------- */
            if ( pStreamStatus->have_received ) {
				if (!IEC_Goose_CompareSqNum(pStreamStatus->SqNum, header.SqNum) ) {
					/* At least one goose transmission was missed. If the
					   Statistical call_back is not NULL, call it here    */
					if (cntxt->SqStats_indication != NULL)
						cntxt->SqStats_indication( entry->handle, entry,  &(header) );
				}
            }

            pStreamStatus->initial_wait = FALSE;
            /* -------------------------------------------------------------------- *
             * If the StNum has changed or this GOOSE has never been received,      *
             * call the user call back function with the timeout FALSE              *
             * -------------------------------------------------------------------- */
            had_received = pStreamStatus->have_received;
            msCount = tmwTargGetMsCount();
            pStreamStatus->have_received = TRUE;
            if ((pStreamStatus->StNum != header.StNum) || !had_received ) {
                entry->status.header = header;
                if (entry->call_back != NULL)
                    entry->call_back( entry->handle, entry->call_back_data, &(header), dataPtr, dataLen, FALSE );
#if defined(USE_MMSD_IN_IEC_GOOSE)                
		if ( entry->lgos != NULL ) {
                    *( entry->lgos->LGOS_RxConf ) = header.confRev;
                    *( entry->lgos->LGOS_StNum ) = header.StNum;
                    *( entry->lgos->LGOS_Status ) = TRUE;
                  }
#endif
            } else {
                entry->status.srcAddress = srcAddr;
                entry->status.header = header;
            }

            /* -------------------------------------------------------------------- *
             * Update the TAL                                                       *
             * -------------------------------------------------------------------- */

            pStreamStatus->StNum = header.StNum;
            pStreamStatus->SqNum = header.SqNum;
            pStreamStatus->HoldTim = header.HoldTim;
            pStreamStatus->timeNext = msCount + pStreamStatus->HoldTim;
       }
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( TRUE );
    }
    return( FALSE );
}

MMSd_length GSE_Encode_Request( IEC_PrimitiveType       idType,       /* Both */
                                char                    *gocbRef,     /* Both */
                                char                    *dataSetName, /* Res  */
                                long                    confRev,      /* Res  */
                                unsigned long           stateId,      /* Both */
                                int                     errCode,      /* Res  */
                                MMSd_length             length,       /* Both */
                                TMW_Packet              *packet )     /* Both */
//{
//    return( length );
//}

{
    MMSd_length     namLen;
    MMSd_descriptor E;

    MMSd_makeDescriptor( E, packet->buffer,
                         (MMSd_ushort) packet->buffer_length,
                         (MMSd_ushort) ( packet->data_offset - 1 ) );
    namLen = 0;
    switch ( idType ) {
    case GSE_Elements_Req:
    case GOOSE_Elements_Req:
        length = IEC_Fast_EmitTagAndLength( offset1, length, &E );
        namLen = IEC_Fast_EmitVisibleString( gocbRef, ident0, &E );
//        length = IEC_Fast_EmitTagAndLength( IEC_Goose_ResponseTag[idType],
//                                            (MMSd_length) (length + namLen),
//                                            &E );
        length = IEC_Fast_EmitTagAndLength( requests1, length, &E );
        break;
    case GSE_References_Req:
    case GOOSE_References_Req:
        length = IEC_Fast_EmitTagAndLength( references1, length, &E );
        namLen = IEC_Fast_EmitVisibleString( gocbRef, ident0, &E );
//        length = IEC_Fast_EmitTagAndLength( IEC_Goose_ResponseTag[idType],
//                                            (MMSd_length) (length + namLen),
//                                            &E );
        length = IEC_Fast_EmitTagAndLength( requests1, length, &E );
        break;
    case GSE_Elements_Res:
    case GOOSE_Elements_Res:
    case GSE_References_Res:
    case GOOSE_References_Res:
        if (errCode >= 0) {
            length = IEC_Fast_EmitIntegerValue( (long) errCode,
                                                responseNegative3,
                                                &E );
        } else {
            length = IEC_Fast_EmitTagAndLength( result1, length, &E );
            if (dataSetName[0] != 0) {
                namLen = IEC_Fast_EmitVisibleString( dataSetName, dataSet0, &E );
            }
            length = IEC_Fast_EmitTagAndLength( responsePositive2,
                                                (MMSd_length) (length + namLen),
                                                &E );
            length = (MMSd_length) ( length + IEC_Fast_EmitIntegerValue( (long) confRev,
                                     confRev1,
                                     &E ) );
        }
        namLen = IEC_Fast_EmitVisibleString( gocbRef, ident0, &E );
//        length = IEC_Fast_EmitTagAndLength( IEC_Goose_ResponseTag[idType],
//                                            (MMSd_length) (length + namLen),
//                                            &E );
        length = IEC_Fast_EmitTagAndLength( responses2, length, &E );
        break;

    case GOOSE_Unsupported_Res:
        length = IEC_Fast_EmitTagAndLength( gseMngtNotSupported0, 0, &E );
        break;
    case Unknown_Mgmt_Req:
    default:
        return( 0 );
    }
    length = (MMSd_length) ( length + IEC_Fast_EmitUnsignedValue( stateId, stateID0, &E ) );
    length = IEC_Fast_EmitTagAndLength( GSEMngtPDU0, length, &E );
    MMSd_takeDescriptor( E, packet->data_offset, packet->data_length );
    return( length );
}



IEC_Goose_Error IEC_Goose_GetReference( IEC_Goose_Context *cntxt,
                                        void              *handle,
                                        unsigned long     stateId,
                                        int               *pElements,
                                        int               numElements )
{
    MMSd_length             length;
    TMW_Packet              packet;
    MMSd_descriptor         E;
    IEC_Goose_Error         retVal;
    IEC_Goose_Subscription  *entry;
    TMW_LanHeader           vlanAddress;
    int                     element;

    if (cntxt->link_context == NULL)
        return( IEC_GOOSE_ERR_PARAMS );

#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_out pool that matches handle       *
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_in) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( IEC_GOOSE_ERR_PARAMS );
    }

    if (entry->status.srcAddress.len == 0) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( IEC_GOOSE_ERR_PARAMS );
    }

    if (!TMW_alloc_packet( &packet,
                           cntxt->link_context->buffers,
                           TMW_POOL_SEND )) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( IEC_GOOSE_ERR_NO_BUFFERS );
    }

    /* -------------------------------------------------------------------- *
     * Make a descriptor for the iecgoose and fill in reverse order!        *
     * -------------------------------------------------------------------- */
    MMSd_makeDescriptor( E, packet.buffer, (MMSd_ushort) packet.buffer_length,
                         (MMSd_ushort) ( packet.buffer_length - 1 ) );
    length = 0;
    /* -------------------------------------------------------------------- *
     * Encode the member indexes being requested                            *
     * -------------------------------------------------------------------- */
    element = numElements - 1;
    while (  element >= 0 ) {
        length = (MMSd_length) (length + IEC_Fast_EmitIntegerValue( (long) pElements[ element-- ],
                                INTEGER,
                                &E ) );
    }
    MMSd_takeDescriptor( E, packet.data_offset, packet.data_length );
    GSE_Encode_Request( GOOSE_References_Req,
                        entry->status.header.gocbRef,
                        NULL,
                        0,
                        stateId,
                        0,
                        length,
                        &packet );

    /* -------------------------------------------------------------------- *
     * Encode the GSE request header                                        *
     * -------------------------------------------------------------------- */
    vlanAddress.etherType = 0x88B9;
    vlanAddress.lsap = -1;
    vlanAddress.VLAN_Priority = entry->status.header.VLAN_Priority;
    vlanAddress.VLAN_VID = entry->status.header.VLAN_VID;
    vlanAddress.ApplID = entry->status.header.VLAN_AppID;
    if ( !TMW_encode_VLAN_header( &packet, &vlanAddress ) ) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( IEC_GOOSE_ERR_PARAMS );
    }

    /* -------------------------------------------------------------------- *
     * Send the GSE Request                                                 *
     * -------------------------------------------------------------------- */
    packet.media = TMW_VLAN;
    if (!TMW_DL_send( &packet, &entry->status.srcAddress, cntxt->link_context))
        retVal = IEC_GOOSE_ERR_LINK;
    else
        retVal = IEC_GOOSE_SUCCESS;
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( retVal );
}


IEC_Goose_Error IEC_Goose_GetElementNumber( IEC_Goose_Context *cntxt,
        void              *handle,
        unsigned long      stateId,
        char              **Elnamelist )
{
    MMSd_length             length;
    TMW_Packet              packet;
    MMSd_descriptor         E;
    IEC_Goose_Error         retVal;
    IEC_Goose_Subscription  *entry;
    TMW_LanHeader           vlanAddress;
    int                     element;

    if (cntxt->link_context == NULL)
        return( IEC_GOOSE_ERR_PARAMS );
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_TAKE( cntxt->sem );
#endif
    /* -------------------------------------------------------------------- *
     * Find Subscription entry in active_out pool that matches handle       *
     * -------------------------------------------------------------------- */
    entry = IEC_Goose_find_handle( handle, &(cntxt->active_in) );
    if (entry == NULL) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( IEC_GOOSE_ERR_PARAMS );
    }
    if (entry->status.srcAddress.len == 0) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( IEC_GOOSE_ERR_PARAMS );
    }
    if (!TMW_alloc_packet( &packet,
                           cntxt->link_context->buffers,
                           TMW_POOL_SEND )) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( IEC_GOOSE_ERR_NO_BUFFERS );
    }
    /* -------------------------------------------------------------------- *
     * Make a descriptor for the iecgoose and fill in reverse order!        *
     * -------------------------------------------------------------------- */
    MMSd_makeDescriptor( E, packet.buffer, (MMSd_ushort) packet.buffer_length,
                         (MMSd_ushort) ( packet.buffer_length - 1 ) );
    length = 0;

    /* -------------------------------------------------------------------- *
     * Encode the member indexes being requested                            *
     * -------------------------------------------------------------------- */
    element = 0;
    while ( Elnamelist[element] != NULL ) {
        element++;
    }
    element--;
    while (element >= 0) {
        length = (MMSd_length) ( length + IEC_Fast_EmitVisibleString( Elnamelist[element--],
                                 VisibleString26, &E ) );
    }

    MMSd_takeDescriptor( E, packet.data_offset, packet.data_length );
    GSE_Encode_Request( GOOSE_Elements_Req,
                        entry->status.header.gocbRef,
                        NULL,
                        0,
                        stateId,
                        0,
                        length,
                        &packet );

    vlanAddress.etherType = 0x88B9;
    vlanAddress.lsap = -1;
    vlanAddress.VLAN_Priority = entry->status.header.VLAN_Priority;
    vlanAddress.VLAN_VID = entry->status.header.VLAN_VID;
    vlanAddress.ApplID = entry->status.header.VLAN_AppID;
    if ( !TMW_encode_VLAN_header( &packet, &vlanAddress ) ) {
#if defined(TMW_USE_THREADS)
        TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
        return( IEC_GOOSE_ERR_PARAMS );
    }

    /* -------------------------------------------------------------------- *
     * Send the GSE Request                                                 *
     * -------------------------------------------------------------------- */
    packet.media = TMW_VLAN;
    if (!TMW_DL_send( &packet, &entry->status.srcAddress, cntxt->link_context))
        retVal = IEC_GOOSE_ERR_LINK;
    else
        retVal = IEC_GOOSE_SUCCESS;
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( cntxt->sem );
#endif
    return( retVal );
}

#endif //#ifdef IECGOOSE_C
