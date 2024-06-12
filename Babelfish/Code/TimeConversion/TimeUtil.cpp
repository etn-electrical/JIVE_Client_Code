/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "TimeUtil.h"
#include "DateUtil.h"
#include "POSIXTimeUtil.h"
#include "TimeZoneDST.h"

namespace Time_Conversion
{

#ifdef TEST_TIME_UTIL
#include <iostream>
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
TimeUtil::TimeUtil()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeUtil::Second_In_Range( uint8_t second )
{
	// in [0, 59]
	return ( ( MAX_SECOND >= second ) ? true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeUtil::Minute_In_Range( uint8_t minute )
{
	// in [0, 59]
	return ( ( MAX_MINUTE >= minute ) ? true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeUtil::Hour_In_Range_12hr( uint8_t hour )
{
	// 12 hr format: in [1, 12]
	return ( ( ( MIN_HOUR_12HR <= hour ) && ( MAX_HOUR_12HR >= hour ) ) ? true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeUtil::Hour_In_Range_24hr( uint8_t hour )
{
	// 24 hr format: in [0, 23]
	return ( ( MAX_HOUR_24HR >= hour ) ? true : false );
}

// check ranges:
// second and minute, in [0, 59]
// hour in [0, 23]
// date in [1, 31]
// month in [1, 12]
// year in [0, 99]
// does NOT check if date in a certain month makes sense
bool TimeUtil::Is_Valid_Time_Internal( const Time_Internal* p_time_internal, uint8_t century )
{
	bool res = true;

	if ( !Second_In_Range( p_time_internal->second ) )
	{
		res = false;
	}
	else if ( !Minute_In_Range( p_time_internal->minute ) )
	{
		res = false;
	}
	else if ( !Hour_In_Range_24hr( p_time_internal->hour ) )
	{
		res = false;
	}
	else if ( !DateUtil::Is_Valid_Day_Of_Month( p_time_internal->day, p_time_internal->month,
												p_time_internal->year, century ) )
	{
		res = false;
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
bool TimeUtil::Move_Time_Forward( const Time_Internal* p_time_input,
								  Time_Internal* p_time_output, uint8_t hour_offset,
								  uint8_t minute_offset )
{
	bool res = true;
	uint8_t hour_temp = p_time_input->hour;
	uint8_t minute_temp = p_time_input->minute;
	Date_Internal this_date;
	Date_Internal result_date;

	// assuming input time is valid?
	if ( ( MAX_MINUTE < minute_offset ) || ( MAX_HOUR_24HR < hour_offset ) )
	{
		// max adjustment 23:59
		res = false;
	}
	else
	{
		this_date.day = p_time_input->day;
		this_date.month = p_time_input->month;
		this_date.year = p_time_input->year;

		hour_temp += hour_offset;
		minute_temp += minute_offset;

		if ( MAX_MINUTE < minute_temp )
		{
			// hour adjustment
			hour_temp++;// should still be < 48
			minute_temp -= ( MAX_MINUTE + 1U );	// now should fall in [0, 59]
		}
		if ( MAX_HOUR_24HR < hour_temp )
		{
			// date needs to move forward a day
			hour_temp -= ( MAX_HOUR_24HR + 1U );// now should fall in [0, 23]
			if ( false == DateUtil::Get_Next_Date( &this_date, &result_date ) )
			{
				// goes beyond this century
				res = false;
			}
		}
		else
		{
			result_date = this_date;
		}
		if ( res )
		{
			p_time_output->second = p_time_input->second;
			p_time_output->minute = minute_temp;
			p_time_output->hour = hour_temp;
			p_time_output->day = result_date.day;
			p_time_output->month = result_date.month;
			p_time_output->year = result_date.year;
		}
	}
	return ( res );
}

// returns false if the offset is out of range,
// or input date or intermediate dates are out of range
bool TimeUtil::Move_Time_Backward( const Time_Internal* p_time_input,
								   Time_Internal* p_time_output, uint8_t hour_offset,
								   uint8_t minute_offset )
{
	bool res = true;
	uint8_t hour_temp = p_time_input->hour;
	uint8_t minute_temp = p_time_input->minute;
	Date_Internal this_date;
	Date_Internal result_date;

	// assuming input time is valid?
	if ( ( MAX_MINUTE < minute_offset ) || ( MAX_HOUR_24HR < hour_offset ) )
	{
		// max adjustment 23:59
		res = false;
	}
	else
	{
		this_date.day = p_time_input->day;
		this_date.month = p_time_input->month;
		this_date.year = p_time_input->year;
		if ( ( hour_temp > hour_offset ) ||
			 ( ( hour_temp == hour_offset ) && ( minute_temp >= minute_offset ) ) )
		{
			result_date = this_date;// no need to roll back a day
			if ( minute_temp < minute_offset )
			{
				// we know that at this point since minute_temp < minute_offset
				// hour_temp has to be greater than hour_offset
				hour_temp -= ( hour_offset + 1U );
				minute_temp += ( MAX_MINUTE + 1U );
				minute_temp -= minute_offset;
			}
			else
			{
				hour_temp -= hour_offset;
				minute_temp -= minute_offset;
			}
		}
		else
		{
			if ( false == DateUtil::Get_Previous_Date( &this_date, &result_date ) )
			{
				// goes to the previous century
				res = false;
			}
			else
			{
				// we know that at this point we either have
				// hour_temp < hour_offset, or
				// hour_temp == hour_offset, and minute_temp < minute_offset
				if ( minute_temp >= minute_offset )
				{
					// hour_temp < hour_offset
					minute_temp -= minute_offset;
					hour_temp += ( MAX_HOUR_24HR + 1U );
					hour_temp -= hour_offset;
				}
				else
				{
					minute_temp += ( MAX_MINUTE + 1U );
					minute_temp -= minute_offset;
					hour_temp += MAX_HOUR_24HR;
					hour_temp -= hour_offset;
				}
			}
		}
		if ( res )
		{
			p_time_output->second = p_time_input->second;
			p_time_output->minute = minute_temp;
			p_time_output->hour = hour_temp;
			p_time_output->day = result_date.day;
			p_time_output->month = result_date.month;
			p_time_output->year = result_date.year;
		}
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeUtil::Is_Time1_Earlier_Than_Time2( const Time_Internal* p_time1,
											const Time_Internal* p_time2 )
{
	// assuming that both times are valid
	bool res = false;

	if ( p_time1->year < p_time2->year )
	{
		res = true;
	}
	else if ( p_time1->year > p_time2->year )
	{
		res = false;
	}
	// from now on years are equal
	else if ( p_time1->month < p_time2->month )
	{
		res = true;
	}
	else if ( p_time1->month > p_time2->month )
	{
		res = false;
	}
	// from now on both years and months are equal
	else if ( p_time1->day < p_time2->day )
	{
		res = true;
	}
	else if ( p_time1->day > p_time2->day )
	{
		res = false;
	}
	// from now on years, months and dates are all equal
	else if ( p_time1->hour < p_time2->hour )
	{
		res = true;
	}
	else if ( p_time1->hour > p_time2->hour )
	{
		res = false;
	}
	// from now on years, months, dates and hours are all equal
	else if ( p_time1->minute < p_time2->minute )
	{
		res = true;
	}
	else if ( p_time1->minute > p_time2->minute )
	{
		res = false;
	}
	// from now on years, months, dates, hours and minutes are all equal
	else if ( p_time1->second < p_time2->second )
	{
		res = true;
	}
	else
	{
		// if all equal returns false too
		res = false;
	}
	return ( res );
}

// get the (absolute) difference in seconds between two time
// which one comes first? use TimeUtil::is_time1_earlier_than_time2()
bool TimeUtil::Get_Absolute_Time_Difference( const Time_Internal* p_time_1,
											 const Time_Internal* p_time_2, uint32_t* p_diff )
{
	uint32_t time_1_POSIX = 0U;
	uint32_t time_2_POSIX = 0U;
	bool res = false;
	bool temp_POSIX_tm_UTC = POSIXTimeUtil::Get_POSIX_Time_From_UTC_Time(
		p_time_2,
		&time_2_POSIX );

	if ( POSIXTimeUtil::Get_POSIX_Time_From_UTC_Time( p_time_1, &time_1_POSIX ) &&
		 temp_POSIX_tm_UTC )
	{
		// get_POSIX_time_from_UTC_time() return value indicates
		// whether input is valid time
		// so no additional checks are necessary here
		if ( time_1_POSIX >= time_2_POSIX )
		{
			*p_diff = time_1_POSIX - time_2_POSIX;
		}
		else
		{
			*p_diff = time_2_POSIX - time_1_POSIX;
		}
		res = true;
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeUtil::Is_Time1_No_Later_Than_Time2( const Time_Internal* p_time1,
											 const Time_Internal* p_time2 )
{
	return ( !Is_Time1_Earlier_Than_Time2( p_time2, p_time1 ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeUtil::Time_Equal( const Time_Internal* p_time1,
						   const Time_Internal* p_time2 )
{
	return ( ( p_time1->second == p_time2->second ) &&
			 ( p_time1->minute == p_time2->minute ) &&
			 ( p_time1->hour == p_time2->hour ) &&
			 ( p_time1->day == p_time2->day ) &&
			 ( p_time1->month == p_time2->month ) &&
			 ( p_time1->year == p_time2->year ) );
}

#ifdef TEST_TIME_UTIL

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeUtil::test_time_move()
{
	bool res = true;
	Time_Internal this_time;
	Time_Internal forward_time;
	Time_Internal backward_time;
	Time_Internal time_for_verification;

	// testing backward, then verify with forward
	for ( uint8_t hour_shift = 0U; res && ( hour_shift <= MAX_HOUR_24HR ); hour_shift++ )
	{
		for ( uint8_t minute_shift = 0U; res && ( minute_shift <= MAX_MINUTE ); minute_shift++ )
		{
			for ( uint8_t year = 0U; res && ( year <= Time_Conversion::DateUtil::MAX_YEAR_TWO_LOWER_DIGITS ); year++ )
			{
				for ( uint8_t month = Time_Conversion::DateUtil::MONTH_MIN;
					  res && ( month <= Time_Conversion::DateUtil::MONTH_MAX );
					  month++ )
				{

					for ( uint8_t day = Time_Conversion::DateUtil::MIN_DATE;
						  res &&
						  ( day <=
							Time_Conversion::DateUtil::number_of_days_in_month( month, year,
																				Time_Conversion::DateUtil::
																				CURRENT_CENTURY_INDEX ) );
						  day++ )
					{
						// skip first day at this moment
						if ( !( ( 0U == year ) && ( Time_Conversion::DateUtil::MONTH_JAN == month ) && ( 1U == day ) ) )
						{
							for ( uint8_t hour = 0U; res && ( hour <= MAX_HOUR_24HR ); hour++ )
							{
								for ( uint8_t minute = 0U; res && ( minute <= MAX_MINUTE ); minute++ )
								{
									this_time.second = MAX_SECOND;	// arbitrary, should not change after conversion
									this_time.minute = minute;
									this_time.hour = hour;
									this_time.day = day;
									this_time.month = month;
									this_time.year = year;
									if ( !Move_Time_Backward( &this_time, &backward_time, hour_shift, minute_shift ) )
									{
										res = false;
									}
									else if ( !Move_Time_Forward( &backward_time, &time_for_verification, hour_shift,
																  minute_shift ) )
									{
										res = false;
									}
									else
									{
										if ( ( this_time.second != time_for_verification.second ) ||
											 ( this_time.minute != time_for_verification.minute ) ||
											 ( this_time.hour != time_for_verification.hour ) ||
											 ( this_time.day != time_for_verification.day ) ||
											 ( this_time.month != time_for_verification.month ) ||
											 ( this_time.year != time_for_verification.year ) )
										{
											res = false;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// testing forward, then verify with backward
	for ( uint8_t hour_shift = 0U; res && ( hour_shift <= MAX_HOUR_24HR ); hour_shift++ )
	{
		for ( uint8_t minute_shift = 0U; res && ( minute_shift <= MAX_MINUTE ); minute_shift++ )
		{
			for ( uint8_t year = 0U; res && ( year <= Time_Conversion::DateUtil::MAX_YEAR_TWO_LOWER_DIGITS ); year++ )
			{
				for ( uint8_t month = Time_Conversion::DateUtil::MONTH_MIN;
					  res && ( month <= Time_Conversion::DateUtil::MONTH_MAX );
					  month++ )
				{

					for ( uint8_t day = Time_Conversion::DateUtil::MIN_DATE;
						  res &&
						  ( day <=
							Time_Conversion::DateUtil::number_of_days_in_month( month, year,
																				Time_Conversion::DateUtil::
																				CURRENT_CENTURY_INDEX ) );
						  day++ )
					{
						// skip last day at this moment
						if ( !( ( Time_Conversion::DateUtil::MAX_YEAR_TWO_LOWER_DIGITS == year ) &&
								( Time_Conversion::DateUtil::MONTH_DEC == month ) && ( 31 == day ) ) )
						{
							for ( uint8_t hour = 0U; res && ( hour <= MAX_HOUR_24HR ); hour++ )
							{
								for ( uint8_t minute = 0U; res && ( minute <= MAX_MINUTE ); minute++ )
								{
									this_time.second = MAX_SECOND;	// arbitrary, should not change after conversion
									this_time.minute = minute;
									this_time.hour = hour;
									this_time.day = day;
									this_time.month = month;
									this_time.year = year;
									if ( !Move_Time_Forward( &this_time, &forward_time, hour_shift, minute_shift ) )
									{
										res = false;
									}
									else if ( !Move_Time_Backward( &forward_time, &time_for_verification, hour_shift,
																   minute_shift ) )
									{
										res = false;
									}
									else
									{
										if ( ( this_time.second != time_for_verification.second ) ||
											 ( this_time.minute != time_for_verification.minute ) ||
											 ( this_time.hour != time_for_verification.hour ) ||
											 ( this_time.day != time_for_verification.day ) ||
											 ( this_time.month != time_for_verification.month ) ||
											 ( this_time.year != time_for_verification.year ) )
										{
											res = false;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// TODO: check the first day and last day in the century

	if ( res )
	{
		std::cout << "Test passed: move time forward and backward" << std::endl;
	}
	else
	{
		std::cout << "Test failed: move time forward and backward" << std::endl;
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TimeUtil::print_time( const Time_Internal* t )
{
	std::cout	<< "20" << high_digit( t->year ) << low_digit( t->year ) << "-"
				<< high_digit( t->month ) << low_digit( t->month ) << "-"
				<< high_digit( t->day ) << low_digit( t->day ) << "T"
				<< high_digit( t->hour ) << low_digit( t->hour ) << ":"
				<< high_digit( t->minute ) << low_digit( t->minute ) << ":"
				<< high_digit( t->second ) << low_digit( t->second );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int main()
{
	// Time_Conversion::DateUtil::test_leap_year();
	// Time_Conversion::DateUtil::test_day_of_week();
	// Time_Conversion::DateUtil::test_adjacent_months();
	// TimeUtil::test_time_move(); // this test takes a really long time to finish
	// TimeUtil::test_DST();
	// Time_Conversion::DateUtil::test_is_valid_date_in_month();
	// POSIXTimeUtil::test_POSIX_time();// this test is time consuming
	TimeZoneDST::test_DST_reverse();
	return ( 0 );
}

#endif

}
