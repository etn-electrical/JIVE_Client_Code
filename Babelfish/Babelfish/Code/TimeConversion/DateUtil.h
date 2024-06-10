/**
 *****************************************************************************************
 *	@file DateUtil.h
 *
 *	@brief This file includes interfaces for Date Utility.
 *
 *	@details Provides interfaces for Date Utilities like
 *	@n Leap Year
 *	@n Range of year,month,week,days and Internal date
 *	@n Previous date,Next date, adjacent months,day of week,day in month
 *
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DATE_UTIL_H
#define DATE_UTIL_H

#include "TimeCommon.h"

namespace Time_Conversion
{
#ifdef TEST_DATE_UTIL
#include <iostream>
#endif	// TEST_DATE_UTIL
/**
 * @brief The DateUtil class provides public methods for date utilities.
 * @details Public methods are provided to-
 * @n -Check ranges of day of month,month,year,day of week,internal date and week in month.
 * @n -Check for leap year,validity of day of month and if two months are adjacent.
 * @n -Compute number of days in month,day of week,previous date,next date,next day of week
 * previous day of week and day in month.
 */
class DateUtil
{

	public:

		/** Specifies the n-th week in the month
		 * no matter which month/day-of-week,
		 * the last, 1st, 2nd, 3rd, 4th week are always complete.
		 */
		static const uint8_t WEEK_IN_MONTH_MIN = 0U;
		static const uint8_t WEEK_IN_MONTH_LAST = 0U;
		static const uint8_t WEEK_IN_MONTH_FIRST = 1U;
		static const uint8_t WEEK_IN_MONTH_SECOND = 2U;
		static const uint8_t WEEK_IN_MONTH_THIRD = 3U;
		static const uint8_t WEEK_IN_MONTH_FOURTH = 4U;
		static const uint8_t WEEK_IN_MONTH_MAX = 4U;

		/** Day of week format: use ISO 8601 format
		 * 1 for MON, 2 for TUE, 3 for WED, 4 for THU, 5 for FRI, 6 for SAT, 7 for SUN
		 * real numerical values do not make a difference in most calculations.
		 */
		static const uint8_t WEEKDAY_ISO8601_MIN = 1U;
		static const uint8_t WEEKDAY_ISO8601_MON = 1U;
		static const uint8_t WEEKDAY_ISO8601_TUE = 2U;
		static const uint8_t WEEKDAY_ISO8601_WED = 3U;
		static const uint8_t WEEKDAY_ISO8601_THU = 4U;
		static const uint8_t WEEKDAY_ISO8601_FRI = 5U;
		static const uint8_t WEEKDAY_ISO8601_SAT = 6U;
		static const uint8_t WEEKDAY_ISO8601_SUN = 7U;
		static const uint8_t WEEKDAY_ISO8601_MAX = 7U;
		/**
		 * constants w.r.t. months
		 */
		static const uint8_t MONTH_MIN = 1U;
		static const uint8_t MONTH_JAN = 1U;
		static const uint8_t MONTH_FEB = 2U;
		static const uint8_t MONTH_MAR = 3U;
		static const uint8_t MONTH_APR = 4U;
		static const uint8_t MONTH_MAY = 5U;
		static const uint8_t MONTH_JUN = 6U;
		static const uint8_t MONTH_JUL = 7U;
		static const uint8_t MONTH_AUG = 8U;
		static const uint8_t MONTH_SEP = 9U;
		static const uint8_t MONTH_OCT = 10U;
		static const uint8_t MONTH_NOV = 11U;
		static const uint8_t MONTH_DEC = 12U;
		static const uint8_t MONTH_MAX = 12U;
		/**
		 * @brief Checks whether a given year is leap year.
		 * @details Is leap year if and only if is multiplier of 400, or is multiplier of 4 but not multiplier of 100;
		 * @n@b Usage: Both year and century should be in [0, 99] for example, for the year 2012, year==12, and
		 * century==20
		 *  the function does not check the input range
		 * @param[in] year: Range is 0 to 99,lower two digits are considered.
		 * @param[in] century: Upper two digits of the year.
		 * @return Returns true if leap year else returns false.
		 */

		static bool Is_Leap_Year( uint8_t year, uint8_t century );	// tested using test_leap_year()

		/**
		 * @brief Checks whether the day of month is in the date range of 1-31
		 * @param[in] day_of_month: provide day of month.
		 * @n@b Usage: For example if  day of month is 32,false is returned.
		 * @return Returns true if day_of_month is in range 1-31 else returns false
		 */
		static bool Day_Of_Month_In_Range( uint8_t day_of_month );

