/**
 **********************************************************************************************
 * @file            Prod_Spec_LP_Operating_Mode.cpp
 *
 * @brief           Low Power behaviour
 * @details         This file shall include the functions to define the desired behaviour
 *					of the device when and how it enters and exits Low Power Mode
 *
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "LP_Manager.h"
#include "Prod_Spec_LP_Operating_Mode.h"
#include "CR_Tasker.h"
#include "FUS.h"

#if 1	// STOP_DEBUG
// #include "uC_PWR.h"
#include "SetSysClock.h"
#include "stm32wbxx_hal_rcc.h"
#include "stm32_wpan_app_ble.h"
#include "uC_OS_Tick.h"
#include "uC_Single_LPTimer.h"
#include "uC_Watchdog_HW.h"
#include "Prod_Spec_App.h"
// #include "Output_Pos.h"
// #include "Output_Neg.h"
// bool flag = 0U;
// static uC_USER_IO_STRUCT const* SWITCH_IO_STRUCT_3 = &PIOD_PIN_0_STRUCT;	// Temp Button
// uC_PWR* powerdown = nullptr;
// void go_to_sleep( void );

uC_Single_LPTimer* pLPTimer = nullptr;	// for wake up
#endif

static void Device_Motion_Monitor( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param );

//extern bool enter_sleep_mode;
bool enter_sleep_mode = false;

/**should be used only in this file**/
static uint32_t MODE_reg_port_A;
static uint32_t MODE_reg_port_B;
static uint32_t PUPD_reg_port_A;
static uint32_t PUPD_reg_port_B;

//Default scan time is 100 msec. We change to 5 sec just to allow device to reset after DELETE fw session
static const uint16_t DEFAULT_MOTION_MONITOR_SCAN_TIME = 100U;
static const uint16_t MAX_MOTION_MONITOR_SCAN_TIME = 5000U;

/*
brief - A callback where user can add and modify the behaviour before executing desired power mode
*/
static bool LP_Config_Cb( BF_Misc::LP_Manager::cback_param_t param,
						  BF_Misc::LP_Manager::lp_cmd_t lp_type_req )
{
	/** create the behaviour you want in LP mode **/

	switch ( lp_type_req )
	{
		case BF_Misc::LP_Manager::STOP_2:
		{
			uC_OS_Tick::Suspend_OS_Tick();

			// Disable LP Timer 
			// and other LP peripherals, if any
//			if ( temp_lptimer )
//			{
//				temp_lptimer->Stop_Timeout();
//			}
//			if ( second_lptimer )
//			{
//				second_lptimer->Stop_Timeout();
//			}

			// Turn of all LEDs before entering in stop mode
			//Turn_Off_All_LEDs();

			// Disable BLE scanning and advertising
			Adv_Stop();
			uC_Delay( 1000 );
			Stop_Discovery();
			uC_Delay( 1000 );

			//PROD_SPEC_LP_Disable_Peripherals();

		}
		break;

		case BF_Misc::LP_Manager::FULL_RUN:
		{
			uC_Watchdog_HW::Set_Timeout( WATCHDOG_TIMEOUT_10SEC );

			/** Trial and tested code for after wake up beahviour**/
			RCC->CRRCR = 0x00008780;
			RCC->C2AHB3ENR = 0x02180000;
			RCC->SMPSCR = 0x00000301;

			/*Reconfigure clock as it was*/
			SetSysClock();

			//PROD_SPEC_LP_Enable_Peripherals();

			uC_Watchdog_HW::Set_Timeout( WATCHDOG_TIMEOUT );

			// Enable syctick
			// Note: Do not remove this call if Suspend_OS_Tick has been called before this.
			// This will be required for FreeRTOS to work after coming out of stop mode.
			uC_OS_Tick::Resume_OS_Tick();

			// Enable LPTimers
			//temp_lptimer->Start_Timeout( TRUE );// this will start the LP TImer counter
			Adv_Request( );
			uC_Delay( 1000 );
			Discovery_Request();
			uC_Delay( 1000 );
		}
		break;

		case BF_Misc::LP_Manager::STOP_0:
		case BF_Misc::LP_Manager::STOP_1:
		case BF_Misc::LP_Manager::STAND_BY:
		case BF_Misc::LP_Manager::LP_RUN:
		case BF_Misc::LP_Manager::SLEEP:
		default:
			break;
	}


	return ( true );// use it to go/no-go for low power mode in LP_Manger

}

/*
brief - Init function which can used as example from Prod-SPec files.
*/
bool PROD_SPEC_LP_Operating_Mode_Init( void )
{
	new BF_Misc::LP_Manager( &LP_Config_Cb,
							 static_cast<BF_Misc::LP_Manager::cback_param_t>( nullptr )
							 );

	new CR_Tasker( &Device_Motion_Monitor, NULL, CR_TASKER_IDLE_PRIORITY,
				   ( char_t const* )( "Device Motion Monitor" )
				   );

	return ( true );
}


