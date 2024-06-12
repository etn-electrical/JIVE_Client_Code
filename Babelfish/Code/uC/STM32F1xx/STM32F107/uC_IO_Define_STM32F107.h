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
#ifndef uC_IO_DEFINE_STM32F107_H
   #define uC_IO_DEFINE_STM32F107_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "uC_STM32F107.h"

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
#define uC_IO_REMAP_MASK_PACKED_SHIFT       4
#define uC_IO_REMAP_MASK_0                  ( 0 << uC_IO_REMAP_MASK_PACKED_SHIFT )
#define uC_IO_REMAP_MASK_1                  ( 1 << uC_IO_REMAP_MASK_PACKED_SHIFT )
#define uC_IO_REMAP_MASK_2                  ( 3 << uC_IO_REMAP_MASK_PACKED_SHIFT )
#define uC_IO_REMAP_MASK_3                  ( 7 << uC_IO_REMAP_MASK_PACKED_SHIFT )
enum
{
	uC_IO_ALTERNATE_MAP_0,
	uC_IO_ALTERNATE_MAP_1,
	uC_IO_ALTERNATE_MAP_2,
	uC_IO_ALTERNATE_MAP_3
};

#define uC_IO_ALTERNATE_MAP_UNSUPPORTED     uC_IO_ALTERNATE_MAP_0
#define uC_IO_ALTERNATE_MAP_MASK        0x03
#define uC_IO_ALTERNATE_MAP_ANALOG      0x08

typedef struct
{
	GPIO_TypeDef* reg_ctrl;
	uint8_t pio_num;
	uint8_t remap_num;	// The mask is contained in the high order nibble.  The actual remap number is stored in the low
						// order nibble.
	uint8_t remap_bit_shift;	// The number of bits where the remap number goes.
} uC_PERIPH_IO_STRUCT;


// ************************************************************
// ****  The pin defines are here to provide searchability for pin numbers
// ************************************************************
#define IO_PIN_UNDEFINED    16
#define IO_PIN_0        0
#define IO_PIN_1        1
#define IO_PIN_2        2
#define IO_PIN_3        3
#define IO_PIN_4        4
#define IO_PIN_5        5
#define IO_PIN_6        6
#define IO_PIN_7        7
#define IO_PIN_8        8
#define IO_PIN_9        9
#define IO_PIN_10       10
#define IO_PIN_11       11
#define IO_PIN_12       12
#define IO_PIN_13       13
#define IO_PIN_14       14
#define IO_PIN_15       15

#define IO_PINM_UNDEFINED   0
#define IO_PINM_0        ( 1 << 0 )
#define IO_PINM_1        ( 1 << 1 )
#define IO_PINM_2        ( 1 << 2 )
#define IO_PINM_3        ( 1 << 3 )
#define IO_PINM_4        ( 1 << 4 )
#define IO_PINM_5        ( 1 << 5 )
#define IO_PINM_6        ( 1 << 6 )
#define IO_PINM_7        ( 1 << 7 )
#define IO_PINM_8        ( 1 << 8 )
#define IO_PINM_9        ( 1 << 9 )
#define IO_PINM_10       ( 1 << 10 )
#define IO_PINM_11       ( 1 << 11 )
#define IO_PINM_12       ( 1 << 12 )
#define IO_PINM_13       ( 1 << 13 )
#define IO_PINM_14       ( 1 << 14 )
#define IO_PINM_15       ( 1 << 15 )


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

/*
 *****************************************************************************************
 *		Peripherals
 *****************************************************************************************
 */
extern const uC_PERIPH_IO_STRUCT PERIPH_NULL_PIO;

extern const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PA9;
extern const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PA10;

extern const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PA2;
extern const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PA3;

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


extern const uC_PERIPH_IO_STRUCT USART4_TX_PIO_PC10;
extern const uC_PERIPH_IO_STRUCT USART4_RX_PIO_PC11;

