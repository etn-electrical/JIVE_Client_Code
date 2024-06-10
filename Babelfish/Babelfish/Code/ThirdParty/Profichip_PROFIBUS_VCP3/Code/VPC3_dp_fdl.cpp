/***********************  Filename: dp_fdl.c  ********************************/
/* ========================================================================= */
/*                                                                           */
/* 0000  000   000  00000 0  000  0   0 0 0000                               */
/* 0   0 0  0 0   0 0     0 0   0 0   0 0 0   0      Einsteinstraße 6        */
/* 0   0 0  0 0   0 0     0 0     0   0 0 0   0      91074 Herzogenaurach    */
/* 0000  000  0   0 000   0 0     00000 0 0000       Germany                 */
/* 0     00   0   0 0     0 0     0   0 0 0                                  */
/* 0     0 0  0   0 0     0 0   0 0   0 0 0          Phone: ++499132744200   */
/* 0     0  0  000  0     0  000  0   0 0 0    GmbH  Fax:   ++4991327442164  */
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/* Technical support:       eMail: support@profichip.com                     */
/*                          Phone: ++49-9132-744-2150                        */
/*                          Fax. : ++49-9132-744-29-2150                     */
/*                                                                           */
/*****************************************************************************/

/*! \file
     \brief Basic functions for DPV1 communication.

*/

/*****************************************************************************/
/* contents:

  - function prototypes
  - data structures
  - internal functions

*/
/*****************************************************************************/
/* include hierarchy */
#include "includes.h"			// Added by Sagar on 24.09.2012 for DPV1 integration
#include "VPC3_platform.h"
#include "VPC3.h"				// Added by Sagar on 24.09.2012 for DPV1 integration
#include "VPC3_dp_inc.h"

#if DP_FDL /*compile only, if fdl-application is selected --> DpCfg.h */

/*---------------------------------------------------------------------------*/
/* function prototypes                                                       */
/*---------------------------------------------------------------------------*/
static void                FDL_SetSaSsap              ( UBYTE bSapNr, UBYTE bReqSa, UBYTE bReqSsap );
static void                FDL_PerformIndDisableSAP50 ( FDL_SAP_HEADER_PTR psFdlHeader );
static void                FDL_PerformIndDisable      ( FDL_SAP_HEADER_PTR psFdlHeader );
static FDL_SAP_HEADER_PTR  FDL_GetSapHeaderPtr        ( UBYTE bSapNr );

/*---------------------------------------------------------------------------*/
/* function: FDL_SetDpv1ResponseLength                                       */
/*---------------------------------------------------------------------------*/
void FDL_SetDpv1ResponseLength( MSG_HEADER_PTR psMsgHeader, UBYTE bLength )
{
   if( psMsgHeader->bDataLength > bLength )
   {
      psMsgHeader->bDataLength = bLength;
   } /* if( psMsgHeader->bDataLength > bLength ) */
} /* void FDL_SetDpv1ResponseLength( MSG_HEADER_PTR psMsgHeader, UBYTE bLength ) */

