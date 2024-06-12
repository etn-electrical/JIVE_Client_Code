/**
 *****************************************************************************************
 *	@file TimeZoneDSTAlt.h
 *
 *	@brief TimeZoneDSTAlt.h provides alternate time conversion functionalities from
 *    POSIXTime for different time zones.

 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TIME_ZONE_DST_ALT_H
#define TIME_ZONE_DST_ALT_H
#include "TimeCommon.h"

/**
 * @details TimeZoneAlt class provides public methods to
 * @n -Conversion of local time to UTC and vice versa.
 * @n -Construct time zone DST rules.
 * @n -DST rule adjustment and switch interval for US pre and post 2007.
 */
class TimeZoneDSTAlt
{
	public:
		/**
		 * DST Switching Time rules: "Spring forward, Fall back"
		 */
		enum
		{
			DST_ST_RULE_NONE = 0,///< No DST Switching time rule.
			DST_ST_RULE_MANUAL = 1,	///< Manual DST ST rule.
			DST_ST_RULE_COUNTRY_START = 2,	///< DST ST rule start.
			DST_ST_RULE_EU = 2,	///< DST ST rule for EU.
			DST_ST_RULE_US = 3,	///< DST ST rule for US post 2007.
			DST_ST_RULE_AU = 4,	///< DST ST rule for AU.
			DST_ST_RULE_BR = 5,	///< DST ST rule for BR with exception if third sunday in Feb falls in Brazillian
								///< Carnival.
			DST_ST_RULE_NZ = 6,	///< DST ST rule for NZ.
			DST_ST_RULE_US_PRE_2007 = 7,///< DST ST rule for US pre year 2007.
			DST_ST_RULE_COUNTRY_END = 7,///< DST ST rule end.
			DST_ST_RULE_COUNTRY_MAX = DST_ST_RULE_COUNTRY_END,	///< Maximum DST ST rule country.
			DST_ST_RULE_TOTAL_NUMBER = DST_ST_RULE_COUNTRY_MAX + 1,	///< Total Number of DST ST rules.
		};

		/**
		 *  DST rule Error Codes
		 */
		typedef enum Zone_DST_rule_err_tag
		{
			ERR_ALL_OK = 0,	///< No error
			ERR_MANUAL_RULE_OK_DST_OK_ZONE_ERR = 1,	///< Error in time zone.
			ERR_MANUAL_RULE_OK_DST_ERR_ZONE_OK = 2,	///< Error in DST.
			ERR_MANUAL_RULE_OK_DST_ERR_ZONE_ERR = 3,///< Error in DST and time zone.
			ERR_MANUAL_RULE_ERR_DST_OK_ZONE_OK = 4,	///< Manual rule error.
			ERR_MANUAL_RULE_ERR_DST_OK_ZONE_ERR = 5,///< Error in Time Zone and Manual rule
			ERR_MANUAL_RULE_ERR_DST_ERR_ZONE_OK = 6,///< Error in Manual rule and DST,Time zone ok.This should not
													///< happen.
			ERR_MANUAL_RULE_ERR_DST_ERR_ZONE_ERR = 7,	///< Error in Time Zone,DST and manual rule.Also should not
														///< happen.
		} Zone_DST_rule_err;

		/**
		 * @brief Method for conversion of time from local time to UTC.Before calling this function:
		 * construct Zone and DST spec structure, validate it and for US time Zone adjust for US 2007 change
		 * @detail POSIX time is considered as standard time, POSIX time is evaluated by
		 * incrementing or  decrementing time shift in seconds according to the time zone.
		 * @n@b Usage: POSIX time is then used for UTC conversion.
		 * @param[in] p_local_time fetches local time for the time zone.
		 * @param[out] p_UTC_time UTC time in terms of seconds,minutes,hours,
		 * day of month,month and year
		 * @param[in] p_zone_DST_spec for time zone rule and DST interval.
		 * @return Returns true if local time can be converted to UTC and the local
		 * time is converted to UTC based on POSIX time. False value is returned if
		 * conversion of local time to UTC not possible.
		 */
		static bool_t Convert_Time_From_Local_To_UTC(
			const Time_Internal* p_local_time, Time_Internal* p_UTC_time,
			const Time_Zone_DST_Spec* p_zone_DST_spec );

		/**
		 * @brief Method for conversion of UTC to local time.Before calling this function:
		 * construct Zone and DST spec structure, validate it and for US time Zone adjust for US 2007 change.
		 * @detail POSIX time is considered as standard time, POSIX time is evaluated by
		 * incrementing or  decrementing time shift in seconds according to the time zone.
		 * @n@b Usage: UTC is converted to POSIX,POSIX time is then used for local time conversion.
		 * @param[in]p_UTC_time fetches UTC for the time zone.
		 * @param[out]p_local_time local time in terms of seconds,minutes,hours,
		 * day of month,month and year.
		 * @param[in]p_zone_DST_spec for time zone rule and DST interval.
		 * @return  Returns true if UTC can be converted to local time and the conversion is done
		 *  based on POSIX time. False value is returned if conversion of UTC to local time not possible.
		 */
		static bool Convert_Time_From_UTC_To_Local( const Time_Internal* p_UTC_time,
													Time_Internal* p_local_time,
													const Time_Zone_DST_Spec* p_zone_DST_spec );

