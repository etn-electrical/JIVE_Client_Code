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
#ifndef uC_BASE_HW_IDS_K66F18_H
#define uC_BASE_HW_IDS_K66F18_H

#include "uC_K66F18.h"

#ifdef __IAR_SYSTEMS_ICC__		// This makes sure that the assembly compile will not include the following.


#include "uC_IO_Define_K66F18.h"
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
	uC_BASE_K66F18_PCLK_INDEX_AHB1 = 0,
	uC_BASE_K66F18_MAX_PCLKS
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

// Strip 1
#define DMA_USART1RX    uC_DMA_CHANNEL_0
#define DMA_USART1TX    uC_DMA_CHANNEL_1
// Strip 2
#define DMA_USART0RX    uC_DMA_CHANNEL_2
#define DMA_USART0TX    uC_DMA_CHANNEL_3
// Strip 3
#define DMA_USART2RX    uC_DMA_CHANNEL_4
#define DMA_USART2TX    uC_DMA_CHANNEL_5
// Strip 4
#define DMA_USART4RX    uC_DMA_CHANNEL_6
#define DMA_USART4TX    uC_DMA_CHANNEL_7
// MODBUS
#define DMA_USART3RX    uC_DMA_CHANNEL_8
#define DMA_USART3TX    uC_DMA_CHANNEL_9
// FRAM
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

static const uint32_t SECTOR_ERASE_TIME_LARGEST_MS = 115;	/* Sector erase time in miliseconds. Considering x32 erase*/
static const uint32_t SECTOR_ERASE_TIME_BASELINE_MS = 30U;	/* Sector erase time in miliseconds. Considering x32 erase.
															   smallest sector size in STM32 is 16kb*/
static const uint32_t BYTE_WRITE_TIME_US = 16U;		/* Writing 4 byte data into flash memory */

