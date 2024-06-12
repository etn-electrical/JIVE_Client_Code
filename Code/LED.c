/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : LED.c
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/19/2021
 *
 ****************************************************************************/

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_int_wdt.h"
#include "Breaker.h"
#include "LED_Breaker.h"
#include "DeviceGPIO.h"
#include "EventManager.h"
#include "uart_events.h"

/****************************************************************************/
/*                      Functions prototypes                                */
/****************************************************************************/



/****************************************************************************/
/*                              Global variables                            */
/****************************************************************************/


extern Device_Info_str DeviceInfo;
extern  uint8_t htim3;

extern M2M_UART_COMMN gm2m_uart_comm;


static RGB_COLOR RGB_Color = UNPROVISIONED_COLOR ;

uint8_t LED_counter = 0;

//uint16_t  BlinkingTimeInterval          = 0;
//uint16_t  BlinkingTimeExpire            = 0;
//uint16_t  BlinkingCounter               = 0;

uint8_t  BlinkingTimeInterval          = 0;
uint8_t  BlinkingTimeExpire            = 0;
uint8_t  BlinkingCounter               = 0;


uint8_t StartBlinkingLEDsFlag          = false;

uint8_t StartBlinking_N_Time_LEDsFlag  = false;
uint8_t StartBlinking_N_Time           = 0;
uint8_t Blinking_N_TimesCounter        = 0;

uint8_t HowManyBlinksCounter;




/*================================ LED_Init   ==============================
**    Function description :
**         Initiate LED info
**--------------------------------------------------------------------------*/
void LED_Init(void)
{

}






/*================================ TurnOffLed   ==============================
**    Function description :
**         Turn Off All 7 LED if LED form 1-7 and special case for the 8th
**--------------------------------------------------------------------------*/
void TurnOffLed ( )
{

}



/*================================ TurnOnLed   ==============================
**    Function description :
**         Turn On specefic LED and it turns off the other LED except for the 8th
**--------------------------------------------------------------------------*/
void TurnOnLed ()
{


}



/*================================ BlinkLEDs   ==============================
**    Function description :
**         This procedure would Blink LED in timeinterval (50 ms based) till
**         TimeExpired or Calling StopBlinkingLEDs () whatever comes first
**
**--------------------------------------------------------------------------*/

void BlinkLEDs (uint8_t  TimeInterval, uint16_t TimeExpired, LED_STATE ledstate)
{

    StartBlinkingLEDsFlag   = true;

    BlinkingTimeInterval    = TimeInterval;
    BlinkingTimeExpire      = TimeExpired;
    BlinkingCounter         = 0;

    SetLED_Level();

    // When we blink, we will blink at the default level so to avoid having on and off LED at the same level
}


/*================================ StopBlinkingLEDs   ==============================
**    Function description :
**         This procedure would stop blinking the LEDs
**
**--------------------------------------------------------------------------*/

void StopBlinkingLEDs ()
{

    //ets_printf("Stop Blinking\n");
    StartBlinkingLEDsFlag = false;

	#if !defined USE_NETWORK_STATUS_LED_APPROACH
    	//to clear LED stuff
    	UpdateSTLED(true);
	#endif

    if (DeviceInfo.DeviceInNetworkStatus == DEVICE_IS_IN_A_NETWORK) 
    {
        TurnOffRGB();
    }
    else
    {
        SetRGB_Color(UNPROVISIONED_COLOR);

        if ( (DeviceInfo.ConnectionStringSaved == false) && (DeviceInfo.DidWeCheckTheConnectionString == true))
        {
          SetRGB_Color(NO_CONNECTION_STRING_PROGRAMMED_COLOR);
        }
        BlinkLEDs(BLINK_500_mS,BLINK_FOREVER,LED_BLINK);
    }

    SetLED_Level();

}



/*==========================   BlinkingLEDStateMachine ======================
**    Function description :
**          Blinking the LED(s)
**
**
**--------------------------------------------------------------------------*/

