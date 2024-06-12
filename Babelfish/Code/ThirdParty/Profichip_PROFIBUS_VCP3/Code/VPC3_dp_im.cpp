/***************************  Filename: DpDiag.c *****************************/
/* ========================================================================= */
/*                                                                           */
/* 0000  000   000  00000 0  000  0   0 0 0000                               */
/* 0   0 0  0 0   0 0     0 0   0 0   0 0 0   0                              */
/* 0   0 0  0 0   0 0     0 0     0   0 0 0   0      Einsteinstraße 6        */
/* 0000  000  0   0 000   0 0     00000 0 0000       91074 Herzogenaurach    */
/* 0     00   0   0 0     0 0     0   0 0 0                                  */
/* 0     0 0  0   0 0     0 0   0 0   0 0 0          Phone: ++499132744200   */
/* 0     0  0  000  0     0  000  0   0 0 0    GmbH  Fax:   ++4991327442164  */
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Technical support:       eMail: support@profichip.com                     */
/*                          Phone: ++49-9132-744-2150                        */
/*                          Fax. : ++49-9132-744-29-2150                     */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/* contents:


*/
/*****************************************************************************/
/* include hierarchy */
#include "includes.h"			// Added by Sagar on 24.09.2012 for DPV1 integration
#include "VPC3_platform.h"	// Path changed for Galaxy //
#include "VPC3.h"				// Added by Sagar on 24.09.2012 for DPV1 integration
#include "VPC3_dp_inc.h"		// Path changed for Galaxy //
#include "VPC3_dp_user.h"
#include "DCI_Constants.h"              // For the defincation of Application firmware version in I&M

#include "Prod_Spec_Services.h"

#if DPV1_IM_SUPP

/*---------------------------------------------------------------------------*/
/* defines, structures                                                       */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* global user data definitions                                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* defines, structures and variables for our demo application                */
/*---------------------------------------------------------------------------*/
extern UBYTE IM_DEFAULT[64];
#if IM_1_4_OPTIONAL_SUPP
   ROMCONST__ uint8_t IM1_DEFAULT[64] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Header, 10 Octets
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // TagFunction
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                          0x20, 0x20,
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // TagLocation
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                          0x20, 0x20
                                        };

   ROMCONST__ uint8_t IM2_DEFAULT[64] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Header, 10 Octets
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // InstallationDate
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Reserved
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                        };

   ROMCONST__ uint8_t IM3_DEFAULT[64] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Header, 10 Octets
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // Descriptor
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                          0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                          0x20, 0x20, 0x20, 0x20
                                        };

   ROMCONST__ uint8_t IM4_DEFAULT[64] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Header, 10 Octets
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Signature
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00
                                        };
#endif//#if IM_1_4_OPTIONAL_SUPP

sIM0     sHeadIM0;

/*---------------------------------------------------------------------------*/
/* function prototypes                                                       */
/*---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function: DpIm_ClearImIndex                                              */
/*--------------------------------------------------------------------------*/
void DpIm_ClearImIndex( uint8_t bSapNr )
{
   pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = 0x0000;
}//void DpIm_ClearImIndex( uint8_t bSapNr )

/*--------------------------------------------------------------------------*/
/* function: DpIm_PowerOn                                                   */
/*--------------------------------------------------------------------------*/
uint8_t DpIm_PowerOn( void )
{
//uint8_t abSwRev[] = { 0x56, VERSION1-0x30, VERSION2-0x30, VERSION3-0x30 };

    UpdateProfiBusIMBuffer();
    memcpy( &sDpAppl.sIM_0.abHeader[0], &IM_DEFAULT[0], cSizeOfIM0 );
    sDpAppl.sIM_0.wIMSupported = IM00_SUPPORTED;

#if IM_1_4_OPTIONAL_SUPP
    sDpAppl.wRevCounter = 0;
    memcpy( &sDpAppl.sIM_1.abHeader[0], &IM1_DEFAULT[0], cSizeOfIM1 );
    sDpAppl.sIM_0.wIMSupported |= IM01_SUPPORTED;
    memcpy( &sDpAppl.sIM_2.abHeader[0], &IM2_DEFAULT[0], cSizeOfIM2 );
    sDpAppl.sIM_0.wIMSupported |= IM02_SUPPORTED;
    memcpy( &sDpAppl.sIM_3.abHeader[0], &IM3_DEFAULT[0], cSizeOfIM3 );
    sDpAppl.sIM_0.wIMSupported |= IM03_SUPPORTED;
    memcpy( &sDpAppl.sIM_4.abHeader[0], &IM4_DEFAULT[0], cSizeOfIM4 );
    sDpAppl.sIM_0.wIMSupported |= IM04_SUPPORTED;
#endif//IM_1_4_OPTIONAL_SUPP

//    memcpy( &sDpAppl.sIM_0.abSoftwareRevision[0], &abSwRev[0], 4 );

   return 0;
}//uint8_t DpIm_PowerOn( void )

