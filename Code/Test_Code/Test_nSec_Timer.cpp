/**
 *****************************************************************************************
 * @file		Test_nSec_Timer.h
 *
 * @brief		Contains the test application function prototype for nano second timer API's.
 *				This tests time elapsed in executing simple nop instruction.
 *
 * @copyright  	2021 Eaton Corporation. All Rights Reserved.
 *****************************************************************************************
 */

#include "Test_nSec_Timer.h"
#include "Test_Example.h"
#include "uC_Time_nSec.h"

#ifdef TEST_NANO_SEC_TIMER
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Test_nSec_Timer_Main( void )
{
	uint32_t start_time = 0U;
	uint32_t end_time = 0U;

	portDISABLE_INTERRUPTS();

	start_time = uC_TIME_NSEC_Sample();
	__asm__ __volatile__ ( "nop" );
	end_time = uC_TIME_NSEC_Elapsed( start_time );

	portENABLE_INTERRUPTS();

	printf( "\n Time taken %d nsecs \n\r", end_time );
}

#endif
