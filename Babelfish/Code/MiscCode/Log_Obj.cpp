/**
 **********************************************************************************************
 *	@file            Log_Obj.cpp
 *
 *	@brief           Implementation for different log type initialization.
 *	@copyright       2018 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Log_Obj.h"

namespace BF_Misc
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

Log_Obj::Log_Obj( Log_DCI* log_dci_handle, DCI_Owner* logging_event_code_owner ) :
	m_log_dci_ptr_handler( log_dci_handle ),
	m_logging_event_code_owner( logging_event_code_owner ),
	// m_event_capture_in_progress( false ),
	m_mem_op_fail_total_count( 0U )
{}


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_Obj::Capture_Log_Event( log_event_t log_event_code )
{
	Log_Mem::error_t return_status = Log_Mem::NO_ERROR;
	uint8_t retry_count = MAX_MEM_OPS_FAIL_ATTEMPTS_PER_EVENT;

	/* if Memory write is failing again and again, lets skip logging till next power cycle if it reaches
	   MAX_MEM_OPS_FAIL_ATTEMPTS_TOTAL */
	// if ( ( m_event_capture_in_progress == false ) && ( m_mem_op_fail_total_count < MAX_MEM_OPS_FAIL_ATTEMPTS_TOTAL )
	// )
	if ( m_mem_op_fail_total_count < MAX_MEM_OPS_FAIL_ATTEMPTS_TOTAL )
	{
		/* While capturing event if log mem write failed and generated another log mem failure event then lets skip
		   capturing again. This will avoid a stack overflow or infinite loop like situation. */
		// m_event_capture_in_progress = true;
		do
		{
			if ( m_logging_event_code_owner != nullptr )
			{
				m_logging_event_code_owner->Check_In( log_event_code );
			}
			return_status = ( m_log_dci_ptr_handler )->Trigger();
			retry_count--;
		} while ( ( return_status != Log_Mem::NO_ERROR ) && ( retry_count > 0U ) );

		// m_event_capture_in_progress = false;

		if ( return_status == Log_Mem::NO_ERROR )
		{
			m_mem_op_fail_total_count = 0U;
		}
		else
		{
			// Memory write is failing, increase counter.
			m_mem_op_fail_total_count++;
		}
	}
}

}// End of namespace BF_Misc
