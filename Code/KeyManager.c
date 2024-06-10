/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : EventManager.c
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/22/2021
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
#include "Breaker.h"
#include "LED_Breaker.h"
#include "EventManager.h"
#include "KeyManager.h"
#include "DeviceGPIO.h"


/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/


Switch_Info_Str SwitchInfo;
extern DRAM_ATTR Device_Info_str DeviceInfo;
static uint8_t  DisableKeyPress = true;

Switch DoubleClkSwt; 

#ifdef _DEBUG_
    DebugSwitchStateMachine_enum DebugSwitch = UnKown;
#endif

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/


/*============================ KeyStateMachineInit ===============================
** Function description
** Initiate SwitchInfo structure
**
** Called from ApplicationInitSW()
**
**-------------------------------------------------------------------------*/
void KeyStateMachineInit(void)
{
    memset ((unsigned char *)&SwitchInfo,0,sizeof (SwitchInfo));
}

/*=================================== DisableSwitchs =======================
**    Function description :
**          disable switchs
**
**--------------------------------------------------------------------------*/
void DisableSwitchs()
{
    DisableKeyPress = true;
}

/*=================================== EnableSwitchs =======================
**    Function description :
**          Enable switchs
**
**--------------------------------------------------------------------------*/
void EnableSwitchs()
{
    DisableKeyPress = false;
}


