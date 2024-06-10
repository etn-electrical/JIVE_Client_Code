/**
 *****************************************************************************************
 * @file        Prod_Spec_RTC.h
 * @details     This file contains macro and prototype declaration for Internal RTC test application
 *              through interface class.
 * @copyright   2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef PROD_SPEC_RTC_H_
#define PROD_SPEC_RTC_H_

// #define EPOCH_TIME_SECOND 1622530451	///< Set to current epoch time second.
///< 1620393543 - Wednesday, November 11, 2020 6:36:47 PM GMT+05:30
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>
#include <sys/time.h>
#include "Timers_Lib.h"
#include "DCI_Owner.h"
#include "sntp_if.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 **********************************************************************************************
 * @brief						   RTC Init function
 * @details                        This function updates Epoch time when sntp service gets new
 *								   epoch time from ntp server or requires local epoch time.
 * @param[in] param 1              False = Get Epoch time , True = Set Epoch Time
 * @param[in] param 2              parameter to get or set epoch time
 * @param[in] param 3              parameter epoch time in seconds
 * @param[in] param 4              parameter subsecond in micro seconds
 * @return[out] void               none
 **********************************************************************************************
 */
void Prod_Spec_RTC_Init( void );

#ifdef __cplusplus
}

/**
 **********************************************************************************************
 * @brief                          This function updates Epoch time when sntp service gets new
 *								   epoch time from ntp server or requires local epoch time.
 * @param[in] param 1              False = Get Epoch time , True = Set Epoch Time
 * @param[in] param 2              parameter to get or set epoch time
 * @param[in] param 3              parameter epoch time in seconds
 * @param[in] param 4              parameter subsecond in micro seconds
 * @return[out] void               none
 * @n
 **********************************************************************************************
 */
void Update_Epoch_Time( SNTP_If::cback_param_t param, SNTP_If::cback_event_t event,
						uint32_t* epoch_time_sec, uint32_t* epoch_time_usec );

/**
 **********************************************************************************************
 * @brief                       This function will provide current epoch time.
 * @param[in] void              none
 * @return[out] uint32_t        epoch time
 * @n
 **********************************************************************************************
 */
uint32_t Get_Epoch_Time( void );

/**
 **********************************************************************************************
 * @brief                       This function provides SNTP Initialization for ESP32.
 * @param[in] void              none
 * @return[out] void            none
 * @n
 **********************************************************************************************
 */
void SNTP_Init( void );

#endif
#endif	/* PROD_SPEC_RTC_H_ */
