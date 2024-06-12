/**
 *****************************************************************************************
 *	@file   uC_Capture_Input.h
 *
 *	@brief  Timer based Input Capture
 *
 *	@details Input Capture is a part of the Timer Peripheral and is used read a digital
 *  signal applied to a pin of the MCU measure the LOW and HIGH transitions.
 *  The Input Capture feature in application related to frequency and pulse measurement.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef UC_CAPTURE_INPUT_H
   #define UC_CAPTURE_INPUT_H

#include "Includes.h"
#include "uC_Interrupt.h"
#include "uC_Timers_HW.h"
#include "Input.h"

/**
 ****************************************************************************************
 * @brief This module provides the ability to Capture input signal using HW timer.
 *
 * @details Input Capture is a part of the Timer Peripheral and is used read a digital
 * signal applied to a pin of the MCU measure the LOW and HIGH transitions. *
 ****************************************************************************************
 */
class uC_Capture_Input : uC_Timers_HW
{
	public:
		/**
		 * @brief            Constructor to create an instance of uC_Capture_Input class.
		 * @param1[in] hw_timer_num:  The timer number of microcontroller which is to be initialized
		 *                          for timing purpose
		 * @param2[in] num_of_channels: Number of channels
		 * @param3[in] Channel_Config:  Configure Input channel of Timer Structure
		 * @param4[in] Timer_Config:  Initialize Timer Base Structure
		 * @param5[in] int_priority:  To set interrupt priority
		 * @note             The constructor is used to configure the timer input capture.
		 */
		uC_Capture_Input( uint8_t hw_timer_num, uint8_t num_of_channels,
						  uC_PERIPH_TIM_CAPTURE_STRUCT const* channel_config,
						  uC_PERIPH_TIM_CONFIG_STRUCT* timer_config,
						  uint8_t int_priority );
		/**
		 * @brief            Destructor to delete the uC_Capture_Input instance when it goes out of scope
		 */
		~uC_Capture_Input();

		/**
		 * @brief Type definition of callback function pointer.
		 */
		typedef void* cback_param_t;

		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 *@brief            Set_Callback function
		 *@details          This function is used to set callback configuration
		 *@param1[in] timer_num:  set timer for callback
		 *@param2[in] func_callback:  callback function
		 *@param3[in] func_param:  callbcack function params
		 */
		void Set_Callback( uint8_t timer_num, cback_func_t func_callback,
						   cback_param_t func_param = nullptr );

		/**
		 *@brief            Initialize input capture
		 *@details          This function is used to enable counter mode for to start input capture
		 */
		void Start_Input_Capture( void );

		/**
		 *@brief            Deinitialize input capture
		 *@details          This function is used to disable counter mode for to stop input capture
		 */
		void Stop_Input_Capture( void );

	private:
		/**
		 *@brief            Interrupt handler
		 *@details          This function is used to set the int handler
		 */
		void Int_Handler( void );

		/**
		 *@brief            This function is used to Configure the Timer in capture mode.
		 *@param1[in] TIMx: Timer Block Definition Structure
		 *@param2[in] TIM_TimeBaseInitStruct: Initialize Timer Base Structure
		 */
		void TIMx_Config( uC_BASE_TIMER_IO_STRUCT* TIMx,
						  uC_PERIPH_TIM_CONFIG_STRUCT* TIM_TimeBaseInitStruct );

		/**
		 *@brief            This function is used to Configure Input channel1 of Timer in capture mode.
		 *@param1[in] TIMx: Timer Block Definition Structure
		 *@param2[in] channel1: Configure Input channel of Timer Structure
		 */
		void TI1_Config( uC_BASE_TIMER_IO_STRUCT* TIMx, uC_PERIPH_TIM_CAPTURE_STRUCT* channel1 );

		/**
		 *@brief            This function is used to Configure Input channel2 of Timer in capture mode.
		 *@param1[in] TIMx: Timer Block Definition Structure
		 *@param2[in] channel2: Configure Input channel of Timer Structure
		 */
		void TI2_Config( uC_BASE_TIMER_IO_STRUCT* TIMx, uC_PERIPH_TIM_CAPTURE_STRUCT* channel2 );

		/**
		 *@brief            This function is used to Configure Input channel3 of Timer in capture mode.
		 *@param1[in] TIMx: Timer Block Definition Structure
		 *@param2[in] channel3: Configure Input channel of Timer Structure
		 */
		void TI3_Config( uC_BASE_TIMER_IO_STRUCT* TIMx, uC_PERIPH_TIM_CAPTURE_STRUCT* channel3 );

		/**
		 *@brief            This function is used to Configure Input channel4 of Timer in capture mode.
		 *@param1[in] TIMx: Timer Block Definition Structure
		 *@param2[in] channel4: Configure Input channel of Timer Structure
		 */
		void TI4_Config( uC_BASE_TIMER_IO_STRUCT* TIMx, uC_PERIPH_TIM_CAPTURE_STRUCT* channel4 );

		/**
		 *@brief            Interrupt handler
		 *@details          This function is used to set the int handler
		 */
		static void Int_Handler_Static( uC_Timers_HW::cback_param_t param )
		{ ( ( uC_Capture_Input* )param )->Int_Handler(); }

		static void Dummy_Static( uC_Timers_HW::cback_param_t param )   {}

		/**
		 *@brief            Structure timer_def_t
		 *@details          Structure timer_def_t used to configure timer parameters
		 */
		struct timer_def_t
		{
			cback_param_t cback_param;
			cback_func_t cback_func;
			TICK_TD timer_counts;
			bool repeat;
		};

		/**
		 *@brief            Timer definition structure
		 */
		timer_def_t* m_timer_defs;

		/**
		 *@brief            Accessing structure for timer
		 */
		uC_BASE_TIMER_IO_STRUCT const* m_tmr_ctrl;

		/**
		 *@brief            Input capture timer channel structure
		 */
		uC_PERIPH_TIM_CAPTURE_STRUCT const* m_chn_ctrl;

		/**
		 * @brief variable to store length of num of timers Channel Available
		 */
		uint8_t m_num_timers_chn_avl;

		/**
		 * @brief static const varibles used to configure outut channel and input capture filter
		 */
		/// Input capture 1 filter position of Capture/compare mode register
		static const uint8_t INPUT_CAP1_FILTER_POS = 4U;

		/// Input capture 2 filter position of Capture/compare mode register
		static const uint8_t INPUT_CAP2_FILTER_POS = 12U;

		/// Input capture select position of Capture/compare mode register
		static const uint8_t INPUT_CAP_SEL_POS = 8U;

		/// output capture2 enable of Capture/compare enab;e register
		static const uint8_t CC2_OUT_EN = 4U;

		/// output capture3 enable of Capture/compare enab;e register
		static const uint8_t CC3_OUT_EN = 8U;

		/// output capture4 enable of Capture/compare enab;e register
		static const uint8_t CC4_OUT_EN = 12U;

};



#endif	// UC_CAPTURE_INPUT_H
