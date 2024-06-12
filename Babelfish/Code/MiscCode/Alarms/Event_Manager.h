/**
 *****************************************************************************************
 *  @file
 *	@brief event manager class definition .
 *
 *	@details of all the necessary API to run event manager .
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 **/

#ifndef _EVENT_MANAGER_H_
#define _EVENT_MANAGER_H_

#include "Includes.h"
#include "Event_Config.h"
#include "Event_Msg_Struct.h"
#include "Bit_List.h"
#include "Event_Defines.h"
#include "OS_Tasker.h"

namespace BF_Event
{
using namespace BF_Lib;
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */

class Event_Manager
{
	public:
		static const status_bitmask_t ACTIVE_BIT = ( 1 << 1 );			// Active Bit
		static const status_bitmask_t CLEAR_ACTIVE_BIT = 0x0D;			// Clear Active Bit
		static const status_bitmask_t LATCHED_BIT = ( 1 << 2 );		// Latched Bit
		static const status_bitmask_t CLEAR_LATCHED_BIT = 0x0B;		// Clear Latched Bit
		static const status_bitmask_t UNACKED_BIT = ( 1 << 0 );		// Un acknowledgment Bit
		static const status_bitmask_t CLEAR_UNACKED_BIT = 0x0E;		// Clear Un acknowledgment Bit
		static const status_bitmask_t ENABLED_BIT = ( 1 << 3 );		// Enable Bit
		static const status_bitmask_t CLEAR_ENABLED_BIT = 0x07;		// Clear Enable Bit
		static const status_bitmask_t RESET_EVENT_STATUS = 0x08;		// Reset event Status

		/*
		 *****************************************************************************************
		 *		Typedefs
		 *****************************************************************************************
		 */
		/**
		 * @brief status type of the events
		 */
		enum status_type_t
		{
			ACTIVATED,
			CLEARED,
			UNLATCHED,
			ACKNOWLEDGED
		};

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members,
		 * configuration members initialized appropriately.
		 * @param[in] event_config : Rules/ events configuration list
		 * @param[in] event_interface_cb_t : Attached callback to raise event
		 */

		Event_Manager( rule_id_t rule_count, event_interface_cb_t new_event_cb );

		/** Default destructor for this class. */
		~Event_Manager();

		/*  event Generation side API */
		/*
		 * @ Set the event in a bit list .
		 * @ This sets the queue for actually activating the active bit.
		 * @ param[in] rule_index: event index
		 * @return None
		 */
		void Set_Active( rule_id_t rule_index );

		/*
		 * @ Clear the event in a bit list .
		 * @ This sets the queue for actually clear the active bit.
		 * @ param[in] rule_index: event index
		 * @return None
		 */
		void Set_Inactive( rule_id_t rule_index );

		/*
		 * @ Enable the event in a bit list .
		 * @ This sets the queue for actually Enable the active bit.
		 * @ param[in] rule_index: event index
		 * @ param[in] is_latched : Rules are defined as latched/ unlatched in rules configuration
		 * @return None
		 */
		void Enable( rule_id_t rule_index, bool is_latched );

		/*
		 * @ Disable the event in a bit list .
		 * @ This sets the queue for actually Disabling the active bit.
		 * @ param[in] rule_index: event index
		 * @return None
		 */
		void Disable( rule_id_t rule_index );

		/*  Protocol Interface side API */
		/*
		 * @ Get the event if any active  .
		 * @ param[in] rule_index: Rule index
		 * @return status event if any
		 */
		status_bitmask_t Status( rule_id_t rule_index );

		/*
		 * @ reset the event, whether it is latched then check of clear event else reset event life cycle.
		 * @ for reset all, its forcefully resetd the event life cycle
		 * @ param[in] rule_index: rule id need to be reset
		 * @ return operation status
		 */
		event_op_status_t Reset( rule_id_t rule_index = MAX_RULES );

		/*
		 * @ Acknowledges an Unacknowledged event.
		 * @ param[in] rule_index: rule id need to be ack
		 * @ return operation status
		 */
		event_op_status_t Ack( rule_id_t rule_index );

		/*
		 * @ Get Open/ active event list.
		 * @ return count of active event
		 */
		active_event_t Get_Active_Event_Count( void );

		/*
		 * @ This is the used to clear all flags and bit list of events
		 * @ param[in] none
		 * @ return None
		 */
		void Clear_Events( void );

	private:
		event_interface_cb_t m_new_event_cb;
		rule_id_t m_rule_count;
		OS_Task* m_event_tasker;
		/* Semaphore to halt other threads which need ssl_context */

		// The following bitlists are for incoming behaviors.
		Bit_List* m_set_q;
		Bit_List* m_clr_q;
		Bit_List* m_enable_q;
		Bit_List* m_disable_q;
		Bit_List* m_islatched_q;

		// The following are from the application side
		Bit_List* m_reset_q;
		Bit_List* m_ack_q;

		// The following are for internal tracking.
		Bit_List* m_active;
		Bit_List* m_latched;
		Bit_List* m_unacked;
		Bit_List* m_enabled;

		/*
		 * @ Wakeup the event scanning functionality if any input generated from Rule
		 * @return None
		 */
		void Wakeup_Run( void );

		/*
		 * @ CR tasker for the event handler
		 * @param[in] : None .
		 * @return : None
		 */
		void Event_Handler_Task( void );

		/*
		 * @ Static CR tasker for the event handler
		 * @param[in] os_param: A  pointer to an object passed as an argument OS tasker handler .
		 * @return : None
		 */
		static void Event_Handler_Static( OS_TASK_PARAM os_param )
		{
			( ( Event_Manager* )os_param )->Event_Handler_Task();
		}

};

}

#endif	/* _EVENT_MANAGER_H_ */
