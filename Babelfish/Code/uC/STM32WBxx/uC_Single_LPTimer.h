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
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_SINGLE_LPTIMER_H
   #define uC_SINGLE_LPTIMER_H

#include "uC_Interrupt.h"
#include "uC_LPTimers_HW.h"

/**
 ****************************************************************************************
 * @brief Provides a single HW timer interface.
 *
 * @details Will take a single hardware timer and turn it into a quite precise
 * interval timer.  This will tend to be quicker than a multi-timer.
 *
 ****************************************************************************************
 */
class uC_Single_LPTimer : private uC_LPTimers_HW
{
	public:

		uC_Single_LPTimer( uint8_t timer_num );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Single_LPTimer() {}

		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @brief	registering callback function
		 * @param
		 *
		 * @details interface function to attach ISR
		 **/
		void Set_Callback( cback_func_t func_callback,
						   cback_param_t func_param, uint8_t int_priority );

		/**
		 * @brief Provides timeout of max 1 sec in frequency
		 * @param
		 *			@in		desired_frequency
		 *			@return	count loaded in ARR
		 * @details
		 **/
		TICK_TD Set_Timeout( uint32_t desired_frequency );

		/**
		 * @brief Provides timeout of more than 1 sec.
		 * @param
		 *			@in		desired_period_sec
		 *			@return	count loaded in ARR
		 * @details Depending upon timeout required, current LP TImer clock and COunter limitiation
		 * 			Prescaler is calculated. The pressacler is then rounded up to next
		 *			8-bit possiblity.
		 *			TODO check the logic behaviour for values less than 10
		 **/
		TICK_TD Set_Wait_Period_Sec( uint32_t desired_period_sec );

		/**
		 * @brief  Start the counter of timer
		 * @param
		 *			@in		bool repeat
		 *			@return	void
		 * @details enables the Timer interrupt
		 **/
		void Start_Timeout( bool repeat = true );

		/**
		 * @brief Disable Timer
		 * @param
		 * @details Disables the timer and also clears Interrupt Flags
		 **/
		void Stop_Timeout( void );

	private:
};

#endif
