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
 * Last Changed     : $Date: 07/12/2022
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

#include <wifi_provisioning/manager.h>

#include "Breaker.h"
#include "DeviceGPIO.h"
#include "LED_Breaker.h"
#include "EventManager.h"
#include "KeyManager.h"
#include "uart_events.h"

#include "Flash.h"
#include "ErrorLog.h"


/****************************************************************************/
/*                      Functions prototypes                                */
/****************************************************************************/



/****************************************************************************/
/*                              Global variables                            */
/****************************************************************************/
extern Switch_Info_Str SwitchInfo;
extern DRAM_ATTR Device_Info_str DeviceInfo;

extern metro_flag_t g_metroFlag;
extern M2M_UART_COMMN gm2m_uart_comm_ss_onoff;

uint8_t OneHourCounter = 0;

static uint8_t ToggleFlag = false; 

uint8_t FirstTimeConnection = true;


TickType_t SendingTimeStartTick, SendingTimeEndTick, SendingTimeDifference;
uint8_t SendingTimeFlagStart  = false;
uint8_t SendingTimeFlagDone = false;



bool WriteToFlashTimerflag = false;
bool WriteToFlash = false;
uint8_t WriteToFlashTimer = WRITING_TO_FLASH_WAIT_TIMER;

/*=========================== Event_10msTimer  ==========================
**    Function description :
**    This function is called from timer_10ms();
**-----------------------------------------------------------------------*/

void Event_10msTimer()
{
	#define METRO_CALIB_SAMPLE_TIMER 2 	//means every 20 msec
    static uint8_t MetroCalibSampleTimer = METRO_CALIB_SAMPLE_TIMER;

	if (MetroCalibSampleTimer != 0)
	{
		MetroCalibSampleTimer--;
	}

	if(MetroCalibSampleTimer == 0)
	{
		MetroCalibSampleTimer = METRO_CALIB_SAMPLE_TIMER;

		if (g_metroFlag.data_average_status)
		{
			Metro_Read_Event_Set();
		}
	}
}


/*=========================== Event_50msTimer  ==========================
**    Function description :
**    This function is called from timer_50ms();
**-----------------------------------------------------------------------*/

