/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
 * casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts
 * are required for all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

#include "Includes.h"
#include "uC_OS_Tick.h"
#include "OS_Tasker.h"
#include "uC_Interrupt.h"
#include "stm32mp1xx_hal.h"

/*
 *****************************************************************************
 *		Prototypes
 *****************************************************************************
 */
#define  TICK_INT_PRIORITY            0x0FU	/*!< tick interrupt priority */
/*
 *****************************************************************************
 *		Static Variables
 *****************************************************************************
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

	SysTick->LOAD =
		( static_cast<uint32_t>( CONFIG_CPU_CLOCK_HZ ) / OS_TICK_FREQUENCY_HERTZ ) - 1U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_OS_Tick::Start_OS_Tick( void )
{

	if ( IS_ENGINEERING_BOOT_MODE() )
	{
		// TODO: According to "https://wiki.st.com/stm32mpu/wiki/STM32CubeMP1_development_guidelines"
		// In engineering mode, the SysTick clock is not configured and we need to use a separate Timer
		// for tick event. Need to replace the HAL_InitTick call with our uC_timers implementation.

		/* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
		HAL_InitTick( TICK_INT_PRIORITY );
	}
	else
	{
		// TODO: Need to check if we really need to configure systick in production mode.

		SysTick->CTRL |= static_cast<uint32_t>( SysTick_CTRL_TICKINT_Msk );
		SysTick->CTRL |= static_cast<uint32_t>( SysTick_CTRL_ENABLE_Msk );

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_OS_Tick::OS_Tick_Handler( void )
{
	SCB->ICSR |= static_cast<uint32_t>( SCB_ICSR_PENDSVSET_Msk );

	asmSetInterruptMask();
	{
		m_tick_handler();
	}
	asmClearInterruptMask();
}

/*lint +e1924 */
