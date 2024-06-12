/**
 *****************************************************************************************
 *	@brief This file includes the interfaces for DCI Workers to access DCI Data base
 *
 *	@details
 *	@n @b DCI Workers are the frinds of the DCI owners,
 *        has lesser permissions than owners but more permissions than Patrons
 *	@n @b Usage :
 *	@n @b - DCI_Owner must be created first and then DCI Worker to be created for the same DCID's
 *	@n @b - DCI_Worker must not access data before DCI_Owner is created, if not assert hits during execution
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef DCI_WORKER_H_
#define DCI_WORKER_H_

#include "Includes.h"
#include "DCI_Defines.h"
#include "DCI_Data.h"
#include "DCI.h"
#include "OS_Queue.h"
#include "Change_Tracker.h"

/**
 ****************************************************************************************
 * @brief This is a DCI_Worker class
 * @details DCI Worker allocates and provides access to individual data items.
 * The DCI Worker also provides the necessary access functions to manipulate the data.
 * DCI Worker has less privilages than DCI owner and more privilages than DCI Patron
 * @n @b Usage: It provides public methods to
 * @n @b 1. Create DCI Worker, DCI Worker are essentially similar to DCI Owners but has lesser permissions
 ****************************************************************************************
 */
class DCI_Worker
{
	public:
		/// TODO:  Determine the usefulness of the following current writable option.
		// static const uint8_t CURRENT_WRITE = 0U;
		/// Range trusted means that any value checked in will NOT be checked.  Meaning that we are trusting the worker
		/// instance to verify range for itself or else understand that it has the power to check in a value
		/// out of range.  It is equivalent to saying to the creation of DCI_Worker "Trust me - I got this range thing
		/// either covered or I know what is best for you.  Just take what I give you."
		static const uint8_t RANGE_TRUSTED = 1U;
		static const uint8_t DEFAULT_WRITE = 2U;
		static const uint8_t INIT_WRITE = 3U;
		static const uint8_t RANGE_WRITE = 4U;
		static const uint8_t ENUM_WRITE = 5U;
		static const uint8_t END_OF_PERMISSION_LIST = 6U;

		typedef uint16_t worker_permission_t;
		static const worker_permission_t FULLY_TRUSTED = ( ( 1 << END_OF_PERMISSION_LIST ) - 1 );
		static const worker_permission_t NOT_TRUSTED = 0U;

		/// Quick bitmasks to make configuring the constructor a little easier.
		static const worker_permission_t RANGE_TRUSTED_BITMASK =
			static_cast<worker_permission_t>( 1 << RANGE_TRUSTED );
		static const worker_permission_t DEFAULT_WRITE_BITMASK =
			static_cast<worker_permission_t>( 1 << DEFAULT_WRITE );
		static const worker_permission_t INIT_WRITE_BITMASK =
			static_cast<worker_permission_t>( 1 << INIT_WRITE );
		static const worker_permission_t RANGE_WRITE_BITMASK =
			static_cast<worker_permission_t>( 1 << RANGE_WRITE );
		static const worker_permission_t ENUM_WRITE_BITMASK =
			static_cast<worker_permission_t>( 1 << ENUM_WRITE );

		/**
		 *  @brief Constructor
		 *  @details This will create the space associated with the data parameter.
		 *  @param[in] data_id DCI parameter id
		 *	@n @b Usage :
		 *	@n @b - DCI_Owner must be created first and then DCI Worker to be created for the same DCID's
		 *	@n @b - DCI_Worker must not access data before DCI_Owner is created, if not assert hits during execution
		 *	@n @b - Use valid DCI ID else this function will throw an exception
		 *  @param[in] privilages Access writes of the worker, by default it doesn't have any
		 *  @return None
		 */
		DCI_Worker( DCI_ID_TD dcid, worker_permission_t privileges = NOT_TRUSTED );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object DCI_Worker goes out of scope or deleted.
		 *  @return None
		 */
		~DCI_Worker();

		/**
		 *  @brief Method to change the permissions of a worker
		 *  @details It accesses the private variable m_privileges and updates it
		 *  @return None
		 */
		void Change_Permissions( worker_permission_t permissions );

		/**
		 *  @brief
		 *  @details Writes the value of ram_data to the DCI RAM data value
		 *  @param[in] ram_data Data to be copied to DCI parameter's RAM value
		 *	@n @b Usage: Use valid pointer to the data to be copied
		 *	@param[in] change_tracker_to_avoid
		 *  @return Always returns True
		 */
		bool Check_In( DCI_DATA_PASS_TD const* data,
					   Change_Tracker const* change_tracker_to_avoid );

