/***************************************************************************
*
* Copyright (c) 2019
* Eaton Cooper wiring devices.
* All Rights Reserved
*
*---------------------------------------------------------------------------
*
*
* Author: Ahmed El-Gayyar
*
* Created   March 26, 2019
****************************************************************************/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/


#ifndef _LED_H_
#define _LED_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "freertos/FreeRTOS.h"
#include <string.h>
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_int_wdt.h"
#include <stdio.h>
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"


#include "DeviceGPIO.h"
#include "Breaker.h"

/****************************************************************************
 *                              General definitions                               
 ****************************************************************************/




/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/





typedef enum
{
  LED_NORMAL = 0,        //0  //normal operation on or off
  LED_BLINK,             //1
  LED_CYCLE              //2
}LED_STATE;

typedef enum
{
  LED_OFF = 0,        //0 
  LED_ON,             //1
}LED_VALUE;



typedef enum
{
  RGB_RED   =  0,   //0
  RGB_YELLOW,       //1
  RGB_GREEN     ,   //2
  RGB_BLUE      ,   //3
  RGB_CYAN	    ,   //4
  RGB_MEGENTA   ,   //5
  RGB_WHITE	    ,   //6 
  RGB_MAX	    ,   //7
  LED_UNASSIGNED_COLOR = 0xFF
}RGB_COLOR;

#define NO_CONNECTION_STRING_PROGRAMMED_COLOR     RGB_BLUE // That means the device has no connection string programmed

#define UNPROVISIONED_COLOR     RGB_MEGENTA // should be uniq not repeated

#define RUNNING_COLOR			RGB_GREEN

#define DELAYED_OFF_COLOR       RGB_GREEN  //OFF Button 2 seconds
#define MAX_BRIGHTNEES_COLOR    RGB_GREEN  //ON Button  2 seconds

#define POWER_UP_STATE_COLOR    RGB_BLUE   //OFF Button 5 seconds
#define MAX_MIN_LEVEL_COLOR     RGB_BLUE   //ON  Button 5 seconds

#define RAMPING_COLOR           RGB_CYAN   //OFF Button 10 seconds
#define LED_KICK_START_COLOR    RGB_CYAN   //ON  Button 10 seconds

#define LED_BRIGHTNESS_COLOR    RGB_WHITE  //OFF and ON Buttons 15 seconds

#define LED_ON_OFF_BRIGHTNESS_COLOR    RGB_YELLOW  //OFF and ON Buttons 15 seconds


#define RESET_COLOR             RGB_RED    //ON Buttons 30 seconds

#define MIN_SET_CONFIG_COLOR    RGB_GREEN  //During configure the Min brightness level
#define MAX_SET_CONFIG_COLOR    RGB_CYAN   //During configure the Max brightness level

#define DEFAULT_COLOR           RGB_YELLOW


#define PROVISION_STARTED_COLOR                               RGB_CYAN
#define PROVISION_BLE_CONNECTED_COLOR                         RGB_BLUE
#define PROVISION_GOT_IP_COLOR                                RGB_YELLOW
#define PROVISION_GOT_SSID_COLOR                              RGB_GREEN
#define PROVISION_IOT_NOT_PROVISIONED_NOT_CONNECTED_COLOR     RGB_RED     //That means we are provisining the device but for whatever reason
                                                                    //something wrong with the connection string
                                                                    //We will remove provising info and reset

#define PROVISION_IOT_PROVISIONED_NOT_CONNECTED_COLOR         RGB_YELLOW  //That means, the device was previously provisioned and connected
                                                                    //but we lost connection. Keep it yellow
                                                                    
//#define PROVISION_IOT_WRONG_PASSOWRD_COLOR                    RGB_YELLOW
#define PROVISION_IOT_WRONG_PASSOWRD_COLOR                    RGB_MEGENTA //Nitin

#define WIFI_IOT_CLOCK_NOT_SET_COLOR                          RGB_YELLOW
#define WIFI_IOT_DISCONNECT_COLOR                             RGB_YELLOW


//#define OTA_START_COLOR          RGB_YELLOW
#define OTA_START_COLOR          RGB_CYAN
#define OTA_BOOT_WRITE_COLOR     RESET_COLOR








/****************************************************************************
 *                              PWM definitions                               
 ****************************************************************************/




//Blinking LED rates

#define BLINK_50_mS                 1  // based on 50 ms timer
#define BLINK_100_mS                2  // based on 50 ms timer
#define BLINK_150_mS                3  // based on 50 ms timer
#define BLINK_200_mS                4  // based on 50 ms timer
#define BLINK_250_mS                5  // based on 50 ms timer
#define BLINK_300_mS                6  // based on 50 ms timer
#define BLINK_400_mS                8  // based on 50 ms timer
#define BLINK_500_mS                10  // based on 50 ms timer
#define BLINK_1000_mS               20  // based on 50 ms timer
#define BLINK_1500_mS               30  // based on 50 ms timer
#define BLINK_2000_mS               40 // based on 50 ms timer
#define BLINK_3000_mS               60 // based on 50 ms timer
#define BLINK_4000_mS               80 // based on 50 ms timer
#define BLINK_5000_mS               100 // based on 50 ms timer around 5 seconds
#define BLINK_10000_mS              200 // based on 50 ms timer around 10 seconds
//#define BLINK_15000_mS              300 // based on 50 ms timer around 10 seconds
//#define BLINK_30000_mS              600 // based on 50 ms timer around 30 seconds
#define BLINK_FOREVER               0xFF


//#define BLINK_INTERVAL_1S           120  //8.3ms * 120 = 1 Second
//#define BLINK_INTERVAL_1S           1

#define ST_STANDARD_UPDATE_INTERVAL		(10 * 20) //seconds * 1000ms/s / 50ms/loop
#define ST_FAST_UPDATE_INTERVAL			(1 * 20) //seconds * 1000ms/s / 50ms/loop

#define BLNINK_LED_RATE             BLINK_300_mS
#define PASUE_BLINIKING             BLINK_2000_mS //Two Seconds

#define WIFI_DISCONNECT_LED_BLINKING_TIME_OUT BLINK_30000_mS


#ifdef __cplusplus
extern "C"
{
#endif

void LED_Init(void);
void  StopBlinkingLEDs (void);
void  BlinkLEDs (uint8_t TimeInterval, uint16_t TimeExpired, LED_STATE ledstate);
void  BlinkingLEDStateMachine(void);

#if defined USE_NETWORK_STATUS_LED_APPROACH
void SetST_LED(void);
#else
void UpdateSTLED(bool force_update);
#endif

void TurnOffLed (void);
void TurnOnLed (void);
bool IsLED_Blinking(void);
void SetLED_Level(void);

void  BlinkLED_N_Times (uint8_t NumberOfBlinks);



void SetRGB_Color (RGB_COLOR Color);
void TurnOnRGB(void);
void TurnOffRGB(void);
    
#ifdef __cplusplus
}
#endif    

#endif //_LED_H_
