/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1997-2011 */
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

/* file: GooseApp.c
 * description: Simple Example Goose application
 *
 *  Starts up a Goose input stram and a Goose output streams.
 *  Each stream uses a dataset of some status and analog points.
 *
 *  If the test is in loopback mode, whatever is received on the input
 *  stream is re-published on the output stream.
 */

#define GOOSEAPP
#ifdef GOOSEAPP

#define GOOSEAPP_COMMENT_CODE

#include <stdio.h>
#include "sys.h" 
   
#include "tmwtarg.h"
#include "vendor.h"        
#include "ber.h"
#include "mms_erro.h"

#include "iecgoose.h"
#include "igotags.h"

#if !defined(_LINUX)
#include "eth1.h"        /* For ETH_adapter_dialog */
#endif

#if defined(_WINDOWS) || defined(_WIN32)
#include "ndis_if.h"                /* To enable loopback */
#endif

#include "gooseapp.h"


/*
 *  Goose Retransmission Strategy
 *
 *  This retransmission strategy will do the following:
 *
 *      1st retransmission at 10ms
 *      2nd retransmission at 20ms
 *      3rd retransmission 1t 100ms
 *
 *      The retransmission interval doubles after that,
 *      until it reaches an interval of 30 seconds, then
 *      continues at that 39 second interval.
 */

IEC_Goose_Strategy test_strategy = {
    IEC_GOOSE_Profile,   /* strategyType */
    {                       /* struct ari */
        2,                      /* multiplier */
        100,                    /* 1st interval */
        3000,                  /* max interval */
    },
    {                       /* struct pro */
        2,                      /* numProfiles */
        0,                      /* currentIndex */
        {10, 20}                 /* 1st 2 intervals (ms) */
    }
};


/*
 * This routine encodes status data, and returns encoded length.
 */
int EncodeMyStatus( MyINS *pData, int nCount, MMSd_descriptor *pOutput )
{
    int         i;
    MMSd_length nTotalLength, nLen;

    /* Encode from back to front */
    nTotalLength = 0;
    for ( i = 0; i < nCount; i++ ) {
        nLen = MMSd_emitUSER_UTCTIME( &pData[ nCount - i - 1 ].t, 8, pOutput );
        nLen = nLen + MMSd_emitUSER_BITSTRING( pData[ nCount - i - 1 ].q, 13, pOutput );
        nLen = nLen + MMSd_emitUSER_INTEGER( pData[ nCount - i - 1 ].stVal, pOutput );
        nTotalLength = MMSd_emitStructure( nLen, pOutput );
    }
    if ( MMSd_checkError( MMSd_anyError, pOutput ) )
        return( 0 );
    return( nTotalLength );
}

/*
 * Encodes analog data, returns encoded length.
 */
int EncodeMyAnalog( MyMV *pData, int nCount, MMSd_descriptor *pOutput )
{
    int         i;
    MMSd_length nTotalLength, nLen, nFloatLen;

    /* Encode from back to front */
    nTotalLength = 0;
    for ( i = 0; i < nCount; i++ ) {
        nLen = MMSd_emitUSER_UTCTIME( &pData[ nCount - i - 1 ].t, 8, pOutput );
        nLen = nLen + MMSd_emitUSER_BITSTRING( pData[ nCount - i - 1 ].q, 13, pOutput );
        nFloatLen = MMSd_emitUSER_FLOAT( pData[ nCount - i - 1 ].mag_f, pOutput );
        nLen = nLen + MMSd_emitStructure( nFloatLen, pOutput );
        nTotalLength = MMSd_emitStructure( nLen, pOutput );
    }
    if ( MMSd_checkError( MMSd_anyError, pOutput ) )
        return( 0 );
    return( nTotalLength );
}

/*
 * Encodes the entire dataset, consisting of status and analog values.
 */
int EncodeDataSet( MyGooseConfig    *pGooseParams,
                   unsigned char    *pBuffer,
                   int              nBufferSize,
                   int              *pCount,
                   int              *pOffset,
                   int              *pLength )
{
    MMSd_descriptor     D;
    int                 nOffset, nLength;

    MMSd_makeDescriptor( D, pBuffer, (MMSd_ushort) nBufferSize, (MMSd_ushort) (nBufferSize - 1) );

    nLength = EncodeMyAnalog( pGooseParams->pMvPoints, pGooseParams->nMvPoints, &D );
    if ( nLength <= 0 )
        return( FALSE );

    nLength = EncodeMyStatus( pGooseParams->pInsPoints, pGooseParams->nInsPoints, &D );
    if ( nLength <= 0 )
        return( FALSE );

    MMSd_takeDescriptor( D, nOffset, nLength );
    *pCount = pGooseParams->nInsPoints + pGooseParams->nMvPoints;
    *pOffset = nOffset;
    *pLength = nLength;
    return( TRUE );
}


/*
 * This routine decodes status points, and returns number decoded.
 */
int DecodeMyStatus( MMSd_descriptor *pInput, MyINS *pData, int nMaxData )
{
    MMSd_descriptor save;
    int             nCount;

    /* Decode from front to back */
    nCount = 0;
    save = *pInput;
    while ( !MMSd_fetchCOMPLETE( &save, pInput ) && ( nCount < nMaxData ) ) {
        if ( nCount >= nMaxData )
            return( 0 );

        MMSd_nextToken( pInput );
        MMSd_beginHandlerStruct( pInput );
        if ( !MMSd_fetchUSER_INTEGER( pInput, &pData[ nCount ].stVal ) )
            return( 0 ) ;
        MMSd_nextToken( pInput );
        if ( !MMSd_fetchUSER_BITSTRING( pInput, pData[ nCount ].q, 13 ) )
            return( 0 ) ;
//        MMSd_nextToken( pInput );
//        if ( !MMSd_fetchUSER_UTCTIME( pInput, &pData[ nCount ].t ) )
//            return( 0 ) ;
        MMSd_endHandlerStruct( pInput );
        nCount++;
    }
    if ( MMSd_checkError( MMSd_anyError, pInput ) )
        return( 0 );
    return( nCount );
}

