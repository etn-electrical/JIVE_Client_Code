/**
 **********************************************************************************************
 * @file            uC_PWR.h Header File for Power Down Driver implementation.
 *
 * @brief           The file contains uC_PWR Class required for accessing Power Down Mode
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef uC_PWR_H
   #define uC_PWR_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "CR_Tasker.h"
#include "Change_Tracker.h"
/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_PWR
{
	public:
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		uC_PWR( void );
		~uC_PWR();

		/**
		 * @brief Configure the microcontroller to go in to Sleep mode.
		 * @param power_sleep_entry - specifies if SLEEP mode in entered with
		 * WFI or WFE instruction.
		 */
		void Enter_PowerDown_SleepMode( uint8_t power_sleep_entry );

		/**
		 * @brief Configure the microcontroller to go in to Stop mode.
		 * @param PWR_Regulator - specifies the regulator state in STOP mode.
		 * @param power_sleep_entry - specifies if STOP mode in entered with WFI or WFE instruction.
		 */
		void Enter_PowerDown_StopMode( uint32_t PWR_Regulator, uint8_t power_sleep_entry );

		/**
		 * @brief Enables or disables the WakeUp Pin functionality..
		 * @param PWR_WakeUpPin - specifies the WakeUpPin.
		 * @param pinState - new state of the WakeUp Pin functionality.
		 */
		void Configure_Wakeup_Pin( uint32_t PWR_WakeUpPin, uint8_t pinState );

		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param );

		static const uint8_t WAIT_FOR_INTERRUPT = 0U;
		static const uint8_t WAIT_FOR_EVENT = 1U;

	private:
		cback_func_t m_cback_func;
		cback_param_t m_cback_param;
};

#endif
