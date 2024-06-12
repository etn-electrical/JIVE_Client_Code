/**
 *****************************************************************************************
 *	@file TimeUtil.h
 *
 *	@brief This file includes interfaces for Time Utility
 *
 *	@details Provides interfaces for Time Utilities like
 *	@n -Checking range of seconds,minutes and hours 24 & 12 hrs format
 *	@n -To move time forward and backward
 *	@n -Comparison of two time instances.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include "TimeCommon.h"

namespace Time_Conversion
{


/**
 * @brief This is TimeUtil class.
 * @details The TimeUtil class provides public methods for handling seconds,minutes and hours
 */
class TimeUtil
{
	public:
		/**
		 * @brief Checks whether second is in range
		 * @param[in] second Input seconds value,range is from 0 to 59.
		 * @n@b Usage: 50 is in range, 60 out of range.
		 * @return Returns true if second lies in range 0 to 59 else returns false.
		 */
		static bool Second_In_Range( uint8_t second );

		/**
		 * @brief Checks whether minute is in range.
		 * @param[in] minute: Input minutes value,range is from 0 to 59.
		 * @n@b Usage: 40 is in range, 60 out of range.
		 * @return Returns true if minute lies in range [0,59] else returns false.
		 */
		static bool Minute_In_Range( uint8_t minute );

		/**
		 * @brief Checks whether hour is in range.
		 * @param[in] hour : Input hours value,range is from 0 to 23.
		 * @n@b Usage: 22 is in range, 55 is out of range so invalid.
		 * @return Returns true if hour lies in range [0,23] for 24 Hr format else returns false.
		 */
		static bool Hour_In_Range_24hr( uint8_t hour );

		/**
		 * @brief Checks for range of Time_Internal fields
		 * @details Checks ranges second and minute in [0-59], hour in  [0,23],day in [1,31]
		 *  month in  [1-12] and year in [0,99].
		 * @param[in] p_time_internal fetches Time_Internal
		 * @param[in] year century. If year is 2016, century will be 20 and year will be 16, default value is set to 20.
		 * @n@b Usage: For Example 22:45:00 23:09:2013.
		 * @return Returns true if all the fields of Time_Internal are in range
		 * otherwise false is returned.
		 */

		static bool Is_Valid_Time_Internal( const Time_Internal* p_time_internal, uint8_t century = 20 );

		/**
		 * @brief Method to move time forward according to hour and minute offsets assuming that input time is valid.
		 * @param[in] p_time_input is input time.
		 * @n@b Usage:  In format hh:mm:ss dd:mm:yyyy.
		 * @param[out] p_time_output retrieves forward time.
		 * @param[in] hour_offset is number of hours input time is to be moved forward.
		 * @param[in] minute_offset is number of minutes input time is to be moved forward
		 * @return Returns true if offsets and century are in range and time is moved forward.
		 * Returns false if Maximum adjustment for a day exceeds 23:59 or if moving a day forward
		 * results in going beyond the range of century [2099].
		 */
		static bool Move_Time_Forward( const Time_Internal* p_time_input,
									   Time_Internal* p_time_output, uint8_t hour_offset,
									   uint8_t minute_offset );

		/**
		 * @brief Method to move time backward  assuming input date is valid,according to hour and minute offsets
		 * provided.
		 * @param[in] p_time_input is input time
		 * @n@b Usage: In Format hh:mm:ss dd:mm:yyyy.
		 * @param[out] p_time_output retrieves backward time.
		 * @param[in] hour_offset number of hours input time is decreased.
		 * @param[in] minute_offset is number of minutes input time is decreased.
		 * @return Returns true if offsets and century are in range and time is moved backward.
		 * Returns false if Maximum adjustment for a day exceeds 23:59 or if moving a day backward
		 * results in going to the previous century.
		 */
		static bool Move_Time_Backward( const Time_Internal* p_time_input,
										Time_Internal* p_time_output, uint8_t hour_offset,
										uint8_t minute_offset );

