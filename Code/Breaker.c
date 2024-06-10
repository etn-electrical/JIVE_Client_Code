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
#include "esp_efuse.h"
#include "esp_efuse_table.h"

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


#ifdef _TRACK_HEAP_SIZE_
#include "esp_heap_trace.h"

#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS]; // This buffer must be in internal RAM
#define _TRACK_HEAP_SIZE_PER_SECOND_
//#define _TRACK_HEAP_SIZE_PER_50_MS_
//#define _TEST_PSRAM_
#endif


#ifdef _FACTORY_BUILD_
	#undef _DEVICE_HAS_PASSWORD_
#endif

// Uncomment to perform Manufacturing Process Test
//#define MANUFACTURING_PROCESS_TEST

extern bool Prod_Code_Sign_Complete;

#ifdef MANUFACTURING_PROCESS_TEST
    /* When MFG_TEST_FORCE is true
    * The 'Manufacturing Process Test State Machine' 
    * will be forced to run even if DeviceInfo.ConnectionStringSaved 
    * is already saved */
    #define MFG_TEST_FORCE					(true)

    /* When MFG_TEST_SKIP_WIFI is true
    * The 'Manufacturing Process Test State Machine' 
    * will skip the states associated with WiFi Settings */
    #define MFG_TEST_SKIP_WIFI				(false)


    /* When MFG_TEST_DISABLE_FACTORY_FLAG is true 
    * DeviceInfo.DeviceInFactoryMode will **NOT** be set/updated 
    * by the 'Manufacturing Process Test State Machine' */
    #define MFG_TEST_DISABLE_FACTORY_FLAG	(false)

    /* When MFG_TEST_SKIP_RESTORE_RESTART is true
    * The 'Manufacturing Process Test State Machine' 
    * will skip restoring the original Connection string and/or WiFi credentials 
    * as well as the device reboot that would typically accompany this.
    * This can be used for intentionally changing the connection string */
    #define MFG_TEST_SKIP_RESTORE_RESTART	(true)

    /* When MFG_TEST_SET_DPS is true 
    * The hardcoded TEST_DpsConcatenatedMessage will be 
    * loaded when running the manufacturing process test */
    #define MFG_TEST_SET_DPS                (true)

    /* When MFG_TEST_SET_CONNECTION_STRING is true 
    * The hardcoded TEST_ConnectionString will be 
    * loaded when running the manufacturing process test */
    #define MFG_TEST_SET_CONNECTION_STRING  (false)

    /* When MFG_TEST_ADDITIONAL_PRINTS is true 
    * There will be additional ets_printf() calls during the 
    * 'Manufacturing Process Test State Machine' execution 
    * providing more detailed information of what is 
    * happening through the process */
    #define MFG_TEST_ADDITIONAL_PRINTS		(true)
#else
    /* default values if MANUFACTURING_PROCESS_TEST is not defined */
    #define MFG_TEST_FORCE					(false)
    #define MFG_TEST_SKIP_WIFI				(false)
    #define MFG_TEST_DISABLE_FACTORY_FLAG   (false)
    #define MFG_TEST_SKIP_RESTORE_RESTART	(false)
    #define MFG_TEST_SET_DPS                (false)
    #define MFG_TEST_SET_CONNECTION_STRING  (false)
    #define MFG_TEST_ADDITIONAL_PRINTS		(false)
#endif

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

static const char *TAG = "Breaker";
extern uint8_t gStm_Firmware_Version[];//major.minor.patch versions
                            
const char device_Name_Default[MAX_SIZE_DEVICE_NAME_DEFAULT]= {"SmartBreaker-2.0"}; // Don't change the name here and in the excel sheet, that may break OTA

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

    KeyStateMachine();

    Event_10msTimer();
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

