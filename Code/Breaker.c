/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : Breaker.c
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/18/2022
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
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "esp_log.h"

#include <string.h>

#include "freertos/task.h"
#include "driver/ledc.h"
#include "hal/gpio_ll.h"
#include "hal/ledc_ll.h"
#include "hal/timer_ll.h"
#include "hal/mcpwm_ll.h"
#include "freertos/event_groups.h"

#include <wifi_provisioning/manager.h>

#include <esp_log.h>

#include "driver/timer.h"
#include "driver/periph_ctrl.h"
#include "hal/timer_hal.h"

#include "esp_task_wdt.h"

#include "Breaker.h"

#include "Prod_Spec_LTK_ESP32.h"


#include "LED_Breaker.h"
#include "EventManager.h"
#include "KeyManager.h"
#include "DeviceGPIO.h"
#include "Flash.h"
#include "SBLCP/sblcp.h"
#include "Log.h"
#include "ErrorLog.h"

//#define MANUFACTURING_PROCESS_TEST


/****************************************************************************
 *                              Global variables
 ****************************************************************************/
extern uint8_t OneHourCounter;
DRAM_ATTR Device_Info_str DeviceInfo;
extern bool Prod_Code_Sign_Complete;
extern ErrorLogStrArray_str ErrLogStr;
uint16_t ms_counter        = 0;
uint16_t sec_counter       = 0;

bool DisableKeyPress         = true;


bool ExtIntEnableFlag        = false;

bool RefreshWDG_Flag         = false;


bool HeartBeatTimer10ms      = false;



bool ServingZC_ISR           = false;


uint64_t PreviousFlashDataValue = 0;

uint32_t ZeroCrossingTickTimer;
bool     ZeroCrossingTickflag = false;


static const char *TAG = "Breaker";

                            
                                            
#ifdef _DEBUG_
    char RingBuffer [RING_BUFFER_ARRAY_SIZE][RING_BUFFER_ARRAY_TEXT_LENGHT];
    uint8_t RingBufferRead;
    uint8_t RingBufferWrite;
    bool    RingBufferFull = false;
    uint8_t RingBufferNoOfRecords = 0;
    bool    RingBufferSendInfoFlag = false;
#endif



#define TimeToReset THREE_SECONDS
#define ManfTimeToReset FIVE_SECONDS
uint8_t TimeToResetflag = false;
uint8_t TimeToResetCounter = 0;

#if 0
#define TWDT_TIMEOUT_S          10
#define TASK_RESET_PERIOD_S     2

static TaskHandle_t task_handles[portNUM_PROCESSORS];    
static bool ResetWD_Flag = false;

/*
 * Macro to check the outputs of TWDT functions and trigger an abort if an
 * incorrect code is returned.
 */
#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected){                                  \
                printf("TWDT ERROR\n");                                \
                abort();                                               \
            }                                                          \
})


//Callback for user tasks created in app_main()
void reset_task(void *arg)
{
    //Subscribe this task to TWDT, then check if it is subscribed
    CHECK_ERROR_CODE(esp_task_wdt_add(NULL), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(NULL), ESP_OK);

    while(1){
        //reset the watchdog every TASK_RESET_PERIOD_S seconds
    	//printf("reset the watchdog\n");
        if (ResetWD_Flag == true)
        {
            // to be sure that the main  task is running
            ResetWD_Flag = false;
            CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);  //Comment this line to trigger a TWDT timeout
        }
        vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_S * 1000));
    }
}
#endif

/*=====================================   timer_10ms  ======================
**    Function description :
**          10 ms timer called from ISR
**
**
**--------------------------------------------------------------------------*/

void timer_10ms(void)
{
    static uint8_t lightlevel;
    static uint8_t Prevlightlevel = 10;

#if 0
    ResetWD_Flag = true;
#endif
    
    //HAL_GPIO_TogglePin(TEST_3_PIN_GPIO_Port, TEST_3_PIN_Pin);
    HeartBeatTimer10ms = true;
    //this is the heart beat and here we should put the watchdog
    RefreshWDG_Flag = true;
    Event_10msTimer();

    KeyStateMachine();
}


