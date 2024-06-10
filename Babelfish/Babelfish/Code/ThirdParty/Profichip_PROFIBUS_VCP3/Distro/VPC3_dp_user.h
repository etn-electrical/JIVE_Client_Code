/************************** Filename: dp_user.h ******************************/
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
/* Function:                                                                 */
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
     \brief Header file for user application.

*/

/*****************************************************************************/
/* contents:
*/
/*****************************************************************************/
/* reinclude protection */


#ifndef DP_USER_H
#define DP_USER_H
#include "C445_Cfg_List.h"
#include "OS_Semaphore.h"

/*---------------------------------------------------------------------------*/
/* defines, structures                                                       */
/*---------------------------------------------------------------------------*/
#define PROD_REFRESH_RATE					10
#define PROFIBUS_IDENITY_REFRESH_RATE		1000  // in milliseconds
#define ACYCLIC_READ_INDEX					42
#define ACYCLIC_WRITE_INDEX					43
#ifdef ENABLE_COVERAGE
#define COVERAGE_DUMP_INDEX					44
#endif
#define MODBUS_EMULATION_INDEX				45

#define MODBUS_EMULATION_MIN_DATA_LENGTH	2
#define MODBUS_EMULATION_MAX_DATA_LENGTH	3

// -- defines for user alarm state
#define USER_AL_STATE_CLOSED        ((UBYTE)0x00)
#define USER_AL_STATE_OPEN          ((UBYTE)0x01)
// -- defines for diagnostics
#define USER_TYPE_RESET_DIAG      static_cast<UBYTE>(0xF8)
#define USER_TYPE_DPV0                static_cast<UBYTE>(0xFA)
#define USER_TYPE_PRM_OK            static_cast<UBYTE>(0xFB)
#define USER_TYPE_PRM_NOK          static_cast<UBYTE>(0xFC)
#define USER_TYPE_CFG_OK            static_cast<UBYTE>(0xFD)
#define USER_TYPE_CFG_NOK          static_cast<UBYTE>(0xFE)
#define USER_TYPE_APPL_RDY         static_cast<UBYTE>(0xFF)

// -- status message data (coded as device diagnosis) -------------------------
// -- Modulstatus -------------------------------------------------------------
typedef struct
{
   UBYTE   bHeader;                    // fix to 0x06
   UBYTE   bModuleStatus;              // fix to 0x82
   UBYTE   bSlotNr;                    // fix to 0x00
   UBYTE   bSpecifier;                 // fix to 0x00
   UBYTE   abModuleStatus[2];
} tModuleStatusDiagnosis;
#define cSizeOfModuleStatusDiagnosis 6

#define cSizeOfUsrDiagnosticAlarmData    ((UBYTE)0x05)
#define cSizeOfUsrProcessAlarmData       ((UBYTE)0x03)
#define cSizeOfDiagnosticAlarm           (cSizeOfAlarmHeader + cSizeOfUsrDiagnosticAlarmData)
#define cSizeOfProcessAlarm              (cSizeOfAlarmHeader + cSizeOfUsrProcessAlarmData)

#define MaxAlarmLength  ((UBYTE)cSizeOfUsrDiagnosticAlarmData) //Max (cSizeOfUsrDiagnosticAlarmData, cSizeOfUsrProcessAlarmData)

