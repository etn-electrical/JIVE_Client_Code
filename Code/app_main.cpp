/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <stdio.h>

// Application header files
#include "Main.h"
#include "Test_Example.h"
#include "Prod_Spec_LTK_ESP32.h"


#include "Breaker.h"
#include "DeviceGPIO.h"
#include "LED_Breaker.h"


//extern bool OTA_Temp_Flag;








#ifdef __cplusplus
extern "C"
{
#endif
static const char *TAG = "app_main";

TickType_t CurrentTick;


uint8_t TenMsCounter = TEN_MS;

extern DRAM_ATTR Device_Info_str DeviceInfo;

void app_main( void )
{
	//Ahmed
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {
        printf("Device Just Powered Up \n\n");
        printf("FirmWare Ver 0x%x \n\n",MY_FIRMWARE_VERSION );
    }
    
    
    HW_Init();
	// Call to main function
	main();

#ifdef TEST_EXAMPLES
	//Test_Example();
#endif
    while (true)
    {


//        static int flag = 0;
//        if (flag)
//        {
//        	gpio_set_level(TEST_PIN_2, HI);
//        }
//        else
//        {
//        	gpio_set_level(TEST_PIN_2, LOW);
//        }
//        flag^=1;



#if 0
        {
			#define MS_COUNTER 5
        	static int MyCounter = MS_COUNTER;

        	if (MyCounter == 0)
        	{
        		CurrentTick = xTaskGetTickCount();
        		ets_printf(" CurrentTick %d \n", CurrentTick);
        		MyCounter = MS_COUNTER;
        	}
        	else
        	{
        		MyCounter--;
        	}
        }
#endif

        


      
        MainFunction();


        
//        if (OTA_Temp_Flag)
//            ets_printf("MyMainTask\n");
            
        vTaskDelay( 10/portTICK_PERIOD_MS); //10 ms, we can't go lower than that

        //        static int var = 0;
        //        if(!var)
        //        {
        //			printf("############ Stop wifi\n");
        //			esp_wifi_stop();
        //			esp_wifi_deinit();
        //			var = 1;
        //        }

    }


}

#ifdef __cplusplus
}
#endif
