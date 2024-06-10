/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Relay_PWM_Output.h"
#include "Babelfish_Assert.h"
namespace BF_Lib
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Relay_PWM_Output::Relay_PWM_Output( uC_PWM* pwm_ctrl, relay_spec_t const* relay_spec,
									BF_Misc::Voltage_Divider* ctrl_voltage_ctrl ) :
	m_timer( reinterpret_cast<BF_Lib::Timers*>( nullptr ) ),
	m_ctrl_voltage_ctrl( reinterpret_cast<BF_Misc::Voltage_Divider*>( nullptr ) ),
	m_relay_spec( reinterpret_cast<relay_spec_t const*>( nullptr ) ),
	m_pwm_out( reinterpret_cast<Output_PWM*>( nullptr ) ),
	Output()
{
	m_timer = new BF_Lib::Timers( &Update_Duty_Cycle_Static,
								  reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
								  BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
								  "Relay PWM Out" );

	m_ctrl_voltage_ctrl = ctrl_voltage_ctrl;

	m_relay_spec = relay_spec;

	m_pwm_out = new Output_PWM( pwm_ctrl, m_relay_spec->frequency, m_relay_spec->polarity );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Relay_PWM_Output::Duty_Cycle_Calc( uint16_t target_coil_v )
{
	uint32_t duty_cycle;
	uint16_t ctrl_voltage;

	ctrl_voltage = m_ctrl_voltage_ctrl->Get_Voltage();

	if ( ctrl_voltage <= target_coil_v )
	{
		duty_cycle = PWM_100_DUTY_CYCLE;
		duty_cycle = duty_cycle << PWM_DUTY_CYCLE_CALC_SHIFT;
	}
	else
	{
		target_coil_v = static_cast<uint16_t>( static_cast<uint32_t>
											   ( target_coil_v ) <<
											   PWM_DUTY_CYCLE_CALC_SHIFT );
		duty_cycle =
			( ( static_cast<uint32_t>
				( target_coil_v ) / ctrl_voltage ) * PWM_100_DUTY_CYCLE );
	}

	// We compliment the duty cycle calc to specify the "OFF" time of the cycle
	return ( static_cast<uint16_t>( ~( duty_cycle >> PWM_DUTY_CYCLE_CALC_SHIFT ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Relay_PWM_Output::Update_Duty_Cycle( void )
{
	m_pwm_out->Duty_Cycle( Duty_Cycle_Calc( m_relay_spec->hold_v ) );
	bool temp_state = State();

	Push_TGINT();

	if ( ( !m_timer->Auto_Reload_Enabled() ) && ( temp_state == true ) )
	{
		m_timer->Start( ( DUTY_CYCLE_UPDATE_MULTIPLIER * m_relay_spec->pull_in_time ), true );
	}

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Relay_PWM_Output::On()
{
	m_pwm_out->Duty_Cycle( Duty_Cycle_Calc( m_relay_spec->pull_in_v ) );

	Push_TGINT();

	m_pwm_out->On();
	m_timer->Start( m_relay_spec->pull_in_time, false );

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Relay_PWM_Output::Off()
{
	Push_TGINT();

	m_timer->Stop();
	m_pwm_out->Off();

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Relay_PWM_Output::~Relay_PWM_Output()
{
	delete m_pwm_out;
	BF_ASSERT( false );
}

}	/* end namespace BF_Lib for this module */

