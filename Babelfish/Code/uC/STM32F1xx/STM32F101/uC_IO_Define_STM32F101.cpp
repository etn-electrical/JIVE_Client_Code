/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 *all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 *signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 *re-enabled at the end of this file
 */
/*lint -e1960  */


#include "Includes.h"
#include "uC_IO_Define_STM32F101.h"
#include "App_IO_Config.h"

/*
 *****************************************************************************************
 *		General User IO
 *****************************************************************************************
 */
#define PIO_NULL_PIN_NUM      IO_PIN_UNDEFINED
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
#define PIOA_PIN_NUM_10        IO_PIN_10
const uC_USER_IO_STRUCT PIOA_PIN_10_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_11        IO_PIN_11
const uC_USER_IO_STRUCT PIOA_PIN_11_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_12        IO_PIN_12
const uC_USER_IO_STRUCT PIOA_PIN_12_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_13        IO_PIN_13
const uC_USER_IO_STRUCT PIOA_PIN_13_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_14        IO_PIN_14
const uC_USER_IO_STRUCT PIOA_PIN_14_STRUCT =
{ GPIOA, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_PIO_INDEX_A ) };
#define PIOA_PIN_NUM_15        IO_PIN_15
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
#define PIOB_PIN_NUM_10        IO_PIN_10
const uC_USER_IO_STRUCT PIOB_PIN_10_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_11        IO_PIN_11
const uC_USER_IO_STRUCT PIOB_PIN_11_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_12        IO_PIN_12
const uC_USER_IO_STRUCT PIOB_PIN_12_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_13        IO_PIN_13
const uC_USER_IO_STRUCT PIOB_PIN_13_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_14        IO_PIN_14
const uC_USER_IO_STRUCT PIOB_PIN_14_STRUCT =
{ GPIOB, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_PIO_INDEX_B ) };
#define PIOB_PIN_NUM_15        IO_PIN_15
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
#define PIOC_PIN_NUM_10        IO_PIN_10
const uC_USER_IO_STRUCT PIOC_PIN_10_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_11        IO_PIN_11
const uC_USER_IO_STRUCT PIOC_PIN_11_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_12        IO_PIN_12
const uC_USER_IO_STRUCT PIOC_PIN_12_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_13        IO_PIN_13
const uC_USER_IO_STRUCT PIOC_PIN_13_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_14        IO_PIN_14
const uC_USER_IO_STRUCT PIOC_PIN_14_STRUCT =
{ GPIOC, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_PIO_INDEX_C ) };
#define PIOC_PIN_NUM_15        IO_PIN_15
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
#define PIOD_PIN_NUM_10        IO_PIN_10
const uC_USER_IO_STRUCT PIOD_PIN_10_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_11        IO_PIN_11
const uC_USER_IO_STRUCT PIOD_PIN_11_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_12        IO_PIN_12
const uC_USER_IO_STRUCT PIOD_PIN_12_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_13        IO_PIN_13
const uC_USER_IO_STRUCT PIOD_PIN_13_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_14        IO_PIN_14
const uC_USER_IO_STRUCT PIOD_PIN_14_STRUCT =
{ GPIOD, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_PIO_INDEX_D ) };
#define PIOD_PIN_NUM_15        IO_PIN_15
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
#define PIOE_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOE_PIN_5_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOE_PIN_6_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOE_PIN_7_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOE_PIN_8_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_8 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOE_PIN_9_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_9 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_10        IO_PIN_10
const uC_USER_IO_STRUCT PIOE_PIN_10_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_11        IO_PIN_11
const uC_USER_IO_STRUCT PIOE_PIN_11_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_12        IO_PIN_12
const uC_USER_IO_STRUCT PIOE_PIN_12_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_13        IO_PIN_13
const uC_USER_IO_STRUCT PIOE_PIN_13_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_14        IO_PIN_14
const uC_USER_IO_STRUCT PIOE_PIN_14_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };
#define PIOE_PIN_NUM_15        IO_PIN_15
const uC_USER_IO_STRUCT PIOE_PIN_15_STRUCT =
{ GPIOE, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_PIO_INDEX_E ) };