/*=====================================   timer_50ms  ======================
**    Function description :
**          50 ms timer called from main loop
**
**
**--------------------------------------------------------------------------*/

void timer_50ms(void)
{

    BlinkingLEDStateMachine();

    EventStateMachine();

    Event_50msTimer();
    
    
}


/*=====================================   timer_1Sec ======================
**    Function description :
**          1 Seconed timer called from the main loop
**
**
**--------------------------------------------------------------------------*/

void timer_1Sec(void)
{
    uint32_t Address;

//#ifdef _DEBUG_
//    static uint32_t SecondsSincePoweringUp = 0;
//    if (!(SecondsSincePoweringUp%5))
//        ACK_DEBUG_PRINT_E("Seconds since power up %u", SecondsSincePoweringUp);
//    SecondsSincePoweringUp++;
//#endif

    sec_counter++;

    if (sec_counter == ONE_MINUTE)
    {
        timer_1Min();
        sec_counter = 0;
    }
    ErrorLogOneSecondTimer();


    //init SBLCP only only if connected to wifi router
	if(!DeviceInfo.bSblcpInit && DeviceInfo.WiFiConnected)
	{
		if(init_SBLCP() > 0)
		{
			DeviceInfo.bSblcpInit = true;
		}
	}

    EventOneSeconedTimer();

    
    PxGreenOneSecondTimer();
    ProvisionEvent();
    
    if (TimeToResetflag)
    {
        if (TimeToResetCounter != 0)
        {
            TimeToResetCounter--;
        }
        else
        {
            esp_restart();
        }
    }

#ifdef MANUFACTURING_PROCESS_TEST    
    static uint8_t StartTestingCounter = 5; // to start testing after 5 seconds from powring up
    static bool TestIsDone = false;
    static uint8_t StartTestingState = 0;
    
    static uint8_t WaitTimeBeforeRemovingWifiCredential = 10; 
    static uint8_t WaitTimeBeforeAddingConnectionString = 4;
    

    
    if (TestIsDone == false)
    {
        if (StartTestingCounter != 0)
        {
            StartTestingCounter--;
        }
        else
        {
            //start after 5 seconds
            switch (StartTestingState)
            {
                case 0 :
                    if (DeviceInfo.ConnectionStringSaved)
                    {
                        TestIsDone = true;
                    }
                    StartTestingState++;                    
                break;
                
                case 1 :
                    DeviceInfo.DeviceInFactoryMode = DEVICE_IS_IN_FACTORY_MODE;
                    //wifi_prov_mgr_reset_provisioning();
                    StartTestingState++;
                break;  
                

                case 2 :
                    Test_AddingWifiSSID();
                    DCI_Update_SSID();
                    GetDeviceSsid();
                    StartTestingState++;
                break;  
                
                case 3 :
                    Test_AddingWifiPWD();
                    DCI_Update_DevicePassWord();
                    GetDevicePassWord();
                    StartTestingState++;
                break;       

                case 4 :
                    Test_ConnectWiFi();
                    StartTestingState++;
                break;       

                case 5 :
                    if (DeviceInfo.WiFiConnected)
                    {
                        StartTestingState++;
                    }
                break;
                    
                case 6 :
                    
                    if (WaitTimeBeforeAddingConnectionString != 0)
                    {
                        WaitTimeBeforeAddingConnectionString--;
                        ets_printf("WiFi is Connected Wait to add Connection string \n");
                    }
                    else
                    {
                        StartTestingState++;
                    }
                break;
                
                case 7 :
                    StartTestingState++;
                    Test_AddingConnectionString();
                break;

                
                case 8: //wait till it connect to Iot
                    if (DeviceInfo.IoTConnected)
                    {
                        StartTestingState++;
                    }
                break;

                case 9: 
                    ets_printf("Writing Build Type \n");
                    Test_WritingBuildType();
                    StartTestingState++;
                break;

                case 10: 
                    ets_printf("Writing New part number \n");
                    Test_WritingNewPartNumber();
                    StartTestingState++;
                break;

                case 11: 
                    //ets_printf("Lock The device \n");
                    //DeviceInfo.TheDeviceIsLocked = true;
                   // WriteByteToFlash(EEOFFSET_DEVICE_LOCKED_LABEL,DEVICE_IS_LOCKED );
                    StartTestingState++;
                break;
                
                case 12:
                    //if (WaitTimeBeforeRemovingWifiCredential != 0)
                    //{
                    //    WaitTimeBeforeRemovingWifiCredential--;
                    //    ets_printf("Wait to reset \n");
                    //}
                   // else
                    {
                        //Test_RemoveWifiCredential();
                        GetConnectStringInfo();
                        //ets_printf("Reset \n");
                        //esp_restart();
                        TestIsDone = true; // Just in case if we don't re-start the Esp32
                        StartTestingState++;
                    }
                break;
                    
                
            }
        }
    }
    
#endif
}

