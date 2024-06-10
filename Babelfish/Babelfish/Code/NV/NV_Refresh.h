/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module is responsible for refreshing the NV Memory on a database upgrade.
 *
 *	@details This is applicable whenever there are new NV parameters added over last few firmware release/s. It will
 * update *  the new parameter values without need of factory reset & without impacting existing parameter values.
 *	This is typically used once the product is released to the field.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef NV_REFRESH_H
   #define NV_REFRESH_H

#include "Includes.h"
#include "DCI.h"

/**
 ****************************************************************************************
 * @brief This module is responsible for refreshing the NV Memory on a database upgrade.
 *
 * @details This is used to update parameters which have been added or changed in a
 *	firmware upgrade.  This is typically used once the product is released to the field.
 *
 ****************************************************************************************
 */
class NV_Refresh
{
	public:
		/**
		 * @brief Structure used to create a single revision update.
		 * the list
		 */
		struct data_update_t
		{
			uint32_t revision;
			uint32_t num_items;
			DCI_ID_TD const* dci_list;
		};

		/**
		 * @brief Structure used to package up a full list of data updates.
		 * the list
		 */
		struct data_update_package_t
		{
			data_update_t const* rev_update_list;
			uint16_t num_items;
		};

		/**
		 * @brief Will work through the list of parameters and update the ones which are
		 * 		tagged with a revision greater than the current FW revision.
		 * @param previous_fw_rev: The previous firmware revision.  This is typically
		 * 		stored in NV memory and updated on a change.
		 * @param rev_update: The list of revision numbers and number of items.
		 * @return Whether changes were actually made.  Typically this should indicate whether we need
		 * to perform a soft reset or not.  If true we should probably do a soft reset.
		 */
		static bool Update( uint32_t previous_fw_rev, data_update_package_t const* rev_update );

	protected:

	private:
		NV_Refresh( void ) {}

		~NV_Refresh( void ) {}

		/**
		 * @brief Apply NV Refresh for all DCIs list which are available in rev_update_list.
		 * @param rev_update_list: revision update list with DCIs.
		 */
		static bool Apply_NV_Refresh( data_update_t const* rev_update_list );

		/**
		 * @brief Find next revision index.
		 * @param check_fw_rev: previous firmware revision
		 * @param num_items: number of firmware revision
		 * @param max_fw_rev_index: maximum firmware revision index
		 * @param rev_data_list: revision data list
		 */
		static uint_fast16_t Find_Next_Revision(
			uint32_t check_fw_rev,
			uint16_t num_items,
			uint_fast16_t max_fw_rev_index,
			data_update_t const* rev_data_list );

		/**
		 * @brief Find max revision.
		 * @param num_items: number of firmware revision
		 * @param rev_data_list: revision data list
		 */
		static uint_fast16_t Find_Max_Revision(
			uint16_t num_items,
			data_update_t const* rev_data_list );

		/**
		 * @brief Apply NV refresh for all .
		 * @param previous_fw_rev: previous firmware revision
		 * @param num_items: number of firmware revision
		 * @param max_fw_rev_index: maximum firmware revision index
		 * @param rev_data_list: revision data list
		 */
		static bool Apply_All_NV_Refresh(
			uint32_t previous_fw_rev,
			uint16_t num_items,
			uint_fast16_t max_fw_rev_index,
			data_update_t const* rev_data_list );

		// Prevent Default Constructors (move to public if necessary)
		NV_Refresh( const NV_Refresh& T );
		NV_Refresh( NV_Refresh& T );

		NV_Refresh & operator =( NV_Refresh& rhbs );

		NV_Refresh & operator =( const NV_Refresh& rhbs );

};


#endif