/*
 * Decodes analog points, returns number decoded.
 */
int DecodeMyAnalog( MMSd_descriptor *pInput, MyMV *pData, int nMaxData )
{
    MMSd_descriptor save;
    int             nCount;

    /* Decode from front to back */
    nCount = 0;
    save = *pInput;
    while ( !MMSd_fetchCOMPLETE( &save, pInput ) && ( nCount < nMaxData ) ) {
        if ( nCount >= nMaxData )
            return( 0 );

        MMSd_nextToken( pInput );
        MMSd_beginHandlerStruct( pInput );

        MMSd_beginHandlerStruct( pInput );
        if ( !MMSd_fetchUSER_FLOAT( pInput, &pData[ nCount ].mag_f ) )
            return( 0 ) ;
        MMSd_endHandlerStruct( pInput );
        MMSd_nextToken( pInput );

        if ( !MMSd_fetchUSER_BITSTRING( pInput, pData[ nCount ].q, 13 ) )
            return( 0 ) ;
//        MMSd_nextToken( pInput );

//        if ( !MMSd_fetchUSER_UTCTIME( pInput, &pData[ nCount ].t ) )
//            return( 0 ) ;

        MMSd_endHandlerStruct( pInput );
        nCount++;
    }
    if ( MMSd_checkError( MMSd_anyError, pInput ) )
        return( 0 );
    return( nCount );
}

/*
 * Copies data from input to output for loopback test.
 */
void CopyGooseData( MyGooseConfig *pFromGooseParams, MyGooseConfig *pToGooseParams )
{
    MMSd_UtcBTime   now;
    int             i;

    MMSd_adjusted_utc_time_stamp( &now );
    for ( i = 0; i < pToGooseParams->nInsPoints; i++ ) {
        pToGooseParams->pInsPoints[ i ] = pFromGooseParams->pInsPoints[ i ];
        pToGooseParams->pInsPoints[ i ].t = now;
    }
    for ( i = 0; i < pToGooseParams->nMvPoints; i++ ) {
        pToGooseParams->pMvPoints[ i ] = pFromGooseParams->pMvPoints[ i ];
        pToGooseParams->pMvPoints[ i ].t = now;
    }
}

/*
 * Initializes the simple point database for output Goose.
 */
void InitializeGooseData( MyGooseConfig *pGooseParams )
{
    MMSd_UtcBTime   now;
    int             i;

    MMSd_adjusted_utc_time_stamp( &now );
    for ( i = 0; i < pGooseParams->nInsPoints; i++ ) {
        pGooseParams->pInsPoints[ i ].stVal = i;
        pGooseParams->pInsPoints[ i ].t = now;
        memset( pGooseParams->pInsPoints[ i ].q, 0, 2 );
    }
    for ( i = 0; i < pGooseParams->nMvPoints; i++ ) {
        pGooseParams->pMvPoints[ i ].mag_f = (float) (1.0 * i);
        pGooseParams->pMvPoints[ i ].t = now;
        memset( pGooseParams->pMvPoints[ i ].q, 0, 2 );
    }
}

/*
 * Increments all points in the database for Goose test.
 */
void IncrementGooseData( MyGooseConfig *pGooseParams )
{
    MMSd_UtcBTime   now;
    int             i;

    MMSd_adjusted_utc_time_stamp( &now );
    for ( i = 0; i < pGooseParams->nInsPoints; i++ ) {
        pGooseParams->pInsPoints[ i ].stVal = pGooseParams->pInsPoints[ i ].stVal + 1;
        pGooseParams->pInsPoints[ i ].t = now;
    }
    for ( i = 0; i < pGooseParams->nMvPoints; i++ ) {
        pGooseParams->pMvPoints[ i ].mag_f = (float) (1.2345);//(pGooseParams->pMvPoints[ i ].mag_f + 1.0);
        pGooseParams->pMvPoints[ i ].t = now;
    }
}

#ifndef GOOSEAPP_COMMENT_CODE
/*
 * Displays the current value of the points database.
 */
void DisplayGooseData( FILE *fd, MyGooseConfig *pGooseParams )
{
    char outStr[ 1024 ];
    int i;
    unsigned int quality;

    for ( i = 0; i < pGooseParams->nInsPoints; i++ ) {
        MMSd_DisplayUtcString( &pGooseParams->pInsPoints[ i ].t, outStr, sizeof( outStr ) );
        quality = (pGooseParams->pInsPoints[ i ].q[ 0 ] << 8)
                  | pGooseParams->pInsPoints[ i ].q[ 1 ];
        fprintf( fd, "INS: %d %s q:0x%x\n", pGooseParams->pInsPoints[ i ].stVal, outStr, quality );
    }
    for ( i = 0; i < pGooseParams->nMvPoints; i++ ) {
        MMSd_DisplayUtcString( &pGooseParams->pMvPoints[ i ].t, outStr, sizeof( outStr ) );
        quality = (pGooseParams->pMvPoints[ i ].q[ 0 ] << 8)
                  | pGooseParams->pMvPoints[ i ].q[ 1 ];
        fprintf( fd, "MV: %f %s q:0x%x\n", pGooseParams->pMvPoints[ i ].mag_f, outStr, quality );
    }
}

/*
 * Displays entire Goose input event, including
 * header information and points data.
 */