/*---------------------------------------------------------------------------*/
/* function: FDL_SetDpv1ErrorCode                                            */
/*---------------------------------------------------------------------------*/
DPV1_RET_VAL FDL_SetDpv1ErrorCode( MSG_HEADER_PTR psMsgHeader, UBYTE bError1, UBYTE bError2 )
{
   psMsgHeader->bIndex = bError1;
   psMsgHeader->bDataLength = bError2;
   return DPV1_NOK;
} /* DPV1_RET_VAL FDL_SetDpv1ErrorCode( MSG_HEADER_PTR psMsgHeader, UBYTE bError1, UBYTE bError2 ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  DP_ERROR_CODE FDL_Init( UBYTE bSlaveAddress )                          */
/*                                                                           */
/*   function:      Initializing FDL-Interface.                              */
/*                                                                           */
/*   parameter:     none                                                     */
/*   return value:  DP_OK        : Buffer length OK                          */
/*                  ERROR        : defined in dp_if.h                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static DP_ERROR_CODE FDL_Init( UBYTE bSlaveAddress, sFdl_Init sFdlInit )
{
FDL_SAP_HEADER_PTR psFdlHeader;
VPC3_UNSIGNED8_PTR pToVpc3;                     /**< pointer to VPC3, µController formatted */
UBYTE            bVpc3SegmentAddress;         /**< segment address in VPC3-ASIC */
UWORD           wResponseBufferLength;       /**< response buffer */
UWORD           wIndicationBufferLength;     /**< first indication buffer */
UWORD           wIndicationBuffer2Length;    /**< second indication buffer */
UWORD           wFdlUsedMemory;              /**< memory used by fdl */
UBYTE            bFdlIndex;
UBYTE            i;

   psFdlHeader = &pDpSystem->sFdl.asSapHeaderList[0];

   memset( &pDpSystem->sFdl, 0, sizeof( FDL_STRUC ) );
   memset( &pDpSystem->sAl,  0, sizeof( AL_STRUC  ) );
   memset( &pDpSystem->sC1,  0, sizeof( C1_STRUC  ) );
   memset( &pDpSystem->sC2,  0, sizeof( C2_STRUC  ) );
   memset( &pDpSystem->sFdl.abSearchSap[0], 0xFF, sizeof( SIZE_OF_SEARCH_KEY ) );

   /* setup FDL settings */
   pDpSystem->sC1.eSapSupported    = sFdlInit.eC1SapSupported;
   pDpSystem->sC2.bC2Enable        = sFdlInit.bC2Enable;
   pDpSystem->sC2.bC2_NumberOfSaps = sFdlInit.bC2_NumberOfSaps;

   wFdlUsedMemory = 0;
   bFdlIndex = 0;

   /* on error: empty sap-list - first byte of sap-list becomes end-criterion */
   VPC3_SET_EMPTY_SAP_LIST();

   pDpSystem->sFdl.eRmState  = RM_LOCKED;
   pDpSystem->sFdl.bOpenChannel = FALSE;

   /* Adresse 17: sap_list_ptr vpc3.sap_list / 8 */
   pVpc3->bFdlSapListPtr VPC3_EXTENSION = VPC3_SAP_CTRL_LIST_START;

   #if VPC3_SERIAL_MODE
      /* pointer mc formatted */
      pToVpc3 = (VPC3_UNSIGNED8_PTR)( Vpc3AsicAddress + DP_ORG_LENGTH + SAP_LENGTH + pDpSystem->wVpc3UsedDPV0BufferMemory );
      /* pointer vpc3 formatted */
      bVpc3SegmentAddress = (UBYTE)( ( pDpSystem->wVpc3UsedDPV0BufferMemory + DP_ORG_LENGTH + SAP_LENGTH ) >> SEG_MULDIV );
   #else
      /* pointer mc formatted */
      pToVpc3 = &pVpc3->abDpBuffer[pDpSystem->wVpc3UsedDPV0BufferMemory] VPC3_EXTENSION;
      /* pointer vpc3 formatted */
      bVpc3SegmentAddress = (UBYTE)( ((VPC3_ADR)pToVpc3-(VPC3_ADR)Vpc3AsicAddress) >> SEG_MULDIV );
   #endif /* #if VPC3_SERIAL_MODE */

   #if DP_MSAC_C2
      if( pDpSystem->sC2.bC2Enable )
      {
         psFdlHeader->bSapNr         = DP_C2_RM_SAP_NR;
         psFdlHeader->bReqSsap       = SAP_ALL;
         psFdlHeader->eSapType       = FDL_RM_SAP;
         psFdlHeader->bIndBufLength  = 0;                                     /* ind-pointer with no length - only buf-header */
         psFdlHeader->bRespBufLength = FDL_RM_BUF_LENGTH;
         psFdlHeader->bServSup       = SS_SRD_LH;
         psFdlHeader->bReqSa         = bSlaveAddress;                         /* lock saps for all masters by setting own address */
         psFdlHeader++;
         bFdlIndex++;

         for( i = 0; i < pDpSystem->sC2.bC2_NumberOfSaps; i++ )
         {
            psFdlHeader->bSapNr         = MSAC_C2_SAP_NR_HIGH - i;
            psFdlHeader->bReqSsap       = SAP_ALL;
            psFdlHeader->eSapType       = FDL_MSAC_C2_SAP;
            psFdlHeader->bIndBufLength  = DP_C2_LEN;
            psFdlHeader->bRespBufLength = DP_C2_LEN;
            psFdlHeader->bServSup       = SS_SRD_LH;
            psFdlHeader->bReqSa         = bSlaveAddress;                      /* lock saps for all masters by setting own address */
            psFdlHeader++;
            bFdlIndex++;
         } /* for( i = 0; i < pDpSystem->sC2.bC2_NumberOfSaps; i++ ) */
      } /* if( pDpSystem->sC2.bC2Enable ) */
   #endif /* #if DP_MSAC_C2 */

   #if DP_MSAC_C1
      #if(DP_C1_SAPS & DP_C1_USE_ALARM_SAP)
         if( ( pDpSystem->sC1.eSapSupported & C1_SAP_50 ) == C1_SAP_50 )
         {
            psFdlHeader->bSapNr         = DP_C1_ALARM_SAP_NR;
            psFdlHeader->bReqSsap       = SAP_ALL;
            psFdlHeader->eSapType       = FDL_MSAC_C1_SAP;
            psFdlHeader->bIndBufLength  = 4;
            psFdlHeader->bRespBufLength = 4;
            psFdlHeader->bServSup       = SS_SRD_LH;
            psFdlHeader->bReqSa         = bSlaveAddress;                      /* lock saps for all masters by setting own address */
            psFdlHeader++;
            bFdlIndex++;
         } /* if( ( pDpSystem->sC1.eSapSupported & C1_SAP_50 ) == C1_SAP_50 ) */
      #endif /* #if(DP_C1_SAPS & DP_C1_USE_ALARM_SAP) */

      #if(DP_C1_SAPS & DP_C1_USE_RD_WR_SAP)
         if( ( pDpSystem->sC1.eSapSupported & C1_SAP_51 ) == C1_SAP_51 )
         {
            psFdlHeader->bSapNr         = DP_C1_RD_WR_SAP_NR;
            psFdlHeader->bReqSsap       = SAP_ALL;
            psFdlHeader->eSapType       = FDL_MSAC_C1_SAP;
            psFdlHeader->bIndBufLength  = DP_C1_LEN;
            psFdlHeader->bRespBufLength = DP_C1_LEN;
            psFdlHeader->bServSup       = SS_SRD_LH;
            psFdlHeader->bReqSa         = bSlaveAddress;                      /* lock saps for all masters by setting own address */
            psFdlHeader++;
            bFdlIndex++;
         } /* if( ( pDpSystem->sC1.eSapSupported & C1_SAP_51 ) == C1_SAP_51 ) */
      #endif /* #if(DP_C1_SAPS & DP_C1_USE_RD_WR_SAP) */
   #endif /* #if DP_MSAC_C1 */

   psFdlHeader = &pDpSystem->sFdl.asSapHeaderList[0];

   if( bFdlIndex > 0 )
   {
      for( i = 0; i < bFdlIndex; i++ )
      {
         pDpSystem->sFdl.abSearchSap[ psFdlHeader->bSapNr ] = i;

         wResponseBufferLength    = 0;
         wIndicationBufferLength  = 0;
         wIndicationBuffer2Length = 0;

         psFdlHeader->eFdlState = DISABLED;

         switch( psFdlHeader->eSapType )
         {
            case FDL_RM_SAP:
            {
               /* the rm-sap has an ind-buffer with length #0 */
               wIndicationBufferLength = ((UWORD) 0 + cSizeFdlBufferHeader + SEG_OFFSET) & SEG_ADDWORD;
               /* one response-buffer */
               wResponseBufferLength = ( (UWORD)FDL_RM_BUF_LENGTH + cSizeFdlBufferHeader + SEG_OFFSET ) & SEG_ADDWORD;
               break;
            } /* case FDL_RM_SAP: */

            case FDL_MSAC_C1_SAP:
            case FDL_MSAC_C2_SAP:
            {
               /* one indication/response-buffer */
               wIndicationBufferLength =   ( psFdlHeader->bIndBufLength > psFdlHeader->bRespBufLength )
                                         ? ( (UWORD)psFdlHeader->bIndBufLength  + cSizeFdlBufferHeader + SEG_OFFSET ) & SEG_ADDWORD
                                         : ( (UWORD)psFdlHeader->bRespBufLength + cSizeFdlBufferHeader + SEG_OFFSET ) & SEG_ADDWORD;

               wResponseBufferLength = 0;

               if( psFdlHeader->eSapType == FDL_MSAC_C2_SAP )
               {
                   wIndicationBuffer2Length = ( (UWORD)FDL_DPV2_SEC_BUF_LENGTH + cSizeFdlBufferHeader + SEG_OFFSET ) & SEG_ADDWORD;
               } /* if( psFdlHeader->eSapType == FDL_MSAC_C2_SAP ) */
               break;
            } /* case FDL_MSAC_C1_SAP: */

            default:
            {
               break;
            } /* default: */
         } /* switch( psFdlHeader->eSapType ) */

         wFdlUsedMemory += wIndicationBufferLength + wResponseBufferLength + wIndicationBuffer2Length;

         if( ( pDpSystem->wVpc3UsedDPV0BufferMemory + wFdlUsedMemory ) > ASIC_USER_RAM )
         {
            memset( &pDpSystem->sFdl.abSearchSap[0], 0xFF, sizeof( SIZE_OF_SEARCH_KEY ) );
            /* on error: empty sap-list - first byte of sap-list becomes end-criterion */
            VPC3_SET_EMPTY_SAP_LIST();

            return DP_LESS_MEM_FDL_ERROR;
         } /* if( ( pDpSystem->wVpc3UsedDPV0BufferMemory + wFdlUsedMemory ) > ASIC_USER_RAM ) */

         #if VPC3_SERIAL_MODE
            psFdlHeader->psToSapCntrlList = ( FDL_SAP_CNTRL_PTR ) (bVpc3RwStartSapCtrlList + (7*i));
         #else
            psFdlHeader->psToSapCntrlList = ( FDL_SAP_CNTRL_PTR ) &( pVpc3->abSapCtrlList[7*i] VPC3_EXTENSION );
         #endif /* #if VPC3_SERIAL_MODE */

         psFdlHeader->sSapCntrlEntry.bRespSendSapNr = psFdlHeader->bSapNr;
         psFdlHeader->sSapCntrlEntry.bReqSa         = psFdlHeader->bReqSa;

         /*  req_ssap any sap but not default-sap or ALL_SSAP */
         if( psFdlHeader->bReqSsap < DEFAULT_SAP )
         {
            psFdlHeader->sSapCntrlEntry.bReqSa |= 0x80;
         } /* if( psFdlHeader->bReqSsap < DEFAULT_SAP ) */

         psFdlHeader->sSapCntrlEntry.bAccessReqSsap     = psFdlHeader->bReqSsap;
         psFdlHeader->sSapCntrlEntry.bEventServSup      = psFdlHeader->bServSup;
         psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[0] = psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[1]  = bVpc3SegmentAddress;
         psFdlHeader->sSapCntrlEntry.bVpc3SegRespPtr    = bVpc3SegmentAddress;
         psFdlHeader->abVpc3SegIndPtr[0] = psFdlHeader->abVpc3SegIndPtr[1] = bVpc3SegmentAddress;

         #if VPC3_SERIAL_MODE
            Vpc3MemSet( (VPC3_ADR)pToVpc3, 0x00, 4 );
            Vpc3Write( ((VPC3_ADR)( pToVpc3 )) + 1, psFdlHeader->bIndBufLength );
         #else
            _INIT_FDL_BUF_HEAD( (FDL_BUFFER_HEADER_PTR)pToVpc3, psFdlHeader->bIndBufLength );
         #endif /* #if VPC3_SERIAL_MODE */

         psFdlHeader->psToIndBuffer[0] = psFdlHeader->psToIndBuffer[1] = (FDL_BUFFER_HEADER_PTR)pToVpc3;
         psFdlHeader->psToRespBuffer   = (FDL_BUFFER_HEADER_PTR)pToVpc3;

         INCR_VPC3_UP_PTR__( wIndicationBufferLength );

         /* msac-c2-saps with wait-buffer */
         if( wIndicationBuffer2Length > 0 )
         {
            psFdlHeader->psToIndBuffer[1] = (FDL_BUFFER_HEADER_PTR)pToVpc3;
            psFdlHeader->abVpc3SegIndPtr[1] = bVpc3SegmentAddress;
            psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[1] = bVpc3SegmentAddress;

            #if VPC3_SERIAL_MODE
               Vpc3MemSet( (VPC3_ADR)pToVpc3, 0x00, 4 );
               Vpc3Write( ((VPC3_ADR)( pToVpc3 )) + 1, FDL_DPV2_SEC_BUF_LENGTH );
            #else
               _INIT_FDL_BUF_HEAD( (FDL_BUFFER_HEADER_PTR)pToVpc3, FDL_DPV2_SEC_BUF_LENGTH );
            #endif /* #if VPC3_SERIAL_MODE */

            INCR_VPC3_UP_PTR__( wIndicationBuffer2Length );
         } /* if( wIndicationBuffer2Length > 0 ) */

         /* rm-sap with response-buffer */
         if( wResponseBufferLength )
         {
            psFdlHeader->sSapCntrlEntry.bVpc3SegRespPtr = bVpc3SegmentAddress;
            psFdlHeader->psToRespBuffer      = (FDL_BUFFER_HEADER_PTR)pToVpc3;

            /* the rm-sap has one response-buffer that will be initialized now */
            #if VPC3_SERIAL_MODE
               Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToRespBuffer )) + 0, 0 );
               Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToRespBuffer )) + 1, FDL_RM_BUF_LENGTH );
               Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToRespBuffer )) + 2, FDL_RM_BUF_LENGTH );
               Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToRespBuffer )) + 3, FC_RESP_L );

               pDpSystem->sFdl.psRmRespBuffer = (FDL_RM_ANSWER_PTR)&pToVpc3[cSizeFdlBufferHeader];

               Vpc3Write( ((VPC3_ADR)( pDpSystem->sFdl.psRmRespBuffer )) + 0, DPV1_FC_RM );
               Vpc3Write( ((VPC3_ADR)( pDpSystem->sFdl.psRmRespBuffer )) + 2, (UBYTE)(( MSAC_C2_TIMEOUT_1_5MBAUD >> 8 ) & 0x00ff ) );
               Vpc3Write( ((VPC3_ADR)( pDpSystem->sFdl.psRmRespBuffer )) + 3, (UBYTE)(  MSAC_C2_TIMEOUT_1_5MBAUD & 0x00ff ) );
            #else
               ((FDL_BUFFER_HEADER_PTR) pToVpc3)->bControl             = 0;
               ((FDL_BUFFER_HEADER_PTR) pToVpc3)->bIndicationLength    = FDL_RM_BUF_LENGTH;
               ((FDL_BUFFER_HEADER_PTR) pToVpc3)->bFunctionCode        = FC_RESP_L;
               ((FDL_BUFFER_HEADER_PTR) pToVpc3)->bMaxIndicationLength = FDL_RM_BUF_LENGTH;

               pDpSystem->sFdl.psRmRespBuffer = (FDL_RM_ANSWER_PTR)&pToVpc3[cSizeFdlBufferHeader];
               pDpSystem->sFdl.psRmRespBuffer->bDpv1Service  = DPV1_FC_RM;
               pDpSystem->sFdl.psRmRespBuffer->bSendTimeOutH = (UBYTE)(( MSAC_C2_TIMEOUT_1_5MBAUD >> 8 ) & 0x00ff );
               pDpSystem->sFdl.psRmRespBuffer->bSendTimeOutL = (UBYTE)(  MSAC_C2_TIMEOUT_1_5MBAUD & 0x00ff );
            #endif /* #if VPC3_SERIAL_MODE */

            INCR_VPC3_UP_PTR__( wResponseBufferLength );
         } /* if( wResponseBufferLength ) */

         /* sap-parameters are copied in vpc3 */
         #if VPC3_SERIAL_MODE
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 0, psFdlHeader->sSapCntrlEntry.bRespSendSapNr );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 1, psFdlHeader->sSapCntrlEntry.bReqSa );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 2, psFdlHeader->sSapCntrlEntry.bAccessReqSsap );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 3, psFdlHeader->sSapCntrlEntry.bEventServSup );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 4, psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[0] );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 5, psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[1] );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 6, psFdlHeader->sSapCntrlEntry.bVpc3SegRespPtr );
         #else
            COPY_SAP_PARAMETER__( psFdlHeader->psToSapCntrlList, &psFdlHeader->sSapCntrlEntry );
         #endif /* #if VPC3_SERIAL_MODE */

         psFdlHeader++;
      } /* for( i = 0; i < bFdlIndex; i++ ) */

      /* initialisation o.k. set sap-list-end-criterion and determine the new vpc3-space left */
      VPC3_SET_SAP_LIST_END( 7 * bFdlIndex );
      pDpSystem->sFdl.bMaxSaps = bFdlIndex;
   } /* if( bFdlIndex > 0 ) */

   pDpSystem->wVpc3UsedDPV1BufferMemory = wFdlUsedMemory;

   return DP_OK;
} /* static DP_ERROR_CODE FDL_Init( UBYTE bSlaveAddress, sFdl_Init sFdlInit ) */

