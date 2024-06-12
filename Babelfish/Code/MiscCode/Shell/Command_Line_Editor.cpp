/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class implementation for Shell::Command_Line_Editor::.
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

Shell_Base* Command_Line_Editor::m_shell_ptr = ( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Command_Line_Editor::Command_Line_Editor( Shell_Base* shell_ptr ) :
	// m_shell_ptr( shell_ptr ),
	m_cle_state( INITIAL ),
	m_buffer( nullptr ),
	m_index( 0U )
{
	m_shell_ptr = shell_ptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Command_Line_Editor::~Command_Line_Editor()
{
	m_shell_ptr = nullptr;
	m_buffer = nullptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::Initialize( void )
{
	Change_State( EDIT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::On_Completion( Shell_Base::command_status_t status )
{
	// TODO use status somehow (to affect CLE prompt, or?) ...
	switch ( status )
	{
		case Shell_Base::SUCCESS:	// Note: fall-through intended
		case Shell_Base::FAILURE:	// Note: fall-through intended
		case Shell_Base::BUSY:		// Note: fall-through intended
		case Shell_Base::ABORTED:	// Note: fall-through intended
		case Shell_Base::NOT_FOUND:	// Note: fall-through intended
			Change_State( EDIT );
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Command_Line_Editor::On_Arrival( shell_char_t in_char )
{
	bool result = false;

	switch ( m_cle_state )
	{
		case EDIT:
			result = On_Edit( in_char );
			break;

		case PROCESS:
			On_Process( in_char );
			break;

		case INITIAL:
		default:
			On_Ignore();
			break;
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Command_Line_Editor::On_Edit( shell_char_t in_char )
{
	bool result = false;

	if ( static_cast<uint8_t>( in_char ) <= static_cast<uint8_t>( Shell_Base::ASCII_DEL ) )
	{
		switch ( static_cast<uint32_t>( in_char ) )
		{
			case Shell_Base::ASCII_NUL:	// NOTE: fall-through intended
			case Shell_Base::ASCII_CR:
				On_Ignore();
				break;

			case Shell_Base::ASCII_LF:
				On_Line_Feed();
				Change_State( PROCESS );
				result = true;
				break;

			case Shell_Base::ASCII_BS:
				On_Backspace();
				break;

			case Shell_Base::ASCII_TAB:
				On_Printable( in_char );
				break;

			case Shell_Base::ASCII_CAN:
				On_Cancel();
				break;

			default:
				if ( static_cast<uint8_t>( in_char ) >= static_cast<uint8_t>( Shell_Base::ASCII_SPACE ) )
				{
					On_Printable( in_char );
				}
				else
				{
					On_Unprintable();
				}
				break;
		}
	}
	else
	{
		On_Unprintable();
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::On_Process( shell_char_t in_char )
{
	if ( static_cast<uint8_t>( in_char ) <= static_cast<uint8_t>( Shell_Base::ASCII_DEL ) )
	{
		switch ( static_cast<uint32_t>( in_char ) )
		{
			case Shell_Base::ASCII_CAN:
				On_Abort();
				break;

			default:
				On_Ignore();
				break;
		}
	}
	else
	{
		On_Ignore();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::Change_State( cle_state_t new_state )
{
	switch ( m_cle_state )
	{
		case INITIAL:
			if ( new_state == EDIT )
			{
				m_cle_state = new_state;
				Get_Shell_Ptr()->Put_Startup_String();
				Get_Shell_Ptr()->Put_Prompt_String();
				m_buffer = Get_Shell_Ptr()->Get_New_Command_Buffer();
				m_index = 0U;
			}
			break;

		case EDIT:
			if ( new_state == PROCESS )
			{
				m_cle_state = new_state;
			}
			break;

		case PROCESS:
			if ( new_state == EDIT )
			{
				m_cle_state = new_state;
				Get_Shell_Ptr()->Put_Prompt_String();
				Get_Shell_Ptr()->Release_Command_Buffer( m_buffer );
				m_buffer = Get_Shell_Ptr()->Get_New_Command_Buffer();
				m_index = 0U;
			}
			break;

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::On_Ignore( void ) const
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::On_Line_Feed( void )
{
	Emit_Newline();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::On_Backspace( void )
{
	if ( m_index != 0U )
	{
		Emit_Rubout_Sequence();
		--m_index;
		m_buffer[m_index] = '\0';
	}
	else
	{
		Emit_Bell();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::On_Cancel( void )
{
	Emit_Bell();
	for ( uint32_t index = 0U; index < m_index; index++ )
	{
		Emit_Rubout_Sequence();
	}
	m_index = 0U;
	m_buffer[0] = '\0';
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::On_Abort( void )
{
	Get_Shell_Ptr()->Get_Command_Ptr()->Async_Abort();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::On_Printable( shell_char_t in_char )
{
	if ( m_index < ( Get_Shell_Config()->m_command_buffer_size - 2U ) )
	{
		m_buffer[m_index] = in_char;
		m_index++;
		m_buffer[m_index] = '\0';
		Get_Shell_Ptr()->Put_Char( in_char );
	}
	else
	{
		if ( static_cast<uint8_t>( in_char ) == static_cast<uint8_t>( Shell_Base::ASCII_BS ) )
		{
			On_Backspace();
		}
		else
		{
			Emit_Bell();
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::On_Unprintable( void )
{
	m_buffer[m_index] = '#';
	m_index++;
	m_buffer[m_index] = '\0';
	Get_Shell_Ptr()->Put_Char( '#' );
	Emit_Bell();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::Emit_Rubout_Sequence( void )
{
	Get_Shell_Ptr()->Put_Char( Shell_Base::ASCII_BS );
	Get_Shell_Ptr()->Put_Char( Shell_Base::ASCII_SPACE );
	Get_Shell_Ptr()->Put_Char( Shell_Base::ASCII_BS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::Emit_Bell( void )
{
	Get_Shell_Ptr()->Put_Char( Shell_Base::ASCII_BEL );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Command_Line_Editor::Emit_Newline( void )
{
	Get_Shell_Ptr()->Put_Char( Shell_Base::ASCII_LF );
}

}	// namespace Shell
