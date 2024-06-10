/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "CR_Tasker.h"
#include "DCI_Owner.h"
#include "Toolkit_Revision.h"
#include "IOT_Test_Task.h"
#include <stdio.h>
#include "Timers_Lib.h"
#include "Test_Example.h"

DCI_Owner* m_owner_group1_default;	// tag_value 102930
DCI_Owner* m_owner_group0_default;	// tag_value 102948
DCI_Owner* m_owner_group2_default;	// tag_value 102960
DCI_Owner* m_owner_group12_default;	// tag_value 102928
DCI_Owner* m_owner_group01_default;	// tag_value 102921
DCI_Owner* m_owner_writable_default;// tag_value 102982


uint16_t group1_default;
uint16_t group0_default;
uint16_t group2_default;
uint16_t group12_default;
uint16_t group01_default;
uint8_t writable_default;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

#ifdef IOT_TEST

void Test_Task( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
{

	CR_Tasker_Begin( cr_task );
	while ( true )
	{
		// Checking out DCIDs
		m_owner_group1_default->Check_Out( group1_default );
		m_owner_group0_default->Check_Out( group0_default );
		m_owner_group2_default->Check_Out( group2_default );
		m_owner_group12_default->Check_Out( group12_default );
		m_owner_group01_default->Check_Out( group01_default );
		// Increamenting values of DCIDs
		group1_default++;
		group0_default++;
		group2_default++;
		group12_default++;
		group01_default++;
		// Checking in Increamented values of DCIDs
		m_owner_group1_default->Check_In( group1_default );
		m_owner_group0_default->Check_In( group0_default );
		m_owner_group2_default->Check_In( group2_default );
		m_owner_group12_default->Check_In( group12_default );
		m_owner_group01_default->Check_In( group01_default );

		// Printing updated value of DCID received from cloud
		m_owner_writable_default->Check_Out( writable_default );
		printf( "writable_default = %d \n", writable_default );

		// Sleep task
		CR_Tasker_Delay( cr_task, 500U );
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Test_Task( void )
{
	m_owner_group1_default = new DCI_Owner( DCI_IOT_PWR_CONV_OUT_ACT_PWR_DCID );
	m_owner_group0_default = new DCI_Owner( DCI_IOT_PWR_CONV_TYPE_DCID );
	m_owner_group2_default = new DCI_Owner( DCI_IOT_PWR_SUM_MODE_DCID );
	m_owner_group12_default = new DCI_Owner( DCI_IOT_PWR_CONV_IN_2_VOLT_DCID );
	m_owner_group01_default = new DCI_Owner( DCI_IOT_PWR_CONV_IN_1_PH_1_VOLT_DCID );
	m_owner_writable_default = new DCI_Owner( DCI_IOT_BATSYS_BAT_TEST_COM_DCID );

	// This is test code
	// coverity[leaked_storage]
	new CR_Tasker( Test_Task, NULL, 3, "Test Task" );
}

#endif

#ifdef IOT_C2D_TIMING_TEST

static DCI_CB_RET_TD Update_IOT_DCID( DCI_CBACK_PARAM_TD cback_param,
									  DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	BF_Lib::Timers::TIMERS_TIME_TD begin_time;

	begin_time = BF_Lib::Timers::Get_Time();
	if ( access_struct->command == DCI_ACCESS_SET_RAM_CMD_MSK )
	{
		printf( "Before DCID update: Callback Time = %u ms counter\n", begin_time );
	}
	else if ( access_struct->command == DCI_ACCESS_POST_SET_RAM_CMD_MSK )
	{
		printf( "After DCID update: Callback Time = %u ms counter\n", begin_time );
	}

	return ( return_status );
}

/*
   To enable this test code, place the call to the function in the PROD_SPEC_Target_Main_Init() function.
   This is to be called after Wifi, IOT and SNTP Initializations.
 */
void IOT_C2D_Timing_Test( void )
{
	m_owner_writable_default = new DCI_Owner( DCI_IOT_BATSYS_BAT_TEST_COM_DCID );
	m_owner_writable_default->Attach_Callback( &Update_IOT_DCID, NULL,
											   DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK );

}

#endif