/*============================ KeyStateMachine ===============================
** Function description
** scan input pins for change and convert it to events.
**
** Called from ZCB_MainTimer()
**
**-------------------------------------------------------------------------*/
void KeyStateMachine(void)
{
#ifndef _ESP32_MINI_1_
    Switch Swt;
    bool KeyPress = false;
    bool KeyReleased = false;

    if (DisableKeyPress == false)
    {
        for (Swt = SWT_ON_OFF; Swt < SWT_MAX ;Swt++)
        {

            switch (Swt)
            {

                case SWT_ON_OFF :

                    if (gpio_get_level(SWITCH_ON_OFF_PIN_Pin) == LOW)
                    {
                        KeyPress = true;
                        //ets_printf("SWT_ON_OFF\n");

                    }
                    else
                    {
                        if (SwitchInfo.SwitchStr[Swt].StateMachine > SWT_DEBOUNCE)
                        {
                            KeyReleased = true;
                            //ets_printf("SWT_OFF_RELEASE\n");
                            if (SwitchInfo.DoublePrssFlag == true)
                            {
                                SwitchInfo.SwitchStr[Swt].StateMachine = SWT_IDLE;

                            }    
                        }
                        else
                        {
                            SwitchInfo.SwitchStr[Swt].Counter = 0;
                        }
                    }

                break;

 
                default:
                break;

            }


            if (KeyPress)
            {

                if (SwitchInfo.SwitchStr[Swt].StateMachine == SWT_IDLE)
                {
                    SwitchInfo.SwitchStr[Swt].Counter = 0;
                    SwitchInfo.SwitchStr[Swt].StateMachine = SWT_DEBOUNCE;
                }
                else
                {
                    SwitchInfo.SwitchStr[Swt].Counter++;
                }
                switch (SwitchInfo.SwitchStr[Swt].StateMachine)
                {
                    case SWT_DEBOUNCE :
                    {
                        if (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_DEBOUNCE)
                        {
                            SwitchInfo.SwitchStr[Swt].StateMachine = SWT_PRESSED;
                            SwitchInfo.SwitchStr[Swt].Press = true;
                            SwitchInfo.NoOfKeysPressed++;
                            DoubleClkSwt = Swt;
                            if (Swt == SWT_ON_OFF)
                            {
                                if ((SwitchInfo.DoublePrssFlag == true) && (SwitchInfo.DoublePrssCounter <= DOUBLE_CLICK_TIMER))
                                {
                                    SwitchInfo.SwitchStr[DoubleClkSwt].StateMachine = SWT_DOUBLE_PRESSED;
                                    SwitchInfo.DoublePrssFlag = false;
                                    SwitchInfo.DoublePrssCounter = 0;
                                }
                            }        
                                
                        }
                    }
                    break;


                    case SWT_PRESSED :
                    {
                        if (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_LONGPRESS)
                        {
                            SwitchInfo.DoublePrssFlag = false;
                            SwitchInfo.DoublePrssCounter = 0;
                            SwitchInfo.SwitchStr[Swt].StateMachine = SWT_LONG_PRESS;
                        }
                    }
                    break;

                    case SWT_LONG_PRESS :
                    {
                        if (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_2_SECOND_PRESS)
                        {
                            SwitchInfo.SwitchStr[Swt].StateMachine = SWT_2S_PRESS;
                            SwitchInfo.Processing = false;
                        }
                    }
                    break;
                    case SWT_2S_PRESS :
                    {
                        if (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_4_SECOND_PRESS)
                        {
                            SwitchInfo.SwitchStr[Swt].StateMachine = SWT_4S_PRESS;
                            SwitchInfo.Processing = false;
                        }
                    }
                    break;

                    case SWT_4S_PRESS :
                    {
                        if (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_5_SECOND_PRESS)
                        {
                            SwitchInfo.SwitchStr[Swt].StateMachine = SWT_5S_PRESS;
                            SwitchInfo.Processing = false;
                        }
                    }
                    break;

                    case SWT_5S_PRESS :
                    {
                        if (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_8_SECOND_PRESS)
                        {
                            SwitchInfo.SwitchStr[Swt].StateMachine = SWT_8S_PRESS;
                            SwitchInfo.Processing = false;
                        }
                    }
                    break;
                    case SWT_8S_PRESS :
                    {
                        if ( (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_10_SECOND_PRESS) )

                        {
                            SwitchInfo.SwitchStr[Swt].StateMachine = SWT_10S_PRESS;
                            SwitchInfo.Processing = false;
                        }
                    }
                    break;

                    case SWT_10S_PRESS :
                    {
                        if ( (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_13_SECOND_PRESS) )
                        {
                           SwitchInfo.SwitchStr[Swt].StateMachine = SWT_13S_PRESS;
                           SwitchInfo.Processing = false;
                        }
                    }
                    break;

                    case SWT_13S_PRESS :
                    {
                        if ( (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_15_SECOND_PRESS))
                        {
                           SwitchInfo.SwitchStr[Swt].StateMachine = SWT_15S_PRESS;
                           SwitchInfo.Processing = false;
                        }
                    }
                    break;


                    case SWT_15S_PRESS :
                    {
                        if ( (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_18_SECOND_PRESS) )
                        {
                           SwitchInfo.SwitchStr[Swt].StateMachine = SWT_18S_PRESS;
                           SwitchInfo.Processing = false;
                        }
                    }
                    break;

                    case SWT_18S_PRESS :
                    {
                        if ( (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_20_SECOND_PRESS) )
                        {
                           SwitchInfo.SwitchStr[Swt].StateMachine = SWT_20S_PRESS;
                           SwitchInfo.Processing = false;
                        }
                    }
                    break;



                    case SWT_20S_PRESS :
                    {
                        if ( (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_23_SECOND_PRESS) )
                        {
                           SwitchInfo.SwitchStr[Swt].StateMachine = SWT_23S_PRESS;
                           SwitchInfo.Processing = false;
                        }
                    }
                    break;

                    case SWT_23S_PRESS :
                    {
                        if ( (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_25_SECOND_PRESS) )
                        {
                           SwitchInfo.SwitchStr[Swt].StateMachine = SWT_25S_PRESS;
                           SwitchInfo.Processing = false;
                        }
                    }
                    break;

                    case SWT_25S_PRESS :
                    {
                        if ( (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_30_SECOND_PRESS) )
                        {
                           SwitchInfo.SwitchStr[Swt].StateMachine = SWT_30S_PRESS;
                           SwitchInfo.Processing = false;
                        }
                    }
                    break;

#if 0
                    // we will keep at SWT_30S_PRESS for reseting
                    case SWT_30S_PRESS :
                    {
                        if ( (SwitchInfo.SwitchStr[Swt].Counter == SWITCH_33_SECOND_PRESS) )
                        {
                           SwitchInfo.SwitchStr[Swt].StateMachine = SWT_33S_PRESS;
                           SwitchInfo.Processing = false;
                        }
                    }
                    break;
#endif

                    default:
                    break;                
                }
            }

            if (SwitchInfo.SwitchStr[Swt].Press == true)
            {
                if (KeyReleased == true)
                {
                    SwitchInfo.SwitchStr[Swt].Press    = false;
                    if (SwitchInfo.SwitchStr[Swt].Counter >= SWITCH_LONGPRESS)
                    {
                        SwitchInfo.SwitchStr[Swt].Released = true;
                    }
                    else
                    {
                        //to handle double click
                        if (SwitchInfo.SwitchStr[Swt].StateMachine == SWT_DOUBLE_PRESSED)
                        {
                            SwitchInfo.SwitchStr[Swt].Released = true;
                        }
                        else
                        {
                            if (SwitchInfo.DoublePrssFlag == false)
                            {
                                SwitchInfo.DoublePrssFlag = true;
                                SwitchInfo.DoublePrssCounter = 0;
                                SwitchInfo.SwitchStr[Swt].Released = false;
                            }
                        }    
                    }

                    if ((SwitchInfo.SwitchStr[Swt].StateMachine >= SWT_PRESSED) && (SwitchInfo.NoOfKeysPressed != 0))
                    {
                        SwitchInfo.NoOfKeysPressed--;
                    }
                    SwitchInfo.SwitchStr[Swt].Counter = 0;
                }
            }
            KeyPress = false;
            KeyReleased = false;

        }
    }
    
    if (SwitchInfo.DoublePrssFlag == true)
    {
        SwitchInfo.DoublePrssCounter++;
    }
    if ((SwitchInfo.DoublePrssCounter > DOUBLE_CLICK_TIMER) && (SwitchInfo.DoublePrssFlag == true))
    {
        SwitchInfo.SwitchStr[DoubleClkSwt].StateMachine = SWT_PRESSED;
        SwitchInfo.DoublePrssFlag = false;
        SwitchInfo.SwitchStr[DoubleClkSwt].Released = true;
        SwitchInfo.DoublePrssCounter = 0;
        
    }    
#endif
}
