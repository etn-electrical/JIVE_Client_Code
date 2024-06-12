/**
 *****************************************************************************************
 * @file		uC_OS_Tick.cpp
 * @details		See header file for module overview.
 * @copyright	2019 Eaton Corporation All Rights Reserved.
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_OS_Tick.h"
#include "OS_Tasker.h"
#include "uC_Interrupt.h"
#include "freertos/portable.h"
#include "freertos/task.h"
#include "esp32/clk.h"
#include "freertos/xtensa_rtos.h"

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
uC_OS_Tick::CALLBACK_FUNC* uC_OS_Tick::m_tick_handler = reinterpret_cast<CALLBACK_FUNC*>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_OS_Tick::uC_OS_Tick( CALLBACK_FUNC* tick_handler )
{
	m_tick_handler = tick_handler;

	uC_Interrupt::Set_Vector( &OS_Tick_Handler, SysTick_IRQn, uC_INT_HW_PRIORITY_LOWEST );

	_xt_tick_divisor_init();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_OS_Tick::Start_OS_Tick( void )
{
	xPortStartScheduler();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_OS_Tick::OS_Tick_Handler( void )
{
	BaseType_t ret;

	portbenchmarkIntLatency();
	traceISR_ENTER( SYSTICK_INTR_ID );
	ret = xTaskIncrementTick();
	if ( ret != pdFALSE )
	{
		portYIELD_FROM_ISR();
	}
	else
	{
		traceISR_EXIT();
	}
}
