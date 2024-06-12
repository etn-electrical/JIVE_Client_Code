/**
 *****************************************************************************************
 *	 @file          uc_Watchdog_HW header file  for watchdog hardware functionality.
 *
 *	 @brief         This file shall wrap all the functions required for watchdog hardware functionality
 *
 *	 @details       The watchdog timer serves to detect and resolve malfunctions due to software
 *	               failure and to trigger system reset or an interrupt when the counter reaches
 *                  a given timeout value.
 *
 *	 @copyright      2014 Eaton Corporation. All Rights Reserved.
 *   @note          Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *
 *****************************************************************************************
 */
#ifndef uC_WATCHDOG_HW_H
#define uC_WATCHDOG_HW_H

#include "uC_Base.h"

/*
 **************************************************************************************************
 *  uc_WatchdogHW class declaration     This class includes all the functions used to reset the watchdog timer(hardware
 *reset).
 **************************************************************************************************
 */
class uC_Watchdog_HW
{
	public:
		/**
		 * @brief           This function is used to set the timeout value of the watchdog timer.
		 * @param[in]       uint32_t     timeout_in_ms.[input the timeout in ms e.g.1000]
		 * @return[out]     void     none
		 * @note            This function is used to set the timeout value of the downcounter by changing value of the
		 *prescaler index.
		 *                  The prescaling is done by changing the value loaded into the m_prescalers array from[000
		 *-111].
		                    The function also tests whether reset has occured.
		 */

		static void Set_Timeout( uint32_t timeout_in_ms );

		/**
		 * @brief           This function is used to reload count value.
		 * @note            This function is used to reload count value of0xAAAA into the key register of IWDG to
		 *restart the counter.
		 */
		static void Kick_Dog( void );

	private:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		uC_Watchdog_HW()
		{}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Watchdog_HW()
		{}

		void* operator new( size_t size );

		static const uint8_t m_prescalers_max;
		static const uint8_t m_prescalers[];

		static const uC_BASE_WDOG_CTRL_STRUCT* m_wd_ctrl;

};

#endif
