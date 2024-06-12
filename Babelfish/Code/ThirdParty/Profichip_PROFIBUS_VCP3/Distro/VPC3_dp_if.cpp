/***********************  Filename: dp_if.c  *********************************/
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
/* Function:       interface service routines for VPC3+ (dp-protocol)        */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/* history                                                                   */
/* ========================================================================= */
/* 08.09.2005 [V5.00]  Urversion                                             */
/* 22.10.2005 [V5.01]  BugFix:                                               */
/*                     Dp_cfg.h:                                             */
/*                     Missing bracket:                                      */
/*                     #define ASIC_USER_RAM (ASIC_RAM_LENGTH -              */
/*                                              DP_ORG_LENGTH - SAP_LENGTH)  */
/*                     DP_VERSION_BUGFIX changed to 0x01                     */
/* 09.01.2006 [V5.02]  BugFix:                                               */
/*                     Problem with SIEMENS PLC 318 and Diag.cfg_Fault       */
/*                     The VPC3+/C does not set the default master address   */
/*                     back in the diagnostic telegram to 0xFF.              */
/*                     Software solution in dp_user.c                        */
/*                     (named with BugFix 502).                              */
/*                     DP_VERSION_BUGFIX changed to 0x02                     */
/* 21.02.2006 [V5.03]  BugFix:                                               */
/*                     I&M Functions: Return Codes of NRS-PDUs changed.      */
/*                     profichip has now it's own MANUFACTURER_ID (0x01BC).  */
/*                     BugFix:                                               */
/*                     The definition of LITTLE_ENDIAN and BIG_ENDIAN is     */
/*                     now correct.                                          */
/*                     BugFix:                                               */
/*                     MSAC_C2-connection: Certification Error Ifak -        */
/*                     ILL31 transistion implemented.                        */
/*                     After disconnecting profibus cable, an open MSAC-C2   */
/*                     connection will be closed after timeout (2*F-timer).  */
/*                     Function acls in dp_msac_c1 optimized.                */
/*                     (named with BugFix 503).                              */
/*                     DP_VERSION_BUGFIX changed to 0x03                     */
/* 13.11.2006 [V5.04]  Certification problem ( itm ):                        */
/*                     The DPV1-alarms will be checked now in the function   */
/*                     UserChkNewPrmData().                                  */
/*                     Certification problem ( procentec ):                  */
/*                     The master class 2 sends an initiate.req with out     */
/*                     data and the slave answers with next free SAP.        */
/*                     After then the master class 2 send an initiate.req    */
/*                     with data, the slave should now answer with the same  */
/*                     SAP and not with new SAP.                             */
/*                     (named with BugFix 504).                              */
/*                     DP_VERSION_BUGFIX changed to 0x04                     */
/* 03.08.2009 [V6.00]  Supports now VPC3+S                                   */
/*                     DP_VERSION_BUGFIX changed to 0x00                     */
/* 08.08.2009 [V6.01]  BugFix:                                               */
/*                     DP_VERSION_BUGFIX changed to 0x01                     */
/* 17.09.2010 [V6.02]  BugFix:                                               */
/*                     Error in function: VPC3_GetMasterAddress              */
/*                     DP_VERSION_BUGFIX changed to 0x02                     */
/* 13.12.2010 [V6.03]  BugFix:                                               */
/*                     dp_if.h: add the macro _PACKED_ to all structures     */
/*                     Modification: Prm-, ChkCfg- and SSA-telegram via      */
/*                     help buffer:  abPrmCfgSsaHelpBuffer[ HELP_BUFSIZE ].  */
/*                     DP_VERSION_BUGFIX changed to 0x03                     */
/* 15.02.2011 [V6.04]  BugFix:                                               */
/*                     dp_if.h: DxB-macros are wrong                         */
/*                      - macro VPC3_GET_NEXT_DXB_OUT_BUFFER_CMD() modified  */
/*                      - macro VPC3_GET_DXB_OUT_BUFFER_SM() modified        */
/*                     modification of VPC3_Initialization( UBYTE            */
/*                     bSlaveAddress, UWORD wIdentNumber, psCFG psCfgData ); */
/*                     DP_VERSION_BUGFIX changed to 0x04                     */
/* 09.05.2011 [V6.05]  BugFix:                                               */
/*                     dp_msac2.c:  MSAC_C2_InitiateReqToRes( ... )          */
/*                      - psMsgHeader->bDataLength = C2_FEATURES_SUPPORTED_2;*/
/*                     Modification dp_if.h:                                 */
/*                      - new macro: VPC3_SET_EMPTY_SAP_LIST()               */
/*                      - new macro: VPC3_SET_SAP_LIST_END(OFFSET)           */
/*                     Modification of following functions:                  */
/*                      - dp_if.c: VPC3_Initialization( ... )                */
/*                      - dp_if.c: VPC3_SetConstants( ... )                  */
/*                      - dp_fdl.c: FDL_Init( ... )                          */
/*                     DP_VERSION_BUGFIX changed to 0x05                     */
/* 21.10.2011 [V6.06]  BugFix:                                               */
/*                     dp_msac2.c: MSAC_C2_TransmitDelay( ... )              */
/*                     If the master sends an Abort message during           */
/*                     MSAC_C2_TransmitDelay call, the Class2 statemachine   */
/*                     breaks down with fatal error.                         */
/*                     Modification of following functions:                  */
/*                     - dp_msac2.c: MSAC_C2_TransmitDelay( ... )            */
/*                     - dp_msac2.c: MSAC_C2_RealizeIndRecv( ... )           */
/*                     DP_VERSION_BUGFIX changed to 0x06                     */
/* 12.12.2011 [V6.07]  BugFix:                                               */
/*                     - macro VPC3Write() now with cast VPC3_ADR            */
/*                     - macro VPC3Read() now with cast VPC3_ADR             */
/*                     If MSAC_C2 connection run out in the function         */
/*                     MSAC_C2_Process the User-Application was informed 3   */
/*                     times via function UserMsac2AbortInd.                 */
/*                     Modification of following functions:                  */
/*                     - platform.h now without the define NOP_              */
/*                     DP_VERSION_BUGFIX changed to 0x07                     */
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
     \brief Basic functions for PROFIBUS communication with VPC3+.

*/

/*****************************************************************************/
/* contents:

  - function prototypes
  - data structures
  - internal functions

*/
/*****************************************************************************/
/* include hierarchy */
#include "Includes.h"  //SSN added
#include "VPC3_platform.h"
#include "VPC3.h" //SSN added
#include "VPC3_dp_inc.h"

/*---------------------------------------------------------------------------*/
/* version                                                                   */
/*---------------------------------------------------------------------------*/
#define DP_VERSION_MAIN_INTERFACE   ((UBYTE)0x06)
#define DP_VERSION_FUNCTION         ((UBYTE)0x00)
#define DP_VERSION_BUGFIX           ((UBYTE)0x07)

/*---------------------------------------------------------------------------*/
/* function prototypes                                                       */
/*---------------------------------------------------------------------------*/
DP_ERROR_CODE  VPC3_SetConstants          ( UBYTE bSlaveAddress, UWORD wIdentNumber );
DP_ERROR_CODE  VPC3_InitSubscriber        ( void );
DP_ERROR_CODE  VPC3_InitBufferStructure   ( void );
UWORD          VPC3_AssignAuxBuffer       ( UWORD MEM_PTR pwLens, UBYTE bCount, MEM_UNSIGNED8_PTR pbAssign, UWORD MEM_PTR pwAuxLen );

/*---------------------------------------------------------------------------*/
/* global user data definitions                                              */
/*---------------------------------------------------------------------------*/
#ifdef DP_DEBUG_ENABLE
   sDP_DEBUG_BUFFER_ITEM   asDebugBuffer[ MAX_NR_OF_DEBUG ];
   UBYTE                   bDebugBufferIndex;
   UBYTE                   bDebugBufferOverlapped;
#endif//#ifdef DP_DEBUG_ENABLE

/*---------------------------------------------------------------------------*/
/* defines                                                                   */
/*---------------------------------------------------------------------------*/
/* Literals */
#define RBL_PRM   ((UBYTE)0x00)
#define RBL_CFG   ((UBYTE)0x01)
#define RBL_SSA   ((UBYTE)0x02)

/*---------------------------------------------------------------------------*/
/* function: GetFirmwareVersion                                              */
/*---------------------------------------------------------------------------*/
void GetFirmwareVersion( psDP_VERSION pVersion )
{
   pVersion->wComponentsInstalled =   0x0000

                         #if DP_MSAC_C1
                                    | DP_COMP_INSTALLED_MSAC1_IFA

                             #if DP_ALARM
                                    | DP_COMP_INSTALLED_SUB_AL
                                  #if DP_ALARM_OVER_SAP50
                                    | DP_COMP_INSTALLED_SUB_AL_50
                                  #endif//DP_ALARM_OVER_SAP50
                             #endif//#if DP_ALARM

                             #if DPV1_IM_SUPP
                                    | DP_COMP_INSTALLED_IM
                             #endif//#if DPV1_IM_SUPP
                         #endif//#if DP_MSAC_C1

                         #if DP_MSAC_C2
                                    | DP_COMP_INSTALLED_MSAC2_IFA

                             #if DPV1_IM_SUPP
                                    | DP_COMP_INSTALLED_IM
                             #endif//#if DPV1_IM_SUPP
                         #endif//#if DP_MSAC_C2

                                        ;

   switch( VPC3_GET_ASIC_TYPE() )
   {
      case AT_VPC3B:
      {
         pVersion->wComponentsInstalled |= DP_COMP_INSTALLED_VPC3_B ;
         break;
      }//case AT_VPC3B:

      case AT_VPC3C:
      {
         pVersion->wComponentsInstalled |= DP_COMP_INSTALLED_VPC3_C ;
         break;
      }//case AT_VPC3C:

      case AT_MPI12X:
      {
         pVersion->wComponentsInstalled |= DP_COMP_INSTALLED_VPC3_D ;
         break;
      }//case AT_MPI12X:

      case AT_VPC3S:
      {
         pVersion->wComponentsInstalled |= DP_COMP_INSTALLED_VPC3_S ;
         break;
      }//case VPC3S:

      default:
      {
         //do nothing
        break;
      }//default:
   }//switch( VPC3_GET_ASIC_TYPE()

   pVersion->bMainInterface = DP_VERSION_MAIN_INTERFACE;
   pVersion->bFunction      = DP_VERSION_FUNCTION;
   pVersion->bBugfix        = DP_VERSION_BUGFIX;
}//void GetFirmwareVersion( psDP_VERSION pVersion )

/*---------------------------------------------------------------------------*/
/* function: DpWriteDebugBuffer                                              */
/*---------------------------------------------------------------------------*/
#ifdef DP_DEBUG_ENABLE
void DpWriteDebugBuffer( DEBUG_CODE bDebugCode, UBYTE bDetail1, UBYTE bDetail2 )
{
   if( bDebugBufferOverlapped == FALSE )
   {
      asDebugBuffer[ bDebugBufferIndex ].bDebugCode = bDebugCode;
      asDebugBuffer[ bDebugBufferIndex ].bDetail1   = bDetail1;
      asDebugBuffer[ bDebugBufferIndex ].bDetail2   = bDetail2;

      if( bDebugBufferIndex == ( MAX_NR_OF_DEBUG - 1 ) )
      {
         bDebugBufferOverlapped = TRUE;
         bDebugBufferIndex = 0;
      }//if( bDebugBufferIndex == ( MAX_NR_OF_DEBUG - 1 ) )
      else
      {
         bDebugBufferIndex++;
      }//else of if( bDebugBufferIndex == ( MAX_NR_OF_DEBUG - 1 ) )
   }//if( bDebugBufferOverlapped == FALSE )
}//void DpWriteDebugBuffer( DEBUG_CODE debug_code, UBYTE detail_1, UBYTE detail_2 )
#endif//#ifdef DP_DEBUG_ENABLE

