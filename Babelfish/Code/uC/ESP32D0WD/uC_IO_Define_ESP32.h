/**
 *****************************************************************************************
 *	@file		uC_IO_Define_ESP32.h
 *
 *	@brief		Contains the uC_IO_Define_ESP32 Class
 *
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_IO_DEFINE_ESP32_H
   #define uC_IO_DEFINE_ESP32_H

#include <stdint.h>
#include "uC_Base.h"

/**
 * @brief GPIO peripheral structure
 */
typedef struct uC_PERIPH_IO_STRUCT
{
	gpio_mode_t mode;			///< GPIO mode: set input/output mode
	gpio_num_t io_num;				///< This is the reference relative to a certain port
	GPIO_INT_TYPE intr_type;	///< GPIO interrupt type
} uC_PERIPH_IO_STRUCT;

/**
 * @brief GPIO user structure
 */
typedef struct
{
	gpio_mode_t mode;			///< GPIO mode: set input/output mode
	uint32_t io_num;			///< This is the reference relative to a certain port
	GPIO_INT_TYPE intr_type;	///< GPIO interrupt type
} uC_USER_IO_STRUCT;


/**
 ***************************************
 * @brief GPIO bits for Masking
 ***************************************
 */

#define GPIO_IO_0        1ULL << 0
#define GPIO_IO_1        1ULL << 0
#define GPIO_IO_2        1ULL << 0
#define GPIO_IO_3        1ULL << 0
#define GPIO_IO_4        1ULL << 4
#define GPIO_IO_5        1ULL << 5
#define GPIO_IO_6        1ULL << 0
#define GPIO_IO_7        1ULL << 0
#define GPIO_IO_8        1ULL << 0
#define GPIO_IO_9        1ULL << 0
#define GPIO_IO_10       1ULL << 0
#define GPIO_IO_11       1ULL << 0
#define GPIO_IO_12       1ULL << 0
#define GPIO_IO_13       1ULL << 0
#define GPIO_IO_14       1ULL << 0
#define GPIO_IO_15       1ULL << 0
#define GPIO_IO_16       1ULL << 0
#define GPIO_IO_17       1ULL << 0
#define GPIO_IO_18       1ULL << 18
#define GPIO_IO_19       1ULL << 19
#define GPIO_IO_20       1ULL << 0
#define GPIO_IO_21       1ULL << 0
#define GPIO_IO_22       1ULL << 0
#define GPIO_IO_23       1ULL << 0
#define GPIO_IO_24       1ULL << 0
#define GPIO_IO_25       1ULL << 0
#define GPIO_IO_26       1ULL << 0
#define GPIO_IO_27       1ULL << 0
#define GPIO_IO_28       1ULL << 0
#define GPIO_IO_29       1ULL << 0
#define GPIO_IO_30       1ULL << 0
#define GPIO_IO_31       1ULL << 0
#define GPIO_IO_32       1ULL << 0
#define GPIO_IO_33       1ULL << 0
#define GPIO_IO_34       1ULL << 0
#define GPIO_IO_35       1ULL << 0
#define GPIO_IO_36       1ULL << 0
#define GPIO_IO_37       1ULL << 0
#define GPIO_IO_38       1ULL << 0
#define GPIO_IO_39       1ULL << 0

#endif