/*
brief - A callback function when an interrupt occurs.Led blinking is executed within this
callback
using the standard library function toggle.
*/
void callback_function_LPTIM( uC_Single_LPTimer::cback_param_t param )
{
  //led->Toggle();
	static int dummy = 0;
	if(dummy == 0)
	{
		dummy = 1;
	}
	else
	{
		dummy = 0;
	}
	
}
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_Enter_LP_Mode( BF_Misc::LP_Manager::lp_cmd_t lp_type_req )
{
	//Configure the wake-up source
	pLPTimer = new uC_Single_LPTimer( LED_EXAMPLE_LPTIMER );
    pLPTimer->Set_Callback( &callback_function_LPTIM, NULL, uC_INT_HW_PRIORITY_7 );
	pLPTimer->Set_Wait_Period_Sec( 5U );
	pLPTimer->Start_Timeout( FALSE );
	
	// Enter into low power mode
	BF_Misc::LP_Manager::Enter_LP_Mode( lp_type_req );
}

/**
 **********************************************************************************************
 * @brief                     Function to monitor device motion status
 *
 * @param[in] void            none
 * @return[out] none          none
 * @n
 **********************************************************************************************
 */
static void Device_Motion_Monitor( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param )
{
	BF_FUS::FUS* fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	uint16_t routine_delay = 100U;	//in msec
	static bool prev_fus_triggered = false;
	
	// example code for stop mode behaviour
	// 10 sec wait after boot up to go to stop
	static int count_to_sleep = 100;

	CR_Tasker_Begin( cr_task );

	while ( true )
	{
		routine_delay = DEFAULT_MOTION_MONITOR_SCAN_TIME;
		count_to_sleep--;
		if(count_to_sleep == 0)
		{
			enter_sleep_mode = true;
		}
		

		if( false == fus_handle->Is_Session_Alive() )
		{
			if( prev_fus_triggered == true )
			{
				prev_fus_triggered = false;

				//Keep enough separation between exit of fw upgrade and enter of low power mode
				routine_delay = MAX_MOTION_MONITOR_SCAN_TIME;
			}
			else if ( enter_sleep_mode == true )
			{
				enter_sleep_mode = false;
				PROD_SPEC_Enter_LP_Mode( BF_Misc::LP_Manager::STOP_2 );
				count_to_sleep = 100;	// reset count for triggering stop mode again after wake-up
			}
			else
			{
				//Misra
			}
		}
		else
		{
			prev_fus_triggered = true;
			routine_delay = MAX_MOTION_MONITOR_SCAN_TIME;
		}


		CR_Tasker_Delay( cr_task,  routine_delay );	// Wait time after no motion interrupt is serviced
	}
	CR_Tasker_End();
}

/**
 **********************************************************************************************
 * @brief					Disable periperals in STOP mode to reduce current consumption
 *
 * @param[in] void          none
 * @return[out] none        none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_LP_Disable_Peripherals()
{
	RCC->CR &= ~( ( uint32_t )RCC_CR_HSIKERON );

	// taking back-up of registers to be restored at after wake-up
	MODE_reg_port_A = GPIOA->MODER;
	MODE_reg_port_B = GPIOB->MODER;
	PUPD_reg_port_A = GPIOA->PUPDR;
	PUPD_reg_port_B = GPIOB->PUPDR;


	/**
	   configuring GPIOs to Analog Mode and No pull-up, pull-down state
	   for saving currnet consumption in STOP Mode
	 **/

	// keeping push buttons PA10 and PA15 untouched
	GPIOA->MODER |= 0x3FCFFFFF;

	// need PB4 and PB5 for interrupt form accelerometer
	// PB8 and PB9 are I2C lines for sensor, so not disabling it
	GPIOB->MODER |= 0xFFF0F0FF;

	// keeping buttons PA10 and PA15 untouched
	GPIOA->PUPDR &= 0xC0300000;

	// need PB4 PB5 for interrupt form accelerometer
	// PB8 PB9 are I2C lines for sensor, so not disabling it
	GPIOB->PUPDR &= 0x000F0F00;

}

/**
 **********************************************************************************************
 * @brief					Enable the GPIOs of LEDs back after wake-up
                            Should be called only if PROD_SPEC_LP_Enable_Peripherals() is executed
 *
 * @param[in] void          none
 * @return[out] none        none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_LP_Enable_Peripherals()
{
	// Re-initialise the GPIOs after wakeup
	// Initialise_GPIO();
	// Restoring the GPIO register as they were
	GPIOA->MODER = MODE_reg_port_A;
	GPIOB->MODER = MODE_reg_port_B;
	GPIOA->PUPDR = PUPD_reg_port_A;
	GPIOB->PUPDR = PUPD_reg_port_B;
}