void DisplayGooseInput( FILE             *fd,
                        void             *pHandle,
                        IEC_GooseHeader  *pHeader,
                        int              timed_out,
                        MyGooseConfig    *pGooseParams )
{
    IEC_Goose_Subscription  *pSubscription;
    MMSd_UtcBTime           now;
    char                    outStr[ 1024 ];

    MMSd_adjusted_utc_time_stamp( &now );

    MMSd_DisplayUtcString( &now, outStr, sizeof( outStr ) );
    fprintf(fd, "Goose indication at %s",  outStr );

    pSubscription = IEC_Goose_find_handle( pHandle, &pGooseParams->pIecGooseContext->active_in );
    if ( pSubscription == NULL ) {
        fprintf( fd, "(Subscription not found?)" );
    } else {
        outStr[0] = 0;
        TMW_addr2hex( (TMW_Address *) &pSubscription->status.srcAddress, outStr, sizeof( outStr ) );
        fprintf( fd, " From: %s", outStr );
        outStr[0] = 0;
        TMW_addr2hex( (TMW_Address *) &pSubscription->status.address, outStr, sizeof( outStr ) );
        fprintf( fd, " To: %s", outStr );
    }
    if ( timed_out )
        fprintf(fd, " (TIMEOUT)");
    fprintf( fd, "\n" );

    /* v7.26 - Show VLAN information */
    fprintf(fd, "  APPID: %u, VID: %u, Priority: %u\n",
            pHeader->VLAN_AppID,
            pHeader->VLAN_VID,
            pHeader->VLAN_Priority );

    fprintf(fd, "  goID: %s", pHeader->goID);
    fprintf(fd, "  gocbRef: %s", pHeader->gocbRef);
    fprintf(fd, "  datSet: %s\n", pHeader->datSet);

    fprintf(fd, "  TimeAllowedToLive: %lu\n", pHeader->HoldTim);
    MMSd_DisplayUtcString( &pHeader->t, outStr, sizeof( outStr ) );
    fprintf(fd, "  t: %s (0x%x)", outStr, pHeader->t.quality);
    fprintf(fd, "  StNum: %lu", pHeader->StNum);
    fprintf(fd, "  SqNum: %lu\n", pHeader->SqNum);

    fprintf(fd, "  confRev: %ld", pHeader->confRev);
    fprintf(fd, "  test: %d", pHeader->test);
    fprintf(fd, "  ndsCom: %d", pHeader->ndsCom);
    fprintf(fd, "  numDataSetEntries: %d\n", pHeader->numDataSetEntries);

    fprintf(fd, "  Data:\n");
    DisplayGooseData( stdout, pGooseParams );
}
#endif

/*
 * This indication routine is the input data indication callback function.
 *  It is passed to IEC_Goose_Subscribe, and is invoked by the library
 *  when either a timeout occurs on the stream, or when a data change
 *  has been published by the sender.
 */
void iec_goose_indication( void             *pHandle,
                           void             *param,
                           IEC_GooseHeader  *pHeader,
                           unsigned char    *pData,
                           MMSd_length      nDataLen,
                           int              timed_out)
{
    MyGooseConfig     *pGooseParams = (MyGooseConfig *) param;
    MMSd_descriptor   descriptor;
    int               nStatus, nAnalog;
    int               ElementIndexList[ IEC_GOOSE_MAX_ELEMENTS ];
    char              *ElementNameList[ IEC_GOOSE_MAX_ELEMENTS ];
    int               i;

    if ( timed_out ) {
        printf("Goose input %s timed out\n", pGooseParams->datSetName);
        return;
    } else {
        MMSd_makeDescriptor( descriptor, pData, (unsigned short) nDataLen, 0 );
        descriptor.tokenLength = nDataLen;
        nStatus = DecodeMyStatus( &descriptor, pGooseParams->pInsPoints, pGooseParams->nInsPoints );
#ifndef GOOSEAPP_COMMENT_CODE
        nAnalog = DecodeMyAnalog( &descriptor, pGooseParams->pMvPoints, pGooseParams->nMvPoints );
#endif
        if ( nStatus != MAX_STATUS_POINTS ) {
            int i;
            printf("\n%p %d: ", pData, nDataLen );
            for ( i = 0; i < nDataLen; i++)
                printf(" %02x", pData[i]);
            printf("\n");
            printf( "Decode of goose status data failed: %d\n", nStatus );
        }
#ifndef GOOSEAPP_COMMENT_CODE
        if ( nAnalog != MAX_ANALOG_POINTS ) {
            int i;
            printf("\n%p %d: ", pData, nDataLen );
            for ( i = 0; i < nDataLen; i++)
                printf(" %02x", pData[i]);
            printf("\n");
            printf( "Decode of goose analog data failed: %d\n", nAnalog );
        }
#endif        
    }

    switch ( pGooseParams->nTestMode ) {
    case Test_Display:
#ifndef GOOSEAPP_COMMENT_CODE
        DisplayGooseInput( stdout, pHandle, pHeader, timed_out, pGooseParams );
#endif
        break;
    case Test_Loopback:
        if ( pGooseParams->pOther == NULL )
            break;
        CopyGooseData( pGooseParams, pGooseParams->pOther );
        if ( !SendGooseData( pGooseParams->pIecGooseContext, pGooseParams->pOther ) )
            printf( "SendGooseData failed for %s\n", pGooseParams->pOther->goID );
        break;
    case Test_LoopbackIncrement:
        if ( pGooseParams->pOther == NULL )
            break;
        CopyGooseData( pGooseParams, pGooseParams->pOther );
        IncrementGooseData( pGooseParams->pOther );
        if ( !SendGooseData( pGooseParams->pIecGooseContext, pGooseParams->pOther ) )
            printf( "SendGooseData failed for %s\n", pGooseParams->pOther->goID );
        break;
    default:
        break;
    }

    /* This code shows an example of how to use the query functions
       to determine if the publisher's configuration matches out (subscriber's)
       view of the dataset.  If this is the first message we have received from
       this stream, we do a query based on dataset indices.

       On the second goose message (first data change), we will do the other
       query (based on name).
    */
#ifndef GOOSEAPP_COMMENT_CODE
    if ( pGooseParams->nQueryDone == 0 ) {
        int i;

        pGooseParams->nQueryDone = 1;
        for ( i = 0; i < pGooseParams->nMemberNames; i++ )
            ElementIndexList[ i ] = i;

        if ( IEC_Goose_GetReference( pGooseParams->pIecGooseContext,
                                     pHandle,
                                     pHeader->StNum,
                                     ElementIndexList,
                                     pGooseParams->nMemberNames ) != IEC_GOOSE_SUCCESS ) {
            printf( "GetReferences failed\n" );
        }
    } else {
        if ( pGooseParams->nQueryDone == 1 ) {
#if defined(HARD_TEST)
            /* This causes queries after every goose - lots of ethernet traffic!! */
            pGooseParams->nQueryDone = 0;
#else
            pGooseParams->nQueryDone = 2;
#endif

            for ( i = 0; i < pGooseParams->nMemberNames; i++ )
                ElementNameList[ i ] = pGooseParams->memberNames[ i ];
            ElementNameList[ i ] = NULL;

            if ( IEC_Goose_GetElementNumber( pGooseParams->pIecGooseContext,
                                             pHandle,
                                             pHeader->StNum,
                                             ElementNameList ) != IEC_GOOSE_SUCCESS ) {
                printf( "GetElementNumber failed\n" );
            }
        }
    }
#endif // GOOSEAPP_COMMENT_CODE
}

