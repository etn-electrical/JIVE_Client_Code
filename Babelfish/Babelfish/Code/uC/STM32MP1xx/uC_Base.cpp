/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
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
	volatile uint32_t* periph_reg1 = nullptr;
	volatile uint32_t* periph_reg2 = nullptr;

	switch ( peripheral->pclock_reg_index )
	{
		case RCC_PERIPH_CLCK_AHB2:
			periph_reg1 = &RCC->AHB2RSTSETR;
			periph_reg2 = &RCC->AHB2RSTCLRR;
			break;

		case RCC_PERIPH_CLCK_AHB3:
			periph_reg1 = &RCC->AHB3RSTSETR;
			periph_reg2 = &RCC->AHB3RSTCLRR;

			break;

		case RCC_PERIPH_CLCK_AHB4:
			periph_reg1 = &RCC->AHB4RSTSETR;
			periph_reg2 = &RCC->AHB4RSTCLRR;
			break;

		case RCC_PERIPH_CLCK_APB1:
			periph_reg1 = &RCC->APB1RSTSETR;
			periph_reg2 = &RCC->APB1RSTCLRR;
			break;

		case RCC_PERIPH_CLCK_APB2:
			periph_reg1 = &RCC->APB2RSTSETR;
			periph_reg2 = &RCC->APB2RSTCLRR;
			break;

		case RCC_PERIPH_CLCK_APB3:
			periph_reg1 = &RCC->APB3RSTSETR;
			periph_reg2 = &RCC->APB3RSTCLRR;
			break;

		default:
			periph_reg1 = nullptr;
			periph_reg2 = nullptr;
			break;
	}
	if ( periph_reg1 != NULL )
	{
		*periph_reg1 |= peripheral->pid;
		*periph_reg1 &= ~peripheral->pid;

		*periph_reg2 |= peripheral->pid;
		*periph_reg2 &= ~peripheral->pid;
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
		case RCC_PERIPH_CLCK_AHB2:
			periph_reg = &RCC->MC_AHB2ENSETR;
			break;

		case RCC_PERIPH_CLCK_AHB3:
			periph_reg = &RCC->MC_AHB3ENSETR;
			break;

		case RCC_PERIPH_CLCK_AHB4:
			periph_reg = &RCC->MC_AHB4ENSETR;
			break;

		case RCC_PERIPH_CLCK_APB1:
			periph_reg = &RCC->MC_APB1ENSETR;
			break;

		case RCC_PERIPH_CLCK_APB2:
			periph_reg = &RCC->MC_APB2ENSETR;
			break;

		case RCC_PERIPH_CLCK_APB3:
			periph_reg = &RCC->MC_APB3ENSETR;
			break;

		default:
			break;
	}

	return ( periph_reg );
}
