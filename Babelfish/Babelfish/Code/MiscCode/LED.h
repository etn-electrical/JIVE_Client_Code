/**
 **********************************************************************************************
 *  @file            LED.h Header File for LED functions
 *
 *  @brief           The file shall wrap all the functions which are required for controlling
 *                   LEDs which are interfaced with GPIO pins of microcontroller
 *  @details
 *  @copyright       2014 Eaton Corporation. All Rights Reserved.
 *  @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef LED_H
#define LED_H

#include "Includes.h"
#include "Output.h"
#include "Timers_Lib.h"
#include "Bit.hpp"

namespace BF_Misc
{
/**
 *  @brief   This module has the capability to control basic LED functions.
 *
 *  @details LED control provides the following functions:
 *           1. Will handle LED test functions when requested.
 *           2. Will handle the flashing and on/off behavior.
 *
 */
class LED
{
	public:
		/**
		 * @brief                   Constructor to create instance of LED class.
		 * @param[in] output_ctrl   The output control which is attached to an LED.
		 * @param[in] led_test_include If true this LED will be included in the LED test
		 *                          which is automatically controlled by this module.
		 *                          If false then the LED test functionality needs to be
		 *                          handled outside by the application.
		 * @return
		 */
		LED( BF_Lib::Output* output_ctrl, bool led_test_include = true );

		/**
		 * @brief Destructor to delete the LED instance when it goes out of scope
		 */
		~LED( void );

		/**
		 * @brief                   Turns LED solid on.
		 */
		void On( void );

		/**
		 * @brief                   Turns LED solid off.
		 */
		void Off( void );

		/**
		 * @brief                   Sets LED to passed in state.
		 * @param[in] state         The desired LED state.Two possible values
		 * @n                       TRUE - Turn LED ON
		 * @n                       FALSE -  Turn Off LED
		 * @note                    Internally it calls On() and Off() functions
		 *
		 */
		void Set_To( bool state );

		/**
		 * @brief                   State indication for the LED i.e. either ON or OFF
		 * @return bool type        Returns the state of the LED.Two Possible values
		 * @n                       TRUE - LED is ON
		 * @n                       FALSE - LED is OFF
		 */
		bool Is_On( void )
		{
			return ( m_output_ctrl->State() );
		}

		/**
		 * @brief                   Toggles the state of the LED. It turn ON the LED
		 *                          if it was in OFF state earlier and vice versa.
		 */
		void Toggle( void );

		/**
		 * @brief                   If set to false the LEDs will be excluded from
		 *                          any future LED tests or vice versa.
		 * @details                 The LED test will take control of the LED from
		 *                          whatever is currently using it.  This is a feature
		 *                          to reduce workload on the LED users. In this case
		 *                          if the LED module user wants to take care of it
		 *                          themselves they can exclude it from the normal LED control.
		 * @param[in] include       Whether to include this LED control from the test or not.
		 */
		void Include_In_LED_Test( bool include );

		/**
		 * @brief                   Triggers an LED test.
		 * @details                 The LED test timing is controlled by the LED module.
		 *                          All additional changes will be held off and applied
		 *                          after the LED test is complete.  If you intend to do
		 *                          an LED test at power-up, it is strongly suggested that
		 *                          you create all the necessary LED controls before the
		 *                          LED test called.
		 * @param[in] time_test     Time in milliseconds for the LED Test to run.
		 */
		static void Test_All( BF_Lib::Timers::TIMERS_TIME_TD time_test );