/*
 *  The following strings are used in:
 *      iec_goose_GetReferenceRes_indication
 *      iec_goose_GetElementRes_indication
 *  for displaying error codes.
 */

static char *iec_goose_gblError[] = {
    "other",
    "unknownControlBlock",
    "responseTooLarge",
    "controlBlockConfigurationError"
};

static char *iec_goose_lclErrors[] = {
    "other",
    "notFound",
};


/*
 * This indication callback routine is called when a response to
 * a previous GetElement query is received. It simply prints the data.
 * In a reral system, it would be used to validate the local (i.e.
 * subscriber) view of the data against the remote publisher's.
 *
 * A function pointer to this routine is set up in the Goose
 * context, along with a void * parameter, which in this case
 * points to the Goose stream configuration data.
 *
 */
void iec_goose_GetElementRes_indication( void             *param,
        unsigned char    *dataPtr,
        MMSd_length      dataLen,
        unsigned char    *securityPtr,
        MMSd_length      securityLen,
        unsigned long    stateId,
        char             *gocbRef,
        char             *dataSetName,
        unsigned long    confRev,
        int              errorCode )
{
    MMSd_descriptor save, D;

    printf("GetElementResponse %lu:", stateId );
    if ( gocbRef != NULL ) {
        printf( " gocbRef=%s\n", gocbRef );
    }

    if (dataPtr == NULL) {
        printf(" Response negative: error=%s\n", iec_goose_gblError[errorCode] );
        return;
    }

    if ( dataSetName != NULL ) {
        printf( " dataSetName=%s", dataSetName );
    }
    printf( " confRev=%ld", confRev );
    if ( errorCode >= 0 )
        printf( " gblError=%s[%d]", iec_goose_gblError[errorCode],
                errorCode );
    printf("\n");

    MMSd_makeDescriptor(D, dataPtr, (unsigned short) dataLen, 0);
    D.tokenLength = dataLen;
    save = D;
    while (!MMSd_fetchCOMPLETE( &save, &D )) {
        char lclName[ 512 ];
        int lclError;
        int lclOffset;

        MMSd_fetchTAG_and_LENGTH( &D );
        switch ( D.tag ) {
        case offset0:
            if (!MMSd_fetchINTEGER( &D, &lclOffset, offset0 )) {
                printf("Encoding error in offset\n");
                return;
            }
            printf("  Offset:%d\n", lclOffset );
            break;
        case reference1:
            if (!MMSd_fetchSTRING( &D, lclName, sizeof( lclName ), reference1 )) {
                printf("Encoding error in name\n");
                return;
            }
            printf("  Reference:%s\n", lclName );
            break;
        case error2:
            if (!MMSd_fetchINTEGER( &D, &lclError, error2 )) {
                printf("Encoding error in response\n");
                return;
            }
            printf("  Error:%s[%d]\n", iec_goose_lclErrors[lclError], lclError );
            break;
        default:
            printf("Encoding error in response tag\n");
            break;
        }
    }
}

/*
 * This indication callback routine is called when a response to
 * a previous GetReference query is received. It simply prints the data.
 * In a reral system, it would be used to validate the local (i.e.
 * subscriber) view of the data against the remote publisher's.
 *
 * A function pointer to this routine is set up in the Goose
 * context, along with a void * parameter, which in this case
 * points to the Goose stream configuration data.
 *
 */
void iec_goose_GetReferenceRes_indication

( void             *param,
        unsigned char    *dataPtr,
        MMSd_length      dataLen,
        unsigned char    *securityPtr,
        MMSd_length      securityLen,
        unsigned long    stateId,
        char             *gocbRef,
        char             *dataSetName,
        unsigned long    confRev,
        int              errorCode )
{
    MMSd_descriptor save, D;

    printf("GetReferenceResponse %lu:", stateId );
    if ( gocbRef != NULL ) {
        printf( " gocbRef=%s\n", gocbRef );
    }

    if (dataPtr == NULL) {
        printf(" Response negative: error=%s\n", iec_goose_gblError[errorCode] );
        return;
    }

    if ( dataSetName != NULL ) {
        printf( " dataSetName=%s", dataSetName );
    }
    printf( " confRev=%ld", confRev );
    if ( errorCode >= 0 )
        printf( " gblError=%s[%d]", iec_goose_gblError[errorCode],
                errorCode );
    printf("\n");

    MMSd_makeDescriptor(D, dataPtr, (unsigned short) dataLen, 0);
    D.tokenLength = dataLen;
    save = D;
    while (!MMSd_fetchCOMPLETE( &save, &D )) {
        char lclName[ 512 ];
        int lclError;
        int lclOffset;

        MMSd_fetchTAG_and_LENGTH( &D );
        switch ( D.tag ) {
        case offset0:
            if (!MMSd_fetchINTEGER( &D, &lclOffset, offset0 )) {
                printf("Encoding error in offset\n");
                return;
            }
            printf("  Offset:%d\n", lclOffset );
            break;
        case reference1:
            if (!MMSd_fetchSTRING( &D, lclName, sizeof( lclName ), reference1 )) {
                printf("Encoding error in name\n");
                return;
            }
            printf("  Reference:%s\n", lclName );
            break;
        case error2:
            if (!MMSd_fetchINTEGER( &D, &lclError, error2 )) {
                printf("Encoding error in response\n");
                return;
            }
            printf("  Error:%s[%d]\n", iec_goose_lclErrors[lclError], lclError );
            break;
        default:
            printf("Encoding error in response tag\n");
            break;
        }
    }
}