void Event_50msTimer()
{
    #define UPDATE_DCI  1 // means every 50ms
    #define UPDATE_OTA_RGB  5 // means every 250ms

	#define READ_METRO_TIMER 20 	//means every 1 sec
	#define READ_ENERGY_TIMER 100	 //means every 5 sec
	#define UPDATE_ENERGY_TIMER 1200	//means every 30 mins
	#define TRENDING_DATA_TIMER 6000 //means every 5 mins

    static uint8_t UpdateDCI = UPDATE_DCI;
    static uint8_t UpdateOTA_RGB = UPDATE_OTA_RGB;

    static RGB_COLOR OTA_LedIndicator  = RGB_RED;
    static bool  TurnRGB_Flag= true;
    static OTA_State_enum PrevOTA_State = OTA_NON;
    
    static uint8_t ReadEnergyTimer = READ_ENERGY_TIMER;
    static uint16_t UpdateEnergyTimer = UPDATE_ENERGY_TIMER;
    static uint8_t ReadMetroTimer = READ_METRO_TIMER;
    static uint16_t TrendingDataTimer = TRENDING_DATA_TIMER;


	if (ReadEnergyTimer != 0)
	 {
		ReadEnergyTimer--;
	 }

	if(ReadEnergyTimer == 0)
	{
		ReadEnergyTimer = READ_ENERGY_TIMER;
		g_metroFlag.read_Energy = true;
	}


	if (UpdateEnergyTimer != 0)
	{
		UpdateEnergyTimer--;
	}
	if(UpdateEnergyTimer == 0)
	{
		UpdateEnergyTimer = UPDATE_ENERGY_TIMER;
		g_metroFlag.energy_nvs_write = true;
	}


	if (TrendingDataTimer != 0)
	{
		TrendingDataTimer--;
	}
	if(TrendingDataTimer == 0)
	{
		TrendingDataTimer = TRENDING_DATA_TIMER;
		g_metroFlag.trending_data_timer = true;
	}


	if (ReadMetroTimer != 0)
	{
		ReadMetroTimer--;
	}

	if(ReadMetroTimer == 0)
	{
		ReadMetroTimer = READ_METRO_TIMER;

		if (!g_metroFlag.data_average_status)
		{
			g_metroFlag.read_data = true;
		}
	}

#ifdef _DEBUG_    
    static uint32_t AppTimer = 0;
    static uint8_t AppTimerFlag = 1;
#endif    

    if (UpdateDCI != 0)
    {
        UpdateDCI--;
    }
    else
    {
        UpdateDCI = UPDATE_DCI;

        if  (DeviceInfo.PxInitiated == true)
        {
            DCI_UpdatePrimaryContactStatus();
            DCI_UpdateSecondaryContactStatus();

        	DCI_UpdatePrimaryState();
        	DCI_UpdateSecondaryState();
        	DCI_UpdatePathStatus();

            if(g_metroFlag.update_trending_data == true)
            {
				DCI_Update_VoltageL1();
				DCI_Update_VoltageL2();

				DCI_Update_CurrentL1();
				DCI_Update_CurrentL2();

				DCI_Update_PowerFactorL1();
				DCI_Update_PowerFactorL2();

				DCI_Update_FrequencyL1();
				DCI_Update_FrequencyL2();

				DCI_Update_ActivePowerL1();
				DCI_Update_ActivePowerL2();

				DCI_Update_ReactivePowerL1();
				DCI_Update_ReactivePowerL2();

				DCI_Update_ApparentPowerL1();
				DCI_Update_ApparentPowerL2();

				DCI_Update_ActiveEnergyL1();
				DCI_Update_ActiveEnergyL2();

				DCI_Update_ReactiveEnergyL1();
				DCI_Update_ReactiveEnergyL2();

				DCI_Update_ApparentEnergyL1();
				DCI_Update_ApparentEnergyL2();

				DCI_Update_RevActiveEnergyL1();
				DCI_Update_RevActiveEnergyL2();

				DCI_Update_RevReactiveEnergyL1();
				DCI_Update_RevReactiveEnergyL2();

				DCI_Update_RevApparentEnergyL1();
				DCI_Update_RevApparentEnergyL2();

				g_metroFlag.update_trending_data = false;

            }

            if (FirstTimeConnection)
            {
                FirstTimeConnection = false;
                DCI_Update_SSID();
                DCI_Update_DevicePassWord();
                DCI_Update_DeviceMacAdd();
                DCI_Update_FirmWarVer();
                DCI_UpdateBuildType();
                DCI_UpdateNewProgrammedPartNumber();
            }
        }

    }

    if (ToggleFlag)
    {
        ToggleFlag = false;
        if (DeviceInfo.SecondaryContactState == SS_CLOSED)
        {
            TurnBreakerOn();
        }
        else if (DeviceInfo.SecondaryContactState == SS_OPEN)
        {
            TurnBreakerOff();
        }
    }


#ifdef _DEBUG_        
    if (AppTimerFlag)
    {
        //every 100ms
        //ets_printf(" CurrentTick %d \n", AppTimer);
        AppTimer++;
    }
    AppTimerFlag^=1;
#endif    
    
    //Handling OTA RGB colors
    if (DeviceInfo.OTA_State > OTA_NON)
    {
    
        if (UpdateOTA_RGB != 0)
        {
            UpdateOTA_RGB--;
        }
        else
        {
            UpdateOTA_RGB = UPDATE_OTA_RGB;
            if (PrevOTA_State != DeviceInfo.OTA_State)
            {
                PrevOTA_State = DeviceInfo.OTA_State;
                TurnRGB_Flag= true;
            }
            switch (DeviceInfo.OTA_State)
            {
                case OTA_START :
                    SetRGB_Color(OTA_START_COLOR);
                break;
                case OTA_MEM_EREASE :
                    SetRGB_Color(OTA_LedIndicator);
                    TurnOnRGB();
                    OTA_LedIndicator++;
                    if (OTA_LedIndicator>=RGB_MAX)
                    {
                        OTA_LedIndicator = RGB_RED;
                    }
                break;
                case OTA_MEM_WRITE :
                    SetRGB_Color(OTA_LedIndicator);
                    TurnOnRGB();
                    if (OTA_LedIndicator != RGB_RED)
                    {
                        OTA_LedIndicator--;
                    }
                    else
                    {
                        OTA_LedIndicator = RGB_WHITE;
                    }                
                break;       
                case OTA_BOOT_WRITE :
                    SetRGB_Color(OTA_BOOT_WRITE_COLOR);
                break;
                case OTA_NON :
                	//do nothing to make the compiler happy
                break;

            }
            if ((DeviceInfo.OTA_State == OTA_START) || (DeviceInfo.OTA_State == OTA_MEM_WRITE))
            {
               if (TurnRGB_Flag)
                {
                    TurnOnRGB();
                }
                else
                {
                    TurnOffRGB();
                }
                TurnRGB_Flag ^=1;                
            }
        }
    }
    

    if (WriteToFlashTimerflag == true)
    {
        if (WriteToFlashTimer != 0)
        {
            WriteToFlashTimer--;
        }
        else
        {
            WriteToFlashTimerflag = false;
            WriteToFlash = true;    
        }
    }
    
    UpdateFlash();
    

}


