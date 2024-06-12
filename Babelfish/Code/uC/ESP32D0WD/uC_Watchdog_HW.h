/**
 *****************************************************************************************
 * @file			uc_Watchdog_HW header file for watchdog hardware functionality.
 *
 * @brief			This file shall wrap all the functions required for watchdog hardware functionality
 *
 * @details			The watchdog timer serves to detect and resolve malfunctions due to software
 *					failure and to trigger system reset or an interrupt when the counter reaches
 *					a given timeout value.
 *
 * @copyright		2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_WATCHDOG_HW_H
   #define uC_WATCHDOG_HW_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "uC_Base.h"
#include "esp_int_wdt.h"
#include "soc/timer_group_struct.h"

/*
 **************************************************************************************************
 * @brief				uc_WatchdogHW class declaration
 * @details				This class includes all the functions used to reset the watchdog timer(hardware reset).
 **************************************************************************************************
 */
class uC_Watchdog_HW
{
	public:
		/**
		 * @brief						This function is used to set the timeout value of the watchdog timer.
		 * @param[in] timeout_in_ms:	input the timeout in ms e.g.1000
		 * @return void					none
		 */
		static void Set_Timeout( uint32_t timeout_in_ms );

		/**
		 * @brief						This function is used to reload count value.
		 * @return void					none
		 */
		static void Kick_Dog( void );

	private:
		/**
		 * @brief						Constructor to create instance of uC_Watchdog_HW class.
		 * @return Void					None
		 */
		uC_Watchdog_HW( void )
		{}

		/**
		 * @brief						Destructor to delete an instance of uC_Watchdog_HW class
		 * @return Void					None.
		 */
		~uC_Watchdog_HW( void )
		{}

		/**
		 * @brief						Copy Constructor and Copy Assignment Operator.
		 * @definitions					made private to disallow usage.
		 */
		uC_Watchdog_HW( const uC_Watchdog_HW& hw_watchdog );
		uC_Watchdog_HW & operator =( const uC_Watchdog_HW& object );

		/**
		 * @brief						Private member variable for a uint8_t.
		 * @n							Prescalars Max value.
		 * @n							Prescalers array.
		 */
		static const uint8_t m_prescalers_max;
		static const uint8_t m_prescalers[];

		/**
		 * @brief						Private member variable of uC_BASE_WDOG_CTRL_STRUCT.
		 * @details						Watchdog control structure pointer.
		 */
		static const uC_BASE_WDOG_CTRL_STRUCT* m_wd_ctrl;
};

#ifdef __cplusplus
}
#endif

#endif
