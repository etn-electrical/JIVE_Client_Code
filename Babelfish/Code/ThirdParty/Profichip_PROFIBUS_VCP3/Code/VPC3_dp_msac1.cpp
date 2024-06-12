/**********************  Filename: dp_msac1.c  *******************************/
/* ========================================================================= */
/*                                                                           */
/* 0000  000   000  00000 0  000  0   0 0 0000                               */
/* 0   0 0  0 0   0 0     0 0   0 0   0 0 0   0      Einsteinstraße 6        */
/* 0   0 0  0 0   0 0     0 0     0   0 0 0   0      91074 Herzogenaurach    */
/* 0000  000  0   0 000   0 0     00000 0 0000       Germany                 */
/* 0     00   0   0 0     0 0     0   0 0 0                                  */
/* 0     0 0  0   0 0     0 0   0 0   0 0 0          Tel: ++49-9132-744-200  */
/* 0     0  0  000  0     0  000  0   0 0 0    GmbH  Fax: ++49-9132-744-204  */
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/* Function:       MSAC_C1 functions                                         */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Technical support:       Peter FREDEHORST                                 */
/*                          Tel. : ++49-9132-744-2150                        */
/*                          Fax. : ++49-9132-744-29-2150                     */
/*                          eMail: pfredehorst@profichip.com                 */
/*                          eMail: support@profichip.com                     */
/*                                                                           */
/*****************************************************************************/

/*! \file
     \brief Functions, state machine of DPV1 class1.
     
*/

/*****************************************************************************/
/* contents:

  - function prototypes
  - data structures
  - internal functions

*/
/*****************************************************************************/
/* include hierarchy */
#include "includes.h"		// Added by Sagar on 24.09.2012 for DPV1 integration
#include "VPC3_platform.h"	// Path changed for Galaxy //
#include "VPC3.h"			// Added by Sagar on 24.09.2012 for DPV1 integration
#include "VPC3_dp_inc.h"		// Path changed for Galaxy //

#if DP_MSAC_C1

/*---------------------------------------------------------------------------*/
/* function prototypes                                                       */
/*---------------------------------------------------------------------------*/
void              MSAC_C1_SetJobRunning   ( UBYTE bRunning );
void              MSAC_C1_IndDisableDone  ( UBYTE bSapNr, UBYTE bRetValue );
void              MSAC_C1_Stop            ( void );

#if DP_ALARM
   void           AL_StopAsm              ( void );
   void           AL_SetNewAlarmMode      ( UBYTE bDpv1Status2, UBYTE bDpv1Status3 );
   UBYTE          AL_AlarmEnabled         ( UBYTE bAlarmType );
   UBYTE          AL_Acls                 ( UBYTE bAlarmType );
   void           AL_InitAlarm            ( void );
   UBYTE          AL_CheckSequenceStatus  ( UBYTE bAction, UBYTE bAlarmType, UBYTE bAlarmSpecifier );
   UBYTE          AL_CheckTypeStatus      ( UBYTE bAction, UBYTE bAlarmType );
   UBYTE          AL_CheckSendDiag        ( UBYTE bAlarmType, ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCallback );
   void           AL_CheckSendAlarm       ( void );
   DPV1_RET_VAL   AL_AlarmQuit            ( UBYTE bSapNr, ALARM_ACK_REQ_PTR psAlarmAck );
#endif // #if DP_ALARM

/*---------------------------------------------------------------------------*/
/* global variables                                                          */
/*---------------------------------------------------------------------------*/
#if DP_ALARM
   // table for decoding the number of parallel alarms
   static UBYTE abAlarmDecode[8];
#endif // #if DP_ALARM

/*--------------------------------------------------------------------------*/
/* function: RedundancyStopMSAC1S                                           */
/*--------------------------------------------------------------------------*/
#ifdef REDUNDANCY
void RedundancyStopMSAC1S( void )
{
   pDpSystem->sC1.bFdlClosing = TRUE;
   // close msac_c1
   FDL_IndDisable( DP_C1_RD_WR_SAP_NR );

   MSAC_C1_SetJobRunning( FALSE );

   #if DP_ALARM
      //close ALARM SAP
      AL_StopAsm();
   #endif // #if DP_ALARM

   pDpSystem->sC1.bMSAC1Sactivate = FALSE;
}//void RedundancyStopMSAC1S( void )
#endif//#ifdef REDUNDANCY

/*--------------------------------------------------------------------------*/
/* function: RedundancyStartMSAC1S                                          */
/*--------------------------------------------------------------------------*/
#ifdef REDUNDANCY
void RedundancyStartMSAC1S( void )
{
   FDL_IndAwait( DP_C1_RD_WR_SAP_NR );
   pDpSystem->sC1.eStartState = DP_SS_RUN;

   #if DP_ALARM_OVER_SAP50
      FDL_IndAwait( DP_C1_ALARM_SAP_NR );
   #endif//#if DP_ALARM_OVER_SAP50

   pDpSystem->sC1.bDxEntered = FALSE;
   pDpSystem->sC1.bMSAC1Sactivate = TRUE;
}//void RedundancyStartMSAC1S( void )
#endif//#ifdef REDUNDANCY

