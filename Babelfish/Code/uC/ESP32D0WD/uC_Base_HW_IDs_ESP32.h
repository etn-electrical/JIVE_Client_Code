/**
 *****************************************************************************************
 *	@file		uC_Base_HW_IDs_ESP32.h
 *
 *	@brief		Contains variables and constants used for uC_Base_HW_IDs_ESP32
 *
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef uC_BASE_HW_IDS_ESP32_H
#define uC_BASE_HW_IDS_ESP32_H

#include "uC_Base.h"
#include "driver/twai.h"

/**
 *****************************************************
 * @brief	PIO Ctrl ID's
 *****************************************************
 */
enum
{
	uC_BASE_PIO_0,					///< GPIO Control 0
	uC_BASE_PIO_1,					///< GPIO Control 1
	uC_BASE_PIO_2,					///< GPIO Control 2
	uC_BASE_PIO_3,					///< GPIO Control 3
	uC_BASE_PIO_4,					///< GPIO Control 4
	uC_BASE_PIO_5,					///< GPIO Control 5
	uC_BASE_PIO_6,					///< GPIO Control 6
	uC_BASE_PIO_7,					///< GPIO Control 7
	uC_BASE_PIO_8,					///< GPIO Control 8
	uC_BASE_PIO_9,					///< GPIO Control 9
	uC_BASE_PIO_10,					///< GPIO Control 10
	uC_BASE_PIO_11,					///< GPIO Control 11
	uC_BASE_PIO_12,					///< GPIO Control 12
	uC_BASE_PIO_13,					///< GPIO Control 13
	uC_BASE_PIO_14,					///< GPIO Control 14
	uC_BASE_PIO_15,					///< GPIO Control 15
	uC_BASE_PIO_16,					///< GPIO Control 16
	uC_BASE_PIO_17,					///< GPIO Control 17
	uC_BASE_PIO_18,					///< GPIO Control 18
	uC_BASE_PIO_19,					///< GPIO Control 19
	uC_BASE_PIO_20,					///< GPIO Control 20
	uC_BASE_PIO_21,					///< GPIO Control 21
	uC_BASE_PIO_22,					///< GPIO Control 22
	uC_BASE_PIO_23,					///< GPIO Control 23
	uC_BASE_PIO_24,					///< GPIO Control 24
	uC_BASE_PIO_25,					///< GPIO Control 25
	uC_BASE_PIO_26,					///< GPIO Control 26
	uC_BASE_PIO_27,					///< GPIO Control 27
	uC_BASE_PIO_28,					///< GPIO Control 28
	uC_BASE_PIO_29,					///< GPIO Control 29
	uC_BASE_PIO_30,					///< GPIO Control 30
	uC_BASE_PIO_31,					///< GPIO Control 31
	uC_BASE_PIO_32,					///< GPIO Control 32
	uC_BASE_PIO_33,					///< GPIO Control 33
	uC_BASE_PIO_34,					///< GPIO Control 34
	uC_BASE_PIO_35,					///< GPIO Control 35
	uC_BASE_PIO_36,					///< GPIO Control 36
	uC_BASE_PIO_37,					///< GPIO Control 37
	uC_BASE_PIO_38,					///< GPIO Control 38
	uC_BASE_PIO_39,					///< GPIO Control 39
	uC_BASE_MAX_PIO_CTRLS
};

/**
 *****************************************************
 * @brief		Timer Ctrl ID's, Two timer groups and each
 * @n			groups contains two timers total 4 timers
 *****************************************************
 */
enum
{
	uC_BASE_TIMER_CTRL_1,			///< Timer Control 1
	uC_BASE_TIMER_CTRL_2,			///< Timer Control 2
	uC_BASE_TIMER_CTRL_3,			///< Timer Control 3
	uC_BASE_TIMER_CTRL_4,			///< Timer Control 4
	uC_BASE_MAX_TIMER_CTRLS
};

/**
 *****************************************************
 * @brief		AtoD Application Peripheral ID's
 *****************************************************
 */
enum uC_BASE_ATOD_CTRL_ENUM
{
	uC_BASE_ATOD_CTRL_1,			///< ADC1 Control
	uC_BASE_ATOD_MAX_CTRLS
};

/**
 *****************************************************
 * @brief		I2C control ID's
 *****************************************************
 */
enum
{
	uC_BASE_I2C_CTRL_1,
	uC_BASE_I2C_CTRL_2,
	uC_BASE_I2C_MAX_CTRLS
};

/**
 *****************************************************
 * @brief		CAN Chip Select Application Peripheral ID's
 *****************************************************
 */
enum
{
	uC_BASE_CAN_CTRL_1,
	uC_BASE_CAN_MAX_CTRLS
};

/**
 *****************************************************
 * @brief		PWM  Peripheral ID's
 *****************************************************
 */
enum
{
	uC_BASE_PWM_CTRL_1,
	uC_BASE_PWM_CTRL_2,
	uC_BASE_PWM_MAX_CTRLS
};

/**
 *****************************************************
 * @brief		PWM  Channnels
 *****************************************************
 */
enum
{
	uC_BASE_PWM_CHANNEL_1 = 1,
	uC_BASE_PWM_CHANNEL_2,
	uC_BASE_PWM_CHANNEL_3,
	uC_BASE_PWM_CHANNEL_4,
	uC_BASE_PWM_CHANNEL_5,
	uC_BASE_PWM_CHANNEL_6,
	uC_BASE_PWM_MAX_CHANNELS
};

/**
 *****************************************************
 * @brief		Structure for log flash memory
 *****************************************************
 */
typedef uint32_t sector_addr_t;
typedef uint32_t sector_num_t;
typedef struct
{
	sector_addr_t sector_size;
	sector_num_t sector_no;
} flash_page_t;

/**
 * @brief		Hardware timer clock divider
 */
static const uint8_t TIMER_DIVIDER = 16;

/**
 * @brief		convert counter value to seconds
 */
static const double TIMER_SCALE = ( TIMER_BASE_CLK / TIMER_DIVIDER );

/**
 * @brief        Timer interval for the first timer
 *               We can vary this time interval as per requirements
 *               for eg. 0.5= 500ms, 1 = 1sec, 2 = 2sec
 */
static const double TIMER_INTERVAL0_SEC = 0.5;

/**
 * @brief		sample test interval for the second timer
 */
static const double TIMER_INTERVAL1_SEC = 5.78;

/**
 * @brief		I2C master clock frequency
 */
static const uint32_t I2C_EXAMPLE_MASTER_FREQ_HZ = 100000L;

/**
 * @brief		CAN standard Mask ID
 */
static const uint32_t CAN_STANDARD_MASK_ID = ~( TWAI_STD_ID_MASK << 21 );

/**
 * @brief		CAN standard message ID
 */
static const uint32_t CAN_STANDARD_MSG_ID = 0x555 << 21;

#endif