		/**
		 *  @brief CURRENTLY NOT IMPLEMENTED
		 *  @details Check in the Null terminated string data
		 *  If data is changed notifies it as changed and triggers change tracker
		 *  @param[in] ram_data Data to be check against RAM value
		 *  @n @b Usage: Use valid pointer to the string to be copied
		 *  @todo confirm the description
		 *  @return Always returns True
		 */
		// bool Check_In_Null_String( char_t const* ram_data );

		/**
		 *  @brief
		 *  @details Checks out (reads) the RAM value
		 *  @param[out] ram_data In this parameter RAM value of DCI ID will be fetched back
		 *  @return Always returns true
		 */
		bool Check_Out( DCI_DATA_PASS_TD* ram_data ) const;

		/**
		 *  @brief CURRENTLY NOT IMPLEMENTED
		 *  @details Allows an Worker to check out one element at a time in an array
		 *  @param[out] ram_data In this parameter value present at given index will be fetched back
		 *  @n @b Usage: Always give valid index
		 *  @param[in] index Index is the native data type index.
		 *  @n @b Usage: For example given an array of 16bit variables to access the 3word, an index of 3 is passed in
		 *  @return Always returns true, but if invalid index is given then throws an exception
		 */
		// bool Check_Out_Index( DCI_DATA_PASS_TD* ram_data, DCI_LENGTH_TD index );

		/**
		 *  @brief CURRENTLY NOT IMPLEMENTED
		 *  @details Allows an Worker to check in one element at a time in an array
		 *  @param[in] ram_data Value at this pointer will be written to the index specified
		 *  @n @b Usage: Always give valid index
		 *  @param[in] index Index is the native data type index.
		 *  @n @b Usage: For example given an array of 16bit variables to access the 3word, an index of 3 is passed in
		 *  @return Always returns true, but if invalid index is given then throws an exception
		 */
		// bool Check_In_Index( DCI_DATA_PASS_TD const* ram_data, DCI_LENGTH_TD index );

		/**
		 *  @brief CURRENTLY NOT IMPLEMENTED
		 *  @details This function is intended for NULL terminated strings.
		 *  Ram data points at the destination and length initially defines the max length that can be
		 *
		 *  @param[out] ram_data This pointer fetches back the checked out data
		 *  @n @b Usage:
		 *  @param[in] length lenghth of data to be copied
		 *  @n @b Usage:  If length is left to point to null then the total string will be copied back with no length
		 * passed back
		 *  @return Always returns true
		 */
		// bool Check_Out_Null_String( DCI_DATA_PASS_TD* ram_data, DCI_LENGTH_TD* length = NULL );

		/**
		 *  @brief
		 *  @details Checks in (Writes) Init value
		 *  @param[in] ram_data Pointer to the date to be written
		 *	@param[in] change_tracker_to_avoid
		 *  @return true if Init value is written successfully else False
		 *  @retval true if Init value is written successfully
		 *  @retval false Init value support is not present or else not able to write the value in NV
		 */
		bool Check_In_Init( DCI_DATA_PASS_TD const* data,
							Change_Tracker const* change_tracker_to_avoid );

		/**
		 *  @brief
		 *  @details Check out ( Reads) back the Init value
		 *  First checks if there is a Init value attribute is present, if init value support is not given it returns
		 * false.
		 *  If init value support is given but if it fails to read the init value looks for default value and gets that
		 * value
		 *  @param[out] ram_data Pointer to a value to fetched back the Init value
		 *  @return true if Init value is read successfully else False
		 *  @retval true  If init vaue attribute is present
		 *  @retval false If Init value attribute is not present
		 */
		bool Check_Out_Init( DCI_DATA_PASS_TD* ram_data ) const;

		/**
		 *  @brief
		 *  @details Loads ram value into default. This only works with a writable default.
		 *  @param[in] ram_data Pointer to a value to be loaded in default value
		 *  @n @b Usage:
		 *	@param[in] change_tracker_to_avoid
		 *  @return true is default value RW attribute is present else returns False
		 */
		bool Check_In_Default( DCI_DATA_PASS_TD const* ram_data,
							   Change_Tracker const* change_tracker_to_avoid );

