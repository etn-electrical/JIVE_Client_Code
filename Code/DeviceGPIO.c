/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform dimmer
 *
 * File name        : DeviceGPIO.c
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/19/2022
 *
 ****************************************************************************/
#include "freertos/FreeRTOS.h"
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_int_wdt.h"

#include <stddef.h>
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "driver/timer.h"

#include "Breaker.h"
#include "LED_Breaker.h"
#include "EventManager.h"
#include "KeyManager.h"

#include "DeviceGPIO.h"


/*=================================== SetGPIO_OutPut   =============================
**    Function description :
**         To configure the GPIO as an output
**
**--------------------------------------------------------------------------*/

void SetGPIO_OutPut(gpio_num_t gpio_num)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1ULL<<gpio_num;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}    

/*=================================== SetGPIO_InPut   =============================
**    Function description :
**         To configure the GPIO as an output
**
**--------------------------------------------------------------------------*/
void SetGPIO_InPut(gpio_num_t gpio_num)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1ULL<<gpio_num;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}  

/*=================================== SetGPIO_ExtInterrupt   =============================
**    Function description :
**         To configure the GPIO as an output
**
**--------------------------------------------------------------------------*/
void SetGPIO_ExtInterrupt(gpio_num_t gpio_num, gpio_int_type_t interrupt_edge_type)
{
    gpio_config_t io_conf;
    //disable interrupt
    //io_conf.intr_type = GPIO_PIN_INTR_POSEDGE; //GPIO interrupt type : rising edge 
    io_conf.intr_type = interrupt_edge_type;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1ULL<<gpio_num;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}  

