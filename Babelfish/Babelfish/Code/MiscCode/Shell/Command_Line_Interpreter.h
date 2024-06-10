/**
 *****************************************************************************************
 * @file
 *
 * @brief Class declaration for Shell::Command_Line_Interpreter::.
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef COMMAND_LINE_INTERPRETER_H
#define COMMAND_LINE_INTERPRETER_H

/**
 *****************************************************************************************
 * @brief Emily Namespace
 *****************************************************************************************
 */
namespace Shell
{

/**
 ****************************************************************************************
 * @brief CLI Command Line Interpreter.
 *
 * @details The CLI parses the command line given by the user, and dispatches it to the
 * corresponding command function.
 *
 ****************************************************************************************
 */

class Command_Line_Interpreter
{
	public:
		// constructors and destructors
		/**
		 * @brief Constructs an instance of the class.
		 * @param shell_ptr: Environment pointer.
		 */
		Command_Line_Interpreter( Shell_Base* shell_ptr );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~Command_Line_Interpreter();

		// methods used by framework
		/**
		 * @brief Initialize the instance.
		 * @note This is a placeholder for now, MISRA made me make it const.
		 * If you implement non-trivial stuff in the future, go ahead and remove the const
		 * constraint.
		 */
		void Initialize( void ) const;

		/**
		 * @brief Handles arrival of command line string from host.
		 * @note MISRA thinks this is const, but future implementations may remove the constraint.
		 * @param[in] command_str: Command string.
		 * @param[in] command_len: Length of command string.
		 * @return Shell status.
		 */
		Shell_Base::command_status_t Parse_Command_Line( shell_char_t* command_str,
														 uint32_t command_len );

		/**
		 * @brief Gets the name of the command.
		 * @return Name of the command.
		 */
		shell_char_t const* Get_Command_Name( void ) const;

		/**
		 * @brief Gets count of arguments.
		 * @return Count of arguments provided in command string.
		 */
		uint32_t Get_Argument_Count( void ) const;

		/**
		 * @brief Gets an argument string.
		 * @param[in] index: Argument index.
		 * @return Argument string, nullptr if no such argument.
		 */
		shell_char_t const* Get_Argument_String( uint32_t index ) const;

		/**
		 * @brief Clears argument table.
		 * @note FOR UNIT TESTING ONLY!
		 */
		void Clear_Argument_Table( void );

		/**
		 * @brief Sets the argument count.
		 * @note FOR UNIT TESTING ONLY!
		 * @param arg_count: Given count.
		 */
		void Set_Argument_Count( uint32_t arg_count );

		/**
		 * @brief Sets pointer to argument string into table at index.
		 * @note FOR UNIT TESTING ONLY!
		 * @param argument_str: Argument string.
		 * @param index: Index into table.
		 */
		void Set_Argument_String( shell_char_t* argument_str, uint32_t index );

		/**
		 * @brief Terminate string at newline.
		 * @note FOR UNIT TESTING ONLY!
		 * @param text_str: Pointer to string.
		 * @return Length of string to first newline.
		 */
		static uint32_t Terminate_At_Newline( shell_char_t* text_str );

		/**
		 * @brief Skips leading white space.
		 * @note FOR UNIT TESTING ONLY!
		 * @param text_str: Pointer to string.
		 * @return Pointer to first non-whitespace character in string.
		 */
		static shell_char_t* Skip_Whitespace( shell_char_t* text_str );

		/**
		 * @brief Parses the next token.
		 * @note FOR UNIT TESTING ONLY!
		 * @param text_str: Pointer to string.
		 * @return Pointer to next token within string.
		 */
		static shell_char_t* Parse_Next_Token( shell_char_t* text_str );

		/**
		 * @brief Compares two strings, ignoring upper/lower case.
		 * @param string1: First string.
		 * @param string2: Second string.
		 * @return Returns true if strings match.
		 */
		static bool String_Equal_Ignore_Case( const shell_char_t* string1,
											  const shell_char_t* string2 );

	private:
		/**
		 * @brief Pointer to environment.
		 */
		Shell_Base* m_shell_ptr;

		/**
		 * @brief Array of pointer to argument strings.
		 */
		shell_char_t** m_argument_tbl;

		/**
		 * @brief Number of arguments 0 to (m_config_ptr->m_argument_table_size - 1).
		 */
		uint32_t m_argument_cnt;

		/**
		 * @brief Gets environment.
		 * @return Pointer to environment.
		 */
		Shell_Base* Get_Shell_Ptr( void ) const
		{
			return ( m_shell_ptr );
		}

		/**
		 * @brief Gets environment configuration.
		 * @return Pointer to environment configuration structure.
		 */
		Shell_Base::shell_config_t const* Get_Shell_Config( void ) const
		{
			return ( m_shell_ptr->Get_Shell_Config() );
		}

		/**
		 * @brief Handles validated arrival of command line string from host.
		 * @note MISRA thinks this is const, but future implementations may remove the constraint.
		 * param[in] command_str: Command string.
		 * returns Shell status.
		 */
		Shell_Base::command_status_t Parse_Command_Line_Helper( shell_char_t* command_str );

		/**
		 * @brief Privatized Copy Constructor.
		 */
		Command_Line_Interpreter( const Command_Line_Interpreter& object );

		/**
		 * @brief Privatized Assignment Operator.
		 */
		Command_Line_Interpreter & operator =( const Command_Line_Interpreter& object );

};

}

#endif	/* COMMAND_LINE_INTERPRETER_H */
