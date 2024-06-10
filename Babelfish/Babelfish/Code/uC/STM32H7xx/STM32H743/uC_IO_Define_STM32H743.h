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
#ifndef uC_IO_DEFINE_STM32H743_H
   #define uC_IO_DEFINE_STM32H743_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "uC_STM32H743.h"

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
	uC_PIO_INDEX_G,
	uC_PIO_INDEX_H,
	uC_PIO_INDEX_I,
	uC_PIO_INDEX_J,
	uC_PIO_INDEX_K,
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

typedef struct uC_PERIPH_IO_STRUCT
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
#define IO_PINM_0        ( 1U << 0 )
#define IO_PINM_1        ( 1U << 1 )
#define IO_PINM_2        ( 1U << 2 )
#define IO_PINM_3        ( 1U << 3 )
#define IO_PINM_4        ( 1U << 4 )
#define IO_PINM_5        ( 1U << 5 )
#define IO_PINM_6        ( 1U << 6 )
#define IO_PINM_7        ( 1U << 7 )
#define IO_PINM_8        ( 1U << 8 )
#define IO_PINM_9        ( 1U << 9 )
#define IO_PINM_10       ( 1U << 10 )
#define IO_PINM_11       ( 1U << 11 )
#define IO_PINM_12       ( 1U << 12 )
#define IO_PINM_13       ( 1U << 13 )
#define IO_PINM_14       ( 1U << 14 )
#define IO_PINM_15       ( 1U << 15 )


/*
 *****************************************************************************************
 *		Standard IO Pins
 *****************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif

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


extern const uC_USER_IO_STRUCT PIOG_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOG_PIN_15_STRUCT;

extern const uC_USER_IO_STRUCT PIOH_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOH_PIN_15_STRUCT;


extern const uC_USER_IO_STRUCT PIOI_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOI_PIN_15_STRUCT;


extern const uC_USER_IO_STRUCT PIOJ_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOJ_PIN_15_STRUCT;


extern const uC_USER_IO_STRUCT PIOK_PIN_0_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_1_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_2_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_3_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_4_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_5_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_6_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_7_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_8_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_9_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_10_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_11_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_12_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_13_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_14_STRUCT;
extern const uC_USER_IO_STRUCT PIOK_PIN_15_STRUCT;
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
extern const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PB14;
extern const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PB15;

extern const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PA2;
extern const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PA3;
extern const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PD5;
extern const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PD6;

extern const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PB10;
extern const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PB11;
extern const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PC10;
extern const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PC11;
extern const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PD8;
extern const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PD9;

extern const uC_PERIPH_IO_STRUCT UART4_TX_PIO_PA0;
extern const uC_PERIPH_IO_STRUCT UART4_RX_PIO_PA1;
extern const uC_PERIPH_IO_STRUCT UART4_TX_PIO_PA12;
extern const uC_PERIPH_IO_STRUCT UART4_RX_PIO_PA11;
extern const uC_PERIPH_IO_STRUCT UART4_TX_PIO_PB9;
extern const uC_PERIPH_IO_STRUCT UART4_RX_PIO_PB8;
extern const uC_PERIPH_IO_STRUCT UART4_TX_PIO_PC10;
extern const uC_PERIPH_IO_STRUCT UART4_RX_PIO_PC11;
extern const uC_PERIPH_IO_STRUCT UART4_TX_PIO_PD1;
extern const uC_PERIPH_IO_STRUCT UART4_RX_PIO_PD0;
extern const uC_PERIPH_IO_STRUCT UART4_TX_PIO_PH13;
extern const uC_PERIPH_IO_STRUCT UART4_RX_PIO_PH14;

extern const uC_PERIPH_IO_STRUCT UART5_TX_PIO_PB6;
extern const uC_PERIPH_IO_STRUCT UART5_RX_PIO_PB5;
extern const uC_PERIPH_IO_STRUCT UART5_TX_PIO_PB13;
extern const uC_PERIPH_IO_STRUCT UART5_RX_PIO_PB12;
extern const uC_PERIPH_IO_STRUCT UART5_TX_PIO_PC12;
extern const uC_PERIPH_IO_STRUCT UART5_RX_PIO_PD2;

extern const uC_PERIPH_IO_STRUCT USART6_TX_PIO_PC6;
extern const uC_PERIPH_IO_STRUCT USART6_RX_PIO_PC7;
extern const uC_PERIPH_IO_STRUCT USART6_TX_PIO_PG14;
extern const uC_PERIPH_IO_STRUCT USART6_RX_PIO_PG9;

extern const uC_PERIPH_IO_STRUCT UART7_TX_PIO_PA15;
extern const uC_PERIPH_IO_STRUCT UART7_RX_PIO_PA8;
extern const uC_PERIPH_IO_STRUCT UART7_TX_PIO_PB4;
extern const uC_PERIPH_IO_STRUCT UART7_RX_PIO_PB3;
extern const uC_PERIPH_IO_STRUCT UART7_TX_PIO_PE8;
extern const uC_PERIPH_IO_STRUCT UART7_RX_PIO_PE7;
extern const uC_PERIPH_IO_STRUCT UART7_TX_PIO_PF7;
extern const uC_PERIPH_IO_STRUCT UART7_RX_PIO_PF6;

extern const uC_PERIPH_IO_STRUCT UART8_TX_PIO_PE1;
extern const uC_PERIPH_IO_STRUCT UART8_RX_PIO_PE0;
extern const uC_PERIPH_IO_STRUCT UART8_TX_PIO_PJ8;
extern const uC_PERIPH_IO_STRUCT UART8_RX_PIO_PJ9;

/*
 *********************************************************
 *		USB
 */

