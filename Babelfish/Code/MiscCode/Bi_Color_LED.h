/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Bi_Color_LED is basically used to serve two purposes with the two colors,
 *	example: Red implies stand by, and Green implies On state.
 *
 *	@details Controls the Bi_Color_LED operations.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BI_COLOR_LED_H
#define BI_COLOR_LED_H

#include "Includes.h"
#include "Timers_Lib.h"
#include "LED.h"

namespace BF_Misc
{

/**
 **************************************************************************************************
 *  @brief The Bi_Color_LED contains the Bi_Color_LED usage functionality.
 *  @details Controls a bi color LED like On, Off, Flashing with particular intervals.
 *  @n @b Usage:
 *  @n @b 1. For flashing, setting On and Off the Bi Color LEDs.
 *  @n @b 2. E.g. of usage could be Red implies stand by, and Green implies On state.
 **************************************************************************************************
 */
class Bi_Color_LED
{
	public:

		/// The two colors for the Bi-color LED and one combined color.
		enum bi_color_state_t
		{
			COLOR_ONE,
			COLOR_TWO,
			COMBINED_COLOR,
		};

		/**
		 *  @brief Bi Color LED Function.
		 *  @details This function controls a dual color LED. Basically initializes the pointers with the two LED
		 * addresses.
		 *  @param[in] led_one: First LED control structure.
		 *  @param[in] led_two: Second LED control structure.
		 */
		Bi_Color_LED( BF_Misc::LED* led_one, BF_Misc::LED* led_two );

		/**
		 *  @brief Bi Color LED Function.
		 *  @details This function controls a dual color LED. Basically initializes the pointers with the two LED
		 * addresses.
		 *  @param[in] led_one: First LED control structure.
		 *  @param[in] led_two: Second LED control structure.
		 */
		~Bi_Color_LED( void );

		/**
		 *  @brief Used to have the LEDs flashed.
		 *  @details For a color it sets and then clears the status of LED. For the combined color,
		 *  it sets the first color and then the next color.
		 *  @param[in] color: Either of the available color or the combined-color.
		 *  @return None
		 */
		void On( bi_color_state_t color );

		/**
		 *  @brief Used to switch off the bi-color LEDs.
		 *  @return None
		 */
		void Off( void );

		/**
		 *  @brief Perform the flash operation on the LEDs.
		 *  @details Flash operation checks if the timer is dead,if yes and led is off, then depending on the initial
		 * state,
		 *  it updates the status of the timer and led. If the flash off time is set to 0 then it will only flash for
		 *  the on time then disable (SINGLE_FLASH_OFF_TIME).
		 *  @param[in] color: The particular LED either say Red or Green.
		 *  @param[in] on_time: The time for which we shall use the flash on.
		 *  @param[in] off_time: The time for which we shall keep the LEDs off.
		 *  @param[in] initial_state: The initial state of the LED.
		 *  @return None
		 */
		void Flash( bi_color_state_t color, BF_Lib::Timers::TIMERS_TIME_TD on_time,
					BF_Lib::Timers::TIMERS_TIME_TD off_time, bool initial_state = false );

		/**
		 *  @brief Perform the flash operation on the LEDs for the same on and off times.
		 *  @details Flash operation checks if the timer is dead,if yes and led is off, then depending on the initial
		 * state,
		 *  it updates the status of the timer and led. The time interval for on and off is same.
		 *  @param[in] color: The particular LED either say Red or Green.
		 *  @param[in] interval: The time for which we shall use the flash on or off.
		 *  @param[in] initial_state: The initial state of the LED.
		 *  @return None
		 */
		void Flash( bi_color_state_t color, BF_Lib::Timers::TIMERS_TIME_TD interval,
					bool initial_state = false );

		/**
		 * @brief Will trigger an LED test.
		 * @details The LED test timing is controlled by the LED module.
		 * All additional changes will be held off and applied after the
		 * LED test is complete.
		 * @param[in] time_test:  Time in milliseconds for the LED Test to run.
		 */
		static void Test_All( BF_Lib::Timers::TIMERS_TIME_TD time_test );

	private:
		Bi_Color_LED( const Bi_Color_LED& rhs );
		Bi_Color_LED & operator =( const Bi_Color_LED& object );

		static void Test_Handler( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

		/**
		 * @brief Will trigger an LED test based on a call from the LED class.
		 * @details The LED test timing is controlled by the LED module.
		 * All additional changes will be held off and applied after the
		 * LED test is complete.
		 * @param[in] param:  Unused because this function is static.
		 * @param[in] time_test:  Time in milliseconds for the LED Test to run.
		 */
		static void Test_All_LED_Cback( BF_Misc::LED::cback_param_t param,
										BF_Lib::Timers::TIMERS_TIME_TD time_test )
		{
			Test_All( time_test );
		}

		static Bi_Color_LED* const LED_LIST_END;
		static Bi_Color_LED* m_led_list;
		static BF_Lib::Timers* m_test_timer;
		static BF_Misc::LED::led_test_cback_t m_led_test_cback;

		BF_Misc::LED* m_color_one_ctrl;
		BF_Misc::LED* m_color_two_ctrl;
		Bi_Color_LED* m_next;

		static const uint8_t STORED_LED1_STATE_ON_BIT = 0;
		static const uint8_t STORED_LED2_STATE_ON_BIT = 1;
		static const uint8_t FLASH_1_ON_BIT = 2;
		static const uint8_t FLASH_2_ON_BIT = 3;
		uint8_t m_ctrl_bits;
};

}
#endif