/*--------------------------------------------------------------------------*/
/* function: RedundancyResetMSAC1S                                          */
/*--------------------------------------------------------------------------*/
#ifdef REDUNDANCY
void RedundancyResetMSAC1S( void )
{
   RedundancyStopMSAC1S();
   RedundancyStartMSAC1S();
}//void RedundancyResetMSAC1S( void )
#endif//#ifdef REDUNDANCY

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_Init                                                   */
/*--------------------------------------------------------------------------*/
DP_ERROR_CODE MSAC_C1_Init( void )
{
DP_ERROR_CODE bError;
UBYTE         bHelp;

   bHelp = DP_C1_LEN;
   if(    ( bHelp >= MSAC_C1_MIN_PDU_SIZE )
       && ( bHelp <= MSAC_C1_MAX_PDU      )
     )
   {
      pDpSystem->sC1.bDxEntered     = FALSE;
      pDpSystem->eDPV1              = DPV0_MODE;
      pDpSystem->sC1.eStartState    = DP_SS_IDLE;
      pDpSystem->sC1.bFdlClosing    = TRUE;
      pDpSystem->sC1.bActiveJob     = FALSE;
      pDpSystem->sC1.bC1Service     = 0x00;

      #if DP_ALARM
         AL_InitAlarm();
      #endif // #if DP_ALARM

      #ifdef REDUNDANCY
         pDpSystem->sC1.bMSAC1Sactivate = TRUE;
      #endif//#ifdef REDUNDANCY

      bError = DP_OK;
   }//if(    ( bHelp >= MSAC_C1_MIN_PDU_SIZE ) ...
   else
   {
      bError = C1_DATA_LEN_ERROR;
   }//else of if(    ( bHelp >= MSAC_C1_MIN_PDU_SIZE ) ...

   return bError;
}//DP_ERROR_CODE MSAC_C1_Init( void )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_PduReceived                                           */
/*--------------------------------------------------------------------------*/
void MSAC_C1_PduReceived( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
   DP_LOCK_IND();

   switch( bSapNr )
   {
      case DP_C1_RD_WR_SAP_NR:
      {
         pDpSystem->sC1.psMsgHeader = psMsgHeader;
         pDpSystem->sC1.pToDpv1Data = pToDpv1Data;

         switch( psMsgHeader->bDpv1Service )
         {
            case DPV1_FC_READ:
            {
               if( ( psMsgHeader->bDataLength > 0 ) && ( psMsgHeader->bDataLength <= 240 ) && ( psMsgHeader->bIndicationLength == 4 ) )
               {
                  pDpSystem->sC1.bC1Service |= MSAC_C1_IND_DS_READ;
               }//if( ( psMsgHeader->bDataLength > 0 ) && ( psMsgHeader->bDataLength <= 240 ) && ( psMsgHeader->bIndicationLength == 4 ) )
               else
               {
                  psMsgHeader->bIndicationLength =  DPV1_LEN_NEG_RESPONSE;
                  psMsgHeader->bFunctionCode     =  FC_RESP_L;

                  psMsgHeader->bDpv1Service      |= DPV1_ERROR_BIT_RESPONSE;

                  psMsgHeader->bSlotNr     = DPV1_ERRDC_DPV1;                             //DPV1 Error_Decode
                  psMsgHeader->bIndex      = DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_TYPE;     //DPV1 Error_Code_1
                  psMsgHeader->bDataLength = 0x00;                                        //DPV1 Error_Code_2

                  FDL_RespProvide( bSapNr, FDL_PRIMARY_BUF );
               }//else of if( ( psMsgHeader->bDataLength > 0 ) && ( psMsgHeader->bDataLength <= 240 ) && ( psMsgHeader->bIndicationLength == 4 ) )
               break;
            }//case DPV1_FC_READ:

            case DPV1_FC_WRITE:
            {
               if( psMsgHeader->bDataLength > 0 )
               {
                  pDpSystem->sC1.bC1Service |= MSAC_C1_IND_DS_WRITE;
               }//if( psMsgHeader->bDataLength > 0 )
               else
               {
                  psMsgHeader->bIndicationLength =  DPV1_LEN_NEG_RESPONSE;
                  psMsgHeader->bFunctionCode     =  FC_RESP_L;

                  psMsgHeader->bDpv1Service      |= DPV1_ERROR_BIT_RESPONSE;

                  psMsgHeader->bSlotNr     = DPV1_ERRDC_DPV1;                             //DPV1 Error_Decode
                  psMsgHeader->bIndex      = DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_TYPE;     //DPV1 Error_Code_1
                  psMsgHeader->bDataLength = 0x00;                                        //DPV1 Error_Code_2

                  FDL_RespProvide( bSapNr, FDL_PRIMARY_BUF );
               }//else of if( psMsgHeader->bDataLength > 0 )
               break;
            }//case DPV1_FC_WRITE:

            #if DP_ALARM

               case DPV1_FC_ALARM_ACK:
               {
                  // check, if Alarm functions enabled
                  if( pDpSystem->sAl.bState == AL_STATE_OPEN )
                  {
                     pDpSystem->sC1.bC1Service |= MSAC_C1_IND_ALARM_QUIT;
                  }//if( pDpSystem->sAl.bState == AL_STATE_OPEN )
                  else
                  {
                     VPC3_SET_USER_LEAVE_MASTER();
                  }//else of if( pDpSystem->sAl.bState == AL_STATE_OPEN )
                  break;
               }//case DPV1_FC_ALARM_ACK:

            #endif

            default:
            {
               VPC3_SET_USER_LEAVE_MASTER();
               break;
            }//default:
         }//switch( psMsgHeader->bDpv1Service )

         break;
      }//case DP_C1_RD_WR_SAP_NR

      #if DP_ALARM

         case DP_C1_ALARM_SAP_NR:
         {
            if( psMsgHeader->bDpv1Service == DPV1_FC_ALARM_ACK )
            {
               //check, if Alarm functions enabled
               if( pDpSystem->sAl.bState == AL_STATE_OPEN )
               {
                  pDpSystem->sC1.psMsgHeaderSAP50 = psMsgHeader;
                  pDpSystem->sC1.pToDpv1DataSAP50 = pToDpv1Data;

                  pDpSystem->sC1.bC1Service |= MSAC_C1_IND_ALARM_QUIT_SAP50;
               }//if( pDpSystem->sAl.bState == AL_STATE_OPEN )
               else
               {
                  VPC3_SET_USER_LEAVE_MASTER();
               }//else of if( pDpSystem->sAl.bState == AL_STATE_OPEN )
            }//if( psMsgHeader->bDpv1Service == DPV1_FC_ALARM_ACK )
            else
            {
               VPC3_SET_USER_LEAVE_MASTER();
            }//else of if( psMsgHeader->bDpv1Service == DPV1_FC_ALARM_ACK )
            break;
         }//case DP_C1_ALARM_SAP_NR:

      #endif//#if DP_ALARM

      default:
      {
         VPC3_SET_USER_LEAVE_MASTER();
         break;
      }//default:
   }//switch( bSapNr )

   DP_UNLOCK_IND();
}//void MSAC_C1_PduReceived( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_Process                                                */
/*--------------------------------------------------------------------------*/
/*!
  \brief The application program has to call this function cyclically so that the MSAC_C1 services can be processed.
*/
void MSAC_C1_Process( void )
{
MSG_HEADER_PTR psMsgHeader;

   if( pDpSystem->eDPV1 == DPV1_MODE )
   {
      if( pDpSystem->sC1.bC1Service & MSAC_C1_IND_DS_READ )
      {
         psMsgHeader = pDpSystem->sC1.psMsgHeader;

         MSAC_C1_SetJobRunning( TRUE );

         switch( UserDpv1ReadReq( DP_C1_RD_WR_SAP_NR, psMsgHeader, pDpSystem->sC1.pToDpv1Data ) )
         {
            case DPV1_OK:
            {
               psMsgHeader->bIndicationLength =  DPV1_LEN_HEAD_DATA + psMsgHeader->bDataLength;
               psMsgHeader->bFunctionCode     =  FC_RESP_L;
               FDL_RespProvide( DP_C1_RD_WR_SAP_NR, FDL_PRIMARY_BUF );
               MSAC_C1_SetJobRunning( FALSE );

               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_DS_READ;
               break;
            }//case DPV1_OK:

            case DPV1_NOK:
            {
               psMsgHeader->bIndicationLength =  DPV1_LEN_NEG_RESPONSE;
               psMsgHeader->bFunctionCode     =  FC_RESP_L;
               psMsgHeader->bDpv1Service      |= DPV1_ERROR_BIT_RESPONSE;
               psMsgHeader->bSlotNr           = DPV1_ERRDC_DPV1;           //DPV1 Error_Decode
               FDL_RespProvide( DP_C1_RD_WR_SAP_NR, FDL_PRIMARY_BUF );
               MSAC_C1_SetJobRunning( FALSE );

               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_DS_READ;
               break;
            }//case DPV1_NOK:

            case DPV1_DELAY:
            {
               //user give pdu later back
               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_DS_READ;
               break;
            }//case DPV1_DELAY:

            default:
            {
               sVpc3Error.bErrorCode = pDpSystem->sC1.bC1Service;
               sVpc3Error.bCnId      = 0;
               // *** no further action of dpv1/c2 ***
               DpAppl_FatalError(_DP_C1, __LINE__, &sVpc3Error);
               break;
            }//default:
         }//switch( UserDpv1ReadReq( DP_C1_RD_WR_SAP_NR, psMsgHeader, pDpSystem->sC1.pToDpv1DataSAP50 ) )
      }//if( pDpSystem->sC1.bC1Service & MSAC_C1_IND_DS_READ )

      if( pDpSystem->sC1.bC1Service & MSAC_C1_IND_DS_WRITE )
      {
         psMsgHeader = pDpSystem->sC1.psMsgHeader;

         MSAC_C1_SetJobRunning( TRUE );

         switch( UserDpv1WriteReq( DP_C1_RD_WR_SAP_NR, psMsgHeader, pDpSystem->sC1.pToDpv1Data ) )
         {
            case DPV1_OK:
            {
               psMsgHeader->bIndicationLength =  DPV1_LEN_HEAD_DATA;
               psMsgHeader->bFunctionCode     =  FC_RESP_L;
               FDL_RespProvide( DP_C1_RD_WR_SAP_NR, FDL_PRIMARY_BUF );
               MSAC_C1_SetJobRunning( FALSE );

               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_DS_WRITE;
               break;
            }//case DPV1_OK:

            case DPV1_NOK:
            {
               psMsgHeader->bIndicationLength =  DPV1_LEN_NEG_RESPONSE;
               psMsgHeader->bFunctionCode     =  FC_RESP_L;
               psMsgHeader->bDpv1Service      |= DPV1_ERROR_BIT_RESPONSE;
               psMsgHeader->bSlotNr           = DPV1_ERRDC_DPV1;           //DPV1 Error_Decode
               FDL_RespProvide( DP_C1_RD_WR_SAP_NR, FDL_PRIMARY_BUF );
               MSAC_C1_SetJobRunning( FALSE );

               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_DS_WRITE;
               break;
            }//case DPV1_NOK:

            case DPV1_DELAY:
            {
               //user give pdu later back
               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_DS_WRITE;
               break;
            }//case DPV1_DELAY:

            default:
            {
               sVpc3Error.bErrorCode = pDpSystem->sC1.bC1Service;
               sVpc3Error.bCnId      = 0;
               // *** no further action of dpv1/c2 ***
               DpAppl_FatalError(_DP_C1, __LINE__, &sVpc3Error);
               break;
            }//default:
         }//switch( UserDpv1WriteReq( DP_C1_RD_WR_SAP_NR, psMsgHeader, pDpSystem->sC1.pToDpv1DataSAP50 ) )
      }//if( pDpSystem->sC1.bC1Service & MSAC_C1_IND_DS_WRITE )
   }//if( pDpSystem->eDPV1 == DPV1_MODE )
}//void MSAC_C1_Process( void )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* UBYTE MSAC_C1_TransmitDelay( UBYTE bSapNr, UBYTE bDpv1Service,            */
/*                               DPV1_RET_VAL bDpv1RetValue )                */
/*                                                                           */
/*   function: msac_c1_transmit_delay receives response data from the user.  */
/*             In this function the response data are checked. In dependence */
/*             of the pdu-opcode und the status the length of the data and   */
/*             the function-code is manipulated. Then the dpv1_msc1 transmit */
/*             function is called.                                           */
/*                                                                           */
/*   return codes: DPV1_OK            ( transmit function is called )        */
/*                 DPV1_ERR_NO_JOB    ( no active job               )        */
/*                 DPV1_ERR_INV_PARAM ( invalid parameter           )        */
/*                 DPV1_NO_VALID_SAP  ( invalid sap                 )        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
UBYTE MSAC_C1_TransmitDelay( UBYTE bSapNr, UBYTE bDpv1Service, DPV1_RET_VAL bDpv1RetValue )
{
MSG_HEADER_PTR psMsgHeader;
UBYTE          bRetValue;

   bRetValue = DPV1_OK;

   if( bSapNr == DP_C1_RD_WR_SAP_NR )
   {
      if( pDpSystem->sC1.bActiveJob )
      {
         psMsgHeader = pDpSystem->sC1.psMsgHeader;

         switch( bDpv1RetValue )
         {
            case DPV1_OK:
            {
               if( bDpv1Service == MSAC_C1_IND_DS_WRITE )
               {
                  psMsgHeader->bIndicationLength =  DPV1_LEN_HEAD_DATA;
                  psMsgHeader->bFunctionCode     =  FC_RESP_L;
                  FDL_RespProvide( DP_C1_RD_WR_SAP_NR, FDL_PRIMARY_BUF );
                  MSAC_C1_SetJobRunning( FALSE );
               }//if( bDpv1Service == MSAC_C1_IND_DS_WRITE )
               else
               {
                  psMsgHeader->bIndicationLength =  DPV1_LEN_HEAD_DATA + psMsgHeader->bDataLength;
                  psMsgHeader->bFunctionCode     =  FC_RESP_L;
                  FDL_RespProvide( DP_C1_RD_WR_SAP_NR, FDL_PRIMARY_BUF );
                  MSAC_C1_SetJobRunning( FALSE );
               }//else of if( bDpv1Service == MSAC_C1_IND_DS_WRITE )
               break;
            }//case DPV1_OK:

            case DPV1_NOK:
            {
               psMsgHeader->bIndicationLength =  DPV1_LEN_NEG_RESPONSE;
               psMsgHeader->bFunctionCode     =  FC_RESP_L;
               psMsgHeader->bDpv1Service      |= DPV1_ERROR_BIT_RESPONSE;
               psMsgHeader->bSlotNr           = DPV1_ERRDC_DPV1;           //DPV1 Error_Decode
               FDL_RespProvide( DP_C1_RD_WR_SAP_NR, FDL_PRIMARY_BUF );
               MSAC_C1_SetJobRunning( FALSE );
               break;
            }//case DPV1_NOK:

            default:
            {
               bRetValue = DPV1_ERR_INV_PARAM;
               break;
            }//default:
         }//switch( bDpv1RetValue )
      }//if( pDpSystem->sC1.active_Job )
      else
      {
         bRetValue = DPV1_ERR_NO_JOB;
      }//else of if( pDpSystem->sC1.bActiveJob )
   }//if( bSapNr == DP_C1_RD_WR_SAP_NR )
   else
   {
      //dummyline for compiler because warning
      bRetValue = DPV1_NO_VALID_SAP;
   }//else of if( bSapNr == DP_C1_RD_WR_SAP_NR )

   return bRetValue;
}//UBYTE MSAC_C1_TransmitDelay( UBYTE bSapNr, UBYTE bDpv1Service, DPV1_RET_VAL bDpv1RetValue )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_SetJobRunning                                        */
/*--------------------------------------------------------------------------*/
void MSAC_C1_SetJobRunning( UBYTE bRunning )
{
   if( bRunning )
   {
      pDpSystem->sC1.bActiveJob = TRUE;
   }//if( bRunning )
   else
   {
      pDpSystem->sC1.bActiveJob = FALSE;

      if( ( pDpSystem->sC1.eStartState == DP_SS_STOP) && (pDpSystem->sC1.bFdlClosing == FALSE ) )
      {
         pDpSystem->sC1.eStartState = DP_SS_IDLE;
      }//if( ( pDpSystem->sC1.eStartState == DP_SS_STOP) && (pDpSystem->sC1.bFdlClosing == FALSE ) )

      if( pDpSystem->sC1.eStartState == DP_SS_START_AGAIN )
      {
         // open C1-SAPs
         FDL_IndAwait(DP_C1_RD_WR_SAP_NR);
         pDpSystem->sC1.eStartState = DP_SS_RUN;
      }//if( pDpSystem->sC1.eStartState == DP_SS_START_AGAIN )
   }//else of if( bRunning )
}//void MSAC_C1_SetJobRunning( UBYTE bRunning )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_IndDisableDone                                       */
/*--------------------------------------------------------------------------*/
void MSAC_C1_IndDisableDone( UBYTE bSapNr, UBYTE bRetValue )
{
   //V504
   if( bRetValue == FDL_IUSE )
   {
      FDL_PerformSapReset( bSapNr );
   }//if( bRetValue == FDL_IUSE )

   if( bSapNr == DP_C1_RD_WR_SAP_NR )
   {
      pDpSystem->sC1.bFdlClosing = FALSE;
      pDpSystem->sC1.eStartState = ( pDpSystem->sC1.bActiveJob ) ? DP_SS_STOP : DP_SS_IDLE;
   }//if( bSapNr == DP_C1_RD_WR_SAP_NR )
}//void MSAC_C1_IndDisableDone( UBYTE bSapNr, UBYTE bRetValue )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_CheckIndNewPrmData                                     */
/*--------------------------------------------------------------------------*/
UBYTE MSAC_C1_CheckIndNewPrmData( MEM_STRUC_PRM_PTR psPrmData, UBYTE bPrmLength )
{
UBYTE bRetValue;

   bRetValue = TRUE;

   if( bPrmLength >= PRM_LEN_DPV1 )
   {
      pDpSystem->eDPV1 = ( psPrmData->bDpv1Status1 & DPV1_STATUS_1_DPV1_ENABLE )? DPV1_MODE : DPV0_MODE;
   }//if( bPrmLength >= PRM_LEN_DPV1 )
   else
   {
      pDpSystem->eDPV1 = DPV0_MODE;
   }//else of if( bPrmLength >= PRM_LEN_DPV1 )

   switch( VPC3_GET_DP_STATE() )
   {
      case WAIT_PRM:
      {
         break;
      }//case WAIT_PRM:

      case WAIT_CFG:
      {
         if( pDpSystem->sC1.eStartState == DP_SS_RUN )
         {
            MSAC_C1_Stop();
         }//if( pDpSystem->sC1.eStartState == DP_SS_RUN )
         break;
      }//case WAIT_CFG:

      case DATA_EX:
      {
         MSAC_C1_Stop();
         bRetValue = FALSE;
         break;
      }//case DATA_EX:
   }//switch( VPC3_GET_DP_STATE() )

   #if DP_ALARM
      AL_SetNewAlarmMode( psPrmData->bDpv1Status2, psPrmData->bDpv1Status3 );
   #endif // #if DP_ALARM

   return bRetValue;
}//UBYTE MSAC_C1_CheckIndNewPrmData( MEM_STRUC_PRM_PTR psPrmData, UBYTE bPrmLength )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_DoCfgOk                                                */
/*--------------------------------------------------------------------------*/
void MSAC_C1_DoCfgOk( void )
{
   #ifdef REDUNDANCY
      if( !pDpSystem->sC1.bMSAC1Sactivate )
      {
         return;
      }//if( !pDpSystem->sC1.bMSAC1Sactivate )
   #endif//#ifdef REDUNDANCY

   if( VPC3_GET_DP_STATE() == WAIT_CFG )
   {
      if( pDpSystem->eDPV1 == DPV1_MODE )
      {
         if( pDpSystem->sC1.eStartState == DP_SS_IDLE )
         {
            FDL_IndAwait( DP_C1_RD_WR_SAP_NR );
            pDpSystem->sC1.eStartState = DP_SS_RUN;

            #if DP_ALARM_OVER_SAP50
               FDL_IndAwait( DP_C1_ALARM_SAP_NR );
            #endif//#if DP_ALARM_OVER_SAP50
         }//if( pDpSystem->sC1.eStartState == DP_SS_IDLE )

         if( pDpSystem->sC1.eStartState == DP_SS_STOP )
         {
            pDpSystem->sC1.eStartState = DP_SS_START_AGAIN;
         }//if( pDpSystem->sC1.eStartState == DP_SS_STOP )
      }//if( pDpSystem->eDPV1 == DPV1_MODE )

      pDpSystem->sC1.bDxEntered = FALSE;
   }//if( VPC3_GET_DP_STATE() == WAIT_CFG )
}//void MSAC_C1_DoCfgOk( void )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_DoCfgNotOk                                          */
/*--------------------------------------------------------------------------*/
void MSAC_C1_DoCfgNotOk( void )
{
   switch( VPC3_GET_DP_STATE() )
   {
      case WAIT_CFG:
      case DATA_EX:
      {
         MSAC_C1_Stop();
         break;
      }//case DATA_EX:

      case WAIT_PRM:
      default:
      {
         //do nothing
         break;
      }//case WAIT_PRM:
   }//switch( VPC3_GET_DP_STATE() )
}//void MSAC_C1_DoCfgNotOk( void )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_CheckIndDxOut                                       */
/*--------------------------------------------------------------------------*/
void MSAC_C1_CheckIndDxOut( void )
{
   #ifdef REDUNDANCY
      if( !pDpSystem->sC1.bMSAC1Sactivate )
      {
         return;
      }//if( !pDpSystem->sC1.bMSAC1Sactivate )
   #endif//#ifdef REDUNDANCY

   if(    ( pDpSystem->sC1.bDxEntered     == FALSE        )
       && ( pDpSystem->eDPV1 == DPV1_MODE )
     )
   {
      if( VPC3_GET_DP_STATE() == DATA_EX )
      {   
         pDpSystem->sC1.bDxEntered = TRUE;
         
         #if DP_ALARM
         
            if( pDpSystem->sAl.bState == AL_STATE_CLOSED )
            {
               if( pDpSystem->sAl.bEnabled > 0x00 )
               {
                  pDpSystem->sAl.bCount    = 0;
                  pDpSystem->sAl.bLimit    = abAlarmDecode[ pDpSystem->sAl.bMode ];
                  pDpSystem->sAl.bSequence = ( pDpSystem->sAl.bMode == SEQC_MODE_TOTAL_00 ) ? FALSE : TRUE;
                  pDpSystem->sAl.bState    = AL_STATE_OPEN;
               }//if( pDpSystem->sAl.bEnabled > 0x00 )
               else
               {
                  pDpSystem->sAl.bEnabled = 0;
                  pDpSystem->sAl.bMode    = 0;
               }//else of if( pDpSystem->sAl.bEnabled > 0x00 )
            }//if(  pDpSystem->sAl.bState == AL_STATE_CLOSED  )
         
         #endif//#if DP_ALARM
      }//if( VPC3_GET_DP_STATE() == DATA_EX )      
   }//if(    ( pDpSystem->sC1.bDxEntered     == FALSE        ) ...
}//void MSAC_C1_CheckIndDxOut(void)

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_LeaveDx                                               */
/*--------------------------------------------------------------------------*/
void MSAC_C1_LeaveDx( void )
{
   if( VPC3_GET_DP_STATE() != DATA_EX )
   {
      #ifdef REDUNDANCY
      UBYTE bTemp;

         bTemp = pDpSystem->sC1.bMSAC1Sactivate;
      #endif//#ifdef REDUNDANCY

      MSAC_C1_Stop();

      #ifdef REDUNDANCY
         pDpSystem->sC1.bMSAC1Sactivate = bTemp;
      #endif//#ifdef REDUNDANCY

      //V505
      MSAC_C1_SetJobRunning( FALSE );

      if( ( VPC3_GetMasterAddress() != 0xFF ) && ( VPC3_GET_DP_STATE() == WAIT_PRM ) )
      {
         VPC3_GoOffline();
         do
         {
            //wait, for offline
         }while( VPC3_GET_OFF_PASS() );

         VPC3_Start();
      }//if( ( VPC3_GetMasterAddress() != 0xFF ) && ( VPC3_GET_DP_STATE() == WAIT_PRM ) )
   }//if( VPC3_GET_DP_STATE() != DATA_EX )
}//void MSAC_C1_LeaveDx( void )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C1_Stop                                                   */
/*--------------------------------------------------------------------------*/
void MSAC_C1_Stop( void )
{
   #ifdef REDUNDANCY
      //pDpSystem->sC1.bMSAC1Sactivate = FALSE;
   #endif//#ifdef REDUNDANCY

   pDpSystem->sC1.bC1Service = 0x00;

   if( pDpSystem->sC1.eStartState == DP_SS_RUN )
   {
      pDpSystem->sC1.bFdlClosing = TRUE;

      // close msac_c1
      FDL_IndDisable( DP_C1_RD_WR_SAP_NR );

      #if DP_ALARM
         //close ALARM SAP
         AL_StopAsm();
      #endif // #if DP_ALARM
   }//if( pDpSystem->sC1.eStartState == DP_SS_RUN )
}//void MSAC_C1_Stop( void )