/*=====================================   timer_1Min ======================
**    Function description :
**          1 minute timer called from timer_1Sec
**
**
**--------------------------------------------------------------------------*/
void timer_1Min(void)
{
    static uint16_t minute = 0;

    minute++;
    //ACK_DEBUG_PRINT_E("Minutes since power up %u", minute);
    Event_1MinuteTimer();
    
}





 /*=================================== InitEE   =============================
**    Function description :
**         FLASH initialization
**
**--------------------------------------------------------------------------*/

void InitEE()
{
    uint8_t  ReadValue;
    uint8_t  lightlevel;
    uint32_t Address;
	uint8_t energy_reset_status = 0;
    wifi_sta_config_t wifi_cfg;
//    char ConnectionString[] = "HostName=EatonCRDSWDeviceIothub1.azure-devices.net;DeviceId=28d4e9fe-4cfb-447c-83d4-c234d3b0b755;SharedAccessKey=vBTkva1Z1f6kEBrgX4Sgm+aHE4utqFe9sxbYq4GUiFI=";
    
    memset (&DeviceInfo,0, sizeof (DeviceInfo));
    
    Metro_Data_Reset();	//reset metrology data and cloud data structure

    /* Initialize NVS partition */
    esp_err_t ret = FlashInit();

    InitiateErrLog();

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init Flash");
        return;
    }

    ReadValue = ReadByteFromFlash(EEOFFSET_MAGIC_LABEL);
    if (ReadValue == MAGIC_VALUE)
    {
        
        ReadValue = ReadByteFromFlash(EEOFFSET_PROVISION_LABEL);
        if (ReadValue == DEVICE_IS_IN_A_NETWORK)
        {
            DeviceInfo.DeviceInNetworkStatus = DEVICE_IS_IN_A_NETWORK;
            DeviceInfo.ProvisionState = ProvisionSavedInfo;
        }
        else
        {
            ResetDevice(PROVISIONING_RESET, false);
        }
        
        ReadDataFromFlash(EEOFFSET_DEVICE_NAME_LABEL, (char *)&DeviceInfo.DeviceName);

        ReadValue = ReadByteFromFlash(EEOFFSET_RE_PROVISION_STARTED_LABEL);
        if (ReadValue != RE_START_PROVISIONING)
        {
            ReadValue = 0;
        }
        else
        {
            ets_printf("EE Restart re-provisioning\n");
        }
        DeviceInfo.StartReProvisioning = ReadValue;
        //Reset re-provisioning
        WriteByteToFlash(EEOFFSET_RE_PROVISION_STARTED_LABEL,0 );
        
    }
    else
    {
     
        ResetDevice(PROVISIONING_RESET, false);
        
        WriteByteToFlash(EEOFFSET_DEVICE_RESET_MODE_LABEL,DEVICE_RESET_STATE_OFF );
        
        DeviceInfo.DeviceInNetworkStatus = DEVICE_IS_NOT_IN_A_NETWORK;
        WriteByteToFlash(EEOFFSET_PROVISION_LABEL,DeviceInfo.DeviceInNetworkStatus );    

        const char *p = (char *)&DEVICE_NAME_DEFAULT;
        memcpy ((char *)&DeviceInfo.DeviceName, p,sizeof (DeviceInfo.DeviceName));
        WriteDataToFlash(EEOFFSET_DEVICE_NAME_LABEL,DeviceInfo.DeviceName,sizeof(DeviceInfo.DeviceName));

        WriteByteToFlash(EEOFFSET_RE_PROVISION_STARTED_LABEL,DeviceInfo.StartReProvisioning );

        WriteByteToFlash(EEOFFSET_METRO_RESET_LABEL, METRO_RESET_CLEAR);
 
        WriteByteToFlash(EEOFFSET_MAGIC_LABEL,MAGIC_VALUE );

        Metro_Load_Default_Calibration();
    }