		/**
		 *  @brief
		 *  @details Loads default value into ram value.
		 *  @param[out] ram_data Pointer to a value to fetched back the default value
		 *  @n @b Usage:
		 *  @return true if default value is read successfully else Default attribute is not present.
		 */
		bool Check_Out_Default( DCI_DATA_PASS_TD* default_data ) const;

		/**
		 *  @brief
		 *  @details
		 *  @param[in] ram_data pointer to data to be written
		 *  @n @b Usage:
		 *	@param[in] change_tracker_to_avoid
		 *  @param[in] offset The byte offset into the block to begin checking in data
		 *  @n @b Usage:
		 *  @param[in] length if it is SET_ALL then data block length will be considered
		 *  @n @b Usage:
		 *  @return true if offset+lengh is lessthan or equal to the destination data block length else returns false
		 */
		bool Check_In_Offset( DCI_DATA_PASS_TD const* ram_data,
							  Change_Tracker const* change_tracker_to_avoid,
							  DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
							  DCI_LENGTH_TD length = ACCESS_LENGTH_SET_ALL );

		bool Check_Out_Offset( DCI_DATA_PASS_TD* ram_data,
							   DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
							   DCI_LENGTH_TD length = ACCESS_LENGTH_GET_ALL ) const;

		/**
		 * @brief
		 * @details This function will store the init value of the Worker beginning at the offset and continuing
		 * for length bytes
		 * @param ram_data[in] The data to be checked in of length length
		 * @n @b Usage:
		 * @param[in] change_tracker_to_avoid
		 * @param change_tracker_to_avoid[out]
		 * @param offset[in] The byte offset into the block to begin checking in data
		 * @n @b Usage:
		 * @param length[in] The byte length of the data to check in
		 * @n @b Usage:
		 * @return true For success, false for failure
		 */
		bool Check_In_Init_Offset( DCI_DATA_PASS_TD const* ram_data,
								   Change_Tracker const* change_tracker_to_avoid,
								   DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
								   DCI_LENGTH_TD length = ACCESS_LENGTH_SET_ALL );

		/**
		 * @brief
		 * @details This function will read the init value of the Worker beginning at the offset and continuing
		 * for length bytes
		 * @param ram_data[out] The data to be checked out of length length
		 * @n @b Usage:
		 * @param offset[in]
		 * @n @b Usage:
		 * @param length[in] The byte length of the data to check in
		 * @n @b Usage:
		 * @return true For success, false for failure
		 */
		bool Check_Out_Init_Offset( DCI_DATA_PASS_TD* ram_data, DCI_LENGTH_TD offset,
									DCI_LENGTH_TD length ) const;

		/**
		 * @brief
		 * @details This function will store the default value of the Worker beginning at the offset and continuing
		 * for length bytes
		 * @param default_data[in] The data to be checked in of length length
		 * @n @b Usage:
		 * @param[in] change_tracker_to_avoid
		 * @param offset[in] The byte offset into the block to begin checking in data
		 * @n @b Usage:
		 * @param length[in] The byte length of the data to check in
		 * @n @b Usage:
		 * @return true For success, false for failure
		 */
		bool Check_In_Default_Offset( DCI_DATA_PASS_TD const* default_data,
									  Change_Tracker const* change_tracker_to_avoid,
									  DCI_LENGTH_TD offset, DCI_LENGTH_TD length );

		/**
		 * @brief
		 * @details This function will read the default value of the Worker beginning at the offset and continuing
		 * for length bytes
		 * @param[out] default_data The data to be checked in of length length
		 * @n @b Usage:
		 * @param[in] offset The byte offset into the block to begin checking in data
		 * @n @b Usage:
		 * @param[in] length The byte length of the data to check in
		 * @n @b Usage:
		 * @return true for success, false for failure
		 */
		bool Check_Out_Default_Offset( DCI_DATA_PASS_TD* default_data, DCI_LENGTH_TD offset,
									   DCI_LENGTH_TD length ) const;

		/**
		 *  @details Loads the range attribute with the passed values.  Only works if Range is writable.
		 *  @param[in] min Parameters min value will be written with the value of min
		 *  @n @b Usage:
		 *  @param[in] change_tracker_to_avoid
		 *  @return true if value is written successfully
		 *  @return Throws exception if Range support or RW range support is not given
		 */
		bool Check_In_Min( DCI_DATA_PASS_TD const* min,
						   Change_Tracker const* change_tracker_to_avoid );

