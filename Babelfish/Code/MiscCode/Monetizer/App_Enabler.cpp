/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "App_Enabler.h"
#include "Babelfish_Assert.h"


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
App_Enabler::App_Enabler( cback_func_t func_callback, cback_param_t func_param ) :
	m_func_callback( func_callback ),
	m_func_param( func_param )
{
	BF_ASSERT( m_func_callback );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
App_Enabler::~App_Enabler()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Enabler::ret_status_t App_Enabler::Lock( void )
{
	Enabler::ret_status_t return_status = false;

	if ( m_func_callback != nullptr )
	{
		m_func_callback( m_func_param, DISABLE, &return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Enabler::ret_status_t App_Enabler::Unlock( void )
{
	Enabler::ret_status_t return_status = false;

	if ( m_func_callback != nullptr )
	{
		m_func_callback( m_func_param, ENABLE, &return_status );
	}

	return ( return_status );
}
