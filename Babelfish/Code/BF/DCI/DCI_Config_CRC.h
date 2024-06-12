/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details Interface used to watch a set of DCI ID parameter values for changes through a CRC
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_CONFIG_CRC_H
#define DCI_CONFIG_CRC_H

#include "Bit.hpp"
#include "Etn_Types.h"
#include "CR_Tasker.h"
#include "Change_Tracker.h"
#include "DCI_Defines.h"
#include "DCI_Owner.h"

/**
 * @brief This is a DCI_Config_CRC class
 * @details It provides an interface to watch a set of DCI ID parameter values for changes through a
 * CRC
 * @n @b Usage:
 * @n It provides public methods to
 * @n @b Get CRC , store CRC in NV and loading the CRC value from NV to RAM.
 * @n @b If the CRC is undefined it will return CRC_CALC_INCOMPLETE
 *
 */
class DCI_Config_CRC
{
	public:

		struct crc_list_t
		{
			DCI_ID_TD const* dci_list;
			DCI_ID_TD dci_list_len;
		};

		/**
		 *  @brief constructor
		 *  @details This interface allows user to watch a list of DCI IDs or all the DCI IDs.
		 *  a CRC is computed on the watched list of DCI IDs and updated when any of the parameters
		 *  in the list changes.
		 *
		 *  @param[in] config_crc_owner: DCI ID of the parameter that stores the CRC value
		 *  of the watched list.
		 *  @n @b Usage: Valid DCI ID
		 *  @param[in] watch_list: is a list of DCI ID's that needs to be watched.
		 *  @n @b Usage: Non-NULL - only watches DCI IDs in the list.
		 *  @n @b Usage: NULL - Entire range of valid DCI IDs shall be watched.
		 *  @param[in] list_length: is the length of data being passed in the watch list.
		 *  @n @b Usage: Number of DCI IDs in the 'watch_list'.
		 *               Ignored if 'watch_list' is null.
		 *  @return: none
		 */
		DCI_Config_CRC( DCI_ID_TD config_crc_owner,
						crc_list_t const* crc_list =
						reinterpret_cast<crc_list_t const*>( nullptr ) );

		/**
		 *  @brief : destructor
		 *  @return: none
		 */
		~DCI_Config_CRC( void );

		/**
		 * An invalid value shall be returned when the CRC is incomplete.  The CRC
		 * calculation takes a considerable amount of time.  To signal that the value is
		 * still being calculated we return the incomplete value.  This value is not allowed
		 * to be returned as a valid CRC.  An additional calculation shall be done to avoid
		 * this value if necessary.
		 */
		static const uint16_t CRC_CALC_INCOMPLETE = 0xffffU;

		/**
		 *   @brief : Returns the current CRC of the watched list.  If the CRC is
		 *   equal to CRC_CALC_INCOMPLETE then the value is still being calculated.
		 *   This shall be documented to the user.
		 *   @return: The CRC value
		 */
		uint16_t Get_CRC( void ) const;

		/**
		 *  @brief Stores the current CRC into the NV
		 *  @return Returns true on success else false
		 *	@retval true - Success
		 *	@retval false - Failure
		 */
		bool Store_CRC( void ) const;

		/**
		 *  @brief Loads init value of the CRC from NV into RAM
		 *  @return Returns true on success else false
		 *	@retval true - Success
		 *	@retval false - Failure
		 */
		bool Retrieve_Stored_CRC( uint16_t* crc ) const;

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		DCI_Config_CRC( const DCI_Config_CRC& object );
		DCI_Config_CRC & operator =( const DCI_Config_CRC& object );

		static const uint16_t CRC_INIT = 0U;

		void Trigger_CRC_Update( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Trigger_CRC_Update_Static( Change_Tracker::cback_param_t param,
											   DCI_ID_TD dci_id,
											   Change_Tracker::attrib_mask_t attribute_mask )
		{
			if ( BF_Lib::Bit::Test( attribute_mask, Change_Tracker::NV_ATTRIB ) )
			{
				( ( DCI_Config_CRC* )param )->Trigger_CRC_Update();
			}
		}

		void Update_Config_CRC_uTask( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Update_Config_CRC_uTask_Static( CR_Tasker* tasker, CR_TASKER_PARAM param )
		{
			( ( DCI_Config_CRC* )param )->Update_Config_CRC_uTask();
		}

		DCI_Owner* m_config_crc;
		uint16_t m_cumulative_crc;
		DCI_ID_TD m_crc_calc_dci_index;
		Change_Tracker* m_change_tracker;
		CR_Tasker* m_cr_task;
};

#endif
