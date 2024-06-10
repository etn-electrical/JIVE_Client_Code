/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Test_Example.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "Main.h"

#ifdef BLINK_LED
#include "Blink_Example.h"
#endif
#ifdef TEST_ESP32WROOM32_GPIO
#include "gpio_example.h"
#endif
#ifdef TEST_ESP32WROOM32_SINGLETIMER
#include "timer_group_example_main.h"
#endif
#ifdef TEST_ESP32WROOM32_USART_DMA
#include "UDMA_example.cpp"
#endif
#ifdef TEST_FREERTOS_OS
#include "Test_OS_Example.h"
#endif
#ifdef TEST_SOFT_TIMERS
#include "Test_Soft_Timers.h"
#endif
#ifdef TEST_FAULT_CATCH_FUNCTION
#include "Test_Fault_Catch.h"
#endif
#ifdef TEST_DEBUG
#include "Test_Debug_manager.h"
#endif
#if defined ( IOT_TEST ) || defined ( IOT_C2D_TIMING_TEST )
#include "IOT_Test_Task.h"
#endif
#ifdef TEST_CR_TASK
#include "Test_CR_Task.h"
#endif
#ifdef TEST_SNTP
#include "Test_SNTP.h"
#endif
#ifdef TEST_ESP32_PARTITION_APIs
#include "Test_Esp32_Partition.h"
#endif
#ifdef FLASH_TEST
#include "Spi_flash_test_example.h"
#endif

#if defined ( TEST_IPCC ) && !defined ( CONFIG_FREERTOS_UNICORE )
#include "Test_IPCC.h"
#endif
#ifdef TEST_NANO_SEC_TIMER
#include "Test_nSec_Timer.h"
#endif
#ifdef TEST_EXT_INT
#include "Test_Ext_Int.h"
#endif

void Test_Example( void )
{
#ifdef TEST_ESP32WROOM32_GPIO
	test_GPIO_app_main();
#endif
#ifdef TEST_DEBUG
	Test_Debug_manager_app_main();
#endif

#ifdef TEST_ESP32_PARTITION_APIs
	Test_Esp32_Partition();
#endif

#ifdef TEST_ESP32WROOM32_SINGLETIMER
	timer_test_app_main();
#endif
#ifdef TEST_ESP32WROOM32_USART_DMA
	uart_test_app_main();
#endif
#ifdef TEST_PHASE1_SAMPLE_APPLICATION
	phase1_sample_demo_app_main();
#endif
#ifdef TEST_ESP32WROOM32_ADC
	test_ADC_app_main();
#endif
#ifdef TEST_I2C_MASTER
	i2c_test_app_main();
#endif
#ifdef CAN_SELF_TEST
	can_self_test_app_main();
#endif
#ifdef CAN_SELF_TEST_250KBPS
	can_self_test_app_main_250kbps();
#endif
#ifdef CAN_SELF_TEST_500KBPS
	can_self_test_app_main_500kbps();
#endif
#ifdef CAN_SELF_TEST_1000KBPS
	can_self_test_app_main_1000kbps();
#endif
#ifdef CAN_SELF_TEST_MASTER
	can_network_test_master_app_main();
#endif
#ifdef CAN_SELF_TEST_SLAVE
	can_network_test_slave_app_main();
#endif
#ifdef ESP32_TEST_CODE

#ifdef TEST_WDT_RESET
	wdt_reset_test_app_main();
#endif
#ifdef TEST_SOFT_RESET
	soft_reset_test_app_main();
#endif
#ifdef TEST_POWER_ON_RESET
	power_on_reset_test_app_main();
#endif
#else
#ifdef TEST_WDT_RESET
	wdt_reset_test_app_main();
#endif
#ifdef TEST_SOFT_RESET
	soft_reset_test_app_main();
#endif
#ifdef TEST_POWER_ON_RESET
	power_on_reset_test_app_main();
#endif
#ifdef TEST_BROWNOUT_RESET
	brownout_reset_test_app_main();
#endif
#endif

#ifdef TEST_ESP32WROOM32_DCI_OS_MAIN
	main();
	DCI_RAM_Test_App_Main();
#endif

#ifdef FLASH_TEST
#ifdef ESP_FLASH_TEST
	Esp32_Spi_Flash_Test_App_Main();
#else
	Spi_Flash_Test_App_Main();
#endif
#endif

#ifdef TEST_ESP32WROOM32_DCI_FLASH
	main();
	DCI_NV_Flash_Test_App_Main();
#endif

#ifdef TEST_WATCHDOG_TEST
	uC_Watchdog_Test_Init_App_Main();
#endif

#ifdef PWM_TEST
	uC_PWM_Test_Init_App_Main();
#endif

#ifdef TEST_ESP32WROOM32_MULTIPLE_HAL
	main();
	Test_App_Multiple_HAL_Main();
#endif

#ifdef TEST_FREERTOS_OS
	Test_OS_Task();
#endif

#ifdef BLINK_LED
	Blink_LED_Example();
#endif

#ifdef TEST_ESP32WROOM32_RTC
	Test_RTC_App_Main();	// call to App Main function for RTC
#endif

#ifdef TEST_SOFT_TIMERS
	Test_Soft_Timers();
#endif

#ifdef TEST_FAULT_CATCH_FUNCTION
	Test_Fault_Catch_App_Main();
#endif

#ifdef IOT_TEST
	IOT_Test_Task();
#endif

#ifdef TEST_CR_TASK
	Test_CR_Task();
#endif

#ifdef TEST_SNTP
	Test_SNTP_App_Main();
#endif

#if defined ( TEST_IPCC ) && !defined ( CONFIG_FREERTOS_UNICORE )
	Test_IPCC_App_Main();
#endif
#ifdef TEST_NANO_SEC_TIMER
	Test_nSec_Timer_Main();
#endif

#ifdef TEST_EXT_INT
	Test_Ext_Int_App_Main();
#endif
}
