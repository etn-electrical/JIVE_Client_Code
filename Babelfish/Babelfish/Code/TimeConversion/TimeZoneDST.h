/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 *	@file TimeZoneDST.h
 *
 *	@brief TimeZoneDST provides Time Conversion functionalities for various time zones.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TIME_ZONE_DST_H
#define TIME_ZONE_DST_H
#include "TimeCommon.h"

namespace Time_Conversion
{
/**
 * @brief TimeZoneDST class provides public methods for Time Conversion according to the time zones of different
 * countries.
 * @details Time conversion  is done according to the Zone and DST specification for EU,US,AU,BR,NZ,US Pre 2007.
 * @n@b Usage:
 * @n It provides public methods to-
 * @n@b 1.Convert Local time to UTC and vice versa.
 * @n@b 2.Construct and adjust zone DST rules.
 * @n@b 3. Zone DST rule adjustment and DST switch interval for US(default time zone).
 * @n@b 4. Manual DST rule. In case of countries other than EU,US,AU,BR,NZ,US Pre 2007,the start and end of DST
 *         has to be manually specified for any time zone.
 */
class TimeZoneDST
{
	public:

		static const uint8_t DST_ST_RULE_NONE = 0U;
		static const uint8_t DST_ST_RULE_MANUAL = 1U;

		static const uint8_t DST_ST_RULE_COUNTRY_START = 2U;
		static const uint8_t DST_ST_RULE_EU = 2U;
		static const uint8_t DST_ST_RULE_US = 3U;
		static const uint8_t DST_ST_RULE_AU = 4U;
		static const uint8_t DST_ST_RULE_BR = 5U;
		static const uint8_t DST_ST_RULE_NZ = 6U;
		static const uint8_t DST_ST_RULE_US_PRE_2007 = 7U;
		static const uint8_t DST_ST_RULE_COUNTRY_END = 7U;

		static const uint8_t DST_ST_RULE_COUNTRY_MAX = DST_ST_RULE_COUNTRY_END;

		static const uint8_t DST_ST_RULE_TOTAL_NUMBER = DST_ST_RULE_COUNTRY_MAX + 1U;
		/**
		 *  Time zone rule error codes.
		 */

		typedef enum Zone_DST_rule_err_tag
		{
			ERR_ALL_OK = 0,	///< No Errror
			ERR_MANUAL_RULE_ERR = 1,///< Rule Error.
			ERR_TIME_ZONE_ERR = 2,	///< Time zone error.
			ERR_INTERNAL_ERR = 3,///< Internal error.
		} Zone_DST_rule_err;


		/**
		 * @details Interface to convert Local Time to UTC for a time zone.Before calling this function:
		 *  construct Zone and DST spec structure, validate it and if US time zone adjust for US 2007 change.
		 * @param[in] p_local_time is the local time of a country.
		 * @param[out] p_UTC_time fetches back the UTC time
		 * @param[in] p_zone_DST_spec specifications for Time Zone and DST for a country.
		 * @return Returns true value if time conversion is possible and UTC time is then fetched back from local time
		 * for a particular time zone.
		 *         False is returned if conversion of time is not possible.
		 */
		static bool Convert_Time_From_Local_To_UTC(
			const Time_Internal* p_local_time, Time_Internal* p_UTC_time,
			const Time_Zone_DST_Spec* p_zone_DST_spec );

		/**
		 * @brief Interface to convert UTC to Local Time for a time zone.Before calling this function:
		 *  construct Zone and DST spec structure, validate it and if US time zone adjust for US 2007 change.
		 * @param[in] p_UTC_time provides UTC for a time zone.
		 * @param[out] p_local_time fetches the local time .
		 * @param[in]  p_zone_DST_spec  specifications for Time Zone and DST for a country.
		 * @return Returns true value if time conversion is possible and local time is then fetched back.
		 *         False value is returned if conversion of time is not possible.
		 */
		static bool Convert_Time_From_UTC_To_Local( const Time_Internal* p_UTC_time,
													Time_Internal* p_local_time,
													const Time_Zone_DST_Spec* p_zone_DST_spec );

		/**
		 * @brief Handles  Construction of DST switching rules based on time zones.
		 * @n@b Usage: Construct with no verification
		 * @param[out] p_spec stores struct for time zone and DST.
		 * @param[in] p_time_zone is time in terms of offset from UTC.
		 * @param[in] rule selects the rules for a particular time zone.
		 * @param[in] p_interval_DST_manual is switching interval-start time and end time for DST in case of manual
		 * rule.
		 * @return none
		 */
		static void Construct_Zone_DST_Rule_Struct( Time_Zone_DST_Spec* p_spec,
													const UTC_Offset* p_time_zone, uint8_t rule,
													const DST_Switch_Interval* p_interval_DST_manual );

