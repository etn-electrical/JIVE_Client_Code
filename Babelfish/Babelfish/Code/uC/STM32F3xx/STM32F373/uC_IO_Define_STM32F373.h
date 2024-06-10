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
#ifndef uC_IO_DEFINE_STM32F373_H
   #define uC_IO_DEFINE_STM32F373_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "uC_STM32F373.h"

/*
 *****************************************************************************************
 *		Defines and Struct Definitions
 *****************************************************************************************
 */
enum
{
	uC_PIO_INDEX_A,
	uC_PIO_INDEX_B,
	uC_PIO_INDEX_C,
	uC_PIO_INDEX_D,
	uC_PIO_INDEX_E,
	uC_PIO_INDEX_F,
	uC_PIO_INDEX_MAX
};

typedef struct
{
	GPIO_TypeDef* reg_ctrl;		// This is the reference to the main config structure.
	uint8_t pio_num;				// This is the reference relative to a certain port (ie: io num 3 of PortA)
	uint8_t port_index;			// This is the port reference.
} uC_USER_IO_STRUCT;


/// This IO configuration behavior is best explained in the datasheet under the GPIO and alternate mapping
/// sections.  This code defines the placement of the remap data if supported for the peripheral pins.
enum
{
	uC_IO_ALTERNATE_MAP_0,
	uC_IO_ALTERNATE_MAP_1,
	uC_IO_ALTERNATE_MAP_2,
	uC_IO_ALTERNATE_MAP_3,
	uC_IO_ALTERNATE_MAP_4,
	uC_IO_ALTERNATE_MAP_5,
	uC_IO_ALTERNATE_MAP_6,
	uC_IO_ALTERNATE_MAP_7,
	uC_IO_ALTERNATE_MAP_8,
	uC_IO_ALTERNATE_MAP_9,
	uC_IO_ALTERNATE_MAP_10,
	uC_IO_ALTERNATE_MAP_11,
	uC_IO_ALTERNATE_MAP_12,
	uC_IO_ALTERNATE_MAP_13,
	uC_IO_ALTERNATE_MAP_14,
	uC_IO_ALTERNATE_MAP_15,
	uC_IO_ALTERNATE_MAP_UNSUPPORTED,
	uC_IO_ALTERNATE_MAP_ANALOG,
	uC_IO_ALTERNATE_MAP_MAX
};

#define uC_IO_ALTERNATE_MAP_MASK        0xFU

typedef struct
{
	GPIO_TypeDef* reg_ctrl;
	uint8_t pio_num;
	uint8_t alt_map;
} uC_PERIPH_IO_STRUCT;


// ************************************************************
// ****  The pin defines are here to provide searchability for pin numbers
// ************************************************************
#define IO_PIN_UNDEFINED    16U
#define IO_PIN_0        0U
#define IO_PIN_1        1U
#define IO_PIN_2        2U
#define IO_PIN_3        3U
#define IO_PIN_4        4U
#define IO_PIN_5        5U
#define IO_PIN_6        6U
#define IO_PIN_7        7U
#define IO_PIN_8        8U
#define IO_PIN_9        9U
#define IO_PIN_10       10U
#define IO_PIN_11       11U
#define IO_PIN_12       12U
#define IO_PIN_13       13U
#define IO_PIN_14       14U
#define IO_PIN_15       15U

#define IO_PINM_UNDEFINED   0U
#define IO_PINM_0        ( static_cast<uint32_t>( 1U ) << 0U )
#define IO_PINM_1        ( static_cast<uint32_t>( 1U ) << 1U )
#define IO_PINM_2        ( static_cast<uint32_t>( 1U ) << 2U )
#define IO_PINM_3        ( static_cast<uint32_t>( 1U ) << 3U )
#define IO_PINM_4        ( static_cast<uint32_t>( 1U ) << 4U )
#define IO_PINM_5        ( static_cast<uint32_t>( 1U ) << 5U )
#define IO_PINM_6        ( static_cast<uint32_t>( 1U ) << 6U )
#define IO_PINM_7        ( static_cast<uint32_t>( 1U ) << 7U )
#define IO_PINM_8        ( static_cast<uint32_t>( 1U ) << 8U )
#define IO_PINM_9        ( static_cast<uint32_t>( 1U ) << 9U )
#define IO_PINM_10       ( static_cast<uint32_t>( 1U ) << 10U )
#define IO_PINM_11       ( static_cast<uint32_t>( 1U ) << 11U )
#define IO_PINM_12       ( static_cast<uint32_t>( 1U ) << 12U )
#define IO_PINM_13       ( static_cast<uint32_t>( 1U ) << 13U )
#define IO_PINM_14       ( static_cast<uint32_t>( 1U ) << 14U )
#define IO_PINM_15       ( static_cast<uint32_t>( 1U ) << 15U )


