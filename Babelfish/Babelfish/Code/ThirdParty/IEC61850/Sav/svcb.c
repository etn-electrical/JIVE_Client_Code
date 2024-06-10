/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2002-2014 */
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
 *  Sampled Analog Values Control Block Handling.
 *
 *  This file should not require modification.
 */
#include "sys.h" //!<IEC61850>

#include "tmwtarg.h"
#include "mmstypes.h"
#include "dictiona.h"
#include "connect.h"
#include "ACSI.h"
#include "sav.h"
#include "svcb.h"
#include "ber.h"
#include "mms_erro.h"
#include "timefunc.h"
#include "HndlXcb.h"

int TMW_SAV_SvControlStart( MMSd_context    *pMmsContext,
                            void            *handle )
{
    TMW_SAV_APDU        *pApdu;
    TMW_SAV_Context     *pSvContext;
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;
    MMSd_ObjectName     *obj;
    MMSd_entry          *nvlEntry;
    int                 domIndex, nvlIndex;
    directoryType       *directory;

    if ( handle == NULL )
        return( 0 );

    directory = (directoryType *) pMmsContext->directory;
    if (directory == NULL)
        return( 0 );

    /* Make sure DataSet exists */
    obj = &reportControl->data.XSVCB.DatSet;
    nvlEntry = MMSd_locateNVL( pMmsContext, obj, &domIndex, &nvlIndex );
    if (nvlEntry == NULL)
        return( 0 );

    pSvContext = (TMW_SAV_Context *) TMW_SAV_SvControlGetContext( handle );

    pApdu = TMW_SAV_GetApdu(  pSvContext, handle );
    if ( pApdu == NULL )
        return( 0 );

    if ( pApdu->start_call_back == NULL )
        return( 0 );

    if (pApdu->start_call_back(handle, pApdu->start_call_back_data, (void *) pMmsContext, pApdu)) {
        reportControl->data.XSVCB.SvEna = TRUE;
        return TRUE;
    } else {
        return FALSE;
    }
}

void   TMW_SAV_SvControlStartAll( MMSd_context *pMmsContext )
{
    MMSd_ReportControl  *pSavControl;
    directoryType       *directory;
    int                 domain;

    directory = (directoryType *) pMmsContext->directory;

    for ( domain = 0; domain < pMmsContext->directorySize; domain++ ) {
        pSavControl = (MMSd_ReportControl *) directory[ domain ].msmv_control_blocks;
        while (pSavControl != NULL) {
            pSavControl->status.context = (void *) pMmsContext;
            TMW_SAV_SvControlSetSvEna(pSavControl, 1);
            pSavControl = pSavControl->next;
        }

        pSavControl = (MMSd_ReportControl *) directory[ domain ].usmv_control_blocks;
        while (pSavControl != NULL) {
            pSavControl->status.context = (void *) pMmsContext;
            TMW_SAV_SvControlSetSvEna(pSavControl, 1);
            pSavControl = pSavControl->next;
        }
    }
}

int TMW_SAV_SvControlStop( MMSd_context    *pMmsContext,
                           void            *handle )
{
    TMW_SAV_APDU *pApdu;
    TMW_SAV_Context *pSvContext;
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    pSvContext = reportControl->data.XSVCB.pContext;

    pApdu = TMW_SAV_GetApdu(  pSvContext, handle );
    if ( pApdu == NULL )
        return( 0 );

    if ( pApdu->stop_call_back == NULL )
        return( 0 );

    if (pApdu->stop_call_back(handle, pApdu->stop_call_back_data, (void *) pMmsContext, pApdu)) {
        reportControl->data.XSVCB.SvEna = FALSE;
        return TRUE;
    } else {
        return FALSE;
    }
}

