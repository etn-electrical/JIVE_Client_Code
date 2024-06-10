/**
 *****************************************************************************************
 *	@file		uC_Reset.cpp Implementation File for micro controller reset functionality
 *	@details	See header file for module overview.
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "uC_Reset.h"
#include "esp_task_wdt.h"
#include "esp32/rom/uart.h"
#include "soc/rtc.h"
#include "soc/cpu.h"
#include "uC_IO_Config.h"
#include "esp_task_wdt.h"
#include "esp32/rom/rtc.h"
#include "system_internal.h"

/**
 * @brief	This variable hold the status of last reset.
 * @n		The variable is of static type and the reset status is thus available for reading
 * @n		purpose to each instance of this class.
 */
uC_RESET_CAUSE_TD uC_Reset::m_reset_id = 0;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Reset::uC_Reset( void )
{
	esp_reset_reason_t reason;

	reason = ESP32_Get_Reset_Reason();
	if ( reason == ESP_RST_POWERON )
	{
		Set_Bit_Reset( RESET_POWER_UP_TRIGRD );
	}
	if ( reason == ESP_RST_SW )
	{
		Set_Bit_Reset( RESET_SOFTWARE_TRIGRD );
	}
	if ( reason == ESP_RST_DEEPSLEEP )
	{
		Set_Bit_Reset( RESET_WAKEUP_TRIGRD );
	}
	if ( reason == ESP_RST_TASK_WDT )
	{
		Set_Bit_Reset( RESET_TASK_WDT );
	}
	if ( reason == ESP_RST_INT_WDT )
	{
		Set_Bit_Reset( RESET_WATCHDOG_TRIGRD );
	}
	if ( reason == ESP_RST_WDT )
	{
		Set_Bit_Reset( RESET_WDT );
	}
	if ( reason == ESP_RST_BROWNOUT )
	{
		Set_Bit_Reset( RESET_BROWN_OUT_TRIGRD );
	}
	if ( reason == ESP_RST_SDIO )
	{
		Set_Bit_Reset( RESET_SDIO );
	}
	if ( reason == ESP_RST_UNKNOWN )
	{
		Set_Bit_Reset( RESET_UNDEFINED );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Reset::Soft_Reset( void )
{
	esp_restart();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Reset::Watchdog_Reset( void )
{
	/* We will spin here until the watchdog resets us.  If it doesn't we have problems. */
	while ( 1 )
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
	for (;;)
	{}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Reset::Set_Bit_Reset( uint8_t val )
{
	m_reset_id |= ( ( ENABLE_VALUE ) << ( val ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Reset::Get_Bit( uint32_t var_val, uint32_t bit_val )
{
	bool ret;

	ret = ( ( static_cast<uint32_t>( var_val ) & ( static_cast<uint32_t>( ENABLE_VALUE ) << ( bit_val ) ) ) !=
			DISABLE_VALUE );
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_reset_reason_t uC_Reset::ESP32_Get_Reset_Reason( void )
{
	esp_reset_reason_t ret;
	RESET_REASON rtc_reset_reason;

	esp_reset_reason_t reset_reason_hint = esp_reset_reason_get_hint();

	rtc_reset_reason = rtc_get_reset_reason( 0 );

	switch ( rtc_reset_reason )
	{
		case POWERON_RESET:
			ret = ESP_RST_POWERON;
			break;

		case SW_CPU_RESET:
		case SW_RESET:
		case EXT_CPU_RESET:
			if ( ( reset_reason_hint == ESP_RST_PANIC ) ||
				 ( reset_reason_hint == ESP_RST_BROWNOUT ) ||
				 ( reset_reason_hint == ESP_RST_TASK_WDT ) ||
				 ( reset_reason_hint == ESP_RST_INT_WDT ) )
			{
				ret = reset_reason_hint;
			}
			ret = ESP_RST_SW;
			break;

		case DEEPSLEEP_RESET:
			ret = ESP_RST_DEEPSLEEP;
			break;

		case TG0WDT_SYS_RESET:
			ret = ESP_RST_TASK_WDT;
			break;

		case TG1WDT_SYS_RESET:
			ret = ESP_RST_INT_WDT;
			break;

		case OWDT_RESET:
		case RTCWDT_SYS_RESET:
		case RTCWDT_RTC_RESET:
		case RTCWDT_CPU_RESET:
		case TGWDT_CPU_RESET:
			ret = ESP_RST_WDT;
			break;

		case RTCWDT_BROWN_OUT_RESET:
			ret = ESP_RST_BROWNOUT;
			break;

		case SDIO_RESET:
			ret = ESP_RST_SDIO;
			break;

		case INTRUSION_RESET:
		default:
			ret = ESP_RST_UNKNOWN;
			break;
	}
	if ( reset_reason_hint != ESP_RST_UNKNOWN )
	{
		uC_IO_Config::Set_Reg_Value( RTC_RESET_CAUSE_REG, 0 );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Reset::Enable_SW_Reset()
{
	uint32_t core_id;
	uint32_t other_core_id;

	core_id = xPortGetCoreID();
	other_core_id = ( core_id == DISABLE_VALUE ) ? ENABLE_VALUE : DISABLE_VALUE;

	uC_IO_Config::Set_Pin_Input( RTC_CNTL_OPTIONS0_REG,
								 other_core_id ==
								 DISABLE_VALUE ? RTC_CNTL_SW_PROCPU_RST_M : RTC_CNTL_SW_APPCPU_RST_M );

	if ( other_core_id == ENABLE_VALUE )
	{
		uC_IO_Config::Reset_Pin_Input( RTC_CNTL_SW_CPU_STALL_REG, RTC_CNTL_SW_STALL_APPCPU_C1_M );
		uC_IO_Config::Set_Pin_Input( RTC_CNTL_SW_CPU_STALL_REG, C1S_SHIFT_VALUE << RTC_CNTL_SW_STALL_APPCPU_C1_S );
		uC_IO_Config::Reset_Pin_Input( RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_STALL_APPCPU_C0_M );
		uC_IO_Config::Set_Pin_Input( RTC_CNTL_OPTIONS0_REG, C0S_SHIFT_VALUE << RTC_CNTL_SW_STALL_APPCPU_C0_S );
	}
	else
	{
		uC_IO_Config::Reset_Pin_Input( RTC_CNTL_SW_CPU_STALL_REG, RTC_CNTL_SW_STALL_PROCPU_C1_M );
		uC_IO_Config::Set_Pin_Input( RTC_CNTL_SW_CPU_STALL_REG, C1S_SHIFT_VALUE << RTC_CNTL_SW_STALL_PROCPU_C1_S );
		uC_IO_Config::Reset_Pin_Input( RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_STALL_PROCPU_C0_M );
		uC_IO_Config::Set_Pin_Input( RTC_CNTL_OPTIONS0_REG, C0S_SHIFT_VALUE << RTC_CNTL_SW_STALL_PROCPU_C0_S );
	}
}