/*
 *****************************************************************************************
 *		Peripherals IO
 *****************************************************************************************
 */
const uC_PERIPH_IO_STRUCT PERIPH_NULL_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_UNDEFINED ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_UNSUPPORTED ) };

/*
 ******************************************
 *		UART Peripheral
 */
const uC_PERIPH_IO_STRUCT USART1_TX_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_9 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 2U };
const uC_PERIPH_IO_STRUCT USART1_RX_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_10 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 2U };
// const uC_PERIPH_IO_STRUCT 	USART1_TX_PIO    = { GPIOB, static_cast<uint8_t>(IO_PIN_6), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 2U };
// const uC_PERIPH_IO_STRUCT 	USART1_RX_PIO    = { GPIOB, static_cast<uint8_t>(IO_PIN_7), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 2U };

const uC_PERIPH_IO_STRUCT USART2_TX_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_2 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 3U };
const uC_PERIPH_IO_STRUCT USART2_RX_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_3 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 3U };
// const uC_PERIPH_IO_STRUCT 	USART2_TX_PIO    = { GPIOD, static_cast<uint8_t>(IO_PIN_5), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 3U };
// const uC_PERIPH_IO_STRUCT 	USART2_RX_PIO    = { GPIOD, static_cast<uint8_t>(IO_PIN_6), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 3U };
// Parrish
const uC_PERIPH_IO_STRUCT USART3_TX_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_10 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 4U };
const uC_PERIPH_IO_STRUCT USART3_RX_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_11 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 4U };
// const uC_PERIPH_IO_STRUCT 	USART3_TX_PIO    = { GPIOC, static_cast<uint8_t>(IO_PIN_10), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_2 ), 4U };
// const uC_PERIPH_IO_STRUCT 	USART3_RX_PIO    = { GPIOC, static_cast<uint8_t>(IO_PIN_11), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_2 ), 4U };
// Parrish
// const uC_PERIPH_IO_STRUCT 	USART3_TX_PIO    = { GPIOD, static_cast<uint8_t>(IO_PIN_8), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 4U };
// const uC_PERIPH_IO_STRUCT 	USART3_RX_PIO    = { GPIOD, static_cast<uint8_t>(IO_PIN_9), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 4U };

const uC_PERIPH_IO_STRUCT USART4_TX_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_10 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ), 0U };
const uC_PERIPH_IO_STRUCT USART4_RX_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_11 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ), 0U };

const uC_PERIPH_IO_STRUCT USART5_TX_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ), 0U };
const uC_PERIPH_IO_STRUCT USART5_RX_PIO =
{ GPIOD, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ), 0U };

/*
 ******************************************
 *		SPI Peripheral
 */
const uC_PERIPH_IO_STRUCT SPI1_MOSI_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_7 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 0U };
const uC_PERIPH_IO_STRUCT SPI1_MISO_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_6 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 0U };
const uC_PERIPH_IO_STRUCT SPI1_CLOCK_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_5 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 0U };
const uC_PERIPH_IO_STRUCT SPI1_SS_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_4 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 0U };
// const uC_PERIPH_IO_STRUCT 	SPI1_MOSI_PIO    = { GPIOB, static_cast<uint8_t>(IO_PIN_5), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 0U };
// const uC_PERIPH_IO_STRUCT 	SPI1_MISO_PIO    = { GPIOB, static_cast<uint8_t>(IO_PIN_4), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 0U };
// const uC_PERIPH_IO_STRUCT 	SPI1_CLOCK_PIO    = { GPIOB, static_cast<uint8_t>(IO_PIN_3), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 0U };
// const uC_PERIPH_IO_STRUCT 	SPI1_SS_PIO    = { GPIOA, static_cast<uint8_t>(IO_PIN_15), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 0U };

