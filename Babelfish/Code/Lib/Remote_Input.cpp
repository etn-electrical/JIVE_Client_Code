/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Remote_Input.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Remote_Input::Remote_Input( bool initial_state )
{
	m_state = initial_state;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Remote_Input::~Remote_Input()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Remote_Input::Get( void )
{
	return ( m_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Remote_Input::Get_Raw( void )
{
	return ( m_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Remote_Input::Set( bool state )
{
	m_state = state;
}
