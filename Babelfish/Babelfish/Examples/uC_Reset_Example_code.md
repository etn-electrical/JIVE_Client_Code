@example  uC_Reset
@brief    The sample code demonstrates sample examples for using uC_Reset class
@n        The features of uC_Reset class like soft reset, watchdog reset are exaplined

@par Board Specific Settings  

@details  Please do the following setting on EasyMXPro Board to run successfully the sample code
@n        1. STM32F207 Card is fitted on the board.

@par Pin Configurations 
@details   Not applicable 

@par Dependency
@details Not applicable 

@par Sample Code Listing
 
~~~~{.cpp}

#include "uC_Reset.h"
#include "uC_Watchdog.h"
#include "Ram.h"
#include "Ram_Static.h"

#define LTK_DOXYGEN_SOFTRESET
// Comment out this #define if you want to perform watchdog reset instead of soft reset
// Function example to demonstrate the use of uC_Reset Class

/*
 * brief                    Example Code for using uC_Reset class
 * note                     The below function does the following
 *                          1. Initialization of uC_Reset class
 *                          2. Performing watchdog reset or soft reset as configured by user
 */
void uC_ResetExample( void )
{
	// Declare one variable which can be used to know the reason of last reset
	uC_RESET_CAUSE_TD u_PreviousResetStatus = 0x00U;
	// Allocate the memory for uC_Reset
	new uC_Reset();
	// Get the root cause of previous Reset. What prompted the reset last time?
	u_PreviousResetStatus = uC_Reset::Get_Reset_Trigger();
#ifdef LTK_DOXYGEN_SOFTRESET
	// Perform soft reset
	uC_Reset::Soft_Reset();
#else
	// Declare here one timeout variable for watchdog reset. Assign the time in ms. here 8000ms = 8sec
	static const uint16_t WATCHDOG_TIMEOUT = 8000U;
	// configure the Timeout
	uC_Watchdog::Init( WATCHDOG_TIMEOUT );
	// perform watchdog Reset
	uC_Reset::Watchdog_Reset();
#endif
	// Control should not reach here
}

~~~~