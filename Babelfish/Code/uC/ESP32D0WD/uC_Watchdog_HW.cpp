/**
 *****************************************************************************************
 * @file				uC_Watchdog_HW.cpp
 * @details				See header file for module overview.
 * @copyright			2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_Watchdog_HW.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint32_t RESET_LENGTH = 7U;			///< Reset length.
static const uint32_t ENABLE_VALUE = 1U;			///< Enable value.
static const uint32_t DISABLE_VALUE = 0U;			///< Disable value.
static const uint32_t CLK_PRESCALE_VAL = 40000U;	///< clock Prescalar value.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog_HW::Set_Timeout( uint32_t timeout_in_ms )
{
	// uC_Base::Periph_Module_Enable( PERIPH_TIMG1_MODULE );
	/*	TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
	    TIMERG1.wdt_config0.sys_reset_length = RESET_LENGTH;
	    TIMERG1.wdt_config0.cpu_reset_length = RESET_LENGTH;
	    TIMERG1.wdt_config0.level_int_en = ENABLE_VALUE;
	    TIMERG1.wdt_config0.stg0 = TIMG_WDT_STG_SEL_INT;
	    TIMERG1.wdt_config0.stg1 = TIMG_WDT_STG_SEL_RESET_SYSTEM;
	    TIMERG1.wdt_config1.clk_prescale = CLK_PRESCALE_VAL;
	    TIMERG1.wdt_config2 = timeout_in_ms;
	    TIMERG1.wdt_config3 = timeout_in_ms;
	    TIMERG1.wdt_config0.en = ENABLE_VALUE;
	    TIMERG1.wdt_feed = ENABLE_VALUE;
	    TIMERG1.wdt_wprotect = DISABLE_VALUE;
	    TIMERG1.int_clr_timers.wdt = ENABLE_VALUE;
	    timer_group_intr_enable( TIMER_GROUP_1, TIMG_WDT_INT_ENA_M );*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog_HW::Kick_Dog( void )
{
	// esp_int_wdt_cpu_init();
}
