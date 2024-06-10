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

#include "TimeZoneDSTAlt.h"
using namespace Time_Conversion;


// years in the 21st century
// when third Sunday in Feb falls in Brazilian Carnival
// so the switching from DST to standard time needs to
// happen in the fourth Sunday in Feb
uint8_t TimeZoneDSTAlt::exception_Brazil[] = { 7, 12, 15, 23, 26, 34, 37, 39, 42,
											   45, 48, 50, 53, 61, 64, 66, 72, 75, 77, 80, 91 };

const DST_Switch_Interval TimeZoneDSTAlt::country_DST_time[DST_ST_RULE_COUNTRY_END - DST_ST_RULE_COUNTRY_START + 1] = \
{
	// adjustment direction for all: "Spring forward, Fall back"

	// EU, switch on UTC time 01:00, starting from 1998
	{
		{DateUtil::MONTH_MAR, DateUtil::WEEK_IN_MONTH_LAST, DateUtil::WEEKDAY_ISO8601_SUN, 1, 0},	// last Sunday
																									// March,
																									// 00:59:59 ->
																									// 02:00:00
		{DateUtil::MONTH_OCT, DateUtil::WEEK_IN_MONTH_LAST, DateUtil::WEEKDAY_ISO8601_SUN, 2, 0}
	},																							// last Sunday October,
																								// 01:59:59 -> 01:00:00

	// US, switch on 02:00 local time, current standard used in US and Canada, starting from 2007
	{
		{DateUtil::MONTH_MAR, DateUtil::WEEK_IN_MONTH_SECOND, DateUtil::WEEKDAY_ISO8601_SUN, 2, 0},	// second Sunday
																									// March,   01:59:59
																									// -> 03:00:00
		{DateUtil::MONTH_NOV, DateUtil::WEEK_IN_MONTH_FIRST, DateUtil::WEEKDAY_ISO8601_SUN, 2, 0}
	},																							// first Sunday
																								// November, 01:59:59 ->
																								// 01:00:00

	// AU, switch on 02:00 STANDARD time, starting from 2008
	{
		{DateUtil::MONTH_OCT, DateUtil::WEEK_IN_MONTH_FIRST, DateUtil::WEEKDAY_ISO8601_SUN, 2, 0},	// first Sunday
																									// October, 01:59:59
																									// -> 03:00:00
		{DateUtil::MONTH_APR, DateUtil::WEEK_IN_MONTH_FIRST, DateUtil::WEEKDAY_ISO8601_SUN, 3, 0}
	},																							// first Sunday April,
																								// 02:59:59 -> 02:00:00

	// BR, switch on midnight local time. Note: subject to Carnival adjustment! according to the 2008 decree
	// when the third Sunday of February is the Carnival Sunday
	// DST switches back to standard time on the fourth Sunday of February
	{
		{DateUtil::MONTH_OCT, DateUtil::WEEK_IN_MONTH_THIRD, DateUtil::WEEKDAY_ISO8601_SUN, 0, 0},	// third Sunday
																									// October,
																									// 23:59:59 ->
																									// 01:00:00
		{DateUtil::MONTH_FEB, DateUtil::WEEK_IN_MONTH_THIRD, DateUtil::WEEKDAY_ISO8601_SUN, 0, 0}
	},																							// third Sunday
																								// February, 23:59:59 ->
																								// 23:00:00

	// NZ, switch on 02:00 STANDARD time, according to the 2007 change
	{
		{DateUtil::MONTH_SEP, DateUtil::WEEK_IN_MONTH_LAST, DateUtil::WEEKDAY_ISO8601_SUN, 2, 0},	// last Sunday
																									// September,
																									// 01:59:59 ->
																									// 03:00:00
		{DateUtil::MONTH_APR, DateUtil::WEEK_IN_MONTH_FIRST, DateUtil::WEEKDAY_ISO8601_SUN, 3, 0}
	},																							// first Sunday April,
																								// 02:59:59 -> 02:00:00

	// US pre-2007, switch on 02:00 local time, obsolete (used in US between 1987 and 2006) , still used in Mexico
	{
		{DateUtil::MONTH_APR, DateUtil::WEEK_IN_MONTH_FIRST, DateUtil::WEEKDAY_ISO8601_SUN, 2, 0},	// first Sunday
																									// April,
																									// 01:59:59 ->
																									// 03:00:00
		{DateUtil::MONTH_OCT, DateUtil::WEEK_IN_MONTH_LAST,  DateUtil::WEEKDAY_ISO8601_SUN, 2, 0}
	},																							// last Sunday October,
																								// 01:59:59 -> 01:00:00
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
TimeZoneDSTAlt::TimeZoneDSTAlt()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TimeZoneDSTAlt::Get_Default_DST_Switch_Interval(
	DST_Switch_Interval* p_interval )
{
	*p_interval = country_DST_time[DEFAULT_DST_RULE_COUNTRY
								   - DST_ST_RULE_COUNTRY_START];
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDSTAlt::Validate_Time_Zone_Offset( const UTC_Offset* p_offset )
{
	bool res = true;

	if ( ( TimeUtil::MAX_MINUTE < p_offset->minute ) ||
		 ( TimeUtil::MAX_HOUR_24HR < p_offset->hour ) )
	{
		res = false;
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDSTAlt::Convert_Time_From_Local_To_UTC(
	const Time_Internal* p_local_time, Time_Internal* p_UTC_time,
	const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res = true;
	uint32_t time_POSIX = 0;
	uint32_t time_shift_in_seconds = 0;

	time_shift_in_seconds = NUM_SECONDS_IN_A_MINUTE
		* ( p_zone_DST_spec->p_time_zone->hour * NUM_MINUTES_IN_AN_HOUR
			+ p_zone_DST_spec->p_time_zone->minute );

	if ( !POSIXTimeUtil::Get_POSIX_Time_From_UTC_Time( p_local_time, &time_POSIX ) )
	{
		res = false;
	}

	if ( res )
	{
		// according to known ranges of operands we know that time_POSIX can
		// handle the +/- without underflow/overflow
		if ( 0 != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
		{
			// east
			time_POSIX -= time_shift_in_seconds;
		}
		else
		{
			// west
			time_POSIX += time_shift_in_seconds;
		}

		// now time_POSIX is the standard time, one more stop before local time
		if ( DST_ST_RULE_EU == p_zone_DST_spec->rule )
		{
			if ( Is_Local_Time_In_Summer_Time_EU( p_local_time, p_zone_DST_spec ) )
			{
				time_POSIX -= NUM_SECONDS_IN_AN_HOUR;
			}
		}
		else if ( DST_ST_RULE_NONE != p_zone_DST_spec->rule )
		{
			// other countries other than EU
			if ( Is_Local_Time_In_Summer_Time_Others( p_local_time, p_zone_DST_spec ) )
			{
				time_POSIX -= NUM_SECONDS_IN_AN_HOUR;
			}
		}
		// no need to change time_POSIX further if DST rule is NONE
	}


	if ( res )
	{
		if ( !POSIXTimeUtil::Get_UTC_Time_From_POSIX_Time( time_POSIX, p_UTC_time ) )
		{
			res = false;
		}
	}

	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDSTAlt::Is_Local_Time_In_Summer_Time_EU( const Time_Internal* p_local_time,
													  const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res = true;

	Time_Internal start_summer_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal end_summer_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	Time_Internal time_temp_1 = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal time_temp_2 = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	if ( DST_ST_RULE_EU == p_zone_DST_spec->rule )
	{
		Construct_Start_DST_Time( &p_zone_DST_spec->p_interval_DST->start, &time_temp_2, p_local_time->year );
		if ( !TimeUtil::Move_Time_Forward( &time_temp_2, &time_temp_1, 1, 0 ) )
		{
			res = false;
		}
		Construct_End_DST_Time( &p_zone_DST_spec->p_interval_DST->end, &time_temp_2, p_local_time->year,
								DST_ST_RULE_EU );

		if ( res )
		{
			// UTC -> local
			if ( 0 != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
			{
				// east
				if ( !TimeUtil::Move_Time_Forward( &time_temp_1, &start_summer_time, p_zone_DST_spec->p_time_zone->hour,
												   p_zone_DST_spec->p_time_zone->minute ) )
				{
					res = false;
				}
				else if ( !TimeUtil::Move_Time_Forward( &time_temp_2, &end_summer_time,
														p_zone_DST_spec->p_time_zone->hour,
														p_zone_DST_spec->p_time_zone->minute ) )
				{
					res = false;
				}
			}
			else
			{
				// west
				if ( !TimeUtil::Move_Time_Backward( &time_temp_1, &start_summer_time,
													p_zone_DST_spec->p_time_zone->hour,
													p_zone_DST_spec->p_time_zone->minute ) )
				{
					res = false;
				}
				else if ( !TimeUtil::Move_Time_Backward( &time_temp_2, &end_summer_time,
														 p_zone_DST_spec->p_time_zone->hour,
														 p_zone_DST_spec->p_time_zone->minute ) )
				{
					res = false;
				}
			}
		}

		if ( res )
		{
			if ( p_zone_DST_spec->p_interval_DST->start.month < p_zone_DST_spec->p_interval_DST->end.month )
			{
				// north
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_summer_time, p_local_time ) &&
					 TimeUtil::Is_Time1_Earlier_Than_Time2( p_local_time, &end_summer_time ) )
				{
					res = true;
				}
				else
				{
					res = false;
				}
			}
			else
			{
				// south
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_summer_time, p_local_time ) ||
					 TimeUtil::Is_Time1_Earlier_Than_Time2( p_local_time, &end_summer_time ) )
				{
					res = true;
				}
				else
				{
					res = false;
				}
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
bool TimeZoneDSTAlt::Is_Local_Time_In_Summer_Time_Others( const Time_Internal* p_local_time,
														  const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res = true;
	Time_Internal start_summer_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal start_summer_time_temp = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal end_summer_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	if ( ( ( ( DST_ST_RULE_COUNTRY_START <= p_zone_DST_spec->rule ) &&
			 ( DST_ST_RULE_COUNTRY_END >= p_zone_DST_spec->rule ) &&
			 ( DST_ST_RULE_EU != p_zone_DST_spec->rule ) ) ) ||
		 ( DST_ST_RULE_MANUAL == p_zone_DST_spec->rule ) )
	{
		Construct_Start_DST_Time( &p_zone_DST_spec->p_interval_DST->start, &start_summer_time_temp,
								  p_local_time->year );
		Construct_End_DST_Time( &p_zone_DST_spec->p_interval_DST->end, &end_summer_time, p_local_time->year,
								p_zone_DST_spec->rule );
		if ( !TimeUtil::Move_Time_Forward( &start_summer_time_temp, &start_summer_time, 1, 0 ) )
		{
			res = false;
		}
		if ( res )
		{
			if ( p_zone_DST_spec->p_interval_DST->start.month < p_zone_DST_spec->p_interval_DST->end.month )
			{
				// north
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_summer_time, p_local_time ) &&
					 TimeUtil::Is_Time1_Earlier_Than_Time2( p_local_time, &end_summer_time ) )
				{
					res = true;
				}
				else
				{
					res = false;
				}
			}
			else
			{
				// south
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_summer_time, p_local_time ) ||
					 TimeUtil::Is_Time1_Earlier_Than_Time2( p_local_time, &end_summer_time ) )
				{
					res = true;
				}
				else
				{
					res = false;
				}
			}
		}
	}
	else
	{
		res = false;
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDSTAlt::Is_UTC_Time_In_Summer_Time_EU( const Time_Internal* p_UTC_time,
													const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res = false;
	Time_Internal start_summer_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal end_summer_time_temp = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal end_summer_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	if ( DST_ST_RULE_EU == p_zone_DST_spec->rule )
	{
		Construct_Start_DST_Time( &p_zone_DST_spec->p_interval_DST->start, &start_summer_time, p_UTC_time->year );
		Construct_End_DST_Time( &p_zone_DST_spec->p_interval_DST->end, &end_summer_time_temp, p_UTC_time->year,
								DST_ST_RULE_EU );
		if ( TimeUtil::Move_Time_Backward( &end_summer_time_temp, &end_summer_time, 1, 0 ) )
		{
			if ( p_zone_DST_spec->p_interval_DST->start.month < p_zone_DST_spec->p_interval_DST->end.month )
			{
				// north
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_summer_time, p_UTC_time ) &&
					 TimeUtil::Is_Time1_Earlier_Than_Time2( p_UTC_time, &end_summer_time ) )
				{
					res = true;
				}
			}
			else
			{
				// south, should not happen, included for completion
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_summer_time, p_UTC_time ) ||
					 TimeUtil::Is_Time1_Earlier_Than_Time2( p_UTC_time, &end_summer_time ) )
				{
					res = true;
				}
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
bool TimeZoneDSTAlt::Is_UTC_Time_In_Summer_Time_Others( const Time_Internal* p_UTC_time,
														const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res = true;
	Time_Internal start_summer_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal end_summer_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	Time_Internal time_temp_1 = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal time_temp_2 = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	if ( ( ( ( DST_ST_RULE_COUNTRY_START <= p_zone_DST_spec->rule ) &&
			 ( DST_ST_RULE_COUNTRY_END >= p_zone_DST_spec->rule ) &&
			 ( DST_ST_RULE_EU != p_zone_DST_spec->rule ) ) ) ||
		 ( DST_ST_RULE_MANUAL == p_zone_DST_spec->rule ) )
	{

		Construct_End_DST_Time( &p_zone_DST_spec->p_interval_DST->end, &time_temp_1, p_UTC_time->year,
								p_zone_DST_spec->rule );
		// local -> standard
		if ( !TimeUtil::Move_Time_Backward( &time_temp_1, &time_temp_2, 1, 0 ) )
		{
			res = false;
		}
		Construct_Start_DST_Time( &p_zone_DST_spec->p_interval_DST->start, &time_temp_1, p_UTC_time->year );

		if ( res )
		{
			// standard -> UTC
			if ( 0 != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
			{
				// east
				if ( !TimeUtil::Move_Time_Backward( &time_temp_1, &start_summer_time,
													p_zone_DST_spec->p_time_zone->hour,
													p_zone_DST_spec->p_time_zone->minute ) )
				{
					res = false;
				}
				else if ( !TimeUtil::Move_Time_Backward( &time_temp_2, &end_summer_time,
														 p_zone_DST_spec->p_time_zone->hour,
														 p_zone_DST_spec->p_time_zone->minute ) )
				{
					res = false;
				}
			}
			else
			{
				// west
				if ( !TimeUtil::Move_Time_Forward( &time_temp_1, &start_summer_time, p_zone_DST_spec->p_time_zone->hour,
												   p_zone_DST_spec->p_time_zone->minute ) )
				{
					res = false;
				}
				else if ( !TimeUtil::Move_Time_Forward( &time_temp_2, &end_summer_time,
														p_zone_DST_spec->p_time_zone->hour,
														p_zone_DST_spec->p_time_zone->minute ) )
				{
					res = false;
				}
			}
		}

		// now we have the start/end time in UTC

		if ( res )
		{
			if ( p_zone_DST_spec->p_interval_DST->start.month < p_zone_DST_spec->p_interval_DST->end.month )
			{
				// north
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_summer_time, p_UTC_time ) &&
					 TimeUtil::Is_Time1_Earlier_Than_Time2( p_UTC_time, &end_summer_time ) )
				{
					res = true;
				}
				else
				{
					res = false;
				}
			}
			else
			{
				// south
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_summer_time, p_UTC_time ) ||
					 TimeUtil::Is_Time1_Earlier_Than_Time2( p_UTC_time, &end_summer_time ) )
				{
					res = true;
				}
				else
				{
					res = false;
				}
			}
		}
	}
	else
	{
		res = false;
	}

	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDSTAlt::Convert_Time_From_UTC_To_Local(
	const Time_Internal* p_UTC_time, Time_Internal* p_local_time,
	const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res = true;
	uint32_t time_POSIX = 0;
	uint32_t time_shift_in_seconds = 0;

	time_shift_in_seconds = NUM_SECONDS_IN_A_MINUTE
		* ( p_zone_DST_spec->p_time_zone->hour * NUM_MINUTES_IN_AN_HOUR
			+ p_zone_DST_spec->p_time_zone->minute );

	if ( !POSIXTimeUtil::Get_POSIX_Time_From_UTC_Time( p_UTC_time, &time_POSIX ) )
	{
		res = false;
	}

	if ( res )
	{
		// according to known ranges of operands we know that time_POSIX can
		// handle the +/- without underflow/overflow
		if ( 0 != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
		{
			// east
			time_POSIX += time_shift_in_seconds;
		}
		else
		{
			// west
			time_POSIX -= time_shift_in_seconds;
		}

		// now time_POSIX is the standard time, one more stop before local time
		if ( DST_ST_RULE_EU == p_zone_DST_spec->rule )
		{
			if ( Is_UTC_Time_In_Summer_Time_EU( p_UTC_time, p_zone_DST_spec ) )
			{
				time_POSIX += NUM_SECONDS_IN_AN_HOUR;
			}
		}
		else if ( DST_ST_RULE_NONE != p_zone_DST_spec->rule )
		{
			// other countries other than EU
			if ( Is_UTC_Time_In_Summer_Time_Others( p_UTC_time, p_zone_DST_spec ) )
			{
				time_POSIX += NUM_SECONDS_IN_AN_HOUR;
			}
		}
		// no need to change time_POSIX further if DST rule is NONE
	}


	if ( res )
	{
		if ( !POSIXTimeUtil::Get_UTC_Time_From_POSIX_Time( time_POSIX, p_local_time ) )
		{
			res = false;
		}
	}

	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDSTAlt::Validate_DST_Switch_Interval(
	const DST_Switch_Interval* p_interval )
{
	bool res = true;

	// both start/end times shall have correct ranges for
	// month, week in month, day in week, hour and minute
	if ( !TimeUtil::Minute_In_Range( p_interval->start.minute ) ||
		 !TimeUtil::Minute_In_Range( p_interval->end.minute ) ||
		 !TimeUtil::Hour_In_Range_24hr( p_interval->start.hour ) ||
		 !TimeUtil::Hour_In_Range_24hr( p_interval->end.hour ) ||
		 !DateUtil::Month_In_Range( p_interval->start.month ) ||
		 !DateUtil::Month_In_Range( p_interval->end.month ) ||
		 !DateUtil::Day_Of_Week_In_Range( p_interval->start.day_of_week ) ||
		 !DateUtil::Day_Of_Week_In_Range( p_interval->end.day_of_week ) ||
		 !DateUtil::Week_In_Month_In_Range(
			 p_interval->start.week_in_month ) ||
		 !DateUtil::Week_In_Month_In_Range( p_interval->end.week_in_month ) )
	{
		res = false;
	}
	// start/end months cannot be the same, or adjacent to each other
	else if ( ( p_interval->start.month == p_interval->end.month ) ||
			  DateUtil::Are_Months_Adjacent( p_interval->start.month,
											 p_interval->end.month ) )
	{
		res = false;
	}
	// start/end time for DST cannot be in first week of January ,
	// or last week of December, to avoid confusion
	else if ( ( DateUtil::MONTH_MIN == p_interval->start.month ) &&
			  ( DateUtil::WEEK_IN_MONTH_FIRST == p_interval->start.week_in_month ) )
	{
		res = false;
	}
	else if ( ( DateUtil::MONTH_MIN == p_interval->end.month ) &&
			  ( DateUtil::WEEK_IN_MONTH_FIRST == p_interval->end.week_in_month ) )
	{
		res = false;
	}
	else if ( ( DateUtil::MONTH_MAX == p_interval->start.month ) &&
			  ( DateUtil::WEEK_IN_MONTH_LAST == p_interval->start.week_in_month ) )
	{
		res = false;
	}
	else if ( ( DateUtil::MONTH_MAX == p_interval->end.month ) &&
			  ( DateUtil::WEEK_IN_MONTH_LAST == p_interval->end.week_in_month ) )
	{
		res = false;
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDSTAlt::Is_Brazil_Exception( uint8_t year )
{
	bool res = false;
	uint8_t size_of_array = sizeof( exception_Brazil )
		/ sizeof( exception_Brazil[0] );

	for ( uint8_t i = 0; i < size_of_array; i++ )
	{
		if ( year == exception_Brazil[i] )
		{
			res = true;
			break;
		}
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TimeZoneDSTAlt::Construct_Switch_Time(
	const DST_Switch_Time_Spec* p_switch_spec, Time_Internal* p_switch_time,
	uint8_t year )
{
	p_switch_time->second = 0;
	p_switch_time->minute = p_switch_spec->minute;
	p_switch_time->hour = p_switch_spec->hour;
	p_switch_time->day = DateUtil::Get_Day_In_Month(
		p_switch_spec->week_in_month, p_switch_spec->day_of_week,
		p_switch_spec->month, year, DateUtil::CURRENT_CENTURY_INDEX );
	p_switch_time->month = p_switch_spec->month;
	p_switch_time->year = year;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TimeZoneDSTAlt::Construct_Start_DST_Time(
	const DST_Switch_Time_Spec* p_switch_spec, Time_Internal* p_switch_time,
	uint8_t year )
{
	Construct_Switch_Time( p_switch_spec, p_switch_time, year );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TimeZoneDSTAlt::Construct_End_DST_Time(
	const DST_Switch_Time_Spec* p_switch_spec, Time_Internal* p_switch_time,
	uint8_t year, uint8_t rule )
{
	DST_Switch_Time_Spec adjusted_Brazil_DST_end_time = *p_switch_spec;

	// only exception, Brazil, on the year
	// when the third Sunday falls in Carnival
	if ( ( DST_ST_RULE_BR == rule ) && ( Is_Brazil_Exception( year ) )
		  )
	{
		adjusted_Brazil_DST_end_time.week_in_month++;
		Construct_Switch_Time( &adjusted_Brazil_DST_end_time, p_switch_time,
							   year );
	}
	else
	{
		Construct_Switch_Time( p_switch_spec, p_switch_time, year );
	}
}

// construct with no verification
void TimeZoneDSTAlt::Construct_Zone_DST_Rule_Struct( Time_Zone_DST_Spec* p_spec,
													 const UTC_Offset* p_time_zone, uint8_t rule,
													 const DST_Switch_Interval* p_interval_DST_manual )
{
	p_spec->p_time_zone = p_time_zone;
	p_spec->rule = rule;
	if ( DST_ST_RULE_MANUAL == rule )
	{
		p_spec->p_interval_DST = p_interval_DST_manual;
	}
	else if ( ( DST_ST_RULE_COUNTRY_START <= rule ) &&
			  ( DST_ST_RULE_COUNTRY_END >= rule ) )
	{
		p_spec->p_interval_DST = &country_DST_time[rule
												   - DST_ST_RULE_COUNTRY_START];
	}
	else
	{
		p_spec->p_interval_DST = NULL;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TimeZoneDSTAlt::Zone_DST_Rule_US_Adjustment( Time_Zone_DST_Spec* p_spec,
												  uint8_t year )
{
	if ( DST_ST_RULE_US == p_spec->rule )
	{
		if ( US_DST_CHANGE_YEAR > year )
		{
			// US DST interval based on year 2007
			p_spec->p_interval_DST = &country_DST_time[DST_ST_RULE_US_PRE_2007
													   - DST_ST_RULE_COUNTRY_START];
		}
		else
		{
			p_spec->p_interval_DST = &country_DST_time[DST_ST_RULE_US
													   - DST_ST_RULE_COUNTRY_START];
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
TimeZoneDSTAlt::Zone_DST_rule_err TimeZoneDSTAlt::Validate_Zone_DST_Rule_Struct(
	const Time_Zone_DST_Spec* p_spec )
{
	uint8_t res = 0;

	if ( !Validate_Time_Zone_Offset( p_spec->p_time_zone ) )
	{
		res |= ERR_ZONE_FIELD;
	}

	if ( DST_ST_RULE_COUNTRY_MAX < p_spec->rule )
	{
		res |= ERR_DST_FIELD;
	}
	else
	{
		// only validate rule when the manual rule is used
		if ( DST_ST_RULE_MANUAL == p_spec->rule )
		{
			if ( NULL == p_spec->p_interval_DST )
			{
				res |= ERR_MANUAL_RULE_FIELD;	// should not happen, defensive programming
			}
			else
			{
				if ( !Validate_DST_Switch_Interval( p_spec->p_interval_DST ) )
				{
					res |= ERR_MANUAL_RULE_FIELD;
				}
			}
		}
	}

	return ( static_cast<Zone_DST_rule_err>( res ) );
}
