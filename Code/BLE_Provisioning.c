/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : BLE_Provisioning.c
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/10/2022
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

#include <string.h>


#include "event_groups.h"
#include "esp_log.h"
#include "esp_sntp.h"

#include <stddef.h>
#include "os/os.h"
#include "sysinit/sysinit.h"
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "esp_nimble_hci.h"
#include "esp_nimble_mem.h"
#if NIMBLE_CFG_CONTROLLER
#include "controller/ble_ll.h"
#endif
#ifdef ESP_PLATFORM
#include "esp_log.h"
#endif


#include "Breaker.h"
#include "ErrorLog.h"

#include <wifi_provisioning/manager.h>
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
    #include <wifi_provisioning/scheme_ble.h>
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
    #include <wifi_provisioning/scheme_softap.h>
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
#include "qrcode.h"

#include "LED_Breaker.h"

extern DRAM_ATTR Device_Info_str DeviceInfo;

#define PROV_MGR_MAX_RETRY_CNT      3

#ifdef _AHMED_PROVISION_TEST_
    #define PROV_TIME_OUT               ONE_MINUTE
#else
    #define PROV_TIME_OUT               THREE_MINUTES
#endif    


static int8_t ProvisinAlreadyStarted = false;

static int16_t ProvisinTimer = 0;

extern uint8_t FirstTimeConnection;

static int8_t WiFiConnectFunctionFlag = false;

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */

static const char* TAG = "BLE Provisioning";


   /* Signal Wi-Fi events on this event-group */
    const int WIFI_CONNECTED_EVENT = BIT0;
    static EventGroupHandle_t wifi_event_group;

    #define PROV_QR_VERSION         "v1"
    #define PROV_TRANSPORT_SOFTAP   "softap"
    #define PROV_TRANSPORT_BLE      "ble"
    #define QRCODE_BASE_URL         "https://espressif.github.io/esp-jumpstart/qrcode.html"
    
    
static void wifi_init_sta(void)
{
    /* Start Wi-Fi in station mode */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}    
    
/* Event handler for catching system events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{

    static int retries;

    if (event_base == WIFI_PROV_EVENT) {
        switch (event_id) {
            case WIFI_PROV_START:
                ESP_LOGI(TAG, "Provisioning started");
                if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
                {
                    ets_printf("Provisioning started \n");
                }
                DeviceInfo.ProvisionState = ProvisionStarted;
                SetRGB_Color(PROVISION_STARTED_COLOR);
                ProvisinTimer = 0;       
                break;
            case WIFI_PROV_CRED_RECV: {
                if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
                {                
                    ets_printf("WIFI_PROV_CRED_RECV \n");
                }
                wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
                ESP_LOGI(TAG, "Received Wi-Fi credentials"
                         "\n\tSSID     : %s\n\tPassword : %s",
                         (const char *) wifi_sta_cfg->ssid,
                         /*(const char *) wifi_sta_cfg->password*/"**********");
                ProvisinTimer = 0;
                memset ((char *)&DeviceInfo.Ssid,0, sizeof (DeviceInfo.Ssid));
                memset ((char *)&DeviceInfo.PassWord,0, sizeof (DeviceInfo.PassWord));
                strcpy((char *)&DeviceInfo.Ssid,(const char *) wifi_sta_cfg->ssid);
                strncpy((char *)&DeviceInfo.PassWord,(const char *) wifi_sta_cfg->password, sizeof(DeviceInfo.PassWord) - 1);
                
                DeviceInfo.ProvisionState = ProvisionGotSSID;
                SetRGB_Color(PROVISION_GOT_SSID_COLOR);
                break;
            }
            case WIFI_PROV_CRED_FAIL: {
                ets_printf("WIFI_PROV_CRED_FAIL \n");
                wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
                ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s"
                         "\n\tPlease reset to factory and retry provisioning",
                         (*reason == WIFI_PROV_STA_AUTH_ERROR) ?
                         "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");
                DeviceInfo.ProvisionState = ProvisionWrongPassword;
                SetRGB_Color(PROVISION_IOT_WRONG_PASSOWRD_COLOR);
                ProvisinTimer = 0;

                retries++;
                if (retries >= PROV_MGR_MAX_RETRY_CNT) {
                    ESP_LOGI(TAG, "Failed to connect with provisioned AP, reseting provisioned credentials");
                    //wifi_prov_mgr_reset_sm_state_on_failure();
                    ResetDevice(PROVISIONING_RESET, true);
                    retries = 0;
                    ProvisinTimer = PROV_TIME_OUT;
                }

                break;
            }
            case WIFI_PROV_CRED_SUCCESS:
                if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
                {                
                    ets_printf("WIFI_PROV_CRED_SUCCESS \n");
                }
                ESP_LOGI(TAG, "Provisioning successful");
                retries = 0;
                ProvisinTimer = 0;