		/**
		 * @brief Handles  Construction of DST switching rules based on time zones.
		 * @param[out] p_spec Retrieves structure for time zone and DST.
		 * @param[in] p_time_zone is time in terms of offset from UTC.
		 * @param[in] rule selects the rules for a particular time zone.
		 * @param[in] p_interval_DST_manual retrieves switching interval-start time and end time for DST.
		 * @return none.
		 */

		static void Construct_Zone_DST_Rule_Struct( Time_Zone_DST_Spec* p_spec,
													const UTC_Offset* p_time_zone, uint8_t rule,
													const DST_Switch_Interval* p_interval_DST_manual );

		/**
		 * @brief Validates and checks for errors in DST switching rules for time zones.
		 * @param[in] p_spec Retrieves structure for time zone and DST.
		 * @return Returns 0 else returns the following error codes.
		 * @retval ERR_ZONE_FIELD Time zone offset not validated.
		 * @retval ERR_DST_FIELD error in DST ST rule.
		 * @retval ERR_MANUAL_RULE_FIELD  In case of manual rule, this error occurs if
		 * DST switch interval is NULL or DST switch interval is not validated.
		 */
		static Zone_DST_rule_err Validate_Zone_DST_Rule_Struct(
			const Time_Zone_DST_Spec* p_spec );

		/**
		 * @brief DST rule adjustment for US time zone pre and post  year 2007.
		 * @param[in,out] p_spec retrieves the  US time zone rules.
		 * @param[in] year is passed to decide DST rule adjustment for US time zone.
		 * @return None
		 */
		static void Zone_DST_Rule_US_Adjustment( Time_Zone_DST_Spec* p_spec,
												 uint8_t year );

		/**
		 *@brief Provides DST Switch interval for US which is the default.
		 *@param[out] p_interval fetches DST switching interval for US.
		 *@return none.
		 */
		static void Get_Default_DST_Switch_Interval(
			DST_Switch_Interval* p_interval );

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		TimeZoneDSTAlt( void );
		~TimeZoneDSTAlt( void );
		TimeZoneDSTAlt( const TimeZoneDSTAlt& rhs );
		TimeZoneDSTAlt & operator =( const TimeZoneDSTAlt& object );


		static uint8_t exception_Brazil[];

		static const DST_Switch_Interval country_DST_time[DST_ST_RULE_COUNTRY_END
														  - DST_ST_RULE_COUNTRY_START + 1];

		static bool Validate_DST_Switch_Interval(
			const DST_Switch_Interval* p_interval );

		static bool Is_Brazil_Exception( uint8_t year );

		static void Construct_Switch_Time( const DST_Switch_Time_Spec* p_spec,
										   Time_Internal* p_switch_time, uint8_t year );

		static void Construct_Start_DST_Time( const DST_Switch_Time_Spec* p_spec,
											  Time_Internal* p_switch_time, uint8_t year );

		static void Construct_End_DST_Time( const DST_Switch_Time_Spec* p_spec,
											Time_Internal* p_switch_time, uint8_t year, uint8_t rule );

		static bool Validate_Time_Zone_Offset( const UTC_Offset* p_offset );

		static bool Is_UTC_Time_In_Summer_Time_EU( const Time_Internal* p_UTC_time,
												   const Time_Zone_DST_Spec* p_zone_DST_spec );

		static bool Is_UTC_Time_In_Summer_Time_Others(
			const Time_Internal* p_UTC_time,
			const Time_Zone_DST_Spec* p_zone_DST_spec );

		static bool Is_Local_Time_In_Summer_Time_EU(
			const Time_Internal* p_local_time,
			const Time_Zone_DST_Spec* p_zone_DST_spec );

		static bool Is_Local_Time_In_Summer_Time_Others(
			const Time_Internal* p_local_time,
			const Time_Zone_DST_Spec* p_zone_DST_spec );

		static const uint8_t US_DST_CHANGE_YEAR = 7;
		// US DST rule changed in (20)07

		static const uint8_t ERR_ZONE_FIELD = 1;
		static const uint8_t ERR_DST_FIELD = 2;
		static const uint8_t ERR_MANUAL_RULE_FIELD = 4;

		static const uint8_t DEFAULT_DST_RULE_COUNTRY = DST_ST_RULE_US;

		static const uint32_t NUM_SECONDS_IN_A_MINUTE = 60;
		static const uint32_t NUM_MINUTES_IN_AN_HOUR = 60;
		static const uint32_t NUM_SECONDS_IN_AN_HOUR = NUM_SECONDS_IN_A_MINUTE
			* NUM_MINUTES_IN_AN_HOUR;
};

#endif	// TIME_ZONE_DST_ALT_H
