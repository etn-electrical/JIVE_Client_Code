/**********************  Filename: dp_debug.c  *******************************/
/* ========================================================================= */
/*                                                                           */
/* 0000  000   000  00000 0  000  0   0 0 0000                               */
/* 0   0 0  0 0   0 0     0 0   0 0   0 0 0   0      Einsteinstra�e 6        */
/* 0   0 0  0 0   0 0     0 0     0   0 0 0   0      91074 Herzogenaurach    */
/* 0000  000  0   0 000   0 0     00000 0 0000       Germany                 */
/* 0     00   0   0 0     0 0     0   0 0 0                                  */
/* 0     0 0  0   0 0     0 0   0 0   0 0 0          Tel: ++49-9132-744-200  */
/* 0     0  0  000  0     0  000  0   0 0 0    GmbH  Fax: ++49-9132-744-204  */
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/* Description: Debug fuctions for VPC3+C/S.                                 */
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

  - function prototypes
  - data structures
  - internal functions

*/
/*****************************************************************************/
/* include hierarchy */
#include "Includes.h"
#include "VPC3_platform.h"
#include "VPC3.h"
#include "VPC3_dp_inc.h"

#ifdef RS232_SERIO

extern UBYTE UserDemoDiagnostic( UBYTE bDiagNr, UBYTE bModuleNr );

/*---------------------------------------------------------------------------*/
/* function prototypes                                                       */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* function: Wait                                                            */
/*---------------------------------------------------------------------------*/
void Wait( void )
{
   do
   {
      // wait!
      VPC3_RESET_USER_WD();   // toggle user watchdog
   }
   while( bSndCounter > 50);
}//void Wait( void )

/*---------------------------------------------------------------------------*/
/* function: PrintDebugBuffer                                                */
/*---------------------------------------------------------------------------*/
#ifdef DP_DEBUG_ENABLE
void PrintDebugBuffer( void )
{
UWORD wIndex;
UWORD i;

   print_string("\r\nDebug_Buffer: ");
   print_hexbyte( bDebugBufferIndex );
   print_hexbyte( bDebugBufferOverlapped );

   Wait();

   wIndex = 0;
   if( bDebugBufferOverlapped == TRUE )
   {
      for( i = bDebugBufferIndex; i < (MAX_NR_OF_DEBUG-1); i++ )
      {
         if( (wIndex%4) == 0 )
         {
            Wait();

            print_string("\r\n");
            print_hexbyte(wIndex);
            print_string("  ");
         }//if( (wIndex%4) == 0 )
         print_hexbyte( asDebugBuffer[i].bDebugCode );
         print_hexbyte( asDebugBuffer[i].bDetail1   );
         print_hexbyte( asDebugBuffer[i].bDetail2   );
         print_string(" - ");
         wIndex++;
      }//for( i = 0; i < bDebugBufferIndex; i++ )
   }//if( bDebugBufferOverlapped == TRUE )

   for( i = 0; i < bDebugBufferIndex; i++ )
   {
      if( (wIndex%4) == 0 )
      {
         Wait();

         print_string("\r\n");
         print_hexbyte(wIndex);
         print_string("  ");
      }//if( (wIndex%4) == 0 )
      print_hexbyte( asDebugBuffer[i].bDebugCode );
      print_hexbyte( asDebugBuffer[i].bDetail1   );
      print_hexbyte( asDebugBuffer[i].bDetail2   );
      print_string(" - ");
      wIndex++;
   }//for( i = 0; i < bDebugBufferIndex; i++ )
}//void PrintDebugBuffer( void )
#endif//#ifdef DP_DEBUG_ENABLE

/*---------------------------------------------------------------------------*/
/* function: PrintHelp                                                      */
/*---------------------------------------------------------------------------*/
void PrintHelp( void )
{
   print_string("\r\nHelp: ");
   print_string("\r\na,A : Statusregister");
   print_string("\r\nb,B : Moderegister");
   print_string("\r\nc,C : Organizational Parameter");
   Wait();
   #if DP_FDL
       print_string("\r\nd,D : FDL-SAP-List");
   #endif//#if DP_FDL
   print_string("\r\ne,E : Diagnostic Data");
   print_string("\r\nf,F : Parameter Data");
   print_string("\r\ng,G : Check Config Data");
   print_string("\r\nj,J : VPC3 Free Memory");
   Wait();
   print_string("\r\n");
   print_string("\r\n  1 : set   diagnostic alarm (module 5)");
   print_string("\r\n  2 : reset diagnostic alarm (module 5)");
   print_string("\r\n  3 : set   diagnostic alarm (module 6)");
   Wait();
   print_string("\r\n  4 : reset diagnostic alarm (module 6)");
   print_string("\r\n  5 : set   process    alarm (module 6)");
   print_string("\r\n  6 : reset process    alarm (module 6)");
   print_string("\r\n");
}//void PrintHelp( void )

