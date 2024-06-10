/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @file
 *
 * @brief Contains the Param Lock class which takes a container of DCIDs and swiftly locks them.
 *
 * @copyright 2015 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PARAM_LOCK_H
	#define PARAM_LOCK_H

#include "Includes.h"
#include "DCI_Lock.h"
#include "DCI_Owner.h"


/**
 ****************************************************************************************
 * @brief Will control external locking and unlocking of a list of parameters.
 *
 * @details This class provides the ability to control DCI patron access utilizing a simple
 * lock/unlock interface to control access for a list of DCIDs.
 *
 ****************************************************************************************
 */
class Param_Lock
{
	public:
		/// Values for the DCI Owner lock flag.
		static const uint8_t PARAMS_UNLOCKED = 0U;		///< DCID lock status value for unlocked.
		static const uint8_t PARAMS_LOCKED = 1U;		///< DCID lock status value for locked.

		typedef DCI_ID_TD dci_lock_list_item_t;			///< A typedef for the list element type.
		struct dci_lock_block_t
		{
			dci_lock_list_item_t const* lock_list;		///< An array of DCIDs which can be locked.
			uint16_t list_length;		///< The length of the list.
		};

		/**
		 * @brief The constructor for the Param Lock class.
		 * @param lock_block: The pointer to a struct of type above.
		 * @param dci_lock: The lock to use for actual DCI access control.
		 * @param lock_flag_owner: The DCID for a parameter in the database indicating state.
		 */
		Param_Lock( dci_lock_block_t const* lock_block, BF::DCI_Lock* dci_lock,
					DCI_Owner* lock_flag_owner = nullptr, DCI_Owner* lock_type_owner = nullptr );
		virtual ~Param_Lock( void );

		/**
		 * @brief Locks the passed in list of parameters.
		 */
		void Lock( DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK );

		/**
		 * @brief Unlocks the passed in list of parameters.
		 */
		void Unlock( DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK );

		/**
		 * @brief Status of the full set of locks.
		 * @returns true: List of DCIDs is locked.
		 * @returns false: List of DCIDs are unlocked.
		 */
		bool Locked( DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_WRITE_MASK )
		{ return ( m_dci_lock->Locked( lock_type ) ); }

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Param_Lock( const Param_Lock& object );
		Param_Lock & operator =( const Param_Lock& object );

		BF::DCI_Lock* m_dci_lock;
		dci_lock_block_t const* m_lock_block;
		DCI_Owner* m_lock_flag_owner;
		DCI_Owner* m_lock_type_owner;

		DCI_CB_RET_TD Lock_Owner_Callback( DCI_ACCESS_ST_TD* access_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD Lock_Owner_Callback_Static( DCI_CBACK_PARAM_TD param,
														 DCI_ACCESS_ST_TD* access_struct )
		{
			return ( reinterpret_cast<Param_Lock*>( param )->Lock_Owner_Callback(
						 access_struct ) );
		}

};

#endif
