/**
 *****************************************************************************************
 *  @file
 *	@brief This file provides the LED indication interface with Modbus protocols
 *
 *  @details LED indication for Modbus protocol
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_DISPLAY_H
#define MODBUS_DISPLAY_H

#include "LED.h"
#include "Timers_Lib.h"

namespace BF_Mbus
{
/**
 ****************************************************************************************
 * @brief This is a Modbus_Display class
 * @details It provides an LED indication for Modbus protocol,
 * @n @b Usage: handle the flashing behavior
 * It provides public methods to
 * @n @b 1. Turn on LED
 * @n @b 2. Turns LED solid off
 ****************************************************************************************
 */
class Modbus_Display
{
	public:

		Modbus_Display();
		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus display
		 *  @param[in] led_ctrl shall control LED on/off behavior
		 *  @n @b Usage: Will handle the flashing and on/off behavior
		 *  @return None
		 */
		Modbus_Display( BF_Misc::LED* led_ctrl );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Modbus_Display goes out of scope
		 *  @return None
		 */
		virtual ~Modbus_Display( void );

		/**
		 *  @brief Turn on LED.
		 *  @details Begins the flash function for the LED.  The timing is all controlled internally.
		 *  If the function is called repeatedly the flash operation will not be interrupted.
		 *  @param[in] None
		 *  @return None
		 */
		virtual void Bus_Active( void );

		/**
		 *  @brief Turns LED solid off.
		 *  @details this function will make modbus indication LED solid off
		 *  @param[in] None
		 *  @return None
		 */
		virtual void Bus_Idle( void );

		/// below public functions are not yet defined
		void Device_Faulted( void ) const;

		void Device_Ok( void ) const;

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Modbus_Display( const Modbus_Display& object );
		Modbus_Display & operator =( const Modbus_Display& object );

		BF_Misc::LED* m_led_control;

		static const BF_Lib::Timers::TIMERS_TIME_TD LED_ON_TIME = 50U;
};

}

#endif	/* MODBUS_DISPLAY_H */
