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
* Created   March 27, 2019
****************************************************************************/

#ifndef _EVENTMAN_H_
#define _EVENTMAN_H_

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

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
void Event_10msTimer(void);
void Event_50msTimer(void);
void Event_1MinuteTimer(void);
void EventStateMachine(void);
void RampingStateMachine(void);


#ifdef __cplusplus
extern "C"
{
#endif

void ToggleBreakerState(void);    
void TurnBreakerOn (void);
void TurnBreakerOff (void);


void Event10msTimer(void);

void EventOneSeconedTimer(void);



void Set_LED_Pattern(void);


void UpdateFlash(void);
void update_event_log(uint8_t);
void nvs_write_event();
void nvs_read_event_log();
void event_resetLog();

#ifdef __cplusplus
}
#endif

#endif
