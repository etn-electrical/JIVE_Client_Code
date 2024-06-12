/**********************  Filename: dp_msac2.c  *******************************/
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
/* Function:       interface service routines for msac_c2                    */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Technical support:       P. Fredehorst                                    */
/*                          Tel. : ++49-9132-744-2150                        */
/*                          Fax. : ++49-9132-744-29-2150                     */
/*                          eMail: pfredehorst@profichip.com                 */
/*                          eMail: support@profichip.com                     */
/*                                                                           */
/*****************************************************************************/

/*! \file
     \brief Functions, state machine of DPV1 class2.

*/

/*****************************************************************************/
/* contents:

  - function prototypes
  - data structures
  - internal functions

*/
/*****************************************************************************/
/* include hierarchy */
#include "Includes.h"
#include "VPC3_platform.h"	// Path changed for Galaxy //
#include "VPC3.h"
#include "VPC3_dp_inc.h"		// Path changed for Galaxy //

#if DP_MSAC_C2

/*---------------------------------------------------------------------------*/
/* function prototypes                                                       */
/*---------------------------------------------------------------------------*/
void  MSAC_C2_RealizeIndRecv           ( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bSapNr );
void  MSAC_C2_SendResponseData         ( MSAC_C2_CONNECT_ITEM_PTR psConnection );
void  MSAC_C2_CallIndAwait             ( MSAC_C2_CONNECT_ITEM_PTR psConnection );
void  MSAC_C2_RespPduProvide           ( UBYTE bSapNr );
void  MSAC_C2_PduProvide               ( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
void  MSAC_C2_HandleUserResponse       ( MSAC_C2_CONNECT_ITEM_PTR psConnection);
void  MSAC_C2_CreateDisconnectPdu      ( MSAC_C2_CONNECT_ITEM_PTR psConnection, MSAC2_DIRECTION bDirection );
void  MSAC_C2_RealizeRespSent          ( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bSapNr );
void  MSAC_C2_RealizeIndDisableDone    ( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bSapNr );
void  MSAC_C2_ConfirmCloseRequest      ( MSAC_C2_CONNECT_ITEM_PTR psConnection );
void  MSAC_C2_DisconnectUserInd        ( MSAC_C2_CONNECT_ITEM_PTR psConnection );
void  MSAC_C2_CloseChannel             ( void );
UBYTE MSAC_C2_StartTimer               ( UBYTE bTimerIndex, UWORD wTimerValue );
UBYTE MSAC_C2_StopTimer                ( UBYTE bTimerIndex );

/*---------------------------------------------------------------------------*/
/* function: MSAC_C2_Process                                                 */
/*---------------------------------------------------------------------------*/
/*!
  \brief The application program has to call this function cyclically so that the MSAC_C2 services can be processed.
*/
void MSAC_C2_Process( void )
{
MSAC_C2_CONNECT_ITEM_PTR   psConnection;
UBYTE                      bSapNr;
UBYTE                      bCnId;
#if DP_MSAC_C2_TIME
   UBYTE                   i;
#endif//#if DP_MSAC_C2_TIME

   // check, if MSAC_C2 functions enabled
   if( pDpSystem->sC2.bEnabled )
   {
      // handle receive queue
      while( pDpSystem->sC2.bReadRecPtr != pDpSystem->sC2.bWriteRecPtr )
      {
         // check bCnId
         pDpSystem->sC2.psActRecQueue = &(pDpSystem->sC2.asRecQueue[pDpSystem->sC2.bReadRecPtr]);
         bSapNr = pDpSystem->sC2.psActRecQueue->bSapNr;

         if( ( bSapNr > MSAC_C2_SAP_NR_HIGH ) || ( ( MSAC_C2_SAP_NR_HIGH - bSapNr ) >=  pDpSystem->sC2.bNrOfSap ) )
         {
            sVpc3Error.bErrorCode = pDpSystem->sC2.psActRecQueue->bFdlCode;
            sVpc3Error.bCnId      = bSapNr;
            // *** no further action of dpv1/c2 ***
            FatalError( _DP_C2, __LINE__, &sVpc3Error );
         }//if( ( bSapNr > MSAC_C2_SAP_NR_HIGH ) || ( ( MSAC_C2_SAP_NR_HIGH - bSapNr ) >=  pDpSystem->sC2.bNrOfSap ) )
         else
         {
            bCnId = MSAC_C2_SAP_NR_HIGH - bSapNr;
            if( bCnId >= DP_C2_NUM_SAPS )
            {
               sVpc3Error.bErrorCode = pDpSystem->sC2.psActRecQueue->bFdlCode;
               sVpc3Error.bCnId      = bCnId;
               // *** no further action of dpv1/c2 ***
               FatalError( _DP_C2, __LINE__, &sVpc3Error );
            }//if( bCnId >= DP_C2_NUM_SAPS )
            else
            {
               // determine pointer of connection description
               psConnection = &(pDpSystem->sC2.asConnectionList[bCnId]);

               switch( pDpSystem->sC2.psActRecQueue->bFdlCode )
               {
                  case MSAC_C2_IND_RECV:
                  {
                     MSAC_C2_RealizeIndRecv( psConnection, bSapNr );
                     break;
                  }//case MSAC_C2_IND_RECV:

                  case MSAC_C2_RESP_SENT:
                  {
                     MSAC_C2_RealizeRespSent( psConnection, bSapNr );
                     break;
                  }//case MSAC_C2_RESP_SENT:

                  case MSAC_C2_IND_DISABLE_DONE:
                  {
                     MSAC_C2_RealizeIndDisableDone( psConnection, bSapNr );
                     break;
                  }//case MSAC_C2_IND_DISABLE_DONE:

                  default:
                  {
                     sVpc3Error.bErrorCode = pDpSystem->sC2.psActRecQueue->bFdlCode;
                     // *** no further action of dpv1/c2 ***
                     FatalError( _DP_C2, __LINE__, &sVpc3Error );
                     break;
                  }//default:
               }//switch( pDpSystem->sC2.psActRecQueue->bFdlCode )
            }//else of if( bCnId >= DP_C2_NUM_SAPS )
         }//else of if( ( bSapNr > MSAC_C2_SAP_NR_HIGH ) || ( ( MSAC_C2_SAP_NR_HIGH - bSapNr ) >=  pDpSystem->sC2.bNrOfSap ) )

         if( pDpSystem->sC2.bReadRecPtr != pDpSystem->sC2.bEndRecPtr )
         {
            pDpSystem->sC2.bReadRecPtr++;
         }//if( pDpSystem->sC2.bReadRecPtr != pDpSystem->sC2.bEndRecPtr )
         else
         {
            pDpSystem->sC2.bReadRecPtr = pDpSystem->sC2.bStartRecPtr;
         }//if( pDpSystem->sC2.bReadRecPtr != pDpSystem->sC2.bEndRecPtr )
      }//while( pDpSystem->sC2.bReadRecPtr != pDpSystem->sC2.bWriteRecPtr )

      #if DP_MSAC_C2_TIME
         //check timer
         for( i = 0; i < DP_C2_NUM_SAPS; i++ )
         {
            if( pDpSystem->sC2.asConnectionList[i].eState != MSAC_C2_CS_CLOSE_CHANNEL ) //sign of free element
            {
               if( pDpSystem->sC2.asConnectionList[i].bTimerActive == TRUE )
               {
                  // determine pointer of connection description
                  psConnection = &(pDpSystem->sC2.asConnectionList[i]);
                  if( pDpSystem->sC2.asTimerList[psConnection->bTimerIndex].bRunning == FALSE )
                  {
                     //reset PCNTRL_RESP bit
                     FDL_ResetCntrlRespBit( psConnection->bSapNr, FDL_PRIMARY_BUF );

                     //BugFix 503
                     //ILL32 transistion, certification Ifak
                     if(    ( psConnection->eErrorCode == MSAC_C2_EC_REQ_TIMEOUT )
                         && ( psConnection->eUserBreak == MSAC_C2_UB_BREAK       )
                       )
                     {
                        psConnection->bTimerBreak = TRUE;
                        //disable timer
                        psConnection->bTimerActive = FALSE;
                        // create dr pdu in reserved buffer
                        MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_REQ_DIRECTION );
                        psConnection->eState = MSAC_C2_CS_DISABLE_CONNECT;
                        FDL_IndDisable( psConnection->bSapNr);
                     }//if(    ( psConnection->eErrorCode == MSAC_C2_EC_REQ_TIMEOUT ) ...
                     else
                     {
                        //new description state
                        psConnection->eState = MSAC_C2_CS_PROVIDE_RESP;
                        //set recognized-flag of eUserBreak
                        psConnection->eUserBreak = MSAC_C2_UB_BREAK;
                        psConnection->eErrorCode = MSAC_C2_EC_REQ_TIMEOUT;
                        //handle request pdu as disconnect-pdu
                        MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_REQ_DIRECTION );
                        //handle pdu as disconnect-pdu
                        MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_RESP_DIRECTION );
                        MSAC_C2_SendResponseData( psConnection);
                        psConnection->bTimerActive = TRUE;
                     }//else of if(    ( psConnection->eErrorCode == MSAC_C2_EC_REQ_TIMEOUT ) ...
                  }//if( pDpSystem->sC2.asTimerList[psConnection->bTimerIndex].bRunning == FALSE )
               }//if( pDpSystem->sC2.asConnectionList[i].bTimerActive == TRUE )
            }//if( pDpSystem->sC2.asConnectionList[i].eState != MSAC_C2_CS_CLOSE_CHANNEL )
         }//for( i = 0; i < DP_C2_NUM_SAPS; i++ )
      #endif//#if DP_MSAC_C2_TIME
   }//if( pDpSystem->sC2.bEnabled )
}//void MSAC_C2_Process( void )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   void MSAC_C2_PduProvide( UBYTE bSapNr, MSAC_C2_DATA_BUF_PTR buf_ptr )   */
/*                                                                           */
/*   function:                                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_PduProvide( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
DPV1_RET_VAL    bRetValue;
UBYTE           bC2Service;

   bRetValue = DPV1_INIT;
   bC2Service = psMsgHeader->bDpv1Service & MSAC_C2_FN_MASK;

   switch( bC2Service )
   {
      case MSAC_C2_FN_DS_READ:
      {
         if( ( psMsgHeader->bDataLength > 0 ) && ( psMsgHeader->bDataLength <= 240 ) && ( psMsgHeader->bIndicationLength ) )
         {
            bRetValue = UserDpv1ReadReq( bSapNr, psMsgHeader, &pToDpv1Data[4] );
         }//if( ( psMsgHeader->bDataLength > 0 ) && ( psMsgHeader->bDataLength <= 240 ) && ( psMsgHeader->bIndicationLength ) )
         else
         {
            bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_TYPE, 0x00 );
         }//else of if( ( psMsgHeader->bDataLength > 0 ) && ( psMsgHeader->bDataLength <= 240 ) && ( psMsgHeader->bIndicationLength ) )
         break;
      }//case MSAC_C2_FN_DS_READ:

      case MSAC_C2_FN_DS_WRITE:
      {
         if( psMsgHeader->bDataLength > 0 )
         {
            bRetValue = UserDpv1WriteReq( bSapNr, psMsgHeader, &pToDpv1Data[4] );
         }//if( psMsgHeader->bDataLength > 0 )
         else
         {
            bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_TYPE, 0x00 );
         }//else of if( psMsgHeader->bDataLength > 0 )
         break;
      }//case MSAC_C2_FN_DS_WRITE:

      case MSAC_C2_FN_DATA:
      {
         if( psMsgHeader->bDataLength > 0 )
         {
            bRetValue = UserMsac2DataTransportReq( bSapNr, psMsgHeader, &pToDpv1Data[4] );
         }//if( psMsgHeader->bDataLength > 0 )
         else
         {
            bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_TYPE, 0x00 );
         }//else of if( psMsgHeader->bDataLength > 0 )
         break;
      }//case MSAC_C2_FN_DATA:

      case MSAC_C2_FN_CONNECT:
      {
         bRetValue = UserMsac2InitiateReq( bSapNr, &pDpSystem->sC2.sInitiateReq, psMsgHeader, pToDpv1Data );
         break;
      }//case MSAC_C2_FN_CONNECT:

      case MSAC_C2_FN_DISCONNECT:
      {
         bRetValue = UserMsac2AbortInd( bSapNr );
         if( bRetValue != DPV1_OK)
         {
            sVpc3Error.bErrorCode = bC2Service;
            sVpc3Error.bCnId      = bSapNr;
            // *** no further action of dpv1/c2 ***
            FatalError( _DP_C2, __LINE__, &sVpc3Error );
         }//if( bRetValue != DPV1_OK)
         bRetValue = DPV1_DELAY;  // ABORT-function always do nothing
         break;
      }//case MSAC_C2_FN_DISCONNECT:

      default:
      {
         // Error unknown service
         sVpc3Error.bErrorCode = bC2Service;
         sVpc3Error.bCnId      = bSapNr;
         // *** no further action of dpv1/c2 ***
         FatalError( _DP_C2, __LINE__, &sVpc3Error );
         break;
      }//default:
   }//switch( bC2Service )

   switch( bRetValue )
   {
      case DPV1_OK:
      {
         if( bC2Service != MSAC_C2_FN_CONNECT )
         {
            if( bC2Service == MSAC_C2_FN_DS_WRITE )
            {
               psMsgHeader->bIndicationLength = DPV1_LEN_HEAD_DATA;
            }//if( bC2Service == MSAC_C2_FN_DS_WRITE )
            else // for READ and TRANSPORT length is defined by userdata
            {
               psMsgHeader->bIndicationLength = DPV1_LEN_HEAD_DATA + psMsgHeader->bDataLength;
            }//else of if( bC2Service == MSAC_C2_FN_DS_WRITE )
         }//if( bC2Service != MSAC_C2_FN_CONNECT )

         psMsgHeader->bFunctionCode =  FC_RESP_L;

         MSAC_C2_RespPduProvide( bSapNr );
         break;
      }//case DPV1_OK:

      case DPV1_NOK:
      {
         //user must create correct response-pdu
         psMsgHeader->bIndicationLength =  DPV1_LEN_NEG_RESPONSE;
         psMsgHeader->bFunctionCode     =  FC_RESP_L;
         psMsgHeader->bDpv1Service      |= DPV1_ERROR_BIT_RESPONSE;
         psMsgHeader->bSlotNr           = DPV1_ERRDC_DPV1;           //DPV1 Error_Decode

         MSAC_C2_RespPduProvide( bSapNr );
         break;
      }//case DPV1_NOK:

      case DPV1_DELAY:
      {
         // do nothing
         break;
      }//case DPV1_DELAY:

      case DPV1_ABORT:
      {
         psMsgHeader->bIndicationLength = MSAC_C2_LEN_DISCONNECT_PDU-1;
         psMsgHeader->bDpv1Service      = MSAC_C2_FN_DISCONNECT | MSAC_C2_FUNCTION_NUM_EXTENSION;
         psMsgHeader->bFunctionCode     =  FC_RESP_L;

         MSAC_C2_RespPduProvide( bSapNr );
         break;
      }//case DPV1_ABORT:

      default:
      {
         sVpc3Error.bErrorCode = bRetValue;
         sVpc3Error.bCnId      = bSapNr;
         // *** no further action of dpv1/c2 ***
         FatalError( _DP_C2, __LINE__, &sVpc3Error );
         break;
      }//default:
   }//switch( bRetValue )
}//void MSAC_C2_PduProvide( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   void MSAC_C2_RealizeIndRecv(MSAC_C2_CONNECT_ITEM_PTR psConnection,      */
/*                                 UBYTE                    bSapNr)          */
/*                                                                           */
/*   function:          MSAC_C2 received a request-pdu.                      */
/*                      Depend on the C2 Service                             */
/*                       - the pdu is given to the user                      */
/*                         (DATA, DISCONNECT),                               */
/*                       - a response is provided (IDLE),                    */
/*                       - a new connection is initialized (CONNECT).        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_RealizeIndRecv( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bSapNr )
{
INITIATE_REQ_PDU_PTR    psInitiateReq;
MSG_HEADER_PTR          psMsgHeader;
UBYTE                   bTimerRetValue;
UBYTE                   bC2Service;
UBYTE                   bCnId;
UBYTE                   bIinitiateSDLength;

   psMsgHeader = pDpSystem->sC2.psActRecQueue->psMsgHeader;
   bCnId = psConnection->bCnId;
   bC2Service = psMsgHeader->bDpv1Service & MSAC_C2_FN_MASK;

   //taking over service and buf_ptr into connection description
   psConnection->bC2Service  = bC2Service;
   psConnection->psMsgHeader = psMsgHeader;
   psConnection->pToDpv1Data = pDpSystem->sC2.psActRecQueue->pToDpv1Data;

   switch( psConnection->eState )
   {
      // state after initialization
      case MSAC_C2_CS_AWAIT_CONNECT:
      {
         // reset saved informations of msac_c2_disconnect()
         psConnection->bDrReasonCode = 0x00;
         psConnection->bDrLocation = 0x00;
         // set init value of error_code
         psConnection->eErrorCode = MSAC_C2_EC_OK;
         // reset timer
         pDpSystem->sC2.asTimerList[psConnection->bTimerIndex].wActValue = 0x0000;
         pDpSystem->sC2.asTimerList[psConnection->bTimerIndex].bRunning  = FALSE;
         psConnection->bTimerBreak = FALSE;

         psInitiateReq = &pDpSystem->sC2.sInitiateReq;

         CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&pDpSystem->sC2.sInitiateReq, psConnection->pToDpv1Data, psMsgHeader->bIndicationLength );

         // s_len and d_len, must be correct set, if S/D-Typ is defined
         bIinitiateSDLength = 0;
         bIinitiateSDLength += ( psInitiateReq->bS_Type == 0 ) ? 2 : psInitiateReq->bS_Len;

         bIinitiateSDLength += ( psInitiateReq->bD_Type == 0 ) ? 2 : psInitiateReq->bD_Len;

         if( bC2Service != MSAC_C2_FN_CONNECT )
         {
            //changed V302 PNO testcase, wrong function number
            psConnection->eState = MSAC_C2_CS_PROVIDE_RESP;
            psConnection->wSendTimeOut = pDpSystem->sC2.wSendTimeOut;
            //changed for Testcase RM11
            psConnection->eErrorCode = MSAC_C2_EC_REMOTE_ERROR;
            //create disconnect pdu
            MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_RESP_DIRECTION );
            MSAC_C2_SendResponseData( psConnection );
            psConnection->bTimerBreak = FALSE;
            psConnection->eState = MSAC_C2_CS_DISABLE_CONNECT;
            FDL_IndDisable( bSapNr ); //close SAP, answer POLL request with RS
         }//if( bC2Service != MSAC_C2_FN_CONNECT )

         else
         if(    ( ( psInitiateReq->bS_Type == 0 ) && ( psInitiateReq->bS_Len != 2 ) )
             || ( ( psInitiateReq->bD_Type == 0 ) && ( psInitiateReq->bD_Len != 2 ) )
           )
         {
            psConnection->eState = MSAC_C2_CS_PROVIDE_RESP;
            psConnection->wSendTimeOut = pDpSystem->sC2.wSendTimeOut;
            //changed for Testcase RM11
            psConnection->eErrorCode = MSAC_C2_EC_REMOTE_ERROR;
            //create disconnect pdu
            MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_RESP_DIRECTION );
            MSAC_C2_SendResponseData( psConnection );
            psConnection->bTimerBreak = FALSE;
         }//else if( (buf_ptr->data_len <  MSAC_C2_MIN_CR_PDU_SIZE) || ...

         else
         if( ( psMsgHeader->bIndicationLength <  MSAC_C2_MIN_CR_PDU_SIZE ) ||
             ( psMsgHeader->bIndicationLength <  MSAC_C2_MIN_CR_PDU_SIZE -4 + bIinitiateSDLength )  )
         {
            psConnection->eState = MSAC_C2_CS_PROVIDE_RESP;
            psConnection->wSendTimeOut = pDpSystem->sC2.wSendTimeOut;
            //changed for Testcase RM11
            psConnection->eErrorCode = MSAC_C2_EC_INV_S_D_LEN_ERR;
            //create disconnect pdu
            MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_RESP_DIRECTION );
            MSAC_C2_SendResponseData( psConnection );
            //manipulate the item of bTimerBreak, because
            //MSAC_C2_SendResponseData() was called without
            //a previous MSAC_C2_START_TIMER
            psConnection->bTimerBreak = FALSE;
         }//else if( (buf_ptr->data_len <  MSAC_C2_MIN_CR_PDU_SIZE) || ...

         else
         {
            //connect-pdu
            //handle poll_timeout
            //BugFix 503
            #if LITTLE_ENDIAN
               psInitiateReq->wSendTimeOut        = SWAP_WORD( psInitiateReq->wSendTimeOut );
               psInitiateReq->wProfileIdentNumber = SWAP_WORD( psInitiateReq->wProfileIdentNumber );
            #endif//#if LITTLE_ENDIAN

            if( pDpSystem->sC2.wSendTimeOut > psInitiateReq->wSendTimeOut )
            {
               psInitiateReq->wSendTimeOut = pDpSystem->sC2.wSendTimeOut;
            }//if( pDpSystem->sC2.wSendTimeOut > psInitiateReq->wSendTimeOut )
            psConnection->wSendTimeOut = psInitiateReq->wSendTimeOut;
            bTimerRetValue = MSAC_C2_StartTimer( psConnection->bTimerIndex, psInitiateReq->wSendTimeOut );

            if( bTimerRetValue != MSAC_C2_TIMER_OK )
            {
               sVpc3Error.bErrorCode = bTimerRetValue;
               sVpc3Error.bCnId      = bCnId;
               // *** no further action of dpv1/c2 ***
               FatalError( _DP_C2, __LINE__, &sVpc3Error );
            }//if( bTimerRetValue != MSAC_C2_TIMER_OK )
            else
            {
               // set connection state
               psConnection->eState = MSAC_C2_CS_PROVIDE_IND;
               MSAC_C2_PduProvide( bSapNr, psConnection->psMsgHeader, psConnection->pToDpv1Data );
               psConnection->bTimerActive = TRUE;
            }//else of if( bTimerRetValue != MSAC_C2_TIMER_OK )
         }//else of if( bC2Service != MSAC_C2_FN_CONNECT )
         break;
      }//case MSAC_C2_CS_AWAIT_CONNECT:

      // state after successfully intiate indication
      case MSAC_C2_CS_AWAIT_IND:
      {
         // Check PollTimeOut
         bTimerRetValue = MSAC_C2_StartTimer( psConnection->bTimerIndex, psConnection->wSendTimeOut );

         //There was a poll_time_out. The timeout-request
         //is behind this request in the input queue.
         //This subject is without importance in this case.
         if( bTimerRetValue == MSAC_C2_TIMER_OK )
         {
            psConnection->bTimerBreak = TRUE;
         }//if( bTimerRetValue == MSAC_C2_TIMER_OK )

         if( bC2Service == MSAC_C2_FN_DISCONNECT )
         {
            //disable timer
            psConnection->bTimerActive = FALSE;
            if( psConnection->eUserBreak != MSAC_C2_UB_NO_BREAK)
            {
               // set recognized-flag of eUserBreak
               psConnection->eUserBreak = MSAC_C2_UB_BREAK;
            }
            else
            {
               psConnection->eErrorCode = MSAC_C2_EC_DC_BY_MASTER;
               // create dr pdu in reserved buffer
               MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_REQ_DIRECTION );
               MSAC_C2_PduProvide( bSapNr, psConnection->psMsgHeaderRes, psConnection->pToDpv1Data );
            }
            psConnection->eState = MSAC_C2_CS_DISABLE_CONNECT;
            FDL_IndDisable( bSapNr );
         }//if( bC2Service == MSAC_C2_FN_DISCONNECT )
         else
         {
            // bC2Service != DPV2_FN_DISCONNECT
            #if DP_MSAC_C2_TIME
               if(    ( psConnection->eUserBreak  != MSAC_C2_UB_NO_BREAK )
                   || ( psConnection->bTimerBreak == TRUE                )
                 )
            #else
               if( psConnection->eUserBreak != MSAC_C2_UB_NO_BREAK )
            #endif //#if DP_MSAC_C2_TIME
               {
                  //disable timer
                  psConnection->bTimerActive = FALSE;
                  //new description state
                  psConnection->eState = MSAC_C2_CS_PROVIDE_RESP;
                  //set recognized-flag of eUserBreak
                  psConnection->eUserBreak = MSAC_C2_UB_BREAK;
                  if( psConnection->bTimerBreak == TRUE )
                  {
                     psConnection->eErrorCode = MSAC_C2_EC_REQ_TIMEOUT;
                     //handle request pdu as disconnect-pdu
                     MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_REQ_DIRECTION );
                     //give the disconnect information to the user
                     MSAC_C2_PduProvide( bSapNr, psConnection->psMsgHeaderRes, psConnection->pToDpv1Data );
                  }//if( psConnection->bTimerBreak == TRUE )
                  else
                  {
                     psConnection->eErrorCode = MSAC_C2_EC_DC_BY_USER;
                  }//else of if( psConnection->bTimerBreak == TRUE )
                  //handle pdu as disconnect-pdu
                  MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_RESP_DIRECTION );
                  MSAC_C2_SendResponseData( psConnection);
               }//if(    ( psConnection->eUserBreak != MSAC_C2_UB_NO_BREAK)
               else
               {
                  //no user break
                  if( bC2Service == MSAC_C2_FN_IDLE )
                  {
                     //handle response data
                     psConnection->eState = MSAC_C2_CS_PROVIDE_RESP;
                     psMsgHeader->bFunctionCode =  FC_RESP_L;
                     MSAC_C2_SendResponseData( psConnection );
                  }//if( bC2Service == MSAC_C2_FN_IDLE )
                  else
                  {
                     bTimerRetValue = MSAC_C2_StartTimer( psConnection->bTimerIndex, psConnection->wSendTimeOut );

                     //There was a request_time_out. The timeout-request
                     //is behind this request in the input queue.
                     //Set a note and reject later the timeout-request.
                     if( bTimerRetValue == MSAC_C2_TIMER_OK )
                     {
                         psConnection->bTimerBreak = TRUE;  //DP_TRUE
                     }//if( bTimerRetValue == MSAC_C2_TIMER_OK )

                     switch( bC2Service )
                     {
                        case MSAC_C2_FN_DATA:
                        case MSAC_C2_FN_DS_READ:
                        case MSAC_C2_FN_DS_WRITE:
                        {
                           psConnection->eState = MSAC_C2_CS_PROVIDE_IND;
                           MSAC_C2_PduProvide( bSapNr, psConnection->psMsgHeader, psConnection->pToDpv1Data );
                           break;
                        }//case MSAC_C2_FN_DATA:

                        default:
                        {
                           //invalid service
                           //set error code
                           psConnection->eErrorCode = MSAC_C2_EC_REMOTE_ERROR;
                           //handle request pdu as disconnect-pdu
                           MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_REQ_DIRECTION );
                           //give the disconnect information to the user
                           MSAC_C2_PduProvide( bSapNr, psConnection->psMsgHeaderRes, psConnection->pToDpv1Data );
                           //handle response pdu as disconnect-pdu
                           MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_RESP_DIRECTION );
                           //new description state
                           psConnection->eState = MSAC_C2_CS_PROVIDE_RESP;
                           MSAC_C2_SendResponseData( psConnection );
                           break;
                        }//default:
                     }//switch( bC2Service )
                  }//else of if( bC2Service == MSAC_C2_FN_IDLE )
               }//else of if(    ( psConnection->eUserBreak != MSAC_C2_UB_NO_BREAK )
         }//else of if( bC2Service == MSAC_C2_FN_DISCONNECT )
         break;
      }//case MSAC_C2_CS_AWAIT_IND:

      case MSAC_C2_CS_PROVIDE_RESP:
      {
         //V302; special behavior for Testcase 2C6
         if( bC2Service == MSAC_C2_FN_DISCONNECT )
         {
            psConnection->bTimerActive = FALSE;
            psConnection->eErrorCode = MSAC_C2_EC_DC_BY_MASTER;
            // create dr pdu in reserved buffer
            MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_REQ_DIRECTION );
            // disconnet to user always by psMsgHeaderRes
            MSAC_C2_PduProvide( bSapNr, psConnection->psMsgHeaderRes, psConnection->pToDpv1Data );
            psConnection->eState = MSAC_C2_CS_DISABLE_CONNECT;
            FDL_IndDisable( bSapNr );
         }//if( bC2Service == MSAC_C2_FN_DISCONNECT )
         break;
      }//case MSAC_C2_CS_PROVIDE_RESP:

      case MSAC_C2_CS_DISABLE_CONNECT:
      {
         //do nothing
         break;
      }//case MSAC_C2_CS_DISABLE_CONNECT:
         
      case MSAC_C2_CS_PROVIDE_IND:
      {
         // Check PollTimeOut
         bTimerRetValue = MSAC_C2_StartTimer( psConnection->bTimerIndex, psConnection->wSendTimeOut );

         if( bTimerRetValue == MSAC_C2_TIMER_OK )
         {
            psConnection->bTimerBreak = TRUE;
         }//if( bTimerRetValue == MSAC_C2_TIMER_OK )

         if( bC2Service == MSAC_C2_FN_DISCONNECT )
         {
            //disable timer
            psConnection->bTimerActive = FALSE;
            if( psConnection->eUserBreak != MSAC_C2_UB_NO_BREAK)
            {
               // set recognized-flag of eUserBreak
               psConnection->eUserBreak = MSAC_C2_UB_BREAK;
            }
            else
            {
               psConnection->eErrorCode = MSAC_C2_EC_DC_BY_MASTER;
               // create dr pdu in reserved buffer
               MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_REQ_DIRECTION );
               MSAC_C2_PduProvide( bSapNr, psConnection->psMsgHeaderRes, psConnection->pToDpv1Data );
            }
            psConnection->eState = MSAC_C2_CS_DISABLE_CONNECT;
            FDL_IndDisable( bSapNr );
         }//if( bC2Service == MSAC_C2_FN_DISCONNECT )
         break;
      }//case MSAC_C2_CS_PROVIDE_IND:
      
      //not allowed here
      case MSAC_C2_CS_CLOSE_CHANNEL:
      default:
      {
         sVpc3Error.bErrorCode = psConnection->eState;
         sVpc3Error.bCnId      = bCnId;
         // *** no further action of dpv1/c2 ***
         FatalError( _DP_C2, __LINE__, &sVpc3Error );
         break;
      }//default
   }//switch( psConnection->eState )
}//void MSAC_C2_RealizeIndRecv( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bSapNr )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* MSAC_C2_RealizeRespSent(MSAC_C2_CONNECT_ITEM_PTR psConnection,            */
/*                           UBYTE bSapNr )                                  */
/*                                                                           */
/*   function: A response pdu was sent to the master. Gives the              */
/*             confirmation to the user and realizes the timer handling.     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_RealizeRespSent( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bSapNr )
{
UBYTE  bTimerRetValue;
UBYTE  bCnId;
UBYTE  bC2Service;
UBYTE  bNoAction;
UBYTE  bFatalState;

   bC2Service = psConnection->bC2Service;
   bCnId = psConnection->bCnId;
   bFatalState = FALSE;
   bNoAction = FALSE;

   //check return value
   if( pDpSystem->sC2.psActRecQueue->bRetValue == MSAC_C2_EC_OK )
   {
      //check connection state
      switch( psConnection->eState )
      {
         case MSAC_C2_CS_PROVIDE_RESP:
         {
            break;
         }//case MSAC_C2_CS_PROVIDE_RESP:

         case MSAC_C2_CS_DISABLE_CONNECT:
         {
            switch( psConnection->eErrorCode )
            {
               //V302; special behavior for Testcase 2C6
               case MSAC_C2_EC_DC_BY_MASTER:
               {
                  //do nothing here, there was a ABORT before
                  bNoAction = TRUE;
                  break;
               }//case MSAC_C2_EC_DC_BY_MASTER:

               default:
               {
                  bFatalState = TRUE;
               }//default:
            }//switch( psConnection->eErrorCode )
            break;
         }//case MSAC_C2_CS_DISABLE_CONNECT:

         case MSAC_C2_CS_CLOSE_CHANNEL:
         case MSAC_C2_CS_AWAIT_CONNECT:
         case MSAC_C2_CS_AWAIT_IND:
         case MSAC_C2_CS_PROVIDE_IND:
         default:
         {
            bFatalState = TRUE;
            break;
         }//default:
      }//switch( psConnection->eState )

      if( bFatalState )
      {
         sVpc3Error.bErrorCode = psConnection->eState;
         sVpc3Error.bCnId      = bCnId;
         // *** no further action of dpv1/c2 ***
         FatalError( _DP_C2, __LINE__, &sVpc3Error );
      }//if( bFatalState )

      if( !bNoAction )
      {
         switch( bC2Service )
         {
            case MSAC_C2_FN_IDLE:
            case MSAC_C2_FN_DATA:
            case MSAC_C2_FN_DS_READ:
            case MSAC_C2_FN_DS_WRITE:
            case MSAC_C2_FN_CONNECT:
            {
               //start request_time_out
               bTimerRetValue = MSAC_C2_StartTimer( psConnection->bTimerIndex, ( psConnection->wSendTimeOut << 1) );

               //There was a request_time_out. The timeout-request
               //is behind this request in the input queue.
               //Set a note and reject later the timeout-request.
               if( bTimerRetValue == MSAC_C2_TIMER_OK )
               {
                   psConnection->bTimerBreak = TRUE;
               }//if( bTimerRetValue == MSAC_C2_TIMER_OK )

               //new description state
               psConnection->eState = MSAC_C2_CS_AWAIT_IND;
               break;
            }//case MSAC_C2_FN_IDLE:

            case MSAC_C2_FN_DISCONNECT:
            {
               //stop PollTimeOut
               bTimerRetValue = MSAC_C2_StopTimer( psConnection->bTimerIndex );

               //There was a poll_time_out. The timeout-request
               //is behind this request in the input queue.
               //This subject is without importance in this case.
               if( bTimerRetValue == MSAC_C2_TIMER_ALREADY_STOPPED )
               {
                   psConnection->bTimerBreak = TRUE;
               }//if( bTimerRetValue == MSAC_C2_TIMER_ALREADY_STOPPED )

               psConnection->eState = MSAC_C2_CS_DISABLE_CONNECT;
               FDL_IndDisable(bSapNr);
               break;
            }//case MSAC_C2_FN_DISCONNECT:

            default:
            {
               //internal error: invalid bC2Service */
               sVpc3Error.bErrorCode = psConnection->bC2Service;
               sVpc3Error.bCnId      = bCnId;
               // *** no further action of dpv1/c2 ***
               FatalError( _DP_C2, __LINE__, &sVpc3Error );
            }//default:
         }//switch( bC2Service )
      }//if( !bNoAction )
   }//if( pDpSystem->sC2.psActRecQueue->bRetValue == MSAC_C2_EC_OK )
   else
   {
      sVpc3Error.bErrorCode = pDpSystem->sC2.psActRecQueue->bRetValue;
      sVpc3Error.bCnId      = bCnId;
      // *** no further action of dpv1/c2 ***
      FatalError( _DP_C2, __LINE__, &sVpc3Error );
   }//else of if( pDpSystem->sC2.psActRecQueue->bRetValue == MSAC_C2_EC_OK )
}//void MSAC_C2_RealizeRespSent( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bSapNr )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* MSAC_C2_RealizeIndDisableDone(MSAC_C2_CONNECT_ITEM_PTR psConnection,      */
/*                                  UBYTE bSapNr )                           */
/*                                                                           */
/*   function: A sap was deactivated. Give the confirmation to the user.     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_RealizeIndDisableDone( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bSapNr )
{
   //V504
   switch( pDpSystem->sC2.psActRecQueue->bRetValue )
   {
      case MSAC_C2_EC_OK:
      {
         if( psConnection->eState == MSAC_C2_CS_DISABLE_CONNECT )
         {
            switch( psConnection->eErrorCode )
            {
               case MSAC_C2_EC_USER_ERR:
               case MSAC_C2_EC_DC_BY_USER:
               case MSAC_C2_EC_DC_BY_MASTER:
               case MSAC_C2_EC_REMOTE_ERROR:
               case MSAC_C2_EC_INV_S_D_LEN_ERR: //Testcase RM11
               {
                  //give a new connection resource to rm sap */
                  MSAC_C2_CallIndAwait( psConnection);
                  break;
               }//case MSAC_C2_EC_USER_ERR:

               case MSAC_C2_EC_REQ_TIMEOUT:
               {
                  MSAC_C2_DisconnectUserInd( psConnection);
                  break;
               }//case MSAC_C2_EC_REQ_TIMEOUT:

               default:
               {
                  sVpc3Error.bErrorCode = psConnection->eErrorCode;
                  sVpc3Error.bCnId      = psConnection->bCnId;
                  // *** no further action of dpv1/c2 ***
                  FatalError( _DP_C2, __LINE__, &sVpc3Error );
               }//default:
            }//switch( psConnection->eErrorCode )
         }//if( psConnection->eState == MSAC_C2_CS_DISABLE_CONNECT )
         else
         {
            sVpc3Error.bErrorCode = psConnection->eState;
            sVpc3Error.bCnId      = psConnection->bCnId;
            // *** no further action of dpv1/c2 ***
            FatalError( _DP_C2, __LINE__, &sVpc3Error );
         }//else of if( psConnection->eState == MSAC_C2_CS_DISABLE_CONNECT )
         break;
      }//case MSAC_C2_EC_OK:

      case MSAC_C2_EC_IUSE:
      {
         FDL_PerformSapReset( bSapNr );
         
		 if( psConnection->eState == MSAC_C2_CS_DISABLE_CONNECT )
         {
            switch( psConnection->eErrorCode )
            {
               case MSAC_C2_EC_USER_ERR:
               case MSAC_C2_EC_DC_BY_USER:
               case MSAC_C2_EC_DC_BY_MASTER:
               case MSAC_C2_EC_REMOTE_ERROR:
               case MSAC_C2_EC_INV_S_D_LEN_ERR: //Testcase RM11
               {
                  //give a new connection resource to rm sap */
                  MSAC_C2_CallIndAwait( psConnection);
                  break;
               }//case MSAC_C2_EC_USER_ERR:

               case MSAC_C2_EC_REQ_TIMEOUT:
               {
                  MSAC_C2_DisconnectUserInd( psConnection);
                  break;
               }//case MSAC_C2_EC_REQ_TIMEOUT:

               default:
               {
                  sVpc3Error.bErrorCode = psConnection->eErrorCode;
                  sVpc3Error.bCnId      = psConnection->bCnId;
                  // *** no further action of dpv1/c2 ***
                  FatalError( _DP_C2, __LINE__, &sVpc3Error );
               }//default:
            }//switch( psConnection->eErrorCode )
         }//if( psConnection->eState == MSAC_C2_CS_DISABLE_CONNECT )
         else
         {
            sVpc3Error.bErrorCode = psConnection->eState;
            sVpc3Error.bCnId      = psConnection->bCnId;
            // *** no further action of dpv1/c2 ***
            FatalError( _DP_C2, __LINE__, &sVpc3Error );
         }//else of if( psConnection->eState == MSAC_C2_CS_DISABLE_CONNECT )		 
		          
         break;
      }//case MSAC_C2_EC_IUSE:

      default:
      {
         sVpc3Error.bErrorCode = pDpSystem->sC2.psActRecQueue->bRetValue;
         sVpc3Error.bCnId      = bSapNr;
         // *** no further action of dpv1/c2 ***
         FatalError( _DP_C2, __LINE__, &sVpc3Error );
         break;
      }//default:
   }//switch( pDpSystem->sC2.psActRecQueue->bRetValue )
}//void MSAC_C2_RealizeIndDisableDone( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bSapNr )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* MSAC_C2_DisconnectUserInd(MSAC_C2_CONNECT_ITEM_PTR psConnection)          */
/*                                                                           */
/*   function: A sap was deactivated. Give the confirmation to the user.     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_DisconnectUserInd( MSAC_C2_CONNECT_ITEM_PTR psConnection )
{
   //give a new connection resource to rm sap
   MSAC_C2_CallIndAwait( psConnection);
   //create disconnect pdu
   MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_REQ_DIRECTION );
   MSAC_C2_PduProvide( psConnection->bSapNr, psConnection->psMsgHeaderRes, psConnection->pToDpv1Data );
}//void MSAC_C2_DisconnectUserInd( MSAC_C2_CONNECT_ITEM_PTR psConnection )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* MSAC_C2_ConfirmCloseRequest( MSAC_C2_CONNECT_ITEM_PTR psConnection )      */
/*                                                                           */
/*   function: The function realizes the close handling of a connection      */
/*             after a close_channel request.                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_ConfirmCloseRequest( MSAC_C2_CONNECT_ITEM_PTR psConnection )
{
UBYTE bAllClosed;
UBYTE i;

   psConnection->eUserBreak = MSAC_C2_UB_NO_BREAK;
   MSAC_C2_StopTimer( psConnection->bTimerIndex );
   psConnection->eState = MSAC_C2_CS_CLOSE_CHANNEL;

   i = 0;
   do
   {
      bAllClosed = (pDpSystem->sC2.asConnectionList[i].eState == MSAC_C2_CS_CLOSE_CHANNEL);
      i++;
   }
   while( bAllClosed && ( i < DP_C2_NUM_SAPS ) );

   if( bAllClosed )
   {
      pDpSystem->sC2.bCloseChannelRequest = FALSE;
   }// if( bAllClosed )
}//void MSAC_C2_ConfirmCloseRequest( MSAC_C2_CONNECT_ITEM_PTR psConnection )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* UBYTE msac_c2_transmit_delay( UBYTE bSapNr, UBYTE status,                 */
/*                               DPV1_PTR pdu_ptr )                          */
/*                                                                           */
/*   function: msac_c2_transmit_delay receives response data from the user.  */
/*             In this function the response data are checked. In dependence */
/*             of the pdu-Service und the status the length of the data and  */
/*             the function-code is manipulated. Then the dpv1_msc2 transmit */
/*             function is called.                                           */
/*                                                                           */
/*   return codes: DPV1_OK          ( transmit function is called )          */
/*                 C2_ENABLED_ERROR ( C2 service not supported    )          */
/*                 C2_INV_CN_ID     ( invalid connection ident    )          */
/*                 C2_USER_ERR      ( invalid response data       )          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
UBYTE MSAC_C2_TransmitDelay( UBYTE bSapNr, DPV1_RET_VAL bDpv1RetValue )
{
MSAC_C2_CONNECT_ITEM_PTR    psConnection;
MSG_HEADER_PTR              psMsgHeader;
UBYTE                       bC2Service;
UBYTE                       bRetValue;

   //check, if MSAC_C2 functions enabled
   if( pDpSystem->sC2.bEnabled )
   {
      //check, if valid sap
      if( ( bSapNr > MSAC_C2_SAP_NR_HIGH ) || ( ( MSAC_C2_SAP_NR_HIGH - bSapNr ) >=  pDpSystem->sC2.bNrOfSap ) )
      {
         bRetValue = C2_INV_CN_ID;
      }//if( ( bSapNr > MSAC_C2_SAP_NR_HIGH ) || ( ( MSAC_C2_SAP_NR_HIGH - bSapNr ) >=  pDpSystem->sC2.bNrOfSap ) )
      else
      {
         // determine psConnection
         psConnection = &(pDpSystem->sC2.asConnectionList[MSAC_C2_SAP_NR_HIGH - bSapNr]);
         if( psConnection->eErrorCode == MSAC_C2_EC_OK )
         {   
            psMsgHeader = psConnection->psMsgHeader;
            bC2Service = psMsgHeader->bDpv1Service & MSAC_C2_FN_MASK;
            bRetValue = DPV1_OK;
            
            switch( bDpv1RetValue )
            {
               case DPV1_OK:
               {
                  if( bC2Service != MSAC_C2_FN_CONNECT )
                  {
                      if( bC2Service == MSAC_C2_FN_DS_WRITE )
                      {
                        psMsgHeader->bIndicationLength = DPV1_LEN_HEAD_DATA;
                      }//if( bC2Service == MSAC_C2_FN_DS_WRITE )
                      else
                      {
                         //for READ and TRANSPORT length is defined by userdata
                         psMsgHeader->bIndicationLength = DPV1_LEN_HEAD_DATA + psMsgHeader->bDataLength;
                      }//else of if( bC2Service == MSAC_C2_FN_DS_WRITE )
                  }//if( bC2Service != MSAC_C2_FN_CONNECT )
            
                  psMsgHeader->bFunctionCode = FC_RESP_L;
            
                  MSAC_C2_RespPduProvide( bSapNr );
                  break;
               }//case DPV1_OK:
            
               case DPV1_NOK:
               {
                  //User must create correct response-pdu */
                  psMsgHeader->bIndicationLength =  DPV1_LEN_NEG_RESPONSE;
                  psMsgHeader->bFunctionCode     =  FC_RESP_L;
                  psMsgHeader->bDpv1Service      |= DPV1_ERROR_BIT_RESPONSE;
                  psMsgHeader->bSlotNr           = DPV1_ERRDC_DPV1;           //DPV1 Error_Decode
            
                  MSAC_C2_RespPduProvide( bSapNr );
                  break;
               }//case DPV1_NOK:
            
               case DPV1_ABORT:
               {
                  psMsgHeader->bIndicationLength = MSAC_C2_LEN_DISCONNECT_PDU-1;
                  psMsgHeader->bDpv1Service      = MSAC_C2_FN_DISCONNECT | MSAC_C2_FUNCTION_NUM_EXTENSION;
                  psMsgHeader->bFunctionCode     =  FC_RESP_L;
            
                  MSAC_C2_RespPduProvide( bSapNr );
                  break;
               }//case DPV1_ABORT:
            
               default:
               {
                  bRetValue = C2_USER_ERR;
                  break;
               }//default:
            }//switch( bDpv1RetValue )
         }//if( psConnection->eErrorCode == MSAC_C2_EC_OK )
         else
         {
            bRetValue = psConnection->eErrorCode;
         }//else of if( psConnection->eErrorCode == MSAC_C2_EC_OK )         
      }//else ofif( ( bSapNr > MSAC_C2_SAP_NR_HIGH ) || ( ( MSAC_C2_SAP_NR_HIGH - bSapNr ) >=  pDpSystem->sC2.bNrOfSap ) )
   }//if( pDpSystem->sC2.bEnabled )
   else
   {
      bRetValue = C2_ENABLED_ERROR;
   }//else of if( pDpSystem->sC2.bEnabled )

   return( bRetValue );
}//UBYTE MSAC_C2_TransmitDelay( UBYTE bSapNr, DPV1_RET_VAL bDpv1RetValue )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* UBYTE MSAC_C2_InitiateReqToRes(DPV1_PTR pdu_ptr)                          */
/*                                                                           */
/*   function: function helps user to create an initiate-response            */
/*             the response is only created, if the slave is endpoint of the */
/*             connection. Otherwise the puffer is untouched.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
UBYTE MSAC_C2_InitiateReqToRes( INITIATE_REQ_PDU_PTR psInitiateReq, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
INITIATE_RES_PDU_PTR psInitiateRes;
UBYTE                bRetValue;
UBYTE                bS_Type;
UBYTE                bS_Len;
UBYTE                i;

    bRetValue = DPV1_NOK;

   if( psInitiateReq->bD_Type == 0 )
   {
      psInitiateRes = (INITIATE_RES_PDU_PTR)pToDpv1Data;

      psMsgHeader->bSlotNr     = DP_C2_LEN - 4;
      psMsgHeader->bIndex      = C2_FEATURES_SUPPORTED_1;
      psMsgHeader->bDataLength = C2_FEATURES_SUPPORTED_2;

      #if VPC3_SERIAL_MODE

         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) +  1, DP_C2_LEN - 4 );
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) +  2, C2_FEATURES_SUPPORTED_1 );
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) +  3, C2_FEATURES_SUPPORTED_2 );

         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) +  4, C2_PROFILE_FEATURES_1 );
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) +  5, C2_PROFILE_FEATURES_2 );
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) +  6, (UBYTE)(C2_PROFILE_NUMBER >>8) );
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) +  7, (UBYTE)C2_PROFILE_NUMBER );

         //D-Typ and S-Typ changed position
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) +  8, psInitiateReq->bD_Type );
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) +  9, psInitiateReq->bD_Len );

         bS_Type = psInitiateReq->bS_Type;           //save S_typ
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) + 10, bS_Type );         //copy S_typ in buffer
         bS_Len =  psInitiateReq->bS_Len;            //save S_len
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) + 11, bS_Len );          //copy S_len in buffer
         //lge/type S
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) + 12, psInitiateReq->abAddrData[bS_Len +0] );
         Vpc3Write( ((VPC3_ADR)( psInitiateRes )) + 13, psInitiateReq->abAddrData[bS_Len +1] );

         //after this point S-len is destroyed
         //copy add. address
         if( bS_Type == 0 )
         {
            //if S-Typ =0
            Vpc3Write( ((VPC3_ADR)( psInitiateRes )) + 14, psInitiateReq->abAddrData[0] );
            Vpc3Write( ((VPC3_ADR)( psInitiateRes )) + 15, psInitiateReq->abAddrData[1] );
         }// if( bS_Type == 0 )
         else
         {
            for( i = 0; i < bS_Len; i++ )
            {
                Vpc3Write( ((VPC3_ADR)( psInitiateRes )) + ( 14 + i ), psInitiateReq->abAddrData[i] );
            }//for( i = 0; i < bS_Len; i++ )
         }//else of  if( bS_Type == 0 )

      #else

         psInitiateRes->bMaxLenDataUnit            = DP_C2_LEN - 4;
         psInitiateRes->bFeaturesSupported1        = C2_FEATURES_SUPPORTED_1;
         psInitiateRes->bFeaturesSupported2        = C2_FEATURES_SUPPORTED_2;

         psInitiateRes->bProfileFeaturesSupported1 = C2_PROFILE_FEATURES_1;
         psInitiateRes->bProfileFeaturesSupported2 = C2_PROFILE_FEATURES_2;
         psInitiateRes->bProfileIdentNumber        = C2_PROFILE_NUMBER;

         //D-Typ and S-Typ changed position
         psInitiateRes->bS_Type  = psInitiateReq->bD_Type;
         psInitiateRes->bS_Len   = psInitiateReq->bD_Len;

         bS_Type = psInitiateReq->bS_Type;           //save S_typ
         psInitiateRes->bD_Type  = bS_Type;          //copy S_typ in buffer
         bS_Len =  psInitiateReq->bS_Len;            //save S_len
         psInitiateRes->bD_Len   = bS_Len;           //copy S_len in buffer
         //lge/type S
         psInitiateRes->abAddrData[0]  =  psInitiateReq->abAddrData[bS_Len +0];
         psInitiateRes->abAddrData[1]  =  psInitiateReq->abAddrData[bS_Len +1];

         //after this point S-len is destroyed
         //copy add. address
         if( bS_Type == 0 )
         {
            //if S-Typ =0
            psInitiateRes->abAddrData[2+0] = psInitiateReq->abAddrData[0];
            psInitiateRes->abAddrData[2+1] = psInitiateReq->abAddrData[1];
         }// if( bS_Type == 0 )
         else
         {
            for( i = 0; i < bS_Len; i++ )
            {
               psInitiateRes->abAddrData[2+i] = psInitiateReq->abAddrData[i];
            }//for( i = 0; i < bS_Len; i++ )
         }//else of  if( bS_Type == 0 )

      #endif//#if VPC3_SERIAL_MODE

      psMsgHeader->bIndicationLength = MSAC_C2_LEN_INITIATE_RES_HEAD + psInitiateReq->bS_Len + psInitiateReq->bD_Len;
      psMsgHeader->bFunctionCode     = FC_RESP_L;

      bRetValue = DPV1_OK;
   }//if( psInitiateReq->bD_Type == 0 )

   return bRetValue;
}//UBYTE MSAC_C2_InitiateReqToRes( INITIATE_REQ_PDU_PTR psInitiateReq, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )

/*---------------------------------------------------------------------------*/
/* function: MSAC_C2_ResetStateMachine                                       */
/*---------------------------------------------------------------------------*/
//V504
void MSAC_C2_ResetStateMachine( void )
{
UBYTE i;

   //init memory item for close_channel request of the user */
   pDpSystem->sC2.bCloseChannelRequest = FALSE;

   // initialize receive queue */
   pDpSystem->sC2.bStartRecPtr = 0;
   pDpSystem->sC2.bWriteRecPtr = pDpSystem->sC2.bStartRecPtr;
   pDpSystem->sC2.bReadRecPtr  = pDpSystem->sC2.bStartRecPtr;

   for( i = 0; i < DP_C2_NUM_SAPS; i++ )
   {
      pDpSystem->sC2.asConnectionList[i].eState  = MSAC_C2_CS_CLOSE_CHANNEL;  //sign of free element */

      pDpSystem->sC2.asConnectionList[i].bTimerBreak = FALSE;
      pDpSystem->sC2.asConnectionList[i].bTimerActive = FALSE;

      pDpSystem->sC2.asConnectionList[i].eUserBreak  = MSAC_C2_UB_NO_BREAK;

      pDpSystem->sC2.asConnectionList[i].bDrReasonCode = 0x00;
      pDpSystem->sC2.asConnectionList[i].bDrLocation    = 0x00;

      MSAC_C2_StopTimer(pDpSystem->sC2.asConnectionList[i].bTimerIndex);
      FDL_PerformSapReset( pDpSystem->sC2.asConnectionList[i].bSapNr );
   }//for( i = 0; i < DP_C2_NUM_SAPS; i++ )

   //RM
   FDL_PerformSapReset( DP_C2_RM_SAP_NR );
}//void MSAC_C2_ResetStateMachine( void )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* DP_ERROR_CODE MSAC_C2_Init( void )                                        */
/*                                                                           */
/*   function: This function is called by the user. It initializes the       */
/*             variables of MSAC_C2 connections                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
DP_ERROR_CODE MSAC_C2_Init( void )
{
DP_ERROR_CODE bError;
UBYTE         bHelp;
UBYTE         i;

   bError = DP_OK;

   // check sap
   bHelp = C2_NUM_SAPS;
   if(    ( bHelp <= MSAC_C2_SAP_NR_HIGH )
       && ( bHelp <= DP_C2_NUM_SAPS      )
     )
   {
      // check c2_len
      bHelp = DP_C2_LEN;
      if(    ( bHelp >= MSAC_C2_MIN_CR_PDU_SIZE )
          && ( bHelp <= MSAC_C2_MAX_PDU         )
        )
      {
         pDpSystem->sC2.wSendTimeOut = MSAC_C2_TIMEOUT_1_5MBAUD;
         pDpSystem->sC2.bNrOfSap   = DP_C2_NUM_SAPS;

         // init connection list
         for( i = 0; i < DP_C2_NUM_SAPS; i++ )
         {
            pDpSystem->sC2.asConnectionList[i].bSapNr = MSAC_C2_SAP_NR_HIGH - i;
            pDpSystem->sC2.asConnectionList[i].eState = MSAC_C2_CS_CLOSE_CHANNEL;  //sign of free element */
            pDpSystem->sC2.asConnectionList[i].bCnId  = i;

            pDpSystem->sC2.asConnectionList[i].bTimerIndex    = i;
            pDpSystem->sC2.asConnectionList[i].bTimerBreak    = FALSE;
            pDpSystem->sC2.asConnectionList[i].bTimerActive   = FALSE;

            pDpSystem->sC2.asConnectionList[i].eUserBreak     = MSAC_C2_UB_NO_BREAK;
            pDpSystem->sC2.asConnectionList[i].psMsgHeaderRes = (MSG_HEADER_PTR)&pDpSystem->sC2.asConnectionList[i].sMsgHeader;

            pDpSystem->sC2.asConnectionList[i].bDrReasonCode = 0x00;
            pDpSystem->sC2.asConnectionList[i].bDrLocation    = 0x00;
         }//for( i = 0; i < DP_C2_NUM_SAPS; i++ )

         //init memory item for close_channel request of the user */
         pDpSystem->sC2.bCloseChannelRequest = FALSE;

         // initialize receive queue */
         pDpSystem->sC2.bStartRecPtr = 0;
         pDpSystem->sC2.bWriteRecPtr = pDpSystem->sC2.bStartRecPtr;
         pDpSystem->sC2.bReadRecPtr  = pDpSystem->sC2.bStartRecPtr;
         pDpSystem->sC2.bEndRecPtr   = MSAC_C2_MAX_INPUT_ITEMS-1;

         // enable msac_c2-functions
         pDpSystem->sC2.bEnabled = TRUE;
      }//if(    ( bHelp >= MSAC_C2_MIN_CR_PDU_SIZE ) ...
      else
      {
         bError = C2_DATA_LEN_ERROR;
      }//else of if(    ( bHelp >= MSAC_C2_MIN_CR_PDU_SIZE ) ...
   }//if(    ( bHelp <= MSAC_C2_SAP_NR_HIGH ) ...
   else
   {
      bError = C2_DATA_SAP_ERROR;
   }//else of if(    ( bHelp <= MSAC_C2_SAP_NR_HIGH ) ...

   return bError;
}//DP_ERROR_CODE MSAC_C2_Init( void )

