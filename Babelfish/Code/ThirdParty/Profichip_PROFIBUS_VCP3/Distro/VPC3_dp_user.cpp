/**************************  Filename: dp_user.c  ****************************/
/* ========================================================================= */
/*                                                                           */
/* 0000  000   000  00000 0  000  0   0 0 0000                               */
/* 0   0 0  0 0   0 0     0 0   0 0   0 0 0   0      Einsteinstraﬂe 6        */
/* 0   0 0  0 0   0 0     0 0     0   0 0 0   0      91074 Herzogenaurach    */
/* 0000  000  0   0 000   0 0     00000 0 0000       Germany                 */
/* 0     00   0   0 0     0 0     0   0 0 0                                  */
/* 0     0 0  0   0 0     0 0   0 0   0 0 0          Tel: ++49-9132-744-200  */
/* 0     0  0  000  0     0  000  0   0 0 0    GmbH  Fax: ++49-9132-744-204  */
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/* Function:      Demo for PROFICHIP Extension Board AT89C5132               */
/*                This example simulates a modular profibus device           */
/*                with max 6 Modules.                                        */
/*                                                                           */
/*                Module  1: dip-switch PORT1                                */
/*                Module  2: dip-switch PORT2                                */
/*                Module  3: led PORT1                                       */
/*                Module  4: led PORT2                                       */
/*                Module  5: dio32                                           */
/*                Module  6: counter                                         */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Hardware requirements: ProfiChip Evaluation Board AT89C5132  (PA006101)   */
/*                        ProfiChip Evaluation Board VPC3+/C    (PA006103)   */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/* memory:  0000H ... 7FFFH: RAM                                             */
/*          8000H ... 8FFFH: VPC3+                                           */
/*          9000H ... 9FFFH: Reserved                                        */
/*          A000H ... AFFFH: FPGA                                            */
/*          B000H ... BFFFH: RTC                                             */
/*          C000H ... CFFFH: LCD                                             */
/*          D000H ... DFFFH: I/O Port 0                                      */
/*          E000H ... EFFFH: I/O Port 1                                      */
/*          F000H ... FFFFH: I/O Port 2                                      */
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
     \brief Application demo - PROFIBUS DPV1 with alarms.

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
#include "VPC3_platform.h"
#include "VPC3.h"
#include "DCI_Owner.h"
#include "ProfiBus_Parameterization.h"
#include "ProfiBus_Configuration.h"
#include "C445_Cfg_Default_Data.h"
#include "ProfiBus_Diagnostics.h"
#include "ProfiBus_Acyclic.h"
#include "Profibus_Data_Exchange.h"
#include "main.h"
#include "Prod_Device_Diag.h"
#include "Modbus_Defines.h"
#include "Services.h"
#include "Prod_Spec_PROFI_STM32F101.h"
#include "DCI_Constants.h"
#include "Prod_Spec_Services.h"
#include "Babelfish_Assert.h"

// Prototype added for Galaxy to avoid error //
UBYTE UserAlarm ( UBYTE bAlarmType, UBYTE bSeqNr, ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCheckDiagFlag );
uint8_t DecodeAndExecuteMBCommand(uint8_t *Dpv1Buffer);

/*---------------------------------------------------------------------------*/
/* defines, structures                                                       */
/*---------------------------------------------------------------------------*/
extern VPC3*  VPC3_ctrl; //SSN added
/*---------------------------------------------------------------------------*/
/* global user data definitions                                              */
/*---------------------------------------------------------------------------*/
VPC3_STRUC_PTR             pVpc3;               // pointer to Vpc3 structure
VPC3_STRUC_PTR             pVpc3Channel1;       // pointer to Vpc3 structure channel 1

VPC3_ADR                   Vpc3AsicAddress;     // global asic address

VPC3_SYSTEM_STRUC_PTR      pDpSystem;           // global system structure
VPC3_SYSTEM_STRUC          sDpSystemChannel1;   // global system structure

VPC3_STRUC_ERRCB           sVpc3Error;          // error structure
USER_STRUC                 sUser;               // user structure

#if VPC3_SERIAL_MODE
   VPC3_STRUC              sVpc3OnlyForInit;    // structure is used for initialization of VPC3+
                                                // buffer calculation
#endif//#if VPC3_SERIAL_MODE

tModuleStatusDiagnosis sModuleStatusDiagnosis;

UBYTE bTemp;
UBYTE bTempOld;

// Define struct for Error to be added by Galaxy //
  DP_ERROR_CODE GetMotorParamData (MEM_STRUC_MOTOR_PARAM_DATA_PTR ptrStructMotordata);

/*---------------------------------------------------------------------------*/
/* defines, structures and variables for our demo application                */
/*---------------------------------------------------------------------------*/
//SSN : In this case the Max Module changed from 0x07 to 0x06 to 0x01 
#define MaxModule ((UBYTE)0x07)
#define 	PROFIBUS_SEMAPHORE_TIMEOUT		10
  OS_Semaphore*				profi_activity; //SSN Added
  Profibus_Param::ProfiBus_Parameterization*  profi_parameterization_ctrl;
  Profibus_Cfg::ProfiBus_Configuration*		profi_cfg_ctrl;
  Profibus_Diag::ProfiBus_Diagnostics*       profi_diag_ctrl;
  Profibus_Data_Exch::Profibus_Data_Exchange*     profi_data_ex_ctrl;
#if VPC3_SERIAL_MODE
   #if VPC3_SPI
   #endif//#if VPC3_SPI

   #if VPC3_I2C
      ROMCONST__ UBYTE NAME[12] = { 0x44, 0x50, 0x56, 0x31, 0x41, 0x46, 0x46, 0x45, 0x2D, 0x49, 0x49, 0x43 }; //DPV1AFFE-IIC
   #endif//#if VPC3_I2C
#else
      ROMCONST__ UBYTE NAME[12] = { 0x44, 0x50, 0x56, 0x31, 0x41, 0x46, 0x46, 0x45, 0x20, 0x20, 0x20, 0x20 }; //DPV1AFFE
#endif//#if VPC3_SERIAL_MODE

ROMCONST__ UBYTE DefStatusDiag[] = { 0x06, 0x82, 0x00, 0x00, 0xAA, 0x02/*0x0A*/ };

//default configuration data for startup
// Changed for Galaxy //
ROMCONST__ UBYTE DefCfg[20] = { 0x42,0x04,0x00,0x01,    // 5 byte i/p; VS data: 0x00,0x01   
                               0x42,0x04,0x00,0x02,    // 5 byte i/p; VS data: 0x00,0x02
                               0x82,0x04,0x00,0x03,    // 5 byte o/p; VS data: 0x00,0x03
                               0x82,0x04,0x00,0x04,    // 5 byte o/p; VS data: 0x00,0x04
                               /*0xC1,0x03,0x03,0x05,*/ //4 byte i/p  4 byte o/p; VS data:0x03,0x05
                               0xC1,0x01,0x01,0x06      // 2 byte i/p 2 byte o/p; VS data:0x01,0x06        
                              };

#if DPV1_IM_SUPP

UBYTE IM_DEFAULT[64] = { 			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Header, 10 Octets
                                    0x01, 0x9D,                                                 // MANUFACTURER_ID, 2 Octets
                                    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // Order ID, 20 Octets, PA006300
                                    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // SerialNumber, 16 Octets, 000112
                                    0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                    0x00, 0x05,                                                 // HardwareRevision, 2 Octets, 1
                                    0x56, PCM_FIRMWARE_REV_MAJOR, PCM_FIRMWARE_REV_MINOR,PCM_FIRMWARE_REV_BUILD_B2,                                     // SoftwareRevision, 4 Octets, V602
                                    0x00, 0x00,                                                 // RevisionCounter
                                    0xF6, 0x00,                                                 // PROFILE ID, 2 Octets
                                    0x00, 0x00,                                                 // PROFILE SPECIFIC TYPE, 2 Octets
                                    0x01, 0x00,                                                 // IM Version, 2 Octets
                                    0x00, 0x00                                                  // IM Supported, 2 Octets
                       };

   #if IM1_SUPP
      ROMCONST__ UBYTE IM1_DEFAULT[64] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Header, 10 Octets
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // TagFunction
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                           0x20, 0x20,
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // TagLocation
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                           0x20, 0x20
                                         };
   #endif//#if IM1_SUPP

   #if IM2_SUPP
      ROMCONST__ UBYTE IM2_DEFAULT[64] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Header, 10 Octets
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // InstallationDate
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Reserved
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                         };
   #endif//#if IM2_SUPP

   #if IM3_SUPP
      ROMCONST__ UBYTE IM3_DEFAULT[64] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Header, 10 Octets
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // Descriptor
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                           0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                           0x20, 0x20, 0x20, 0x20
                                         };
   #endif//#if IM3_SUPP

   #if IM4_SUPP
      ROMCONST__ UBYTE IM4_DEFAULT[64] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Header, 10 Octets
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Signature
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00
                                         };
   #endif//#if IM4_SUPP
#endif//#if DPV1_IM_SUPP

STRUC_SYSTEM sSystem;
static volatile BOOL profiCommuStatus = FALSE;
static volatile BOOL fieldbusFault = FALSE;

/*---------------------------------------------------------------------------*/
/* function prototypes                                                       */
/*---------------------------------------------------------------------------*/
// Added by Sagar for Task based Profi-Bus implementation on 12-06-2012
void PROFI_Bus_Task(CR_Tasker* tasker, CR_TASKER_PARAM param);
/*--------------------------------------------------------------------------*/
/* function: UserAlarmInit                                                  */
/*--------------------------------------------------------------------------*/
/*!
   \brief Initializes user alarm structure.

   Set all values to zero.
*/
void UserAlarmInit(void)
{
   dpl_init_list__( &sUser.dplDiagQueue );

   memset( &sUser.sAlarm.abBufferUsed[0], 0, ALARM_MAX_FIFO );
}//void UserAlarmInit(void)

/*--------------------------------------------------------------------------*/
/* function: UserAlarmAlloc                                                 */
/*--------------------------------------------------------------------------*/
/*!
   \brief Fetch alarm/diagnostic buffer.

   \retval NULL_PTR; - no alarm/diagnostic buffer available.
   \retval != NULL_PTR; - alarm/diagnostic buffer
*/
ALARM_STATUS_PDU_PTR UserAlarmAlloc( void )
{
   ALARM_STATUS_PDU_PTR psAlarm;
   UBYTE                i;
    //SSN Typecasted the NULL_PTR  to remove the error.
   psAlarm = (ALARM_STATUS_PDU_PTR)NULL_PTR;
   for( i = 0; i < ALARM_MAX_FIFO; i++ )
   {
      if( sUser.sAlarm.abBufferUsed[i] == FALSE )
      {
         sUser.sAlarm.abBufferUsed[i] = TRUE;

         psAlarm = &(sUser.sAlarm.asAlarmBuffers[i]);
         psAlarm->bHeader         = 0;
         psAlarm->bAlarmType      = 0;
         psAlarm->bSlotNr         = 0;
         psAlarm->bSpecifier      = 0;
         psAlarm->bUserDiagLength = 0;
         psAlarm->pToUserDiagData = &(sUser.sAlarm.abUserAlarmData[i][0]);

         break;
      }//if( sUser.sAlarm.abBufferUsed[i] == FALSE )
   }//for( i = 0; i < ALARM_MAX_FIFO; i++ )

   return psAlarm;
}//ALARM_STATUS_PDU_PTR UserAlarmAlloc( void )

