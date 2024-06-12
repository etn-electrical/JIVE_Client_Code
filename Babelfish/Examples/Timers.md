@example  Timers
@brief    The sample code to use the Timers class on STM32F407.

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

/* Refresh time for the task in ms */
static const uint16_t OS_TASK_MON_REFRESH_TIME = 900U;

/* Timer object to be used */
BF_Lib::Timers*	mon_timer;

/*
 brief - Callback function for the timer.
 It is a task responsible for giving the usage details about Stack and Heap usage, etc.
*/
void Checker_Task( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

/*
 brief - Example to showcase the use of Timers class.
 In this example, a CR task "Checker_task" has been made which is responsible for the diagnostics.
 This will get called as per the OS_TASK_MON_REFRESH_TIME set - which is the timeout period for the Timer.
*/
void Monitor_Init( void )
{
	/* Timers class is initialized with 4 arguments to the constructor.
	 First argument is the callback function for the timer.
	 Second argument is the timer callback parameters, if any.
	 Third is the CR Tasker priority - Default is CR_TASKER_PRIORITY_1.
	 We can have CR_TASKER_PRIORITY_0 (IDLE) to CR_TASKER_PRIORITY_6 - total 6 priorities.
	 Priority 0 is the lowest and 6 is the highest.
	 Last argument is the name of the task to be executed.
	*/
	mon_timer = new BF_Lib::Timers( &Checker_Task, reinterpret_cast<void *>(nullptr),
					BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY, "Task Usage Monitor" );

	/* We now start the timer.
	 We pass the Refresh time in ms (the value of time out for the timer) along with the auto-refresh flag set.
	*/
	mon_timer->Start( OS_TASK_MON_REFRESH_TIME, true );
}

/*
 brief - Callback function for the timer.
*/
void Checker_Task( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	/*
	 Code for printing the diagnostics of the OS_Task - Stack and Heap usage, etc.
	*/
}

~~~~