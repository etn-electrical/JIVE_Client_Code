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

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions
 * shall not change the signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as
 * is The note is re-enabled at the end of this file
 */
/*lint -e1960  */
#include "Includes.h"
#include "uC_Watchdog_HW.h"
#include "uC_Reset.h"

// We are going to use the independent watchdog.
// it runs off of a 40kHz clock.
#define IWDG_BASE_CLOCK_FREQ        40000U	/// Range is 30to60kHz.
#define MAX_IWDG_COUNT_VALUE        0xFFFU

#define IWDG_WRITE_ACCESS_ENABLE    0x5555U
#define IWDG_WRITE_ACCESS_DISABLE   0x0000U
#define IWDG_COUNT_RELOAD       0xAAAAU		// essentially the kick/feed.
#define IWDG_ENABLE_WDOG        0xCCCCU

#define DBGMCU_IWDG_STOP   ( 0x00000100U )

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
const uint8_t uC_Watchdog_HW::m_prescalers_max = 8U;
const uint8_t uC_Watchdog_HW::m_prescalers[m_prescalers_max] =
{
	DIV_BY_4, DIV_BY_8, DIV_BY_16, DIV_BY_32,
	DIV_BY_64, DIV_BY_128, DIV_BY_256, DIV_BY_256
};


const uC_BASE_WDOG_CTRL_STRUCT* uC_Watchdog_HW::m_wd_ctrl;

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog_HW::Set_Timeout( uint32_t timeout_in_ms )
{
	uint32_t test_count;
	volatile uint8_t prescaler_index;

	if ( timeout_in_ms != 0U )
	{
		prescaler_index = 0U;
		do
		{
			test_count = timeout_in_ms *
				( static_cast<uint32_t>
				  ( IWDG_BASE_CLOCK_FREQ ) >> m_prescalers[prescaler_index] );
			test_count = test_count / 1000U;	// Because we are in milliseconds.
			prescaler_index++;
		} while ( ( prescaler_index < m_prescalers_max ) &&
				  ( test_count > MAX_IWDG_COUNT_VALUE ) );

		prescaler_index--;

#ifndef DEBUG_DISABLE_WDOG
		IWDG->KR = IWDG_WRITE_ACCESS_ENABLE;	// open for write access.
		IWDG->PR = prescaler_index;
		IWDG->RLR = test_count;

		IWDG->KR = IWDG_COUNT_RELOAD;
		IWDG->KR = IWDG_ENABLE_WDOG;
#endif

#ifdef DEBUG
		DBGMCU->CR |= DBGMCU_IWDG_STOP;
#endif

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog_HW::Kick_Dog( void )
{
	IWDG->KR = IWDG_COUNT_RELOAD;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* uC_Watchdog_HW::operator new( size_t size )
{
	if ( size == static_cast<size_t>( 0U ) )
	{}
	return ( reinterpret_cast<void*>( nullptr ) );		// Ram::Allocate( size );
}

/* lint +e1924
  lint +e1960*/
