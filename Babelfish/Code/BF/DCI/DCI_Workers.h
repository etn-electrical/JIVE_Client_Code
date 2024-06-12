/**
 *****************************************************************************************
 *	@brief This file includes the interfaces for DCI Workers to access DCI Data base
 *
 *	@details
 *	@n @b DCI Workers are the frinds of the DCI owners,
 *        has lesser permissions than owners but more permissions than Patrons
 *	@n @b Usage :
 *	@n @b - DCI_Owner must be created first and then DCI Workers to be created for the same DCID's
 *	@n @b - DCI_Workers must not access data before DCI_Owner is created, if not assert hits during execution
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef DCI_WORKERS_H_
#define DCI_WORKERS_H_

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
 * @n @b 1. Create DCI Worker, DCI Worker are essentially similar to DCI Owners but has lesser
 * permissions
 ****************************************************************************************
 */
class DCI_Workers
{
	public:
		/**
		 *  @brief Constructor
		 *  @details Will create a general database access point with enhanced privileges.
		 *	@n @b Usage :
		 *	@n @b - DCI_Owner must be created first and then DCI Workers to be created for the same DCID's
		 *	@n @b - DCI_Workers must not access data before DCI_Owner is created, if not assert hits during execution
		 *  @param[in] correct_out_of_range_sets: Determines whether the out of range
		 *  values should be corrected or return error when incorrect.
		 *  @n @b Usage: true means if the DCI value written using patron falls out of range then
		 * min or max value will be written
		 *  @n @b Usage: False means if the DCI value written using patron falls out of range then
		 * error will be returned
		 *  @param[in] owner_access: Creates a privileged access point if owner access is true.
		 *  Otherwise this workers class will act as a patron.
		 *  @param[in] change_tracker_to_avoid: To avoid an instance where it is desired to avoid
		 *  kicking the change tracker on an action taken by the patron user, it is possible to
		 *  attach the change tracker which should not be kicked.
		 *  @param[in] dci_lock: A pointer to a DCI Lock instance.  The DCI Worker will not
		 *  change the lock but will use only for read access.  Any lock/unlock of this parameter
		 *  must be done externally.
		 *  @return None
		 */
		DCI_Workers( bool correct_out_of_range_sets = true, bool owner_access = false,
					 Change_Tracker const* change_tracker_to_avoid =
					 reinterpret_cast<Change_Tracker const*>( nullptr ),
					 BF::DCI_Lock* dci_lock = reinterpret_cast<BF::DCI_Lock*>( nullptr ) );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object goes out of scope or deleted.
		 */
		~DCI_Workers( void );

		/**
		 *  @brief Method to change the permissions of a worker
		 *  @details It accesses the private variable m_privileges and updates it
		 *  @param[in] owner_access: A set of the permission level.
		 */
		void Change_Permissions( bool owner_access = false );

		/**
		 *  @brief Method to change the permissions of a worker
		 *  @details An alternate path to include a change tracker.  In case the change tracker
		 *  changes at some point or a change tracker is instantiated later.
		 */
		void Assign_Change_Tracker( Change_Tracker const* change_tracker_to_avoid );

