@example  uC_Single_Timers
@brief    The sample code to use the uC_Single_Timer class on STM32F407.

@par Board Specific Settings  

@details  Please do the following setting on EasyMXPro Board to run the sample code successfully.
@n        1. STM32F407 Card is fitted on the board.
          2. Port E/H switch for LEDs should be ON.


@par Pin Configurations 
@details Not applicable

@par Dependency
@n        1. LED module.

@par Sample Code Listing

~~~~{.cpp}

#include "LED.h"
#include "uC_Timers.h"
#include "uC_Timers_HW.h"
#include "uC_Single_Timer.h"

/* Things need to be initialized for the Example */
BF_Misc::LED *led;
uC_Single_Timer* s_timer;

/*
 brief - A callback function when an interrupt occurs. Led blinking is executed within this callback
 using the standard library function toggle.
*/
void Single_Timer_Cback( uC_Single_Timer::cback_param_t param )
{
	led->Toggle();
}

/*
 brief - Example to showcase the use of uC_Single_Timer class.
 In this example, we use PatB board LED to be toggled when timer interrupt occurs.
 'Single_Timer_Cback()' is the function that will be called when the interrupt occurs.
*/
void Single_Timer_Example( void )
{
	/* Create instance for uC_Single_Timer.
	 Here we have used uC_BASE_TIMER_CTRL_4 - HW Timer 4 will be used.
	 We can use uC_BASE_TIMER_CTRL_1 to uC_BASE_TIMER_CTRL_8. (See uC_Base_HW_IDs_STM32F407.h)
	*/
	s_timer = new uC_Single_Timer( uC_BASE_TIMER_CTRL_4 );

	/* Once the object of of uC_Single_Timer is created, we set the callback for this timer.
	 First argument is the function address which we need to configure as a callback routine.
	 Second is the function parameter is needed to pass.
	 Third is the interrupt priority - we can have uC_INT_HW_PRIORITY_0 (Highest) to uC_INT_HW_PRIORITY_15 (Lowest)
	 priorities for the HW interrupt. (See uC_Interrupt.h for more)
	*/
	s_timer->Set_Callback( &Single_Timer_Cback, NULL, uC_INT_HW_PRIORITY_7 );

	/* We now set the timeout value for the timer expiry.
	 We have set it to 1U - the desired frequency is in Hz, 1Hz in this case and the timer interval will be 1s.
	 When set to 2U, the timer interval will be 500ms.
	 Simililarly when set to 4U, the timer interval will be 250ms.
	 These are the calculations when the clock frequency is 50MHz.
	 Second argument is the REPEAT flag - If set true, the timer will be reloaded automatically.
	*/
	s_timer->Set_Timeout( 1U, TRUE );

	/* Initialize GPIO Port PIN-E8 for LED interfacing
	 Here PINE8 output is interfaced to anode of LED. Hence we are using Positive output control(Output_Pos)
	 In positive output control when LED is turned ON, the logic at port pin will be 1.
	 the logical volatge at PINE8 will be 0 when LED is turned OFF
	 in Negative Output Control(Output_Neg), the situation is reversed.
	*/
	led = new BF_Misc::LED( new BF_Lib::Output_Pos( &PIOE_PIN_8_STRUCT ) );

	/* Start the timer and the count */
	s_timer->Start_Timeout();
}


/*
 Other uC_Single_Timer class public methods:
*/

/* To stop the timer. */
void Stop_Timeout( void );

/* To restart the timer. */
void Restart_Timeout( void );

/* To know if the timer is expired.
	*  @return The status if the timer expired or not(false).
*/
bool Time_Expired( void );

/* To check if the timeout is yet to occur.
	*  @return The status if the timeout is yet to occur (true).
*/
bool Timeout_Running( void ) const;

~~~~