/*******************************************************************/
	energy_reset_status = ReadByteFromFlash(EEOFFSET_ENERGY_RESET_LABEL);
	if(energy_reset_status == UPDATE_ENERGY)
	{
		read_PhA_frwrd_energy();
		read_PhA_reverse_energy();
		read_PhB_frwrd_energy();
		read_PhB_reverse_energy();
	}
	else
	{
		reset_energy();
		//Metro calibration and configuration are not yet finalized
		//setting NVS with default values which will be changed once Calibration and configuration is finalized
		setDefaultMetroCalibConfig();
		WriteByteToFlash(EEOFFSET_ENERGY_RESET_LABEL, UPDATE_ENERGY);
	}
/************************************************************************/
    //Check if the device is locked or not
    ReadValue = ReadByteFromFlash(EEOFFSET_DEVICE_LOCKED_LABEL);
    if (ReadValue == DEVICE_IS_LOCKED)
    {
        DeviceInfo.TheDeviceIsLocked = true;
    }

    //Check if this build is a special build
    ReadValue = ReadByteFromFlash(EEOFFSET_BUILD_TYPE_SAVED_LABEL);
    if (ReadValue == BUILD_TYPE_SAVED)
    {
        ReadDataFromFlash(EEOFFSET_BUILD_TYPE_LABEL, (char *)&DeviceInfo.BuildType);
        if (strcmp (DeviceInfo.BuildType,"Development")== 0)
        {
            ets_printf("Build is Development \n");
        }
        else
        {
            ets_printf("Build is: %s \n",  DeviceInfo.BuildType);
        }

    }
    else
    {
        WriteDataToFlash(EEOFFSET_BUILD_TYPE_LABEL,DeviceInfo.BuildType,sizeof(DeviceInfo.BuildType));
    }


    //Check if there is special part number
    ReadValue = ReadByteFromFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_SAVED_LABEL);
    if (ReadValue == NEW_PROGRAMMED_PART_NO_SAVED)
    {
        ReadDataFromFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_LABEL, (char *)&DeviceInfo.NewProgramedPartNumber);
    }
    else
    {
        WriteDataToFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_LABEL,DeviceInfo.NewProgramedPartNumber,sizeof(DeviceInfo.NewProgramedPartNumber));
    }

    //temporary change for init of SBLCP
    DeviceInfo.bSblcpInit = false;
    //Initiate WiFi configuration

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    if (esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_cfg) == ESP_OK)
    {
       //This is to aviod a lot of issues like slow wi-fi
        esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    }
}

/*=================================== HW_Init   =============================
**    Function description :
**         Hardware initialization
**
**--------------------------------------------------------------------------*/

