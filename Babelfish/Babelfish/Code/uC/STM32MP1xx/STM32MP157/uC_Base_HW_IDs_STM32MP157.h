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
#ifndef uC_BASE_HW_IDS_STM32MP157_H
   #define uC_BASE_HW_IDS_STM32MP157_H

#include "uC_STM32MP157.h"
#include "stm32mp1xx_hal_dma.h"

#ifdef __IAR_SYSTEMS_ICC__		// This makes sure that the assembly compile will not include the following.


#include "uC_IO_Define_STM32MP157.h"


/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first of these freqs have to match the base clocks.
// The other clocks can be defined here.
enum
{
	// These first three must match the "Peripheral Power Definition Structure"
	uC_BASE_STM32MP157_PCLK_INDEX_AHB2,		// enumeration in uC_Base.h.
	uC_BASE_STM32MP157_PCLK_INDEX_AHB3,
	uC_BASE_STM32MP157_PCLK_INDEX_AHB4,
	uC_BASE_STM32MP157_PCLK_INDEX_APB1,
	uC_BASE_STM32MP157_PCLK_INDEX_APB2,
	uC_BASE_STM32MP157_PCLK_INDEX_APB3,
	uC_BASE_STM32MP157_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
	uC_BASE_STM32MP157_PCLK_INDEX_TIMER1_8_15_TO_17,
	uC_BASE_STM32MP157_PCLK_INDEX_TIMER_ADC,
	uC_BASE_STM32MP157_MAX_PCLKS
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
	uC_BASE_PIO_G,
	uC_BASE_PIO_H,
	uC_BASE_PIO_I,
	uC_BASE_PIO_J,
	uC_BASE_PIO_K,
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
	uC_BASE_TIMER_CTRL_4,
	uC_BASE_TIMER_CTRL_5,
	uC_BASE_TIMER_CTRL_6,
	uC_BASE_TIMER_CTRL_7,
	uC_BASE_TIMER_CTRL_8,
	uC_BASE_TIMER_CTRL_12,
	uC_BASE_TIMER_CTRL_13,
	uC_BASE_TIMER_CTRL_14,
	uC_BASE_TIMER_CTRL_15,
	uC_BASE_TIMER_CTRL_16,
	uC_BASE_TIMER_CTRL_17,
	uC_BASE_MAX_TIMER_CTRLS
};

/*
 *****************************************************************************************
 *      USART Application Peripheral ID's
 *			File: uC_USART
 *****************************************************************************************
 */
static const uint8_t uC_BASE_FS_USB_PORT = 0;
static const uint8_t uC_BASE_HS_FS_USB_PORT = 1;
static const uint8_t uC_BASE_MAX_USB_PORTS = 2;

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
	uC_BASE_UART_PORT_4,
	uC_BASE_UART_PORT_5,
	uC_BASE_USART_PORT_6,
	uC_BASE_UART_PORT_7,
	uC_BASE_UART_PORT_8,
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
	uC_BASE_PWM_CTRL_3 = uC_BASE_TIMER_CTRL_3,
	uC_BASE_PWM_CTRL_2 = uC_BASE_TIMER_CTRL_2,
	uC_BASE_PWM_CTRL_4 = uC_BASE_TIMER_CTRL_4,
	uC_BASE_PWM_CTRL_5 = uC_BASE_TIMER_CTRL_5,
	uC_BASE_PWM_CTRL_6 = uC_BASE_TIMER_CTRL_6,
	uC_BASE_PWM_CTRL_7 = uC_BASE_TIMER_CTRL_7,
	uC_BASE_PWM_CTRL_8 = uC_BASE_TIMER_CTRL_8,
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
	uC_BASE_ATOD_CTRL_3,
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
	uC_DMA1_STREAM_0,
	uC_DMA1_STREAM_1,
	uC_DMA1_STREAM_2,
	uC_DMA1_STREAM_3,
	uC_DMA1_STREAM_4,
	uC_DMA1_STREAM_5,
	uC_DMA1_STREAM_6,
	uC_DMA1_STREAM_7,
	uC_DMA2_STREAM_0,
	uC_DMA2_STREAM_1,
	uC_DMA2_STREAM_2,
	uC_DMA2_STREAM_3,
	uC_DMA2_STREAM_4,
	uC_DMA2_STREAM_5,
	uC_DMA2_STREAM_6,
	uC_DMA2_STREAM_7,
	uC_BASE_DMA_STREAM_MAX
};

#define uC_DMA_STREAM_DNE       ( static_cast<uint8_t>( uC_BASE_DMA_STREAM_MAX ) + 1U )