void   TMW_SAV_SvControlStopAll( MMSd_context *pMmsContext )
{
    MMSd_ReportControl  *pSavControl;
    directoryType       *directory;
    int                 domain;

    directory = (directoryType *) pMmsContext->directory;

    for ( domain = 0; domain < pMmsContext->directorySize; domain++ ) {
        pSavControl = (MMSd_ReportControl *) directory[ domain ].msmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlStop( pMmsContext, (void *) pSavControl );
            pSavControl = pSavControl->next;
        }

        pSavControl = (MMSd_ReportControl *) directory[ domain ].usmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlStop( pMmsContext, (void *) pSavControl );
            pSavControl = pSavControl->next;
        }
    }
}

/* CAUTION: OutStr must be at least (MMSD_OBJECT_NAME_LEN + 1) in length!! */
int TMW_SAV_ObjectNameString( MMSd_ObjectName *pObj, char *outStr )
{
    int i;
    int datSetIndex;

    /* Construct DatSet name from saved format */
    datSetIndex = 0;
    switch (pObj->scope) {
    case 0:
        break;
    case 1:
        for (i = 0;
                (datSetIndex < (int) (MMSD_OBJECT_NAME_LEN + 1))
                && (pObj->domain[i] != '\0')
                && (i < MMSD_OBJECT_DOMAIN_LEN);
                i++) {
            outStr[datSetIndex++] = pObj->domain[i];
        }
        break;
    case 2:
        outStr[datSetIndex++] = '@';
        break;
    default:
        return( 0 );
    }

    if (pObj->name[0] != '\0') {
        outStr[datSetIndex++] = '/';
        for (i = 0;
                (datSetIndex < (int) (MMSD_OBJECT_NAME_LEN + 1))
                && (pObj->name[i] != '\0')
                && (i < MMSD_OBJECT_VAR_LEN);
                i++) {
            outStr[datSetIndex++] = pObj->name[i];
        }
    }
    outStr[datSetIndex] = '\0';
    return( 1 );
}

char *TMW_SAV_SvControlGetSvCbName(  void    *handle )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( NULL );

    return( reportControl->common.pName );
}

void *TMW_SAV_SvControlGetHandle(   MMSd_context    *pMmsContext,
                                    char            *SvCbName )
{
    MMSd_entry      *pRcbDictEntry;
    MMSd_entry      *pChildEntry;
    MMSd_ObjectName obj;
    int             domainIndex, varIndex;

    if ( !MMSd_parseObjectName( SvCbName, &obj ) )
        return( NULL );

    pRcbDictEntry = MMSd_locateNV( pMmsContext, &obj, &domainIndex, &varIndex );
    if (pRcbDictEntry == NULL)
        return( NULL );

    if ( pRcbDictEntry->H.readHandler.rhandler == NULL ) {
        /* Must be parent of control bock, look for member to get handle */
        pChildEntry = MMSd_locateNvMember( pMmsContext,
                                           pRcbDictEntry,
                                           "SvEna" );
        if (pChildEntry == NULL)
            return( NULL );

        if ( pChildEntry->H.readHandler.rhandler == NULL )
            return( NULL );
        return( pChildEntry->H.readHandler.parameter );
    }
    return( pRcbDictEntry->H.readHandler.parameter );
}

void *TMW_SAV_SvControlGetContext( void *handle )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( NULL );

    return( reportControl->data.XSVCB.pContext );
}

int    TMW_SAV_SvControlSetContext(  void    *handle,
                                     void    *pContext )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    if ( (reportControl->data.XSVCB.pContext != NULL) && reportControl->data.XSVCB.SvEna )
        return( 0 );        /* Fail if in use */

    reportControl->data.XSVCB.pContext = pContext;
    return( 1 );
}

void   TMW_SAV_SvControlSetAllContext( MMSd_context    *pMmsContext,
                                       void            *pContext )
{
    MMSd_ReportControl  *pSavControl;
    directoryType       *directory;
    int                 domain;

    directory = (directoryType *) pMmsContext->directory;

    for ( domain = 0; domain < pMmsContext->directorySize; domain++ ) {
        pSavControl = (MMSd_ReportControl *) directory[ domain ].msmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlSetContext( (void *) pSavControl, pContext );
            pSavControl = pSavControl->next;
        }

        pSavControl = (MMSd_ReportControl *) directory[ domain ].usmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlSetContext( (void *) pSavControl, pContext );
            pSavControl = pSavControl->next;
        }
    }
}

