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
#ifndef uC_BASE_HW_IDS_STM32L451_H
   #define uC_BASE_HW_IDS_STM32L451_H

#include "uC_STM32L451.h"



#include "uC_IO_Define_STM32L451.h"


/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first of these freqs have to match the base clocks.
// The other clocks can be defined here.
enum
{
	uC_BASE_STM32L451_PCLK_INDEX_AHB1,		// These first three must match the "Peripheral Power Definition Structure"
	uC_BASE_STM32L451_PCLK_INDEX_AHB2,		// enumeration in uC_Base.h.
	uC_BASE_STM32L451_PCLK_INDEX_APB1,
	uC_BASE_STM32L451_PCLK_INDEX_APB2,
	uC_BASE_STM32L451_PCLK_INDEX_TIMERS2to7,
	uC_BASE_STM32L451_PCLK_INDEX_TIMER1and8,
	uC_BASE_STM32L451_PCLK_INDEX_TIMER_ADC,
	uC_BASE_STM32L451_MAX_PCLKS
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
	uC_BASE_PIO_H,
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
	uC_BASE_MAX_TIMER_CAP_COMP_CH
};

static const uint8_t uC_BASE_0_MAX_TIMER_CAP_COMP_CH = 0U;
static const uint8_t uC_BASE_1_MAX_TIMER_CAP_COMP_CH = 1U;
static const uint8_t uC_BASE_2_MAX_TIMER_CAP_COMP_CH = 2U;
static const uint8_t uC_BASE_4_MAX_TIMER_CAP_COMP_CH = 4U;

enum
{
	uC_BASE_TIMER_CTRL_1,
	uC_BASE_TIMER_CTRL_2,
	uC_BASE_TIMER_CTRL_3,
	uC_BASE_TIMER_CTRL_15,
	uC_BASE_TIMER_CTRL_16,
	uC_BASE_TIMER_CTRL_6,
	uC_BASE_MAX_TIMER_CTRLS
};

/*
 *****************************************************************************************
 *      USART Application Peripheral ID's
 *			File: uC_USART
 *****************************************************************************************
 */
static const uint8_t uC_BASE_FS_USB_PORT = 0U;
static const uint8_t uC_BASE_HS_FS_USB_PORT = 1U;
static const uint8_t uC_BASE_MAX_USB_PORTS = 1U;

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
	uC_BASE_USART_PORT_4,
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
 *      I2C Chip Select Application Peripheral ID's
 *			File: uC_I2C
 *****************************************************************************************
 */
enum
{
	uC_BASE_I2C_CTRL_1,
	uC_BASE_I2C_CTRL_2,
	uC_BASE_I2C_CTRL_3,
	uC_BASE_I2C_MAX_CTRLS
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
	uC_BASE_PWM_CTRL_4 = uC_BASE_TIMER_CTRL_15,
	uC_BASE_PWM_CTRL_5 = uC_BASE_TIMER_CTRL_15,
	uC_BASE_PWM_CTRL_6 = uC_BASE_TIMER_CTRL_15,
	uC_BASE_PWM_CTRL_7 = uC_BASE_TIMER_CTRL_16,
	uC_BASE_PWM_CTRL_8 = uC_BASE_TIMER_CTRL_16,
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
	/* Only one ADC available on STM32L451 */
	uC_BASE_ATOD_CTRL_1,
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
	uC_ATOD_CHANNEL_18 = 0x1 << 18,
	uC_ATOD_CHANNEL_ALL = 0xFF
};

#define uC_ATOD_CHANNEL_MAX         19

// #define ADC1_DMA_SELECT			DMA_ADC1_D2S4C0	//Alternate DMA definition

