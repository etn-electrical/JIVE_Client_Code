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
#include "Advanced_Vect.h"

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

		// The following callback type is for applications where you are planning on using
		// the coroutine functionality.
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
							  uint8_t int_priority, bool raw_interrupt = false );

		void Attach_CC_Callback( cback_func_t func_callback, cback_param_t func_param,
								 uint8_t int_priority, bool raw_interrupt = false );

		void Set_Callback( INT_CALLBACK_FUNC* int_callback, uint8_t int_priority ) const;

		uC_TIMERS_TICK_TD Set_Timeout( uint32_t desired_frequency, bool repeat = true );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Restart_Timeout( void )
		{
			Push_TGINT();
			m_tc->CR1 |= TIM_CR1_UDIS;
			m_tc->EGR = TIM_EGR_UG;
			m_tc->CR1 &= ~TIM_CR1_UDIS;
			Pop_TGINT();
		}

		void Start_Timeout( void );

		void Stop_Timeout( void );

		bool Timeout_Running( void ) const;

		bool Time_Expired( void )            { return ( ( m_tc->SR & TIM_SR_UIF ) != 0 ); }

		void Run( void );

		void Stop( void );

		uC_TIMERS_TICK_TD Set_Timer_Frequency( uint32_t desired_frequency );

		void Set_Tick_Frequency( uint32_t desired_frequency );

		uC_TIMERS_TICK_TD Get_Timer_Ticks( uint32_t desired_frequency ) const;			// How many ticks to get what
																						// freq.

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

		inline void Ack_Int( void )                 { m_tc->SR = 0;Nop();Nop();Nop();Nop(); }

	private:
		void Int_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Int_Handler_Static( Advanced_Vect::cback_param_t param )
		{ ( ( uC_Timers* )param )->Int_Handler(); }

		void Raw_Int_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Raw_Int_Handler_Static( Advanced_Vect::cback_param_t param )
		{ ( ( uC_Timers* )param )->Raw_Int_Handler(); }

		Advanced_Vect* m_advanced_vect;
		cback_param_t m_cback_param;
		cback_func_t m_cback_func;

		TIM_TypeDef* m_tc;
		uC_BASE_TIMER_IO_STRUCT const* m_tmr_ctrl;
		uint32_t m_tclock_freq;
};



#endif