/*=========================== UpdateFlash  ==========================
**    Function description :
**    This function is called from Event_50msTimer();
**-------------------------------------------------------------------*/
void UpdateFlash()
{
    //if ( WriteToFlash )
    {
        if (DeviceInfo.DeviceNameChanged)
        {
            DeviceInfo.DeviceNameChanged = false;
            WriteDataToFlash(EEOFFSET_DEVICE_NAME_LABEL,DeviceInfo.DeviceName,sizeof(DeviceInfo.DeviceName));
        }
    }
}
    




/*=========================== Event_1MinuteTimer  ==========================
**    Function description :
**    This function is called from timer_1Min();
**-----------------------------------------------------------------------*/
void Event_1MinuteTimer()
{
    static uint8_t OneMinuteCounter = 0;

    OneMinuteCounter++;
    if (OneMinuteCounter == ONE_HOUR_M)
    {
        OneMinuteCounter = 0;
        OneHourCounter++;
    }

    if (OneHourCounter == ONE_DAY)
    {
        OneHourCounter = 0;
        //Disable sending report based on Dmitry recommendation
        //DeviceInfo.ACK_SendChangeReportDueToLocalControl = true;
    }

}




/*=========================== EventStateMachine  ==========================
**    Function description :
**    This the button press event
**    This function is called from timer_50ms();
**--------------------------------------------------------------------------*/