extern const uC_PERIPH_IO_STRUCT USB_FS_ID;
extern const uC_USER_IO_STRUCT USB_FS_VBUS;
extern const uC_PERIPH_IO_STRUCT USB_FS_DM;
extern const uC_PERIPH_IO_STRUCT USB_FS_DP;

extern const uC_PERIPH_IO_STRUCT USB_HS_FS_ID;
extern const uC_USER_IO_STRUCT USB_HS_FS_VBUS;
extern const uC_PERIPH_IO_STRUCT USB_HS_FS_DM;
extern const uC_PERIPH_IO_STRUCT USB_HS_FS_DP;

/*
 *********************************************************
 *		SPI
 */
extern const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PA7;
extern const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PB5;
extern const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PA6;
extern const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PA5;
extern const uC_PERIPH_IO_STRUCT SPI1_SS_PIO_PA4;

extern const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PA5;
extern const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PA4;
extern const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PA3;
extern const uC_PERIPH_IO_STRUCT SPI1_SS_PIO_PA15;

extern const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PC3;
extern const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PC2;
extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PB10;
extern const uC_PERIPH_IO_STRUCT SPI2_SS_PIO_PB9;


extern const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PB15;
extern const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PB14;
extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PB13;
extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PD3;
extern const uC_PERIPH_IO_STRUCT SPI2_SS_PIO_PB12;


extern const uC_PERIPH_IO_STRUCT SPI3_MOSI_PIO_PB5;
extern const uC_PERIPH_IO_STRUCT SPI3_MISO_PIO_PB4;
extern const uC_PERIPH_IO_STRUCT SPI3_CLOCK_PIO_PB3;
extern const uC_PERIPH_IO_STRUCT SPI3_SS_PIO_PA15;

extern const uC_PERIPH_IO_STRUCT SPI3_MOSI_PIO_PC12;
extern const uC_PERIPH_IO_STRUCT SPI3_MISO_PIO_PC11;
extern const uC_PERIPH_IO_STRUCT SPI3_CLOCK_PIO_PC10;
extern const uC_PERIPH_IO_STRUCT SPI3_SS_PIO_PA4;

extern const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PI3;
extern const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PI2;
extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PI1;
extern const uC_PERIPH_IO_STRUCT SPI2_SS_PIO_PI0;

/*
 *********************************************************
 *		I2C
 */
extern const uC_PERIPH_IO_STRUCT I2C1_SCL_PIO_PB6;
extern const uC_PERIPH_IO_STRUCT I2C1_SDA_PIO_PB7;

extern const uC_PERIPH_IO_STRUCT I2C1_SCL_PIO_PB8;
extern const uC_PERIPH_IO_STRUCT I2C1_SDA_PIO_PB9;

extern const uC_PERIPH_IO_STRUCT I2C2_SCL_PIO_PF1;
extern const uC_PERIPH_IO_STRUCT I2C2_SDA_PIO_PF0;

extern const uC_PERIPH_IO_STRUCT I2C3_SCL_PIO_PA8;
extern const uC_PERIPH_IO_STRUCT I2C3_SDA_PIO_PC9;

