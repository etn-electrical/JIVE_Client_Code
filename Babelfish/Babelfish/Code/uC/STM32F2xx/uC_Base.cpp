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
#include "uC_Base.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uC_Base* uC_Base::m_myself = reinterpret_cast<uC_Base*>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Base::uC_Base( void )
{
	m_myself = this;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base::Enable_Periph_Clock( PERIPH_DEF_ST const* peripheral )
{
	volatile uint32_t* periph_reg;

	periph_reg = Get_Periph_Clk_Enable_Reg( peripheral );
	if ( periph_reg != NULL )
	{
		*periph_reg |= peripheral->pid;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base::Disable_Periph_Clock( PERIPH_DEF_ST const* peripheral )
{
	volatile uint32_t* periph_reg;

	periph_reg = Get_Periph_Clk_Enable_Reg( peripheral );
	if ( periph_reg != NULL )
	{
		*periph_reg &= ~peripheral->pid;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Base::Is_Periph_Clock_Enabled( PERIPH_DEF_ST const* peripheral )
{
	volatile uint32_t* periph_reg;
	bool enabled;

	periph_reg = Get_Periph_Clk_Enable_Reg( peripheral );
	if ( periph_reg != NULL )
	{
		enabled = ( *periph_reg & peripheral->pid ) != 0U;
	}
	else
	{
		enabled = false;
	}

	return ( enabled );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base::Reset_Periph( PERIPH_DEF_ST const* peripheral )
{
	volatile uint32_t* periph_reg = reinterpret_cast<uint32_t*>( nullptr );

	switch ( peripheral->pclock_reg_index )
	{
		case RCC_PERIPH_CLCK_AHB1:
			periph_reg = &RCC->AHB1RSTR;
			break;

		case RCC_PERIPH_CLCK_AHB2:
			periph_reg = &RCC->AHB2RSTR;
			break;

		case RCC_PERIPH_CLCK_AHB3:
			periph_reg = &RCC->AHB3RSTR;
			break;

		case RCC_PERIPH_CLCK_APB1:
			periph_reg = &RCC->APB1RSTR;
			break;

		case RCC_PERIPH_CLCK_APB2:
			periph_reg = &RCC->APB2RSTR;
			break;

		default:
			periph_reg = reinterpret_cast<uint32_t*>( nullptr );
			break;
	}
	if ( periph_reg != NULL )
	{
		*periph_reg |= peripheral->pid;
		*periph_reg &= ~peripheral->pid;

	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
volatile uint32_t* uC_Base::Get_Periph_Clk_Enable_Reg( PERIPH_DEF_ST const* peripheral )
{
	volatile uint32_t* periph_reg = reinterpret_cast<uint32_t*>( nullptr );

	switch ( peripheral->pclock_reg_index )
	{
		case RCC_PERIPH_CLCK_AHB1:
			periph_reg = &RCC->AHB1ENR;
			break;

		case RCC_PERIPH_CLCK_AHB2:
			periph_reg = &RCC->AHB2ENR;
			break;

		case RCC_PERIPH_CLCK_AHB3:
			periph_reg = &RCC->AHB3ENR;
			break;

		case RCC_PERIPH_CLCK_APB1:
			periph_reg = &RCC->APB1ENR;
			break;

		case RCC_PERIPH_CLCK_APB2:
			periph_reg = &RCC->APB2ENR;
			break;

		default:
			break;
	}

	return ( periph_reg );
}

/* lint +e1924
  lint +e1960*/
