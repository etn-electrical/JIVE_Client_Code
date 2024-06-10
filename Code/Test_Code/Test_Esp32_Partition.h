/**
 *****************************************************************************************
 *  @file	Test_Esp32_Partition.h
 *	@brief 	Example code for providing partition information.
 *
 *	@details This test code is responsible for providing partition information when called.
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TEST_ESP32_PARTITION_H_
	#define TEST_ESP32_PARTITION_H_

#include <stdio.h>
#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "Esp32_Partition.h"
#include "uC_Code_Range.h"
#include "uC_Fw_Code_Range.h"
#include "FUS_Debug.h"
#include "esp_ota_ops.h"

/*
 *****************************************************************************************
 *@brief         Function to test ESP32 OTA partition
 *@param[in]     none
 *@return        none
 *****************************************************************************************
 */
void Test_Esp32_Partition( void );

#endif	/* TEST_ESP32_PARTITION_H_ */
