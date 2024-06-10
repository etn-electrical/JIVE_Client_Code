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
#ifndef DEVICE_CONFIG_H
   #define DEVICE_CONFIG_H

#include "Device_Defines.h"
#include "sdkconfig.h"

// Defines the RTOS Task
#define USE_FULL_RTOS

static const uint16_t WATCHDOG_TIMEOUT = 1000U;

/*
 *****************************************************************************************
 *		Hardware clock frequency configuration
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *      Clock Frequency Definition
 *****************************************************************************************
 */

#ifdef CONFIG_IDF_TARGET_ESP32
#define MASTER_CLOCK_FREQUENCY CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
#endif

/*
 *****************************************************************************************
 *      Clock Frequency Definition
 *****************************************************************************************
 */
static const uint8_t MAIN_RAM_BLOCK = 0U;
static const uint8_t DCI_RAM_BLOCK = 1U;
static const uint8_t MAX_NUMBER_OF_RAM_BLOCKS = 1U;

#ifdef DEBUG
	#define OS_TASKER_DEBUG
// #define CR_TASKER_DEBUG
#endif


#endif