extern const uC_PERIPH_IO_STRUCT USART5_TX_PIO_PC12;
extern const uC_PERIPH_IO_STRUCT USART5_RX_PIO_PD2;


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
extern const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PA6;
extern const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PA5;
extern const uC_PERIPH_IO_STRUCT SPI1_SS_PIO_PA4;

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
extern const uC_PERIPH_IO_STRUCT SPI3_SS_PIO_PC4;

/*
 *********************************************************
 *		I2C
 */
extern const uC_PERIPH_IO_STRUCT I2C1_SCL_PIO_PB6;
extern const uC_PERIPH_IO_STRUCT I2C1_SDA_PIO_PB7;

extern const uC_PERIPH_IO_STRUCT I2C1_SCL_PIO_PB8;
extern const uC_PERIPH_IO_STRUCT I2C1_SDA_PIO_PB9;

extern const uC_PERIPH_IO_STRUCT I2C2_SCL_PIO_PB10;
extern const uC_PERIPH_IO_STRUCT I2C2_SDA_PIO_PB11;

extern const uC_PERIPH_IO_STRUCT TIMER1_PIO1_PA8;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO2_PA9;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO3_PA10;
extern const uC_PERIPH_IO_STRUCT TIMER1_PIO4_PA11;

extern const uC_PERIPH_IO_STRUCT TIMER2_PIO1_PA0;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO2_PA1;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO3_PA2;
extern const uC_PERIPH_IO_STRUCT TIMER2_PIO4_PA3;

extern const uC_PERIPH_IO_STRUCT TIMER3_PIO1_PC6;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO2_PC7;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO3_PC8;
extern const uC_PERIPH_IO_STRUCT TIMER3_PIO4_PC9;

extern const uC_PERIPH_IO_STRUCT TIMER4_PIO1_PB6;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO2_PB7;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO3_PB8;
extern const uC_PERIPH_IO_STRUCT TIMER4_PIO4_PB9;

extern const uC_PERIPH_IO_STRUCT TIMER5_PIO1_PA0;
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO2_PA1;
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO3_PA2;
extern const uC_PERIPH_IO_STRUCT TIMER5_PIO4_PA3;



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
 ******************************************
 *		Ethernet MII Peripheral
 */
extern const uC_PERIPH_IO_STRUCT ETH_MCO_CLOCK_OUT_PIO;

/// ********** Outputs **********
extern const uC_PERIPH_IO_STRUCT ETH_MII_TX_EN_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD0_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD1_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD2_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD3_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_MDC_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_MDIO_PIO;
// extern const uC_PERIPH_IO_STRUCT 	ETH_MII_PPS_OUT_PIO; //Unused

/// ********** Inputs **********
extern const uC_PERIPH_IO_STRUCT ETH_MII_COL_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_CRS_WKUP_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TX_CLK_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RX_ER_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RX_CLK_PIO;

extern const uC_PERIPH_IO_STRUCT ETH_MII_RX_DV_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD0_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD1_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD2_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_RXD3_PIO;

/*
 ******************************************
 *		Ethernet RMII Peripheral
 */
/// ********** Outputs **********
extern const uC_PERIPH_IO_STRUCT ETH_RMII_MDIO_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_TX_EN_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_TXD0_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_TXD1_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_MDC_PIO;
// extern const uC_PERIPH_IO_STRUCT 	ETH_RMII_PPS_OUT_PIO; //Unused
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


/*
 ******************************************
 *		CAN Peripheral
 */
extern const uC_PERIPH_IO_STRUCT CAN1_RX_PIO_PA11;
extern const uC_PERIPH_IO_STRUCT CAN1_TX_PIO_PA12;
extern const uC_PERIPH_IO_STRUCT CAN1_RX_PIO_PB8;
extern const uC_PERIPH_IO_STRUCT CAN1_TX_PIO_PB9;
extern const uC_PERIPH_IO_STRUCT CAN1_RX_PIO_PD0;
extern const uC_PERIPH_IO_STRUCT CAN1_TX_PIO_PD1;
#endif

