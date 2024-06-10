/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class declaration for Shell::Command_Line_Editor::
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef COMMAND_LINE_EDITOR_H
#define COMMAND_LINE_EDITOR_H

namespace Shell
{

/**
 ****************************************************************************************
 * @brief CLE Command Line Editor.
 *
 * @details The CLE handles the editing of a stream of characters as typed by a human user
 * at an ASCII terminal keyboard. It presumes only a minimum functionality of the terminal,
 * such as ability to perform backspace and bell.
 ****************************************************************************************
 */

class Command_Line_Editor
{
	public:
		/**
		 * @brief Enumeration of CLE finite state machine states.
		 */
		enum cle_state_t
		{
			INITIAL,//!< Initial
			EDIT,	//!< Edit
			PROCESS	//!< Process
		};

		/**
		 * @brief Constructs an instance of the class.
		 * @param[in] shell_ptr: Shell pointer.
		 */
		Command_Line_Editor( Shell_Base* shell_ptr );

		/**
		 * @brief Destroys an instance of the class.
		 */
		virtual ~Command_Line_Editor();

		/**
		 * @brief Initialize the instance.
		 */
		void Initialize( void );

		/**
		 * @brief Handles arrival of character from host.
		 * @param[in] in_char: Character.
		 * @return True if newline character.
		 */
		bool On_Arrival( shell_char_t in_char );

		/**
		 * @brief Handles completion of asynchronous commands.
		 * @param[in] status
		 */
		void On_Completion( Shell_Base::command_status_t status );

		/**
		 * @brief Gets CLE state.
		 * @return Current state of CLE finite state machine.
		 */
		cle_state_t Get_State( void )
		{
			return ( m_cle_state );
		}

		/**
		 * @brief Gets buffer (for unit test).
		 * @note FOR UNIT TESTING ONLY!
		 * @return Pointer to buffer.
		 */
		shell_char_t* Get_Buffer_Ptr( void ) { return ( m_buffer ); }

		/**
		 * @brief Gets buffer index (for unit test)
		 * @note FOR UNIT TESTING ONLY!
		 * @return Index into buffer.
		 */
		uint32_t Get_Buffer_Index( void ) { return ( m_index ); }

		/**
		 * @brief Gets environment.
		 * @return Pointer to environment.
		 */
		static Shell_Base* Get_Shell_Ptr( void )
		{
			return ( m_shell_ptr );
		}

	protected:
		/**
		 * @brief Pointer to our environment.
		 */
		static Shell_Base* m_shell_ptr;

		/**
		 * @brief CLE state.
		 */
		cle_state_t m_cle_state;

		/**
		 * @brief Buffer to store command line while editing.
		 */
		shell_char_t* m_buffer;

		/**
		 * @brief Index into m_buffer.
		 */
		uint32_t m_index;

		/**
		 * @brief Gets environment configuration.
		 * @return Pointer to environment configuration structure.
		 */
		Shell_Base::shell_config_t const* Get_Shell_Config( void )
		{
			return ( m_shell_ptr->Get_Shell_Config() );
		}

		/**
		 * @brief Get CLI command line interpreter.
		 * @return Pointer to CLI instance.
		 */
		Command_Line_Interpreter* Get_Cli_Ptr( void )
		{
			return ( m_shell_ptr->Get_Cli_Ptr() );
		}

		/**
		 * @brief Changes state.
		 * @param new_state: New state.
		 */
		void Change_State( cle_state_t new_state );

		/**
		 * @brief Handles arrival of character from host (during editing state).
		 * param[in] in_char: Character.
		 */
		bool On_Edit( shell_char_t in_char );

		/**
		 * @brief Handles arrival of character from host (during processing state).
		 * param[in] in_char: Character.
		 */
		void On_Process( shell_char_t in_char );

		/**
		 * @brief Handles arrival of ignorable character.
		 */
		void On_Ignore( void ) const;

		/**
		 * @brief Handles arrival of line feed character.
		 */
		void On_Line_Feed( void );

		/**
		 * @brief Handles arrival of backspace character.
		 */
		void On_Backspace( void );

		/**
		 * @brief Handles arrival of cancel (ctrl-X) character (during editing state).
		 */
		void On_Cancel( void );

		/**
		 * @brief Handles arrival of cancel (ctrl-X) character (during processing state).
		 */
		void On_Abort( void );

		/**
		 * @brief Handles arrival of printable character.
		 * @param[in] in_char: Character to handle.
		 */
		void On_Printable( shell_char_t in_char );

		/**
		 * @brief Handles arrival of unprintable character.
		 */
		void On_Unprintable( void );

		/**
		 * @brief Emits rubout sequence of characters.
		 */
		void Emit_Rubout_Sequence( void );

		/**
		 * @brief Emits bell character.
		 */
		void Emit_Bell( void );

		/**
		 * @brief Emits newline sequence of characters.
		 */
		void Emit_Newline( void );

};

}	// namespace Emily

#endif	/* COMMAND_LINE_EDITOR_H */
