/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module provides the ability to create multiple timers on one HW timer.
 *
 *	@details Using one hardware timer with a single time base we can use the compare
 * 	behavior to create up to X timers.  The callbacks are all from an interrupt so
 * 	care must be taken as to the time taken inside this interrupt.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_MULTI_TIMERS_H
   #define uC_MULTI_TIMERS_H

#include "Includes.h"
#include "uC_Interrupt.h"
#include "uC_Timers_HW.h"

/**
 ****************************************************************************************
 * @brief This module provides the ability to create multiple timers on one HW timer.
 *
 * @details Using one hardware timer with a single time base we can use the compare
 * behavior to create up to X timers.  The callbacks are all from an interrupt so
 * care must be taken as to the time taken inside this interrupt.
 *
 ****************************************************************************************
 */
class uC_Multi_Timers : uC_Timers_HW
{
	public:
		uC_Multi_Timers( uint8_t hw_timer_num, uint8_t num_timers, uint8_t int_priority );
		~uC_Multi_Timers();

		void Set_Min_Frequency( uint32_t desired_frequency );

		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );
		void Set_Callback( uint8_t timer_num, cback_func_t func_callback,
						   cback_param_t func_param = NULL );

		void Set_Timeout( uint8_t timer_num, uint32_t desired_frequency, bool repeat = true );

		void Start_Timeout( uint8_t timer_num );

		void Stop_Timeout( uint8_t timer_num );

		bool Timeout_Running( uint8_t timer_num ) const;

		void Restart_Timeout( uint8_t timer_num );

		void Reload_Timeout( uint8_t timer_num );

		bool Timeout_Pending( uint8_t timer_num ) const;

	private:
		void Int_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Int_Handler_Static( uC_Timers_HW::cback_param_t param )
		{ ( ( uC_Multi_Timers* )param )->Int_Handler(); }

		static void Dummy_Static( uC_Timers_HW::cback_param_t param )   {}

		struct timer_def_t
		{
			cback_param_t cback_param;
			cback_func_t cback_func;
			TICK_TD timer_counts;
			bool repeat;
		};

		timer_def_t* m_timer_defs;
		uint8_t m_num_timers;

};



#endif
