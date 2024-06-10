/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Watchdog_HW.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define Test_Dbg_0_On()         Debug_0_On()
#define Test_Dbg_0_Off()        Debug_0_Off()
#define Test_Dbg_0_Toggle()     Debug_0_Toggle()

#define Test_Dbg_1_On()         Debug_1_On()
#define Test_Dbg_1_Off()        Debug_1_Off()
#define Test_Dbg_1_Toggle()     Debug_1_Toggle()

#define Test_Dbg_2_On()         Debug_2_On()
#define Test_Dbg_2_Off()        Debug_2_Off()
#define Test_Dbg_2_Toggle()     Debug_2_Toggle()

#define Test_Dbg_3_On()         Debug_3_On()
#define Test_Dbg_3_Off()        Debug_3_Off()
#define Test_Dbg_3_Toggle()     Debug_3_Toggle()

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
#include "Timers_uSec.h"
bool uC_TEST_Timers_uSec_Test_Init( void );

#include "uC_Timers.h"
void uC_TEST_Timers_Int_CBACK( void );

#include "uC_Timers_HW.h"
bool uC_TEST_Timers_HW_Test_Init( void );

void uC_TEST_Timers_HW_Int_CBACK( uC_Timers_HW::cback_param_t param );

#include "uC_Single_Timer.h"
bool uC_TEST_Single_Timer_Test_Init( void );

void uC_TEST_Single_Timer_Int_CBACK( uC_Single_Timer::cback_param_t param );

#include "uC_Multi_Timers.h"
#include "uC_Base.h"
bool uC_TEST_Multi_Timers_Test_Init( void );

void uC_Multi_Timers0_Test_Cback( uC_Multi_Timers::cback_param_t param );

void uC_Multi_Timers1_Test_Cback( uC_Multi_Timers::cback_param_t param );

void uC_Multi_Timers2_Test_Cback( uC_Multi_Timers::cback_param_t param );

void uC_Multi_Timers3_Test_Cback( uC_Multi_Timers::cback_param_t param );

#include "uC_PWM.h"
bool uC_TEST_PWM_Test_Init( void );

void uC_TEST_PWM_Timers_Int_CBACK( void );

