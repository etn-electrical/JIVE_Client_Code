/**
 *****************************************************************************************
 * @file
 * @brief
 *
 *****************************************************************************************
 */

#include "Shell_Includes.h"

#include <stdio.h>
#include <stdarg.h>
#include "Shell_Base.h"
#include "Command_Base.h"
#include "Command_Line_Editor.h"
#include "Command_Line_Interpreter.h"

#ifdef UART_DEBUG
#define UART_BUFF_SIZE 1024
char szBuf[UART_BUFF_SIZE];
#endif

/*
 *****************************************************************************************
 * BF_Shell_Examples_TEST_1()
 *****************************************************************************************
 */

static test_status_t g_TEST_1_Status = TEST_ENUMS_UNKNOWN;

test_status_t BF_Shell_Examples_TEST_1( test_command_t cmd, Shell::Shell_Base* ptr )
{
	ptr->Set_Tested_Function_Name( "g_TEST_1_Status = TEST_ENUMS_PASSED" );
	ptr->Set_Test_Criteria( "g_TEST_1_Status = TEST_ENUMS_PASSED" );

	for ( int i = 1; i < ptr->Get_Argument_Count(); i++ )
	{
		const char* arg = ptr->Get_Argument_String( i );
		int j = ptr->Get_Argument_Count();
		printf( "arg number %d is %s", i, arg );
	}

	switch ( cmd )
	{
		case TEST_ENUMS_START:
			g_TEST_1_Status = TEST_ENUMS_PASSED;
			break;

		case TEST_ENUMS_GET_STATUS:
			// No action needed.
			break;

		case TEST_ENUMS_ABORT:
			g_TEST_1_Status = TEST_ENUMS_ABORTED;
			break;
	}
	return ( g_TEST_1_Status );
}

/*
 *****************************************************************************************
 * BF_Shell_Examples_TEST_2()
 *****************************************************************************************
 */

static test_status_t g_TEST_2_Status = TEST_ENUMS_UNKNOWN;

test_status_t BF_Shell_Examples_TEST_2( test_command_t cmd, Shell::Shell_Base* ptr )
{
	switch ( cmd )
	{
		case TEST_ENUMS_START:
			g_TEST_2_Status = TEST_ENUMS_FAILED;
			// TODO start your test process. For example register something that will result in
			// MyTest_Callback()
			break;

		case TEST_ENUMS_GET_STATUS:
			// No action needed.
			break;

		case TEST_ENUMS_ABORT:
			g_TEST_2_Status = TEST_ENUMS_ABORTED;
			// TODO abort your test process
			break;
	}
	return ( g_TEST_2_Status );
}

/*
 *****************************************************************************************
 * BF_Shell_Examples_TEST_3()
 *****************************************************************************************
 */

static test_status_t g_TEST_3_Status = TEST_ENUMS_UNKNOWN;

void BF_Shell_Examples_TEST_3_Callback( int32_t test_results )
{
	int32_t test_acceptance_criteria = 1234;

	if ( test_results >= test_acceptance_criteria )
	{
		g_TEST_3_Status = TEST_ENUMS_PASSED;
	}
	else
	{
		g_TEST_3_Status = TEST_ENUMS_FAILED;
	}

	// NOTE: or, if something goes really bad, you can...
	// g_TEST_3_Status = TEST_ENUMS_ERROR;

	// NOTE: and if you've implemented a timeout mechanism, you can...
	// g_TEST_3_Status = TEST_ENUMS_TIMED_OUT;
}

test_status_t BF_Shell_Examples_TEST_3( test_command_t cmd, Shell::Shell_Base* ptr )
{
	static int32_t simulate_time_passing;

	for ( int i = 1; i < ptr->Get_Argument_Count(); i++ )
	{
		const char* arg = ptr->Get_Argument_String( i );
		int j = ptr->Get_Argument_Count();
		printf( "arg number %d is %s", i, arg );
	}

	switch ( cmd )
	{
		case TEST_ENUMS_START:
			g_TEST_3_Status = TEST_ENUMS_IN_PROGRESS;
			// TODO start your test process. For example register something that will result in
			// MyTest_Callback()
			simulate_time_passing = 0;
			break;

		case TEST_ENUMS_GET_STATUS:
			if ( ++simulate_time_passing > 10 )
			{
				BF_Shell_Examples_TEST_3_Callback( 5678 );
			}
			break;

		case TEST_ENUMS_ABORT:
			g_TEST_3_Status = TEST_ENUMS_ABORTED;
			// TODO abort your test process
			break;
	}
	return ( g_TEST_3_Status );
}

/*
 *****************************************************************************************
 * BF_Shell_Examples_TEST_4()
 *****************************************************************************************
 */

static test_status_t g_TEST_4_Status = TEST_ENUMS_UNKNOWN;

void BF_Shell_Examples_TEST_4_Callback( int32_t test_results )
{
	int32_t test_acceptance_criteria = 1234;

	if ( test_results >= test_acceptance_criteria )
	{
		g_TEST_4_Status = TEST_ENUMS_PASSED;
	}
	else
	{
		g_TEST_4_Status = TEST_ENUMS_FAILED;
	}

	// NOTE: or, if something goes really bad, you can...
	// g_TEST_4_Status = TEST_ENUMS_ERROR;

	// NOTE: and if you've implemented a timeout mechanism, you can...
	// g_TEST_4_Status = TEST_ENUMS_TIMED_OUT;
}

test_status_t BF_Shell_Examples_TEST_4( test_command_t cmd, Shell::Shell_Base* ptr )
{
	static int32_t simulate_time_passing;

	switch ( cmd )
	{
		case TEST_ENUMS_START:
			g_TEST_4_Status = TEST_ENUMS_IN_PROGRESS;
			// TODO start your test process. For example register something that will result in
			// MyTest_Callback()
			simulate_time_passing = 0;
			break;

		case TEST_ENUMS_GET_STATUS:
			if ( ++simulate_time_passing > 10 )
			{
				BF_Shell_Examples_TEST_4_Callback( -1 );
			}
			break;

		case TEST_ENUMS_ABORT:
			g_TEST_4_Status = TEST_ENUMS_ABORTED;
			// TODO abort your test process
			break;
	}
	return ( g_TEST_4_Status );
}

void UARTDebug( const char* strDebug, ... )
{
#ifdef UART_DEBUG
	va_list args;
	va_start( args, strDebug );
	vsprintf( szBuf, strDebug, args );
	va_end( args );

	Shell::Shell_Base* tmpPtr = Shell::Command_Line_Editor::Get_Shell_Ptr();
	if ( tmpPtr != nullptr )
	{
		tmpPtr->Put_String( szBuf );
	}
#endif
}