void BlinkingLEDStateMachine()
{
    #define Blinking_N_Time 2
    static uint8_t HowManyTimeBlinking_N_TimesCounter = Blinking_N_Time;
    static bool    PauseBlinking = false;

#if defined USE_NETWORK_STATUS_LED_APPROACH
    SetST_LED();
#else
    UpdateSTLED(false);
#endif

    if (StartBlinkingLEDsFlag)
    {
        //ets_printf("Blink State Machine\n");
        if (BlinkingCounter < BlinkingTimeExpire)
        {

            if (!(BlinkingCounter%BlinkingTimeInterval))
            {
                if (LED_counter == 0)
                {
                    LED_counter = 1;
                    TurnOffRGB();
                }
                else
                {
                    LED_counter = 0;
                    TurnOnRGB();
                }
                if (BlinkingTimeExpire == BLINK_FOREVER)
                {
                    BlinkingCounter = 0;
                }
            }
            BlinkingCounter++;
        }
        else
        {
            StopBlinkingLEDs();
            LED_counter = 0;
        }
    }


    if (StartBlinking_N_Time_LEDsFlag == true)
    {
        //ets_printf("Start Blink n Timer");
        Blinking_N_TimesCounter++;
        if (HowManyBlinksCounter != 0)
        {
            if (!(Blinking_N_TimesCounter%BLNINK_LED_RATE))
            {
                HowManyBlinksCounter--;
                if (LED_counter == 0)
                {
                    LED_counter = 1;
                    TurnOffRGB();
                }
                else
                {
                    LED_counter = 0;
                    TurnOnRGB();
                }
            }
        }
        else
        {
            if (PauseBlinking == false)
            {
                Blinking_N_TimesCounter = 0;
                PauseBlinking = true;
            }
            else
            {
                Blinking_N_TimesCounter++;
                if (Blinking_N_TimesCounter == PASUE_BLINIKING)
                {
                    PauseBlinking = false;
                    LED_counter = 0;
                    HowManyBlinksCounter = StartBlinking_N_Time;
                    if (HowManyTimeBlinking_N_TimesCounter != 0)
                    {
                        HowManyTimeBlinking_N_TimesCounter--;
                    }
                    else
                    {
                        StartBlinking_N_Time_LEDsFlag = false;
                        HowManyTimeBlinking_N_TimesCounter = Blinking_N_Time;
                        StopBlinkingLEDs();
                    }
                }
            }
        }
    }
}



/*==========================   IsLED_Blinking ======================
**    Function description :
**         To check if LED is blinking or not
**
**
**--------------------------------------------------------------------------*/

bool IsLED_Blinking()
{
    return StartBlinkingLEDsFlag;
}

#if defined USE_NETWORK_STATUS_LED_APPROACH
/**************************************************************************************************/
/**SetST_LED
 * @brief sends led control messages to the ST micro
 *
 * This function will check the current state of the esp device info and periodically update the
 * st micro to allow it to properly report on its operation.
 *
 * @return void
 *
 * @exception none
 *
 **************************************************************************************************/
void SetST_LED()
{
	static uint16_t update_st_timer;
	RGB_COLOR LED_color;
	uint8_t interval_s;
	uint8_t time_to_espire_s;
	time_to_espire_s = BLINK_FOREVER;

    update_st_timer++;

	if(update_st_timer >= ST_UPDATE_INTERVAL)
	{
		update_st_timer = 0;
		switch (DeviceInfo.NetworkState)
		{
			case UnProvisioned :
				LED_color = UNPROVISIONED_COLOR;
				interval_s = BLINK_INTERVAL_1S;
				if ( (DeviceInfo.ConnectionStringSaved == false) && (DeviceInfo.DidWeCheckTheConnectionString == true))
				{
					LED_color = NO_CONNECTION_STRING_PROGRAMMED_COLOR;
				}
				break;
			case ProvisionStarted :
				LED_color = PROVISION_STARTED_COLOR;
				interval_s = BLINK_FOREVER;
				break;
			case ProvisionGotIP :
				LED_color = PROVISION_GOT_IP_COLOR;
				interval_s = BLINK_FOREVER;
				break;
			case ProvisionGotSSID :
				LED_color = PROVISION_GOT_SSID_COLOR;
				interval_s = BLINK_FOREVER;
				break;
			//case NotProvisionedNotConnected :
				//state does not seem to be used?
				//LED_color = PROVISION_IOT_NOT_PROVISIONED_NOT_CONNECTED_COLOR;
				//interval_s = BLINK_INTERVAL_1S;
				//break;
			case ProvisionedNotConnected :
				LED_color = PROVISION_IOT_PROVISIONED_NOT_CONNECTED_COLOR;
				interval_s = BLINK_INTERVAL_1S;
				break;
			case ProvisionWrongPassword :
				LED_color = PROVISION_IOT_WRONG_PASSOWRD_COLOR;
				interval_s = BLINK_FOREVER;
				break;
			case Provisioned:
				//if(DeviceInfo.IoTConnected) //determine if we should look for this to turn green and if not what other color to be.
				{
					LED_color = RUNNING_COLOR; //provisioned and connected to our network.
					interval_s = BLINK_INTERVAL_1S;
				}
				if(DeviceInfo.OTA_State != OTA_NON)
				{
					LED_color = OTA_START_COLOR;
					interval_s = BLINK_INTERVAL_1S;
				}
				break;
			default:
				LED_color = UNPROVISIONED_COLOR;
				interval_s = BLINK_INTERVAL_1S;
			break;
		}

		memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
		prepare_uart_command(&gm2m_uart_comm, LED_COLOR_BLINK_RATE, LED_color, interval_s, time_to_espire_s);
	}

    return;
}