/*--------------------------------------------------------------------------*/
/* function: UserFreeAlarmBuffer                                            */
/*--------------------------------------------------------------------------*/
/*!
   \brief Set alarm/diagnostic buffer to state free.

   \param[in] psAlarm - alarm/diagnostic buffer
*/
void UserFreeAlarmBuffer( ALARM_STATUS_PDU_PTR psAlarm )
{
ALARM_STATUS_PDU_PTR psFindAlarm;
UBYTE                i;

   //V504
   for( i = 0; i < ALARM_MAX_FIFO; i++ )
   {
      if( sUser.sAlarm.abBufferUsed[i] == TRUE )
      {
         psFindAlarm = &(sUser.sAlarm.asAlarmBuffers[i]);
         if( psFindAlarm == psAlarm )
         {
            sUser.sAlarm.abBufferUsed[i] = FALSE;
            memset( psFindAlarm->pToUserDiagData, 0, MaxAlarmLength );
            memset( psFindAlarm, 0, sizeof( ALARM_STATUS_PDU ) );
            break;
         }//if( psFindAlarm == psAlarm )
      }//if( sUser.sAlarm.abBufferUsed[i] == FALSE )
   }//for( i = 0; i < ALARM_MAX_FIFO; i++ )
}//void UserFreeAlarmBuffer( ALARM_STATUS_PDU_PTR ptr )

/*---------------------------------------------------------------------------*/
/* function: UserResetDiagnosticBuffer                                       */
/*---------------------------------------------------------------------------*/
/*!
   \brief Reset diagnostic buffer.
*/
void UserResetDiagnosticBuffer( void )
{
   do
   {
      //fetch new diagnosis buffer
      pDpSystem->pDiagBuffer = VPC3_GetDiagBufPtr();
   }
   while( pDpSystem->pDiagBuffer == NULL_PTR );

   //clear diagnostic buffer
   UserAlarm( USER_TYPE_RESET_DIAG, USER_TYPE_RESET_DIAG, (ALARM_STATUS_PDU_PTR) NULL_PTR, FALSE );
}//void UserResetDiagnosticBuffer( void )

/*---------------------------------------------------------------------------*/
/* function: UserAlarm ( is also called from alarm state machine !!!! )      */
/*---------------------------------------------------------------------------*/
UBYTE UserAlarm( UBYTE bAlarmType, UBYTE bSeqNr, ALARM_STATUS_PDU_PTR psAlarm, UBYTE bCheckDiagFlag )
{
ROMCONST__ UBYTE abChnDiagSlot5[3] = { 0x84, 0xC1, 0x82 };  //module 5
ROMCONST__ UBYTE abChnDiagSlot6[3] = { 0x85, 0xC1, 0x82 };  //module 6

MEM_UNSIGNED8_PTR pbToDiagArray;
UBYTE           bRetValue;
UBYTE           bExtDiagFlag;
UBYTE           bDiagLength;
UBYTE           bError;
UWORD           wDiagEvent;

   bRetValue = 0x00;

   wDiagEvent = ( (UWORD)(bAlarmType << 8) | ((UWORD)bSeqNr) );

   //don't send diagnostic twice!
   if(    ( wDiagEvent  != pDpSystem->wOldDiag  )
       && ( FALSE == pDpSystem->bUserDiagActive )
     )
   {
      memset( &pDpSystem->abUserDiagnostic[0], 0x00, sizeof( pDpSystem->abUserDiagnostic ) );
      pbToDiagArray = pDpSystem->abUserDiagnostic;

      bExtDiagFlag = 0x00;
      bDiagLength = 0x00;

      switch( bAlarmType )
      {
         case USER_TYPE_CFG_OK:
         {
            bExtDiagFlag = STAT_DIAG_SET; 
            break;
         }//case USER_TYPE_CFG_OK:

         case USER_TYPE_CFG_NOK:
         {
            //modul_status diagnose
            memcpy( pbToDiagArray, &sModuleStatusDiagnosis.bHeader, cSizeOfModuleStatusDiagnosis );

            bExtDiagFlag = EXT_DIAG_SET;
            bDiagLength = cSizeOfModuleStatusDiagnosis;
            break;
         }//case USER_TYPE_CFG_NOK:

         case ALARM_TYPE_DIAGNOSTIC:
         {
            //only an example
            //add here for the appearing diagnostic alarm the channel related diagnostic
            bExtDiagFlag = EXT_DIAG_SET;
            //Commented following part for alarm implementation:Sumit,E9898897
        /*    if( psAlarm->bSlotNr == 0x05 )
            {
               //module 5
               memcpy( pbToDiagArray+bDiagLength, (UBYTE *)&abChnDiagSlot5[0], 3 );
               bDiagLength += 3;
            }//if( psAlarm->bSlotNr == 0x05 )
            else
            {
               //module 6
               memcpy( pbToDiagArray+bDiagLength, (UBYTE *)&abChnDiagSlot6[0], 3 );
               bDiagLength += 3;
            }//else of if( psAlarm->bSlotNr == 0x05 )*/

            memcpy( pbToDiagArray+bDiagLength, (UBYTE *)&psAlarm->bHeader, cSizeOfAlarmHeader );
            bDiagLength += cSizeOfAlarmHeader;
            memcpy( pbToDiagArray+bDiagLength, psAlarm->pToUserDiagData, cSizeOfUsrDiagnosticAlarmData );
            bDiagLength += psAlarm->bUserDiagLength;
            break;
         }//ALARM_TYPE_DIAGNOSTIC

         case USER_TYPE_DPV0:
         {
            if( psAlarm->bAlarmType == STATUS_MESSAGE )
            {
               //diagnostic alarm
               if( (psAlarm->bSpecifier & SPEC_MASK) == SPEC_APPEARS )
               {
                  //only an example
                  //add here for the appearing diagnostic alarm the channel related diagnostic
                  bExtDiagFlag = EXT_DIAG_SET;
                  if( psAlarm->bSlotNr == 0x05 )
                  {
                     //module 5
                     memcpy( pbToDiagArray+bDiagLength, (UBYTE *)&abChnDiagSlot5[0], 3 );
                     bDiagLength += 3;
                  }//if( psAlarm->bSlotNr == 0x05 )
                  else
                  {
                     //module 6
                     memcpy( pbToDiagArray+bDiagLength, (UBYTE *)&abChnDiagSlot6[0], 3 );
                     bDiagLength += 3;
                  }//if( psAlarm->bSlotNr == 0x05 )
               }//if( (psAlarm->bSpecifier & SPEC_MASK) == SPEC_APPEARS )
               memcpy( pbToDiagArray+bDiagLength, (UBYTE *)&psAlarm->bHeader, cSizeOfAlarmHeader );
               bDiagLength += cSizeOfAlarmHeader;
               memcpy( pbToDiagArray+bDiagLength, psAlarm->pToUserDiagData, cSizeOfUsrDiagnosticAlarmData );
               bDiagLength += cSizeOfUsrDiagnosticAlarmData;
            }//if( psAlarm->bAlarmType == STATUS_MESSAGE )
            else
            {
               //process alarm
               memcpy( pbToDiagArray+bDiagLength, (UBYTE *)&psAlarm->bHeader, cSizeOfAlarmHeader );
               bDiagLength += cSizeOfAlarmHeader;
               memcpy( pbToDiagArray+bDiagLength, psAlarm->pToUserDiagData, cSizeOfUsrProcessAlarmData );
               bDiagLength += cSizeOfUsrProcessAlarmData;
            }//else of if( psAlarm->bAlarmType == STATUS_MESSAGE )
            break;
         }//case USER_TYPE_DPV0:

         case USER_TYPE_RESET_DIAG:
         case USER_TYPE_APPL_RDY:
         case USER_TYPE_PRM_NOK:
         case USER_TYPE_PRM_OK:
         default:
         {
            bExtDiagFlag = 0x00;
            bDiagLength = 0x00;
            break;
         }//default:
      }//switch(user[vpc3_channel].diag_byte)

      pDpSystem->bUserDiagActive = TRUE;

      bError = VPC3_SetDiagnosis( pDpSystem->abUserDiagnostic, bDiagLength, bExtDiagFlag, bCheckDiagFlag );

      if( bError == DP_OK )
      {
         pDpSystem->wOldDiag = wDiagEvent;
         if( bAlarmType >= USER_TYPE_PRM_OK )
         {
            pDpSystem->bUserDiagActive = FALSE;
         }//if( alarm_type >= USER_TYPE_PRM_OK )

         bRetValue = DP_OK;
      }//if( bError == DP_OK )
      else
      {
         pDpSystem->bUserDiagActive = FALSE;

         #ifdef RS232_SERIO
            print_string("DIAG_DP_Error: ");
            print_hexbyte( bError );
            print_hexbyte( bAlarmType );
         #endif//#ifdef RS232_SERIO

         bRetValue = bError;
      }//else of if( bError == DP_OK )
   }//if(    ( wDiagEvent  != pDpSystem->wOldDiag  ) ...

   return bRetValue;
}//UBYTE UserAlarm( UBYTE diag_event, UBYTE crc_byte, ALARM_STATUS_PDU_PTR psAlarm )

uint8_t Alarm_Handler( uint8_t bModuleNr,uint8_t *data_source, uint8_t Alarm_Data_Length )
{
	ALARM_STATUS_PDU_PTR psAlarm;
	UBYTE bRetValue;
	UBYTE bError;

   bRetValue = TRUE;

   psAlarm = UserAlarmAlloc();
   if( psAlarm != NULL_PTR )
   {		
		sUser.bDiagnosticAlarmSeqNr = (sUser.bDiagnosticAlarmSeqNr + 1) & (MAX_SEQ_NR - 1);
		psAlarm->bHeader            = cSizeOfAlarmHeader + Alarm_Data_Length;
		psAlarm->bAlarmType         = ALARM_TYPE_DIAGNOSTIC;
		psAlarm->bSlotNr            = bModuleNr;
		psAlarm->bSpecifier         = (SPEC_GENERAL + (sUser.bDiagnosticAlarmSeqNr << SPEC_SEQ_START));
		psAlarm->bUserDiagLength    = Alarm_Data_Length;
		memcpy( psAlarm->pToUserDiagData, data_source, Alarm_Data_Length );
		bError = VPC3_SetAlarm( psAlarm, TRUE );
		if( bError != SET_ALARM_OK )
		{
			//error
			UserFreeAlarmBuffer( psAlarm );
			bRetValue = FALSE;
		}
   }
   else
   {
	   bRetValue = FALSE;
   }
   return bRetValue;
}

