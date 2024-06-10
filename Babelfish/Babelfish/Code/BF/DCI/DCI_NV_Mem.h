/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details This module is responsible for NV memory access.  It provides a
 *			fragmented memory access method for the DCI.
 *			- One area of concern could be the fact that lengths are not locked out.
 *			The odds are VERY VERY SLIM it will ever be an issue.
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_NV_MEM_H
#define DCI_NV_MEM_H

#include "DCI_Defines.h"
#include "NV_Address.h"
#include "DCI_Owner.h"

/*
 **************************************************************************************************
 *  Constants
 **************************************************************************************************
 */
#define DCI_NV_MEM_FACTORY_RESET_DONE       0x55

/**
 ****************************************************************************************
 * @brief This is a DCI_NV_Mem class
 * @details It provides an interface to access NV memory for DCI
 * @n @b Usage: It provides public methods to
 * @n @b 1. To Read/Write from/to NV memory using DCI Patron and Owners
 ****************************************************************************************
 */
class DCI_NV_Mem
{
	public:
		/**
		 *  @brief constructor
		 *  @details The key feature is fragmented access to data stored.  This is done by retrieving the
		 * data from NV - modifying it then writing it back.
		 *  @param[in] factory_reset_done_key
		 *  @n @b Usage:
		 *  @param[in] buffer_sizecontrols the amount of buffer space for variable storage
		 *  @n @b Usage:
		 *  @return none
		 */
		static void Init( uint8_t factory_reset_done_key = DCI_NV_MEM_FACTORY_RESET_DONE,
						  uint32_t buffer_size = NV_MAX_DATA_VAL_LENGTH );

		/**
		 *  @brief Reads the data from NV memory using Patron
		 *  @details This is used for the instances where we need to find out if the ram value is going to be changed.
		 *  For patron access this functionality will likely never be used.  It is here for balance.
		 *  @param[out] data_access Fetches back the data read from NV memory
		 *  @n @b Usage:
		 *  @param[in] data_block
		 *  @n @b Usage:
		 *  @param[out] ram_data_changed  Ram_data_changed indicates whether or not the ram data was different than the
		 * nv memory
		 *  @n @b Usage: True data is changed else not changed
		 *  @return DCI_ERROR_TD
		 *  @retval DCI_ERR_NV_ACCESS_FAILURE if fails to access the NV data
		 *  @retval DCI_ERR_NO_ERROR Able to access NV data successfully
		 */
		static DCI_ERROR_TD Patron_Read( DCI_ACCESS_DATA_TD* data_access,
										 DCI_DATA_BLOCK_TD const* data_block,
										 bool* ram_data_changed = NULL );

		/**
		 *  @brief Writes the NV data using Patron
		 *  @details Interface to store the data in the NV memory using Patron.
		 *  @param[out] data_access
		 *  @n @b Usage:
		 *  @param[in] data_block
		 *  @n @b Usage:
		 *  @param[in] fix_value
		 *  @n @b Usage: if true Source data range will be checked and
		 *  if it falls out of range it will be fixed to min or max value and out_of_range error will not be returned
		 *  @n @b Usage: if false source data range will not be checked and if it falls out_of_range error will be
		 * returned
		 *  @param[out] nv_data_changed
		 *  @n @b Usage:true Means data was different than originally stored else false
		 *  @return DCI_ERROR_TD
		 *  @retval DCI_ERR_EXCEEDS_RANGE if fix_value is passed as true and data written falls out of range
		 *  @retval DCI_ERR_NV_ACCESS_FAILURE if there is no init support given to the DCI ID or source data size is
		 * more than NV mem buffer size
		 *
		 */
		static DCI_ERROR_TD Patron_Write( DCI_ACCESS_DATA_TD* data_access,
										  DCI_DATA_BLOCK_TD const* data_block, bool fix_value,
										  bool* nv_data_changed = NULL );

		/**
		 *  @brief Reads the data from NV memory using Owner
		 *  @details This is used for the instances where we need to find out if the ram value changed.
		 *  @param[out] data_ptr Fetches back the data read from NV memory
		 *  @n @b Usage:
		 *  @param[in] data_block
		 *  @n @b Usage:
		 *  @param[out] ram_data_changed  Ram_data_changed indicates whether or not the ram data was different than the
		 * nv memory
		 *  @n @b Usage: If we get True then data is changed else not changed
		 *  @param[in] offset
		 *  @n @b Usage:
		 *  @param[in] length
		 *  @n @b Usage:
		 *  @return Always returns True
		 */
		static bool Owner_Read( DCI_DATA_PASS_TD* data_ptr,
								DCI_DATA_BLOCK_TD const* data_block,
								bool* ram_data_changed = NULL,
								DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
								DCI_LENGTH_TD length = ACCESS_LENGTH_GET_ALL );

		/**
		 *  @brief Writes the NV data using Patron
		 *  @details Interface to store the data in the NV memory using Patron.
		 *  @param[out] data_ptr
		 *  @n @b Usage:
		 *  @param[in] data_block
		 *  @n @b Usage:
		 *  @param[out] nv_data_changed
		 *  @n @b Usage:true Means data was different than originally stored else false
		 *  @param[in] offset
		 *  @n@b Usage:
		 *  @param[in] length
		 *  @n@b Usage:
		 *  @return True means  data is written successfully else there was a issue while writing data using
		 * NV_Mem::Write()
		 *
		 */
		static bool Owner_Write( DCI_DATA_PASS_TD const* data_ptr,
								 DCI_DATA_BLOCK_TD const* data_block,
								 bool* nv_data_changed = NULL,
								 DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
								 DCI_LENGTH_TD length = ACCESS_LENGTH_SET_ALL );

	private:
		/**
		 * Constructor, Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		DCI_NV_Mem( void );
		~DCI_NV_Mem( void );
		DCI_NV_Mem( const DCI_NV_Mem& rhs );
		DCI_NV_Mem & operator =( const DCI_NV_Mem& object );

		static uint8_t* m_frag_buffer;
		static uint32_t m_buffer_size;

};

#endif