#ifndef _FACTORY_BUILD_
	if(!DeviceInfo.bSblcpInit)
	{
		//init SBLCP only when device is connected to IOT hub when Factory mode is disable
		//If factory mode is enable init SBLCP, when device is connected to Wi-Fi even if there is no IoT connection
		//if(get_FactoryModeStatus() == FACTORY_MODE_DISABLE)
		if(DeviceInfo.DeviceInFactoryMode == FACTORY_MODE_DISABLE)
		{
			//init SBLCP only only if connected to wifi router
			if((DeviceInfo.IoTConnected) && (DeviceInfo.WiFiConnected))
			{
				if(init_SBLCP() > 0)
				{
					DeviceInfo.bSblcpInit = true;
				}
			}
		}
		else
		{
			if(DeviceInfo.WiFiConnected)
			{
				if(init_SBLCP() > 0)
				{
					DeviceInfo.bSblcpInit = true;
				}
			}
		}
	}
#endif

#ifdef _TRACK_HEAP_SIZE_PER_50_MS_
     ets_printf(" Heap Size %d Min HeapSize %d \n", esp_get_free_heap_size(),esp_get_minimum_free_heap_size());
#endif
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

#ifdef  _RESET_AFTER_CONNECTION_
	#define RESET_TIME		30

    static uint8_t ResetCounter = 0;


    if ( (ResetCounter == 0) && (DeviceInfo.IoTConnected == true))
    {
    	ResetCounter++;
    }
    if (ResetCounter != 0)
    {
    	ResetCounter++;
    	if (ResetCounter == RESET_TIME)
    	{
            //Manfacutre Reset
           // ResetDevice(MANUFACTURING_RESET, true);
           StartProvisioning();
    	}
    }
#endif    //_RESET_AFTER_CONNECTION_




#ifdef _TRACK_HEAP_SIZE_PER_SECOND_
     ets_printf(" Heap Size %d Min HeapSize %d \n", esp_get_free_heap_size(),esp_get_minimum_free_heap_size());
#endif  

#ifdef _TEST_PSRAM_
    #define SPRAM_BASE_SIZE   1024  //10K
    char *buffer;
    static char i = 10;

    buffer = (char *) heap_caps_malloc(SPRAM_BASE_SIZE * i, MALLOC_CAP_SPIRAM);
    if (buffer != NULL)
    {
        ets_printf("Allocating %dK of SPRAM \n", i);
        ets_printf(" Heap Size %d Min HeapSize %d \n", esp_get_free_heap_size(),esp_get_minimum_free_heap_size());
        free(buffer);
        ets_printf("Freed %dK of SPRAM\n", i);
        i+=10;
        if (i>100)
            i=10;
    }
    else
    {
        ets_printf("Couldn't allocate %d K of SPRAM", i);
    }
#endif




    sec_counter++;

    if (sec_counter == ONE_MINUTE)
    {
        timer_1Min();
        sec_counter = 0;
    }
    ErrorLogOneSecondTimer();

    EventOneSeconedTimer();

    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {
    	PxGreenOneSecondTimer();
    }
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
//    static uint8_t count = 0;
//    float percentLoad;
//    if(count == 20)
//    {
//    	getPercentageLoad(&percentLoad);
//    	printf("percentLoad %f\n", percentLoad);
//    }
//    count++;

//#define DPS_DCI_TEST
//#define INDIVIDUAL_DPS_WRITE_TEST
//#define CONCAT_DPS_WRITE_TEST

