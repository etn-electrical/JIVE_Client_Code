/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 ******************************************************************************************
 */
#ifndef uC_BASE_HW_IDS_K60D10_H
#define uC_BASE_HW_IDS_K60D10_H

#include "uC_K60D10.h"

#ifdef __IAR_SYSTEMS_ICC__		// This makes sure that the assembly compile will not include the following.


#include "uC_IO_Define_K60D10.h"
// #include "Drv_Lib_Inc.h"


/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first of these freqs have to match the base clocks.
// The other clocks can be defined here.
// These first three must match the "Peripheral Power Definition Structure" in uc_Base.h
enum
{
	uC_BASE_K60D10_PCLK_INDEX_AHB1 = 0,
	uC_BASE_K60D10_MAX_PCLKS
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
};

const uint8_t uC_BASE_0_MAX_TIMER_CAP_COMP_CH = 0;
const uint8_t uC_BASE_2_MAX_TIMER_CAP_COMP_CH = 2;
const uint8_t uC_BASE_4_MAX_TIMER_CAP_COMP_CH = 4;
enum
{
	uC_BASE_TIMER_CTRL_0,
	uC_BASE_TIMER_CTRL_1,
	uC_BASE_TIMER_CTRL_2,
	uC_BASE_TIMER_CTRL_3,
	uC_BASE_MAX_TIMER_CTRLS
};

enum
{
	uC_BASE_LP_TIMER_CTRL_0,
	uC_BASE_MAX_LP_TIMER_CTRLS
};

/*
 *****************************************************************************************
 *      USART Application Peripheral ID's
 *			File: uC_USART
 *****************************************************************************************
 */
enum
{
	uC_BASE_USART_PORT_0,
	uC_BASE_USART_PORT_1,
	uC_BASE_USART_PORT_2,
	uC_BASE_USART_PORT_3,
	uC_BASE_USART_PORT_4,
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
	uC_BASE_SPI_CTRL_0,
	uC_BASE_SPI_CTRL_1,
	uC_BASE_SPI_CTRL_2,
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
	uC_BASE_I2C_CTRL_0,
	uC_BASE_I2C_CTRL_1,
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
	uC_BASE_EXT_INT_PORT_A,
	uC_BASE_EXT_INT_PORT_B,
	uC_BASE_EXT_INT_PORT_C,
	uC_BASE_EXT_INT_PORT_D,
	uC_BASE_EXT_INT_PORT_E,

	uC_BASE_EXT_INT_MAX_PORTS
};

/*
 *****************************************************************************************
 *      PWM Application Peripheral ID's
 *			File: uC_PWM
 *****************************************************************************************
 */
enum
{
	uC_BASE_PWM_MAX_CHANNELS
};

enum
{
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
	uC_BASE_ATOD_CTRL_3,
	uC_BASE_ATOD_CTRL_4,
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
	uC_ATOD_CHANNEL_ALL = 0xFF,
};

#define uC_ATOD_CHANNEL_MAX         19

#define ADC1_DMA_SELECT         DMA_ADC1_D2S4C0	// Alternate DMA definition

/*
 *****************************************************************************************
 *      DMA Application Peripheral ID's
 *			File: uC_DMA
 *****************************************************************************************
 */