/*
 *****************************************************************************************
 *      AtoD Application Peripheral ID's
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
	uC_DMA2_CHANNEL_6,
	uC_DMA2_CHANNEL_7,
	uC_BASE_DMA_CHANNEL_MAX
};

#define uC_DMA_CHANNEL_DNE      ( static_cast<uint8_t>( uC_BASE_DMA_CHANNEL_MAX ) + 1U )

#define DMA_STREAM_CHANNEL_SHIFT        4U

#define DMA_USART2TX_D1C7               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_7 ) | \
										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART2RX_D1C6               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_6 ) | \
										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART1RX_D1C5               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_5 ) | \
										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART1TX_D1C4               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_4 ) | \
										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART3RX_D1C3               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_3 ) | \
										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART3TX_D1C2               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_2 ) | \
										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_ADC1_D1C1                   ( static_cast<uint8_t>( uC_DMA1_CHANNEL_1 ) | \
										  ( static_cast<uint8_t>( 0 ) << DMA_STREAM_CHANNEL_SHIFT ) )

#define DMA_QUADSPI_D2C7                ( static_cast<uint8_t>( uC_DMA2_CHANNEL_7 ) | \
										  ( static_cast<uint8_t>( 3 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART4RX_D2C5                ( static_cast<uint8_t>( uC_DMA2_CHANNEL_5 ) | \
										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_ADC2_D2C4                   ( static_cast<uint8_t>( uC_DMA2_CHANNEL_4 ) | \
										  ( static_cast<uint8_t>( 0 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART4TX_D2C3                ( static_cast<uint8_t>( uC_DMA2_CHANNEL_3 ) | \
										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )

#define DMA_SPI2TX_D1C5               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_5 ) | \
										( static_cast<uint8_t>( 1 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_SPI2RX_D1C4               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_4 ) | \
										( static_cast<uint8_t>( 1 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_SPI1TX_D1C3               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_3 ) | \
										( static_cast<uint8_t>( 1 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_SPI1RX_D1C2               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_2 ) | \
										( static_cast<uint8_t>( 1 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_SPI3TX_D2C2               ( static_cast<uint8_t>( uC_DMA2_CHANNEL_2 ) | \
										( static_cast<uint8_t>( 3 ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_SPI3RX_D2C1               ( static_cast<uint8_t>( uC_DMA2_CHANNEL_1 ) | \
										( static_cast<uint8_t>( 3 ) << DMA_STREAM_CHANNEL_SHIFT ) )



/*
 *****************************************************************************************
 *      EMAC Controller Peripheral ID's
 *			File: uC_EMAC
 *****************************************************************************************
 */
static const uint32_t SECTOR_ERASE_TIME_LARGEST_MS = 2000U;	/* Sector erase time in miliseconds. Considering x32 erase*/
static const uint32_t SECTOR_ERASE_TIME_BASELINE_MS = 400U;	/* Sector erase time in miliseconds. Considering x32 erase*/
static const uint32_t BYTE_WRITE_TIME_US = 16U;		/* Writing 4 byte data into flash memory */
// there is no sector definition in L451, just 2K bytes page

typedef uint32_t sector_addr_t;
typedef uint32_t sector_num_t;
typedef struct
{
	sector_addr_t sector_size;
	sector_num_t sector_no;
} flash_page_t;
const flash_page_t INTERNAL_FLASH_PAGE_SIZES[25] =
{
	{0x4000, 0},
	{0x4000, 1},
	{0x4000, 2},
	{0x4000, 3},
	{0x10000, 4},
	{0x20000, 5},
	{0x20000, 6},
	{0x20000, 7},
	{0x20000, 8},
	{0x20000, 9},
	{0x20000, 10},
	{0x20000, 11},
	{0x4000, 16},
	{0x4000, 17},
	{0x4000, 18},
	{0x4000, 19},
	{0x10000, 20},
	{0x20000, 21},
	{0x20000, 22},
	{0x20000, 23},
	{0x20000, 24},
	{0x20000, 25},
	{0x20000, 26},
	{0x20000, 27},
	{0x00000, 0xFFFF}
};
const UINT32 INTERNAL_FLASH_START_ADDRESS = 0x08000000U;

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

#endif	// This is the endif for the multiple include.