/*---------------------------------------------------------------------------*/
/* function: FDL_InitAcyclicServices                                         */
/*---------------------------------------------------------------------------*/
/*!
  \brief Initializes VPC3+ acyclic services.

  \param[in] bSlaveAddress - PROFIBUS slave address ( range: 1..126 )
  \param[in] sFdlInit - acyclic services

  \retval DP_OK - Initialization OK
  \retval DP_LESS_MEM_FDL_ERROR - Error Overall, too much memory used
*/
DP_ERROR_CODE FDL_InitAcyclicServices( UBYTE bSlaveAddress, sFdl_Init sFdlInit )
{
DP_ERROR_CODE bError;

   bError = FDL_Init( bSlaveAddress, sFdlInit );

   if( DP_OK == bError )
   {
      /*---------------------------------------------------------------------*/
      /* initialize MSAC_C1                                                  */
      /*---------------------------------------------------------------------*/
      #if DP_MSAC_C1
         bError = MSAC_C1_Init();
      #endif /* #if DP_MSAC_C1 */

      #if DP_MSAC_C2
         if( DP_OK == bError )
         {
            /*---------------------------------------------------------------*/
            /* initialize MSAC_C2                                            */
            /*---------------------------------------------------------------*/
            bError = MSAC_C2_Init();
         } /* if( DP_OK == bError ) */
      #endif /* #if DP_MSAC_C2 */
   } /* if( DP_OK == bError ) */

   return bError;
} /* DP_ERROR_CODE FDL_InitAcyclicServices( UBYTE bSlaveAddress, sFdl_Init sFdlInit ) */

