/**
 **********************************************************************************************
 * @file            Prod_Specific_BACnet.cpp  product specific initialization
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
/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "Prod_Specific_BACnet.h"

/**
 **********************************************************************************************
 * @brief                     Function for Checking Prod_Spec Property
 * @param[in] void            none
 * @return[out] void          ret_val
 * @n
 **********************************************************************************************
 */
int32_t Check_Prod_Spec_Property( dword pid, frVbag* vb, octet iswrite, bool* musthave )
{
	int32_t ret_val = vsbe_noerror;

	switch ( pid )
	{
		default:
			ret_val = vsbe_unknownproperty;
	}
	return ( ret_val );
}
