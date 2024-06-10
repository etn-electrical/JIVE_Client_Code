/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Single timeout timer.  It uses an overflow time.  It is quite accurate and
 *	easy to reset compared to multi-timer.
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_SINGLE_TIMER_H
   #define uC_SINGLE_TIMER_H

#include "uC_Interrupt.h"
#include "uC_Timers_HW.h"

/**
 ****************************************************************************************
 * @brief Provides a single HW timer interface.
 *
 * @details Will take a single hardware timer and turn it into a quite precise
 * interval timer.  This will tend to be quicker than a multi-timer.
 *
 ****************************************************************************************
 */
class uC_Single_Timer : private uC_Timers_HW
{
	public:

		uC_Single_Timer( uint8_t timer_num );
		~uC_Single_Timer() {}

		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );
		void Set_Callback( cback_func_t func_callback,
						   cback_param_t func_param, uint8_t int_priority );

		TICK_TD Set_Timeout( uint32_t desired_frequency, bool repeat = true );

		void Restart_Timeout( void );

		void Start_Timeout( void );

		void Stop_Timeout( void );

		bool Timeout_Running( void ) const;

		bool Time_Expired( void )       { return ( ( m_tc->SR & TIM_SR_UIF ) != 0 ); }

	private:
};



#endif
