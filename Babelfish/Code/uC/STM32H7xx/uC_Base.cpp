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

		case RCC_PERIPH_CLCK_AHB4:
			periph_reg = &RCC->AHB4RSTR;
			break;

		case RCC_PERIPH_CLCK_APB1L:
			periph_reg = &RCC->APB1LRSTR;
			break;

		case RCC_PERIPH_CLCK_APB1H:
			periph_reg = &RCC->APB1HRSTR;
			break;

		case RCC_PERIPH_CLCK_APB2:
			periph_reg = &RCC->APB2RSTR;
			break;

		case RCC_PERIPH_CLCK_APB3:
			periph_reg = &RCC->APB3RSTR;
			break;

		case RCC_PERIPH_CLCK_APB4:
			periph_reg = &RCC->APB4RSTR;
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

		case RCC_PERIPH_CLCK_AHB4:
			periph_reg = &RCC->AHB4ENR;
			break;

		case RCC_PERIPH_CLCK_APB1L:
			periph_reg = &RCC->APB1LENR;
			break;

		case RCC_PERIPH_CLCK_APB1H:
			periph_reg = &RCC->APB1HENR;
			break;

		case RCC_PERIPH_CLCK_APB2:
			periph_reg = &RCC->APB2ENR;
			break;

		case RCC_PERIPH_CLCK_APB3:
			periph_reg = &RCC->APB3ENR;
			break;

		case RCC_PERIPH_CLCK_APB4:
			periph_reg = &RCC->APB4ENR;
			break;

		default:
			break;
	}

	return ( periph_reg );
}
