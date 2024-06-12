/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : DeviceGPIO.h
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/19/2022
 *
 ****************************************************************************/

/****************************************************************************
 *                              INCLUDE FILES                               
 ****************************************************************************/



/****************************************************************************
 *                              General definitions                               
 ****************************************************************************/

#ifndef __GPIO__H
#define __GPIO__H


#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
//#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_int_wdt.h"
#include "Breaker.h"





#define SWITCH_ON_OFF_PIN_Pin                       GPIO_NUM_37


#define RGB_R_PIN_Pin                              GPIO_NUM_25 //GPIO_NUM_18  //GPIO_NUM_8
#define RGB_G_PIN_Pin                              GPIO_NUM_27//GPIO_NUM_5  //GPIO_NUM_5
#define RGB_B_PIN_Pin                              GPIO_NUM_26//GPIO_NUM_23  //GPIO_NUM_7

#define BOARD_PIN_CS							   GPIO_NUM_5
#define BOARD_PIN_MOSI							   GPIO_NUM_7
#define BOARD_PIN_MISO							   GPIO_NUM_19
#define BOARD_PIN_SCLK							   GPIO_NUM_8
#define BOARD_PIN_RST 							   GPIO_NUM_20
#define BOARD_PIN_IRQ1							   GPIO_NUM_21
#define BOARD_PIN_TEST							   GPIO_NUM_4
#define BOARD_PIN_TEST1							   GPIO_NUM_2
#define BOARD_PIN_TEST2							   GPIO_NUM_15
#define BOARD_PIN_CF3_ZA						   GPIO_NUM_22



//#ifdef _DEBUG_
	#define  TEST_PIN_2  GPIO_NUM_26
//#endif

#define ESP_INTR_FLAG_DEFAULT 0
void SetGPIO_OutPut(gpio_num_t gpio_num);
void SetGPIO_InPut(gpio_num_t gpio_num);
void SetGPIO_ExtInterrupt(gpio_num_t gpio_num, gpio_int_type_t);

#endif //__GPIO__H

