/**
 *****************************************************************************************
 *   @file
 *   @details See header file for module overview.
 *   @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "NV_Refresh.h"

/**
 *****************************************************************************************
 * @brief See header file for function definition.
 *****************************************************************************************
 */
uint_fast16_t NV_Refresh::Find_Max_Revision(
	uint16_t num_items,
	data_update_t const* rev_data_list )
{
	uint_fast16_t max_fw_rev_index = 0U;

	/* Find Maximum Firmware Index from rev_update_list num_items array */
	for ( uint_fast16_t i = 0U; i < num_items; i++ )
	{
		if ( rev_data_list[i].revision > rev_data_list[max_fw_rev_index].revision )
		{
			max_fw_rev_index = i;
		}
	}
	return ( max_fw_rev_index );
}

/**
 *****************************************************************************************
 * @brief See header file for function definition.
 *****************************************************************************************
 */
uint_fast16_t NV_Refresh::Find_Next_Revision(
	uint32_t check_fw_rev,
	uint16_t num_items,
	uint_fast16_t max_fw_rev_index,
	data_update_t const* rev_data_list )
{
	uint_fast16_t current_rev_list_index = max_fw_rev_index;

	for ( uint_fast16_t i = 0U; i < num_items; i++ )
	{
		/* To find next revision, compare rev_data_list revision with previous firmware and rev_data_list's max firmware
		   index with rev_data_list for loop current index*/
		if ( ( rev_data_list[i].revision > check_fw_rev ) &&
			 ( rev_data_list[current_rev_list_index].revision > rev_data_list[i].revision ) )
		{
			current_rev_list_index = i;
		}
	}
	return ( current_rev_list_index );
}

/**
 *****************************************************************************************
 * @brief See header file for function definition.
 *****************************************************************************************
 */
bool NV_Refresh::Apply_NV_Refresh( data_update_t const* rev_update_list )
{
	bool values_changed = false;

	/* Factory Reset of num_items DCID */
	for ( uint_fast16_t k = 0U; k < rev_update_list->num_items; k++ )
	{
		DCI::Factory_Reset_DCID( rev_update_list->dci_list[k] );
		values_changed = true;
	}
	return ( values_changed );
}

/**
 *****************************************************************************************
 * @brief See header file for function definition.
 *****************************************************************************************
 */
bool NV_Refresh::Apply_All_NV_Refresh(
	uint32_t previous_fw_rev,
	uint16_t num_items,
	uint_fast16_t max_fw_rev_index,
	data_update_t const* rev_data_list )
{
	bool values_changed = false;
	uint32_t check_fw_rev = previous_fw_rev;
	uint_fast16_t current_rev_list_index;

	do
	{
		/* Find current firmware revision index */
		current_rev_list_index = Find_Next_Revision( check_fw_rev, num_items, max_fw_rev_index, rev_data_list );

		/* if Current firmware revision is greater than previos firmware revision*/
		if ( rev_data_list[current_rev_list_index].revision > check_fw_rev )
		{
			/* go for factory reset*/
			values_changed = Apply_NV_Refresh( &rev_data_list[current_rev_list_index] );
		}
		check_fw_rev = rev_data_list[current_rev_list_index].revision;
	} while ( current_rev_list_index != max_fw_rev_index );
	return ( values_changed );
}

/**
 *****************************************************************************************
 * @brief See header file for function definition.
 *****************************************************************************************
 */
bool NV_Refresh::Update( uint32_t previous_fw_rev, data_update_package_t const* rev_update )
{
	bool values_changed = false;
	uint_fast16_t max_fw_rev_index;
	data_update_t const* rev_data_list = rev_update->rev_update_list;

	if ( nullptr != rev_data_list )
	{
		/* Find Maximum Firmware Index from rev_update_list Array */
		max_fw_rev_index = Find_Max_Revision( rev_update->num_items, rev_data_list );

		/* Get value changed of firmware revision */
		values_changed =
			Apply_All_NV_Refresh( previous_fw_rev, rev_update->num_items, max_fw_rev_index, rev_data_list );
	}
	return ( values_changed );
}
