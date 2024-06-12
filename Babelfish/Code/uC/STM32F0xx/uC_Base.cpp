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
	volatile uint32_t* periph_reg = reinterpret_cast<uint32_t*>( nullptr );

	switch ( peripheral->pclock_reg_index )
	{
		case RCC_PERIPH_CLCK_AHB:
			periph_reg = &RCC->AHBENR;
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
	volatile uint32_t* periph_reg = reinterpret_cast<uint32_t*>( nullptr );

	switch ( peripheral->pclock_reg_index )
	{
		case RCC_PERIPH_CLCK_AHB:
			periph_reg = &RCC->AHBENR;
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
	volatile uint32_t* periph_reg = reinterpret_cast<uint32_t*>( nullptr );
	bool enabled;

	switch ( peripheral->pclock_reg_index )
	{
		case RCC_PERIPH_CLCK_AHB:
			periph_reg = &RCC->AHBENR;
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
		case RCC_PERIPH_CLCK_AHB:
			periph_reg = reinterpret_cast<uint32_t*>( nullptr );
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
