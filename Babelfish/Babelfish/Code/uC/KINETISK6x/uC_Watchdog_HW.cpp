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
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 * all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 * signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 * re-enabled at the end of this file
 */
/*lint -e1960  */
#include "Includes.h"
#include "uC_Watchdog_HW.h"
#include "uC_Reset.h"

// We are going to use the independent watchdog.
// it runs off of a 1kHz clock.
#define WDOG_BASE_CLOCK_FREQ        1000
#define MAX_WDOG_COUNT_VALUE        0xFFFFFFFF

#define WDOG_UNLOCK_KEY1 ( UINT16 )0xC520
#define WDOG_UNLOCK_KEY2 ( UINT16 )0xD928
#define WDOG_REFRESH_KEY1 ( UINT16 )0xA602
#define WDOG_REFRESH_KEY2 ( UINT16 )0xB480

// #define DBGMCU_IWDG_STOP   (0x00000100U)

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
bool uC_Watchdog_HW::m_wd_reset_occurred = FALSE;

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
				( static_cast<uint32_t>( WDOG_BASE_CLOCK_FREQ ) >> m_prescalers[prescaler_index] );
			test_count = test_count / 1000U;	// Because we are in milliseconds.
			prescaler_index++;
		} while ( ( prescaler_index < m_prescalers_max ) &&
				  ( test_count >= MAX_WDOG_COUNT_VALUE ) );

		prescaler_index--;

#ifndef DEBUG_DISABLE_WDOG
		// Unlock the watchdog
		WDOG->UNLOCK = WDOG_UNLOCK_KEY1;
		WDOG->UNLOCK = WDOG_UNLOCK_KEY2;
		// setup the watchdog
		WDOG->PRESC = WDOG_PRESC_PRESCVAL( prescaler_index - 1 );	// prescaler is actually prescale value + 1
		WDOG->TOVALH = ( UINT16 )( ( test_count & 0xFFFF0000 ) >> 16 );
		WDOG->TOVALL = ( UINT16 )( test_count & 0x0000FFFF );
		/* WDOG_STCTRLH:
		   ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,??=0,WAITEN=0,STOPEN=0,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=1,CLKSRC=0,WDOGEN=1
		 */
		WDOG->STCTRLH = ( UINT16 )0x0015;
#endif

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Watchdog_HW::Watchdog_Reset_Occurred( void )
{
	UINT16 previous_wd_reset;

	previous_wd_reset = uC_Reset::Get_Reset_Trigger();
	if ( Test_Bit( previous_wd_reset, RESET_WATCHDOG_TRIGRD ) )
	{
		m_wd_reset_occurred = TRUE;
	}

	return ( m_wd_reset_occurred );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog_HW::Kick_Dog( void )
{
#ifndef DEBUG_DISABLE_WDOG
	// only try to kick the dog if it has ticked
	if ( WDOG->TMROUTL >= 5 )
	{
		/*The time differnece between first refresh value written and second refresh value
		   should be less than 20 bus cycles so disable interrupts during this action
		 */
		Push_TGINT();
		WDOG->REFRESH = WDOG_REFRESH_KEY1;
		WDOG->REFRESH = WDOG_REFRESH_KEY2;
		Pop_TGINT();
		// while ( WDOG->TMROUTL >= 2) {};
	}

#endif
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