const uC_PERIPH_IO_STRUCT SPI2_MOSI_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ), 0U };
const uC_PERIPH_IO_STRUCT SPI2_MISO_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_14 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ), 0U };
const uC_PERIPH_IO_STRUCT SPI2_CLOCK_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_13 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ), 0U };
const uC_PERIPH_IO_STRUCT SPI2_SS_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_12 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ), 0U };

// const uC_PERIPH_IO_STRUCT 	SPI3_MOSI_PIO    = { GPIOB, static_cast<uint8_t>(IO_PIN_5), ( uC_IO_ALTERNATE_MAP_0 |
// uC_IO_REMAP_MASK_1 ), 28U };
// const uC_PERIPH_IO_STRUCT 	SPI3_MISO_PIO    = { GPIOB, static_cast<uint8_t>(IO_PIN_4), ( uC_IO_ALTERNATE_MAP_0 |
// uC_IO_REMAP_MASK_1 ), 28U };
// const uC_PERIPH_IO_STRUCT 	SPI3_CLOCK_PIO    = { GPIOB, static_cast<uint8_t>(IO_PIN_3), ( uC_IO_ALTERNATE_MAP_0 |
// uC_IO_REMAP_MASK_1 ), 28U };
// const uC_PERIPH_IO_STRUCT 	SPI3_SS_PIO    = { GPIOA, static_cast<uint8_t>(IO_PIN_15), ( uC_IO_ALTERNATE_MAP_0 |
// uC_IO_REMAP_MASK_1 ), 28U };
const uC_PERIPH_IO_STRUCT SPI3_MOSI_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_12 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 28U };
const uC_PERIPH_IO_STRUCT SPI3_MISO_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_11 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 28U };
const uC_PERIPH_IO_STRUCT SPI3_CLOCK_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_10 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 28U };
const uC_PERIPH_IO_STRUCT SPI3_SS_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_4 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_1 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 28U };


/*
 ******************************************
 *		Timer Peripheral
 */
const uC_PERIPH_IO_STRUCT TIMER1_PIO1 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_8 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 6U };
const uC_PERIPH_IO_STRUCT TIMER1_PIO2 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_9 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 6U };
const uC_PERIPH_IO_STRUCT TIMER1_PIO3 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_10 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 6U };
const uC_PERIPH_IO_STRUCT TIMER1_PIO4 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_11 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 6U };
// const uC_PERIPH_IO_STRUCT 	TIMER1_PIO1    = { GPIOE, static_cast<uint8_t>(IO_PIN_9), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 6U };
// const uC_PERIPH_IO_STRUCT 	TIMER1_PIO2    = { GPIOE, static_cast<uint8_t>(IO_PIN_11), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 6U };
// const uC_PERIPH_IO_STRUCT 	TIMER1_PIO3    = { GPIOE, static_cast<uint8_t>(IO_PIN_13), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 6U };
// const uC_PERIPH_IO_STRUCT 	TIMER1_PIO4    = { GPIOE, static_cast<uint8_t>(IO_PIN_14), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 6U };

