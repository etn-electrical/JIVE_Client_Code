/**************************  Filename: dp_inc.h  *****************************/
/* ========================================================================= */
/*                                                                           */
/* 0000  000   000  00000 0  000  0   0 0 0000                               */
/* 0   0 0  0 0   0 0     0 0   0 0   0 0 0   0      Einsteinstrae 6        */
/* 0   0 0  0 0   0 0     0 0     0   0 0 0   0      91074 Herzogenaurach    */
/* 0000  000  0   0 000   0 0     00000 0 0000       Germany                 */
/* 0     00   0   0 0     0 0     0   0 0 0                                  */
/* 0     0 0  0   0 0     0 0   0 0   0 0 0          Tel: ++49-9132-744-200  */
/* 0     0  0  000  0     0  000  0   0 0 0    GmbH  Fax: ++49-9132-744-204  */
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/* Description: Extern modules to be included in all DP files.               */
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


/*****************************************************************************/
/* contents:

    - header include hierarchy for system environment

*/
/*****************************************************************************/
/* reinclude protection */


#ifndef DP_INC_H
#define DP_INC_H


/*****************************************************************************/
/* header include hierarchy for system environment */

/*------------------------------------------------------------------------*/
/* Debug Codes                                                            */
/*------------------------------------------------------------------------*/
typedef enum
{
   START__                          = ((UBYTE)0x01),

   DP_INTERRUPT__                   = ((UBYTE)0x20),
   DP_INTERRUPT_PRM__               = ((UBYTE)0x21),
   DP_INTERRUPT_CFG__               = ((UBYTE)0x22),

   FdlIndicationIsr__               = ((UBYTE)0x30),
   FdlPollEndIsr__                  = ((UBYTE)0x31),

   MSAC_C2_INPUT_QUEUE__            = ((UBYTE)0x40),
   MSAC_C2_PROCESS__                = ((UBYTE)0x41),
   MSAC_C2_PRU_PROVIDE__            = ((UBYTE)0x42),
   MSAC_C2_REALIZE_IND_REC__        = ((UBYTE)0x43),
   MSAC_C2_REALIZE_RESP_SENT__      = ((UBYTE)0x44),
   MSAC_C2_CREATE_DISCONNECT_PDU__  = ((UBYTE)0x45),
   MSAC_C2_SEND_RESPONSE_DATA__     = ((UBYTE)0x46),
   MSAC_C2_HANDLE_USER_RESPONSE__   = ((UBYTE)0x47),
   MSAC_C2_RESP_PDU_PROVIDE__       = ((UBYTE)0x48),

   USER_FORCE_ABORT__               = ((UBYTE)0x49),

   RED_SM__                         = ((UBYTE)0x55),

   RED_RISM_ST_POWER_ON             = ((UBYTE)0x60),
   RED_RISM_ST_S_WAITING            = ((UBYTE)0x61),
   RED_RISM_ST_S_PRIMARY            = ((UBYTE)0x62),
   RED_RISM_ST_C_CONFIGURE          = ((UBYTE)0x63),
   RED_RISM_ST_BACKUP               = ((UBYTE)0x64),
   RED_RISM_ST_BP_PARTNER_ACK       = ((UBYTE)0x65),
   RED_RISM_ST_BP_SWITCHOVER        = ((UBYTE)0x66),
   RED_RISM_ST_BP_PRM_CMD           = ((UBYTE)0x67),
   RED_RISM_ST_BP_DX                = ((UBYTE)0x68),
   RED_RISM_ST_PRIMARY              = ((UBYTE)0x69),
   RED_RISM_ST_PB_PARTNER_ACK       = ((UBYTE)0x6A),
   RED_RISM_ST_PB_SWITCHOVER        = ((UBYTE)0x6B),
   RED_RISM_ST_NIL                  = ((UBYTE)0x6C),

   FatalError__                     = ((UBYTE)0xFF)
} DEBUG_CODE;

/*------------------------------------------------------------------------*/
/* Debug Structure                                                        */
/*------------------------------------------------------------------------*/
#define MAX_NR_OF_DEBUG     256

