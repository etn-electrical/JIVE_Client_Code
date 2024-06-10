/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef SYSTEM_RESET_H
	#define SYSTEM_RESET_H

#include "CR_Tasker.h"
#include "Linked_List.h"
#include "Timers_Lib.h"

/*
 **************************************************************************************************
 *  Constants
   --------------------------------------------------------------------------------------------------
 */

namespace BF
{
/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class System_Reset
{
	public:
		/// Type of resets allowed.
		enum reset_select_t
		{
			SOFT_RESET,				///< This reset will simply do a hardware reset.
			FACTORY_RESET,				///< This reset brings the device to out of box state.
			APP_PARAM_RESET,			///< This reset service will bring all application values to defaults.  The
										///< application parameters are defined in the app column of the spreadshet.
			WATCHDOG_TEST_RESET,		///< This reset will sit in a loop until the watchdog catches it.
			NO_RESET_REQUESTED_ID		///< This is used as a flag for when the system reset machine is idle
		};

		/**
		 * Callback function for reset handling.  This function callback is called by system_reset
		 * until it returns true.  Once all function callbacks return true, system_reset will trigger
		 * the actual firmware reset.
		 */
		typedef void* cback_param_t;
		typedef bool (* cback_func_t)( cback_param_t handle, reset_select_t reset_req );

		/**
		 * Each task passed into this constructor will be attached to a linked list of functions
		 * to call on a reset request.  When the function returns true, System_Reset will remove
		 * the function from the linked list until all functions are removed.  At this point the
		 * code will trigger the reset.  The passed in callbacks are told what reset was requested
		 * and they are responsible for indicating whether or not they are ready to be reset.
		 * @param
		 */
		System_Reset( cback_func_t reset_ready_cb, cback_param_t reset_ready_handle );

		~System_Reset( void );

		/**
		 * This function causes the reset to happen.  This function does not block.  It will continue on
		 * and trigger the linked list mentioned in the constructor.
		 */
		static void Reset_Device( reset_select_t reset_cmd );

		static bool Is_Reset_Pending( void );

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		System_Reset( const System_Reset& rhs );
		System_Reset & operator =( const System_Reset& object );

		static void Reset_Waiting_Task( CR_Tasker* cr_task, CR_TASKER_PARAM handle );

		/// A default timeout for each individual element in the reset list.
		static const BF_Lib::Timers::TIMERS_TIME_TD RESET_SUCCESS_TIMEOUT_TIME = 1000U;
		static BF_Lib::Timers::TIMERS_TIME_TD m_reset_success_time_sample;

		// Indication of the end of a linked list of resets.
		static System_Reset* const END_OF_RESET_LIST;

		static reset_select_t m_reset_id;
		static System_Reset* m_reset_ready_cb_list;
		static System_Reset* m_reset_pending_cb_list;
		static CR_Tasker* m_cr_task;

		System_Reset* m_next;
		cback_func_t m_reset_ready_cb;
		cback_param_t m_reset_ready_handle;
};

}

#endif