void EventStateMachine()
{
    static Switch Swt = SWT_ON_OFF; //Intialization

    // we should not process any pressing key action till
    // we finished what we have started

    if (SwitchInfo.NoOfKeysPressed <= 1)
    {

        if (SwitchInfo.SwitchStr[Swt].StateMachine >= SWT_LONG_PRESS)
        {
            // long press
            if (SwitchInfo.SwitchStr[Swt].Press == true)
            {
                if (SwitchInfo.Processing == false)
                {
                    SwitchInfo.Processing = true;
                    switch (Swt)
                    {
                        case SWT_ON_OFF:
                        {
                            switch  (SwitchInfo.SwitchStr[Swt].StateMachine)
                            {
                                case SWT_2S_PRESS :
                                   
                                break;

                                case SWT_5S_PRESS :
                                
                                break;

                                case SWT_8S_PRESS :

                                break;

                                case SWT_10S_PRESS :
                                break;

                                case SWT_13S_PRESS :
                                break;

                                case SWT_15S_PRESS :
                                break;

                                case SWT_20S_PRESS :
                                break;


                                case SWT_25S_PRESS :
                                break;

                                case SWT_30S_PRESS :
                                        //if (DeviceInfo.Installed == true)
                                        {
                                            // Self Reset
                                            SetRGB_Color(RESET_COLOR);
                                            BlinkLEDs(BLINK_50_mS,BLINK_FOREVER,LED_BLINK);
                                        }
                                    break;
                                default:
                                break;
                            }
                            
                        }
                        break;


                        default:
                        break;
                    }
                }
            }
            else
            {
                if (SwitchInfo.SwitchStr[Swt].Released == true)
                {
                    //released after long press
                    SwitchInfo.SwitchID   = Swt;
                    switch (Swt)
                    {
                        case SWT_ON_OFF:
                        {

                            switch  (SwitchInfo.SwitchStr[Swt].StateMachine)
                            {
                                case SWT_2S_PRESS :
 
                                break;
                                case SWT_5S_PRESS :
                                 break;

                                case SWT_10S_PRESS :

                                break;

                                case SWT_15S_PRESS :

                                break;

                                case SWT_20S_PRESS :

                                break;


                                case SWT_25S_PRESS :

                                break;

                                case SWT_30S_PRESS :
                                    //Reset
                                    BlinkLEDs(BLINK_100_mS,BLINK_FOREVER,LED_BLINK);
                                    ResetDevice(MANUFACTURING_RESET, true);
                                break;

                                default:
                                break;
                            }
                        }
                        break;


                        default:
                        break;
                    }
                    memset ((unsigned char *)&SwitchInfo.SwitchStr,0,sizeof (SwitchInfo_Str)*SWT_MAX);
                    SwitchInfo.Processing = false;
                }
            }
        }// if long press
        else
        {
            if (SwitchInfo.SwitchStr[Swt].Released == true)
            {
                if (SwitchInfo.SwitchStr[Swt].StateMachine == SWT_PRESSED)
                {
                    SwitchInfo.SwitchID   = Swt;
                    switch (Swt)
                    {
                        case SWT_ON_OFF:
                        	ToggleBreakerState();
                        break;

                        default:
                        break;
                    }
                }
                else
                {
                    if (SwitchInfo.SwitchStr[Swt].StateMachine == SWT_DOUBLE_PRESSED)
                    {
                        SwitchInfo.DoublePrssFlag = false;
                        ets_printf("Double Press\n");
                        if (DeviceInfo.DeviceInNetworkStatus == DEVICE_IS_NOT_IN_A_NETWORK)
                        {
                            StartProvisioning();
                        }
                    }
                }

                memset ((unsigned char *)&SwitchInfo.SwitchStr,0,sizeof (SwitchInfo_Str)*SWT_MAX);
            }
        }

        if (SwitchInfo.Processing == false)
        {
            Swt++;
            if (Swt >=SWT_MAX)
            {
                Swt = SWT_ON_OFF;
            }
        }
    }
    else
    {
        // more than 1 key was pressed
        // we shouldn't be here, but just in case, we will reset the key structure
        KeyStateMachineInit();
    }

}


/*=================================== ToggleBreakerState   =======================
**    Function description :
**         toggle the Breaker between on and off
**
**--------------------------------------------------------------------------*/

void ToggleBreakerState(void)
{
	//if (DeviceInfo.SecondaryContactState == SS_CLOSED)
	//{
	//	TurnBreakerOn();
	//}
	//else if (DeviceInfo.SecondaryContactState == SS_OPEN)
	//{
	//	TurnBreakerOff();
	//}

	 if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
	    {
	        ets_printf("ToggleBreaker\n");
	    }
	    SendingTimeFlagDone = false;
	    SendingTimeFlagStart = false;

	#ifdef _ERROR_LOG_DEBUG_
	//    ErrLog(LOG_BREAKER_TURN_ON);
	#endif


	//    if (DeviceInfo.IoTConnected)
	//    {
	//        SendingTimeFlagStart = true;
	//        SendingTimeStartTick = xTaskGetTickCount();
	//        ets_printf(" SendingTimeStartTick %d \n", SendingTimeStartTick);
	//    }

	memset(&gm2m_uart_comm_ss_onoff, 0x00, sizeof(gm2m_uart_comm_ss_onoff));
	//enable code and test on P0
	prepare_uart_command(&gm2m_uart_comm_ss_onoff, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_TOGGLE, NO_PAYLOAD_2, NO_PAYLOAD_2);
}