int TMW_SAV_SvControlGetSvEna( void *handle, unsigned char *pEna )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    *pEna = reportControl->data.XSVCB.SvEna;
    return( 1 );
}

int TMW_SAV_SvControlSetSvEna(  void    *handle, int newState )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;
    MMSd_context        *pMmsContext = reportControl->status.context;

    if ( ( handle == NULL ) || ( pMmsContext == NULL ) )
        return( 0 );

    if (newState && !reportControl->data.XSVCB.SvEna) {
        if (!MMSd_setReportInfo( pMmsContext, reportControl, &reportControl->data.XSVCB.DatSet ))
            return( 0 );

        if (!TMW_SAV_SvControlStart( pMmsContext, (void *) reportControl ) ) {
            return( 0 );
        }
    }
    if (!newState && reportControl->data.XSVCB.SvEna) {
        if (!TMW_SAV_SvControlStop( pMmsContext, (void *) reportControl ) ) {
            return( 0 );
        }
    }

    return( 1 );
}

int TMW_SAV_SvControlGetConfRev( void *handle, unsigned long *pConfRev )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    *pConfRev = reportControl->common.ConfRev;
    return( 1 );
}

int TMW_SAV_SvControlSetConfRev( void *handle, unsigned long confRev )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;
    reportControl->common.ConfRev = confRev;
    return( 1 );
}

int TMW_SAV_SvControlGetDataSet(  void *handle, char *DatSet )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    return( TMW_SAV_ObjectNameString( &(reportControl->data.XSVCB.DatSet), DatSet ) );
}

int    TMW_SAV_SvControlSetDataSet(  void    *handle,
                                     char            *DatSet )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;
    MMSd_ObjectName     obj;

    if ( handle == NULL )
        return( 0 );

    if ( (reportControl->data.XSVCB.pContext != NULL) && reportControl->data.XSVCB.SvEna )
        return( 0 );        /* Fail if in use */

    if ( !MMSd_parseObjectName( DatSet, &obj ) )
        return( 0 );


    if (!MMSd_setReportInfo( reportControl->status.context,
                             reportControl, &obj ))
        return( 0 );
    else
        return( 1 );
}

int TMW_SAV_SvControlGetLanPars( void            *handle,
                                 TMW_Mac_Address *pAddress,
                                 unsigned int    *VLAN_AppID,
                                 unsigned int    *VLAN_Vid,
                                 unsigned char   *VLAN_Priority )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    *pAddress      = reportControl->data.XSVCB.DstAddress.GoAddr;
    *VLAN_Vid      = reportControl->data.XSVCB.DstAddress.GoVid;
    *VLAN_AppID    = reportControl->data.XSVCB.DstAddress.GoAppid;
    *VLAN_Priority = reportControl->data.XSVCB.DstAddress.GoPriority;
    return( 1 );
}

int    TMW_SAV_SvControlSetLanPars(  void    *handle,
                                     TMW_Mac_Address *pAddress,
                                     unsigned int    VLAN_AppID,
                                     unsigned int    VLAN_Vid,
                                     unsigned char   VLAN_Priority )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    if ( (reportControl->data.XSVCB.pContext != NULL) && reportControl->data.XSVCB.SvEna )
        return( 0 );        /* Fail if in use */

    reportControl->data.XSVCB.DstAddress.GoAddr       = *pAddress;
    reportControl->data.XSVCB.DstAddress.GoVid        = (unsigned short)VLAN_Vid;
    reportControl->data.XSVCB.DstAddress.GoAppid      = (unsigned short)VLAN_AppID;
    reportControl->data.XSVCB.DstAddress.GoPriority   = VLAN_Priority;

    return( 1 );
}