typedef uint32_t sector_addr_t;
typedef uint32_t sector_num_t;
typedef struct
{
	sector_addr_t sector_size;
	sector_num_t sector_no;
} flash_page_t;
const flash_page_t INTERNAL_FLASH_PAGE_SIZES[513] =
{
	{   0X1000,   0   },
	{   0X1000,   1   },
	{   0X1000,   2   },
	{   0X1000,   3   },
	{   0X1000,   4   },
	{   0X1000,   5   },
	{   0X1000,   6   },
	{   0X1000,   7   },
	{   0X1000,   8   },
	{   0X1000,   9   },
	{   0X1000,   10  },
	{   0X1000,   11  },
	{   0X1000,   12  },
	{   0X1000,   13  },
	{   0X1000,   14  },
	{   0X1000,   15  },
	{   0X1000,   16  },
	{   0X1000,   17  },
	{   0X1000,   18  },
	{   0X1000,   19  },
	{   0X1000,   20  },
	{   0X1000,   21  },
	{   0X1000,   22  },
	{   0X1000,   23  },
	{   0X1000,   24  },
	{   0X1000,   25  },
	{   0X1000,   26  },
	{   0X1000,   27  },
	{   0X1000,   28  },
	{   0X1000,   29  },
	{   0X1000,   30  },
	{   0X1000,   31  },
	{   0X1000,   32  },
	{   0X1000,   33  },
	{   0X1000,   34  },
	{   0X1000,   35  },
	{   0X1000,   36  },
	{   0X1000,   37  },
	{   0X1000,   38  },
	{   0X1000,   39  },
	{   0X1000,   40  },
	{   0X1000,   41  },
	{   0X1000,   42  },
	{   0X1000,   43  },
	{   0X1000,   44  },
	{   0X1000,   45  },
	{   0X1000,   46  },
	{   0X1000,   47  },
	{   0X1000,   48  },
	{   0X1000,   49  },
	{   0X1000,   50  },
	{   0X1000,   51  },
	{   0X1000,   52  },
	{   0X1000,   53  },
	{   0X1000,   54  },
	{   0X1000,   55  },
	{   0X1000,   56  },
	{   0X1000,   57  },
	{   0X1000,   58  },
	{   0X1000,   59  },
	{   0X1000,   60  },
	{   0X1000,   61  },
	{   0X1000,   62  },
	{   0X1000,   63  },
	{   0X1000,   64  },
	{   0X1000,   65  },
	{   0X1000,   66  },
	{   0X1000,   67  },
	{   0X1000,   68  },
	{   0X1000,   69  },
	{   0X1000,   70  },
	{   0X1000,   71  },
	{   0X1000,   72  },
	{   0X1000,   73  },
	{   0X1000,   74  },
	{   0X1000,   75  },
	{   0X1000,   76  },
	{   0X1000,   77  },
	{   0X1000,   78  },
	{   0X1000,   79  },
	{   0X1000,   80  },
	{   0X1000,   81  },
	{   0X1000,   82  },
	{   0X1000,   83  },
	{   0X1000,   84  },
	{   0X1000,   85  },
	{   0X1000,   86  },
	{   0X1000,   87  },
	{   0X1000,   88  },
	{   0X1000,   89  },
	{   0X1000,   90  },
	{   0X1000,   91  },
	{   0X1000,   92  },
	{   0X1000,   93  },
	{   0X1000,   94  },
	{   0X1000,   95  },
	{   0X1000,   96  },
	{   0X1000,   97  },
	{   0X1000,   98  },
	{   0X1000,   99  },
	{   0X1000,   100 },
	{   0X1000,   101 },
	{   0X1000,   102 },
	{   0X1000,   103 },
	{   0X1000,   104 },
	{   0X1000,   105 },
	{   0X1000,   106 },
	{   0X1000,   107 },
	{   0X1000,   108 },
	{   0X1000,   109 },
	{   0X1000,   110 },
	{   0X1000,   111 },
	{   0X1000,   112 },
	{   0X1000,   113 },
	{   0X1000,   114 },
	{   0X1000,   115 },
	{   0X1000,   116 },
	{   0X1000,   117 },
	{   0X1000,   118 },
	{   0X1000,   119 },
	{   0X1000,   120 },
	{   0X1000,   121 },
	{   0X1000,   122 },
	{   0X1000,   123 },
	{   0X1000,   124 },
	{   0X1000,   125 },
	{   0X1000,   126 },
	{   0X1000,   127 },
	{   0X1000,   128 },
	{   0X1000,   129 },
	{   0X1000,   130 },
	{   0X1000,   131 },
	{   0X1000,   132 },
	{   0X1000,   133 },
	{   0X1000,   134 },
	{   0X1000,   135 },
	{   0X1000,   136 },
	{   0X1000,   137 },
	{   0X1000,   138 },
	{   0X1000,   139 },
	{   0X1000,   140 },
	{   0X1000,   141 },
	{   0X1000,   142 },
	{   0X1000,   143 },
	{   0X1000,   144 },
	{   0X1000,   145 },
	{   0X1000,   146 },
	{   0X1000,   147 },
	{   0X1000,   148 },
	{   0X1000,   149 },
	{   0X1000,   150 },
	{   0X1000,   151 },
	{   0X1000,   152 },
	{   0X1000,   153 },
	{   0X1000,   154 },
	{   0X1000,   155 },
	{   0X1000,   156 },
	{   0X1000,   157 },
	{   0X1000,   158 },
	{   0X1000,   159 },
	{   0X1000,   160 },
	{   0X1000,   161 },
	{   0X1000,   162 },
	{   0X1000,   163 },
	{   0X1000,   164 },
	{   0X1000,   165 },
	{   0X1000,   166 },
	{   0X1000,   167 },
	{   0X1000,   168 },
	{   0X1000,   169 },
	{   0X1000,   170 },
	{   0X1000,   171 },
	{   0X1000,   172 },
	{   0X1000,   173 },
	{   0X1000,   174 },
	{   0X1000,   175 },
	{   0X1000,   176 },
	{   0X1000,   177 },
	{   0X1000,   178 },
	{   0X1000,   179 },
	{   0X1000,   180 },
	{   0X1000,   181 },
	{   0X1000,   182 },
	{   0X1000,   183 },
	{   0X1000,   184 },
	{   0X1000,   185 },
	{   0X1000,   186 },
	{   0X1000,   187 },
	{   0X1000,   188 },
	{   0X1000,   189 },
	{   0X1000,   190 },
	{   0X1000,   191 },
	{   0X1000,   192 },
	{   0X1000,   193 },
	{   0X1000,   194 },
	{   0X1000,   195 },
	{   0X1000,   196 },
	{   0X1000,   197 },
	{   0X1000,   198 },
	{   0X1000,   199 },
	{   0X1000,   200 },
	{   0X1000,   201 },
	{   0X1000,   202 },
	{   0X1000,   203 },
	{   0X1000,   204 },
	{   0X1000,   205 },
	{   0X1000,   206 },
	{   0X1000,   207 },
	{   0X1000,   208 },
	{   0X1000,   209 },
	{   0X1000,   210 },
	{   0X1000,   211 },
	{   0X1000,   212 },
	{   0X1000,   213 },
	{   0X1000,   214 },
	{   0X1000,   215 },
	{   0X1000,   216 },
	{   0X1000,   217 },
	{   0X1000,   218 },
	{   0X1000,   219 },
	{   0X1000,   220 },
	{   0X1000,   221 },
	{   0X1000,   222 },
	{   0X1000,   223 },
	{   0X1000,   224 },
	{   0X1000,   225 },
	{   0X1000,   226 },
	{   0X1000,   227 },
	{   0X1000,   228 },
	{   0X1000,   229 },
	{   0X1000,   230 },
	{   0X1000,   231 },
	{   0X1000,   232 },
	{   0X1000,   233 },
	{   0X1000,   234 },
	{   0X1000,   235 },
	{   0X1000,   236 },
	{   0X1000,   237 },
	{   0X1000,   238 },
	{   0X1000,   239 },
	{   0X1000,   240 },
	{   0X1000,   241 },
	{   0X1000,   242 },
	{   0X1000,   243 },
	{   0X1000,   244 },
	{   0X1000,   245 },
	{   0X1000,   246 },
	{   0X1000,   247 },
	{   0X1000,   248 },
	{   0X1000,   249 },
	{   0X1000,   250 },
	{   0X1000,   251 },
	{   0X1000,   252 },
	{   0X1000,   253 },
	{   0X1000,   254 },
	{   0X1000,   255 },
	{   0X1000,   256 },
	{   0X1000,   257 },
	{   0X1000,   258 },
	{   0X1000,   259 },
	{   0X1000,   260 },
	{   0X1000,   261 },
	{   0X1000,   262 },
	{   0X1000,   263 },
	{   0X1000,   264 },
	{   0X1000,   265 },
	{   0X1000,   266 },
	{   0X1000,   267 },
	{   0X1000,   268 },
	{   0X1000,   269 },
	{   0X1000,   270 },
	{   0X1000,   271 },
	{   0X1000,   272 },
	{   0X1000,   273 },
	{   0X1000,   274 },
	{   0X1000,   275 },
	{   0X1000,   276 },
	{   0X1000,   277 },
	{   0X1000,   278 },
	{   0X1000,   279 },
	{   0X1000,   280 },
	{   0X1000,   281 },
	{   0X1000,   282 },
	{   0X1000,   283 },
	{   0X1000,   284 },
	{   0X1000,   285 },
	{   0X1000,   286 },
	{   0X1000,   287 },
	{   0X1000,   288 },
	{   0X1000,   289 },
	{   0X1000,   290 },
	{   0X1000,   291 },
	{   0X1000,   292 },
	{   0X1000,   293 },
	{   0X1000,   294 },
	{   0X1000,   295 },
	{   0X1000,   296 },
	{   0X1000,   297 },
	{   0X1000,   298 },
	{   0X1000,   299 },
	{   0X1000,   300 },
	{   0X1000,   301 },
	{   0X1000,   302 },
	{   0X1000,   303 },
	{   0X1000,   304 },
	{   0X1000,   305 },
	{   0X1000,   306 },
	{   0X1000,   307 },
	{   0X1000,   308 },
	{   0X1000,   309 },
	{   0X1000,   310 },
	{   0X1000,   311 },
	{   0X1000,   312 },
	{   0X1000,   313 },
	{   0X1000,   314 },
	{   0X1000,   315 },
	{   0X1000,   316 },
	{   0X1000,   317 },
	{   0X1000,   318 },
	{   0X1000,   319 },
	{   0X1000,   320 },
	{   0X1000,   321 },
	{   0X1000,   322 },
	{   0X1000,   323 },
	{   0X1000,   324 },
	{   0X1000,   325 },
	{   0X1000,   326 },
	{   0X1000,   327 },
	{   0X1000,   328 },
	{   0X1000,   329 },
	{   0X1000,   330 },
	{   0X1000,   331 },
	{   0X1000,   332 },
	{   0X1000,   333 },
	{   0X1000,   334 },
	{   0X1000,   335 },
	{   0X1000,   336 },
	{   0X1000,   337 },
	{   0X1000,   338 },
	{   0X1000,   339 },
	{   0X1000,   340 },
	{   0X1000,   341 },
	{   0X1000,   342 },
	{   0X1000,   343 },
	{   0X1000,   344 },
	{   0X1000,   345 },
	{   0X1000,   346 },
	{   0X1000,   347 },
	{   0X1000,   348 },
	{   0X1000,   349 },
	{   0X1000,   350 },
	{   0X1000,   351 },
	{   0X1000,   352 },
	{   0X1000,   353 },
	{   0X1000,   354 },
	{   0X1000,   355 },
	{   0X1000,   356 },
	{   0X1000,   357 },
	{   0X1000,   358 },
	{   0X1000,   359 },
	{   0X1000,   360 },
	{   0X1000,   361 },
	{   0X1000,   362 },
	{   0X1000,   363 },
	{   0X1000,   364 },
	{   0X1000,   365 },
	{   0X1000,   366 },
	{   0X1000,   367 },
	{   0X1000,   368 },
	{   0X1000,   369 },
	{   0X1000,   370 },
	{   0X1000,   371 },
	{   0X1000,   372 },
	{   0X1000,   373 },
	{   0X1000,   374 },
	{   0X1000,   375 },
	{   0X1000,   376 },
	{   0X1000,   377 },
	{   0X1000,   378 },
	{   0X1000,   379 },
	{   0X1000,   380 },
	{   0X1000,   381 },
	{   0X1000,   382 },
	{   0X1000,   383 },
	{   0X1000,   384 },
	{   0X1000,   385 },
	{   0X1000,   386 },
	{   0X1000,   387 },
	{   0X1000,   388 },
	{   0X1000,   389 },
	{   0X1000,   390 },
	{   0X1000,   391 },
	{   0X1000,   392 },
	{   0X1000,   393 },
	{   0X1000,   394 },
	{   0X1000,   395 },
	{   0X1000,   396 },
	{   0X1000,   397 },
	{   0X1000,   398 },
	{   0X1000,   399 },
	{   0X1000,   400 },
	{   0X1000,   401 },
	{   0X1000,   402 },
	{   0X1000,   403 },
	{   0X1000,   404 },
	{   0X1000,   405 },
	{   0X1000,   406 },
	{   0X1000,   407 },
	{   0X1000,   408 },
	{   0X1000,   409 },
	{   0X1000,   410 },
	{   0X1000,   411 },
	{   0X1000,   412 },
	{   0X1000,   413 },
	{   0X1000,   414 },
	{   0X1000,   415 },
	{   0X1000,   416 },
	{   0X1000,   417 },
	{   0X1000,   418 },
	{   0X1000,   419 },
	{   0X1000,   420 },
	{   0X1000,   421 },
	{   0X1000,   422 },
	{   0X1000,   423 },
	{   0X1000,   424 },
	{   0X1000,   425 },
	{   0X1000,   426 },
	{   0X1000,   427 },
	{   0X1000,   428 },
	{   0X1000,   429 },
	{   0X1000,   430 },
	{   0X1000,   431 },
	{   0X1000,   432 },
	{   0X1000,   433 },
	{   0X1000,   434 },
	{   0X1000,   435 },
	{   0X1000,   436 },
	{   0X1000,   437 },
	{   0X1000,   438 },
	{   0X1000,   439 },
	{   0X1000,   440 },
	{   0X1000,   441 },
	{   0X1000,   442 },
	{   0X1000,   443 },
	{   0X1000,   444 },
	{   0X1000,   445 },
	{   0X1000,   446 },
	{   0X1000,   447 },
	{   0X1000,   448 },
	{   0X1000,   449 },
	{   0X1000,   450 },
	{   0X1000,   451 },
	{   0X1000,   452 },
	{   0X1000,   453 },
	{   0X1000,   454 },
	{   0X1000,   455 },
	{   0X1000,   456 },
	{   0X1000,   457 },
	{   0X1000,   458 },
	{   0X1000,   459 },
	{   0X1000,   460 },
	{   0X1000,   461 },
	{   0X1000,   462 },
	{   0X1000,   463 },
	{   0X1000,   464 },
	{   0X1000,   465 },
	{   0X1000,   466 },
	{   0X1000,   467 },
	{   0X1000,   468 },
	{   0X1000,   469 },
	{   0X1000,   470 },
	{   0X1000,   471 },
	{   0X1000,   472 },
	{   0X1000,   473 },
	{   0X1000,   474 },
	{   0X1000,   475 },
	{   0X1000,   476 },
	{   0X1000,   477 },
	{   0X1000,   478 },
	{   0X1000,   479 },
	{   0X1000,   480 },
	{   0X1000,   481 },
	{   0X1000,   482 },
	{   0X1000,   483 },
	{   0X1000,   484 },
	{   0X1000,   485 },
	{   0X1000,   486 },
	{   0X1000,   487 },
	{   0X1000,   488 },
	{   0X1000,   489 },
	{   0X1000,   490 },
	{   0X1000,   491 },
	{   0X1000,   492 },
	{   0X1000,   493 },
	{   0X1000,   494 },
	{   0X1000,   495 },
	{   0X1000,   496 },
	{   0X1000,   497 },
	{   0X1000,   498 },
	{   0X1000,   499 },
	{   0X1000,   500 },
	{   0X1000,   501 },
	{   0X1000,   502 },
	{   0X1000,   503 },
	{   0X1000,   504 },
	{   0X1000,   505 },
	{   0X1000,   506 },
	{   0X1000,   507 },
	{   0X1000,   508 },
	{   0X1000,   509 },
	{   0X1000,   510 },
	{   0X1000,   511 },

	{0x00000, 0xFFFFF}
	// {0x00000, 0xFFFF}

};
const UINT32 INTERNAL_FLASH_START_ADDRESS = 0x00000000U;
#endif	// This is the endif for the ICC Check (Are we compiling for C/C++)

#endif	// This is the endif for the multiple include.