/*UBYTE UserDemoDiagnostic( UBYTE bDiagNr, UBYTE bModuleNr )
{
ROMCONST__ UBYTE diagnostic_alarm_appears[5]    = { 0x01,0x07,0x0F,0x67,0x33 };
ROMCONST__ UBYTE diagnostic_alarm_disappears[5] = { 0x01,0x00,0x0F,0x00,0x00 };
ROMCONST__ UBYTE process_alarm_upper_limit[3]   = { 0x02,0x01,0x00 };
ROMCONST__ UBYTE process_alarm_lower_limit[3]   = { 0x02,0x00,0x01 };

ALARM_STATUS_PDU_PTR psAlarm;
UBYTE bRetValue;
UBYTE bCallback;
UBYTE bError;

   bRetValue = TRUE;

   psAlarm = UserAlarmAlloc();
   if( psAlarm != NULL_PTR )
   {
      bCallback = FALSE;
      switch( bDiagNr )
      {
         case 1:
         case 3:
         {
            #ifdef RS232_SERIO
                print_string("\r\nDiag: ");
                print_hexbyte( bDiagNr );
            #endif//#ifdef RS232_SERIO

            sUser.bDiagnosticAlarmSeqNr = (sUser.bDiagnosticAlarmSeqNr + 1) & (MAX_SEQ_NR - 1);
            psAlarm->bHeader            = cSizeOfDiagnosticAlarm;
            psAlarm->bAlarmType         = ( pDpSystem->eDPV1 == DPV1_MODE ) ? ALARM_TYPE_DIAGNOSTIC : STATUS_MESSAGE;
            psAlarm->bSlotNr            = bModuleNr;
            psAlarm->bSpecifier         = SPEC_APPEARS + (sUser.bDiagnosticAlarmSeqNr << SPEC_SEQ_START);
            psAlarm->bUserDiagLength    = cSizeOfUsrDiagnosticAlarmData;
            memcpy( psAlarm->pToUserDiagData, &diagnostic_alarm_appears[0], cSizeOfUsrDiagnosticAlarmData );
            //The alarm state machine calls the function Useralarm again.
            //The user can can add to the alarm e.g. module status or channel related diagnostic.
            bCallback = TRUE;
            break;
         }//case 1:

         case 2:
         case 4:
         {

            #ifdef RS232_SERIO
                print_string("\r\nDiag: ");
                print_hexbyte( bDiagNr );
            #endif//#ifdef RS232_SERIO

            sUser.bDiagnosticAlarmSeqNr = (sUser.bDiagnosticAlarmSeqNr + 1) & (MAX_SEQ_NR - 1);
            psAlarm->bHeader            = cSizeOfDiagnosticAlarm;
            psAlarm->bAlarmType         = ( pDpSystem->eDPV1 == DPV1_MODE ) ? ALARM_TYPE_DIAGNOSTIC : STATUS_MESSAGE;
            psAlarm->bSlotNr            = bModuleNr;
            psAlarm->bSpecifier         = SPEC_DISAPPEARS + (sUser.bDiagnosticAlarmSeqNr << SPEC_SEQ_START);
            psAlarm->bUserDiagLength    = cSizeOfUsrDiagnosticAlarmData;
            memcpy( psAlarm->pToUserDiagData, &diagnostic_alarm_disappears[0], cSizeOfUsrDiagnosticAlarmData );
            break;
         }//case 2:

         case 5:
         {

            #ifdef RS232_SERIO
                print_string("\r\nDiag: ");
                print_hexbyte( bDiagNr );
            #endif//#ifdef RS232_SERIO

            sUser.bProcessAlarmSeqNr = (sUser.bProcessAlarmSeqNr + 1) & (MAX_SEQ_NR - 1);
            psAlarm->bHeader         = cSizeOfProcessAlarm;
            psAlarm->bAlarmType      = ( pDpSystem->eDPV1 == DPV1_MODE ) ? ALARM_TYPE_PROCESS : STATUS_MESSAGE;
            psAlarm->bSlotNr         = bModuleNr;
            psAlarm->bSpecifier      = SPEC_GENERAL + (sUser.bProcessAlarmSeqNr << SPEC_SEQ_START);
            psAlarm->bUserDiagLength = cSizeOfUsrProcessAlarmData;
            memcpy( psAlarm->pToUserDiagData, &process_alarm_upper_limit[0], cSizeOfUsrProcessAlarmData );
            break;
         }//case 5:

         case 6:
         {

            #ifdef RS232_SERIO
                print_string("\r\nDiag: ");
                print_hexbyte( bDiagNr );
            #endif//#ifdef RS232_SERIO

            sUser.bProcessAlarmSeqNr = (sUser.bProcessAlarmSeqNr + 1) & (MAX_SEQ_NR - 1);
            psAlarm->bHeader         = cSizeOfProcessAlarm;
            psAlarm->bAlarmType      = ( pDpSystem->eDPV1 == DPV1_MODE ) ? ALARM_TYPE_PROCESS : STATUS_MESSAGE;
            psAlarm->bSlotNr         = bModuleNr;
            psAlarm->bSpecifier      = SPEC_GENERAL + (sUser.bProcessAlarmSeqNr << SPEC_SEQ_START);
            psAlarm->bUserDiagLength = cSizeOfUsrProcessAlarmData;
            memcpy( psAlarm->pToUserDiagData, &process_alarm_lower_limit[0], cSizeOfUsrProcessAlarmData );
            break;
         }//case 6:

         default:
         {
            break;
         }//default:
      }//switch( bDiagNr )

      if( pDpSystem->eDPV1 == DPV1_MODE )
      {
         #ifdef RS232_SERIO
             print_string(" - DPV1");
         #endif//#ifdef RS232_SERIO

         bError = VPC3_SetAlarm( psAlarm, bCallback );

         if( bError != SET_ALARM_OK )
         {
            #ifdef RS232_SERIO
               print_string(" - Error!");
               print_hexbyte( bError );
            #endif//#ifdef RS232_SERIO

            //error
            UserFreeAlarmBuffer( psAlarm );
            bRetValue = FALSE;
         }//if( bError != SET_ALARM_OK )
      }//if( pDpSystem->eDPV1 == DPV1_MODE )
      else
      {
         UserAlarm( USER_TYPE_DPV0, psAlarm->bSpecifier, psAlarm, FALSE );
         UserFreeAlarmBuffer( psAlarm );
      }//else of if( pDpSystem->eDPV1 == DPV1_MODE )
   }//if( psAlarm != NULL_PTR )

   return bRetValue;
}//UBYTE UserDemoDiagnostic( UBYTE bDiagNr, UBYTE bModuleNr )*/
/*--------------------------------------------------------------------------*/
/* function: read_input_scan_data                                           */
/*--------------------------------------------------------------------------*/
void read_input_scan_data( void )
{
	DCI_LENGTH_TD			prod_data_len = 0;
	DCI_ERROR_TD			error;
	if ( profi_data_ex_ctrl->Is_Input_Scan_Data_Changed() )
	{
		prod_data_len = pDpSystem->bInputDataLength;
		// Bring the data into Chip buffer from Application
		error = profi_data_ex_ctrl->Copy_Input_Scan_Data( sSystem.abProdData, prod_data_len );
		if (error == DCI_ERR_NO_ERROR )
		{
			VPC3_InputDataUpdate( sSystem.abProdData );
		}
		else
		{
			VPC3_ctrl->VPC3_Update_Profi_Display(DP_ERROR);
		}
	}

}
/*--------------------------------------------------------------------------*/
/* function: ApplicationReady                                               */
/*--------------------------------------------------------------------------*/
/*!
  \brief The firmware calls this function when a ChkCfg.req is pending and the application program
   quit the function UserChkNewCfgData with DPV1_OK/DPV1_UPDATE.

   The user has now possibility to parameterize peripheriy modules and to read the
   input periphery.
*/
void ApplicationReady( void )
{
   #if DPV1_IM_SUPP
      pDpSystem->awImIndex[ 0x03 ] = 0x0000;
      pDpSystem->abCallService[ 0x03 ] = 0x00;
   #endif//#if DPV1_IM_SUPP
   read_input_scan_data();   
   {   
      pDpSystem->wEvent &= ~VPC3_EV_NEW_CFG_DATA;    // clear event
      pDpSystem->bApplicationReady = TRUE;
   }
}

#include "uC_USART.h"
extern uC_USART* usart_ctrl;
uint8_t VPC3_Memory_Test_Success1[22]= { '\n', '\r',
                              'V', 'P', 'C',
                              '3', 'M', 'E', 'M', '-',
                              'T', 'E', 'S', 'T', 
                              '\t', 
                              'S', 'U', 'C', 'C', 'E',
                              'S', 'S'
                              
                        };
uint8_t VPC3_Memory_Test_Success2[26]=
                              {
                              '\n', '\r','E', 'x', 'e', 'c', 'u','t','i',
                              'n','g',' ','P','r','o','f','i','b','u','s',
                              ' ','C','o','m','m','.',};
                                
uint8_t VPC3_Memory_Test_Success3[28]={
                              ' ','C','o','n','n','e','c','t',' ','C','a','r',
                              'd',' ','T','o',' ','P','r','o','f','i','b','u','s',
                              ' ','N','W',
                              };