int TMW_SAV_SvControlGetSmpRate( void *handle, int *pSmpRate )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    *pSmpRate = reportControl->data.XSVCB.SmpRate;
    return( 1 );
}

int TMW_SAV_SvControlSetSmpRate( void *handle, int smpRate )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    if ( (reportControl->data.XSVCB.pContext != NULL) && reportControl->data.XSVCB.SvEna )
        return( 0 );        /* Fail if in use */

    reportControl->data.XSVCB.SmpRate = smpRate;
    return( 1 );
}

int TMW_SAV_SvControlGetSmpMod( void *handle, int *pSmpMod )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    *pSmpMod = reportControl->data.XSVCB.SmpMod;
    return( 1 );
}

int TMW_SAV_SvControlSetSmpMod( void *handle, int smpMod )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    if ( (reportControl->data.XSVCB.pContext != NULL) && reportControl->data.XSVCB.SvEna )
        return( 0 );        /* Fail if in use */

    if ( ( smpMod < 0 ) || ( smpMod > 2 ) )
        return( 0 );

    reportControl->data.XSVCB.SmpMod  = (unsigned char) smpMod;

    return( 1 );
}

int TMW_SAV_SvControlGetOptFlds( void *handle, unsigned char *pOptFlds )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    *pOptFlds = reportControl->data.XSVCB.OptFlds[ 0 ];
    return( 1 );
}

int TMW_SAV_SvControlSetOptFlds( void *handle, unsigned char optFlds )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    if ( (reportControl->data.XSVCB.pContext != NULL) && reportControl->data.XSVCB.SvEna )
        return( 0 );        /* Fail if in use */

    reportControl->data.XSVCB.OptFlds[ 0 ] = optFlds;
    reportControl->data.XSVCB.OptFlds[ 1 ] = 0;

    return( 1 );
}

int TMW_SAV_SvControlGetNoASDUs( void *handle, unsigned int *pNoAsdus )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    *pNoAsdus = reportControl->data.XSVCB.noASDU;
    return( 1 );
}

int TMW_SAV_SvControlSetNoASDUs( void *handle, unsigned int noASDU )
{
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) handle;

    if ( handle == NULL )
        return( 0 );

    if ( (reportControl->data.XSVCB.pContext != NULL) && reportControl->data.XSVCB.SvEna )
        return( 0 );        /* Fail if in use */

    reportControl->data.XSVCB.noASDU = noASDU;

    return( 1 );
}

int TMW_SAV_SvControlSetStartStop( void             *handle,
                                   TMW_SAV_Start    start_call_back,
                                   void             *start_call_back_data,
                                   TMW_SAV_Stop     stop_call_back,
                                   void             *stop_call_back_data )
{
    TMW_SAV_APDU *pApdu;
    TMW_SAV_Context *pSvContext;

    pSvContext = (TMW_SAV_Context *) TMW_SAV_SvControlGetContext( handle );

    pApdu = TMW_SAV_GetApdu(  pSvContext, handle );
    if ( pApdu == NULL )
        return( 0 );

    pApdu->start_call_back = start_call_back;
    pApdu->start_call_back_data = start_call_back_data;
    pApdu->stop_call_back = stop_call_back;
    pApdu->stop_call_back_data = stop_call_back_data;
    return( 1 );
}

void   TMW_SAV_SvControlSetAllStartStop( MMSd_context     *pMmsContext,
        TMW_SAV_Start    start_call_back_data,
        void             *start_call_back,
        TMW_SAV_Stop     stop_call_back,
        void             *stop_call_back_data )
{
    MMSd_ReportControl  *pSavControl;
    directoryType       *directory;
    int                 domain;

    directory = (directoryType *) pMmsContext->directory;

    for ( domain = 0; domain < pMmsContext->directorySize; domain++ ) {
        pSavControl = (MMSd_ReportControl *) directory[ domain ].msmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlSetStartStop( (void *) pSavControl,
                                           start_call_back_data,
                                           start_call_back,
                                           stop_call_back,
                                           stop_call_back_data );
            pSavControl = pSavControl->next;
        }

        pSavControl = (MMSd_ReportControl *) directory[ domain ].usmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlSetStartStop( (void *) pSavControl,
                                           start_call_back_data,
                                           start_call_back,
                                           stop_call_back,
                                           stop_call_back_data );
            pSavControl = pSavControl->next;
        }
    }
}