/*---------------------------------------------------------------------------*/
/* function: FDL_SetPollTimeOutIsr                                           */
/*---------------------------------------------------------------------------*/
void FDL_SetPollTimeOutIsr( UWORD wSendTimeOut )
{
   #if VPC3_SERIAL_MODE
      Vpc3Write( ((VPC3_ADR)( pDpSystem->sFdl.psRmRespBuffer )) + 2, (UBYTE)(( wSendTimeOut >> 8 ) & 0x00ff ) );
      Vpc3Write( ((VPC3_ADR)( pDpSystem->sFdl.psRmRespBuffer )) + 3, (UBYTE)(  wSendTimeOut & 0x00ff ) );
   #else
      pDpSystem->sFdl.psRmRespBuffer->bSendTimeOutH = (UBYTE)(( wSendTimeOut >> 8 ) & 0x00ff );
      pDpSystem->sFdl.psRmRespBuffer->bSendTimeOutL = (UBYTE)(  wSendTimeOut & 0x00ff );
   #endif /* #if VPC3_SERIAL_MODE */
} /* void FDL_SetPollTimeOutIsr( UWORD wSendTimeOut ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void FDL_IndicationIsr( void )                                            */
/*                                                                           */
/* function: fdl-state-machine - treat an mac-indication - REQ-PDU-IND       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void FDL_IndicationIsr( void )
{
FDL_SAP_HEADER_PTR  psFdlHeader;
UBYTE             abControl[2];
UBYTE             bHelp;
UBYTE             i;

   psFdlHeader = &pDpSystem->sFdl.asSapHeaderList[0];

   for( i = 0; i < pDpSystem->sFdl.bMaxSaps; i++ )
   {
      #if VPC3_SERIAL_MODE
         abControl[0] = Vpc3Read( (VPC3_ADR)psFdlHeader->psToIndBuffer[0] );
         abControl[1] = Vpc3Read( (VPC3_ADR)psFdlHeader->psToIndBuffer[1] );
      #else
         abControl[0] = psFdlHeader->psToIndBuffer[0]->bControl;
         abControl[1] = psFdlHeader->psToIndBuffer[1]->bControl;
      #endif /* #if VPC3_SERIAL_MODE */

      if(    ( ( abControl[0] & PCNTRL_IND ) || ( abControl[1] & PCNTRL_IND ) )
          && ( psFdlHeader->eFdlState == ENABLED ) )
      {
         if( psFdlHeader->eSapType == FDL_RM_SAP )
         {
            #if DP_MSAC_C2
               FDL_SAP_HEADER_PTR  psFdlHeaderRm;

               /* set the dpv2-sap to it's new master */
               #if VPC3_SERIAL_MODE
                  bHelp = Vpc3Read( ((VPC3_ADR)( pDpSystem->sFdl.psRmRespBuffer )) + 1 );
                  psFdlHeaderRm = FDL_GetSapHeaderPtr( bHelp );
                  /*  allow access to the sap from the detected master and it's source-sap */
                  FDL_SetSaSsap( psFdlHeaderRm->bSapNr,
                                 Vpc3Read( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 1 ),
                                 Vpc3Read( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 2 ) & 0x7f );
               #else
                  psFdlHeaderRm = FDL_GetSapHeaderPtr(pDpSystem->sFdl.psRmRespBuffer->bSapNr);
                  /* allow access to the sap from the detected master and it's source-sap */
                  FDL_SetSaSsap( psFdlHeaderRm->bSapNr, (UBYTE)( psFdlHeader->psToSapCntrlList->bReqSa ),
                                          (UBYTE)( psFdlHeader->psToSapCntrlList->bAccessReqSsap & 0x7f ) );
               #endif /* #if VPC3_SERIAL_MODE */

               /* try to reload the rm-sap */
               pDpSystem->sFdl.eRmState = RM_LOADABLE;

               #if VPC3_SERIAL_MODE
                  /* reset several control-values of sap 49 in vpc3-sap_list */
                  Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 0, DP_C2_RM_SAP_NR );
                  Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 1, SA_ALL );
                  Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 2, SAP_ALL );

                  Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 4, 0 );

                  /* V504 */
                  /* preset the control-byte of the immediate-response-buffer */
                  Vpc3Write( (VPC3_ADR)( psFdlHeader->psToRespBuffer ), 0 );
               #else
                  /* reset several control-values of sap 49 in vpc3-sap_list */
                  psFdlHeader->psToSapCntrlList->bRespSendSapNr   = DP_C2_RM_SAP_NR;
                  psFdlHeader->psToSapCntrlList->bReqSa           = SA_ALL;
                  psFdlHeader->psToSapCntrlList->bAccessReqSsap   = SAP_ALL;

                  psFdlHeader->psToSapCntrlList->abVpc3SegIndPtr[0] = 0;

                  /* V504 */
                  /* preset the control-byte of the immediate-response-buffer */
                  psFdlHeader->psToRespBuffer->bControl = 0x00;
               #endif /* #if VPC3_SERIAL_MODE */

               /* dpv2-saps start from index #1 (index #0 is always the rm-sap) */
               psFdlHeaderRm = &pDpSystem->sFdl.asSapHeaderList[1];

               bHelp = pDpSystem->sC2.bC2_NumberOfSaps;

               while( bHelp-- )
               {
                  if( ( psFdlHeaderRm->eFdlState == DISABLED_FOR_RM ) || ( psFdlHeaderRm->eFdlState == DISABLED ) )
                  {
                     pDpSystem->sFdl.eRmState = RM_LOADED;

                     psFdlHeaderRm->eFdlState = DISABLED_IN_RM;

                     /* take the vpc3-address of the ind-buffer of the desired dpv2-sap */
                     bHelp = psFdlHeaderRm->abVpc3SegIndPtr[0];

                     /* store the uP-addresses of the ind-buffers of the desired dpv2-sap */
                     psFdlHeader->psToIndBuffer[0] = psFdlHeader->psToIndBuffer[1] = psFdlHeaderRm->psToIndBuffer[0];

                     #if VPC3_SERIAL_MODE
                        /* force rs of rm-sap by storing #0 in vpc3-sap-list */
                        Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 4, 0 );
                        /* store the vpc3-address the ind-buffer of the desired dpv2-sap in vpc3-sap-list (ind_ptr 1) */
                        Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 5, bHelp );
                        /* do the same in vpc3-sap-list with ind_ptr 0  that means: sap 49 is enabled now */
                        Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 4, bHelp );

                        /* reset the control-byte of the ind-buffer of the desired dpv2-sap */
                        Vpc3Write( (VPC3_ADR)( psFdlHeaderRm->psToIndBuffer[0] ), 0 );
                        Vpc3Write( (VPC3_ADR)( psFdlHeaderRm->psToIndBuffer[1] ), 0 );
                        Vpc3Write( (VPC3_ADR)( psFdlHeaderRm->psToRespBuffer ) , 0 );

                        /* store the new sap-number in the immediate-response-buffer */
                        Vpc3Write( ((VPC3_ADR)( pDpSystem->sFdl.psRmRespBuffer )) + 1, psFdlHeaderRm->bSapNr );

                        MSAC_C2_OpenChannelSap( psFdlHeaderRm->bSapNr );

                        /* preset the control-byte of the immediate-response-buffer for sending */
                        Vpc3Write( (VPC3_ADR)( psFdlHeader->psToRespBuffer ), ( PCNTRL_RESP + PCNTRL_USER ) );
                     #else
                        /* force rs of rm-sap by storing #0 in vpc3-sap-list */
                        psFdlHeader->psToSapCntrlList->abVpc3SegIndPtr[0] = 0;
                        /* store the vpc3-address the ind-buffer of the desired dpv2-sap in vpc3-sap-list (ind_ptr 1) */
                        psFdlHeader->psToSapCntrlList->abVpc3SegIndPtr[1] = bHelp;
                        /* do the same in vpc3-sap-list with ind_ptr 0  that means: sap 49 is enabled now */
                        psFdlHeader->psToSapCntrlList->abVpc3SegIndPtr[0] = bHelp;

                        /* reset the control-byte of the ind-buffer of the desired dpv2-sap */
                        psFdlHeaderRm->psToIndBuffer[0]->bControl = 0;
                        psFdlHeaderRm->psToIndBuffer[1]->bControl = 0;
                        psFdlHeaderRm->psToRespBuffer->bControl = 0;

                        /* store the new sap-number in the immediate-response-buffer */
                        pDpSystem->sFdl.psRmRespBuffer->bSapNr = psFdlHeaderRm->bSapNr;

                        MSAC_C2_OpenChannelSap( psFdlHeaderRm->bSapNr );

                        /* preset the control-byte of the immediate-response-buffer for sending */
                        psFdlHeader->psToRespBuffer->bControl = PCNTRL_RESP + PCNTRL_USER;
                     #endif /* #if VPC3_SERIAL_MODE */

                     break;
                  } /* if( ( psFdlHeaderRm->eFdlState == DISABLED_FOR_RM ) || ( psFdlHeaderRm->eFdlState == DISABLED )) */
                  else
                  {
                     psFdlHeaderRm++;
                  } /* else of if( ( psFdlHeaderRm->eFdlState == DISABLED_FOR_RM ) || ( psFdlHeaderRm->eFdlState == DISABLED ) ) */
               } /* while( bHelp-- ) */
            #endif /* #if DP_MSAC_C2 */
         } /* if( psFdlHeader->eSapType == FDL_RM_SAP ) */
         else
         {
            bHelp = ( abControl[0] & PCNTRL_IND ) ? 0 : 1;
            if( psFdlHeader->eFdlState == ENABLED )
            {
               /* indication-buffer for user */
               abControl[ bHelp ] &= ~PCNTRL_IND;

               #if VPC3_SERIAL_MODE
                  Vpc3Write( (VPC3_ADR)psFdlHeader->psToIndBuffer[ bHelp ], abControl[ bHelp ] );
                  CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&psFdlHeader->sMsgHeader, (VPC3_UNSIGNED8_PTR)psFdlHeader->psToIndBuffer[ bHelp ], cSizeMsgHeader );
               #else
                  psFdlHeader->psToIndBuffer[ bHelp ]->bControl = abControl[ bHelp ];
                  CopyFromVpc3_( &psFdlHeader->sMsgHeader, psFdlHeader->psToIndBuffer[ bHelp ], cSizeMsgHeader );
               #endif /* #if VPC3_SERIAL_MODE */

               switch( psFdlHeader->eSapType )
               {
                  #if DP_MSAC_C1

                     case FDL_MSAC_C1_SAP:
                     {
                         MSAC_C1_PduReceived( psFdlHeader->bSapNr,
                                              &psFdlHeader->sMsgHeader,
                                              ((VPC3_UNSIGNED8_PTR)( psFdlHeader->psToIndBuffer[ bHelp ] ) + 8 ) );

                         break;
                     } /* case FDL_MSAC_C1_SAP: */

                  #endif /* #if DP_MSAC_C1 */

                  #if DP_MSAC_C2

                     case FDL_MSAC_C2_SAP:
                     {
                         MSAC_C2_InputQueue( psFdlHeader->bSapNr,
                                              FDL_OK,
                                              MSAC_C2_IND_RECV,
                                              &psFdlHeader->sMsgHeader,
                                              ((VPC3_UNSIGNED8_PTR)( psFdlHeader->psToIndBuffer[ bHelp ] ) + 4 ) );
                         break;
                     } /* case FDL_MSAC_C2_SAP: */

                  #endif /* #if DP_MSAC_C2 */

                     default:
                     {
                         /* do nothing here */
                         break;
                     } /* default: */
               } /* switch( psFdlHeader->eSapType ) */
            } /* if( psFdlHeader->eFdlState == ENABLED ) */
         } /* else of if( psFdlHeader->eSapType == FDL_RM_SAP ) */
      } /* if(    ( ( abControl[0] & PCNTRL_IND ) || ( abControl[1] & PCNTRL_IND ) ) ... */

      psFdlHeader++;
   } /* for( i = 0; i < pDpSystem->sFdl.bMaxSaps; i++ ) */
} /* void FDL_IndicationIsr( void ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void FDL_PollEndIsr (void)                                                */
/*                                                                           */
/* function: fdl-state-machine - treat a flc-indication - POLL-END-IND       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void FDL_PollEndIsr( void )
{
FDL_SAP_HEADER_PTR  psFdlHeader;
UBYTE             bRespSendSapNr;
UBYTE             i;

   psFdlHeader = &pDpSystem->sFdl.asSapHeaderList[0];

   for( i = 0; i < pDpSystem->sFdl.bMaxSaps; i++ )
   {
      #if VPC3_SERIAL_MODE
         bRespSendSapNr = Vpc3Read( (VPC3_ADR)psFdlHeader->psToSapCntrlList );
      #else
         bRespSendSapNr = psFdlHeader->psToSapCntrlList->bRespSendSapNr;
      #endif /* #if VPC3_SERIAL_MODE */

      if( bRespSendSapNr & 0x80 )
      {
         bRespSendSapNr &= 0x7f;

         #if VPC3_SERIAL_MODE
            Vpc3Write( (VPC3_ADR)psFdlHeader->psToSapCntrlList, bRespSendSapNr );
         #else
            psFdlHeader->psToSapCntrlList->bRespSendSapNr = bRespSendSapNr;
         #endif /* #if VPC3_SERIAL_MODE */

         if( psFdlHeader->eFdlState == ENABLED )
         {
            switch( psFdlHeader->eSapType )
            {
               #if DP_MSAC_C2

                  case FDL_RM_SAP:
                  {
                     /* V504 */
                     /* preset the control-byte of the immediate-response-buffer for sending */
                     #if VPC3_SERIAL_MODE
                        Vpc3Write( (VPC3_ADR)psFdlHeader->psToRespBuffer, ( PCNTRL_RESP + PCNTRL_USER ) );
                     #else
                        psFdlHeader->psToRespBuffer->bControl = PCNTRL_RESP + PCNTRL_USER;
                     #endif /* #if VPC3_SERIAL_MODE */

                     break;
                  } /* case FDL_RM_SAP: */

                  case FDL_MSAC_C2_SAP:
                  {
                     MSAC_C2_InputQueue( psFdlHeader->bSapNr,
                                         FDL_OK,
                                         MSAC_C2_RESP_SENT,
		// Added by Sagar on 24.09.2012 for DPV1 integration   -- Org Comment: SSN Typecasted the NULL_PTR to remove the error.
                                         (MSG_HEADER_PTR)NULL_PTR,
                                         (VPC3_UNSIGNED8_PTR)NULL_PTR );
                     break;
                  } /* case FDL_MSAC_C2_SAP: */

               #endif /* #if DP_MSAC_C2 */

                  default:
                  {
                     /* do nothing here */
                     break;
                  } /* default: */
            } /* switch( psFdlHeader->eSapType ) */
         } /* if( psFdlHeader->eFdlState == ENABLED ) */
      } /* if( bRespSendSapNr & 0x80 ) */

      psFdlHeader++;
   } /* for( i = 0; i < pDpSystem->sFdl.bMaxSaps; i++ ) */
} /* void FDL_PollEndIsr( void ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void FDL_SetSaSsap( UBYTE bSapNr, UBYTE bReqSa, UBYTE bReqSsap )    */
/*                                                                           */
/*   function: This function sets temporary sa and ssap of the desired sap.  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void FDL_SetSaSsap( UBYTE bSapNr, UBYTE bReqSa, UBYTE bReqSsap )
{
FDL_SAP_HEADER_PTR  psFdlHeader;
UBYTE             bReqSaLoc;

   psFdlHeader = FDL_GetSapHeaderPtr( bSapNr );
   bReqSaLoc = bReqSa & 0x7f;

   /* perform service only, if req_ssap is in a valid range */
   if( ( bReqSsap <= DEFAULT_SAP ) || ( bReqSsap == SAP_ALL ) )
   {
      if( bSapNr != DEFAULT_SAP )
      {
         if( bReqSa != SA_ALL )
         {
            bReqSaLoc |= 0x80;
         } /* if( bReqSa != SA_ALL ) */
      } /* if( bSapNr != DEFAULT_SAP ) */

      psFdlHeader->eFdlState = ENABLED;

      /* set sap-parameters */
      #if VPC3_SERIAL_MODE
         Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 2, bReqSsap  );
         Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 1, bReqSaLoc );
      #else
         psFdlHeader->psToSapCntrlList->bAccessReqSsap = bReqSsap;
         psFdlHeader->psToSapCntrlList->bReqSa = bReqSaLoc;
      #endif /* #if VPC3_SERIAL_MODE */
   } /* if( ( bReqSsap <= DEFAULT_SAP ) || ( bReqSsap == SAP_ALL ) ) */
} /* static void FDL_SetSaSsap( UBYTE bSapNr, UBYTE bReqSa, UBYTE bReqSsap ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FDL_SAP_HEADER_PTR FDL_GetSapHeaderPtr( UBYTE bSapNr )                  */
/*                                                                           */
/*   function: return 0 or the pointer to the desired fdl-sap-structure      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
static FDL_SAP_HEADER_PTR FDL_GetSapHeaderPtr( UBYTE bSapNr )
{
UBYTE bIndex;

   bIndex = pDpSystem->sFdl.abSearchSap[ bSapNr ];
   if( ( bSapNr >= sizeof( pDpSystem->sFdl.abSearchSap ) ) || ( bIndex == 0xFF ) )
   {
      /*  error function */
      sVpc3Error.bErrorCode = bIndex;
      sVpc3Error.bCnId      = bSapNr;
      /* no further action of dpv1/c2 */
      DpAppl_FatalError( _DP_FDL, __LINE__, &sVpc3Error );
   } /* if( ( bSapNr >= sizeof( pDpSystem->sFdl.abSearchSap ) ) || ( bIndex == 0xFF ) ) */

   return ( &pDpSystem->sFdl.asSapHeaderList[ bIndex ] );
} /* static FDL_SAP_HEADER_PTR FDL_GetSapHeaderPtr( UBYTE bSapNr ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void FDL_RespProvide( UBYTE bSapNr, UBYTE bPrimSecBuf )               */
/*                                                                           */
/*   function: The function prepares the response-provide of a desired SAP   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void FDL_RespProvide( UBYTE bSapNr, UBYTE bPrimSecBuf )
{
FDL_SAP_HEADER_PTR      psFdlHeader;
FDL_BUFFER_HEADER_PTR   psToRespBuffer;
UBYTE                 bRetValue;
UBYTE                 bControl;
UBYTE                 bHelp;

   DP_LOCK_IND();

   bRetValue = FDL_OK;
   psFdlHeader = FDL_GetSapHeaderPtr( bSapNr );

   if( ( psFdlHeader->eSapType != FDL_MSAC_C2_SAP ) && ( bPrimSecBuf == FDL_SECONDARY_BUF ) )
   {
      /* no buffer available */
      bRetValue = FDL_NURB;
   } /* if( ( psFdlHeader->eSapType != FDL_MSAC_C2_SAP ) && ( bPrimSecBuf == FDL_SECONDARY_BUF ) ) */

   if( bRetValue == FDL_OK )
   {
      switch( bPrimSecBuf )
      {
         case FDL_PRIMARY_BUF:
         {
            psToRespBuffer  = psFdlHeader->psToRespBuffer;
            bHelp = psFdlHeader->bRespBufLength;
            break;
         } /* case FDL_PRIMARY_BUF: */

         case FDL_SECONDARY_BUF:
         {
            psToRespBuffer  = psFdlHeader->psToIndBuffer[1];
            bHelp = FDL_DPV2_SEC_BUF_LENGTH;
            break;
         } /* case FDL_SECONDARY_BUF: */

         default:
         {
            psToRespBuffer = (FDL_BUFFER_HEADER_PTR)0;   /*  NIL; */
            bHelp = 0;
            break;
         } /* default: */
      } /* switch( bPrimSecBuf ) */

      #if VPC3_SERIAL_MODE
         bControl = Vpc3Read( (VPC3_ADR)psToRespBuffer );
      #else
         bControl = psToRespBuffer->bControl;
      #endif /* #if VPC3_SERIAL_MODE */

      if( !( psToRespBuffer ) || ( bControl & PCNTRL_RESP ) )
      {
         /* no buffer available */
         bRetValue = FDL_NURB;
      } /* if( !( psToRespBuffer ) || ( bControl & PCNTRL_RESP ) ) */
      else
      {
         if( psFdlHeader->sMsgHeader.bIndicationLength <= bHelp )
         {
            /* length fits into buffer */
            bHelp = psFdlHeader->sMsgHeader.bFunctionCode;
            if( ( bHelp == FC_RESP_L ) || ( bHelp == FC_RESP_H ) )
            {
               if( ( psFdlHeader->eSapType == FDL_MSAC_C1_SAP ) || ( psFdlHeader->eSapType == FDL_MSAC_C2_SAP ) )
               {
                  bHelp = ( bPrimSecBuf == FDL_PRIMARY_BUF ) ? psFdlHeader->abVpc3SegIndPtr[0] : psFdlHeader->abVpc3SegIndPtr[1];

                  /* BugFix V602 */
                  #if VPC3_SERIAL_MODE
                     Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 4, bHelp );
                     Vpc3Write( ((VPC3_ADR)( psFdlHeader->psToSapCntrlList )) + 6, bHelp );
                  #else
                     psFdlHeader->psToSapCntrlList->abVpc3SegIndPtr[0] = (UBYTE)bHelp;
                     psFdlHeader->psToSapCntrlList->bVpc3SegRespPtr = (UBYTE)bHelp;
                  #endif /* #if VPC3_SERIAL_MODE */
               } /* if( ( psFdlHeader->eSapType == FDL_MSAC_C1_SAP ) || ( psFdlHeader->eSapType == FDL_MSAC_C2_SAP ) ) */

               psFdlHeader->sMsgHeader.bControl |= PCNTRL_RESP;

               #if VPC3_SERIAL_MODE
                  CopyToVpc3_( (VPC3_UNSIGNED8_PTR)psToRespBuffer+1, (MEM_UNSIGNED8_PTR)&psFdlHeader->sMsgHeader.bMaxIndicationLength, cSizeMsgHeader - 1 );
                  Vpc3Write( (VPC3_ADR)psToRespBuffer, psFdlHeader->sMsgHeader.bControl );
               #else
                  CopyToVpc3_( (VPC3_UNSIGNED8_PTR)psToRespBuffer+1, (MEM_UNSIGNED8_PTR)&psFdlHeader->sMsgHeader.bMaxIndicationLength, cSizeMsgHeader - 1 );
                  psToRespBuffer->bControl = psFdlHeader->sMsgHeader.bControl;
               #endif /* #if VPC3_SERIAL_MODE */
            } /* if( (bHelp == FC_RESP_L) || (bHelp == FC_RESP_H) ) */
            else
            {
               bRetValue = FDL_IVP;
            } /* else of if( (bHelp == FC_RESP_L) || (bHelp == FC_RESP_H) ) */
         } /* if( psFdlHeader->sMsgHeader.bIndicationLength <= bHelp ) */
         else
         {
            /* length exceeds buffer */
            bRetValue = FDL_RLEN;
         } /* else of if( psFdlHeader->sMsgHeader.bIndicationLength <= bHelp ) */
      } /* else of if( !( psToRespBuffer ) || ( bControl & PCNTRL_RESP ) ) */
   } /* if( bRetValue == FDL_OK ) */

   DP_UNLOCK_IND();

   if( bRetValue != FDL_OK )
   {
      switch( psFdlHeader->eSapType )
      {
         #if DP_MSAC_C2

            case FDL_MSAC_C2_SAP:
            {
               MSAC_C2_InputQueue( psFdlHeader->bSapNr,
                                   bRetValue,
                                   MSAC_C2_RESP_SENT,
// Added by Sagar on 24.09.2012 for DPV1 integration   -- Org Comment: SSN Typecasted the NULL_PTR to remove the error.
                                   (MSG_HEADER_PTR)NULL_PTR,        
                                   (VPC3_UNSIGNED8_PTR)NULL_PTR );
               break;
            } /* case FDL_MSAC_C2_SAP: */

         #endif /* #if DP_MSAC_C2 */

            default:
            {
               /* do nothing here */
               break;
            } /* default: */
      } /* switch( psFdlHeader->eSapType ) */
   } /* if( bRetValue != FDL_OK ) */
} /* void FDL_RespProvide( UBYTE bSapNr, UBYTE bPrimSecBuf ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void FDL_ResetCntrlRespBit( UBYTE bSapNr, UBYTE bPrimSecBuf )         */
/*                                                                           */
/*   function: The function reset the PCNTRL_RESP bit in the buffer          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void FDL_ResetCntrlRespBit( UBYTE bSapNr, UBYTE bPrimSecBuf )
{
FDL_SAP_HEADER_PTR      psFdlHeader;
FDL_BUFFER_HEADER_PTR   psToRespBuffer;
UBYTE                 bRetValue;

   DP_LOCK_IND();

   bRetValue = FDL_OK;
   psFdlHeader = FDL_GetSapHeaderPtr( bSapNr );

   if( ( psFdlHeader->eSapType != FDL_MSAC_C2_SAP ) && ( bPrimSecBuf == FDL_SECONDARY_BUF ) )
   {
      /* no buffer available */
      bRetValue = FDL_NURB;
   } /* if( ( psFdlHeader->eSapType != FDL_MSAC_C2_SAP ) && ( bPrimSecBuf == FDL_SECONDARY_BUF ) ) */

   if( bRetValue == FDL_OK )
   {
      switch( bPrimSecBuf )
      {
         case FDL_PRIMARY_BUF:
         {
             psToRespBuffer = psFdlHeader->psToRespBuffer;
             break;
         } /* case FDL_PRIMARY_BUF: */

         case FDL_SECONDARY_BUF:
         {
             psToRespBuffer = psFdlHeader->psToIndBuffer[1];
             break;
         } /* case FDL_SECONDARY_BUF: */

         default:
         {
             psToRespBuffer = (FDL_BUFFER_HEADER_PTR)0;  /*  NIL; */
             break;
         } /* default: */
      } /* switch( bPrimSecBuf ) */

      if( psToRespBuffer )
      {
         #if VPC3_SERIAL_MODE
            Vpc3Write( (VPC3_ADR)psToRespBuffer, Vpc3Read( (VPC3_ADR)psToRespBuffer ) & ~PCNTRL_RESP );
         #else
            psToRespBuffer->bControl &= ~PCNTRL_RESP;
         #endif /* #if VPC3_SERIAL_MODE */
      } /* if( psToRespBuffer ) */
   } /* if( bRetValue == FDL_OK ) */

   DP_UNLOCK_IND();
} /* void FDL_ResetCntrlRespBit( UBYTE bSapNr, UBYTE bPrimSecBuf ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void FDL_IndDisable(UBYTE bSapNr)                                       */
/*                                                                           */
/*   function: The function resets the sap to its initial value              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void FDL_IndDisable( UBYTE bSapNr )
{
FDL_SAP_HEADER_PTR  psFdlHeader;

   DP_LOCK_IND();
   psFdlHeader = FDL_GetSapHeaderPtr( bSapNr );
   /* DP_UNLOCK_IND() done in following function */
   FDL_PerformIndDisable( psFdlHeader );
} /* void FDL_IndDisable( UBYTE bSapNr ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void FDL_IndDisableSAP50( UBYTE bSapNr )                                */
/*                                                                           */
/*   function: The function resets the sap to its initial value              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#if DP_ALARM_OVER_SAP50
void FDL_IndDisableSAP50( void )
{
FDL_SAP_HEADER_PTR  psFdlHeader;

   DP_LOCK_IND();
   psFdlHeader = FDL_GetSapHeaderPtr( DP_C1_ALARM_SAP_NR );
   /* DP_UNLOCK_IND() done in following function */
   FDL_PerformIndDisableSAP50( psFdlHeader );
} /* void FDL_IndDisableSAP50( void ) */
#endif /* #if DP_ALARM_OVER_SAP50 */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* UBYTE FDL_IndAwait( UBYTE bSapNr )                                    */
/*                                                                           */
/*   function: The function the rm for a connection at the desired sap s     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
UBYTE FDL_IndAwait( UBYTE bSapNr )
{
FDL_SAP_HEADER_PTR      psFdlHeader;
UBYTE                 bRetValue;
#if DP_MSAC_C2
    FDL_SAP_HEADER_PTR  psFdlHeaderRm;  /* header to resource manager */
    UBYTE             bHelp;
