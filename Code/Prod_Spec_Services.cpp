/**
 **********************************************************************************************
 * @file            Prod_Spec_Services.cpp  product specific Service initialization
 *
 * @brief           Product Specific Service initialization
 * @details         This file shall include the initialization routines specific to product.
 *                  Adopter can initialize the ethernet/EMAC here
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "Includes.h"
#include "Prod_Spec_Services.h"
#include "Services.h"
#include "NV_Mem.h"
#include "CR_Tasker.h"
#include "Timers_Lib.h"
#include "uC_Reset.h"
#include "uC_Code_Range.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define SOFT_RESET_RESPONSE_TIME_uS  5000U
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

DCI_Owner* serial_num_owner = nullptr;
DCI_Owner* processor_serial_num_owner = nullptr;

DCI_Owner* hardware_rev_owner = nullptr;
DCI_Owner* reset_update_owner = nullptr;

DCI_Owner* m_owner_rest_reset_command;
enum rest_reset_commands
{
	NO_RESET,
	SOFT_RESET,
	FACTORY_RESET
};

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

bool PROD_SPEC_SERVICES_Wipe_NV_SVC( BF_Lib::Services::SERVICES_HANDLE_CB handle,
									 uint8_t service_id,
									 BF_Lib::Services::SERVICES_HANDLE_DATA_ST* service_data );

BOOL PROD_SPEC_SERVICES_Run_Bootloader_SVC( BF_Lib::Services::SERVICES_HANDLE_CB handle,
											UINT8 service_id,
											BF_Lib::Services::SERVICES_HANDLE_DATA_ST* service_data );

void Update_Reset_Monitor( void );

void Rest_Reset_Command_Init( void );

/*
 ********************************************************************************
 * See header file for function definition.
 ********************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif
UINT16 GetResetCounter( void )			// Added this new function call to get new value on every
										// reset. To fix conformance issue.
{
	UINT16 reset_counter = 0U;

	reset_update_owner->Check_Out( reset_counter );
	return ( reset_counter );
}

#ifdef __cplusplus
}
#endif

DCI_CB_RET_TD Rest_Reset_Command_CB( DCI_CBACK_PARAM_TD cback_param,
									 DCI_ACCESS_ST_TD* access_struct );

/**
 **********************************************************************************************
 * @brief                     Function for PROD_SPEC service Setup
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_SERVICES_Setup( void )
{
	DCI_Owner* crc_owner = nullptr;
	uint16_t crc_val = 0U;

	// It should be enabled later when we will implement complete functionality
#ifdef ESP32_TODO
	serial_num_owner = new DCI_Owner( DCI_PRODUCT_SERIAL_NUM_DCID );		// Initialize the Serial

	// Number.

	serial_num_owner->Attach_Callback( PROD_SPEC_SERVICES_MFG_Unlock_Write_Check_CB, NULL,
									   ( DCI_ACCESS_SET_RAM_CMD_MSK |
										 DCI_ACCESS_SET_INIT_CMD_MSK ) );

	processor_serial_num_owner = new DCI_Owner( DCI_PRODUCT_PROC_SERIAL_NUM_DCID );		// Initialize

	// the Serial
	// Number.

	processor_serial_num_owner->Attach_Callback( PROD_SPEC_SERVICES_MFG_Unlock_Write_Check_CB, NULL,
												 ( DCI_ACCESS_SET_RAM_CMD_MSK |
												   DCI_ACCESS_SET_INIT_CMD_MSK ) );


	hardware_rev_owner = new DCI_Owner( DCI_HARDWARE_VER_ASCII_DCID );			// Initialize the

	// Hardware Revision



	hardware_rev_owner->Attach_Callback( PROD_SPEC_SERVICES_MFG_Unlock_Write_Check_CB, NULL,
										 ( DCI_ACCESS_SET_RAM_CMD_MSK |
										   DCI_ACCESS_SET_INIT_CMD_MSK ) );

	// coverity[leaked_storage]
	new BF_Lib::Services();			/// System services setup.

	BF_Lib::Services::Add_Service_Handler( SERVICES_APP_WIPE_NV_MEM,
										   PROD_SPEC_SERVICES_Wipe_NV_SVC, NULL );
	BF_Lib::Services::Add_Service_Handler( SERVICES_APP_RUN_BOOTLOADER,
										   PROD_SPEC_SERVICES_Run_Bootloader_SVC, NULL );

	// crc_owner = new DCI_Owner( DCI_FIRMWARE_CRC_VAL_DCID );

	// crc_val = uC_CODE_RANGE_CRC_Val();
	// crc_owner->Check_In( crc_val );
	Rest_Reset_Command_Init();
#endif
	Update_Reset_Monitor();

	// coverity[leaked_storage]

}

/**
 **********************************************************************************************
 * @brief                     Function for Getting the Product Serial Number
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void GetProductSerialNumber( uint32_t* serialNumber )
{
	uint32_t product_serial_number = 0U;

	serial_num_owner = new DCI_Owner( DCI_PRODUCT_SERIAL_NUM_DCID );		// Initialize the Serial

	// Number.

	processor_serial_num_owner = new DCI_Owner( DCI_PRODUCT_PROC_SERIAL_NUM_DCID );

	serial_num_owner->Check_Out(
		reinterpret_cast<uint32_t*>( &product_serial_number ) );

	*serialNumber = product_serial_number;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Update_Reset_Monitor( void )
{
	// Must be done after the NV is initialized.
	uC_RESET_CAUSE_TD reset_cause;
	uint16_t reset_counter;
	DCI_Owner* reset_cause_owner = nullptr;

	reset_cause_owner = new DCI_Owner( DCI_RESET_CAUSE_DCID );

	reset_cause = uC_Reset::Get_Reset_Trigger();
	reset_cause_owner->Check_In( reset_cause );

	reset_update_owner = new DCI_Owner( DCI_RESET_COUNTER_DCID );

	reset_update_owner->Check_Out( reset_counter );
	if ( reset_counter < UINT16_MAX )
	{
		reset_counter++;
		reset_update_owner->Check_In( reset_counter );
		reset_update_owner->Load_Current_To_Init();
	}
	// coverity[leaked_storage]
}

/**
 **********************************************************************************************
 * @brief                     Function for PROD_SPEC_SERVICES_MFG_Unlock_Write_Check_CB
 * @param[in] void            none
 * @return[out] void          return status
 * @n
 **********************************************************************************************
 */