		/**
		 * @brief Checks whether month is in range of 1-12.
		 * @param[in] month: provide month.
		 * @n@b Usage: For example if month is 12,function returns a true value.
		 * @return Returns true if month lies in the range 1-12 i.e Jan To Dec or returns false.
		 */
		static bool Month_In_Range( uint8_t month );

		/**
		 * @brief Checks whether the lower two digits of the year lies
		 *        in range 0-99 (for year 2000-2099)
		 * @param[in] year: input lower two digits of the year.
		 * @n@b Usage: For example 2005 is valid year.
		 * @return Returns a true value if the year lies in range 0-99 else returns false.
		 */
		static bool Year_In_Range( uint8_t year );

		/**
		 * @brief Checks whether day_of_week is in range 1-7
		 * @details ISO 8601 format is used for the day of the week.
		 * 1 for Mon,2 for Tue,3 for Wed,4 for Thu,5 For Fri,6 for Sat
		 * 7 for Sun with 1being the min and 7 being the max value.
		 * @param[in] day_of_week: provide numerical value.
		 * @n@b Usage: For example day of week is 9 is not valid.
		 * @return Returns true if day_of_week lies in the range 1-7
		 *         else returns false.
		 */
		static bool Day_Of_Week_In_Range( uint8_t day_of_week );

		/**
		 * @brief Checks whether Date_Intenal in day,month,year format is in range.
		 * @details Verifies range of day_of_month,month and validity of day_of_month for current
		 * century index assuming 21st century.
		 * @param[in] p_date where the date will be stored.
		 * @n@b Usage: 29/01/2006 -valid date. 32/00/2214 -invalid date.
		 * @return Returns true is Date_Internal is in range else false is returned.
		 */
		static bool Date_Internal_In_Range( const Date_Internal* p_date );

		/**
		 * @brief Checks whether week_in_month lies in range 0U-4U.
		 * @param[in] week_in_month: provide week
		 * @n@b Usage: First(1U),second(2U),third(3U),fourth(4U) and last week(0U) of month is valid.
		 * @return Returns true if week_in_month is in range else false value is returned
		 */
		static bool Week_In_Month_In_Range( uint8_t week_in_month );

		/**
		 * @brief Computes number of days in a particular month in valid range. Assuming that the month
		 *        range check has already been performed
		         for safety,  0  is returned for out of range month.
		 * @param[in] month: Month of the year is passed
		 * @param[in] year: Lower two digits of the year is passed to check for leap year.
		 * @param[in] century: 20U only 21st century is considered.
		 * @retval 31U for the months of JAN,MAR,MAY,JUL,AUG,OCT and DEC.
		 * @retval 30U for the months of APR,JUN,SEP,NOV.
		 * @retval 29U for the month of FEB if leap year.
		 * @retval 28U for the month of FEB if not a leap year.
		 * @retval 0U if month is not in range.
		 */

		static uint8_t Number_Of_Days_In_Month( uint8_t month, uint8_t year,
												uint8_t century );	// tested with test_day_of_week()

		/**
		 *@brief Validates date of the month.
		 *@details Checks for Number of days in month,day of month in range,year in range and month in range.
		 *@param[in] day: Day of month.Range is different for each month Max=1U and MIN=31U
		 *@param[in] month: Month of year in the date. Valid range is 1U-12U.
		 *@n@b Usage: 3U for the month of March.
		 *@param[in] year: Year in date. Valid range is 0-99.
		 *@n@b Usage: 14 for year 2014
		 *@param[in] century: provide century.
		 *@n@b Usage: 19U for 20th century,20U for 21st century.
		 *@return Returns true if day,number of days in a month,month and year are in range.
		 *        else false is returned.
		 */
		static bool Is_Valid_Day_Of_Month( uint8_t day, uint8_t month,
										   uint8_t year, uint8_t century );

		/**
		 * @brief Method calculates day of the week i.e Mon,Tue etc.
		 * @details Determines the day of the week for any particular date. Gaussian algorithm is used to
		 * determine the day of the week.Should work starting from 9/14/1752, when the British Empire
		 * adopted the Gregorian calendar.Does NOT check validity of date/month/year/century.
		 * @n@b Usage: For example if day is 18, month is 3,year is 13 and centry is 20 then day of week is 4(thursday)
		 * @param[in] day: Day of the month.
		 * @param[in] month: Assign 1, 2, 3, ..., 10 to Mar, Apr, ..., Dec, assign 11 and 12 to Jan and Feb.
		 * @param[in] year: 0-99.
		 * @param[in] centry : use 20 for year 2000 to 2099.
		 * @return Returns 1 for Monday,2 for Tuesday,3 for Wednesday,4 for Thursday,5 for Friday,6 for Saturday
		 * and 7 for Sunday.
		 */
		static uint8_t Calculate_Day_Of_Week( uint8_t day, uint8_t month,
											  uint8_t year, uint8_t centry );	// tested with test_day_of_week()