/*---------------------------------------------------------------------------*/
/* function: DpClearDebugBuffer                                              */
/*---------------------------------------------------------------------------*/
#ifdef DP_DEBUG_ENABLE
void DpClearDebugBuffer( void )
{
UWORD i;

   bDebugBufferOverlapped = FALSE;
   bDebugBufferIndex      = 0x00;

   for( i = 0; i < MAX_NR_OF_DEBUG; i++ )
   {
      asDebugBuffer[i].bDebugCode = 0x00;
      asDebugBuffer[i].bDetail1   = 0x00;
      asDebugBuffer[i].bDetail2   = 0x00;
   }//for( i = 0; i < MAX_NR_OF_DEBUG; i++ )
}//void DpClearDebugBuffer( void )
#endif//#ifdef DP_DEBUG_ENABLE

/*---------------------------------------------------------------------------*/
/* function: VPC3_MemoryTest                                                */
/*---------------------------------------------------------------------------*/
/*!
  \brief Check VPC3 memory.
   This function checks the memory of VPC3+. The starting address is 16hex and
   the end address depends on DP_VPC3_4KB_MODE (dp_cfg.h).

  \retval DP_OK - memory OK
  \retval DP_VPC3_ERROR - Memory Error
*/
DP_ERROR_CODE VPC3_MemoryTest( void )
{
#if VPC3_SERIAL_MODE
   VPC3_ADR       wAddress;
   UWORD          wTemp;
#else
   VPC3_UNSIGNED8_PTR  pToVpc3;  // pointer to vpc3 memory area
#endif//#if VPC3_SERIAL_MODE
DP_ERROR_CODE     bError;
UWORD             j;
UWORD             i;

   //#ifdef MPI12x
      VPC3_SET_INT_REQ_REG_L( 0x03 );
// This can be replaced by 4 NOPs to be updated by Eaton/Galaxy/Pythia
      Vpc3Wait_1ms();
   //#endif//#ifdef MPI12x
   //neccessary, if 4Kbyte mode enabled
   VPC3_SET_MODE_REG_2( INIT_VPC3_MODE_REG_2 );

   /*-----------------------------------------------------------------------*/
   /* test and clear vpc3 ram                                               */
   /*-----------------------------------------------------------------------*/
   bError = DP_OK;

   #if VPC3_SERIAL_MODE

      j = 0;
      wAddress = bVpc3RwTsAddr;
      for( i = 0x16; i < ASIC_RAM_LENGTH; )
      {
         Vpc3Write( wAddress++, (UBYTE)( j >> 8 ) );
         Vpc3Write( wAddress++, (UBYTE)( j      ) );

         i+=2;
         j++;
      }//for( i = 0x16; i < ASIC_RAM_LENGTH; )

      j = 0;
      wAddress = bVpc3RwTsAddr;
      for( i = 0x16; i < ASIC_RAM_LENGTH; )
      {
         wTemp = (((UWORD)Vpc3Read( wAddress++ )) << 8 );
         wTemp +=  (UWORD)Vpc3Read( wAddress++ );
         if( wTemp != j )
         {
            bError = DP_VPC3_ERROR; // ERROR
         }//if( wTemp != j )

         i+=2;
         j++;
      }//for( i = 0x16; i < ASIC_RAM_LENGTH; )

   #else

      j = 0;
      pToVpc3 = &pVpc3->bTsAddr VPC3_EXTENSION;
      for( i = 0x16; i < ASIC_RAM_LENGTH; )
      {
         *((UWORD VPC3_PTR)pToVpc3)=j;

         i+=2;
         j++;
         pToVpc3 += 2;
      }//for( i = 0x16; i < ASIC_RAM_LENGTH; )

      j = 0;
      pToVpc3 = &pVpc3->bTsAddr VPC3_EXTENSION;
      for( i = 0x16; i < ASIC_RAM_LENGTH; )
      {
         if(*((UWORD VPC3_PTR)pToVpc3)!=j)
         {
            bError = DP_VPC3_ERROR; // ERROR
         }

         i+=2;
         j++;
         pToVpc3 += 2;
      }//for( i = 0x16; i < ASIC_RAM_LENGTH; )

   #endif//#if VPC3_SERIAL_MODE

   return bError;
}//DP_ERROR_CODE VPC3_MemoryTest( void )

/*---------------------------------------------------------------------------*/
/* function: VPC3_Initialization                                             */
/*---------------------------------------------------------------------------*/
/*!
  \brief Initializes VPC3+.

  \param[in] bSlaveAddress - PROFIBUS slave address ( range: 1..126 )
  \param[in] wIdentNumber - PROFIBUS ident number
  \param[in] sCfgData - default configuration

  \retval DP_OK - Initialization OK
  \retval DP_NOT_OFFLINE_ERROR - Error VPC3 is not in OFFLINE state
  \retval DP_ADDRESS_ERROR - Error, DP Slave address
  \retval DP_CALCULATE_IO_ERROR - Error with configuration bytes
  \retval DP_DOUT_LEN_ERROR - Error with Dout length
  \retval DP_DIN_LEN_ERROR - Error with Din length
  \retval DP_DIAG_LEN_ERROR - Error with diagnostics length
  \retval DP_PRM_LEN_ERROR - Error with parameter assignment data length
  \retval DP_SSA_LEN_ERROR - Error with address data length
  \retval DP_CFG_LEN_ERROR - Error with configuration data length
  \retval DP_LESS_MEM_ERROR - Error Overall, too much memory used
  \retval DP_LESS_MEM_FDL_ERROR - Error Overall, too much memory used
*/
DP_ERROR_CODE VPC3_Initialization( UBYTE bSlaveAddress, UWORD wIdentNumber, psCFG psCfgData )
{
DP_ERROR_CODE bError;

   /*-----------------------------------------------------------------------*/
   /* initialize global system structure                                    */
   /*-----------------------------------------------------------------------*/

   #ifdef DP_DEBUG_ENABLE
      DP_ClearDebugBuffer();
   #endif//#ifdef DP_DEBUG_ENABLE

   /*-----------------------------------------------------------------------*/
   /* init vpc3 data                                                        */
   /*-----------------------------------------------------------------------*/

   //#ifdef MPI12x
      VPC3_SET_INT_REQ_REG_L( 0x03 );
// This can be replaced by 4 NOPs to be updated by Eaton/Galaxy/Pythia
      Vpc3Wait_1ms();
   //#endif//#ifdef MPI12x

   /*-------------------------------------------------------------------*/
   /* check VPC3 is in OFFLINE                                          */
   /*-------------------------------------------------------------------*/
   if( !VPC3_GET_OFF_PASS() )
   {
      //neccessary, if 4Kbyte mode enabled
      VPC3_SET_MODE_REG_2( INIT_VPC3_MODE_REG_2 );

      //clear VPC3
      #if VPC3_SERIAL_MODE
         Vpc3MemSet_( bVpc3RwTsAddr, 0, (ASIC_RAM_LENGTH-0x16) );
      #else
         Vpc3MemSet_( &pVpc3->bTsAddr VPC3_EXTENSION, 0, (ASIC_RAM_LENGTH-0x16) );
      #endif//#if VPC3_SERIAL_MODE

      /*--------------------------------------------------------------*/
      /* set constant values                                          */
      /*--------------------------------------------------------------*/
      bError = VPC3_SetConstants( bSlaveAddress, wIdentNumber );
      if( DP_OK == bError )
      {
         /*-----------------------------------------------------------*/
         /* calculate length of input and output data using cfg-data  */
         /*-----------------------------------------------------------*/
         bError = VPC3_CalculateInpOutpLength( &psCfgData->abData[0], psCfgData->bLength );
         if( DP_OK == bError )
         {
            /*--------------------------------------------------------*/
            /* initialize buffer structure                            */
            /*--------------------------------------------------------*/
            bError = VPC3_InitBufferStructure();
            if( DP_OK == bError )
            {
               /*-----------------------------------------------------*/
               /* initialize subscriber                               */
               /*-----------------------------------------------------*/
               #if DP_SUBSCRIBER
                  bError = VPC3_InitSubscriber();
               #endif//#if DP_SUBSCRIBER

               /*-----------------------------------------------------*/
               /* initialize fdl_interface                            */
               /*-----------------------------------------------------*/
               #if DP_FDL
                  if( DP_OK == bError )
                  {
                     bError = FDL_Init( bSlaveAddress );

                     if( DP_OK == bError )
                     {
                        /*--------------------------------------------*/
                        /* initialize MSAC_C1                         */
                        /*--------------------------------------------*/
                        #if DP_MSAC_C1
                           bError = MSAC_C1_Init();
                        #endif//#if DP_MSAC_C1

                        #if DP_MSAC_C2
                           if( DP_OK == bError )
                           {
                              /*--------------------------------------*/
                              /* initialize MSAC_C2                   */
                              /*--------------------------------------*/
                              bError = MSAC_C2_Init();
                           }//if( DP_OK == bError )
                        #endif//#if DP_MSAC_C2
                     }//if( DP_OK == bError )
                  }//if( DP_OK == bError )
               #endif //#if DP_FDL
            }//if( DP_OK == bError )
         }//if( DP_OK == bError )
      }//if( DP_OK == bError )

      if( DP_OK == bError )
      {
         #if VPC3_SERIAL_MODE
            CopyToVpc3_( (VPC3_UNSIGNED8_PTR)Vpc3AsicAddress, (MEM_UNSIGNED8_PTR)pVpc3, 0x40 );
         #endif//#if VPC3_SERIAL_MODE

         /*-----------------------------------------------------------*/
         /* set real configuration data                               */
         /*-----------------------------------------------------------*/
         VPC3_SET_READ_CFG_LEN( psCfgData->bLength );      // set configuration length
         CopyToVpc3_( VPC3_GET_READ_CFG_BUF_PTR(), &psCfgData->abData[0], psCfgData->bLength );
      }//if( DP_OK == bError )
   }//if( !VPC3_GET_OFF_PASS() )
   else
   {
      bError = DP_NOT_OFFLINE_ERROR;
   }//else of if( !VPC3_GET_OFF_PASS() )

   return bError;
}//DP_ERROR_CODE VPC3_Initialization( UBYTE bSlaveAddress, UWORD wIdentNumber, psCFG psCfgData )

/*---------------------------------------------------------------------------*/
/* function: VPC3_Start                                                      */
/*---------------------------------------------------------------------------*/
/*!
  \brief Set VPC3+ to online mode.
  If the ASIC could be correctly initialized with VPC3_Initialization(), it still has to be started. Between initialization and
  start, the user can still initialize buffers in the ASIC.

  Reaction after this service:
  - VPC3+ generates DX_OUT event, all outputs will be cleared
  - VPC3+ generates BAUDRATE_DETECT event, if master connected
  - master sends FDL-Status.req --> slave answers with FDL-Status.resp ( RTS signal! )
*/
void VPC3_Start( void )
{
   #if DP_MSAC_C2
      MSAC_C2_OpenChannel();
   #endif//#if DP_MSAC_C2

   /*-----------------------------------------------------------------------*/
   /* start vpc3                                                            */
   /*-----------------------------------------------------------------------*/
   VPC3_Start__();

   // Fetch the first diagnosis buffer
   pDpSystem->pDiagBuffer = VPC3_GetDiagBufPtr();
}//void VPC3_Start( void )

