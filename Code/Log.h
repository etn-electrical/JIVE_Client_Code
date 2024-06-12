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
#define RESET_ENERGY				0x00
#define UPDATE_ENERGY				0x11

#define METRO_RESET_CLEAR			0xA5
#define METRO_RESET					0x5A
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
void write_PhA_Frwrd_energy();
void write_PhB_frwrd_energy();
void write_PhA_reverse_energy();
void write_PhB_reverse_energy();
int8_t reset_energy();
int8_t reset_nonResettable_energy();
void read_PhA_frwrd_energy();
void read_PhB_frwrd_energy();
void read_PhA_reverse_energy();
void read_PhB_reverse_energy();

void nvs_write_metro_calib(metro_calib_struct calib_factors);
void write_metro_config();
size_t nvs_read_metro_calib(metro_calib_struct *calib_factors);
void read_metro_config();
void setDefaultMetroCalibConfig();

#ifdef __cplusplus
}
#endif
