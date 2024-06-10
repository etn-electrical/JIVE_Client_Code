\example  uC_Watchdog
\brief    The sample code on how to use the watchdog class

\par Board Settings  
 \details  Please do the following setting on EasyMXPro Board to run successfully the sample code.
 \n        1. STM32F207 Card is fitted on the board.

 \par Configurations 
      Not Applicable
      
\par Sample Code Listing


~~~~{.cpp}

#include "uC_Watchdog.h"
#include "uC_Watchdog_Hw.h"
#include "uC_reset.h"
#include "Ram.h"
#include "Ram_Static.h"
//* void Watchdog_Reset (void)
{
	typedef uint8_t BYTE;
	/* Variable to hold the status of previous reset */
	uC_RESET_CAUSE_TD u_PreviousResetStatus = 0x00;
	/* time counter */
	BYTE time_count = 0U;
	/* Create an instance of uC_watchdog class */
	uC_Watchdog* test_watchdog = NULL;
	/* Create an instance of uC_reset to know the reason of last reset */
	new uC_Reset();
	/* Get the cause of last reset */
	u_PreviousResetStatus = uC_Reset::Get_Reset_Trigger();
	/* Initialize uC_watchdog Class */
	test_watchdog = new uC_Watchdog();
	/* Initialize watchdog for 1000ms */
	uC_Watchdog::Init(1000U);
	/* In below loop reset will not occur */
	while (time_count < 10U)
	{
		test_watchdog->Feed_Dog();
		uC_Delay(9000U);
		uC_Watchdog::Handler();
		time_count++;
	}
	/* Watchdog reset will occur as Feed_Dog() and Handler are called more than 1 sec apart */
	test_watchdog->Feed_Dog();
	uC_Delay(10000000U);
	uC_Watchdog::Handler();
	time_count++;

}

~~~~
