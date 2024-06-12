\example  uC_PWM
\brief    The sample code to initialize and control PWM channels on STM32F207.
 \par Board Specific Settings  

 \details On EasyMXPRoV7 board, place Oscilloscope probe on port pin PA0 to read the pwm output 
 
 \par Pin Configurations 
 \details Timer 5 channel 1 is configured for PWM output. This signal is mapped on port pin   
  PA0 on STM32F207 microcontroller.

 \par Dependency
 \details Please make sure that port pin PA0 is not being used for another purpose.
 \n Following DCID needs to be created in Tools\PatB DCI DB Creator.xls for storing PWM configuration.
 \n 1. DCI_EXAMPLE_PWM_FREQUENCY_DCID
 \n 2. DCI_EXAMPLE_PWM_DUTYCYCLE_DCID

 \par Sample Code Listing
 
~~~~{.cpp}
#include "uC_PWM.h"

void Example_Pwm( void )
{
	uC_PWM* example_pwm;
	bool pwm_status = false;
	DCI_Owner* pwm_dutycycle;
	DCI_Owner* pwm_freq;
	uint16_t dutycycle = 1U;
	uint16_t frequency = 0U;

	pwm_freq = new DCI_Owner( DCI_EXAMPLE_PWM_FREQUENCY_DCID );
	pwm_dutycycle = new DCI_Owner( DCI_EXAMPLE_PWM_DUTYCYCLE_DCID );

	pwm_freq->Check_Out( &frequency );

	pwm_dutycycle->Check_Out( &dutycycle );

	if ( dutycycle != 0U )
	{
		dutycycle = dutycycle * ( 65535U / 100U );
	}

	// create uC_PWM instance, select timer and PWM channel
	example_pwm = new uC_PWM( uC_BASE_PWM_CTRL_5, uC_BASE_PWM_CHANNEL_1, &TIMER5_PIO1_PA0 );

	// configure active High, left aligned and frequency received from NV memory
	example_pwm->Configure( uC_PWM_DUTY_CYCLE_HIGH, uC_PWM_LEFT_ALIGNED, frequency );

	// Set PWM duty cycle received from NV memory
	example_pwm->Set_Duty_Cycle_Pct( dutycycle );

	// turn Off PWM Mode
	example_pwm->Turn_Off();

	// read PWM status
	pwm_status = example_pwm->Is_On();

	// check if PWM is turned off
	if ( pwm_status == false )
	{
		// turn On PWM
		example_pwm->Turn_On();
	}
}

~~~~