#if DP_ALARM

/*--------------------------------------------------------------------------*/
/* function: AL_InitAlarm                                                   */
/*--------------------------------------------------------------------------*/
void AL_InitAlarm( void )
{
   pDpSystem->sAl.bState = AL_STATE_CLOSED;

   UserAlarmInit();

   dpl_init_list__( &pDpSystem->sAl.dplAlarmQueue );
   dpl_init_list__( &pDpSystem->sAl.dplAlarmAckQueue );

   pDpSystem->sAl.bEnabled = 0;
   pDpSystem->sAl.bMode    = 0;

   pDpSystem->sAl.bTypeStatus = 0;
   memset( &pDpSystem->sAl.abSequenceStatus[0], 0, AL_SEQUENCE_STATUS_SIZE );

   pDpSystem->sAl.bSequence = 0;
   pDpSystem->sAl.bLimit    = 0;
   pDpSystem->sAl.bCount    = 0;

   // init coding table
   abAlarmDecode[0] = SEQC_MODE_OFF     ;   // no sequence mode - 01 alarm  of every type
   abAlarmDecode[1] = SEQC_MODE_TOTAL_02;   // sequence mode - 02 alarms in total
   abAlarmDecode[2] = SEQC_MODE_TOTAL_04;   // sequence mode - 04 alarms in total
   abAlarmDecode[3] = SEQC_MODE_TOTAL_08;   // sequence mode - 08 alarms in total
   abAlarmDecode[4] = SEQC_MODE_TOTAL_12;   // sequence mode - 12 alarms in total
   abAlarmDecode[5] = SEQC_MODE_TOTAL_16;   // sequence mode - 16 alarms in total
   abAlarmDecode[6] = SEQC_MODE_TOTAL_24;   // sequence mode - 24 alarms in total
   abAlarmDecode[7] = SEQC_MODE_TOTAL_32;   // sequence mode - 32 alarms in total
}//void AL_InitAlarm( void )

