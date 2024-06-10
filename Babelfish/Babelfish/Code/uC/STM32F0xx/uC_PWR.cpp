/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_PWR.h"
#include "uC_RTC.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_rtc.h"


/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
/* ------------------ PWR registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_DS_MASK               ( ( uint32_t )0xFFFFFFFC )
#define CR_PLS_MASK              ( ( uint32_t )0xFFFFFF1F )
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_PWR::uC_PWR() :
	m_cback_func( nullptr ),
	m_cback_param( nullptr )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWR::Enter_PowerDown_SleepMode( uint8_t power_sleep_entry )
{
	( *m_cback_func )( m_cback_param );
	/* Clear SLEEPDEEP bit of Cortex-M0 System Control Register */
	SCB->SCR &= ( uint32_t ) ~( ( uint32_t )SCB_SCR_SLEEPDEEP_Msk );

	if ( power_sleep_entry == WAIT_FOR_INTERRUPT )
	{
		/* Request Wait For Interrupt */
		__WFI();
	}
	else if ( power_sleep_entry == WAIT_FOR_EVENT )
	{
		/* Request Wait For Event */
		__WFE();
	}
	else
	{
		/* Do Nothing */
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWR::Enter_PowerDown_StopMode( uint32_t PWR_Regulator, uint8_t power_sleep_entry )
{
	uint32_t tmpreg = 0;

	/* Select the regulator state in STOP mode ---------------------------------*/
	tmpreg = PWR->CR;

	/* Clear PDDS and LPDSR bits */
	tmpreg &= CR_DS_MASK;

	/* Set LPDSR bit according to PWR_Regulator value */
	tmpreg |= PWR_Regulator;

	/* Store the new value */
	PWR->CR = tmpreg;

	/* Set SLEEPDEEP bit of Cortex-M0 System Control Register */
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	if ( power_sleep_entry == WAIT_FOR_INTERRUPT )
	{
		/* Request Wait For Interrupt */
		__WFI();
	}
	else if ( power_sleep_entry == WAIT_FOR_EVENT )
	{
		/* Request Wait For Event */
		__WFE();
	}
	else
	{
		/* Do Nothing */
	}
	/* Reset SLEEPDEEP bit of Cortex System Control Register */
	SCB->SCR &= ( uint32_t ) ~( ( uint32_t )SCB_SCR_SLEEPDEEP_Msk );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWR::Configure_Wakeup_Pin( uint32_t PWR_WakeUpPin, uint8_t pinState )
{

	if ( pinState != DISABLE )
	{
		/* Enable the EWUPx pin */
		PWR->CSR |= PWR_WakeUpPin;
	}
	else
	{
		/* Disable the EWUPx pin */
		PWR->CSR &= ~PWR_WakeUpPin;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWR::Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
{
	m_cback_param = func_param;
	m_cback_func = func_callback;
}