void HW_Init(void)
{
//	ets_printf("HW init\n");
    //Inputs
    SetGPIO_InPut(SWITCH_ON_OFF_PIN_Pin);


    SetGPIO_OutPut(RGB_R_PIN_Pin);
    SetGPIO_OutPut(RGB_G_PIN_Pin);
    SetGPIO_OutPut(RGB_B_PIN_Pin);
    
    SetGPIO_OutPut(TEST_PIN_2);

    SetGPIO_OutPut(BOARD_PIN_CS);
    SetGPIO_OutPut(BOARD_PIN_RST);
    SetGPIO_OutPut(BOARD_PIN_TEST);
    SetGPIO_OutPut(BOARD_PIN_TEST1);
    SetGPIO_OutPut(BOARD_PIN_TEST2);
    SetGPIO_ExtInterrupt(BOARD_PIN_CF3_ZA, GPIO_INTR_POSEDGE);
    SetGPIO_ExtInterrupt(BOARD_PIN_IRQ1, GPIO_INTR_NEGEDGE);
}

/*================== InitZeroCrossingInterrupt   =============================
**    Function description :
**         This function will initiate Zero crossing, work around and 3-way
**
**--------------------------------------------------------------------------*/
void InitZeroCrossingInterrupt(void)
{

	uint8_t eth_mac[6];
	int8_t ret = METRO_SUCCESS;
    //This fucntion should be called one time after one second of powering up
    // To avoid a lot of issues releated to reseting while writing to EE
    // This time will be enough for Px green to intialize, writing cert and other tasks
    
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {    
    	//ets_printf("HostName: %s \n",  DeviceInfo.HostName);
    	ets_printf("DeviceId: %s\n", DeviceInfo.DeviceId);
    	//ets_printf("SharedAccessKey: %s\n",  DeviceInfo.SharedAccessKey);
    }
    
    //Nitin : not required to send secondary on at power up
    //TurnBreakerOn();

        
    //EnableSwitchs(); 

#if 0    
    //printf("Initialize TWDT\n");
    //Initialize or reinitialize TWDT
    CHECK_ERROR_CODE(esp_task_wdt_init(TWDT_TIMEOUT_S, true), ESP_OK);
    xTaskCreate( reset_task, "reset", 2048, NULL, 10, NULL );
#endif   
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf((char *)&DeviceInfo.MacAddress, sizeof (DeviceInfo.MacAddress), "%02X:%02X:%02X:%02X:%02X:%02X",
             eth_mac[0], eth_mac[1], eth_mac[2],eth_mac[3],eth_mac[4],eth_mac[5]);

    if (DeviceInfo.MetroInitStatus)
	{
    	if ((ret = Metro_GPIO_IRQ_Init()) != METRO_SUCCESS)
    	{
    		ets_printf("IRQ Init fail\n");
    		Metro_Task_Stop_Execution();
    		Metro_Task_ReInit();
    		Metro_Task_Delete();

    	}
    	else
    	{
    		ets_printf("IRQ Init success\n");
    	}
	}


    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {
        ets_printf("ZeroCrossingIsInitiated \n");
        ets_printf("MacAddress %s\n",DeviceInfo.MacAddress);
    } 

#ifdef _AUTOMATIC_PROVISIONIG_NO_BUTTONS_OR_RESET_
    if (DeviceInfo.StartReProvisioning == RE_START_PROVISIONING)
    {
        //that means the device was provisioned and we re-provisining it again
        // reset reprovisiong flag and start provisining
        StartProvisioning();
    }
    else
    {
        if (DeviceInfo.DeviceInNetworkStatus != DEVICE_IS_IN_A_NETWORK)
        {    
            //start provisiong
            StartProvisioning();
        }

    }
#else    

    if (DeviceInfo.StartReProvisioning == RE_START_PROVISIONING)
    {
        //that means the device was provisioned and we re-provisining it again
    	BlinkLEDs(BLINK_250_mS,BLINK_FOREVER,LED_BLINK);
    	SetRGB_Color(PROVISION_STARTED_COLOR);
        StartProvisioning();
    }
#endif
}    


/*=================================== SW_Init   =============================
**    Function description :
**         Software initialization
**
**--------------------------------------------------------------------------*/