enum
{
	uC_DMAMUX0_SRC_DISABLED = 0,
	// uC_DMAMUX0_SRC_RESERVED = 1,
	uC_DMAMUX0_SRC_UART0_RX = 2,
	uC_DMAMUX0_SRC_UART0_TX = 3,
	uC_DMAMUX0_SRC_UART1_RX = 4,
	uC_DMAMUX0_SRC_UART1_TX = 5,
	uC_DMAMUX0_SRC_UART2_RX = 6,
	uC_DMAMUX0_SRC_UART2_TX = 7,
	uC_DMAMUX0_SRC_UART3_RX = 8,
	uC_DMAMUX0_SRC_UART3_TX = 9,
	uC_DMAMUX0_SRC_UART4_RX = 10,
	uC_DMAMUX0_SRC_UART4_TX = 11,
	uC_DMAMUX0_SRC_UART5_RX = 12,
	uC_DMAMUX0_SRC_UART5_TX = 13,
	uC_DMAMUX0_SRC_I2S0_RX = 14,
	uC_DMAMUX0_SRC_I2S0_TX = 15,
	uC_DMAMUX0_SRC_SPI0_RX = 16,
	uC_DMAMUX0_SRC_SPI0_TX = 17,
	uC_DMAMUX0_SRC_SPI1_RX = 18,
	uC_DMAMUX0_SRC_SPI1_TX = 19,
	uC_DMAMUX0_SRC_SPI2_RX = 20,
	uC_DMAMUX0_SRC_SPI2_TX = 21,
	uC_DMAMUX0_SRC_I2C0 = 22,
	uC_DMAMUX0_SRC_I2C_1_2 = 23,
	uC_DMAMUX0_SRC_FTM0_0 = 24,
	uC_DMAMUX0_SRC_FTM0_1 = 25,
	uC_DMAMUX0_SRC_FTM0_2 = 26,
	uC_DMAMUX0_SRC_FTM0_3 = 27,
	uC_DMAMUX0_SRC_FTM0_4 = 28,
	uC_DMAMUX0_SRC_FTM0_5 = 29,
	uC_DMAMUX0_SRC_FTM0_6 = 30,
	uC_DMAMUX0_SRC_FTM0_7 = 31,
	uC_DMAMUX0_SRC_FTM1_0 = 32,
	uC_DMAMUX0_SRC_FTM1_1 = 33,
	uC_DMAMUX0_SRC_FTM2_0 = 34,
	uC_DMAMUX0_SRC_FTM2_1 = 35,
	uC_DMAMUX0_SRC_IEEE_T_0 = 36,
	uC_DMAMUX0_SRC_IEEE_T_1 = 37,
	uC_DMAMUX0_SRC_IEEE_T_2 = 38,
	uC_DMAMUX0_SRC_IEEE_T_3 = 39,
	uC_DMAMUX0_SRC_ADC0 = 40,
	uC_DMAMUX0_SRC_ADC1 = 41,
	uC_DMAMUX0_SRC_CMP0 = 42,
	uC_DMAMUX0_SRC_CMP1 = 43,
	uC_DMAMUX0_SRC_CMP2 = 44,
	uC_DMAMUX0_SRC_DAC0 = 45,
	uC_DMAMUX0_SRC_DAC1 = 46,
	uC_DMAMUX0_SRC_CMT = 47,
	uC_DMAMUX0_SRC_PDB = 48,
	uC_DMAMUX0_SRC_PCM_A = 49,
	uC_DMAMUX0_SRC_PCM_B = 50,
	uC_DMAMUX0_SRC_PCM_C = 51,
	uC_DMAMUX0_SRC_PCM_D = 52,
	uC_DMAMUX0_SRC_PCM_E = 53,
	// uC_DMAMUX0_SRC_DMAMUX   = 54,
	// uC_DMAMUX0_SRC_DMAMUX   = 55,
	// uC_DMAMUX0_SRC_DMAMUX   = 56,
	// uC_DMAMUX0_SRC_DMAMUX   = 57,
	// uC_DMAMUX0_SRC_DMAMUX   = 58,
	// uC_DMAMUX0_SRC_DMAMUX   = 59,
	// uC_DMAMUX0_SRC_DMAMUX   = 60,
	// uC_DMAMUX0_SRC_DMAMUX   = 61,
	// uC_DMAMUX0_SRC_DMAMUX   = 62,
	// uC_DMAMUX0_SRC_DMAMUX   = 63,

	uC_DMAMUX0_SRC_MAX = 64
};

