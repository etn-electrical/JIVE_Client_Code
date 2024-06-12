/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Alarm_DCI.h"
#include "Alarms_DCI_Data.h"
#include "DCI_Defines.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Alarm_DCI::Alarm_DCI( const alarm_target_info_st_t* alarm_target_info ) :
	m_alarm_struct( alarm_target_info )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Alarm_DCI::Get_Total_Rules( void ) const
{
	return ( m_alarm_struct->total_alarm_rules );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Alarm_DCI::Get_Total_Severity_Levels( void ) const
{
	return ( m_alarm_struct->total_severity_levels );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const severity_t* Alarm_DCI::Get_Severity_Level_Config( uint8_t severity_level_index ) const
{
	static const severity_t* severity_levels_list = m_alarm_struct->severity_level_list;
	static const severity_t* severity_levels_config = nullptr;

	BF_ASSERT( severity_level_index < m_alarm_struct->total_severity_levels );
	severity_levels_config = &severity_levels_list[severity_level_index];
	return ( severity_levels_config );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const alarm_rule_config_struct_t* Alarm_DCI::Get_Rule_Config( uint8_t index ) const
{
	static const alarm_rule_config_struct_t* rule_list = m_alarm_struct->alarm_rules_list;
	static const alarm_rule_config_struct_t* rule_config = nullptr;

	BF_ASSERT( index < m_alarm_struct->total_alarm_rules );

	if ( index < m_alarm_struct->total_alarm_rules )
	{
		rule_config = &rule_list[index];
	}
	return ( rule_config );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Alarm_DCI::Is_Delay_DCI_Type( uint8_t index )
{
	static const alarm_rule_config_struct_t* rule_list = Get_Rule_Config( index );

	return ( rule_list[index].delay.is_dci );
}