void uC_TEST_uC_Delay( void );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_Timers_Init( void )
{
	volatile bool test_success = 0;

	// uC_TEST_uC_Delay();
	// uC_TEST_Timers_uSec_Test_Init();
	// uC_TEST_Single_Timer_Test_Init();
	uC_TEST_Multi_Timers_Test_Init();
	// test_success |= uC_TEST_Timers_HW_Test_Init();
	// test_success |= uC_TEST_Single_Timer_Test_Init();
	// test_success |= uC_TEST_Multi_Timers_Test_Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define uC_TEST_TIMERS_uC_DELAY_TIME_INIT       10
#define uC_TEST_TIMERS_uC_DELAY_TIME_FINAL      1000
#define uC_TEST_TIMERS_uC_DELAY_TIME_MULT       10
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_uC_Delay( void )
{
	Push_TGINT();

	for ( uint32_t delay = uC_TEST_TIMERS_uC_DELAY_TIME_INIT;
		  delay <= uC_TEST_TIMERS_uC_DELAY_TIME_FINAL;
		  delay *= uC_TEST_TIMERS_uC_DELAY_TIME_MULT )
	{
		uC_Watchdog_HW::Kick_Dog();
		uC_Delay( uC_TEST_TIMERS_uC_DELAY_TIME_INIT );
		Test_Dbg_0_On();
		uC_Delay( delay );
		Test_Dbg_0_Off();
	}

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define uC_TEST_TIMERS_uSEC_TIME_INIT       10
#define uC_TEST_TIMERS_uSEC_TIME_FINAL      10000
#define uC_TEST_TIMERS_uSEC_TIME_MULT       10
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_Timers_uSec_Test_Init( void )
{
	volatile Timers_uSec::TIME_TD time_sample;

	Push_TGINT();

	for ( uint8_t i = uC_BASE_TIMER_CTRL_1; i < uC_BASE_MAX_TIMER_CTRLS; i++ )
	{
		new Timers_uSec( new uC_Timers_HW( i ) );

		for ( Timers_uSec::TIME_TD time = uC_TEST_TIMERS_uSEC_TIME_INIT;
			  time <= uC_TEST_TIMERS_uSEC_TIME_FINAL; time *= uC_TEST_TIMERS_uSEC_TIME_MULT )
		{
			time_sample = Timers_uSec::Get_Time();
			while ( Timers_uSec::Expired( time_sample, uC_TEST_TIMERS_uSEC_TIME_INIT ) != true )
			{
				uC_Watchdog_HW::Kick_Dog();
			}
			Test_Dbg_1_On();

			time_sample = Timers_uSec::Get_Time();
			while ( Timers_uSec::Expired( time_sample, time ) != true )
			{
				uC_Watchdog_HW::Kick_Dog();
			}
			Test_Dbg_1_Off();
		}
	}

	Pop_TGINT();

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define uC_TEST_TIMERS_SINGLE_FREQ_TO_USEC_ADJUST       1000000
#define uC_TEST_TIMERS_SINGLE_uSEC_TOLERANCE_PERC       2
#define uC_TEST_TIMERS_SINGLE_FREQ_INIT     100
#define uC_TEST_TIMERS_SINGLE_FREQ_FINAL    10000
#define uC_TEST_TIMERS_SINGLE_FREQ_MULT     10
bool single_time_expired;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_Single_Timer_Test_Init( void )
{
	uC_Single_Timer* temp_timer;
	Timers_uSec::TIME_TD expected_usec;
	Timers_uSec::TIME_TD measured_usec;
	Timers_uSec::TIME_TD time_sample;
	uint32_t single_usec_tolerance;

	new Timers_uSec( new uC_Timers_HW( uC_BASE_TIMER_CTRL_1 ) );

	for ( uint8_t i = uC_BASE_TIMER_CTRL_2; i < uC_BASE_MAX_TIMER_CTRLS; i++ )
	{
		temp_timer = new uC_Single_Timer( i );
		temp_timer->Set_Callback( uC_TEST_Single_Timer_Int_CBACK, NULL, uC_INT_HW_PRIORITY_15 );

		for ( uint32_t freq = uC_TEST_TIMERS_SINGLE_FREQ_INIT;
			  freq <= uC_TEST_TIMERS_SINGLE_FREQ_FINAL;
			  freq *= uC_TEST_TIMERS_SINGLE_FREQ_MULT )
		{
			expected_usec = uC_TEST_TIMERS_SINGLE_FREQ_TO_USEC_ADJUST / freq;
			temp_timer->Set_Timeout( freq, true );
			single_time_expired = false;
			time_sample = Timers_uSec::Get_Time();
			temp_timer->Start_Timeout();
			while ( single_time_expired != true )
			{
				uC_Watchdog_HW::Kick_Dog();
			}
			measured_usec = Timers_uSec::Time_Passed( time_sample );
			temp_timer->Stop_Timeout();
			single_usec_tolerance = expected_usec * uC_TEST_TIMERS_SINGLE_uSEC_TOLERANCE_PERC;
			single_usec_tolerance = single_usec_tolerance / 100;
			BF_ASSERT( ( ( measured_usec + single_usec_tolerance ) > expected_usec ) &&
				 ( ( measured_usec - single_usec_tolerance ) < expected_usec ) );
			Test_Dbg_2_Toggle();
		}
	}

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_Single_Timer_Int_CBACK( uC_Single_Timer::cback_param_t param )
{
	single_time_expired = true;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define uC_TEST_TIMERS_MULTI_FREQ_TO_USEC_ADJUST        1000000
#define uC_TEST_TIMERS_MULTI_uSEC_TOLERANCE_PERC        5
#define uC_TEST_TIMERS_MULTI_FREQ_INIT      100
#define uC_TEST_TIMERS_MULTI_FREQ_FINAL     10000
#define uC_TEST_TIMERS_MULTI_FREQ_MULT      10
uint8_t mult_time_expired;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers0_Test_Cback( uC_Multi_Timers::cback_param_t param )
{
	mult_time_expired++;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers1_Test_Cback( uC_Multi_Timers::cback_param_t param )
{
	mult_time_expired++;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers2_Test_Cback( uC_Multi_Timers::cback_param_t param )
{
	mult_time_expired++;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Multi_Timers3_Test_Cback( uC_Multi_Timers::cback_param_t param )
{
	mult_time_expired++;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_Multi_Timers_Test_Init( void )
{
	uC_Multi_Timers* multi_tmr;
	volatile Timers_uSec::TIME_TD expected_usec;
	volatile Timers_uSec::TIME_TD measured_usec;
	Timers_uSec::TIME_TD time_sample;
	volatile uint32_t usec_tolerance;
	uint8_t num_timers;
	uC_BASE_TIMER_IO_STRUCT const* timer_def;

	// new Timers_uSec( new uC_Timers_HW( uC_BASE_TIMER_CTRL_1 ) );

	for ( uint8_t i = uC_BASE_TIMER_CTRL_1; i < uC_BASE_MAX_TIMER_CTRLS; i++ )
	{
		timer_def = ( uC_Base::Self() )->Get_Timer_Ctrl( i );
		num_timers = timer_def->num_compares;
		if ( num_timers > 0 )
		{
			multi_tmr = new uC_Multi_Timers( i, num_timers, uC_INT_HW_PRIORITY_15 );
			multi_tmr->Set_Min_Frequency( uC_TEST_TIMERS_MULTI_FREQ_INIT );
			switch ( num_timers )
			{
				case 4:
					multi_tmr->Set_Callback( 3, uC_Multi_Timers3_Test_Cback, NULL );

				case 3:
					multi_tmr->Set_Callback( 2, uC_Multi_Timers2_Test_Cback, NULL );

				case 2:
					multi_tmr->Set_Callback( 1, uC_Multi_Timers1_Test_Cback, NULL );

				case 1:
					multi_tmr->Set_Callback( 0, uC_Multi_Timers0_Test_Cback, NULL );
					break;

				default:
					BF_ASSERT(false);
					break;
			}

			for ( uint32_t freq = uC_TEST_TIMERS_MULTI_FREQ_INIT;
				  freq <= uC_TEST_TIMERS_MULTI_FREQ_FINAL;
				  freq *= uC_TEST_TIMERS_MULTI_FREQ_MULT )
			{
				mult_time_expired = 0;

				for ( uint8_t sub_timer = 0; sub_timer < num_timers; sub_timer++ )
				{
					// expected_usec = uC_TEST_TIMERS_MULTI_FREQ_TO_USEC_ADJUST / freq;
					multi_tmr->Set_Timeout( sub_timer, freq, false );
				}

				for ( uint8_t sub_timer = 0; sub_timer < num_timers; sub_timer++ )
				{
					multi_tmr->Start_Timeout( sub_timer );
				}
				// time_sample = Timers_uSec::Get_Time();
				while ( mult_time_expired < num_timers )
				{
					uC_Watchdog_HW::Kick_Dog();
				}
				// measured_usec = Timers_uSec::Time_Passed( time_sample );

				// usec_tolerance = expected_usec * uC_TEST_TIMERS_MULTI_uSEC_TOLERANCE_PERC;
				// usec_tolerance = usec_tolerance / 100;
				// if ( ( ( measured_usec + usec_tolerance ) > expected_usec ) &&
				// ( ( measured_usec - usec_tolerance ) < expected_usec ) )
				// {
				Test_Dbg_3_Toggle();
				// }
				// else
				// {
				// BF_ASSERT(false);
				// }
			}
		}
	}

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
volatile uint16_t percent_duty;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_PWM_Test_Init( void )
{
#if 0
	uC_PWM* test_pwm;

	// Setup a known timer with a 1second period.
	uC_IO_Config::Set_Pin_To_Output( &DEBUG_OUTPUT_1_IO_CTRL );

	temp_timer = new uC_Timers( uC_BASE_TIMER_CTRL_1 );

	temp_timer->Set_Callback( uC_TEST_PWM_Timers_Int_CBACK, uC_INT_HW_PRIORITY_15 );

	temp_timer->Set_Timeout( 10000, true );

	temp_timer->Start_Timeout();

	// Setup the PWM to run.
	test_pwm = new uC_PWM( uC_BASE_PWM_CTRL_2, uC_BASE_PWM_CHANNEL_3, &TIMER2_PIO3_PB10 );


	test_pwm->Configure( uC_PWM_DUTY_CYCLE_LOW, uC_PWM_LEFT_ALIGNED, 1 );
	test_pwm->Set_Duty_Cycle_Pct( 0x1FFF );

	test_pwm->Turn_On();
	percent_duty = 0;
	while ( percent_duty == 0 )
	{}

	test_pwm->Turn_Off();

	test_pwm->Turn_On();

	test_pwm->Configure( uC_PWM_DUTY_CYCLE_LOW, uC_PWM_CENTER_ALIGNED, 1 );
	test_pwm->Set_Duty_Cycle_Pct( 0x1FFF );
	test_pwm->Turn_On();
	percent_duty = 0;
	while ( percent_duty == 0 )
	{}
	test_pwm->Turn_Off();

	test_pwm->Configure( uC_PWM_DUTY_CYCLE_HIGH, uC_PWM_LEFT_ALIGNED, 1 );
	test_pwm->Set_Duty_Cycle_Pct( 0x1FFF );
	test_pwm->Turn_On();
	percent_duty = 0;
	while ( percent_duty == 0 )
	{}

	test_pwm->Configure( uC_PWM_DUTY_CYCLE_HIGH, uC_PWM_CENTER_ALIGNED, 1 );
	test_pwm->Set_Duty_Cycle_Pct( 0x1FFF );
	test_pwm->Turn_On();
	percent_duty = 0;
	while ( percent_duty == 0 )
	{}

	test_pwm->Configure( uC_PWM_DUTY_CYCLE_LOW, uC_PWM_LEFT_ALIGNED, 1, true );
	test_pwm->Set_Duty_Cycle_Pct( 0x1FFF );
	test_pwm->Turn_On();
	percent_duty = 0;
	while ( percent_duty == 0 )
	{}
	test_pwm->Turn_Off();

	test_pwm->Configure( uC_PWM_DUTY_CYCLE_HIGH, uC_PWM_LEFT_ALIGNED, 1, true );
	test_pwm->Set_Duty_Cycle_Pct( 0x1FFF );
	test_pwm->Turn_On();
	percent_duty = 0;
	while ( percent_duty == 0 )
	{}
	test_pwm->Turn_Off();


	test_pwm->Configure( uC_PWM_DUTY_CYCLE_HIGH, uC_PWM_LEFT_ALIGNED, 10000, true );
	percent_duty = 0x8000;
	test_pwm->Set_Duty_Cycle_Pct( percent_duty );
	test_pwm->Turn_On();
	while ( true )
	{
		if ( percent_duty != 0 )
		{
			// test_pwm->Set_Duty_Cycle_Pct( percent_duty );
		}
	}

	test_pwm->Configure( uC_PWM_DUTY_CYCLE_LOW, uC_PWM_CENTER_ALIGNED, 10000, true );
	percent_duty = 1;
	test_pwm->Set_Duty_Cycle_Pct( percent_duty );
	test_pwm->Turn_On();
	while ( true )
	{
		if ( percent_duty != 0 )
		{
			test_pwm->Set_Duty_Cycle_Pct( percent_duty );
		}
	}
#endif
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_PWM_Timers_Int_CBACK( void )
{
#if 0
	temp_timer->Ack_Int();

	if ( !uC_IO_Config::Get_Out( &DEBUG_OUTPUT_1_IO_CTRL ) )
	{
		uC_IO_Config::Set_Out( &DEBUG_OUTPUT_1_IO_CTRL );
	}
	else
	{
		uC_IO_Config::Clr_Out( &DEBUG_OUTPUT_1_IO_CTRL );
	}

	percent_duty += ( 0xFFFF * .1 );
#endif
}