/*
 *****************************************************************************************
 *		Standard IO Pins
 *****************************************************************************************
 */
extern const uC_USER_IO_STRUCT PIO_NULL_IO_STRUCT;

extern const uC_USER_IO_STRUCT PIOA_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_15_STRUCT;


extern const uC_USER_IO_STRUCT PIOB_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_15_STRUCT;


extern const uC_USER_IO_STRUCT PIOC_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_15_STRUCT;


extern const uC_USER_IO_STRUCT PIOD_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOD_PIN_15_STRUCT;


extern const uC_USER_IO_STRUCT PIOE_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_15_STRUCT;


extern const uC_USER_IO_STRUCT PIOF_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOF_PIN_15_STRUCT;

/*
 *****************************************************************************************
 *		Peripherals
 *****************************************************************************************
 */
extern const uC_PERIPH_IO_STRUCT PERIPH_NULL_PIO;


/*
 *********************************************************
 *		USART
 */
extern const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PA9;
extern const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PA10;
extern const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PB6;
extern const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PB7;
extern const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PC4;
extern const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PC5;
extern const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PE0;
extern const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PE1;

extern const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PA2;
extern const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PA3;
extern const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PB3;
extern const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PB4;
extern const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PD5;
extern const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PD6;

extern const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PB8;
extern const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PB9;
extern const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PB10;
extern const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PC10;
extern const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PC11;
extern const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PD8;
extern const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PD9;
extern const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PE15;



/*
 *********************************************************
 *		SPI
 */
extern const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PA7;
extern const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PF6;
extern const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PC9;
extern const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PB5;

extern const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PA13;
extern const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PA6;
extern const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PB4;
extern const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PC8;

extern const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PC7;
extern const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PB3;
extern const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PA12;
extern const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PA5;

extern const uC_PERIPH_IO_STRUCT SPI1_SS_PIO_PA4;
extern const uC_PERIPH_IO_STRUCT SPI1_SS_PIO_PA11;
extern const uC_PERIPH_IO_STRUCT SPI1_SS_PIO_PA15;
extern const uC_PERIPH_IO_STRUCT SPI1_SS_PIO_PC6;

extern const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PA10;
extern const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PB15;
extern const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PC3;
extern const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PD4;

extern const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PA9;
extern const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PB14;
extern const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PC2;
extern const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PD3;

extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PA8;
extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PB8;
extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PB10;
extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PD7;
extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PD8;

extern const uC_PERIPH_IO_STRUCT SPI2_SS_PIO_PA11;
extern const uC_PERIPH_IO_STRUCT SPI2_SS_PIO_PB9;
extern const uC_PERIPH_IO_STRUCT SPI2_SS_PIO_PD6;


extern const uC_PERIPH_IO_STRUCT SPI3_MOSI_PIO_PA3;
extern const uC_PERIPH_IO_STRUCT SPI3_MOSI_PIO_PB5;
extern const uC_PERIPH_IO_STRUCT SPI3_MOSI_PIO_PC12;

extern const uC_PERIPH_IO_STRUCT SPI3_MISO_PIO_PA2;
extern const uC_PERIPH_IO_STRUCT SPI3_MISO_PIO_PB4;
extern const uC_PERIPH_IO_STRUCT SPI3_MISO_PIO_PC11;

extern const uC_PERIPH_IO_STRUCT SPI3_CLOCK_PIO_PA1;
extern const uC_PERIPH_IO_STRUCT SPI3_CLOCK_PIO_PB3;
extern const uC_PERIPH_IO_STRUCT SPI3_CLOCK_PIO_PC10;