/*--------------------------------------------------------------------------*/
/* function: AL_SetNewAlarmMode                                             */
/*--------------------------------------------------------------------------*/
void AL_SetNewAlarmMode( UBYTE bDpv1Status2, UBYTE bDpv1Status3 )
{
   pDpSystem->sAl.bEnabled = 0;
   pDpSystem->sAl.bMode    = 0;

   if( pDpSystem->eDPV1 == DPV1_MODE )
   {
      pDpSystem->sAl.bEnabled = ( bDpv1Status2 & DPV1_STATUS_2_ALARM_TYPE_MASK );
      pDpSystem->sAl.bMode    = ( bDpv1Status3 & DPV1_STATUS_3_ALARM_MODE_MASK );
   }//if( pDpSystem->eDPV1 == DPV1_MODE )
   else
   {
      pDpSystem->sAl.bEnabled = 0x00;
      pDpSystem->sAl.bMode    = 0x00;
   }//else of if( pDpSystem->eDPV1 == DPV1_MODE )
}//void AL_SetNewAlarmMode( UBYTE bDpv1Status2, UBYTE bDpv1Status3 )

/*--------------------------------------------------------------------------*/
/* function: AL_AlarmEnabled                                                */
/*--------------------------------------------------------------------------*/
UBYTE AL_AlarmEnabled( UBYTE bAlarmType )
{
UBYTE bRetValue;

   bRetValue = FALSE;
      
   if(    (( bAlarmType == ALARM_TYPE_PULL
       ||    bAlarmType == ALARM_TYPE_PLUG)      && ( pDpSystem->sAl.bEnabled & ALARM_TYPE_PULLPLUG_VALUE     ) )
       ||  ( bAlarmType == ALARM_TYPE_PROCESS    && ( pDpSystem->sAl.bEnabled & ALARM_TYPE_PROCESS_VALUE      ) )
       ||  ( bAlarmType == ALARM_TYPE_DIAGNOSTIC && ( pDpSystem->sAl.bEnabled & ALARM_TYPE_DIAGNOSTIC_VALUE   ) )
       ||  ( bAlarmType == ALARM_TYPE_STATUS     && ( pDpSystem->sAl.bEnabled & ALARM_TYPE_STATUS_VALUE       ) )
       ||  ( bAlarmType == ALARM_TYPE_UPDATE     && ( pDpSystem->sAl.bEnabled & ALARM_TYPE_UPDATE_VALUE       ) )
       || (( bAlarmType >= ALARM_TYPE_MANU_MIN
       &&    bAlarmType <= ALARM_TYPE_MANU_MAX)  && ( pDpSystem->sAl.bEnabled & ALARM_TYPE_MANUFACTURER_VALUE ) )
     )
   {
      bRetValue = TRUE;
   }//if(    (( bAlarmType == ALARM_TYPE_PULL ...
   
   return bRetValue;
}//UBYTE AL_AlarmEnabled( UBYTE bAlarmType )

