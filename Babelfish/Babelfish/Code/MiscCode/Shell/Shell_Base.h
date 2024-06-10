/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class declaration for Shell::Shell_Base::
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef SHELL_BASE_H
#define SHELL_BASE_H

namespace Shell
{
/**
 * @brief ASCII character data type.
 */
typedef char shell_char_t;


/**
 ****************************************************************************************
 * @brief Abstract base class describing target environment.
 *
 * @details Shell is agnostic of the details of the environment, such as how the character
 * stream is implemented (e.g. via UART, USB, Tcp stream, etc.) or whether or not
 * an RTOS is used. This class encapsulates and hides such platform-specific details.
 *
 ****************************************************************************************
 */

class Shell_Base
{
	friend class Command_Line_Interpreter;
	friend class Command_Line_Editor;

	public:
		// ASCII (American Standard Code for Information Interchange) characters.
		// For reference, see http://www.asciitable.com/.
		static const shell_char_t ASCII_NUL = 0x00;					///< ASCII character  NUL
		static const shell_char_t ASCII_SOH = 0x01;					///< ASCII character  start of
		// heading
		static const shell_char_t ASCII_STX = 0x02;					///< ASCII character  start of
		// text
		static const shell_char_t ASCII_ETX = 0x03;					///< ASCII character  end of text
		static const shell_char_t ASCII_EOT = 0x04;					///< ASCII character  end of
		// transmission
		static const shell_char_t ASCII_ENQ = 0x05;					///< ASCII character  enquiry
		static const shell_char_t ASCII_ACK = 0x06;					///< ASCII character  acknowledge
		static const shell_char_t ASCII_BEL = 0x07;					///< ASCII character  bell
		static const shell_char_t ASCII_BS = 0x08;					///< ASCII character  backspace
		static const shell_char_t ASCII_TAB = 0x09;					///< ASCII character  tab
		static const shell_char_t ASCII_LF = 0x0A;					///< ASCII character  line feed,
		// new line
		static const shell_char_t ASCII_VT = 0x0B;					///< ASCII character  vertical
		// tab
		static const shell_char_t ASCII_FF = 0x0C;					///< ASCII character  form feed,
		// new page
		static const shell_char_t ASCII_CR = 0x0D;					///< ASCII character  carriage
		// return
		static const shell_char_t ASCII_SO = 0x0E;					///< ASCII character  shift out
		static const shell_char_t ASCII_SI = 0x0F;					///< ASCII character  shift in

		static const shell_char_t ASCII_DLE = 0x10;					///< ASCII character  data link
		// escape
		static const shell_char_t ASCII_DC1 = 0x11;					///< ASCII character  device
		// control 1
		static const shell_char_t ASCII_DC2 = 0x12;					///< ASCII character  device
		// control 2
		static const shell_char_t ASCII_DC3 = 0x13;					///< ASCII character  device
		// control 3
		static const shell_char_t ASCII_DC4 = 0x14;					///< ASCII character  device
		// control 4
		static const shell_char_t ASCII_NAK = 0x15;					///< ASCII character  negative
		// acknowledge
		static const shell_char_t ASCII_SYN = 0x16;					///< ASCII character  synchronous
		// idle
		static const shell_char_t ASCII_ETB = 0x17;					///< ASCII character  end of
		// trans. block
		static const shell_char_t ASCII_CAN = 0x18;					///< ASCII character  cancel
		static const shell_char_t ASCII_EM = 0x19;					///< ASCII character  end of
		// medium
		static const shell_char_t ASCII_SUB = 0x1A;					///< ASCII character  substitute
		static const shell_char_t ASCII_ESC = 0x1B;					///< ASCII character  escape
		static const shell_char_t ASCII_FS = 0x1C;					///< ASCII character  file
		// separator
		static const shell_char_t ASCII_GS = 0x1D;					///< ASCII character  group
		// separator
		static const shell_char_t ASCII_RS = 0x1E;					///< ASCII character  record
		// separator
		static const shell_char_t ASCII_US = 0x1F;					///< ASCII character  unit
		// separator

		static const shell_char_t ASCII_SPACE = 0x20;				///< ASCII character  space
		static const shell_char_t ASCII_DEL = 0x7F;					///< ASCII character  delete
		// (rubout)