extern const uC_PERIPH_IO_STRUCT I2C2_SCL_PIO_PH4;
extern const uC_PERIPH_IO_STRUCT I2C2_SDA_PIO_PH5;

extern const uC_PERIPH_IO_STRUCT I2C3_SCL_PIO_PH7;
extern const uC_PERIPH_IO_STRUCT I2C3_SDA_PIO_PH8;

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
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO1_PK1;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO2_PJ11;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO3_PJ9;

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
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO1_PH10;
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO2_PH11;
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO3_PH12;
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO4_PI0;

extern const uC_PERIPH_IO_STRUCT TIMER8_PIO1_PC6;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO2_PC7;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO3_PC8;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO4_PC9;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO1_PI5;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO2_PI6;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO3_PI7;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO4_PI2;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO1_PJ8;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO2_PJ6;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO2_PJ10;
extern const uC_PERIPH_IO_STRUCT TIMER8_PIO3_PK0;

extern const uC_PERIPH_IO_STRUCT TIMER12_PIO1_PH6;
extern const uC_PERIPH_IO_STRUCT TIMER12_PIO2_PH9;
extern const uC_PERIPH_IO_STRUCT TIMER12_PIO1_PB14;
extern const uC_PERIPH_IO_STRUCT TIMER12_PIO2_PB15;

extern const uC_PERIPH_IO_STRUCT TIMER13_PIO1_PA6;
extern const uC_PERIPH_IO_STRUCT TIMER13_PIO1_PF8;

extern const uC_PERIPH_IO_STRUCT TIMER14_PIO1_PA7;
extern const uC_PERIPH_IO_STRUCT TIMER14_PIO1_PF9;

extern const uC_PERIPH_IO_STRUCT TIMER15_PIO1_PA2;
extern const uC_PERIPH_IO_STRUCT TIMER15_PIO2_PA3;
extern const uC_PERIPH_IO_STRUCT TIMER15_PIO1_PE5;
extern const uC_PERIPH_IO_STRUCT TIMER15_PIO2_PE6;

extern const uC_PERIPH_IO_STRUCT TIMER16_PIO1_PF6;
extern const uC_PERIPH_IO_STRUCT TIMER16_PIO1_PB8;

extern const uC_PERIPH_IO_STRUCT TIMER17_PIO1_PF7;
extern const uC_PERIPH_IO_STRUCT TIMER17_PIO1_PB9;

/*
 ******************************************
 *		ADC Peripheral
 */
extern const uC_PERIPH_IO_STRUCT ADC1_IN2_PIO;
extern const uC_PERIPH_IO_STRUCT ADC1_IN6_PIO;
extern const uC_PERIPH_IO_STRUCT ADC1_IN16_PIO;
extern const uC_PERIPH_IO_STRUCT ADC1_IN17_PIO;

extern const uC_PERIPH_IO_STRUCT ADC2_IN2_PIO;
extern const uC_PERIPH_IO_STRUCT ADC2_IN6_PIO;

extern const uC_PERIPH_IO_STRUCT ADC3_IN2_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN3_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN4_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN5_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN6_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN7_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN8_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN9_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN13_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN14_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN15_PIO;
extern const uC_PERIPH_IO_STRUCT ADC3_IN16_PIO;

extern const uC_PERIPH_IO_STRUCT ADC12_IN3_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN4_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN5_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN7_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN8_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN9_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN14_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN15_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN18_PIO;
extern const uC_PERIPH_IO_STRUCT ADC12_IN19_PIO;

extern const uC_PERIPH_IO_STRUCT ADC123_IN10_PIO;
extern const uC_PERIPH_IO_STRUCT ADC123_IN11_PIO;
extern const uC_PERIPH_IO_STRUCT ADC123_IN12_PIO;

/*
 ******************************************
 *		Ethernet MII Peripheral
 */
extern const uC_PERIPH_IO_STRUCT ETH_MCO_CLOCK_OUT_PIO;

extern const uC_PERIPH_IO_STRUCT ETH_MII_TX_EN_PIO;
/// ********** Outputs **********
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD0_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD1_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD2_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD3_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_MDC_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_MDIO_PIO;
// extern const uC_PERIPH_IO_STRUCT  ETH_MII_PPS_OUT_PIO; //Unused
extern const uC_PERIPH_IO_STRUCT ETH_MII_TX_EN_PIO_PG11;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD0_PIO_PG13;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD1_PIO_PG14;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD3_PIO_PE2;
//
extern const uC_PERIPH_IO_STRUCT ETH_MII_COL_PIO;
///// ********** Inputs **********
extern const uC_PERIPH_IO_STRUCT ETH_MII_CRS_WKUP_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TX_CLK_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RX_ER_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RX_CLK_PIO;

