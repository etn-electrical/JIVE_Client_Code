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

#include "TimeZoneDST.h"

namespace Time_Conversion
{

// years in the 21st century
// when third Sunday in Feb falls in Brazilian Carnival
// so the switching from DST to standard time needs to
// happen in the fourth Sunday in Feb
uint8_t TimeZoneDST::exception_Brazil[] =
{ 7U, 12U, 15U, 23U, 26U, 34U, 37U, 39U, 42U,
  45U, 48U, 50U, 53U, 61U, 64U, 66U, 72U, 75U, 77U, 80U, 91U };

const DST_Switch_Interval TimeZoneDST::country_DST_time[
	( DST_ST_RULE_COUNTRY_END - DST_ST_RULE_COUNTRY_START ) + 1U] =
{
	// adjustment direction for all: "Spring forward, Fall back"

	// EU, switch on UTC time 01:00, starting from 1998
	{
		{ DateUtil::MONTH_MAR,
		  DateUtil::WEEK_IN_MONTH_LAST,
		  DateUtil::WEEKDAY_ISO8601_SUN, 1U, 0U },						// last Sunday
		// March,
		// 00:59:59
		// -> 02:00:00
		{ DateUtil::MONTH_OCT,
		  DateUtil::WEEK_IN_MONTH_LAST,
		  DateUtil::WEEKDAY_ISO8601_SUN, 2U, 0U }
	},																					// last
																						// Sunday
																						// October,
																						// 01:59:59
																						// ->
																						// 01:00:00

	// US, switch on 02:00 local time, current standard used in US and Canada, starting from 2007
	{
		{ DateUtil::MONTH_MAR,
		  DateUtil::WEEK_IN_MONTH_SECOND,
		  DateUtil::WEEKDAY_ISO8601_SUN, 2U, 0U },						// second
		// Sunday
		// March,
		// 01:59:59
		// -> 03:00:00
		{ DateUtil::MONTH_NOV,
		  DateUtil::WEEK_IN_MONTH_FIRST,
		  DateUtil::WEEKDAY_ISO8601_SUN, 2U, 0U }
	},																					// first
																						// Sunday
																						// November,
																						// 01:59:59
																						// ->
																						// 01:00:00

	// AU, switch on 02:00 STANDARD time, starting from 2008
	{
		{ DateUtil::MONTH_OCT,
		  DateUtil::WEEK_IN_MONTH_FIRST,
		  DateUtil::WEEKDAY_ISO8601_SUN, 2U, 0U },						// first Sunday
		// October,
		// 01:59:59 ->
		// 03:00:00
		{ DateUtil::MONTH_APR,
		  DateUtil::WEEK_IN_MONTH_FIRST,
		  DateUtil::WEEKDAY_ISO8601_SUN, 3U, 0U }
	},																					// first
																						// Sunday
																						// April,
																						// 02:59:59
																						// ->
																						// 02:00:00

	// BR, switch on midnight local time. Note: subject to Carnival adjustment! according to the
	// 2008 decree
	// when the third Sunday of February is the Carnival Sunday
	// DST switches back to standard time on the fourth Sunday of February
	{
		{ DateUtil::MONTH_OCT,
		  DateUtil::WEEK_IN_MONTH_THIRD,
		  DateUtil::WEEKDAY_ISO8601_SUN, 0U, 0U },						// third Sunday
		// October,
		// 23:59:59
		// -> 01:00:00
		{ DateUtil::MONTH_FEB,
		  DateUtil::WEEK_IN_MONTH_THIRD,
		  DateUtil::WEEKDAY_ISO8601_SUN, 0U, 0U }
	},																					// third
																						// Sunday
																						// February,
																						// 23:59:59
																						// ->
																						// 23:00:00

	// NZ, switch on 02:00 STANDARD time, according to the 2007 change
	{
		{ DateUtil::MONTH_SEP,
		  DateUtil::WEEK_IN_MONTH_LAST,
		  DateUtil::WEEKDAY_ISO8601_SUN, 2U, 0U },						// last Sunday
		// September,
		// 01:59:59 ->
		// 03:00:00
		{ DateUtil::MONTH_APR,
		  DateUtil::WEEK_IN_MONTH_FIRST,
		  DateUtil::WEEKDAY_ISO8601_SUN, 3U, 0U }
	},																					// first
																						// Sunday
																						// April,
																						// 02:59:59
																						// ->
																						// 02:00:00

	// US pre-2007, switch on 02:00 local time, obsolete (used in US between 1987 and 2006) ,
	// still used in Mexico
	{
		{ DateUtil::MONTH_APR,
		  DateUtil::WEEK_IN_MONTH_FIRST,
		  DateUtil::WEEKDAY_ISO8601_SUN, 2U, 0U },						// first Sunday
		// April,
		// 01:59:59
		// -> 03:00:00
		{ DateUtil::MONTH_OCT,
		  DateUtil::WEEK_IN_MONTH_LAST,
		  DateUtil::WEEKDAY_ISO8601_SUN, 2U, 0U }
	},																					// last
																						// Sunday
																						// October,
																						// 01:59:59
																						// ->
																						// 01:00:00
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
TimeZoneDST::TimeZoneDST()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TimeZoneDST::Get_Default_DST_Switch_Interval(
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
bool TimeZoneDST::Validate_Time_Zone_Offset(
	const UTC_Offset* p_offset )
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
bool TimeZoneDST::Convert_Time_From_Local_To_UTC(
	const Time_Internal* p_local_time, Time_Internal* p_UTC_time,
	const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res = true;

	if ( DST_ST_RULE_NONE == p_zone_DST_spec->rule )
	{
		if ( 0U != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
		{
			res = TimeUtil::Move_Time_Backward( p_local_time, p_UTC_time,
												p_zone_DST_spec->p_time_zone->hour,
												p_zone_DST_spec->p_time_zone->minute );
		}
		else
		{
			res = TimeUtil::Move_Time_Forward( p_local_time, p_UTC_time,
											   p_zone_DST_spec->p_time_zone->hour,
											   p_zone_DST_spec->p_time_zone->minute );
		}
	}
	else if ( DST_ST_RULE_EU == p_zone_DST_spec->rule )
	{
		res = Convert_Time_From_Local_To_UTC_EU( p_local_time, p_UTC_time,
												 p_zone_DST_spec );
	}
	else
	{
		// for all countries that uses DST,
		// including selected countries and manual
		// excluding those in the EU
		res = Convert_Time_From_Local_To_UTC_Generic( p_local_time, p_UTC_time,
													  p_zone_DST_spec, true );
	}

	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDST::Convert_Time_From_UTC_To_Local(
	const Time_Internal* p_UTC_time,
	Time_Internal* p_local_time,
	const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res = true;

	if ( DST_ST_RULE_NONE == p_zone_DST_spec->rule )
	{
		if ( 0U != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
		{
			res = TimeUtil::Move_Time_Forward( p_UTC_time, p_local_time,
											   p_zone_DST_spec->p_time_zone->hour,
											   p_zone_DST_spec->p_time_zone->minute );
		}
		else
		{
			res = TimeUtil::Move_Time_Backward( p_UTC_time, p_local_time,
												p_zone_DST_spec->p_time_zone->hour,
												p_zone_DST_spec->p_time_zone->minute );
		}
	}
	else if ( DST_ST_RULE_EU == p_zone_DST_spec->rule )
	{
		res = Convert_Time_From_UTC_To_Local_EU( p_UTC_time, p_local_time,
												 p_zone_DST_spec );
	}
	else
	{
		// for all countries that uses DST,
		// including selected countries and manual
		// excluding those in the EU
		res = Convert_Time_From_UTC_To_Local_Generic( p_UTC_time, p_local_time,
													  p_zone_DST_spec, true );
	}

	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDST::Convert_Time_From_Local_To_UTC_EU(
	const Time_Internal* p_local_time,
	Time_Internal* p_UTC_time,
	const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res = true;
	Time_Internal time_WET_WEST = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	// step 1. from local to WET/WEST
	if ( 0U != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
	{
		res = TimeUtil::Move_Time_Backward( p_local_time, &time_WET_WEST,
											p_zone_DST_spec->p_time_zone->hour,
											p_zone_DST_spec->p_time_zone->minute );
	}
	else
	{
		res = TimeUtil::Move_Time_Forward( p_local_time, &time_WET_WEST,
										   p_zone_DST_spec->p_time_zone->hour,
										   p_zone_DST_spec->p_time_zone->minute );
	}
	// step 2. from WET/WEST to UTC
	if ( res )
	{
		res = Convert_Time_From_Local_To_UTC_Generic( &time_WET_WEST, p_UTC_time,
													  p_zone_DST_spec, false );
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDST::Convert_Time_From_UTC_To_Local_EU(
	const Time_Internal* p_UTC_time,
	Time_Internal* p_local_time,
	const Time_Zone_DST_Spec* p_zone_DST_spec )
{
	bool res;
	Time_Internal time_WET_WEST = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	// step 1. from UTC to WET/WEST
	res = Convert_Time_From_UTC_To_Local_Generic( p_UTC_time, &time_WET_WEST,
												  p_zone_DST_spec, false );
	// step 2. from WET/WEST to local
	if ( res )
	{
		if ( 0U != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
		{
			res = TimeUtil::Move_Time_Forward( &time_WET_WEST, p_local_time,
											   p_zone_DST_spec->p_time_zone->hour,
											   p_zone_DST_spec->p_time_zone->minute );
		}
		else
		{
			res = TimeUtil::Move_Time_Backward( &time_WET_WEST, p_local_time,
												p_zone_DST_spec->p_time_zone->hour,
												p_zone_DST_spec->p_time_zone->minute );
		}
	}
	return ( res );
}

// when a local to UTC is performed
// if a time is specified to be impossible (due to Spring Forward),
// it's assumed to be the standard time
// (although at this moment the DST is already applied)
// e.g., for US, 01:59 jumps to 03:00 directly; if by any chance a 02:30 is
// specified, it's assumed to be the standard time (it should be 03:30 DST)
// if a time is specified to be ambiguous (due to Fall Back), it's assumed to
// be the DST time (although in reality it could be standard time too)
// e.g., for US, 01:59 jumps back to 01:00; if a 01:30 is specified,
// it's assumed to be 01:30 DST (although in reality it could be 01:30
// standard time too)
bool TimeZoneDST::Convert_Time_From_Local_To_UTC_Generic(
	const Time_Internal* p_local_time,
	Time_Internal* p_UTC_time,
	const Time_Zone_DST_Spec* p_zone_DST_spec,
	bool time_zone_adjustment )
{
	bool res = true;
	Time_Internal standard_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal start_time_DST = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal end_time_DST = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal start_time_DST_1_hour_later = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	bool temp_t1_earlier_t2;

	// step 1. from local to standard time
	Construct_Start_DST_Time( &p_zone_DST_spec->p_interval_DST->start,
							  &start_time_DST, p_local_time->year );
	Construct_End_DST_Time( &p_zone_DST_spec->p_interval_DST->end, &end_time_DST,
							p_local_time->year, p_zone_DST_spec->rule );
	temp_t1_earlier_t2 = TimeUtil::Is_Time1_Earlier_Than_Time2(
		p_local_time,   &end_time_DST );

	if ( !TimeUtil::Move_Time_Forward( &start_time_DST,
									   &start_time_DST_1_hour_later, 1U, 0U ) )
	{
		res = false;
	}
	else
	{
		if ( p_zone_DST_spec->p_interval_DST->start.month <
			 p_zone_DST_spec->p_interval_DST->end.month )
		{
			// northern hemisphere
			if ( TimeUtil::Is_Time1_No_Later_Than_Time2(
					 &start_time_DST_1_hour_later, p_local_time ) &&
				 temp_t1_earlier_t2 )
			{
				// DST effective
				// e.g., for US, the local switching time is 2AM
				// DST interval (local time) will be
				// [spring 03:00:00, fall 01:59:59]
				if ( !TimeUtil::Move_Time_Backward( p_local_time,
													&standard_time,
													1U, 0U ) )
				{
					res = false;
				}
			}
			else
			{
				// standard time effective
				standard_time = *p_local_time;
			}
		}
		else if ( p_zone_DST_spec->p_interval_DST->start.month >
				  p_zone_DST_spec->p_interval_DST->end.month )
		{
			// southern hemisphere
			if ( TimeUtil::Is_Time1_No_Later_Than_Time2(
					 &start_time_DST_1_hour_later, p_local_time ) ||
				 temp_t1_earlier_t2 )
			{
				// DST effective
				if ( !TimeUtil::Move_Time_Backward( p_local_time,
													&standard_time,
													1U, 0U ) )
				{
					res = false;
				}
			}
			else
			{
				// standard time effective
				standard_time = *p_local_time;
			}
		}
		else
		{
			// not really necessary, defensive programming
			res = false;
		}
	}

	// step 2. from standard time to UTC
	if ( res )
	{
		if ( time_zone_adjustment )
		{
			if ( 0U != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
			{
				res = TimeUtil::Move_Time_Backward( &standard_time, p_UTC_time,
													p_zone_DST_spec->p_time_zone->hour,
													p_zone_DST_spec->p_time_zone->minute );
			}
			else
			{
				res = TimeUtil::Move_Time_Forward( &standard_time, p_UTC_time,
												   p_zone_DST_spec->p_time_zone->hour,
												   p_zone_DST_spec->p_time_zone->minute );
			}
		}
		else
		{
			*p_UTC_time = standard_time;
		}
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDST::Convert_Time_From_UTC_To_Local_Generic(
	const Time_Internal* p_UTC_time,
	Time_Internal* p_local_time,
	const Time_Zone_DST_Spec* p_zone_DST_spec,
	bool time_zone_adjustment )
{
	bool res = true;
	bool temp_t1_earlier_t2;
	Time_Internal standard_time = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal start_time_DST = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal end_time_DST = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
	Time_Internal end_time_DST_1_hour_earlier = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

	// step 1. from UTC to standard time
	if ( time_zone_adjustment )
	{
		if ( 0U != p_zone_DST_spec->p_time_zone->ahead_of_UTC )
		{
			res = TimeUtil::Move_Time_Forward( p_UTC_time, &standard_time,
											   p_zone_DST_spec->p_time_zone->hour,
											   p_zone_DST_spec->p_time_zone->minute );
		}
		else
		{
			res = TimeUtil::Move_Time_Backward( p_UTC_time, &standard_time,
												p_zone_DST_spec->p_time_zone->hour,
												p_zone_DST_spec->p_time_zone->minute );
		}
	}
	else
	{
		standard_time = *p_UTC_time;
	}

	// step 2. from standard time to local time
	if ( res )
	{
		// DST adjustment
		Construct_Start_DST_Time( &p_zone_DST_spec->p_interval_DST->start,
								  &start_time_DST, standard_time.year );
		Construct_End_DST_Time( &p_zone_DST_spec->p_interval_DST->end,
								&end_time_DST, standard_time.year, p_zone_DST_spec->rule );

		if ( !TimeUtil::Move_Time_Backward( &end_time_DST,
											&end_time_DST_1_hour_earlier, 1U,
											0U ) )
		{
			res = false;
		}
		else
		{
			temp_t1_earlier_t2 = Time_Conversion::TimeUtil::Is_Time1_Earlier_Than_Time2(
				&standard_time,
				&end_time_DST_1_hour_earlier );

			if ( p_zone_DST_spec->p_interval_DST->start.month <
				 p_zone_DST_spec->p_interval_DST->end.month )
			{
				// northern hemisphere
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_time_DST,
															 &standard_time )
					 &&
					 temp_t1_earlier_t2 )
				{
					// DST effective
					// e.g., for US, the local switching time is 2AM
					// DST interval (standard time) will be
					// [spring 02:00:00, fall 00:59:59]
					if ( !TimeUtil::Move_Time_Forward( &standard_time,
													   p_local_time, 1U, 0U ) )
					{
						res = false;
					}
				}
				else
				{
					*p_local_time = standard_time;
				}
			}
			else if ( p_zone_DST_spec->p_interval_DST->start.month >
					  p_zone_DST_spec->p_interval_DST->end.month )
			{
				// southern hemisphere
				if ( TimeUtil::Is_Time1_No_Later_Than_Time2( &start_time_DST,
															 &standard_time )
					 ||
					 temp_t1_earlier_t2 )
				{
					if ( !TimeUtil::Move_Time_Forward( &standard_time,
													   p_local_time, 1U, 0U ) )
					{
						res = false;
					}
				}
				else
				{
					*p_local_time = standard_time;
				}
			}
			else
			{
				// not really necessary, defensive programming
				res = false;
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
bool TimeZoneDST::Validate_DST_Switch_Interval(
	const DST_Switch_Interval* p_interval )
{
	bool res = true;
	bool temp_mnths_adj = DateUtil::Are_Months_Adjacent(
		p_interval->start.month,
		p_interval->end.month );
	bool temp_min_in_rng = TimeUtil::Minute_In_Range(
		p_interval->end.minute );
	bool temp_hr_in_rng_strt = TimeUtil::Hour_In_Range_24hr(
		p_interval->start.hour );
	bool temp_hr_in_rng_end = TimeUtil::Hour_In_Range_24hr(
		p_interval->end.hour );
	bool temp_mnth_in_rng_strt = DateUtil::Month_In_Range(
		p_interval->start.month );
	bool temp_mnth_in_rng_end = DateUtil::Month_In_Range(
		p_interval->end.month );
	bool temp_dayofwk_in_rng_strt = DateUtil::Day_Of_Week_In_Range(
		p_interval->start.day_of_week );
	bool temp_dayofwk_in_rng_end = DateUtil::Day_Of_Week_In_Range(
		p_interval->end.day_of_week );
	bool temp_wk_in_rng_strt = DateUtil::Week_In_Month_In_Range(
		p_interval->start.week_in_month );
	bool temp_wk_in_rng_end = DateUtil::Week_In_Month_In_Range(
		p_interval->end.week_in_month );

	// both start/end times shall have correct ranges for
	// month, week in month, day in week, hour and minute
	if ( ( !TimeUtil::Minute_In_Range( p_interval->start.minute ) ) ||
		 ( !temp_min_in_rng ) ||
		 ( !temp_hr_in_rng_strt ) ||
		 ( !temp_hr_in_rng_end ) ||
		 ( !temp_mnth_in_rng_strt ) ||
		 ( !temp_mnth_in_rng_end ) ||
		 ( !temp_dayofwk_in_rng_strt ) ||
		 ( !temp_dayofwk_in_rng_end ) ||
		 ( !temp_wk_in_rng_strt ) ||
		 ( !temp_wk_in_rng_end ) )
	{
		res = false;
	}
	// start/end months cannot be the same, or adjacent to each other
	else if ( ( p_interval->start.month == p_interval->end.month ) ||
			  temp_mnths_adj )
	{
		res = false;
	}
	// start/end time for DST cannot be in first week of January ,
	// or last week of December, to avoid confusion
	else if ( ( DateUtil::MONTH_MIN == p_interval->start.month ) &&
			  ( DateUtil::WEEK_IN_MONTH_FIRST ==
				p_interval->start.week_in_month ) )
	{
		res = false;
	}
	else if ( ( DateUtil::MONTH_MIN == p_interval->end.month ) &&
			  ( DateUtil::WEEK_IN_MONTH_FIRST ==
				p_interval->end.week_in_month ) )
	{
		res = false;
	}
	else if ( ( DateUtil::MONTH_MAX == p_interval->start.month ) &&
			  ( DateUtil::WEEK_IN_MONTH_LAST ==
				p_interval->start.week_in_month ) )
	{
		res = false;
	}
	else if ( ( DateUtil::MONTH_MAX == p_interval->end.month ) &&
			  ( DateUtil::WEEK_IN_MONTH_LAST ==
				p_interval->end.week_in_month ) )
	{
		res = false;
	}
	else
	{	/* do nothing */
	}
	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDST::Brazil_Exception_Applicable( uint8_t year )
{
	bool res = false;
	uint8_t size_of_array = static_cast<uint8_t>( sizeof ( exception_Brazil )
												  / sizeof ( exception_Brazil[0] ) );

	for ( uint8_t i = 0U; i < size_of_array; i++ )
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
void TimeZoneDST::Construct_Switch_Time(
	const DST_Switch_Time_Spec* p_switch_spec,
	Time_Internal* p_switch_time,
	uint8_t year )
{
	p_switch_time->second = 0U;
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
void TimeZoneDST::Construct_Start_DST_Time(
	const DST_Switch_Time_Spec* p_switch_spec,
	Time_Internal* p_switch_time,
	uint8_t year )
{
	Construct_Switch_Time( p_switch_spec, p_switch_time, year );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TimeZoneDST::Construct_End_DST_Time(
	const DST_Switch_Time_Spec* p_switch_spec,
	Time_Internal* p_switch_time,
	uint8_t year,
	uint8_t rule )
{
	DST_Switch_Time_Spec adjusted_Brazil_DST_end_time = *p_switch_spec;

	// only exception, Brazil, on the year
	// when the third Sunday falls in Carnival
	if ( ( DST_ST_RULE_BR == rule ) && ( Brazil_Exception_Applicable( year ) ) )
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDST::Is_Rule_Manual( uint8_t rule )
{
	bool res = false;

	if ( DST_ST_RULE_MANUAL == rule )
	{
		res = true;
	}

	return ( res );
}

// construct with no verification
void TimeZoneDST::Construct_Zone_DST_Rule_Struct(
	Time_Zone_DST_Spec* p_spec,
	const UTC_Offset* p_time_zone,
	uint8_t rule,
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
		p_spec->p_interval_DST = &country_DST_time[rule - DST_ST_RULE_COUNTRY_START];
	}
	else
	{
		p_spec->p_interval_DST = reinterpret_cast<DST_Switch_Interval*>( nullptr );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TimeZoneDST::Zone_DST_Rule_US_Adjustment(
	Time_Zone_DST_Spec* p_spec,
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
TimeZoneDST::Zone_DST_rule_err TimeZoneDST::Validate_Zone_DST_Rule_Struct(
	const Time_Zone_DST_Spec* p_spec )
{
	Zone_DST_rule_err res = ERR_ALL_OK;

	if ( !Validate_Time_Zone_Offset( p_spec->p_time_zone ) )
	{
		res = ERR_TIME_ZONE_ERR;
	}

	if ( DST_ST_RULE_COUNTRY_MAX < p_spec->rule )
	{
		res = ERR_INTERNAL_ERR;
	}
	else
	{
		// only validate rule when the manual rule is used
		if ( DST_ST_RULE_MANUAL == p_spec->rule )
		{
			if ( nullptr == p_spec->p_interval_DST )
			{
				res = ERR_INTERNAL_ERR;		// should not happen, defensive programming
			}
			else
			{
				if ( !Validate_DST_Switch_Interval( p_spec->p_interval_DST ) )
				{
					res = ERR_MANUAL_RULE_ERR;
				}
			}
		}
	}

	return ( res );
}

#ifdef TEST_TIME_UTIL

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDST::test_DST_reverse()
{
	bool res = true;
	Time_Internal time_UTC =
	{   0, 0, 0, 2, 1, 0};
	Time_Internal time_local =
	{   0};
	Time_Internal time_UTC_calculated =
	{   0};
	Time_Internal time_temp =
	{   0};
	const Time_Internal time_max =
	{   59, 59, 23, 30, 12, 99};

#if 0
	// Washington, DC
	UTC_Offset time_zone =
	{   false, 5, 0};	// Eastern Standard Time
	uint8_t rule = DST_ST_RULE_US;
#endif
#if 0
	// Berlin
	UTC_Offset time_zone =
	{   true, 1, 0};	// CET
	uint8_t rule = DST_ST_RULE_EU;
#endif
#if 1
	// Rio de Janeiro
	UTC_Offset time_zone =
	{   false, 3, 0};	// CET
	uint8_t rule = DST_ST_RULE_BR;
#endif

	DST_Switch_Time_Spec end_DST_time_spec =
	{   0};
	Time_Internal time_DST_end =
	{   0};
	Time_Internal time_DST_end_minus_one_hour =
	{   0};

	while ( res && ( TimeUtil::Is_Time1_No_Later_Than_Time2( &time_UTC, &time_max ) ) )
	{
		if ( ( DST_ST_RULE_US == rule ) && ( US_DST_CHANGE_YEAR > time_UTC.year ) )
		{
			end_DST_time_spec =
				country_DST_time[DST_ST_RULE_US_PRE_2007 - DST_ST_RULE_COUNTRY_START].end;
		}
		else
		{
			end_DST_time_spec = country_DST_time[rule - DST_ST_RULE_COUNTRY_START].end;
		}

		Construct_End_DST_Time( &end_DST_time_spec, &time_DST_end, time_UTC.year, rule );
		if ( DST_ST_RULE_EU == rule )
		{
			time_temp = time_DST_end;
			if ( 0 != time_zone.ahead_of_UTC )
			{
				if ( !TimeUtil::Move_Time_Forward( &time_temp, &time_DST_end, time_zone.hour,
												   time_zone.minute ) )
				{
					res = false;
				}
			}
			else
			{
				if ( !TimeUtil::Move_Time_Backward( &time_temp, &time_DST_end, time_zone.hour,
													time_zone.minute ) )
				{
					res = false;
				}
			}
		}
		if ( res )
		{
			if ( !TimeUtil::Move_Time_Backward( &time_DST_end, &time_DST_end_minus_one_hour, 1,
												0 ) )
			{
				res = false;
			}
			else
			{
				if ( TIME_ZONE_DST_ERR_NONE !=
					 convert_from_UTC_to_local( &time_local, &time_UTC, &time_zone, rule,
												nullptr ) )
				{
					res = false;
				}
				else if ( TIME_ZONE_DST_ERR_NONE !=
						  convert_from_local_to_UTC( &time_UTC_calculated, &time_local,
													 &time_zone, rule,
													 nullptr ) )
				{
					res = false;
				}
				else
				{
					if ( !( TimeUtil::Is_Time1_No_Later_Than_Time2( &time_DST_end_minus_one_hour,
																	&time_local ) &&
							TimeUtil::Is_Time1_Earlier_Than_Time2( &time_local,
																   &time_DST_end ) ) )
					{
						if ( !TimeUtil::time_equal( &time_UTC, &time_UTC_calculated ) )
						{
							res = false;
						}
					}
				}
			}
		}

		time_temp = time_UTC;
		if ( !TimeUtil::Move_Time_Forward( &time_temp, &time_UTC, 0, 1 ) )
		{
			res = false;
		}
	}

	if ( res )
	{
		std::cout << "Test passed: DST conversion back and forth" << std::endl;
	}
	else
	{
		std::cout << "Test failed: DST conversion back and forth" << std::endl;
	}

	return ( res );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool TimeZoneDST::test_DST()
{
	bool res = true;

	return ( res );
}

#endif	// TEST_TIME_UTIL
}
