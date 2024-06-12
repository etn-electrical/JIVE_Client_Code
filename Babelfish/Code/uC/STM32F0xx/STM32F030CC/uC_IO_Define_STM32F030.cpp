/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_IO_Define_STM32F030.h"
#include "App_IO_Config.h"

/*
 *****************************************************************************************
 *		General User IO
 *****************************************************************************************
 */
#define PIO_NULL_PIN_NUM      IO_PIN_UNDEFINED
const uC_USER_IO_STRUCT PIO_NULL_IO_STRUCT =
{ GPIOA, IO_PIN_UNDEFINED, uC_PIO_INDEX_A };

#define PIOA_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOA_PIN_0_STRUCT =
{ GPIOA, IO_PIN_0, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOA_PIN_1_STRUCT =
{ GPIOA, IO_PIN_1, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOA_PIN_2_STRUCT =
{ GPIOA, IO_PIN_2, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOA_PIN_3_STRUCT =
{ GPIOA, IO_PIN_3, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOA_PIN_4_STRUCT =
{ GPIOA, IO_PIN_4, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOA_PIN_5_STRUCT =
{ GPIOA, IO_PIN_5, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOA_PIN_6_STRUCT =
{ GPIOA, IO_PIN_6, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOA_PIN_7_STRUCT =
{ GPIOA, IO_PIN_7, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOA_PIN_8_STRUCT =
{ GPIOA, IO_PIN_8, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOA_PIN_9_STRUCT =
{ GPIOA, IO_PIN_9, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_10        IO_PIN_10
const uC_USER_IO_STRUCT PIOA_PIN_10_STRUCT =
{ GPIOA, IO_PIN_10, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_11        IO_PIN_11
const uC_USER_IO_STRUCT PIOA_PIN_11_STRUCT =
{ GPIOA, IO_PIN_11, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_12        IO_PIN_12
const uC_USER_IO_STRUCT PIOA_PIN_12_STRUCT =
{ GPIOA, IO_PIN_12, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_13        IO_PIN_13
const uC_USER_IO_STRUCT PIOA_PIN_13_STRUCT =
{ GPIOA, IO_PIN_13, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_14        IO_PIN_14
const uC_USER_IO_STRUCT PIOA_PIN_14_STRUCT =
{ GPIOA, IO_PIN_14, uC_PIO_INDEX_A };
#define PIOA_PIN_NUM_15        IO_PIN_15
const uC_USER_IO_STRUCT PIOA_PIN_15_STRUCT =
{ GPIOA, IO_PIN_15, uC_PIO_INDEX_A };

#define PIOB_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOB_PIN_0_STRUCT =
{ GPIOB, IO_PIN_0, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOB_PIN_1_STRUCT =
{ GPIOB, IO_PIN_1, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOB_PIN_2_STRUCT =
{ GPIOB, IO_PIN_2, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOB_PIN_3_STRUCT =
{ GPIOB, IO_PIN_3, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOB_PIN_4_STRUCT =
{ GPIOB, IO_PIN_4, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOB_PIN_5_STRUCT =
{ GPIOB, IO_PIN_5, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOB_PIN_6_STRUCT =
{ GPIOB, IO_PIN_6, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOB_PIN_7_STRUCT =
{ GPIOB, IO_PIN_7, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOB_PIN_8_STRUCT =
{ GPIOB, IO_PIN_8, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOB_PIN_9_STRUCT =
{ GPIOB, IO_PIN_9, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_10        IO_PIN_10
const uC_USER_IO_STRUCT PIOB_PIN_10_STRUCT =
{ GPIOB, IO_PIN_10, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_11        IO_PIN_11
const uC_USER_IO_STRUCT PIOB_PIN_11_STRUCT =
{ GPIOB, IO_PIN_11, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_12        IO_PIN_12
const uC_USER_IO_STRUCT PIOB_PIN_12_STRUCT =
{ GPIOB, IO_PIN_12, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_13        IO_PIN_13
const uC_USER_IO_STRUCT PIOB_PIN_13_STRUCT =
{ GPIOB, IO_PIN_13, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_14        IO_PIN_14
const uC_USER_IO_STRUCT PIOB_PIN_14_STRUCT =
{ GPIOB, IO_PIN_14, uC_PIO_INDEX_B };
#define PIOB_PIN_NUM_15        IO_PIN_15
const uC_USER_IO_STRUCT PIOB_PIN_15_STRUCT =
{ GPIOB, IO_PIN_15, uC_PIO_INDEX_B };

#define PIOC_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOC_PIN_0_STRUCT =
{ GPIOC, IO_PIN_0, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOC_PIN_1_STRUCT =
{ GPIOC, IO_PIN_1, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOC_PIN_2_STRUCT =
{ GPIOC, IO_PIN_2, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOC_PIN_3_STRUCT =
{ GPIOC, IO_PIN_3, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOC_PIN_4_STRUCT =
{ GPIOC, IO_PIN_4, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOC_PIN_5_STRUCT =
{ GPIOC, IO_PIN_5, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOC_PIN_6_STRUCT =
{ GPIOC, IO_PIN_6, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOC_PIN_7_STRUCT =
{ GPIOC, IO_PIN_7, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOC_PIN_8_STRUCT =
{ GPIOC, IO_PIN_8, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOC_PIN_9_STRUCT =
{ GPIOC, IO_PIN_9, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_10        IO_PIN_10
const uC_USER_IO_STRUCT PIOC_PIN_10_STRUCT =
{ GPIOC, IO_PIN_10, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_11        IO_PIN_11
const uC_USER_IO_STRUCT PIOC_PIN_11_STRUCT =
{ GPIOC, IO_PIN_11, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_12        IO_PIN_12
const uC_USER_IO_STRUCT PIOC_PIN_12_STRUCT =
{ GPIOC, IO_PIN_12, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_13        IO_PIN_13
const uC_USER_IO_STRUCT PIOC_PIN_13_STRUCT =
{ GPIOC, IO_PIN_13, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_14        IO_PIN_14
const uC_USER_IO_STRUCT PIOC_PIN_14_STRUCT =
{ GPIOC, IO_PIN_14, uC_PIO_INDEX_C };
#define PIOC_PIN_NUM_15        IO_PIN_15
const uC_USER_IO_STRUCT PIOC_PIN_15_STRUCT =
{ GPIOC, IO_PIN_15, uC_PIO_INDEX_C };

#define PIOD_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOD_PIN_0_STRUCT =
{ GPIOD, IO_PIN_0, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOD_PIN_1_STRUCT =
{ GPIOD, IO_PIN_1, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_2        IO_PIN_2
const uC_USER_IO_STRUCT PIOD_PIN_2_STRUCT =
{ GPIOD, IO_PIN_2, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_3        IO_PIN_3
const uC_USER_IO_STRUCT PIOD_PIN_3_STRUCT =
{ GPIOD, IO_PIN_3, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOD_PIN_4_STRUCT =
{ GPIOD, IO_PIN_4, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOD_PIN_5_STRUCT =
{ GPIOD, IO_PIN_5, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOD_PIN_6_STRUCT =
{ GPIOD, IO_PIN_6, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOD_PIN_7_STRUCT =
{ GPIOD, IO_PIN_7, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_8        IO_PIN_8
const uC_USER_IO_STRUCT PIOD_PIN_8_STRUCT =
{ GPIOD, IO_PIN_8, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_9        IO_PIN_9
const uC_USER_IO_STRUCT PIOD_PIN_9_STRUCT =
{ GPIOD, IO_PIN_9, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_10        IO_PIN_10
const uC_USER_IO_STRUCT PIOD_PIN_10_STRUCT =
{ GPIOD, IO_PIN_10, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_11        IO_PIN_11
const uC_USER_IO_STRUCT PIOD_PIN_11_STRUCT =
{ GPIOD, IO_PIN_11, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_12        IO_PIN_12
const uC_USER_IO_STRUCT PIOD_PIN_12_STRUCT =
{ GPIOD, IO_PIN_12, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_13        IO_PIN_13
const uC_USER_IO_STRUCT PIOD_PIN_13_STRUCT =
{ GPIOD, IO_PIN_13, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_14        IO_PIN_14
const uC_USER_IO_STRUCT PIOD_PIN_14_STRUCT =
{ GPIOD, IO_PIN_14, uC_PIO_INDEX_D };
#define PIOD_PIN_NUM_15        IO_PIN_15
const uC_USER_IO_STRUCT PIOD_PIN_15_STRUCT =
{ GPIOD, IO_PIN_15, uC_PIO_INDEX_D };

#define PIOF_PIN_NUM_0        IO_PIN_0
const uC_USER_IO_STRUCT PIOF_PIN_0_STRUCT =
{ GPIOF, IO_PIN_0, uC_PIO_INDEX_F };
#define PIOF_PIN_NUM_1        IO_PIN_1
const uC_USER_IO_STRUCT PIOF_PIN_1_STRUCT =
{ GPIOF, IO_PIN_1, uC_PIO_INDEX_F };
#define PIOF_PIN_NUM_4        IO_PIN_4
const uC_USER_IO_STRUCT PIOF_PIN_4_STRUCT =
{ GPIOF, IO_PIN_4, uC_PIO_INDEX_F };
#define PIOF_PIN_NUM_5        IO_PIN_5
const uC_USER_IO_STRUCT PIOF_PIN_5_STRUCT =
{ GPIOF, IO_PIN_5, uC_PIO_INDEX_F };
#define PIOF_PIN_NUM_6        IO_PIN_6
const uC_USER_IO_STRUCT PIOF_PIN_6_STRUCT =
{ GPIOF, IO_PIN_6, uC_PIO_INDEX_F };
#define PIOF_PIN_NUM_7        IO_PIN_7
const uC_USER_IO_STRUCT PIOF_PIN_7_STRUCT =
{ GPIOF, IO_PIN_7, uC_PIO_INDEX_F };

/*
 *****************************************************************************************
 *		Peripherals IO
 *****************************************************************************************
 */

/*
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   Pin name | AF0              |  AF1              |  AF2        |  AF3        |  AF4              |
        AF5          | AF6      |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA0      | -                |  USART1_CTS(2)    |  -          |  -          |  USART4_TX(1)     |
        -            | -        |
 |                  |-------------------|             |             |                   |
 |               |          |
 |                  |  USART2_CTS(1)(3) |             |             |                   |
 |               |          |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA1      | EVENTOUT         |  USART1_RTS(2)    |  -          |  -          |  USART2_RTS(1)(3) |
 |  TIM15_CH1N(1)| -        |
 |                  |-------------------|             |             |                   |
 |               |          |
 |                  |  USART4_RX(1)     |             |             |                   |
 |               |          |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA2      | TIM15_CH1(1)(3)  |  USART1_TX(2)     |  -          |  -          |  -                |
 |  -            | -        |
 |                  |-------------------|             |             |                   |
 |               |          |
 |                  |  USART2_TX(1)(3)  |             |             |                   |
 |               |          |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA3      | TIM15_CH2(1)(3)  |  USART1_RX(2)     |  -          |  -          |  -                |
 |  -            | -        |
 |                  |-------------------|             |             |                   |
 |               |          |
 |                  |  USART2_RX(1)(3)  |             |             |                   |
 |               |          |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA4      | SPI1_NSS         |  USART1_CK(2)     |  -          |  -          |  TIM14_CH1        |
 |  USART6_TX(1) | -        |
 |                  |-------------------|             |             |                   |
 |               |          |
 |                  |  USART2_CK(1)(3)  |             |             |                   |
 |               |          |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA5      | SPI1_SCK         |  -                |  -          |  -          |  -                |
 |  USART6_RX(1) | -        |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA6      | SPI1_MISO        |  TIM3_CH1         |  TIM1_BKIN  |  -          |  USART3_CTS(1)    |
 |  TIM16_CH1    | EVENTOUT |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA7      | SPI1_MOSI        |  TIM3_CH2         |  TIM1_CH1N  |  -          |  TIM14_CH1        |
 |  TIM17_CH1    | EVENTOUT |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA8      | MCO              |  USART1_CK        |  TIM1_CH1   |  EVENTOUT   |  -                |
 |  -            | -        |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA9      | TIM15_BKIN(1)(3) |  USART1_TX        |  TIM1_CH2   |  -          |  I2C1_SCL(1)(2)   |
 |  MCO(1)       | -        |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA10     | TIM17_BKIN       |  USART1_RX        |  TIM1_CH3   |  -          |  I2C1_SDA(1)(2)   |
 |  -            | -        |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA11     | EVENTOUT         |  USART1_CTS       |  TIM1_CH4   |  -          |  -                |
 |  SCL          | -        |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA12     | EVENTOUT         |  USART1_RTS       |  TIM1_ETR   |  -          |  -                |
 |  SDA          | -        |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA13     | SWDIO            |  IR_OUT           |  -          |  -          |  -                |
 |  -            | -        |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA14     | SWCLK            |  USART1_TX(2)     |  -          |  -          |  -                |
 |  -            | -        |
 |                  |-------------------|             |             |                   |
 |               |          |
 |                  |  USART2_TX(1)(3)  |             |             |                   |
 |               |          |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|
   PA15     | SPI1_NSS         |  USART1_RX(2)     |  -          |  EVENTOUT   |  USART4_RTS(1)    |
 |  -            | -        |
 |                  |-------------------|             |             |                   |
 |               |          |
 |                  |  USART2_RX(1)(3)  |             |             |                   |
 |               |          |
   ---------|------------------|-------------------|-------------|-------------|-------------------|----------------|----------|

   (1). This feature is available on STM32F030xC devices.
   (2). This feature is available on STM32F030x4 and STM32F030x6 devices.
   (3). This feature is available on STM32F030x8 devices.

   ---------|-----------------|----------------|-------------|-----------------|-----------------|----------------|
   Pin name |   AF0           |	AF1	    |      AF2	  |   AF3	    |      AF4	      |   AF5
 |        |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB0      |  EVENTOUT       | TIM3_CH3       |  TIM1_CH2N  | -               | USART3_CK(1)    | -
 |            |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB1      |  TIM14_CH1      | TIM3_CH4       |  TIM1_CH3N  | -               | USART3_RTS(1)   | -
 |            |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB2      |  -              | -              |  -          | -               | -               | -
 |            |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB3      |  SPI1_SCK       | EVENTOUT       |  -          | -               | USART5_TX(1)    | -
 |            |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB4      |  SPI1_MISO      | TIM3_CH1       |  EVENTOUT   | -               | USART5_RX(1)    |
 |TIM17_BKIN(1) |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB5      |  SPI1_MOSI      | TIM3_CH2       |  TIM16_BKIN | I2C1_SMBA       | USART5_CK_RTS(1)| -
 |            |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB6      |  USART1_TX      | I2C1_SCL       |  TIM16_CH1N | -               | -               | -
 |            |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB7      |  USART1_RX      | I2C1_SDA       |  TIM17_CH1N | -               | USART4_CTS(1)   | -
 |            |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB8      |  -              | I2C1_SCL       |  TIM16_CH1  | -               | -               | -
 |            |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB9      |  IR_OUT         | I2C1_SDA       |  TIM17_CH1  | EVENTOUT        | -               |
 |SPI2_NSS(1)   |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB10     |  -              | I2C1_SCL(2)    |  -          | -               | USART3_TX(1)    |
 |SPI2_SCK(1)   |
 |                 |----------------|             |                 |                 |
 |              |
 |                 | I2C2_SCL(1)(3) |             |                 | USART3_RX(1)    | -
 |            |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB11     |  EVENTOUT       | I2C1_SDA(2)    |  -          | -               | USART3_RX(1)    | -
 |            |
 |                 |----------------|             |                 |                 |
 |              |
 |                 | I2C2_SDA(1)(3) |             |                 |                 |
 |              |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB12     |  SPI1_NSS(2)    | EVENTOUT       |  TIM1_BKIN  | -               | USART3_RTS(1)   |
 |TIM15(1)      |
 |-----------------|                |             |                 |                 |
 |              |
 |  SPI2_NSS(1)(3) |                |             |                 |                 |
 |              |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB13     |  SPI1_SCK(2)    | -              |  TIM1_CH1N  | -               | USART3_CTS((1)  |
 |I2C2_SCL(1)   |
 |-----------------|                |             |                 |                 |
 |              |
 |  SPI2_SCK(1)(3) |                |             |                 |                 |
 |              |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB14     |  SPI1_MISO(2)   | TIM15_CH1(1)(3)|  TIM1_CH2N  | -               | USART3_RTS(1)   |
 |I2C2_SDA(1)   |
 |-----------------|                |             |                 |                 |
 |              |
 |  SPI2_MISO(1)(3)|                |             |                 |                 |
 |              |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|
   PB15     |  SPI1_MOSI(2)   | TIM15_CH2(1)(3)|  TIM1_CH3N  | TIM15_CH1N(1)(3)| -               | -
 |            |
 |-----------------|                |             |                 |                 |
 |              |
 |  SPI2_MOSI(1)(3)|                |             |                 |                 |
 |              |
   ---------|-----------------|----------------|-------------|-----------------|-----------------|---------------|

   (1). This feature is available on STM32F030xC devices.
   (2). This feature is available on STM32F030x4 and STM32F030x6 devices.
   (3). This feature is available on STM32F030x8 devices.



   ---------|------------|------------|-------------|
   Pin name |   AF0      |  AF1(1)	   |   AF2(1)	 |
   ---------|------------|------------|-------------|
   PC0      |EVENTOUT    |  -         | USART6_TX   |
   ---------|------------|------------|-------------|
   PC1      |EVENTOUT    |  -         | USART6_RX   |
   ---------|------------|------------|-------------|
   PC2      |EVENTOUT    |  SPI2_MISO | -           |
   ---------|------------|------------|-------------|
   PC3      |EVENTOUT    |  SPI2_MOSI | -           |
   ---------|------------|------------|-------------|
   PC4      |EVENTOUT    |  USART3_TX | -           |
   ---------|------------|------------|-------------|
   PC5      |-           |  USART3_RX | -           |
   ---------|------------|------------|-------------|
   PC6      |TIM3_CH1    |  -         | -           |
   ---------|------------|------------|-------------|
   PC7      |TIM3_CH2    |  -         | -           |
   ---------|------------|------------|-------------|
   PC8      |TIM3_CH3    |  -         | -           |
   ---------|------------|------------|-------------|
   PC9      |TIM3_CH4    |  -         | -           |
   ---------|------------|------------|-------------|
   PC10     |USART4_TX(1)|  USART3_TX | -           |
   ---------|------------|------------|-------------|
   PC11     |USART4_RX(1)|  USART3_RX | -           |
   ---------|------------|------------|-------------|
   PC12     |USART4_CK(1)|  USART3_CK | USART5_TX   |
   ---------|------------|------------|-------------|
   PC13     |-           |  -         | -           |
   ---------|------------|------------|-------------|
   PC14     |-           |  -         | -           |
   ---------|------------|------------|-------------|
   PC15     |-           |  -         | -           |
   ---------|------------|------------|-------------|
   (1). Available on STM32F030xC devices only.


   ---------|------------|------------|-------------|
   Pin name |   AF0      |  AF1(1)	   |   AF2(1)	 |
   ---------|------------|------------|-------------|
   PD2	 |TIM3_ETR    |	USART3_RTS | USART5_RX   |
   ---------|------------|------------|-------------|
   (1). Available on STM32F030xC devices only.

   ---------|------------|------------|
   Pin name |   AF0      |  AF1(1)	   |
   ---------|------------|------------|
   PF0	 |  -	      |I2C1_SDA    |
   ---------|------------|------------|
   PF1	 |  -	      |I2C1_SCL    |
   ---------|------------|------------|

   (1). Available on STM32F030xC devices only.

 */

// const uC_PERIPH_IO_STRUCT     PERIPH_NULL_PIO     = { GPIOA, IO_PIN_UNDEFINED,
// uC_IO_ALTERNATE_MAP_UNSUPPORTED };
/*
 ******************************************
 *		UART Peripheral
 */
const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PA9 =
{ GPIOA, IO_PIN_9, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PA10 =
{ GPIOA, IO_PIN_10, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT USART1_TX_PIO_PB6 =
{ GPIOB, IO_PIN_6, uC_IO_ALTERNATE_FUNC_0 };
const uC_PERIPH_IO_STRUCT USART1_RX_PIO_PB7 =
{ GPIOB, IO_PIN_7, uC_IO_ALTERNATE_FUNC_0 };

const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PA2 =
{ GPIOA, IO_PIN_2, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PA3 =
{ GPIOA, IO_PIN_3, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT USART2_TX_PIO_PA14 =
{ GPIOA, IO_PIN_14, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT USART2_RX_PIO_PA15 =
{ GPIOA, IO_PIN_15, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PB10 =
{ GPIOB, IO_PIN_10, uC_IO_ALTERNATE_FUNC_4 };
const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PB11 =
{ GPIOB, IO_PIN_11, uC_IO_ALTERNATE_FUNC_4 };

const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PC4 =
{ GPIOC, IO_PIN_4, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PC5 =
{ GPIOC, IO_PIN_5, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT USART3_TX_PIO_PC10 =
{ GPIOC, IO_PIN_10, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT USART3_RX_PIO_PC11 =
{ GPIOC, IO_PIN_11, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT USART4_TX_PIO_PA0 =
{ GPIOA, IO_PIN_0, uC_IO_ALTERNATE_FUNC_4 };
const uC_PERIPH_IO_STRUCT USART4_RX_PIO_PA1 =
{ GPIOA, IO_PIN_1, uC_IO_ALTERNATE_FUNC_4 };

const uC_PERIPH_IO_STRUCT USART4_TX_PIO_PC10 =
{ GPIOC, IO_PIN_10, uC_IO_ALTERNATE_FUNC_0 };
const uC_PERIPH_IO_STRUCT USART4_RX_PIO_PC11 =
{ GPIOC, IO_PIN_11, uC_IO_ALTERNATE_FUNC_0 };

const uC_PERIPH_IO_STRUCT USART5_TX_PIO_PB3 =
{ GPIOB, IO_PIN_3, uC_IO_ALTERNATE_FUNC_4 };
const uC_PERIPH_IO_STRUCT USART5_RX_PIO_PB4 =
{ GPIOB, IO_PIN_4, uC_IO_ALTERNATE_FUNC_4 };

const uC_PERIPH_IO_STRUCT USART5_TX_PIO_PC12 =
{ GPIOC, IO_PIN_12, uC_IO_ALTERNATE_FUNC_2 };
const uC_PERIPH_IO_STRUCT USART5_RX_PIO_PD2 =
{ GPIOD, IO_PIN_2, uC_IO_ALTERNATE_FUNC_2 };

const uC_PERIPH_IO_STRUCT USART6_TX_PIO_PC0 =
{ GPIOC, IO_PIN_0, uC_IO_ALTERNATE_FUNC_2 };
const uC_PERIPH_IO_STRUCT USART6_RX_PIO_PC1 =
{ GPIOC, IO_PIN_1, uC_IO_ALTERNATE_FUNC_2 };

const uC_PERIPH_IO_STRUCT USART6_TX_PIO_PA4 =
{ GPIOA, IO_PIN_4, uC_IO_ALTERNATE_FUNC_5 };
const uC_PERIPH_IO_STRUCT USART6_RX_PIO_PA5 =
{ GPIOA, IO_PIN_5, uC_IO_ALTERNATE_FUNC_5 };

/*
 ******************************************
 *		I2C Peripheral
 */

const uC_PERIPH_IO_STRUCT I2C1_SCL_PIO_PB6 =
{ GPIOB, IO_PIN_6, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT I2C1_SDA_PIO_PB7 =
{ GPIOB, IO_PIN_7, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT I2C1_SCL_PIO_PB8 =
{ GPIOB, IO_PIN_8, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT I2C1_SDA_PIO_PB9 =
{ GPIOB, IO_PIN_9, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT I2C1_SCL_PIO_PF1 =
{ GPIOF, IO_PIN_1, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT I2C1_SDA_PIO_PF0 =
{ GPIOF, IO_PIN_0, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT I2C2_SCL_PIO_PB10 =
{ GPIOB, IO_PIN_10, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT I2C2_SDA_PIO_PB11 =
{ GPIOB, IO_PIN_11, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT I2C2_SCL_PIO_PB13 =
{ GPIOB, IO_PIN_13, uC_IO_ALTERNATE_FUNC_5 };
const uC_PERIPH_IO_STRUCT I2C2_SDA_PIO_PB14 =
{ GPIOB, IO_PIN_14, uC_IO_ALTERNATE_FUNC_5 };

const uC_PERIPH_IO_STRUCT I2C2_SCL_PIO_PA11 =
{ GPIOA, IO_PIN_11, uC_IO_ALTERNATE_FUNC_5 };
const uC_PERIPH_IO_STRUCT I2C2_SDA_PIO_PA12 =
{ GPIOA, IO_PIN_12, uC_IO_ALTERNATE_FUNC_5 };
/*
 ******************************************
 *		Timer Peripheral
 *//* reg_ctrl , pio_num   , alternate_func   */
const uC_PERIPH_IO_STRUCT TIMER1_PIO1_PA8 =
{ GPIOA, IO_PIN_8, uC_IO_ALTERNATE_FUNC_2 };
const uC_PERIPH_IO_STRUCT TIMER1_PIO2_PA9 =
{ GPIOA, IO_PIN_9, uC_IO_ALTERNATE_FUNC_2 };
const uC_PERIPH_IO_STRUCT TIMER1_PIO3_PA10 =
{ GPIOA, IO_PIN_10, uC_IO_ALTERNATE_FUNC_2 };
const uC_PERIPH_IO_STRUCT TIMER1_PIO4_PA11 =
{ GPIOA, IO_PIN_11, uC_IO_ALTERNATE_FUNC_2 };

const uC_PERIPH_IO_STRUCT TIMER3_PIO1_PC6 =
{ GPIOC, IO_PIN_6, uC_IO_ALTERNATE_FUNC_0 };
const uC_PERIPH_IO_STRUCT TIMER3_PIO2_PC7 =
{ GPIOC, IO_PIN_7, uC_IO_ALTERNATE_FUNC_0 };
const uC_PERIPH_IO_STRUCT TIMER3_PIO3_PC8 =
{ GPIOC, IO_PIN_8, uC_IO_ALTERNATE_FUNC_0 };
const uC_PERIPH_IO_STRUCT TIMER3_PIO4_PC9 =
{ GPIOC, IO_PIN_9, uC_IO_ALTERNATE_FUNC_0 };

const uC_PERIPH_IO_STRUCT TIMER3_PIO1_PB4 =
{ GPIOB, IO_PIN_4, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT TIMER3_PIO2_PA7 =
{ GPIOA, IO_PIN_7, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT TIMER3_PIO3_PB0 =
{ GPIOB, IO_PIN_0, uC_IO_ALTERNATE_FUNC_1 };
const uC_PERIPH_IO_STRUCT TIMER3_PIO4_PB1 =
{ GPIOB, IO_PIN_1, uC_IO_ALTERNATE_FUNC_1 };

const uC_PERIPH_IO_STRUCT TIMER14_PIO1_PA4 =
{ GPIOA, IO_PIN_4, uC_IO_ALTERNATE_FUNC_4 };
const uC_PERIPH_IO_STRUCT TIMER14_PIO1_PA7 =
{ GPIOA, IO_PIN_7, uC_IO_ALTERNATE_FUNC_4 };

const uC_PERIPH_IO_STRUCT TIMER15_PIO1_PA2 =
{ GPIOA, IO_PIN_2, uC_IO_ALTERNATE_FUNC_0 };
const uC_PERIPH_IO_STRUCT TIMER15_PIO2_PB15 =
{ GPIOB, IO_PIN_15, uC_IO_ALTERNATE_FUNC_1 };
// const uC_PERIPH_IO_STRUCT TIMER15_PIO2_PA3 =
// { GPIOA, IO_PIN_3, uC_IO_ALTERNATE_FUNC_0 };

const uC_PERIPH_IO_STRUCT TIMER16_PIO1_PA6 =
{ GPIOA, IO_PIN_6, uC_IO_ALTERNATE_FUNC_5 };
const uC_PERIPH_IO_STRUCT TIMER16_PIO1_PB8 =
{ GPIOB, IO_PIN_8, uC_IO_ALTERNATE_FUNC_2 };

const uC_PERIPH_IO_STRUCT TIMER17_PIO1_PB9 =
{ GPIOB, IO_PIN_9, uC_IO_ALTERNATE_FUNC_2 };
const uC_PERIPH_IO_STRUCT TIMER17_PIO1_PA7 =
{ GPIOA, IO_PIN_7, uC_IO_ALTERNATE_FUNC_5 };


/*
 ******************************************
 *		ADC Peripheral
 */
const uC_PERIPH_IO_STRUCT ADC12_IN0_PIO_PA0 =
{ GPIOA, IO_PIN_0, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN1_PIO_PA1 =
{ GPIOA, IO_PIN_1, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN2_PIO_PA2 =
{ GPIOA, IO_PIN_2, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN3_PIO_PA3 =
{ GPIOA, IO_PIN_3, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN4_PIO_PA4 =
{ GPIOA, IO_PIN_4, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN5_PIO_PA5 =
{ GPIOA, IO_PIN_5, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN6_PIO_PA6 =
{ GPIOA, IO_PIN_6, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN7_PIO_PA7 =
{ GPIOA, IO_PIN_7, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN8_PIO_PB0 =
{ GPIOB, IO_PIN_0, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN9_PIO_PB1 =
{ GPIOB, IO_PIN_1, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN10_PIO_PC0 =
{ GPIOC, IO_PIN_0, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN11_PIO_PC1 =
{ GPIOC, IO_PIN_1, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN12_PIO_PC2 =
{ GPIOC, IO_PIN_2, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN13_PIO_PC3 =
{ GPIOC, IO_PIN_3, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN14_PIO_PC4 =
{ GPIOC, IO_PIN_4, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN15_PIO_PC5 =
{ GPIOC, IO_PIN_5, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN16_PIO =
{ GPIOC, IO_PIN_UNDEFINED, uC_IO_ALTERNATE_MAP_ANALOG };
const uC_PERIPH_IO_STRUCT ADC12_IN17_PIO =
{ GPIOC, IO_PIN_UNDEFINED, uC_IO_ALTERNATE_MAP_ANALOG };

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

#define PIO_F_SET_OUTPUT_MASK       0
#define PIO_F_OUTPUT_INIT_MASK      0
#define PIO_F_SET_INPUT_MASK        0
#define PIO_F_INPUT_PULLUP_MASK     0
#define PIO_F_INPUT_PULLDOWN_MASK   0
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
// **		IO Port F
const uint32_t SET_PIO_F_OUTPUT = PIO_F_SET_OUTPUT_MASK;
const uint32_t SET_PIO_F_OUTPUT_INIT = PIO_F_OUTPUT_INIT_MASK;
const uint32_t SET_PIO_F_INPUT = PIO_F_SET_INPUT_MASK;
const uint32_t SET_PIO_F_INPUT_PULLUP = PIO_F_INPUT_PULLUP_MASK;
const uint32_t SET_PIO_F_INPUT_PULLDOWN = PIO_F_INPUT_PULLDOWN_MASK;

