/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module provides the ability to create multiple timers on one HW timer.
 *
 *	@details Using one hardware timer with a single time base we can use the compare
 *  behavior to create up to X timers.  The callbacks are all from an interrupt so
 *  care must be taken as to the time taken inside this interrupt.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_Capture_Input_H
   #define uC_Capture_Input_H

#include "Includes.h"
#include "uC_Interrupt.h"
#include "uC_Timers_HW.h"
#include "Input.h"
#include "stm32f0xx_hal_tim.h"
// #include "stm32f4xx_hal_tim.h"

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
class uC_Capture_Input : uC_Timers_HW
{
	public:
		uC_Capture_Input( uint8_t hw_timer_num, uint8_t num_Of_Channels,
						  uC_PERIPH_TIM_CAPTURE_STRUCT const* Channel_Config,
						  uC_PERIPH_TIM_CONFIG_STRUCT* Timer_Config,
						  uint8_t int_priority );
		~uC_Capture_Input();

		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );
		void Set_Callback( uint8_t timer_num, cback_func_t func_callback,
						   cback_param_t func_param = NULL );

		void Start_Input_Capture( void );

		void Stop_Input_Capture( void );

		void Deinit_Input_Capture( uC_BASE_TIMER_IO_STRUCT* htim );

		// void	Start_Input_Capture( TIM_HandleTypeDef *htim, uint32_t Channel );
		// void  Stop_Input_Capture( TIM_HandleTypeDef *htim, uint32_t Channel );
		uC_BASE_TIMER_IO_STRUCT const* m_tmr_ctrl;

	private:
		void Int_Handler( void );

		void TIMx_Config( uC_BASE_TIMER_IO_STRUCT* TIMx,
						  uC_PERIPH_TIM_CONFIG_STRUCT* TIM_TimeBaseInitStruct );

		void TI1_Config( uC_BASE_TIMER_IO_STRUCT* TIMx, uC_PERIPH_TIM_CAPTURE_STRUCT* Channel1 );

		void TI2_Config( uC_BASE_TIMER_IO_STRUCT* TIMx, uC_PERIPH_TIM_CAPTURE_STRUCT* Channel2 );

		void TI3_Config( uC_BASE_TIMER_IO_STRUCT* TIMx, uC_PERIPH_TIM_CAPTURE_STRUCT* Channel3 );

		void TI4_Config( uC_BASE_TIMER_IO_STRUCT* TIMx, uC_PERIPH_TIM_CAPTURE_STRUCT* Channel4 );


		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Int_Handler_Static( uC_Timers_HW::cback_param_t param )
		{ ( ( uC_Capture_Input* )param )->Int_Handler(); }

		static void Dummy_Static( uC_Timers_HW::cback_param_t param )   {}

		void Init_Input_Capture( TIM_HandleTypeDef* htim );

		void Deinit_Input_Capture( TIM_HandleTypeDef* htim );

		struct timer_def_t
		{
			cback_param_t cback_param;
			cback_func_t cback_func;
			TICK_TD timer_counts;
			bool repeat;
		};

		timer_def_t* m_timer_defs;
		// uC_BASE_TIMER_IO_STRUCT const* m_tmr_ctrl;
		uC_PERIPH_TIM_CAPTURE_STRUCT const* m_chn_ctrl;
		uint8_t m_num_timers_Chn_Avl;

};



#endif
