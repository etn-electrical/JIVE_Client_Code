/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Output_PWM.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Output_PWM::Output_PWM( uC_PWM* pwm_ctrl, uint32_t frequency, uint8_t polarity )
{
	m_pwm_ctrl = pwm_ctrl;
	m_pwm_ctrl->Configure( polarity, uC_PWM_LEFT_ALIGNED, frequency );
	Off();
	m_pwm_ctrl->Set_Duty_Cycle_Pct( 0x8000 );
}
