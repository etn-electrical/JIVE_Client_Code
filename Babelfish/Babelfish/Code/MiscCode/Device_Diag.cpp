/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Device_Diag.h"
#include "OS_Task_Mon.h"
#include "uC_CStack.h"

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
DCI_Owner* Device_Diag::m_critical_os_task_stack_name = NULL;
DCI_Owner* Device_Diag::m_critical_os_task_stack_size = NULL;
DCI_Owner* Device_Diag::m_heap_usage = NULL;
DCI_Owner* Device_Diag::m_max_temperature = NULL;
DCI_Owner* Device_Diag::m_heap_usage_max = NULL;
DCI_Owner* Device_Diag::m_os_stack_usage_max = NULL;
DCI_Owner* Device_Diag::m_cstack_usage = NULL;

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define DEVICE_DIAG_CHECK_TIMEOUT       577


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Device_Diag::Init( void )
{
	BF_Lib::Timers* diag_check_timer;

	diag_check_timer = new BF_Lib::Timers( Checker_Task, NULL,
										   BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY, "Device Diag Check" );
	diag_check_timer->Start( DEVICE_DIAG_CHECK_TIMEOUT, true );

	// m_critical_os_task_stack_name = new DCI_Owner( DEVICE_DIAG_TASK_NAME_OWNID );
	// m_critical_os_task_stack_size = new DCI_Owner( DEVICE_DIAG_TASK_USAGE_OWNID );
	m_heap_usage = new DCI_Owner( DEVICE_DIAG_HEAP_SIZE_OWNID );

	m_heap_usage_max = new DCI_Owner( DEVICE_DIAG_HEAP_USE_MAX_OWNID );
	// m_os_stack_usage_max = new DCI_Owner( DEVICE_DIAG_TASK_USAGE_MAX_OWNID );

	m_cstack_usage = new DCI_Owner( DEVICE_DIAG_CSTACK_USAGE_OWNID );

	uC_CSTACK_Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Device_Diag::Checker_Task( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	uint8_t stored_max;
	uint8_t temp_percentage;

	temp_percentage = Ram::Used();
	m_heap_usage->Check_In( temp_percentage );

	m_heap_usage_max->Check_Out( stored_max );
	if ( temp_percentage > stored_max )
	{
		m_heap_usage_max->Check_In( temp_percentage );
		m_heap_usage_max->Load_Current_To_Init();
	}

	temp_percentage = OS_Task_Mon::Fragile_Task_Stack_Usage();
	// m_critical_os_task_stack_size->Check_In( temp_percentage );
	// if ( temp_percentage != 0 )
	// {
	// m_critical_os_task_stack_name->Check_In_Null_String( (char_t const *)OS_Task_Mon::Fragile_Task_Name() );
	// }

	// m_os_stack_usage_max->Check_Out( stored_max );
	// if ( temp_percentage > stored_max )
	// {
	// m_os_stack_usage_max->Check_In( temp_percentage );
	// m_os_stack_usage_max->Load_Current_To_Init();
	// }

	temp_percentage = uC_CSTACK_Used();
	m_cstack_usage->Check_In( temp_percentage );
}
