/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module is responsible for handling a PWMed relay.
 *
 *	@details In order to reduce the amount of power used holding a relay closed we
 *	can utilize the PWM to control the amount of current utilized by the relay.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RELAY_PWM_OUTPUT_H
#define RELAY_PWM_OUTPUT_H

#include "Includes.h"
#include "Output.h"
#include "uC_IO_Config.h"
#include "Voltage_Divider.h"
#include "Output_PWM.h"

namespace BF_Lib
{

/**
 ****************************************************************************************
 * @brief This is the Relay_PWM_Output class to work with the relay using the
 * Pulse Width Modulation.
 *
 * @details In order to reduce the amount of power used holding a relay closed we can
 * utilize the PWM to control the amount of current utilized by the relay. The duty cycle
 * of the PWM should be regulated by the supply voltage.
 * @n @b Usage:
 * @n @b 1. This module is responsible for handling a PWMed relay.
 * @n @b 2. It provides public methods to: turn on or off the relay with the PWM
 * control, to set the desired state, all with the PWM control and know the output state.
 ****************************************************************************************
 */
class Relay_PWM_Output : public BF_Lib::Output
{
	public:
		/**
		 * @brief Specifies the relay details. @todo
		 */
		struct relay_spec_t
		{
			uint16_t pull_in_v;			///< Voltage required for pull in (millivolts).
			uint16_t hold_v;			///< Voltage required for hold (millivolts).
			BF_Lib::Timers::TIMERS_TIME_TD pull_in_time;	///< Time in milliseconds required for a valid close.
			uint32_t frequency;			///< PWM frequency to be used on the relay.
			bool polarity;				///< Output polarity.
		};

		/**
		 * @brief @todo
		 * @param pwm_ctrl:
		 * @param relay_spec: The relay specifications related to frequency, voltages, and other specs.
		 * @param ctrl_voltage_ctrl:
		 */
		Relay_PWM_Output( uC_PWM* pwm_ctrl,
						  relay_spec_t const* relay_spec,	// uint16_t pullin_v, uint16_t hold_v,
						  BF_Misc::Voltage_Divider* ctrl_voltage_ctrl );

		/**
		 * @brief Destructor.
		 * @details The Destructor is not supported.
		 */
		~Relay_PWM_Output();

		/**
		 * @brief
		 * @details Turn on the Relay.  It will start a state machine to kick the
		 * relay then reduce the voltage to minimum.
		 * return None
		 */
		void On( void );

		/**
		 * @brief
		 * @details Turn off the Relay.
		 * return None
		 */
		void Off( void );

		/**
		 * @brief
		 * @details Set the relay to the passed in state.
		 * @param state: Desired output state.
		 * return None
		 */
		void Set_To( bool state )
		{
			if ( state != false )
			{
				On();
			}
			else
			{
				Off();
			}
		}

		/**
		 * @brief
		 * @details Provides the present relay state.
		 * @return The present relay state.
		 */
		bool State( void )
		{
			return ( m_pwm_out->State() );
		}

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Relay_PWM_Output( const Relay_PWM_Output& object );
		Relay_PWM_Output & operator =( const Relay_PWM_Output& object );

		static const uint16_t PWM_100_DUTY_CYCLE = 0xFFFFU;
		static const uint8_t PWM_DUTY_CYCLE_CALC_SHIFT = 16U;

		// The hold state duty cycle update rate will be a multiple of the pull in time.
		static const uint8_t DUTY_CYCLE_UPDATE_MULTIPLIER = 5U;

		BF_Lib::Timers* m_timer;
		BF_Misc::Voltage_Divider* m_ctrl_voltage_ctrl;
		relay_spec_t const* m_relay_spec;

		Output_PWM* m_pwm_out;

		uint16_t Duty_Cycle_Calc( uint16_t target_coil_v );

		void Update_Duty_Cycle( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Update_Duty_Cycle_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{
			( ( Relay_PWM_Output* )handle )->Update_Duty_Cycle();
		}

};

}

#endif	/* RELAY_PWM_OUTPUT_H */