/*---------------------------------------------------------------------------*/
/* function: ProfibusInit                                                    */
/*---------------------------------------------------------------------------*/
/*!
  \brief Initializing of PROFIBUS slave communication.
*/
void ProfibusInit( uint8_t Slave_Addr )
{
	DP_ERROR_CODE       bError;	
	ProfiBus_DCI*				profi_dci_ctrl;
   /*-----------------------------------------------------------------------*/
   /* init user data                                                        */
   /*-----------------------------------------------------------------------*/
   memset( &sUser, 0, sizeof(sUser) );

   /*-----------------------------------------------------------------------*/
   /* init Module-System                                                    */
   /*-----------------------------------------------------------------------*/
   memset( &sSystem, 0, sizeof(STRUC_SYSTEM) );
   sSystem.bNrOfModules = MaxModule;

   /*-----------------------------------------------------------------------*/
   /* initialize VPC3                                                       */
   /*-----------------------------------------------------------------------*/
   #if VPC3_SERIAL_MODE
      Vpc3AsicAddress = (VPC3_ADR)VPC3_ASIC_ADDRESS;
      pVpc3 = &sVpc3OnlyForInit;
      pDpSystem = &sDpSystemChannel1;

      memset( pVpc3, 0, sizeof( VPC3_STRUC ) );
   #else
      pVpc3Channel1 = (VPC3_STRUC_PTR)VPC3_ASIC_ADDRESS;
      Vpc3AsicAddress = (VPC3_ADR)VPC3_ASIC_ADDRESS;
      pVpc3 = pVpc3Channel1;
      pDpSystem = &sDpSystemChannel1;
   #endif//#if VPC3_SERIAL_MODE

   /*-----------------------------------------------------------------------*/
   /* initialize global system structure                                    */
   /*-----------------------------------------------------------------------*/
   memset( pDpSystem, 0, sizeof( VPC3_SYSTEM_STRUC ));

   /* TRUE  deactivates diagnosis handling ! */
   /* FALSE activates   diagnosis handling ! */
   pDpSystem->bUserDiagActive   = FALSE;
   pDpSystem->wOldDiag          = 0x00;
   pDpSystem->bApplicationReady = FALSE;
   pDpSystem->bState           = USER_STATE_CLEAR;

   //todo:
   //insert your real configuration data here
   // 3 bytes of configuration for RedBall //
   sUser.sCfgData.bLength = (MaxModule*4); // length of configuration data
   memcpy( &sUser.sCfgData.abData[0], &DefCfg[0], sUser.sCfgData.bLength );

   #if DPV1_IM_SUPP
   	  UpdateProfiBusIMBuffer();
      memcpy( &sUser.sIM_0.abHeader[0], &IM_DEFAULT[0], cSizeOfIM0 );
      sUser.sIM_0.wIMSupported = IM00_SUPPORTED;

      #if IM1_SUPP
         memcpy( &sUser.sIM_1.abHeader[0], &IM1_DEFAULT[0], cSizeOfIM1 );
         sUser.sIM_0.wIMSupported |= IM01_SUPPORTED;
      #endif//#if IM1_SUPP

      #if IM2_SUPP
         memcpy( &sUser.sIM_2.abHeader[0], &IM2_DEFAULT[0], cSizeOfIM2 );
         sUser.sIM_0.wIMSupported |= IM02_SUPPORTED;
      #endif//#if IM2_SUPP

      #if IM3_SUPP
         memcpy( &sUser.sIM_3.abHeader[0], &IM3_DEFAULT[0], cSizeOfIM3 );
         sUser.sIM_0.wIMSupported |= IM03_SUPPORTED;
      #endif//#if IM3_SUPP

      #if IM4_SUPP
         memcpy( &sUser.sIM_4.abHeader[0], &IM4_DEFAULT[0], cSizeOfIM4 );
         sUser.sIM_0.wIMSupported |= IM04_SUPPORTED;
      #endif//#if IM4_SUPP
   #endif//#if DPV1_IM_SUPP

   ClrResetVPC3Channel1();

#if 0 //TODO: Disable the Memory test till actuall root cause is identified  
   //Below routine is vary slow to execute,change the watch dog time temperory  and revert back
   // once test is done
   uC_Watchdog::Init( WATCHDOG_TIMEOUT_AT_POWER_UP_MS );
   bError = VPC3_MemoryTest();
   // Restore the watch dog time now
   uC_Watchdog::Init( WATCHDOG_TIMEOUT_AT_SCHEDULER_ENTRY_MS );
#endif
   if( DP_OK == bError )
   {
         #ifdef Hyperterminal_Test_Enable 
          for( i=0; i< 22; i++)
         {
            usart_ctrl->Set_TX_Data(VPC3_Memory_Test_Success1[i]);  
            while ( !usart_ctrl->TX_Data_Empty_Status() )
            {};             
         } 
         for( i=0; i< 26; i++)
         {
            usart_ctrl->Set_TX_Data(VPC3_Memory_Test_Success2[i]);  
            while ( !usart_ctrl->TX_Data_Empty_Status() )
            {};             
         }
         for( i=0; i< 28; i++)
         {
            usart_ctrl->Set_TX_Data(VPC3_Memory_Test_Success3[i]);  
            while ( !usart_ctrl->TX_Data_Empty_Status() )
            {};             
         } 
         #endif//Hyperterminal_Test_Enable
         profi_dci_ctrl = new ProfiBus_DCI( DCI_SOURCE_IDS_Get() );

         profi_cfg_ctrl = new Profibus_Cfg::ProfiBus_Configuration( profi_dci_ctrl );
         profi_cfg_ctrl->Initialize_Default_Configuration( &sUser.sCfgData.abData[0], &sUser.sCfgData.bLength );	 
          
         profi_parameterization_ctrl = new Profibus_Param::ProfiBus_Parameterization( profi_dci_ctrl );
         profi_cfg_ctrl->m_profi_usr_configuration = profi_parameterization_ctrl->m_profi_usr_parameterization;
         profi_data_ex_ctrl = new Profibus_Data_Exch::Profibus_Data_Exchange(profi_dci_ctrl,profi_cfg_ctrl);
         profi_data_ex_ctrl->Start_Input_Scan(PROD_REFRESH_RATE);
         profi_diag_ctrl = new Profibus_Diag::ProfiBus_Diagnostics( profi_dci_ctrl );
         bError = VPC3_Initialization( Slave_Addr, IDENT_NR, (psCFG)&sUser.sCfgData );
         uint8_t         Rd_Confg_Buffer[C445_DEFAULT_CFG_SIZE];
         CopyFromVpc3_(&Rd_Confg_Buffer[0],VPC3_GET_READ_CFG_BUF_PTR(), VPC3_GET_READ_CFG_LEN() );

         profi_activity = new OS_Semaphore();
      if( DP_OK == bError )
      {
      new CR_Tasker( PROFI_Bus_Task, NULL, CR_TASKER_PRIORITY_2, "PROFIBus Task" );
 
      	 //Initialize Extended User Data in Diagnostic buffer
		profi_diag_ctrl->Get_User_Diag_Data();
		profi_diag_ctrl->Diagnostic_Alarm ( USER_TYPE_DPV0, TRUE );
      
        //Enable_Interrupts(); //SSN Added 
        EnableInterruptVPC3Channel1();

         //todo: before startup the VPC3+, make here your own initialzations

         VPC3_Start();
      }//if( DP_OK == bError )
      else
      {
         sVpc3Error.bErrorCode = bError;
         FatalError( _DP_USER, __LINE__, &sVpc3Error );
      }//else of if( DP_OK == bError )
   }//if( DP_OK == bError )
   else
   {
      sVpc3Error.bErrorCode = bError;
      FatalError( _DP_USER, __LINE__, &sVpc3Error );
   }//else of if( DP_OK == bError )
}//void ProfibusInit( void )
// Added by Sagar for Task based Profi-Bus implementation on 12-06-2012
void PROFI_Bus_Task (CR_Tasker* tasker, CR_TASKER_PARAM param)
{
  	CR_Tasker_Begin( tasker );	
	while (TRUE)
	{
          ProfibusMain();
          CR_Tasker_Delay( tasker, 5 );	// mSec     // TBD -- This time needs to be decided after Timing analysis
	}
	CR_Tasker_End();
}   // End of PROFI_Bus_Task 

