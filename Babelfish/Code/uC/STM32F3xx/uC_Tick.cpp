/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Tick.h"
#include "Device_Config.h"



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

	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 5-2-4
	 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
	 * casts shall not be used
	 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style
	 * casts are required for all headers used
	 * @n in the HAL. The note is re-enabled at the end of this file
	 */
	/*lint -e{1924} */
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 5-0-4
	 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral
	 * conversions shall not change the signedness of the underlying type
	 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used
	 * as is The note is re-enabled at the end of this file
	 */
	/*lint -e{1960}  */
	SysTick->LOAD =
		( static_cast<uint32_t>( SYSTICK_CLOCK_FREQ ) / tick_frequency_in_hertz ) - 1U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Tick::Start_Tick( void )
{
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 5-2-4
	 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
	 * casts shall not be used
	 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style
	 * casts are required for all headers used
	 * @n in the HAL. The note is re-enabled at the end of this file
	 */
	/*lint -e{1924} */
	SysTick->CTRL |= static_cast<uint32_t>( SysTick_CTRL_TICKINT_Msk );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 5-2-4
	 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
	 * casts shall not be used
	 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style
	 * casts are required for all headers used
	 * @n in the HAL. The note is re-enabled at the end of this file
	 */
	/*lint -e{1924} */
	SysTick->CTRL |= static_cast<uint32_t>( SysTick_CTRL_ENABLE_Msk );
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