/*
 * This indication callback routine is called when a response to
 * a previous GetElement or GetReference query is received, and
 * Goose publisher responds that it does not support the service.
 *
 * A function pointer to this routine is set up in the Goose
 * context, along with a void * parameter, which in this case
 * points to the Goose stream configuration data.
 *
 */
void iec_goose_NotSupportedRes_indication( void             *param,
        unsigned long    stateId,
        unsigned char    *securityPtr,
        MMSd_length      securityLen )
{
    printf("NotSupportedResponse %lu\n", stateId );
}

/*
 * This indication callback function is called to respond to queries about
 * the dataset members made by subscribers. It receives a list of member
 * indices and encodes the corresponding list of names, which is passed
 * back to the stack for transmission.
 *
 * A function pointer to this routine is set up in the Goose
 * context, along with a void * parameter, which in this case
 * points to the Goose stream configuration data.
 *
 */
int iec_goose_GetReferenceReq_indication( IEC_Goose_Context    *gseCntxt,
        void                 *mmsCntxt,
        void                 *param,
        char                 *gocbRef,
        unsigned char        *pDataPtr,
        MMSd_length          nDataLen,
        unsigned char        *pSecurityPtr,
        MMSd_length          nSecurityLen,
        char                 *dataSetName,
        TMW_Packet           *packet,
        long                 *pConfRev,
        int                  *pGblError )
{
    MyGooseConfig       *pGooseParams = (MyGooseConfig *) param;
    int                 elementNumber;
    int                 elementList[ IEC_GOOSE_MAX_ELEMENTS ];
    int                 nElements;
    MMSd_descriptor     Data, Save, Emit;
    MMSd_length         length, subLen;

    if ( pGooseParams == NULL ) {
        *pGblError = IEC_GblError_controlBlockConfigurationError;
        return( 0 );
    }

    if ( strcmp( gocbRef, pGooseParams->gocbRef ) != 0 ) {
        *pGblError = IEC_GblError_unknownControlBlock;
        return( 0 );
    }

    /* Pass back current dataset name and confRev */
    strcpy( dataSetName, pGooseParams->datSetName );
    *pConfRev = pGooseParams->confRev;

    /* Descriptor used for fetching from Goose query request */
    MMSd_makeDescriptor( Data, pDataPtr, nDataLen, 0 );
    Data.tokenLength = Data.bufLen;
    Save = Data;

    /* Descriptor used for emitting results into same packet */
    MMSd_makeDescriptor( Emit, packet->buffer,
                         (MMSd_ushort) packet->buffer_length,
                         (MMSd_ushort) (packet->buffer_length - 1));

    /* Fetch indices being queried */
    nElements = 0;
    while (!MMSd_fetchCOMPLETE( &Save, &Data )) {
        MMSd_fetchTAG_and_LENGTH( &Data );
        if (!MMSd_fetchINTEGER( &Data, &elementNumber, INTEGER2 ) ) {
            MMSd_emptyDescriptor( Emit, Emit );
            *pGblError = IEC_GblError_other;
            return( 0 );
        }
        elementList[ nElements++ ] = elementNumber;
    }
    nElements--;

    /*
     * Find the name for each index and encode the results.
     * Note the reverse order (back-to-front) for encoding ASN.1.
     */
    length = 0;
    while ( nElements >= 0 ) {
        elementNumber = elementList[ nElements-- ];
        if ( ( elementNumber < 0 )
                || ( elementNumber >=
                     ( pGooseParams->nInsPoints + pGooseParams->nMvPoints ) ) ) {
            subLen = MMSd_emitTAGGEDVALUE( error2,
                                           (long) IEC_ErrorReason_notFound,
                                           &Emit );
        } else {
            subLen = MMSd_emitSTRING( pGooseParams->memberNames[ elementNumber ],
                                      &Emit );
            subLen = MMSd_emitTAG_and_LENGTH( reference1, subLen, &Emit );
        }
        length = length + subLen;
    }

    MMSd_takeDescriptor( Emit, packet->data_offset, packet->data_length );
    return( length );
}

/*
 * This indication callback function is called to respond to queries about
 * the dataset members made by subscribers. It receives a list of member
 * indices and encodes the corresponding list of names, which is passed
 * back to the stack for transmission.
 *
 * A function pointer to this routine is set up in the Goose
 * context, along with a void * parameter, which in this case
 * points to the Goose stream configuration data.
 *
 */