enum
{
	uC_DMAMUX1_SRC_DISABLED = 0,
	// uC_DMAMUX1_SRC_RESERVED = 1,
	uC_DMAMUX1_SRC_UART0_RX = 2,
	uC_DMAMUX1_SRC_UART0_TX = 3,
	uC_DMAMUX1_SRC_UART1_RX = 4,
	uC_DMAMUX1_SRC_UART1_TX = 5,
	uC_DMAMUX1_SRC_UART2_RX = 6,
	uC_DMAMUX1_SRC_UART2_TX = 7,
	uC_DMAMUX1_SRC_UART3_RX = 8,
	uC_DMAMUX1_SRC_UART3_TX = 9,
	uC_DMAMUX1_SRC_UART4_RX = 10,
	uC_DMAMUX1_SRC_UART4_TX = 11,
	uC_DMAMUX1_SRC_UART5_RX = 12,
	uC_DMAMUX1_SRC_UART5_TX = 13,
	uC_DMAMUX1_SRC_I2S1_RX = 14,
	uC_DMAMUX1_SRC_I2S1_TX = 15,
	uC_DMAMUX1_SRC_SPI0_RX = 16,
	uC_DMAMUX1_SRC_SPI0_TX = 17,
	uC_DMAMUX1_SRC_SPI1_RX = 18,
	uC_DMAMUX1_SRC_SPI1_TX = 19,
	uC_DMAMUX1_SRC_SPI2_RX = 20,
	uC_DMAMUX1_SRC_SPI2_TX = 21,
	// uC_DMAMUX1_SRC_RESERVED = 22,
	// uC_DMAMUX1_SRC_RESERVED = 23,
	uC_DMAMUX1_SRC_FTM3_0 = 24,
	uC_DMAMUX1_SRC_FTM3_1 = 25,
	uC_DMAMUX1_SRC_FTM3_2 = 26,
	uC_DMAMUX1_SRC_FTM3_3 = 27,
	uC_DMAMUX1_SRC_FTM3_4 = 28,
	uC_DMAMUX1_SRC_FTM3_5 = 29,
	uC_DMAMUX1_SRC_FTM3_6 = 30,
	uC_DMAMUX1_SRC_FTM3_7 = 31,
	// uC_DMAMUX1_SRC_RESERVED = 32,
	// uC_DMAMUX1_SRC_RESERVED = 33,
	// uC_DMAMUX1_SRC_RESERVED = 34,
	// uC_DMAMUX1_SRC_RESERVED = 35,
	uC_DMAMUX1_SRC_IEEE_T_0 = 36,
	uC_DMAMUX1_SRC_IEEE_T_1 = 37,
	uC_DMAMUX1_SRC_IEEE_T_2 = 38,
	uC_DMAMUX1_SRC_IEEE_T_3 = 39,
	uC_DMAMUX1_SRC_ADC0 = 40,
	uC_DMAMUX1_SRC_ADC1 = 41,
	uC_DMAMUX1_SRC_ADC2 = 42,
	uC_DMAMUX1_SRC_ADC3 = 43,
	// uC_DMAMUX1_SRC_RESERVED = 44,
	uC_DMAMUX1_SRC_DAC0 = 45,
	uC_DMAMUX1_SRC_DAC1 = 46,
	uC_DMAMUX1_SRC_CMP0 = 47,
	uC_DMAMUX1_SRC_CMP1 = 48,
	uC_DMAMUX1_SRC_CMP2 = 49,
	uC_DMAMUX1_SRC_CMP3 = 50,
	// uC_DMAMUX1_SRC_RESERVED = 51,
	// uC_DMAMUX1_SRC_RESERVED = 52,
	uC_DMAMUX1_SRC_PCM_F = 53,
	// uC_DMAMUX1_SRC_DMAMUX   = 54,
	// uC_DMAMUX1_SRC_DMAMUX   = 55,
	// uC_DMAMUX1_SRC_DMAMUX   = 56,
	// uC_DMAMUX1_SRC_DMAMUX   = 57,
	// uC_DMAMUX1_SRC_DMAMUX   = 58,
	// uC_DMAMUX1_SRC_DMAMUX   = 59,
	// uC_DMAMUX1_SRC_DMAMUX   = 60,
	// uC_DMAMUX1_SRC_DMAMUX   = 61,
	// uC_DMAMUX1_SRC_DMAMUX   = 62,
	// uC_DMAMUX1_SRC_DMAMUX   = 63,

