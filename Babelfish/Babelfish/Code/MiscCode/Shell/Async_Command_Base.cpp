/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class implementation for Shell::Async_Command_Base::.
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
Async_Command_Base::Async_Command_Base( Shell_Base::command_config_t const* config_ptr ) :
	Command_Base( config_ptr )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Async_Command_Base::~Async_Command_Base()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::command_status_t Async_Command_Base::Execute( Shell_Base* shell_ptr )
{
	// recommended prolog
	Async_Start( shell_ptr );

	// TODO implement spawning Async_Body() asynchronously on your platform.
	YOUR_PLATFORM_SPAWN( Async_Body );

	// recommended epilog
	return ( Shell_Base::BUSY );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Async_Command_Base::Async_Abort( void )
{
	// recommended prolog
	shell_char_t your_result_string[RESULT_STRING_LENGTH];

	Get_Shell_Ptr()->Enter_Critical();

	// TODO implement killing your asynchronously spawned Async_Body()
	YOUR_PLATFORM_KILL( Async_Body );

	// recommended epilog
	Get_Shell_Ptr()->Exit_Critical();

	sprintf( your_result_string, "BazAsync aborted...\n" );

	// recommended epilog
	Put_String( your_result_string );
	Async_Finish( Shell_Base::ABORTED );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Async_Command_Base::Async_Body( void )
{
	// recommended prolog
	shell_char_t your_result_string[RESULT_STRING_LENGTH];

	// TODO implement your function here
	sprintf( your_result_string, "Async_Body says hey!!!\n" );

	// recommended epilog
	Put_String( your_result_string );
	Async_Finish( Shell_Base::SUCCESS );
}

}	// namespace Shell