/*--------------------------------------------------------------------------*/
/* function: AL_AlarmProcess                                                */
/*--------------------------------------------------------------------------*/
void AL_AlarmProcess( void )
{
MSG_HEADER_PTR psMsgHeader;

   // check, if Alarm functions enabled
   if( pDpSystem->sAl.bState == AL_STATE_OPEN )
   {
      AL_CheckSendAlarm();

      if( pDpSystem->sC1.bC1Service & MSAC_C1_IND_ALARM_QUIT )
      {
         psMsgHeader = pDpSystem->sC1.psMsgHeader;

         MSAC_C1_SetJobRunning( TRUE );

         switch( AL_AlarmQuit( DP_C1_RD_WR_SAP_NR, (ALARM_ACK_REQ_PTR)&psMsgHeader->bDpv1Service ) )
         {
            case DPV1_OK:
            {
               psMsgHeader->bIndicationLength =  DPV1_LEN_HEAD_DATA;
               psMsgHeader->bFunctionCode     =  FC_RESP_L;
               FDL_RespProvide( DP_C1_RD_WR_SAP_NR, FDL_PRIMARY_BUF );
               MSAC_C1_SetJobRunning( FALSE );

               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_ALARM_QUIT;
               break;
            }//case DPV1_OK:

            case DPV1_NOK:
            {
               MSAC_C1_SetJobRunning( FALSE );
               pDpSystem->sAl.bState = AL_STATE_CLOSED;
               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_ALARM_QUIT;

               VPC3_SET_USER_LEAVE_MASTER();
               break;
            }//case DPV1_NOK:

            case DPV1_DELAY:
            {
               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_ALARM_QUIT;
               break;
            }//case DPV1_DELAY:

            default:
            {
               break;
            }//default:
         }//switch( AL_AlarmQuit( DP_C1_RD_WR_SAP_NR, (ALARM_ACK_REQ_PTR)&psMsgHeader->bDpv1Service ) )
      }//if( pDpSystem->sC1.bC1Service & MSAC_C1_IND_ALARM_QUIT )

      if( pDpSystem->sC1.bC1Service & MSAC_C1_IND_ALARM_QUIT_SAP50 )
      {
         psMsgHeader = pDpSystem->sC1.psMsgHeaderSAP50;

         MSAC_C1_SetJobRunning( TRUE );

         switch( AL_AlarmQuit( DP_C1_ALARM_SAP_NR, (ALARM_ACK_REQ_PTR)&psMsgHeader->bDpv1Service ) )
         {
            case DPV1_OK:
            {
               psMsgHeader->bIndicationLength =  DPV1_LEN_HEAD_DATA;
               psMsgHeader->bFunctionCode     =  FC_RESP_L;
               FDL_RespProvide( DP_C1_ALARM_SAP_NR, FDL_PRIMARY_BUF );
               MSAC_C1_SetJobRunning( FALSE );

               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_ALARM_QUIT_SAP50;
               break;
            }//case DPV1_OK:

            case DPV1_NOK:
            {
               MSAC_C1_SetJobRunning( FALSE );
               pDpSystem->sAl.bState = AL_STATE_CLOSED;
               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_ALARM_QUIT_SAP50;

               VPC3_SET_USER_LEAVE_MASTER();
               break;
            }//case DPV1_NOK:

            case DPV1_DELAY:
            {
               pDpSystem->sC1.bC1Service &= ~MSAC_C1_IND_ALARM_QUIT_SAP50;
               break;
            }//case DPV1_DELAY:

            default:
            {
               break;
            }//default:
         }//switch( AL_AlarmQuit( DP_C1_ALARM_SAP_NR, (ALARM_ACK_REQ_PTR)&psMsgHeader->bDpv1Service ) )
      }//if( pDpSystem->sC1.bC1Service & MSAC_C1_IND_ALARM_QUIT_SAP50 )
   }//if( pDpSystem->sAl.bState == AL_STATE_OPEN )
}//void AL_AlarmProcess( void )

