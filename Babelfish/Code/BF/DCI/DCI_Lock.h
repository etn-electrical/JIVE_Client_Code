/**
 *****************************************************************************************
 * @file
 *
 * @brief Contains the DCI Lock and Patron Lock classes.
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_LOCK_H
	#define DCI_LOCK_H

#include "Includes.h"
#include "DCI_Defines.h"
#include "Bit_List.h"

namespace BF
{
/**
 ****************************************************************************************
 * @brief A class which can be used to restrict patron write access to any parameter
 * in the database.
 *
 * @details DCI Lock provides the ability for an external function to maintain a lock and
 * control global access to certain parameters.  This class will tell the DCI to block
 * write access from the patron (or patron level worker).  Several DCI_Lock instances can be
 * created.  Access can only be gained if everyone who has an instance of a lock has released
 * there lock.  If the lock is read only then it is only used to check status and not control
 * access.
 *
 ****************************************************************************************
 */

class DCI_Lock
{
	public:
		enum lock_type_t
		{
			READ_ONLY,		///< This lock can only read the state of access.
			READ_WRITE		///< This lock type can also write the state of access.
		};

		static const uint8_t LOCK_WRITE_MASK = 0x01;	///< This Lock can only lock or unlock the write access.
		static const uint8_t LOCK_READ_MASK = 0x02;	///< This Lock can only lock or unlock the Read access.
		static const uint8_t LOCK_ALL_MASK = LOCK_WRITE_MASK | LOCK_READ_MASK;	///< This Lock can only lock or unlock
																				///< both read and write access.

		/**
		 * @brief Constructor for a lock.
		 * @details Will construct a DCI Lock instance providing control over patron write
		 * accesses.  Remember that the locks are ORed together.  All must be released before
		 * the access can be granted again.
		 * @param lock_type: Determines whether the lock has the ability to only read
		 * state or also change the global lock state.
		 */
		DCI_Lock( lock_type_t lock_type = READ_ONLY );
		virtual ~DCI_Lock( void );

		/**
		 * @brief Check to see if a specific DCID is locked globally (not specific to this
		 * instance).
		 * @param dcid: The DCID to check
		 * @param lock_type: Determines whether the lock is for read acces or the write access,By default the write lock
		 * is enabled.
		 * @returns true: the DCID is locked globally.
		 * @returns false: the DCID is free for access.
		 */
		virtual bool Locked( DCI_ID_TD dcid, DCI_LOCK_TYPE_TD lock_type = LOCK_WRITE_MASK )
		{
			bool return_status = false;

			if ( lock_type == LOCK_WRITE_MASK )
			{
				return_status = m_write_glocked_ids->Test( dcid );
			}
			else if ( lock_type == LOCK_READ_MASK )
			{
				return_status = m_read_glocked_ids->Test( dcid );
			}
			else if ( lock_type == LOCK_ALL_MASK )
			{
				bool write_status;
				bool read_status;
				write_status = m_write_glocked_ids->Test( dcid );
				read_status = m_read_glocked_ids->Test( dcid );
				return_status = ( write_status && read_status );		///< returns status as Locked only when locked
																		///< for both read and write access
			}
			else
			{}

			return ( return_status );
		}

		/**
		 * @brief Lock a DCID.
		 * @details This function is not interrupt safe.
		 * @param dcid: DCID to lock.
		 * @param lock_type: Determines whether the lock is for read acces or the write access,By default the write lock
		 * is enabled.
		 * @param dcid: .
		 */
		virtual void Lock( DCI_ID_TD dcid, DCI_LOCK_TYPE_TD lock_type = LOCK_WRITE_MASK );

		/**
		 * @brief Unlocks a DCID.
		 * @details This function is not interrupt safe.  Do not call it from the context of
		 * an interrupt.
		 * @param dcid: DCID to unlock.
		 * @param lock_type: Determines whether the lock is for read acces or the write access,By default the write lock
		 * is enabled.
		 */
		virtual void Unlock( DCI_ID_TD dcid, DCI_LOCK_TYPE_TD lock_type = LOCK_WRITE_MASK );

		/**
		 * @brief Provides information on this specific lock.  Whether there is any item locked
		 * or not.  It will only return for this specific instance of the lock.  Not the
		 * global state.  This is an indicator of whether this specific instance of a lock
		 * is active or not.
		 * @param lock_type: Determines whether the lock is for read acces or the write access,By default the write lock
		 * is enabled.
		 * @return
		 * @returns true: if any items associated with just this lock instance are locked.
		 * @returns false: if all the locks are released for this specific instance of the lock.
		 */
		virtual bool Locked( DCI_LOCK_TYPE_TD lock_type = LOCK_WRITE_MASK )
		{
			bool return_status = false;

			if ( lock_type == LOCK_WRITE_MASK )
			{
				return_status = ( !m_write_locked_ids->Is_Empty() );
			}
			else if ( lock_type == LOCK_READ_MASK )
			{
				return_status = ( !m_read_locked_ids->Is_Empty() );
			}
			else if ( lock_type == LOCK_ALL_MASK )
			{
				bool write_status;
				bool read_status;
				write_status = ( !m_write_locked_ids->Is_Empty() );
				read_status = ( !m_read_locked_ids->Is_Empty() );
				return_status = ( write_status && read_status );		///< returns status as Locked only when locked
																		///< for both read and write access
			}
			else
			{}

			return ( return_status );
		}

