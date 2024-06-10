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
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_BASE_HW_IDS_STM32WB55R_H
   #define uC_BASE_HW_IDS_STM32WB55R_H

#include "uC_STM32WB55R.h"



#include "uC_IO_Define_STM32WB55R.h"


/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first of these freqs have to match the base clocks.
// The other clocks can be defined here.
enum
{
	uC_BASE_STM32WB55R_PCLK_INDEX_AHB1,		// These first three must match the "Peripheral Power Definition Structure"
	uC_BASE_STM32WB55R_PCLK_INDEX_AHB2,		// enumeration in uC_Base.h.
	uC_BASE_STM32WB55R_PCLK_INDEX_AHB3,
	uC_BASE_STM32WB55R_PCLK_INDEX_AHB4,
	uC_BASE_STM32WB55R_PCLK_INDEX_AHB5,

	uC_BASE_STM32WB55R_PCLK_INDEX_APB1_REG1,
	uC_BASE_STM32WB55R_PCLK_INDEX_APB1_REG2,
	uC_BASE_STM32WB55R_PCLK_INDEX_APB2,
	uC_BASE_STM32WB55R_PCLK_INDEX_APB3,

	uC_BASE_STM32WB55R_PCLK_INDEX_TIMERS2to7,	// TODO check w.r.t datasheet
	uC_BASE_STM32WB55R_PCLK_INDEX_TIMER1and8,	// TODO check w.r.t datasheet

	uC_BASE_STM32WB55R_PCLK_INDEX_TIMER_ADC,

	uC_BASE_STM32WB55R_MAX_PCLKS
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
	uC_BASE_TIMER_CTRL_16,
	uC_BASE_TIMER_CTRL_17,
	uC_BASE_MAX_TIMER_CTRLS
};

