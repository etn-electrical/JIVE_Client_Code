/**
 *****************************************************************************************
 *  @file
 *	@brief This file provides the DCI interface to Alarms.
 *
 *	@details Alarm_DCI contains all the necessary structures and methods to access the DCI interface.
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef ALARM_DCI_H
#define ALARM_DCI_H
#include "DCI_Patron.h"

///< This structure is used to support both - DCID configured delay and user-defined delay
struct alarms_delay_t
{
	bool is_dci;// Flag that indicates whether delay is populated by DCID or user defined
	union
	{
		uint16_t non_dci_delay;	// Delay configured in DCI descriptors sheet e.g DCI_AE_DELAY0
		DCI_ID_TD dci_delay;	// Hardcoded user defined value e.g 100
	};

};

///< This structure defines severity level and its publishing cadence
struct severity_t
{
	const char* severity_level_name;
	DCI_ID_TD publishing_cadence;
};

///< This structure defines the ALARM_DCI to DCID lookup for one rule.
typedef struct dci_alarms_to_dcid_lkup_st_t
{
	const char* rule_name;	///< Free-form alarm rule name
	const char* message;///< Free-form alarm message
	uint8_t severity_level_index;	///< Index of severity_level_table[]
	DCI_ID_TD priority;	///< DCID of priority
	alarms_delay_t delay;	///< Either DCI_ID_TD type or uint16_t types decided by is_dci flag
} alarm_rule_config_struct_t;

/** This structure defines the Alarm target in the DCI database.
 * This is the container for the list of all Alarm rules in the DCI and their metadate
 */
struct alarm_target_info_st_t
{
	uint8_t total_severity_levels;
	uint16_t total_alarm_rules;
	const dci_alarms_to_dcid_lkup_st_t* alarm_rules_list;
	const severity_t* severity_level_list;
};


/**
 ****************************************************************************************
 * @brief This is an Alarm_DCI class
 * @details It provides an interface between alarms and DCID.
 * @ingroup alarm_dci_config
 *
 * @li It provides public methods to
 * @li 1. Initialize an Alarm_DCI connection.
 * @li 2. Find the total number of rules available.
 * @li 3. Find the total number of severity levels available.
 * @li 4. Get severity level config..
 * @li 5. Get alarm rule config.
 ****************************************************************************************
 */
class Alarm_DCI
{
	public:
		/**
		 * @brief Constructor.
		 * @details Initialize an Alarm_DCI connection
		 * @param alarm_target_info: Defines the alarm target, it is a container for the entire identity
		 * of the Alarm_DCI interface.
		 */
		Alarm_DCI( const alarm_target_info_st_t* alarm_target_info );

		~Alarm_DCI( void );

		/**
		 * @brief Find the total number of rules available.
		 * @details This function returns the total number of rules available.
		 * @return number of rules.
		 */
		uint16_t Get_Total_Rules( void ) const;

		/**
		 * @brief Find the total number of severity levels available.
		 * @details This function returns the total number of severity levels available.
		 * @return number of severity levels.
		 */
		uint16_t Get_Total_Severity_Levels( void ) const;

		/**
		 * @brief Get the severity level configuration, given its index.
		 * @details This is a convenience function; looks up the severity levels table in
		 * 			severity_level_table[TOTAL_SEVERITY_LEVELS] from the given index.
		 * @param index The index of the desired severity level in the table
		 * @return Pointer to Struct containing severity level and its publishing cadence, or nullptr if
		 * 			the index is out of range.
		 */
		const severity_t* Get_Severity_Level_Config( uint8_t severity_level_index ) const;

		/**
		 * @brief Get the rule configuration, given its index.
		 * @details This is a convenience function; looks up the rules config in structure
		 * 			alarm_target_info_st_t element alarm_rules_list
		 * @param index The index of the desired alarm rule in the alarm_rules_list array
		 * @return Pointer to alarm rule configuration, or nullptr if
		 * 			the index is out of range.
		 */
		const alarm_rule_config_struct_t* Get_Rule_Config( uint8_t index ) const;

		/**
		 * @brief Check whether delay is DCI type or user-defined(hardcoded), given its index.
		 * @details This is a convenience function; looks up the rules config in structure
		 * 			alarm_target_info_st_t element alarm_rules_list and returns whether delay
		 * 			is DCI type or user-defined(hardcoded)
		 * @param index The index of the desired alarm rule in the alarm_rules_list array
		 * @return True if delay is DCI type, else false
		 */
		bool Is_Delay_DCI_Type( uint8_t index );

	private:

		const alarm_target_info_st_t* m_alarm_struct;

};


#endif
