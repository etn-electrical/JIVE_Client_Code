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
#ifndef uC_BASE_HW_IDS_STM32F302_H
   #define uC_BASE_HW_IDS_STM32F302_H

#include "uC_STM32F302.h"

#ifdef __IAR_SYSTEMS_ICC__		// This makes sure that the assembly compile will not include the following.


#include "uC_IO_Define_STM32F302.h"



/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
enum
{
	uC_BASE_STM32F302_PCLK_INDEX_AHB,		// These first three must match the "Peripheral Power Definition Structure"
	uC_BASE_STM32F302_PCLK_INDEX_APB1,		// enumeration in uC_Base.h.
	uC_BASE_STM32F302_PCLK_INDEX_APB2,
	uC_BASE_STM32F302_PCLK_INDEX_TIMERS2to7,
	uC_BASE_STM32F302_PCLK_INDEX_TIMER1,
	uC_BASE_STM32F302_PCLK_INDEX_TIMER_ADC,
	uC_BASE_STM32F302_MAX_PCLKS
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
enum
{
	uC_BASE_PIO_A,
	uC_BASE_PIO_B,
	uC_BASE_PIO_C,
	uC_BASE_PIO_D,
	uC_BASE_PIO_E,
	uC_BASE_PIO_F,
	uC_BASE_MAX_PIO_CTRLS
};

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

typedef uint32_t sector_addr_t;
typedef uint32_t sector_num_t;

const uint8_t uC_BASE_0_MAX_TIMER_CAP_COMP_CH = 0;
const uint8_t uC_BASE_2_MAX_TIMER_CAP_COMP_CH = 2;
const uint8_t uC_BASE_4_MAX_TIMER_CAP_COMP_CH = 4;


static const uint8_t uC_BASE_TIMER_CTRL_1 = 0U;
static const uint8_t uC_BASE_TIMER_CTRL_2 = 1U;
static const uint8_t uC_BASE_TIMER_CTRL_3 = 2U;
static const uint8_t uC_BASE_TIMER_CTRL_4 = 3U;
// static const uint8_t uC_BASE_TIMER_CTRL_5 = 4U;
static const uint8_t uC_BASE_TIMER_CTRL_6 = 4U;
static const uint8_t uC_BASE_TIMER_CTRL_15 = 5U;
static const uint8_t uC_BASE_TIMER_CTRL_16 = 6U;
static const uint8_t uC_BASE_TIMER_CTRL_17 = 7U;
static const uint8_t uC_BASE_MAX_TIMER_CTRLS = 8U;

/*
 *****************************************************************************************
 *      USB Application Peripheral ID's
 *			File: uC_USART
 *****************************************************************************************
 */
static const uint8_t uC_BASE_FS_USB_PORT = 0;
static const uint8_t uC_BASE_MAX_USB_PORTS = 1;
/*
 *****************************************************************************************
 *      USART Application Peripheral ID's
 *			File: uC_USART
 *****************************************************************************************
 */
/*
   enum
   {
    uC_BASE_USART_PORT_1,
    uC_BASE_USART_PORT_2,
    uC_BASE_USART_PORT_3,
    uC_BASE_MAX_USART_PORTS
   };
 */
static const uint8_t uC_BASE_USART_PORT_1 = 0U;
static const uint8_t uC_BASE_USART_PORT_2 = 1U;
static const uint8_t uC_BASE_USART_PORT_3 = 2U;
static const uint8_t uC_BASE_MAX_USART_PORTS = 3U;


/*

 *****************************************************************************************
 *      I2C Chip Select Application Peripheral ID's
 *			File: uC_I2C
 *****************************************************************************************
 */
enum
{
	uC_BASE_I2C_CTRL_1,
	uC_BASE_I2C_CTRL_2,
	uC_BASE_I2C_MAX_CTRLS
};

/*
 *****************************************************************************************
 *      SPI Chip Select Application Peripheral ID's
 *			File: uC_Interrupt
 *****************************************************************************************
 */
/*
   enum
   {
    uC_BASE_SPI_CTRL_1,
    uC_BASE_SPI_CTRL_2,
    uC_BASE_SPI_CTRL_3,
    uC_BASE_SPI_MAX_CTRLS
   };
 */
static const uint8_t uC_BASE_SPI_CTRL_1 = 0U;
static const uint8_t uC_BASE_SPI_CTRL_2 = 1U;
static const uint8_t uC_BASE_SPI_CTRL_3 = 2U;
static const uint8_t uC_BASE_SPI_MAX_CTRLS = 3U;
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
	// uC_BASE_PWM_CTRL_1 = uC_BASE_TIMER_CTRL_1,
	uC_BASE_PWM_CTRL_2 = uC_BASE_TIMER_CTRL_2,
	uC_BASE_PWM_CTRL_3 = uC_BASE_TIMER_CTRL_3,
	uC_BASE_PWM_CTRL_4 = uC_BASE_TIMER_CTRL_4,
	uC_BASE_PWM_CTRL_6 = uC_BASE_TIMER_CTRL_6,
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
	uC_BASE_ATOD_CTRL_2,
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

#define uC_ATOD_CHANNEL_MAX         18


/*
 *****************************************************************************************
 *      DMA Channels
 *			File: uC_DMA
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




static const uint32_t SECTOR_ERASE_TIME_MS = 2000U;	/* Sector erase time in miliseconds. Considering x32 erase*/
static const uint32_t DWORD_WRITE_TIME_US = 16U;	/* Writing 4 byte data into flash memory */
const UINT32 FLASH_PAGE_SIZE = 0x800;/* 2KB page */
static const uint32_t SECTOR_ERASE_TIME_LARGEST_MS = 400U;	/* Sector erase time in miliseconds. Considering x32 erase*/
static const uint32_t SECTOR_ERASE_TIME_BASELINE_MS = 200U;	/* Sector erase time in miliseconds. Considering x32 erase.
															   smallest sector size in STM32 is 16kb*/
static const uint32_t BYTE_WRITE_TIME_US = 16U;		/* Writing 4 byte data into flash memory */

typedef uint32_t sector_addr_t;
typedef uint32_t sector_num_t;
typedef struct
{
	sector_addr_t sector_size;
	sector_num_t sector_no;
} flash_page_t;
const flash_page_t INTERNAL_FLASH_PAGE_SIZES[129] =
{
	{ 0x800, 0 },
	{ 0x800, 1 },
	{ 0x800, 2 },
	{ 0x800, 3 },
	{ 0x800, 4 },
	{ 0x800, 5 },
	{ 0x800, 6 },
	{ 0x800, 7 },
	{ 0x800, 8 },
	{ 0x800, 9 },
	{ 0x800, 10 },
	{ 0x800, 11 },
	{ 0x800, 12 },
	{ 0x800, 13 },
	{ 0x800, 14 },
	{ 0x800, 15 },
	{ 0x800, 16 },
	{ 0x800, 17 },
	{ 0x800, 18 },
	{ 0x800, 19 },
	{ 0x800, 20 },
	{ 0x800, 21 },
	{ 0x800, 22 },
	{ 0x800, 23 },
	{ 0x800, 24 },
	{ 0x800, 25 },
	{ 0x800, 26 },
	{ 0x800, 27 },
	{ 0x800, 28 },
	{ 0x800, 29 },
	{ 0x800, 30 },
	{ 0x800, 31 },
	{ 0x800, 32 },
	{ 0x800, 33 },
	{ 0x800, 34 },
	{ 0x800, 35 },
	{ 0x800, 36 },
	{ 0x800, 37 },
	{ 0x800, 38 },
	{ 0x800, 39 },
	{ 0x800, 40 },
	{ 0x800, 41 },
	{ 0x800, 42 },
	{ 0x800, 43 },
	{ 0x800, 44 },
	{ 0x800, 45 },
	{ 0x800, 46 },
	{ 0x800, 47 },
	{ 0x800, 48 },
	{ 0x800, 49 },
	{ 0x800, 50 },
	{ 0x800, 51 },
	{ 0x800, 52 },
	{ 0x800, 53 },
	{ 0x800, 54 },
	{ 0x800, 55 },
	{ 0x800, 56 },
	{ 0x800, 57 },
	{ 0x800, 58 },
	{ 0x800, 59 },
	{ 0x800, 60 },
	{ 0x800, 61 },
	{ 0x800, 62 },
	{ 0x800, 63 },
	{ 0x800, 64 },
	{ 0x800, 65 },
	{ 0x800, 66 },
	{ 0x800, 67 },
	{ 0x800, 68 },
	{ 0x800, 69 },
	{ 0x800, 70 },
	{ 0x800, 71 },
	{ 0x800, 72 },
	{ 0x800, 73 },
	{ 0x800, 74 },
	{ 0x800, 75 },
	{ 0x800, 76 },
	{ 0x800, 77 },
	{ 0x800, 78 },
	{ 0x800, 79 },
	{ 0x800, 80 },
	{ 0x800, 81 },
	{ 0x800, 82 },
	{ 0x800, 83 },
	{ 0x800, 84 },
	{ 0x800, 85 },
	{ 0x800, 86 },
	{ 0x800, 87 },
	{ 0x800, 88 },
	{ 0x800, 89 },
	{ 0x800, 90 },
	{ 0x800, 91 },
	{ 0x800, 92 },
	{ 0x800, 93 },
	{ 0x800, 94 },
	{ 0x800, 95 },
	{ 0x800, 96 },
	{ 0x800, 97 },
	{ 0x800, 98 },
	{ 0x800, 99 },
	{ 0x800, 100 },
	{ 0x800, 101 },
	{ 0x800, 102 },
	{ 0x800, 103 },
	{ 0x800, 104 },
	{ 0x800, 105 },
	{ 0x800, 106 },
	{ 0x800, 107 },
	{ 0x800, 108 },
	{ 0x800, 109 },
	{ 0x800, 110 },
	{ 0x800, 111 },
	{ 0x800, 112 },
	{ 0x800, 113 },
	{ 0x800, 114 },
	{ 0x800, 115 },
	{ 0x800, 116 },
	{ 0x800, 117 },
	{ 0x800, 118 },
	{ 0x800, 119 },
	{ 0x800, 120 },
	{ 0x800, 121 },
	{ 0x800, 122 },
	{ 0x800, 123 },
	{ 0x800, 124 },
	{ 0x800, 125 },
	{ 0x800, 126 },
	{ 0x800, 127 },
	{ 0x00000, 0xFFFF }
};

static const unsigned int INTERNAL_FLASH_START_ADDRESS = 0x08000000;
/*
 *****************************************************************************************
 *      CAN Chip Select Application Peripheral ID's
 *			File: uC_CAN
 *****************************************************************************************
 */
enum
{
	uC_BASE_CAN_CTRL_1,
	uC_BASE_CAN_MAX_CTRLS
};

#endif	// This is the endif for __IAR_SYSTEMS_ICC__


#endif	// This is the endif for the multiple include.