/*---------------------------------------------------------------------------*/
/* function: ShowBuffer                                                     */
/*---------------------------------------------------------------------------*/
void ShowBuffer( char * pcMsg, UBYTE * pbPtr, UWORD wLength )
{
UWORD i;

   print_string("\r\n");
   print_string( pcMsg );

   print_string("\r\nAdr: ");
   print_hexword( pbPtr );

   print_string("  length: ");
   print_hexword( wLength );

   print_string("\r\nAdr:   00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F");
   for( i = 0; i < wLength; i++ )
   {
      if( i%16 == 0 )
      {
         Wait();

         print_string("\r\n");
         print_hexword(i);
         print_string("  ");
      }//if( i%16 == 0 )

      print_string(" ");

      #if VPC3_SERIAL_MODE
         print_hexbyte( Vpc3Read( (VPC3_ADR)(pbPtr+i) ) );
      #else
         print_hexbyte( *(pbPtr+i) );
      #endif//#if VPC3_SERIAL_MODE
   }//for( i = 0; i < length; i++ )

   print_string("\r\n");
}//void ShowBuffer( char * pcMsg, UBYTE * pbPtr, UWORD wLength )

/*---------------------------------------------------------------------------*/
/* function: PrintOrganizationalParameter                                    */
/*---------------------------------------------------------------------------*/
void PrintOrganizationalParameter( void )
{
   #if VPC3_SERIAL_MODE
      ShowBuffer("Organizational Parameters:", (UBYTE *)bVpc3RwTsAddr, 42 );
   #else
      ShowBuffer("Organizational Parameters:", &pVpc3->bTsAddr, 42 );
   #endif//#if VPC3_SERIAL_MODE
}//void PrintOrganizationalParameter( void )

/*---------------------------------------------------------------------------*/
/* function: PrintDiagBuffer                                                 */
/*---------------------------------------------------------------------------*/
void PrintDiagBuffer( void )
{
   print_string("\r\nDiag_SM:");
   ShowBuffer("Diag Buffer1:", VPC3_GET_DIAG1_PTR(), VPC3_GET_DIAG1_LENGTH() );
   ShowBuffer("Diag Buffer2:", VPC3_GET_DIAG2_PTR(), VPC3_GET_DIAG2_LENGTH() );
}//void PrintDiagBuffer( void )

/*---------------------------------------------------------------------------*/
/* function: PrintPrmData                                                    */
/*---------------------------------------------------------------------------*/
void PrintPrmData( void )
{
   ShowBuffer("NEW_PRM_DATA:", VPC3_GET_PRM_BUF_PTR(), VPC3_GET_PRM_LEN() );
}//void PrintPrmData( void )

/*---------------------------------------------------------------------------*/
/* function: print cfg_data                                                  */
/*---------------------------------------------------------------------------*/
void PrintCfgData( void )
{
   ShowBuffer("Read  CFG Buffer:", VPC3_GET_READ_CFG_BUF_PTR(), VPC3_GET_READ_CFG_LEN() );
   ShowBuffer("Check CFG Buffer:", VPC3_GET_CFG_BUF_PTR(), VPC3_GET_CFG_LEN() );
}//void PrintCfgData( void )

/*---------------------------------------------------------------------------*/
/* function: PrintSapList                                                    */
/*---------------------------------------------------------------------------*/
#if DP_FDL
void PrintSapList( void )
{
UBYTE i;

   print_string("SAPL");
   print_hexbyte(SAP_LENGTH);
   print_hexbyte(SAP_LENGTH/7);

   for( i = 0; i < SAP_LENGTH/7; i++ )
   {
      #if VPC3_SERIAL_MODE
         ShowBuffer("SAP-List:", (UBYTE *)bVpc3RwStartSapCtrlList + i*7, 7 );
         ShowBuffer("Ind-Ptr:",  (UBYTE *)(VPC3_UNSIGNED8_PTR)((VPC3_ADR)(Vpc3Read(bVpc3RwStartSapCtrlList+(i*7)+4) << SEG_MULDIV)+(VPC3_ADR)VPC3_ASIC_ADDRESS), 16 );
         //resource manager
         if( Vpc3Read(bVpc3RwStartSapCtrlList+(i*7)) == 49 )
         {
            ShowBuffer("Rsp-Ptr:",   (UBYTE *)(VPC3_UNSIGNED8_PTR)((VPC3_ADR)(Vpc3Read(bVpc3RwStartSapCtrlList+(i*7)+6) << SEG_MULDIV)+(VPC3_ADR)VPC3_ASIC_ADDRESS), 8 );
         }//if( Vpc3Read(bVpc3RwStartSapCtrlList+(i*7)) == 49 )
      #else
         ShowBuffer("SAP-List:", &pVpc3->abSapCtrlList[i*7], 7 );
         ShowBuffer("Ind-Ptr:", (UBYTE *)(VPC3_UNSIGNED8_PTR)((VPC3_ADR)(pVpc3->abSapCtrlList[(i*7)+4] << SEG_MULDIV)+(VPC3_ADR)VPC3_ASIC_ADDRESS), 48 );
         //resource manager
         if( pVpc3->abSapCtrlList[i*7] == 49 )
         {
            ShowBuffer("Rsp-Ptr:", (UBYTE *)(VPC3_UNSIGNED8_PTR)((VPC3_ADR)(pVpc3->abSapCtrlList[(i*7)+6] << SEG_MULDIV)+(VPC3_ADR)VPC3_ASIC_ADDRESS), 48 );
         }//if( pVpc3->abSapCtrlList[i*7] == 49 )
      #endif//#if VPC3_SERIAL_MODE
   }//for( i = 0; i < SAP_LENGTH/7; i++ )
}//void PrintSapList( void )
#endif//#if DP_FDL

