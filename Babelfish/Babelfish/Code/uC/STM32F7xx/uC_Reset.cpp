/**
 **********************************************************************************************
 *  @file           uC_Reset.cpp Implementation File for microcontroller reset functionality
 *
 *  @brief          The file shall wrap all the functions which are required for the
 *                  resetting the microcontroller
 *  @details
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 *all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 *signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 *re-enabled at the end of this file
 */
/*lint -e1960  */

#include "Includes.h"
#include "uC_Reset.h"
#include "Exception.h"
#include "Babelfish_Assert.h"

/**
 * @brief This variable hold the status of last reset.
 *        The variable is of static type and the reset status is thus available for reading
 *        purpose to each instance of this class.
 */
uC_RESET_CAUSE_TD uC_Reset::m_reset_id;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Reset::uC_Reset( void )
{
	uint32_t reset_id;

	reset_id = RCC->CSR;	// Get the Reset Cause Register.
	RCC->CSR |= RCC_CSR_RMVF;

	if ( ( reset_id & RCC_CSR_PORRSTF ) != 0U )
	{
		Set_Bit( m_reset_id, RESET_POWER_UP_TRIGRD );
	}

	if ( ( ( reset_id & RCC_CSR_WWDGRSTF ) != 0U ) ||
		 ( ( reset_id & RCC_CSR_IWDGRSTF ) != 0U ) )
	{
		Set_Bit( m_reset_id, RESET_WATCHDOG_TRIGRD );
	}

	if ( ( reset_id & RCC_CSR_SFTRSTF ) != 0U )
	{
		Set_Bit( m_reset_id, RESET_SOFTWARE_TRIGRD );
	}

	if ( ( reset_id & RCC_CSR_LPWRRSTF ) != 0U )
	{
		Set_Bit( m_reset_id, RESET_BROWN_OUT_TRIGRD );
	}

	// The pin reset seems to be triggered by any reset.
	// So we mask it off if another reset was the cause.
	if ( ( ( reset_id & RCC_CSR_PINRSTF ) != 0U ) &&
		 ( m_reset_id == 0U ) )
	{
		Set_Bit( m_reset_id, RESET_USER_PIN_TRIGRD );
	}

	if ( m_reset_id == 0U )
	{
		Set_Bit( m_reset_id, static_cast<uint8_t>( RESET_UNDEFINED ) );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Reset::Soft_Reset( void )
{
	Disable_Interrupts();

	NVIC_SystemReset();		// Cortex standard library function.
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Reset::Watchdog_Reset( void )
{
	Disable_Interrupts();

	// We will spin here until the watchdog resets us.  If it doesn't we have problems.
	while ( true )
	{}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_RESET_CAUSE_TD uC_Reset::Get_Reset_Trigger( void )
{
	return ( m_reset_id );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Reset::Periph_Reset( void )
{
	BF_ASSERT(false);
}

/* lint +e1924
  lint +e1960*/