/*---------------------------------------------------------------------------*/
/* function: VPC3_Stop                                                       */
/*---------------------------------------------------------------------------*/
//V504
/*!
  \brief Set VPC3+ to offline mode.
*/
void VPC3_Stop( void )
{
   /*-----------------------------------------------------------------------*/
   /* start vpc3                                                            */
   /*-----------------------------------------------------------------------*/
   VPC3_GoOffline();
   do
   {
      //wait, for offline
   }while( VPC3_GET_OFF_PASS() );

   #if DP_MSAC_C2
      MSAC_C2_ResetStateMachine();
   #endif//#if DP_MSAC_C2
}//void  VPC3_Stop( void )

/*---------------------------------------------------------------------------*/
/* function: VPC3_SetConstants                                                */
/*---------------------------------------------------------------------------*/
/*!
  \brief Initializes VPC3+ with all constant values.

  \param[in] bSlaveAddress - PROFIBUS slave address ( range: 1..126 )
  \param[in] wIdentNumber - PROFIBUS ident number

  \retval DP_OK - Initialization OK
  \retval DP_ADDRESS_ERROR - Error, DP Slave address
*/
DP_ERROR_CODE VPC3_SetConstants( UBYTE bSlaveAddress, UWORD wIdentNumber )
{
DP_ERROR_CODE bError;

   bError = DP_OK;

   pDpSystem->bDinBufsize  = DIN_BUFSIZE;
   pDpSystem->bDoutBufsize = DOUT_BUFSIZE;
   pDpSystem->bPrmBufsize  = PRM_BUFSIZE;
   pDpSystem->bDiagBufsize = DIAG_BUFSIZE;
   pDpSystem->bCfgBufsize  = CFG_BUFSIZE;
   pDpSystem->bSsaBufsize  = SSA_BUFSIZE;

   pDpSystem->wAsicUserRam = ASIC_USER_RAM;

   /*-----------------------------------------------------------------------*/
   /* initialize  control logic                                             */
   /*-----------------------------------------------------------------------*/
   pVpc3->bIntReqReg_L VPC3_EXTENSION                = 0x00;
   pVpc3->bIntReqReg_H VPC3_EXTENSION                = 0x00;
   pVpc3->sReg.sWrite.bIntAck_L VPC3_EXTENSION       = 0xFF;
   pVpc3->sReg.sWrite.bIntAck_H VPC3_EXTENSION       = 0xFF;
   pVpc3->sCtrlPrm.sWrite.bModeReg1_R VPC3_EXTENSION = 0xFF;      // reset mode register 1

   /*-----------------------------------------------------------------------*/
   /* set modes of vpc3                                                     */
   /*-----------------------------------------------------------------------*/
   pVpc3->bModeReg0_H VPC3_EXTENSION = INIT_VPC3_MODE_REG_H;
   pVpc3->bModeReg0_L VPC3_EXTENSION = INIT_VPC3_MODE_REG_L;

   pVpc3->sCtrlPrm.sWrite.bModeReg2 VPC3_EXTENSION = INIT_VPC3_MODE_REG_2;
   pVpc3->sCtrlPrm.sWrite.bModeReg3 VPC3_EXTENSION = INIT_VPC3_MODE_REG_3;

   /*-----------------------------------------------------------------------*/
   /* set interrupt triggers                                                */
   /*-----------------------------------------------------------------------*/
   pVpc3->sReg.sWrite.bIntMask_H VPC3_EXTENSION = (UBYTE)(~(INIT_VPC3_IND_H));
   pVpc3->sReg.sWrite.bIntMask_L VPC3_EXTENSION = (UBYTE)(~(INIT_VPC3_IND_L));

   pDpSystem->bIntIndHigh = (UBYTE)(~(INIT_VPC3_IND_H));
   pDpSystem->bIntIndLow  = (UBYTE)(~(INIT_VPC3_IND_L));

   /*-----------------------------------------------------------------------*/
   /* set time-variables                                                    */
   /*-----------------------------------------------------------------------*/
   pVpc3->sCtrlPrm.sWrite.bWdBaudControlVal VPC3_EXTENSION = 0x10;    // WD Timeout
   pVpc3->sCtrlPrm.sWrite.bMinTsdrVal VPC3_EXTENSION       = 0x0B;    // Min Tsdr 11 BIT (11Bit .. 255Bit)

   /*-----------------------------------------------------------------------*/
   /* set variables for synch-mode                                          */
   /*-----------------------------------------------------------------------*/
   #if DP_ISOCHRONOUS_MODE
      pVpc3->sCtrlPrm.sWrite.bSyncPwReg VPC3_EXTENSION = SYNCH_PULSEWIDTH;   // sync pulse width register
      pVpc3->sCtrlPrm.sWrite.bGroupSelectReg VPC3_EXTENSION = 0x80;
      pVpc3->sCtrlPrm.sWrite.bControlCommandReg VPC3_EXTENSION  = 0x00;
   #endif//#if DP_ISOCHRONOUS_MODE

   /*-----------------------------------------------------------------------*/
   /* set user watchdog (dataexchange telegram counter)                     */
   /*-----------------------------------------------------------------------*/
   pVpc3->abUserWdValue[1] VPC3_EXTENSION = (UBYTE)(USER_WD >> 8);
   pVpc3->abUserWdValue[0] VPC3_EXTENSION = (UBYTE)(USER_WD);

   /*-----------------------------------------------------------------------*/
   /* set pointer to FF                                                     */
   /*-----------------------------------------------------------------------*/
   pVpc3->bFdlSapListPtr   VPC3_EXTENSION = VPC3_SAP_CTRL_LIST_START;
   VPC3_SET_EMPTY_SAP_LIST();

   /*-----------------------------------------------------------------------*/
   /* ssa support                                                           */
   /*-----------------------------------------------------------------------*/
   pVpc3->bRealNoAddChange VPC3_EXTENSION = ( SSA_BUFSIZE == 0 ) ? 0xFF : 0x00;

   /*-----------------------------------------------------------------------*/
   /* set profibus ident number                                             */
   /*-----------------------------------------------------------------------*/
   pVpc3->bIdentHigh VPC3_EXTENSION = (UBYTE)(wIdentNumber >> 8);
   pVpc3->bIdentLow  VPC3_EXTENSION = (UBYTE)(wIdentNumber);

   /*-----------------------------------------------------------------------*/
   /* set and check slave address                                           */
   /*-----------------------------------------------------------------------*/
   if( bSlaveAddress < 127 && bSlaveAddress != 0 )
   {
      pVpc3->bTsAddr VPC3_EXTENSION = bSlaveAddress;
   }//if( bSlaveAddress < 127 && bSlaveAddress != 0 )
   else
   {
      bError = DP_ADDRESS_ERROR;
   }//else of if( bSlaveAddress < 127 && bSlaveAddress != 0 )

   return bError;
}//DP_ERROR_CODE VPC3_SetConstants( UBYTE bSlaveAddress, UWORD wIdentNumber )

/*---------------------------------------------------------------------------*/
/* function: VPC3_InitSubscriber                                             */
/*---------------------------------------------------------------------------*/
#if DP_SUBSCRIBER
DP_ERROR_CODE VPC3_InitSubscriber( void )
{
DP_ERROR_CODE       bError;
VPC3_UNSIGNED8_PTR  pToVpc3;              // pointer to VPC3, µController formatted
UWORD               wDxbBufferLength;     // segmented length of DxB-Buffer
UBYTE               bVpc3SegmentAddress;  // segment address in VPC3-ASIC

   bError = DP_OK;

   /*-----------------------------------------------------------------------*/
   /* check buffer length                                                   */
   /*-----------------------------------------------------------------------*/
   if( DP_MAX_DATA_PER_LINK > 244 )
   {
      bError = SSC_MAX_DATA_PER_LINK;
   }//if( DP_MAX_DATA_PER_LINK > 244 )
   else
   {
      #if VPC3_SERIAL_MODE
         //pointer mc formatted
         pToVpc3 = (VPC3_UNSIGNED8_PTR)( Vpc3AsicAddress + DP_ORG_LENGTH + SAP_LENGTH + pDpSystem->wVpc3UsedDpBufferMemory );
         //pointer vpc3 formatted
         bVpc3SegmentAddress = (UBYTE)( ( pDpSystem->wVpc3UsedDpBufferMemory + DP_ORG_LENGTH + SAP_LENGTH ) >> SEG_MULDIV );
      #else
         //pointer mc formatted
         pToVpc3 = &pVpc3->abDpBuffer[pDpSystem->wVpc3UsedDpBufferMemory] VPC3_EXTENSION;
         //pointer vpc3 formatted
         bVpc3SegmentAddress = (UBYTE)( ((VPC3_ADR)pToVpc3-(VPC3_ADR)Vpc3AsicAddress) >> SEG_MULDIV );
      #endif//#if VPC3_SERIAL_MODE

      // length dxb_out
      wDxbBufferLength = (((DP_MAX_DATA_PER_LINK+2)+SEG_OFFSET) & SEG_ADDBYTE);
      pVpc3->bLenDxbOutBuf = (DP_MAX_DATA_PER_LINK+2);
      pDpSystem->wVpc3UsedDpBufferMemory += (3*wDxbBufferLength);
      // length link status
      pVpc3->bLenDxbStatusBuf = ((((DP_MAX_LINK_SUPPORTED*2)+4)+SEG_OFFSET) & SEG_ADDBYTE);
      pDpSystem->wVpc3UsedDpBufferMemory += pVpc3->bLenDxbStatusBuf;
      // length link table
      pVpc3->bLenDxbLinkBuf = (((DP_MAX_LINK_SUPPORTED*4)+SEG_OFFSET) & SEG_ADDBYTE);
      pDpSystem->wVpc3UsedDpBufferMemory += pVpc3->bLenDxbLinkBuf;

      /*-------------------------------------------------------------------*/
      /* check memory consumption                                          */
      /*-------------------------------------------------------------------*/
      if( pDpSystem->wVpc3UsedDpBufferMemory > ASIC_USER_RAM )
      {
         // Error: user needs too much memory
         pDpSystem->wVpc3UsedDpBufferMemory = 0;
         bError = DP_LESS_MEM_ERROR;
      }//if( pDpSystem->wVpc3UsedDpBufferMemory > ASIC_USER_RAM )
      else
      {
         /*---------------------------------------------------------------*/
         /* set buffer pointer                                            */
         /*---------------------------------------------------------------*/
         pVpc3->bDxbLinkBufPtr   = bVpc3SegmentAddress;
         pVpc3->bDxbStatusBufPtr = pVpc3->bDxbLinkBufPtr   + ( pVpc3->bLenDxbLinkBuf >> SEG_MULDIV );
         pVpc3->bDxbOutBufPtr1   = pVpc3->bDxbStatusBufPtr + ( pVpc3->bLenDxbStatusBuf >> SEG_MULDIV );
         pVpc3->bDxbOutBufPtr2   = pVpc3->bDxbOutBufPtr1   + ( wDxbBufferLength >> SEG_MULDIV );
         pVpc3->bDxbOutBufPtr3   = pVpc3->bDxbOutBufPtr2   + ( wDxbBufferLength >> SEG_MULDIV );

         pDpSystem->pDxbOutBuffer1 = (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)(pVpc3->bDxbOutBufPtr1 VPC3_EXTENSION) << SEG_MULDIV)+(VPC3_ADR)Vpc3AsicAddress);
         pDpSystem->pDxbOutBuffer2 = (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)(pVpc3->bDxbOutBufPtr2 VPC3_EXTENSION) << SEG_MULDIV)+(VPC3_ADR)Vpc3AsicAddress);
         pDpSystem->pDxbOutBuffer3 = (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)(pVpc3->bDxbOutBufPtr3 VPC3_EXTENSION) << SEG_MULDIV)+(VPC3_ADR)Vpc3AsicAddress);
      }//else of if( pDpSystem->wVpc3UsedDpBufferMemory > ASIC_USER_RAM )
   }//else of if( DP_MAX_DATA_PER_LINK > 244 )

   return bError;
}//DP_ERROR_CODE VPC3_InitSubscriber( void )
#endif//#if DP_SUBSCRIBER


