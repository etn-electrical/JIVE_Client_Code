@example  uC_Multi_Timers
@brief    The sample code to use the uC_Multi_Timers class on STM32F407.

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
#include "uC_Multi_Timers.h"

/*
 brief - Number of software timers to be used.
 */
static const uint8_t TOTAL_TIMERS = 2;

/* Things need to be initialized for the Example */
BF_Misc::LED *led1 = nullptr;
BF_Misc::LED *led2 = nullptr;;
uC_Multi_Timers* m_timer = nullptr;;

/* Function declarations */
void Multi_Timer_Cback1( uC_Multi_Timers::cback_param_t param );
void Multi_Timer_Cback2( uC_Multi_Timers::cback_param_t param )

/*
 brief - Example to showcase the use of uC_Multi_Timers class.
 In this example, two LEDs are used for two separate callbacks for two timer interrupts.
 Both will be toggeled in the two callbacks so that user can see the LED pattern on board.
*/
void Multi_Timers_Example( void )
{
	/* Create instance for uC_Multi_Timers.
	 Here we have used uC_BASE_TIMER_CTRL_4 - HW Timer 4 will be used.
	 We can use uC_BASE_TIMER_CTRL_1 to uC_BASE_TIMER_CTRL_8. (See uC_Base_HW_IDs_STM32F407.h)
	 Second argument is the number of software timers we want to use using a single HW timer.
	 Third is the interrupt priority - we can have uC_INT_HW_PRIORITY_0 (Highest) to uC_INT_HW_PRIORITY_15 (Lowest)
	 priorities for the HW interrupt. (See uC_Interrupt.h for more)
	*/
	m_timer = new uC_Multi_Timers( uC_BASE_TIMER_CTRL_4, TOTAL_TIMERS,
								   static_cast < uint8_t > ( uC_INT_HW_PRIORITY_7 ) );

	/* Once the object of of uC_Multi_Timers is created, we set the callback for this timer.
	 First argument is the timer number for which we need to set the callback.
	 Here we have 2 software timers so the indices will be 0 and 1 for timer1 and timer2.
	 Second argument is the function address which we need to configure as a callback routine.
	 Third is the function argument to be passed, if any.
	*/
	m_timer->Set_Callback( 0U, &Multi_Timer_Cback1, NULL );
	m_timer->Set_Callback( 1U, &Multi_Timer_Cback2, NULL );

	/* To set the timer frequency is Hz - sets the calculated prescalar value.
	 Desired_frequency: Frequeny in Hz is passed as an argument.
	 This is the lowest frequency you can use.
	*/
	m_timer->Set_Min_Frequency( 1U );

	/* We now set the timeout values for each timer.
	 First argument is the timer number to which we want to configure the frequency.
	 Second argument is the desired frequency is in Hz, 1Hz in this case and the timer interval will be 1s.
	 When set to 2U, the timer interval will be 500ms.
	 Simililarly when set to 4U, the timer interval will be 250ms.
	 These are the calculations when the clock frequency is 50MHz.
	 Third argument is the REPEAT flag - If set true, the timer will be reloaded automatically.
	*/
	m_timer->Set_Timeout( 0U, 1U, TRUE );

	/* 
	 Set the desired freq as 4 so that the timeout for second timer will be 4 times faster than the first timer.
	 So, the user can see the blinking behaviour difference on board.
	*/
	m_timer->Set_Timeout( 1U, 4U, TRUE );

	/* Initialize GPIO Port Pins for LED interfacing */
	led1 = new BF_Misc::LED( new BF_Lib::Output_Pos( &PIOE_PIN_12_STRUCT ) );
	led2 = new BF_Misc::LED( new BF_Lib::Output_Pos( &PIOE_PIN_13_STRUCT ) );

	/* Make the LED on just to see the difference on the board when both the timer callbacks will play with the LEDs. */
	led1->Toggle();

	/* Start the timer1 so that we will hit its callback and the other timer is handled from that callback. */
	m_timer->Start_Timeout(0U);
}


/*
 brief - A callback function when an interrupt occurs. Led blinking is executed within this callback
 using the standard library function toggle.
*/
void Multi_Timer_Cback1( uC_Multi_Timers::cback_param_t param )
{
	/* Toggle LED1 and LED2 */
	led1->Toggle();
	led2->Toggle();

	/* Start the timer2 here and stop the timer1 
	 The timer that needs to be Start or Stop is passed as an argument.
	*/
	m_timer->Start_Timeout(1U);
	m_timer->Stop_Timeout(0U);
}

/*
 brief - A callback function when an interrupt occurs. Led blinking is executed within this callback
 using the standard library function toggle.
*/
void Multi_Timer_Cback2( uC_Multi_Timers::cback_param_t param )
{
	/* Toggle LED1 and LED2 */
	led2->Toggle();
	led1->Toggle();

	/* Start the timer1 here and stop the timer2 */
	m_timer->Start_Timeout(0U);
	m_timer->Stop_Timeout(1U);
}


/*
 Other uC_Multi_Timers class public methods:
*/

/*
	Set_Timeout will use Get_Timer_Ticks (uC_Timers_HW class function).
	We pass desired freq in Hz and that will be the divisor to the Tickcount.
	Tickcount is calculated as using t_clock_freq and the prescalar value.
*/
void Set_Timeout( uint8_t timer_num, uint32_t desired_frequency, bool repeat = true );

/* To set the timer frequency is Hz - sets the calculated prescalar value.
	*  @param[in] desired_frequency: Frequeny in Hz.
*/
void Set_Min_Frequency( uint32_t desired_frequency );

/* To restart the timer.
	*  @param[in] timer_num: Timer number that needs to be restarted.
*/
void Restart_Timeout( uint8_t timer_num );

/* To check the timeout running status of the timer.
	*  @param[in] timer_num: Timer number that needs to be checked for the timeout running status.
	*  @return The status if the timer interrupt is enabled i.e. the timer is running within the timeout period.
*/
bool Timeout_Running( uint8_t timer_num ) const;

/* To check the timeout pending status of the timer.
	*  @param[in] timer_num: Timer number that needs to be checked for the timeout pending status.
	*  @return The status if the timer interrupt flag is set i.e. the timer callback execution has happened and the interrupt flag has been reset.
*/
bool Timeout_Pending( uint8_t timer_num ) const;

/* To reload the specified timeout value.
	*  @param[in] timer_num: Timer number that needs to be checked for the timeout pending status.
*/
void Reload_Timeout( uint8_t timer_num );

~~~~