/**
 *****************************************************************************************
 *  @file
 *	@brief Alarm Rule class definition .
 *
 *	@details of all the necessary API to run Alarm Rule .
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 **/

#ifndef _RULE_H_
#define _RULE_H_

#include "Event_Config.h"
#include "Event_Msg_struct.h"

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
/**
 ****************************************************************************************
 * @Rule: This is the rule class to define the alarms for the different parameters
 * 			It is communicated with the alarm Manager with the specific API to set ,
 * 			unset ,enable and disable the alarm
 * @ Using this class created the alarm rule name as Fault_Overvoltage_Rule.cpp as an
 * example
 *
 ****************************************************************************************
 */
class Rule
{
	public:
		enum rule_state_t
		{
			AE_IDLE,
			AE_ENABLE,
			AE_ACTIVE,
			AE_DISABLE,
			AE_ERROR
		};

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members,
		 * configuration members initialized appropriately.
		 */
		Rule( void )
		{}

		~Rule( void );

		/**
		 * @Function to enable the alarm.
		 * @return rule_op_status_t : rule status accept or reject
		 *
		 */
		virtual rule_op_status_t Enable_Rule( void ) = 0;

		/**
		 * @ Function to disable the alarm
		 * @return rule_op_status_t : rule status accept or reject
		 *
		 */
		virtual rule_op_status_t Disable_Rule( void ) = 0;

		/*
		 * @ Function to set the alarm  .
		 * @ param[in] th_value : THreshold value to be monitor for alarms
		 * @ return rule_op_status_t
		 */
		virtual rule_op_status_t Update_Value( const void* th_value );

};



#endif	/* _RULE_H_ */