/*---------------------------------------------------------------------------*/
/* function: VPC3_CalculateInpOutpLength                                     */
/*---------------------------------------------------------------------------*/
/*!
   \brief Calculation of input- and output data.

   \param[in] pToCfgData - address of configuration data
   \param[in] bCfgLength - length of configuration data

   \retval DP_OK - Initialization OK
   \retval DP_CFG_LEN_ERROR - Error with CFG length
   \retval DP_CALCULATE_IO_ERROR - Error with DIN or DOUT length
   \retval DP_CFG_FORMAT_ERROR - Error in special configuration format
*/
DP_ERROR_CODE VPC3_CalculateInpOutpLength( MEM_UNSIGNED8_PTR pToCfgData, UBYTE bCfgLength )
{
DP_ERROR_CODE bError;
UBYTE         bSpecificDataLength;
UBYTE         bTempOutputDataLength;
UBYTE         bTempInputDataLength;
UBYTE         bLength;
UBYTE         bCount;

   bError = DP_OK;
   bTempOutputDataLength = 0;
   bTempInputDataLength  = 0;

   if( ( bCfgLength > 0 ) && ( bCfgLength <= CFG_BUFSIZE ) )
   {
      for( ; bCfgLength > 0; bCfgLength -= bCount )
      {
         bCount = 0;

         if( *pToCfgData & VPC3_CFG_IS_BYTE_FORMAT )
         {
            //general identifier format
            bCount++;
            //pToCfgData points to "Configurationbyte", CFG_BF means "CFG_IS_BYTE_FORMAT"
            bLength = (UBYTE)( ( *pToCfgData & VPC3_CFG_BF_LENGTH) + 1 );

            if( *pToCfgData & VPC3_CFG_BF_OUTP_EXIST )
            {
               bTempOutputDataLength += ( *pToCfgData & VPC3_CFG_LENGTH_IS_WORD_FORMAT ) ? ( 2 * bLength ) : bLength;
            }//if( *pToCfgData & VPC3_CFG_BF_OUTP_EXIST )

            if( *pToCfgData & VPC3_CFG_BF_INP_EXIST )
            {
               bTempInputDataLength += ( *pToCfgData & VPC3_CFG_LENGTH_IS_WORD_FORMAT ) ? ( 2 * bLength ) : bLength;
            }//if( *pToCfgData & VPC3_CFG_BF_INP_EXIST )

            pToCfgData++;
         }//if( *pToCfgData & VPC3_CFG_IS_BYTE_FORMAT )
         else
         {
            // pToCfgData points to the headerbyte of "special identifier format
            // CFG_SF means "CFG_IS_SPECIAL_FORMAT"
            if( *pToCfgData & VPC3_CFG_SF_OUTP_EXIST )
            {
               bCount++;    // next UBYTE contains the length of outp_data
               bLength = (UBYTE)( ( *( pToCfgData + bCount ) & VPC3_CFG_SF_LENGTH ) + 1 );

               bTempOutputDataLength += ( *( pToCfgData + bCount ) & VPC3_CFG_LENGTH_IS_WORD_FORMAT ) ? ( 2 * bLength ) : bLength;
            }//if( *pToCfgData & VPC3_CFG_SF_OUTP_EXIST )

            if( *pToCfgData & VPC3_CFG_SF_INP_EXIST )
            {
               bCount++;  // next UBYTE copntains the length of inp_data
               bLength = (UBYTE)( ( *( pToCfgData + bCount ) & VPC3_CFG_SF_LENGTH ) + 1 );

               bTempInputDataLength += ( *( pToCfgData + bCount ) & VPC3_CFG_LENGTH_IS_WORD_FORMAT ) ? ( 2 * bLength ) : bLength;
            }//if( *pToCfgData & VPC3_CFG_SF_INP_EXIST )

            bSpecificDataLength = (UBYTE)( *pToCfgData & VPC3_CFG_BF_LENGTH );

            if( bSpecificDataLength != 15 )
            {
               bCount = (UBYTE)( bCount + 1 + bSpecificDataLength );
               pToCfgData = pToCfgData + bCount;
            }//if( bSpecificDataLength != 15 )
            else
            {
               // specific data length = 15 not allowed
               pDpSystem->bInputDataLength  = 0xFF;
               pDpSystem->bOutputDataLength = 0xFF;
               bError = DP_CFG_FORMAT_ERROR;
            }//else of if( bSpecificDataLength != 15 )
         }//else of if( *pToCfgData & VPC3_CFG_IS_BYTE_FORMAT )
      }//for( ; bCfgLength > 0; bCfgLength -= bCount )

      if( ( bCfgLength != 0 ) || ( bTempInputDataLength > DIN_BUFSIZE ) || ( bTempOutputDataLength > DOUT_BUFSIZE ) )
      {
         pDpSystem->bInputDataLength  = 0xFF;
         pDpSystem->bOutputDataLength = 0xFF;

         bError = DP_CALCULATE_IO_ERROR;
      }//if( ( bCfgLength != 0 ) || ( bTempInputDataLength > DIN_BUFSIZE ) || ( bTempOutputDataLength > DOUT_BUFSIZE ) )
      else
      {
         pDpSystem->bInputDataLength  = bTempInputDataLength;
         pDpSystem->bOutputDataLength = bTempOutputDataLength;
         bError = DP_OK;
      }//else of if( ( bCfgLength != 0 ) || ( bTempInputDataLength > DIN_BUFSIZE ) || ( bTempOutputDataLength > DOUT_BUFSIZE ) )
   }//if( ( bCfgLength > 0 ) && ( bCfgLength <= CFG_BUFSIZE ) )
   else
   {
      pDpSystem->bInputDataLength  = 0xFF;
      pDpSystem->bOutputDataLength = 0xFF;
      bError = DP_CFG_LEN_ERROR;
   }//else of if( ( bCfgLength > 0 ) && ( bCfgLength <= CFG_BUFSIZE ) )

   return bError;
}//DP_ERROR_CODE VPC3_CalculateInpOutpLength( MEM_UNSIGNED8_PTR pToCfgData, UBYTE bCfgLength )

