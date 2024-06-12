/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "DCI_Lock.h"
#include "DCI_Data.h"

namespace BF
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
DCI_Lock* const DCI_Lock::END_DCI_LOCK = nullptr;
BF_Lib::Bit_List* const DCI_Lock::NO_BIT_LIST = nullptr;

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
BF_Lib::Bit_List* DCI_Lock::m_write_glocked_ids = NO_BIT_LIST;
DCI_Lock* DCI_Lock::m_write_locks_list = END_DCI_LOCK;
BF_Lib::Bit_List* DCI_Lock::m_read_glocked_ids = NO_BIT_LIST;
DCI_Lock* DCI_Lock::m_read_locks_list = END_DCI_LOCK;


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Lock::DCI_Lock( lock_type_t lock_type ) :
	m_write_locked_ids( NO_BIT_LIST ),
	m_read_locked_ids( NO_BIT_LIST ),
	m_write_next( END_DCI_LOCK ),
	m_read_next( END_DCI_LOCK )
{
	if ( m_write_glocked_ids == nullptr )
	{
		m_write_glocked_ids = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );

	}

	if ( m_read_glocked_ids == nullptr )
	{
		m_read_glocked_ids = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );

	}
	if ( lock_type == READ_ONLY )
	{
		m_write_locked_ids = NO_BIT_LIST;
		m_read_locked_ids = NO_BIT_LIST;
		m_write_next = END_DCI_LOCK;
		m_read_next = END_DCI_LOCK;
	}
	else
	{
		// If we are the first RW lock list then we take a shortcut and just assign
		// the internal lock to the global lock.  Saves some RAM on the first created RW one.
		if ( m_write_locks_list == END_DCI_LOCK )
		{
			m_write_locked_ids = m_write_glocked_ids;

		}
		else if ( m_write_locks_list->m_write_next == END_DCI_LOCK )	// If this is the second RW item then we
		// undo what we did above.
		{
			// Make a copy of the global locked bits bit list in the previous shortcutted one
			// above and then create a new for this instance.
			m_write_locks_list->m_write_locked_ids = new BF_Lib::Bit_List( *m_write_glocked_ids );
			m_write_locked_ids = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );
		}
		else	// if this is greater than the second one then we just create a new list and move
				// on.
		{
			m_write_locked_ids = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );
		}

		m_write_next = m_write_locks_list;
		m_write_locks_list = this;

		if ( m_read_locks_list == END_DCI_LOCK )
		{
			m_read_locked_ids = m_read_glocked_ids;

		}
		else if ( m_read_locks_list->m_read_next == END_DCI_LOCK )	// If this is the second RW item then we
		// undo what we did above.
		{
			// Make a copy of the global locked bits bit list in the previous shortcutted one
			// above and then create a new for this instance.
			m_read_locks_list->m_read_glocked_ids = new BF_Lib::Bit_List( *m_read_glocked_ids );
			m_read_locked_ids = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );

		}
		else	// if this is greater than the second one then we just create a new list and move
				// on.
		{
			m_read_locked_ids = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );

		}

		m_read_next = m_read_locks_list;
		m_read_locks_list = this;

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Lock::~DCI_Lock( void )
{
	DCI_Lock* walker;

	// If this thing is not RO then we have to go remove it from the linked list.
	if ( m_write_locked_ids != NO_BIT_LIST )
	{
		if ( m_write_locks_list == this )
		{
			m_write_locks_list = m_write_next;
		}
		else
		{
			walker = m_write_locks_list;
			while ( ( walker->m_write_next != this ) && ( walker->m_write_next != END_DCI_LOCK ) )
			{
				walker = walker->m_write_next;
			}

			if ( walker->m_write_next != this )
			{
				walker->m_write_next = m_write_next;
			}

			m_write_next = END_DCI_LOCK;
		}

		// If we hit the end of the road and there are no more locks.  We clear out the static
		// one.
		if ( m_write_locks_list == END_DCI_LOCK )
		{
			delete m_write_glocked_ids;
		}

		// If we are pointing at a unique list and not the global list then we can delete it.
		if ( m_write_locked_ids != m_write_glocked_ids )
		{
			delete m_write_locked_ids;
		}
	}

	m_write_next = END_DCI_LOCK;
	if ( m_read_locked_ids != NO_BIT_LIST )
	{
		if ( m_read_locks_list == this )
		{
			m_read_locks_list = m_read_next;
		}
		else
		{
			walker = m_read_locks_list;
			while ( ( walker->m_read_next != this ) && ( walker->m_read_next != END_DCI_LOCK ) )
			{
				walker = walker->m_read_next;
			}

			if ( walker->m_read_next != this )
			{
				walker->m_read_next = m_read_next;
			}

			m_read_next = END_DCI_LOCK;
		}

		// If we hit the end of the road and there are no more locks.  We clear out the static
		// one.
		if ( m_read_locks_list == END_DCI_LOCK )
		{
			delete m_read_glocked_ids;
		}

		// If we are pointing at a unique list and not the global list then we can delete it.
		if ( m_read_locked_ids != m_read_glocked_ids )
		{
			delete m_read_locked_ids;
		}
	}

	m_read_next = END_DCI_LOCK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Lock::Lock( DCI_ID_TD dcid, DCI_LOCK_TYPE_TD lock_type )
{
	if ( ( lock_type == LOCK_WRITE_MASK ) || ( lock_type == LOCK_ALL_MASK ) )
	{
		if ( m_write_locked_ids != NO_BIT_LIST )
		{
			m_write_glocked_ids->Set( dcid );
			m_write_locked_ids->Set( dcid );
		}
	}
	if ( ( lock_type == LOCK_READ_MASK ) || ( lock_type == LOCK_ALL_MASK ) )
	{
		if ( m_read_locked_ids != NO_BIT_LIST )
		{
			m_read_glocked_ids->Set( dcid );
			m_read_locked_ids->Set( dcid );
		}
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Lock::Unlock( DCI_ID_TD dcid, uint8_t lock_type )
{
	DCI_Lock* walker;
	bool unlock;

	if ( ( lock_type == LOCK_WRITE_MASK ) || ( lock_type == LOCK_ALL_MASK ) )
	{
		if ( m_write_locked_ids != NO_BIT_LIST )
		{
			m_write_locked_ids->Dump( dcid );

			walker = m_write_locks_list;
			unlock = true;
			while ( ( walker != END_DCI_LOCK ) && ( unlock == true ) )
			{
				if ( walker->m_write_locked_ids->Test( dcid ) )
				{
					unlock = false;
				}
				walker = walker->m_write_next;
			}

			if ( unlock == true )
			{
				m_write_glocked_ids->Dump( dcid );
			}
		}
	}
	if ( ( lock_type == LOCK_READ_MASK ) || ( lock_type == LOCK_ALL_MASK ) )
	{
		if ( m_read_locked_ids != NO_BIT_LIST )
		{
			m_read_locked_ids->Dump( dcid );

			walker = m_read_locks_list;
			unlock = true;
			while ( ( walker != END_DCI_LOCK ) && ( unlock == true ) )
			{
				if ( walker->m_read_locked_ids->Test( dcid ) )
				{
					unlock = false;
				}
				walker = walker->m_read_next;
			}

			if ( unlock == true )
			{
				m_read_glocked_ids->Dump( dcid );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Patron_Lock::Patron_Lock( void ) :
	DCI_Lock( DCI_Lock::READ_ONLY ),
	m_write_plocked_ids( nullptr ),
	m_read_plocked_ids( nullptr )
{
	m_write_plocked_ids = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );
	m_read_plocked_ids = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Patron_Lock::~Patron_Lock( void )
{
	delete m_write_plocked_ids;
	delete m_read_plocked_ids;
}

}
