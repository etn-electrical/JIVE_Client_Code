/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class implementation for Shell::Command_Line_Interpreter::.
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
Command_Line_Interpreter::Command_Line_Interpreter( Shell_Base* shell_ptr ) :
	m_shell_ptr( shell_ptr ),
	m_argument_tbl( nullptr ),
	m_argument_cnt( 0U )
{
	m_argument_tbl = new shell_char_t*[Get_Shell_Config()->m_argument_table_size];
	Clear_Argument_Table();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Command_Line_Interpreter::~Command_Line_Interpreter()
{
	m_shell_ptr = nullptr;
	delete[] m_argument_tbl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Interpreter::Initialize( void ) const
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::command_status_t Command_Line_Interpreter::Parse_Command_Line(
	shell_char_t* command_str,
	uint32_t command_len )
{
	Shell_Base::command_status_t result = Shell_Base::FAILURE;
	uint32_t min_command_len = Get_Shell_Config()->m_min_command_len;
	uint32_t command_buffer_size = Get_Shell_Config()->m_command_buffer_size;

	// validate parking
	if ( command_str != nullptr )
	{
		if ( ( command_len >= min_command_len ) &&
			 ( command_len < command_buffer_size ) )
		{
			if ( command_str[command_len] == '\0' )
			{
				result = Parse_Command_Line_Helper( command_str );
			}
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::command_status_t Command_Line_Interpreter::Parse_Command_Line_Helper(
	shell_char_t* command_str )
{
	uint32_t index = 0U;
	bool done = false;
	shell_char_t* argument_str;
	shell_char_t* argument_end_str;

	// initialize environment
	Clear_Argument_Table();
	Terminate_At_Newline( command_str );
	argument_str = Skip_Whitespace( command_str );

	// isolate command name sub-string, find start of argument list
	argument_end_str = Parse_Next_Token( argument_str );

	// save the command sub-string pointer
	Set_Argument_String( argument_str, index );
	index++;

	if ( argument_end_str != nullptr )
	{
		// for each argument
		while ( !done )
		{
			// walk the pointers, skipping leading whitespace
			argument_str = Skip_Whitespace( argument_end_str );

			if ( argument_str != nullptr )
			{
				// isolate the argument name, find the argument value
				argument_end_str = Parse_Next_Token( argument_str );

				// save the pointer
				Set_Argument_String( argument_str, index );
				index++;

				if ( argument_end_str != nullptr )
				{
					// stop after so many
					if ( index >= Get_Shell_Config()->m_argument_table_size )
					{
						done = true;
					}
				}
				else
				{
					done = true;
				}

			}
			else
			{
				done = true;
			}
		}
	}
	Set_Argument_Count( index );
	return ( Shell_Base::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Interpreter::Clear_Argument_Table( void )
{
	uint32_t index;

	m_argument_cnt = 0U;
	for ( index = 0U; index < Get_Shell_Config()->m_argument_table_size; index++ )
	{
		m_argument_tbl[index] = nullptr;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
shell_char_t const* Command_Line_Interpreter::Get_Command_Name( void ) const
{
	shell_char_t* name_ptr = nullptr;

	if ( m_argument_cnt != 0U )
	{
		name_ptr = m_argument_tbl[0];
	}
	return ( name_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Command_Line_Interpreter::Get_Argument_Count( void ) const
{
	return ( m_argument_cnt );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Interpreter::Set_Argument_Count( uint32_t arg_count )
{
	m_argument_cnt = arg_count;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
shell_char_t const* Command_Line_Interpreter::Get_Argument_String( uint32_t index ) const
{
	shell_char_t* arg_string = nullptr;

	if ( index < m_argument_cnt )
	{
		arg_string = m_argument_tbl[index];
	}
	return ( arg_string );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Interpreter::Set_Argument_String( shell_char_t* argument_str, uint32_t index )
{
	m_argument_tbl[index] = argument_str;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Command_Line_Interpreter::Terminate_At_Newline( shell_char_t* text_str )
{
	bool done = false;
	uint32_t length = 0U;

	if ( text_str == nullptr )
	{
		done = true;
	}

	while ( !done )
	{
		switch ( *text_str )
		{
			case '\0':
				done = true;
				break;

			case '\r':	// NOTE: fall-through intended!!!
			case '\n':
				*text_str = '\0';
				done = true;
				break;

			default:
				text_str++;
				length++;
				break;
		}
	}
	return ( length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
shell_char_t* Command_Line_Interpreter::Skip_Whitespace( shell_char_t* text_str )
{
	bool done = false;
	shell_char_t* retval = nullptr;

	if ( text_str == nullptr )
	{
		done = true;
	}

	while ( !done )
	{
		switch ( *text_str )
		{
			case ' ':	// NOTE: fall-through intended!!!
			case '\t':
				text_str++;
				break;

			default:// NOTE: including '\0'
				retval = text_str;
				done = true;
				break;
		}
	}
	return ( retval );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
shell_char_t* Command_Line_Interpreter::Parse_Next_Token( shell_char_t* text_str )
{
	bool done = false;
	shell_char_t* retval = nullptr;

	if ( text_str == nullptr )
	{
		done = true;
	}

	uint32_t quote_nest_level = 0U;

	while ( !done )
	{
		switch ( *text_str )
		{
			case '\"':
				if ( quote_nest_level == 0U )
				{
					quote_nest_level++;
				}
				else
				{
					quote_nest_level--;
				}
				text_str++;
				break;

			case ' ':	// NOTE: fall-through intended!!!
			case '\t':
				if ( quote_nest_level == 0U )
				{
					// we're outside of quotation, so terminate the token
					*text_str = '\0';
					text_str++;
					if ( *text_str == '\0' )
					{
						// following string is empty
						done = true;
					}
					else
					{
						retval = text_str;
						done = true;
					}
				}
				else
				{
					// we're inside a quotation, so skip it
					text_str++;
				}
				break;

			case '\0':
				done = true;
				break;

			default:
				text_str++;
				break;
		}
	}
	return ( retval );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Command_Line_Interpreter::String_Equal_Ignore_Case( const shell_char_t* string1,
														 const shell_char_t* string2 )
{
	bool done = false;
	bool result = false;
	shell_char_t char1;
	shell_char_t char2;

	if ( ( string1 != nullptr ) &&
		 ( string2 != nullptr ) )
	{
		while ( !done )
		{
			// fetch 'em
			char1 = *string1;
			string1++;
			char2 = *string2;
			string2++;

			// if both terminate together, it's a match
			if ( ( char1 == '\0' ) && ( char2 == '\0' ) )
			{
				result = true;
				done = true;
			}

			// if either terminates before other, it's not a match
			if ( ( char1 == '\0' ) || ( char2 == '\0' ) )
			{
				done = true;
			}

			// if characters differ, it's not a match
			if ( toupper( int( char2 ) ) != toupper( int( char1 ) ) )
			{
				done = true;
			}
		}
	}
	return ( result );
}

}	// Namespace Shell