		/**
		 *  @brief Patron Like Access.
		 *  @details This interface will mimic a patron like data access interface.
		 *  This is analogous to filling out an order slip first and depending on order it will be
		 * processed.
		 *  @param[in,out] access_struct The access struct is the order slip.  The order slip will
		 * be verified for
		 * accuracy then passed down to the DCI to extract/set the data.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Data_Access( DCI_ACCESS_ST_TD* access_struct ) const;

		/**
		 *  @brief Set the current value.
		 *  @details Writes the value of ram_data to the DCI RAM data value
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[in] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Set_Current( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
								  DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Get the current value.
		 *  @details Reads the RAM value from the database.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[out] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Get_Current( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
								  DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Set of initial value.
		 *  @details Will store the value into the initial space.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[in] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Set_Init( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
							   DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Get of initial value.
		 *  @details Get the initial value out of the NV storage.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[out] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Get_Init( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
							   DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Set of default value.
		 *  @details Sets the default value if we have owner level access.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[in] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Set_Default( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
								  DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Get of default value.
		 *  @details Gets the default value.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[out] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Get_Default( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
								  DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Set the minimum value.
		 *  @details Sets the minimum value as long as the range is writable and the access is owner
		 * level.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[in] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Set_Min( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
							  DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Get the minimum value.
		 *  @details Gets the minimum value.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[out] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Get_Min( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
							  DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Set the maximum value.
		 *  @details Sets the maximum value as long as the range is writable and the access is owner
		 * level.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[in] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Set_Max( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
							  DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Get the maximum value.
		 *  @details Gets the maximum value.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[out] data: Data pointer to be operated on.
		 *  @param[in] length: Byte length of data to write.  If a zero is passed in then the
		 * parameter
		 *  defined size is used.  Note that the offset must also be zero to avoid an error.
		 *  @param[in] offset: Byte offset into the database value.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Get_Max( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
							  DCI_LENGTH_TD length = 0U, DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Set the length.
		 *  @details Set the length only if the length is writable and an owner access level.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[in] length: Length value to write to.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Set_Length( DCI_ID_TD dcid, DCI_LENGTH_TD length ) const;

		/**
		 *  @brief Get the length.
		 *  @details Gets the length.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[out] length: Place to write length value to.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Get_Length( DCI_ID_TD dcid, DCI_LENGTH_TD* length ) const;

		/**
		 *  @brief Get the datatype.
		 *  @details Gets the datatype.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[out] datatype: The location to store the datatype.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Get_Datatype( DCI_ID_TD dcid, DCI_DATATYPE_TD* datatype ) const;

		/**
		 *  @brief Set the current value to the initial.
		 *  @details If the value has an initial value, the value will be written to the current
		 *  value.  If the value does not have an initial value then the current is set to zero.
		 *  @param[in] dcid: The DCID to work with.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Initialize_Current( DCI_ID_TD dcid ) const;

		/**
		 *  @brief Retrieve the owner level access.
		 *  @details
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[out] attrib: The place to store the attribute bits.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Get_Owner_Attrib( DCI_ID_TD dcid, DCI_OWNER_ATTRIB_TD* attrib ) const;

		/**
		 *  @brief Retrieve the patron level access.
		 *  @details
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[out] attrib: The place to store the attribute bits.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Get_Patron_Attrib( DCI_ID_TD dcid, DCI_PATRON_ATTRIB_TD* attrib ) const;

		/**
		 *  @brief A method to range check a potential value.
		 *  @details Provides a method to range check a value and choose whether to fix it or not.
		 *  @param[in,out] access_struct The access struct is the order slip.  The order slip will
		 * be verified for
		 * accuracy then passed down to the DCI to extract/set the data.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Range_Check( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
								  bool correct_out_of_range, DCI_LENGTH_TD length = 0U,
								  DCI_LENGTH_TD offset = 0U ) const;

		/**
		 *  @brief Get lock status.
		 *  @details This allows the Worker to get the status of access.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[in] command: Determines whether the lock is for read acces or the write access,By default the write
		 *  lock is enabled.
		 *  @param[out] locked: True if locked else False
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Is_Locked( DCI_ID_TD dcid, bool* locked,
								DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK ) const;

		/**
		 *  @brief Set lock status.
		 *  @details This allows the Worker to get the status of access.
		 *  @param[in] dcid: The DCID to work with.
		 *  @param[in] command: Determines whether the lock is for read acces or the write access,By default the write
		 *  lock is enabled.
		 *  @param[in] lock: True to lock.
		 *  @return Returns a DCI ERROR defined in DCI_Defines.
		 */
		DCI_ERROR_TD Lock( DCI_ID_TD dcid, bool lock = true,
						   DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK ) const;

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		DCI_Workers( const DCI_Workers& object );
		DCI_Workers & operator =( const DCI_Workers& object );

		DCI_ERROR_TD Check_Length( DCI_DATA_BLOCK_TD const* data_block,
								   DCI_ACCESS_DATA_TD* data_struct,
								   DCI_ACCESS_CMD_TD command ) const;

		DCI_ERROR_TD Check_Access( DCI_DATA_BLOCK_TD const* data_block,
								   DCI_ACCESS_CMD_TD command ) const;

		bool Ignore_Range( DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Execute_Operation( DCI_DATA_BLOCK_TD const* data_block,
										DCI_ACCESS_ST_TD* access_struct ) const;

		Change_Tracker::attrib_t Change_Tracker_Attrib( DCI_ACCESS_CMD_TD command ) const;

		DCI_ERROR_TD Get_RAM_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Set_RAM_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Init_Data( DCI_ACCESS_ST_TD* access_struct,
									DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Set_Init_Data( DCI_ACCESS_ST_TD* access_struct,
									DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Default_Data( DCI_ACCESS_ST_TD* access_struct,
									   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Set_Default_Data( DCI_ACCESS_ST_TD* access_struct,
									   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Min_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Set_Min_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Max_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Set_Max_Data( DCI_ACCESS_ST_TD* access_struct,
								   DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Get_Length_Data( DCI_ACCESS_ST_TD* access_struct,
									  DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Set_Length_Data( DCI_ACCESS_ST_TD* access_struct,
									  DCI_DATA_BLOCK_TD const* data_block ) const;

		DCI_ERROR_TD Clear_Ram_Data( DCI_ACCESS_ST_TD* access_struct,
									 DCI_DATA_BLOCK_TD const* data_block ) const;

		Change_Tracker const* m_change_tracker;
		bool m_owner_access;
		bool m_correct_out_of_range_sets;
		DCI_SOURCE_ID_TD m_source_id;
		BF::DCI_Lock* m_dci_lock;

};

#endif	/* DCI_WORKERS_H_ */