	protected:
		static DCI_Lock* const END_DCI_LOCK;
		static BF_Lib::Bit_List* const NO_BIT_LIST;

		/**
		 * @brief _g indicates the lock is a global lock
		 */
		static BF_Lib::Bit_List* m_write_glocked_ids;
		static BF_Lib::Bit_List* m_read_glocked_ids;

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		DCI_Lock( const DCI_Lock& object );
		DCI_Lock & operator =( const DCI_Lock& object );

		static DCI_Lock* m_write_locks_list;
		static DCI_Lock* m_read_locks_list;

		BF_Lib::Bit_List* m_write_locked_ids;
		BF_Lib::Bit_List* m_read_locked_ids;
		DCI_Lock* m_write_next;
		DCI_Lock* m_read_next;
};

/**
 ****************************************************************************************
 * @brief
 *
 * @details
 *
 ****************************************************************************************
 */
class Patron_Lock : public DCI_Lock
{
	public:
		/**
		 * @brief Constructs a patron specific lock.  This lock is associated with only a
		 * specific patron.  It must be passed in to the patron so it knows which lock check
		 * to use.
		 * @details This class would be used if it is necessary to block a specific patron
		 * instead of global access.  It is necessary to pass this instance in to the patron
		 * constructor for the patron to use.  This instance can then be controlled outside the
		 * patron to control access for that specific patron.  For example:  You wish to
		 * control the access on a specific port for a select list of parameters.  Construct
		 * a Patron_Lock and pass it along to the DCI_Patron constructor.  Then externally set
		 * the parameters you wish to have locked.
		 */
		Patron_Lock( void );
		~Patron_Lock( void );

		/**
		 * @brief See DCI_Lock class.
		 */
		bool Locked( DCI_ID_TD dcid, DCI_LOCK_TYPE_TD lock_type = LOCK_WRITE_MASK )
		{
			bool global_locked;
			bool patron_locked;
			bool return_status = false;

			if ( lock_type == LOCK_WRITE_MASK )
			{
				global_locked = DCI_Lock::m_write_glocked_ids->Test( dcid );
				patron_locked = m_write_plocked_ids->Test( dcid );
				return_status = ( global_locked || patron_locked );
			}
			else if ( lock_type == LOCK_READ_MASK )
			{

				global_locked = DCI_Lock::m_read_glocked_ids->Test( dcid );
				patron_locked = m_read_plocked_ids->Test( dcid );
				return_status = ( global_locked || patron_locked );
			}
			else if ( lock_type == LOCK_ALL_MASK )
			{
				bool read_status;
				bool write_status;
				global_locked = DCI_Lock::m_write_glocked_ids->Test( dcid );
				patron_locked = m_write_plocked_ids->Test( dcid );
				write_status = ( global_locked || patron_locked );
				global_locked = DCI_Lock::m_read_glocked_ids->Test( dcid );
				patron_locked = m_read_plocked_ids->Test( dcid );
				read_status = ( global_locked || patron_locked );
				return_status = ( write_status && read_status );
			}
			else
			{}

			return ( return_status );
		}

		/**
		 * @brief See DCI_Lock class.
		 */
		void Lock( DCI_ID_TD dcid, DCI_LOCK_TYPE_TD lock_type = LOCK_WRITE_MASK )
		{
			if ( ( lock_type == LOCK_WRITE_MASK ) || ( lock_type == LOCK_ALL_MASK ) )
			{
				m_write_plocked_ids->Set( dcid );
			}
			if ( ( lock_type == LOCK_WRITE_MASK ) || ( lock_type == LOCK_ALL_MASK ) )
			{
				m_read_plocked_ids->Set( dcid );
			}
		}

		/**
		 * @brief See DCI_Lock class.
		 */
		void Unlock( DCI_ID_TD dcid, DCI_LOCK_TYPE_TD lock_type = LOCK_WRITE_MASK )
		{
			if ( ( lock_type == LOCK_WRITE_MASK ) || ( lock_type == LOCK_ALL_MASK ) )
			{
				m_write_plocked_ids->Dump( dcid );
			}
			if ( ( lock_type == LOCK_WRITE_MASK ) || ( lock_type == LOCK_ALL_MASK ) )
			{
				m_read_plocked_ids->Dump( dcid );
			}
		}

		/**
		 * @brief See DCI_Lock class.
		 * @return Whether this instance has a lock set or not (true vs false).
		 */
		bool Locked( DCI_LOCK_TYPE_TD lock_type = LOCK_WRITE_MASK )
		{
			bool return_status = false;

			if ( lock_type == LOCK_WRITE_MASK )
			{
				return_status = ( !m_write_plocked_ids->Is_Empty() );
			}
			else if ( lock_type == LOCK_READ_MASK )
			{
				return_status = ( !m_read_plocked_ids->Is_Empty() );
			}
			else if ( lock_type == LOCK_ALL_MASK )
			{
				bool write_status;
				bool read_status;
				write_status = ( !m_write_plocked_ids->Is_Empty() );
				read_status = ( !m_read_plocked_ids->Is_Empty() );
				return_status = ( write_status && read_status );		///< returns status as Locked only when locked
																		///< for both read and write access
			}
			else
			{}

			return ( return_status );
		}

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Patron_Lock( const Patron_Lock& object );
		Patron_Lock & operator =( const Patron_Lock& object );

		BF_Lib::Bit_List* m_write_plocked_ids;
		BF_Lib::Bit_List* m_read_plocked_ids;
};

}
#endif