//                ProvisinAlreadyStarted = false;
                DeviceInfo.WiFiConnected = true;
                DeviceInfo.WiFiConnectedWaitTimer = WIFI_CONNECT_WAIT_TIMER;
                //set running color to indicate success.
                SetRGB_Color(RUNNING_COLOR);

                break;
            case WIFI_PROV_END:
                /* De-initialize manager once provisioning is finished */
                if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
                {
                    ets_printf("WIFI_PROV_END \n");
                }
                ESP_LOGI(TAG, "Provisioning End");
                wifi_prov_mgr_deinit();
                ProvisinTimer = 0;
                ProvisinAlreadyStarted = false;
                
            
                break;
            default:
                break;
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        DeviceInfo.WiFiConnected = false;
        if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
        {        
            ets_printf("WIFI_EVENT_STA_START \n");
        }
        esp_wifi_connect();
        ProvisinTimer = 0;
        
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
        {
            ets_printf("IP_EVENT_STA_GOT_IP \n");
        }
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        DeviceInfo.ProvisionState = ProvisionGotIP;
        SetRGB_Color(PROVISION_GOT_IP_COLOR);
        ProvisinTimer = 0;
        sntp_restart();
        if (WiFiConnectFunctionFlag)
        {
            DeviceInfo.WiFiConnected = true;
            DeviceInfo.WiFiConnectedWaitTimer = WIFI_CONNECT_WAIT_TIMER;
        }
            

        /* Signal main application to continue execution */
        //xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_EVENT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
        {        
        	ESP_LOGI(TAG,"WIFI_EVENT_STA_DISCONNECTED \n");
        }

        //Nitin
        /****************************************************************/
        DeviceInfo.ProvisionState = ProvisionedNotConnected;
        SetRGB_Color(PROVISION_IOT_PROVISIONED_NOT_CONNECTED_COLOR);
        /****************************************************************/

        ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
        DeviceInfo.WiFiConnected = false;
        esp_wifi_connect();
        ProvisinTimer = 0;
        
    }
}

static void get_device_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6] = {0};

#ifdef _OLD_APP_NO_BREAKER_
    const char *ssid_prefix = "ETN_DIM_";
#else
    const char *ssid_prefix = "ETN_BR2_";
#endif

    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%s%02X%02X%02X",
             ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
}


/* Handler for the optional provisioning endpoint registered by the application.
 * The data format can be chosen by applications. Here, we are using plain ascii text.
 * Applications can choose to use other formats like protobuf, JSON, XML, etc.
 */
esp_err_t custom_prov_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                          uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{


    if (inbuf) {
        ESP_LOGI(TAG, "Received data: %.*s", inlen, (char *)inbuf);
    }
    ESP_LOGI(TAG, "ConnectionString: %.*s", sizeof(DeviceInfo.DeviceId), DeviceInfo.DeviceId);
    char response[50];
    memset ((char *)&response,0, sizeof (response));
    memcpy ( (char *)&response, (char *)&DeviceInfo.DeviceId, sizeof (DeviceInfo.DeviceId));

    *outbuf = (uint8_t *)strdup(response);
    if (*outbuf == NULL) {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }
    *outlen = strlen(response) + 1; /* +1 for NULL terminating byte */

    return ESP_OK;
}


static void wifi_prov_print_qr(const char *name, const char *pop, const char *transport)
{
    if (!name || !transport) {
        ESP_LOGW(TAG, "Cannot generate QR code payload. Data missing.");
        return;
    }
    char payload[150] = {0};
    if (pop) {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                    ",\"pop\":\"%s\",\"transport\":\"%s\"}",
                    PROV_QR_VERSION, name, pop, transport);
    } else {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                    ",\"transport\":\"%s\"}",
                    PROV_QR_VERSION, name, transport);
    }
