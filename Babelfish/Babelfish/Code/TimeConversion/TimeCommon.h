/**
 *****************************************************************************************
 *	@file TimeCommon.h
 *
 *	@brief This file defines following structures used for time conversion.
 *	@n Time_Internal
 *	@n UTC_Offset
 *	@n DST_Switch_Time_Spec
 *	@n DST_Switch_Interval
 *	@n Date_Internal
 *	@n Time_Zone_DST_Spec
 *
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TIME_COMMON_H
#define TIME_COMMON_H

// #define PC_TESTING
// #define TEST_TIME_UTIL
// #define TEST_DATE_UTIL

#ifdef PC_TESTING

#include "iostream"

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;

#else
#include "Includes.h"
#endif

/** Internal time does not include a day-of-week field
 * this data structure is used instead of RTC internal data formats.
 * Proper conversion functions are provided.
 */
typedef struct Time_Internal_tag
{
	uint32_t microsecond;///< Micro Second field
	uint8_t second;	///< Second field
	uint8_t minute;	///< Minute field
	uint8_t hour;	///< 24 hour format only
	uint8_t day;///< Day field
	uint8_t month;	///< Month field
	uint8_t year;	///< lower 2 digits (only consider years between 2000 and 2099)
} Time_Internal;

/**
 * Information needed to specify a time zone.
 */
typedef struct UTC_Offset_tag
{
	uint8_t ahead_of_UTC;	///< range: [0, 1]
	uint8_t hour;	///< Hours ahead of UTC. range: [0, 23]
	uint8_t minute;	///< Minutes ahead of UTC. range: [0, 59]
} UTC_Offset;

/**
 * Format for switching from/to date
 * Switching rule: Spring Forward, Fall Back
 * in the Southern Hemisphere, Fall date is earlier than Spring date.
 */
typedef struct DST_Switch_Time_Spec_tag
{
	uint8_t month;	///< Month for switching.
	uint8_t week_in_month;	///< Week of the month for switching.
	uint8_t day_of_week;///< Weekday for switching.
	uint8_t hour;///< Hour for switching. range:[0,23]
	uint8_t minute;	///< Minute for switching. range:[0,59]
} DST_Switch_Time_Spec;

/**
 * This completes a DST specification
 */
typedef struct DST_Switch_Interval_tag
{
	DST_Switch_Time_Spec start;	///< Start time for DST.
	DST_Switch_Time_Spec end;///< End time for DST.
} DST_Switch_Interval;	//

typedef struct Date_Internal_tag
{
	uint8_t day;///< Day internal.
	uint8_t month;	///< Month Internal.
	uint8_t year;///< Year Internal.
} Date_Internal;

/**
 * This is a complete specification for time zone and DST
 */
typedef struct Time_Zone_DST_Spec_tag
{
	const UTC_Offset* p_time_zone;	///< UTC offset specifying time zone.
	const DST_Switch_Interval* p_interval_DST;	///< DST switch interval.
	uint8_t rule;
} Time_Zone_DST_Spec;

/** This union-struct is used to convert time/date from/to 32 bit format.
 *  (not to confuse with epoch time)since its a union, either date or time accessible at at time.
 */
typedef union date_time_format_t
{
	uint32_t date_time;
	struct p
	{
		uint8_t date;
		uint8_t month;
		uint16_t year;
	} p;

	struct q
	{
		uint8_t hh;
		uint8_t mm;
		uint8_t ss;
	} q;

} date_time_format_t;

#endif
