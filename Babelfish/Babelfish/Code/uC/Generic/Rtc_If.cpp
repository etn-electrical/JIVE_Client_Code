/**
 **********************************************************************************************
 * @file            Rtc_If.cpp C++ RTC Interface  File for RTC Hardware module.
 *
 * @brief           The class contains all APIs required for accessing stm internal RTC. This
 * interface create a
 *                  single instance of RTC and use HAL uC_RTC to access RTC module.
 * @details         See header file for module overview.
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Rtc_If.h"

uC_RTC* Rtc_If::s_rtc = reinterpret_cast<uC_RTC*>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Rtc_If::Rtc_If( RTC_clock_source_t clk_src, RTC_format_t format )
{
	s_rtc = new uC_RTC( clk_src, format );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Rtc_If::Get_Date( Time_Internal& utc_date )
{
	RTC_date_t rtc_date = { 0U, 0U, 0U, 0U };

	s_rtc->Get_Date( &rtc_date );

	utc_date.day = rtc_date.date;
	utc_date.month = rtc_date.month;
	utc_date.year = rtc_date.year;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Rtc_If::Get_Time( Time_Internal& utc_time )
{
	RTC_time_t rtc_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	s_rtc->Get_Time( &rtc_time );

	utc_time.hour = rtc_time.hours;
	utc_time.minute = rtc_time.minutes;
	utc_time.second = rtc_time.seconds;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Rtc_If::Get_Date_Time( Time_Internal& utc_date_time )
{
	RTC_time_t rtc_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	RTC_date_t rtc_date = { 0U, 0U, 0U, 0U };

	s_rtc->Get_Date_Time( &rtc_date, &rtc_time );

	utc_date_time.hour = rtc_time.hours;
	utc_date_time.minute = rtc_time.minutes;
	utc_date_time.second = rtc_time.seconds;

	// subseconds represents 0 to 999999 micro seconds
	utc_date_time.microsecond = rtc_time.subseconds;

	utc_date_time.day = rtc_date.date;
	utc_date_time.month = rtc_date.month;
	utc_date_time.year = rtc_date.year;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Rtc_If::Set_Date( Time_Internal curr_date )
{
	RTC_date_t rtc_date = { 0U, 0U, 0U, 0U };

	rtc_date.date = curr_date.day;
	rtc_date.month = curr_date.month;
	rtc_date.year = curr_date.year;
	rtc_date.weekday = 1U;

	s_rtc->Set_Date( &rtc_date );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Rtc_If::Set_Time( Time_Internal curr_time )
{
	RTC_time_t rtc_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	rtc_time.hours = curr_time.hour;
	rtc_time.minutes = curr_time.minute;
	rtc_time.seconds = curr_time.second;
	rtc_time.timeformat = 0x00U;

	s_rtc->Set_Time( &rtc_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Rtc_If::Set_Date_Time( Time_Internal curr_date_time )
{
	RTC_date_t rtc_date = { 0U, 0U, 0U, 0U };
	RTC_time_t rtc_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	rtc_date.date = curr_date_time.day;
	rtc_date.month = curr_date_time.month;
	rtc_date.year = curr_date_time.year;
	rtc_date.weekday = 1U;
	rtc_time.hours = curr_date_time.hour;
	rtc_time.minutes = curr_date_time.minute;
	rtc_time.seconds = curr_date_time.second;

	// subseconds represents 0 to 999999 micro seconds
	rtc_time.subseconds = curr_date_time.microsecond;

	rtc_time.timeformat = 0x00U;

	s_rtc->Set_Date_Time( &rtc_date, &rtc_time );
}