extern const uC_PERIPH_IO_STRUCT ETH_MII_RX_DV_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD0_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD1_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD2_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD3_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RX_ER_PIO_PI10;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD3_PIO_PH7;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD2_PIO_PH6;
extern const uC_PERIPH_IO_STRUCT ETH_MII_COL_PIO_PH3;
extern const uC_PERIPH_IO_STRUCT ETH_MII_CRS_WKUP_PIO_PH2;
/*
 ******************************************
 *		Ethernet RMII Peripheral
 */
extern const uC_PERIPH_IO_STRUCT ETH_RMII_MDIO_PIO;
/// ********** Outputs **********
extern const uC_PERIPH_IO_STRUCT ETH_RMII_TX_EN_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_TXD0_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_TXD1_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_MDC_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_TX_EN_PIO_PG11;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_TXD0_PIO_PG13;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_TXD1_PIO_PG14;

// extern const uC_PERIPH_IO_STRUCT    ETH_RMII_PPS_OUT_PIO; //Unused
/// ********** Inputs **********
extern const uC_PERIPH_IO_STRUCT ETH_RMII_REF_CLK_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_CRS_DV_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_RXD0_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_RXD1_PIO;

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

extern const uint32_t SET_PIO_G_OUTPUT;
extern const uint32_t SET_PIO_G_OUTPUT_INIT;
extern const uint32_t SET_PIO_G_INPUT;
extern const uint32_t SET_PIO_G_INPUT_PULLUP;
extern const uint32_t SET_PIO_G_INPUT_PULLDOWN;

extern const uint32_t SET_PIO_H_OUTPUT;
extern const uint32_t SET_PIO_H_OUTPUT_INIT;
extern const uint32_t SET_PIO_H_INPUT;
extern const uint32_t SET_PIO_H_INPUT_PULLUP;
extern const uint32_t SET_PIO_H_INPUT_PULLDOWN;

extern const uint32_t SET_PIO_I_OUTPUT;
extern const uint32_t SET_PIO_I_OUTPUT_INIT;
extern const uint32_t SET_PIO_I_INPUT;
extern const uint32_t SET_PIO_I_INPUT_PULLUP;
extern const uint32_t SET_PIO_I_INPUT_PULLDOWN;

extern const uint32_t SET_PIO_J_OUTPUT;
extern const uint32_t SET_PIO_J_OUTPUT_INIT;
extern const uint32_t SET_PIO_J_INPUT;
extern const uint32_t SET_PIO_J_INPUT_PULLUP;
extern const uint32_t SET_PIO_J_INPUT_PULLDOWN;

extern const uint32_t SET_PIO_K_OUTPUT;
extern const uint32_t SET_PIO_K_OUTPUT_INIT;
extern const uint32_t SET_PIO_K_INPUT;
extern const uint32_t SET_PIO_K_INPUT_PULLUP;
extern const uint32_t SET_PIO_K_INPUT_PULLDOWN;

/*
 ******************************************
 *		CAN Peripheral
 */
// extern const uC_PERIPH_IO_STRUCT 	CAN1_RX_PIO_PA11;
// extern const uC_PERIPH_IO_STRUCT 	CAN1_TX_PIO_PA12;
// extern const uC_PERIPH_IO_STRUCT 	CAN1_RX_PIO_PB8;
// extern const uC_PERIPH_IO_STRUCT 	CAN1_TX_PIO_PB9;
// extern const uC_PERIPH_IO_STRUCT 	CAN1_RX_PIO_PD0;
// extern const uC_PERIPH_IO_STRUCT 	CAN1_TX_PIO_PD1;
// extern const uC_PERIPH_IO_STRUCT 	CAN1_RX_PIO_PI9;
// extern const uC_PERIPH_IO_STRUCT 	CAN1_TX_PIO_PH13;
// extern const uC_PERIPH_IO_STRUCT        CAN2_RX_PIO_PB5;
// extern const uC_PERIPH_IO_STRUCT        CAN2_TX_PIO_PB6;
// extern const uC_PERIPH_IO_STRUCT 	CAN2_RX_PIO_PB12;
// extern const uC_PERIPH_IO_STRUCT 	CAN2_TX_PIO_PB13;


#ifdef __cplusplus
}
#endif

#endif

