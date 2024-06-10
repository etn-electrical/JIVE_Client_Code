/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : Breaker
 *
 * File name        : Flash.C
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 05/03/21
 *
 ****************************************************************************/
 
 
 
 
/****************************************************************************
 *                              INCLUDE FILES                               
 ****************************************************************************/
 
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "Breaker.h"
#include "LED_Breaker.h"
//#include "KeyManager.h"
//#include "EventManager.h"

#include "Flash.h"

extern DRAM_ATTR Device_Info_str DeviceInfo;


/*========================== FlashInit   =============================
**    Function description :
**         To Init to Flash
**          
**---------------------------------------------------------------------*/   
esp_err_t FlashInit()
{
	esp_err_t ret;
#if 0
    //Already initiated from Esp32_Station::Init 
    ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());
        /* Retry nvs_flash_init */
        ret |= nvs_flash_init();

    }
    
    if (ret != ESP_OK)
    {
        printf("Failed to init NVS \n\n");
        return ret;
    }    
#endif
    
    

    
    return ESP_OK;
}

/*========================== WriteDataToFlash   =============================
**    Function description :
**         To write data in a specefic location
**         Limitation 200 Bytes 
**          
**---------------------------------------------------------------------*/   
esp_err_t WriteDataToFlash(const char *offeset, const char * String, size_t length )
{
    esp_err_t err;
    nvs_handle_t NVS_Handle;
#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, HI);
#endif
    
    // Rahul:- Commented as log data is greater than 200 bytes
//    if (length >=200)
//    {
//        ets_printf("Error , Trying to write (%s) to the flash and max limit by Ahmed is 200 bytes !\n", length);
//        return;
//    }
    

    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return err;
    } 
    
    err = nvs_set_blob(NVS_Handle, offeset, String, length);
    if (err == ESP_OK)
        nvs_commit(NVS_Handle);
    else
        ets_printf("Error (%s) WriteDataToFlash!\n", esp_err_to_name(err));
    

    // Close
    nvs_close(NVS_Handle);    

#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, LOW);
#endif
    return err;

}

/*========================== ReadDataFromFlash   =============================
**    Function description :
**         To read srting from  a specefic location and return lenth
**          
**---------------------------------------------------------------------*/   


size_t ReadDataFromFlash(const char *offeset, char *out_value)
{
    esp_err_t err;
    nvs_handle_t NVS_Handle;
    
    size_t length = 200;
    
    //ets_printf("ReadDataFromFlash \n");
    
    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return err;
    } 

    err = nvs_get_blob(NVS_Handle, offeset, out_value,&length);
    if (err != ESP_OK)
    {
    	ets_printf("Error (%s) ReadDataFromFlash!\n", esp_err_to_name(err));
    }
    // Close
    nvs_close(NVS_Handle);        

    return length;
}


/*========================== WriteByteToFlash   =============================
**    Function description :
**         To write unsignned char in a specefic location
**          
**---------------------------------------------------------------------*/   
void WriteByteToFlash(const char *offeset, uint8_t value )
{
    esp_err_t err;
    nvs_handle_t NVS_Handle;
    
#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, HI);
#endif
    

    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    } 
    
    err = nvs_set_u8(NVS_Handle, offeset, value);
    if (err == ESP_OK)
        nvs_commit(NVS_Handle);
    else
        ets_printf("Error (%s) WriteByteToFlash!\n", esp_err_to_name(err));
    

    // Close
    nvs_close(NVS_Handle);    

#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, LOW);
#endif

}

/*========================== ReadFlash   =============================
**    Function description :
**         To read unsignned char in a specefic location
**          
**---------------------------------------------------------------------*/   
uint8_t ReadByteFromFlash(const char *offeset)
{
    esp_err_t err;
    uint8_t out_value = 0;
    nvs_handle_t NVS_Handle;

    //ets_printf("ReadByteFromFlash \n");

    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);

    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return 0;
    }

    err = nvs_get_u8(NVS_Handle, offeset, &out_value);

    if (err != ESP_OK)
    {
		ets_printf("Error (%s) in reading uint8_t!\n", esp_err_to_name(err));
		return 0;
    }

    // Close
    nvs_close(NVS_Handle);        

    return out_value;
}



