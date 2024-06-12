/**
 **********************************************************************************************
 *  @file           uC_Reset.h Header File for microcontroller reset functionality
 *
 *  @brief          The file shall wrap all the functions which are required for the
 *                  resetting the microcontroller
 *  @details
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef uC_RESET_H
#define uC_RESET_H

#include "uC_Base.h"
/** @brief Reset Trigger Flags */
enum
{
	/** @brief Power up Trigger */
	RESET_POWER_UP_TRIGRD,
	/** @brief Wakeup Trigger */
	RESET_WAKEUP_TRIGRD,
	/** @brief Watchdog Trigger */
	RESET_WATCHDOG_TRIGRD,
	/** @brief Software Trigger */
	RESET_SOFTWARE_TRIGRD,
	/** @brief External Pin Trigger */
	RESET_USER_PIN_TRIGRD,
	/** @brief Brown Out Trigger */
	RESET_BROWN_OUT_TRIGRD,
	/** @brief Maximum */
	RESET_UNDEFINED
};

/** @brief Typedef for cause of the last reset */
typedef uint8_t uC_RESET_CAUSE_TD;

/**
 * @brief                   uC_Reset Class Declaration. The class will include all the
 *                          features of the microcontroller Reset functionality
 */
class uC_Reset
{
	public:
		/**
		 * @brief                    Constructor to create an instance of uC_Reset class.
		 * @note                     When the object is created using this constructor, the constructor
		 *                           finds out the cause of last microcontroller reset by reading the
		 *                           microcontroller status register and update the reset cause in
		 *                           global variable "m_reset_id"
		 */
		uC_Reset( void );

		/**
		 * @brief                   Destructor to delete the uC_Reset instance when it goes out of scope
		 */
		~uC_Reset()
		{}

		/**
		 * @brief                   The function puts the controller in soft reset condition.
		 *                          Integrator should use this function when he wants to perform soft
		 *                          reset
		 * @note
		 */
		static void Soft_Reset( void );

		/**
		 * @brief                   This function envokes watchdog reset
		 * @note
		 */
		static void Watchdog_Reset( void );

		/**
		 * @brief                   Function returns the cause of the last reset
		 * @return[out] uC_RESET_CAUSE_TD Each bit of this return variable holds significance.
		 * @n                       Bit 0 : Power On Reset
		 * @n                       Bit 1 : Not applicable
		 * @n                       Bit 2 : Watchdog Reset
		 * @n                       Bit 3 : Software Reset
		 * @n                       Bit 4 : Brown Out Reset
		 * @n                       Bit 5 : Reset via external user defined pin
		 * @note
		 */
		static uC_RESET_CAUSE_TD Get_Reset_Trigger( void );

		/**
		 * @brief                   This function envokes watchdog reset
		 * @note                    It put the system/micro in exception state and triggers watchdog reset
		 *                          Do not use this if you dont want to put micro in exception state.
		 *
		 */
		static void Periph_Reset( void );

	private:
		/** @brief The variable which hold the status of last reset.*/
		static uC_RESET_CAUSE_TD m_reset_id;

};

#endif
