/**
 **********************************************************************************************
 * @file            Prod_Spec_BACNET.h  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to BACNET
 * component.
 *
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef PROD_SPEC_BACNET_H

#define PROD_SPEC_BACNET_H


/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */
void PROD_SPEC_BACNET_Init( void );

bool_t Is_Bacnet_Ip_Enabled( void );

#endif