/*--------------------------------------------------------------------------*/
/* function: DpIm_StoreImRecord                                             */
/*--------------------------------------------------------------------------*/
#if IM_1_4_OPTIONAL_SUPP
static DPV1_RET_VAL DpIm_StoreImRecord( uint8_t bSapNr, uint16_t wImIndex, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
DPV1_RET_VAL bRetValue;

   bRetValue = DPV1_OK;

   switch( wImIndex )
   {
      case IM0_INDEX_1:
      {
         pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_1;
         pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] = IM_CS_WRITE_RECORD;
         CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sDpAppl.sIM_1.abHeader[0], pToDpv1Data+4, cSizeOfIM1 );
         sDpAppl.wRevCounter++;
         break;
      } /* case IM0_INDEX_1 */

      case IM0_INDEX_2:
      {
         pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_2;
         pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] = IM_CS_WRITE_RECORD;
         CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sDpAppl.sIM_2.abHeader[0], pToDpv1Data+4, cSizeOfIM2 );
         sDpAppl.wRevCounter++;
         break;
      } /* case IM0_INDEX_2 */

      case IM0_INDEX_3:
      {
         pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_3;
         pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] = IM_CS_WRITE_RECORD;
         CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sDpAppl.sIM_3.abHeader[0], pToDpv1Data+4, cSizeOfIM3 );
         sDpAppl.wRevCounter++;
         break;
      } /* case IM0_INDEX_3 */

      case IM0_INDEX_4:
      {
         pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_4;
         pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] = IM_CS_WRITE_RECORD;
         CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sDpAppl.sIM_4.abHeader[0], pToDpv1Data+4, cSizeOfIM4 );
         sDpAppl.wRevCounter++;
         break;
      } /* case IM0_INDEX_4 */

      default:
      {
         bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_ACCESS, 0 );
         break;
      } /* default: */
   } /* switch( wImIndex ) */

   return bRetValue;
} /* static DPV1_RET_VAL DpIm_StoreImRecord( uint8_t bSapNr, uint16_t wImIndex, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data ) */
#endif /* #if IM_1_4_OPTIONAL_SUPP */