/*--------------------------------------------------------------------------*/
/* function: AL_StopAsm                                                     */
/*--------------------------------------------------------------------------*/
void AL_StopAsm( void )
{
   #ifdef REDUNDANCY
   #else
      //clear alarm message in diagnostic buffer
      UserResetDiagnosticBuffer();
   #endif//#ifdef REDUNDANCY

   #if DP_ALARM_OVER_SAP50
      FDL_IndDisable( DP_C1_ALARM_SAP_NR );
   #endif//#if DP_ALARM_OVER_SAP50

   AL_InitAlarm();
}//void AL_StopAsm( void )

/*--------------------------------------------------------------------------*/
/* function: AL_Acls                                                        */
/*--------------------------------------------------------------------------*/
UBYTE AL_Acls( UBYTE bAlarmType )
{
   //BugFix 503
   return ( ( ALARM_TYPE_UPDATE >= bAlarmType ) ? ( bAlarmType - 1 ) : ( ALARM_TYPE_MAX - 1 ) );
}//UBYTE AL_Acls( UBYTE bAlarmType )

/*--------------------------------------------------------------------------*/
/* function:  AL_SetAlarm                                                   */
/*--------------------------------------------------------------------------*/
UBYTE AL_SetAlarm( ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCallback )
{
UBYTE bSeqNr;

   bSeqNr = ( ( psAlarm->bSpecifier & SEQ_NR_MASK ) >> SPEC_SEQ_START );

   if( pDpSystem->sAl.bState == AL_STATE_CLOSED )
      return SET_ALARM_AL_STATE_CLOSED;

   if( AL_AlarmEnabled( psAlarm->bAlarmType ) == FALSE )
      return SET_ALARM_ALARMTYPE_NOTSUPP;

   if( bSeqNr > MAX_SEQ_NR )
      return SET_ALARM_SEQ_NR_ERROR;

   if( ( psAlarm->bSpecifier & SPEC_MASK ) > 0x03 )
      return SET_ALARM_SPECIFIER_ERROR;

   psAlarm->bCallback = bCallback;
   dpl_put_blk_to_list_end__( &pDpSystem->sAl.dplAlarmQueue, &psAlarm->dplListHead );

   return SET_ALARM_OK;
}//UBYTE AL_SetAlarm( ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCallback )

/*--------------------------------------------------------------------------*/
/* function:  AL_CheckSendAlarm                                             */
/*--------------------------------------------------------------------------*/
void AL_CheckSendAlarm( void )
{
ALARM_UNION_ALARM sTempAnchor;
ALARM_UNION_ALARM sTempAlarm;
ALARM_UNION_ALARM sTempAlarmNext;
UBYTE             bTmpSpecifier;
UBYTE             bTmpCallback;
UBYTE             bTmpAlarmType;
//UBYTE             bAlarmFound;  // Commented for Galaxy //

   sTempAnchor.sListPtr = &pDpSystem->sAl.dplAlarmQueue;
   sTempAlarm .sListPtr =  pDpSystem->sAl.dplAlarmQueue.NextBlkPtr;

   bTmpSpecifier = 0;
   bTmpAlarmType = 0;
//   bAlarmFound   = 0; // Commented for Galaxy //

   while( (DPL_HOST_PTR_COMPARE_TYPE) sTempAlarm.sListPtr != (DPL_HOST_PTR_COMPARE_TYPE) sTempAnchor.sListPtr )
   {
      sTempAlarmNext.sListPtr = sTempAlarm.sListPtr->NextBlkPtr;

      bTmpAlarmType = sTempAlarm.psAlarm->bAlarmType;
      bTmpSpecifier = sTempAlarm.psAlarm->bSpecifier;
      bTmpCallback  = sTempAlarm.psAlarm->bCallback;

      if( pDpSystem->sAl.bSequence == FALSE )
      {
         //type mode
         //only one alarm of a specific ALARM_TYPE can be active at one time
         if( AL_CheckTypeStatus( AL_ALARM_STATUS_ACTION_CHECK, bTmpAlarmType ) == FALSE )
         {
            if( AL_CheckSendDiag( bTmpAlarmType, sTempAlarm.psAlarm, bTmpCallback ) == DP_OK )
            {
               AL_CheckTypeStatus( AL_ALARM_STATUS_ACTION_SET, bTmpAlarmType );
               AL_CheckSequenceStatus( AL_ALARM_STATUS_ACTION_SET, bTmpAlarmType, bTmpSpecifier );

               dpl_remove_blk__( sTempAlarm.sListPtr );

               dpl_put_blk_to_list_end__( &pDpSystem->sAl.dplAlarmAckQueue, sTempAlarm.sListPtr );
            }//if( AL_CheckSendDiag( bTmpAlarmType, sTempAlarm.psAlarm, bTmpCallback ) == DP_OK )
            break;
         }//if( AL_CheckTypeStatus( AL_ALARM_STATUS_ACTION_CHECK, bTmpAlarmType ) == FALSE )
      }//if( alarm_sequence == FALSE )
      else
      {
         //bSequence mode
         //several alarms (2 to 32) of the same or different ALARM_TYPE can be active at one time
         if( pDpSystem->sAl.bSequence == TRUE && pDpSystem->sAl.bCount < pDpSystem->sAl.bLimit )
         {
            AL_CheckSequenceStatus( AL_ALARM_STATUS_ACTION_SET, bTmpAlarmType, bTmpSpecifier );

            dpl_remove_blk__( sTempAlarm.sListPtr );

            dpl_put_blk_to_list_end__( &pDpSystem->sAl.dplAlarmAckQueue, sTempAlarm.sListPtr );

            pDpSystem->sAl.bCount++;
         }//if( pDpSystem->sAl.bSequence == TRUE && pDpSystem->sAl.bCount < pDpSystem->sAl.bLimit )
         else
         {
            //don't send alarm, because limit is reached
            break;
         }//else of if( pDpSystem->sAl.bSequence == TRUE && pDpSystem->sAl.bCount < pDpSystem->sAl.bLimit )
      }//else of if( alarm_sequence == FALSE )

      sTempAlarm.sListPtr = sTempAlarmNext.sListPtr;
   }//while( (DPL_HOST_PTR_COMPARE_TYPE) sTempAlarm.sListPtr != (DPL_HOST_PTR_COMPARE_TYPE) sTempAnchor.sListPtr )
}//void AL_CheckSendAlarm( void )

