/**
 *****************************************************************************************
 *  @file
 *	@brief This file provides the Patron interfaces to access DCI Database
 *
 *	@details DCI Patrons are normally the outsiders who are interested in the DCI parameters
 *	but actually doesn't own it.
 *  @n DCI - Data store
 *  @n DCI Owners - Owners of the store and hence has more privileges
 *  @n DCI Patron - Customers of the store which get access to DCI depending on their membership
 *   attributes (Patron attributes)
 *  @n DCI Worker - Workers in the store which has more access than Patron but lesser access than
 * owners
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_PATRON_H
#define DCI_PATRON_H

#include "DCI_Defines.h"
#include "DCI_Data.h"
#include "DCI.h"
#include "DCI_Source_IDs.h"
#include "Change_Tracker.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
typedef DCI_PATRON_ATTRIB_TD DCI_PATRON_ATTRIB_SUPPORT_TD;

// The following are bit numbers to be used in a Test Bit.
static const uint8_t DCI_PATRON_DCID_RAM_WR_SUPPORT = DCI_PATRON_ATTRIB_RAM_WR_DATA;
static const uint8_t DCI_PATRON_DCID_INIT_SUPPORT = DCI_PATRON_ATTRIB_NVMEM_DATA;
static const uint8_t DCI_PATRON_DCID_INIT_WR_SUPPORT = DCI_PATRON_ATTRIB_NVMEM_WR_DATA;
static const uint8_t DCI_PATRON_DCID_DEFAULT_SUPPORT = DCI_PATRON_ATTRIB_DEFAULT_DATA;
static const uint8_t DCI_PATRON_DCID_RANGE_SUPPORT = DCI_PATRON_ATTRIB_RANGE_DATA;
static const uint8_t DCI_PATRON_DCID_ENUM_SUPPORT = DCI_PATRON_ATTRIB_ENUM_DATA;

/**
 *
 * @brief This is a DCI_Patron class
 * @details It provides an interface to Create the Patron, provide the data access to DC database
 * @n @b Usage: Use DCI Patron whenever access to DCI data bases is needed from an DCI Patron point
 * of view
 * It provides public methods to
 * @n @b 1. To Create the Patron
 * @n @b 2. To access data using Patron, to check if access_struct is correct, access_struct acts as
 * an order slip, so Patron fills the order slip and its order is being validated by DCI against
 * proper data_id, range, length, size etc and then its processed
 *
 */
class DCI_Patron
{
	public:
		/**
		 *  @brief Constructs a DCI Patron interface.
		 *  @details Creates an instance of a patron interface to gain access to the database.
		 *  @param[in] correct_out_of_range_sets: Determines whether the out of range
		 *  values should be corrected or return error when incorrect.
		 *  @n @b Usage: true means if the DCI value written using patron falls out of range then
		 * min or max value will be written
		 *  @n @b Usage: False means if the DCI value written using patron falls out of range then
		 * error will be returned
		 *  @param[in] owner_access: Creates a privileged access point if owner access is true.
		 *  Otherwise this workers class will act as a patron.
		 *  @param[in] dci_lock: A pointer to a DCI Lock instance.  The DCI Worker will not
		 *  change the lock but will use only for read access.  Any lock/unlock of this parameter
		 *  must be done externally.
		 *  @param[in] change_tracker_to_avoid: To avoid an instance where it is desired to avoid
		 *  kicking the change tracker on an action taken by the patron user, it is possible to
		 *  attach the change tracker which should not be kicked.
		 *  @return None
		 */
		DCI_Patron( bool correct_out_of_range_sets = false,
					BF::DCI_Lock* dci_lock = nullptr,
					Change_Tracker const* change_tracker_to_avoid = nullptr );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of DCI_Patron goes out of scope
		 *  @return None
		 */
		~DCI_Patron( void ) {}

		/**
		 *  @brief Provides main access to database for Patron
		 *  @details This is analogous to filling out an order slip first and depending on order it
		 * will be processed.
		 *  @param[in,out] access_struct The access struct is the order slip.  The order slip will
		 * be verified for
		 * accuracy then passed down to the DCI to extract/set the data.
		 *  @return If successful returns DCI_ERR_NO_ERROR else returns below mentione error codes
		 *  @retval DCI_ERR_INVALID_COMMAND Access command is invalid
		 *  @retval DCI_ERR_ACCESS_VIOLATION Patron doesn't have access attribute
		 *  @retval DCI_ERR_INVALID_DATA_LENGTH Data length is not valid
		 *  @retval DCI_ERR_INVALID_DATA_ID Data Id passed is invalid
		 */
		DCI_ERROR_TD Data_Access( DCI_ACCESS_ST_TD* access_struct );

		/**
		 *  @brief
		 *  @details Provides a method to verify the general qualities of the access struct.
		 * Main purpose is to provide callback functions a quick method of verifying that the access
		 * struct has the correct information and meets the spirit of the ID being accessed.
		 * This is analogous to filling out an order slip.
		 * @param[in,out] The access struct is the order slip.
		 * The order slip will be verified for accuracy then passed down to the
		 * DCI to extract/set the data.
		 *  @return Returns DCI_ERR_NO_ERROR else returns following error codes
		 *  @retval DCI_ERR_RAM_READ_ONLY Read Only alue
		 *  @retval DCI_ERR_EXCEEDS_RANGE Out of range
		 *  @retval DCI_ERR_INVALID_DATA_LENGTH data length not valid
		 *  @retval DCI_ERR_NV_NOT_AVAILABLE NV attribute support not available
		 *  @retval DCI_ERR_NV_READ_ONLY NV value is not modifiable
		 *  @retval DCI_ERR_DEFAULT_NOT_AVAILABLE default attribute support not present
		 *  @retval DCI_ERR_RANGE_NOT_AVAILABLE range support not available
		 *  @retval DCI_ERR_ENUM_NOT_AVAILABLE enum support not available
		 *  @retval DCI_ERR_INVALID_COMMAND command given is not valid
		 */
		DCI_ERROR_TD Error_Check_Access( DCI_ACCESS_ST_TD* access_struct );