extern const uC_PERIPH_IO_STRUCT SPI3_SS_PIO_PA4;
extern const uC_PERIPH_IO_STRUCT SPI3_SS_PIO_PA15;


/*
 *********************************************************
 *		Timer
 */
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO1_PA8;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO2_PA9;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO3_PA10;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO4_PA11;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO1_PE9;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO2_PE11;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO3_PE13;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO4_PE14;

extern const uC_PERIPH_IO_STRUCT TIMER2_PIO1_PA0;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO2_PA1;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO3_PA2;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO4_PA3;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO1_PA5;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO1_PA15;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO2_PB3;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO3_PB10;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO4_PB11;

extern const uC_PERIPH_IO_STRUCT TIMER3_PIO1_PA6;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO2_PA7;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO3_PB0;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO4_PB1;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO1_PB4;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO2_PB5;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO1_PC6;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO2_PC7;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO3_PC8;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO4_PC9;

extern const uC_PERIPH_IO_STRUCT TIMER4_PIO1_PB6;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO2_PB7;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO3_PB8;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO4_PB9;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO1_PD12;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO2_PD13;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO3_PD14;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO4_PD15;

extern const uC_PERIPH_IO_STRUCT TIMER5_PIO1_PA0;
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO2_PA1;
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO3_PA2;
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO4_PA3;


/*
 ******************************************
 *		ADC Peripheral
 */
extern const uC_PERIPH_IO_STRUCT ADC12_IN0_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN1_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN2_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN3_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN4_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN5_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN6_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN7_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN8_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN9_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN10_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN11_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN12_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN13_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN14_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN15_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN16_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN17_PIO;

/*
 ********************************************
 * Special Functions
 * MCO Definition
 */

extern const uC_PERIPH_IO_STRUCT MCO_CLOCK_OUT_PIO_PA8;

/*
 *****************************************************************************************
 *		Direction Selection
 *****************************************************************************************
 */
extern const uint32_t SET_PIO_A_OUTPUT;
extern const uint32_t SET_PIO_A_OUTPUT_INIT;
extern const uint32_t SET_PIO_A_INPUT;
extern const uint32_t SET_PIO_A_INPUT_PULLUP;
extern const uint32_t SET_PIO_A_INPUT_PULLDOWN;

extern const uint32_t SET_PIO_B_OUTPUT;
extern const uint32_t SET_PIO_B_OUTPUT_INIT;
extern const uint32_t SET_PIO_B_INPUT;
extern const uint32_t SET_PIO_B_INPUT_PULLUP;
extern const uint32_t SET_PIO_B_INPUT_PULLUP;
extern const uint32_t SET_PIO_B_INPUT_PULLDOWN;

extern const uint32_t SET_PIO_C_OUTPUT;
extern const uint32_t SET_PIO_C_OUTPUT_INIT;
extern const uint32_t SET_PIO_C_INPUT;
extern const uint32_t SET_PIO_C_INPUT_PULLUP;
extern const uint32_t SET_PIO_C_INPUT_PULLDOWN;

extern const uint32_t SET_PIO_D_OUTPUT;
extern const uint32_t SET_PIO_D_OUTPUT_INIT;
extern const uint32_t SET_PIO_D_INPUT;
extern const uint32_t SET_PIO_D_INPUT_PULLUP;
extern const uint32_t SET_PIO_D_INPUT_PULLDOWN;

extern const uint32_t SET_PIO_E_OUTPUT;
extern const uint32_t SET_PIO_E_OUTPUT_INIT;
extern const uint32_t SET_PIO_E_INPUT;
extern const uint32_t SET_PIO_E_INPUT_PULLUP;
extern const uint32_t SET_PIO_E_INPUT_PULLDOWN;

extern const uint32_t SET_PIO_F_OUTPUT;
extern const uint32_t SET_PIO_F_OUTPUT_INIT;
extern const uint32_t SET_PIO_F_INPUT;
extern const uint32_t SET_PIO_F_INPUT_PULLUP;
extern const uint32_t SET_PIO_F_INPUT_PULLDOWN;


#endif