/*--------------------------------------------------------------------------*/
/* function: DpIm_WriteImFunction                                           */
/*--------------------------------------------------------------------------*/
DPV1_RET_VAL DpIm_WriteImFunction( uint8_t bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
DPV1_RET_VAL bRetValue;
sIMCALL      sImCall;

   bRetValue = DPV1_OK;

   if( psMsgHeader->bSlotNr == 0 )
   {
      if( psMsgHeader->bIndex == 255 )
      {
         /* check_extended function number, reserved byte and IM_INDEX */
         CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sImCall, pToDpv1Data, cSizeOfImCall );
         DpIm_ClearImIndex( bSapNr );

         if(    ( sImCall.bExtendedFunctionNumber == IM_FN_CALL )
             && ( sImCall.bReserved == 0x00                     )
           )
         {
               if( psMsgHeader->bDataLength == 0x04 )
               {
                  pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] = 0x00;

                  switch( sImCall.wIndex )
                  {
                        case IM0_INDEX_0:
                        {
                           pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_0;
                           break;
                        }  /* case IM0_INDEX_0: */

                     #if IM_1_4_OPTIONAL_SUPP
                        case IM0_INDEX_1:
                        {
                           pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_1;
                           break;
                        }  /* case IM0_INDEX_1: */

                        case IM0_INDEX_2:
                        {
                           pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_2;
                           break;
                        }  /* case IM0_INDEX_2: */

                        case IM0_INDEX_3:
                        {
                           pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_3;
                           break;
                        }  /* case IM0_INDEX_3: */

                        case IM0_INDEX_4:
                        {
                           pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_4;
                           break;
                        }  /* case IM0_INDEX_4: */
                     #endif /* #f IM_1_4_OPTIONAL_SUPP */

                        default:
                        {
                           bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0 );
                        } /* default: */
                  } /* switch( sImCall.wIndex ) */
               } /* if( psMsgHeader->bDataLength == 0x04 ) */

            #if IM_1_4_OPTIONAL_SUPP
               else
               if( psMsgHeader->bDataLength == 0x44 )
               {
                  /* check IM_INDEX */
                  if(    ( sImCall.wIndex == IM0_INDEX_1 )
                      || ( sImCall.wIndex == IM0_INDEX_2 )
                      || ( sImCall.wIndex == IM0_INDEX_3 )
                      || ( sImCall.wIndex == IM0_INDEX_4 )
                    )
                  {
                     bRetValue = DpIm_StoreImRecord( bSapNr, sImCall.wIndex, psMsgHeader, pToDpv1Data );
                  } /* if(    ( sImCall.wIndex == IM0_INDEX_1 ) ... */
                  else
                  {
                     bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_ACCESS, 0 );
                  } /* else of if(    ( sImCall.wIndex == IM0_INDEX_1 ) ... */
               } /* else if( psMsgHeader->bDataLength == 0x44 ) */
            #endif /* #f IM_1_4_OPTIONAL_SUPP */

               else
               {
                  pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = 0;
                  pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] = 0x00;
                  bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_ACCESS, 0 );
               } /* else of if( psMsgHeader->bDataLength == 0x04 ) */
         } /* if(    ( sImCall.bExtendedFunctionNumber == IM_FN_CALL ) ... */
         else
         {
            pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = 0;
            pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] = 0x00;
            bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_PARAM, 0 );
         } /* else of if(    ( sImCall.bExtendedFunctionNumber == IM_FN_CALL ) ... */
      } /* if( psMsgHeader->bIndex == 255 ) */
      else
      {
         bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0 );
      } /* else of if( psMsgHeader->bIndex == 255 ) */
   } /* if( psMsgHeader->bSlotNr == 0 ) */
   else
   {
      bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0 );
   } /* else of if( psMsgHeader->bSlotNr == 0 ) */

   return bRetValue;
} /* DPV1_RET_VAL DpIm_WriteImFunction( uint8_t bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data ) */

