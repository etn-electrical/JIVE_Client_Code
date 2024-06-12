/**
 *****************************************************************************************
 *	@file Log_Obj.h
 *
 *	@brief A file which contains a part of the Log functionality. Module responsible
 *	capturing log events on manual trigger.
 *	@n @b Definitions:
 *	@copyright 2017 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef LOG_OBJ_H
	#define LOG_OBJ_H

#include "Includes.h"
#include "DCI_Defines.h"
#include "NV_Mem.h"
#include "DCI_Owner.h"
#include "Log_Mem.h"
#include "Log_DCI.h"

namespace BF_Misc
{
class Log_Obj
{
	public:
		/**
		 *  @brief Constructor
		 *  @details Log_Obj class shall be intantiated only when log event need to be captured on manual trigger
		 *	@param log_dci_handle: Instance of Log_DCI required to get DCI data and add an entry to the log.
		 *	@param logging_event_code_owner: Owner for DCI_LOGGING_EVENT_CODE
		 */

		Log_Obj( Log_DCI* log_dci_handle, DCI_Owner* logging_event_code_owner );

		/**
		 *  @brief Capture the log event on manual trigger
		 *  @param log_event_code: Logging event code
		 */
		void Capture_Log_Event( log_event_t log_event_code );

		/**
		 *  @brief Max trial attempts constant for a logging event.
		 */
#ifndef MAX_MEM_OPS_FAIL_ATTEMPTS_PER_EVENT
		static const uint8_t MAX_MEM_OPS_FAIL_ATTEMPTS_PER_EVENT = 3U;
#endif

		/**
		 *  @brief Max attempts trial constant to trip "capture of logs" in NV when same NV
		 *   is having the issue in writing data. This scenario could lead to a watchdog/stack overflow
		 *   situation due to infinite recursive operation,
		 *   when NV memory fail operation is being captured in form log into same memory and fails always
		 *   the max fail counter will restrict this to a defined number of failed trials only.
		 */
#ifndef MAX_MEM_OPS_FAIL_ATTEMPTS_TOTAL
		static const uint8_t MAX_MEM_OPS_FAIL_ATTEMPTS_TOTAL = 8U;
#endif

	private:
		Log_DCI* m_log_dci_ptr_handler;
		DCI_Owner* m_logging_event_code_owner;
		uint8_t m_mem_op_fail_total_count;
		// bool m_event_capture_in_progress;
};

}
#endif	/* End for LOG_OBJ_H */
