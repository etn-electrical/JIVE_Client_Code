/**
 *****************************************************************************************
 * @file		RTC_Example.h
 *
 * @brief		Contains the test application function prototype for internal RTC.
 *
 *****************************************************************************************
 */

#ifndef RTC_EXAMPLE_H_
	#define RTC_EXAMPLE_H_

#define STRFTIME_BUF_SIZE 64
#define EPOCH_TIME_SEC 1605100007	///< Set to current epoch time second.
									///< 1605100007 - Wednesday, November 11, 2020 6:36:47 PM GMT+05:30
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>
#include <sys/time.h>

/**
 * @brief  : RTC test Example function
 * @details: This function will print current date and time string for every 1 sec.
 * @return void.
 */
void Test_RTC_App_Main( void );

#endif	/* RTC_EXAMPLE_H_ */