#ifdef DPS_DCI_TEST
    static uint8_t StartDpsTestingCounter = 5; // to start testing 2 sec after powering up
    static bool DpsTestIsDone = false;
    static uint8_t StartDpsTestingState = 0;
    if (DpsTestIsDone == false)
    {
        if (StartDpsTestingCounter != 0)
        {
            StartDpsTestingCounter--;
        }
        else
        {
            //start after 5 seconds
            switch (StartDpsTestingState)
            {
                case 0 :
#ifdef INDIVIDUAL_DPS_WRITE_TEST
                    Test_AddingDPSEndpoint();
#endif                    
                    StartDpsTestingState++;                    
                break;

                case 1 :
#ifdef INDIVIDUAL_DPS_WRITE_TEST
                    Test_AddingDPSIDScope();
#endif                    
                    StartDpsTestingState++;                    
                break;

                case 2 :
#ifdef INDIVIDUAL_DPS_WRITE_TEST
                    Test_AddingDPSDeviceRegId();
#endif                    
                    StartDpsTestingState++;                    
                break;

                case 3 :
#ifdef INDIVIDUAL_DPS_WRITE_TEST
                    Test_AddingDPSSymmetricKey();
#endif                    
                    StartDpsTestingState++;                    
                break;

                case 4 :
                    Test_ReadingDPSEndpoint();
                    StartDpsTestingState++;                    
                break;

                case 5 :
                    Test_ReadingDPSIDScope();
                    StartDpsTestingState++;                    
                break;

                case 6 :
                    Test_ReadingDPSDeviceRegId();
                    StartDpsTestingState++;                    
                break;
                
                case 7 :
                    Test_ReadingDPSSymmetricKey();
                    StartDpsTestingState++;                    
                break;

                case 8 :
#ifdef CONCAT_DPS_WRITE_TEST
                    Test_AddingDPSMessage();
#endif                    
                    StartDpsTestingState++;
                break;

                case 9 :
                    Test_ReadingDPSMessage();
                    StartDpsTestingState++;
                break;

                case 10 :
                    Test_AddingWifiSSID();

                    StartDpsTestingState++;
                break;


                case 11 :

                    Test_AddingWifiPWD();
                    StartDpsTestingState++;
                break;

                case 12 :
                    Test_ConnectWiFi();
                    StartDpsTestingState++;
                break;           



                  case 13 :
                    if (DeviceInfo.WiFiConnected)
                    {
                        StartDpsTestingState++;
                    }
                break;              
                case 14: //wait till it connect to Iot
                    if (DeviceInfo.IoTConnected)
                    {
                        //#if MFG_TEST_ADDITIONAL_PRINTS
                            ets_printf("IoT Connected!\n");
                        //#endif
                        StartDpsTestingState++;
                    }
                break;

                case 15 :
                    DpsTestIsDone = true;
                break;
            }
        }
    }

#endif

//    static uint8_t count = 0;
//    float percentLoad;
//    if(count == 20)
//    {
//    	getPercentageLoad(&percentLoad);
//    	printf("percentLoad %f\n", percentLoad);
//    }
//    count++;

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
                    //MFG_TEST_FORCE allows forcing test mode even when ConnectionString is Saved
                    if (DeviceInfo.ConnectionStringSaved && !MFG_TEST_FORCE )
                    {
                        TestIsDone = true;
                    }
                    StartTestingState++;                    
                break;
                
                case 1 :
                    #if !MFG_TEST_DISABLE_FACTORY_FLAG
                    DeviceInfo.DeviceInFactoryMode = DEVICE_IS_IN_FACTORY_MODE;
                    #endif

                    #if MFG_TEST_ADDITIONAL_PRINTS
                        ets_printf("Start factory test \n");
                    #endif
                    //ZeroCrossing_Disable();

                    #if MFG_TEST_SKIP_WIFI
                        StartTestingState = 7;  //force to the state after WiFi testing based on MFG_TEST_SKIP_WIFI 
                    #else
                        wifi_prov_mgr_reset_provisioning();
                    StartTestingState++;
                    #endif
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
                    #if MFG_TEST_SET_DPS
                        Test_AddingDPSMessage();
                        #if MFG_TEST_ADDITIONAL_PRINTS
                            ets_printf("New DPS parameters!\n");
                            ets_printf("    DpsEndpoint:      %s\n", DeviceInfo.DpsEndpoint);
                            ets_printf("    DpsIdScope:       %s\n", DeviceInfo.DpsIdScope);
                            ets_printf("    DeviceId:         %s\n", DeviceInfo.DeviceId);
                            ets_printf("    DpsSymmetricKey:  %s\n", DeviceInfo.DpsSymmetricKey);
                        #endif
                    #endif
                    StartTestingState++;
                break;
                
                case 8 :
                    #if MFG_TEST_SET_CONNECTION_STRING
                    Test_AddingConnectionString();
                        #if MFG_TEST_ADDITIONAL_PRINTS
                            ets_printf("New ConnectionString: %s\n", DeviceInfo.ConnectionString);
                            ets_printf("    HostName:         %s\n", DeviceInfo.HostName);
                            ets_printf("    DeviceId:         %s\n", DeviceInfo.DeviceId);
                            ets_printf("    SharedAccessKey:  %s\n", DeviceInfo.SharedAccessKey);
                        #endif
                    #endif
                    StartTestingState++;
                break;

                
                case 9: //wait till it connect to Iot
                    if (DeviceInfo.IoTConnected)
                    {
                        #if MFG_TEST_ADDITIONAL_PRINTS
                            ets_printf("IoT Connected!\n");
                        #endif
                        StartTestingState++;
                    }
                break;

                case 10: 
                    ets_printf("Writing Build Type \n");
                    Test_WritingBuildType();
                    #if MFG_TEST_ADDITIONAL_PRINTS
                        ets_printf("New BuildType: %s\n", DeviceInfo.BuildType);
                    #endif
                    StartTestingState++;
                break;

                case 11: 
                    ets_printf("Writing New part number \n");
                    Test_WritingNewPartNumber();
                    #if MFG_TEST_ADDITIONAL_PRINTS
                        ets_printf("New NewProgramedPartNumber: %s\n", DeviceInfo.NewProgramedPartNumber);
                    #endif
                    StartTestingState++;
                break;

                case 12: 
                    //ets_printf("Lock The device \n");
                    //DeviceInfo.TheDeviceIsLocked = true;
                   // WriteByteToFlash(EEOFFSET_DEVICE_LOCKED_LABEL,DEVICE_IS_LOCKED );
                    StartTestingState++;
                break;
                
                case 13:
                #if MFG_TEST_SKIP_RESTORE_RESTART
                    #if MFG_TEST_ADDITIONAL_PRINTS
                        ets_printf("Testing Done \n");
                    #endif
                    TestIsDone = true; // Because we are not re-starting the Esp32
                #else
                    if (WaitTimeBeforeRemovingWifiCredential != 0)
                    {
                        WaitTimeBeforeRemovingWifiCredential--;
                        ets_printf("Wait to reset \n");
                    }
                    else
                    {
                        Test_RemoveWifiCredential();
                        GetConnectStringInfo();
                        //ets_printf("Reset \n");
                        //esp_restart();
                        TestIsDone = true; // Just in case if we don't re-start the Esp32
                        StartTestingState++;
                    }
                #endif
                break;
                    
                
            }
        }
    }
    