/*========================== WriteIntToFlash   =============================
**    Function description :
**         To write unsigned integer in a specefic location
**
**---------------------------------------------------------------------*/
void WriteIntToFlash(const char *offeset, uint16_t value )
{
    esp_err_t err;
    nvs_handle_t NVS_Handle;

#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, HI);
#endif


    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    err = nvs_set_u16(NVS_Handle, offeset, value);
    if (err == ESP_OK)
        nvs_commit(NVS_Handle);
    else
        ets_printf("Error (%s) WriteWordToFlash!\n", esp_err_to_name(err));


    // Close
    nvs_close(NVS_Handle);

#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, LOW);
#endif

}

/*========================== ReadFlash   =============================
**    Function description :
**         To read unsigned integer in a specefic location
**
**---------------------------------------------------------------------*/
uint16_t ReadIntFromFlash(const char *offeset)
{
    esp_err_t err;
    uint16_t out_value = 0;
    nvs_handle_t NVS_Handle;

    //ets_printf("ReadIntFromFlash \n");

    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return err;
    }

    err = nvs_get_u16(NVS_Handle, offeset, &out_value);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) in reading uint16_t!\n", esp_err_to_name(err));
        return err;
    }

    // Close
    nvs_close(NVS_Handle);

    return out_value;
}



/*========================== WriteWordToFlash   =============================
**    Function description :
**         To write unsigned 32 bit in a specefic location
**
**---------------------------------------------------------------------*/
void WriteWordToFlash(const char *offeset, uint32_t value )
{
    esp_err_t err;
    nvs_handle_t NVS_Handle;

#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, HI);
#endif


    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    err = nvs_set_u32(NVS_Handle, offeset, value);
    if (err == ESP_OK)
        nvs_commit(NVS_Handle);
    else
        ets_printf("Error (%s) WriteWordToFlash!\n", esp_err_to_name(err));


    // Close
    nvs_close(NVS_Handle);

#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, LOW);
#endif

}

/*========================== ReadFlash   =============================
**    Function description :
**         To read unsigned integer in a specefic location
**
**---------------------------------------------------------------------*/
uint32_t ReadWordFromFlash(const char *offeset)
{
    esp_err_t err;
    uint32_t out_value = 0;
    nvs_handle_t NVS_Handle;

    //ets_printf("ReadWordFromFlash \n");

    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return err;
    }

    err = nvs_get_u32(NVS_Handle, offeset, &out_value);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) in reading uint32_t!\n", esp_err_to_name(err));
        return err;
    }

    // Close
    nvs_close(NVS_Handle);

    return out_value;
}




size_t ReadArrayFromFlash(const char *offeset, char *out_value, size_t length)
{
    esp_err_t err;
    nvs_handle_t NVS_Handle;


    //ets_printf("ReadStringFromFlash \n");

    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return err;
    }

    err = nvs_get_blob(NVS_Handle, offeset, out_value,&length);
    if (err != ESP_OK)
    {
    	ets_printf("Error (%s) ReadArrayFromFlash!\n", esp_err_to_name(err));
    }
    // Close
    nvs_close(NVS_Handle);

    return length;
}

/*========================== WriteArrayToFlash   =============================
**    Function description :
**         To write String in a specefic location
**
**
**---------------------------------------------------------------------*/
void WriteArrayToFlash(const char *offeset, const char * String, size_t length )
{
    esp_err_t err;
    nvs_handle_t NVS_Handle;
#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, HI);
#endif



    err = nvs_open("DeviceStorage", NVS_READWRITE, &NVS_Handle);
    if (err != ESP_OK)
    {
        ets_printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    err = nvs_set_blob(NVS_Handle, offeset, String, length);
    if (err == ESP_OK)
        nvs_commit(NVS_Handle);
    else
        ets_printf("Error (%s) WriteArrayToFlash!\n", esp_err_to_name(err));


    // Close
    nvs_close(NVS_Handle);

#ifdef _DEBUG_
    //gpio_set_level(TEST_PIN_2, LOW);
#endif

}
