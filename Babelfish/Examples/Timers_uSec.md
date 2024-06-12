@example  Timers_uSec
@brief    The sample code to use the Timers_uSec class on STM32F407.

@par Board Specific Settings  

@details  Please do the following setting on EasyMXPro Board to run the sample code successfully.
@n        1. STM32F407 Card is fitted on the board.


@par Pin Configurations 
@details Not applicable

@par Dependency
@details Not applicable

@par Sample Code Listing

~~~~{.cpp}

#include "uC_Timers.h"
#include "uC_Timers_HW.h"
#include "Timers_uSec.h"

/* This event is a flag that can be set outside the scope( volatile ) - by hardware so that we an detect. */
volatile bool event = false;

/* Timeout value for the  */
static const uint16_t timeoutMilliseconds = 12000; // 12ms.

/* Function Decalrations */
bool Get_Event_Flag_Status();

/*
 brief - Example to showcase the use of Timers_uSec class.
 In this example, the microsecond timer is used to get the timestamps and check whether
 the delta time calculated everytime is less than the actual 'timeoutMilliseconds' along with the 
 event flag. So if the event flag is set OR the timeoutMilliseconds has been elapsed, we get out of 
 the checker while loop. and will send the status back to the caller ( TRUE / FALSE ).
*/
bool Event_Timeout_Example()
{
	uint32_t startTime;
	uint32_t elapsedTime;
	
	bool status = false;

	/* 
	 Initialize the Timer_uSec class with Timer HW 1 to be used.
	 We can use uC_BASE_TIMER_CTRL_1 to uC_BASE_TIMER_CTRL_8. (See uC_Base_HW_IDs_STM32F407.h)
	*/
	BF_Lib::Timers_uSec::Init( new uC_Timers_HW( uC_BASE_TIMER_CTRL_1 ) );

	event = Get_Event_Flag_Status();

	/* 
	 Get the timestamp from the timer.
	 Returns the value of the tick count. (TIME_TD = uint32_t)
	*/
	uint32_t getTime = BF_Lib::Timers_uSec::Get_Time();

	if ( timeoutMilliseconds && event )
	{
		startTime = getTime;

		do
		{
			/* Get the current timestamp */
			getTime = BF_Lib::Timers_uSec::Get_Time();
			/* Calculate the time elapsed */
			elapsedTime = ( getTime - startTime );
		} while ( ( event == false ) && !( BF_Lib::Timers_uSec::Expired(timeoutMilliseconds) ) );

		/* Set the event flag to false explicitely. */
		event = false;

		/* Check whether we have the timeout condition */
		if ( BF_Lib::Timers_uSec::Expired(timeoutMilliseconds) )
		{
			status = true;
		}
		else
		{
			status = false;
		}
	}

	return ( status );
}

/*
 brief - Function that will return the status of event flag - TRUE/FALSE.
*/
bool Get_Event_Flag_Status()
{
	bool status = false;

	/* 
	Check the register flag if set or not 
	*/

	return ( status );
}

/*
 Other Timers_uSec class public methods:
*/

/* See header file for details. */
TIME_TD Get_Time( void );

/* See header file for details. */
bool Expired( TIME_TD start_time, TIME_TD duration );

/* See header file for details. */
TIME_TD Time_Passed( TIME_TD start_time );

~~~~