/*--------------------------------------------------------------------------*/
/* function: AL_CheckSendDiag                                               */
/*--------------------------------------------------------------------------*/
UBYTE AL_CheckSendDiag( UBYTE bAlarmType, ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCallback )
{
UBYTE bRetValue;

   if( !VPC3_GET_DIAG_FLAG() )
   {
      //old diagnostic message is send
      if( bCallback == TRUE )
      {
         //send alarm over the function user_alarm
         bRetValue = UserAlarm( bAlarmType, psAlarm->bSpecifier, psAlarm, TRUE );
      }//if( bCallback == TRUE )
      else
      {
         bRetValue = VPC3_SetDiagnosis( (MEM_UNSIGNED8_PTR)psAlarm->bHeader, ( psAlarm->bHeader & DIAG_TYPE_MASK ), 0x00, TRUE );
      }//else of if( bCallback == TRUE )
   }//if( !VPC3_GET_DIAG_FLAG() )
   else
   {
      //wait - old diagnosis is not send
      bRetValue = DP_DIAG_OLD_DIAG_NOT_SEND_ERROR;
   }//else of if( !VPC3_GET_DIAG_FLAG() )

    return bRetValue;
}//UBYTE AL_CheckSendDiag( UBYTE bAlarmType, ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCallback )

/*--------------------------------------------------------------------------*/
/* function: AL_ClearAlarm                                                  */
/*--------------------------------------------------------------------------*/
void AL_ClearAlarm( UBYTE bAlarmTypeBitField, UBYTE bSeqNr )
{
ALARM_UNION_ALARM sTempAnchor;
ALARM_UNION_ALARM sTempAlarm;
UBYTE             bTmpSpecifier;
UBYTE             bTmpAlarmType;
UBYTE             bTmpAlarmTypeBitField;

   //reset all alarms
   if( ( bAlarmTypeBitField == ALARM_TYPE_ALL_VALUE ) && ( bSeqNr == SEQUENCE_NUMBER_ALL ) )
   {
      dpl_init_list__( &pDpSystem->sAl.dplAlarmQueue );
      dpl_init_list__( &pDpSystem->sAl.dplAlarmAckQueue );

      pDpSystem->sAl.bTypeStatus = 0;
      memset( &pDpSystem->sAl.abSequenceStatus[0], 0, AL_SEQUENCE_STATUS_SIZE );

      pDpSystem->sAl.bCount    = 0;
   }//if( ( bAlarmTypeBitField == ALARM_TYPE_ALL_VALUE ) && ( bSeqNr == SEQUENCE_NUMBER_ALL ) )
   else
   {
      bTmpAlarmTypeBitField = 0;

      if( ALARM_TYPE_DIAGNOSTIC_VALUE & (bAlarmTypeBitField) )
      {
         bTmpAlarmTypeBitField |= (1 << (ALARM_TYPE_DIAGNOSTIC - 1));
      }
      if( ALARM_TYPE_PROCESS_VALUE & (bAlarmTypeBitField) )
      {
         bTmpAlarmTypeBitField |= (1 << (ALARM_TYPE_PROCESS - 1));
      }
      if( ALARM_TYPE_PULLPLUG_VALUE & (bAlarmTypeBitField) )
      {
         bTmpAlarmTypeBitField |= (1 << (ALARM_TYPE_PULL - 1)) | (1 << (ALARM_TYPE_PLUG - 1));
      }
      if( ALARM_TYPE_STATUS_VALUE & (bAlarmTypeBitField) )
      {
         bTmpAlarmTypeBitField |= (1 << (ALARM_TYPE_STATUS - 1));
      }
      if( ALARM_TYPE_UPDATE_VALUE & (bAlarmTypeBitField) )
      {
         bTmpAlarmTypeBitField |= (1 << (ALARM_TYPE_UPDATE - 1));
      }
      if( ALARM_TYPE_MANUFACTURER_VALUE & (bAlarmTypeBitField) )
      {
         bTmpAlarmTypeBitField |= (1 << (ALARM_TYPE_MAX - 1));
      }

      sTempAnchor.sListPtr = &pDpSystem->sAl.dplAlarmQueue;
      sTempAlarm .sListPtr =  pDpSystem->sAl.dplAlarmQueue.NextBlkPtr;

      bTmpSpecifier = 0;
      bTmpAlarmType = 0;

      while( (DPL_HOST_PTR_COMPARE_TYPE) sTempAlarm.sListPtr != (DPL_HOST_PTR_COMPARE_TYPE) sTempAnchor.sListPtr )
      {
         bTmpSpecifier = sTempAlarm.psAlarm->bSpecifier;
         bTmpAlarmType = sTempAlarm.psAlarm->bAlarmType;

         if(    ( ( bTmpAlarmTypeBitField & ( 1 << ( AL_Acls( bTmpAlarmType ) ) ) ) || ( bAlarmTypeBitField == ALARM_TYPE_ALL_VALUE ) )
             && ( ( ( bTmpSpecifier >> SPEC_SEQ_START ) == bSeqNr ) || ( bSeqNr == SEQUENCE_NUMBER_ALL ) )
           )
         {
            if( pDpSystem->sAl.bSequence == FALSE )
            {
               //type mode
               //only one alarm of a specific ALARM_TYPE can be active at one time
               AL_CheckTypeStatus( AL_ALARM_STATUS_ACTION_RESET, bTmpAlarmType );
               AL_CheckSequenceStatus( AL_ALARM_STATUS_ACTION_RESET, bTmpAlarmType, bTmpSpecifier );
            }//if( pDpSystem->sAl.bSequence == FALSE )
            else
            {
               //sequence mode
               //several alarms (2 to 32) of the same or different ALARM_TYPE can be active at one time
               AL_CheckSequenceStatus( AL_ALARM_STATUS_ACTION_RESET, bTmpAlarmType, bTmpSpecifier );

               pDpSystem->sAl.bCount--;
            }//else of if( pDpSystem->sAl.bSequence == FALSE )
         }//if(  .....

         sTempAlarm.sListPtr = sTempAlarm.sListPtr->NextBlkPtr;
      }//while( (DPL_HOST_PTR_COMPARE_TYPE) sTempAlarm.sListPtr != (DPL_HOST_PTR_COMPARE_TYPE) sTempAnchor.sListPtr )
   }//else of if( ( bAlarmTypeBitField == ALARM_TYPE_ALL_VALUE ) && ( bSeqNr == SEQUENCE_NUMBER_ALL ) )
}//void AL_ClearAlarm( UBYTE bAlarmTypeBitField, UBYTE bSeqNr )