	uC_DMAMUX1_SRC_MAX = 64
};

enum
{
	uC_DMAMUX_STREAM_0,
	uC_DMAMUX_STREAM_1,
	uC_DMAMUX_STREAM_2,
	uC_DMAMUX_STREAM_3,
	uC_DMAMUX_STREAM_4,
	uC_DMAMUX_STREAM_5,
	uC_DMAMUX_STREAM_6,
	uC_DMAMUX_STREAM_7,
	uC_DMAMUX_STREAM_8,
	uC_DMAMUX_STREAM_9,
	uC_DMAMUX_STREAM_10,
	uC_DMAMUX_STREAM_11,
	uC_DMAMUX_STREAM_12,
	uC_DMAMUX_STREAM_13,
	uC_DMAMUX_STREAM_14,
	uC_DMAMUX_STREAM_15,
	uC_BASE_DMAMUX_STREAM_MAX
};

#define uC_DMAMUX_STREAM_DNE        ( uC_BASE_DMAMUX_STREAM_MAX + 1 )

enum
{
	uC_DMA_CHANNEL_0,
	uC_DMA_CHANNEL_1,
	uC_DMA_CHANNEL_2,
	uC_DMA_CHANNEL_3,
	uC_DMA_CHANNEL_4,
	uC_DMA_CHANNEL_5,
	uC_DMA_CHANNEL_6,
	uC_DMA_CHANNEL_7,
	uC_DMA_CHANNEL_8,
	uC_DMA_CHANNEL_9,
	uC_DMA_CHANNEL_10,
	uC_DMA_CHANNEL_11,
	uC_DMA_CHANNEL_12,
	uC_DMA_CHANNEL_13,
	uC_DMA_CHANNEL_14,
	uC_DMA_CHANNEL_15,
	uC_DMA_CHANNEL_16,
	uC_DMA_CHANNEL_17,
	uC_DMA_CHANNEL_18,
	uC_DMA_CHANNEL_19,
	uC_DMA_CHANNEL_20,
	uC_DMA_CHANNEL_21,
	uC_DMA_CHANNEL_22,
	uC_DMA_CHANNEL_23,
	uC_DMA_CHANNEL_24,
	uC_DMA_CHANNEL_25,
	uC_DMA_CHANNEL_26,
	uC_DMA_CHANNEL_27,
	uC_DMA_CHANNEL_28,
	uC_DMA_CHANNEL_29,
	uC_DMA_CHANNEL_30,
	uC_DMA_CHANNEL_31,
	uC_BASE_DMA_CHANNEL_MAX
};

#define uC_DMA_STREAM_DNE           ( uC_BASE_DMA_CHANNEL_MAX + 1 )

// UART 0
#define DMA_USART0RX    uC_DMA_CHANNEL_2
#define DMA_USART0TX    uC_DMA_CHANNEL_3
// UART 1
#define DMA_USART1RX    uC_DMA_CHANNEL_0
#define DMA_USART1TX    uC_DMA_CHANNEL_1
// UART 2
#define DMA_USART2RX    uC_DMA_CHANNEL_4
#define DMA_USART2TX    uC_DMA_CHANNEL_5
// UART 3
#define DMA_USART3RX    uC_DMA_CHANNEL_8
#define DMA_USART3TX    uC_DMA_CHANNEL_9
// UART 4
#define DMA_USART4RX    uC_DMA_CHANNEL_6
#define DMA_USART4TX    uC_DMA_CHANNEL_7
// UART 5
#define DMA_USART5RX    uC_DMA_CHANNEL_12
#define DMA_USART5TX    uC_DMA_CHANNEL_13
// SPI
#define DMA_SPI2RX      uC_DMA_CHANNEL_10
#define DMA_SPI2TX      uC_DMA_CHANNEL_11

