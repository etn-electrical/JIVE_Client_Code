/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TEST_EXAMPLE_H
#define TEST_EXAMPLE_H

/****************************************************************************************************
*
* Below are the defines required to enable test examples from test_main.cpp
*
****************************************************************************************************/
/****************************************************************************************************
*
* Below are the defines required to enable test examples from test_main.cpp
*
****************************************************************************************************/
// #define TEST_EXAMPLES

#ifdef TEST_EXAMPLES
// #define TEST_ESP32WROOM32_GPIO			///<To test GPIO
// #define TEST_ESP32WROOM32_SINGLETIMER		///< To test single timers
// #define TEST_ESP32WROOM32_USART_DMA		///< To test USART DMA
// #define TEST_PHASE1_SAMPLE_APPLICATION	///< To test Phase1 requirements (GPIO,Timer,Interrupt and UART)
// #define TEST_ESP32WROOM32_ADC				///< To test ADC
// #define TEST_I2C_MASTER					///< To test I2C Master
// #define CAN_SELF_TEST						///< To test CAN protocol
// #define CAN_SELF_TEST_250KBPS				///< To test CAN protocol with 250kbps
// #define CAN_SELF_TEST_500KBPS				///< To test CAN protocol with 500kbps
// #define CAN_SELF_TEST_1000KBPS			///< To test CAN protocol with 1000kbps
// #define CAN_SELF_TEST_SLAVE				///< To test CAN network
// #define CAN_SELF_TEST_MASTER				///< To test CAN network
// #define TEST_WDT_RESET					///< To test watch dog reset
// #define TEST_SOFT_RESET					///<  To test Soft reset
// #define TEST_POWER_ON_RESET				///< To test Power on reset
// #define TEST_BROWNOUT_RESET				///< To test Power on reset
// #define TEST_ESP32WROOM32_DCI_OS_MAIN		///< To test DCI
// #define TEST_WDT_RESET					///< To test watch dog reset
// #define TEST_SOFT_RESET					///< To test Soft reset
// #define TEST_POWER_ON_RESET				///< To test Power on reset
// #define TEST_BROWNOUT_RESET				///< To test Power on reset
// #define FLASH_TEST						///< To test Internal flash module
// #define TEST_ESP32WROOM32_DCI_FLASH		///< To test Power on reset
// #define TEST_WATCHDOG_TEST				///< To test Watchdog timer
// #define PWM_TEST							///< To test PWM
// #define TEST_ESP32WROOM32_MULTIPLE_HAL	///< Test multiple HAL layers
// #define TEST_FREERTOS_OS					///< To test multiple OS task creation and there functionality
// #define BLINK_LED							///< To test LED Blinking >
// #define TEST_ESP32WROOM32_RTC				///< To test internal RTC
// #define TEST_SOFT_TIMERS					///< To test soft timers>
// #define TEST_FAULT_CATCH_FUNCTION			///< To test fault catch>
// #define TEST_DEBUG							///< To test debug manager>
// #define IOT_TEST							///< To test IOT
// #define TEST_CR_TASK						///< To test CR Tasker functionality
// #define TEST_SNTP							///< To test SNTP(Simple Network Time Protocol) functionality
// #define IOT_C2D_TIMING_TEST				///< To test timing for C2D command
// #define TEST_IPCC						///< To test Inter-Processor communication
// #define TEST_ESP32_PARTITION_APIs					///< To test OTA partition
// #define TEST_NANO_SEC_TIMER				///< To test time elapsed in nano second
// #define TEST_EXT_INT                     ///< To test external interrupt
#endif

void Test_Example( void );

#endif	// #define TEST_EXAMPLE_H