/*---------------------------------------------------------------------------*/
/* function: ProfibusMain                                                    */
/*---------------------------------------------------------------------------*/
/*!
  \brief The application program has to call this function cyclically so that the PROFIBUS DP slave services can be processed.
*/
void ProfibusMain( void )
{
	VPC3_UNSIGNED8_PTR pToOutputBuffer;    // pointer to output buffer
	UBYTE              bOutputState;       // state of output data
	volatile UBYTE bResult;
	uint8_t VPC3_DP_State;
	static bool Prm_Cfg_Error_Occured = FALSE;
	uint8_t         Confg_Buffer[C445_DEFAULT_CFG_SIZE];
   /*-------------------------------------------------------------------*/
   /* Poll PROFIBUS events                                              */
   /*-------------------------------------------------------------------*/
   //SSN commented since we will be using the VPC3_isr() invoked by interrupt 
   //VPC3_Poll();

   /*-------------------------------------------------------------------*/
   /* trigger watchdogs                                                 */
   /*-------------------------------------------------------------------*/
   // toggle user watchdog
   VPC3_RESET_USER_WD();   // toggle user watchdog
   
   #ifdef RS232_SERIO
      if( bRecCounter > 0 )
      {
         DisableInterruptVPC3Channel1();

         PrintSerialInputs();

         EnableInterruptVPC3Channel1();
      }//if( bRecCounter > 0 )
   #endif//#ifdef RS232_SERIO
    /*-------------------------------------------------------------------*/
    /* Diagnostic Alarm                                                  */
    /*-------------------------------------------------------------------*/
    profi_diag_ctrl->Diagnostic_Alarm ( USER_TYPE_DPV0, TRUE );
 
    //Handle Alarms if the DPv1 Mode is active
    if( pDpSystem->eDPV1 == DPV1_MODE )
    {
         profi_diag_ctrl->User_Alarm();
    }


	
   /*-------------------------------------------------------------------*/
   /* internal state machine                                            */
   /*-------------------------------------------------------------------*/
   if( pDpSystem->bState == USER_STATE_CLEAR )
   {
        // clear data // Clear is not allowed for Motor, to be changed.
        memset( &sSystem.sOutput.Control, 0, sizeof(STRUC_OUTPUT) );
        memset( &sSystem.sInput.Motor_Volt, 0, sizeof(STRUC_INPUT) );

        pDpSystem->bState = USER_STATE_RUN;
   }

   /*-------------------------------------------------------------------*/
   /* dpv1 statemachines                                                */
   /*-------------------------------------------------------------------*/
   VPC3_ProcessDpv1StateMachine();    
    /*-------------------------------------------------------------------*/
   /* VPC3+ DP-state                                                    */
   /*-------------------------------------------------------------------*/
   updateProfibusCommunicationStatus();
   VPC3_DP_State = VPC3_GET_DP_STATE();
   //Tweak around this for correct display
   if((VPC3_DP_State == WAIT_PRM) && (Prm_Cfg_Error_Occured == TRUE))
   {
	   //For Parameterization and Configuration errors,display state is same
	   // So change the display state forcefully for waiting for configuration
	   VPC3_DP_State = WAIT_CFG;
   }
   switch(VPC3_DP_State)
   {
      case WAIT_PRM:
      {
    	  VPC3_ctrl->VPC3_Update_Profi_Display( VPC3_DP_State );
    	  break;
      }

      case WAIT_CFG:
      {
    	 VPC3_ctrl->VPC3_Update_Profi_Display( VPC3_DP_State );
         break;
      }

      case DATA_EX:
      {         
         if(    ( pDpSystem->bApplicationReady == 1 )
             && ( pDpSystem->bState == USER_STATE_RUN  )
           )
         {
            /*-------------------------------------------------------------------*/
            /* user application                                                  */
            /*-------------------------------------------------------------------*/
            pDpSystem->wEvent |= VPC3_EV_NEW_INPUT_DATA;
           VPC3_ctrl->VPC3_Update_Profi_Display( VPC3_DP_State );
         }
         break;
      }

      case DP_ERROR:
    	  VPC3_ctrl->VPC3_Update_Profi_Display( VPC3_DP_State );
        break;

      default:
      {
         sVpc3Error.bErrorCode = VPC3_GET_DP_STATE();
         FatalError( _DP_USER, __LINE__, &sVpc3Error );
         break;
      }
   }
   //Check for the Hardware errors
   if(Profibus_System::Prod_Device_Diag::B_HW_FaultOccured() || Profibus_Diag::ProfiBus_Diagnostics::B_Trip_Fault_Occured() ||\
		   Profibus_System::B_Is_Slave_Address_Incorrect())
   {
	   VPC3_ctrl->VPC3_Update_Profi_Display( DP_ERROR );
   }
   else
   {
	   VPC3_ctrl->VPC3_Update_Profi_Display( NO_SYSTEM_FAULT );
   }
   #if VPC3_SERIAL_MODE
      //pDpSystem->wPollInterruptEvent = PollEvent();
   #endif//#if VPC3_SERIAL_MODE

      /*-------------------------------------------------------------------*/
      /* profibus input                                                    */
      /*-------------------------------------------------------------------*/
      if( pDpSystem->wEvent & VPC3_EV_NEW_INPUT_DATA )
      {
          read_input_scan_data();
         pDpSystem->wEvent &= ~VPC3_EV_NEW_INPUT_DATA;
      }//if( pDpSystem->wEvent & VPC3_EV_NEW_INPUT_DATA )
      /*-------------------------------------------------------------------*/
      /* profibus new parameter data for C445  Relay                   */                               
      /*-------------------------------------------------------------------*/
   if( pDpSystem->wEvent & VPC3_EV_NEW_PRM_DATA )
   {

        UBYTE bPrmLength;
          
         bResult = VPC3_PRM_FINISHED;

         #if DP_INTERRUPT_MASK_8BIT == 0
            VPC3_CON_IND_NEW_PRM_DATA();
         #endif//#if DP_INTERRUPT_MASK_8BIT == 0

         do
         {
			bPrmLength = VPC3_GET_PRM_LEN();
			CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&pDpSystem->abPrmCfgSsaHelpBuffer[0], VPC3_GET_PRM_BUF_PTR(), bPrmLength );
			 switch ( profi_parameterization_ctrl->Check_Prm( pDpSystem->abPrmCfgSsaHelpBuffer  ) )
			{
			  case Profibus_Param::PRM_OK:
					bResult = VPC3_SET_PRM_DATA_OK();
					break;

			  case Profibus_Param::PRM_ENTRY_ERROR:
					bResult = VPC3_SET_PRM_DATA_NOT_OK();
					Prm_Cfg_Error_Occured = TRUE;
                    Profibus_System::Prod_Device_Diag::Set_Parameterization_Fault();
					break;

			  case Profibus_Param::PRM_OK_UPDATE_PRM:
					bResult = VPC3_SET_PRM_DATA_OK();
					//TODO
					#if DP_MSAC_C1
						MSAC_C1_CheckIndNewPrmData( (MEM_STRUC_PRM_PTR)&pDpSystem->abPrmCfgSsaHelpBuffer[0], bPrmLength );
					#endif
					profi_parameterization_ctrl->CheckInMotorSpecificParam();
					//Add Acyclic parameterization here
					profi_parameterization_ctrl->Acyclic_Read_Parameterization();
					pDpSystem->wEvent &= ~VPC3_EV_NEW_PRM_DATA;
					Prm_Cfg_Error_Occured = FALSE;
					Profibus_System::Prod_Device_Diag::Clr_Parameterization_Fault();
					break;

			  default:
					bResult = VPC3_SET_PRM_DATA_NOT_OK();
					break;
			}
         }
         while( bResult == VPC3_PRM_CONFLICT );
   }//if( pDpSystem->wEvent & VPC3_EV_NEW_PRM_DATA )
          /*-------------------------------------------------------------------*/
          /* handle here profibus interrupt events                             */
          /*-------------------------------------------------------------------*/
          if( pDpSystem->wEvent & VPC3_EV_NEW_CFG_DATA )
          {				
                  bResult  = VPC3_CFG_FINISHED;
                  #if DP_INTERRUPT_MASK_8BIT == 0
                  VPC3_CON_IND_NEW_CFG_DATA();
                  #endif//#if DP_INTERRUPT_MASK_8BIT == 0
                  do
                  {
					  switch( profi_cfg_ctrl->Check_New_Config_Data(VPC3_GET_CFG_BUF_PTR(), VPC3_GET_READ_CFG_BUF_PTR(),
																	   VPC3_GET_CFG_LEN(), VPC3_GET_READ_CFG_LEN() ) )
					  {
						case VPC3_CFG_OK:
							  bResult = VPC3_SET_CFG_DATA_OK();
							  ApplicationReady();
							  break;

						case VPC3_CFG_FAULT:
							  bResult = VPC3_SET_CFG_DATA_NOT_OK();
							  Prm_Cfg_Error_Occured = TRUE;
                              Profibus_System::Prod_Device_Diag::Set_Configuration_Fault();
							  break;

						case VPC3_CFG_UPDATE:
							   //Calculate the length of the input and output using the configuration bytes
							  CopyFromVpc3_(&Confg_Buffer[0],VPC3_GET_CFG_BUF_PTR(),VPC3_GET_CFG_LEN());
							  if( DP_OK != VPC3_CalculateInpOutpLength( &Confg_Buffer[0], VPC3_GET_CFG_LEN() ) )
							  {
									  bResult = VPC3_SET_CFG_DATA_NOT_OK();
							  }
							  else
							  {
								  VPC3_SetIoDataLength();
								  profi_data_ex_ctrl->m_configuration_successful = true;
								  bResult = VPC3_SET_CFG_DATA_OK();
								  ApplicationReady();
							  }
							  Prm_Cfg_Error_Occured = FALSE;
							  Profibus_System::Prod_Device_Diag::Clr_Configuration_Fault();
							  break;

						default:
							  break;
					  }
                  }
                  while ( bResult == VPC3_CFG_CONFLICT );
                  pDpSystem->wEvent &= ~VPC3_EV_NEW_CFG_DATA;    // clear event			
                  
          }

   /*-------------------------------------------------------------------*/
   /* profibus output                                                   */
   /*-------------------------------------------------------------------*/
   if( pDpSystem->wEvent & VPC3_EV_DX_OUT )
   {
      
      pDpSystem->wEvent &= ~VPC3_EV_DX_OUT;       // clear event
      pToOutputBuffer = VPC3_GetDoutBufPtr( &bOutputState );
	  if( pToOutputBuffer != NULL_PTR )
	  {
		  CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sSystem.abConsData[0], pToOutputBuffer, (UWORD)pDpSystem->bOutputDataLength ); 	// Changed for Galaxy //
		  //handle data here (master --> slave) Copies output buffer to DCI area
		  profi_data_ex_ctrl->Copy_Cons_Mod_Data( &sSystem.abConsData[0] );
	  }

   }

   /*-------------------------------------------------------------------*/
   /* handle here profibus interrupt events                             */
   /*-------------------------------------------------------------------*/
   if( pDpSystem->wEvent & VPC3_EV_NEW_CFG_DATA )
   {
      ApplicationReady();
   }//if( pDpSystem->wEvent & VPC3_EV_NEW_CFG_DATA )
}//void ProfibusMain( void )
BOOL getProfiBusCommunicationStatus( void )
{
    return( profiCommuStatus );
}

void updateProfibusCommunicationStatus( void )
{
    profiCommuStatus = VPC3_ctrl->getProfiCommStatus( VPC3_GET_DP_STATE() );	// Updated by Sagar on 22 Nov 2012
}

BOOL getFieldbusFault( void )
{
    return( fieldbusFault );
}

/*---------------------------------------------------------------------------*/
/* function: FatalError                                                     */
/*---------------------------------------------------------------------------*/
void FatalError( DP_ERROR_FILE bFile, UWORD wLine, VPC3_ERRCB_PTR sVpc3Error )
{


   DP_WriteDebugBuffer__( FatalError__, sVpc3Error->bFunction, sVpc3Error->bErrorCode );

   #ifdef RS232_SERIO
      do
      {
         // wait!
      }
      while( bSndCounter > 80);

      print_string("\r\nFatalError:");
      print_string("\r\nFile: ");
      print_hexbyte( bFile );
      print_string("\r\nLine: ");
      print_hexword( wLine );
      print_string("\r\nFunction: ");
      print_hexbyte( sVpc3Error->bFunction);
      print_string("\r\nError_Code: ");
      print_hexbyte( sVpc3Error->bErrorCode );
      print_string("\r\nDetail: ");
      print_hexbyte( sVpc3Error->bDetail );
      print_string("\r\ncn_id: ");
      print_hexbyte( sVpc3Error->bCnId );
   #endif//#ifdef RS232_SERIO

   #ifdef EvaBoard_AT89C5132
      *WRITE_PORT0 = sVpc3Error->bErrorCode;
      *WRITE_PORT1 = bFile;
      *WRITE_PORT2 = (UBYTE)wLine;
   #endif//#ifdef EvaBoard_AT89C5132

// Commented for Galaxy as not applicable for our application //
//   SET_LED_YLW__;
//   SET_LED_RED__;
		BF_ASSERT(false);
}//void FatalError( DP_ERROR_FILE bFile, UWORD wLine, VPC3_ERRCB_PTR sVpc3Error )