/*
 *****************************************************************************************
 *      EMAC Controller Peripheral ID's
 *			File: uC_EMAC
 *****************************************************************************************
 */
enum
{
	uC_BASE_EMAC_CTRL_0,
	uC_BASE_EMAC_MAX_CTRLS
};

static const uint32_t SECTOR_ERASE_TIME_LARGEST_MS = 113U;	/* Sector erase time in miliseconds. Considering x32 erase*/
static const uint32_t SECTOR_ERASE_TIME_BASELINE_MS = 15U;	/* Sector erase time in miliseconds. Considering x32 erase.
															   smallest sector size in STM32 is 16kb*/
static const uint32_t BYTE_WRITE_TIME_US = 16U;		/* Writing 4 byte data into flash memory */

typedef uint32_t sector_addr_t;
typedef uint32_t sector_num_t;
typedef struct
{
	sector_addr_t sector_size;
	sector_num_t sector_no;
} flash_page_t;
const flash_page_t INTERNAL_FLASH_PAGE_SIZES[257] =
{
	{0x800, 0},
	{0x800, 1},
	{0x800, 2},
	{0x800, 3},
	{0x800, 4},
	{0x800, 5},
	{0x800, 6},
	{0x800, 7},
	{0x800, 8},
	{0x800, 9},
	{0x800, 10},
	{0x800, 11},
	{0x800, 12},
	{0x800, 13},
	{0x800, 14},
	{0x800, 15},
	{0x800, 16},
	{0x800, 17},
	{0x800, 18},
	{0x800, 19},
	{0x800, 20},
	{0x800, 21},
	{0x800, 22},
	{0x800, 23},
	{0x800, 24},
	{0x800, 25},
	{0x800, 26},
	{0x800, 27},
	{0x800, 28},
	{0x800, 29},
	{0x800, 30},
	{0x800, 31},
	{0x800, 32},
	{0x800, 33},
	{0x800, 34},
	{0x800, 35},
	{0x800, 36},
	{0x800, 37},
	{0x800, 38},
	{0x800, 39},
	{0x800, 40},
	{0x800, 41},
	{0x800, 42},
	{0x800, 43},
	{0x800, 44},
	{0x800, 45},
	{0x800, 46},
	{0x800, 47},
	{0x800, 48},
	{0x800, 49},
	{0x800, 50},
	{0x800, 51},
	{0x800, 52},
	{0x800, 53},
	{0x800, 54},
	{0x800, 55},
	{0x800, 56},
	{0x800, 57},
	{0x800, 58},
	{0x800, 59},
	{0x800, 60},
	{0x800, 61},
	{0x800, 62},
	{0x800, 63},
	{0x800, 64},
	{0x800, 65},
	{0x800, 66},
	{0x800, 67},
	{0x800, 68},
	{0x800, 69},
	{0x800, 70},
	{0x800, 71},
	{0x800, 72},
	{0x800, 73},
	{0x800, 74},
	{0x800, 75},
	{0x800, 76},
	{0x800, 77},
	{0x800, 78},
	{0x800, 79},
	{0x800, 80},
	{0x800, 81},
	{0x800, 82},
	{0x800, 83},
	{0x800, 84},
	{0x800, 85},
	{0x800, 86},
	{0x800, 87},
	{0x800, 88},
	{0x800, 89},
	{0x800, 90},
	{0x800, 91},
	{0x800, 92},
	{0x800, 93},
	{0x800, 94},
	{0x800, 95},
	{0x800, 96},
	{0x800, 97},
	{0x800, 98},
	{0x800, 99},
	{0x800, 100},
	{0x800, 101},
	{0x800, 102},
	{0x800, 103},
	{0x800, 104},
	{0x800, 105},
	{0x800, 106},
	{0x800, 107},
	{0x800, 108},
	{0x800, 109},
	{0x800, 110},
	{0x800, 111},
	{0x800, 112},
	{0x800, 113},
	{0x800, 114},
	{0x800, 115},
	{0x800, 116},
	{0x800, 117},
	{0x800, 118},
	{0x800, 119},
	{0x800, 120},
	{0x800, 121},
	{0x800, 122},
	{0x800, 123},
	{0x800, 124},
	{0x800, 125},
	{0x800, 126},
	{0x800, 127},
	{0x800, 128},
	{0x800, 129},
	{0x800, 130},
	{0x800, 131},
	{0x800, 132},
	{0x800, 133},
	{0x800, 134},
	{0x800, 135},
	{0x800, 136},
	{0x800, 137},
	{0x800, 138},
	{0x800, 139},
	{0x800, 140},
	{0x800, 141},
	{0x800, 142},
	{0x800, 143},
	{0x800, 144},
	{0x800, 145},
	{0x800, 146},
	{0x800, 147},
	{0x800, 148},
	{0x800, 149},
	{0x800, 150},
	{0x800, 151},
	{0x800, 152},
	{0x800, 153},
	{0x800, 154},
	{0x800, 155},
	{0x800, 156},
	{0x800, 157},
	{0x800, 158},
	{0x800, 159},
	{0x800, 160},
	{0x800, 161},
	{0x800, 162},
	{0x800, 163},
	{0x800, 164},
	{0x800, 165},
	{0x800, 166},
	{0x800, 167},
	{0x800, 168},
	{0x800, 169},
	{0x800, 170},
	{0x800, 171},
	{0x800, 172},
	{0x800, 173},
	{0x800, 174},
	{0x800, 175},
	{0x800, 176},
	{0x800, 177},
	{0x800, 178},
	{0x800, 179},
	{0x800, 180},
	{0x800, 181},
	{0x800, 182},
	{0x800, 183},
	{0x800, 184},
	{0x800, 185},
	{0x800, 186},
	{0x800, 187},
	{0x800, 188},
	{0x800, 189},
	{0x800, 190},
	{0x800, 191},
	{0x800, 192},
	{0x800, 193},
	{0x800, 194},
	{0x800, 195},
	{0x800, 196},
	{0x800, 197},
	{0x800, 198},
	{0x800, 199},
	{0x800, 200},
	{0x800, 201},
	{0x800, 202},
	{0x800, 203},
	{0x800, 204},
	{0x800, 205},
	{0x800, 206},
	{0x800, 207},
	{0x800, 208},
	{0x800, 209},
	{0x800, 210},
	{0x800, 211},
	{0x800, 212},
	{0x800, 213},
	{0x800, 214},
	{0x800, 215},
	{0x800, 216},
	{0x800, 217},
	{0x800, 218},
	{0x800, 219},
	{0x800, 220},
	{0x800, 221},
	{0x800, 222},
	{0x800, 223},
	{0x800, 224},
	{0x800, 225},
	{0x800, 226},
	{0x800, 227},
	{0x800, 228},
	{0x800, 229},
	{0x800, 230},
	{0x800, 231},
	{0x800, 232},
	{0x800, 233},
	{0x800, 234},
	{0x800, 235},
	{0x800, 236},
	{0x800, 237},
	{0x800, 238},
	{0x800, 239},
	{0x800, 240},
	{0x800, 241},
	{0x800, 242},
	{0x800, 243},
	{0x800, 244},
	{0x800, 245},
	{0x800, 246},
	{0x800, 247},
	{0x800, 248},
	{0x800, 249},
	{0x800, 250},
	{0x800, 251},
	{0x800, 252},
	{0x800, 253},
	{0x800, 254},
	{0x800, 255},
	{0x00000, 0xFFFF}
};
const UINT32 INTERNAL_FLASH_START_ADDRESS = 0x00000000U;
#endif	// This is the endif for the ICC Check (Are we compiling for C/C++)

#endif	// This is the endif for the multiple include.