#endif
    //ets_printf("I am alive \n");
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
    wifi_sta_config_t wifi_cfg;
    uint8_t calibration_init_status = 0;
	eeram_err_t eeram_ret = EERAM_SUCCESS;

//    char ConnectionString[] = "HostName=EatonCRDSWDeviceIothub1.azure-devices.net;DeviceId=28d4e9fe-4cfb-447c-83d4-c234d3b0b755;SharedAccessKey=vBTkva1Z1f6kEBrgX4Sgm+aHE4utqFe9sxbYq4GUiFI=";

    memset (&DeviceInfo,0, sizeof (DeviceInfo));
    
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
#ifndef _DEVICE_HAS_PASSWORD_
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
#else
        DeviceInfo.DeviceInNetworkStatus = DEVICE_IS_IN_A_NETWORK;
        DeviceInfo.ProvisionState = ProvisionSavedInfo;
#endif
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

        const char *default_name = device_Name_Default;

        memcpy ((char *)&DeviceInfo.DeviceName, default_name, sizeof (device_Name_Default));

        WriteDataToFlash(EEOFFSET_DEVICE_NAME_LABEL,DeviceInfo.DeviceName,sizeof(DeviceInfo.DeviceName));

        WriteByteToFlash(EEOFFSET_RE_PROVISION_STARTED_LABEL,DeviceInfo.StartReProvisioning );

        WriteByteToFlash(EEOFFSET_METRO_RESET_LABEL, METRO_RESET_CLEAR);

        WriteByteToFlash(EEOFFSET_EERAM_RESET_LABEL, EERAM_RESET_CLEAR);
 
        WriteByteToFlash(EEOFFSET_MAGIC_LABEL, MAGIC_VALUE);
    }

