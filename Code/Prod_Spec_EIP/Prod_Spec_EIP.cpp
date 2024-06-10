/**
 **********************************************************************************************
 * @file            Prod_Spec_EIP.cpp
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to EIP component.
 *
 *
 * @copyright       2022 Eaton Corporation. All Rights Reserved.
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
#include "Includes.h"
#include "Prod_Spec_EIP.h"
#include "EIP_Display.h"
#include "EIP_Service.h"
#include "CIP_Ident_Svc.h"
#include "CIP_Ctrl_Spvsr.h"
#include "EIP_DCI_Data.h"
#include "CIP_Asm.h"
#include "Shadow_LTK.h"
#include "Dynamic_In_Asm.h"
#include "Dynamic_out_Asm.h"
#include "Prod_Spec_Debug.h"

/*
 ********************************************************************************
 *		Defines
 ********************************************************************************
 */
/*
 * Update below macros with required board specific gpio pin numbers
 * Currently pin number used here is same as BLE LED
 */
#define BLINK_Green_MS_GPIO   2
#define BLINK_Red_MS_GPIO     2
#define BLINK_Green_NS_GPIO   2
#define BLINK_Red_NS_GPIO     2

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint32_t PROD_SPEC_RESET_ACTIVE_COM_FAULTS_BIT = 0U;
static const uint32_t PROD_SPEC_FLUSH_COM_FAULT_QUE_BIT = 1U;
/*
 ********************************************************************************
 *		IO Definitions
 ********************************************************************************
 */
uC_USER_IO_STRUCT green_ms_led = {GPIO_MODE_OUTPUT, BLINK_Green_MS_GPIO, GPIO_PIN_INTR_HILEVEL};
uC_USER_IO_STRUCT red_ms_led = {GPIO_MODE_OUTPUT, BLINK_Red_MS_GPIO, GPIO_PIN_INTR_HILEVEL};
uC_USER_IO_STRUCT green_ns_led = {GPIO_MODE_OUTPUT, BLINK_Green_NS_GPIO, GPIO_PIN_INTR_HILEVEL};
uC_USER_IO_STRUCT red_ns_led = {GPIO_MODE_OUTPUT, BLINK_Red_NS_GPIO, GPIO_PIN_INTR_HILEVEL};

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
DCI_Owner* fault_reset_owner = nullptr;
Shadow_LTK* shadow_ltk = nullptr;
static EIP_Display* eip_display = nullptr;
static EIP_Service* eip_service = nullptr;
static Dynamic_In_Asm* Dynamic_In_Asm_object = nullptr;
static Dynamic_out_Asm* Dynamic_Out_Asm_object = nullptr;
EIP_Service::EIP_SVC_CALLBACK EIP_Service::m_active_msg_count_callback =
	reinterpret_cast<EIP_SVC_CALLBACK>( nullptr );

CIP_Asm::CIP_ASM_SVC_CALLBACK CIP_Asm::m_update_default_dynamic_asm_callback =
	reinterpret_cast<CIP_ASM_SVC_CALLBACK>( nullptr );


/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
void Setup_EtherNetIP( void );

void EIP_Led_Setup( void );

void EIP_Fault_Setup( void );

/*
 *****************************************************************************************
 * EIP can be setup before dynamic memory (new/delete) is turned on.
   It initializes the EIP stack but not the port.
 *****************************************************************************************
 */
