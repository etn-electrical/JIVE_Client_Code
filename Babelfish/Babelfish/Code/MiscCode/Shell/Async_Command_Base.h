/**
 *****************************************************************************************
 * @file
 *
 * @brief	Class declaration for Shell::Async_Command_Base::
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/*
 * CmdFunctionAsyncBase.h
 *
 *  Created on: Apr 3, 2014
 *      Author: C9907079
 */

#ifndef ASYNC_COMMAND_BASE_H
#define ASYNC_COMMAND_BASE_H

namespace Shell
{

/**
 ****************************************************************************************
 * @brief Asynchronous Command Function (intermediate base class).
 *
 * @details Derive your asynchronous command functions from this class.
 *
 ****************************************************************************************
 */

class Async_Command_Base : public Command_Base
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 * @param[in] config_ptr
		 */
		Async_Command_Base( Shell_Base::command_config_t const* config_ptr );

		/**
		 * @brief Destroys an instance of the class.
		 */
		virtual ~Async_Command_Base();

		/**
		 * @brief Initiates execution of the command function.
		 * @details Called by CmdLineInterpreter:: thread context.
		 * Derived classes must implement this virtual function.
		 * @param[in] shell_ptr: Pointer to invoking environment.
		 * @return Command function status - Busy if executing asynchronously.
		 */
		virtual Shell_Base::command_status_t Execute( Shell_Base* shell_ptr );

		/**
		 * @brief Aborts the execution of the command function.
		 * @details Called by CmdLineInterpreter:: thread context.
		 * Derived classes which support asynchronous operation *MUST* implement
		 * this virtual function. Synchronous derivatives are expected to use
		 * the base class implementation.
		 */
		virtual void Async_Abort( void );

		/**
		 * @brief Asynchronous command function body.
		 * @details Called by CmdFunctionBase:: thread context which is asynchronous from
		 * CmdLineInterpreter::.
		 * Derived classes which support asynchronous operation *MUST* implement
		 * this virtual function. Synchronous derivatives are expected to use
		 * the base class implementation.
		 */
		virtual void Async_Body( void );

	private:
		/**
		 * @brief Length of Result String.
		 * @details Stores the maximum length of the result string.
		 */
		static const uint32_t RESULT_STRING_LENGTH = 80U;
};

}	// namespace Emily

#endif	/* ASYNC_COMMAND_BASE_H */
