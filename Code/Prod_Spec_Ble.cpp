/**
 *****************************************************************************************
 *	@file		Ble_Gatt_Server_Test.cpp
 *	@details    See header file for  overview.
 *	@copyright  2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Prod_Spec_Ble.h"
#include "Babelfish_Assert.h"
#include "gpio.h"
#include "Output_Pos.h"

/*
 *****************************************************************************************
 *	variables/pointers
 *****************************************************************************************
 */
static Ble_Api* ble_wrapper_obj = nullptr;
Ble_Advertisement* ble_adv = nullptr;
Ble_User_Management* ble_user_mgmt = nullptr;

///< Ble_DCI Object Pointer
Ble_DCI* ble_dci = nullptr;
Single_LED* ble_led = nullptr;

DCI_Owner* temp_url_owner = nullptr;
DCI_Owner* temp_tx_power_owner = nullptr;
DCI_Owner* ble_find_target_led_owner = nullptr;

#define BLINK_GPIO 2	// Change the gpio pin whichever you want

uC_USER_IO_STRUCT ble_led_ctrl = {GPIO_MODE_OUTPUT, BLINK_GPIO, GPIO_PIN_INTR_ANYEDGE};
/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void Create_DCI_Owner( const Ble_DCI::dci_ble_to_dcid_lkup_st_td_t* ble_to_dcid_struct, uint16_t totol_char_ids )
{
	uint16_t char_ids_idx = 0;

	for ( char_ids_idx = 0; char_ids_idx < totol_char_ids; char_ids_idx++ )
	{
		// coverity[leaked_storage]
		new DCI_Owner( ble_to_dcid_struct[char_ids_idx].dcid );
	}
}

/*
 *****************************************************************************************
 *  See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_BLE_Init()
{
	///< Create DCI Owners for each DCID parameters
	Create_DCI_Owner( ble_dci_data_target_info.ble_to_dcid_struct, DCI_BLE_DATA_TOTAL_CHAR_ID );

	/* If adopters wants to use the Eddystone Advertisement data
	    Eddystone_URL object can be created and accessed */

	///< Ble advertisement Common Dci Owner
	temp_tx_power_owner = new DCI_Owner( DCI_BLE_TX_POWER_DCID );

	///< Eddystone_URL Dci owner
	temp_url_owner = new DCI_Owner( DCI_BLE_EDDYSTONE_URL_DCID );

	///< BLE LED control DCI owner
	ble_find_target_led_owner = new DCI_Owner( DCI_BLE_FIND_TARGET_DEVICE_LED_CTRL_DCID );

	///< Create Object for Eddystone URL with ble advertisement and eddystone info
	ble_adv = new Eddystone_URL( temp_url_owner, temp_tx_power_owner );

	///< Create the Ble_DCI object with Ble DCI data info
	ble_dci = new Ble_DCI( &ble_dci_data_target_info, &ble_dci_data_group_info, DCI_SOURCE_IDS_Get() );

	///< Ble User management Object
	ble_user_mgmt = new Ble_User_Management( &user_entry, &role, &user_name, &user_pwd, &user_role );

	///< Create the Single LED object with ble led owner
	ble_led = new Single_LED( ble_find_target_led_owner, new BF_Misc::LED( new BF_Lib::Output_Pos(
																			   &ble_led_ctrl ) ), NULL );
	///< Initialization of ESP32 BLE stack
	ble_wrapper_obj = new Esp32_Ble_Api( ble_adv, ble_dci, ble_user_mgmt, ble_led );
	BF_ASSERT( ble_wrapper_obj );

	///< For BLE LED Display thread
	PROD_SPEC_BLE_App();
}

/**
 **********************************************************************************************
 * @brief                             This function is executed infinitely to handle the Ble
 *                                    Transmit and Receive messages through the blue gecko lib
 *									  BGAPI protocol is used for the communication
 * @param[in] void                    none
 * @return[out] void                  none
 * @n
 **********************************************************************************************
 */
static void Sample_Ble_App_Thread( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param )
{
	/*Call the Ble Display function to indicate the Ble connection states in a single color LED */
	ble_wrapper_obj->Ble_Led_Display_Control();
}

/**
 **********************************************************************************************
 *  See header file for function definition.
 **********************************************************************************************
 */
void PROD_SPEC_BLE_App( void )
{
	/* Create task to launch the BLE Communication */
	new CR_Tasker( Sample_Ble_App_Thread, NULL );
}
