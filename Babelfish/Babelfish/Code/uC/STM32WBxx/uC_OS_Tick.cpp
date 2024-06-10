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

/*
 *****************************************************************************
 *		Prototypes
 *****************************************************************************
 */

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
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
	SysTick->CTRL |= static_cast<uint32_t>( SysTick_CTRL_TICKINT_Msk );
	SysTick->CTRL |= static_cast<uint32_t>( SysTick_CTRL_ENABLE_Msk );
	SysTick->CTRL |= static_cast<uint32_t>( SysTick_CTRL_CLKSOURCE_Msk );
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_OS_Tick::Suspend_OS_Tick( void )
{
	Push_TGINT();
	SysTick->CTRL &= ~static_cast<uint32_t>( SysTick_CTRL_TICKINT_Msk );
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_OS_Tick::Resume_OS_Tick( void )
{
	Push_TGINT();
	SysTick->CTRL |= static_cast<uint32_t>( SysTick_CTRL_TICKINT_Msk );
	Pop_TGINT();
}
