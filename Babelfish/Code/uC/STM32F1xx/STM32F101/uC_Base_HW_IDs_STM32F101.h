/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_BASE_HW_IDS_STM32F101_H
   #define uC_BASE_HW_IDS_STM32F101_H

#include "uC_STM32F101.h"

#ifdef __IAR_SYSTEMS_ICC__		// This makes sure that the assembly compile will not include the following.


#include "uC_IO_Define_STM32F101.h"
#include "Drv_Lib_Inc.h"


/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
enum
{
	uC_BASE_STM32F101_PCLK_INDEX_AHB,		// These first three must match the "Peripheral Power Definition Structure"
	uC_BASE_STM32F101_PCLK_INDEX_APB1,		// enumeration in uC_Base.h.
	uC_BASE_STM32F101_PCLK_INDEX_APB2,
	uC_BASE_STM32F101_PCLK_INDEX_TIMERS2to7,
	uC_BASE_STM32F101_PCLK_INDEX_TIMER1,
	uC_BASE_STM32F101_PCLK_INDEX_TIMER_ADC,
	uC_BASE_STM32F101_MAX_PCLKS
};


/*
 *****************************************************************************************
 *      Watchdog
 *			File: uC_Watchdog_HW
 *****************************************************************************************
 */
enum
{
	uC_BASE_MAX_WDOG_CTRLS
};


/*
 *****************************************************************************************
 *      PIO Ctrl ID's
 *			File: uC_IO_Config
 *****************************************************************************************
 */
static const uint_fast8_t uC_BASE_PIO_A = 0U;
static const uint_fast8_t uC_BASE_PIO_B = 1U;
static const uint_fast8_t uC_BASE_PIO_C = 2U;
static const uint_fast8_t uC_BASE_PIO_D = 3U;
static const uint_fast8_t uC_BASE_PIO_E = 4U;
static const uint_fast8_t uC_BASE_MAX_PIO_CTRLS = 5U;


/*
 *****************************************************************************************
 *      Timer Application Peripheral ID's (
 *			File: uC_Timers
 *****************************************************************************************
 */
enum
{
	uC_BASE_TIMER_CAP_COMP_CH1,
	uC_BASE_TIMER_CAP_COMP_CH2,
	uC_BASE_TIMER_CAP_COMP_CH3,
	uC_BASE_TIMER_CAP_COMP_CH4,
	uC_BASE_MAX_TIMER_CAP_COMP_CH,
};

const uint8_t uC_BASE_0_MAX_TIMER_CAP_COMP_CH = 0;
const uint8_t uC_BASE_2_MAX_TIMER_CAP_COMP_CH = 2;
const uint8_t uC_BASE_4_MAX_TIMER_CAP_COMP_CH = 4;
enum
{
	uC_BASE_TIMER_CTRL_1,
	uC_BASE_TIMER_CTRL_2,
	uC_BASE_TIMER_CTRL_3,
	uC_BASE_TIMER_CTRL_4,
	uC_BASE_TIMER_CTRL_6,
	uC_BASE_TIMER_CTRL_7,
	uC_BASE_MAX_TIMER_CTRLS
};

/*
 *****************************************************************************************
 *      USART Application Peripheral ID's
 *			File: uC_USART
 *****************************************************************************************
 */
enum
{
	uC_BASE_USART_PORT_1,
	uC_BASE_USART_PORT_2,
	uC_BASE_USART_PORT_3,
	uC_BASE_USART_PORT_4,	// #k: New
	uC_BASE_USART_PORT_5,
	uC_BASE_MAX_USART_PORTS
};

/*
 *****************************************************************************************
 *      SPI Chip Select Application Peripheral ID's
 *			File: uC_Interrupt
 *****************************************************************************************
 */
enum
{
	uC_BASE_SPI_CTRL_1,
	uC_BASE_SPI_CTRL_2,
	uC_BASE_SPI_CTRL_3,
	uC_BASE_SPI_MAX_CTRLS
};

/*
 *****************************************************************************************
 *      External Interrupts Definition.
 *			File: uC_Ext_Int
 *****************************************************************************************
 */
enum
{
	uC_BASE_EXT_INT_1,
	uC_BASE_EXT_INT_2,
	uC_BASE_EXT_INT_3,
	uC_BASE_EXT_INT_4,
	uC_BASE_EXT_INT_5,
	uC_BASE_EXT_INT_6,
	uC_BASE_EXT_INT_7,
	uC_BASE_EXT_INT_8,
	uC_BASE_EXT_INT_9,
	uC_BASE_EXT_INT_10,
	uC_BASE_EXT_INT_11,
	uC_BASE_EXT_INT_12,
	uC_BASE_EXT_INT_13,
	uC_BASE_EXT_INT_14,
	uC_BASE_EXT_INT_15,
	uC_BASE_EXT_INT_16,
	uC_BASE_EXT_INT_MAX_INPUTS
};

