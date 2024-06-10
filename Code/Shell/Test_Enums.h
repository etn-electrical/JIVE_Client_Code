/**
 *****************************************************************************************
 * @file
 *
 * @brief	Definition of enum test_result_t
 *
 * @details This type test_result_t is the return value of test functions.
 * The shell uses these values to report test results back to the TestRunner.exe
 * application.
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef TEST_ENUMS_H
#define TEST_ENUMS_H

/**
 * @brief List of test result states.
 */
enum test_status_t
{
	/** Unknown, test not started. */
	TEST_ENUMS_UNKNOWN = 0,

	/* InProgress, test is in progress. */
	TEST_ENUMS_IN_PROGRESS,

	/* Passed, test completed successfully. */
	TEST_ENUMS_PASSED,

	/* Failed, test completed but did not meet acceptance criteria. */
	TEST_ENUMS_FAILED,

	/* Aborted, test was aborted by user. */
	TEST_ENUMS_ABORTED,

	/* Error, test did not complete due to unrecoverable error. */
	TEST_ENUMS_ERROR,

	/* Timed out, test did not complete within timeout limit. */
	TEST_ENUMS_TIMED_OUT
};

/**
 * @brief List of test command values.
 */
enum test_command_t
{
	/** None. */
	TEST_ENUMS_NONE = 0,

	/** Start, commence performing the test. */
	TEST_ENUMS_START,

	/** Get status of test in progress. */
	TEST_ENUMS_GET_STATUS,

	/** Abort the test in progress. */
	TEST_ENUMS_ABORT
};


#endif	/* TEST_ENUMS_H */