int iec_goose_GetElementReq_indication( IEC_Goose_Context    *gseCntxt,
                                        void                 *mmsCntxt,
                                        void                 *param,
                                        char                 *gocbRef,
                                        unsigned char        *pDataPtr,
                                        MMSd_length          nDataLen,
                                        unsigned char        *pSecurityPtr,
                                        MMSd_length          nSecurityLen,
                                        char                 *dataSetName,
                                        TMW_Packet           *packet,
                                        long                 *pConfRev,
                                        int                  *pGblError )
{
    MyGooseConfig       *pGooseParams = (MyGooseConfig *) param;
    MMSd_descriptor     Data, Save, Emit;
    MMSd_length         length, subLen;
    char                elementName[ IEC_GOOSE_IDENT_SIZE ];
    int                 elementList[ IEC_GOOSE_MAX_ELEMENTS ];
    int                 i, nElements;

    if ( pGooseParams == NULL ) {
        *pGblError = IEC_GblError_controlBlockConfigurationError;
        return( 0 );
    }

    if ( strcmp( gocbRef, pGooseParams->gocbRef ) != 0 ) {
        *pGblError = IEC_GblError_unknownControlBlock;
        return( 0 );
    }

    /* Pass back current dataset name and confRev */
    strcpy( dataSetName, pGooseParams->datSetName );
    *pConfRev = pGooseParams->confRev;

    /* Descriptor used for fetching from Goose query request */
    MMSd_makeDescriptor( Data, pDataPtr, nDataLen, 0 );
    Data.tokenLength = Data.bufLen;
    Save = Data;

    /* Descriptor used for emitting results into same packet */
    MMSd_makeDescriptor( Emit, packet->buffer,
                         (MMSd_ushort) packet->buffer_length,
                         (MMSd_ushort) (packet->buffer_length - 1) );

    /* Fetch each name, and search for it in our list of member names. */
    nElements = 0;
    while (!MMSd_fetchCOMPLETE( &Save, &Data )) {
        MMSd_fetchTAG_and_LENGTH( &Data );
        if ( !MMSd_fetchSTRING( &Data, elementName, sizeof( elementName ), VisibleString26 ) ) {
            MMSd_emptyDescriptor( Emit, Emit );
            *pGblError = IEC_GblError_other;
            return( 0 );
        }
        for ( i = 0; i < pGooseParams->nMemberNames; i++ ) {
            if ( strcmp( elementName, pGooseParams->memberNames[ i ] ) == 0 )
                break;
        }
        if ( i < pGooseParams->nMemberNames )
            elementList[ nElements ] = i;
        else
            elementList[ nElements ] = -1;

        nElements++;
    }

    /*
     * Encode the indexes.  Note he reverse order for encoding, building the
     * response back-to-front.
     */
    nElements--;
    length = 0;
    while ( nElements >= 0 ) {
        if ( elementList[ nElements] < 0 ) {
            subLen = MMSd_emitTAGGEDVALUE( error2,
                                           (long) IEC_ErrorReason_notFound,
                                           &Emit );
        } else {
            subLen = MMSd_emitTAGGEDVALUE( offset0,
                                           (long) elementList[ nElements ],
                                           &Emit );
        }
        length = length + subLen;
        nElements--;
    }
    MMSd_takeDescriptor( Emit, packet->data_offset,
                         packet->data_length );
    return( length );
}

/*
 * Initializes the context and resolve the adapter name.
 */
int OpenGooseContext( IEC_Goose_Context *pIecGooseContext )
{
    
  if (pIecGooseContext->adapter_name[0] == 0) {
        ETH_adapter_dialog( "GOOSE adapter is not specified, use -l option",
                            pIecGooseContext->adapter_name );
    }

    IEC_Goose_Initialize( pIecGooseContext );

    pIecGooseContext->getElementNumResFcn = NULL;
    pIecGooseContext->getElementNumResParam =  NULL;
    pIecGooseContext->getReferenceResFcn = NULL;
    pIecGooseContext->getReferenceResParam = NULL;
    pIecGooseContext->notSupportedResFcn = NULL;
    pIecGooseContext->notSupportedResParam = NULL;
    pIecGooseContext->getElementNumReqFcn = NULL;
    pIecGooseContext->getElementNumReqParam = NULL;
    pIecGooseContext->getReferenceReqFcn = NULL;
    pIecGooseContext->getReferenceReqParam = NULL;



    return( TRUE );
}

/*
 * Sets up a goose input stream, including query callback
 * routines for resolving the dataSet names.
 */
int StartGooseInput( IEC_Goose_Context *pIecGooseContext,
                     MyGooseConfig     *pGooseParams )
{
    /* Set up handler functions for responses to queries */
    /* Many systems do not use the query functions - they are
       optional, but included here as examples */
    pIecGooseContext->getElementNumResFcn =
        iec_goose_GetElementRes_indication;
    pIecGooseContext->getElementNumResParam = pGooseParams;

    pIecGooseContext->getReferenceResFcn =
        iec_goose_GetReferenceRes_indication;
    pIecGooseContext->getReferenceResParam =pGooseParams;

    pIecGooseContext->notSupportedResFcn =
        iec_goose_NotSupportedRes_indication;
    pIecGooseContext->notSupportedResParam = NULL;

    if ( !IEC_Goose_Subscribe( pIecGooseContext,
                               pGooseParams->goID,
                               pGooseParams->gocbRef,
                               &pGooseParams->multicastAddr,
                               pGooseParams->vlan_AppID,
                               (void *) pGooseParams,
                               iec_goose_indication,
                               (void *) pGooseParams ) ) {
        printf( "Subscription for %s failed\n", pGooseParams->goID );
        return( FALSE );
    }

    /* This call sets up the time to wait for an initial Goose
     * from the publisher before signalling a timeout.
     */
    IEC_Goose_SetInitialWait( pIecGooseContext,
                              (void *) pGooseParams,
                              INITIAL_GOOSE_WAIT );
    return( TRUE );
}

/*
 * Sets up a goose output stream, including query callback
 * routines for resolving the dataSet names.
 */