/*---------------------------------------------------------------------------*/
/* function: VPC3_InitBufferStructure                                        */
/*---------------------------------------------------------------------------*/
/*!
  \brief Initializes VPC3+ buffer structure.

  \retval DP_OK - Initialization OK
  \retval DP_DOUT_LEN_ERROR - Error with Dout length
  \retval DP_DIN_LEN_ERROR - Error with Din length
  \retval DP_DIAG_LEN_ERROR - Error with diagnostics length
  \retval DP_PRM_LEN_ERROR - Error with parameter assignment data length
  \retval DP_SSA_LEN_ERROR - Error with address data length
  \retval DP_CFG_LEN_ERROR - Error with configuration data length
  \retval DP_LESS_MEM_ERROR - Error Overall, too much memory used
*/
DP_ERROR_CODE VPC3_InitBufferStructure( void )
{
DP_ERROR_CODE  bError;
UWORD          awAuxBufferLength[2];      // calculated length of AUX buffer
UWORD          wInputOutputBufferLength;  // calculated length of real I/O Data length
UWORD          awRealBufferLength[3];     // real length of AUX-Buffer
UBYTE          bAuxBufSel;                // Bit array, in which the assignments of AUX1/2 are defined

   bError = DP_OK;

   /*-----------------------------------------------------------------------*/
   /* check buffer length                                                   */
   /*-----------------------------------------------------------------------*/
   if( pDpSystem->bDoutBufsize > 244 )
   {
      bError = DP_DOUT_LEN_ERROR;
   }//if( pDpSystem->bDoutBufsize > 244 )

   else
   if( pDpSystem->bDinBufsize > 244 )
   {
      bError = DP_DIN_LEN_ERROR;
   }//if( pDpSystem->bDinBufsize > 244 )

   else
   if( ( pDpSystem->bDiagBufsize < 6 ) || ( pDpSystem->bDiagBufsize > 244 ) )
   {
      bError = DP_DIAG_LEN_ERROR;
   }//if( ( pDpSystem->bDiagBufsize < 6 ) || ( pDpSystem->bDiagBufsize > 244 )

   else
   if( ( pDpSystem->bPrmBufsize < 7 ) || ( pDpSystem->bPrmBufsize > 244 ) )
   {
      bError = DP_PRM_LEN_ERROR;
   }//if( ( pDpSystem->bPrmBufsize < 7 ) || ( pDpSystem->bPrmBufsize > 244 ) )

   else
   if( ( pDpSystem->bCfgBufsize < 1 ) || ( pDpSystem->bCfgBufsize > 244 ) )
   {
      bError = DP_CFG_LEN_ERROR;
   }//if( ( pDpSystem->bCfgBufsize < 1 ) || ( pDpSystem->bCfgBufsize > 244 ) )

   else
   if( pDpSystem->bSsaBufsize != 0 && ( ( pDpSystem->bSsaBufsize < 4 ) || ( pDpSystem->bSsaBufsize > 244 ) ) )
   {
      bError = DP_SSA_LEN_ERROR;
   }//if( pDpSystem->bSsaBufsize != 0 && ( ( pDpSystem->bSsaBufsize < 4 ) || ( pDpSystem->bSsaBufsize > 244 ) ) )

   if( bError == DP_OK )
   {
      /*-------------------------------------------------------------------*/
      /* length of buffers ok, check memory consumption                    */
      /*-------------------------------------------------------------------*/

      // length of prm buffer
      awRealBufferLength[RBL_PRM] = ( pVpc3->bModeReg0_H VPC3_EXTENSION & VPC3_SPEC_PRM_BUF_MODE ) ? 0 : pDpSystem->bPrmBufsize;

      // length of cfg and ssa buffer
      awRealBufferLength[RBL_CFG] = pDpSystem->bCfgBufsize;
      awRealBufferLength[RBL_SSA] = pDpSystem->bSsaBufsize;

      /*-------------------------------------------------------------------*/
      /* calculate aux-buffer                                              */
      /*-------------------------------------------------------------------*/
      pDpSystem->wVpc3UsedDpBufferMemory = VPC3_AssignAuxBuffer( &awRealBufferLength[0], 3, &bAuxBufSel, &awAuxBufferLength[0] );

      /*-------------------------------------------------------------------*/
      /* check memory consumption                                          */
      /*-------------------------------------------------------------------*/
      wInputOutputBufferLength = ( ( pDpSystem->bDoutBufsize + SEG_OFFSET ) & SEG_ADDWORD ) + ( ( pDpSystem->bDinBufsize + SEG_OFFSET ) & SEG_ADDWORD );
      // DIn DOut buffer
      pDpSystem->wVpc3UsedDpBufferMemory += wInputOutputBufferLength * 3;
      // add Read Config Buffer
      pDpSystem->wVpc3UsedDpBufferMemory += awRealBufferLength[RBL_CFG];
      // diagnose buffer
      pDpSystem->wVpc3UsedDpBufferMemory += ( ( pDpSystem->bDiagBufsize + SEG_OFFSET ) & SEG_ADDBYTE ) * 2;
      // prm buffer
      if( pVpc3->bModeReg0_H VPC3_EXTENSION & VPC3_SPEC_PRM_BUF_MODE )
      {
         // spec. PRM buffer
         awRealBufferLength[RBL_PRM] = (pDpSystem->bPrmBufsize + SEG_OFFSET) & SEG_ADDBYTE;
         pDpSystem->wVpc3UsedDpBufferMemory += awRealBufferLength[RBL_PRM];
         pVpc3->bLenSpecPrmBuf VPC3_EXTENSION = pDpSystem->bPrmBufsize;
      }//if( pVpc3->bModeReg0_H VPC3_EXTENSION & VPC3_SPEC_PRM_BUF_MODE )
      else
      {
         pVpc3->bLenSpecPrmBuf VPC3_EXTENSION = 0;
      }//else of if( pVpc3->bModeReg0_H VPC3_EXTENSION & VPC3_SPEC_PRM_BUF_MODE )

      //if( pDpSystem->wVpc3UsedDpBufferMemory > sizeof( pVpc3->abDpBuffer ) )
      if( pDpSystem->wVpc3UsedDpBufferMemory > pDpSystem->wAsicUserRam )
      {
         // Error: user needs too much memory
         pDpSystem->wVpc3UsedDpBufferMemory = 0;
         bError = DP_LESS_MEM_ERROR;
      }//if( pDpSystem->wVpc3UsedDpBufferMemory > pDpSystem->wAsicUserRam )
      else
      {
         /*---------------------------------------------------------------*/
         /* set buffer pointer                                            */
         /*---------------------------------------------------------------*/
         pVpc3->bAuxBufSel VPC3_EXTENSION       = bAuxBufSel;

         pVpc3->abDoutBufPtr[0] VPC3_EXTENSION  = VPC3_DP_BUF_START;
         pVpc3->abDoutBufPtr[1] VPC3_EXTENSION  = pVpc3->abDoutBufPtr[0] VPC3_EXTENSION  + (((pDpSystem->bDoutBufsize + SEG_OFFSET) & SEG_ADDBYTE)>>SEG_MULDIV);
         pVpc3->abDoutBufPtr[2] VPC3_EXTENSION  = pVpc3->abDoutBufPtr[1] VPC3_EXTENSION  + (((pDpSystem->bDoutBufsize + SEG_OFFSET) & SEG_ADDBYTE)>>SEG_MULDIV);

         pVpc3->abDinBufPtr[0] VPC3_EXTENSION   = pVpc3->abDoutBufPtr[2] VPC3_EXTENSION  + (((pDpSystem->bDoutBufsize + SEG_OFFSET) & SEG_ADDBYTE)>>SEG_MULDIV);
         pVpc3->abDinBufPtr[1] VPC3_EXTENSION   = pVpc3->abDinBufPtr[0]  VPC3_EXTENSION  + (((pDpSystem->bDinBufsize + SEG_OFFSET) & SEG_ADDBYTE)>>SEG_MULDIV);
         pVpc3->abDinBufPtr[2] VPC3_EXTENSION   = pVpc3->abDinBufPtr[1]  VPC3_EXTENSION  + (((pDpSystem->bDinBufsize + SEG_OFFSET) & SEG_ADDBYTE)>>SEG_MULDIV);

         pVpc3->abDiagBufPtr[0] VPC3_EXTENSION  = pVpc3->abDinBufPtr[2]  VPC3_EXTENSION  + (((pDpSystem->bDinBufsize + SEG_OFFSET) & SEG_ADDBYTE)>>SEG_MULDIV);
         pVpc3->abDiagBufPtr[1] VPC3_EXTENSION  = pVpc3->abDiagBufPtr[0] VPC3_EXTENSION  + (((pDpSystem->bDiagBufsize + SEG_OFFSET) & SEG_ADDBYTE)>>SEG_MULDIV);

         pVpc3->bCfgBufPtr VPC3_EXTENSION       = pVpc3->abDiagBufPtr[1] VPC3_EXTENSION  + (((pDpSystem->bDiagBufsize + SEG_OFFSET) & SEG_ADDBYTE)>>SEG_MULDIV);
         pVpc3->bReadCfgBufPtr VPC3_EXTENSION   = pVpc3->bCfgBufPtr      VPC3_EXTENSION  + (awRealBufferLength[RBL_CFG]>>SEG_MULDIV);

         pVpc3->bPrmBufPtr VPC3_EXTENSION       = pVpc3->bReadCfgBufPtr  VPC3_EXTENSION  + (awRealBufferLength[RBL_CFG]>>SEG_MULDIV);

         pVpc3->abAuxBufPtr[0] VPC3_EXTENSION   = pVpc3->bPrmBufPtr VPC3_EXTENSION       + (awRealBufferLength[RBL_PRM]>>SEG_MULDIV);
         pVpc3->abAuxBufPtr[1] VPC3_EXTENSION   = ( awAuxBufferLength[1] != 0 ) ? pVpc3->abAuxBufPtr[0] VPC3_EXTENSION   + ( awAuxBufferLength[0] >> SEG_MULDIV ) : 0;

         if( awRealBufferLength[RBL_SSA] == 0 )
         {
            // no ssa supported
            pVpc3->bSsaBufPtr VPC3_EXTENSION = 0;
         }//if( awRealBufferLength[RBL_SSA] == 0 )
         else
         {
            // ssa supported
            pVpc3->bSsaBufPtr VPC3_EXTENSION = ( awAuxBufferLength[1] != 0 ) ? pVpc3->abAuxBufPtr[1] VPC3_EXTENSION + ( awAuxBufferLength[1] >> SEG_MULDIV )
                                                             : pVpc3->abAuxBufPtr[0] VPC3_EXTENSION + ( awAuxBufferLength[0] >> SEG_MULDIV );
         }//else of if( awRealBufferLength[RBL_SSA] == 0 )

         /*---------------------------------------------------------------*/
         /* set buffer length                                             */
         /*---------------------------------------------------------------*/
         pVpc3->bLenDoutBuf VPC3_EXTENSION     = pDpSystem->bOutputDataLength;
         pVpc3->bLenDinBuf  VPC3_EXTENSION     = pDpSystem->bInputDataLength;

         pVpc3->abLenDiagBuf[0] VPC3_EXTENSION = 6;                // length of Diag Buffers
         pVpc3->abLenDiagBuf[1] VPC3_EXTENSION = 6;

         pVpc3->abLenCtrlBuf[0] VPC3_EXTENSION = ( awAuxBufferLength[0] >= 244 ) ? 244 : awAuxBufferLength[0];      // Length AUX1-Buffer
         pVpc3->abLenCtrlBuf[1] VPC3_EXTENSION = ( awAuxBufferLength[1] >= 244 ) ? 244 : awAuxBufferLength[1];      // Length AUX2-Buffer

         pVpc3->bLenCfgData VPC3_EXTENSION     = pDpSystem->bCfgBufsize;
         pVpc3->bLenPrmData VPC3_EXTENSION     = pDpSystem->bPrmBufsize;
         pVpc3->bLenSsaBuf  VPC3_EXTENSION     = pDpSystem->bSsaBufsize;

         pVpc3->bLenDoutBuf VPC3_EXTENSION     = pDpSystem->bOutputDataLength;
         pVpc3->bLenDinBuf  VPC3_EXTENSION     = pDpSystem->bInputDataLength;

         pDpSystem->pDoutBuffer1 = (VPC3_UNSIGNED8_PTR)((VPC3_ADR)(pVpc3->abDoutBufPtr[0] VPC3_EXTENSION << SEG_MULDIV)+(VPC3_ADR)Vpc3AsicAddress);                                                                                                                                                                                                       ;
         pDpSystem->pDoutBuffer2 = (VPC3_UNSIGNED8_PTR)((VPC3_ADR)(pVpc3->abDoutBufPtr[1] VPC3_EXTENSION << SEG_MULDIV)+(VPC3_ADR)Vpc3AsicAddress);
         pDpSystem->pDoutBuffer3 = (VPC3_UNSIGNED8_PTR)((VPC3_ADR)(pVpc3->abDoutBufPtr[2] VPC3_EXTENSION << SEG_MULDIV)+(VPC3_ADR)Vpc3AsicAddress);

         pDpSystem->pDinBuffer1  = (VPC3_UNSIGNED8_PTR)((VPC3_ADR)(pVpc3->abDinBufPtr[0] VPC3_EXTENSION << SEG_MULDIV)+(VPC3_ADR)Vpc3AsicAddress);
         pDpSystem->pDinBuffer2  = (VPC3_UNSIGNED8_PTR)((VPC3_ADR)(pVpc3->abDinBufPtr[1] VPC3_EXTENSION << SEG_MULDIV)+(VPC3_ADR)Vpc3AsicAddress);
         pDpSystem->pDinBuffer3  = (VPC3_UNSIGNED8_PTR)((VPC3_ADR)(pVpc3->abDinBufPtr[2] VPC3_EXTENSION << SEG_MULDIV)+(VPC3_ADR)Vpc3AsicAddress);

         pDpSystem->pDiagBuffer1 = (VPC3_UNSIGNED8_PTR) (((VPC3_ADR)(pVpc3->abDiagBufPtr[0] VPC3_EXTENSION) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress));
         pDpSystem->pDiagBuffer2 = (VPC3_UNSIGNED8_PTR) (((VPC3_ADR)(pVpc3->abDiagBufPtr[1] VPC3_EXTENSION) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress));
      }//else of if( pDpSystem->wVpc3UsedDpBufferMemory > pDpSystem->wAsicUserRam )
   }//if( bError == DP_OK )

   return bError;
}//DP_ERROR_CODE VPC3_InitBufferStructure( void )

/*---------------------------------------------------------------------------*/
/* function: VPC3_GetDoutBufPtr                                              */
/*---------------------------------------------------------------------------*/
/*!
  \brief Fetch buffer pointer of the current output buffer.

  \param[out] pbState

  \retval !0 - pointer to current output buffer.
  \retval 0 - no buffer available
*/
VPC3_UNSIGNED8_PTR VPC3_GetDoutBufPtr( MEM_UNSIGNED8_PTR pbState )
{
VPC3_UNSIGNED8_PTR pToOutputBuffer;             // pointer to output buffer ( DP-Master -> VPC3+ )

   *pbState = VPC3_GET_NEXT_DOUT_BUFFER_CMD();

   switch( VPC3_GET_DOUT_BUFFER_SM() )          // locate user Dout Buffer
   {
      case 0x10:
      {
         pToOutputBuffer = pDpSystem->pDoutBuffer1;
         break;
      }//case 0x10:

      case 0x20:
      {
         pToOutputBuffer = pDpSystem->pDoutBuffer2;
         break;
      }//case 0x20:

      case 0x30:
      {
         pToOutputBuffer = pDpSystem->pDoutBuffer3;
         break;
      }//case 0x30:

      default:
      {
        //SSN Typecasted the NULL_PTR  to remove the error. 
        pToOutputBuffer = (VPC3_UNSIGNED8_PTR)NULL_PTR;
         break;
      }//default:
   }//switch( VPC3_GET_DOUT_BUFFER_SM() )

   return pToOutputBuffer;
}//VPC3_UNSIGNED8_PTR VPC3_GetDoutBufPtr( MEM_UNSIGNED8_PTR pbState )