#else

/**************************************************************************************************/
/**SetST_LED
 * @brief sends led control messages to the ST micro
 *
 * This function will check the current state of the esp device info and periodically update the
 * st micro to allow it to properly report on its operation.
 *
 * @return void
 *
 * @exception none
 *
 * Note: This driver is a lot clunky as it needs to tie in the existing LED operation to the M2M interface with packet loss.
 * 	     as such it relies on the existing driver to clear the interval and tte when it is not blinking or time has expired.
 * 	     additionally if there are multiple expiring messages in a row with the same interval the other micro cannot know when
 * 	     to clear its blink counter due to potential message loss and as such will rely on the driver in this module to restart
 * 	     the blinking.
 *
 *    I would have preferred to create a separate driver that uses network state however the direction from management was
 *	  to use this existing driver and operation.
 *
 **************************************************************************************************/
void UpdateSTLED(bool force_update)
{
	static uint16_t update_st_timer = 0;
	uint16_t update_interval = ST_STANDARD_UPDATE_INTERVAL;
	if(IsDeviceInProvisioningMode() == true)
	{
		update_interval = ST_FAST_UPDATE_INTERVAL;
	}
	else
	{
		update_interval = ST_STANDARD_UPDATE_INTERVAL;
	}

	update_st_timer++;
	if((update_st_timer >= update_interval) || force_update)
	{
		update_st_timer = 0;
		memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
		if (StartBlinkingLEDsFlag)
		{
			prepare_uart_command(&gm2m_uart_comm, LED_COLOR_BLINK_RATE, RGB_Color, BlinkingTimeInterval, BlinkingTimeExpire);
			//prepare_uart_command(&gm2m_uart_comm, LED_COLOR_BLINK_RATE, RGB_Color, BlinkingTimeInterval, BLINK_FOREVER);
			//ets_printf("\n## LED_COLOR::[%d] payload_2:[%d] payload_3:[%d]\n",RGB_Color, BlinkingTimeInterval, BlinkingTimeExpire);
		}
		else
		{
			prepare_uart_command(&gm2m_uart_comm, LED_COLOR_BLINK_RATE, LED_UNASSIGNED_COLOR, 0, 0);
			//ets_printf("\n## LED_COLOR::[%d] payload_2:[%d] payload_3:[%d]\n",RGB_Color, 0, 0);
		}
	}
}
#endif

/*==========================   SetLED_Level ======================
**    Function description :
**         it handles how the LED_PWM set to
**
**
**--------------------------------------------------------------------------*/

void SetLED_Level()
{


}



/*======================   BlinkLED_N_Times    =================================
**   This function to blink the LED NumberOfBlinks times,
**----------------------------------------------------------------------*/

void  BlinkLED_N_Times (uint8_t NumberOfBlinks)
{
    //ets_printf("BlinkLED_N_Times\n");

    StartBlinking_N_Time_LEDsFlag = true;
    StartBlinking_N_Time = (NumberOfBlinks * 2) + 1;
    TurnOffLed();
    Blinking_N_TimesCounter = 0;
    HowManyBlinksCounter = StartBlinking_N_Time;
    LED_counter = 0;
}


