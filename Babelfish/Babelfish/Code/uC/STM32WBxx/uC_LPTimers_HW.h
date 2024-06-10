/**
 *****************************************************************************************
 *	@file           uC_LPTimers_HW.h header file for hardware  low power timer functionality.
 *
 *	@brief          This file shall wrap up all the functions required for the  functionality
 *                      of Hardware Based Timer Interface
 *
 *	@details
 *
 *      @copyright      2020 Eaton Corporation. All Rights Reserved.
 *
 *      @note          Eaton Corporation claims proprietary rights to the material disclosed
 *                     here in.  This technical information should not be reproduced or used
 *                     without direct written permission from Eaton Corporation.
 *
 *****************************************************************************************
 */
#ifndef uC_LPTIMERS_HW_H
   #define uC_LPTIMERS_HW_H

#include "Includes.h"
#include "uC_Interrupt.h"
#include "Advanced_Vect.h"

/**
 *
 * @brief        LPTimer _HW class declaration
 * @details
 *
 *
 */

static const uint32_t CLEAR_ALL_INTR_FLAG = 0x007F;
static const uint32_t CLEAR_ARR_INTR_FLAG = 0x0002;

class uC_LPTimers_HW
{
	public:
		/**
		 * @brief               Constructor to create an instance of uC_LPTimers_HW class.
		 * @param[in]           uC_BASE_LPTIMER_IO_STRUCT  Timer block definition structure
		 * @note
		 */
		uC_LPTimers_HW( uint8_t timer_num, uC_BASE_LPTIMER_IO_STRUCT const* lptimer_io = NULL );

		/**
		 * @brief               Destructor to delete the uC_LPTimer_HW instance when it goes out of scope
		 */
		~uC_LPTimers_HW()
		{}

		/**@brief  The following callback type is for applications where you are planning on using
		   the coroutine functionality.
		 */
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @brief                   This function  configures the function which is to be called when a
		 *                          timer interrupt is called.
		 * @param[in] func_callback The function pointer of the function to be called when an interrupt
		   occurs
		 * @param[in] func_param    The function parameter if nay
		 * @param[in] int_priority  Interrupt priority
		 * @param[in] raw_interrupt Two possible values
		                           TRUE - If its a raw interrupt
		                           FALSE -  otherwise
		 */
		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
							  uint8_t int_priority, bool raw_interrupt = false );

		/**
		 * @brief                   This function  configures the function which is to be called when a
		 *                          timer interrupt is called.
		 * @param[in] func_callback The function pointer of the function to be called when
		                            an interrupt occurs
		 * @param[in] func_param    The function parameter if nay
		 * @param[in] int_priority  Interrupt priority
		 * @param[in] raw_interrupt Two possible values
		                            TRUE - If its a raw interrupt
		                            FALSE -  otherwise
		 */
		void Attach_CC_Callback( cback_func_t func_callback, cback_param_t func_param,
								 uint8_t int_priority, bool raw_interrupt = false );

		/**@brief  Typedef to get the count of the timer ticks. */
		typedef uint32_t TICK_TD;

		/**@brief    This function is used to enable the lptimer.This is done by enablimg the control register */
		inline void Enable( void )
		{
			m_lptc->CR |= LPTIM_CR_ENABLE;
		}

		/**@brief    This function is used to disable the lptimer.This is done by disablimg the control register */
		inline void Disable( void )
		{
			m_lptc->CR &= ~LPTIM_CR_ENABLE;
		}

		/**
		 *@brief            This function is used to set the timer frequency by changing the prescaler value..
		 *@param[in]desired_frequency The desired frequency is given as input.
		   @param[out]        The number of timer ticks
		 */
		TICK_TD Set_Timer_Frequency( uint32_t desired_frequency );

		/**
		 *@brief           This function is used to set the tick frequency by dividing the m_tclock_freq and
		 * desired_frequency.
		 *@param[in] desired_frequency The desired frequency is used as input.
		 */
		void Set_Tick_Frequency( uint32_t desired_frequency );

		/**
		 *@brief           This function is used to get the number of timer ticks according to the desired frequency.
		 *@param[in] desired_frequencyThe desired frequency is used as input.
		 *@return[out]    The numebr of timer ticks
		 */
		TICK_TD Get_Timer_Ticks( uint32_t desired_frequency ) const;		// How many ticks to get what freq.

		/**@brief  This function is used to get the byte size of the count.*/
		inline uint8_t Get_Timer_Byte_Size( void )
		{
			return ( m_lptmr_ctrl->cnt_size_bytes );
		}

		/**@brief  This function is used to get the count value*/
		inline TICK_TD Get_Timer_Value( void )
		{
			return ( m_lptc->CNT );
		}

		/**
		 *@brief           This function is used to check if the set timeout value has passed by
		 *                 checking the value in the counter.
		 *@param[in]start_time  The start time for the counting to begin.
		 *@param[in]duration    The duration of the count
		 *@return[out]bool      Two possible values True/False
		 */
		bool Time_Expired( uint16_t start_time, uint16_t duration )
		{
			return ( ( m_lptc->CNT - start_time ) >= duration );
		}

		/**@brief  This function enables count to compare interrupt*/
		inline void Enable_CC_Int( void )
		{
			uC_Interrupt::Enable_Int( m_lptmr_ctrl->irq_num_cc );
		}

		/**@brief  This function disables count to compare interrupt*/
		inline void Disable_CC_Int( void )
		{
			uC_Interrupt::Disable_Int( m_lptmr_ctrl->irq_num_cc );
		}

		/**@brief  This function is used to enable the interrupt*/
		inline void Enable_Int( void )
		{
			uC_Interrupt::Enable_Int( m_lptmr_ctrl->irq_num );
		}

		/**@brief  This function is used to disable the interrupt*/
		inline void Disable_Int( void )
		{
			uC_Interrupt::Disable_Int( m_lptmr_ctrl->irq_num );
		}

		/**@brief  This function is used to acknowledge the occurence of an interrupt*/
		inline void Ack_Int( void )
		{
			m_lptc->ICR = 0x7F;	// for all interrupts	2; //for ARR
			Nop();
			Nop();
			Nop();
			Nop();
		}	// while( m_tc->SR != 0 ) { m_tc->SR = 0; Nop(); Nop(); } }

		/**@brief  This function is used to get compares*/
		inline uint8_t Num_Compares( void )
		{
			return ( m_lptmr_ctrl->num_compares );
		}

	protected:
		LPTIM_TypeDef* m_lptc;
		uC_BASE_LPTIMER_IO_STRUCT const* m_lptmr_ctrl;
		uint32_t m_tclock_freq;

	private:
		void Int_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Int_Handler_Static( Advanced_Vect::cback_param_t param )
		{
			( ( uC_LPTimers_HW* )param )->Int_Handler();
		}

		void Raw_Int_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Raw_Int_Handler_Static( Advanced_Vect::cback_param_t param )
		{
			( ( uC_LPTimers_HW* )param )->Raw_Int_Handler();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Dummy_Vect( void )
		{
			Ack_Int();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Dummy_Vect_Static( cback_param_t param )
		{
			( ( uC_LPTimers_HW* )param )->Dummy_Vect();
		}

		Advanced_Vect* m_advanced_vect;
		cback_param_t m_cback_param;
		cback_func_t m_cback_func;

};

#endif
