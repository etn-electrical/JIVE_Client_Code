/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <iostream>
#include "TimeCommon.h"
#include "DateUtil.h"
#include "POSIXTimeUtil.h"
#include "TimeZoneDST.h"
#include "TimeZoneDSTAlt.h"
using namespace Time_Conversion;

#define START_TIME_POSIX 946857600UL		// 2000-01-03T00:00:00Z
// #define END_TIME_POSIX 978307199UL         // 2000-12-31T23:59:59Z
#define END_TIME_POSIX 4102271999UL			// 2099-12-29T23:59:59Z

#define NUM_TEST_ITERATIONS 14

const UTC_Offset time_zones[NUM_TEST_ITERATIONS] =
{
	{1, 10, 0},	// AEST UTC+10, Sydney
	{0, 3, 0},	// BRT UTC-3, Brazilia
	{1, 1, 0},	// CET UTC+1, Madrid
	{1, 0, 0},	// GMT UTC+/-0, London
	{1, 2, 0},	// EET UTC+2, Athens
	{1, 5, 45},	// NPT UTC+5:45, Kathmandu
	{1, 12, 0},	// NZST UTC+12, Wellington NZ
	{0, 5, 0},	// US Eastern UTC-5, New York
	{0, 6, 0},	// US Central UTC-6, Chicago
	{0, 7, 0},	// US Mountain UTC-7, Denver
	{0, 8, 0},	// US Pacific UTC-8, Los Angeles
	{0, 6, 0},	// US Central, UTC-6, Mexico City
	{0, 9, 0},	// arbitrary
	{1, 8, 0},	// arbitrary
};

const uint8_t rules[NUM_TEST_ITERATIONS] = {
	TimeZoneDST::DST_ST_RULE_AU,
	TimeZoneDST::DST_ST_RULE_BR,
	TimeZoneDST::DST_ST_RULE_EU,
	TimeZoneDST::DST_ST_RULE_EU,
	TimeZoneDST::DST_ST_RULE_EU,
	TimeZoneDST::DST_ST_RULE_NONE,
	TimeZoneDST::DST_ST_RULE_NZ,
	TimeZoneDST::DST_ST_RULE_US,
	TimeZoneDST::DST_ST_RULE_US,
	TimeZoneDST::DST_ST_RULE_US,
	TimeZoneDST::DST_ST_RULE_US,
	TimeZoneDST::DST_ST_RULE_US_PRE_2007,
	TimeZoneDST::DST_ST_RULE_MANUAL,
	TimeZoneDST::DST_ST_RULE_MANUAL,
};

const DST_Switch_Interval manual_interval_north =
{
	{DateUtil::MONTH_APR, DateUtil::WEEK_IN_MONTH_LAST,
	 DateUtil::WEEKDAY_ISO8601_SUN, 3, 0},
	{DateUtil::MONTH_NOV, DateUtil::WEEK_IN_MONTH_FIRST,
	 DateUtil::WEEKDAY_ISO8601_SUN, 3, 0},
};	// arbitrary date

const DST_Switch_Interval manual_interval_south =
{
	{DateUtil::MONTH_OCT, DateUtil::WEEK_IN_MONTH_SECOND,
	 DateUtil::WEEKDAY_ISO8601_SUN, 3, 0},
	{DateUtil::MONTH_MAR, DateUtil::WEEK_IN_MONTH_THIRD,
	 DateUtil::WEEKDAY_ISO8601_SUN, 3, 0},
};	// arbitrary date

const DST_Switch_Interval* pIntervals[NUM_TEST_ITERATIONS] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&manual_interval_north,	// needed for manual mode
	&manual_interval_south,	// needed for manual mode
};


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int main()
{
	bool res = true;

	// testing Rio de Janeiro
	// UTC_Offset time_zone = {0, 3, 0};	// BRT UTC-3
	// uint8_t rule = TimeZoneDST::DST_ST_RULE_BR;

	Time_Internal t_UTC_input = { };
	Time_Internal t_local_res = { };
	Time_Internal t_local_res_alt = { };

	Time_Internal t_local_input = { };
	Time_Internal t_UTC_res = { };
	Time_Internal t_UTC_res_alt = { };

	Time_Zone_DST_Spec zone_DST_spec = { };

	for ( uint8_t i = 0; i < NUM_TEST_ITERATIONS; i++ )
	{
		TimeZoneDST::Construct_Zone_DST_Rule_Struct( &zone_DST_spec, &time_zones[i], rules[i],
													 pIntervals[i] );



		if ( res )
		{
			for ( uint32_t t_POSIX = START_TIME_POSIX; t_POSIX <= END_TIME_POSIX; t_POSIX += 60 )
			{
				if ( !POSIXTimeUtil::Get_UTC_Time_From_POSIX_Time( t_POSIX, &t_UTC_input ) )
				{
					res = false;
				}
				else if ( !TimeZoneDST::Convert_Time_From_UTC_To_Local( &t_UTC_input, &t_local_res,
																		&zone_DST_spec ) )
				{
					res = false;
				}
				else
				{
					if ( !TimeZoneDSTAlt::Convert_Time_From_UTC_To_Local( &t_UTC_input, &t_local_res_alt,
																		  &zone_DST_spec ) )
					{
						res = false;
					}
					else
					{
						if ( ( t_local_res.second != t_local_res_alt.second ) ||
							 ( t_local_res.minute != t_local_res_alt.minute ) ||
							 ( t_local_res.hour != t_local_res_alt.hour ) ||
							 ( t_local_res.day != t_local_res_alt.day ) ||
							 ( t_local_res.month != t_local_res_alt.month ) ||
							 ( t_local_res.year != t_local_res_alt.year ) )
						{
							res = false;
						}
					}
				}
				if ( !res )
				{
					break;
				}
			}
		}


		if ( res )
		{
			for ( uint32_t t_POSIX = START_TIME_POSIX; t_POSIX <= END_TIME_POSIX; t_POSIX += 60 )
			{
				if ( !POSIXTimeUtil::Get_UTC_Time_From_POSIX_Time( t_POSIX, &t_local_input ) )
				{
					res = false;
				}
				else if ( !TimeZoneDST::Convert_Time_From_Local_To_UTC( &t_local_input, &t_UTC_res,
																		&zone_DST_spec ) )
				{
					res = false;
				}
				else
				{
					if ( !TimeZoneDSTAlt::Convert_Time_From_Local_To_UTC( &t_local_input, &t_UTC_res_alt,
																		  &zone_DST_spec ) )
					{
						res = false;
					}
					else
					{
						if ( ( t_UTC_res.second != t_UTC_res_alt.second ) ||
							 ( t_UTC_res.minute != t_UTC_res_alt.minute ) ||
							 ( t_UTC_res.hour != t_UTC_res_alt.hour ) ||
							 ( t_UTC_res.day != t_UTC_res_alt.day ) ||
							 ( t_UTC_res.month != t_UTC_res_alt.month ) ||
							 ( t_UTC_res.year != t_UTC_res_alt.year ) )
						{
							res = false;
						}
					}
				}
				if ( !res )
				{
					break;
				}
			}
		}

	}

	if ( res )
	{
		std::cout << "Time zone DST test passed." << std::endl;
	}
	else
	{
		std::cout << "Time zone DST test failed." << std::endl;
	}

	return ( 0 );
}
