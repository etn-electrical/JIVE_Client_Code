/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */


/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 * all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 * signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 * re-enabled at the end of this file
 */
/*lint -e1960  */



/*TODO [Sumit] Verify the port w.r.t. device availaibility, file names were changed
 * rom STM32L451 to STM32WB55R
 */


#include "Includes.h"
#include "uC_IO_Define_STM32WB55R.h"
#include "App_IO_Config.h"

/*
 *****************************************************************************************
 *		General User IO
 *****************************************************************************************
 */
#define PIO_nullptr_PIN_NUM   IO_PIN_UNDEFINED
const uC_USER_IO_STRUCT PIO_NULL_IO_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_UNDEFINED ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };

#define PIOA_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOA_PIN_0_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOA_PIN_1_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOA_PIN_2_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOA_PIN_3_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOA_PIN_4_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOA_PIN_5_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOA_PIN_6_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOA_PIN_7_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOA_PIN_8_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOA_PIN_9_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_10       IO_PIN_10
const uC_USER_IO_STRUCT PIOA_PIN_10_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_11       IO_PIN_11
const uC_USER_IO_STRUCT PIOA_PIN_11_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_12       IO_PIN_12
const uC_USER_IO_STRUCT PIOA_PIN_12_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_13       IO_PIN_13
const uC_USER_IO_STRUCT PIOA_PIN_13_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_14       IO_PIN_14
const uC_USER_IO_STRUCT PIOA_PIN_14_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_15       IO_PIN_15
const uC_USER_IO_STRUCT PIOA_PIN_15_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };


#define PIOB_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOB_PIN_0_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOB_PIN_1_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOB_PIN_2_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOB_PIN_3_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOB_PIN_4_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOB_PIN_5_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOB_PIN_6_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOB_PIN_7_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOB_PIN_8_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOB_PIN_9_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_10       IO_PIN_10
const uC_USER_IO_STRUCT PIOB_PIN_10_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_11       IO_PIN_11
const uC_USER_IO_STRUCT PIOB_PIN_11_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_12       IO_PIN_12
const uC_USER_IO_STRUCT PIOB_PIN_12_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_13       IO_PIN_13
const uC_USER_IO_STRUCT PIOB_PIN_13_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_14       IO_PIN_14
const uC_USER_IO_STRUCT PIOB_PIN_14_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_15       IO_PIN_15
const uC_USER_IO_STRUCT PIOB_PIN_15_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };


#define PIOC_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOC_PIN_0_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOC_PIN_1_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOC_PIN_2_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOC_PIN_3_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOC_PIN_4_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOC_PIN_5_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOC_PIN_6_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOC_PIN_7_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOC_PIN_8_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOC_PIN_9_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_10       IO_PIN_10
const uC_USER_IO_STRUCT PIOC_PIN_10_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_11       IO_PIN_11
const uC_USER_IO_STRUCT PIOC_PIN_11_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_12       IO_PIN_12
const uC_USER_IO_STRUCT PIOC_PIN_12_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_13       IO_PIN_13
const uC_USER_IO_STRUCT PIOC_PIN_13_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_14       IO_PIN_14
const uC_USER_IO_STRUCT PIOC_PIN_14_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_15       IO_PIN_15
const uC_USER_IO_STRUCT PIOC_PIN_15_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };


#define PIOD_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOD_PIN_0_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOD_PIN_1_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOD_PIN_2_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOD_PIN_3_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOD_PIN_4_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOD_PIN_5_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOD_PIN_6_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOD_PIN_7_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOD_PIN_8_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOD_PIN_9_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_10       IO_PIN_10
const uC_USER_IO_STRUCT PIOD_PIN_10_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_11       IO_PIN_11
const uC_USER_IO_STRUCT PIOD_PIN_11_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_12       IO_PIN_12
const uC_USER_IO_STRUCT PIOD_PIN_12_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_13       IO_PIN_13
const uC_USER_IO_STRUCT PIOD_PIN_13_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_14       IO_PIN_14
const uC_USER_IO_STRUCT PIOD_PIN_14_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_15       IO_PIN_15
const uC_USER_IO_STRUCT PIOD_PIN_15_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };


#define PIOE_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOE_PIN_0_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOE_PIN_1_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOE_PIN_2_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOE_PIN_3_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOE_PIN_4_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };


#define PIOH_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOH_PIN_0_STRUCT =
{ GPIOH, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_PIO_INDEX_H ) };
#define PIOH_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOH_PIN_1_STRUCT =
{ GPIOH, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_PIO_INDEX_H ) };
#define PIOH_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOH_PIN_3_STRUCT =
{ GPIOH, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_PIO_INDEX_H ) };