typedef struct
{
   CFG_STRUCT                 sCfgData;

   struct                     //alarm state machine data
   {
      UBYTE                   abBufferUsed[ ALARM_MAX_FIFO ];
      UBYTE                   abUserAlarmData[ ALARM_MAX_FIFO ][ MaxAlarmLength ];
      ALARM_STATUS_PDU        asAlarmBuffers[ ALARM_MAX_FIFO ];
   }sAlarm;

   UBYTE                      bDiagnosticAlarmSeqNr;
   UBYTE                      bProcessAlarmSeqNr;

   UBYTE                      bDiagnosticAlarm;
   UBYTE                      bProcessAlarm;

   DPL_STRUC_LIST_CB          dplDiagQueue;

   #if DP_FDL
      UBYTE                   abDpv1RwBuffer[ ACYCLIC_WRITE_NUMBER_OF_BYTES  ];// Write buffer
      UBYTE                   abDpv1RdBuffer[ ACYCLIC_READ_NUMBER_OF_BYTES  ]; // Read only buffer
      UBYTE					  abModbusEmulationBuffer[MODBUS_EMULATION_MAX_DATA_LENGTH];
      #if DPV1_IM_SUPP
         UWORD                awImIndex[0x10];

         sIM0                 sIM_0;

         #if IM1_SUPP
            sIM1              sIM_1;
         #endif//#if IM1_SUPP

         #if IM2_SUPP
            sIM1              sIM_2;
         #endif//#if IM2_SUPP

         #if IM3_SUPP
            sIM1              sIM_3;
         #endif//#if IM3_SUPP

         #if IM4_SUPP
            sIM1              sIM_4;
         #endif//#if IM4_SUPP
     #endif//#if DPV1_IM_SUPP
   #endif//#if DP_FDL
}USER_STRUC;

extern USER_STRUC                sUser;               // user structure

typedef struct	// Updated for Galaxy //
{
  UWORD Nom_M_Volt;
  UWORD Nom_M_Curr;
  UWORD Curr_Limit;
  UWORD Nom_M_Speed;
  UWORD Nom_M_Power;
  signed char Nom_M_PF;
  UWORD Nom_M_Freq;
  UBYTE Nom_M_Dir;
} STRUC_PRM_MOTOR;  //  14 bytes of parameter //
#define MEM_STRUC_MOTOR_PARAM_DATA_PTR  STRUC_PRM_MOTOR MEM_PTR

typedef struct  // Module definations need to change for Galaxy //
{
   UBYTE bLength;
   UBYTE bSlotNr;
   UBYTE bData;
}STRUC_MODULE_PRM_BLOCK;
#define MEM_STRUC_MODULE_PRM_BLOCK_PTR  STRUC_MODULE_PRM_BLOCK MEM_PTR

typedef struct	// Updated for Galaxy //
{
  UWORD Control;
  UWORD Speed;
  UWORD Volt;
  UWORD Current;
  UWORD Resistance;
  UWORD Temperature;
} STRUC_OUTPUT; // 12 bytes of output //

typedef struct	// Updated for Galaxy //
{
  UWORD Motor_Volt;
  UWORD Motor_Curr;
  UWORD Motor_Speed;
  UWORD Motor_Freq;
  UWORD Motor_PF;
  UWORD Motor_Status;
} STRUC_INPUT;  // 12 bytes of input //

typedef struct	// Updated for Galaxy //
{
   UBYTE               bNrOfModules;
   STRUC_INPUT         sInput;
   UBYTE abProdData[C445_MAX_PROD_DATA_SIZE];
   STRUC_OUTPUT        sOutput;
   UBYTE abConsData[C445_MAX_CONS_DATA_SIZE];
   STRUC_PRM_MOTOR  sPrmCounter;  // Galaxy Defination //
} STRUC_SYSTEM;

//***************************************************************************************
//*******   	External Variables.
//***************************************************************************************
extern OS_Semaphore*				profi_activity;

/*****************************************************************************/
/* reinclude-protection */
BOOL getProfiBusCommunicationStatus( void );
void updateProfibusCommunicationStatus( void );
BOOL getFieldbusFault( void );
uint8_t Alarm_Handler( uint8_t bModuleNr,uint8_t *data_source, uint8_t Alarm_Data_Length );
void UpdateProfiBusIMBuffer( void );
#else
    #pragma message "The header DP_USER.H is included twice or more !"
#endif


/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2006. Confidential.                         */
/*****************************************************************************/