void SW_Init(void)
{
    uint8_t ret = METRO_SUCCESS;

    DisableSwitchs();


    KeyStateMachineInit();

    // reset timer countet
    ms_counter = 0;

    BreakerDCIInit();

    GetConnectStringInfo();

    //initiate LED structure
    LED_Init();    
    
    if (DeviceInfo.TheDeviceIsLocked == false)
    {
        initial_Uart();
    }
    else
    {
        initial_Uart();
        periph_module_disable(PERIPH_UART0_MODULE); //disable Uart0
    }

    	//Initialize Metrology

    if ((ret = Metro_Init()) != METRO_SUCCESS)
    {
    	ets_printf("Metro Init fail\n");
    	Metro_Task_Stop_Execution();
    	Metro_Task_ReInit();

    }
    else
    {
    	DeviceInfo.MetroInitStatus = true;
    	ets_printf("Metro Init success\n");
    }

#ifdef _AHMED_ADD_CONNECTION_STRING_
    if (DeviceInfo.ConnectionStringSaved == false)
    {
    	Test_AddingConnectionString();
    }

#endif
}




/*========================== RefreshWatchDog   =============================
**    Function description :
**         To refresh watchdog0
**
**--------------------------------------------------------------------------*/

void RefreshWatchDog()
{
#ifndef _DEBUG_
    //HAL_IWDG_Refresh(&hiwdg);
#endif
}
/*=================================== MainFunction   =============================
**    Function description :
**         Software main function
**
**--------------------------------------------------------------------------*/

void MainFunction()
{
//
//	// https://esp32developer.com/programming-in-c-c/timing/hardware-timers
//
//    uint32_t LocalTickTimer;
//
//    ms_counter++;
//
//    timer_10ms();
//	if (!(ms_counter%FIFTY_MS))
//	{
//			timer_50ms();
//	}
//	if (!(ms_counter%ONE_SECOND))
//	{
//		timer_1Sec();
//		ms_counter = 0;
//	}
//
//	if (Prod_Code_Sign_Complete)
//	{
//		Prod_Code_Sign_Complete= false;
//		InitZeroCrossingInterrupt();
//	}
}



void ResetDevice(uint8_t ResetType, uint8_t Reset)
{
    
    switch (ResetType)
    {
        // Please notice, no break is needed in this switch case        
        case MANUFACTURING_RESET :
            WriteByteToFlash(EEOFFSET_MAGIC_LABEL,0x00 );
            WriteByteToFlash(EEOFFSET_ENERGY_RESET_LABEL, RESET_ENERGY);

        case PROVISIONING_RESET :
            if ((DeviceInfo.DeviceInNetworkStatus == DEVICE_IS_IN_A_NETWORK) || (DeviceInfo.WiFiConnected == true))
            {
                wifi_prov_mgr_reset_provisioning();
            }
            //ets_printf("RESET REMOVE CREDINTIAL \n");
            DeviceInfo.DeviceInNetworkStatus = DEVICE_IS_NOT_IN_A_NETWORK;
            DeviceInfo.WiFiConnected = false;
            WriteByteToFlash(EEOFFSET_PROVISION_LABEL,DeviceInfo.DeviceInNetworkStatus );
           
        break;
        default:
        	ets_printf("RESET NOT REMOVING CREDINTIAL \n");
        break;

    }
    

    if (Reset)
    {
        TimeToResetflag = true;
        BlinkLEDs(BLINK_250_mS,BLINK_FOREVER,LED_BLINK);
        SetRGB_Color(RESET_COLOR);
        ets_printf("RESET \n");
        if (ResetType == DEVICE_RESET)
        {
            TimeToResetCounter = ManfTimeToReset;
        }
        else
        {
            TimeToResetCounter = TimeToReset;
        }
    
    }
    else
    {
        BlinkLEDs(BLINK_250_mS,BLINK_FOREVER,LED_BLINK);
        SetRGB_Color(UNPROVISIONED_COLOR);
        if ( (DeviceInfo.ConnectionStringSaved == false) && (DeviceInfo.DidWeCheckTheConnectionString == true))
        {
          SetRGB_Color(NO_CONNECTION_STRING_PROGRAMMED_COLOR);
        }
    }
}    

            
    
