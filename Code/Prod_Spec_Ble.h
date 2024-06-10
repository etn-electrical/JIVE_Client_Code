/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 *	@file		Ble_Gatt_Server_Test.h
 *
 *	@brief      Sample code to demonstrate the BLE GATT and GAP functionality.
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef CODE_PROD_SPEC_BLE_H_
#define CODE_PROD_SPEC_BLE_H_


#include "Esp32_Ble_Api.h"
#include "Eddystone_URL.h"
#include "Ble_Api.h"
#include "DCI_Data.h"
#include "BLE_DCI_Data.h"
#include "Ble_DCI.h"
#include "DCI_Owner.h"
#include "DCI_Patron.h"
#include "Single_LED.h"

#include "Ble_Advertisement.h"
#include "Ble_User_Management.h"
#include "Ble_Defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *****************************************************************************************
 *@brief         Function to Create DCI Owner for each DCID parameters
 *@param[in]     ble_to_dcid_struct Pointer to Ble UUID to DCID lookup
 *@return        totol_char_ids Total number of Char ID
 *****************************************************************************************
 */
void Create_DCI_Owner( const Ble_DCI::dci_ble_to_dcid_lkup_st_td_t* ble_to_dcid_struct,
					   uint16_t totol_char_ids );

/*
 *****************************************************************************************
 *@brief         Function to test the BLE GATT server implementation
 *@param[in]     none
 *@return        none
 *****************************************************************************************
 */
void PROD_SPEC_BLE_Init( void );

/*
 *****************************************************************************************
 *@brief         Function for creating BLE LED Display thread
 *@param[in]     none
 *@return        none
 *****************************************************************************************
 */
void PROD_SPEC_BLE_App( void );

#ifdef __cplusplus
}
#endif


#endif	/* CODE_PROD_SPEC_BLE_H_ */