typedef struct
{
   DEBUG_CODE  bDebugCode;
   UBYTE       bDetail1;
   UBYTE       bDetail2;
} sDP_DEBUG_BUFFER_ITEM;
#define psDP_BUFFER_ITEM   sDP_DEBUG_BUFFER_ITEM  PTR_ATTR *

/*------------------------------------------------------------------------*/
/* Debug function, external defines                                       */
/*------------------------------------------------------------------------*/
#ifdef DP_DEBUG_ENABLE
   #define DP_WriteDebugBuffer__( _bDebugCode, _bDetail1, _bDetail2 )\
                                         DpWriteDebugBuffer( _bDebugCode, _bDetail1, _bDetail2 )

   extern sDP_DEBUG_BUFFER_ITEM    asDebugBuffer[ MAX_NR_OF_DEBUG ];
   extern UBYTE                    bDebugBufferIndex;
   extern UBYTE                    bDebugBufferOverlapped;

   extern void DpClearDebugBuffer( void );
#else
   #define DP_WriteDebugBuffer__( _bDebugCode, _bDetail1, _bDetail2 )\
                                         /* not used ( DP_WriteDebugBuffer__ ) */
#endif//#ifdef DP_DEBUG_ENABLE


/*------------------------------------------------------------------------*/
/* dp_user.c                                                              */
/*------------------------------------------------------------------------*/
extern VPC3_STRUC      VPC3_PTR     pVpc3;     // pointer to Vpc3 structure
extern VPC3_SYSTEM_STRUC            sDpSystem; // global system structure

/*------------------------------------------------------------------------*/
/* main.c                                                                 */
/*------------------------------------------------------------------------*/
extern void                   Swap16                        ( UWORD MEM_PTR w );
extern void                   SetResetVPC3Channel1          ( void );
extern void                   ClrResetVPC3Channel1          ( void );
extern void                   SetResetVPC3Channel2          ( void );
extern void                   ClrResetVPC3Channel2          ( void );
extern void                   EnableInterruptVPC3Channel1   ( void );
extern void                   DisableInterruptVPC3Channel1  ( void );
extern void                   EnableInterruptVPC3Sync       ( void );
extern void                   DisableInterruptVPC3Sync      ( void );
extern void                   EnableAllInterrupts           ( void );
extern void                   DisableAllInterrupts          ( void );
extern void                   Vpc3Wait_1ms                  ( void );

#if VPC3_SERIAL_MODE
   extern VPC3*  VPC3_ctrl;
   #define Vpc3Write( wAddress, bData )  VPC3_ctrl->Write( wAddress, bData )
   #define Vpc3Read( wAddress )    VPC3_ctrl->Read( wAddress )
   #define CopyToVpc3_( _pToVpc3Memory, _pLocalMemory, _wLength )\
               VPC3_ctrl->CopyToVpc3( _pToVpc3Memory, _pLocalMemory, _wLength )

   #define CopyFromVpc3_( _pLocalMemory, _pToVpc3Memory, _wLength )\
               VPC3_ctrl->CopyFromVpc3( _pLocalMemory, _pToVpc3Memory, _wLength )

   #define Vpc3MemSet_( _pToVpc3Memory, _bValue, _wLength )\
               VPC3_ctrl->MemSet( _pToVpc3Memory, _bValue, _wLength )

   #define Vpc3MemCmp_( _pToVpc3Memory1, _pToVpc3Memory2, _wLength )\
               VPC3_ctrl->MemCmp( _pToVpc3Memory1, _pToVpc3Memory2, _wLength )
   
   //SSN This definition is for VPC3_dp_fdl.cpp
  #define Vpc3MemSet( _pToVpc3Memory, _bValue, _wLength )\
               VPC3_ctrl->MemSet( _pToVpc3Memory, _bValue, _wLength )  
#else

   #define CopyToVpc3_( _pToVpc3Memory, _pLocalMemory, _wLength )\
               memcpy( _pToVpc3Memory, _pLocalMemory, _wLength )

   #define CopyFromVpc3_( _pLocalMemory, _pToVpc3Memory, _wLength )\
               memcpy( _pLocalMemory, _pToVpc3Memory, _wLength )

   #define Vpc3MemSet_( _pToVpc3Memory, _bValue, _wLength )\
               memset( _pToVpc3Memory, _bValue, _wLength )

   #define Vpc3MemCmp_( _pToVpc3Memory1, _pToVpc3Memory2, _wLength )\
               memcmp( _pToVpc3Memory1, _pToVpc3Memory2, _wLength )