/*---------------------------------------------------------------------------*/
/* function: UserChkDpv1StatusBytes                                          */
/*---------------------------------------------------------------------------*/
DP_ERROR_CODE UserChkDpv1StatusBytes( UBYTE bDpv1Status1, UBYTE bDpv1Status2, UBYTE bDpv1Status3 )
{
DP_ERROR_CODE bRetValue;
   
   bRetValue = DP_OK;
   
   if( pDpSystem->eDPV1 == DPV1_MODE )
   {
      // DPV1 enabled
      if(    (( bDpv1Status1 & 0x18 ) != 0x00 )
          || (( bDpv1Status2 & 0x9F ) != 0x00 )
          || (( bDpv1Status3 & 0xFF ) != 0x00 )
        )
      {
         bRetValue = DP_PRM_DPV1_STATUS;
      }
   }//if( pDpSystem->eDPV1 == DPV1_MODE )
   else
   {
      //DPV0-Mode
      if(    (( bDpv1Status1 & 0x18 ) != 0x00 )
          || (( bDpv1Status2 & 0xFF ) != 0x00 )
          || (( bDpv1Status3 & 0xFF ) != 0x00 )
        )
      {
         bRetValue = DP_PRM_DPV1_STATUS;
      }
   }//else of if( pDpSystem->eDPV1 == DPV1_MODE )
   
   return bRetValue;
}//DP_ERROR_CODE UserChkDpv1StatusBytes( UBYTE bDpv1Status1, UBYTE bDpv1Status2, UBYTE bDpv1Status3 )

/*---------------------------------------------------------------------------*/
/* function: UserChkPrmCounterModulePrm                                      */
/*---------------------------------------------------------------------------*/
DP_ERROR_CODE UserChkPrmCounterModulePrm( MEM_STRUC_MODULE_PRM_BLOCK_PTR psModuleBlock )
{
   return DP_OK;
}//DP_ERROR_CODE UserChkPrmCounterModulePrm( MEM_STRUC_MODULE_PRM_BLOCK_PTR psModuleBlock )

/*---------------------------------------------------------------------------*/
/* function: UserChkNewPrmData                                               */
/*---------------------------------------------------------------------------*/
/*!
   \brief Checking parameter data.
   The user has to program the function for checking the received parameter data.

   \param pbPrmData - address of parameter data
   \param bPrmLength - length of parameter data

   \retval DP_OK - The transferred parameterization is OK.
   \retval DP_NOK - The transferred parameterization isn't OK.
*/
DP_ERROR_CODE UserChkNewPrmData( MEM_UNSIGNED8_PTR pbPrmData, UBYTE bPrmLength )
{
MEM_STRUC_PRM_PTR psToPrmData;
DP_ERROR_CODE     bRetValue;

   pDpSystem->eDPV1 = DPV0_MODE;

   bRetValue = DP_OK;
   //SSN: The Expected bPrmLength should be 0x18 (7 Bytes mandatory + 3 for DPV1 + 14 Device specific)
   if( bPrmLength == 0x13 ) // Galaxy input length //
   {
      psToPrmData = ( MEM_STRUC_PRM_PTR )pbPrmData;

      //DPV1 Statusbyte 1
      pDpSystem->eDPV1 = ( psToPrmData->bDpv1Status1 & DPV1_STATUS_1_DPV1_ENABLE )? DPV1_MODE : DPV0_MODE;

      bRetValue = UserChkDpv1StatusBytes( psToPrmData->bDpv1Status1, psToPrmData->bDpv1Status2, psToPrmData->bDpv1Status3 );

      if( bRetValue == DP_OK )
      {
		// Updated for RedBall //
        //user parameter data, modify this structure
        // receiving the motor parameter data from GSD File
            //bRetValue = GetMotorParamData( (MEM_STRUC_MOTOR_PARAM_DATA_PTR)&psToPrmData->bUserPrmData );
          //user parameter data
         bRetValue = UserChkPrmCounterModulePrm( (MEM_STRUC_MODULE_PRM_BLOCK_PTR)&psToPrmData->bUserPrmData );      
      }//if( bRetValue == DP_OK )
   }//if( bPrmLength == 0x13 )
   else
   {
      bRetValue = DP_PRM_LEN_ERROR;
   }//else of if( bPrmLength == 0x13 )
  
   if( ( VPC3_GET_DP_STATE() == DATA_EX ) && ( bRetValue == DP_OK ) )
   {
      //don't send diagnostic here
   }//if( ( VPC3_GET_DP_STATE() == DATA_EX ) && ( bRetValue == DP_OK ) )
   else
   {
      UserAlarm( USER_TYPE_PRM_OK, 0, (ALARM_STATUS_PDU_PTR) NULL_PTR, FALSE );
   }//else of if( ( VPC3_GET_DP_STATE() == DATA_EX ) && ( bRetValue == DP_OK ) )
   return bRetValue;
}//DP_ERROR_CODE UserChkNewPrmData( MEM_UNSIGNED8_PTR pbPrmData, UBYTE bPrmLength )

/*---------------------------------------------------------------------------*/
/* function: UserChkNewCfgData                                               */
/*---------------------------------------------------------------------------*/
/*!
   \brief Checking configuration data.
   The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has received a Check_Cfg message and has made the data available in the Cfg buffer.

   The user has to program the function for checking the received configuration data.

   \param[in] pbCfgData - address of check configuration data
   \param[in] bCfgLength - length of configuration data

   \retval VPC3_CFG_OK - The transferred configuration is OK.
   \retval VPC3_CFG_FAULT - The transferred configuration isn't OK.
   \retval VPC3_CFG_UPDATE - The transferred configuration is OK, but it's different
                             from read configuration buffer. The user will exchange
                             verified configuration with read configuration buffer.
*/
UBYTE UserChkNewCfgData( MEM_UNSIGNED8_PTR pbCfgData, UBYTE bCfgLength )
{
UBYTE bNrOfCheckModules;
UBYTE bNrOfModules;
UBYTE bRealCfgLength;
UBYTE bRetValue;
UBYTE i;


  bRealCfgLength = sSystem.bNrOfModules * 4; //4 cfg-byte per module
  //build modulstatus diagnose
  memcpy( &sModuleStatusDiagnosis.bHeader, &DefStatusDiag[0], cSizeOfModuleStatusDiagnosis );

  bNrOfCheckModules = sSystem.bNrOfModules;
if( bCfgLength != bRealCfgLength )
   {
      if( bCfgLength < bRealCfgLength )
      {
         bNrOfCheckModules = ( bCfgLength / 4 );
      }//if( bCfgLength < bRealCfgLength )
      else
      {
         bNrOfModules = bCfgLength / 4;
         bNrOfModules += ( bCfgLength % 4 ) ? 1 : 0 ;

         for( i = bNrOfCheckModules; i < bNrOfModules; i++ )
         {
            sModuleStatusDiagnosis.abModuleStatus[ i/4 ] |= (0x03 << ((i%4)*2));
         }//for( i = bNrOfCheckModules; i < bNrOfModules; i++ )
      }//else of if( bCfgLength < bRealCfgLength )

      bRetValue = VPC3_CFG_FAULT;
   }//if( bCfgLength != bRealCfgLength )
   else
   {
      bRetValue = VPC3_CFG_OK;
   }//else of if( bCfgLength != bRealCfgLength )
    for( i = 0; i < bNrOfCheckModules; i++ )
   {
      if( memcmp( &sUser.sCfgData.abData[i*4], pbCfgData, 0x04 ) == 0 )
      {
         sModuleStatusDiagnosis.abModuleStatus[ i/4 ] &= ~(0x02 << ((i%4)*2));
      }//if( memcmp( &sUser.sCfgData.abData[i*4], pToCfgData, 0x04 ) == 0 )
      else
      {
         bRetValue = VPC3_CFG_FAULT;
      }//else of if( memcmp( &sUser.sCfgData.abData[i*4], pToCfgData, 0x04 ) == 0 )

      pbCfgData += 4;
   }//for( i = 0; i < bNrOfCheckModules; i++ )
  
   if( ( bRetValue == VPC3_CFG_OK ) || ( bRetValue == VPC3_CFG_UPDATE ) )
   {
      UserAlarm( USER_TYPE_CFG_OK, 0, (ALARM_STATUS_PDU_PTR) NULL_PTR, FALSE );
      pDpSystem->wEvent |= VPC3_EV_NEW_CFG_DATA;
   }//if( ( bRetValue == VPC3_CFG_OK ) || ( bRetValue == VPC3_CFG_UPDATE ) )
   else
   {
      UserAlarm( USER_TYPE_CFG_NOK, sModuleStatusDiagnosis.abModuleStatus[ 0 ], (ALARM_STATUS_PDU_PTR) NULL_PTR, FALSE );
   }//else of if( ( bRetValue == VPC3_CFG_OK ) || ( bRetValue == VPC3_CFG_UPDATE ) )

   UserAlarmInit();  

   return bRetValue;
}//UBYTE UserChkNewCfgData( MEM_UNSIGNED8_PTR pbCfgData, UBYTE bCfgLength )

/*---------------------------------------------------------------------------*/
/* function: UserIsrGoLeaveDataExchange                                      */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the
   DP-Statemachine has entered the DataEx-mode or has exited it.
   With the function VPC3_GET_DP_STATE() you can find out the state of VPC3+.
*/
void UserIsrGoLeaveDataExchange( void )
{
   if( VPC3_GET_DP_STATE() != DATA_EX )
   {
      pDpSystem->bState = USER_STATE_CLEAR;
      pDpSystem->bApplicationReady = FALSE;
   }
   else
   {
	 fieldbusFault = FALSE;
   }
}//void UserIsrGoLeaveDataExchange( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrDxOut                                                    */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has received a DataExchange message and has made the new output data
   available in the N-buffer. In the case of Power_On or Leave_Master, the
   VPC3+ clears the content of the buffer, and generates this event also.
*/
static unsigned char bDp_State = 0; //SSN added
void UserIsrDxOut( void )
{
  bDp_State = VPC3_GET_DP_STATE();  //SSN added
  if(bDp_State == 0x30)
  {
    bDp_State = bDp_State + 1;
  }
  pDpSystem->wEvent |= VPC3_EV_DX_OUT;
}//void UserIsrDxOut( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrDiagBufferChanged                                        */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+ has
   exchanged the diagnostic buffers, and made the old buffer available again to the user.
*/
void UserIsrDiagBufferChanged( void )
{
    // diagnosis buffer has been changed
    pDpSystem->bUserDiagActive = FALSE;
    // Fetch new diagnosis buffer
    pDpSystem->pDiagBuffer = VPC3_GetDiagBufPtr();
}//void UserIsrDiagBufferChanged( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrNewWdDpTimeout                                           */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the
   watchdog timer expired in the WD mode DP_Control.
   The communication between master and slave is time controlled, every time you're
   disconnecting the PROFIBUS master or you're disconnecting the PROFIBUS cable you'll
   get this event.
*/
void UserIsrNewWdDpTimeout( void )
{
  fieldbusFault = TRUE ;
	UserResetDiagnosticBuffer();
}//void UserIsrNewWdDpTimeout( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrClockSynchronisation                                     */
/*---------------------------------------------------------------------------*/
#if DP_TIMESTAMP
   void UserIsrClockSynchronisation( void )
   {
      //not used in our application
   }//void UserIsrClockSynchronisation( void )
#endif//#if DP_TIMESTAMP

