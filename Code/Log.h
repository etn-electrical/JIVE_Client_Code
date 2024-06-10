/***************************************************************************
*
* Copyright (c) 2019
* Eaton Cooper wiring devices.
* All Rights Reserved
*
*---------------------------------------------------------------------------
*
*
* Author: Rahul Sakhare
*
* Created   Jan 30 2023
****************************************************************************/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "Breaker.h"
/****************************************************************************
 *                              CONSTANTS & MACROS:
 ****************************************************************************/
#define METRO_RESET_CLEAR			0xA5
#define METRO_RESET					0x5A
#define CALIBRATION_INIT_DONE		0x22
#define EERAM_READY				0x11
#define EERAM_RESET_CLEAR			0xA0
#define EERAM_RESET					0x0A
/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/


/****************************************************************************/
/*                     Variables and Structures                       */
/****************************************************************************/
/**
   * @brief Structure used to read/write energy from/to NVS
   *
   */
typedef struct __attribute__((__packed__))
{
	double active_energy;
	double reactive_energy;
	double apparent_energy;
}nvm_energy_st;

#define NVS_TEST 1

 /***************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
void nvs_write_metro_calib(metro_calib_struct calib_factors);
size_t nvs_read_metro_calib(metro_calib_struct *calib_factors);

#ifdef __cplusplus
}
#endif