#endif//#if VPC3_SERIAL_MODE


/*------------------------------------------------------------------------*/
/* dp_user.c                                                              */
/*------------------------------------------------------------------------*/
extern void                   ProfibusInit                  ( uint8_t Slave_Addr );
extern void                   ProfibusMain                  ( void );
extern void                   FatalError                    ( DP_ERROR_FILE file, UWORD line, VPC3_ERRCB_PTR errcb_ptr );
extern void                   UserAlarmAckReq               ( ALARM_STATUS_PDU_PTR psAlarm );
extern ALARM_STATUS_PDU_PTR   UserAlarmAlloc                ( void );
extern void                   UserAlarmInit                 ( void );
extern void                   UserFreeAlarmBuffer           ( ALARM_STATUS_PDU_PTR alarm_ptr );
extern void                   UserResetDiagnosticBuffer     ( void );

extern DP_ERROR_CODE          UserChkNewPrmData             ( MEM_UNSIGNED8_PTR pbPrmData, UBYTE bPrmLength );
extern DP_ERROR_CODE          UserChkNewExtPrmData          ( MEM_UNSIGNED8_PTR pbPrmData, UBYTE bPrmLength );
extern UBYTE                  UserChkNewCfgData             ( MEM_UNSIGNED8_PTR pbCfgData, UBYTE bCfgLength );
extern void                   UserIsrGoLeaveDataExchange    ( void );
extern void                   UserIsrDxOut                  ( void );
extern void                   UserIsrDiagBufferChanged      ( void );
extern void                   UserIsrNewWdDpTimeout         ( void );
extern void                   UserIsrBaudrateDetect         ( void );
extern void                   UserIsrNewGlobalControlCommand( void );
extern void                   UserIsrNewSetSlaveAddress     ( void );
extern void                   UserIsrTimerClock             ( void );

extern void                   UserClearIdentRelatedDiag     ( void );

#if DP_TIMESTAMP
   extern void                UserIsrClockSynchronisation   ( void );
#endif//#if DP_TIMESTAMP

#if DP_SUBSCRIBER
   extern void                UserIsrDxbOut                 ( void );
   extern void                UserIsrDxbLinkError           ( void );
#endif//#if DP_SUBSCRIBER

#if DP_ISOCHRONOUS_MODE
   extern void                UserIsoWriteOutputData        ( void );
   extern void                UserIsoReadInputData          ( void );
#endif//#if DP_ISOCHRONOUS_MODE