#ifdef CONFIG_EXAMPLE_PROV_SHOW_QR
    ESP_LOGI(TAG, "Scan this QR code from the provisioning application for Provisioning.");
    esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
    esp_qrcode_generate(&cfg, payload);
#endif /* CONFIG_APP_WIFI_PROV_SHOW_QR */
    ESP_LOGI(TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", QRCODE_BASE_URL, payload);
}

 
/*=======================   StartProvisioning  ===============================
**    Function description :
**              This function to Start Provisioning
**--------------------------------------------------------------------------*/
void StartProvisioning(void)
{
#if 0    
    if (DeviceInfo.ConnectionStringSaved == false)
    {
        //that means no connection string, return 
        ets_printf("Can't start provising process, there is no connection string\n");
        return;
    }
#endif
    if (DeviceInfo.DeviceInNetworkStatus == DEVICE_IS_IN_A_NETWORK)
    {
        // We will check first if we already here because we are re-provisining
        //if this is the case, then we start provisining but disable the flag so we
        // don't go for a loop, else we set the re-provising flag
        if (DeviceInfo.StartReProvisioning == RE_START_PROVISIONING)
        {
            DeviceInfo.StartReProvisioning = 0;
            WriteByteToFlash(EEOFFSET_RE_PROVISION_STARTED_LABEL,DeviceInfo.StartReProvisioning );       
        }
        else
        {
            //We reset provisining and restart
    	    ets_printf("Device was provisioned but we are restarting the provisioning process again \n");
            WriteByteToFlash(EEOFFSET_RE_PROVISION_STARTED_LABEL,RE_START_PROVISIONING );
            ResetDevice(DEVICE_RESET, true);
            return;
        }
#ifdef _ERROR_LOG_DEBUG_        
        ErrLog(LOG_BREAKER_RE_PROVISIONING);
#endif        
    }
#ifdef _ERROR_LOG_DEBUG_  
    else 
    {      
        ErrLog(LOG_BREAKER_PROVISIONING);
    }
#endif    
                    

    if (ProvisinAlreadyStarted == false)
    {
        ProvisinTimer = 0;
        ProvisinAlreadyStarted = true;
        FirstTimeConnection = true;
        
        ets_printf("SB2.0 Start Provisioning\n");

        /* Initialize TCP/IP */
        ESP_ERROR_CHECK(esp_netif_init());
        
        /* Initialize the event loop */
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        wifi_event_group = xEventGroupCreate();

        /* Register our event handler for Wi-Fi, IP and Provisioning related events */
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

        /* Initialize Wi-Fi including netif with default config */
        esp_netif_create_default_wifi_sta();
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
        esp_netif_create_default_wifi_ap();
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        
        /* Configuration for the provisioning manager */
        wifi_prov_mgr_config_t config = {
            /* What is the Provisioning Scheme that we want ?
             * wifi_prov_scheme_softap or wifi_prov_scheme_ble */
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
            .scheme = wifi_prov_scheme_ble,
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
            .scheme = wifi_prov_scheme_softap,
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */

            /* Any default scheme specific event handler that you would
             * like to choose. Since our example application requires
             * neither BT nor BLE, we can choose to release the associated
             * memory once provisioning is complete, or not needed
             * (in case when device is already provisioned). Choosing
             * appropriate scheme specific event handler allows the manager
             * to take care of this automatically. This can be set to
             * WIFI_PROV_EVENT_HANDLER_NONE when using wifi_prov_scheme_softap*/
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
            .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
            .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
        };

        /* Initialize provisioning manager with the
         * configuration parameters set above */
        ESP_ERROR_CHECK(wifi_prov_mgr_init(config));
        
        
        ESP_LOGI(TAG, "Starting provisioning");

        /* What is the Device Service Name that we want
         * This translates to :
         *     - Wi-Fi SSID when scheme is wifi_prov_scheme_softap
         *     - device name when scheme is wifi_prov_scheme_ble
         */

        char service_name[15];
        
        get_device_service_name(service_name, sizeof(service_name));

        /* What is the security level that we want (0 or 1):
         *      - WIFI_PROV_SECURITY_0 is simply plain text communication.
         *      - WIFI_PROV_SECURITY_1 is secure communication which consists of secure handshake
         *          using X25519 key exchange and proof of possession (pop) and AES-CTR
         *          for encryption/decryption of messages.
         */
        wifi_prov_security_t security = WIFI_PROV_SECURITY_1;

        /* Do we want a proof-of-possession (ignored if Security 0 is selected):
         *      - this should be a string with length > 0
         *      - NULL if not used
         */
        const char *pop = "abcd1234";

        /* What is the service key (could be NULL)
         * This translates to :
         *     - Wi-Fi password when scheme is wifi_prov_scheme_softap
         *     - simply ignored when scheme is wifi_prov_scheme_ble
         */
        const char *service_key = NULL;

    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        /* This step is only useful when scheme is wifi_prov_scheme_ble. This will
         * set a custom 128 bit UUID which will be included in the BLE advertisement
         * and will correspond to the primary GATT service that provides provisioning
         * endpoints as GATT characteristics. Each GATT characteristic will be
         * formed using the primary service UUID as base, with different auto assigned
         * 12th and 13th bytes (assume counting starts from 0th byte). The client side
         * applications must identify the endpoints by reading the User Characteristic
         * Description descriptor (0x2901) for each characteristic, which contains the
         * endpoint name of the characteristic */
        uint8_t custom_service_uuid[] = {
            /* LSB <---------------------------------------
             * ---------------------------------------> MSB */
            0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
            0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
            
			/*0x21, 0x43, 0x65, 0x87, 0x09, 0xba, 0xdc, 0xfe,
            0xef, 0xcd, 0xab, 0x90, 0x78, 0x56, 0x34, 0x12    */
        };

        /* If your build fails with linker errors at this point, then you may have
         * forgotten to enable the BT stack or BTDM BLE settings in the SDK (e.g. see
         * the sdkconfig.defaults in the example project) */
        wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

        /* An optional endpoint that applications can create if they expect to
         * get some additional custom data during provisioning workflow.
         * The endpoint name can be anything of your choice.
         * This call must be made before starting the provisioning.
         */
        wifi_prov_mgr_endpoint_create("custom-endpoint");
        /* Start provisioning service */
        ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));

        /* The handler for the optional endpoint created above.
         * This call must be made after starting the provisioning, and only if the endpoint
         * has already been created above.
         */
        wifi_prov_mgr_endpoint_register("custom-endpoint", custom_prov_data_handler, NULL);

        /* Uncomment the following to wait for the provisioning to finish and then release
         * the resources of the manager. Since in this case de-initialization is triggered
         * by the default event loop handler, we don't need to call the following */
        // wifi_prov_mgr_wait();
        // wifi_prov_mgr_deinit();
        /* Print QR code for provisioning */
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        wifi_prov_print_qr(service_name, pop, PROV_TRANSPORT_BLE);
    #else /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
        wifi_prov_print_qr(service_name, pop, PROV_TRANSPORT_SOFTAP);
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

        /* Wait for Wi-Fi connection */
        //xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_EVENT, false, true, portMAX_DELAY);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}



    