		/**
		 * @brief Checks whether time1 is earlier to time2 .It is assumed that both time1 and time2 are valid.
		 * @n@b Usage: Compares year,month,day,hour,minutes to get which of the two time instances is earlier.
		 * @param[in] p_time1 pointer fetches one time in Time Internal format .
		 * @param[in] p_time2 pointer fetches another time to be compared.
		 * @return Returns true if time1 is earlier than time2, False is returned if time2 is
		 * is earlier than time1 and also if the times are equal.
		 */

		static bool Is_Time1_Earlier_Than_Time2( const Time_Internal* p_time1,
												 const Time_Internal* p_time2 );

		/**
		 * @brief Checks whether time 1 is not later to time2.It is assumed that both time1 and time 2 are valid.
		 * @n@b Usage: Time is compared using Is_Time1_Earlier_Than_Time2.
		 * @param[in] p_time1 pointer fetches one time in Time Internal format.
		 * @param[in] p_time2 pointer fetches other time.
		 * @return Returns true if time1 occurs earlier to time2 and also if the two time instances are same else
		 * returns false.
		 *
		 */
		static bool Is_Time1_No_Later_Than_Time2( const Time_Internal* p_time1,
												  const Time_Internal* p_time2 );

		/**
		 * @brief Checks if two time instants are same.It is assumed that the two time instances are valid.
		 * @n@b Usage: Equality of year,month,day,hour,minute,second is investigated.
		 * @param[in] p_time1 pointer fetches one time in Time Internal format .
		 * @param[in] p_time2 pointer fetches other time to be compared.
		 * @return Returns true if the time being compared are equal else returns false.
		 */
		static bool Time_Equal( const Time_Internal* p_time1,
								const Time_Internal* p_time2 );

		/**
		 * @brief Gets the absolute time difference in seconds between two time.
		 * @param[in] p_time_1 pointer fetches one time.
		 * @param[in] p_time_2 fetches the other time.
		 * @param[out] p_diff fetches back the absolute time difference between two time.
		 * @return Returns false if there is invalid entry of time,returns true is there is difference
		 *  in the two time and the absolute time difference in seconds is generated.
		 */
		static bool Get_Absolute_Time_Difference( const Time_Internal* p_time_1,
												  const Time_Internal* p_time_2, uint32_t* p_diff );

		/**
		 * @brief Checks whether hour lies in range[1,12].
		 * @param[in] hour is value for hour in 12 hour format.
		 * @n@b Usage: For example 13 is out of range.
		 * @return Returns true if hour lies in range [1,12] for 12
		 * hours format else false is returned.
		 */

		static bool Hour_In_Range_12hr( uint8_t hour );

		static const uint8_t MAX_SECOND = 59U;

		static const uint8_t MAX_MINUTE = 59U;
		/**
		 * 12AM/12PM maps to 0:00/12:00 in 24hr format
		 */
		static const uint8_t MAX_HOUR_24HR = 23U;


		static const uint8_t MIN_HOUR_12HR = 1U;
		static const uint8_t MAX_HOUR_12HR = 12U;

	#ifdef TEST_TIME_UTIL
		static bool test_DST();

		static void print_time( const Time_Internal* t );

		static bool test_time_move();

	#endif

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		TimeUtil( const TimeUtil& object );
		TimeUtil & operator =( const TimeUtil& object );

		TimeUtil( void );	// do not construct externally
		~TimeUtil( void );	// do not destruct externally

	#ifdef TEST_TIME_UTIL
		static char low_digit( uint8_t num )
		{   return ( '0' + ( num % 10 ) );}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static char high_digit( uint8_t num )
		{   return ( '0' + ( ( num % 100 ) / 10 ) );}

	#endif	// TEST_TIME_UTIL
};

}

#endif	// TIME_UTIL_H