#endif /* #if DP_MSAC_C2 */

   DP_LOCK_IND();

   bRetValue = FDL_OK;
   psFdlHeader = FDL_GetSapHeaderPtr( bSapNr );

   switch( psFdlHeader->eSapType )
   {
      #if DP_MSAC_C1

         case FDL_MSAC_C1_SAP:
         {
            FDL_SetSaSsap( bSapNr, SA_ALL, SAP_ALL );
            break;
         } /* case FDL_MSAC_C1_SAP: */

      #endif /* #if DP_MSAC_C1 */

      #if DP_MSAC_C2

         case FDL_MSAC_C2_SAP:
         {
            if( pDpSystem->sFdl.bOpenChannel != FALSE )
            {
               if( psFdlHeader->eFdlState == DISABLED )
               {
                  psFdlHeader->eFdlState = DISABLED_FOR_RM;

                  if( pDpSystem->sFdl.eRmState == RM_LOADABLE )
                  {
                     pDpSystem->sFdl.eRmState = RM_LOADED;

                     psFdlHeader->eFdlState = DISABLED_IN_RM;

                     /* take the vpc3-address of the ind-buffer of the desired dpv2-sap */
                     bHelp = psFdlHeader->abVpc3SegIndPtr[0];

                     psFdlHeaderRm = &pDpSystem->sFdl.asSapHeaderList[0];

                     /* store the uP-addresses of the ind-buffers of the desired dpv2-sap */
                     psFdlHeaderRm->psToIndBuffer[0] = psFdlHeaderRm->psToIndBuffer[1] = psFdlHeader->psToIndBuffer[0];

                     #if VPC3_SERIAL_MODE
                        /* reset several control-values of sap 49 in vpc3-sap_list/ */
                        Vpc3Write( ((VPC3_ADR)( psFdlHeaderRm->psToSapCntrlList )) + 0, DP_C2_RM_SAP_NR );
                        Vpc3Write( ((VPC3_ADR)( psFdlHeaderRm->psToSapCntrlList )) + 1, SA_ALL );
                        Vpc3Write( ((VPC3_ADR)( psFdlHeaderRm->psToSapCntrlList )) + 2, SAP_ALL );
                        /* force rs of rm-sap by storing #0 in vpc3-sap-list */
                        Vpc3Write( ((VPC3_ADR)( psFdlHeaderRm->psToSapCntrlList )) + 4, 0 );
                        /* store the vpc3-address the ind-buffer of the desired dpv2-sap in vpc3-sap-list (ind_ptr 1) */
                        Vpc3Write( ((VPC3_ADR)( psFdlHeaderRm->psToSapCntrlList )) + 5, bHelp );
                        /* do the same in vpc3-sap-list with ind_ptr 0  that means: sap 49 is enabled now */
                        Vpc3Write( ((VPC3_ADR)( psFdlHeaderRm->psToSapCntrlList )) + 4, bHelp );

                        /* reset the control-byte of the ind-buffer of the desired dpv2-sap */
                        Vpc3Write( (VPC3_ADR)( psFdlHeader->psToIndBuffer[0] ), 0 );
                        Vpc3Write( (VPC3_ADR)( psFdlHeader->psToIndBuffer[1] ), 0 );
                        Vpc3Write( (VPC3_ADR)( psFdlHeader->psToRespBuffer ) , 0 );

                        /* store the new sap-number in the immediate-response-buffer */
                        Vpc3Write( ((VPC3_ADR)( pDpSystem->sFdl.psRmRespBuffer )) + 1, psFdlHeader->bSapNr );
                        /* preset the control-byte of the immediate-response-buffer for sending */
                        Vpc3Write( (VPC3_ADR)( psFdlHeaderRm->psToRespBuffer ), ( PCNTRL_RESP + PCNTRL_USER ) );
                     #else
                        /* reset several control-values of sap 49 in vpc3-sap_list/ */
                        psFdlHeaderRm->psToSapCntrlList->bRespSendSapNr   = DP_C2_RM_SAP_NR;
                        psFdlHeaderRm->psToSapCntrlList->bReqSa           = SA_ALL;
                        psFdlHeaderRm->psToSapCntrlList->bAccessReqSsap   = SAP_ALL;
                        /* force rs of rm-sap by storing #0 in vpc3-sap-list */
                        psFdlHeaderRm->psToSapCntrlList->abVpc3SegIndPtr[0] = 0;
                        /* store the vpc3-address the ind-buffer of the desired dpv2-sap in vpc3-sap-list (ind_ptr 1) */
                        psFdlHeaderRm->psToSapCntrlList->abVpc3SegIndPtr[1] = bHelp;
                        /* do the same in vpc3-sap-list with ind_ptr 0  that means: sap 49 is enabled now */
                        psFdlHeaderRm->psToSapCntrlList->abVpc3SegIndPtr[0] = bHelp;

                        /* reset the control-byte of the ind-buffer of the desired dpv2-sap */
                        psFdlHeader->psToIndBuffer[0]->bControl = 0;
                        psFdlHeader->psToIndBuffer[1]->bControl = 0;
                        psFdlHeader->psToRespBuffer->bControl = 0;

                        /* store the new sap-number in the immediate-response-buffer */
                        pDpSystem->sFdl.psRmRespBuffer->bSapNr = psFdlHeader->bSapNr;

                        /* preset the control-byte of the immediate-response-buffer for sending */
                        psFdlHeaderRm->psToRespBuffer->bControl = PCNTRL_RESP + PCNTRL_USER;
                     #endif /* #if VPC3_SERIAL_MODE */
                  } /* if( pDpSystem->sFdl.eRmState == RM_LOADABLE ) */
               } /* if( psFdlHeader->eFdlState == DISABLED ) */
               else
               {
                  bRetValue = FDL_IUSE;
               } /* else of if( psFdlHeader->eFdlState == DISABLED ) */
            } /* if( pDpSystem->sFdl.bOpenChannel != FALSE ) */
            else
            {
               bRetValue = FDL_OPEN_ERR;
            } /* else of if( pDpSystem->sFdl.bOpenChannel != FALSE ) */
            break;
         } /* case FDL_MSAC_C2_SAP: */

      #endif /* #if DP_MSAC_C2 */

         default:
         {
            /* do nothing here */
            break;
         }
   } /* switch( psFdlHeader->eSapType ) */

   DP_UNLOCK_IND();

   return( bRetValue );
} /* UBYTE FDL_IndAwait( UBYTE bSapNr ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FDL_PerformSapReset( UBYTE bSapNr )                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* V504 */
void FDL_PerformSapReset( UBYTE bSapNr )
{
FDL_SAP_HEADER_PTR  psFdlHeader;

   psFdlHeader = FDL_GetSapHeaderPtr( bSapNr );

   psFdlHeader->eFdlState = DISABLED;

   #if VPC3_SERIAL_MODE
      Vpc3Write( (VPC3_ADR)( psFdlHeader->psToSapCntrlList ) + 4, 0 );
   #else
      psFdlHeader->psToSapCntrlList->abVpc3SegIndPtr[0] = 0;
   #endif /* #if VPC3_SERIAL_MODE */

   /* perform sap-reset */
   /* set indication-buffer to vpc3 */
   #if VPC3_SERIAL_MODE
      Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 0, psFdlHeader->sSapCntrlEntry.bRespSendSapNr );
      Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 1, psFdlHeader->sSapCntrlEntry.bReqSa );
      Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 2, psFdlHeader->sSapCntrlEntry.bAccessReqSsap );
      Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 3, psFdlHeader->sSapCntrlEntry.bEventServSup );
      Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 4, psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[0] );
      Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 5, psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[1] );
      Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 6, psFdlHeader->sSapCntrlEntry.bVpc3SegRespPtr );
   #else
      COPY_SAP_PARAMETER__( psFdlHeader->psToSapCntrlList, &psFdlHeader->sSapCntrlEntry );
   #endif /* #if VPC3_SERIAL_MODE */

   #if VPC3_SERIAL_MODE
      Vpc3Write( (VPC3_ADR)psFdlHeader->psToIndBuffer[0], 0 );
      Vpc3Write( (VPC3_ADR)psFdlHeader->psToIndBuffer[1], 0 );
      Vpc3Write( (VPC3_ADR)psFdlHeader->psToRespBuffer, 0 );
   #else
      psFdlHeader->psToIndBuffer[0]->bControl = 0;
      psFdlHeader->psToIndBuffer[1]->bControl = 0;
      psFdlHeader->psToRespBuffer->bControl = 0;
   #endif /* #if VPC3_SERIAL_MODE */

   /* V504 */
   /* RM */
   if( bSapNr == DP_C2_RM_SAP_NR )
   {
      pDpSystem->sFdl.eRmState     = RM_LOCKED;
      pDpSystem->sFdl.bOpenChannel = FALSE;
   } /* if( bSapNr == DP_C2_RM_SAP_NR ) */
} /* void FDL_PerformSapReset( UBYTE bSapNr ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void FDL_PerformIndDisable( FDL_SAP_HEADER_PTR  psFdlHeader )             */
/*                                                                           */
/*   function: The function resets the sap to its initial value, unlocks     */
/*             interrupt and if due: performs the user-callback-function     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* V504 */
static void FDL_PerformIndDisable( FDL_SAP_HEADER_PTR psFdlHeader )
{
UBYTE bControl1;
UBYTE bControl2;
UBYTE bControl3;
UBYTE bRetValue;

   if( psFdlHeader->eFdlState == ENABLED )
   {
      psFdlHeader->eFdlState = DISABLED;

      #if VPC3_SERIAL_MODE
         Vpc3Write( (VPC3_ADR)( psFdlHeader->psToSapCntrlList ) + 4, 0 );

         bControl1 = Vpc3Read( (VPC3_ADR)( psFdlHeader->psToIndBuffer[0] ) );
         bControl2 = Vpc3Read( (VPC3_ADR)( psFdlHeader->psToIndBuffer[1] ) );
         bControl3 = Vpc3Read( (VPC3_ADR)( psFdlHeader->psToRespBuffer ) );
      #else
         psFdlHeader->psToSapCntrlList->abVpc3SegIndPtr[0] = 0;

         bControl1 = psFdlHeader->psToIndBuffer[0]->bControl;
         bControl2 = psFdlHeader->psToIndBuffer[1]->bControl;
         bControl3 = psFdlHeader->psToRespBuffer->bControl;
      #endif /* #if VPC3_SERIAL_MODE */

      if(    ( bControl1 & PCNTRL_INUSE )
          || ( bControl2 & PCNTRL_INUSE )
          || ( bControl3 & PCNTRL_INUSE )
        )
      {
         bRetValue = FDL_IUSE;
      } /* if(    ( bControl1 & PCNTRL_INUSE ) ... */
      else
      {
         /* perform sap-reset */
         /* set indication-buffer to vpc3 */
         #if VPC3_SERIAL_MODE
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 0, psFdlHeader->sSapCntrlEntry.bRespSendSapNr );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 1, psFdlHeader->sSapCntrlEntry.bReqSa );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 2, psFdlHeader->sSapCntrlEntry.bAccessReqSsap );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 3, psFdlHeader->sSapCntrlEntry.bEventServSup );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 4, psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[0] );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 5, psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[1] );
            Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 6, psFdlHeader->sSapCntrlEntry.bVpc3SegRespPtr );
         #else
            COPY_SAP_PARAMETER__( psFdlHeader->psToSapCntrlList, &psFdlHeader->sSapCntrlEntry );
         #endif /* #if VPC3_SERIAL_MODE */

         #if VPC3_SERIAL_MODE
            Vpc3Write( (VPC3_ADR)psFdlHeader->psToIndBuffer[0], 0 );
            Vpc3Write( (VPC3_ADR)psFdlHeader->psToIndBuffer[1], 0 );
            Vpc3Write( (VPC3_ADR)psFdlHeader->psToRespBuffer, 0 );
         #else
            psFdlHeader->psToIndBuffer[0]->bControl = 0;
            psFdlHeader->psToIndBuffer[1]->bControl = 0;
            psFdlHeader->psToRespBuffer->bControl = 0;
          #endif /* #if VPC3_SERIAL_MODE */

         bRetValue = FDL_OK;
      } /* else of if(    ( bControl1 & PCNTRL_INUSE ) ... */
   } /* if( psFdlHeader->eFdlState == ENABLED ) */
   else
   {
      bRetValue = FDL_DACT;
   } /* else of if( psFdlHeader->eFdlState == ENABLED ) */

   DP_UNLOCK_IND();

   switch( psFdlHeader->eSapType )
   {
      #if DP_MSAC_C2

         case FDL_MSAC_C2_SAP:
         {
            MSAC_C2_InputQueue( psFdlHeader->bSapNr,
                                 bRetValue,
                                 MSAC_C2_IND_DISABLE_DONE,
// Added by Sagar on 24.09.2012 for DPV1 integration   -- Org Comment: SSN Typecasted the NULL_PTR to remove the error.
                                   (MSG_HEADER_PTR)NULL_PTR,
                                   (VPC3_UNSIGNED8_PTR)NULL_PTR );
            break;
         } /* case FDL_MSAC_C2_SAP: */

      #endif /* #if DP_MSAC_C2 */

      #if DP_MSAC_C1

         case FDL_MSAC_C1_SAP:
         {
            MSAC_C1_IndDisableDone( psFdlHeader->bSapNr, bRetValue );
            break;
         } /* case FDL_MSAC_C1_SAP: */

      #endif /* #if DP_MSAC_C1 */

         default:
         {
            /* do nothing here */
            break;
         } /* default: */
   } /* switch( psFdlHeader->eSapType ) */
} /* static void FDL_PerformIndDisable( FDL_SAP_HEADER_PTR psFdlHeader ) */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void FDL_PerformIndDisableSAP50( FDL_SAP_HEADER_PTR psFdlHeader )         */
/*                                                                           */
/*   function: The function resets the sap 50 to its initial value, unlocks  */
/*             interrupt and if due: performs the user-callback-function     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* V504 */
#if DP_ALARM_OVER_SAP50

