/**
 **********************************************************************************************
 * @file            Rtc_If.cpp C++ RTC Interface  File for RTC Hardware module
 *
 * @brief           The class contains all APIs required for accessing stm internal RTC. This
 * interface create a
 *                  single instance of RTC and use HAL uC_RTC to access RTC module.
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef __RTC_IF_H__
#define __RTC_IF_H__

#include "Includes.h"
#include "uC_RTC.h"
#include "TimeCommon.h"

/**
 **********************************************************************************************
 * @brief        RTC_If class declaration
 * @n            This class controls all RTC specific operations such
 *               as initialize RTC, get and set date/time.
 **********************************************************************************************
 */
class Rtc_If : public uC_RTC
{
	public:
		/**
		 * @brief                       Constructor to create RTC Interface class instance.
		 *                              This constructor also initialises RTC hardware module.
		 * @param[in] clk_src           RTC clock source, Supports HSE, LSE, LSI
		 * @param[in] format            Supports binary or BCD format
		 */
		Rtc_If( RTC_clock_source_t clk_src = RTC_CLOCK_SOURCE,
				RTC_format_t format = RTC_FORMAT );

		/**
		 * @brief                       Destructor for RTC_If class object.
		 */
		~Rtc_If()
		{}

		/**
		 * @brief                       Function to get RTC time.
		 *                              gets time in BCD/BIN format depending upon uC_RTC
		 * constructor argument.
		 * @param[in] utc_time          utc_time will updated with RTC time in Time_Internal.
		 */
		static void Get_Time( Time_Internal& utc_time );

		/**
		 * @brief                       Function to get RTC date.
		 *                              gets date in BCD/BIN format depending upon uC_RTC
		 * constructor argument.
		 * @param[in] utc_date          utc_date will updated with RTC date in Time_Internal.
		 */
		static void Get_Date( Time_Internal& utc_date );

		/**
		 * @brief                       Function to get RTC date and time.
		 *                              gets date and time in BCD/BIN format depending upon uC_RTC
		 * constructor argument.
		 * @param[in] utc_date_time     utc_date_time will updated with RTC date and time in
		 * Time_Internal.
		 */
		static void Get_Date_Time( Time_Internal& utc_date_time );

		/**
		 * @brief                       This function can be used to initialize/sync RTC with
		 * required date.
		 *                              sets date in BCD/BIN format depending upon uC_RTC
		 * constructor argument.
		 * @param[in] curr_date     Date to be set into RTC.
		 */
		static void Set_Date( Time_Internal curr_date );

		/**
		 * @brief                       This function can be used to initialize/sync RTC with
		 * required time.
		 *                              sets time in BCD/BIN format depending upon uC_RTC
		 * constructor argument.
		 * @param[in] curr_time     Time to be set into RTC.
		 */
		static void Set_Time( Time_Internal curr_time );

		/**
		 * @brief                       This function can be used to initialize/sync RTC with
		 * required date and time.
		 *                              sets time in BCD/BIN format depending upon uC_RTC
		 * constructor argument.
		 * @param[in] curr_date_time     Time and Date to be set into RTC.
		 */
		static void Set_Date_Time( Time_Internal curr_date_time );

	private:
		/**
		 * @brief                      static instance of uC_RTC class.
		 */
		static uC_RTC* s_rtc;
};

#endif

