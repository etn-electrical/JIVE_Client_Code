/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_PWM_H
   #define uC_PWM_H

#include "uC_Interrupt.h"
#include "uC_Base.h"

/*
 **************************************************************************************************
 *  Constants
   --------------------------------------------------------------------------------------------------
 */
enum		// Polarity
{
	uC_PWM_DUTY_CYCLE_LOW,
	uC_PWM_DUTY_CYCLE_HIGH
};

enum		// Alignment
{
	uC_PWM_LEFT_ALIGNED,
	uC_PWM_CENTER_ALIGNED
};

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_PWM
{
	public:
		uC_PWM( uint8_t pwm_ctrl_block, uint8_t pwm_channel,
				uC_PERIPH_IO_STRUCT const* port_io = NULL );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_PWM() {}

		void Turn_On( void );

		void Turn_Off( void );

		bool Is_On( void ) const;

		void Configure( uint8_t polarity, uint8_t alignment,
						uint32_t desired_freq, bool default_state = false );

		void Set_Duty_Cycle_Pct( uint16_t percent );		// ATTENTION - This is a 16 bit percent. 50% = 0x8000

	private:
		uC_BASE_TIMER_IO_STRUCT const* m_pwm_ctrl;
		uint8_t m_pwm_ch;
		uint16_t m_def_state_mask;
		uint32_t volatile* m_compare_reg;
		uint32_t m_percent_adj;
		uC_PERIPH_IO_STRUCT const* m_port_io;
};



#endif