static void FDL_PerformIndDisableSAP50( FDL_SAP_HEADER_PTR psFdlHeader )
{
UBYTE bControl1;
UBYTE bControl2;
UBYTE bControl3;

   psFdlHeader->eFdlState = DISABLED;

   #if VPC3_SERIAL_MODE
      Vpc3Write( (VPC3_ADR)( psFdlHeader->psToSapCntrlList ) + 4, 0 );

      bControl1 = Vpc3Read( (VPC3_ADR)( psFdlHeader->psToIndBuffer[0] ) );
      bControl2 = Vpc3Read( (VPC3_ADR)( psFdlHeader->psToIndBuffer[1] ) );
      bControl3 = Vpc3Read( (VPC3_ADR)( psFdlHeader->psToRespBuffer ) );
   #else
      psFdlHeader->psToSapCntrlList->abVpc3SegIndPtr[0] = 0;

      bControl1 = psFdlHeader->psToIndBuffer[0]->bControl;
      bControl2 = psFdlHeader->psToIndBuffer[1]->bControl;
      bControl3 = psFdlHeader->psToRespBuffer->bControl;
   #endif /* #if VPC3_SERIAL_MODE */

   if(    ( bControl1 & PCNTRL_INUSE )
       || ( bControl2 & PCNTRL_INUSE )
       || ( bControl3 & PCNTRL_INUSE )
     )
   {
      MSAC_C1_IndDisableDone( psFdlHeader->bSapNr, FDL_IUSE );
   } /* if(    ( bControl1 & PCNTRL_INUSE ) ... */
   else
   {
      /* perform sap-reset */
      /* set indication-buffer to vpc3 */
      #if VPC3_SERIAL_MODE
         Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 0, psFdlHeader->sSapCntrlEntry.bRespSendSapNr );
         Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 1, psFdlHeader->sSapCntrlEntry.bReqSa );
         Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 2, psFdlHeader->sSapCntrlEntry.bAccessReqSsap );
         Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 3, psFdlHeader->sSapCntrlEntry.bEventServSup );
         Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 4, psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[0] );
         Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 5, psFdlHeader->sSapCntrlEntry.abVpc3SegIndPtr[1] );
         Vpc3Write( ((VPC3_ADR)(psFdlHeader->psToSapCntrlList )) + 6, psFdlHeader->sSapCntrlEntry.bVpc3SegRespPtr );
      #else
         COPY_SAP_PARAMETER__( psFdlHeader->psToSapCntrlList, &psFdlHeader->sSapCntrlEntry );
      #endif /* #if VPC3_SERIAL_MODE */

      #if VPC3_SERIAL_MODE
         Vpc3Write( (VPC3_ADR)psFdlHeader->psToIndBuffer[0], 0 );
         Vpc3Write( (VPC3_ADR)psFdlHeader->psToIndBuffer[1], 0 );
         Vpc3Write( (VPC3_ADR)psFdlHeader->psToRespBuffer, 0 );
      #else
         psFdlHeader->psToIndBuffer[0]->bControl = 0;
         psFdlHeader->psToIndBuffer[1]->bControl = 0;
         psFdlHeader->psToRespBuffer->bControl = 0;
      #endif /* #if VPC3_SERIAL_MODE */
   } /* else of if(    ( bControl1 & PCNTRL_INUSE ) ... */

   DP_UNLOCK_IND();
} /* static void FDL_PerformIndDisableSAP50( FDL_SAP_HEADER_PTR psFdlHeader ) */