		/**
		 * @brief Validates and checks for errors in DST switching rules for time zones.
		 * @param[in] p_spec  struct for time zone and DST.
		 * @return Returns ERR_ALL_OK else returns the following error codes.
		 * @retval ERR_TIME_ZONE_ERR Time Zone not validated.
		 * @retval ERR_INTERNAL_ERR rule not validated.
		 * @retval ERR_MANUAL_RULE_ERR DST switching interval not validated.
		 */
		static Zone_DST_rule_err Validate_Zone_DST_Rule_Struct(
			const Time_Zone_DST_Spec* p_spec );

		/**
		 * @brief DST rule adjustment for US time zone pre and post  year 2007.
		 * @param[in] year: argument is used to decide DST rule adjustment for US time zone.
		 * @param[in,out] p_spec: pointer to time zone structure.
		 * @return None
		 *
		 */
		static void Zone_DST_Rule_US_Adjustment( Time_Zone_DST_Spec* p_spec,
												 uint8_t year );

		/**
		 *@brief Provides DST Switch interval for US which is the default country.
		 *@param[out] p_interval fetches DST switching interval for US.
		 *@return none.
		 */

		static void Get_Default_DST_Switch_Interval(
			DST_Switch_Interval* p_interval );

		/**
		 * @brief Checks for manual rule.
		 * @param[in] rule selects the time zone rules.
		 * @n@b Usage: For example 2U for EU.1U for manual rule.
		 * @return Returns true if manual rule is applicable else returns false.
		 */

		static bool Is_Rule_Manual( uint8_t rule );

#ifdef TEST_TIME_UTIL
		static bool TimeZoneDST::test_DST();

		static bool TimeZoneDST::test_DST_reverse();

#endif	// TEST_TIME_UTIL

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		TimeZoneDST( const TimeZoneDST& object );
		TimeZoneDST & operator =( const TimeZoneDST& object );

		TimeZoneDST( void );// do not instantiate
		~TimeZoneDST( void );	// do not destroy

		static uint8_t exception_Brazil[];

		static const DST_Switch_Interval country_DST_time[
			( DST_ST_RULE_COUNTRY_END - DST_ST_RULE_COUNTRY_START ) + 1U];

		static bool Validate_DST_Switch_Interval(
			const DST_Switch_Interval* p_interval );

		static bool Brazil_Exception_Applicable( uint8_t year );

		static void Construct_Switch_Time( const DST_Switch_Time_Spec* p_switch_spec,
										   Time_Internal* p_switch_time, uint8_t year );

		static void Construct_Start_DST_Time( const DST_Switch_Time_Spec* p_switch_spec,
											  Time_Internal* p_switch_time, uint8_t year );

		static void Construct_End_DST_Time( const DST_Switch_Time_Spec* p_switch_spec,
											Time_Internal* p_switch_time, uint8_t year, uint8_t rule );

		static bool Validate_Time_Zone_Offset( const UTC_Offset* p_offset );

		static bool Convert_Time_From_Local_To_UTC_EU(
			const Time_Internal* p_local_time, Time_Internal* p_UTC_time,
			const Time_Zone_DST_Spec* p_zone_DST_spec );

		static bool Convert_Time_From_UTC_To_Local_EU(
			const Time_Internal* p_UTC_time, Time_Internal* p_local_time,
			const Time_Zone_DST_Spec* p_zone_DST_spec );

		static bool Convert_Time_From_Local_To_UTC_Generic(
			const Time_Internal* p_local_time, Time_Internal* p_UTC_time,
			const Time_Zone_DST_Spec* p_zone_DST_spec,
			bool time_zone_adjustment /*=true*/ );

		static bool Convert_Time_From_UTC_To_Local_Generic(
			const Time_Internal* p_UTC_time, Time_Internal* p_local_time,
			const Time_Zone_DST_Spec* p_zone_DST_spec,
			bool time_zone_adjustment /*=true*/ );

		static const uint8_t US_DST_CHANGE_YEAR = 7U;
		// US DST rule changed in (20)07

		static const uint8_t DEFAULT_DST_RULE_COUNTRY = DST_ST_RULE_US;
};

}

#endif	// TIME_ZONE_DST_H
