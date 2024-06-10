/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI_Enabler.h"
#include "Babelfish_Assert.h"


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Enabler::DCI_Enabler( Param_Lock* param_lock_module_handle, DCI_LOCK_TYPE_TD lock_type ) :
	m_lock_type( lock_type )
{
	if ( param_lock_module_handle != nullptr )
	{
		m_param_lock_module_handle = param_lock_module_handle;
	}
	BF_ASSERT( m_param_lock_module_handle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Enabler::~DCI_Enabler()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Enabler::ret_status_t DCI_Enabler::Lock( void )
{
	ret_status_t return_status = false;

	m_param_lock_module_handle->Lock( m_lock_type );

	if ( m_param_lock_module_handle->Locked( m_lock_type ) == true )
	{
		return_status = true;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Enabler::ret_status_t DCI_Enabler::Unlock( void )
{
	ret_status_t return_status = false;

	m_param_lock_module_handle->Unlock( m_lock_type );

	if ( m_param_lock_module_handle->Locked( m_lock_type ) == false )
	{
		return_status = true;
	}
	return ( return_status );
}
