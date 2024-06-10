/*
 *****************************************************************************************
 *
 *		Copyright 2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_IO_Define_K60F12.h"
#include "App_IO_Config.h"

/*
 *****************************************************************************************
 *		General User IO
 *****************************************************************************************
 */
#define PIO_NULL_PIN_NUM      IO_PIN_UNDEFINED
const uC_USER_IO_STRUCT PIO_NULL_IO_STRUCT = { PTA, PORTA, IO_PIN_UNDEFINED, uC_PIO_INDEX_A };

#define PIOA_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOA_PIN_0_STRUCT = { PTA, PORTA, IO_PIN_0, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOA_PIN_1_STRUCT = { PTA, PORTA, IO_PIN_1, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOA_PIN_2_STRUCT = { PTA, PORTA, IO_PIN_2, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOA_PIN_3_STRUCT = { PTA, PORTA, IO_PIN_3, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOA_PIN_4_STRUCT = { PTA, PORTA, IO_PIN_4, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOA_PIN_5_STRUCT = { PTA, PORTA, IO_PIN_5, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOA_PIN_6_STRUCT = { PTA, PORTA, IO_PIN_6, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOA_PIN_7_STRUCT = { PTA, PORTA, IO_PIN_7, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOA_PIN_8_STRUCT = { PTA, PORTA, IO_PIN_8, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOA_PIN_9_STRUCT = { PTA, PORTA, IO_PIN_9, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_10       IO_PIN_10
const uC_USER_IO_STRUCT PIOA_PIN_10_STRUCT = { PTA, PORTA, IO_PIN_10, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_11       IO_PIN_11
const uC_USER_IO_STRUCT PIOA_PIN_11_STRUCT = { PTA, PORTA, IO_PIN_11, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_12       IO_PIN_12
const uC_USER_IO_STRUCT PIOA_PIN_12_STRUCT = { PTA, PORTA, IO_PIN_12, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_13       IO_PIN_13
const uC_USER_IO_STRUCT PIOA_PIN_13_STRUCT = { PTA, PORTA, IO_PIN_13, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_14       IO_PIN_14
const uC_USER_IO_STRUCT PIOA_PIN_14_STRUCT = { PTA, PORTA, IO_PIN_14, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_15       IO_PIN_15
const uC_USER_IO_STRUCT PIOA_PIN_15_STRUCT = { PTA, PORTA, IO_PIN_15, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_16       IO_PIN_16
const uC_USER_IO_STRUCT PIOA_PIN_16_STRUCT = { PTA, PORTA, IO_PIN_16, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_17       IO_PIN_17
const uC_USER_IO_STRUCT PIOA_PIN_17_STRUCT = { PTA, PORTA, IO_PIN_17, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_18       IO_PIN_18
const uC_USER_IO_STRUCT PIOA_PIN_18_STRUCT = { PTA, PORTA, IO_PIN_18, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_19       IO_PIN_19
const uC_USER_IO_STRUCT PIOA_PIN_19_STRUCT = { PTA, PORTA, IO_PIN_19, uC_PIO_INDEX_A };
// 20-23 unused
#define PIOA_PIN_NUM_24       IO_PIN_24
const uC_USER_IO_STRUCT PIOA_PIN_24_STRUCT = { PTA, PORTA, IO_PIN_24, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_25       IO_PIN_25
const uC_USER_IO_STRUCT PIOA_PIN_25_STRUCT = { PTA, PORTA, IO_PIN_25, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_26       IO_PIN_26
const uC_USER_IO_STRUCT PIOA_PIN_26_STRUCT = { PTA, PORTA, IO_PIN_26, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_27       IO_PIN_27
const uC_USER_IO_STRUCT PIOA_PIN_27_STRUCT = { PTA, PORTA, IO_PIN_27, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_28       IO_PIN_28
const uC_USER_IO_STRUCT PIOA_PIN_28_STRUCT = { PTA, PORTA, IO_PIN_28, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_29       IO_PIN_29
const uC_USER_IO_STRUCT PIOA_PIN_29_STRUCT = { PTA, PORTA, IO_PIN_29, uC_PIO_INDEX_A };
// 30-31 unused



#define PIOB_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOB_PIN_0_STRUCT = { PTB, PORTB, IO_PIN_0, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOB_PIN_1_STRUCT = { PTB, PORTB, IO_PIN_1, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOB_PIN_2_STRUCT = { PTB, PORTB, IO_PIN_2, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOB_PIN_3_STRUCT = { PTB, PORTB, IO_PIN_3, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOB_PIN_4_STRUCT = { PTB, PORTB, IO_PIN_4, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOB_PIN_5_STRUCT = { PTB, PORTB, IO_PIN_5, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOB_PIN_6_STRUCT = { PTB, PORTB, IO_PIN_6, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOB_PIN_7_STRUCT = { PTB, PORTB, IO_PIN_7, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOB_PIN_8_STRUCT = { PTB, PORTB, IO_PIN_8, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOB_PIN_9_STRUCT = { PTB, PORTB, IO_PIN_9, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_10       IO_PIN_10
const uC_USER_IO_STRUCT PIOB_PIN_10_STRUCT = { PTB, PORTB, IO_PIN_10, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_11       IO_PIN_11
const uC_USER_IO_STRUCT PIOB_PIN_11_STRUCT = { PTB, PORTB, IO_PIN_11, uC_PIO_INDEX_B };
// 12-15 unused
#define PIOB_PIN_NUM_16       IO_PIN_16
const uC_USER_IO_STRUCT PIOB_PIN_16_STRUCT = { PTB, PORTB, IO_PIN_16, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_17       IO_PIN_17
const uC_USER_IO_STRUCT PIOB_PIN_17_STRUCT = { PTB, PORTB, IO_PIN_17, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_18       IO_PIN_18
const uC_USER_IO_STRUCT PIOB_PIN_18_STRUCT = { PTB, PORTB, IO_PIN_18, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_19       IO_PIN_19
const uC_USER_IO_STRUCT PIOB_PIN_19_STRUCT = { PTB, PORTB, IO_PIN_19, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_20       IO_PIN_20
const uC_USER_IO_STRUCT PIOB_PIN_20_STRUCT = { PTB, PORTB, IO_PIN_20, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_21       IO_PIN_21
const uC_USER_IO_STRUCT PIOB_PIN_21_STRUCT = { PTB, PORTB, IO_PIN_21, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_22       IO_PIN_22
const uC_USER_IO_STRUCT PIOB_PIN_22_STRUCT = { PTB, PORTB, IO_PIN_22, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_23       IO_PIN_23
const uC_USER_IO_STRUCT PIOB_PIN_23_STRUCT = { PTB, PORTB, IO_PIN_23, uC_PIO_INDEX_B };
// 24-31 unused


#define PIOC_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOC_PIN_0_STRUCT = { PTC, PORTC, IO_PIN_0, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOC_PIN_1_STRUCT = { PTC, PORTC, IO_PIN_1, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOC_PIN_2_STRUCT = { PTC, PORTC, IO_PIN_2, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOC_PIN_3_STRUCT = { PTC, PORTC, IO_PIN_3, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOC_PIN_4_STRUCT = { PTC, PORTC, IO_PIN_4, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOC_PIN_5_STRUCT = { PTC, PORTC, IO_PIN_5, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOC_PIN_6_STRUCT = { PTC, PORTC, IO_PIN_6, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOC_PIN_7_STRUCT = { PTC, PORTC, IO_PIN_7, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOC_PIN_8_STRUCT = { PTC, PORTC, IO_PIN_8, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOC_PIN_9_STRUCT = { PTC, PORTC, IO_PIN_9, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_10       IO_PIN_10
const uC_USER_IO_STRUCT PIOC_PIN_10_STRUCT = { PTC, PORTC, IO_PIN_10, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_11       IO_PIN_11
const uC_USER_IO_STRUCT PIOC_PIN_11_STRUCT = { PTC, PORTC, IO_PIN_11, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_12       IO_PIN_12
const uC_USER_IO_STRUCT PIOC_PIN_12_STRUCT = { PTC, PORTC, IO_PIN_12, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_13       IO_PIN_13
const uC_USER_IO_STRUCT PIOC_PIN_13_STRUCT = { PTC, PORTC, IO_PIN_13, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_14       IO_PIN_14
const uC_USER_IO_STRUCT PIOC_PIN_14_STRUCT = { PTC, PORTC, IO_PIN_14, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_15       IO_PIN_15
const uC_USER_IO_STRUCT PIOC_PIN_15_STRUCT = { PTC, PORTC, IO_PIN_15, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_16       IO_PIN_16
const uC_USER_IO_STRUCT PIOC_PIN_16_STRUCT = { PTC, PORTC, IO_PIN_16, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_17       IO_PIN_17
const uC_USER_IO_STRUCT PIOC_PIN_17_STRUCT = { PTC, PORTC, IO_PIN_17, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_18       IO_PIN_18
const uC_USER_IO_STRUCT PIOC_PIN_18_STRUCT = { PTC, PORTC, IO_PIN_18, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_19       IO_PIN_19
const uC_USER_IO_STRUCT PIOC_PIN_19_STRUCT = { PTC, PORTC, IO_PIN_19, uC_PIO_INDEX_C };
// 20-31 unused


#define PIOD_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOD_PIN_0_STRUCT = { PTD, PORTD, IO_PIN_0, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOD_PIN_1_STRUCT = { PTD, PORTD, IO_PIN_1, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOD_PIN_2_STRUCT = { PTD, PORTD, IO_PIN_2, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOD_PIN_3_STRUCT = { PTD, PORTD, IO_PIN_3, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOD_PIN_4_STRUCT = { PTD, PORTD, IO_PIN_4, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOD_PIN_5_STRUCT = { PTD, PORTD, IO_PIN_5, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOD_PIN_6_STRUCT = { PTD, PORTD, IO_PIN_6, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOD_PIN_7_STRUCT = { PTD, PORTD, IO_PIN_7, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOD_PIN_8_STRUCT = { PTD, PORTD, IO_PIN_8, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOD_PIN_9_STRUCT = { PTD, PORTD, IO_PIN_9, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_10       IO_PIN_10
const uC_USER_IO_STRUCT PIOD_PIN_10_STRUCT = { PTD, PORTD, IO_PIN_10, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_11       IO_PIN_11
const uC_USER_IO_STRUCT PIOD_PIN_11_STRUCT = { PTD, PORTD, IO_PIN_11, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_12       IO_PIN_12
const uC_USER_IO_STRUCT PIOD_PIN_12_STRUCT = { PTD, PORTD, IO_PIN_12, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_13       IO_PIN_13
const uC_USER_IO_STRUCT PIOD_PIN_13_STRUCT = { PTD, PORTD, IO_PIN_13, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_14       IO_PIN_14
const uC_USER_IO_STRUCT PIOD_PIN_14_STRUCT = { PTD, PORTD, IO_PIN_14, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_15       IO_PIN_15
const uC_USER_IO_STRUCT PIOD_PIN_15_STRUCT = { PTD, PORTD, IO_PIN_15, uC_PIO_INDEX_D };
// 16-31 unused


#define PIOE_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOE_PIN_0_STRUCT = { PTE, PORTE, IO_PIN_0, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOE_PIN_1_STRUCT = { PTE, PORTE, IO_PIN_1, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOE_PIN_2_STRUCT = { PTE, PORTE, IO_PIN_2, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOE_PIN_3_STRUCT = { PTE, PORTE, IO_PIN_3, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOE_PIN_4_STRUCT = { PTE, PORTE, IO_PIN_4, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOE_PIN_5_STRUCT = { PTE, PORTE, IO_PIN_5, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOE_PIN_6_STRUCT = { PTE, PORTE, IO_PIN_6, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOE_PIN_7_STRUCT = { PTE, PORTE, IO_PIN_7, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOE_PIN_8_STRUCT = { PTE, PORTE, IO_PIN_8, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOE_PIN_9_STRUCT = { PTE, PORTE, IO_PIN_9, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_10       IO_PIN_10
const uC_USER_IO_STRUCT PIOE_PIN_10_STRUCT = { PTE, PORTE, IO_PIN_10, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_11       IO_PIN_11
const uC_USER_IO_STRUCT PIOE_PIN_11_STRUCT = { PTE, PORTE, IO_PIN_11, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_12       IO_PIN_12
const uC_USER_IO_STRUCT PIOE_PIN_12_STRUCT = { PTE, PORTE, IO_PIN_12, uC_PIO_INDEX_E };
// 13-23 unused
#define PIOE_PIN_NUM_24       IO_PIN_24
const uC_USER_IO_STRUCT PIOE_PIN_24_STRUCT = { PTE, PORTE, IO_PIN_24, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_25       IO_PIN_25
const uC_USER_IO_STRUCT PIOE_PIN_25_STRUCT = { PTE, PORTE, IO_PIN_25, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_26       IO_PIN_26
const uC_USER_IO_STRUCT PIOE_PIN_26_STRUCT = { PTE, PORTE, IO_PIN_26, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_27       IO_PIN_27
const uC_USER_IO_STRUCT PIOE_PIN_27_STRUCT = { PTE, PORTE, IO_PIN_27, uC_PIO_INDEX_E };
#define PIOE_PIN_NUM_28       IO_PIN_28
const uC_USER_IO_STRUCT PIOE_PIN_28_STRUCT = { PTE, PORTE, IO_PIN_28, uC_PIO_INDEX_E };
// 29-31 unused

/*
 *****************************************************************************************
 *		Peripherals IO
 *****************************************************************************************
 */
const uC_PERIPH_IO_STRUCT PERIPH_NULL_PIO = { PTA, PORTA, IO_PIN_UNDEFINED, uC_IO_ALTERNATE_MAP_UNSUPPORTED };

/*
 ******************************************
 *		UART Peripheral
 */
// UART0 - Strip Port 2
const uC_PERIPH_IO_STRUCT USART0_TX_PIO_PD7 = { PTD, PORTD, IO_PIN_7, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART0_RX_PIO_PD6 = { PTD, PORTD, IO_PIN_6, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART0_RTS_PIO_PD4 = { PTD, PORTD, IO_PIN_4, uC_IO_ALTERNATE_MAP_1 };
// UART1 - Strip Port 1
const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PC4 = { PTC, PORTC, IO_PIN_4, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PC3 = { PTC, PORTC, IO_PIN_3, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART1_RTS_PIO_PC1 = { PTC, PORTC, IO_PIN_1, uC_IO_ALTERNATE_MAP_1 };
// UART2 - Strip Port 3
const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PD3 = { PTD, PORTD, IO_PIN_3, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PD2 = { PTD, PORTD, IO_PIN_2, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART2_RTS_PIO_PD0 = { PTD, PORTD, IO_PIN_0, uC_IO_ALTERNATE_MAP_1 };
// UART3 - MODBUS
const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PC16 = { PTC, PORTC, IO_PIN_16, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PC17 = { PTC, PORTC, IO_PIN_17, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART3_RTS_PIO_PC18 = { PTC, PORTC, IO_PIN_18, uC_IO_ALTERNATE_MAP_1 };
// UART4 - Strip Port 4
const uC_PERIPH_IO_STRUCT USART4_TX_PIO_PC15 = { PTC, PORTC, IO_PIN_15, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART4_RX_PIO_PC14 = { PTC, PORTC, IO_PIN_14, uC_IO_ALTERNATE_MAP_3 };
const uC_PERIPH_IO_STRUCT USART4_RTS_PIO_PC12 = { PTC, PORTC, IO_PIN_12, uC_IO_ALTERNATE_MAP_1 };
// UART5 - Unused

/*
 ******************************************
 *		SPI Peripheral
 */
// SPI 0 unused
// SPI 1 unused
//
// SPI 2
const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PD13 = { PTD, PORTD, IO_PIN_13, uC_IO_ALTERNATE_MAP_2 };
const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PD14 = { PTD, PORTD, IO_PIN_14, uC_IO_ALTERNATE_MAP_2 };
const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PD12 = { PTD, PORTD, IO_PIN_12, uC_IO_ALTERNATE_MAP_2 };
const uC_PERIPH_IO_STRUCT SPI2_PCS0_PIO_PD11 = { PTD, PORTD, IO_PIN_11, uC_IO_ALTERNATE_MAP_1 };

/*
 ******************************************
 *		I2C Peripheral
 */
// No IO pins in use for I2C at this time

/*
 ******************************************
 *		Timer Peripheral
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


/*
 ******************************************
 *		Ethernet RMII Peripheral
 */
/// ********** Outputs **********
const uC_PERIPH_IO_STRUCT ETH_RMII_MDIO_PIO = { PTB, PORTB, IO_PIN_0, uC_IO_ALTERNATE_MAP_4 };
const uC_PERIPH_IO_STRUCT ETH_RMII_MDC_PIO = { PTB, PORTB, IO_PIN_1, uC_IO_ALTERNATE_MAP_4 };
const uC_PERIPH_IO_STRUCT ETH_RMII_TX_EN_PIO = { PTA, PORTA, IO_PIN_15, uC_IO_ALTERNATE_MAP_4 };
const uC_PERIPH_IO_STRUCT ETH_RMII_TXD0_PIO = { PTA, PORTA, IO_PIN_16, uC_IO_ALTERNATE_MAP_4 };
const uC_PERIPH_IO_STRUCT ETH_RMII_TXD1_PIO = { PTA, PORTA, IO_PIN_17, uC_IO_ALTERNATE_MAP_4 };

/// ********** Inputs **********
const uC_PERIPH_IO_STRUCT ETH_RMII_CRS_DV_PIO = { PTA, PORTA, IO_PIN_14, uC_IO_ALTERNATE_MAP_4 };
const uC_PERIPH_IO_STRUCT ETH_RMII_RXD0_PIO = { PTA, PORTA, IO_PIN_13, uC_IO_ALTERNATE_MAP_4 };
const uC_PERIPH_IO_STRUCT ETH_RMII_RXD1_PIO = { PTA, PORTA, IO_PIN_12, uC_IO_ALTERNATE_MAP_4 };
const uC_PERIPH_IO_STRUCT ETH_RMII_RXER_PIO = { PTA, PORTA, IO_PIN_5, uC_IO_ALTERNATE_MAP_4 };
const uC_PERIPH_IO_STRUCT ETH_RMII_PHY_IRQ = { PTB, PORTB, IO_PIN_6, uC_IO_ALTERNATE_MAP_1};

/*
 *****************************************************************************************
 *		Direction Selections
 *****************************************************************************************
 */
#ifndef PIO_A_SET_OUTPUT_MASK
	#define PIO_A_SET_OUTPUT_MASK       0
	#define PIO_A_OUTPUT_INIT_MASK      0
	#define PIO_A_SET_INPUT_MASK        0
	#define PIO_A_INPUT_PULLUP_MASK     0
		#define PIO_A_INPUT_PULLDOWN_MASK   0

	#define PIO_B_SET_OUTPUT_MASK       0
	#define PIO_B_OUTPUT_INIT_MASK      0
	#define PIO_B_SET_INPUT_MASK        0
	#define PIO_B_INPUT_PULLUP_MASK     0
		#define PIO_B_INPUT_PULLDOWN_MASK   0

	#define PIO_C_SET_OUTPUT_MASK       0
	#define PIO_C_OUTPUT_INIT_MASK      0
	#define PIO_C_SET_INPUT_MASK        0
	#define PIO_C_INPUT_PULLUP_MASK     0
		#define PIO_C_INPUT_PULLDOWN_MASK   0

	#define PIO_D_SET_OUTPUT_MASK       0
	#define PIO_D_OUTPUT_INIT_MASK      0
	#define PIO_D_SET_INPUT_MASK        0
	#define PIO_D_INPUT_PULLUP_MASK     0
		#define PIO_D_INPUT_PULLDOWN_MASK   0

	#define PIO_E_SET_OUTPUT_MASK       0
	#define PIO_E_OUTPUT_INIT_MASK      0
	#define PIO_E_SET_INPUT_MASK        0
	#define PIO_E_INPUT_PULLUP_MASK     0
		#define PIO_E_INPUT_PULLDOWN_MASK   0
#endif


// *****************************************
// **		IO Port A
const uint32_t SET_PIO_A_OUTPUT = PIO_A_SET_OUTPUT_MASK;
const uint32_t SET_PIO_A_OUTPUT_INIT = PIO_A_OUTPUT_INIT_MASK;
const uint32_t SET_PIO_A_INPUT = PIO_A_SET_INPUT_MASK;
const uint32_t SET_PIO_A_INPUT_PULLUP = PIO_A_INPUT_PULLUP_MASK;
const uint32_t SET_PIO_A_INPUT_PULLDOWN = PIO_A_INPUT_PULLDOWN_MASK;

// *****************************************
// **		IO Port B
const uint32_t SET_PIO_B_OUTPUT = PIO_B_SET_OUTPUT_MASK;
const uint32_t SET_PIO_B_OUTPUT_INIT = PIO_B_OUTPUT_INIT_MASK;
const uint32_t SET_PIO_B_INPUT = PIO_B_SET_INPUT_MASK;
const uint32_t SET_PIO_B_INPUT_PULLUP = PIO_B_INPUT_PULLUP_MASK;
const uint32_t SET_PIO_B_INPUT_PULLDOWN = PIO_B_INPUT_PULLDOWN_MASK;

// *****************************************
// **		IO Port C
const uint32_t SET_PIO_C_OUTPUT = PIO_C_SET_OUTPUT_MASK;
const uint32_t SET_PIO_C_OUTPUT_INIT = PIO_C_OUTPUT_INIT_MASK;
const uint32_t SET_PIO_C_INPUT = PIO_C_SET_INPUT_MASK;
const uint32_t SET_PIO_C_INPUT_PULLUP = PIO_C_INPUT_PULLUP_MASK;
const uint32_t SET_PIO_C_INPUT_PULLDOWN = PIO_C_INPUT_PULLDOWN_MASK;

// *****************************************
// **		IO Port D
const uint32_t SET_PIO_D_OUTPUT = PIO_D_SET_OUTPUT_MASK;
const uint32_t SET_PIO_D_OUTPUT_INIT = PIO_D_OUTPUT_INIT_MASK;
const uint32_t SET_PIO_D_INPUT = PIO_D_SET_INPUT_MASK;
const uint32_t SET_PIO_D_INPUT_PULLUP = PIO_D_INPUT_PULLUP_MASK;
const uint32_t SET_PIO_D_INPUT_PULLDOWN = PIO_D_INPUT_PULLDOWN_MASK;

// *****************************************
// **		IO Port E
const uint32_t SET_PIO_E_OUTPUT = PIO_E_SET_OUTPUT_MASK;
const uint32_t SET_PIO_E_OUTPUT_INIT = PIO_E_OUTPUT_INIT_MASK;
const uint32_t SET_PIO_E_INPUT = PIO_E_SET_INPUT_MASK;
const uint32_t SET_PIO_E_INPUT_PULLUP = PIO_E_INPUT_PULLUP_MASK;
const uint32_t SET_PIO_E_INPUT_PULLDOWN = PIO_E_INPUT_PULLDOWN_MASK;