/*---------------------------------------------------------------------------*/
/* function: VPC3_GetDinBufPtr                                               */
/*---------------------------------------------------------------------------*/
/*!
  \brief Fetch buffer pointer of the next input buffer.

  \retval !0 - pointer to current input buffer.
  \retval 0 - no buffer available
*/
VPC3_UNSIGNED8_PTR VPC3_GetDinBufPtr( void )
{
VPC3_UNSIGNED8_PTR pToInputBuffer;     // pointer to input buffer ( VPC3 -> DP-Master )

   switch( VPC3_GET_DIN_BUFFER_SM() )  // locate user Din Buffer
   {
      case 0x10:
      {
         pToInputBuffer = pDpSystem->pDinBuffer1;
         break;
      }//case 0x10:

      case 0x20:
      {
         pToInputBuffer = pDpSystem->pDinBuffer2;
         break;
      }//case 0x20:

      case 0x30:
      {
         pToInputBuffer = pDpSystem->pDinBuffer3;
         break;
      }//case 0x30:

      default:
      {
         //SSN Typecasted the NULL_PTR  to remove the error. 
          pToInputBuffer = (VPC3_UNSIGNED8_PTR)NULL_PTR;
         break;
      }//default:
   }//switch( VPC3_GET_DIN_BUFFER_SM() )

   return pToInputBuffer;
}//VPC3_UNSIGNED8_PTR VPC3_GetDinBufPtr( void )

/*--------------------------------------------------------------------------*/
/* function: VPC3_InputDataUpdate                                           */
/*--------------------------------------------------------------------------*/

/*!
  \brief Copy input data to VPC3+ and perform buffer exchange.

  \param[in] pToInputData - pointer to local input data
*/
void VPC3_InputDataUpdate( MEM_UNSIGNED8_PTR pToInputData )
{
VPC3_UNSIGNED8_PTR   pToInputBuffer;  // pointer to input buffer
volatile UBYTE       bResult;

   // write DIN data to VPC3
   pToInputBuffer = VPC3_GetDinBufPtr();
   if( pToInputBuffer != NULL_PTR )
   {
      CopyToVpc3_( pToInputBuffer, pToInputData, (UWORD)pDpSystem->bInputDataLength );

      // the user makes a new Din-Buffer available in the state N
      bResult = VPC3_INPUT_UPDATE();
   }//if( pToInputBuffer != NULL_PTR )
}//void VPC3_InputDataUpdate( MEM_UNSIGNED8_PTR pToInputData )

/*---------------------------------------------------------------------------*/
/* function: VPC3_GetDxbBufPtr                                               */
/*---------------------------------------------------------------------------*/
/*!
  \brief Fetch buffer pointer of the current dxb buffer.

  \param[out] pbState

  \retval !0 - pointer to current dxb buffer.
  \retval 0 - no buffer available
*/
#if DP_SUBSCRIBER
VPC3_UNSIGNED8_PTR VPC3_GetDxbBufPtr( MEM_UNSIGNED8_PTR pbState )
{
VPC3_UNSIGNED8_PTR pToDxbBuffer; // pointer to DxB buffer

   *pbState = VPC3_GET_NEXT_DXB_OUT_BUFFER_CMD();

   switch( VPC3_GET_DXB_OUT_BUFFER_SM() )          // locate user Dxb out Buffer
   {
      case 0x10:
      {
         pToDxbBuffer = pDpSystem->pDxbOutBuffer1;
         break;
      }//case 0x10:

      case 0x20:
      {
         pToDxbBuffer = pDpSystem->pDxbOutBuffer2;
         break;
      }//case 0x20:

      case 0x30:
      {
         pToDxbBuffer = pDpSystem->pDxbOutBuffer3;
         break;
      }//case 0x30:

      default:
      {
         pToDxbBuffer = NULL_PTR;
         break;
      }//default:
   }//switch( VPC3_GET_DXB_OUT_BUFFER_SM() )

   return pToDxbBuffer;
}//VPC3_UNSIGNED8_PTR VPC3_GetDxbBufPtr( MEM_UNSIGNED8_PTR pbState )

/*-------------------------------------------------------------------*/
/* function: VPC3_CheckDxbLinkTable                                  */
/*-------------------------------------------------------------------*/
DP_ERROR_CODE VPC3_CheckDxbLinkTable( UBYTE bNrOfLinks )
{
DP_ERROR_CODE  bRetValue;
UBYTE          i;

   bRetValue = DP_OK;

   for( i = 0; i < bNrOfLinks; i++ )
   {
      if(     ( pDpSystem->sLinkTable.asLinkTableEntry[i].bPublisherAddress > 125 )
          ||  ( pDpSystem->sLinkTable.asLinkTableEntry[i].bPublisherLength  < 1   )
          ||  ( pDpSystem->sLinkTable.asLinkTableEntry[i].bPublisherLength  > 244 )
          ||  ( pDpSystem->sLinkTable.asLinkTableEntry[i].bSampleOffset     > ( pDpSystem->sLinkTable.asLinkTableEntry[i].bPublisherLength - 1 ) )
          || (( pDpSystem->sLinkTable.asLinkTableEntry[i].bSampleOffset + pDpSystem->sLinkTable.asLinkTableEntry[i].bSampleLength) >
                                                                              (pDpSystem->sLinkTable.asLinkTableEntry[i].bPublisherLength))
          ||  ( pDpSystem->sLinkTable.asLinkTableEntry[i].bSampleLength > MAX_DATA_PER_LINK-2 )
        )
      {
         bRetValue = DP_PRM_DXB_ERROR;
         break;
      }//if(     ( pDpSystem->sLinkTable.sLinkTableEntry[i].bPublisherAddress > 125 ) ...
   }//for( i = 0; i < bNrOfLinks; i++ )

   return bRetValue;
}//DP_ERROR_CODE VPC3_CheckDxbLinkTable( UBYTE bNrOfLinks )

/*-------------------------------------------------------------------*/
/* function: VPC3_BuildDxbLinkStatus                                 */
/*-------------------------------------------------------------------*/
void VPC3_BuildDxbLinkStatus( UBYTE bNrOfLinks )
{
DXB_LINK_STATUS   sLinkStatus;
UBYTE             i;

   memset( &sLinkStatus, 0, sizeof( DXB_LINK_STATUS ) );

   sLinkStatus.bHeader     = 4 + ( bNrOfLinks * 2 );
   sLinkStatus.bStatusType = 0x83;
   sLinkStatus.bSlotNr     = 0x00;
   sLinkStatus.bSpecifier  = 0x00;

   for( i = 0; i < bNrOfLinks; i++ )
   {
      sLinkStatus.asLinkStatus[i].bPublisherAddress = pDpSystem->sLinkTable.asLinkTableEntry[i].bPublisherAddress;
      sLinkStatus.asLinkStatus[i].bLinkStatus       = 0x00;
   }//for( i = 0; i < bNrOfLinks; i++ )

   VPC3_SET_DXB_LINK_STATUS_LEN( sLinkStatus.bHeader );
   CopyToVpc3_( VPC3_GET_DXB_LINK_STATUS_BUF_PTR(), (MEM_UNSIGNED8_PTR)&sLinkStatus, sLinkStatus.bHeader );
}//void VPC3_BuildDxbLinkStatus( UBYTE bNrOfLinks )

/*-------------------------------------------------------------------*/
/* function: VPC3_SubscriberToLinkTable                              */
/*-------------------------------------------------------------------*/
DP_ERROR_CODE VPC3_SubscriberToLinkTable( MEM_PRM_SUBSCRIBER_TABLE_PTR psSubsTable, UBYTE bNrOfLinks )
{
DP_ERROR_CODE  bRetValue;
UBYTE          i;

   memset( &pDpSystem->sLinkTable, 0, sizeof( PRM_DXB_LINK_TABLE ) );

   pDpSystem->sLinkTable.bVersion = psSubsTable->bVersion;

   for( i = 0; i < bNrOfLinks; i++ )
   {
      pDpSystem->sLinkTable.asLinkTableEntry[i].bPublisherAddress  = psSubsTable->asSubscriberTableEntry[i].bPublisherAddress;
      pDpSystem->sLinkTable.asLinkTableEntry[i].bPublisherLength   = psSubsTable->asSubscriberTableEntry[i].bPublisherLength;
      pDpSystem->sLinkTable.asLinkTableEntry[i].bSampleOffset      = psSubsTable->asSubscriberTableEntry[i].bSampleOffset;
      pDpSystem->sLinkTable.asLinkTableEntry[i].bSampleLength      = psSubsTable->asSubscriberTableEntry[i].bSampleLength;
   }//for( i = 0; i < bNrOfLinks; i++ )

   bRetValue = VPC3_CheckDxbLinkTable( bNrOfLinks );

   if( bRetValue == DP_OK )
   {
      VPC3_SET_DXB_LINK_TABLE_LEN( bNrOfLinks << 2 );
      CopyToVpc3_( VPC3_GET_DXB_LINK_TABLE_BUF_PTR(), &pDpSystem->sLinkTable.asLinkTableEntry[0].bPublisherAddress, ( bNrOfLinks << 2 ) );
      VPC3_BuildDxbLinkStatus( bNrOfLinks );
   }//if( bRetValue == DP_OK )

   return bRetValue;
}//DP_ERROR_CODE VPC3_SubscriberToLinkTable( MEM_PRM_SUBSCRIBER_TABLE_PTR psSubsTable, UBYTE bNrOfLinks )
#endif//#if DP_SUBSCRIBER

/*---------------------------------------------------------------------------*/
/* function: VPC3_GetMasterAddress                                           */
/*---------------------------------------------------------------------------*/
UBYTE VPC3_GetMasterAddress( void )
{
UBYTE  bMasterAddress;

   #if VPC3_SERIAL_MODE

      bMasterAddress = Vpc3Read( (VPC3_ADR)pDpSystem->pDiagBuffer1 + 3 );

      if( bMasterAddress == 0xFF )
      {
         bMasterAddress = Vpc3Read( (VPC3_ADR)pDpSystem->pDiagBuffer2 + 3 );
      }//if( bMasterAddress == 0xFF )

   #else

      bMasterAddress = *( pDpSystem->pDiagBuffer1 + 3 );

      if( bMasterAddress == 0xFF )
      {
         bMasterAddress = *( pDpSystem->pDiagBuffer2 + 3 );
      }//if( bMasterAddress == 0xFF )

   #endif//#if VPC3_SERIAL_MODE

   return( bMasterAddress );
}//UBYTE VPC3_GetMasterAddress( void )