		/**
		 *  @brief
		 *  @details Retrieves the length of the requested data id.
		 *  @param[in] data_id
		 *  @param[out] length Fetches back the length of the given DCI ID
		 *  @return If successful returns DCI_ERR_NO_ERROR else returns DCI_ERR_INVALID_DATA_ID DCI
		 * ID is not valid
		 */
		DCI_ERROR_TD Get_Length( DCI_ID_TD data_id, DCI_LENGTH_TD* length ) const;

		/**
		 *  @brief
		 *  @details Retrieves the data id datatype
		 *  @param[in] data_id DCI parameter ID
		 *  @param[out] datatype Fetches back the data type of the given DCI ID
		 *  @return If successful returns DCI_ERR_NO_ERROR else returns DCI_ERR_INVALID_DATA_ID DCI
		 * ID is not valid
		 */
		DCI_ERROR_TD Get_Datatype( DCI_ID_TD data_id, DCI_DATATYPE_TD* datatype ) const;

		/**
		 *  @brief
		 *  @details Retrieves the length of the datatype associated with the data id.
		 *  @param[in] data_id DCI parameter ID
		 *  @param[out] datatype_size Fetches back the size of the data type of the given DCI ID
		 *  @return If successful returns DCI_ERR_NO_ERROR else returns DCI_ERR_INVALID_DATA_ID DCI
		 * ID is not valid
		 */
		DCI_ERROR_TD Get_Datatype_Length( DCI_ID_TD data_id,
										  DCI_DATATYPE_SIZE_TD* datatype_size ) const;

		/**
		 *  @brief
		 *  @details Retrieves the attribute support bitfield.
		 *  @param[in] data_id DCI parameter ID
		 *  @param[out] attrib_support Fetches back the attribute support for the given DCI ID
		 *  @return If successful returns DCI_ERR_NO_ERROR else returns DCI_ERR_INVALID_DATA_ID DCI
		 * ID is not valid
		 */
		DCI_ERROR_TD Get_Attribute_Support( DCI_ID_TD data_id,
											DCI_PATRON_ATTRIB_SUPPORT_TD* attrib_support ) const;

		/**
		 *  @brief
		 *  @details A change tracker can be added to track parameters. The change tracker will be
		 * triggered whenever the value is changed outside
		 *  of this patron.
		 *  @see Change_Tracker.h
		 *  @return none
		 */
		void Attach_Tracker( Change_Tracker* change_tracker )
		{
			m_change_tracker = change_tracker;
		}

	private:
		static const DCI_CB_MSK_TD SKIP_LENGTH_CHECKING_MSK;

		DCI_ERROR_TD Get_RAM_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Set_RAM_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block );

		DCI_ERROR_TD Get_Length_Data( DCI_ACCESS_ST_TD* access_struct,
									  DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Attrib_Support_Data( DCI_ACCESS_ST_TD* access_struct,
											  DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Init_Data( DCI_ACCESS_ST_TD* access_struct,
									DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Set_Init_Data( DCI_ACCESS_ST_TD* access_struct,
									DCI_DATA_BLOCK_TD const* data_block );

		DCI_ERROR_TD Get_Min_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Max_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Enum_List_Length( DCI_ACCESS_ST_TD* access_struct,
										   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Enum_Data( DCI_ACCESS_ST_TD* access_struct,
									DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Default_Data( DCI_ACCESS_ST_TD* access_struct,
									   DCI_DATA_BLOCK_TD const* data_block ) const;

		/// The following commands perform data movement.  It will do background loads of the passed
		// DCID.
		DCI_ERROR_TD Load_Current_To_Init( DCI_ACCESS_ST_TD* access_struct,
										   DCI_DATA_BLOCK_TD const* data_block );

		DCI_ERROR_TD Load_Default_To_Init( DCI_ACCESS_ST_TD* access_struct,
										   DCI_DATA_BLOCK_TD const* data_block );

		DCI_ERROR_TD Load_Default_To_Current( DCI_ACCESS_ST_TD* access_struct,
											  DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Load_Init_To_Current( DCI_ACCESS_ST_TD* access_struct,
										   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Clear_Ram( DCI_ACCESS_ST_TD* access_struct,
								DCI_DATA_BLOCK_TD const* data_block );

		DCI_ERROR_TD Clear_Init( DCI_ACCESS_ST_TD* access_struct,
								 DCI_DATA_BLOCK_TD const* data_block );

		DCI_LENGTH_TD Trim_Offset_To_Single_Datatype( DCI_DATA_BLOCK_TD const* data_block,
													  DCI_LENGTH_TD offset_to_trim ) const;

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool Good_Alignment( DCI_LENGTH_TD offset, DCI_DATATYPE_SIZE_TD datatype_size )
		{
			return ( ( offset & ( ( datatype_size ) - 1U ) ) == 0U );
		}

		Change_Tracker const* m_change_tracker;
		bool m_correct_out_of_range_sets;
		BF::DCI_Lock* m_dci_lock;
};


#endif