		/**
		 * @brief                   These callbacks will provide the ability for an LED
		 *                          which has overridden the LED test to get a callback
		 *                          when a test is activated.
		 */
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param,
									   BF_Lib::Timers::TIMERS_TIME_TD time_test );

		/**
		 * @brief                   Structure to provide a class to get a call when an
		 *                          LED test is commanded.
		 * @details                 To provide external LED controls with a that a test
		 *                          is occurring, the LED test can call a holder of an LED.
		 *                          From there it is up to the other module to handle the
		 *                          LED test appropriate for its definition.
		 */
		struct led_test_cback_t
		{
			/** @brief The function to call when an LED test is to be executed */
			cback_func_t func;
			/** @brief The parameter to pass back (typically a this) */
			cback_param_t param;
			/** @brief This parameter shall not be touched by the callbackee */
			led_test_cback_t* next;
		};

		/**
		 * @brief                   Provides the ability for an outside party to attach
		 *                          a callback for a test request.
		 * @param[in] callback_struct  The callback structure.
		 * @return    None
		 */
		static void Attach_Test_Handler( led_test_cback_t* callback_struct );

		/**
		 * @brief Helps support the single flash capability of the LED Flash function.
		 */
		static const BF_Lib::Timers::TIMERS_TIME_TD SINGLE_FLASH_OFF_TIME = 0;

		/**
		 * @brief                   Begins the flash function for the LED.  The timing is
		 *                          all controlled internally. If the function is called
		 *                          repeatedly the flash operation will not be interrupted.
		 *                          The new times will be applied in the next round of flashes.
		 * @param[in] on_time       Sets the on time for the LED flash. This will be repeated
		 *                          unless off time is set to the single flash off time above.
		 *                          Enter here time in milliseconds.
		 * @param[in] off_time      The off time for for the LED flash. If the flash off time is
		 *                          set to 0,then it will only flash for the on time then disable
		 *                          (SINGLE_FLASH_OFF_TIME).Enter her time in milliseconds.
		 * @param[in] initial_state Whether the LED is on or off initially on the beginning of
		 *                          the flash functionality.
		 */
		void Flash( BF_Lib::Timers::TIMERS_TIME_TD on_time,
					BF_Lib::Timers::TIMERS_TIME_TD off_time, bool initial_state = false );

		/**
		 * @brief                   Begins the flash function for the LED. The timing is all
		 *                          controlled internally.If the function is called repeatedly
		 *                          the flash operation will not be interrupted. The new time
		 *                          will be applied in the next round of flashes.
		 * @param[in] interval      Sets the interval for the LED flash.
		 * @param[in] initial_state Whether the LED is on or off initially on the beginning of
		 *                          the flash functionality.
		 */
		void Flash( BF_Lib::Timers::TIMERS_TIME_TD interval, bool initial_state = false );

		/**
		 * @brief                   Will resume the LED flash using the previous timing parameters.
		 */
		void Flash( void );

		/**
		 * The following constants are for backwards compatibility.
		 */
		static const bool EXCLUDE_FROM_LED_TEST = false;
		static const bool INCLUDE_IN_LED_TEST = true;

	private:
		LED( const LED& rhs );
		LED & operator =( const LED& object );

		/**
		 * @brief Deconstructor is not supported.
		 */

		static void Call_External_Test_Handlers( BF_Lib::Timers::TIMERS_TIME_TD time_test );

		static void Test_End( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

		void Flash_Task( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Flash_Task_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
		{
			( ( LED* )param )->Flash_Task();
		}

		static BF_Lib::Timers* m_test_timer;
		static LED* m_led_list;
		static led_test_cback_t* const CALLBACK_LIST_END;
		static led_test_cback_t* m_test_callbacks;
		static LED* const LED_LIST_END;

		static const uint8_t STORED_STATE_ON_BIT = 0;
		static const uint8_t FLASH_ON_BIT = 1;
		static const uint8_t LED_TEST_INCLUDE_BIT = 2;
		uint8_t m_ctrl_bits;

		BF_Lib::Output* m_output_ctrl;
		LED* m_next;
		BF_Lib::Timers* m_flash_timer;
		BF_Lib::Timers::TIMERS_TIME_TD m_on_time;
		BF_Lib::Timers::TIMERS_TIME_TD m_off_time;

};

}
#endif