const uC_PERIPH_IO_STRUCT TIMER2_PIO1 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_0 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 8U };
const uC_PERIPH_IO_STRUCT TIMER2_PIO2 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_1 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 8U };
const uC_PERIPH_IO_STRUCT TIMER2_PIO3 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_2 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 8U };
const uC_PERIPH_IO_STRUCT TIMER2_PIO4 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_3 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 8U };
// const uC_PERIPH_IO_STRUCT 	TIMER2_PIO1    = { GPIOA, static_cast<uint8_t>(IO_PIN_15), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_2 ), 8U };
// const uC_PERIPH_IO_STRUCT 	TIMER2_PIO2    = { GPIOB, static_cast<uint8_t>(IO_PIN_3), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_2 ), 8U };
// const uC_PERIPH_IO_STRUCT 	TIMER2_PIO3    = { GPIOA, static_cast<uint8_t>(IO_PIN_2), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_2 ), 8U };
// const uC_PERIPH_IO_STRUCT 	TIMER2_PIO4    = { GPIOA, static_cast<uint8_t>(IO_PIN_3), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_2 ), 8U };
// const uC_PERIPH_IO_STRUCT 	TIMER2_PIO1    = { GPIOA, static_cast<uint8_t>(IO_PIN_15), ( uC_IO_ALTERNATE_MAP_2 |
// uC_IO_REMAP_MASK_2 ), 8U };
// const uC_PERIPH_IO_STRUCT 	TIMER2_PIO2    = { GPIOB, static_cast<uint8_t>(IO_PIN_3), ( uC_IO_ALTERNATE_MAP_2 |
// uC_IO_REMAP_MASK_2 ), 8U };
// const uC_PERIPH_IO_STRUCT 	TIMER2_PIO3    = { GPIOB, static_cast<uint8_t>(IO_PIN_10), ( uC_IO_ALTERNATE_MAP_2 |
// uC_IO_REMAP_MASK_2 ), 8U };
// const uC_PERIPH_IO_STRUCT 	TIMER2_PIO4    = { GPIOB, static_cast<uint8_t>(IO_PIN_11), ( uC_IO_ALTERNATE_MAP_2 |
// uC_IO_REMAP_MASK_2 ), 8U };

const uC_PERIPH_IO_STRUCT TIMER3_PIO1 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_6 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 10U };
const uC_PERIPH_IO_STRUCT TIMER3_PIO2 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_7 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 10U };
const uC_PERIPH_IO_STRUCT TIMER3_PIO3 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_0 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 10U };
const uC_PERIPH_IO_STRUCT TIMER3_PIO4 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_1 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_2 ) ), 10U };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO3    = { GPIOB, static_cast<uint8_t>(IO_PIN_0), ( uC_IO_ALTERNATE_MAP_0 |
// uC_IO_REMAP_MASK_2 ), 10U };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO4    = { GPIOB, static_cast<uint8_t>(IO_PIN_1), ( uC_IO_ALTERNATE_MAP_0 |
// uC_IO_REMAP_MASK_2 ), 10U };


// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO1    = { GPIOB, static_cast<uint8_t>(IO_PIN_4), ( uC_IO_ALTERNATE_MAP_2 |
// uC_IO_REMAP_MASK_2 ), 10U };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO2    = { GPIOB, static_cast<uint8_t>(IO_PIN_5), ( uC_IO_ALTERNATE_MAP_2 |
// uC_IO_REMAP_MASK_2 ), 10U };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO3    = { GPIOB, static_cast<uint8_t>(IO_PIN_0), ( uC_IO_ALTERNATE_MAP_2 |
// uC_IO_REMAP_MASK_2 ), 10U };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO4    = { GPIOB, static_cast<uint8_t>(IO_PIN_1), ( uC_IO_ALTERNATE_MAP_2 |
// uC_IO_REMAP_MASK_2 ), 10U };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO1    = { GPIOC, static_cast<uint8_t>(IO_PIN_6), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 10U };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO2    = { GPIOC, static_cast<uint8_t>(IO_PIN_7), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 10U };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO3    = { GPIOC, static_cast<uint8_t>(IO_PIN_8), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 10U };
// const uC_PERIPH_IO_STRUCT 	TIMER3_PIO4    = { GPIOC, static_cast<uint8_t>(IO_PIN_9), ( uC_IO_ALTERNATE_MAP_3 |
// uC_IO_REMAP_MASK_2 ), 10U };

const uC_PERIPH_IO_STRUCT TIMER4_PIO1 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_6 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 12U };
const uC_PERIPH_IO_STRUCT TIMER4_PIO2 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_7 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 12U };
const uC_PERIPH_IO_STRUCT TIMER4_PIO3 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_8 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 12U };
const uC_PERIPH_IO_STRUCT TIMER4_PIO4 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_9 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 12U };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO1    = { GPIOD, static_cast<uint8_t>(IO_PIN_12), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 12U };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO2    = { GPIOD, static_cast<uint8_t>(IO_PIN_13), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 12U };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO3    = { GPIOD, static_cast<uint8_t>(IO_PIN_14), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 12U };
// const uC_PERIPH_IO_STRUCT 	TIMER4_PIO4    = { GPIOD, static_cast<uint8_t>(IO_PIN_15), ( uC_IO_ALTERNATE_MAP_1 |
// uC_IO_REMAP_MASK_1 ), 12U };