#define DMA_STREAM_CHANNEL_SHIFT        8U
// ********** DMA1 Stream definition options. ********************************
#define DMA_USART1TX_D1S0       ( static_cast<uint16_t>( uC_DMA1_STREAM_0 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_USART1_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART2TX_D1S1       ( static_cast<uint16_t>( uC_DMA1_STREAM_1 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_USART2_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART3TX_D1S2       ( static_cast<uint16_t>( uC_DMA1_STREAM_2 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_USART3_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART4TX_D1S3        ( static_cast<uint16_t>( uC_DMA1_STREAM_3 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_UART4_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART5TX_D1S4        ( static_cast<uint16_t>( uC_DMA1_STREAM_4 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_UART5_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART6TX_D1S5       ( static_cast<uint16_t>( uC_DMA1_STREAM_5 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_USART6_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART7TX_D1S6        ( static_cast<uint16_t>( uC_DMA1_STREAM_6 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_UART7_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART8TX_D1S7        ( static_cast<uint16_t>( uC_DMA1_STREAM_7 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_UART8_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )

#define DMA_SPI1TX_D1S5         ( static_cast<uint16_t>( uC_DMA1_STREAM_5 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_SPI1_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_SPI2TX_D1S6         ( static_cast<uint16_t>( uC_DMA1_STREAM_6 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_SPI2_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_SPI3TX_D1S7         ( static_cast<uint16_t>( uC_DMA1_STREAM_7 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_SPI3_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )

#define DMA_I2C1TX_D1S0         ( static_cast<uint16_t>( uC_DMA1_STREAM_0 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_I2C1_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_I2C2TX_D1S1         ( static_cast<uint16_t>( uC_DMA1_STREAM_1 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_I2C2_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_I2C3TX_D1S2         ( static_cast<uint16_t>( uC_DMA1_STREAM_2 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_I2C3_TX ) << DMA_STREAM_CHANNEL_SHIFT ) )


// ********** DMA2 Stream definition options. ********************************
#define DMA_USART1RX_D2S0       ( static_cast<uint16_t>( uC_DMA2_STREAM_0 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_USART1_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART2RX_D2S1       ( static_cast<uint16_t>( uC_DMA2_STREAM_1 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_USART2_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART3RX_D2S2       ( static_cast<uint16_t>( uC_DMA2_STREAM_2 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_USART3_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART4RX_D2S3        ( static_cast<uint16_t>( uC_DMA2_STREAM_3 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_UART4_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART5RX_D2S4        ( static_cast<uint16_t>( uC_DMA2_STREAM_4 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_UART5_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_USART6RX_D2S5       ( static_cast<uint16_t>( uC_DMA2_STREAM_5 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_USART6_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART7RX_D2S6        ( static_cast<uint16_t>( uC_DMA2_STREAM_6 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_UART7_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_UART8RX_D2S7        ( static_cast<uint16_t>( uC_DMA2_STREAM_7 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_UART8_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )

#define DMA_SPI1RX_D2S5         ( static_cast<uint16_t>( uC_DMA2_STREAM_5 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_SPI1_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_SPI2RX_D2S6         ( static_cast<uint16_t>( uC_DMA2_STREAM_6 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_SPI2_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_SPI3RX_D2S7         ( static_cast<uint16_t>( uC_DMA2_STREAM_7 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_SPI3_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )

#define DMA_I2C1RX_D2S0         ( static_cast<uint16_t>( uC_DMA2_STREAM_0 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_I2C1_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_I2C2RX_D2S1         ( static_cast<uint16_t>( uC_DMA2_STREAM_1 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_I2C2_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )
#define DMA_I2C3RX_D2S2         ( static_cast<uint16_t>( uC_DMA2_STREAM_2 ) | \
								  ( static_cast<uint16_t>( DMA_REQUEST_I2C3_RX ) << DMA_STREAM_CHANNEL_SHIFT ) )

/*
 *****************************************************************************************
 *      EMAC Controller Peripheral ID's
 *			File: uC_EMAC
 *****************************************************************************************
 */
/*
   enum
   {
    uC_BASE_EMAC_CTRL_0,
    uC_BASE_EMAC_MAX_CTRLS
   };
 */
static const uint8_t uC_BASE_EMAC_CTRL_0 = 0U;
static const uint8_t uC_BASE_EMAC_MAX_CTRLS = 1U;

#endif	// This is the endif for the ICC Check (Are we compiling for C/C++)
static const uint32_t SECTOR_ERASE_TIME_LARGEST_MS = 2000U;	/* Sector erase time in miliseconds. Considering x32 erase*/
static const uint32_t SECTOR_ERASE_TIME_BASELINE_MS = 400U;	/* Sector erase time in miliseconds. Considering x32 erase.
															   smallest sector size in STM32 is 16kb*/
static const uint32_t BYTE_WRITE_TIME_US = 16U;		/* Writing 4 byte data into flash memory */

typedef uint32_t sector_addr_t;
typedef uint32_t sector_num_t;
typedef struct
{
	sector_addr_t sector_size;
	sector_num_t sector_no;
} flash_page_t;
const flash_page_t INTERNAL_FLASH_PAGE_SIZES[13] =
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
	uC_BASE_CAN_CTRL_2,
	uC_BASE_CAN_MAX_CTRLS
};

#endif	// This is the endif for the multiple include.
