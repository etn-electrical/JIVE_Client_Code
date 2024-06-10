/**
 *****************************************************************************************
 *   @file
 *
 *   @brief This file is autogenerated.  It is created to handle all the processor and
 *   memory IDs across products.
 *
 *   @version
 *   C++ Style Guide Version 1.0
 *
 *   @copyright 2013 Eaton Corporation. All Rights Reserved.
 *****************************************************************************************
 */
#ifndef PROC_MEM_ID_CODES_H
  #define PROC_MEM_ID_CODES_H

// *******************************************************
// ******     Definitions follow
// *******************************************************

/* PRODUCT_GUID and PRODUCT_CODE is same
   PX-Green has reserved PRODUCT_GUID = 0x1234 for Sample Application and demonstration.
   Product should must different product code
   Contact PX Green Team to assign new product code for your project.
 */
static const uint16_t PRODUCT_GUID = 0x1234;

/* NA - Not Defined -reserved, proc id/comp id as 0 should not be used */
const uint16_t NA_NOT_DEFINED_PROC_ID = 0;

/* local component ( processor ) GUID. ( guid and index is NOT required to be same )
   If value of LOCAL_COMP_GUID is changed, same should be updated in Uberloder ( PX_GREEN_LOCAL_PROC_ID -
      Proc_Mem_ID_Codes.h )
 */
static const uint16_t LOCAL_COMP_GUID = 1;

/* GUID of all Images/firmware of local component/processor. ( guid and index is NOT required to be same )
   If value of LOCAL_COMP_APP_IMG_GUID is changed, same should be updated in Uberloder (
      PX_GREEN_LOCAL_PROC_ID_INT_FLASH_MEM_ID - Proc_Mem_ID_Codes.h )
 */
static const uint16_t LOCAL_COMP_APP_IMG_GUID = 0;
static const uint16_t LOCAL_COMP_WEB_IMG_GUID = 1;
#ifdef REST_I18N_LANGUAGE
static const uint16_t LOCAL_COMP_LANG_IMG_GUID = 2;
#endif

#endif