int TMW_SAV_SvControlCreate( TMW_SAV_Context *pSvContext, void *handle )
{
    MMSd_ReportControl  *pReportControl = (MMSd_ReportControl *) handle;
    TMW_SAV_APDU        *pApdu;
    TMW_SAV_Error       nError;
    TMW_SAV_ASDU        *pAsdu;
    char                datSet[ MMSD_OBJECT_NAME_LEN + 1 ];
    int                 noASDUs, i;
    unsigned int        optFlds;

    if ( !TMW_SAV_ObjectNameString( &pReportControl->data.XSVCB.DatSet, datSet ) )
        return( 0 );

    nError = TMW_SAV_CreateSecure( pSvContext,
                                   handle,
                                   NULL,
                                   0,
                                   &pReportControl->data.XSVCB.DstAddress.GoAddr,
                                   pReportControl->data.XSVCB.DstAddress.GoAppid,
                                   pReportControl->data.XSVCB.DstAddress.GoVid,
                                   pReportControl->data.XSVCB.DstAddress.GoPriority,
                                   pReportControl->data.XSVCB.pPrivateSecurityData,      /* PrivateSecurity    */
                                   pReportControl->data.XSVCB.nPrivateSecurityDataLen,   /* PrivateSecurityLen */
                                   pReportControl->data.XSVCB.genVlanSignature,
                                   pReportControl->data.XSVCB.pCryptoContext,
                                   pReportControl->data.XSVCB.pVlanSignatureData );
    if ( nError != TMW_SAV_SUCCESS )
        return( 0 );

    pApdu = TMW_SAV_GetApdu( pSvContext, handle );
    if ( pApdu == NULL )
        return( 0 );

    pApdu->smpRate = pReportControl->data.XSVCB.SmpRate;
    pApdu->smpCnt = 0;

    optFlds = ( pReportControl->data.XSVCB.OptFlds[ 0 ] << 8 ) | pReportControl->data.XSVCB.OptFlds[1];

    noASDUs = (int) pReportControl->data.XSVCB.noASDU;
    for ( i = 0; i < noASDUs; i++ ) {
        pAsdu = TMW_SAV_Add_ASDU( pApdu, NULL, (void *) NULL );
        if ( pAsdu == NULL )
            return( 0 );

        pAsdu->smpCnt = 0;
        pAsdu->confRev = pReportControl->common.ConfRev;
        MMSd_adjusted_utc_time_stamp( &pAsdu->refrTm );
        pAsdu->smpSynch = pReportControl->data.XSVCB.SmpSynch;
        pAsdu->smpRate = pApdu->smpRate;
        pAsdu->smpMod = pReportControl->data.XSVCB.SmpMod;

        /* This allows us to do 9-2LE, but also get smpMod when it is needed. */
        if ( optFlds & TMW_SAV_smpRate_VALID )
            optFlds |= TMW_SAV_smpMod_VALID;

        pAsdu->validMembers = optFlds
                              | TMW_SAV_svID_VALID            /* Always present */
                              | TMW_SAV_smpCnt_VALID          /* Always present */
                              | TMW_SAV_confRev_VALID         /* Always present */
                              | TMW_SAV_smpSynch_VALID        /* Always present (although in SvOptFlds) */
#if (0)
                              /* This doesn't appear in OptFlds, but if present breaks 9-2LE! */
                              | TMW_SAV_smpMod_VALID;         /* Always present */
#else
                              ;
#endif
        /* Controlled by SvOptFlds:
                            | TMW_SAV_refrTm_VALID
                            | TMW_SAV_datSet_VALID
                            | TMW_SAV_smpRate_VALID */

        pAsdu->encode_callback = NULL;
        pAsdu->encode_callback_data = NULL;

        strcpy( pAsdu->svID, pReportControl->data.XSVCB.XsvID );
        strcpy( pAsdu->datSet, datSet );
    }
    pApdu->start_call_back = NULL;
    pApdu->start_call_back_data = NULL;
    pApdu->stop_call_back = NULL;
    pApdu->stop_call_back_data = NULL;

    return( 1 );
}

