/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "System_Reset.h"
#include "uC_Reset.h"
#include "Exception.h"

namespace BF
{

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
System_Reset* const System_Reset::END_OF_RESET_LIST =
	reinterpret_cast<System_Reset*>( nullptr );
System_Reset::reset_select_t System_Reset::m_reset_id = NO_RESET_REQUESTED_ID;
BF_Lib::Timers::TIMERS_TIME_TD System_Reset::m_reset_success_time_sample;

System_Reset* System_Reset::m_reset_ready_cb_list = System_Reset::END_OF_RESET_LIST;
System_Reset* System_Reset::m_reset_pending_cb_list = System_Reset::END_OF_RESET_LIST;
CR_Tasker* System_Reset::m_cr_task = reinterpret_cast<CR_Tasker*>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
System_Reset::System_Reset( cback_func_t reset_ready_cb,
							cback_param_t reset_ready_handle ) :
	m_next( m_reset_ready_cb_list ),
	m_reset_ready_cb( reset_ready_cb ),
	m_reset_ready_handle( reset_ready_handle )
{
	m_reset_ready_cb_list = this;

	if ( m_cr_task == reinterpret_cast<CR_Tasker*>( nullptr ) )
	{
		m_cr_task = new CR_Tasker( &Reset_Waiting_Task,
								   reinterpret_cast<CR_TASKER_PARAM>( nullptr ),
								   CR_TASKER_IDLE_PRIORITY,
								   "System Reset Device" );

		m_cr_task->Suspend();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
System_Reset::~System_Reset( void )
{
	BF_Lib::Exception::Destructor_Not_Supported();

	// TODO: Need to cycle through the list and remove this item from the list.

	m_next = END_OF_RESET_LIST;
	m_reset_ready_cb = reinterpret_cast<cback_func_t>( nullptr );
	m_reset_ready_handle = reinterpret_cast<cback_param_t>( nullptr );
	// Delete the CR Task
	// delete m_cr_task;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void System_Reset::Reset_Device( reset_select_t reset_cmd )
{
	if ( m_reset_id == NO_RESET_REQUESTED_ID )
	{
		m_reset_id = reset_cmd;
		m_reset_pending_cb_list = END_OF_RESET_LIST;

		m_cr_task->Resume();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void System_Reset::Reset_Waiting_Task( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
{
	bool reset_ready;
	System_Reset* next_reset_cb;

	BF_Lib::Unused<CR_Tasker*>::Okay( cr_task );
	BF_Lib::Unused<CR_TASKER_PARAM>::Okay( handle );

	if ( ( m_reset_ready_cb_list == END_OF_RESET_LIST ) &&
		 ( ( m_reset_pending_cb_list == END_OF_RESET_LIST ) ) )
	{
		if ( m_reset_id == WATCHDOG_TEST_RESET )
		{
			uC_Reset::Watchdog_Reset();
		}
		else
		{
			uC_Reset::Soft_Reset();
		}
	}
	else if ( m_reset_ready_cb_list != END_OF_RESET_LIST )
	{
		while ( m_reset_ready_cb_list != END_OF_RESET_LIST )
		{
			reset_ready = m_reset_ready_cb_list->m_reset_ready_cb(
				m_reset_ready_cb_list->m_reset_ready_handle, m_reset_id );

			next_reset_cb = m_reset_ready_cb_list->m_next;

			if ( reset_ready == false )
			{
				m_reset_ready_cb_list->m_next = m_reset_pending_cb_list;
				m_reset_pending_cb_list = m_reset_ready_cb_list;
			}

			m_reset_ready_cb_list = next_reset_cb;
		}

		m_reset_success_time_sample = BF_Lib::Timers::Get_Time();
	}
	else
	{
		reset_ready = m_reset_pending_cb_list->m_reset_ready_cb(
			m_reset_pending_cb_list->m_reset_ready_handle, m_reset_id );

		if ( BF_Lib::Timers::Expired( m_reset_success_time_sample,
									  RESET_SUCCESS_TIMEOUT_TIME ) ||
			 ( reset_ready == true ) )
		{
			m_reset_pending_cb_list = m_reset_pending_cb_list->m_next;
			m_reset_success_time_sample = BF_Lib::Timers::Get_Time();
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool System_Reset::Is_Reset_Pending( void )
{
	bool reset_pending;

	if ( m_reset_id != NO_RESET_REQUESTED_ID )
	{
		reset_pending = true;
	}
	else
	{
		reset_pending = false;
	}

	return ( reset_pending );
}

}