		/**
		 *  @details Loads the range attribute with the passed values.  Only works if Range is writable.
		 *  @param[in] max Parameters max value will be written with the value of max
		 *  @n @b Usage:
		 *  @param[in] change_tracker_to_avoid
		 *  @return true if value is written successfully
		 *  @return Throws exception if Range support or RW range support is not given
		 */
		bool Check_In_Max( DCI_DATA_PASS_TD const* max,
						   Change_Tracker const* change_tracker_to_avoid );

		/**
		 *  @details Loads the range attribute with the passed values.  Only works if Range is writable.
		 *  @param[out] min min value will be fetched back
		 *  @n @b Usage:
		 *  @return true if range attribute support is given else goes into exception
		 */
		bool Check_Out_Min( DCI_DATA_PASS_TD* min ) const;

		/**
		 *  @details Loads the range attribute with the passed values.  Only works if Range is writable.
		 *  @param[out] max max value will be fetched back
		 *  @n @b Usage:
		 *  @return true if range attribute support is given else goes into exception
		 */
		bool Check_Out_Max( DCI_DATA_PASS_TD* max ) const;

		/**
		 *  @brief
		 *  @details Provides a method to extract length
		 *  @param[out] length To fetch back the length
		 *  @return Always returns true
		 */
		bool Check_Out_Length( DCI_LENGTH_TD* length ) const;

		/**
		 *  @details Provides a quick method to get the length
		 *  @return DCI length
		 */
		DCI_LENGTH_TD Get_Length( void ) const;

		/**
		 *  @details Provides a quick method to get the datatype
		 *  @return DCI Datatype
		 */
		DCI_DATATYPE_TD Get_Datatype( void ) const;

		/**
		 *  @details Moves data from current to init.
		 *  @param[in] change_tracker_to_avoid
		 *  @return true If operation is successful else False
		 *  @retval true If current value is copied to Init value
		 *  @retval false If DCI parameter attribute doesn't have Init support
		 */
		bool Load_Current_To_Init( Change_Tracker const* change_tracker_to_avoid ) const;

		/**
		 *  @details Moves data from default to current. Will trigger the change tracker.
		 *  @param[in] change_tracker_to_avoid
		 *  @return true If operation is successful else False
		 *  @retval true
		 *  @retval false
		 */
		bool Load_Default_To_Current( Change_Tracker const* change_tracker_to_avoid ) const;

		/**
		 *  @details Moves data from default to init.
		 *  @param[in] change_tracker_to_avoid
		 *  @return true If operation is successful else False
		 *  @retval true If default value is copied to Init value
		 *  @retval false DCI parameter doesn't have either default value support or Init value support
		 */
		bool Load_Default_To_Init( Change_Tracker const* change_tracker_to_avoid ) const;

		/**
		 *  @details Moves data from init to current.  Will trigger the change tracker
		 *  @param[in] change_tracker_to_avoid
		 *  @return true If operation is successful else False
		 *  @retval true If init value is copied to current value
		 *  @retval false If DCI parameter attribute doesn't have Init support
		 */
		bool Load_Init_To_Current( Change_Tracker const* change_tracker_to_avoid ) const;

		/**
		 *  @details Writes value of DCI::CLEAR_DATA_VALUE to the current value.
		 *  Will trigger the change tracker
		 *  @return true if operation is successful else False
		   @retval true if current value is cleared to DCI::CLEAR_DATA_VALUE
		   @retval false Probably DCI parameter attribute is read only
		 */
		bool Clear_Current_Val( Change_Tracker const* change_tracker_to_avoid ) const;

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		DCI_OWNER_ATTRIB_TD Get_Owner_Attrib( void ) const
		{
			return ( m_data_block->owner_attrib_info );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		DCI_PATRON_ATTRIB_TD Get_Patron_Attrib( void ) const
		{
			return ( m_data_block->patron_attrib_info );
		}

		/**
		 *  @brief Get lock status
		 *  @details This allows the Worker to get the status of access
		 *  @param[in] command: Determines whether the lock is for read acces or the write access,By default the write
		 *  lock is enabled.
		 *  @return True if locked else False
		 */
		bool Is_Locked( DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK ) const
		{
			return ( DCI::Is_Locked( m_data_block->dcid, lock_type ) );
		}

	private:

		worker_permission_t m_privileges;
		DCI_DATA_BLOCK_TD const* m_data_block;
};

#endif	/* DCI_WORKER_H_ */