/*---------------------------------------------------------------------------*/
/* function: VPC3_AssignAuxBuffer                                            */
/*---------------------------------------------------------------------------*/
UWORD VPC3_AssignAuxBuffer( UWORD MEM_PTR pwLens, UBYTE bCount, MEM_UNSIGNED8_PTR pbAssign, UWORD MEM_PTR pwAuxLen )
{
UWORD   wMinLen;        // calculated minimal length
UBYTE   bMinStep;       // step by calculated minimal length
UBYTE   i,j;
UBYTE   abPos[3];       // memory for position of length
UWORD   awLensx[3];     // workarea for calculated length
UBYTE   bStep;          // counter
UWORD   wTmp;           // tmp-variable for calculated length
UBYTE   bTmp;           // tmp-variable for calculated length
                        // tmp-varaiable for sorted positions

   wMinLen  = 0xFFFF;
   bMinStep = 0;

   // check count
   if(( bCount == 2 ) || ( bCount == 3 ))
   {
      for( i = 0; i < bCount; i++ )
      {
         // init memory for position of length
         abPos[i] = i;
         // copy length into workarea for calculated length
         awLensx[i] = pwLens[i];
         // put length to 8 Bit Segmentaddresses
         awLensx[i] = (awLensx[i] + SEG_OFFSET) & SEG_ADDWORD;
      }//for( i = 0; i < bCount; i++ )

      // sort length, highest length to index 0
      for( i = 0; i < bCount-1; i++ )
      {
         for( j = i+1; j < bCount; j++ )
         {
            if( awLensx[i] < awLensx[j] )
            {
               // found higher length
               //V504
               wTmp = awLensx[i];          // swap length
               awLensx[i] = awLensx[j];
               awLensx[j] = wTmp;

               bTmp = abPos[i];            // swap position
               abPos[i] = abPos[j];
               abPos[j] = bTmp;
            }//if( awLensx[i] < awLensx[j] )
         }//for( j = i+1; j < bCount; j++ )
      }//for( i = 0; i < bCount-1; i++ )

      // delete zero length from list lensx
      for( i = bCount; i > 0; i-- )
      {
         if( awLensx[i-1] == 0 )
         {
            bCount--;
         }//if( awLensx[i-1] == 0 )
      }//for( i = bCount; i > 0; i-- )

      if( bCount == 0 )
      {
         wMinLen = 0;     // Error: no defined buffer length
      }//if( bCount == 0 )

      // gradual length assign to aux-buffer
      for( bStep = 0; bStep < bCount; bStep++ )
      {
         // get length for AUX-Buffer 1
         pwAuxLen[0] = 0;

         for( i = bStep; i < bCount; i++ )
         {
            if( pwAuxLen[0] < awLensx[i] )
            {
               pwAuxLen[0] = awLensx[i];
            }//if( pwAuxLen[0] < awLensx[i] )
         }//for( i = bStep; i < bCount; i++ )

         pwAuxLen[0] = pwAuxLen[0] * (bCount - bStep + 1);

         // get length for AUX-Buffer 2
         pwAuxLen[1] = 0;

         for( i = 0; i < bStep; i++ )
         {
            if( pwAuxLen[1] < awLensx[i] )
            {
               pwAuxLen[1] = awLensx[i];
            }//if( pwAuxLen[1] < awLensx[i] )
         }//for( i = 0; i < bStep; i++ )

         pwAuxLen[1] = pwAuxLen[1] * (bStep + 1);

         if((pwAuxLen[0] + pwAuxLen[1]) < wMinLen )
         {
            // new minimal length found
            wMinLen = pwAuxLen[0] + pwAuxLen[1];
            bMinStep = bStep;
         }//if((pwAuxLen[0] + pwAuxLen[1]) < wMinLen )
      }//for( bStep = 0; bStep < bCount; bStep++ )

      // calculate length for AUX Buffer 1
      pwAuxLen[0] = 0;

      for( i = bMinStep; i < bCount; i++ )
      {
         if( pwAuxLen[0] < awLensx[i] )
         {
            pwAuxLen[0] = awLensx[i];
         }//if( pwAuxLen[0] < awLensx[i] )
      }//for( i = bMinStep; i < bCount; i++ )

      // put length, assigned for AUX buffer 1
      for( i = bMinStep; i < bCount; i++ )
      {
         pwLens[abPos[i]] = pwAuxLen[0];
      }//for( i = bMinStep; i < bCount; i++ )

      // calculate length of AUX-Buffer 2
      pwAuxLen[1] = 0;

      for( i = 0; i < bMinStep; i++ )
      {
         if( pwAuxLen[1] < awLensx[i] )
         {
            pwAuxLen[1] = awLensx[i];
         }//if( pwAuxLen[1] < awLensx[i] )
      }//for( i = 0; i < bMinStep; i++ )

      *pbAssign = 0;    // put all buffer to AUX-Buffer 1

      for(i = 0; i < bMinStep; i++)
      {
         pwLens[abPos[i]] = pwAuxLen[1];
         *pbAssign |= 0x1 << abPos[i];
      }

      //check here for VPC3+/B
      //handle Cfg-telegram only over AUX-Buffer 1
      if( *pbAssign & 0x02 )
      {
         bTmp = (*pbAssign);
         *pbAssign = ((~bTmp)&0x07);

         wTmp = pwAuxLen[0];
         pwAuxLen[0] = pwAuxLen[1];
         pwAuxLen[1] = wTmp;
      }//if( *pbAssign & 0x02 )
   }//if(( bCount == 2 ) || ( bCount == 3 ))
   else
   {
      wMinLen = 0; // error
   }//else of if(( bCount == 2 ) || ( bCount == 3 ))

   return wMinLen;
}//UWORD VPC3_AssignAuxBuffer( UWORD MEM_PTR pwLens, UBYTE bCount, MEM_UNSIGNED8_PTR pbAssign, UWORD MEM_PTR pwAuxLen )

/*---------------------------------------------------------------------------*/
/* function: VPC3_SetIoDataLength                                            */
/*---------------------------------------------------------------------------*/
void VPC3_SetIoDataLength( void )
{
   #if VPC3_SERIAL_MODE

      //length of buffers OK, set real buffers
      Vpc3Write( bVpc3RwLenDoutBuf, pDpSystem->bOutputDataLength );
      Vpc3Write( bVpc3RwLenDinBuf, pDpSystem->bInputDataLength );

   #else

      //length of buffers OK, set real buffers
      pVpc3->bLenDoutBuf VPC3_EXTENSION = pDpSystem->bOutputDataLength;
      pVpc3->bLenDinBuf  VPC3_EXTENSION = pDpSystem->bInputDataLength;

   #endif//#if VPC3_SERIAL_MODE
}//void VPC3_SetIoDataLength( void )


/*---------------------------------------------------------------------------*/
/* function: VPC3_GetDiagBufPtr                                             */
/*---------------------------------------------------------------------------*/
/*!
  \brief Fetch buffer pointer of the next diagnostic buffer.

  \param

  \retval !0 - pointer to current diagnostic buffer.
  \retval 0 - no buffer available
*/
VPC3_UNSIGNED8_PTR VPC3_GetDiagBufPtr( void )
{
VPC3_UNSIGNED8_PTR pToDiagBuffer;               // pointer to diagnosis buffer
UBYTE              bDiagBufferSm;

   bDiagBufferSm = VPC3_GET_DIAG_BUFFER_SM();

   if( ( bDiagBufferSm & 0x03 ) == 0x01 )       // locate Diag Buffer
   {
      pToDiagBuffer = pDpSystem->pDiagBuffer1;
   }//if( ( bDiagBufferSm & 0x03 ) == 0x01 )
   else
   {
      if( ( bDiagBufferSm & 0x0C ) == 0x04 )
      {
         pToDiagBuffer = pDpSystem->pDiagBuffer2;
      }//if( ( bDiagBufferSm & 0x0C ) == 0x04 )
      else
      {
         //SSN Typecasted the NULL_PTR  to remove the error. 
          pToDiagBuffer = (VPC3_UNSIGNED8_PTR)NULL_PTR;
      }//else of if( ( bDiagBufferSm & 0x0C ) == 0x04 )
   }//else of if( ( bDiagBufferSm & 0x03 ) == 0x01 )

   return pToDiagBuffer;
}//VPC3_UNSIGNED8_PTR VPC3_GetDiagBufPtr( void )

/*---------------------------------------------------------------------------*/
/* function: VPC3_UpdateDiagnosis                                            */
/*---------------------------------------------------------------------------*/
VPC3_UNSIGNED8_PTR VPC3_UpdateDiagnosis( UBYTE bDiagState, UBYTE bDiagLen )
{
VPC3_UNSIGNED8_PTR pDiagBuffer; // pointer to diagnosis buffer
UBYTE              bNewDiagBufferCmd;
UBYTE              bDiagBufferSm;

   bDiagBufferSm = VPC3_GET_DIAG_BUFFER_SM();

   if( ( bDiagBufferSm & 0x03 ) == 0x01 )                      // locate Diag Buffer
   {
      #if VPC3_SERIAL_MODE
         Vpc3Write( bVpc3RwLenDiagBuf1, bDiagLen );
         Vpc3Write( (VPC3_ADR)pDpSystem->pDiagBuffer1, bDiagState );
      #else
         pVpc3->abLenDiagBuf[0] VPC3_EXTENSION = bDiagLen;     // length of Diag Buffer 1
         *(pDpSystem->pDiagBuffer1) = bDiagState;
      #endif//#if VPC3_SERIAL_MODE
   }//if( ( bDiagBufferSm & 0x03 ) == 0x01 )
   else
   {
      if( ( bDiagBufferSm & 0x0C ) == 0x04 )
      {
         #if VPC3_SERIAL_MODE
            Vpc3Write( bVpc3RwLenDiagBuf2, bDiagLen );
            Vpc3Write( (VPC3_ADR)pDpSystem->pDiagBuffer2, bDiagState );
         #else
            pVpc3->abLenDiagBuf[1] VPC3_EXTENSION = bDiagLen;  // length of Diag Buffer 2
            *(pDpSystem->pDiagBuffer2) = bDiagState;
         #endif//#if VPC3_SERIAL_MODE
      }//if( ( bDiagBufferSm & 0x0C ) == 0x04 )
   }//else of if( ( bDiagBufferSm & 0x03 ) == 0x01 )

   bNewDiagBufferCmd = VPC3_GET_NEW_DIAG_BUFFER_CMD();

   switch( bNewDiagBufferCmd & 0x03 )
   {
      case 1:   // use buffer 1
      {
         pDiagBuffer = pDpSystem->pDiagBuffer1;
         break;
      }//case 1:

      case 2:   // use buffer 2
      {
         pDiagBuffer = pDpSystem->pDiagBuffer2;
         break;
      }//case 2:

      default:
      {
         // no buffer available
          //SSN Typecasted the NULL_PTR  to remove the error. 
         pDiagBuffer = (VPC3_UNSIGNED8_PTR)NULL_PTR;
         break;
      }//default:
   }//switch( bNewDiagBufferCmd & 0x03 )

   return pDiagBuffer;
}//VPC3_UNSIGNED8_PTR VPC3_UpdateDiagnosis( UBYTE bDiagState, UBYTE bDiagLen )