int StartGooseOutput( IEC_Goose_Context *pIecGooseContext,
                      MyGooseConfig     *pGooseParams )
{
    unsigned char   buffer[ IEC_GOOSE_MAX_DATA_LEN ];
    int             nMembers, nOffset, nLength;

    /* Set up handler functions for responding to queries */
    /* Many systems do not use the query functions - they are
       optional, but included here as examples */
    pIecGooseContext->getElementNumReqFcn =
        iec_goose_GetElementReq_indication;
    pIecGooseContext->getElementNumReqParam = pGooseParams;

    pIecGooseContext->getReferenceReqFcn =
        iec_goose_GetReferenceReq_indication;
    pIecGooseContext->getReferenceReqParam = pGooseParams;

    /* Encode data for initial values */
    if ( !EncodeDataSet( pGooseParams, buffer, sizeof( buffer ),
                         &nMembers, &nOffset, &nLength ) ) {
        printf( "Encode of dataset %s failed!\n", pGooseParams->datSetName );
        return( FALSE );
    }

    /*
     * Start Goose output stream with initial data.
     * This will cause the first transmission of data on this stream,
     * and the stack will handle the retransmissions on its own.
     */
    if ( IEC_Goose_CreateOut( pIecGooseContext,
                              pGooseParams->goID,
                              pGooseParams->gocbRef,
                              pGooseParams->datSetName,
                              &pGooseParams->multicastAddr,
                              pGooseParams->pStrategy,
                              pGooseParams->confRev,
                              pGooseParams->ndsCommissioning,
                              pGooseParams->test,
                              nMembers,
                              FALSE,        /* do NOT use fixed length encoding */
                              &buffer[ nOffset ],
                              (MMSd_length) nLength,
                              (void *) pGooseParams,
                              pGooseParams->vlan_AppID,
                              pGooseParams->vlan_VID,
                              pGooseParams->vlan_Priority ) != IEC_GOOSE_SUCCESS ) {
        printf( "Create for %s failed\n", pGooseParams->goID );
        return( FALSE );
    }
    return(TRUE);
}

/* This routine would be called by the application whenever
 * the data has been updated, and new values must be published.
 */
int SendGooseData( IEC_Goose_Context *pIecGooseContext,
                   MyGooseConfig     *pGooseParams )
{
    IEC_GooseHeader     header;
    unsigned char       buffer[ IEC_GOOSE_MAX_DATA_LEN ];
    int                 nMembers, nOffset, nLength;

    if ( !EncodeDataSet( pGooseParams, buffer, sizeof( buffer ), &nMembers, &nOffset, &nLength ) ) {
        printf( "Encode of dataset %s failed!\n", pGooseParams->datSetName );
        return( FALSE );
    }

    if ( !IEC_Goose_Publish( pIecGooseContext,
                             pGooseParams,
                             &header,           /* In case you need to see sqNum, etc. */
                             nMembers,
                             &buffer[ nOffset ],
                             nLength ) ) {
        printf("Sending goose %s failed\n", pGooseParams->goID );
        return( FALSE );
    }
    return( TRUE );
}


/* ***************************************** */
/*
 * Test data used for the sample test application
 */
/* ***************************************** */
MyINS   myInputStatusData[ MAX_STATUS_POINTS ];
MyMV    myInputAnalogData[ MAX_ANALOG_POINTS ];

MyINS   myOutputStatusData[ MAX_STATUS_POINTS ];
MyMV    myOutputAnalogData[ MAX_ANALOG_POINTS ];

char    *myInputNames[] = {
    "IED1_LDEV/LLN0.Do1",
    "IED1_LDEV/LLN0.Do2",
    "IED1_LDEV/LLN0.Do3",
    "IED1_LDEV/LLN0.Do4",
    "IED1_LDEV/TestMMXU0.Do1",
    "IED1_LDEV/TestMMXU0.Do2",
    "IED1_LDEV/TestMMXU0.Do3",
    "IED1_LDEV/TestMMXU0.Do4"
};

char    *myOutputNames[] = {
    "IED2_LDEV/LLN0.Do1",
    "IED2_LDEV/LLN0.Do2",
    "IED2_LDEV/LLN0.Do3",
    "IED2_LDEV/LLN0.Do4",
    "IED2_LDEV/TestMMXU0.Do1",
    "IED2_LDEV/TestMMXU0.Do2",
    "IED2_LDEV/TestMMXU0.Do3",
    "IED2_LDEV/TestMMXU0.Do4"
};

MyGooseConfig inputGooseParams = {
    "TestGoose1",                                   /* goID[IEC_GOOSE_IDENT_SIZE]       */
    "IEDname/LLN0.gocb1",                           /* gocbRef[IEC_GOOSE_IDENT_SIZE]    */
    "IEDname/Goose1",                                /* datSetName[IEC_GOOSE_IDENT_SIZE] */
    { 6, { 0x01, 0x0C, 0xCD, 0xBE, 0xEF, 0x02 } },  /* mulitcastAddr                    */
    NULL,                                           /* pStrategy                        */
    0,                                              /* confRev                          */
    0,                                              /* ndsCommissioning                 */
    0,                                              /* test                             */
    0,                                              /* vlan_AppID                       */
    0,                                              /* vlan_VID                         */
    4,                                              /* vlan_Priority                    */
    MAX_STATUS_POINTS,                              /* nInsPoints                       */
    myInputStatusData,                              /* pInsPoints                       */
    MAX_ANALOG_POINTS,                              /* nMvPoints                        */
    myInputAnalogData,                              /* pMvPoints                        */
    Test_Display,                                   /* nTestMode                        */
    NULL,                                           /* pOther                           */
    NULL,                                           /* pIecGooseContext                 */
    myInputNames,                                   /* memberNames                      */
    sizeof(myInputNames)/sizeof(char *),            /* nMemberNames                     */
    0                                               /* nQueryDone                       */
};

MyGooseConfig outputGooseParams = {
    "TestGoose2",                                   /* goID[IEC_GOOSE_IDENT_SIZE]       */
    "IEDname/LLN0.gocb2",                           /* gocbRef[IEC_GOOSE_IDENT_SIZE]    */
    "IEDname/Goose2",                                /* datSetName[IEC_GOOSE_IDENT_SIZE] */
    { 6, { 0x01, 0x0C, 0xCD, 0xBE, 0xEF, 0x02 } },  /* mulitcastAddr                    */
    NULL,                                           /* pStrategy                        */
    0,                                              /* confRev                          */
    0,                                              /* ndsCommissioning                 */
    0,                                              /* test                             */
    0,                                              /* vlan_AppID                       */
    0,                                              /* vlan_VID                         */
    4,                                              /* vlan_Priority                    */
    MAX_STATUS_POINTS,                              /* nInsPoints                       */
    myOutputStatusData,                             /* pInsPoints                       */
    MAX_ANALOG_POINTS,                              /* nMvPoints                        */
    myOutputAnalogData,                             /* pMvPoints                        */
    Test_Display,                                   /* nTestMode                        */
    NULL,                                           /* pOther                           */
    NULL,                                           /* pIecGooseContext                 */
    myOutputNames,                                  /* memberNames                      */
    (sizeof(myOutputNames)/sizeof(char *)),         /* nMemberNames                     */
    0                                               /* nQueryDone                       */
};

