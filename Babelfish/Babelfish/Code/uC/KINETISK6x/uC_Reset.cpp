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
	UINT8 reset_id_0 = 0;
	UINT8 reset_id_1 = 0;

	/* The register settings for Reset module are different from other k6x family controllers */
#if defined ( uC_KINETISK60DNx_USAGE ) || ( uC_KINETISK66x_USAGE )	// For MK60D10 and MK66F18 target
	reset_id_0 = RCM->SRS0;	// Get the Reset Cause Register 0.
	reset_id_1 = RCM->SRS1;	// Get the Reset Cause Register 1.

	// for k60 POR, both the POR and the LVD bits are set in RCR0
	if ( ( reset_id_0 & ( RCM_SRS0_POR_MASK | RCM_SRS0_LVD_MASK ) ) == ( RCM_SRS0_POR_MASK | RCM_SRS0_LVD_MASK ) )
	{
		Set_Bit( m_reset_id, RESET_POWER_UP_TRIGRD );
	}

	if ( ( reset_id_0 & RCM_SRS0_WDOG_MASK ) != 0 )
	{
		Set_Bit( m_reset_id, RESET_WATCHDOG_TRIGRD );
	}

	if ( ( reset_id_1 & RCM_SRS1_SW_MASK ) != 0 )
	{
		Set_Bit( m_reset_id, RESET_SOFTWARE_TRIGRD );
	}

	if ( ( reset_id_0 & RCM_SRS0_LVD_MASK ) != 0 )
	{
		Set_Bit( m_reset_id, RESET_BROWN_OUT_TRIGRD );
	}

	// The pin reset seems to be triggered by any reset.
	// So we mask it off if another reset was the cause.
	if ( ( ( reset_id_0 & RCM_SRS0_PIN_MASK ) != 0 ) &&
		 ( m_reset_id == 0 ) )
	{
		Set_Bit( m_reset_id, RESET_USER_PIN_TRIGRD );
	}

	if ( m_reset_id == 0U )
	{
		Set_Bit( m_reset_id, static_cast<uint8_t>( RESET_UNDEFINED ) );
	}
#else
	reset_id_0 = MC->SRSL;	// Get the Reset Cause Register 0.
	reset_id_1 = MC->SRSH;	// Get the Reset Cause Register 1.

	// for k60 POR, both the POR and the LVD bits are set in RCR0
	if ( ( reset_id_0 & ( MC_SRSL_POR_MASK | MC_SRSL_LVD_MASK ) ) == ( MC_SRSL_POR_MASK | MC_SRSL_LVD_MASK ) )
	{
		Set_Bit( m_reset_id, RESET_POWER_UP_TRIGRD );
	}

	if ( ( reset_id_0 & MC_SRSL_COP_MASK ) != 0 )
	{
		Set_Bit( m_reset_id, RESET_WATCHDOG_TRIGRD );
	}

	if ( ( reset_id_1 & MC_SRSH_SW_MASK ) != 0 )
	{
		Set_Bit( m_reset_id, RESET_SOFTWARE_TRIGRD );
	}

	if ( ( reset_id_0 & MC_SRSL_LVD_MASK ) != 0 )
	{
		Set_Bit( m_reset_id, RESET_BROWN_OUT_TRIGRD );
	}

	// The pin reset seems to be triggered by any reset.
	// So we mask it off if another reset was the cause.
	if ( ( ( reset_id_0 & MC_SRSL_PIN_MASK ) != 0 ) &&
		 ( m_reset_id == 0 ) )
	{
		Set_Bit( m_reset_id, RESET_USER_PIN_TRIGRD );
	}

	if ( m_reset_id == 0U )
	{
		Set_Bit( m_reset_id, static_cast<uint8_t>( RESET_UNDEFINED ) );
	}
#endif
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
	BF_ASSERT( false );
}

/* lint +e1924
  lint +e1960*/
