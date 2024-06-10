/**
 *****************************************************************************************
 * @file		uC_Tick.cpp
 * @details		See header file for module overview.
 * @copyright	2019 Eaton Corporation All Rights Reserved.
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Tick.h"
#include "freertos/portable.h"
#include "freertos/task.h"
#include "esp32/clk.h"
#include "freertos/xtensa_rtos.h"

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
uC_TICK_CB uC_Tick::m_tick_cb = reinterpret_cast<uC_TICK_CB>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Tick::uC_Tick( uC_TICK_CB tick_cback,
				  uint32_t tick_frequency_in_hertz, uint8_t tick_interrupt_priority )
{
	m_tick_cb = tick_cback;

	uC_Interrupt::Set_Vector( &Tick_Handler, SysTick_IRQn, tick_interrupt_priority );

	_xt_tick_divisor_init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Tick::Start_Tick( void )
{
	xPortStartScheduler();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Tick::Tick_Handler( void )
{
	( m_tick_cb )( );
}