		/**
		 ****************************************************************************************
		 * @brief Configuration structure for the shell.
		 * @note This structure is designed to contain only constant values such that it may be
		 * located in read-only flash memory.
		 ****************************************************************************************
		 */
		struct shell_config_t
		{
			/** @brief Allocated size of command buffer. */
			uint32_t m_command_buffer_size;

			/** @brief Allocated size of results buffer. */
			uint32_t m_result_buffer_size;

			/** @brief Minimum length of a valid command line. */
			uint32_t m_min_command_len;

			/** @brief Allocated size of command argument table. */
			uint32_t m_argument_table_size;

			/** @brief Array of pointer to commands */
			Command_Base** m_command_table;

			/** @brief Count of items in command table */
			uint32_t m_command_cnt;

			/** @brief Startup string. */
			shell_char_t const* m_startup_string;

			/** @brief Prompt string. */
			shell_char_t const* m_prompt_string;

			/* FUTURE ideas
			 *      flag for bell?
			 *      flag for remote echo?
			 *      flag for bs-sp-bs editing?
			 *      flag for xon/xoff????
			 *      character for cancel ctrl-X vs ctrl-C vs ctrl-Z
			 * */

		};

		/**
		 * @brief Typical default values for shell_config_t.
		 */
		static const shell_config_t DEFAULT_SHELL_CONFIG;

		/**
		 * @brief Enumeration of command function status values.
		 */
		enum command_status_t
		{
			/** @brief Command completed successfully. */
			SUCCESS,

			/** @brief Command completed with errors. */
			FAILURE,

			/** @brief Command is currently executing asynchronously. */
			BUSY,

			/** @brief Asynchronous command execution was aborted. */
			ABORTED,

			/** @brief Command not found. */
			NOT_FOUND
		};

		/**
		 * @brief Structure to define configuration characteristics for CmdFunctionBase::
		 * @note This structure is designed to contain only constant values such that it may be
		 * located in read-only flash memory.
		 */
		struct command_config_t
		{
			/** @brief Name of command function. */
			shell_char_t* m_name;

			/** @brief Help description of command function. */
			shell_char_t* m_description;

			/** @brief Minimum number of arguments required. */
			uint32_t m_min_argument_cnt;

			/** @brief Maximum number of arguments permitted. */
			uint32_t m_max_argument_cnt;
		};

		/**
		 * @brief Constructs an instance of the class.
		 * @param shell_config_ptr: Configuration structure.
		 * @param cle: CLE command line editor (use nullptr, provided only for unit testability).
		 * @param cli: CLI command line interpreter (use nullptr, provided only for unit
		 * testability).
		 */
		Shell_Base( shell_config_t const* shell_config_ptr, Command_Line_Editor* cle = nullptr,
					Command_Line_Interpreter* cli = nullptr );

		/**
		 * @brief Destroys an instance of the class.
		 */
		virtual ~Shell_Base();

		/**
		 * @brief Initialize the instance.
		 */
		void Initialize( void );

		/**
		 * @brief Puts a character out to the host environment.
		 * @param out_char: The given character.
		 */
		virtual void Put_Char( shell_char_t out_char ) = 0;

		/**
		 * @brief Puts a string out to the host environment.
		 * @param out_string: The given string.
		 */
		virtual void Put_String( shell_char_t const* out_string ) = 0;

		/**
		 * @brief Puts a string out to the host environment.
		 * @param out_string: The given string.
		 * @param bytes_to_send: Number of bytes to transmit
		 */
		virtual void Put_String( shell_char_t const* out_string, uint32_t bytes_to_send ) = 0;

		/**
		 * @brief BF shell uses CR tasker to flush all data on serial terminal
		 * 		  This function will be used to transmit data on serial independent of
		 * 		  CR tasker. Hence the usage of this function should me minimum
		 * 		  and only on places where it is required
		 */
		virtual void Put_String_Now( shell_char_t const* out_string, uint32_t bytes_to_send ) = 0;

		/**
		 * @brief Enters critical section.
		 * @details Called by either CLE/CLI thread context or asynchronous command function thread
		 * context.
		 * @code
		   void Your_Derivative_Environment::Enter_Critical(void)
		   {
		   // TODO implement entering critical section e.g. lock mutex, disable RTOS preemption,
		 * disable interrupts, etc.
		   YOUR_PLATFORM_ENTER_CRITICAL();
		   }
		 * @endcode
		 */
		virtual void Enter_Critical( void ) = 0;