DCI_CB_RET_TD PROD_SPEC_SERVICES_MFG_Unlock_Write_Check_CB( DCI_CBACK_PARAM_TD handle,
															DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status;

	if ( BF_Lib::Services::MFG_Access_Unlocked() )
	{
		return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	}
	else
	{
		switch ( access_struct->command )
		{
			case DCI_ACCESS_SET_RAM_CMD:
				return_status = DCI_CBack_Encode_Error( DCI_ERR_RAM_READ_ONLY );
				break;

			case DCI_ACCESS_SET_INIT_CMD:
				return_status = DCI_CBack_Encode_Error( DCI_ERR_NV_READ_ONLY );
				break;

			default:
				return_status = DCI_CBack_Encode_Error( DCI_ERR_ACCESS_VIOLATION );
				break;
		}
	}

	return ( return_status );
}

/**
 **********************************************************************************************
 * @brief                     Function to initialize Soft Reset and Facory Reset DCIDs
 * @param[in] void            none
 * @return[out] none          none
 * @n
 **********************************************************************************************
 */
void Rest_Reset_Command_Init( void )
{
	/* initialize DCI parameters */
	m_owner_rest_reset_command = new DCI_Owner( DCI_REST_RESET_COMMAND_DCID );

	m_owner_rest_reset_command->Attach_Callback( &Rest_Reset_Command_CB,
												 NULL, ( DCI_ACCESS_POST_SET_RAM_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief                             static function for Soft Reset callback
 * @param[in] cr_task                 CR_Tasker handle
 * @param[in] access_struct           DCI parameter information
 * @return[out] DCI_CB_RET_TD         status
 * @n
 **********************************************************************************************
 */
DCI_CB_RET_TD Rest_Reset_Command_CB( DCI_CBACK_PARAM_TD cback_param,
									 DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	uint8_t reset_temp = 0;

	/* check if command is valid */
	if ( access_struct->command == DCI_ACCESS_POST_SET_RAM_CMD )
	{
		/* read the Soft Reset DCID  */
		m_owner_rest_reset_command->Check_Out( reset_temp );

		switch ( reset_temp )
		{
			case SOFT_RESET:
				/*Delay added for the 200 OK response*/
				uC_Delay( SOFT_RESET_RESPONSE_TIME_uS );
				BF_Lib::Services::Execute_Service( SERVICES_SOFT_RESET,
												   nullptr );
				break;

			case FACTORY_RESET:
				BF_Lib::Services::Execute_Service( SERVICES_FACTORY_RESET,
												   nullptr );
				break;

			default:
				break;
		}
	}
	return ( return_status );
}