/*--------------------------------------------------------------------------*/
/* function: AL_CheckTypeStatus                                             */
/*--------------------------------------------------------------------------*/
UBYTE AL_CheckTypeStatus( UBYTE bAction, UBYTE bAlarmType )
{
UBYTE bRetValue;

   bAlarmType = (UBYTE)(1 << ( AL_Acls( bAlarmType )));

   if( bAction == AL_ALARM_STATUS_ACTION_SET )
   {
      pDpSystem->sAl.bTypeStatus |= bAlarmType;
      bRetValue = TRUE;
   }//if( bAction == AL_ALARM_STATUS_ACTION_SET )
   else
   if( bAction == AL_ALARM_STATUS_ACTION_RESET )
   {
      pDpSystem->sAl.bTypeStatus &= ~bAlarmType;
      bRetValue = FALSE;
   }//else if( bAction == AL_ALARM_STATUS_ACTION_RESET )
   else
   {
      bRetValue = ( pDpSystem->sAl.bTypeStatus & bAlarmType ) ? TRUE : FALSE;
   }//else

   return bRetValue;
}//UBYTE AL_CheckTypeStatus( UBYTE bAction, UBYTE bAlarmType )

/*--------------------------------------------------------------------------*/
/* function: AL_CheckSequenceStatus                                         */
/*--------------------------------------------------------------------------*/
UBYTE AL_CheckSequenceStatus( UBYTE bAction, UBYTE bAlarmType, UBYTE bAlarmSpecifier )
{
MEM_UNSIGNED8_PTR  pToAlarmSequenceStatus;
UBYTE              bRetValue;

   bAlarmType = (1 << ( AL_Acls( bAlarmType )));

   // bit field index in alarm_sequence_status
   bAlarmType = bAlarmType * MAX_SEQ_NR + (bAlarmSpecifier >> SPEC_SEQ_START);

   // bit mask at byte field index in alarm_sequence_status
   bAlarmSpecifier = (UBYTE)(1 << (bAlarmType & 0x07));

   // byte field index in alarm_sequence_status
   pToAlarmSequenceStatus = pDpSystem->sAl.abSequenceStatus + ( bAlarmType >> 3 );

   if( *pToAlarmSequenceStatus & bAlarmSpecifier )
   {
      if( bAction == AL_ALARM_STATUS_ACTION_RESET )
      {
         // reset only if set
         *pToAlarmSequenceStatus &= ~bAlarmSpecifier;
      }//if( bAction == AL_ALARM_STATUS_ACTION_RESET )

      bRetValue = TRUE;
   }//if( *pToAlarmSequenceStatus & bAlarmSpecifier )
   else
   {
      if( bAction == AL_ALARM_STATUS_ACTION_SET )
      {
         // set only if reset
         *pToAlarmSequenceStatus |= bAlarmSpecifier;
      }//if( bAction == AL_ALARM_STATUS_ACTION_SET )

      bRetValue = FALSE;
   }//else of if( *pToAlarmSequenceStatus & bAlarmSpecifier )

   return( bRetValue );
}//UBYTE AL_CheckSequenceStatus( UBYTE bAction, UBYTE bAlarmType, UBYTE bAlarmSpecifier )

/*--------------------------------------------------------------------------*/
/* function: AL_AlarmQuit (called by DPV1)                                  */
/*--------------------------------------------------------------------------*/
DPV1_RET_VAL AL_AlarmQuit( UBYTE bSapNr, ALARM_ACK_REQ_PTR psAlarmAck )
{
ALARM_UNION_ALARM   sTempAnchor;
ALARM_UNION_ALARM   sTempAlarm;
DPV1_RET_VAL        bRetValue;
UBYTE               bTmpSpecifier;
UBYTE               bTmpAlarmType;
UBYTE               bTmpSlotNr;
UBYTE               bAlarmFound;
UBYTE               bAlarmType;
UBYTE               bSlotNr;
UBYTE               bSeqNr;

   bRetValue = DPV1_OK;

   if( ( bSapNr == DP_C1_RD_WR_SAP_NR ) || ( bSapNr == DP_C1_ALARM_SAP_NR ) )
   {
      bSlotNr    = psAlarmAck->bSlotNr;
      bAlarmType = psAlarmAck->bAlarmType;
      bSeqNr     = ( psAlarmAck->bSpecifier & SEQ_NR_MASK ) >> SPEC_SEQ_START;

      sTempAnchor.sListPtr = &pDpSystem->sAl.dplAlarmAckQueue;
      sTempAlarm .sListPtr =  pDpSystem->sAl.dplAlarmAckQueue.NextBlkPtr;

      bTmpSpecifier = 0;
      bTmpSlotNr    = 0;
      bTmpAlarmType = 0;
      bAlarmFound   = FALSE;

      while( (DPL_HOST_PTR_COMPARE_TYPE) sTempAlarm.sListPtr != (DPL_HOST_PTR_COMPARE_TYPE) sTempAnchor.sListPtr )
      {
         bTmpSpecifier = sTempAlarm.psAlarm->bSpecifier;
         bTmpSlotNr    = sTempAlarm.psAlarm->bSlotNr;
         bTmpAlarmType = sTempAlarm.psAlarm->bAlarmType;

         if(    ( (bTmpSpecifier & SEQ_NR_MASK) >> SPEC_SEQ_START == bSeqNr     )
             && (  bTmpAlarmType                                  == bAlarmType )
             && (  bTmpSlotNr                                     == bSlotNr    )
           )
         {
            //alarm found
            bAlarmFound = TRUE;
            break;
         }

         sTempAlarm.sListPtr = sTempAlarm.sListPtr->NextBlkPtr;
      }//while( (DPL_HOST_PTR_COMPARE_TYPE) sTempAlarm.sListPtr != (DPL_HOST_PTR_COMPARE_TYPE) sTempAnchor.sListPtr )

      if( bAlarmFound == TRUE )
      {
         if( pDpSystem->sAl.bSequence == FALSE )
         {
            //type mode
            //only one alarm of a specific ALARM_TYPE can be active at one time
            AL_CheckTypeStatus( AL_ALARM_STATUS_ACTION_RESET, bTmpAlarmType );
            AL_CheckSequenceStatus( AL_ALARM_STATUS_ACTION_RESET, bTmpAlarmType, bTmpSpecifier );
         }//if( alarm_sequence == FALSE )
         else
         {
            //sequence mode
            //several alarms (2 to 32) of the same or different ALARM_TYPE can be active at one time
            AL_CheckSequenceStatus( AL_ALARM_STATUS_ACTION_RESET, bTmpAlarmType, bTmpSpecifier );

            pDpSystem->sAl.bCount--;
         }//else of if( alarm_sequence == FALSE )

         dpl_remove_blk__( sTempAlarm.sListPtr );

         UserAlarmAckReq( sTempAlarm.psAlarm );
      }//if( bAlarmFound == TRUE )
      else
      {
         bRetValue = DPV1_NOK;
      }//else of if( bAlarmFound == TRUE )
   }//if( ( bSapNr == DP_C1_RD_WR_SAP_NR ) || ( bSapNr == DP_C1_ALARM_SAP_NR ) )
   else
   {
      return DPV1_NOK;
   }//else of if( ( bSapNr == DP_C1_RD_WR_SAP_NR ) || ( bSapNr == DP_C1_ALARM_SAP_NR ) )

   return bRetValue;
}//DPV1_RET_VAL AL_AlarmQuit( UBYTE bSapNr, ALARM_ACK_REQ_PTR psAlarmAck )

#endif//#if DP_ALARM


#endif //#if DP_MSAC_C1

/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