/*--------------------------------------------------------------------------*/
/* function: DpIm_ReadImFunction                                            */
/*--------------------------------------------------------------------------*/
DPV1_RET_VAL DpIm_ReadImFunction( uint8_t bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
DPV1_RET_VAL bRetValue;
uint16_t     wImIndex;
#if IM_1_4_OPTIONAL_SUPP
   uint8_t * pbImRec;
#endif /* #if IM_1_4_OPTIONAL_SUPP */
#if VPC3_SERIAL_MODE
#else
   psVPC3IMCALL psVpc3ImCall;
#endif /* #if VPC3_SERIAL_MODE */

   bRetValue = DPV1_OK;

   if( psMsgHeader->bSlotNr == 0 )
   {
      if( psMsgHeader->bIndex == 255 )
      {
            /* check_extended function number, reserved byte and IM_INDEX */
            wImIndex = pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ];
            if( wImIndex == IM0_INDEX_0 )
            {
               #if IM_1_4_OPTIONAL_SUPP
                  #if BIG_ENDIAN
                     sDpAppl.sIM_0.wRevCounter = sDpAppl.wRevCounter;
                  #else
                     sDpAppl.sIM_0.wRevCounter = SWAP_WORD( sDpAppl.wRevCounter );
                  #endif /* #if BIG_ENDIAN */
               #endif /* #if IM_1_4_OPTIONAL_SUPP */

               #if VPC3_SERIAL_MODE
                  Vpc3Write( ((VPC3_ADR)( pToDpv1Data )) + 0, IM_FN_CALL );
                  Vpc3Write( ((VPC3_ADR)( pToDpv1Data )) + 1, 0x00 );
                  //#Pre-Conformance Change:wImIndex MSB,LSB Correction
                  Vpc3Write( ((VPC3_ADR)( pToDpv1Data )) + 2, (uint8_t) wImIndex  );
                  Vpc3Write( ((VPC3_ADR)( pToDpv1Data )) + 3, (uint8_t)( wImIndex >> 8 ) );

                  CopyToVpc3_( (pToDpv1Data + 4), (MEM_UNSIGNED8_PTR)&sDpAppl.sIM_0.abHeader[0], cSizeOfIM0 );
               #else
                  psVpc3ImCall = (psVPC3IMCALL)pToDpv1Data;
                  psVpc3ImCall->bExtendedFunctionNumber = IM_FN_CALL;
                  psVpc3ImCall->bReserved = 0x00;
                  psVpc3ImCall->wIndex = wImIndex;

                  CopyToVpc3_( &psVpc3ImCall->abData[0], &sDpAppl.sIM_0.abHeader[0], cSizeOfIM0 );
               #endif /* #if VPC3_SERIAL_MODE */

               if( psMsgHeader->bDataLength > ( 4 + cSizeOfIM0 ) )
               {
                  psMsgHeader->bDataLength = 4 + cSizeOfIM0;
               } /* if( psMsgHeader->bDataLength > ( 4 + cSizeOfIM0 ) ) */
            } /* if( wImIndex == IM0_INDEX_0 ) */

         #if IM_1_4_OPTIONAL_SUPP

            else
            if(    ( wImIndex == IM0_INDEX_1 )
                || ( wImIndex == IM0_INDEX_2 )
                || ( wImIndex == IM0_INDEX_3 )
                || ( wImIndex == IM0_INDEX_4 )
              )
            {
               switch( wImIndex )
               {
                  case IM0_INDEX_1:
                     pbImRec = (uint8_t *)&sDpAppl.sIM_1.abHeader[0];
                     break;
                  case IM0_INDEX_2:
                     pbImRec = (uint8_t *)&sDpAppl.sIM_2.abHeader[0];
                     break;
                  case IM0_INDEX_3:
                     pbImRec = (uint8_t *)&sDpAppl.sIM_3.abHeader[0];
                     break;
                  case IM0_INDEX_4:
                  default:
                     pbImRec = (uint8_t *)&sDpAppl.sIM_4.abHeader[0];
                     break;
               } /* switch( wImIndex ) */

               #if VPC3_SERIAL_MODE
                  Vpc3Write( ((VPC3_ADR)( pToDpv1Data )) + 0, IM_FN_CALL );
                  Vpc3Write( ((VPC3_ADR)( pToDpv1Data )) + 1, 0x00 );
                  //#Pre-Conformance Change:wImIndex MSB,LSB Correction
                  Vpc3Write( ((VPC3_ADR)( pToDpv1Data )) + 2, (uint8_t) wImIndex  );
                  Vpc3Write( ((VPC3_ADR)( pToDpv1Data )) + 3, (uint8_t)( wImIndex >> 8 ) );

                  CopyToVpc3_( (pToDpv1Data + 4), pbImRec, cSizeOfIM1 );
               #else
                  psVpc3ImCall = (psVPC3IMCALL)pToDpv1Data;
                  psVpc3ImCall->bExtendedFunctionNumber = IM_FN_CALL;
                  psVpc3ImCall->bReserved = 0x00;
                  psVpc3ImCall->wIndex = wImIndex;

                  CopyToVpc3_( &psVpc3ImCall->abData[0], pbImRec, cSizeOfIM1 );
               #endif /* #if VPC3_SERIAL_MODE */

               if( pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] & IM_CS_WRITE_RECORD )
               {
                  psMsgHeader->bDataLength = 4;
               } /* if( pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] & IM_CS_WRITE_RECORD ) */
               else
               {
                  if( psMsgHeader->bDataLength > (4 + cSizeOfIM1) )
                  {
                     psMsgHeader->bDataLength = 4 + cSizeOfIM1;
                  } /* if( psMsgHeader->bDataLength > (4 + cSizeOfIM1) ) */
               } /* else of if( pDpSystem->abCallService[ ( bSapNr & 0x0F ) ] & IM_CS_WRITE_RECORD ) */
            } /* if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_1 ) */

         #endif /* #if IM_1_4_OPTIONAL_SUPP */

            else
            {
               bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_STATE, 0 );
            } /* else */
      } /* if( psMsgHeader->bIndex == 255 ) */
      else
      {
         bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0 );
      } /* else of if( psMsgHeader->bSlotNr == 0 ) */
   } /* if( psMsgHeader->bSlotNr == 0 ) */
   else
   {
      bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0 );
   } /* else of if( psMsgHeader->bSlotNr == 0 ) */

   return bRetValue;
} /* DPV1_RET_VAL DpIm_ReadImFunction( uint8_t bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data ) */


void UpdateProfiBusIMBuffer( void )
{
    uint32_t serial_number;
    int8_t i = 0;
    GetProductSerialNumber( &serial_number );
    while(serial_number)
    {
    	IM_DEFAULT[47+i] = (serial_number%10)+48;
    	serial_number/=10;
        i--;
    }
}

#endif//#if DPV1_IM_SUPP


/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

