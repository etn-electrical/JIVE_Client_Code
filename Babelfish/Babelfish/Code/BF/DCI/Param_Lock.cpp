/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Param_Lock.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Param_Lock::Param_Lock( dci_lock_block_t const* lock_block, BF::DCI_Lock* dci_lock,
						DCI_Owner* lock_flag_owner, DCI_Owner* lock_type_owner ) :
	m_dci_lock( dci_lock ),
	m_lock_block( lock_block ),
	m_lock_flag_owner( lock_flag_owner ),
	m_lock_type_owner( lock_type_owner )
{
	DCI_OWNER_ATTRIB_TD owner_attrib;

	if ( m_lock_flag_owner != nullptr )
	{
		owner_attrib = m_lock_flag_owner->Get_Owner_Attrib();

		if ( BF_Lib::Bit::Test( owner_attrib, DCI_OWNER_ATTRIB_INFO_CALLBACK ) )
		{
			m_lock_flag_owner->Attach_Callback( &Lock_Owner_Callback_Static,
												reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
												DCI_ACCESS_POST_SET_RAM_CMD_MSK );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Param_Lock::~Param_Lock( void )
{
	delete m_dci_lock;
	m_lock_block = reinterpret_cast<dci_lock_block_t*>( nullptr );
	if ( m_lock_flag_owner != ( reinterpret_cast<DCI_Owner*>( nullptr ) ) )
	{
		delete m_lock_flag_owner;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Param_Lock::Lock( DCI_LOCK_TYPE_TD lock_type )
{
	uint8_t lock_status = PARAMS_LOCKED;

	if ( m_lock_flag_owner != nullptr )
	{
		m_lock_flag_owner->Check_In( lock_status );
	}

	for ( uint16_t i = 0U; i < m_lock_block->list_length; i++ )
	{
		m_dci_lock->Lock( m_lock_block->lock_list[i], lock_type );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Param_Lock::Unlock( DCI_LOCK_TYPE_TD lock_type )
{
	uint8_t lock_status = PARAMS_UNLOCKED;

	if ( m_lock_flag_owner != nullptr )
	{
		m_lock_flag_owner->Check_In( lock_status );
	}

	for ( uint16_t i = 0U; i < m_lock_block->list_length; i++ )
	{
		m_dci_lock->Unlock( m_lock_block->lock_list[i], lock_type );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Param_Lock::Lock_Owner_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	uint8_t lock_status;
	uint8_t lock_type;

	BF_Lib::Unused<DCI_ACCESS_ST_TD*>::Okay( access_struct );

	m_lock_type_owner->Check_Out( lock_type );
	m_lock_flag_owner->Check_Out( lock_status );

	if ( lock_status == PARAMS_LOCKED )
	{
		Lock( lock_type );
	}
	else
	{
		Unlock( lock_type );
	}

	return ( DCI_CBACK_RET_PROCESS_NORMALLY );
}
