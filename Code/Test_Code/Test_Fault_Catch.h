/**
 *****************************************************************************************
 * @file		Test_Fault_Catch.h
 *
 * @brief		Contains the test application function for Fault Catch.
 *
 *****************************************************************************************
 */

#ifndef Test_Fault_Catch_H_
	#define Test_Fault_Catch_H_

///< Enable only one fault type for fault catch test.
#define HARDFAULT_FAULTCATCH_TEST
// #define WATCHDOG_FAULTCATCH_TEST
// #define EXCEPTION_FAULTCATCH_TEST

#if ( defined( HARDFAULT_FAULTCATCH_TEST ) && defined( WATCHDOG_FAULTCATCH_TEST ) ) || \
	( defined( HARDFAULT_FAULTCATCH_TEST ) && defined( EXCEPTION_FAULTCATCH_TEST ) ) || \
	( defined( WATCHDOG_FAULTCATCH_TEST ) && defined( EXCEPTION_FAULTCATCH_TEST ) )

	#error "Enable only one fault type for fault catch test."

#endif

/**
 * @brief  : Fault Catch test Example function
 * @details: This function will generate the Fault catch forcefully.
 * @return void.
 */
void Test_Fault_Catch_App_Main( void );

#endif	/* Test_Fault_Catch_H_ */