/*
 *****************************************************************************************
 *		Peripherals IO
 *****************************************************************************************
 */
const uC_PERIPH_IO_STRUCT PERIPH_NULL_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_UNDEFINED ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_UNSUPPORTED ) };

/*
 ******************************************
 *		USART Peripheral
 */

const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PA9 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_7 ) };
const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PA10 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_7 ) };
const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PB6 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_7 ) };
const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PB7 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_7 ) };



/*
 ******************************************
 *		LPUART Peripheral
 */
const uC_PERIPH_IO_STRUCT LPUART1_TX_PIO_PA2 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_8 ) };
const uC_PERIPH_IO_STRUCT LPUART1_RX_PIO_PA3 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_8 ) };
const uC_PERIPH_IO_STRUCT LPUART1_TX_PIO_PC1 =
{ GPIOC, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_8 ) };
const uC_PERIPH_IO_STRUCT LPUART1_RX_PIO_PC0 =
{ GPIOC, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_8 ) };
const uC_PERIPH_IO_STRUCT LPUART1_TX_PIO_PB11 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_8 ) };
const uC_PERIPH_IO_STRUCT LPUART1_RX_PIO_PB10 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_8 ) };
const uC_PERIPH_IO_STRUCT LPUART1_RTS_PIO_PB12 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_8 ) };
const uC_PERIPH_IO_STRUCT LPUART1_CTS_PIO_PB13 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_8 ) };

/*
 ******************************************
 *		SPI Peripheral
 */

const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PA7 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PA6 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PA5 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI1_NSS_PIO_PA4 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };

const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PA12 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PA11 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PA1 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI1_NSS_PIO_PA15 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };

const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO_PB5 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO_PB4 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO_PB3 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI1_NSS_PIO_PB2 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };

const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PB15 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PB14 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PB13 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI2_NSS_PIO_PB12 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };

const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PD4 =
{ GPIOD, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PD3 =
{ GPIOD, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PD1 =
{ GPIOD, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI2_NSS_PIO_PD0 =
{ GPIOD, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };

const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO_PC3 =
{ GPIOC, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO_PC2 =
{ GPIOC, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO_PB10 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };
const uC_PERIPH_IO_STRUCT SPI2_NSS_PIO_PB9 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_5 ) };

/*
 ******************************************
 *		I2C Peripheral
 */
const uC_PERIPH_IO_STRUCT I2C1_SCL_PIO_PB8 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_4 ) };
const uC_PERIPH_IO_STRUCT I2C1_SDA_PIO_PB9 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_4 ) };

const uC_PERIPH_IO_STRUCT I2C3_SCL_PIO_PA8 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_4 ) };
const uC_PERIPH_IO_STRUCT I2C3_SDA_PIO_PC9 =
{ GPIOC, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_4 ) };

/*
 ******************************************
 *		Timer Peripheral
 */