/*
 *****************************************************************************************
 *      PWM Application Peripheral ID's
 *			File: uC_PWM
 *****************************************************************************************
 */
enum
{
	uC_BASE_PWM_CHANNEL_1 = uC_BASE_TIMER_CAP_COMP_CH1,
	uC_BASE_PWM_CHANNEL_2 = uC_BASE_TIMER_CAP_COMP_CH2,
	uC_BASE_PWM_CHANNEL_3 = uC_BASE_TIMER_CAP_COMP_CH3,
	uC_BASE_PWM_CHANNEL_4 = uC_BASE_TIMER_CAP_COMP_CH4,
	uC_BASE_PWM_MAX_CHANNELS
};

enum
{
	uC_BASE_PWM_CTRL_1 = uC_BASE_TIMER_CTRL_1,
	uC_BASE_PWM_CTRL_2 = uC_BASE_TIMER_CTRL_2,
	uC_BASE_PWM_CTRL_3 = uC_BASE_TIMER_CTRL_3,
	uC_BASE_PWM_CTRL_4 = uC_BASE_TIMER_CTRL_4,
	// uC_BASE_PWM_CTRL_5 = uC_BASE_TIMER_CTRL_5,
	uC_BASE_PWM_MAX_CTRLS
};

/*
 *****************************************************************************************
 *      AtoD Application Peripheral ID's
 *			File: uC_PWM
 *****************************************************************************************
 */
typedef enum uC_BASE_ATOD_CTRL_ENUM
{
	uC_BASE_ATOD_CTRL_1,
	// uC_BASE_ATOD_CTRL_2,
	uC_BASE_ATOD_MAX_CTRLS
} uC_BASE_ATOD_CTRL_ENUM;

enum
{
	uC_ATOD_CHANNEL_0 = 0x1 << 0,
	uC_ATOD_CHANNEL_1 = 0x1 << 1,
	uC_ATOD_CHANNEL_2 = 0x1 << 2,
	uC_ATOD_CHANNEL_3 = 0x1 << 3,
	uC_ATOD_CHANNEL_4 = 0x1 << 4,
	uC_ATOD_CHANNEL_5 = 0x1 << 5,
	uC_ATOD_CHANNEL_6 = 0x1 << 6,
	uC_ATOD_CHANNEL_7 = 0x1 << 7,
	uC_ATOD_CHANNEL_8 = 0x1 << 8,
	uC_ATOD_CHANNEL_9 = 0x1 << 9,
	uC_ATOD_CHANNEL_10 = 0x1 << 10,
	uC_ATOD_CHANNEL_11 = 0x1 << 11,
	uC_ATOD_CHANNEL_12 = 0x1 << 12,
	uC_ATOD_CHANNEL_13 = 0x1 << 13,
	uC_ATOD_CHANNEL_14 = 0x1 << 14,
	uC_ATOD_CHANNEL_15 = 0x1 << 15,
	uC_ATOD_CHANNEL_16 = 0x1 << 16,
	uC_ATOD_CHANNEL_17 = 0x1 << 17,
	uC_ATOD_CHANNEL_ALL = 0xFF,
};

#define uC_ATOD_CHANNEL_MAX         18U


/*
 *****************************************************************************************
 *      AtoD Application Peripheral ID's
 *			File: uC_PWM
 *****************************************************************************************
 */
enum
{
	uC_DMA1_CHANNEL_1,
	uC_DMA1_CHANNEL_2,
	uC_DMA1_CHANNEL_3,
	uC_DMA1_CHANNEL_4,
	uC_DMA1_CHANNEL_5,
	uC_DMA1_CHANNEL_6,
	uC_DMA1_CHANNEL_7,
	uC_DMA2_CHANNEL_1,
	uC_DMA2_CHANNEL_2,
	uC_DMA2_CHANNEL_3,
	uC_DMA2_CHANNEL_4,
	uC_DMA2_CHANNEL_5,
	uC_BASE_DMA_CHANNEL_MAX
};

#define uC_DMA_CHANNEL_DNE      ( static_cast<uint8_t>( uC_BASE_DMA_CHANNEL_MAX ) + 1U )


#endif	// This is the endif for the ICC Check (Are we compiling for C/C++)

static const uint32_t SECTOR_ERASE_TIME_MS = 2000U;	/* Sector erase time in miliseconds. Considering x32 erase*/
static const uint32_t DWORD_WRITE_TIME_US = 16U;	/* Writing 4 byte data into flash memory */
const UINT32 INTERNAL_FLASH_START_ADDRESS = 0x08000000U;
const UINT32 FLASH_PAGE_SIZE = 0x800;/* 2KB page */
#endif	// This is the endif for the multiple include.
