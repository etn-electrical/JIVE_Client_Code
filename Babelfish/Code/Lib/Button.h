/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This contains button class that implements the functionality associated after the button
 * pressed.
 *
 *	@details This handles the functionality after there is a button pressed from
 *	on a module.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BUTTON_H_
#define BUTTON_H_

#include "Includes.h"
#include "Input.h"
#include "CR_Tasker.h"
#include "DCI_Owner.h"
#include "Timers_Lib.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is the Button class containing functionality to handle the input
 * at the button.
 *
 * @details
 * @n @b Usage: To get the state of the button.
 * @n @b Usage: To set the button input, debounce time and other button parameters.
 *
 ****************************************************************************************
 */
class Button
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Initializes the data members with input, button owner, debounce time
		 *  and other button parameters. Also initializes the CR Tasker.
		 *  @param[in] input: The port pin where the input is connected.
		 *  @param[in] button_owner: TODO
		 *  @param[in] changes_owner: TODO
		 *  @param[in] debounce_time: The debounce time after the button press.
		 *  @return None
		 */
		Button( Input* input, DCI_Owner* button_owner,
				DCI_Owner* changes_owner = reinterpret_cast<DCI_Owner*>( nullptr ),
				uint32_t debounce_time = DEFAULT_DEBOUNCE_TIME );

		/**
		 *  @brief Destructor for the Button Module.
		 *  @details This will get called when object of Button goes out of scope.
		 */
		virtual ~Button( void )
		{}

		/**
		 *  @brief
		 *  @details To get the state of the button.
		 *  @return The status of the button, debounced or not. True implies debounced.
		 */
		bool Get_State( void ) const;

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Button( const Button& rhs );
		Button & operator =( const Button& object );

		void Update_Button();

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Update_Button_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
		{
			( ( Button* )param )->Update_Button();
		}

		Input* m_input;
		DCI_Owner* m_button_owner;
		DCI_Owner* m_changes_owner;
		uint32_t m_debounce_time;
		BF_Lib::Timers::TIMERS_TIME_TD m_change_time;
		bool m_state;
		bool m_debounced_state;
		static const uint32_t DEFAULT_DEBOUNCE_TIME = 10U;

};

}

#endif