int TMW_SAV_SvControlCreateAll( MMSd_context *pMmsContext, TMW_SAV_Context *pSvContext )
{
    MMSd_ReportControl  *pSavControl;
    directoryType       *directory;
    int                 domain;
    int                 count = 0;

    directory = (directoryType *) pMmsContext->directory;

    for ( domain = 0; domain < pMmsContext->directorySize; domain++ ) {
        pSavControl = (MMSd_ReportControl *) directory[ domain ].msmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlCreate( pSvContext, (void *) pSavControl );
            count++;
            pSavControl = pSavControl->next;
        }

        pSavControl = (MMSd_ReportControl *) directory[ domain ].usmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlCreate( pSvContext, (void *) pSavControl );
            count++;
            pSavControl = pSavControl->next;
        }
    }
    return( count );
}

int TMW_SAV_SvControlDelete( TMW_SAV_Context *pSvContext, void *handle )
{
    if ( TMW_SAV_Delete( pSvContext, handle ) != TMW_SAV_SUCCESS )
        return( 0 );
    return( 1 );
}

int TMW_SAV_SvControlDeleteAll( MMSd_context *pMmsContext, TMW_SAV_Context *pSvContext )
{
    MMSd_ReportControl  *pSavControl;
    directoryType       *directory;
    int                 domain;
    int                 count = 0;

    directory = (directoryType *) pMmsContext->directory;

    for ( domain = 0; domain < pMmsContext->directorySize; domain++ ) {
        pSavControl = (MMSd_ReportControl *) directory[ domain ].msmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlDelete( pSvContext, (void *) pSavControl );
            count++;
            pSavControl = pSavControl->next;
        }

        pSavControl = (MMSd_ReportControl *) directory[ domain ].usmv_control_blocks;
        while (pSavControl != NULL) {
            TMW_SAV_SvControlDelete( pSvContext, (void *) pSavControl );
            count++;
            pSavControl = pSavControl->next;
        }
    }
    return( count );
}


int     TMW_SAV_readSvEna( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readBoolean( N, size, (void *) &reportControl->data.XSVCB.SvEna, E ));
}

void    TMW_SAV_writeSvEna( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    int temp = 0;


    MMSd_writeBoolean( N, D, size, (void *) &temp, E );

    if (temp) {
        TMW_SAV_Context * pSvContext = (TMW_SAV_Context *) TMW_SAV_SvControlGetContext( parameter );
        TMW_SAV_APDU * pApdu = TMW_SAV_GetApdu( pSvContext, parameter );
        TMW_SAV_ASDU * pAsdu;
        int i;
        pAsdu = pApdu->pHead;
        for (i = 0; i < pApdu->noASDUs; i++) {
            pAsdu->confRev = reportControl->common.ConfRev;
            pAsdu = pAsdu->pNext;
        }
    }

    if ( MMSd_noError( *((MMSd_descriptor *) E) ) && MMSd_noError( *((MMSd_descriptor *) D) ) ) {
        if ( !TMW_SAV_SvControlSetSvEna( (void *) reportControl, (temp != 0) ) ) {
            MMSd_setError( MMSd_data_temporarily_unavailable, E );
        }
    }
}

int     TMW_SAV_readSvID( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readString( N, size, (void *) reportControl->data.XSVCB.XsvID, E ));
}

void    TMW_SAV_writeSvID( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    if ( reportControl->data.XSVCB.SvEna ) {
        MMSd_setError( MMSd_data_temporarily_unavailable, E );
        return;
    }
    MMSd_writeString( N, D, size, (void *) reportControl->data.XSVCB.XsvID, E );
}