		/**
		 * @brief Generates previous date from current date.
		 * @param[in] p_current_date_input: stores the present date.Range of current date is checked.
		 * @n@b Usage: Valid current date.Should not be first day of current century.
		 * @param[out] p_previous_date_res retrieves the previous date.
		 * @return Returns false if the input date goes out of range or if result date goes to previous
		 * century. Returns true if input date is within range and previous date is calculated.
		 */
		static bool Get_Previous_Date( const Date_Internal* p_current_date_input,
									   Date_Internal* p_previous_date_res );// tested with test_day_of_week()

		/**
		 * @brief Generates next date from current date.
		 * @param[in] p_current_date_input stores the present date.
		 * @n@b Usage: Range of current date is checked for valid current date.Should not be last day of the current
		 * century.
		 * @param[out] p_next_date_res retrieves the next date.
		 * @return Returns false if the input date goes out of range or if result date goes to next
		 * century. Returns true if input date is within range and next date is calculated.
		 */
		static bool Get_Next_Date( const Date_Internal* p_current_date_input,
								   Date_Internal* p_next_date_res );// tested with test_day_of_week()

		/**
		 * @brief Gets the next weekday.
		 * @n@b Usage: Next day of the week is TUE if present weekday is MON.
		 * @param[in] day_of_week is the present day of the week Eg. MON,TUE.
		 * @n@b Usage: If 1U then next day of week will be 2U(Tue).
		 * @return Returns next weekday.
		 */

		static uint8_t Get_Next_Day_Of_Week( uint8_t day_of_week );	// tested with test_day_of_week();

		/**
		 * @brief Gets the previous weekday.
		 * @n@b Usage: Previous day of the week will be 3U (WED) if present weekday is 4U(THU).
		 * @param[in] day_of_week is the present day of the week.
		 * @n@b Usage:Eg. MON,TUE.
		 * @return Returns previous weekday.
		 */

		static uint8_t Get_Previous_Day_Of_Week( uint8_t day_of_week );

		/**
		 * @brief  Method to check if two months are adjacent
		 * @n@b Usage: March and April are adjacent months.
		 * @param[in] month_a: First month
		 * @param[in] month_b: Other month
		 * @return Returns true if month_a and month_b are adjacent else returns false.
		 */

		static bool Are_Months_Adjacent( uint8_t month_a, uint8_t month_b );// tested with test_adjacent_months()

		/**
		 * @brief Method gets the day of the month.
		 * @details It can be proven that since there are at least 28 days in a month,
		 * for any day of week (weekday), the first, second, third, fourth, or last weekday can always be found
		 * in the month
		 * @param[in] week_in_month is first,second,third,fourth or last week in month.
		 * @param[in] day_of_week 1-7 for Monday to Friday.
		 * @param[in] month: pass the month for which the day is needed.
		 * @param[in] year: pass the year (lower two digits).
		 * @param[in] century: pass upper two digits for century.
		 * @return Returns 0U for out of range month,year,century and day of week.
		 * @retval 1U to 28U for Feb month (non leap year).
		 * @retval 1U to 29U for Feb Month (leap year).
		 * @retval 1U to 30U for Apr,Jun,Sep and Nov month.
		 * @retval 1U to 31U for Jan,Mar,May,Jul,Aug,Oct and Dec months.
		 */
		static uint8_t Get_Day_In_Month( uint8_t week_in_month, uint8_t day_of_week,
										 uint8_t month, uint8_t year, uint8_t century );

		static const uint8_t MIN_DATE = 1U;
		static const uint8_t MAX_DATE = 31U;
		/**
		 * only considering years 2000~2099
		 */
		static const uint8_t CURRENT_CENTURY_INDEX = 20U;	// only considering years 2000~2099
		static const uint8_t MAX_YEAR_TWO_LOWER_DIGITS = 99U;

#ifdef TEST_DATE_UTIL
		static bool test_day_of_week();

		static bool test_leap_year();

		static bool test_adjacent_months();

		static bool test_is_valid_date_in_month();

		static bool month_adjacent[12][12];
		static uint8_t year_is_leap[400];

#endif	// TEST_DATE_UTIL

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		DateUtil( const DateUtil& object );
		DateUtil & operator =( const DateUtil& object );

		DateUtil( void );	// do not instantiate
		~DateUtil( void );	// do not destroy

};

}

#endif	// DATE_UTIL_H
