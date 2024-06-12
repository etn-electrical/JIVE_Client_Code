/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "POSIXTimeUtil.h"
#include "TimeUtil.h"
#include "DateUtil.h"

namespace Time_Conversion
{

// status: reviewed and tested - pretty solid
uint16_t POSIXTimeUtil::days_in_previous_months_non_leap_year[12] =
{
	( 0U ),
	( 31U ),
	( 31U + 28U ),
	( 31U + 28U + 31U ),
	( 31U + 28U + 31U + 30U ),
	( 31U + 28U + 31U + 30U + 31U ),
	( 31U + 28U + 31U + 30U + 31U + 30U ),
	( 31U + 28U + 31U + 30U + 31U + 30U + 31U ),
	( 31U + 28U + 31U + 30U + 31U + 30U + 31U + 31U ),
	( 31U + 28U + 31U + 30U + 31U + 30U + 31U + 31U + 30U ),
	( 31U + 28U + 31U + 30U + 31U + 30U + 31U + 31U + 30U + 31U ),
	( 31U + 28U + 31U + 30U + 31U + 30U + 31U + 31U + 30U + 31U + 30U ),
};

uint16_t POSIXTimeUtil::days_in_leap_year_cycle[4] =
{
	DAYS_IN_A_LEAP_YEAR,
	DAYS_IN_A_LEAP_YEAR + DAYS_IN_A_NON_LEAP_YEAR,
	DAYS_IN_A_LEAP_YEAR + DAYS_IN_A_NON_LEAP_YEAR + DAYS_IN_A_NON_LEAP_YEAR,
	DAYS_IN_A_LEAP_YEAR + DAYS_IN_A_NON_LEAP_YEAR + DAYS_IN_A_NON_LEAP_YEAR
	+ DAYS_IN_A_NON_LEAP_YEAR,
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
POSIXTimeUtil::POSIXTimeUtil()
{}

// get POSIX time as a 32-bit unsigned integer
// for time between 00:00:00, 01/01/2000 and 23:59:59, 12/31/2099, both UTC
// we know that the largest number possible is 4,102,444,799 < 2^32 - 1,
// so uint32_t is large enough to contain all possible return values
bool POSIXTimeUtil::Get_POSIX_Time_From_UTC_Time(
	const Time_Internal* const p_time_UTC_input, uint32_t* p_time_POSIX_res )
{
	bool res = false;
	bool temp_is_valid_day = Time_Conversion::DateUtil::Is_Valid_Day_Of_Month(
		p_time_UTC_input->day,
		p_time_UTC_input->month, p_time_UTC_input->year,
		Time_Conversion::DateUtil::CURRENT_CENTURY_INDEX );

	if ( TimeUtil::Is_Valid_Time_Internal( p_time_UTC_input ) &&
		 temp_is_valid_day )
	{
		*p_time_POSIX_res = POSIX_TIME_BEGIN_21ST_CENTURY_UTC;
		*p_time_POSIX_res += SECONDS_IN_A_NON_LEAP_YEAR
			* p_time_UTC_input->year;	// considering all previous years
		*p_time_POSIX_res += ( ( 3U + static_cast<uint32_t>( p_time_UTC_input->year ) ) / 4U )
			* SECONDS_IN_A_DAY;	// leap year adjustment
		// now we have the POSIX time at the beginning of the year

		*p_time_POSIX_res += Get_Seconds_In_Previous_Months(
			p_time_UTC_input->month, p_time_UTC_input->year );
		// now we have the POSIX time beginning of the months
		*p_time_POSIX_res += ( static_cast<uint32_t>( p_time_UTC_input->day ) - 1U )
			* SECONDS_IN_A_DAY;
		// seconds in previous days in the same month

		*p_time_POSIX_res += static_cast<uint32_t>( p_time_UTC_input->hour ) * SECONDS_IN_AN_HOUR;
		*p_time_POSIX_res += static_cast<uint32_t>( p_time_UTC_input->minute )
			* SECONDS_IN_A_MINUTE;
		*p_time_POSIX_res += p_time_UTC_input->second;
		res = true;
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool POSIXTimeUtil::Get_UTC_Time_From_POSIX_Time( uint32_t time_POSIX_input,
												  Time_Internal* p_time_UTC_res )
{
	bool res = false;
	uint16_t temp = 0U;
	uint32_t remainder = 0U;
	uint32_t second_in_day = 0U;
	uint8_t month = 0U;

	if ( ( POSIX_TIME_BEGIN_21ST_CENTURY_UTC <= time_POSIX_input ) &&
		 ( POSIX_TIME_END_21ST_CENTURY_UTC >= time_POSIX_input ) )
	{
		time_POSIX_input -= POSIX_TIME_BEGIN_21ST_CENTURY_UTC;
		second_in_day = time_POSIX_input % SECONDS_IN_A_DAY;// second count in the day
		temp = static_cast<uint16_t>( time_POSIX_input / SECONDS_IN_A_DAY );
		remainder = static_cast<uint32_t>( temp ) % days_in_leap_year_cycle[3];
		temp /= days_in_leap_year_cycle[3];	// 4-year brackets
		temp *= 4U;
		if ( days_in_leap_year_cycle[2] <= remainder )
		{
			temp += 3U;
			remainder -= days_in_leap_year_cycle[2];
		}
		else if ( days_in_leap_year_cycle[1] <= remainder )
		{
			temp += 2U;
			remainder -= days_in_leap_year_cycle[1];
		}
		else if ( days_in_leap_year_cycle[0] <= remainder )
		{
			temp += 1U;
			remainder -= days_in_leap_year_cycle[0];
		}
		else
		{/* do nothing */
		}
		// remainder now is the past days in the year
		if ( Time_Conversion::DateUtil::MAX_YEAR_TWO_LOWER_DIGITS >= temp )
		{
			p_time_UTC_res->year = static_cast<uint8_t>( temp );
			for ( month = Time_Conversion::DateUtil::MONTH_DEC;
				  month >= Time_Conversion::DateUtil::MONTH_JAN;
				  month-- )
			{
				temp = Get_Days_In_Previous_Months( month, p_time_UTC_res->year );
				if ( temp <= remainder )
				{
					remainder -= temp;
					break;
				}
				else
				{/* do nothing */
				}
			}
			p_time_UTC_res->month = month;
			p_time_UTC_res->day = static_cast<uint8_t>( remainder ) + 1U;
			if ( Time_Conversion::DateUtil::Is_Valid_Day_Of_Month( p_time_UTC_res->day,
																   p_time_UTC_res->month, p_time_UTC_res->year,
																   Time_Conversion::DateUtil::CURRENT_CENTURY_INDEX ) )
			{
				// now the date is valid, generate hh:mm:ss
				p_time_UTC_res->hour = static_cast<uint8_t>
					( second_in_day / SECONDS_IN_AN_HOUR );

				p_time_UTC_res->minute = static_cast<uint8_t>
					( ( second_in_day % SECONDS_IN_AN_HOUR ) / SECONDS_IN_A_MINUTE );

				p_time_UTC_res->second = static_cast<uint8_t>
					( second_in_day % SECONDS_IN_A_MINUTE );
				res = true;
			}
			else
			{/* do nothing */
			}
		}
		else
		{/* do nothing */
		}
	}
	else
	{/* do nothing */
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t POSIXTimeUtil::Get_Days_In_Previous_Months( uint8_t month, uint8_t year )
{
	uint16_t res = 0U;
	bool temp_is_leap_yr = Time_Conversion::DateUtil::Is_Leap_Year( year,
																	Time_Conversion::DateUtil::CURRENT_CENTURY_INDEX );

	if ( ( Time_Conversion::DateUtil::MONTH_MIN <= month ) &&
		 ( Time_Conversion::DateUtil::MONTH_MAX >= month ) )
	{
		res = days_in_previous_months_non_leap_year[month - 1U];
		// for leap years, the result needs to be adjusted for months after Feb
		if ( ( Time_Conversion::DateUtil::MONTH_MAR <= month ) && temp_is_leap_yr )
		{
			++res;
		}
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t POSIXTimeUtil::Get_Seconds_In_Previous_Months( uint8_t month,
														uint8_t year )
{
	return ( ( static_cast<uint32_t>( Get_Days_In_Previous_Months( month, year ) ) )
			 * SECONDS_IN_A_DAY );
}

#ifdef TEST_TIME_UTIL
// exhaustive test, from 00:00:00 01/01/2000 to 23:59:59 12/31/2099
// testing method:
// compare two values:
// 1) simply incrementing per second
// 2) incrementing based on Calendar, then convert to POSIX time
// if the two values match for all time in the 21st century, pass
// otherwise, fail
bool POSIXTimeUtil::test_POSIX_time()
{
	bool res = true;

	uint8_t second = 0U;
	uint8_t minute = 0U;
	uint8_t hour = 0U;
	uint8_t day = 0U;
	uint8_t max_date = 0U;
	uint8_t month = 0U;
	uint8_t year = 0U;

	Time_Internal time_UTC =
	{   0, 0, 0, 1, 1, 0};	// 2000-01-01, 00:00:00 UTC
	uint32_t time_POSIX_result = 0U;
	uint32_t time_POSIX_reference = POSIX_TIME_BEGIN_21ST_CENTURY_UTC;
	Time_Internal time_calculated =
	{   0};

	// 2000~2099
	for ( year = 0U; res && ( year < 100U ); year++ )
	{
		time_UTC.year = year;
		for ( month = Time_Conversion::DateUtil::MONTH_JAN;
			  res && ( month <= Time_Conversion::DateUtil::MONTH_DEC );
			  month++ )
		{
			time_UTC.month = month;
			max_date = Time_Conversion::DateUtil::number_of_days_in_month( month, year,
																		   Time_Conversion::DateUtil::CURRENT_CENTURY_INDEX );
			for ( day = 1U; res && ( day <= max_date ); day++ )
			{
				time_UTC.day = day;
				for ( hour = 0U; res && ( hour <= TimeUtil::MAX_HOUR_24HR ); hour++ )
				{
					time_UTC.hour = hour;
					for ( minute = 0U; res && ( minute <= TimeUtil::MAX_MINUTE ); minute++ )
					{
						time_UTC.minute = minute;
						for ( second = 0U; res && ( second <= TimeUtil::MAX_SECOND ); second++ )
						{
							time_UTC.second = second;
							if ( !Get_POSIX_Time_From_UTC_Time( &time_UTC, &time_POSIX_result ) )
							{
								res = false;
							}
							else
							{
								if ( time_POSIX_result != time_POSIX_reference )
								{
									res = false;
								}
								else if ( !Get_UTC_Time_From_POSIX_Time( time_POSIX_reference, &time_calculated ) )
								{
									res = false;
								}
								else if ( !TimeUtil::time_equal( &time_calculated, &time_UTC ) )
								{
									res = false;
								}
							}
							time_POSIX_reference++;
						}
					}
				}
			}
		}
	}

	if ( res )
	{
		std::cout << "Test passed: POSIX time conversion" << std::endl;
	}
	else
	{
		std::cout << "Test failed: POSIX time conversion" << std::endl;
	}
	return ( res );
}

#endif	// TEST_TIME_UTIL
}