int     TMW_SAV_readSvDatSet( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readObjectName( N, size, (void *) &(reportControl->data.XSVCB.DatSet), E ));
}

void    TMW_SAV_writeSvDatSet( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_context        *mms_context;
    MMSd_ObjectName     temp;
    MMSd_ReportControl  *reportControl = (MMSd_ReportControl *) parameter;

    if ( reportControl->data.XSVCB.SvEna ) {
        MMSd_setError( MMSd_data_temporarily_unavailable, E );
        return;
    }
    mms_context = MMSd_get_handler_mms_context( E );
    MMSd_writeObjectName( N, D, size, (void *) &(temp), E );

#if defined(TMW_USE_TEST_APP_CALLBACKS)
    {
        MMSd_ServerInterfaceTable * sTable;

        MMSd_context * pMmsContext;
        pMmsContext = MMSd_get_handler_mms_context( E );
        sTable = pMmsContext->serverInterface;

        if ( pMmsContext->serverInterface != NULL ) {
            if ( sTable->pCB_write_callback != NULL ) {
                unsigned long applicationError;
				char tmpDataSetName[MMSD_OBJECT_NAME_LEN];
				strcpy(tmpDataSetName, temp.domain);
				strcat(tmpDataSetName, "/" );
				strcat(tmpDataSetName, temp.name);
				applicationError = sTable->pCB_write_callback(
						pMmsContext, reportControl, "DatSet", (void*)tmpDataSetName);
                if ( applicationError ) {
                    TMW_TARG_TRACE_MSG(
                            TmwTargTraceMaskAcsi, TmwTargTraceLevelWarn,
                            "Application blocked read of  %s$%s with error %ld\n",
                            reportControl->common.pName, "DatSet", applicationError );
                    MMSd_setError( applicationError, E );
                    return;
                }
            }
        }
    }
#endif

    if (!MMSd_setReportInfo( mms_context, reportControl, &temp )) {
        MMSd_setError( MMSd_data_object_attribute_inconsistent, E );
        return;
    }
    reportControl->data.XSVCB.DatSet = temp;
}

int     TMW_SAV_readSvConfRev( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return( MMSd_readUnsigned( N, size, &reportControl->common.ConfRev, E ) );
}

void    TMW_SAV_writeSvConfRev( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_writeFailure( N, D, size, parameter, E );
}

int  TMW_SAV_readSvOptFlds( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readBitString( N, size, (void *) reportControl->data.XSVCB.OptFlds, E ));
}

void TMW_SAV_writeSvOptFlds( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    MMSd_writeBitString( N, D, size, (void *) reportControl->data.XSVCB.OptFlds, E );
}

int  TMW_SAV_readSvSmpRate( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readUnsigned( N, size, (void *) &reportControl->data.XSVCB.SmpRate, E ));
}

void TMW_SAV_writeSvSmpRate( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    MMSd_writeUnsigned( N, D, size, (void *) &reportControl->data.XSVCB.SmpRate, E );
}

int  TMW_SAV_readSvResv( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;

    if ( size != 1 ) {
        MMSd_setError( MMSd_data_type_inconsistent, E );
        return( 0 );
    }
    return(MMSd_readBoolean( N, size, (void *) &reportControl->common.Resv, E ));
}

void TMW_SAV_writeSvResv( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    if ( size != 1 )
        MMSd_setError( MMSd_data_type_inconsistent, E );
    else
        MMSd_writeBoolean( N, D, size, (void *) &reportControl->common.Resv, E );
}
int  TMW_SAV_readSvSmpMod( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;

    if ( size != 1 ) {
        MMSd_setError( MMSd_data_type_inconsistent, E );
        return( 0 );
    }
    return(MMSd_readInteger( N, 1, (void *) &reportControl->data.XSVCB.SmpMod, E ));
}

