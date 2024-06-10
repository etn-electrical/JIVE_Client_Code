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
#include "stm32wbxx_hal_rcc.h"
#include "stm32wbxx_hal_rtc.h"
#include "uC_Single_LPTimer.h"
#include "SetSysClock.h"
#include "uC_OS_Tick.h"
#include "uC_Watchdog_HW.h"

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
 * Note: This function is not tested and is kept only for reference
 *****************************************************************************************
 */
void uC_PWR::Enter_PowerDown_SleepMode( uint8_t power_sleep_entry )
{
	// ( *m_cback_func )( m_cback_param );
	/// * Clear SLEEPDEEP bit of Cortex-M0 System Control Register */
	// SCB->SCR &= ( uint32_t ) ~( ( uint32_t )SCB_SCR_SLEEPDEEP_Msk );
	//
	// if ( power_sleep_entry == WAIT_FOR_INTERRUPT )
	// {
	/// * Request Wait For Interrupt */
	// __WFI();
	// }
	// else if ( power_sleep_entry == WAIT_FOR_EVENT )
	// {
	/// * Request Wait For Event */
	// __WFE();
	// }
	// else
	// {
	/// * Do Nothing */
	// }
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWR::Configure_StopMode_Level( uint32_t STOP_mode_level )
{
	/* Check the parameters */
	// assert_param(IS_PWR_STOP_ENTRY(STOPEntry));

	/* select the desired STOP mode */
	MODIFY_REG( PWR->CR1, PWR_CR1_LPMS, STOP_mode_level );

	/* Clear any pending EXTI */
	EXTI->PR1 = 0xFFFFFFFF;
	EXTI->PR2 = 0xFFFFFFFF;

	/* Clear any pending peripheral interrupts */
	// TODO verify if this can be removed
	for ( uint8_t intr_count = 0U; intr_count < 8U; intr_count++ )
	{
		NVIC->ICPR[intr_count] = 0xFFFFFFFF;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWR::Enter_PowerDown_StopMode( uint8_t power_sleep_entry )
{
	// Uncommenting this in release mode will impact on current consumption
	// Recommended to use in debug mode only
	// SET_BIT( DBGMCU->CR, DBGMCU_CR_DBG_STOP );

	/* Clear any pending peripheral interrupts */
	for ( uint8_t intr_count = 0U; intr_count < 8U; intr_count++ )
	{
		NVIC->ICPR[intr_count] = 0xFFFFFFFF;
	}

	/* Select Stop mode entry */
	if ( power_sleep_entry == PWR_STOPENTRY_WFI )
	{
		SET_BIT( SCB->SCR, ( ( uint32_t )SCB_SCR_SLEEPDEEP_Msk ) );
		// while ( ( PWR->EXTSCR & PWR_EXTSCR_C2DS ) == 0 );	// ideally this bit should be 1 before entering __WFI()
		/* Request Wait For Interrupt */
		__WFI();
	}
	else if ( power_sleep_entry == PWR_STOPENTRY_WFE )
	{
		/* Request Wait For Event */
		__SEV();
		__WFE();
		__WFE();
	}

	uC_Watchdog::Force_Feed_Dog();	// makes system safer

	/* Reset SLEEPDEEP bit of Cortex System Control Register */
	CLEAR_BIT( SCB->SCR, ( ( uint32_t )SCB_SCR_SLEEPDEEP_Msk ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWR::SYSCLKConfig_STOP( void )
{
	SetSysClock();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 * Note: This function is not tested and is kept only for reference
 *****************************************************************************************
 */
void uC_PWR::Configure_Wakeup_Pin( uint32_t PWR_WakeUpPin, uint8_t pinState )
{

	// if ( pinState != DISABLE )
	// {
	/// * Enable the EWUPx pin */
	// PWR->CSR |= PWR_WakeUpPin;
	// }
	// else
	// {
	/// * Disable the EWUPx pin */
	// PWR->CSR &= ~PWR_WakeUpPin;
	// }
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