/*---------------------------------------------------------------------------*/
/* function: UserIsrBaudrateDetect                                           */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has exited the Baud_Search mode and has found a baudrate.
   With the macro VPC3_GET_BAUDRATE() you can detect the baudrate.
*/
void UserIsrBaudrateDetect( void )
{
    //not used in our application
}//void UserIsrBaudrateDetect( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrNewGlobalControlCommand                                  */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has received a Global_Control message. The GC_Command_Byte can be read out
   with the macro VPC3_GET_GC_COMMAND().
*/
void UserIsrNewGlobalControlCommand( void )
{

}

/*---------------------------------------------------------------------------*/
/* function: UserIsrNewSetSlaveAddress                                       */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has received a Set_Slave_Address message and made the data available in the SSA
   buffer.
*/
void UserIsrNewSetSlaveAddress( void )
{
  //SSN  To Do : can be used in our application
   //copy ssa-data from VPC3+S to local structure
   CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&pDpSystem->abPrmCfgSsaHelpBuffer[0], VPC3_GET_SSA_BUF_PTR(), 4 );
   //psSsa = (MEM_STRUC_SSA_BLOCK_PTR)&pDpSystem->abPrmCfgSsaHelpBuffer[0];
/*
   //store the new address and the bit bNoAddressChanged for the next startup
   print_string("\r\nNewAddr: ");
   print_hexbyte(psSsa->bTsAddr);
   print_hexbyte(psSsa->bNoAddressChanged);
   print_hexbyte(psSsa->bIdentHigh);
   print_hexbyte(psSsa->bIdentLow);
*/
}//void UserIsrNewSetSlaveAddress( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrTimerClock (10ms)                                        */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() calls this function if the time base
   of the User_Timer_Clock has expired (1/10ms).

   \attention Use this function only interrupt driven!
*/
void UserIsrTimerClock( void )
{

}//void UserIsrTimerClock( void )

//This is dummy function to compile with IAR,actual function is in Bull's eye
#ifdef IAR_BUILD
void cov_dumpData()
{
}
#endif
#if DP_FDL
/*--------------------------------------------------------------------------*/
/* function: UserDpv1ReadReq (called by DPV1)                               */
/*--------------------------------------------------------------------------*/
/*!
  \brief The firmware calls this function when a DPV1_Read request is pending.
   The array pToDpv1Data[] is undefined when the function is called.
   The application program has to fill in the array pToDpv1Data[], and enter the
   corresponding length in the field ëpsMsgHeader->bDataLengthí.
   The firmware handles the function number. If there is an error, the user normally provides a negative
   response PDU. This retains the connection.

  \param[in] bSapNr - PROFIBUS SAP ( service access point, 51 - Class1 master, 48,47... Class2 master )
  \param[in] psMsgHeader - pointer to DPV1 header ( slot, index, data length )
  \param[in] pToDpv1Data - pointer to DPV1 data

  \retval DPV1_OK  - The function was completed successfully.
  \retval DPV1_NOK - An error occurred. The user entered more detailed
                     information about the error in the error block for this channel.
  \retval DPV1_DELAY - The function is still being processed. The application
                       program signals the end of the function with calling function
                       MSAC_C1_TransmitDelay or MSAC_C2_TransmitDelay.
*/
DPV1_RET_VAL UserDpv1ReadReq( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
	DPV1_RET_VAL bRetValue = DPV1_OK;

   if( psMsgHeader->bIndex == 255 )
   {
      bRetValue = ImReadReq( bSapNr, psMsgHeader, pToDpv1Data );
   }
   else
   {
	   if( psMsgHeader->bSlotNr == 0 )
	   {
		  if( psMsgHeader->bIndex == ACYCLIC_READ_INDEX )
		  {
			 if( psMsgHeader->bDataLength > C445_ACYCLIC_READ_BYTES_LENGTH )
			 {
				 psMsgHeader->bDataLength = C445_ACYCLIC_READ_BYTES_LENGTH;
			 }
			 if(profi_cfg_ctrl->Is_Acyclic_Read_Module_Configured())
			 {
				 // Read everything that is acyclic read
				 profi_data_ex_ctrl->Acyclic_Read(&sUser.abDpv1RdBuffer[0]);
				 // Return only which is requested by the user
				 CopyToVpc3_( pToDpv1Data, &sUser.abDpv1RdBuffer[0], psMsgHeader->bDataLength );
			 }
			 else
			 {
				 //Set user access error
				 bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_USER, 0x00 );
			 }
		  }
		  else if(psMsgHeader->bIndex == ACYCLIC_WRITE_INDEX )
		  {
			  if( psMsgHeader->bDataLength > C445_ACYCLIC_WRITE_BYTES_LENGTH)
			  {
				 psMsgHeader->bDataLength = C445_ACYCLIC_WRITE_BYTES_LENGTH;
			  }
			  if(profi_cfg_ctrl->Is_Acyclic_Write_Module_Configured())
			  {
				  CopyToVpc3_( pToDpv1Data, &sUser.abDpv1RwBuffer[0], psMsgHeader->bDataLength );
			  }
			  else
			  {
				  //Set user access error
				  bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_USER, 0x00 );
			  }
		  }
		  else
		  {
			 bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0x00 );
		  }
	   }
	   else
	   {
		   bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_SLOT, 0x00 );
	   }
   }
   return bRetValue;
}

/*--------------------------------------------------------------------------*/
/* function: UserDpv1WriteReq (called by DPV1)                           */
/*--------------------------------------------------------------------------*/
/*!
  \brief The firmware calls this function when a DPV1_Write request is pending.

   Example for Write Processing:
   ∑ Write.req(length = 20) for a data set with the length 20 octets => the length of data indicated in the
     request is written, and the length is mirrored in the reply.
   ∑ Write.req(length > 20) for a data set with the length 20 octets => there is to be no writing; an error
     message has to be transmitted.

  \param[in] bSapNr - PROFIBUS SAP ( service access point, 51 - Class1 master, 48,47... Class2 master )
  \param[in] psMsgHeader - pointer to DPV1 header ( slot, index, user data length )
  \param[in] pToDpv1Data - pointer to DPV1 user data

  \retval DPV1_OK  - The function was completed successfully.
  \retval DPV1_NOK - An error occurred. The user entered more detailed
                     information about the error in the error block for this channel.
  \retval DPV1_DELAY - The function is still being processed. The application
                       program signals the end of the function with calling function
                       MSAC_C1_TransmitDelay or MSAC_C2_TransmitDelay.
*/
DPV1_RET_VAL UserDpv1WriteReq( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
    DPV1_RET_VAL bRetValue = DPV1_OK;

   if( psMsgHeader->bIndex == 255 )
   {
      bRetValue = ImWriteReq( bSapNr, psMsgHeader, pToDpv1Data );
   }
   else if( psMsgHeader->bSlotNr == 0 )
   {
      if( psMsgHeader->bIndex == ACYCLIC_WRITE_INDEX)
      {
         if( psMsgHeader->bDataLength > C445_ACYCLIC_WRITE_BYTES_LENGTH )
         {
            psMsgHeader->bDataLength = C445_ACYCLIC_WRITE_BYTES_LENGTH;
         }
         if(profi_cfg_ctrl->Is_Acyclic_Write_Module_Configured())
         {
			 //Flush the buffer before use
			 memset(&sUser.abDpv1RwBuffer[0],0,sizeof(sUser.abDpv1RwBuffer[0]));
			 CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.abDpv1RwBuffer[0], pToDpv1Data, psMsgHeader->bDataLength );
			 profi_data_ex_ctrl->Acyclic_Write(&sUser.abDpv1RwBuffer[0]);
         }
         else
         {
        	 bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_USER, 0x00 );
         }
      }
#ifdef ENABLE_COVERAGE
      else if( psMsgHeader->bIndex == COVERAGE_DUMP_INDEX)
      {
    	  cov_dumpData();
      }
#endif
      else if(psMsgHeader->bIndex == MODBUS_EMULATION_INDEX)
      {
    	  	if( (psMsgHeader->bDataLength == MODBUS_EMULATION_MIN_DATA_LENGTH) || (psMsgHeader->bDataLength == MODBUS_EMULATION_MAX_DATA_LENGTH) )
			{
    	  		memset(sUser.abModbusEmulationBuffer,0,sizeof(sUser.abModbusEmulationBuffer));
    	  		CopyFromVpc3_((MEM_UNSIGNED8_PTR)sUser.abModbusEmulationBuffer, pToDpv1Data, psMsgHeader->bDataLength);
    	  		if (MB_ILLEGAL_FUNCTION_ERROR_CODE == DecodeAndExecuteMBCommand(sUser.abModbusEmulationBuffer))
    	  		{
    	  			bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_USER, 0x00 );
    	  		}
			}
			else
			{
				bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_USER, 0x00 );
			}
      }
      else
      {
          bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0x00 );
      }
   }

   else
   {
      bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_SLOT, 0x00 );
   }
   return bRetValue;
}

#endif // #if DP_FDL

#if DP_ALARM
/*--------------------------------------------------------------------------*/
/* function: UserAlarmAckReq (called by DPV1)                               */
/*--------------------------------------------------------------------------*/
/*!
  \brief The firmware calls this functon if a Class1 master acknowledge an alarm.

  \param[in] psAlarm - pointer to alarm structure
*/
void UserAlarmAckReq( ALARM_STATUS_PDU_PTR psAlarm )
{
    // alarm_ack from master received
    // todo: do your alarm-acknowledge handling here
    UserFreeAlarmBuffer( psAlarm );
}//void UserAlarmAckReq( ALARM_STATUS_PDU_PTR psAlarm )
#endif // #if DP_ALARM