/*---------------------------------------------------------------------------*/
/* function: PrintSerialInputs                                               */
/*---------------------------------------------------------------------------*/
void PrintSerialInputs( void )
{
   if( bRecCounter > 0 )
   {
      switch( abRecBuffer[bReadRecPtr] )
      {
         case 0x3F: // Key '?':
         {
            PrintHelp();
            bRecCounter--;
            break;
         }//case 0x3F:

         case 0x31: // Key '1'
         case 0x32: // Key '2'
         {
            print_string("Diag:");
            UserDemoDiagnostic( (abRecBuffer[bReadRecPtr]-0x30), 5 );
            bRecCounter--;
            break;
         }//case 0x31:

         case 0x33: // Key '3'
         case 0x34: // Key '4'
         case 0x35: // Key '5'
         case 0x36: // Key '6'
         {
            print_string("Diag1:");
            UserDemoDiagnostic( (abRecBuffer[bReadRecPtr]-0x30), 6 );
            bRecCounter--;
            break;
         }//case 0x33:

         case 0x61: // Key 'a'
         case 0x41: // Key 'A'
         {
            print_string("\r\nStatusreg: ");
            print_hexbyte( VPC3_GET_STATUS_H() );
            print_string(" ");
            print_hexbyte( VPC3_GET_STATUS_L() );
            bRecCounter--;
            break;
         }//case 0x61:

         // SPC3: mode register only writable
         case 0x62: // Key 'b'
         case 0x42: // Key 'B'
         {
            print_string("\r\nModereg1: ");
            print_hexbyte( VPC3_GET_MODE_REG_1() );
            print_hexbyte( VPC3_GET_HW_MODE_HIGH());
            print_hexbyte( VPC3_GET_HW_MODE_LOW());
            bRecCounter--;
            break;
         }//case 0x62:

         case 0x63: // Key 'c'
         case 0x43: // Key 'C'
         {
            PrintOrganizationalParameter();
            bRecCounter--;
            break;
         }//case 0x63:

         case 0x64: // Key 'd'
         case 0x44: // Key 'D'
         {
            PrintSapList();
            bRecCounter--;
            break;
         }//case 0x64:

         case 0x65: // Key 'e'
         case 0x45: // Key 'E'
         {
            PrintDiagBuffer();
            bRecCounter--;
            break;
         }//case 0x65:

         case 0x66: // Key 'f'
         case 0x46: // Key 'F'
         {
            PrintPrmData();
            bRecCounter--;
            break;
         }//case 0x66:

         case 0x67: // Key 'g'
         case 0x47: // Key 'G'
         {
            PrintCfgData();
            bRecCounter--;
            break;
         }//case 0x67:

         case 0x6A: // Key 'j'
         case 0x4A: // Key 'J'
         {
            print_string("\r\nVPC3+ free memory: 0x");
            print_hexword( VPC3_GetFreeMemory() );
            bRecCounter--;
            break;
         }//case 0x6A:

         #ifdef DP_DEBUG_ENABLE
            case 0x79: // Taste 'y'
            case 0x59: // Taste 'Y'
            {
               DP_ClearDebugBuffer();
               bRecCounter--;
               break;
            }//case 0x79:

            case 0x7A: // Taste 'z'
            case 0x5A: // Taste 'Z'
            {
               PrintDebugBuffer();
               bRecCounter--;
               break;
            }//case 0x7A:
         #endif//#ifdef DP_DEBUG_ENABLE

         default:
         {
             bRecCounter--;
             break;
         }//default:
      }//switch (abRecBuffer[bReadRecPtr])

      bReadRecPtr++;
   }//if( bRecCounter > 0 )
}//void PrintSerialInputs( void )

#endif//#ifdef RS232_SERIO

/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

