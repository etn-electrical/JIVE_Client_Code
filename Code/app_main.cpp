/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


// Application header files
#include "Main.h"
#include "Test_Example.h"
#include "Prod_Spec_LTK_ESP32.h"
// Jonathan
#include "udp_client_for_GUI.h"


#include "DeviceGPIO.h"
#include "LED_Breaker.h"

//extern bool OTA_Temp_Flag;

#ifdef __cplusplus
extern "C"
{
#endif

TickType_t CurrentTick;

uint8_t TenMsCounter = TEN_MS;

extern DRAM_ATTR Device_Info_str DeviceInfo;

void app_main( void )
{
    HW_Init();
	// Call to main function
	main();

	app_main_client();
}

#ifdef __cplusplus
}
#endif