enum
{
	uC_BASE_LPTIMER_CTRL_1,
	uC_BASE_LPTIMER_CTRL_2,
	uC_BASE_MAX_LPTIMER_CTRLS
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
 *      USART and LPUART Application Peripheral ID's
 *			File: uC_USART
 *****************************************************************************************
 */
enum
{
	uC_BASE_USART_PORT_1,
	uC_BASE_LPUART_PORT_1,
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
	// uC_BASE_I2C_CTRL_2,
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
	// uC_BASE_PWM_CTRL_3 = uC_BASE_TIMER_CTRL_3,   //TODO verify PWM functionality w.r.t datasheet/user mannual
	// uC_BASE_PWM_CTRL_4 = uC_BASE_TIMER_CTRL_15,
	// uC_BASE_PWM_CTRL_5 = uC_BASE_TIMER_CTRL_15,
	// uC_BASE_PWM_CTRL_6 = uC_BASE_TIMER_CTRL_15,
	uC_BASE_PWM_CTRL_16 = uC_BASE_TIMER_CTRL_16,
	uC_BASE_PWM_CTRL_17 = uC_BASE_TIMER_CTRL_17,
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
	/* Only one ADC available on STM32WB55R */
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
 *      DMA Application Peripheral ID's
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



// Hard-wired DMA Channel mapping not applicable for STM32WB controller
#define DMA_STREAM_CHANNEL_SHIFT        4U		// TODO check if part of dead code
//
// #define DMA_USART2TX_D1C7               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_7 ) | \
// //										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_USART2RX_D1C6               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_6 ) | \
// //										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_USART1RX_D1C5               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_5 ) | \
// //										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_USART1TX_D1C4               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_4 ) | \
// //										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_USART3RX_D1C3               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_3 ) | \
// //										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_USART3TX_D1C2               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_2 ) | \
// //										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_ADC1_D1C1                   ( static_cast<uint8_t>( uC_DMA1_CHANNEL_1 ) | \
// //										  ( static_cast<uint8_t>( 0 ) << DMA_STREAM_CHANNEL_SHIFT ) )
//
// #define DMA_QUADSPI_D2C7                ( static_cast<uint8_t>( uC_DMA2_CHANNEL_7 ) | \
// //										  ( static_cast<uint8_t>( 3 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_UART4RX_D2C5                ( static_cast<uint8_t>( uC_DMA2_CHANNEL_5 ) | \
// //										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_ADC2_D2C4                   ( static_cast<uint8_t>( uC_DMA2_CHANNEL_4 ) | \
// //										  ( static_cast<uint8_t>( 0 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_UART4TX_D2C3                ( static_cast<uint8_t>( uC_DMA2_CHANNEL_3 ) | \
// //										  ( static_cast<uint8_t>( 2 ) << DMA_STREAM_CHANNEL_SHIFT ) )
//
// #define DMA_SPI2TX_D1C5               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_5 ) | \
// //										( static_cast<uint8_t>( 1 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_SPI2RX_D1C4               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_4 ) | \
// //										( static_cast<uint8_t>( 1 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_SPI1TX_D1C3               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_3 ) | \
// //										( static_cast<uint8_t>( 1 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_SPI1RX_D1C2               ( static_cast<uint8_t>( uC_DMA1_CHANNEL_2 ) | \
// //										( static_cast<uint8_t>( 1 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_SPI3TX_D2C2               ( static_cast<uint8_t>( uC_DMA2_CHANNEL_2 ) | \
// //										( static_cast<uint8_t>( 3 ) << DMA_STREAM_CHANNEL_SHIFT ) )
// #define DMA_SPI3RX_D2C1               ( static_cast<uint8_t>( uC_DMA2_CHANNEL_1 ) | \
// //										( static_cast<uint8_t>( 3 ) << DMA_STREAM_CHANNEL_SHIFT ) )
/** @defgroup DMAREQ_ID Transfer request
 * @{
 */
#define DMAMUX_REQ_MEM2MEM             0x00U	/*!< memory to memory transfer  */
#define DMAMUX_REQ_GENERATOR0          0x01U	/*!< DMAMUX request generator 0 */
#define DMAMUX_REQ_GENERATOR1          0x02U	/*!< DMAMUX request generator 1 */
#define DMAMUX_REQ_GENERATOR2          0x03U	/*!< DMAMUX request generator 2 */
#define DMAMUX_REQ_GENERATOR3          0x04U	/*!< DMAMUX request generator 3 */
#define DMAMUX_REQ_ADC1                0x05U	/*!< DMAMUX ADC1 request        */
#define DMAMUX_REQ_SPI1_RX             0x06U	/*!< DMAMUX SPI1 RX request     */
#define DMAMUX_REQ_SPI1_TX             0x07U	/*!< DMAMUX SPI1 TX request     */
#define DMAMUX_REQ_SPI2_RX             0x08U	/*!< DMAMUX SPI2 RX request     */
#define DMAMUX_REQ_SPI2_TX             0x09U	/*!< DMAMUX SPI2 TX request     */
#define DMAMUX_REQ_I2C1_RX             0x0AU	/*!< DMAMUX I2C1 RX request     */
#define DMAMUX_REQ_I2C1_TX             0x0BU	/*!< DMAMUX I2C1 TX request     */
#define DMAMUX_REQ_I2C3_RX             0x0CU	/*!< DMAMUX I2C3 RX request     */
#define DMAMUX_REQ_I2C3_TX             0x0DU	/*!< DMAMUX I2C3 TX request     */
#define DMAMUX_REQ_USART1_RX           0x0EU	/*!< DMAMUX USART1 RX request   */
#define DMAMUX_REQ_USART1_TX           0x0FU	/*!< DMAMUX USART1 TX request   */
#define DMAMUX_REQ_LPUART1_RX          0x10U	/*!< DMAMUX LPUART1 RX request  */
#define DMAMUX_REQ_LPUART1_TX          0x11U	/*!< DMAMUX LPUART1 TX request  */
#define DMAMUX_REQ_SAI1_A              0x12U	/*!< DMAMUX SAI1 A request      */
#define DMAMUX_REQ_SAI1_B              0x13U	/*!< DMAMUX SAI1 B request      */
#define DMAMUX_REQ_QUADSPI             0x14U	/*!< DMAMUX QUADSPI request     */
#define DMAMUX_REQ_TIM1_CH1            0x15U	/*!< DMAMUX TIM1 CH1 request    */
#define DMAMUX_REQ_TIM1_CH2            0x16U	/*!< DMAMUX TIM1 CH2 request    */
#define DMAMUX_REQ_TIM1_CH3            0x17U	/*!< DMAMUX TIM1 CH3 request    */
#define DMAMUX_REQ_TIM1_CH4            0x18U	/*!< DMAMUX TIM1 CH4 request    */
#define DMAMUX_REQ_TIM1_UP             0x19U	/*!< DMAMUX TIM1 UP request     */
#define DMAMUX_REQ_TIM1_TRIG           0x1AU	/*!< DMAMUX TIM1 TRIG request   */
#define DMAMUX_REQ_TIM1_COM            0x1BU	/*!< DMAMUX TIM1 COM request    */
#define DMAMUX_REQ_TIM2_CH1            0x1CU	/*!< DMAMUX TIM2 CH1 request    */
#define DMAMUX_REQ_TIM2_CH2            0x1DU	/*!< DMAMUX TIM2 CH2 request    */
#define DMAMUX_REQ_TIM2_CH3            0x1EU	/*!< DMAMUX TIM2 CH3 request    */
#define DMAMUX_REQ_TIM2_CH4            0x1FU	/*!< DMAMUX TIM2 CH4 request    */
#define DMAMUX_REQ_TIM2_UP             0x20U	/*!< DMAMUX TIM2 UP request     */
#define DMAMUX_REQ_TIM16_CH1           0x21U	/*!< DMAMUX TIM16 CH1 request   */
#define DMAMUX_REQ_TIM16_UP            0x22U	/*!< DMAMUX TIM16 UP request    */
#define DMAMUX_REQ_TIM17_CH1           0x23U	/*!< DMAMUX TIM17 CH1 request   */
#define DMAMUX_REQ_TIM17_UP            0x24U	/*!< DMAMUX TIM17 UP request    */
#define DMAMUX_REQ_AES1_IN             0x25U	/*!< DMAMUX AES1_IN request     */
#define DMAMUX_REQ_AES1_OUT            0x26U	/*!< DMAMUX AES1_OUT request    */
#define DMAMUX_REQ_AES2_IN             0x27U	/*!< DMAMUX AES2_IN request     */
#define DMAMUX_REQ_AES2_OUT            0x28U	/*!< DMAMUX AES2_OUT request    */
#define DMAMUX_MAX_REQ_ID              0x29U	/*!< DMAMUX Max Request         */
/**
 * @}
 */


/*
 *****************************************************************************************
 *      EMAC Controller Peripheral ID's
 *			File: uC_EMAC
 *****************************************************************************************
 */
// ToDo: Please verify these erase time from the STM32WB55 datasheet before using. Below is the link to datasheet
// https://www.st.com/resource/en/datasheet/stm32wb55cc.pdf
static const uint32_t SECTOR_ERASE_TIME_LARGEST_MS = 2000U;	/* TODO:Sector erase time in miliseconds. Considering x32
															   erase*/

/* As per datasheet SECTOR_ERASE_TIME_BASELINE_MS should be 400ms, it is reduced to 100ms to optimize firmware upgrade
   time */
static const uint32_t SECTOR_ERASE_TIME_BASELINE_MS = 100U;	/* TODO:Sector erase time in miliseconds. Considering x32
															   erase*/

/* As per datasheet, maximum BYTE_WRITE_TIME should be 16us, it is reduced to 8us to optimize firmware upgrade time */
static const uint32_t BYTE_WRITE_TIME_US = 16U;
static const uint32_t BYTE_READ_TIME_US = 1U;

// there is no sector definition in WB55R, just 2K bytes page

typedef uint32_t sector_addr_t;
typedef uint32_t sector_num_t;
typedef struct
{
	sector_addr_t sector_size;
	sector_num_t sector_no;
} flash_page_t;
const flash_page_t INTERNAL_FLASH_PAGE_SIZES[257] =
{
	{0x1000, 0},
	{0x1000, 1},
	{0x1000, 2},
	{0x1000, 3},
	{0x1000, 4},
	{0x1000, 5},
	{0x1000, 6},
	{0x1000, 7},
	{0x1000, 8},
	{0x1000, 9},
	{0x1000, 10},
	{0x1000, 11},
	{0x1000, 12},
	{0x1000, 13},
	{0x1000, 14},
	{0x1000, 15},
	{0x1000, 16},
	{0x1000, 17},
	{0x1000, 18},
	{0x1000, 19},
	{0x1000, 20},
	{0x1000, 21},
	{0x1000, 22},
	{0x1000, 23},
	{0x1000, 24},
	{0x1000, 25},
	{0x1000, 26},
	{0x1000, 27},
	{0x1000, 28},
	{0x1000, 29},
	{0x1000, 30},
	{0x1000, 31},
	{0x1000, 32},
	{0x1000, 33},
	{0x1000, 34},
	{0x1000, 35},
	{0x1000, 36},
	{0x1000, 37},
	{0x1000, 38},
	{0x1000, 39},
	{0x1000, 40},
	{0x1000, 41},
	{0x1000, 42},
	{0x1000, 43},
	{0x1000, 44},
	{0x1000, 45},
	{0x1000, 46},
	{0x1000, 47},
	{0x1000, 48},
	{0x1000, 49},
	{0x1000, 50},
	{0x1000, 51},
	{0x1000, 52},
	{0x1000, 53},
	{0x1000, 54},
	{0x1000, 55},
	{0x1000, 56},
	{0x1000, 57},
	{0x1000, 58},
	{0x1000, 59},
	{0x1000, 60},
	{0x1000, 61},
	{0x1000, 62},
	{0x1000, 63},
	{0x1000, 64},
	{0x1000, 65},
	{0x1000, 66},
	{0x1000, 67},
	{0x1000, 68},
	{0x1000, 69},
	{0x1000, 70},
	{0x1000, 71},
	{0x1000, 72},
	{0x1000, 73},
	{0x1000, 74},
	{0x1000, 75},
	{0x1000, 76},
	{0x1000, 77},
	{0x1000, 78},
	{0x1000, 79},
	{0x1000, 80},
	{0x1000, 81},
	{0x1000, 82},
	{0x1000, 83},
	{0x1000, 84},
	{0x1000, 85},
	{0x1000, 86},
	{0x1000, 87},
	{0x1000, 88},
	{0x1000, 89},
	{0x1000, 90},
	{0x1000, 91},
	{0x1000, 92},
	{0x1000, 93},
	{0x1000, 94},
	{0x1000, 95},
	{0x1000, 96},
	{0x1000, 97},
	{0x1000, 98},
	{0x1000, 99},
	{0x1000, 100},
	{0x1000, 101},
	{0x1000, 102},
	{0x1000, 103},
	{0x1000, 104},
	{0x1000, 105},
	{0x1000, 106},
	{0x1000, 107},
	{0x1000, 108},
	{0x1000, 109},
	{0x1000, 110},
	{0x1000, 111},
	{0x1000, 112},
	{0x1000, 113},
	{0x1000, 114},
	{0x1000, 115},
	{0x1000, 116},
	{0x1000, 117},
	{0x1000, 118},
	{0x1000, 119},
	{0x1000, 120},
	{0x1000, 121},
	{0x1000, 122},
	{0x1000, 123},
	{0x1000, 124},
	{0x1000, 125},
	{0x1000, 126},
	{0x1000, 127},
	{0x1000, 128},
	{0x1000, 129},
	{0x1000, 130},
	{0x1000, 131},
	{0x1000, 132},
	{0x1000, 133},
	{0x1000, 134},
	{0x1000, 135},
	{0x1000, 136},
	{0x1000, 137},
	{0x1000, 138},
	{0x1000, 139},
	{0x1000, 140},
	{0x1000, 141},
	{0x1000, 142},
	{0x1000, 143},
	{0x1000, 144},
	{0x1000, 145},
	{0x1000, 146},
	{0x1000, 147},
	{0x1000, 148},
	{0x1000, 149},
	{0x1000, 150},
	{0x1000, 151},
	{0x1000, 152},
	{0x1000, 153},
	{0x1000, 154},
	{0x1000, 155},
	{0x1000, 156},
	{0x1000, 157},
	{0x1000, 158},
	{0x1000, 159},
	{0x1000, 160},
	{0x1000, 161},
	{0x1000, 162},
	{0x1000, 163},
	{0x1000, 164},
	{0x1000, 165},
	{0x1000, 166},
	{0x1000, 167},
	{0x1000, 168},
	{0x1000, 169},
	{0x1000, 170},
	{0x1000, 171},
	{0x1000, 172},
	{0x1000, 173},
	{0x1000, 174},
	{0x1000, 175},
	{0x1000, 176},
	{0x1000, 177},
	{0x1000, 178},
	{0x1000, 179},
	{0x1000, 180},
	{0x1000, 181},
	{0x1000, 182},
	{0x1000, 183},
	{0x1000, 184},
	{0x1000, 185},
	{0x1000, 186},
	{0x1000, 187},
	{0x1000, 188},
	{0x1000, 189},
	{0x1000, 190},
	{0x1000, 191},
	{0x1000, 192},
	{0x1000, 193},
	{0x1000, 194},
	{0x1000, 195},
	{0x1000, 196},
	{0x1000, 197},
	{0x1000, 198},
	{0x1000, 199},
	{0x1000, 200},
	{0x1000, 201},
	{0x1000, 202},
	{0x1000, 203},
	{0x1000, 204},
	{0x1000, 205},
	{0x1000, 206},
	{0x1000, 207},
	{0x1000, 208},
	{0x1000, 209},
	{0x1000, 210},
	{0x1000, 211},
	{0x1000, 212},
	{0x1000, 213},
	{0x1000, 214},
	{0x1000, 215},
	{0x1000, 216},
	{0x1000, 217},
	{0x1000, 218},
	{0x1000, 219},
	{0x1000, 220},
	{0x1000, 221},
	{0x1000, 222},
	{0x1000, 223},
	{0x1000, 224},
	{0x1000, 225},
	{0x1000, 226},
	{0x1000, 227},
	{0x1000, 228},
	{0x1000, 229},
	{0x1000, 230},
	{0x1000, 231},
	{0x1000, 232},
	{0x1000, 233},
	{0x1000, 234},
	{0x1000, 235},
	{0x1000, 236},
	{0x1000, 237},
	{0x1000, 238},
	{0x1000, 239},
	{0x1000, 240},
	{0x1000, 241},
	{0x1000, 242},
	{0x1000, 243},
	{0x1000, 244},
	{0x1000, 245},
	{0x1000, 246},
	{0x1000, 247},
	{0x1000, 248},
	{0x1000, 249},
	{0x1000, 250},
	{0x1000, 251},
	{0x1000, 252},
	{0x1000, 253},
	{0x1000, 254},
	{0x1000, 255},
	{0x00000, 0xFFFF}
};
const UINT32 INTERNAL_FLASH_START_ADDRESS = 0x08000000U;

/*
 *****************************************************************************************
 *      CAN Chip Select Application Peripheral ID's
 *			File: uC_CAN
 *****************************************************************************************
 */
// STM32WB55 doesn't have any CAN peripheral

// enum
// {
// uC_BASE_CAN_CTRL_1,
// uC_BASE_CAN_MAX_CTRLS
// };
#endif	// This is the endif for the multiple include.