/*---------------------------------------------------------------------------*/
/* function: MSAC_C2_SetTimeoutIsr                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_SetTimeoutIsr( void )
{
   switch( VPC3_GET_BAUDRATE() )
   {
      case BAUDRATE_500KBAUD:
      {
         pDpSystem->sC2.wSendTimeOut = MSAC_C2_TIMEOUT_500KBAUD;
         break;
      }//case BAUDRATE_500KBAUD:

      case BAUDRATE_187_50KBAUD:
      case BAUDRATE_93_75KBAUD:
      case BAUDRATE_45_45KBAUD:
      case BAUDRATE_19_20KBAUD:
      case BAUDRATE_9_60KBAUD:
      {
         pDpSystem->sC2.wSendTimeOut = MSAC_C2_TIMEOUT_187_50KBAUD;
         break;
      }//case BAUDRATE_187_50KBAUD:

      default:
      {
         pDpSystem->sC2.wSendTimeOut = MSAC_C2_TIMEOUT_1_5MBAUD;
         break;
      }//default:
   }//switch( VPC3_GET_BAUDRATE() )

   FDL_SetPollTimeOutIsr( pDpSystem->sC2.wSendTimeOut );
}//void MSAC_C2_SetTimeoutIsr( void )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void MSAC_C2_OpenChannelSap( UBYTE bSapNr )                               */
/*                                                                           */
/*   function: internal function                                             */
/*             activate sap                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_OpenChannelSap( UBYTE bSapNr )
{
UBYTE i;

   i = 0;
   while( pDpSystem->sC2.asConnectionList[i].bSapNr != bSapNr )
   {
      i++;
   }//while( pDpSystem->sC2.asConnectionList[i].bSapNr != bSapNr )
   pDpSystem->sC2.asConnectionList[i].eState = MSAC_C2_CS_AWAIT_CONNECT;
}//void MSAC_C2_OpenChannelSap( UBYTE bSapNr )


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* DP_ERROR_CODE MSAC_C2_OpenChannel (void )                                 */
/*                                                                           */
/*   function: implementation of open_channel-function, analyze the data,    */
/*             allocate timer for sap 49, activate sap 49                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
DP_ERROR_CODE MSAC_C2_OpenChannel( void )
{
DP_ERROR_CODE bError;

   //check, if msac_c2-functions enabled
   if( pDpSystem->sC2.bEnabled )
   {
      //check connection state: connection 0 represents the connection description
      if( pDpSystem->sC2.asConnectionList[0].eState == MSAC_C2_CS_CLOSE_CHANNEL )
      {
         bError = DP_OK;

         if( FDL_OpenChannel() == FDL_OK )
         {
            //set description state of connection
            pDpSystem->sC2.asConnectionList[0].eState = MSAC_C2_CS_AWAIT_CONNECT;

            //assign connection resource of lower layer
            if( FDL_IndAwait( pDpSystem->sC2.asConnectionList[0].bSapNr ) != FDL_OK )
            {
               bError = C2_NO_CONN_RESOURCE;
            }//if( FDL_IndAwait( pDpSystem->sC2.asConnectionList[0].bSapNr ) != FDL_OK )
         }//if( FDL_OpenChannel() == FDL_OK )
         else
         {
            bError = C2_INV_LOWER_LAYER;
         }//else of if( FDL_OpenChannel() == FDL_OK )
      }//if( pDpSystem->sC2.asConnectionList[0].eState == MSAC_C2_CS_CLOSE_CHANNEL )
      else
      {
         //function is called twice
         bError = C2_RESOURCE_ERROR;
      }//else of if( pDpSystem->sC2.asConnectionList[0].eState == MSAC_C2_CS_CLOSE_CHANNEL )
   }//if( pDpSystem->sC2.bEnabled )
   else
   {
      bError = C2_ENABLED_ERROR;
   }//else of if( pDpSystem->sC2.bEnabled )

   return bError;
}//DP_ERROR_CODE MSAC_C2_OpenChannel( void )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* MSAC_C2_CreateDisconnectPdu( MSAC_C2_CONNECT_ITEM_PTR psConnection,       */
/*                                UBYTE                    bDirection )      */
/*                                                                           */
/*   function: internal function                                             */
/*             manipulate or create a pdu as disconnect pdu                  */
/*             Note: A disconnect pdu to the user is always to               */
/*                   send by the reserved buffer.                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_CreateDisconnectPdu( MSAC_C2_CONNECT_ITEM_PTR psConnection, MSAC2_DIRECTION bDirection )
{
MSG_HEADER_PTR      psMsgHeader;
MSAC_C2_DR_BUF_PTR  psDisconnect;
UBYTE               bDrLocation;
UBYTE               bDrError;

   //disable timer
   psConnection->bTimerActive = FALSE;
   bDrLocation = MSAC_C2_SUBNET_NO;
   bDrError = 0x00;

   switch( psConnection->eErrorCode )
   {
      case MSAC_C2_EC_DC_BY_MASTER:
      {
         //read the source information of dr pdu
         psMsgHeader = psConnection->psMsgHeader;
         psDisconnect = (MSAC_C2_DR_BUF_PTR)&( psMsgHeader->bDpv1Service );
         bDrLocation = psDisconnect->bLocation;
         bDrError = psDisconnect->bReasonCode;
         break;
      }//case MSAC_C2_EC_DC_BY_MASTER:

      case MSAC_C2_EC_USER_ERR:
      {
         bDrError = MSAC_C2_ABT_IV | MSAC_C2_INSTANCE_MSAC_C2;
         break;
      }//case MSAC_C2_EC_USER_ERR:

      case MSAC_C2_EC_INV_S_D_LEN_ERR:
      {
         bDrError = MSAC_C2_ABT_IA | MSAC_C2_INSTANCE_MSAC_C2;
         break;
      }//case MSAC_C2_EC_INV_S_D_LEN_ERR:

      case MSAC_C2_EC_REQ_TIMEOUT:
      {
         bDrError = MSAC_C2_ABT_TO | MSAC_C2_INSTANCE_MSAC_C2;
         break;
      }//case MSAC_C2_EC_REQ_TIMEOUT:

      case MSAC_C2_EC_REMOTE_ERROR:
      {
         bDrError = MSAC_C2_ABT_FE | MSAC_C2_INSTANCE_MSAC_C2;
         break;
      }//case MSAC_C2_EC_REMOTE_ERROR:

      case MSAC_C2_EC_DC_BY_USER:
      {
         bDrLocation = psConnection->bDrLocation;
         bDrError = ( psConnection->bDrReasonCode) | MSAC_C2_INSTANCE_USER;
         break;
      }//case MSAC_C2_EC_DC_BY_USER:

      default:
      {
         sVpc3Error.bErrorCode = psConnection->eErrorCode;
         sVpc3Error.bCnId      = psConnection->bCnId;
         // *** no further action of dpv1/c2 ***
         FatalError( _DP_C2, __LINE__, &sVpc3Error );
         break;
      }//default:
   }//end switch */

   //define destination pointer
   if( bDirection == MSAC_C2_RESP_DIRECTION )
   {
      psMsgHeader = psConnection->psMsgHeader;
   }//if( bDirection == MSAC_C2_RESP_DIRECTION )
   else
   {
      //MSAC_C2_REQ_DIRECTION
      psMsgHeader = psConnection->psMsgHeaderRes;
   }//else of if( bDirection == MSAC_C2_RESP_DIRECTION )

   psMsgHeader->bIndicationLength = MSAC_C2_LEN_DISCONNECT_PDU - 1;
   psDisconnect = (MSAC_C2_DR_BUF_PTR)&( psMsgHeader->bDpv1Service );

   //fill dr pdu */
   psDisconnect->bC2Service  = MSAC_C2_FN_DISCONNECT | MSAC_C2_FUNCTION_NUM_EXTENSION;
   psDisconnect->bLocation   = bDrLocation;
   psDisconnect->bReasonCode = bDrError;
   psDisconnect->bReserved   = 0x00;

   psConnection->bC2Service = MSAC_C2_FN_DISCONNECT;

   psMsgHeader->bFunctionCode = ( bDirection == MSAC_C2_REQ_DIRECTION ) ? MSAC_C2_FC_SRD_L : FC_RESP_L;
}//void MSAC_C2_CreateDisconnectPdu( MSAC_C2_CONNECT_ITEM_PTR psConnection, UBYTE bDirection )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* void MSAC_C2_SendResponseData( MSAC_C2_CONNECT_ITEM_PTR psConnection )    */
/*                                                                           */
/*   function: internal function                                             */
/*             send response data to fdl  (without wait pdu)                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_SendResponseData( MSAC_C2_CONNECT_ITEM_PTR psConnection )
{
   //There was a timeout. The timeout-request
   //is behind this request in the input queue.
   //Set a note and reject later the timeout-request.
   if( MSAC_C2_StartTimer( psConnection->bTimerIndex, psConnection->wSendTimeOut) == MSAC_C2_TIMER_OK )
   {
      psConnection->bTimerBreak = TRUE;
   }//if( MSAC_C2_StartTimer( psConnection->bTimerIndex, psConnection->wSendTimeOut) == MSAC_C2_TIMER_OK )

   FDL_RespProvide( psConnection->bSapNr, FDL_PRIMARY_BUF );
}//void MSAC_C2_SendResponseData( MSAC_C2_CONNECT_ITEM_PTR psConnection )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  void MSAC_C2_CallIndAwait (MSAC_C2_CONNECT_ITEM_PTR psConnection)        */
/*                                                                           */
/*   function: Set the connection state of MSAC_C2_CS_AWAIT_CONNECT, call    */
/*             the output function MSAC_C2_IND_AWAIT() and check the return  */
/*             code of the function.                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_CallIndAwait( MSAC_C2_CONNECT_ITEM_PTR psConnection )
{
UBYTE bError;

   //new description state */
   //BugFix 503
   psConnection->eState        = MSAC_C2_CS_AWAIT_CONNECT;
   psConnection->eUserBreak   = MSAC_C2_UB_NO_BREAK;
   psConnection->bTimerBreak  = FALSE;
   psConnection->bTimerActive = FALSE;

   bError = FDL_IndAwait( psConnection->bSapNr );
   if( bError != FDL_OK )
   {
      sVpc3Error.bErrorCode = bError;
      sVpc3Error.bCnId      = psConnection->bCnId;
      // *** no further action of dpv1/c2 ***
      FatalError( _DP_C2, __LINE__, &sVpc3Error );
   }//if( bError != FDL_OK )
}//void MSAC_C2_CallIndAwait( MSAC_C2_CONNECT_ITEM_PTR psConnection )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* MSAC_C2_HandleUserResponse (MSAC_C2_CONNECT_ITEM_PTR psConnection )       */
/*                                                                           */
/*   function: internal function                                             */
/*             check user data and set the reaction (no call of              */
/*             MSAC_C2_SendResponseData, no set of connection description)   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_HandleUserResponse( MSAC_C2_CONNECT_ITEM_PTR psConnection )
{
MSG_HEADER_PTR psMsgHeader;
UBYTE          bC2Service;
UBYTE          bResponseOk;

   psMsgHeader = psConnection->psMsgHeader;
   bC2Service = psMsgHeader->bDpv1Service & MSAC_C2_FN_MASK;
   bResponseOk = TRUE;

   if( ( bC2Service != psConnection->bC2Service ) && ( bC2Service != MSAC_C2_FN_DISCONNECT ) )
   {
      bResponseOk = FALSE;
   }//if( ( bC2Service != psConnection->bC2Service ) && ( bC2Service != MSAC_C2_FN_DISCONNECT ) )
   else
   {
      switch( bC2Service )
      {
         case MSAC_C2_FN_CONNECT:
         {
            //negativ response connection state not opened
            if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
            {
               //negativ response by FN_CONNECT, internal disconnect
               psConnection->bC2Service = MSAC_C2_FN_DISCONNECT;

               if( psMsgHeader->bIndicationLength < DPV1_LEN_NEG_RESPONSE )
               {
                  bResponseOk = FALSE;
               }//if( psMsgHeader->bIndicationLength < DPV1_LEN_NEG_RESPONSE )
            }//if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
            else
            {
               //positiv response
               if( psMsgHeader->bIndicationLength < MSAC_C2_MIN_CC_PDU_SIZE )
               {
                  bResponseOk = FALSE;
               }//if( psMsgHeader->bIndicationLength < MSAC_C2_MIN_CC_PDU_SIZE )
            }//else of if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
            break;
         }//case MSAC_C2_FN_CONNECT:

         case MSAC_C2_FN_DATA:
         {
             if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
             {
                //new negative response
                if( psMsgHeader->bIndicationLength != DPV1_LEN_NEG_RESPONSE )
                {
                   bResponseOk = FALSE;
                }//if( psMsgHeader->bIndicationLength != DPV1_LEN_NEG_RESPONSE )
             }//if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
             else
             {
                if(    ( psMsgHeader->bIndicationLength != ( psMsgHeader->bDataLength + DPV1_LEN_HEAD_DATA ) )
                    || ( psMsgHeader->bDataLength > MSAC_C2_MAX_PDU_SIZE )
                  )
                {
                   bResponseOk = FALSE;
                }//if(    ( psMsgHeader->bIndicationLength != ( psMsgHeader->bDataLength + DPV1_LEN_HEAD_DATA ) ) ...
             }//else of if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
             break;
         }//case MSAC_C2_FN_DATA:

         case MSAC_C2_FN_DS_READ:
         case MSAC_C2_FN_DS_WRITE:
         {
            if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
            {
               if( psMsgHeader->bIndicationLength != DPV1_LEN_NEG_RESPONSE )
               {
                  bResponseOk = FALSE;
               }//if( psMsgHeader->bIndicationLength != DPV1_LEN_NEG_RESPONSE )
            }//if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
            else
            {
               if( bC2Service == MSAC_C2_FN_DS_READ )
               {
                  if( psMsgHeader->bIndicationLength != ( psMsgHeader->bDataLength + DPV1_LEN_HEAD_DATA ) )
                  {
                     bResponseOk = FALSE;
                  }//if( psMsgHeader->bIndicationLength != ( psMsgHeader->bDataLength + DPV1_LEN_HEAD_DATA ) )
               }//if( bC2Service == MSAC_C2_FN_DS_READ )
               else
               {
                  if( psMsgHeader->bIndicationLength != DPV1_LEN_HEAD_DATA )
                  {
                     bResponseOk = FALSE;
                  }//if( psMsgHeader->bIndicationLength != DPV1_LEN_HEAD_DATA )
               }//else of if( bC2Service == MSAC_C2_FN_DS_READ )

               if( bResponseOk == TRUE )
               {
                  //BugFix 503
                  #if DPV1_IM_SUPP
                     if(    ( psMsgHeader->bSlotNr == 255 )
                         || ( psMsgHeader->bDataLength > MSAC_C2_MAX_PDU_SIZE )
                       )
                     {
                        bResponseOk = FALSE;
                     }// if(    ( psMsgHeader->bSlotNr == 255 )
                  #else
                     if(    ( psMsgHeader->bSlotNr == 255 )
                         || ( psMsgHeader->bIndex  == 255 )
                         || ( psMsgHeader->bDataLength > MSAC_C2_MAX_PDU_SIZE )
                       )
                     {
                        bResponseOk = FALSE;
                     }//if(    ( psMsgHeader->bSlotNr == 255 ) ...
                  #endif//#if DPV1_IM_SUPP
               }//if( bResponseOk == TRUE )
            }//else of if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
            break;
         }//case MSAC_C2_FN_DS_READ:

         case MSAC_C2_FN_DISCONNECT:
         {
            psConnection->bC2Service = MSAC_C2_FN_DISCONNECT;
            break;
         }//case MSAC_C2_FN_DISCONNECT:

         default:
         {
            bResponseOk = FALSE;
            break;
         }//default:
      }//switch( bC2Service )
   }//if( ( bC2Service != psConnection->bC2Service ) && ( bC2Service != MSAC_C2_FN_DISCONNECT ) )

   if( bResponseOk )
   {
      //response data_ok
      switch( bC2Service )
      {
         case MSAC_C2_FN_CONNECT:
         {
            //negative response is now handled
            if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
            {
                psConnection->eErrorCode = MSAC_C2_EC_DC_BY_USER;
            }//if( ( psMsgHeader->bDpv1Service & DPV1_ERROR_BIT_RESPONSE ) != 0 )
            break;
         }//case MSAC_C2_FN_CONNECT:

         case MSAC_C2_FN_DISCONNECT:
         {
            psConnection->eErrorCode = MSAC_C2_EC_DC_BY_USER;
            break;
         }//case MSAC_C2_FN_DISCONNECT:

         default:
         {
            //do nothing
            break;
         }//default:
      }//switch( bC2Service )
   }//if( bResponseOk )
   else
   {
      //invalid response data
      psConnection->eErrorCode = MSAC_C2_EC_USER_ERR;
      //handle response data as disconnect-pdu
      MSAC_C2_CreateDisconnectPdu( psConnection, MSAC_C2_RESP_DIRECTION );
   }//else of if( bResponseOk )
}//void MSAC_C2_HandleUserResponse( MSAC_C2_CONNECT_ITEM_PTR psConnection )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   void MSAC_C2_RespPduProvide( UBYTE bSapNr,                              */
/*                                  SAC_C2_DATA_BUF_PTR buf_ptr )            */
/*                                                                           */
/*   function: MSAC_C2 received response data from the user. In this         */
/*             function the response data are checked. In dependence of the  */
/*             pdu-service the response data are given to the layer 2        */
/*             (DATA_TRANSPORT and DISCONNECT) or the productiv data sap is  */
/*             the productiv data sap is activated (CONNECT).                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_RespPduProvide( UBYTE bSapNr )
{
MSAC_C2_CONNECT_ITEM_PTR psConnection;

   //determine psConnection
   psConnection = &(pDpSystem->sC2.asConnectionList[MSAC_C2_SAP_NR_HIGH - bSapNr]);

   switch( psConnection->eState )
   {
      case MSAC_C2_CS_PROVIDE_IND:   //wait for response data
      {
         switch( psConnection->eErrorCode )
         {
            case MSAC_C2_EC_OK:
            {
               //new description state */
               psConnection->eState = MSAC_C2_CS_PROVIDE_RESP;
               MSAC_C2_HandleUserResponse( psConnection );
               MSAC_C2_SendResponseData( psConnection );
               break;
            }//case MSAC_C2_EC_OK:

            case MSAC_C2_EC_USER_ERR:
            case MSAC_C2_EC_INV_S_D_LEN_ERR:
            case MSAC_C2_EC_REQ_TIMEOUT:
            case MSAC_C2_EC_DC_BY_USER:
            case MSAC_C2_EC_DC_BY_MASTER:
            case MSAC_C2_EC_REMOTE_ERROR:
            default:
            {
               sVpc3Error.bErrorCode = psConnection->eErrorCode;
               sVpc3Error.bCnId      = MSAC_C2_SAP_NR_HIGH - bSapNr;
               // *** no further action of dpv1/c2 ***
               FatalError( _DP_C2, __LINE__, &sVpc3Error );
            }//default:
         }//switch( psConnection->eErrorCode )
         break;
      }//case MSAC_C2_CS_PROVIDE_IND:

      case MSAC_C2_CS_CLOSE_CHANNEL:
      case MSAC_C2_CS_AWAIT_CONNECT:
      case MSAC_C2_CS_AWAIT_IND:
      case MSAC_C2_CS_PROVIDE_RESP:
      case MSAC_C2_CS_DISABLE_CONNECT:
      default:
      {
         sVpc3Error.bErrorCode = psConnection->eState;
         sVpc3Error.bCnId      = MSAC_C2_SAP_NR_HIGH - bSapNr;
         // *** no further action of dpv1/c2 ***
         FatalError( _DP_C2, __LINE__, &sVpc3Error );
         break;
      }//default:
   }//switch( psConnection->eState )
}//void MSAC_C2_RespPduProvide( UBYTE bSapNr )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   void MSAC_C2_CloseChannel( void )                                       */
/*                                                                           */
/*   function:     implementation of close_channel function                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_CloseChannel( void )
{
   if( ( pDpSystem->sC2.bCloseChannelRequest == FALSE ) && ( pDpSystem->sC2.asConnectionList[0].eState != MSAC_C2_CS_CLOSE_CHANNEL ) )
   {
      //accept close_channel request
      pDpSystem->sC2.bCloseChannelRequest = TRUE;

      FDL_CloseChannel( FDL_MSAC_C2_SAP );
   }//if( ( pDpSystem->sC2.bCloseChannelRequest == FALSE ) && ( pDpSystem->sC2.asConnectionList[0].eState != MSAC_C2_CS_CLOSE_CHANNEL ) )
}//void MSAC_C2_CloseChannel( void )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   void MSAC_C2_InputQueue( MSAC_C2_DATA_BUF_PTR buf_ptr, BYTE bRetValue   */
/*                             UBYTE user_id, UBYTE bFdlCode)                */
/*                                                                           */
/*   function:     copy message into msac_c2 input queue                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void MSAC_C2_InputQueue( UBYTE bSapNr, UBYTE bRetValue, UBYTE bFdlCode, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
MSAC_C2_REC_QUEUE_PTR  psRecQueue;

   psRecQueue = &(pDpSystem->sC2.asRecQueue[pDpSystem->sC2.bWriteRecPtr]);

   psRecQueue->psMsgHeader = psMsgHeader;
   psRecQueue->pToDpv1Data = pToDpv1Data;
   psRecQueue->bRetValue   = bRetValue;
   psRecQueue->bSapNr      = bSapNr;
   psRecQueue->bFdlCode    = bFdlCode;

   if( pDpSystem->sC2.bWriteRecPtr != pDpSystem->sC2.bEndRecPtr )
   {
      pDpSystem->sC2.bWriteRecPtr++;
   }//if( pDpSystem->sC2.bWriteRecPtr != pDpSystem->sC2.bEndRecPtr )
   else
   {
      pDpSystem->sC2.bWriteRecPtr = pDpSystem->sC2.bStartRecPtr;
   }//else of if( pDpSystem->sC2.bWriteRecPtr != pDpSystem->sC2.bEndRecPtr )
}//void MSAC_C2_InputQueue( UBYTE bSapNr, UBYTE bRetValue, UBYTE bFdlCode, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )

/*--------------------------------------------------------------------------*/
/* function: MSAC_C2_UserForceAbort                                         */
/*--------------------------------------------------------------------------*/
UBYTE MSAC_C2_UserForceAbort( UBYTE bSapNr, UBYTE bReasonCode, UBYTE bLocation )
{
MSAC_C2_CONNECT_ITEM_PTR psConnection;
UBYTE                    bRetValue;

   if( pDpSystem->sC2.bEnabled )
   {
      if( ( bSapNr > MSAC_C2_SAP_NR_HIGH ) || ( ( MSAC_C2_SAP_NR_HIGH - bSapNr ) >=  pDpSystem->sC2.bNrOfSap ) )
      {
         bRetValue = C2_DATA_SAP_ERROR;
      }//if( ( bSapNr > MSAC_C2_SAP_NR_HIGH ) || ( ( MSAC_C2_SAP_NR_HIGH - bSapNr ) >=  pDpSystem->sC2.bNrOfSap ) )
      else
      {
         // determine pointer of connection description
         psConnection = &( pDpSystem->sC2.asConnectionList[ MSAC_C2_SAP_NR_HIGH - bSapNr ] );

         //check, if double request
         if( psConnection->eUserBreak != MSAC_C2_UB_NO_BREAK )
         {
             bRetValue = C2_DOUBLE_REQUEST;
         }//if( psConnection->eUserBreak != MSAC_C2_UB_NO_BREAK )
         else
         {
            //check connection state
            switch( psConnection->eState )
            {
               case MSAC_C2_CS_CLOSE_CHANNEL:
               case MSAC_C2_CS_AWAIT_CONNECT:
               {
                  bRetValue = C2_ALREADY_DISCONNECTED;
                  break;
               }//case MSAC_C2_CS_CLOSE_CHANNEL:

               default:
               {
                  psConnection->eUserBreak = MSAC_C2_UB_BREAK;
                  psConnection->bDrReasonCode = bReasonCode;
                  psConnection->bDrLocation = bLocation;
                  bRetValue = DP_OK;
                  break;
               }//default:
            }//switch( psConnection->eState )
         }//else of if( psConnection->eUserBreak != MSAC_C2_UB_NO_BREAK )
      }//else of if( ( bSapNr > MSAC_C2_SAP_NR_HIGH ) || ( ( MSAC_C2_SAP_NR_HIGH - bSapNr ) >=  pDpSystem->sC2.bNrOfSap ) )
   }//if( pDpSystem->sC2.bEnabled )
   else
   {
      bRetValue = C2_ENABLED_ERROR;
   }//else of if( pDpSystem->sC2.bEnabled )

   return bRetValue;
}//UBYTE MSAC_C2_UserForceAbort( UBYTE bSapNr, UBYTE bReasonCode, UBYTE bLocation )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   void MSAC_C2_TimerTick10msec( void )                                    */
/*                                                                           */
/*   function:     10msec Timer Function for MSAC_2 connections,             */
/*                 called by interrupt function.                             */
/*---------------------------------------------------------------------------*/
void MSAC_C2_TimerTick10msec( void )
{
UBYTE i;

   #ifdef REDUNDANCY
      if( sRedCom.bRedTimerTick & pDpSystem->bChannel )
      {
          sRedCom.bRedTimerTick &= ~pDpSystem->bChannel;

   #endif//#ifdef REDUNDANCY

          for( i = 0; i < DP_C2_NUM_SAPS; i++ )
          {
             if( pDpSystem->sC2.asTimerList[i].bRunning != FALSE )
             {
                if( pDpSystem->sC2.asTimerList[i].wActValue )
                {
                   pDpSystem->sC2.asTimerList[i].wActValue--;
                }//if( pDpSystem->sC2.asTimerList[i].wActValue )
                else
                {
                   pDpSystem->sC2.asTimerList[i].bRunning = FALSE;
                }//else of if( pDpSystem->sC2.asTimerList[i].wActValue )
             }//if( pDpSystem->sC2.asTimerList[i].bRunning != FALSE )
          }//for( i = 0; i < DP_C2_NUM_SAPS; i++ )

   #ifdef REDUNDANCY
      }//if( sRedCom.bRedTimerTick & pDpSystem->bChannel )
   #endif//#ifdef REDUNDANCY
}//void MSAC_C2_TimerTick10msec( void )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   UBYTE MSAC_C2_StartTimer( UBYTE bTimerIndex, UWORD wTimerValue )        */
/*                                                                           */
/*   function:     start timer                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
UBYTE MSAC_C2_StartTimer( UBYTE bTimerIndex, UWORD wTimerValue )
{
UBYTE bRetValue;

   //set new reload-value
   pDpSystem->sC2.asTimerList[ bTimerIndex ].wActValue = wTimerValue;
   if( pDpSystem->sC2.asTimerList[ bTimerIndex ].bRunning == TRUE )
   {
      bRetValue = MSAC_C2_TIMER_ALREADY_RUNS;
   }//if( pDpSystem->sC2.asTimerList[ bTimerIndex ].bRunning == TRUE )
   else
   {
      pDpSystem->sC2.asTimerList[ bTimerIndex ].bRunning = TRUE;
      bRetValue = MSAC_C2_TIMER_OK;
   }//else of if( pDpSystem->sC2.asTimerList[ bTimerIndex ].bRunning == TRUE )

   return bRetValue;
}//UBYTE MSAC_C2_StartTimer( UBYTE bTimerIndex, UWORD wTimerValue )

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   UBYTE MSAC_C2_StopTimer( UBYTE bTimerIndex )                            */
/*                                                                           */
/*   function:     stop timer                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
UBYTE MSAC_C2_StopTimer( UBYTE bTimerIndex )
{
UBYTE bRetValue;

   if( pDpSystem->sC2.asTimerList[ bTimerIndex ].bRunning == FALSE )
   {
      bRetValue = MSAC_C2_TIMER_ALREADY_STOPPED;
   }//if( pDpSystem->sC2.asTimerList[ bTimerIndex ].bRunning == FALSE )
   else
   {
      pDpSystem->sC2.asTimerList[ bTimerIndex ].wActValue = 0x0000;
      pDpSystem->sC2.asTimerList[ bTimerIndex ].bRunning   = FALSE;
      bRetValue = MSAC_C2_TIMER_OK;
   }//else of if( pDpSystem->sC2.asTimerList[ bTimerIndex ].bRunning == FALSE )

   return bRetValue;
}//UBYTE MSAC_C2_StopTimer( UBYTE bTimerIndex )

#endif//#if DP_MSAC_C2


/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

