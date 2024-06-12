@example  LED_Class
@brief    The sample code demonstrates sample examples for using LED class
@n		  LED class comes up with functionalities like toggle LED, Turn ON/OFF LED etc
@n        We have added the example codes for 
@n        1. LED Toggle
@n	 	  2. LED ON/OFF
@n        3. Reading LED status
@n        4. Set LED status
@n        5. LED flashing
@n        6. LED test functionality
 
@par Board Specific Settings  

@details  Please do the following setting on EasyMXPro Board to run successfully the sample code
@n        1. STM32F207 Card is fitted on the board.
@n        2. Enable SW15.7. This connects GPIO PORTD pins of microcontroller to on board LEDs.

@par Pin Configurations 
@details   Not applicable 

@par Dependency
@details Certain functions of LED class namely Test_All() and Flash() uses CR timers  for timing purpose
@n       Hence it is necessary to configure CR tick. In babelfish architecture it is alreday taken care.
@n       However for understanding purpose an example code for CR tick initialization is added as a part 
@n       of this example code

@par Sample Code Listing
 
~~~~{.cpp}
#include "Output_Neg.h"
#include "Output_Pos.h"
#include "LED.h"
#include "CR_Tasker.h"
#include "uC_Tick.h"
#include "Ram.h"
#include "Ram_Static.h"
#include "Timers.h"

/*
 * brief                    Example Code to turn ON and OFF the LED
 * note                     The below function does the following
 *                          1. Initialization of LED class
 *                          2. Turning ON LED and Turning it OFF after 1 sec
 *                          3. LED Library functions On()and Off() are used for said purpose
*/
void LED_ON_OFF_Example( void )
{
	static BF_Misc::LED *led = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_0_STRUCT ) );
	// Initialize GPIO Port PIN-D0 for LED interfacing
	// Here PIND0 output is interfaced to anode of LED. Hence we are using Positive output control(Output_Pos)
	// In positive output control when LED is turned ON, the logic at port pin will be 1.
	// the logical volatge at PIND will be 0 when LEd is turned OFF
	// in Negative Output Control(Output_Neg), the situation is reversed.
	for ( ;; )
	{
		led->On();
		// Turn on the LED
		uC_Delay( 1000000U );
		// 1 sec Delay
		led->Off();
		// Turn OFF LED
		uC_Delay( 1000000U );
		// 1 sec Delay
	}
}

/*
 * brief                    Example Code to toggle LED
 * note                     The below function does the following
 *                          1. Initialization of LED class
 *                          2. Toggling LED after every 1 sec
 *                          3. LED Library function Toggle() is used for toggling LEDs
*/
void LED_TOGGLE_Example( void )
{
	static BF_Misc::LED *led = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_0_STRUCT ) );
	// Initialize GPIO Port PIN-D0 for LED interfacing
	for ( ;; )
	{
		led->Toggle();
		// Toggle the LED
		uC_Delay( 1000000U );
		// 1 sec Delay
	}
}

/*
 * brief                    Example Code to read the status of LED - LED is ON or OFF
 * note                     The below function implements following things
 *                          1. Initialization of LED class
 *                          2. LED Library function Is_On() is used for reading LED status
*/
void LED_Status_Example( void )
{
	bool_t led_status = false;
	// Variable which will hold the status of LED
	static BF_Misc::LED *led = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_0_STRUCT ) );
	// Initialize GPIO Port PIN-D0 for LED interfacing
	led->On();
	// Turn on the LED
	led_status = led->Is_On();
	// Read the status of LED if it is really ON
	led->Off();
	// Turn off the LED
	led_status = led->Is_On();
	// Read the status of LED if it is really OFF
}

/*
 * brief                    Example Code to set the state of LED. 
 * note                     The below function does the following
 *                          1. Initialization of LED class
 *                          2. Set LED to ON and OFF state
 *                          3. LED Library function Set_To() is used for said purpose
*/
void LED_Set_State_Example( void )
{
	static BF_Misc::LED *led = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_0_STRUCT ) );
	// Initialize GPIO Port PIN-D0 for LED interfacing
	for ( ;; )
	{
		led->Set_To( 0U );
		// Set LED to state 0(OFF)
		uC_Delay( 1000000U );
		// 1sec delay
		led->Set_To( 1U );
		// Set LED to state 1(ON)
		uC_Delay( 1000000U );
		// 1sec delay
	}
}

/*
 * brief                    Example Code to demonstrate LED flashing 
 * note                     The below function does the following
 *                          1. Initialization of LED class
 *                          2. LED Library function Flash() is used for LED flashing
*/
void LED_Flash_Example( void )
{
	static BF_Misc::LED *led = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_0_STRUCT ) );
	// Initialize GPIO Port PIN-D0 for LED interfacing
	led->Flash( 100U );
	// Flash the LED at 100ms interval
	// Note the flashing timer is based on CR timer. So configure CR/OS tick timer.
	// As an example we have written small function CR_Init()- about how to initialize CR timer.
	// However if you are planning to use this code inside babelfish, it is already taken care
	// and no need to call below function
	CR_Init();
	// CR tick initialization
}

/*
 * brief                    Example Code to test the LEDs
 * note                     The below function implements following things
 *                          1. LED Library function Test_All() is used for testing the LEDs
 *                          2. Initialize all the LEDs which you will be using in your appliation
 *                          3. TestAll functionality turns on all the configured LEDs to notify
 *                             to the user if any of them is damaged or not working
*/  
void LED_TestAll_Example( void )
{

	static BF_Misc::LED *led0 = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_0_STRUCT ) );
	static BF_Misc::LED *led1 = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_1_STRUCT ) );
	static BF_Misc::LED *led2 = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_2_STRUCT ) );
	static BF_Misc::LED *led3 = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_3_STRUCT ) );
	static BF_Misc::LED *led4 = new BF_Misc::LED( new Output_Pos( &PIOD_PIN_4_STRUCT ) );
	// Initialize GPIO Port PIN-D0 throgh Port PIN-D4 for LED interfacing
	BF_Misc::LED::Test_All( 1000U );
	// Tests all the LEDs and turn them on for 1 sec. It uses CR timer for counting
	// Hence similar to earlier example configure correctly the CR timer
	CR_Init();
	// CR tick initialization

}

/*
 * brief                    function for configuring CR/OS tick
 * note                     1. LED time counting is based on CR timere tick.
 *                          2. In babelfish it is already configured. However it is still 
 *                             put here for better understanding
*/  
void CR_Init(void)
{
	new uC_Tick( CR_Tasker::Tick_Event, CR_TASKER_TICK_FREQUENCY, uC_INT_HW_PRIORITY_7 );
	// Configure the Tick of the CR_Tasker. Attach tick callbackfunction of CR_Tasker
	// Give frequency of tick in Hz. In above case CR_TASKER_TICK_FREQUENCY = 1000Hz
	// So Scheduler tick will occur at every 1/1000 = 1ms interval
	uC_Tick::Start_Tick();
	// Start Tick generating timer
	CR_Tasker::Scheduler( NULL );
	// Starts Scheduler
}
~~~~