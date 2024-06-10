/*
 *****************************************************************************************
 *
 *		Copyright 2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *
 *****************************************************************************************
 */
#include "includes.h"
#include "Output_PWM_CtrlV.h"

/*
 *****************************************************************************************
 *		Constants and Macros
 *****************************************************************************************
 */
#define PWM_100_DUTY_CYCLE              ( ( UINT16 )0xFFFF )
#define PWM_FREQUENCY                   20000			// in Hz
#define PWM_DUTY_CYCLE_CALC_SHIFT   16
#define MIN_PULLIN_TIME             20					// in msec
#define DUTY_CYCLE_UPDATE_RATE      211					// in msec
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
Output_PWM_CtrlV::Output_PWM_CtrlV( uC_PWM* pwm_ctrl, UINT16 min_pullin_v, UINT16 min_hold_v,
									Ctrl_Voltage* ctrl_voltage_ctrl, BOOL initial_state )
{

	m_min_pullin_v = min_pullin_v;
	m_min_hold_v = min_hold_v;
	m_ctrl_voltage_ctrl = ctrl_voltage_ctrl;
	m_pwm_ctrl = pwm_ctrl;
	m_polarity = uC_PWM_DUTY_CYCLE_LOW;
	m_use_pullin_v = true;


	m_pwm_ctrl->Configure( uC_PWM_DUTY_CYCLE_LOW, uC_PWM_LEFT_ALIGNED,
						   PWM_FREQUENCY );
	Set_To( initial_state );

	m_timer = new Timers( Update_Duty_Cycle_Static, ( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD )this,
						  TIMERS_DEFAULT_CR_PRIORITY, "Output PWM" );

}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

UINT16 Output_PWM_CtrlV::Duty_Cycle_Calc( void )
{
	BF_Lib::SPLIT_UINT32 duty_cycle;
	UINT16 ctrl_voltage;
	UINT32 target_coil_v;

	ctrl_voltage = m_ctrl_voltage_ctrl->Get_Voltage();

	if ( m_use_pullin_v )
	{
		target_coil_v = m_min_pullin_v;
	}
	else
	{
		target_coil_v = m_min_hold_v;
	}

	if ( ctrl_voltage <= target_coil_v )
	{
		duty_cycle.u16[1] = PWM_100_DUTY_CYCLE;
	}
	else
	{
		target_coil_v = ( ( UINT32 ) target_coil_v << PWM_DUTY_CYCLE_CALC_SHIFT );
		duty_cycle.u32 = ( target_coil_v / ctrl_voltage * PWM_100_DUTY_CYCLE );
	}

	// We compliment the duty cycle calc to specify the "OFF" time of the cycle
	return ( ~( duty_cycle.u16[1] ) );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

void Output_PWM_CtrlV::Update_Duty_Cycle()
{
	if ( m_use_pullin_v )
	{
		m_timer->Start( DUTY_CYCLE_UPDATE_RATE, true );
	}

	m_use_pullin_v = false;
	m_pwm_ctrl->Set_Duty_Cycle_Pct( Duty_Cycle_Calc() );

}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

void Output_PWM_CtrlV::On()
{
	m_timer->Start( MIN_PULLIN_TIME, true );
	m_use_pullin_v = true;
	m_pwm_ctrl->Set_Duty_Cycle_Pct( Duty_Cycle_Calc() );
	m_pwm_ctrl->Turn_On();
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

void Output_PWM_CtrlV::Off()
{
	m_pwm_ctrl->Turn_Off();
	m_timer->Stop();
}
