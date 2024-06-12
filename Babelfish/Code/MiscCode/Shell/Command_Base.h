/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class declaration for Shell::Command_Base::
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef COMMAND_BASE_H
#define COMMAND_BASE_H

namespace Shell
{

/**
 ****************************************************************************************
 * @brief Command_Base (abstract base class).
 *
 * @details Each command to be executed is derived this abstract base class.
 *
 ****************************************************************************************
 */

class Command_Base
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 * @param[in] cmd_config_ptr: Pointer to configuration structure.
		 */
		Command_Base( Shell_Base::command_config_t const* cmd_config_ptr );

		/**
		 * @brief Destroys an instance of the class.
		 */
		virtual ~Command_Base();

		/**
		 * @brief Gets pointer to configuration structure.
		 * @return
		 */
		Shell_Base::command_config_t const* Get_Cmd_Config( void ) const;

		/**
		 * @brief Gets the status.
		 * @details Called by CmdLineInterpreter:: thread context.
		 * @return Command function status.
		 */
		Shell_Base::command_status_t Get_Status( void );

		/**
		 * @brief Initiates execution of the command function.
		 * @details Called by CmdLineInterpreter:: thread context.
		 * Derived classes must implement this virtual function.
		 * @param[in] shell_ptr: Pointer to invoking environment.
		 * @return Command function status - Busy if executing asynchronously.
		 * @code
		   // example template for synchronous implementation
		   CmdFunctionBase::cmd_status_t Your_Synchronous_Derivative::Execute( Shell_Base* shell_ptr
		 *)
		   {
		   // recommended prolog
		   shell_char_t your_result_string[80];
		   Sync_Start( shell_ptr );

		   // TODO implement your function here
		   sprintf(your_result_string, "Hello, world!!!\n");

		   // recommended epilog
		   Put_String(your_result_string);
		   return Sync_Finish( SUCCESS );
		   }

		   // example template for asynchronous implementation
		   CmdFunctionBase::cmd_status_t Your_Asynchronous_Derivative::Execute( Shell_Base*
		 * shell_ptr
		 *)
		   {
		   // recommended prolog
		   Async_Start( shell_ptr );

		   // TODO implement spawning Async_Body() asynchronously on your platform.
		   YOUR_PLATFORM_SPAWN(Async_Body);

		   // recommended epilog
		   return Busy;
		   }
		 * @endcode
		 */
		virtual Shell_Base::command_status_t Execute( Shell_Base* shell_ptr ) = 0;

		/**
		 * @brief Aborts the execution of the command function.
		 * @details Called by CmdLineInterpreter:: thread context.
		 * Derived classes which support asynchronous operation *MUST* implement
		 * this virtual function. Synchronous derivatives are expected to use
		 * the base class implementation.
		 * @code
		   void Your_Asynchronous_Derivative::Async_Abort(void)
		   {
		   // recommended prolog
		   shell_char_t your_result_string[80];
		   GetEnvPtr()->Enter_Critical();

		   // TODO implement killing your asynchronously spawned Async_Body()
		   YOUR_PLATFORM_KILL(Async_Body);

		   // recommended epilog
		   GetEnvPtr()->Exit_Critical();

		   sprintf(your_result_string, "Your_Asynchronous_Derivative aborted...\n");

		   // recommended epilog
		   Put_String(your_result_string);
		   Async_Finish( ABORTED );
		   }
		 * @endcode
		 */
		virtual void Async_Abort( void );

	protected:
		/**
		 * @brief Pointer to configuration structure (typically in ROM flash memory).
		 */
		Shell_Base::command_config_t const* m_config_ptr;

		/**
		 * @brief Puts a string out via the CLI.
		 * @details Called by CmdFunctionBase:: thread context, which might be asynchronous from
		 * CmdLineInterpreter::.
		 * @param[in] put_string: The given string.
		 */
		void Put_String( shell_char_t* put_string );

		/**
		 * @brief Commences asynchronous execute.
		 * @details Called by CmdLineInterpreter:: thread context.
		 * @param shell_ptr: Pointer to invoking environment.
		 */
		void Async_Start( Shell_Base* shell_ptr );

		/**
		 * @brief Asynchronous command function body.
		 * @details Called by CmdFunctionBase:: thread context which is asynchronous from
		 * CmdLineInterpreter::.
		 * Derived classes which support asynchronous operation *MUST* implement
		 * this virtual function. Synchronous derivatives are expected to use
		 * the base class implementation.
		 * @code
		   void Your_Asynchronous_Derivative::Async_Body(void)
		   {
		   // recommended prolog
		   shell_char_t your_result_string[80];

		   // TODO implement your function here
		   sprintf(your_result_string, "Hello, asynchronous world!!!\n");

		   // recommended epilog
		   Put_String(your_result_string);
		   Async_Finish( SUCCESS );
		   }
		 * @endcode
		 */
		virtual void Async_Body( void );

		/**
		 * @brief Completes asynchronous execution, and notifies the CLI.
		 * @details Called by CmdFunctionBase:: thread context which is asynchronous from
		 * CmdLineInterpreter::.
		 * @param[in] status: Status result of command function.
		 */
		void Async_Finish( Shell_Base::command_status_t status );

		/**
		 * @brief Commences synchronous execution.
		 * @details Called by CmdLineInterpreter:: thread context.
		 * @param[in] shell_ptr: Pointer to invoking environment.
		 */
		void Sync_Start( Shell_Base* shell_ptr );

		/**
		 * @brief Completes synchronous execution.
		 * @details Called by CmdLineInterpreter:: thread context.
		 * @param[in] status: Status result of command function.
		 * @return Status result of command function.
		 */
		Shell_Base::command_status_t Sync_Finish( Shell_Base::command_status_t status );

		/**
		 * @brief Status of command function.
		 */
		Shell_Base::command_status_t m_status;

		/**
		 * @brief Pointer to environment.
		 */
		Shell_Base* m_shell_ptr;

		/**
		 * @brief Gets environment.
		 * @return Pointer to environment.
		 */
		Shell_Base* Get_Shell_Ptr( void )
		{
			return ( m_shell_ptr );
		}

		/**
		 * @brief Gets environment configuration.
		 * @return Pointer to environment configuration structure.
		 */
		Shell_Base::shell_config_t const* Get_Shell_Config( void )
		{
			return ( m_shell_ptr->Get_Shell_Config() );
		}

		/**
		 * @brief Get CLE command line editor.
		 * @return Pointer to CLE instance.
		 */
		Command_Line_Editor* Get_Cle_Ptr( void )
		{
			return ( m_shell_ptr->Get_Cle_Ptr() );
		}

		/**
		 * @brief Get CLI command line interpreter.
		 * @return Pointer to CLI instance.
		 */
		Command_Line_Interpreter* Get_Cli_Ptr( void )
		{
			return ( m_shell_ptr->Get_Cli_Ptr() );
		}

};

}	// namespace Emily

#endif	/* COMMAND_BASE_H */