/************************************************************************/
    //Check if the device is locked or not
    ReadValue = ReadByteFromFlash(EEOFFSET_DEVICE_LOCKED_LABEL);
    if (ReadValue == DEVICE_IS_LOCKED)
    {
        DeviceInfo.TheDeviceIsLocked = true;
    }
    {
        WriteByteToFlash(EEOFFSET_DEVICE_LOCKED_LABEL, DEVICE_IS_NOT_LOCKED );
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
        WriteByteToFlash(EEOFFSET_BUILD_TYPE_SAVED_LABEL,BUILD_TYPE_NOT_SAVED );
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
        WriteByteToFlash(EEOFFSET_PROGRAMED_NEW_PART_NO_SAVED_LABEL,NEW_PROGRAMMED_PART_NO_NOT_SAVED );
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

    /************************************************************************/
    calibration_init_status = ReadByteFromFlash(EEOFFSET_CALIBRATION_INIT_STATUS_LABEL);
	if (calibration_init_status != CALIBRATION_INIT_DONE)
	{
		 Metro_Load_Default_Calibration();
		 WriteByteToFlash(EEOFFSET_CALIBRATION_INIT_STATUS_LABEL, CALIBRATION_INIT_DONE);
	}

	/************************************************************************/

	if ((eeram_ret = EERAM_Init()) != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR;
	}

	if (eeram_ret != EERAM_SUCCESS)
	{
		printf("EERAM init fail\n");
	   	EERAM_Task_Stop_Execution();
	   	EERAM_ReInit(ret);
	   	EERAM_Task_Delete();

	   	DeviceInfo.EERAMInitStatus = false;
	}
	else
	{
		DeviceInfo.EERAMInitStatus = true;
		printf("EERAM Init Success\n");
	}
/****************** Init PRoduction data ********************************************/
	ReadValue = ReadByteFromFlash(EEOFFSET_PROD_DATA_NVS_INIT);
	if(ReadValue != PROD_NVS_INIT_DONE)
	{
		//Code comes here means the device is powered up for the very first time.
		//Code comes here only one time at very first power up.
		//Init the NVS space where production details are stored with
		//default values. Set the Production data NVS space init done in NVS
		SBLCP_enbaleFactoyMode();
		SBLCP_resetProdDetails();
		WriteByteToFlash(EEOFFSET_PROD_DATA_NVS_INIT, PROD_NVS_INIT_DONE);
	}
	else
	{
		// read production details from ESP NVS
		DeviceDetails_st prod_details;
		Pcbdetsils_st pcb_details;
		size_t length;

		set_FactoryModeStatus(ReadByteFromFlash(EEOFFSET_SBLCP_FACTORY_MODE));

		memset(&prod_details, 0, sizeof(DeviceDetails_st));
		length = ReadDataFromFlash(EEOFFSET_SET_PRODUCTION_DETAILS, (char *)&prod_details);
		if(length > 0)
		{
			DeviceInfo.current_rating = prod_details.current_rating;
			DeviceInfo.breaker_test_status = prod_details.breaker_test_status;
			memcpy(&DeviceInfo.catalog_number, prod_details.catalog_number, sizeof(prod_details.catalog_number));
			memcpy(&DeviceInfo.style_number, prod_details.style_number, sizeof(prod_details.style_number));
			memcpy(&DeviceInfo.breaker_serial_no, prod_details.breaker_serial_no, sizeof(prod_details.breaker_serial_no));
		}
		uint32_t Communication_FW_Version = MY_FIRMWARE_VERSION;
		sprintf(DeviceInfo.ESP_firmware_ver, "%x", Communication_FW_Version);

		memset(&pcb_details, 0, sizeof(Pcbdetsils_st));
		length = ReadDataFromFlash(EEOFFSET_SET_PCB_DETAILS, (char *)&pcb_details);
		if(length > 0)
		{
			DeviceInfo.PCBA_test_status = pcb_details.PCBA_test_status;
			DeviceInfo.PCB_hardware_ver = pcb_details.PCB_hardware_ver;
			memcpy(&DeviceInfo.PCB_serial_number, pcb_details.PCB_serial_number, sizeof(pcb_details.PCB_serial_number));
		}

#if SBLCP_DEBUG
		printf("\n");
		printf("Current Rating:-[%d]\n", DeviceInfo.current_rating);
		printf("breaker_test_status:-[%d]\n", DeviceInfo.breaker_test_status);
		printf("catalog_number:-[%s]\n", DeviceInfo.catalog_number);
		printf("style_number:-[%s]\n", DeviceInfo.style_number);
		printf("breaker_serial_no:-[%s]\n", DeviceInfo.breaker_serial_no);
		printf("ESP_firmware_ver:-[%s]\n", DeviceInfo.ESP_firmware_ver);
		printf("STM_firmware_ver:-[%s]\n", DeviceInfo.STM_firmware_ver);

		printf("PCBA_test_status:-[%d]\n", DeviceInfo.PCBA_test_status);
		printf("PCB_hardware_ver:-[%d]\n", DeviceInfo.PCB_hardware_ver);
		printf("PCB_serial_number:-[%s]\n", DeviceInfo.PCB_serial_number);
		printf("\n");
#endif
	}
    #ifdef _KEEP_REPORVISINING_EVERY_TIME_POWERING_UP_
        if (DeviceInfo.DeviceInNetworkStatus == DEVICE_IS_IN_A_NETWORK)
        {
            wifi_prov_mgr_reset_provisioning();
        }
        ets_printf("RESET REMOVE CREDINTIAL \n");
        DeviceInfo.DeviceInNetworkStatus = DEVICE_IS_NOT_IN_A_NETWORK;
        DeviceInfo.WiFiConnected = false;
        WriteByteToFlash(EEOFFSET_PROVISION_LABEL,DeviceInfo.DeviceInNetworkStatus );

    #endif
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
#ifndef _ESP32_MINI_1_
    SetGPIO_InPut(SWITCH_ON_OFF_PIN_Pin);
#endif


//    SetGPIO_OutPut(RGB_R_PIN_Pin);
//    SetGPIO_OutPut(RGB_G_PIN_Pin);
    
    SetGPIO_OutPut(BOARD_PIN_I2C_HS);


//    SetGPIO_OutPut(TEST_PIN_2);

    SetGPIO_OutPut(BOARD_PIN_CS);
    SetGPIO_OutPut(BOARD_PIN_RST);
    SetGPIO_OutPut(BOARD_PIN_TEST);
    SetGPIO_OutPut(BOARD_PIN_TEST1);
    //SetGPIO_OutPut(BOARD_PIN_TEST2);
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

	uint8_t eth_mac[6] = {0};
	metro_err_t ret = METRO_SUCCESS;

    //This fucntion should be called one time after one second of powering up
    // To avoid a lot of issues releated to reseting while writing to EE
    // This time will be enough for Px green to intialize, writing cert and other tasks
    
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {    
        //ets_printf("ConnectionString: %s\n",  DeviceInfo.ConnectionString);
    	ets_printf("HostName: %s \n",  DeviceInfo.HostName);
    	ets_printf("DeviceId: %s\n", DeviceInfo.DeviceId);

                            ets_printf("New DPS parameters!\n");
                            ets_printf("    DpsEndpoint:      %s\n", DeviceInfo.DpsEndpoint);
                            ets_printf("    DpsIdScope:       %s\n", DeviceInfo.DpsIdScope);
    	                    ets_printf("DeviceId: %s\n", DeviceInfo.DeviceId);
                            ets_printf("    DpsSymmetricKey:  %s\n", DeviceInfo.DpsSymmetricKey);
                            ets_printf("New ConnectionString: %s\n", DeviceInfo.ConnectionString);
                            ets_printf("    HostName:         %s\n", DeviceInfo.HostName);
                            ets_printf("    DeviceId:         %s\n", DeviceInfo.DeviceId);
                            ets_printf("    SharedAccessKey:  %s\n", DeviceInfo.SharedAccessKey);                            


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
	metro_err_t ret = METRO_SUCCESS;

	//Initialize Metrology
	if (DeviceInfo.EERAMInitStatus)
	{
		if ((ret = Metro_Init()) != METRO_SUCCESS)
		{
			ets_printf("Metro Init fail\n");
		}
		else
		{
			ets_printf("Metro Init success\n");
			if ((ret = Metro_GPIO_IRQ_Init()) != METRO_SUCCESS)
			{
				ets_printf("IRQ Init fail\n");
			}
			else
			{
				ets_printf("IRQ Init success\n");
				DeviceInfo.MetroInitStatus = true;
			}
		}
	}

	if (ret != METRO_SUCCESS)
	{
		Metro_Task_Stop_Execution();
		Metro_ReInit(ret);
		Metro_Task_Delete();
	}

    DisableSwitchs();

    KeyStateMachineInit();

    // reset timer countet
    ms_counter = 0;

	BreakerDCIInit();

	GetConnectStringInfo();
	GetDPSConcatenatedMessageFromIndividualFields();

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

#ifdef AHMED_ADD_CONNECTION_STRING_
   // if (DeviceInfo.ConnectionStringSaved == false)
    {
    	Test_AddingConnectionString();
    }

#endif

#ifdef  _DELETE_CONNECTION_STRING_TO_TEST_DPS_
    if (DeviceInfo.ConnectionStringSaved == false)
    {
        ets_printf("Deleting Connection String \n");
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

	// https://esp32developer.com/programming-in-c-c/timing/hardware-timers

    uint32_t LocalTickTimer;

    ms_counter++;

    timer_10ms();
	if (!(ms_counter%FIFTY_MS))
	{
			timer_50ms();
	}
	if (!(ms_counter%ONE_SECOND))
	{
		timer_1Sec();
		ms_counter = 0;
	}

	if (Prod_Code_Sign_Complete)
	{
		Prod_Code_Sign_Complete= false;
		InitZeroCrossingInterrupt();
	}
}



void ResetDevice(uint8_t ResetType, uint8_t Reset)
{
    
    switch (ResetType)
    {
        // Please notice, no break is needed in this switch case        
        case MANUFACTURING_RESET :
            WriteByteToFlash(EEOFFSET_MAGIC_LABEL,0x00 );
            WriteByteToFlash(EEOFFSET_EERAM_INIT_LABEL, 0x00);

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

int lockSB2Device()
{
	//TODO: remove below line for production build
	esp_err_t err = ESP_FAIL;
	ets_printf("Locking ESP32 \n");
#if 0
	esp_err_t err = ESP_OK;
	if (get_statusOfBuildType() == true)
	{

		if (strcmp (DeviceInfo.BuildType,"Development")!= 0)
		{
			// Lock the device if we are not Development mode
			uint8_t jtag;
			ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_DISABLE_JTAG, &jtag, 1));
			ESP_LOGI(TAG, "2. jtag = %d", jtag);
			if(jtag == 0)
			{
			   jtag = 1;
			   ESP_ERROR_CHECK(esp_efuse_write_field_blob(ESP_EFUSE_DISABLE_JTAG, &jtag, 1));   //disable the JTAG
			}

			ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_DISABLE_JTAG, &jtag, 1));
			ESP_LOGI(TAG, "2. jtag = %d", jtag);
			if(jtag == 0)
			{
				err = ESP_FAIL;
			}
			else
			{
				uint8_t uart_download;
				ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_UART_DOWNLOAD_DIS, &uart_download, 1));
				ESP_LOGI(TAG, "2. uart_download = %d", uart_download);

				if(uart_download == 0)
				{
				   uart_download = 1 ;
				   ESP_ERROR_CHECK(esp_efuse_write_field_blob(ESP_EFUSE_UART_DOWNLOAD_DIS, &uart_download, 1));  //disable the UART DOWNLOAD
				}
				ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_UART_DOWNLOAD_DIS, &uart_download, 1));
				ESP_LOGI(TAG, "2. uart_download = %d", uart_download);
				if(uart_download == 0)
				{
					err = ESP_FAIL;
				}
				else
				{
					  //  ESP_LOGI(TAG, "Done");
						DeviceInfo.TheDeviceIsLocked = true;
						WriteByteToFlash(EEOFFSET_DEVICE_LOCKED_LABEL,DEVICE_IS_LOCKED );
						err = ESP_OK;
				}
			}

		}
		else
		{
			printf("build type is development \n");
			err = ESP_FAIL;
		}
	}
	else
	{
		printf("build type is not programmed\n");
		err = ESP_FAIL;
	}
#endif
	return err;
}

            
/**
 * @brief      This function returns current task state
 * @param[in]  TaskHandle_t task_handle
 * @retval     eTaskState Task state
*/
eTaskState Get_Task_State(TaskHandle_t task_handle)
{
	eTaskState task_state = 0;

	if (task_handle != NULL)
	{
		task_state = eTaskGetState(task_handle);
	}

	return task_state;
}
