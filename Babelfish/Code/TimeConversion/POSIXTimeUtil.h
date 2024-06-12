/**
 *****************************************************************************************
 *	@file POSIXTimeUtil.h
 *
 *	@brief POSIXTimeUtil provides time conversion functionalities based on POSIXTime.
 *	@details POSIX time or Epoch time is a system for describing instants in time, defined as the number of seconds
 *	that have elapsed since 00:00:00 Coordinated Universal Time (UTC).
 *	@n The POSIX epoch is the time 00:00:00 UTC on 1 January 1970 (or 1970-01-01T00:00:00Z ISO 8601).
 *	@n The POSIX time number is zero at the epoch, and increases by exactly 86400 per day since the epoch.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef POSIX_TIME_UTIL_H
#define POSIX_TIME_UTIL_H

#include "TimeCommon.h"

namespace Time_Conversion
{
/**
 * @brief POSIXTimeUtil class provide public methods to
 * @n - convert UTC time to POSIX time.
 * @n - convert POSIX time to UTC time.
 */

// tested using test_POSIX_time()
class POSIXTimeUtil
{
	public:
		/**
		 * @brief Method to convert UTC Time to POSIX time with adjustment for leap year seconds.
		 * @n@b Usage: For example  the time 00:00:00,16/09/2004, 12677 days after the epoch, is represented
		 * by the POSIX time number 12677 × 86400 = 1095292800
		 * param[in] p_time_UTC_input: UTC time input in internal time format i.e in seconds:minute:hour day/month/year
		 * format.
		 * @n@b Usage: {23,50,04,01,08,14} will be UTC time input for the time 23 sec:50 min:4 hours on 1 August 2014.
		 * @param[out] p_time_POSIX_res fetches POSIX time.
		 * @n@b Usage: Get POSIX time as a 32-bit unsigned integer
		 *  for time between 00:00:00, 01/01/2000 and 23:59:59, 12/31/2099, both UTC we know that
		 *  the largest number possible is 4,102,444,799 < 2^32 - 1, so uint32_t is large enough
		 *  to contain all possible return values.
		 * @return Returns true and POSIX Time is fetched if UTC input time and day is valid else returns false.
		 */
		static bool Get_POSIX_Time_From_UTC_Time(
			const Time_Internal* const p_time_UTC_input,
			uint32_t* p_time_POSIX_res );

		/**
		 *@brief Method to convert from POSIX time to UTC time in Time Internal format.
		 *@param[in] time_POSIX_input: give number of seconds that have elapsed since epoch.
		 *@param[out] p_time_UTC_res retrieves UTC time from POSIX time.
		 *@return Returns true if date is valid and time is converted to UTC after generating day of month,
		 * month,year,hours,minutes and seconds from POSIX. Else false is returned.
		 */
		static bool Get_UTC_Time_From_POSIX_Time( uint32_t time_POSIX_input,
												  Time_Internal* p_time_UTC_res );

		static const uint32_t POSIX_TIME_UNINITIALIZED = 0xFFFFFFFF;
		static const uint32_t POSIX_TIME_BEGIN_21ST_CENTURY_UTC = 946684800UL;	///< Related POSIX time value of
																				///< 2000-01-01T00:00:00Z


#ifdef TEST_TIME_UTIL
		static bool test_POSIX_time();

#endif	// TEST_TIME_UTIL	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		POSIXTimeUtil( const POSIXTimeUtil& object );
		POSIXTimeUtil & operator =( const POSIXTimeUtil& object );

		POSIXTimeUtil( void );	// do not instantiate
		~POSIXTimeUtil( void );	// do not destroy

		static uint16_t days_in_previous_months_non_leap_year[12];
		static uint16_t days_in_leap_year_cycle[4];

		static uint32_t Get_Seconds_In_Previous_Months( uint8_t month,
														uint8_t year );

		static uint16_t Get_Days_In_Previous_Months( uint8_t month, uint8_t year );

		// constants
		static const uint8_t SECONDS_IN_A_MINUTE = 60U;
		static const uint8_t MINUTES_IN_AN_HOUR = 60U;
		static const uint8_t HOURS_IN_A_DAY = 24U;
		static const uint16_t DAYS_IN_A_NON_LEAP_YEAR = 365U;
		static const uint16_t DAYS_IN_A_LEAP_YEAR = DAYS_IN_A_NON_LEAP_YEAR + 1U;

		static const uint32_t SECONDS_IN_AN_HOUR =
			( static_cast<uint16_t>( MINUTES_IN_AN_HOUR ) ) * SECONDS_IN_A_MINUTE;
		static const uint32_t SECONDS_IN_A_DAY =
			( static_cast<uint32_t>( HOURS_IN_A_DAY ) ) * SECONDS_IN_AN_HOUR;
		static const uint32_t SECONDS_IN_A_NON_LEAP_YEAR =
			( static_cast<uint32_t>( DAYS_IN_A_NON_LEAP_YEAR ) )
			* SECONDS_IN_A_DAY;

		static const uint32_t POSIX_TIME_END_21ST_CENTURY_UTC = 4102444799UL;
		// 2099-12-31T23:59:59Z

};

}

#endif	// POSIX_TIME_UTIL_H
