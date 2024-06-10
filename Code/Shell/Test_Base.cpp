/**
 *****************************************************************************************
 * @file
 *
 * @brief   Class implementation for Shell::TestBase::.
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Shell_Includes.h"

namespace Shell
{
/**
 * @brief Lookup table to map test_status_t to shell response strings.
 */
static const char* G_SHELL_TEST_RESULT_STRINGS[] =
{
	"Unknown",	// TEST_ENUMS_UNKNOWN
	"Busy",		// TEST_ENUMS_IN_PROGRESS
	"Pass",		// TEST_ENUMS_PASSED
	"Fail",		// TEST_ENUMS_FAILED
	"Aborted",	// TEST_ENUMS_ABORTED
	"Error",	// TEST_ENUMS_ERROR
	"Timedout"	// TEST_ENUMS_TIMED_OUT
};

static const Shell_Base::command_status_t G_STATUS_TABLE_LOOKUP[] =
{
	Shell_Base::FAILURE,			// TEST_ENUMS_UNKNOWN
	Shell_Base::BUSY,				// TEST_ENUMS_IN_PROGRESS
	Shell_Base::SUCCESS,			// TEST_ENUMS_PASSED
	Shell_Base::SUCCESS,			// TEST_ENUMS_FAILED
	Shell_Base::FAILURE,			// TEST_ENUMS_ABORTED
	Shell_Base::FAILURE,			// TEST_ENUMS_ERROR
	Shell_Base::ABORTED,			// TEST_ENUMS_TIMED_OUT
};


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Test_Base::Test_Base( Shell_Base::command_config_t const* cmd_config_ptr ) :
	Async_Command_Base( cmd_config_ptr )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Test_Base::~Test_Base()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shell_Base::command_status_t Test_Base::Execute( Shell_Base* shell_ptr )
{
	Term_Print( "\n%s,%s,%d", FILE_NAME, __PRETTY_FUNCTION__, __LINE__ );
	Async_Start( shell_ptr );
	static_cast<BF_Shell*>( shell_ptr )->Set_Current_Test( this );
	test_status_t status = Invoke_Test( TEST_ENUMS_START );

	m_function_tested = static_cast<BF_Shell*>( shell_ptr )->Get_Tested_Function_Name();
	m_test_criteria = static_cast<BF_Shell*>( shell_ptr )->Get_Test_Criteria();
	return ( G_STATUS_TABLE_LOOKUP[status] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
test_status_t Test_Base::Get_Test_Status( void )
{
	test_status_t status = Invoke_Test( TEST_ENUMS_GET_STATUS );

	if ( status != TEST_ENUMS_IN_PROGRESS )
	{
		shell_char_t result_string[240];
		sprintf( result_string, "%s@%s@%s\n", G_SHELL_TEST_RESULT_STRINGS[status], m_function_tested, m_test_criteria );
		Put_String( result_string );
		Async_Finish( Shell_Base::SUCCESS );
	}
	return ( status );
}

}	// namespace Shell

