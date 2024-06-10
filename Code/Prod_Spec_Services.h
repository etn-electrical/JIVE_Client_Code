/**
 **********************************************************************************************
 * @file			Prod_Spec_Services.h
 *
 * @brief			This file contain macro used to enable different code sets with different
 * 					functionality on Product Specific Services.
 *
 * @details			These macros has been used in test_main.cpp file to call specific
 * 					functions
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef PROD_SPEC_SERVICES_H
#define PROD_SPEC_SERVICES_H

#include "DCI_Owner.h"


/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */

void PROD_SPEC_SERVICES_Setup( void );

void GetProductSerialNumber( uint32_t* serialNumber );

DCI_CB_RET_TD PROD_SPEC_SERVICES_MFG_Unlock_Write_Check_CB( DCI_CBACK_PARAM_TD handle,
															DCI_ACCESS_ST_TD* access_struct );

#ifdef __cplusplus
extern "C" {
#endif
UINT16 GetResetCounter( void );

#ifdef __cplusplus
}
#endif

#endif