#endif /* #if DP_ALARM_OVER_SAP50 */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   void FDL_OpenChannel( void )                                            */
/*                                                                           */
/*   function: open channel                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#if DP_MSAC_C2

UBYTE FDL_OpenChannel( void )
{
UBYTE bRetValue;

   DP_LOCK_IND();

   if( pDpSystem->sFdl.bOpenChannel == FALSE )
   {
      pDpSystem->sFdl.bOpenChannel = TRUE;
      pDpSystem->sFdl.eRmState = RM_LOADABLE;
      FDL_SetSaSsap( DP_C2_RM_SAP_NR, SA_ALL, SAP_ALL );

      bRetValue = FDL_OK;
   } /* if( pDpSystem->sFdl.bOpenChannel == FALSE ) */
   else
   {
      bRetValue = FDL_OPEN_ERR;
   } /* else of if( pDpSystem->sFdl.bOpenChannel == FALSE ) */

   DP_UNLOCK_IND();

   return bRetValue;
} /* UBYTE FDL_OpenChannel( void ) */

#endif /* #if DP_MSAC_C2 */

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   void FDL_CloseChannel( FDL_SAP_TYPE eSapType )                          */
/*                                                                           */
/*   function: close channel                                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#if DP_MSAC_C2
void FDL_CloseChannel( FDL_SAP_TYPE eSapType )
{
FDL_SAP_HEADER_PTR  psFdlHeader;
UBYTE             bRetValue;

   DP_LOCK_IND();

   switch( eSapType )
   {
      case FDL_MSAC_C2_SAP:
      {
         if( pDpSystem->sFdl.bOpenChannel != FALSE )
         {
            /*  force rs but do not touch the pointers yet */
            pDpSystem->sFdl.asSapHeaderList[0].psToSapCntrlList->bReqSa = (UBYTE) VPC3_GET_STATION_ADDRESS();

            pDpSystem->sFdl.bOpenChannel = FALSE;

            pDpSystem->sFdl.eRmState = RM_LOCKED;

            /*  set all msac_c2-saps DISABLED */
            psFdlHeader = &pDpSystem->sFdl.asSapHeaderList[1];    /*  MSAC_C2-saps start at index #1 */

            bRetValue = pDpSystem->sC2.bC2_NumberOfSaps;

            while( bRetValue-- )
            {
               switch( psFdlHeader->eFdlState )
               {
                  case  ENABLED:
                  {
                     /* let old connections run */
                     break;
                  } /* case  ENABLED: */

                  case  DISABLED:
                  {
                     break;
                  } /* case  DISABLED: */

                  case  DISABLED_FOR_RM:
                  case  DISABLED_IN_RM:
                  {
                     psFdlHeader->eFdlState = DISABLED;
                     break;
                  } /* case  DISABLED_FOR_RM: */

                  default:
                  {
                     /* do nothing here */
                     break;
                  } /* default: */
               } /* switch( psFdlHeader->eFdlState ) */
            } /* while( bRetValue-- ) */

            FDL_PerformIndDisable( &pDpSystem->sFdl.asSapHeaderList[0] );
         } /* if( pDpSystem->sFdl.bOpenChannel != FALSE ) */
         else
         {
             DP_UNLOCK_IND();
         } /* else of if( pDpSystem->sFdl.bOpenChannel != FALSE ) */
         break;
      } /* case FDL_MSAC_C2_SAP: */

      default:
      {
          /* do nothing here */
          break;
      } /* default */
   } /* switch( eSapType ) */
} /* void FDL_CloseChannel( FDL_SAP_TYPE eSapType ) */

#endif /* #if DP_MSAC_C2 */

#endif /* #if DP_FDL */ /* compile only, if fdl-application is selected --> DpCfg.h */


/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