/*
 * Starts up the test consisting of a publisher and subscriber.
 */
void *StartGooseTest( TestMode nTestMode, int bSwitch, int bEtherLoop, char *pInAddr, char *pOutAddr )
{
    //char goose_adapter_name[  ] = "eth0";
 
    MyTestContext       *pMyTestContext;
    IEC_Goose_Context   *pIecGooseContext;

    pMyTestContext = (MyTestContext *) TMW_TARG_MALLOC( sizeof( MyTestContext ) );
    if ( pMyTestContext == NULL ) {
        printf("Unable to malloc Test context\n" );
        return( NULL );
    }
    memset( pMyTestContext, 0, sizeof( MyTestContext ) );

    pIecGooseContext = (IEC_Goose_Context *) TMW_TARG_MALLOC( sizeof( IEC_Goose_Context ) );
    
    if ( pIecGooseContext == NULL ) {
        TMW_TARG_FREE( pMyTestContext );
        printf("Unable to malloc Goose context\n" );
        return( NULL );
    }
    pMyTestContext->pIecGooseContext = pIecGooseContext;
    
       
    memset( pMyTestContext->pIecGooseContext, 0, sizeof( IEC_Goose_Context ) );
    
    //strcpy( pIecGooseContext->adapter_name, "eth0" );
    
    if ( !OpenGooseContext( pIecGooseContext ) ) {
        TMW_TARG_FREE( pMyTestContext );
        TMW_TARG_FREE( pIecGooseContext );
        printf( "Goose context failed to open!\n" );
        return( NULL );
    }

#if defined(_WINDOWS) || defined(_WIN32)
    if ( bEtherLoop )
        NDIS_EnableLoopback( pMyTestContext->pIecGooseContext->link_context );
#endif

    InitializeGooseData( &inputGooseParams );
    inputGooseParams.nTestMode = nTestMode;
    inputGooseParams.pOther = &outputGooseParams;
    inputGooseParams.pIecGooseContext = pIecGooseContext;

    if ( pInAddr != NULL )
        TMW_parse_mac_address( (unsigned char *) pInAddr, &inputGooseParams.multicastAddr );

    InitializeGooseData( &outputGooseParams );
    outputGooseParams.nTestMode = nTestMode;
    outputGooseParams.pOther = &inputGooseParams;

    if ( pOutAddr != NULL )
        TMW_parse_mac_address( (unsigned char *) pOutAddr, &outputGooseParams.multicastAddr );
    outputGooseParams.pIecGooseContext = pIecGooseContext;


    if ( !bSwitch ) {
        pMyTestContext->pInputGoose = &inputGooseParams;
        pMyTestContext->pOutputGoose = &outputGooseParams;
        pMyTestContext->nTestMode = nTestMode;
    } else {
        pMyTestContext->pInputGoose = &outputGooseParams;
        pMyTestContext->pOutputGoose = &inputGooseParams;
        pMyTestContext->nTestMode = nTestMode;
    } 

    pMyTestContext->pOutputGoose->pStrategy = &test_strategy; 

    return( (void *) pMyTestContext );
}

/*
 * Terminates the test.
 */
void StopGooseTest( void *pTestHandle )
{
    MyTestContext *pMyTestContext = (MyTestContext *) pTestHandle;
    IEC_Goose_Context *pIecGooseContext = pMyTestContext->pIecGooseContext;

    if ( pMyTestContext->pInputGoose != NULL ) {
        if ( !IEC_Goose_Unsubscribe( pIecGooseContext, pMyTestContext->pInputGoose ) )
            printf(" Unsubscribe of Goose input failed!\n" );
    }
    if ( pMyTestContext->pOutputGoose != NULL ) {
        if ( !IEC_Goose_Delete( pIecGooseContext, pMyTestContext->pOutputGoose ) )
            printf(" Delete of Goose output failed!\n" );
    }
    IEC_Goose_Close( pIecGooseContext );
    TMW_TARG_FREE( pIecGooseContext );
    TMW_TARG_FREE( pMyTestContext );
}


/*
 * This routine must be called periodically to keep retransmission
 * happening, as well as to detect timeouts.
 */
void ServiceTest( void *pTestHandle )
{
    MyTestContext *pMyTestContext = (MyTestContext *) pTestHandle;
    IEC_Goose_Service( pMyTestContext->pIecGooseContext );
}

/*
 * Test routine to publish new data. It increments all points before
 * sending the data.
 */
void SendTestData( void *pTestHandle )
{
    MyTestContext *pMyTestContext = (MyTestContext *) pTestHandle;

    IncrementGooseData( pMyTestContext->pOutputGoose );
    if ( !SendGooseData( pMyTestContext->pIecGooseContext, pMyTestContext->pOutputGoose ) )
        printf( "SendGooseData failed for SendTestData\n" );
}

void SendGooseFirst( void *pTestHandle)
{
    MyTestContext *pMyTestContext = (MyTestContext *) pTestHandle;
    IEC_Goose_Context *pIecGooseContext = pMyTestContext->pIecGooseContext;
  
       if ( !StartGooseOutput( pIecGooseContext, pMyTestContext->pOutputGoose ) ) {
        printf( "Goose output failed to start!\n" );
        TMW_TARG_FREE( pMyTestContext );
        TMW_TARG_FREE( pIecGooseContext );
        //return( NULL );       
        }
           
       if ( !StartGooseInput( pIecGooseContext, pMyTestContext->pInputGoose ) ) {
         printf( "Goose input failed to start!\n" );
         TMW_TARG_FREE( pMyTestContext );
         TMW_TARG_FREE( pIecGooseContext );
         //return( NULL );
       }
       
}
#endif // #ifdef GOOSEAPP
