/**Example code for testing STM32WB55 Stop mode functionality **/

// Enable Below code for testing stop mode
// Please note for testing stop mode we need to DISABLE WATCHDOG as 
// Watchdog operation is not yet supported in stop mode
// This example code triggers LPTimer for 10 sec timeout for interrupt to get out of 
// Stop mode

#include "uC_PWR.h"
#include "LED.h"
#include "CR_Tasker.h"
#include "Output_Pos.h"
#include "uC_Single_LPTimer.h"
#include "Stop_Mode_Example.h"

uC_PWR* pPowerDown = nullptr;
uC_Single_LPTimer* pLPTimer = nullptr;
static BF_Misc::LED* led = nullptr;
static BF_Misc::LED* gled = nullptr;

/*
 ******************************************************************************************
 * CR Tasker for testing Stop mode
 *****************************************************************************************
 */
void stop_task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
   
	CR_Tasker_Begin( cr_task );

	while ( 1 )
	{
		gled->Off();
		
        // Start LPTimer
        pLPTimer->Start_Timeout( FALSE );
        // Enter in Stop mode
        pPowerDown->Enter_PowerDown_StopMode( PWR_STOPENTRY_WFI );

		gled->On();

		CR_Tasker_Delay( cr_task, 10 );
	}

	CR_Tasker_End();
}

/*
   brief - A callback function when an interrupt occurs.Led blinking is executed within this
      callback
   using the standard library function toggle.
 */
void callback_function_LPTIM( uC_Single_LPTimer::cback_param_t param )
{
	led->Toggle();
}
/*
 *****************************************************************************************
 * This function is used to configure stop mode and creating a CR Tasker for entering in power
 * down mode
 * Call this function in main for testing stop mode
 *****************************************************************************************
 */
void Stop_Mode_Init( void )
{
	led = new BF_Misc::LED( new BF_Lib::Output_Pos( &PIOB_PIN_5_STRUCT ) );
	gled = new BF_Misc::LED( new BF_Lib::Output_Pos( &PIOB_PIN_0_STRUCT ) );
	gled->Off();

	// Create instance of Power class
	pPowerDown = new uC_PWR();
	// Configure Stop mode level
	pPowerDown->Configure_StopMode_Level( PWR_LP_MODE_STOP2 );

	// Create instance of LPTimer class
	pLPTimer = new uC_Single_LPTimer( LED_EXAMPLE_LPTIMER );
	pLPTimer->Set_Callback( &callback_function_LPTIM, NULL, uC_INT_HW_PRIORITY_7 );

	// This will set LPTimer timeout to 10 sec
	pLPTimer->Set_Wait_Period_Sec( 10U );

	// Create a new CR Tasker
	new CR_Tasker( stop_task, NULL, CR_TASKER_PRIORITY_2, "stop_task" );
}