void TMW_SAV_writeSvSmpMod( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    if ( size != 1 )
        MMSd_setError( MMSd_data_type_inconsistent, E );
    else
        MMSd_writeInteger( N, D, 1, (void *) &reportControl->data.XSVCB.SmpMod, E );
}

int  TMW_SAV_readSvNoASDU( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readUnsigned( N, size, (void *) &reportControl->data.XSVCB.noASDU, E ));
}

void TMW_SAV_writeSvNoASDU( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    MMSd_writeUnsigned( N, D, size, (void *) &reportControl->data.XSVCB.noASDU, E );
}

int     TMW_SAV_readSvDstAddr( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readOctetString( N, size,
                                 (void *) reportControl->data.XSVCB.DstAddress.GoAddr.addr,
                                 E ));
}

void    TMW_SAV_writeSvDstAddr( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    if ( reportControl->data.XSVCB.SvEna ) {
        MMSd_setError( MMSd_data_temporarily_unavailable, E );
        return;
    }

    if (size != 6) {
        MMSd_setError( MMSd_data_type_inconsistent, E );
        return;
    }
    MMSd_writeOctetString( N, D, size,
                           (void *) reportControl->data.XSVCB.DstAddress.GoAddr.addr,
                           E );
    reportControl->data.XSVCB.DstAddress.GoAddr.len = (unsigned char) size;
}

void    TMW_SAV_writeSvDstAddrR( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    if ( reportControl->data.XSVCB.SvEna ) {
        MMSd_setError( MMSd_data_temporarily_unavailable, E );
        return;
    }

    if (size != 4) {
        MMSd_setError( MMSd_data_type_inconsistent, E );
        return;
    }
    MMSd_writeOctetString( N, D, size,
                           (void *) reportControl->data.XSVCB.DstAddress.GoAddr.addr,
                           E );
    reportControl->data.XSVCB.DstAddress.GoAddr.len = (unsigned char) size;
}

int     TMW_SAV_readSvPriority( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readUnsigned( N, size,
                              (void *) &reportControl->data.XSVCB.DstAddress.GoPriority,
                              E ));
}

void    TMW_SAV_writeSvPriority( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    if ( reportControl->data.XSVCB.SvEna ) {
        MMSd_setError( MMSd_data_temporarily_unavailable, E );
        return;
    }

    if (size != 1) {
        MMSd_setError( MMSd_data_type_inconsistent, E );
        return;
    }
    MMSd_writeUnsigned( N, D, size,
                        (void *) &reportControl->data.XSVCB.DstAddress.GoPriority,
                        E );
}

int     TMW_SAV_readSvVid( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readUnsigned( N, size,
                              (void *) &reportControl->data.XSVCB.DstAddress.GoVid,
                              E ));
}

void    TMW_SAV_writeSvVid( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    if ( reportControl->data.XSVCB.SvEna ) {
        MMSd_setError( MMSd_data_temporarily_unavailable, E );
        return;
    }
    if (size > 2) {
        MMSd_setError( MMSd_data_type_inconsistent, E );
        return;
    }
    MMSd_writeUnsigned( N, D, size,
                        (void *) &reportControl->data.XSVCB.DstAddress.GoVid,
                        E );
}

int     TMW_SAV_readSvAppid( void * N, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    return(MMSd_readUnsigned( N, size,
                              (void *) &reportControl->data.XSVCB.DstAddress.GoAppid,
                              E ));
}

void    TMW_SAV_writeSvAppid( void * N, void *D, int size, void * parameter, void * E )
{
    MMSd_ReportControl *reportControl = (MMSd_ReportControl *) parameter;
    if ( reportControl->data.XSVCB.SvEna ) {
        MMSd_setError( MMSd_data_temporarily_unavailable, E );
        return;
    }
    if (size > 2) {
        MMSd_setError( MMSd_data_type_inconsistent, E );
        return;
    }
    MMSd_writeUnsigned( N, D, size,
                        (void *) &reportControl->data.XSVCB.DstAddress.GoAppid,
                        E );
}

