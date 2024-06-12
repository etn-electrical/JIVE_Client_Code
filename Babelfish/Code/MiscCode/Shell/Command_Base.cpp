/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class implementation for Shell::Command_Base::.
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Shell_Lib.h"

namespace Shell
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Command_Base::Command_Base( Shell_Base::command_config_t const* cmd_config_ptr ) :
	m_config_ptr( cmd_config_ptr ),
	m_status( Shell_Base::SUCCESS ),
	m_shell_ptr( nullptr )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Command_Base::~Command_Base()
{
	m_config_ptr = nullptr;
	m_shell_ptr = nullptr;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::command_config_t const* Command_Base::Get_Cmd_Config( void ) const
{
	return ( m_config_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::command_status_t Command_Base::Get_Status( void )
{
	Shell_Base::command_status_t status;

	Get_Shell_Ptr()->Enter_Critical();
	status = m_status;
	Get_Shell_Ptr()->Exit_Critical();

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Base::Async_Abort( void )
{
	m_status = Shell_Base::ABORTED;
	Get_Cle_Ptr()->On_Completion( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Base::Put_String( shell_char_t* put_string )
{
	Get_Shell_Ptr()->Put_String( put_string );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Base::Async_Start( Shell_Base* shell_ptr )
{
	m_shell_ptr = shell_ptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Base::Async_Body( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Base::Async_Finish( Shell_Base::command_status_t status )
{
	m_status = status;
	Get_Cle_Ptr()->On_Completion( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Base::Sync_Start( Shell_Base* shell_ptr )
{
	m_shell_ptr = shell_ptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::command_status_t Command_Base::Sync_Finish( Shell_Base::command_status_t status )
{
	return ( m_status = status );
}

}	// namespace Shell

