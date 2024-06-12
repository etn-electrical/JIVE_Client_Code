/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Hardware Based Timer Interface
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
#ifndef uC_TIMERS_HW_H
   #define uC_TIMERS_HW_H

#include "Includes.h"
#include "uC_Interrupt.h"
#include "Advanced_Vect.h"


/**
 ****************************************************************************************
 * @brief Provides the low level interface and config for the USART.  This will likely
 * always be replaced by another derived class.
 *
 * @details
 *
 ****************************************************************************************
 */
class uC_Timers_HW
{
	public:
		uC_Timers_HW( uint8_t timer_num );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Timers_HW() {}

		// The following callback type is for applications where you are planning on using
		// the coroutine functionality.
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
							  uint8_t int_priority, bool raw_interrupt = false );

		typedef uint32_t TICK_TD;

		inline void Run( void )         { m_tc->EGR = TIM_EGR_UG; }

		inline void Enable( void )          { m_tc->CR1 |= TIM_CR1_CEN; }

		inline void Disable( void )     { m_tc->CR1 &= ~TIM_CR1_CEN; }

		TICK_TD Set_Timer_Frequency( uint32_t desired_frequency );

		void Set_Tick_Frequency( uint32_t desired_frequency );

		TICK_TD Get_Timer_Ticks( uint32_t desired_frequency ) const;		// How many ticks to get what freq.

		inline uint8_t Get_Timer_Byte_Size( void )         { return ( m_tmr_ctrl->cnt_size_bytes ); }

		inline TICK_TD Get_Timer_Value( void )             { return ( m_tc->CNT ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		bool Time_Expired( uint16_t start_time, uint16_t duration )
		{ return ( ( m_tc->CNT - start_time ) >= duration ); }

		inline void Enable_CC_Int( void )               { uC_Interrupt::Enable_Int( m_tmr_ctrl->irq_num ); }

		inline void Disable_CC_Int( void )          { uC_Interrupt::Disable_Int( m_tmr_ctrl->irq_num ); }

		inline void Enable_Int( void )              { uC_Interrupt::Enable_Int( m_tmr_ctrl->irq_num ); }

		inline void Disable_Int( void )             { uC_Interrupt::Disable_Int( m_tmr_ctrl->irq_num ); }

		inline void Ack_Int( void )                 { while ( m_tc->SR != 0 ){ m_tc->SR = 0;Nop();Nop(); } }		// while(
																													// m_tc->SR
																													// !=
																													// 0
																													// )
																													// {
																													// m_tc->SR
																													// =
																													// 0;
																													// Nop();
																													// Nop();
																													// }
																													// }

		inline uint8_t Num_Compares( void )                { return ( m_tmr_ctrl->num_compares ); }

		/*
		 *****************************************************************************************
		 *		Nanosecond timer behavior.
		 *****************************************************************************************
		 */
		static uint32_t volatile* const DWT_CYCCNT;	// address of the register
		static uint32_t volatile* const DWT_CONTROL;// address of the register
		static uint32_t volatile* const SCB_DEMCR;	// address of the register

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static void Init_nSec( void )
		{
			*SCB_DEMCR = *SCB_DEMCR | 0x01000000U;
			*DWT_CYCCNT = 0U;	// reset the counter
			*DWT_CONTROL = *DWT_CONTROL | 1U;	// enable the counter
		}

		inline static uint32_t Get_Time_nSec( void ) { return ( *DWT_CYCCNT ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint32_t Elapsed_nSec( uint32_t sample_time )
		{
			uint32_t elapsed_time;

			elapsed_time = *DWT_CYCCNT;

			elapsed_time = elapsed_time - sample_time;
			elapsed_time = ( elapsed_time * 1000U ) / ( MASTER_CLOCK_FREQUENCY / 1000000U );

			return ( elapsed_time );
		}

	protected:
		TIM_TypeDef* m_tc;
		uC_BASE_TIMER_IO_STRUCT const* m_tmr_ctrl;
		uint32_t m_tclock_freq;

	private:
		void Int_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Int_Handler_Static( Advanced_Vect::cback_param_t param )
		{ ( ( uC_Timers_HW* )param )->Int_Handler(); }

		void Raw_Int_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Raw_Int_Handler_Static( Advanced_Vect::cback_param_t param )
		{ ( ( uC_Timers_HW* )param )->Raw_Int_Handler(); }

		void Dummy_Vect( void )             { Ack_Int(); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Dummy_Vect_Static( cback_param_t param )
		{ ( ( uC_Timers_HW* )param )->Dummy_Vect(); }

		Advanced_Vect* m_advanced_vect;
		cback_param_t m_cback_param;
		cback_func_t m_cback_func;

};



#endif
