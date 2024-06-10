/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Hardware Based microsecond timer interface.
 *
 *	@details Uses a hardware timer to set a tick frequency in the microseconds.
 *	 It is a free running timer.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TIMERS_USEC_H
#define TIMERS_USEC_H

#include "Includes.h"

// LTK-4767: TODO: This should be removed once uc_Timers_HW.h file is available for ESP32 platform
#ifndef ESP32_SETUP
#include "uC_Timers_HW.h"
#endif

namespace BF_Lib
{

/**
 ****************************************************************************************
 * @brief Its is the Hardware Based microsecond timer interface.
 * @details Uses a hardware timer to set a tick frequency in the microseconds.
 * It is a free running timer.  You can simple check for an expired value.
 * @n @b Usage:
 * @n @b 1. It provides public methods to: To get the time count value,
 * know if the timer is expired, the value of the time passed.
 ****************************************************************************************
 */

class Timers_uSec
{
	public:
		/**
		 * Type alias for the type TICK_TD, used for the time values.
		 */
		typedef uC_Timers_HW::TICK_TD TIME_TD;

		/**
		 *  @brief
		 *  @details Initializes the data members for the microsecond timer interface.
		 *  @param[in] timer_ctrl: the function to be called that gives a handle,
		 *  assigned to the member m_cback.
		 *  @return None
		 */
		static void Init( uC_Timers_HW* timer_ctrl );

		/**
		 *  @brief
		 *  @details To get the time count value (time).
		 *  @return The value of the tick count.
		 */
		inline static TIME_TD Get_Time( void )
		{
			volatile TIME_TD temp_time;

			Push_TGINT();
			temp_time = m_timer_ctrl->Get_Timer_Value();
			Pop_TGINT();
			return ( temp_time );
		}

		/**
		 *  @brief
		 *  @details To know if the timer is expired depending on the duration,
		 *  start and tick count.
		 *  @param[in] start_time: The start time value for the timer.
		 *  @param[in] duration: The duration of the timer.
		 *  @return The status if the timer expired or not(false).
		 */
		inline static bool Expired( TIME_TD start_time, TIME_TD duration )
		{
			volatile TIME_TD temp_time;
			bool return_val;

			Push_TGINT();
			temp_time = m_timer_ctrl->Get_Timer_Value();
			temp_time = temp_time - start_time;
			temp_time &= m_timer_mask;
			return_val = ( temp_time >= duration );
			Pop_TGINT();

			return ( return_val );
		}

		/**
		 *  @brief
		 *  @details To know the value of the time passed, depending on the
		 *  start time and tick count.
		 *  @param[in] start_time: The start time value for the timer.
		 *  @return The value of the time passed.
		 */
		inline static TIME_TD Time_Passed( TIME_TD start_time )
		{
			volatile TIME_TD temp_time;

			Push_TGINT();
			temp_time = m_timer_ctrl->Get_Timer_Value();
			temp_time = temp_time - start_time;
			temp_time &= m_timer_mask;
			Pop_TGINT();

			return ( temp_time );
		}

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Timers_uSec( void );
		~Timers_uSec( void );
		Timers_uSec( const Timers_uSec& rhs );
		Timers_uSec & operator =( const Timers_uSec& object );

		static const uint32_t uSEC_FREQ = 1000000U;
		static uC_Timers_HW* m_timer_ctrl;
		static uint32_t m_timer_mask;
};

}

#endif	/* TIMERS_USEC_H */
