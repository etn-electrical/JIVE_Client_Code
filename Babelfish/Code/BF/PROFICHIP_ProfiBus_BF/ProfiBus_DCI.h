/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PROFIBUS_DCI_H
	#define PROFIBUS_DCI_H

#include "DCI_Defines.h"
#include "DCI_Data.h"
#include "DCI_Owner.h"
#include "DCI_Patron.h"
#include "DCI_ProfiBus_Data.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define     PROFI_ERR_NO_ERROR                DCI_ERR_NO_ERROR
#define     PROFI_ERR_INVALID_COMMAND         DCI_ERR_INVALID_COMMAND	///< Invalid command was requested.
#define     PROFI_ERR_INVALID_PROFI_ID        DCI_ERR_INVALID_DATA_ID	///< Data ID does not exist.
#define     PROFI_ERR_ACCESS_VIOLATION        DCI_ERR_ACCESS_VIOLATION	///< Target data does not exist.  Null ram ptr.
#define     PROFI_ERR_DATA_EXCEEDS_RANGE      DCI_ERR_EXCEEDS_RANGE		///< The set value resulted in a range error.
#define     PROFI_ERR_RAM_READ_ONLY           DCI_ERR_RAM_READ_ONLY		///< This value is read only.  No write access
																		///< is allowed.
#define     PROFI_ERR_NV_NOT_AVAILABLE        DCI_ERR_NV_NOT_AVAILABLE	///< NV Value does not exist or is inaccessible.
#define     PROFI_ERR_NV_ACCESS_FAILURE       DCI_ERR_NV_ACCESS_FAILURE	///< NV is Busy or Broken.
#define     PROFI_ERR_NV_READ_ONLY            DCI_ERR_NV_READ_ONLY		///< The NV value is listed as read only.
#define     PROFI_ERR_DEFAULT_NOT_AVAILABLE   DCI_ERR_DEFAULT_NOT_AVAILABLE	///< The Default is not available.
#define     PROFI_ERR_RANGE_NOT_AVAILABLE     DCI_ERR_RANGE_NOT_AVAILABLE	///< The Range is not available.
#define     PROFI_ERR_INVALID_DATA_LENGTH     DCI_ERR_INVALID_DATA_LENGTH	///< The data length is invalid.
#define     PROFI_ERR_DATA_LOCKED             DCI_ERR_VALUE_LOCKED			///< The data value has been locked and is
																			///< not accessible right now.  Please come
																			///< back later.

#define     ONE_BYTE                        1
#define     TWO_BYTES                       2
#define     FOUR_BYTES                      4
#define     EIGHT_BYTES                     8


typedef uint16_t PROFI_ID_TD;
typedef uint16_t MOD_NUM_TD;

enum
{
	BIG_ENDIAN_FORMAT,
	LITTLE_ENDIAN_FORMAT
};

#define     MOTOROLA_FORMAT         BIG_ENDIAN_FORMAT
#define     INTEL_FORMAT            LITTLE_ENDIAN_FORMAT
#define     DCI_FORMAT              LITTLE_ENDIAN_FORMAT




/*
 *****************************************************************************************
 *		Structures
 *****************************************************************************************
 */


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class ProfiBus_DCI
{
	public:
		ProfiBus_DCI( DCI_SOURCE_ID_TD unique_id );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~ProfiBus_DCI( void ) {}


		/**
		 * Provides "GET" data access to single PROFIBUS_DCI parameter. Data bytes are swapped before being copied
		 */
		DCI_ERROR_TD Get_Profi_Data( PROFI_ID_TD profi_id, DCI_LENGTH_TD* length_ptr, uint8_t* dest_data_ptr );

		/**
		 * Provides "GET" data access to single PROFIBUS_DCI parameter. Data bytes are NOT swapped before being copied
		 */
		DCI_ERROR_TD Get_DCI_Data( PROFI_ID_TD profi_id, DCI_LENGTH_TD* length_ptr, uint8_t* dest_data_ptr );

		/**
		 * Provides "SET" data access to single PROFIBUS_DCI parameter.  Sets Initial and RAM value
		 */
		DCI_ERROR_TD Set_Profi_Prm_Data( PROFI_ID_TD profi_id, DCI_LENGTH_TD length, uint8_t* src_data_ptr );

		/**
		 * Provides "SET" data access to single PROFIBUS_DCI parameter.  Sets only RAM value
		 */
		DCI_ERROR_TD Set_Profi_IO_Data( PROFI_ID_TD profi_id, DCI_LENGTH_TD length, uint8_t* src_data_ptr );

		/**
		 * Provides the data length associated with a DCID.
		 */
		DCI_LENGTH_TD Get_Length( PROFI_ID_TD profi_id ) const;

		/**
		 * Provides the datatype length associated with a DCID.
		 */
		DCI_LENGTH_TD Get_Datatype_Length( PROFI_ID_TD profi_id ) const;

		/**
		 * Provides access to the DCID associated with the passed in profibus module ID.
		 * 0xFFFF means it was not found.
		 */
		static DCI_ID_TD Find_Profi_Mod_DCID( PROFI_ID_TD profibus_id );

		/**
		 * Provides access to the PROFI_ID associated with the passed in DCI ID.
		 * 0xFFFF means it was not found.
		 */
		static PROFI_ID_TD Find_Profi_Mod_ID( DCI_ID_TD dci_id );

		/**
		 * Swaps data from little endian to big endian or from big endian to little endian.
		 */
		void Swap_Data_Bytes( uint8_t data_type_len, DCI_LENGTH_TD data_len, uint8_t* data ) const;

	private:

		DCI_SOURCE_ID_TD m_source_id;
		DCI_Patron* m_dci_access;


};

#endif
