/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "DateUtil.h"

namespace Time_Conversion
{
// status: reviewed and tested for all functionalities throughly. pretty solid.
DateUtil::DateUtil()
{}

// both year and century should be in [0, 99]
// for example, for the year 2012, year==12, and century==20
// the function does not check the input range
bool DateUtil::Is_Leap_Year( uint8_t year, uint8_t century )
{
	// is leap year if and only if
	// is multiplier of 400,
	// or is multiplier of 4 but not multiplier of 100;

	return ( ( ( 0U == year ) && ( 0U == ( century & 0x03U ) ) ) ||
			 ( ( 0U != year ) && ( 0U == ( year & 0x03U ) ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t DateUtil::Number_Of_Days_In_Month( uint8_t month, uint8_t year,
										   uint8_t century )
{
	// assuming that the month range check has already been performed
	// for safety, return 0 for out of range month
	uint8_t res = 0U;

	switch ( month )
	{
		case MONTH_JAN:
		case MONTH_MAR:
		case MONTH_MAY:
		case MONTH_JUL:
		case MONTH_AUG:
		case MONTH_OCT:
		case MONTH_DEC:
			res = 31U;
			break;

		case MONTH_APR:
		case MONTH_JUN:
		case MONTH_SEP:
		case MONTH_NOV:
			res = 30U;
			break;

		case MONTH_FEB:
			res = Is_Leap_Year( year, century ) ? 29U : 28U;
			break;

		default:
			res = 0U;
			break;
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::Is_Valid_Day_Of_Month( uint8_t day, uint8_t month, uint8_t year,
									  uint8_t century )
{
	bool temp_Yr_in_Range = Year_In_Range( year );
	bool temp_Mn_in_Range = Month_In_Range( month );
	bool res = true;

	if ( ( !Day_Of_Month_In_Range( day ) ) || ( !temp_Mn_in_Range ) ||
		 ( !temp_Yr_in_Range ) )
	{
		res = false;
	}
	else if ( Number_Of_Days_In_Month( month, year, century ) < day )
	{
		res = false;
	}
	else
	{
		res = true;
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::Day_Of_Month_In_Range( uint8_t day_of_month )
{
	return ( ( ( MIN_DATE <= day_of_month ) && ( MAX_DATE >= day_of_month ) ) ?
			 true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::Month_In_Range( uint8_t month )
{
	return ( ( ( MONTH_MIN <= month ) && ( MONTH_MAX >= month ) ) ? true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::Year_In_Range( uint8_t year )
{
	return ( ( MAX_YEAR_TWO_LOWER_DIGITS >= year ) ? true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::Day_Of_Week_In_Range( uint8_t day_of_week )
{
	return ( ( ( WEEKDAY_ISO8601_MIN <= day_of_week ) &&
			   ( WEEKDAY_ISO8601_MAX >= day_of_week ) ) ? true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::Week_In_Month_In_Range( uint8_t week_in_month )
{
	// commented unnecessary comparison (unsigned char >= 0)
	return ( ( /* (WEEK_IN_MONTH_MIN <= week_in_month)
			      && */( WEEK_IN_MONTH_MAX >= week_in_month ) ) ? true : false );
}

// returns false if input date is out of range,
// or the result date would be out of range
bool DateUtil::Get_Previous_Date( const Date_Internal* p_current_date_input,
								  Date_Internal* p_previous_date_res )
{
	bool res = true;

	if ( !Date_Internal_In_Range( p_current_date_input ) )
	{
		res = false;
	}
	else if ( ( MIN_DATE >= p_current_date_input->day ) &&
			  ( MONTH_MIN >= p_current_date_input->month ) &&
			  ( 0U == p_current_date_input->year ) )
	{
		// goes back to the last century
		res = false;
	}
	else
	{
		p_previous_date_res->day = p_current_date_input->day;
		p_previous_date_res->month = p_current_date_input->month;
		p_previous_date_res->year = p_current_date_input->year;
		if ( MIN_DATE >= p_current_date_input->day )
		{
			if ( MONTH_MIN >= p_current_date_input->month )
			{
				// today is a New Year, yesterday was December 31st of last year
				p_previous_date_res->year--;
				p_previous_date_res->month = MONTH_DEC;
				p_previous_date_res->day = 31U;
			}
			else
			{
				// beginning of month, but not a new year
				p_previous_date_res->month--;
				p_previous_date_res->day = Number_Of_Days_In_Month(
					p_previous_date_res->month, p_previous_date_res->year,
					CURRENT_CENTURY_INDEX );
			}
		}
		else
		{
			p_previous_date_res->day--;
		}
	}
	return ( res );
}

// returns false if input day is out of range,
// or the result day would be out of range
bool DateUtil::Get_Next_Date( const Date_Internal* p_current_date_input,
							  Date_Internal* p_next_date_res )
{
	bool res = true;

	if ( !Date_Internal_In_Range( p_current_date_input ) )
	{
		res = false;
	}
	else if ( ( MAX_DATE <= p_current_date_input->day ) &&
			  ( MONTH_MAX <= p_current_date_input->month ) &&
			  ( MAX_YEAR_TWO_LOWER_DIGITS <= p_current_date_input->year ) )
	{
		// goes to the next century
		res = false;
	}
	else
	{
		p_next_date_res->day = p_current_date_input->day;
		p_next_date_res->month = p_current_date_input->month;
		p_next_date_res->year = p_current_date_input->year;

		if ( p_current_date_input->day >=
			 Number_Of_Days_In_Month( p_current_date_input->month,
									  p_current_date_input->year, CURRENT_CENTURY_INDEX ) )
		{
			// end of month
			if ( MONTH_MAX <= p_current_date_input->month )
			{
				// last day of the year - tomorrow will be New Year
				p_next_date_res->day = 1U;
				p_next_date_res->month = MONTH_JAN;
				p_next_date_res->year++;
			}
			else
			{
				// last day of the month
				p_next_date_res->day = 1U;
				p_next_date_res->month++;
			}
		}
		else
		{
			p_next_date_res->day++;
		}
	}
	return ( res );
}

// return value: 1 for Monday, ..., 6 for Saturday, and 7 for Sunday
// should work starting from 9/14/1752, when the British Empire
// adopted the Gregorian calendar - consistent with UNIX "cal" command
// centry: use 20 for year 2000~2099
// does NOT check validity of date/month/year/century
uint8_t DateUtil::Calculate_Day_Of_Week( uint8_t day, uint8_t month,
										 uint8_t year, uint8_t centry )
{
	// apply Gaussian algorithm
	// w = (d + floor(2.6*m-0.2) + y + floor(y/4) + floor(c/4) - 2c) mod 7
	// Y: year-1 for  Jan/Feb (adjusted to the year before)
	// year   for  the rest of the year
	// d: day - no adjustment, [1, 31]
	// m: shifted month starting from March(March as 1,..., and February as 12), use formula ((month + 9) % 12) + 1
	// y: lower 2 digits of Y
	// c: higer 2 digits of Y
	// w: day of week (0 means Sunday, ..., 6 means Saturday)

	// numerical calculation considerations:
	// d: [1, 31]
	// floor(2.6*m-0.2): [2, 31]; values 2, 5, 7, 10, 12, 15, 18, 20, 23, 25, 28, 31
	// y: [0,99]
	// floor(y/4): [0, 24]
	// floor(c/4): [0, 24]
	// 2c: [0, 198]
	// added 203 (29*7), so that no negative number is encountered

	uint16_t y_adjusted;
	uint16_t c_adjusted;
	uint16_t m_adjusted = 0U;
	uint16_t res = 203U;

	y_adjusted = year;
	c_adjusted = centry;

	// assign 1, 2, 3, ..., 10 to Mar, Apr, ..., Dec
	// assign 11 and 12 to Jan and Feb
	if ( month > MONTH_FEB )
	{
		// Mar, Apr, ..., Dec; no need to adjust year
		m_adjusted = static_cast<uint16_t>( month ) - 2U;
	}
	else
	{
		// Jan and Feb, goes back a year
		m_adjusted = static_cast<uint16_t>( month ) + 10U;
		if ( year == 0U )
		{
			y_adjusted = 99U;
			c_adjusted--;
		}
		else
		{
			y_adjusted--;
		}
	}

	res += day;
	res += y_adjusted;
	res += ( y_adjusted >> 2U );
	res -= static_cast<uint16_t>( c_adjusted << 1U );
	res += ( c_adjusted >> 2U );

	switch ( m_adjusted )
	{
		case 1:
		case 2:
			res += static_cast<uint16_t>( m_adjusted * 3U ) - 1U;
			break;

		case 3:
		case 4:
			res += static_cast<uint16_t>( m_adjusted * 3U ) - 2U;
			break;

		case 5:
		case 6:
		case 7:
			res += static_cast<uint16_t>( m_adjusted * 3U ) - 3U;
			break;

		case 8:
		case 9:
			res += static_cast<uint16_t>( m_adjusted * 3U ) - 4U;
			break;

		case 10:
		case 11:
		case 12:
			res += static_cast<uint16_t>( m_adjusted * 3U ) - 5U;
			break;

		default:
			break;
	}

	res %= 7U;
	if ( 0U == res )
	{
		res = 7U;
	}
	return ( static_cast<uint8_t>( res ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t DateUtil::Get_Next_Day_Of_Week( uint8_t day_of_week )
{
	return ( ( WEEKDAY_ISO8601_MAX > day_of_week ) ?
			 ( day_of_week + 1U ) : WEEKDAY_ISO8601_MON );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t DateUtil::Get_Previous_Day_Of_Week( uint8_t day_of_week )
{
	return ( ( WEEKDAY_ISO8601_MIN < day_of_week ) ?
			 ( day_of_week - 1U ) : WEEKDAY_ISO8601_SUN );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::Are_Months_Adjacent( uint8_t month_a, uint8_t month_b )
{
	bool res = false;

	if ( ( ( month_a + 1U ) == month_b ) || ( ( month_b + 1U ) == month_a ) )
	{
		res = true;
	}
	else if ( ( MONTH_MIN == month_a ) && ( MONTH_MAX == month_b ) )
	{
		res = true;
	}
	else if ( ( MONTH_MIN == month_b ) && ( MONTH_MAX == month_a ) )
	{
		res = true;
	}
	else
	{	/* do nothing */
	}
	return ( res );
}

// for out-of-range order and weekday, or month, return 0
// it can be proven that since there are at least 28 days in a month
// for any day of week (weekday), the first, second, third, fourth, or last weekday
// can always be found in the month
uint8_t DateUtil::Get_Day_In_Month( uint8_t week_in_month, uint8_t day_of_week,
									uint8_t month, uint8_t year, uint8_t century )
{
	uint8_t res = 0U;
	uint8_t total_num_of_days = Number_Of_Days_In_Month( month, year, century );
	uint8_t temp_date = 0U;
	uint8_t temp_day_of_week = 0U;

	if ( ( WEEK_IN_MONTH_FOURTH < week_in_month ) || ( 0U == total_num_of_days ) ||
		 ( !Day_Of_Week_In_Range( day_of_week ) ) )
	{
		res = 0U;
	}
	else
	{
		if ( WEEK_IN_MONTH_LAST == week_in_month )
		{
			temp_date = total_num_of_days;
			temp_day_of_week = Calculate_Day_Of_Week( temp_date, month, year,
													  century );
			for ( uint8_t i = 0U; i < 7U; i++ )
			{
				if ( temp_day_of_week == day_of_week )
				{
					res = temp_date;
					break;
				}
				temp_date--;
				temp_day_of_week = Get_Previous_Day_Of_Week( temp_day_of_week );
			}
		}
		else
		{
			temp_date = 1U;
			temp_day_of_week = Calculate_Day_Of_Week( temp_date, month, year,
													  century );
			for ( uint8_t i = 0U; i < 7U; i++ )
			{
				if ( temp_day_of_week == day_of_week )
				{
					res = temp_date;
					break;
				}
				temp_date++;
				temp_day_of_week = Get_Next_Day_Of_Week( temp_day_of_week );
			}
			if ( WEEK_IN_MONTH_SECOND == week_in_month )
			{
				res += 7U;
			}
			else if ( WEEK_IN_MONTH_THIRD == week_in_month )
			{
				res += 14U;
			}
			else if ( WEEK_IN_MONTH_FOURTH == week_in_month )
			{
				res += 21U;
			}
			// res does not need to be adjusted for WEEK_IN_MONTH_FIRST

			else
			{	/* do nothing*/
			}
		}
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::Date_Internal_In_Range( const Date_Internal* p_date )
{
	bool temp_Mn_in_Rng = Month_In_Range( p_date->month );
	bool temp_Day_of_Mn_in_Rng = Day_Of_Month_In_Range( p_date->day );
	bool temp_Vld_Day_of_Mn = Is_Valid_Day_Of_Month( p_date->day, p_date->month,
													 p_date->year, CURRENT_CENTURY_INDEX );

	// assuming the 21st century
	return ( ( Year_In_Range( p_date->year ) ) && temp_Mn_in_Rng &&
			 temp_Day_of_Mn_in_Rng && temp_Vld_Day_of_Mn );
}

#ifdef TEST_DATE_UTIL
uint8_t DateUtil::year_is_leap[400] =
{

	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,

	0U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,

	0U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,

	0U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,
	1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 1U, 0U, 0U, 0U,

};

bool DateUtil::month_adjacent[12][12] =
{
	{   false, true, false, false, false, false, false, false, false, false, false, true},
	{   true, false, true, false, false, false, false, false, false, false, false, false},
	{   false, true, false, true, false, false, false, false, false, false, false, false},
	{   false, false, true, false, true, false, false, false, false, false, false, false},
	{   false, false, false, true, false, true, false, false, false, false, false, false},
	{   false, false, false, false, true, false, true, false, false, false, false, false},
	{   false, false, false, false, false, true, false, true, false, false, false, false},
	{   false, false, false, false, false, false, true, false, true, false, false, false},
	{   false, false, false, false, false, false, false, true, false, true, false, false},
	{   false, false, false, false, false, false, false, false, true, false, true, false},
	{   false, false, false, false, false, false, false, false, false, true, false, true},
	{   true, false, false, false, false, false, false, false, false, false, true, false},
};

// test the 21st century
bool DateUtil::test_is_valid_date_in_month()
{
	bool res = true;
	uint16_t century = 0U;
	uint16_t year = 0U;
	uint16_t month = 0U;
	uint16_t day = 0U;

	// if year > 99, should return false;
	for ( year = 100U; year <= 255U; year++ )
	{
		for ( month = 0U; month <= 255U; month++ )
		{
			for ( day = 0U; day <= 255U; day++ )
			{
				if ( false !=
					 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
											 static_cast<uint8_t>( month ),
											 static_cast<uint8_t>( year ),
											 CURRENT_CENTURY_INDEX ) )
				{
					res = false;
				}
			}
		}
	}

	// if year in [0, 99], but month == 0 or month > 12, should return false;
	for ( year = 0U; year <= 99U; year++ )
	{
		month = 0U;
		for ( day = 0U; day <= 255U; day++ )
		{
			if ( false !=
				 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
										 static_cast<uint8_t>( month ),
										 static_cast<uint8_t>( year ), CURRENT_CENTURY_INDEX ) )
			{
				res = false;
			}
		}

		for ( month = MONTH_MAX + 1U; month <= 255U; month++ )
		{
			for ( day = 0U; day <= 255U; day++ )
			{
				if ( false !=
					 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
											 static_cast<uint8_t>( month ),
											 static_cast<uint8_t>( year ),
											 CURRENT_CENTURY_INDEX ) )
				{
					res = false;
				}
			}
		}
	}

	// if year in [0, 99], month in [1, 12], day == 0 or day > 31, should return false;
	for ( year = 0U; year <= 99U; year++ )
	{
		for ( month = MONTH_MIN; month <= MONTH_MAX; month++ )
		{
			day = 0U;
			if ( false !=
				 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
										 static_cast<uint8_t>( month ),
										 static_cast<uint8_t>( year ), CURRENT_CENTURY_INDEX ) )
			{
				res = false;
			}

			for ( date = MAX_DATE + 1U; day <= 255U; day++ )
			{
				if ( false !=
					 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
											 static_cast<uint8_t>( month ),
											 static_cast<uint8_t>( year ),
											 CURRENT_CENTURY_INDEX ) )
				{
					res = false;
				}
			}
		}
	}

	// year in [0, 99], month in [1, 12], date in [1, 31]
	for ( year = 0U; year <= 99U; year++ )
	{
		for ( month = MONTH_MIN; month <= MONTH_MAX; month++ )
		{
			for ( day = MIN_DATE; day <= 28U; day++ )
			{
				if ( true !=
					 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
											 static_cast<uint8_t>( month ),
											 static_cast<uint8_t>( year ),
											 CURRENT_CENTURY_INDEX ) )
				{
					res = false;
				}
			}

			day = 31U;
			if ( ( MONTH_JAN == month ) || ( MONTH_MAR == month ) || ( MONTH_MAY == month ) ||
				 ( MONTH_JUL == month ) || ( MONTH_AUG == month ) ||
				 ( MONTH_OCT == month ) || ( MONTH_DEC == month ) )
			{
				if ( true !=
					 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
											 static_cast<uint8_t>( month ),
											 static_cast<uint8_t>( year ),
											 CURRENT_CENTURY_INDEX ) )
				{
					res = false;
				}
			}
			else
			{
				if ( false !=
					 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
											 static_cast<uint8_t>( month ),
											 static_cast<uint8_t>( year ),
											 CURRENT_CENTURY_INDEX ) )
				{
					res = false;
				}
			}

			day = 30U;
			if ( MONTH_FEB == month )
			{
				if ( false !=
					 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
											 static_cast<uint8_t>( month ),
											 static_cast<uint8_t>( year ),
											 CURRENT_CENTURY_INDEX ) )
				{
					res = false;
				}
			}
			else
			{
				if ( true !=
					 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
											 static_cast<uint8_t>( month ),
											 static_cast<uint8_t>( year ),
											 CURRENT_CENTURY_INDEX ) )
				{
					res = false;
				}
			}

			day = 29U;
			if ( MONTH_FEB != month )
			{
				if ( true !=
					 Is_Valid_Date_In_Month( static_cast<uint8_t>( day ),
											 static_cast<uint8_t>( month ),
											 static_cast<uint8_t>( year ),
											 CURRENT_CENTURY_INDEX ) )
				{
					res = false;
				}
			}
			else
			{
				if ( 0U != year_is_leap[year] )
				{
					if ( true !=
						 Is_Valid_Date_In_Month(
							 static_cast<uint8_t>( day ),
							 static_cast<uint8_t>( month ),
							 static_cast<uint8_t>( year ),
							 CURRENT_CENTURY_INDEX ) )
					{
						res = false;
					}
				}
				else
				{
					if ( false !=
						 Is_Valid_Date_In_Month(
							 static_cast<uint8_t>( day ),
							 static_cast<uint8_t>( month ),
							 static_cast<uint8_t>( year ),
							 CURRENT_CENTURY_INDEX ) )
					{
						res = false;
					}
				}
			}
		}
	}

	if ( res )
	{
		std::cout << "Test passed: is_valid_date_in_month()" << std::endl;
	}
	else
	{
		std::cout << "Test failed: is_valid_date_in_month()" << std::endl;
	}

	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::test_adjacent_months()
{
	bool res = true;

	for ( uint8_t i = MONTH_MIN; res && ( i <= MONTH_MAX ); i++ )
	{
		for ( uint8_t j = MONTH_MIN; res && ( j <= MONTH_MAX ); j++ )
		{
			if ( are_months_adjacent( i, j ) !=
				 month_adjacent[i - MONTH_MIN][j - MONTH_MIN] )
			{
				res = false;
			}
		}
	}
	if ( res )
	{
		std::cout << "Test passed: adjacent months" << std::endl;
	}
	else
	{
		std::cout << "Test failed: adjacent months" << std::endl;
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::test_leap_year()
{
	uint16_t year = 0U;
	uint16_t century = 0U;
	uint8_t is_leap = 0U;
	bool res = true;

	for ( century = 0U; century < 100U; century++ )
	{
		for ( year = 0U; year < 100U; year++ )
		{
			is_leap =
				( Is_Leap_Year( static_cast<uint8_t>( year ),
								static_cast<uint8_t>( century ) ) ) ? 1 : 0;
			if ( is_leap != year_is_leap[( century * 100U + year ) % 400U] )
			{
				res = false;
				std::cout	<< "leap year error: year " << century * 100U + year
							<< std::endl;
				break;
			}
		}
		if ( !res )
		{
			break;
		}
	}

	if ( res )
	{
		std::cout << "Test passed: leap year" << std::endl;
	}

	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DateUtil::test_day_of_week()
{
	Date_Internal today =
	{   1, 1, 0};	// 2000/01/01
	Date_Internal tomorrow =
	{   0};
	Date_Internal yesterday =
	{   0};
	bool res = true;
	uint8_t day_of_week = WEEKDAY_ISO8601_SAT;	// we know that 01/01/2000 is a Saturday
	uint8_t calculated_day_of_week = 0U;

	uint8_t order = 0U;
	uint8_t num_days_from_end_of_month = 0U;

	while ( !( today.date == 31U && today.month == 12U && today.year == 99U ) )
	{
		calculated_day_of_week = calculate_day_of_week( today.date, today.month,
														today.year, CURRENT_CENTURY_INDEX );
		if ( calculated_day_of_week != day_of_week )
		{
			std::cout	<< "calculate_day_of_week() error: "
						<< static_cast<uint16_t>( today.month ) << "/"
						<< static_cast<uint16_t>( today.date ) << "/"
						<< static_cast<uint16_t>( today.year )
						<< " calculated day of week: "
						<< static_cast<uint16_t>( calculated_day_of_week )
						<< std::endl;
			res = false;
			break;
		}
		if ( today.date <= 7U )
		{
			order = WEEK_IN_MONTH_FIRST;
		}
		else if ( today.date <= 14U )
		{
			order = WEEK_IN_MONTH_SECOND;
		}
		else if ( today.date <= 21U )
		{
			order = WEEK_IN_MONTH_THIRD;
		}
		else if ( today.date <= 28U )
		{
			order = WEEK_IN_MONTH_FOURTH;
		}
		else
		{
			order = WEEK_IN_MONTH_LAST;
		}
		if ( WEEK_IN_MONTH_LAST != order )
		{
			if ( today.date !=
				 Get_Day_In_Month( order, calculated_day_of_week,
								   today.month, today.year, CURRENT_CENTURY_INDEX ) )
			{
				std::cout	<< "get_week_date_in_month() error  - "
							<< static_cast<uint16_t>( today.month ) << "/"
							<< static_cast<uint16_t>( today.date ) << "/"
							<< static_cast<uint16_t>( today.year ) << std::endl;
				res = false;
				break;
			}
		}

		if ( !get_next_day( &today, &tomorrow ) )
		{
			std::cout	<< "get_next_day() error - "
						<< static_cast<uint16_t>( today.month ) << "/"
						<< static_cast<uint16_t>( today.date ) << "/"
						<< static_cast<uint16_t>( today.year ) << std::endl;
			res = false;
			break;
		}
		today = tomorrow;
		day_of_week = Get_Next_Day_Of_Week( day_of_week );
	}

	if ( res )
	{
		today.date = 31U;
		today.month = 12U;
		today.year = 99U;
		day_of_week = WEEKDAY_ISO8601_THU;	// we know 12/31/2099 is a Thursday
		while ( !( today.date == 1U && today.month == 1U && today.year == 0U ) )
		{
			calculated_day_of_week = calculate_day_of_week( today.date,
															today.month, today.year, CURRENT_CENTURY_INDEX );
			if ( calculated_day_of_week != day_of_week )
			{
				std::cout	<< "calculate_day_of_week() error: "
							<< static_cast<uint16_t>( today.month ) << "/"
							<< static_cast<uint16_t>( today.date ) << "/"
							<< static_cast<uint16_t>( today.year )
							<< " calculated day of week: "
							<< static_cast<uint16_t>( calculated_day_of_week )
							<< std::endl;
				res = false;
				break;
			}
			if ( 7U > num_days_from_end_of_month )
			{
				if ( today.date !=
					 Get_Day_In_Month( WEEK_IN_MONTH_LAST,
									   calculated_day_of_week, today.month, today.year,
									   CURRENT_CENTURY_INDEX ) )
				{
					std::cout	<< "get_week_date_in_month() error  - "
								<< static_cast<uint16_t>( today.month ) << "/"
								<< static_cast<uint16_t>( today.date ) << "/"
								<< static_cast<uint16_t>( today.year ) << std::endl;
					res = false;
					break;
				}
				num_days_from_end_of_month++;
			}
			if ( !get_previous_day( &today, &yesterday ) )
			{
				std::cout	<< "get_previous_day() error - "
							<< static_cast<uint16_t>( today.month ) << "/"
							<< static_cast<uint16_t>( today.date ) << "/"
							<< static_cast<uint16_t>( today.year ) << std::endl;
				res = false;
				break;
			}
			if ( 1U == today.date )
			{
				num_days_from_end_of_month = 0U;
			}
			today = yesterday;
			day_of_week = Get_Previous_Day_Of_Week( day_of_week );
		}
	}
	if ( res )
	{
		std::cout << "Test passed: day of week" << std::endl;
	}
	else
	{
		std::cout << "Test failed: day of week" << std::endl;
	}

	return ( res );
}

#endif	// TEST_DATE_UTIL
}
