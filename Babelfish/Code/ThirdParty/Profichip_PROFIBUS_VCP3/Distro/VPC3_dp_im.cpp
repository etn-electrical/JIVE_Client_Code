/***************************  Filename: dp_im.c  *****************************/
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
/* Function:      I&M-functions                                              */
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
     \brief Identification and Maintenace functions.

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
#include "VPC3_dp_user.h"

#if DP_FDL
/*--------------------------------------------------------------------------*/
/* function: ImReadReq (called by DPV1)                                     */
/*--------------------------------------------------------------------------*/
DPV1_RET_VAL ImReadReq( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
DPV1_RET_VAL bRetValue;
#if VPC3_SERIAL_MODE
#else
   psVPC3IMCALL psVpc3ImCall;
#endif//#if VPC3_SERIAL_MODE

   bRetValue = DPV1_OK;

   if( psMsgHeader->bSlotNr == 0 )
   {
      if( psMsgHeader->bIndex == 255 )
      {
            //check_extended function number, reserved byte and IM_INDEX
            if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_0 )
            {
               #if VPC3_SERIAL_MODE
              /*SSN: The below pointer has to typecasted to VPC3_ADR since the arguement 
              of Vpc3Write function wAddress is now a pointer of type unsigned int instead of a byte */
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 0, IM_FN_CALL ); /*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 1, 0x00 );  /*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 2, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] >> 8 ) );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 3, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] ) );/*SSN*/

                  CopyToVpc3_( (pToDpv1Data + 4), (MEM_UNSIGNED8_PTR)&sUser.sIM_0.abHeader[0], cSizeOfIM0 );/*SSN*/
               #else
                  psVpc3ImCall = (psVPC3IMCALL)pToDpv1Data;
                  psVpc3ImCall->bExtendedFunctionNumber = IM_FN_CALL;
                  psVpc3ImCall->bReserved = 0x00;
                  psVpc3ImCall->wIndex = pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ];

                  CopyToVpc3_( &psVpc3ImCall->abData[0], &sUser.sIM_0.abHeader[0], cSizeOfIM0 );
               #endif//#if VPC3_SERIAL_MODE

               if( psMsgHeader->bDataLength > ( 4 + cSizeOfIM0 ) )
               {
                  psMsgHeader->bDataLength = 4 + cSizeOfIM0;
               }//if( psMsgHeader->bDataLength > ( 4 + cSizeOfIM0 ) )
            }//if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_0 )

         #if IM1_SUPP

            //check_extended function number, reserved byte and IM_INDEX
            else
            if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_1 )
            {
               #if VPC3_SERIAL_MODE
              /*SSN: The below pointer has to typecasted to VPC3_ADR since the arguement 
              of Vpc3Write function wAddress is now a pointer of type unsigned int instead of a byte */
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 0, IM_FN_CALL );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 1, 0x00 );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 2, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] >> 8 ) );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 3, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] ) );/*SSN*/

                  CopyToVpc3_( (pToDpv1Data + 4), (MEM_UNSIGNED8_PTR)&sUser.sIM_1.abHeader[0], cSizeOfIM1 );
               #else
                  psVpc3ImCall = (psVPC3IMCALL)pToDpv1Data;
                  psVpc3ImCall->bExtendedFunctionNumber = IM_FN_CALL;
                  psVpc3ImCall->bReserved = 0x00;
                  psVpc3ImCall->wIndex = pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ];

                  CopyToVpc3_( &psVpc3ImCall->abData[0], &sUser.sIM_1.abHeader[0], cSizeOfIM1 );
               #endif//#if VPC3_SERIAL_MODE

               if( psMsgHeader->bDataLength > (4 + cSizeOfIM1) )
               {
                  psMsgHeader->bDataLength = 4 + cSizeOfIM1;
               }//if( psMsgHeader->bDataLength > (4 + cSizeOfIM1) )
            }//if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_1 )

         #endif//#if IM1_SUPP

         #if IM2_SUPP

            //check_extended function number, reserved byte and IM_INDEX
            else
            if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_2 )
            {
               #if VPC3_SERIAL_MODE
              /*SSN: The below pointer has to typecasted to VPC3_ADR since the arguement 
              of Vpc3Write function wAddress is now a pointer of type unsigned int instead of a byte */
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 0, IM_FN_CALL );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 1, 0x00 );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 2, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] >> 8 ) );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 3, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] ) );/*SSN*/

                  CopyToVpc3_( (pToDpv1Data + 4), (MEM_UNSIGNED8_PTR)&sUser.sIM_2.abHeader[0], cSizeOfIM2 );
               #else
                  psVpc3ImCall = (psVPC3IMCALL)pToDpv1Data;
                  psVpc3ImCall->bExtendedFunctionNumber = IM_FN_CALL;
                  psVpc3ImCall->bReserved = 0x00;
                  psVpc3ImCall->wIndex = pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ];

                  CopyToVpc3_( &psVpc3ImCall->abData[0], &sUser.sIM_2.abHeader[0], cSizeOfIM2 );
               #endif//#if VPC3_SERIAL_MODE

               if( psMsgHeader->bDataLength > (4 + cSizeOfIM2) )
               {
                  psMsgHeader->bDataLength = 4 + cSizeOfIM2;
               }//if( psMsgHeader->bDataLength > (4 + cSizeOfIM2) )
            }//if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_2 )

         #endif//#if IM2_SUPP

         #if IM3_SUPP

            //check_extended function number, reserved byte and IM_INDEX
            else
            if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_3 )
            {
               #if VPC3_SERIAL_MODE
              /*SSN: The below pointer has to typecasted to VPC3_ADR since the arguement 
              of Vpc3Write function wAddress is now a pointer of type unsigned int instead of a byte */
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 0, IM_FN_CALL );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 1, 0x00 );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 2, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] >> 8 ) );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 3, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] ) );/*SSN*/

                  CopyToVpc3_( (pToDpv1Data + 4), (MEM_UNSIGNED8_PTR)&sUser.sIM_3.abHeader[0], cSizeOfIM3 );
               #else
                  psVpc3ImCall = (psVPC3IMCALL)pToDpv1Data;
                  psVpc3ImCall->bExtendedFunctionNumber = IM_FN_CALL;
                  psVpc3ImCall->bReserved = 0x00;
                  psVpc3ImCall->wIndex = pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ];

                  CopyToVpc3_( &psVpc3ImCall->abData[0], &sUser.sIM_3.abHeader[0], cSizeOfIM3 );
               #endif//#if VPC3_SERIAL_MODE

               if( psMsgHeader->bDataLength > (4 + cSizeOfIM3) )
               {
                  psMsgHeader->bDataLength = 4 + cSizeOfIM3;
               }//if( psMsgHeader->bDataLength > (4 + cSizeOfIM3) )
            }//if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_3 )

         #endif//#if IM3_SUPP

         #if IM4_SUPP

            //check_extended function number, reserved byte and IM_INDEX
            else
            if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_4 )
            {
               #if VPC3_SERIAL_MODE
              /*SSN: The below pointer has to typecasted to VPC3_ADR since the arguement 
              of Vpc3Write function wAddress is now a pointer of type unsigned int instead of a byte */
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 0, IM_FN_CALL );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 1, 0x00 );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 2, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] >> 8 ) );/*SSN*/
                  Vpc3Write( (VPC3_ADR)pToDpv1Data + 3, (UBYTE)( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] ) );/*SSN*/

                  CopyToVpc3_( (pToDpv1Data + 4), (MEM_UNSIGNED8_PTR)&sUser.sIM_4.abHeader[0], cSizeOfIM4 );
               #else
                  psVpc3ImCall = (psVPC3IMCALL)pToDpv1Data;
                  psVpc3ImCall->bExtendedFunctionNumber = IM_FN_CALL;
                  psVpc3ImCall->bReserved = 0x00;
                  psVpc3ImCall->wIndex = pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ];

                  CopyToVpc3_( &psVpc3ImCall->abData[0], &sUser.sIM_4.abHeader[0], cSizeOfIM4 );
               #endif//#if VPC3_SERIAL_MODE

               if( psMsgHeader->bDataLength > (4 + cSizeOfIM4) )
               {
                  psMsgHeader->bDataLength = 4 + cSizeOfIM4;
               }//if( psMsgHeader->bDataLength > (4 + cSizeOfIM4) )
            }//if( pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] == IM0_INDEX_4 )

         #endif//#if IM4_SUPP

            else
            {
               //BugFix 503
               bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_STATE, 0x00 );
            }//else
      }//if( psMsgHeader->bIndex == 255 )
      else
      {
         bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0x00 );
      }//else of if( psMsgHeader->bSlotNr == 0 )
   }//if( psMsgHeader->bSlotNr == 0 )
   else
   {
      bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_APPLICATION | DPV1_ERRCL_APP_NOTSUPP, 0x00 );
   }//else of if( psMsgHeader->bSlotNr == 0 )

   return bRetValue;
}//DPV1_RET_VAL ImReadReq( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )

/*--------------------------------------------------------------------------*/
/* function: ImWriteReq (called by DPV1)                                    */
/*--------------------------------------------------------------------------*/
DPV1_RET_VAL ImWriteReq( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )
{
DPV1_RET_VAL bRetValue;
sIMCALL      sImCall;

   bRetValue = DPV1_OK;

   if( psMsgHeader->bSlotNr == 0 )
   {
      if( psMsgHeader->bIndex == 255 )
      {
         //check_extended function number, reserved byte and IM_INDEX
         CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sImCall, pToDpv1Data, cSizeOfImCall );

         if(    ( sImCall.bExtendedFunctionNumber == IM_FN_CALL )
             && ( sImCall.bReserved == 0x00                     )
           )
         {
            //BugFix 503
            //check length
            if( psMsgHeader->bDataLength == 0x04 )
            {
                  //check IM_INDEX
                  if( sImCall.wIndex == IM0_INDEX_0 )
                  {
                     //OK, save IM_INDEX
                     pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_0;
                  }//if( sImCall.wIndex == IM0_INDEX_0 )

               #if IM1_SUPP
                  //check IM_INDEX
                  else
                  if( sImCall.wIndex == IM0_INDEX_1 )
                  {
                     //OK, save IM_INDEX
                     pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_1;
                  }//if( sImCall.wIndex == IM0_INDEX_1 )
               #endif//#if IM1_SUPP

               #if IM2_SUPP
                  else
                  if( sImCall.wIndex == IM0_INDEX_2 )
                  {
                     //OK, save IM_INDEX
                     pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_2;
                  }//if( sImCall.wIndex == IM0_INDEX_2 )
               #endif//#if IM2_SUPP

               #if IM3_SUPP
                  else
                  if( sImCall.wIndex == IM0_INDEX_3 )
                  {
                     //OK, save IM_INDEX
                     pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_3;
                  }//if( sImCall.wIndex == IM0_INDEX_3 )
               #endif//#if IM3_SUPP

               #if IM4_SUPP
                  else
                  if( sImCall.wIndex == IM0_INDEX_4 )
                  {
                     //OK, save IM_INDEX
                     pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_4;
                  }//if( sImCall.wIndex == IM0_INDEX_4 )
               #endif//#if IM4_SUPP

                  else
                  {
                      bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0x00 );
                  }//else of if( sImCall.wIndex == IM0_INDEX_0 )
            }//if( psMsgHeader->bDataLength == 0x04 )

            else
            if( psMsgHeader->bDataLength == 0x44 )
            {
               #if IM1_SUPP
                  //check IM_INDEX
                  if( sImCall.wIndex == IM0_INDEX_1 )
                  {
                     //OK, save IM_INDEX
                     pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_1;
                     CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.sIM_1.abHeader[0], pToDpv1Data+4, cSizeOfIM1 );

                     sUser.sIM_0.wRevCounter++;
                  }//if( sImCall.wIndex == IM0_INDEX_1 )

                  //else /* SSN commented */
               #endif//#if IM1_SUPP

               #if IM2_SUPP
                  else
                  if( sImCall.wIndex == IM0_INDEX_2 )
                  {
                     //OK, save IM_INDEX
                     pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_2;
                     CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.sIM_2.abHeader[0], pToDpv1Data+4, cSizeOfIM2 );

                     sUser.sIM_0.wRevCounter++;
                  }//if( sImCall.wIndex == IM0_INDEX_2 )

                  //else /* SSN commented */
               #endif//#if IM2_SUPP

               #if IM3_SUPP
                  else
                  if( sImCall.wIndex == IM0_INDEX_3 )
                  {
                     //OK, save IM_INDEX
                     pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_3;
                     CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.sIM_3.abHeader[0], pToDpv1Data+4, cSizeOfIM3 );

                     sUser.sIM_0.wRevCounter++;
                  }//if( sImCall.wIndex == IM0_INDEX_3 )

                  //else /* SSN commented */
               #endif//#if IM3_SUPP

               #if IM4_SUPP
                  else
                  if( sImCall.wIndex == IM0_INDEX_4 )
                  {
                     //OK, save IM_INDEX
                     pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = IM0_INDEX_4;
                     CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.sIM_4.abHeader[0], pToDpv1Data+4, cSizeOfIM4 );

                     sUser.sIM_0.wRevCounter++;
                  }//if( sImCall.wIndex == IM0_INDEX_4 )

                  //else /* SSN commented */
               #endif//#if IM4_SUPP

                  {
                     bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_ACCESS, 0x00 );
                  }//else of if( sImCall.wIndex == IM0_INDEX_0 )
             }//else if( psMsgHeader->bDataLength == 0x44 )

            else
            {
               pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = 0;
               bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_ACCESS, 0x00 );
            }//else of if( psMsgHeader->bDataLength == 0x04 )
         }//if(    ( sImCall.bExtendedFunctionNumber == IM_FN_CALL ) ...
         else
         {
            pDpSystem->awImIndex[ ( bSapNr & 0x0F ) ] = 0;
            bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_PARAM, 0x00 );
         }//else of if(    ( sImCall.bExtendedFunctionNumber == IM_FN_CALL ) ...
      }//if( psMsgHeader->bIndex == 255 )
      else
      {
         bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_ACCESS | DPV1_ERRCL_ACC_INV_INDEX, 0x00 );
      }//else of if( psMsgHeader->bIndex == 255 )
   }//if( psMsgHeader->bSlotNr == 0 )
   else
   {
      bRetValue = FDL_SetDpv1ErrorCode( psMsgHeader, DPV1_ERRCL_APPLICATION | DPV1_ERRCL_APP_NOTSUPP, 0x00 );
   }//else of if( psMsgHeader->bSlotNr == 0 )

   return bRetValue;
}//DPV1_RET_VAL ImWriteReq( UBYTE bSapNr, MSG_HEADER_PTR psMsgHeader, VPC3_UNSIGNED8_PTR pToDpv1Data )

#endif // #if DP_FDL


/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