const uC_PERIPH_IO_STRUCT TIMER5_PIO1 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_UNSUPPORTED ), 16U };
const uC_PERIPH_IO_STRUCT TIMER5_PIO2 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_UNSUPPORTED ), 16U };
const uC_PERIPH_IO_STRUCT TIMER5_PIO3 =
{ GPIOB, static_cast<uint8_t>( IO_PIN_15 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_UNSUPPORTED ), 16U };
const uC_PERIPH_IO_STRUCT TIMER5_PIO4 =
{ GPIOA, static_cast<uint8_t>( IO_PIN_3 ),
  ( static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) | static_cast<uint8_t>( uC_IO_REMAP_MASK_1 ) ), 16U };
// const uC_PERIPH_IO_STRUCT 	TIMER5_PIO4    = { GPIOB, static_cast<uint8_t>(IO_PIN_UNDEFINED), (
// uC_IO_ALTERNATE_MAP_1 | uC_IO_REMAP_MASK_1 ), 16U };	//Likely never used for us.


/*
 ******************************************
 *		ADC Peripheral
 */
const uC_PERIPH_IO_STRUCT ADC12_IN0_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN1_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN2_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN3_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN4_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN5_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN6_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_6 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN7_PIO =
{ GPIOA, static_cast<uint8_t>( IO_PIN_7 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN8_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN9_PIO =
{ GPIOB, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN10_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_0 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN11_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_1 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN12_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_2 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN13_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_3 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN14_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_4 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN15_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_5 ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN16_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_UNDEFINED ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };
const uC_PERIPH_IO_STRUCT ADC12_IN17_PIO =
{ GPIOC, static_cast<uint8_t>( IO_PIN_UNDEFINED ), static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ), 0U };



/*
 *****************************************************************************************
 *		Direction Selections
 *****************************************************************************************
 */
#ifndef PIO_A_SET_OUTPUT_MASK
	#define PIO_A_SET_OUTPUT_MASK       0U
	#define PIO_A_OUTPUT_INIT_MASK      0U
	#define PIO_A_SET_INPUT_MASK        0U
	#define PIO_A_INPUT_PULLUP_MASK     0U
	#define PIO_A_INPUT_PULLDOWN_MASK   0U

	#define PIO_B_SET_OUTPUT_MASK       0U
	#define PIO_B_OUTPUT_INIT_MASK      0U
	#define PIO_B_SET_INPUT_MASK        0U
	#define PIO_B_INPUT_PULLUP_MASK     0U
	#define PIO_B_INPUT_PULLDOWN_MASK   0U

	#define PIO_C_SET_OUTPUT_MASK       0U
	#define PIO_C_OUTPUT_INIT_MASK      0U
	#define PIO_C_SET_INPUT_MASK        0U
	#define PIO_C_INPUT_PULLUP_MASK     0U
	#define PIO_C_INPUT_PULLDOWN_MASK   0U

	#define PIO_D_SET_OUTPUT_MASK       0U
	#define PIO_D_OUTPUT_INIT_MASK      0U
	#define PIO_D_SET_INPUT_MASK        0U
	#define PIO_D_INPUT_PULLUP_MASK     0U
	#define PIO_D_INPUT_PULLDOWN_MASK   0U

	#define PIO_E_SET_OUTPUT_MASK       0U
	#define PIO_E_OUTPUT_INIT_MASK      0U
	#define PIO_E_SET_INPUT_MASK        0U
	#define PIO_E_INPUT_PULLUP_MASK     0U
	#define PIO_E_INPUT_PULLDOWN_MASK   0U
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

/* lint +e1924
  lint +e1960*/