void SetRGB_Color (RGB_COLOR Color)
{

    if ( (RGB_Color == RGB_RED) && (DeviceInfo.OTA_State == OTA_NON))
    {
        //That means we are about reseting , so we will ignore any updated color except if we are in OTA mode
        return;
    }
    
    if (Color < RGB_MAX)
    {
        if ( (DeviceInfo.DeviceInNetworkStatus != DEVICE_IS_IN_A_NETWORK) && (Color == UNPROVISIONED_COLOR))
        {
            switch (DeviceInfo.ProvisionState)
            {
                case UnProvisioned :
                    RGB_Color = UNPROVISIONED_COLOR;
                    if ( (DeviceInfo.ConnectionStringSaved == false) && (DeviceInfo.DidWeCheckTheConnectionString == true))
                    {
                        RGB_Color = NO_CONNECTION_STRING_PROGRAMMED_COLOR;
                    }
                break;
                case ProvisionStarted :
                    RGB_Color = PROVISION_STARTED_COLOR;
                break;
                case ProvisionGotIP :
                    RGB_Color = PROVISION_GOT_IP_COLOR;
                break;
                case ProvisionGotSSID :
                    RGB_Color = PROVISION_GOT_SSID_COLOR;
                break;
                case NotProvisionedNotConnected :
                    RGB_Color = PROVISION_IOT_NOT_PROVISIONED_NOT_CONNECTED_COLOR;
                break;                
                case ProvisionedNotConnected :
                    RGB_Color = PROVISION_IOT_PROVISIONED_NOT_CONNECTED_COLOR;
                break;                
                case ProvisionWrongPassword :
                    RGB_Color = PROVISION_IOT_WRONG_PASSOWRD_COLOR;
                break;

                default:
                	RGB_Color = Color;
                break;
            }                
            
        }
        else
        {
            RGB_Color = Color;
        }
    }
    else
    {
        RGB_Color = DEFAULT_COLOR;
    }
	#if !defined USE_NETWORK_STATUS_LED_APPROACH
    	UpdateSTLED(true);
	#endif
}


void TurnOnRGB(void)
{
//Nitin
//commented to stop the LED connected to ESP32 as on P1, there is
//only 1 RGB connected to STM32
//    gpio_set_level(TEST_PIN_2, HI);

    switch (RGB_Color)
    {
        case RGB_RED :
//            gpio_set_level(RGB_R_PIN_Pin, LOW);
//            gpio_set_level(RGB_G_PIN_Pin, HI);
//            gpio_set_level(RGB_B_PIN_Pin, HI);
        break;

        case RGB_GREEN :
//            gpio_set_level(RGB_R_PIN_Pin, HI);
//            gpio_set_level(RGB_G_PIN_Pin, LOW);
//            gpio_set_level(RGB_B_PIN_Pin, HI);
        break;

        case RGB_BLUE :
//            gpio_set_level(RGB_R_PIN_Pin, HI);
//            gpio_set_level(RGB_G_PIN_Pin, HI);
//            gpio_set_level(RGB_B_PIN_Pin, LOW);
        break;

        case RGB_YELLOW :
//            gpio_set_level(RGB_R_PIN_Pin, LOW);
//            gpio_set_level(RGB_G_PIN_Pin, LOW);
//            gpio_set_level(RGB_B_PIN_Pin, HI);
        break;

        case RGB_CYAN :
//            gpio_set_level(RGB_R_PIN_Pin, HI);
//            gpio_set_level(RGB_G_PIN_Pin, LOW);
//            gpio_set_level(RGB_B_PIN_Pin, LOW);
        break;

        case RGB_MEGENTA :
//            gpio_set_level(RGB_R_PIN_Pin, LOW);
//            gpio_set_level(RGB_G_PIN_Pin, HI);
//            gpio_set_level(RGB_B_PIN_Pin, LOW);
        break;

        case RGB_WHITE :
//            gpio_set_level(RGB_R_PIN_Pin, LOW);
//            gpio_set_level(RGB_G_PIN_Pin, LOW);
//            gpio_set_level(RGB_B_PIN_Pin, LOW);
        break;

        default :
            SetRGB_Color(DEFAULT_COLOR);
    }
}


void TurnOffRGB(void)
{
    gpio_set_level(TEST_PIN_2, LOW);
    gpio_set_level(RGB_R_PIN_Pin, HI);
    gpio_set_level(RGB_G_PIN_Pin, HI);
    gpio_set_level(RGB_B_PIN_Pin, HI);
}


