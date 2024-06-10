/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Timers_Lib.h"
#include "Test_Soft_Timers.h"
#include "CR_Tasker.h"

/* Refresh time for the task in ms */
static const uint16_t OS_TASK_MON_REFRESH_TIME = 900U;

/* Timer object to be used */
BF_Lib::Timers* mon_timer;
BF_Lib::Timers* mon_timer_1;

/*
   brief - Callback function for the timer.
   It is a task responsible for giving the usage details about Stack and Heap usage, etc.
 */
void Checker_Task( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

void Checker_Task_2( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

/*
   brief - Example to showcase the use of Timers class.
   In this example, a CR task "Checker_task" has been made which is responsible for the diagnostics.
   This will get called as per the OS_TASK_MON_REFRESH_TIME set - which is the timeout period for the Timer.
 */

void Test_Soft_Timers( void )
{
	/* Timers class is initialized with 4 arguments to the constructor.
	   First argument is the callback function for the timer.
	   Second argument is the timer callback parameters, if any.
	   Third is the CR Tasker priority - Default is CR_TASKER_PRIORITY_1.
	   We can have CR_TASKER_PRIORITY_0 (IDLE) to CR_TASKER_PRIORITY_6 - total 6 priorities.
	   Priority 0 is the lowest and 6 is the highest.
	   Last argument is the name of the task to be executed.
	 */
	mon_timer = new BF_Lib::Timers( &Checker_Task, reinterpret_cast<void*>( nullptr ),
									BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY, "Task Usage Monitor" );

	mon_timer_1 = new BF_Lib::Timers( &Checker_Task_2, reinterpret_cast<void*>( nullptr ),
									  CR_TASKER_PRIORITY_4, "Task Usage Monitor Test" );

	/* We now start the timer.
	   We pass the Refresh time in ms (the value of time out for the timer) along with the auto-refresh flag set.
	 */
	mon_timer->Start( OS_TASK_MON_REFRESH_TIME, true );
	mon_timer_1->Start( 500, true );
}

/*
   brief - Callback function for the timer.
 */
void Checker_Task( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	printf( "Inside Soft Timer Checker Task 1 \n" );
}

void Checker_Task_2( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	printf( "Inside Soft Timer Checker Task 2 **** \n" );
}
