@example  Capture Input Example 
@brief    The sample code demonstrates sample examples for input capture using timer

@par Board Specific Settings  
@n        1. Nucleo STM32F429ZI MCU

@par Pin Configurations 
@details Timer 1 channel 1 [pin-->PE9] is configured for Timer input Capture.

@par Dependency
@details Not applicable

@par Sample Code Listing

~~~~{.cpp}		   
#include "uC_Capture_Input.h"

/*timer Input capture Application*/
uC_Capture_Input* temp_capt_timer;
uC_PERIPH_IO_STRUCT const*  m_timers_pio[2] = {&TIMER1_PIO1_PE9, &TIMER1_PIO2_PA9};
uC_PERIPH_TIM_CAPTURE_STRUCT channel_config[2];
uC_PERIPH_TIM_CONFIG_STRUCT timer_config;

/* See header file for details. */
void Timer_Input_Capture_Mode( void );
/* See header file for details. */
void Tim_Chn( uC_Capture_Input::cback_param_t param );

/*
 * brief                    Example Code to start timer capture
 * note                     The below function does the following
 *                          1. Initialize the timer channel for input capture mode
 *                          2. set callback interupt function
 *                          3. Start timer capture
*/
void Timer_Input_Capture_Mode( void )
{
	timer_config.tim_period = 0U;
	timer_config.tim_clockDivision = 0U;/// TIM_CLOCKDIVISION_DIV1
	timer_config.tim_counterMode = 0U;	/// TIM_COUNTERMODE_UP
	timer_config.tim_prescaler = 0U;
	timer_config.tim_repetitionCounter = 0U;
	timer_config.tim_autoReloadPreload = TIM_CR1_ARPE;

	channel_config[0].tim_channel = 0U;
	channel_config[0].tim_icfilter = 0U;
	channel_config[0].tim_icpolarity = 0U;	/// TIM_ICPOLARITY_RISING
	channel_config[0].tim_icprescaler = 0U;	/// TIM_ICPSC_DIV1
	channel_config[0].tim_icselection = ( ( uint32_t )0x0001U );/// TIM_ICSELECTION_DIRECTTI
	channel_config[0].port_io = ( ( uC_PERIPH_IO_STRUCT* )m_timers_pio[0] );

	temp_capt_timer = new uC_Capture_Input( 0, 1, &channel_config[0], &timer_config, uC_INT_HW_PRIORITY_2 );

	temp_capt_timer->Set_Callback( 0, &Tim_Chn, NULL );
	temp_capt_timer->Start_Input_Capture();

}

/*
 * brief                    Timer Input Capture Callback function
 * note                     The below function does the following
 *                          1.Handles TIM capture interrupt request
 *                          2.Change the state of the LED(Pin-->PB7) once get the capture interrupt                     
*/
void Tim_Chn( uC_Capture_Input::cback_param_t param )
{
	static bool led_flag = false;

	if ( led_flag == true )
	{
		uC_IO_Config::Set_Out( &PIOB_PIN_7_STRUCT );
		led_flag = false;
	}
	else
	{
		uC_IO_Config::Clr_Out( &PIOB_PIN_7_STRUCT );
		led_flag = true;
	}

}
