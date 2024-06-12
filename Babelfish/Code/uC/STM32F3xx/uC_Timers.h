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
#ifndef uC_TIMERS_H
   #define uC_TIMERS_H

#include "uC_Interrupt.h"
#include "uC_Base.h"

/*
 **************************************************************************************************
 *  Constants
   --------------------------------------------------------------------------------------------------
 */
#define MAX_NUM_TC_PRESCALERS   5

enum	// Capture Config
{
	uC_TMRS_CAPTURE_EDGE_NONE,
	uC_TMRS_CAPTURE_EDGE_RISING,
	uC_TMRS_CAPTURE_EDGE_FALLING,
	uC_TMRS_CAPTURE_EDGE_CHANGE,
	uC_TMRS_CAPTURE_MAX_OPTIONS
};

typedef uint16_t uC_TIMERS_TICK_TD;

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Timers
{
	public:
		uC_Timers( uint8_t timer_num );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Timers() {}

		void Set_Callback( INT_CALLBACK_FUNC* int_callback, uint8_t priority ) const;

		uint32_t Set_Timeout( uint32_t desired_frequency, bool repeat = true );

		inline void Restart_Timeout( void )     { m_tc->EGR = TIM_EGR_UG; }

		void Start_Timeout( void );

		void Stop_Timeout( void );

		bool Time_Expired( void )            { return ( ( m_tc->SR & TIM_SR_UIF ) != 0 ); }

		void Run( void );

		void Stop( void );

		uC_TIMERS_TICK_TD Set_Timer_Frequency( uint32_t desired_frequency );

		void Set_Tick_Frequency( uint32_t desired_frequency );

		uC_TIMERS_TICK_TD Get_Timer_Ticks( uint32_t desired_freq ) const;		// How many ticks to get what freq.

		inline uC_TIMERS_TICK_TD Get_Timer_Value( void )                     { return ( m_tc->CNT ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		bool Time_Expired( uint16_t start_time, uint16_t duration )
		{ return ( ( m_tc->CNT - start_time ) >= duration ); }

		inline void Enable_Int( void )              { uC_Interrupt::Enable_Int( m_tmr_ctrl->irq_num ); }

		inline void Disable_Int( void )             { uC_Interrupt::Disable_Int( m_tmr_ctrl->irq_num ); }

		inline void Ack_Int( void )                 {while ( m_tc->SR != 0 ){ m_tc->SR = 0;Nop();Nop(); } }			// {
																													// m_tc->SR
																													// =
																													// 0;
																													// Nop();
																													// Nop();
																													// Nop();
																													// Nop();
																													// }

	private:
		TIM_TypeDef* m_tc;
		uC_BASE_TIMER_IO_STRUCT const* m_tmr_ctrl;
		uint32_t m_tclock_freq;
};



#endif