/*=================================== TurnBreakerOn   =======================
**    Function description :
**         Turn on Breaker (secondery contact
**
**--------------------------------------------------------------------------*/
void TurnBreakerOn ()
{

    
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {    
        ets_printf("TurnBreakerOn\n");
    }
    SendingTimeFlagDone = false;
    SendingTimeFlagStart = false;
    
#ifdef _ERROR_LOG_DEBUG_
    ErrLog(LOG_BREAKER_TURN_ON);
#endif
    

//    if (DeviceInfo.IoTConnected)
//    {
//        SendingTimeFlagStart = true;
//        SendingTimeStartTick = xTaskGetTickCount();
//        ets_printf(" SendingTimeStartTick %d \n", SendingTimeStartTick);
//    }
    
              
#if defined PREVENT_REQUESTING_CURRENT_STATE
    if ( (DeviceInfo.SecondaryContactState != SS_CLOSED) )
#endif
    {
    	memset(&gm2m_uart_comm_ss_onoff, 0x00, sizeof(gm2m_uart_comm_ss_onoff));
		//enable code and test on P0
		prepare_uart_command(&gm2m_uart_comm_ss_onoff, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_CLOSE, NO_PAYLOAD_2, NO_PAYLOAD_2);
    }

   // DeviceInfo.SecondaryContactState        = LOAD_ON;
    SetLED_Level();
    Set_LED_Pattern();
 
#ifdef _AUTOMATIC_PROVISIONIG_NO_BUTTONS_OR_RESET_    
    SetRGB_Color(RGB_WHITE);
    TurnOnRGB();
#endif
}

/*=================================== RF_ToggleBreakerState()   =======================
**    Function description :
**         When we call from clouds
**
**--------------------------------------------------------------------------*/
void RF_ToggleBreakerState()
{
    ToggleFlag = true;
}




/*=================================== TurnBreakerOff   =======================
**    Function description :
**         turn off breaker
**
**--------------------------------------------------------------------------*/
void TurnBreakerOff ()
{
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {
        ets_printf("TurnBreakerOFF\n");
    }
    SendingTimeFlagDone = false;
    SendingTimeFlagStart = false;
    
#ifdef _ERROR_LOG_DEBUG_
    ErrLog(LOG_BREAKER_TURN_OFF);
#endif
    

    //normal operation
#if defined PREVENT_REQUESTING_CURRENT_STATE
    if ( (DeviceInfo.SecondaryContactState != SS_OPEN) )
#endif
    {
      // DeviceInfo.SecondaryContactState = LOAD_OFF;
      memset(&gm2m_uart_comm_ss_onoff, 0x00, sizeof(gm2m_uart_comm_ss_onoff));
      //	enable code and test on P0
      prepare_uart_command(&gm2m_uart_comm_ss_onoff, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_OPEN, NO_PAYLOAD_2, NO_PAYLOAD_2);
    }
#ifdef _AUTOMATIC_PROVISIONIG_NO_BUTTONS_OR_RESET_    
    SetRGB_Color(RGB_WHITE);
    TurnOffRGB();
#endif
}


/*========================== EventOneSeconedTimer   ==========================
**    Function description :
**        1 second event timer, called from timer_1Sec()
**
**--------------------------------------------------------------------------*/

void EventOneSeconedTimer(void)
{
    static bool SendMaticMessageFlag = false;

#ifdef _DEBUG_
    static uint8_t i = 0;
#endif
    
    if (SendingTimeFlagDone)
    {
        SendingTimeFlagStart = false;
        SendingTimeFlagDone = false;
        SendingTimeDifference = SendingTimeEndTick - SendingTimeStartTick;
        ets_printf(" \n SendingTimeDifference %d \n", SendingTimeDifference);
    }

}

/****************************************************************************
 *                              Set_LED_Pattern
 *  To handle LED pattern if the device is connected or not connected
 ****************************************************************************/
void Set_LED_Pattern(void)
{
    if ( DeviceInfo.DeviceInNetworkStatus == DEVICE_IS_IN_A_NETWORK)
    {

        //if (DeviceInfo.ACK_ConnectivityState == ACK_NOT_CONNECTED_TO_ALEXA)
        //{
        //    ACK_WiFi_DisconnectDelayCounter = ACK_WIFI_DISCONNECT_DELAY; // since the last time we pressed any buttons
        //    ACK_WiFi_DisconnectDelayCounterflag = true;
        //}
    }
    else
    {
        SetRGB_Color(UNPROVISIONED_COLOR);

        if ( (DeviceInfo.ConnectionStringSaved == false) && (DeviceInfo.DidWeCheckTheConnectionString == true))
        {
            SetRGB_Color(NO_CONNECTION_STRING_PROGRAMMED_COLOR);
        }

        BlinkLEDs(BLINK_500_mS,BLINK_FOREVER,LED_BLINK); // Not part of a network
    }
}
