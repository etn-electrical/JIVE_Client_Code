/**
 *****************************************************************************************
 *	@file           uC_Timers.h header file for timer functionality.
 *
 *	@brief          This file shall wrap up all the functions required for the timer functionality.
 *
 *  @details
 *
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *	@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here in.  This technical information should not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *
 *****************************************************************************************
 */
#ifndef uC_TIMERS_H
   #define uC_TIMERS_H

#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "Advanced_Vect.h"

/**@brief Number of timer/counter prescalers*/
#define MAX_NUM_TC_PRESCALERS   5

/**@brief Capture configuration*/
enum
{
	/** @brief  Timer capture edge :no edge*/
	uC_TMRS_CAPTURE_EDGE_NONE,
	/** @brief Timer capture rising edge*/
	uC_TMRS_CAPTURE_EDGE_RISING,
	/** @brief Timer capture falling edge*/
	uC_TMRS_CAPTURE_EDGE_FALLING,
	/** @brief Timer capture edge change*/
	uC_TMRS_CAPTURE_EDGE_CHANGE,
	/** @brief Timer capture maximum*/
	uC_TMRS_CAPTURE_MAX_OPTIONS
};

/** @brief Typedef to get the count of the timer ticks.*/
typedef uint32_t uC_TIMERS_TICK_TD;

/**
 **************************************************************************************************
 * @brief          Timers Class Declaration
 * @details        This class includes all the functions used to start and stop the timer as well as
 *                 configure the interrupts.
 **************************************************************************************************
 */
class uC_Timers
{
	public:
		/**
		 * @brief                    Constructor to create an instance of uC_Timers class.
		 * @param[in]timer_num       The timer number of microcontroller which is to be initialized
		 *                           for timing purpose
		 *
		 * @note                     The constructor is used to select the timer number.It is used to
		 *                           reset the peripheral timer.
		 *                           It is used enable the peripheral clock for the timer and
		 *                           set the clock frequency.
		 *
		 *
		 */
		uC_Timers( uint8_t timer_num );

		/**
		 * @brief                   Destructor to delete the uC_Timer instance when it goes out of scope
		 */
		~uC_Timers()
		{}