const uC_PERIPH_IO_STRUCT TIMER1_PIO1_PA8 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO2_PA9 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO3_PA10 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO4_PA11 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO1_PE9 =
{ GPIOE, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO2_PE11 =
{ GPIOE, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO3_PE13 =
{ GPIOE, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO4_PE14 =
{ GPIOE, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO1_PB13 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO2_PB14 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO3_PB15 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO1_PA7 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO2_PB8 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER1_PIO3_PB9 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };

const uC_PERIPH_IO_STRUCT TIMER2_PIO1_PA0 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER2_PIO2_PA1 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER2_PIO3_PA2 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER2_PIO4_PA3 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER2_PIO1_PA5 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER2_PIO1_PA15 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER2_PIO2_PB3 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER2_PIO3_PB10 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT TIMER2_PIO4_PB11 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };


// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO1_PA6    = { GPIOA, static_cast<uint8_t>(IO_PIN_6),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO2_PA7    = { GPIOA, static_cast<uint8_t>(IO_PIN_7),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO3_PB0    = { GPIOB, static_cast<uint8_t>(IO_PIN_0),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO4_PB1    = { GPIOB, static_cast<uint8_t>(IO_PIN_1),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO1_PB4    = { GPIOB, static_cast<uint8_t>(IO_PIN_4),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO2_PB5    = { GPIOB, static_cast<uint8_t>(IO_PIN_5),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO1_PC6    = { GPIOC, static_cast<uint8_t>(IO_PIN_8),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO2_PC7    = { GPIOB, static_cast<uint8_t>(IO_PIN_1),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO3_PC8    = { GPIOC, static_cast<uint8_t>(IO_PIN_6),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO4_PC9    = { GPIOC, static_cast<uint8_t>(IO_PIN_9),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
//
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO1_PB6    = { GPIOB, static_cast<uint8_t>(IO_PIN_6),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO2_PB7    = { GPIOB, static_cast<uint8_t>(IO_PIN_7),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO3_PB8    = { GPIOB, static_cast<uint8_t>(IO_PIN_8),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO4_PB9    = { GPIOB, static_cast<uint8_t>(IO_PIN_9),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO1_PD12    = { GPIOD, static_cast<uint8_t>(IO_PIN_12),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO2_PD13    = { GPIOD, static_cast<uint8_t>(IO_PIN_13),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO3_PD14    = { GPIOD, static_cast<uint8_t>(IO_PIN_14),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO4_PD15    = { GPIOD, static_cast<uint8_t>(IO_PIN_15),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_2) };
//
const uC_PERIPH_IO_STRUCT TIMER5_PIO1_PA0 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_2 ) };
const uC_PERIPH_IO_STRUCT TIMER5_PIO2_PA1 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_2 ) };
const uC_PERIPH_IO_STRUCT TIMER5_PIO3_PA2 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_2 ) };
const uC_PERIPH_IO_STRUCT TIMER5_PIO4_PA3 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_2 ) };
//
// const uC_PERIPH_IO_STRUCT 	TIMER8_PIO1_PC6    = { GPIOC, static_cast<uint8_t>(IO_PIN_6),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_3) };
// const uC_PERIPH_IO_STRUCT 	TIMER8_PIO2_PC7    = { GPIOC, static_cast<uint8_t>(IO_PIN_7),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_3) };
// const uC_PERIPH_IO_STRUCT 	TIMER8_PIO3_PC8    = { GPIOC, static_cast<uint8_t>(IO_PIN_8),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_3) };
// const uC_PERIPH_IO_STRUCT 	TIMER8_PIO4_PC9    = { GPIOC, static_cast<uint8_t>(IO_PIN_9),
// static_cast<uint8_t>(uC_IO_ALTERNATE_MAP_3) };

const uC_PERIPH_IO_STRUCT TIMER16_PIO1_PA6 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };
const uC_PERIPH_IO_STRUCT TIMER16_PIO1_PE0 =
{ GPIOE, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };
const uC_PERIPH_IO_STRUCT TIMER16_PIO1_PB8 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };
const uC_PERIPH_IO_STRUCT TIMER16_PIO1_PB6 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };

const uC_PERIPH_IO_STRUCT TIMER17_PIO1_PB7 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };
const uC_PERIPH_IO_STRUCT TIMER17_PIO1_PE1 =
{ GPIOE, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };
const uC_PERIPH_IO_STRUCT TIMER17_PIO1_PA7 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };
const uC_PERIPH_IO_STRUCT TIMER17_PIO1_PB9 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };

const uC_PERIPH_IO_STRUCT LPTIMER1_PIO1_PC1 =
{ GPIOC, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT LPTIMER1_PIO1_PB2 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };
const uC_PERIPH_IO_STRUCT LPTIMER1_PIO1_PA14 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) };

const uC_PERIPH_IO_STRUCT LPTIMER2_PIO1_PA4 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };
const uC_PERIPH_IO_STRUCT LPTIMER2_PIO1_PA8 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_14 ) };

/*
 ******************************************
 *		ADC Peripheral
 */
const uC_PERIPH_IO_STRUCT ADC1_IN0_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_UNDEFINED ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN1_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN2_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN3_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN4_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN5_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN6_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN7_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN8_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN9_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN10_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN11_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN12_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN13_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN14_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN15_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN16_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN17_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_UNDEFINED ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };
const uC_PERIPH_IO_STRUCT ADC1_IN18_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_UNDEFINED ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) };

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

	#define PIO_H_SET_OUTPUT_MASK       0
	#define PIO_H_OUTPUT_INIT_MASK      0
	#define PIO_H_SET_INPUT_MASK        0
	#define PIO_H_INPUT_PULLUP_MASK     0
	#define PIO_H_INPUT_PULLDOWN_MASK   0

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
// *****************************************
// **		IO Port H
const uint32_t SET_PIO_H_OUTPUT = PIO_H_SET_OUTPUT_MASK;
const uint32_t SET_PIO_H_OUTPUT_INIT = PIO_H_OUTPUT_INIT_MASK;
const uint32_t SET_PIO_H_INPUT = PIO_H_SET_INPUT_MASK;
const uint32_t SET_PIO_H_INPUT_PULLUP = PIO_H_INPUT_PULLUP_MASK;
const uint32_t SET_PIO_H_INPUT_PULLDOWN = PIO_H_INPUT_PULLDOWN_MASK;


/*lint +e1924
   lint +e1960*/
