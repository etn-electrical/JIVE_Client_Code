/***************************************************************************
*
* Copyright (c) 2017
* Eaton Cooper wiring devices.
* All Rights Reserved
*
*---------------------------------------------------------------------------
*
*
* Author: Ahmed El-Gayyar
*
* Created   Feb 7, 2017
****************************************************************************/

#ifndef _KEYMAN_H_
#define _KEYMAN_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "freertos/FreeRTOS.h"
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_int_wdt.h"

//#include "ack_examples.h"
/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/



/****************************************************************************
 *                              Timers definitions                               
 ****************************************************************************/

#define ONE_SECOND                  100 // based on 10ms timer
#define FIFTY_MS                    5  // based on 10 ms timer

#define SWITCH_DEBOUNCE             4    // 10 ms based on 10ms timer
#define SWITCH_LONGPRESS            40   // 1  Seconds based on 10ms timer
#define SWITCH_2_SECOND_PRESS       200  // 2  Seconds based on 10ms timer
#define SWITCH_4_SECOND_PRESS       400  // 4  Seconds based on 10ms timer
#define SWITCH_5_SECOND_PRESS       500  // 5  Seconds based on 10ms timer
#define SWITCH_6_SECOND_PRESS       600  // 6  Seconds based on 10ms timer
#define SWITCH_8_SECOND_PRESS       800  // 8  Seconds based on 10ms timer
#define SWITCH_10_SECOND_PRESS      1000 // 10 Seconds based on 10ms timer
#define SWITCH_13_SECOND_PRESS      1300 // 13 Seconds based on 10ms timer
#define SWITCH_15_SECOND_PRESS      1500 // 15 Seconds based on 10ms timer
#define SWITCH_18_SECOND_PRESS      1800 // 18 Seconds based on 10ms timer
#define SWITCH_19_SECOND_PRESS      1900 // 19 Seconds based on 10ms timer
#define SWITCH_20_SECOND_PRESS      2000 // 20 Seconds based on 10ms timer
#define SWITCH_23_SECOND_PRESS      2300 // 23 Seconds based on 10ms timer
#define SWITCH_25_SECOND_PRESS      2500 // 25 Seconds based on 10ms timer
#define SWITCH_28_SECOND_PRESS      2800 // 28 Seconds based on 10ms timer
#define SWITCH_30_SECOND_PRESS      3000 // 30 Seconds based on 10ms timer
#define SWITCH_33_SECOND_PRESS      3300 // 33 Seconds based on 10ms timer

#define DOUBLE_CLICK_TIMER          25  //based on 10 ms timer
#define TIME_BETWEEN_DOUBLE_CLICK_TIMER      DOUBLE_CLICK_TIMER + 60  //based on 10 ms timer



#define KEY_PRESSED     0
#define KEY_RELEASED    1 


typedef enum
{
    SWT_ON_OFF = 0,            //0
    SWT_MAX,                
    SWT_ON,                 
    SWT_DIM_UP,             
    SWT_DIM_DOWN,           

}Switch;




//Switch state Machine

// Relay definition
typedef enum
{
    SWT_IDLE = 0,       //0
    SWT_DEBOUNCE,       //1
    SWT_PRESSED,        //2
    SWT_DOUBLE_PRESSED, //3 
    SWT_LONG_PRESS,     //4
    SWT_2S_PRESS,       //5
    SWT_4S_PRESS,       //6
    SWT_5S_PRESS,       //7
    SWT_6S_PRESS,       //8
    SWT_8S_PRESS,       //9
    SWT_10S_PRESS,      //10  
    SWT_13S_PRESS,      //11  
    SWT_15S_PRESS,      //12
    SWT_18S_PRESS,      //13
    SWT_19S_PRESS,      //14      
    SWT_20S_PRESS,      //15    
    SWT_23S_PRESS,      //16    
    SWT_25S_PRESS,      //17    
    SWT_28S_PRESS,      //18    
    SWT_30S_PRESS,      //19 
    SWT_33S_PRESS       //20 
}SwitchStateMachine_enum;





typedef struct
{
    uint16_t                    Counter;         // for the timer to determin if is long press or short press
    uint8_t                     Press;    
    uint8_t                     Released;
    SwitchStateMachine_enum     StateMachine;

}SwitchInfo_Str;

typedef struct
{
    Switch                 SwitchID;
    SwitchInfo_Str         SwitchStr[SWT_MAX];
    uint8_t                Processing;
    uint8_t                NoOfKeysPressed;
    uint8_t                DoublePrssFlag;
    uint8_t                DoublePrssCounter;
    uint8_t                DoublePressState;   //only for on/off
    uint8_t                PressAndHold;       //only for on/off
    
}Switch_Info_Str;



/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/


/** 
 * @brief KeyScan
 * Scan input pins for change and convert it to events. Function should be
 * placed in ApplicationPoll() routine.
 */
void KeyScan(void);
void KeyStateMachine(void);
void KeyStateMachineInit(void);
void DisableSwitchs(void);
void EnableSwitchs(void);
bool IsSwitchDisabled(void);


#endif /* _KEYMAN_H_ */