		/**
		 * @brief Exits critical section.
		 * @details Called by either CLE/CLI thread context or asynchronous command function thread
		 * context.
		 * @code
		   void Your_Derivative_Environment::Exit_Critical(void)
		   {
		   // TODO implement exiting critical section e.g. unlock mutex, enable RTOS preemption,
		 * enable interrupts, etc.
		   YOUR_PLATFORM_EXIT_CRITICAL();
		   }
		 * @endcode
		 */
		virtual void Exit_Critical( void ) = 0;

		/**
		 * @brief Gets a new command buffer.
		 * @details Size is defined in configuration struct.
		 * @return Pointer to new command buffer.
		 */
		virtual shell_char_t* Get_New_Command_Buffer( void ) = 0;

		/**
		 * @brief Releases command buffer.
		 * @param[in] buffer: Buffer to be released (previously allocated with
		 * Get_New_Command_Buffer()).
		 * @return Pointer to new command buffer.
		 */
		virtual void Release_Command_Buffer( shell_char_t* buffer ) = 0;

		/**
		 * @brief Get configuration pointer.
		 * @return Pointer to configuration structure.
		 */
		shell_config_t const* Get_Shell_Config( void ) const
		{
			return ( m_shell_config_ptr );
		}

		/**
		 * @brief Get CLE command line editor.
		 * @return Pointer to CLE instance.
		 */
		Command_Line_Editor* Get_Cle_Ptr( void ) const
		{
			return ( m_cle );
		}

		/**
		 * @brief Get CLI command line interpreter.
		 * @return Pointer to CLI instance.
		 */
		Command_Line_Interpreter* Get_Cli_Ptr( void ) const
		{
			return ( m_cli );
		}

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
		 * @brief Gets command function.
		 * @return Pointer to current command function.
		 */
		Command_Base* Get_Command_Ptr( void )
		{
			return ( m_command_ptr );
		}

		/**
		 * @brief Handles arrival of character from host.
		 * @param[in] in_char: Character.
		 * @return True if newline character.
		 */
		bool On_Arrival( shell_char_t in_char );

		/**
		 * @brief Handles arrival of command line string from host.
		 * returns Shell status.
		 */
		Shell_Base::command_status_t Parse_Command_Line( void );

		/**
		 * @brief Dispatches parsed command line to correct command function.
		 * returns Shell status.
		 */
		command_status_t Dispatch( void );

		virtual void Set_Tested_Function_Name( const shell_char_t* name ) {       }

		virtual void Set_Test_Criteria( const shell_char_t* name ) {       }

	protected:

		/**
		 * @brief Pointer to environment configuration structure
		 */
		shell_config_t const* m_shell_config_ptr;

		/**
		 * @brief Pointer to CLE command line editor.
		 */
		Command_Line_Editor* m_cle;

		/**
		 * @brief Pointer to CLI command line interpreter.
		 */
		Command_Line_Interpreter* m_cli;

		/**
		 * @brief Pointer to currently executing command.
		 */
		Command_Base* m_command_ptr;

		/**
		 * @brief Puts startup string.
		 */
		void Put_Startup_String( void )
		{
			Put_String( m_shell_config_ptr->m_startup_string );
		}

		/**
		 * @brief Puts prompt string.
		 */
		void Put_Prompt_String( void )
		{
			Put_String( m_shell_config_ptr->m_prompt_string );
		}

		/**
		 * @brief Sets current command function.
		 * @param cmd_function: Given command function.
		 */
		void Set_Command_Ptr( Command_Base* cmd_function )
		{
			m_command_ptr = cmd_function;
		}

		/**
		 * @brief Internal initialization.
		 */
		virtual void Internal_Initialize( void );

	private:
		/**
		 * @brief Privatized Copy Constructor.
		 */
		Shell_Base( const Shell_Base& object );

		/**
		 * @brief Privatized Assignment Operator.
		 */
		Shell_Base & operator =( const Shell_Base& object );

};

}	// namespace Shell

#endif	/* SHELL_BASE_H */