/*---------------------------------------------------------------------------*/
/* function: VPC3_SetDiagnosis                                               */
/*---------------------------------------------------------------------------*/
/*!
  \brief Set diagnostic data to VPC3+.
  By calling this function, the user provides diagnostic data to the slave. The diagnostic
  data is sent at the next possible time.
  The user has to make sure that the buffer size does not exceed the size of the
  diagnostic buffer that was set when the slave’s memory resources were set up:

  Diagnostic Buffer Length >= LengthDiag_User + LengthAlarm_User

  \attention This function is not suitable for setting alarms.

  The user can set up DP diagnostics; the following applies:
   - The 6 byte standard diagnostic (refer to EN 50170) is not part of the user diagnostic.
   - In DP standard operation, one ID-related, several channel-related, and one
   - device-related diagnostics may be utilized.
   - In DPV1 operation, one revision, one ID-related, several channel-related, and
     one alarm.
   - The user is responsible for the content of the diagnostic data.

  \param[in] pbToDiagBuffer - pointer to structure with alarm/diagnostic data
  \param[in] bUserDiagLength - length of the current user diagnostic
  \param[in] bDiagControl - add diagnostic bits
  \param[in] bCheckDiagFlag - check VPC3+ diagnostic flag

  Values for bUserDiagLength:
  - 0 - A previously set user diagnostic is deleted from the slave’s diagnostic buffer. Only 6 bytes
        standard diagnostic are sent in the diagnostic telegram. In this case, the user does not have to transfer a
        pointer to a diagnostic buffer.
  - 1..DIAG_BUFSIZE-6 - Length of the new user diagnostic data.

  Values for bDiagControl:
  - DIAG_RESET - Reset bit 'extended diagnostic, static diagnostic, diagnostic overflow'
  - EXT_DIAG_SET - Set bit 'extended diagnostic'.
  - STAT_DIAG_SET - Set bit 'static diagnostic'.

  \retval DP_OK - Execution OK, diagnostic message is copied into VPc3+
  \retval DP_DIAG_OLD_DIAG_NOT_SEND_ERROR - Error, wait because last diagnostic message isn't send
  \retval DP_DIAG_BUFFER_LENGTH_ERROR - Error, diagnostic message is too long
  \retval DP_DIAG_CONTROL_BYTE_ERROR - Error, sequence number is wrong
  \retval DP_DIAG_BUFFER_ERROR - Error, diagnostic header is wrong
  \retval DP_DIAG_SEQUENCE_ERROR - Error, revision will be send in wrong state
  \retval DP_DIAG_NOT_POSSIBLE_ERROR - Error, unknown diagnostic header
*/
DP_ERROR_CODE VPC3_SetDiagnosis( MEM_UNSIGNED8_PTR pbToDiagBuffer, UBYTE bUserDiagLength, UBYTE bDiagControl, UBYTE bCheckDiagFlag )
{
MEM_UNSIGNED8_PTR pbToDiagArray;
DP_ERROR_CODE     bRetValue;
UBYTE             bTmpUserDiagnosisLength;
UBYTE             bTmpLength;
UBYTE             bHeader;
UBYTE             bDpState;

   bRetValue = DP_OK;

   //check available diag buffer
   if( pDpSystem->pDiagBuffer != NULL_PTR )
   {
      bTmpUserDiagnosisLength = bUserDiagLength;
      pbToDiagArray = pbToDiagBuffer;

      bDpState = VPC3_GET_DP_STATE();
      if( ( bDpState == DATA_EX ) && ( bCheckDiagFlag == TRUE ) )
      {
         if( VPC3_GET_DIAG_FLAG() )
         {
            //old diagnosis not send
            bRetValue = DP_DIAG_OLD_DIAG_NOT_SEND_ERROR;
         }//if( VPC3_GET_DIAG_FLAG() )
      }//if( ( bDpState == DATA_EX ) && ( bCheckDiagFlag == TRUE ) )

      // check bUserDiagLength
      if( bUserDiagLength > ( DIAG_BUFSIZE - 6 ) )
      {
         bRetValue = DP_DIAG_BUFFER_LENGTH_ERROR;
      }//if( bUserDiagLength > ( DIAG_BUFSIZE - 6 ) )

      if( bRetValue == DP_OK )
      {
         //check control byte
         switch( bDiagControl )
         {
            case EXT_DIAG_SET:
            {
               if( bUserDiagLength == 0 )
               {
                   bRetValue = DP_DIAG_CONTROL_BYTE_ERROR;
               }//if( bUserDiagLength == 0 )
               break;
            }//case EXT_DIAG_SET:

            default:
            {
               //do nothing
               break;
            }//default:
         }//switch( bDiagControl )

         //check user diag buffer contents
         while( ( 0 < bTmpUserDiagnosisLength ) && ( DP_OK == bRetValue ) )
         {
            bHeader = pbToDiagArray[0];
            switch( DIAG_TYPE_MASK & bHeader )
            {
               case DIAG_TYPE_DEV:
               {
                  bTmpLength = (( ~DIAG_TYPE_MASK ) & bHeader );
                  if( STATUS_DIAG_HEAD_SIZE > bTmpLength )
                  {
                     bRetValue = DP_DIAG_BUFFER_ERROR;
                  }//if( STATUS_DIAG_HEAD_SIZE > bTmpLength )
                  break;
               }//case DIAG_TYPE_DEV:

               case DIAG_TYPE_KEN:
               {
                  bTmpLength = (( ~DIAG_TYPE_MASK ) & bHeader );
                  if ( bTmpLength == 0 )
                  {
                     bRetValue = DP_DIAG_BUFFER_ERROR;
                  }//if ( bTmpLength == 0 )
                  break;
               }//case DIAG_TYPE_KEN:

               case DIAG_TYPE_CHN:
               {
                  bTmpLength = DIAG_TYPE_CHN_SIZE;
                  break;
               }//case DIAG_TYPE_CHN:

               case DIAG_TYPE_REV:
               {
                  bTmpLength = DIAG_TYPE_REV_SIZE;
                  if( bDpState != DATA_EX )
                  {
                     // only allowed in state DATA_EX
                     bRetValue = DP_DIAG_SEQUENCE_ERROR;
                  }//if( bDpState != DATA_EX )
                  break;
               }//case DIAG_TYPE_REV:

               default:
               {
                  //not possible!
                  bTmpLength = 0;
                  bRetValue = DP_DIAG_NOT_POSSIBLE_ERROR;
                  break;
               }//default:
            }//switch( DIAG_TYPE_MASK & bHeader )

            bTmpUserDiagnosisLength -= bTmpLength;
            pbToDiagArray += bTmpLength;
         }//while( ( 0 < bTmpUserDiagnosisLength ) && ( DP_OK == bRetValue ) )

         if( bRetValue == DP_OK )
         {
            //copy to diagnosis buffer
            CopyToVpc3_( pDpSystem->pDiagBuffer+DIAG_NORM_DIAG_SIZE, pbToDiagBuffer, bUserDiagLength );

            pDpSystem->pDiagBuffer = VPC3_UpdateDiagnosis( bDiagControl, (DIAG_NORM_DIAG_SIZE + bUserDiagLength) );
         }//if( bRetValue == DP_OK )
      }//if( bRetValue == DP_OK )
   }//if( pDpSystem->pDiagBuffer != NULL_PTR )
   else
   {
      //Fetch new diagnosis buffer
      pDpSystem->pDiagBuffer = VPC3_GetDiagBufPtr();
      //wait for next free diag_buffer
      bRetValue = DP_DIAG_NO_BUFFER_ERROR;
   }//else of if( pDpSystem->pDiagBuffer != NULL_PTR )

   return bRetValue;
}//DP_ERROR_CODE VPC3_SetDiagnosis( MEM_UNSIGNED8_PTR pbToDiagBuffer, UBYTE bUserDiagLength, UBYTE bDiagControl, UBYTE bCheckDiagFlag )

/*-------------------------------------------------------------------*/
/* function: VPC3_SetAlarm                                           */
/*-------------------------------------------------------------------*/
/*!
  \brief Set alarm to VPC3+.
  By calling this function, the stack accepts the transferred alarm data. In addition to the net
  data, the alarm data also includes control information according to the DPV1
  specification . The data is transmitted at the next possible time. The user has to make sure that
  the buffer size does not exceed the size of the diagnostic buffer that was set when the
  slave’s memory resources were defined.

  Diagnostic Buffer Length >= LengthDiag_User + LengthAlarm_User

  Specifications:
   - When setting alarms, the user has to adhere to the requirements regarding
     permissible alarm types that he was informed of when the alarm state machine was
     started.
   - The number of alarms that are permitted to be processed simultaneously during
     communication between parameterization master and slave is specified by the
     type- or sequence mode. It is entirely handled automatically by the stack; the user has no
     influence on it, and can thus set any number of alarms of all permitted types.
   - The user is responsible for the content of the alarm data.
   - The alarm buffer is to contain only one alarm.
   - The user has possibility to add DPV0 diagnostics like identifier related, modulestatus, etc. via the
     function UserAlarm.

  \param[in] psAlarm - pointer to structure with alarm data
  \param[in] bCallback - TRUE - the stack calls the function UserAlarm to add diagnostics like channel related / identifier related diagnostics.
  \param[in] bCallback - FALSE - the stack sends directly alarm data

  \retval SET_ALARM_OK - Execution OK, alarm add to alarm state machine
  \retval SET_ALARM_AL_STATE_CLOSED - Error, alarm state machine is closed
  \retval SET_ALARM_ALARMTYPE_NOTSUPP - Error, alarm type is not supported
  \retval SET_ALARM_SEQ_NR_ERROR - Error, sequence number is wrong
  \retval SET_ALARM_SPECIFIER_ERROR - Error, alarm specifier is wrong
*/
UBYTE VPC3_SetAlarm( ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCallback )
{
   #if DP_ALARM

      return AL_SetAlarm( psAlarm, bCallback );

   #else

      psAlarm   = psAlarm;    //avoid warning
      bCallback = bCallback;  //avoid warning

      return SET_ALARM_AL_STATE_CLOSED;

   #endif
}//UBYTE VPC3_SetAlarm( ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCallback )

/*-------------------------------------------------------------------*/
/* function: VPC3_GetFreeMemory                                      */
/*-------------------------------------------------------------------*/
/*!
   \brief Determine free memory space.

   \retval free memory size.
*/
UWORD VPC3_GetFreeMemory( void )
{
UWORD wFreeMemorySize;

   //return number of bytes of unused VPC3-ram
   wFreeMemorySize = ASIC_USER_RAM - pDpSystem->wVpc3UsedDpBufferMemory;

   return wFreeMemorySize;
}//UWORD VPC3_GetFreeMemory( void )

/*-------------------------------------------------------------------*/
/* function: VPC3_ProcessDpv1StateMachine                            */
/*-------------------------------------------------------------------*/
/*!
  \brief The application program has to call this function cyclically so that the DPV1 services can be processed.
*/
void VPC3_ProcessDpv1StateMachine( void )
{
   #if DP_MSAC_C1
      MSAC_C1_Process();   // state machine MSAC_C1
   #endif//#if DP_MSAC_C1

   #if DP_ALARM
      AL_AlarmProcess();   // state machine ALARM
   #endif//#if DP_ALARM

   #if DP_MSAC_C2
      MSAC_C2_Process();   // state machine MSAC_C2
   #endif//#if DP_MSAC_C2
}//void VPC3_ProcessDpv1StateMachine( void )

// Added for Galaxy //
void Vpc3Wait_1ms(void)
{
  unsigned int icount,jcount = 0;
   for(icount = 0;icount < 65535; icount++)
   {
     jcount++;
   }
    
}

/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

