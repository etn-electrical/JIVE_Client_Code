/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class implementation for Shell::Shell_Base::.
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
const Shell_Base::shell_config_t Shell_Base::DEFAULT_SHELL_CONFIG =
{
	128U,				/* Allocated size of command buffer. */

	128U,				/* Allocated size of results buffer. */

	3U,					/* Minimum length of a valid command line. */

	6U,					/* Allocated size of command argument table. */

	nullptr,			/* Array of pointer to commands */

	0U,					/* Count of items in command table */

	"Welcome! \n",		/* Startup string. */

	"> ",				/* Prompt string. */
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::Shell_Base( shell_config_t const* shell_config_ptr, Command_Line_Editor* cle,
						Command_Line_Interpreter* cli ) :
	m_shell_config_ptr( shell_config_ptr ),
	m_cle( cle ),
	m_cli( cli ),
	m_command_ptr( nullptr )
{
	if ( m_cle == nullptr )
	{
		m_cle = new Command_Line_Editor( this );
	}

	if ( m_cli == nullptr )
	{
		m_cli = new Command_Line_Interpreter( this );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::~Shell_Base()
{
	m_shell_config_ptr = nullptr;
	delete m_cle;
	delete m_cli;
	m_command_ptr = nullptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Shell_Base::Initialize( void )
{
	Internal_Initialize();
	m_cle->Initialize();
	m_cli->Initialize();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Shell_Base::On_Arrival( shell_char_t in_char )
{
	return ( m_cle->On_Arrival( in_char ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::command_status_t Shell_Base::Parse_Command_Line( void )
{
	return ( m_cli->Parse_Command_Line(
				 Get_Cle_Ptr()->Get_Buffer_Ptr(),
				 Get_Cle_Ptr()->Get_Buffer_Index()
				 ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::command_status_t Shell_Base::Dispatch( void )
{
	bool done = false;
	uint32_t index;
	uint32_t cmd_count = Get_Shell_Config()->m_command_cnt;
	uint32_t arg_count;
	Shell_Base::command_status_t result = Shell_Base::NOT_FOUND;
	Command_Base* cmd_ptr;
	Shell_Base::command_config_t const* cmd_config_ptr;

	// for each command in the table
	for ( index = 0U; ( ( done == false ) && ( index < cmd_count ) ); index++ )
	{
		cmd_ptr = Get_Shell_Config()->m_command_table[index];
		cmd_config_ptr = cmd_ptr->Get_Cmd_Config();

		// this this the command?
		if ( Command_Line_Interpreter::String_Equal_Ignore_Case( Get_Command_Name(),
																 cmd_config_ptr->m_name ) )
		{
			done = true;
			// validate argument count
			arg_count = Get_Argument_Count();
			if ( ( arg_count >= cmd_config_ptr->m_min_argument_cnt ) &&
				 ( arg_count <= cmd_config_ptr->m_max_argument_cnt ) )
			{
				// execute the command
				result = cmd_ptr->Execute( this );
			}
			else
			{
				result = Shell_Base::FAILURE;
			}
		}
	}
	return ( result );
}

/**
 * @brief Gets the name of the command.
 * @return Name of the command.
 */
shell_char_t const* Shell_Base::Get_Command_Name( void ) const
{
	return ( m_cli->Get_Command_Name() );
}

/**
 * @brief Gets count of arguments.
 * @return Count of arguments provided in command string.
 */
uint32_t Shell_Base::Get_Argument_Count( void ) const
{
	return ( m_cli->Get_Argument_Count() );
}

/**
 * @brief Gets an argument string.
 * @param[in] index: Argument index.
 * @return Argument string, nullptr if no such argument.
 */
shell_char_t const* Shell_Base::Get_Argument_String( uint32_t index ) const
{
	return ( m_cli->Get_Argument_String( index ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Shell_Base::Internal_Initialize( void )
{}

}	// namespace Shell
