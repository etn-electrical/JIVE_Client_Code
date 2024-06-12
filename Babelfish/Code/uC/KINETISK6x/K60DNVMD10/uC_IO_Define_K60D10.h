/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *
 *****************************************************************************************
 */
#ifndef uC_IO_DEFINE_K60D10_H
#define uC_IO_DEFINE_K60D10_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "uC_K60D10.h"

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
	GPIO_Type* gio_ctrl;
	PORT_Type* pin_ctrl;		// This is the reference to the main config structure.
	UINT8 pio_num;				// This is the reference relative to a certain port (ie: io num 3 of PortA)
	UINT8 port_index;			// This is the port reference.
} uC_USER_IO_STRUCT;


/// This IO configuration behavior is best explained in the datasheet under the GPIO and alternate mapping
/// sections.  This code defines the placement of the remap data if supported for the peripheral pins.
enum
{
	uC_IO_ALTERNATE_MAP_ANALOG,
	uC_IO_ALTERNATE_MAP_1,	// almost always GPIO
	uC_IO_ALTERNATE_MAP_2,
	uC_IO_ALTERNATE_MAP_3,
	uC_IO_ALTERNATE_MAP_4,
	uC_IO_ALTERNATE_MAP_5,
	uC_IO_ALTERNATE_MAP_6,
	uC_IO_ALTERNATE_MAP_7,
	uC_IO_ALTERNATE_MAP_UNSUPPORTED,
	uC_IO_ALTERNATE_MAP_MAX
};

#define uC_IO_ALTERNATE_MAP_MASK        0xF

typedef struct
{
	GPIO_Type* gio_ctrl;
	PORT_Type* pin_ctrl;
	UINT8 pio_num;
	UINT8 alt_map;
} uC_PERIPH_IO_STRUCT;


// ************************************************************
// ****  The pin defines are here to provide searchability for pin numbers
// ************************************************************
#define IO_PIN_UNDEFINED    32
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
#define IO_PIN_16       16
#define IO_PIN_17       17
#define IO_PIN_18       18
#define IO_PIN_19       19
#define IO_PIN_20       20
#define IO_PIN_21       21
#define IO_PIN_22       22
#define IO_PIN_23       23
#define IO_PIN_24       24
#define IO_PIN_25       25
#define IO_PIN_26       26
#define IO_PIN_27       27
#define IO_PIN_28       28
#define IO_PIN_29       29
#define IO_PIN_30       30
#define IO_PIN_31       31

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
#define IO_PINM_16       ( 1 << 16 )
#define IO_PINM_17       ( 1 << 17 )
#define IO_PINM_18       ( 1 << 18 )
#define IO_PINM_19       ( 1 << 19 )
#define IO_PINM_20       ( 1 << 20 )
#define IO_PINM_21       ( 1 << 21 )
#define IO_PINM_22       ( 1 << 22 )
#define IO_PINM_23       ( 1 << 23 )
#define IO_PINM_24       ( 1 << 24 )
#define IO_PINM_25       ( 1 << 25 )
#define IO_PINM_26       ( 1 << 26 )
#define IO_PINM_27       ( 1 << 27 )
#define IO_PINM_28       ( 1 << 28 )
#define IO_PINM_29       ( 1 << 29 )
#define IO_PINM_30       ( 1 << 30 )
#define IO_PINM_31       ( 1 << 31 )


/*
 *****************************************************************************************
 *		Standard IO Pins
 *****************************************************************************************
 */
extern const uC_USER_IO_STRUCT PIO_NULL_IO_STRUCT;

// commented out lines indicate unused on K60D10

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
extern const uC_USER_IO_STRUCT PIOA_PIN_16_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_17_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_18_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_19_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOA_PIN_20_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOA_PIN_21_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOA_PIN_22_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOA_PIN_23_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_24_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_25_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_26_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_27_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_28_STRUCT;
extern const uC_USER_IO_STRUCT PIOA_PIN_29_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOA_PIN_30_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOA_PIN_31_STRUCT;


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
// extern const uC_USER_IO_STRUCT        PIOB_PIN_12_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_13_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_14_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_15_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_16_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_17_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_18_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_19_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_20_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_21_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_22_STRUCT;
extern const uC_USER_IO_STRUCT PIOB_PIN_23_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_24_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_25_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_26_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_27_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_28_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_29_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_30_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOB_PIN_31_STRUCT;


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
extern const uC_USER_IO_STRUCT PIOC_PIN_16_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_17_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_18_STRUCT;
extern const uC_USER_IO_STRUCT PIOC_PIN_19_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_20_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_21_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_22_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_23_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_24_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_25_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_26_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_27_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_28_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_29_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_30_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOC_PIN_31_STRUCT;


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
// extern const uC_USER_IO_STRUCT        PIOD_PIN_16_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_17_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_18_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_19_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_20_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_21_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_22_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_23_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_24_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_25_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_26_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_27_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_28_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_29_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_30_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOD_PIN_31_STRUCT;


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
// extern const uC_USER_IO_STRUCT        PIOE_PIN_13_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_14_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_15_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_16_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_17_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_18_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_19_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_20_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_21_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_22_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_23_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_24_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_25_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_26_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_27_STRUCT;
extern const uC_USER_IO_STRUCT PIOE_PIN_28_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_29_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_30_STRUCT;
// extern const uC_USER_IO_STRUCT        PIOE_PIN_31_STRUCT;


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
extern const uC_PERIPH_IO_STRUCT USART0_TX_PIO_PD7;
extern const uC_PERIPH_IO_STRUCT USART0_RX_PIO_PD6;
extern const uC_PERIPH_IO_STRUCT USART0_RTS_PIO_PD4;