#if DP_FDL
   extern DPV1_RET_VAL        UserDpv1ReadReq               ( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
   extern DPV1_RET_VAL        UserDpv1WriteReq              ( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );

   #if DP_MSAC_C2
      extern DPV1_RET_VAL     UserMsac2InitiateReq          ( UBYTE bSapNr, INITIATE_REQ_PDU_PTR psInitiateReq, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
      extern DPV1_RET_VAL     UserMsac2DataTransportReq     ( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
      extern DPV1_RET_VAL     UserMsac2AbortInd             ( UBYTE bSapNr );
   #endif//#if DP_MSAC_C2

   extern UBYTE               UserAlarm                     ( UBYTE bAlarmType, UBYTE bSeqNr, ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCheckDiagFlag );
#endif //#if DP_FDL

#ifdef REDUNDANCY
   extern DP_ERROR_CODE       InitRedundancyChannel         ( UBYTE bAddress );

   #ifdef RS232_REDUNDANCY
      extern void             InitRedundancySerio           ( UWORD wBaudrate );
      extern void             ParseRS232                    ( void );
      extern void             StartSerialCommunication      ( void );
   #endif//#ifdef RS232_REDUNDANCY
#endif//#ifdef REDUNDANCY

/*------------------------------------------------------------------------*/
/* dp_if.c                                                                */
/*------------------------------------------------------------------------*/

extern void                   GetFirmwareVersion            ( psDP_VERSION pVersion );
extern DP_ERROR_CODE          VPC3_MemoryTest               ( void );
extern DP_ERROR_CODE          VPC3_Initialization           ( UBYTE bSlaveAddress, UWORD wIdentNumber, psCFG psCfgData );
extern void                   VPC3_Start                    ( void );
extern void                   VPC3_Stop                     ( void );
extern void                   VPC3_ProcessDpv1StateMachine  ( void );
extern DP_ERROR_CODE          VPC3_CalculateInpOutpLength   ( MEM_UNSIGNED8_PTR pToCfgData, UBYTE bCfgLength );
extern void                   VPC3_SetIoDataLength          ( void );
extern VPC3_UNSIGNED8_PTR     VPC3_GetDinBufPtr             ( void );
extern void                   VPC3_InputDataUpdate          ( MEM_UNSIGNED8_PTR pToInputData );
extern VPC3_UNSIGNED8_PTR     VPC3_GetDoutBufPtr            ( MEM_UNSIGNED8_PTR pbState );
extern VPC3_UNSIGNED8_PTR     VPC3_GetDiagBufPtr            ( void );
extern UBYTE                  VPC3_GetMasterAddress         ( void );
extern DP_ERROR_CODE          VPC3_SetDiagnosis             ( MEM_UNSIGNED8_PTR pbToDiagBuffer, UBYTE bUserDiagLength, UBYTE bDiagControl, UBYTE bCheckDiagFlag );
extern UWORD                  VPC3_GetFreeMemory            ( void );
extern UBYTE                  VPC3_SetAlarm                 ( ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCallback );

#if DP_SUBSCRIBER
   extern DP_ERROR_CODE       VPC3_SubscriberToLinkTable    ( MEM_PRM_SUBSCRIBER_TABLE_PTR psSubsTable, UBYTE bNrOfLinks );
   extern VPC3_UNSIGNED8_PTR  VPC3_GetDxbBufPtr             ( MEM_UNSIGNED8_PTR pbState );
   extern void                VPC3_BuildDxbLinkStatus       ( UBYTE bNrOfLinks );
   extern DP_ERROR_CODE       VPC3_CheckDxbLinkTable        ( UBYTE bNrOfLinks );
#endif//#if DP_SUBSCRIBER

#ifdef DP_DEBUG_ENABLE
   extern void                DpWriteDebugBuffer            ( DEBUG_CODE bDebugCode, UBYTE bDetail1, UBYTE bDetail2 );
#endif//#ifdef DP_DEBUG_ENABLE

/*------------------------------------------------------------------------*/
/* dp_isr.c                                                               */
/*------------------------------------------------------------------------*/
extern void                   VPC3_Isr                       ( void );
extern void                   VPC3_Poll                      ( void );

/*------------------------------------------------------------------------*/
/* dp_fdl.c                                                               */
/*------------------------------------------------------------------------*/
#if DP_FDL
   extern DP_ERROR_CODE       FDL_Init                      ( UBYTE bSlaveAddress );
   extern void                FDL_SetPollTimeOutIsr         ( UWORD wSendTimeOut );
   extern void                FDL_IndicationIsr             ( void );
   extern void                FDL_PollEndIsr                ( void );
   extern void                FDL_CyclicService             ( void );
   extern void                FDL_SetSaSsap                 ( UBYTE bSapNr, UBYTE bReqSa, UBYTE bReqSsap );
   extern FDL_SAP_HEADER_PTR  FDL_GetSapHeaderPtr           ( UBYTE bSapNr );
   extern void                FDL_RespProvide               ( UBYTE bSapNr, UBYTE bPrimSecBuf );
   extern void                FDL_ResetCntrlRespBit         ( UBYTE bSapNr, UBYTE bPrimSecBuf );
   extern void                FDL_IndDisable                ( UBYTE bSapNr );
   extern UBYTE               FDL_IndAwait                  ( UBYTE bSapNr );
   extern void                FDL_PerformSapReset           ( UBYTE bSapNr );
   extern void                FDL_PerformIndDisable         ( FDL_SAP_HEADER_PTR psFdlHeader );
   extern UBYTE               FDL_OpenChannel               ( void );
   extern void                FDL_CloseChannel              ( FDL_SAP_TYPE eSapType );
   extern void                FDL_IndDisableSAP50           ( void );
   extern void                FDL_PerformIndDisableSAP50    ( FDL_SAP_HEADER_PTR psFdlHeader );
   extern void                FDL_SetDpv1ResponseLength     ( MSG_HEADER_PTR psMsgHeader, UBYTE bLength );
   extern DPV1_RET_VAL        FDL_SetDpv1ErrorCode          ( MSG_HEADER_PTR psMsgHeader, UBYTE bError1, UBYTE bError2 );
#endif //#if DP_FDL

/*------------------------------------------------------------------------*/
/* dp_msac1.c                                                             */
/*------------------------------------------------------------------------*/
#if DP_MSAC_C1
   extern DP_ERROR_CODE       MSAC_C1_Init                  ( void );
   extern UBYTE               MSAC_C1_CheckIndNewPrmData    ( MEM_STRUC_PRM_PTR psPrmData, UBYTE bPrmLength );
   extern void                MSAC_C1_PduReceived           ( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
   extern void                MSAC_C1_Process               ( void );
   extern void                MSAC_C1_LeaveDx               ( void );
   extern void                MSAC_C1_CheckIndDxOut         ( void );
   extern void                MSAC_C1_DoCfgNotOk            ( void );
   extern void                MSAC_C1_DoCfgOk               ( void );
   extern void                MSAC_C1_IndDisableDone        ( UBYTE bSapNr, UBYTE bRetValue );
   extern UBYTE               MSAC_C1_TransmitDelay         ( UBYTE bSapNr, UBYTE bDpv1Service, DPV1_RET_VAL bDpv1RetValue );

   #if DP_ALARM
      extern void             AL_AlarmProcess               ( void );
      extern UBYTE            AL_SetAlarm                   ( ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCallback );
      extern void             AL_ClearAlarm                 ( UBYTE bAlarmTypeBitField, UBYTE bSeqNr );
      extern UBYTE            AL_AlarmEnabled               ( UBYTE bAlarmType );
      extern UBYTE            AL_CheckTypeStatus            ( UBYTE bAction, UBYTE bAlarmType );
   #endif//#if DP_ALARM

   #ifdef REDUNDANCY
      extern void             RedundancyStopMSAC1S          ( void );
      extern void             RedundancyStartMSAC1S         ( void );
      extern void             RedundancyResetMSAC1S         ( void );
   #endif//#ifdef REDUNDANCY
#endif//#if DP_MSAC_C1

/*------------------------------------------------------------------------*/
/* dp_msac2.c                                                             */
/*------------------------------------------------------------------------*/
#if DP_MSAC_C2
   extern DP_ERROR_CODE       MSAC_C2_Init                  ( void );
   extern void                MSAC_C2_Process               ( void );
   extern void                MSAC_C2_OpenChannelSap        ( UBYTE bSapNr );
   extern DP_ERROR_CODE       MSAC_C2_OpenChannel           ( void );
   extern UBYTE               MSAC_C2_InitiateReqToRes      ( INITIATE_REQ_PDU_PTR psInitiateReq, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
   extern void                MSAC_C2_InputQueue            ( UBYTE bSapNr, UBYTE bRetValue, UBYTE bFdlCode, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
   extern void                MSAC_C2_TimerTick10msec       ( void );
   extern UBYTE               MSAC_C2_TransmitDelay         ( UBYTE bSapNr, DPV1_RET_VAL bDpv1RetValue );
   extern void                MSAC_C2_SetTimeoutIsr         ( void );
   extern void                MSAC_C2_ResetStateMachine     ( void );
   extern UBYTE               MSAC_C2_UserForceAbort        ( UBYTE bSapNr, UBYTE bReasonCode, UBYTE bLocation );
#endif//#if DP_MSAC_C2

/*------------------------------------------------------------------------*/
/* dp_im.c                                                                */
/*------------------------------------------------------------------------*/
#if DP_FDL
   extern DPV1_RET_VAL        ImWriteReq                    ( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
   extern DPV1_RET_VAL        ImReadReq                     ( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
#endif //#if DP_FDL

/*------------------------------------------------------------------------*/
/* iso_if.c                                                               */
/*------------------------------------------------------------------------*/
#if DP_ISOCHRONOUS_MODE
   extern void                IsochronousInit               ( void );
   extern DP_ERROR_CODE       IsochronousPrm                ( UBYTE bDpv1Status3, psIsochron psIso, MEM_STRUC_PRM_BLOCK_ISO_PTR psIsoPrmBlock );
   extern void                IsochronousCfgHwPll           ( UBYTE bNrOfOutputs, UBYTE bNrOfInputs, psIsochron psIso );
   extern void                IsochronousIsr                ( void );
#endif//#if DP_ISOCHRONOUS_MODE

/*------------------------------------------------------------------------*/
/* time_if.c                                                              */
/*------------------------------------------------------------------------*/
#if DP_TIMESTAMP
   extern void                GetInputData                  ( MEM_UNSIGNED8_PTR pToData );

   extern void                TS_SetNewCsEvent              ( void );
   
   extern void                TimeStampProcess              ( void );
   extern void                TimeStampInit                 ( UBYTE bInitStopTheProcess );
   extern void                TimeStampTimer                ( void );
   extern void                TimeStampTimer10ms            ( void );
   extern void                TimeStampGC                   ( UBYTE bGlobalControl );
   extern void                TimeStampGoLeaveDx            ( UBYTE bDpState );
   extern void                TimeStampPrm                  ( DP_ERROR_CODE bPrmRetValue );
   extern DPV1_RET_VAL        TimeStampCsPrm                ( UBYTE bDPV1Alarm, MEM_STRUC_PRM_BLOCK_CS_PTR psCs );
   extern void                TimeStampCfg                  ( UBYTE bCfgRetValue );
   extern void                TimeStampApplicationReady     ( void );
   extern void                TimeStampProcessAlarmSend     ( UBYTE bSeqNr );
   extern void                TimeStampProcessAlarmAck      ( UBYTE bSeqNr );
   extern DPV1_RET_VAL        TimeStampDpv1Read             ( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
   extern DPV1_RET_VAL        TimeStampDpv1Write            ( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data );
   extern void                UserTimeStampParameter        ( UBYTE bSlotNr, UBYTE bRisingEdge, UBYTE bFallingEdge );
   extern void                UserTimeStampNewData          ( UBYTE bSlotNr, UBYTE bOldData, UBYTE bNewData );

   extern void                TimeStampSetChannel           ( UBYTE bChannel );
   extern void                TimeStampInitChannel          ( UBYTE bChannel );
   extern void                TimeStampSwitchover           ( UBYTE bRedState );
   extern void                TimeStampSwitchoverDone       ( UBYTE bRedState );
   extern void                RedDrvSwitchWaitingToBackup   ( UBYTE bChannel );
#endif//#if DP_TIMESTAMP

/*------------------------------------------------------------------------*/
/* red_drv.c                                                              */
/*------------------------------------------------------------------------*/
#ifdef REDUNDANCY
   extern void                InitRedDrv                    ( void );
   extern void                RedDrvTimer10ms               ( void );
   extern void                RedInstanceStateMachine       ( void );
   extern void                RedDrvBaudrateDetect          ( void );
   extern void                RedDrvGC                      ( UBYTE bCommand );
   extern void                RedDrvWdTimeout               ( void );
   extern void                RedDrvGoLeaveDx               ( UBYTE bDpState );
   extern UBYTE               RedDrvPrmCmd                  ( VPC3_STRUC_PRM_BLOCK_PRMCMD_PTR psPrmCmd );
   extern void                RedDrvGetRedState             ( VPC3_UNSIGNED8_PTR pToData );
   
   extern DP_ERROR_CODE       InitRedundancyChannel_1       ( UBYTE bAddress );
   extern DP_ERROR_CODE       InitRedundancyChannel_2       ( UBYTE bAddress );
   
   extern void                SendDebug                     ( UBYTE bStatus1, UBYTE bStatus2, UBYTE bStatus3 );
#endif//#ifdef REDUNDANCY

/*****************************************************************************/
/* reinclude-protection */


#else
    #pragma message "The header DP_INC.H is included twice or more !"
#endif


/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

