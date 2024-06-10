/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class declaration for Shell::Test_Base::
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef Test_Base_H
#define Test_Base_H

namespace Shell
{

/**
 ****************************************************************************************
 * @brief Test_Base (abstract base class).
 *
 * @details Each test command to be executed is derived this intermediate base class.
 *
 ****************************************************************************************
 */

class Test_Base : public Async_Command_Base
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 * @param[in] cmd_config_ptr: Pointer to configuration structure.
		 */
		Test_Base( Shell_Base::command_config_t const* cmd_config_ptr );

		/**
		 * @brief Destroys an instance of the class.
		 */
		virtual ~Test_Base();

		/**
		 * @brief Initiates execution of the command function.
		 * @details Called by CmdLineInterpreter:: thread context.
		 * Derived classes must implement this virtual function.
		 * @param[in] shell_ptr: Pointer to invoking environment.
		 * @return Command function status - Busy if executing asynchronously.
		 */
		Shell_Base::command_status_t Execute( Shell_Base* shell_ptr );

		/**
		 * @brief Get status of currently executing test.
		 * @return See enum test_status_t for list of return values.
		 */
		test_status_t Get_Test_Status( void );

		/**
		 * @brief Invoke the test with TEST_ENUMS_START.
		 *
		 * @note Implementation provided by derived classes.
		 *
		 * @return See enum test_result_t for list of return values.
		 */
		virtual test_status_t Invoke_Test( test_command_t cmd ) = 0;

	protected:

		/**
		 * variable for storing the class and function tested in the test case
		 * gives more information in the automated Test reports
		 */
		shell_char_t* m_function_tested;

		/**
		 * variable for storing the test criteria based on which the test is marked as failed
		 * gives more information in the automated Test reports
		 */
		shell_char_t* m_test_criteria;

};

}	// namespace Emily

#endif	/* Test_Base_H_ */
