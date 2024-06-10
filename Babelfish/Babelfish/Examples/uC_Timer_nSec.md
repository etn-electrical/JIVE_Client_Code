@example  uC_Timer_nSec
@brief    The sample code to use the uC_Timer_nSec class on STM32F407.

@par Board Specific Settings  

@details  Please do the following setting on EasyMXPro Board to run the sample code successfully.
@n        1. STM32F407 Card is fitted on the board.


@par Pin Configurations 
@details Not applicable

@par Dependency
@details Not applicable

@par Sample Code Listing

~~~~{.cpp}


#include "uC_Timer_nSec.h"
#include "uC_Timers_HW.h"
#include "uC_Multi_Timers.h"

/*
 brief - Example to showcase the use of timer class.
 In this example, we can get the nanosecond timestamp from the DWT counter using uC_TIME_NSEC_Sample() API.
 Also we can calculate the execution time for the 'User code' by using passing uC_TIME_NSEC_Elapsed(nSec_time) API
 with passing the previously taken timestamp.
*/
void Nanosecond_Timer_Example(void)
{
	/* initialize the Nanoseconds timer. */
	uC_TIME_NSEC_Init();

	/* Returns the current timestamp since the device is up in nanoseconds. */
	static uint32_t n_sec_val = uC_TIME_NSEC_Sample();

	/*
	 User code.
	*/

	/* Returns the delta time from the passed nanoseconds value to the current nanoseconds time. */
	static uint32_t n_sec_elapsed = uC_TIME_NSEC_Elapsed(n_sec_val);
}

~~~~