extern const uC_PERIPH_IO_STRUCT USART0_TX_PIO_PA14;
extern const uC_PERIPH_IO_STRUCT USART0_RX_PIO_PA15;

extern const uC_PERIPH_IO_STRUCT USART0_TX_PIO_PB17;
extern const uC_PERIPH_IO_STRUCT USART0_RX_PIO_PB16;

extern const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PC4;
extern const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PC3;
extern const uC_PERIPH_IO_STRUCT USART1_RTS_PIO_PC1;

extern const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PE0;
extern const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PE1;

extern const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PD3;
extern const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PD2;
extern const uC_PERIPH_IO_STRUCT USART2_RTS_PIO_PD0;

extern const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PC17;
extern const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PC16;
extern const uC_PERIPH_IO_STRUCT USART3_RTS_PIO_PC18;

extern const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PB11;
extern const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PB10;

extern const uC_PERIPH_IO_STRUCT USART4_TX_PIO_PC15;
extern const uC_PERIPH_IO_STRUCT USART4_RX_PIO_PC14;
extern const uC_PERIPH_IO_STRUCT USART4_RTS_PIO_PC12;

extern const uC_PERIPH_IO_STRUCT USART5_TX_PIO_PD9;
extern const uC_PERIPH_IO_STRUCT USART5_RX_PIO_PD8;

/*
 *********************************************************
 *		SPI
 */
// SPI 0
extern const uC_PERIPH_IO_STRUCT SPI0_MOSI_PIO_PC7;
extern const uC_PERIPH_IO_STRUCT SPI0_MISO_PIO_PC6;
extern const uC_PERIPH_IO_STRUCT SPI0_CLOCK_PIO_PC5;
extern const uC_PERIPH_IO_STRUCT SPI0_PCS0_PIO_PC4;

// SPI 1 unused
//
// SPI 2
extern const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PD13;
extern const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PD14;
extern const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PD12;
extern const uC_PERIPH_IO_STRUCT SPI2_PCS0_PIO_PD11;


/*
 *********************************************************
 *		I2C
 */
// No IO pins in use for I2C at this time


/*
 *********************************************************
 *		Timer
 */
// No IO pins in use for timers at this time


/*
 ******************************************
 *		ADC Peripheral
 */
// No IO pins in use for ADC at this time

/*
 ******************************************
 *		Ethernet MII Peripheral
 */
// MII interface not in use at this time
/// ********** Outputs **********
extern const uC_PERIPH_IO_STRUCT ETH_MII_MDIO_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TX_EN_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD0_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD1_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD2_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_TXD3_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_MII_MDC_PIO;

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

/// ********** Inputs **********
extern const uC_PERIPH_IO_STRUCT ETH_RMII_REF_CLK_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_CRS_DV_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_RXD0_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_RXD1_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_RXER_PIO;
extern const uC_PERIPH_IO_STRUCT ETH_RMII_PHY_IRQ;

/*
 *****************************************************************************************
 *		Direction Selection
 *****************************************************************************************
 */
extern const UINT32 SET_PIO_A_OUTPUT;
extern const UINT32 SET_PIO_A_OUTPUT_INIT;
extern const UINT32 SET_PIO_A_INPUT;
extern const UINT32 SET_PIO_A_INPUT_PULLUP;
extern const UINT32 SET_PIO_A_INPUT_PULLDOWN;

extern const UINT32 SET_PIO_B_OUTPUT;
extern const UINT32 SET_PIO_B_OUTPUT_INIT;
extern const UINT32 SET_PIO_B_INPUT;
extern const UINT32 SET_PIO_B_INPUT_PULLUP;
extern const UINT32 SET_PIO_B_INPUT_PULLUP;
extern const UINT32 SET_PIO_B_INPUT_PULLDOWN;

extern const UINT32 SET_PIO_C_OUTPUT;
extern const UINT32 SET_PIO_C_OUTPUT_INIT;
extern const UINT32 SET_PIO_C_INPUT;
extern const UINT32 SET_PIO_C_INPUT_PULLUP;
extern const UINT32 SET_PIO_C_INPUT_PULLDOWN;

extern const UINT32 SET_PIO_D_OUTPUT;
extern const UINT32 SET_PIO_D_OUTPUT_INIT;
extern const UINT32 SET_PIO_D_INPUT;
extern const UINT32 SET_PIO_D_INPUT_PULLUP;
extern const UINT32 SET_PIO_D_INPUT_PULLDOWN;

extern const UINT32 SET_PIO_E_OUTPUT;
extern const UINT32 SET_PIO_E_OUTPUT_INIT;
extern const UINT32 SET_PIO_E_INPUT;
extern const UINT32 SET_PIO_E_INPUT_PULLUP;
extern const UINT32 SET_PIO_E_INPUT_PULLDOWN;


#endif