void WiFi_Connect()
{
        wifi_sta_config_t wifi_cfg;

        memset(&wifi_cfg, 0, sizeof(wifi_sta_config_t));

        /* Initialize TCP/IP */
        ESP_ERROR_CHECK(esp_netif_init());        
                
        /* Initialize the event loop */
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        wifi_event_group = xEventGroupCreate();

        /* Register our event handler for Wi-Fi, IP and Provisioning related events */
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

        /* Initialize Wi-Fi including netif with default config */
        esp_netif_create_default_wifi_sta();
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
        esp_netif_create_default_wifi_ap();
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
                /* Configuration for the provisioning manager */
        wifi_prov_mgr_config_t config = {
            /* What is the Provisioning Scheme that we want ?
             * wifi_prov_scheme_softap or wifi_prov_scheme_ble */
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
            .scheme = wifi_prov_scheme_ble,
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
            .scheme = wifi_prov_scheme_softap,
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */

            /* Any default scheme specific event handler that you would
             * like to choose. Since our example application requires
             * neither BT nor BLE, we can choose to release the associated
             * memory once provisioning is complete, or not needed
             * (in case when device is already provisioned). Choosing
             * appropriate scheme specific event handler allows the manager
             * to take care of this automatically. This can be set to
             * WIFI_PROV_EVENT_HANDLER_NONE when using wifi_prov_scheme_softap*/
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
            .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
            .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
        };

        /* Initialize provisioning manager with the
         * configuration parameters set above */
        ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

        vTaskDelay(10 / portTICK_PERIOD_MS);

        wifi_prov_mgr_deinit();

        vTaskDelay(10 / portTICK_PERIOD_MS);

        if (esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_cfg) == ESP_OK) {
            memcpy ((char *)&DeviceInfo.Ssid, (char *)&wifi_cfg.ssid,sizeof(DeviceInfo.Ssid));
            memcpy ((char *)&DeviceInfo.PassWord, (char *)&wifi_cfg.password,sizeof(DeviceInfo.PassWord));
        }   

        /* Start Wi-Fi station */
        wifi_init_sta();
        
        WiFiConnectFunctionFlag = true;

}


