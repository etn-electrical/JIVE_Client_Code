@example  uC_Timers
@brief    The example code demonstrate the timer functionality
@par Board Specific Settings  
@details  Please do the following setting on EasyMXPro Board to run successfully the sample code. 
@n        1. STM32Fx07 Card is fitted on the board. 
@n        

@par Pin Configurations 
@details   Not applicable

@par Dependency 
@details Not applicable 

@par Sample Code Listing 

~~~~{.cpp}


#include "uC_Timers.h"
#include"uC_Timers_HW.h"

/*
 brief - Example to showcase the use of uC_Timers class.
 A callback function is set to be executed on the expiry of the timer.
*/
void Timer_Example_Code(void)
{
	/* Create instance for uC_Timers.
	 Here we have used uC_BASE_TIMER_CTRL_1 - HW Timer 1 will be used.
	 We can use uC_BASE_TIMER_CTRL_1 to uC_BASE_TIMER_CTRL_8. (See uC_Base_HW_IDs_STM32F407.h)
	*/
	uC_Timers* timer;
	timer = new uC_Timers(uC_BASE_TIMER_CTRL_1);

	/* Configure the interrupts and set its priority
	 First argument is the function address which we need to configure as a callback routine.
	 Second is the interrupt priority - we can have uC_INT_HW_PRIORITY_0 (Highest) to uC_INT_HW_PRIORITY_15 (Lowest).
	 (See uC_Interrupt.h for more)
	*/
	timer->Set_Callback(&Callback_Function, uC_INT_HW_PRIORITY_5);

	/* Set the timer frequency in Hz.
	 This function is used to set the timer frequency by changing the prescaler value.
	*/
	timer->Set_Timer_Frequency(10U);

	/* Set the timeout value in Hz with auto-reload flag.
	 The timeout value is set by varying the prescaler index and setting the ARPE(AUTORELOAD PRELOAD ENABLE) bit of the timer.
	*/
	timer->Set_Timeout(10U, true);

	/* Start the timer and the count */
	timer->Start_Timeout();
}

/*
 brief - Function that will be executed when the timer intterupt occurs.
*/
void Callback_Function()
{

}

/*
 Other uC_Timers class public methods:
*/

/* See header file for details. */
void Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
	uint8_t int_priority, bool raw_interrupt = false );

/* See header file for details. */
uC_TIMERS_TICK_TD 	Set_Timeout( uint32_t desired_frequency, bool repeat = true );

/* See header file for details. */
inline void Restart_Timeout( void );

/* See header file for details. */
void Stop_Timeout( void );

/* See header file for details. */
bool Timeout_Running( void ) const;

/* See header file for details. */
bool Time_Expired( void );

/* See header file for details. */
void Run( void );

/* See header file for details. */
void Stop( void );

/* See header file for details. */
uC_TIMERS_TICK_TD Set_Timer_Frequency( uint32_t desired_frequency );

/* See header file for details. */
void Set_Tick_Frequency( uint32_t desired_frequency );

/* See header file for details. */
uC_TIMERS_TICK_TD Get_Timer_Ticks( uint32_t desired_frequency ) const;

/* See header file for details. */
inline uint8_t Get_Timer_Byte_Size( void );

/* See header file for details. */
inline uC_TIMERS_TICK_TD Get_Timer_Value( void );

/* See header file for details. */
bool Time_Expired( uint16_t start_time, uint16_t duration );

/* See header file for details. */
inline void Enable_Int( void );

/* See header file for details. */
inline void Disable_Int( void );

/* See header file for details. */
inline void Ack_Int( void );

~~~~