		/**
		 *  @brief                   The following callback type is for applications where you are planning
		 *                           on using the coroutine functionality.
		 */
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @brief                   This function  configures the function which is to be called when
		 *                          a timer interrupt is called.
		 * @param[in] func_callback The function pointer of the function to be called when an interrupt
		                            occurs
		 * @param[in] func_param    The function parameter if any
		 * @param[in] int_priority  Interrupt priority
		 * @param[in] raw_interrupt Two possible values
		                            TRUE - If its a raw interrupt
		                            FALSE -  otherwise
		 */
		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
							  uint8_t int_priority, bool raw_interrupt = false );

		/**
		 * @brief                    This function  configures the function which is to be called when a timer interrupt
		 *is called.
		 * @param[in] func_callback  The function pointer of the function to be called when an interrupt
		                             occurs
		 * @param[in] func_param     The function parameter if any
		 * @param[in] int_priority   Interrupt priority
		 * @param[in] raw_interrupt  Two possible values
		                             TRUE - If its a raw interrupt
		                             FALSE -  otherwise
		 */
		void Attach_CC_Callback( cback_func_t func_callback, cback_param_t func_param,
								 uint8_t int_priority, bool raw_interrupt = false );

		/**
		 * @brief                    This function  is used to set the interrupt priority.
		 * @param[in]int_callback    Function to call the interrupt
		 * @param[in]int_priority    Interrupt priority.
		 */
		void Set_Callback( INT_CALLBACK_FUNC* int_callback, uint8_t int_priority ) const;

		/**
		 * @brief                         This function  is used to set the timeout value.
		 * @param[in]desired frequency    The desired frequency is given as input.
		 * @param[in]repeat               Two possible values True-to repeat the function
		 *                                                    False - Otherwise

		 * @note                         The timeout value is set by varying the prescaler index and setting
		 *                               the ARPE(AUTORELOAD PRELOAD ENABLE) bit of the timer.
		 */
		uC_TIMERS_TICK_TD Set_Timeout( uint32_t desired_frequency, bool repeat = true );

		/**
		 * @brief                       This function is used to restart the timer.
		 */
		inline void Restart_Timeout( void )
		{
			Push_TGINT();
			m_tc->CR1 |= TIM_CR1_UDIS;
			m_tc->EGR = TIM_EGR_UG;
			m_tc->CR1 &= ~TIM_CR1_UDIS;
			Pop_TGINT();
		}

		/**
		 * @brief                    This function  is used to start the count for the timeout value.
		                             (start the timer again)
		                             This function is used to enable the interrupt.
		 */
		void Start_Timeout( void );

		/**@brief        This function is used to stop the timer (stop the counting)
		 *               by disabling and acknowledging the interrupt.
		 */
		void Stop_Timeout( void );

		/**
		 *@brief          This function is used to test if the counting is going on by comparing the value in the
		                  CR1 register(timer control register1).
		 *@return[out]    Two possible values True/false
		 */
		bool Timeout_Running( void ) const;

		/**
		 *@brief          This function is used to test if the counting is completed by checking the value
		 *                in the timer status register.
		 *@return[out]    Two possible values True/false
		 */
		bool Time_Expired( void )
		{
			return ( ( m_tc->SR & TIM_SR_UIF ) != 0 );
		}

		/**
		 * @brief      This function is used to run the timer.This is done by enablimg
		               the event generation register
		 */
		void Run( void );

		/**
		 * @brief    This function is used to stop the timer.
		 */
		void Stop( void );

		/**
		 *@brief                        This function is used to set the timer frequency by
		                                changing the prescaler value.
		 *@param[in] desired_frequency  The desired frequency is given as input
		 *@return[out]                  The count of timer ticks
		 */
		uC_TIMERS_TICK_TD Set_Timer_Frequency( uint32_t desired_frequency );

		/**
		 *@brief           This function is used to set the tick frequency by dividing
		                   the m_tclock_freq and desired_frequency.
		 *@param[in] desired_frequencyThe desired frequency is given as input.
		 */
		void Set_Tick_Frequency( uint32_t desired_frequency );

		/**
		 *@brief           This function is used to get the number of timer ticks according to the desired frequency.
		 *@param[in]desired_frequency The desired frequency is given as input.
		 *@return[out]     The count of timer ticks
		 */
		uC_TIMERS_TICK_TD Get_Timer_Ticks( uint32_t desired_frequency ) const;	// How many ticks to get what freq.

		/**
		 * @brief  This function is used to get the byte size of the count.
		 */
		inline uint8_t Get_Timer_Byte_Size( void )
		{
			return ( m_tmr_ctrl->cnt_size_bytes );
		}

		/**
		 * @brief  This function is used to get the count value
		 */
		inline uC_TIMERS_TICK_TD Get_Timer_Value( void )
		{
			return ( m_tc->CNT );
		}

		/**
		 *@brief                This function is used to check if the set timeout value has passed by checking
		                        the value in the counter.
		 *@param[in]start_time  The start time for the counting to begin.
		 *@param[in]duration    The duration of the count
		 *@return[out]bool      Two possible values True/False
		 */
		bool Time_Expired( uint16_t start_time, uint16_t duration )
		{
			return ( ( m_tc->CNT - start_time ) >= duration );
		}

		/**
		 * @brief  This function is used to enable the interrupt
		 */
		inline void Enable_Int( void )
		{
			uC_Interrupt::Enable_Int( m_tmr_ctrl->irq_num );
		}

		/**
		 * @brief  This function is used to disable the interrupt
		 */
		inline void Disable_Int( void )
		{
			uC_Interrupt::Disable_Int( m_tmr_ctrl->irq_num );
		}

		/**
		 * @brief  This function is used to acknowledge the occurence of an interrupt
		 */
		inline void Ack_Int( void )
		{
			m_tc->SR = 0;
			Nop();
			Nop();
			Nop();
			Nop();
		}

	private:

		void Int_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Int_Handler_Static( Advanced_Vect::cback_param_t param )
		{
			( ( uC_Timers* )param )->Int_Handler();
		}

		void Raw_Int_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Raw_Int_Handler_Static( Advanced_Vect::cback_param_t param )
		{
			( ( uC_Timers* )param )->Raw_Int_Handler();
		}

		Advanced_Vect* m_advanced_vect;
		cback_param_t m_cback_param;
		cback_func_t m_cback_func;

		TIM_TypeDef* m_tc;
		uC_BASE_TIMER_IO_STRUCT const* m_tmr_ctrl;
		uint32_t m_tclock_freq;
};

#endif