#if DP_MSAC_C2
/*--------------------------------------------------------------------------*/
/* function: UserMsac2InitiateReq (called by DPV1)                          */
/*--------------------------------------------------------------------------*/
/*!
  \brief The firmware calls this functon if a master wants to establish a C2 connection.

   When this function is called, the parameter PDU points to the structure MSAC_C2_INITIATE_REQ_PDU. When
   leaving the function, the user program has to have preprocessed the buffer according to the structure
   MSAC_C2_INITIATE_RES_PDU.
   The user is supported with the function MSAC_C2_InitiateReqToRes; it generates the response structure
   from the request structure. This applies only if the slave is the endpoint of the connection. If the macro
   MSAC_C2_InitiateReqToRes returns the value DPV1_NOK, the PDU that was received remains
   unchanged. The user has to either make the evaluation himself, or reject the request for establishing a
   connection.

  \param[in] bSapNr - PROFIBUS SAP ( 48,47... Class2 master )
  \param[in] psInitiateReq -
  \param[in] psMsgHeader -
  \param[in] pToDpv1Data -

  \retval DPV1_OK  - The function was completed successfully.
  \retval DPV1_NOK - An error occurred. The user entered more detailed
                     information about the error in the error block for this channel.
  \retval DPV1_DELAY - The function is still being processed. The application
                       program signals the end of the function with calling function
                       MSAC_C2_TransmitDelay.
*/
DPV1_RET_VAL UserMsac2InitiateReq( UBYTE bSapNr, INITIATE_REQ_PDU_PTR psInitiateReq, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
D_ADDR_PTR     psToDestAddr;
DPV1_RET_VAL   bRetValue;

   #if DPV1_IM_SUPP
      pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = 0x0000;
      pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] = 0x00;
   #else
      bSapNr = bSapNr;  //avoid warning
   #endif//#if DPV1_IM_SUPP

   psToDestAddr = (D_ADDR_PTR)(&psInitiateReq->abAddrData[0] + psInitiateReq->bS_Len);

   if( psToDestAddr->bApi == 0 )
   {
      // in this example only API 0 is supported
      if( MSAC_C2_InitiateReqToRes( psInitiateReq, psMsgHeader, pToDpv1Data ) == DPV1_OK )
      {
         // Initiate-Request-PDU has been changed to Initiate-Response-PDU
         bRetValue = DPV1_OK;
      }//if( MSAC_C2_InitiateReqToRes( psInitiateReq, psMsgHeader, pToDpv1Data ) == DPV1_OK )
      else
      {
         // D-Typ is no endpoint of connection
         // user must handle Initiate-PDU (no change by MSAC_C2)
         // in this example slave must always be endpoint
         // create negative response
         psMsgHeader->bIndex      = DPV1_ERRCL_APPLICATION | DPV1_ERRCL_APP_NOTSUPP;
         psMsgHeader->bDataLength = 0x00;
         bRetValue = DPV1_NOK;
      }//else of if( MSAC_C2_InitiateReqToRes( psInitiateReq, psMsgHeader, pToDpv1Data ) == DPV1_OK )
   }//if( psToDestAddr->bApi == 0 )
   else
   {
      // create ABORT-PDU
      psMsgHeader->bSlotNr  = MSAC_C2_SUBNET_NO;
      psMsgHeader->bIndex   = MSAC_C2_INSTANCE_MSAC_C2 | MSAC_C2_ABT_IA;
      psMsgHeader->bDataLength = 0x00;

      bRetValue = DPV1_ABORT;
   }//else of if( psToDestAddr->bApi == 0 )

   return bRetValue;
}//DPV1_RET_VAL UserMsac2InitiateReq( UBYTE bSapNr, INITIATE_REQ_PDU_PTR psInitiateReq, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )

/*--------------------------------------------------------------------------*/
/* function: UserMsac2DataTransportReq (called by DPV1)                     */
/*--------------------------------------------------------------------------*/
/*!
  \brief The firmware calls this function if a DPV1 Datatransport request was received.
   When the function is called, the array pToDpv1Data[] contains the received data.
   The application program has to fill the array pToDpv1Data[] with the data that is to be sent,
   and set the field ëpsMsgHeader->bDataLengthí correspondingly.
   If there is an error, the user normally sets up a negative response PDU. This retains the connection.

  \param[in] bSapNr - PROFIBUS SAP ( service access point, 48,47... Class2 master )
  \param[in] psMsgHeader - pointer to DPV1 header ( slot, index, user data length )
  \param[in] pToDpv1Data - pointer to DPV1 user data

  \retval DPV1_OK  - The function was completed successfully.
  \retval DPV1_NOK - An error occurred. The user entered more detailed
                     information about the error in the error block for this channel.
  \retval DPV1_DELAY - The function is still being processed. The application
                       program signals the end of the function with calling function
                       MSAC_C2_TransmitDelay.
*/
DPV1_RET_VAL UserMsac2DataTransportReq( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
DPV1_RET_VAL bRetValue;

   if( bSapNr == DP_C1_RD_WR_SAP_NR )
   {
      bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_PARAM, 0x00 );
   }//if( bSapNr == DP_C1_RD_WR_SAP_NR )
   else
   {
      if( psMsgHeader->bDataLength > (C2_LEN-4) )
      {
         psMsgHeader->bDataLength = (C2_LEN-4);
      }//if( psMsgHeader->bDataLength > (C2_LEN-4) )

      if( psMsgHeader->bSlotNr == 0x00 )
      {
         if( psMsgHeader->bIndex == 0x02 )
         {
            CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.abDpv1RwBuffer[0], pToDpv1Data, psMsgHeader->bDataLength );
            bRetValue = DPV1_OK;
         }//if( psMsgHeader->bIndex == 0x02 )
         else
         {
            bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0x00 );
         }//else of if( psMsgHeader->bIndex == 0x02 )
      }//if( psMsgHeader->bSlotNr == 0x00 )
      else
      {
         bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_SLOT, 0x00 );
      }//else of if( psMsgHeader->bSlotNr == 0x00 )
   }//else of if( bSapNr == DP_C1_RD_WR_SAP_NR )

   return bRetValue;
}//DPV1_RET_VAL UserMsac2DataTransportReq( UBYTE bSapNr, DPV1_PTR pdu )

/*--------------------------------------------------------------------------*/
/* function: UserMsac2AbortInd (called by DPV1)                             */
/*--------------------------------------------------------------------------*/
/*!
  \brief The firmware calls this function if a C2 connection was aborted by the master, or the firmware detects a
   reason for cancelling it (for example, timeout).
   A C1 connection is coupled to the processing mode (cyclical state machine) of the slave. In the case of
   LEAVE_DATA_EXCHANGE, the C1 connection is cancelled automatically.

  \param[in] bSapNr - PROFIBUS SAP ( 48,47... Class2 master )

  \retval DPV1_OK  - The function was completed successfully.
*/
DPV1_RET_VAL UserMsac2AbortInd( UBYTE bSapNr )
{
   #if DPV1_IM_SUPP
      pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = 0x0000;
      pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] = 0x00;
   #else
      bSapNr = bSapNr;  //avoid warning
   #endif//#if DPV1_IM_SUPP

   return DPV1_OK;
}//DPV1_RET_VAL UserMsac2AbortInd( UBYTE bSapNr )

   /*------------------------------------------------------------------------*/
   /* interrupt: Timer 0 (10ms)                                              */
   /*------------------------------------------------------------------------*/
   #ifdef EvaBoard_AT89C5132
      void it_timer0(void) interrupt IRQ_T0
      {
         TL0 = 0xCB;//0xE6;    // init value, reload value
         TH0 = 0x7D;//0xBE;

         TF0 = 0;
         UserIsrTimerClock();
         MSAC_C2_TimerTick10msec();
      }//void it_timer0(void) interrupt IRQ_T0
   #endif//#ifdef EvaBoard_AT89C5132

#endif//#if DP_MSAC_C2
// Added for Galaxy //
DP_ERROR_CODE GetMotorParamData (MEM_STRUC_MOTOR_PARAM_DATA_PTR ptrStructMotordata)
{
  DP_ERROR_CODE     bRetValue;
   // Check the parameter data...
  // for no error
  bRetValue = DP_PRM_ENTRY_ERROR;
  bRetValue = DP_OK;
  return (bRetValue);
  // for error
//  return (bRetValue = DP_PRM_ENTRY_ERROR;)
}

uint8_t DecodeAndExecuteMBCommand(uint8_t *Dpv1Buffer)
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint8_t command;
	if(*Dpv1Buffer == MB_SYS_CMD_USER_FUNC_CODE)
	{
		Dpv1Buffer++;
		command = *Dpv1Buffer;
		switch ( command )
		{
			case MB_SYS_CMD_FACTORY_RESET:
				BF_Lib::Services::Execute_Service( SERVICES_FACTORY_RESET,
					reinterpret_cast< BF_Lib::Services::SERVICES_HANDLE_DATA_ST* >( nullptr ) );
				break;

			case MB_SYS_CMD_SOFT_RESET:
				BF_Lib::Services::Execute_Service( SERVICES_SOFT_RESET,
					reinterpret_cast< BF_Lib::Services::SERVICES_HANDLE_DATA_ST* >( nullptr ) );
				break;

			case MB_SYS_CMD_APP_PARAM_RESET:
				BF_Lib::Services::Execute_Service( SERVICES_APP_PARAM_RESET,
					reinterpret_cast< BF_Lib::Services::SERVICES_HANDLE_DATA_ST* >( nullptr ) );
				break;

			case MB_SYS_CMD_WATCHDOG_TEST:
				BF_Lib::Services::Execute_Service( SERVICES_WATCHDOG_TEST_RESET,
					reinterpret_cast< BF_Lib::Services::SERVICES_HANDLE_DATA_ST* >( nullptr ) );
				break;
			
			case MB_SYS_CMD_MFG_UNLOCK:
				BF_Lib::Services::SERVICES_HANDLE_DATA_ST temp_service_data;
				Dpv1Buffer++;
				temp_service_data.rx_data = Dpv1Buffer;
				temp_service_data.rx_len = SERVICES_MFG_ACCESS_UNLOCK_DATA_LEN;
				//Populate dummy data,it does not matter for us since I am not sending any data back
				temp_service_data.tx_data = nullptr;
				temp_service_data.tx_len = SERVICES_MFG_ACCESS_UNLOCK_DATA_LEN;
				BF_Lib::Services::Execute_Service( static_cast<uint8_t>(MB_SYS_CMD_MFG_UNLOCK), &temp_service_data );
				break;
         #ifdef TEST_SIMU

			case TEST_BRD_REV_FAULT:
				err_sim = 0x98;
                                break;
                                
			case TEST_HW_ID_FAULT:
				err_sim = 0x99;
                                break;
                                
			case TEST_OVR_TEMP:
				err_sim = 0x91;
                                break;
                                
			case TEST_HEAP_FAULT:
				err_sim = 0x92;
                                break;                                
  
			case TEST_STACK_FAULT:
				err_sim = 0x93;
                                break;      
                                                        
         #endif                            
                                
				

			default:
				if ( command > static_cast<uint8_t>(SERVICES_APP_SPECIFIC_START) )
				{
					if ( BF_Lib::Services::Execute_Service( command,
						reinterpret_cast< BF_Lib::Services::SERVICES_HANDLE_DATA_ST* >( nullptr ) )
						== false )
					{
						return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;
					}
				}
				else
				{
					return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;
				}
				break;
		}
	}
	else
	{
		return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;
	}
	return return_status;
}

void UpdateProfiBusIMBuffer( void )
{
    uint32_t serial_number;
    int8_t i = 0;
    Profibus_System::GetProductSerialNumber( &serial_number );    
    while(serial_number)
    {
    	IM_DEFAULT[47+i] = (serial_number%10)+48;
    	serial_number/=10;
        i--;
    }
}

/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