void Factory_WiFi_Connect()
{
        wifi_sta_config_t wifi_cfg;
        /* Initialize TCP/IP */
        ESP_ERROR_CHECK(esp_netif_init());        
                
        /* Initialize the event loop */
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        wifi_event_group = xEventGroupCreate();

        /* Register our event handler for Wi-Fi, IP and Provisioning related events */
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

        /* Initialize Wi-Fi including netif with default config */
        esp_netif_create_default_wifi_sta();
    #ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP
        esp_netif_create_default_wifi_ap();
    #endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_SOFTAP */
    
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        /* We don't need the manager as device is already provisioned,
         * so let's release it's resources */
        //wifi_prov_mgr_deinit();

        if (esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_cfg) == ESP_OK) {
            memcpy ((char *)&wifi_cfg.ssid,(char *)&DeviceInfo.Ssid,sizeof(DeviceInfo.Ssid));
            memcpy ((char *)&wifi_cfg.password,(char *)&DeviceInfo.PassWord, sizeof(DeviceInfo.PassWord));
        }   
        

        /* Start Wi-Fi in station mode */
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        
        ESP_ERROR_CHECK( esp_wifi_set_config( ESP_IF_WIFI_STA, &wifi_cfg ) );
        
        ESP_ERROR_CHECK(esp_wifi_start());
        
        WiFiConnectFunctionFlag = true;
    
}

#if 0
void Reset_provisioning(void)
{

    SetRGB_Color(UNPROVISIONED_COLOR);
    
    ESP_LOGI(TAG, "Reset_provisioning");

    /* De-initialize manager once provisioning is finished */
    wifi_prov_mgr_deinit();
	
    wifi_prov_mgr_reset_provisioning();

	DeviceInfo.Installed = DEVICE_IS_NOT_IN_A_NETWORK;
	WriteByteToFlash(EEOFFSET_PROVISION_LABEL,DeviceInfo.Installed );

}
#endif

/*=====================================   ProvisionEvent  ======================
**    Function description :
**          called from timer_1Sec
**
**
**--------------------------------------------------------------------------*/

void ProvisionEvent(void)
{
 

#ifdef  _AHMED_PROVISION_TEST_
    static uint8_t TestReInitiateProvisioningAgainCounter = 30;
    static uint8_t TestReInitiateProvisioningAgainFlag = false; 
#endif   


    if (ProvisinAlreadyStarted == true)
    {
        ProvisinTimer++;
        if (ProvisinTimer >= PROV_TIME_OUT)
        {
        	ProvisinAlreadyStarted = false;
            ResetDevice(DEVICE_RESET, true);
        }
    }
    
    

#ifdef  _AHMED_PROVISION_TEST_
    if (TestReInitiateProvisioningAgainFlag)
    {
        if (TestReInitiateProvisioningAgainCounter != 0)
        {
            TestReInitiateProvisioningAgainCounter--;
        }
        else
        {
            TestReInitiateProvisioningAgainFlag = false;
            StartProvisioning();
        }
    }
#endif   
  
}

bool IsDeviceInProvisioningMode(void)
{
	return ProvisinAlreadyStarted;
}
