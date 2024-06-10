/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module provides a PWM output control.
 *
 *	@details This control can provide a generic output with the
 *	benefit of a duty cycle adjustment.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OUTPUT_PWM_H
   #define OUTPUT_PWM_H

#include "Includes.h"
#include "Output.h"
#include "uC_IO_Config.h"
#include "uC_PWM.h"

/**
 ****************************************************************************************
 *	@brief This module is responsible for providing a generic output as a PWM channel.
 *
 *	@details There are instances where someone would want to have a PWM output which
 *	runs as a standard output.  This could be used for something like an LED array.
 *
 ****************************************************************************************
 */
class Output_PWM : public BF_Lib::Output
{
	public:
		/**
		 * @brief The default PWM frequency.  Typically above audible range.
		 */
		static const uint32_t PWM_DEFAULT_FREQ = 20000;

		/**
		 * @brief Constructor for the PWM.  Will create the PWM channel but not
		 * actually start it up.  Should behave like other outputs.
		 * @param pwm_ctrl - The uC_PWM control interface.
		 * @param frequency - The run frequency.
		 * @param polarity - The polarity for the duty cycle.  So a polarity of low and 10%
		 * duty cycle will provide a low period of 10%.
		 */
		Output_PWM( uC_PWM* pwm_ctrl,
					uint32_t frequency = PWM_DEFAULT_FREQ,
					uint8_t polarity = uC_PWM_DUTY_CYCLE_LOW );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Output_PWM() {}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void On( void )
		{ m_pwm_ctrl->Turn_On(); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Off( void )
		{ m_pwm_ctrl->Turn_Off(); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Set_To( bool state )
		{
			if ( state != 0 )
			{
				m_pwm_ctrl->Turn_Off();
			}
			else
			{
				m_pwm_ctrl->Turn_On();
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool State( void )
		{ return ( m_pwm_ctrl->Is_On() ); }


		void Duty_Cycle( uint16_t percent );

	private:
		uC_PWM* m_pwm_ctrl;
};



#endif