void EIP_Led_Setup( void )
{
	BF_Misc::LED* m_MS_Green_LED_object;
	BF_Misc::LED* m_MS_Red_LED_object;
	BF_Misc::LED* m_NS_Green_LED_object;
	BF_Misc::LED* m_NS_Red_LED_object;

	m_MS_Green_LED_object = new BF_Misc::LED( ( new BF_Lib::Output_Pos( &green_ms_led ) ),
											  BF_Misc::LED::EXCLUDE_FROM_LED_TEST );

	m_MS_Red_LED_object = new BF_Misc::LED( new BF_Lib::Output_Pos( &red_ms_led ),
											BF_Misc::LED::EXCLUDE_FROM_LED_TEST );

	m_NS_Green_LED_object = new BF_Misc::LED( new BF_Lib::Output_Pos( &green_ns_led ),
											  BF_Misc::LED::EXCLUDE_FROM_LED_TEST );

	m_NS_Red_LED_object = new BF_Misc::LED( new BF_Lib::Output_Pos( &red_ns_led ),
											BF_Misc::LED::EXCLUDE_FROM_LED_TEST );

	/*m_NS_Green_LED_object = new BF_Misc::LED( new BF_Lib::Output_Pos( &EIP_NS_LED_GREEN_IO_CTRL ),
	                                          BF_Misc::LED::EXCLUDE_FROM_LED_TEST );

	   m_NS_Red_LED_object = new BF_Misc::LED( new BF_Lib::Output_Pos( &EIP_NS_LED_RED_IO_CTRL ),
	                                        BF_Misc::LED::EXCLUDE_FROM_LED_TEST );*/

	eip_display = new EIP_Display( m_MS_Green_LED_object, m_MS_Red_LED_object,
								   m_NS_Green_LED_object, m_NS_Red_LED_object );


	eip_display->LED_Test();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void update_default_dynamic_assembly( void )
{
	Dynamic_In_Asm_object->update_default_DCID_in_D_IN_ASMBuffer();
	Dynamic_Out_Asm_object->update_default_DCID_in_D_OUT_ASMBuffer();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Setup_EtherNetIP( void )
{
	uint8_t temp_state;
	// DCI_Owner* app_firm = reinterpret_cast<DCI_Owner*>( NULL );
	DCI_Owner* cip_device_type = reinterpret_cast<DCI_Owner*>( NULL );
	DCI_Owner* cip_state = reinterpret_cast<DCI_Owner*>( NULL );

	// coverity[leaked_storage]
	new DCI_Owner( DCI_CIP_VENDOR_ID_DCID );	// This value is hard coded for Eaton.
	// coverity[leaked_storage]
	new DCI_Owner( DCI_CIP_DEVICE_IDENT_DEV_STATUS_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_INPUT_TEST_BIT_1_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_OUTPUT_TEST_BIT_1_DCID );

	shadow_ltk = new Shadow_LTK();

	cip_state = new DCI_Owner( DCI_CIP_DEVICE_IDENT_STATE_DCID );

	temp_state = 3U;/* Magic number for now matches with the CIP "State_Operational" */

	cip_state->Check_In( temp_state );

	cip_device_type = new DCI_Owner( DCI_CIP_DEVICE_TYPE_DCID );

	cip_device_type->Check_In(
		reinterpret_cast<const uint8_t*>( &eip_dci_data_target_info.identity_dev_type ) );

	// ********************************************
	// CIP Specific initialization.
	// ********************************************
	CIP_IDENT_SVC_Init();

	// CIP_CTRL_SPVSR_Init( (CIP_CTRL_SUP_RESET_CBACK*)Shadow_LTK::Reset_Or_Trip_Static,
	// (CIP_CTRL_SUP_RESET_PARAM)shadow_ltk );

	// EIP_Service::Set_Active_Msg_Count_Callback(
	// & ( ETH_System::Com_Dog::Inc_Active_Message_Count ) );
	CIP_Asm::Set_Update_Dynamic_Asm_Callback( &( update_default_dynamic_assembly ) );

	eip_service = new EIP_Service( &eip_dci_data_target_info, eip_display, shadow_ltk );

	Dynamic_In_Asm_object = new Dynamic_In_Asm( eip_service );

	Dynamic_Out_Asm_object = new Dynamic_out_Asm();
	// coverity[leaked_storage]
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ID_TD Return_Mapped_DCID( uint8_t index )
{
	return ( Dynamic_Out_Asm_object->Return_DCID( index ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD PROD_SPEC_Fault_Reset_CB( DCI_CBACK_PARAM_TD handle,
										DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
	uint8_t reset_data;

	// reset_data = *(uint8_t*)access_struct->data_access.data;
	reset_data = *( reinterpret_cast<uint8_t*>( access_struct->data_access.data ) );
	switch ( access_struct->command )
	{
		case DCI_ACCESS_POST_SET_RAM_CMD:
			if ( Test_Bit( reset_data, PROD_SPEC_RESET_ACTIVE_COM_FAULTS_BIT ) )
			{
				BF_Misc::Faults::Reset();
			}
			if ( Test_Bit( reset_data, PROD_SPEC_FLUSH_COM_FAULT_QUE_BIT ) )
			{
				BF_Misc::Faults::Flush_Lists();
			}
			reset_data = 0U;
			fault_reset_owner->Check_In( reset_data );
			break;

		default:
			break;
	}
	return ( return_status );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'handle' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.
	 * Rather than create confusing code where we assign the param to itself
	 * which could imply an error or peppering the code with lint exceptions we
	 * mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Fault_Setup( void )
{
	DCI_Owner* fault_reset_owner_temp = new DCI_Owner( DCI_FAULT_RESET_DCID );

	new BF_Misc::Faults( DCI_LTK_FAULT_QUEUE_SORTED_DCID, DCI_LTK_FAULT_QUEUE_EVENT_ORDER_DCID,
						 DCI_LTK_FAULT_STATUS_BITS_DCID );

	// Do a complimentary reset on power up. User may expect it. We can provide it.
	BF_Misc::Faults::Reset();
	fault_reset_owner = fault_reset_owner_temp;
	fault_reset_owner->Attach_Callback( &PROD_SPEC_Fault_Reset_CB,
										nullptr, DCI_ACCESS_POST_SET_RAM_CMD_MSK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_EIP_Stack_Init( void )
{
	EIP_Service::Start_EIP_Stack();
	EIP_Service::Restart_EIP_Stack();	// Added by Shekhar during LTKBI to make m_restart_eip_stack
										// TRUE
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_EIP_Init( void )
{
	EIP_Fault_Setup();
	EIP_Led_Setup();
	Setup_EtherNetIP();
	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "EIP initialised" );
}
